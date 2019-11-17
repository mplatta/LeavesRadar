#ifndef _SYMMETRY_DETECTOR_H_
#define _SYMMETRY_DETECTOR_H_

#include "opencv_headers.hpp"
#include "straight.hpp"

using namespace std;
using namespace cv;


class SymmetryDetector {
public:
    SymmetryDetector( Mat image );
    void vote( Mat& image, double min_pair_dist );
    inline void rotateEdges( vector<Point2f>& edges, int theta );    
   
    straight_t getResult();
    straight_t getLine( float rho, float theta );
    
private:
    vector<Mat> rotMatrices;
    Mat rotEdges;
    vector<float*> reRows;
    Mat accum;
    
    Mat image;
    Size imageSize;
    Point2f center;
    float diagonal;
    int rhoMax;
    int thetaMax;
};

#endif