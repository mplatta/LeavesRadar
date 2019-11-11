#include "starting_point.hpp"

void StartingPoint::setImgBin(cv::Mat img){
    // formatted_log("Size this [%d, %d]", this->img.rows, this->img.cols);
    // formatted_log("Size img [%d, %d]", img.rows, img.cols);

	cv::Mat img_gray, temp;
	img.convertTo(temp, CV_8U);
	cvtColor(temp, img_gray, CV_BGR2GRAY);
	threshold(img_gray, this->img, 200.0, 255.0, CV_THRESH_BINARY);    

    // formatted_log("Size this 2 [%d, %d]", this->img.rows, this->img.cols);
}

cv::Point2f StartingPoint::getStartingPoint(double percent){
	cv::Point2f a(0.0f, 0.0f), b(0.0f, 0.0f);    
    int rows = this->img.rows;
    int cols = this->img.cols;

    formatted_log("Size [%d, %d]", rows, cols);

    cv::Point2f check = getPointOnStraightX(this->str, 0.0);

    if(check.y >= 0 && check.y <= rows){
        // formatted_log("if true");
formatted_log("T1");
        for(int i = cols; i >= 0; i--){
            cv::Point2f p = getPointOnStraightX(this->str, (double)i);

            // formatted_log("loop1 %d", i);
            // formatted_log("Point p [%f, %f]", p.y, p.x);
            // formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
            
            if(this->img.at<uchar>(p) == 0 && p.y >= 0 && p.x >= 0 && p.y <= rows && p.x <= cols){
                b = p;
                break;
            }
        }    

        for(int i = 0; i < cols; i++){
            cv::Point2f p = getPointOnStraightX(this->str, (double)i);

            // formatted_log("loop2 %d", i);
            // formatted_log("Point p [%f, %f]", p.y, p.x);
            // formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));

            if(this->img.at<uchar>(p) == 0 && p.y >= 0 && p.x >= 0 && p.y <= rows && p.x <= cols){
                a = p;
                break;
            }
        }        
    }else{
        formatted_log("T2");
        // formatted_log("if false");

        for(int i = rows; i >= 0; i--){
            cv::Point2f p = getPointOnStraightY(this->str, (double)i);
        formatted_log("T4");

            // formatted_log("loop1 %d", i);
            // formatted_log("Point p [%f, %f]", p.y, p.x);
            // formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));
            
            if(this->img.at<uchar>(p) == 0 && p.y >= 0 && p.x >= 0 && p.y <= rows && p.x <= cols){
                b = p;
                break;
            }
        }    

        for(int i = 0; i < rows; i++){
        formatted_log("T5");
        	cv::Point2f p = getPointOnStraightY(this->str, (double)i);

            // formatted_log("loop2 %d", i);
            // formatted_log("Point p [%f, %f]", p.y, p.x);
            // formatted_log("this->img.at<uchar>(p) %d", this->img.at<uchar>(p));

        	if(this->img.at<uchar>(p) == 0 && p.y >= 0 && p.x >= 0 && p.y <= rows && p.x <= cols){
        	    a = p;
        	    break;
        	}
        }
    }
    formatted_log("T3");
    // formatted_log("Point a [%f, %f]", a.y, a.x);
    // formatted_log("Point b [%f, %f]", b.y, b.x);

    double ysp = ((b.y - a.y) * percent) + a.y;
    return getPointOnStraightY(this->str, ysp);
}