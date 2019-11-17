#include "starting_point.hpp"

void StartingPoint::setImgEdge(cv::Mat img){
	// formatted_log("Size this [%d, %d]", this->img.rows, this->img.cols);
	// formatted_log("Size img [%d, %d]", img.rows, img.cols);

	cv::Mat img_gray, temp;
	img.convertTo(temp, CV_8U);
	cvtColor(temp, img_gray, CV_BGR2GRAY);
	threshold(img_gray, temp, 200.0, 255.0, CV_THRESH_BINARY);
	Canny( temp, this->img, 100, 50);

	// formatted_log("Size this 2 [%d, %d]", this->img.rows, this->img.cols);
}

cv::Point2f StartingPoint::getStartingPoint(double percent) {
	cv::Point2f a(0.0f, 0.0f), b(0.0f, 0.0f);    
	int rows = this->img.rows;
	int cols = this->img.cols;

	formatted_log("Size [%d, %d]", rows, cols);

	cv::Point2f check;

	if(this->str.coeff.a == 0){
		check = getPointOnStraightX(this->str, 0.0);
	}
	else{
		check = getPointOnStraightY(this->str, 0.0);
	}

	if(check.x >= 0 && check.x <= cols && this->str.coeff.b != 0) {
		// formatted_log("if true");

		for(int i = cols; i >= 0; i--) {
			cv::Point2f p = getPointOnStraightX(this->str, (double)i);

			// formatted_log("loop1 %d", i);
			// formatted_log("Point p [%f, %f]", p.y, p.x);
			// formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
			if(p.y > 0 && p.x > 0 && p.y < rows && p.x < cols)
			{
				if(this->img.at<uchar>(p) == 0) {
					b = p;
					break;
				}
			}
		}    

		for(int i = 0; i < cols; i++){
			cv::Point2f p = getPointOnStraightX(this->str, (double)i);

			// formatted_log("loop2 %d", i);
			// formatted_log("Point p [%f, %f]", p.y, p.x);
			// formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
			if(p.y > 0 && p.x > 0 && p.y < rows && p.x < cols)
			{
				if(this->img.at<uchar>(p) == 0) {
					a = p;
					break;
				}
			}
		}        
	} else {
		// formatted_log("if false");

		for(int i = rows; i >= 0; i--){
			cv::Point2f p = getPointOnStraightY(this->str, (double)i);

			// formatted_log("loop1 %d", i);
			// formatted_log("Point p [%f, %f]", p.y, p.x);
			// formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
			if(p.y > 0 && p.x > 0 && p.y < rows && p.x < cols) {
				if(this->img.at<uchar>(p) == 0) {
					b = p;
					break;
				}
			}
		}    

		for(int i = 0; i < rows; i++){
			cv::Point2f p = getPointOnStraightY(this->str, (double)i);

			// formatted_log("loop2 %d", i);
			// formatted_log("Point p [%f, %f]", p.y, p.x);
			// formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
			if(p.y > 0 && p.x > 0 && p.y < rows && p.x < cols) {
				if(this->img.at<uchar>(p) == 0) {
					a = p;
					break;
				}
			}
		}
	}

	double ysp;
    cv::Point2f result;

    if(this->str.coeff.a == 0){
       ysp = ((b.x - a.x) * percent) + a.x;
       result = getPointOnStraightX(this->str, ysp);
    }
    else{
        ysp = ((b.y - a.y) * percent) + a.y;
        result = getPointOnStraightY(this->str, ysp);
    }	

    formatted_log("Point result [%f, %f]", result.y, result.x);

	cv::Point2f result; 

	if(this->str.coeff.b == 0){
		double ysp = ((b.y - a.y) * percent) + a.y;
		result = getPointOnStraightY(this->str, ysp);
	}
	else{
		double xsp = ((b.x - a.x) * percent) + a.x;
		result = getPointOnStraightX(this->str, xsp);
	}	

	return result;
}