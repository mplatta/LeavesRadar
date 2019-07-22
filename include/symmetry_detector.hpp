#ifndef _SYMMETRY_DETECTOR_H_
#define _SYMMETRY_DETECTOR_H_

#include "opencv_headers.hpp"

using namespace std;
using namespace cv;


class SymmetryDetector {
public:
    SymmetryDetector( const Size image_size, const Size hough_size, const int rot_resolution = 1 );
    void vote( Mat& image, int min_pair_dist, int max_pair_dist  );
    inline void rotateEdges( vector<Point2f>& edges, int theta );    
   
    pair<Point, Point> getResult(Mat frame, float threshold = -1.0f );
    pair<Point, Point> getLine( float rho, float theta );
    
private:
    vector<Mat> rotMatrices;
    Mat rotEdges;
    vector<float*> reRows;
    Mat accum;
    
    Size imageSize;
    Point2f center;
    float diagonal;
    int rhoDivision;
    int rhoMax;
    int thetaMax;
};

#endif