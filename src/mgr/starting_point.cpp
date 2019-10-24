#include "starting_point.hpp"

void StartingPoint::setImgBin(cv::Mat img){
	cv::Mat img_gray, temp;
	img.convertTo(temp, CV_8U);
	cvtColor(temp, img_gray, CV_BGR2GRAY);
	threshold(img_gray, this->img, 200, 255, 0);
}

cv::Point2f StartingPoint::getStartingPoint(double percent){
	cv::Point2f a(0.0f, 0.0f), b(0.0f, 0.0f);

    for(int i = this->img.cols; i >= 0; i--){
    	cv::Point p = getPointOnStraightX(this->str, i);
    	
    	if(this->img.at<uchar>(p.y, p.x) != 255 && p.y >= 0 && p.x >= 0 && p.y <= this->img.rows && p.y <= this->img.cols){
    		b = p;
    		break;
    	}
    }    

    for(int i = 0; this->img.cols; i++){
    	cv::Point2f p = getPointOnStraightX(this->str, i);

    	if(this->img.at<uchar>(p.y, p.x) != 255 && p.y >= 0 && p.x >= 0 && p.y <= this->img.rows && p.y <= this->img.cols){
    	    a = p;
    	    break;
    	}
    }

    float xsp = ((b.x - a.x) * percent) + a.x;
    return getPointOnStraightX(this->str, xsp);
}