#ifndef _RECTIFICATION_H_
#define _RECTIFICATION_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "straight.hpp"
#include "opencv_headers.hpp"
#include "formatted_log.hpp"

class Rectification
{
private:
	cv::Mat img;	
	Straight_t str;
	double angle = 999.0;
	double translationX = 0.0;
	double translationY = 0.0;
	
	cv::Point2f getPointOnStraightY(double y);
	cv::Point2f getPointOnStraightX(double x);
	void computeAngle();

public:
	cv::Mat straightenImg();
	void straightenPoint(cv::Point2f &p);

	// getters
	cv::Mat getImg() {return this->img;};
	Straight_t getStr() {return this->str;};
	double getAngle() {return this->angle;};
	
	// setters
	void setImg  (cv::Mat img) {this->img = img;};
	void setStr  (Straight_t str) {this->str = str;};

	Rectification () {};
	Rectification (cv::Mat img, Straight_t str) {this->img = img; this->str = str;};
	~Rectification() {};
};

#endif