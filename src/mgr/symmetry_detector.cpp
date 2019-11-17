#include "symmetry_detector.hpp"

SymmetryDetector::SymmetryDetector( Mat image ) {

    this->image         = image.clone();
    this->imageSize     = image.size();
    this->center        = Point2f( imageSize.width - 1.0, imageSize.height - 1.0 ) * 0.5;
    this->diagonal      = hypotf( imageSize.width, imageSize.height );
    this->rhoMax        = diagonal + 1;
    this->thetaMax      = 180.0;
    
    rotMatrices.resize( thetaMax, Mat(2, 2, CV_32FC1) );
    
    float thetaIncDeg = 180.0f / thetaMax;
    float half_theta_max = thetaMax * 0.5f;
    
    /* Pre calculate rotation matrices from -90 deg to 90 deg (actually to 89 deg) */
    for( int t = 0; t < thetaMax; t++ ){
        double angle = thetaIncDeg * ( t - half_theta_max );
        Mat rotation = getRotationMatrix2D( Point2f(0.0, 0.0), angle, 1.0);
        rotation.convertTo( rotation, CV_32FC1 );
        
        rotMatrices[t] = Mat( rotation, Rect(0, 0, 2, 2) );
        rotMatrices[t].row(0) *= 0.5;
    }
    
    accum       = Mat::zeros( thetaMax + 2, rhoMax, CV_32FC1 );
    rotEdges    = Mat::zeros( rhoMax - 1, diagonal, CV_32FC1 );
    reRows.resize( rhoMax - 1 );
}

/**
 * Rotate each edge for a given theta, and append them into rotated edges matrix
 */
void SymmetryDetector::rotateEdges( vector<Point2f>& edges, int theta ) {
    /* Get the cos and sin values from our pre-calculated rotation matrices */
    float r0 = rotMatrices[theta].at<float>(0, 0);
    float r1 = rotMatrices[theta].at<float>(0, 1);
    float r2 = rotMatrices[theta].at<float>(1, 0);
    float r3 = rotMatrices[theta].at<float>(1, 1);
    
    /* Reset our row pointers to start of each row in rotated edges matrix */
    for( int i = 0; i < rhoMax - 1; i++ )
        reRows[i] = rotEdges.ptr<float>(i);
    
    float half_diag = cvRound(diagonal) * 0.5;
    float fourth_rho = rhoMax * 0.25;
    
    /* Now append the corresponding rho values of the rotated edges ot rotated edges matrix */
    for( Point2f& edge: edges ) {
        int rho = r2 * edge.x + r3 * edge.y + half_diag;
        *(reRows[rho]++) = r0 * edge.x + r1 * edge.y + fourth_rho;
    }
}

/**
 * Create the hough accumulation matrix, and vote for each pair of symmetrical edges
 */
void SymmetryDetector::vote( Mat& image, double min_pair_dist) {
    float min_dist = min_pair_dist * this->rhoMax;   
    
    /* Make sure that we reset the accumulation matrix and rotated edges matrix */
    accum    = Scalar::all(0);
    rotEdges = Scalar::all(0);
    
    /* Find all the pixels of the edges */
    vector<Point> temp_edges;
    findNonZero( image, temp_edges );
    
    /* Translate them in relation to center of the image */
    vector<Point2f> edges;
    for( Point point: temp_edges )
        edges.push_back( Point2f( point.x - center.x, point.y - center.y ) );
    
    /* For each degree of rotation */
    for( int t = 0; t < thetaMax; t++ ) {
        float * accum_ptr = accum.ptr<float>(t);
        
        /* Rotate edge to that degree */
        rotateEdges( edges, t );
        
        for( int i = 0; i < rhoMax - 1; i++ ) {
            float * col_start   = rotEdges.ptr<float>(i);
            float * col_end     = reRows[i];
            
            /* Ignore edges that have smaller number of pairings */
            if( (col_end - col_start) <= 1 ) {
                continue;
            }
            
            /* Vote for Hough matrix */
            for( float * x0 = col_start; x0 != col_end - 1; x0++ ) {
                for( float * x1 = x0 + 1; x1 != col_end; x1++ ) {
                    float dist = fabs( *x1 - *x0 );

                    if( dist < min_dist )
                        break;
                    
                    int rho_index = static_cast<int>(*x0 + *x1);
                    accum_ptr[rho_index]++;
                }
            }
        }
    }
}



