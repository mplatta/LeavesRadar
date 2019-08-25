#include "rectification.hpp"

void Rectification::computeAngle(){
    if(-360.0 < this->angle && this->angle < 360.0){
        return;
    }
    //Getting points on symmetry line
    cv::Point2f first_point = this->getPointOnStraightY(this->img.rows);
    // formatted_log("First point [%f, %f]", first_point.x, first_point.y);
    cv::Point2f second_point = this->getPointOnStraightX(this->img.cols / 2.0);
    // formatted_log("Second point [%f, %f]", second_point.x, second_point.y);

    //Changing points to vectors
    std::pair<double, double> vector_AB( second_point.x - first_point.x, second_point.y - first_point.y);
    std::pair<double, double> vectro_BC( second_point.x - this->img.cols / 2.0, second_point.y - this->img.rows);

    // formatted_log("vector AB [%f, %f]", vector_AB.first, vector_AB.second);
    // formatted_log("vectro BC [%f, %f]", vectro_BC.first, vectro_BC.second);

    //Computing angle
    double angle = (vector_AB.first * vectro_BC.first + vector_AB.second * vectro_BC.second)/
        (sqrt(vector_AB.first * vector_AB.first + vector_AB.second * vector_AB.second)*
        sqrt(vectro_BC.first * vectro_BC.first + vectro_BC.second * vectro_BC.second));
    angle = acos(angle);
    angle = angle * 180/M_PI;
    this->angle = angle;

    // formatted_log("angle = %f", this->angle);    
}

void Rectification::straightenPoint(cv::Point2f &p){
    this->computeAngle();

    // formatted_log("angle = %f", this->angle);
    // formatted_log("Point [%f, %f]", p.x, p.y);
    // formatted_log("Pivot point [%f, %f]", this->img.cols / 2.0, this->img.rows / 2.0);

    p.x -= this->img.cols / 2.0;
    p.y -= this->img.rows / 2.0;  

    double x = p.x;
    double y = p.y;

    p.x = x * cos(-this->angle * M_PI/180) - y * sin(-this->angle * M_PI/180);
    p.y = x * sin(-this->angle * M_PI/180) + y * cos(-this->angle * M_PI/180);

    p.x += this->img.cols / 2.0;
    p.y += this->img.rows / 2.0;

    if(this->translationX != 0.0)
        p.x += this->translationX;
    if(this->translationY != 0.0)
        p.y += this->translationY;
}

cv::Point2f Rectification::getPointOnStraightY(double y){
	// formatted_log("y = %f", y);
	// formatted_log("A = %f, B = %f, C = %f", this->str.coeff.a, this->str.coeff.b, this->str.coeff.c);	
	// formatted_log("x = %f", (this->str.coeff.c - this->str.coeff.b * y)/this->str.coeff.a);
	cv::Point2f result(((this->str.coeff.c - this->str.coeff.b * y)/this->str.coeff.a), y);
	// formatted_log("Point on straight [%f, %f]", result.x, result.y);
	return result;
}

cv::Point2f Rectification::getPointOnStraightX(double x){
	// formatted_log("x = %f", x);
	// formatted_log("A = %f, B = %f, C = %f", this->str.coeff.a, this->str.coeff.b, this->str.coeff.c);	
	// formatted_log("y = %f", (this->str.coeff.c - this->str.coeff.a * x)/this->str.coeff.b);
	cv::Point2f result(x, ((this->str.coeff.c - this->str.coeff.a * x)/this->str.coeff.b));
	// formatted_log("Point on straight [%f, %f]", result.x, result.y);
	return result;
}

cv::Mat Rectification::straightenImg(){
	cv::Mat dst;
	// this->computeAngle();


 //    //Rotation and translation
 //    cv::Point2f center(this->img.cols / 2.0, this->img.rows / 2.0);

 //    cv::Mat rotation_matrix = getRotationMatrix2D(center, this->angle, 1);

 //    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), this->img.size(), this->angle).boundingRect2f();  

 //    this->translationX = bbox.width / 2.0 - center.x;
 //    this->translationY = bbox.height / 2.0 - center.y;  

 //    rotation_matrix.at<double>(0,2) += this->translationX;
 //    rotation_matrix.at<double>(1,2) += this->translationY;

 //    //Rotating image
 //    warpAffine(this->img, dst, rotation_matrix, bbox.size());

    return dst;
}