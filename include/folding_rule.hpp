#ifndef _FOLDING_RULE_H_
#define _FOLDING_RULE_H_

#include <fstream>
#include <iostream>

#include "opencv_headers.hpp"
#include "formatted_log.hpp"
#include "constants.hpp"
#include "straight.hpp"

class FoldingRule
{
private:
	std::vector<cv::Point> contour ;
	// std::vector<std::vector<cv Point> > segregated_contour;
	cv::Point              center  ;
	/** array of distances between point zero and contour points
		in 0-1 interval 	
	*/
	std::vector<double>    histogram;
	std::vector<cv::Point> points;

	//scaling distance to 0-1 interval and save to histogram
	void scaling             ( double min, double max ) ;
	void createHistogram     ( double angle )           ;

	/**
		isIntersect Checks if the ray starting at point A passing 
		through point B crosses the segment C-D
		Line-Line intersection
		https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
	*/
	bool      isIntersect     ( cv::Point A, cv::Point B, cv::Point C, cv::Point D );
	/**
		getIntersect return pointer cv::Point if two lines intersect, 
		if not, return NULL pointer
		A and B - points for "zero" to end pointer
		C and D - points for line segment in contour
	*/
	cv::Point *getIntersect   ( cv::Point A, cv::Point B, cv::Point C, cv::Point D );
	/**
		return new point from rotated point "rotated" around "center" by angle 
	*/
	cv::Point  rotatePoint   ( cv::Point rotated, cv::Point center, double angle  );
	cv::Point  *findIntersect( cv::Point A, cv::Point B );

public:
	/* -------------------------- GETTERS ------------------------ */
	std::vector<cv::Point> getContour  () { return this->contour; };
	cv::Point              getCenter   () { return this->center ; };
	std::vector<cv::Point> getPoints   () { return this->points ; };
	/** if force is true, histogram always will be calculate from zero;
		if false, function return histogram without calculation (if was calculate before)
	*/
	std::vector<double>    getHistogram(double angle, bool force);
	// std::vector<double>    getHistogram()           { return this->histogram; };

	/* -------------------------- SETTERS ------------------------ */
	void setContour   ( std::vector<cv::Point> _contour ) { this->contour = _contour; };
	void setCenter    ( cv::Point              _center  ) { this->center  = _center ; };

	/* -------------------------- METHODS ------------------------ */
	void saveHistogram(std::string path, std::string name);
	// unnecessary
	// inline static float distance(cv::Point a, cv::Point b) { return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); };
	
	/* ------------------ CONSTRUCTORS/DESTRUCTORS --------------- */
	FoldingRule (std::vector<cv::Point> _contour, cv::Point _center) { this->contour = _contour; this->center = _center; };
	FoldingRule () {};
	~FoldingRule() {};
};

#endif