/**
 * Find the lines that fit the symmetrical object from the calculated Hough accumulation matrix
 **/
straight_t SymmetryDetector::getResult() {
    straight_t result; 
    Mat temp, edge;   

    double min_pair_dist  = 0.01;

    temp = this->image.clone();
        
    /* Find the edges */
    cvtColor( temp, edge, CV_BGR2GRAY );
    threshold(edge, edge, 200.0, 255.0, CV_THRESH_BINARY);
    Canny( edge, edge, 100, 50);
        
    /* Vote for the accumulation matrix */
    this->vote( edge, min_pair_dist);
    
    /* Create a mask to avoid searching peaks around the padding */
    Mat mask( accum.size(), CV_8UC1, Scalar(255) );
    mask.row(0)             = Scalar(0);
    mask.row(mask.rows - 1) = Scalar(0);    
    
    temp = accum.clone();    
    
    
    /* Find the peak from the Hough accumulation matrix */
    double max_val;
    Point max_loc;
    minMaxLoc( temp, NULL, &max_val, NULL, &max_loc, mask );
        
    int rho_index   = max_loc.x;
    int theta_index = max_loc.y; 
        
    /* Convert from Hough space back to x-y space */
    result = getLine(rho_index, theta_index );

    formatted_log("A = %f, B = %f, C = %f", result.coeff.a, result.coeff.b, result.coeff.c);
    
    return result;
}


/**
 * Return a pair of points that describe the line based on the given
 * rho and theta in the Hough space
 */
straight_t SymmetryDetector::getLine( float rho_index, float theta_index ) {
    straight_t result;

    float half_rho_max   = rhoMax * 0.5f;
    float half_theta_max = thetaMax * 0.5f;
    
    float rho   = (rho_index - half_rho_max + 0.5f) * (diagonal / (rhoMax - 1.0f));
    float theta = (theta_index - half_theta_max - 1.0f) * (M_PI / thetaMax);
    
    float cos_theta = cosf( theta );
    float sin_theta = sinf( theta );
    
    float x_r = center.x + rho * cos_theta;
    float y_r = center.y + rho * sin_theta;
    
    vector<float> d(4, std::numeric_limits<float>::max());
    if( sin_theta != 0.0 ) {
        d[0] = x_r / sin_theta;
        d[1] = (x_r - imageSize.width + 1) / sin_theta;
    }
    if( cos_theta != 0.0 ) {
        d[2] = -y_r / cos_theta;
        d[3] = (imageSize.height - 1 - y_r) / cos_theta;
    }
    
    float min_d = std::numeric_limits<float>::max();
    for( float val : d ) {
        if (val > 0)
            min_d = MIN( min_d, val );
    }
    
    Point p0( -min_d * sin_theta + x_r, min_d * cos_theta + y_r );
    
    d.assign(4, std::numeric_limits<float>::max());
    if( sin_theta != 0.0 ) {
        d[0] = -x_r / sin_theta;
        d[1] = (imageSize.width - x_r - 1) / sin_theta;
    }
    if( cos_theta != 0.0 ) {
        d[2] = y_r / cos_theta;
        d[3] = (1 + y_r - imageSize.height) / cos_theta;
    }
    min_d = std::numeric_limits<float>::max();
    for( float val : d ) {
        if (val > 0)
            min_d = MIN( min_d, val );
    }
    
    Point p1( min_d * sin_theta + x_r, -min_d * cos_theta + y_r );

    result = createStraightFrom2Point (p0, p1);

    // formatted_log("A = %f, B = %f, C = %f", result.coeff.a, result.coeff.b, result.coeff.c);
    
    return result;
}