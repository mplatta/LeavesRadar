#include "cartographer.hpp"

/* ------------ PRIVATE FUNCTIONS ------------*/
void Cartographer::shiftHueSpace(cv::Mat *h, int const shift)
{
	formatted_log("Start shifting hue space by: %d", shift);
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

void Cartographer::cutImage()
{
	const int frame = 30;

	int min_x = this->src.size().width;
	int min_y = this->src.size().height;
	int max_x = 0;
	int max_y = 0;

	std::vector<cv::Point> contour_;

	for (size_t i = 0; i < this->contour.size(); i++)
	{
		int b = (i + this->contour.size() - 1) % this->contour.size();
		int n = (i + 1) % this->contour.size();
		float avX = ((this->contour[b].x + this->contour[n].x) / 2);
		float avY = ((this->contour[n].y + this->contour[n].y) / 2);

		if ( ((this->contour[i].x / avX) - 1 > 0.15) || 
			 ((this->contour[i].y / avY) - 1 > 0.15) )
		{
			formatted_war("Some contour point is different from its neighbors by more than 15%");
			continue;
		}

		contour_.push_back(this->contour[i]);

		if (this->contour[i].x < min_x) min_x = this->contour[i].x;
		if (this->contour[i].y < min_y) min_y = this->contour[i].y;

		if (this->contour[i].x > max_x) max_x = this->contour[i].x;
		if (this->contour[i].y > max_y) max_y = this->contour[i].y;
	}

	int min_frame_x = ((min_x - frame) > 0) ? (min_x - frame) : 0;
	int min_frame_y = ((min_y - frame) > 0) ? (min_y - frame) : 0;
	int max_frame_x = ((max_x + frame) < this->src.size().width) ? max_x - min_frame_x + frame : this->src.size().width - min_frame_x;
	int max_frame_y = ((max_y + frame) < this->src.size().height) ? max_y - min_frame_y + frame : this->src.size().height - min_frame_y;

	this->cuted = cv::Mat(this->src.clone(), cv::Rect(min_frame_x, min_frame_y, max_frame_x, max_frame_y));

	for (size_t i = 0; i < contour_.size(); i++)
	{
		contour_[i].x -= min_frame_x;
		contour_[i].y -= min_frame_y;
	}
	
	this->contour.clear();
	this->contour.swap(contour_);
}

void Cartographer::extractContours(cv::Mat canny) 
{
	formatted_log("Start extract contours");
	cv::findContours(canny, this->contours, this->hierarchy,
					 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
}

void Cartographer::binarization(cv::Mat *img) 
{
	formatted_log("Start binarization image");
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

	// tmp solution:
	double ar = 0.0;
	int index;

	for(int i = 0; i < this->contours.size(); ++i)
	{
		if (cv::contourArea(contours[i]) > ar)
		{
			ar = cv::contourArea(contours[i]);
			index = i;
		}
	}

	this->contour = this->contours[index];
	this->cutImage();

	cv::drawContours(output, this->contours, index, cv::Scalar(0, 0, 255), 2, 8, this->hierarchy, 0);

	this->bordered = output;
}

/* ------------ PUBLIC FUNTIONS ------------*/

void Cartographer::makeBorder(bool force)
{
	if (this->bordered.empty() || force) {	
		this->getBorder();
	}
}

void Cartographer::setSrcImg(cv::Mat _src) 
{
	if (_src.empty()) 
	{
		formatted_err("Image not exist");

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
