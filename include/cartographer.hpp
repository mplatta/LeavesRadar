#ifndef _CARTOGRAPHER_H_
#define _CARTOGRAPHER_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "opencv_headers.hpp"
#include "formatted_log.hpp"

class Cartographer
{
private:
	cv::Mat src;
	cv::Mat bordered;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i>               hierarchy;

	void getBorder      ();
	void shiftHueSpace  (cv::Mat *h, int const shift);
	void extractContours(cv::Mat canny);
	void binarization   (cv::Mat *img );

public:
	// getters
	cv::Mat                              getSrc      ()              { return this->src;      };
	cv::Mat                              getBordered ( bool force );
	std::vector<std::vector<cv::Point> > getContours ()              { return this->contours; };
	std::vector<cv::Vec4i>               getHierarchy()              { return this->hierarchy;};
	
	// setters
	void setSrcImg  (cv::Mat _src);

	Cartographer () {};
	~Cartographer() {};
};

#endif