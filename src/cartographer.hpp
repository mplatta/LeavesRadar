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
	Mat src;
	Mat bordered;
	void getBorder();

public:
	void setSrcImg(Mat src);
	Mat getBordered();

	Cartographer();
	~Cartographer();
};

#endif