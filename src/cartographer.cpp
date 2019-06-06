#include "cartographer.hpp"

/* ------------ PRIVATE FUNCTIONS ------------*/
void Cartographer::shiftHueSpace(cv::Mat *h, int const shift)
{
	cv::Mat h_tmp = *h;
	int cv_shift = shift / 2;	// open cv have hue values from 0 to 180 (cast from 0 to 360)

	for(int i = 0; i < h_tmp.rows; ++i)
	{
		for(int j = 0; j < h_tmp.cols; ++j) 
		{
			h_tmp.at<unsigned char>(i, j) = (h_tmp.at<unsigned char>(i, j) 
											+ cv_shift) % 180;
		} 
	}

	*h = h_tmp;
}

void Cartographer::extractContours(cv::Mat canny) 
{
	cv::findContours(canny, this->contours, this->hierarchy,
					 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
}

void Cartographer::binarization(cv::Mat *img) 
{
	cv::Mat im_gray;
	cv::cvtColor(*img, im_gray, CV_RGB2GRAY);
	cv::threshold(im_gray, *img, 200.0, 255.0, CV_THRESH_BINARY);
}

void Cartographer::getBorder() 
{
	cv::Mat output = this->src.clone();
	cv::Mat img_bw = this->src.clone();
	
	binarization(&img_bw);
	cv::Canny(img_bw, img_bw, 100, 50);
	cv::dilate(img_bw, img_bw, cv::Mat());
	cv::dilate(img_bw, img_bw, cv::Mat());
	// cv::dilate(img_bw, img_bw, cv::Mat());
	this->extractContours(img_bw);
	
	/*------------FUNCTION TO EXTRACT CONTOURS FROM HSV-------------*/
	// cv::Mat hsv;
	// cv::Mat H, S, V;
	// cv::Mat cannyH, cannyS;
	// cv::Mat output;

	// cv::cvtColor(this->src, hsv, CV_BGR2HSV);

	// std::vector<cv::Mat> channels;
	// cv::split(hsv, channels);

	// H = channels[0];
	// S = channels[1];
	// V = channels[2];

	// this->shiftHueSpace(&H, 50);
	// cv::Canny(H, cannyH, 100, 50);
	// // cv::Canny(S, cannyS, 200, 100);
	// cv::dilate(cannyH, cannyH, cv::Mat());
	// cv::dilate(cannyH, cannyH, cv::Mat());
	// cv::dilate(cannyH, cannyH, cv::Mat());

	// this->extractContours(cannyH);
	/*------------------------------------------------------------*/

	// draw contours to source img
	for(int i = 0; i < this->contours.size(); ++i)
	{
		if (cv::contourArea(contours[i]) < 400) continue;	// ignore area smaller than 400
		if (hierarchy[i][3] < 0) continue; 					// ignore inside contours

		cv::drawContours(output, this->contours, i,
						 cv::Scalar(0, 0, 255), 2, 8, this->hierarchy, 0);
	}

	this->bordered = output;
}

/* ------------ PUBLIC FUNTIONS ------------*/
void Cartographer::setSrcImg(cv::Mat _src) 
{
	//TODO: add some allert
	if (_src.empty()) 
	{
		std::cout << "dupa" << std::endl;

		return;
	}
	this->src = _src;
}

cv::Mat Cartographer::getBordered(bool force)
{
	if (this->bordered.empty() || force) {	
		this->getBorder();
	}

	return this->bordered;
}

// Cartographer::Cartographer() 
// {
// 	// this->bordered = NULL;
// }