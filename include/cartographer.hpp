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
	cv::Mat cutted;
	cv::Mat bordered;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point>				 contour;
	std::vector<cv::Vec4i>               hierarchy;

	void getBorder      ();
	void shiftHueSpace  (cv::Mat *h, int const shift);
	void cutImage       ();
	void extractContours(cv::Mat canny);
	void binarization   (cv::Mat *img );

public:
	void                                 makeBorder  ( bool force );

	// getters
	cv::Mat                              getSrc      ()              { return this->src;      };
	cv::Mat                              getCutted    ()             { return this->cutted;   };
	cv::Mat                              getBordered ( bool force );
	std::vector<std::vector<cv::Point> > getContours ()              { return this->contours; };
	std::vector<cv::Point>				 getContour  ()				 { return this->contour;  };
	std::vector<cv::Vec4i>               getHierarchy()              { return this->hierarchy;};
	
	// setters
	void setSrcImg  (cv::Mat _src);

	Cartographer () {};
	~Cartographer() {};
};

#endif