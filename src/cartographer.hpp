#ifndef _CARTOGRAPHER_H_
#define _CARTOGRAPHER_H_

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

class Cartographer
{
private:
	cv::Mat src;
	cv::Mat bordered;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	void getBorder();
	void shiftHueSpace  (cv::Mat *h, int const shift);
	void extractContours(cv::Mat canny);
	void binarization   (cv::Mat *img);

public:
	cv::Mat getSrc     ()              { return this->src; };
	cv::Mat getBordered(bool force);
	void    setSrcImg  (cv::Mat _src);

	Cartographer();
	~Cartographer();
};

#endif