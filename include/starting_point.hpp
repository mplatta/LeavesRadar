#ifndef _STARTING_POINT_H_
#define _STARTING_POINT_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "straight.hpp"
#include "opencv_headers.hpp"
#include "formatted_log.hpp"

class StartingPoint
{
private:
	cv::Mat img;	
	Straight_t str;	

	void setImgEdge(cv::Mat img);
public:
	cv::Point2f getStartingPoint(double percent);	

	// getters
	cv::Mat getImg() {return this->img;};
	Straight_t getStr() {return this->str;};
	
	// setters
	void setImg  (cv::Mat img) { this->setImgEdge(img);};
	void setStr  (Straight_t str) {this->str = str;};

	StartingPoint () {};
	StartingPoint (cv::Mat img, Straight_t str) {this->setImgEdge(img); this->str = str;};
	~StartingPoint() {};
};

#endif