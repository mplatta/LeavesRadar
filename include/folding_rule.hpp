#ifndef _FOLDING_RULE_H_
#define _FOLDING_RULE_H_

#include "opencv_headers.hpp"

class FoldingRule
{
private:
	std::vector<cv::Point> contour  ;
	cv::Point              center   ;
	/** array of distances between point zero and contour points
		in 0-1 interval 	
	*/
	std::vector<double>    histogram;

	//scaling distance to 0-1 interval and save to histogram
	void scaling        (double min, double max);
	void createHistogram()                      ;

public:
	// getters
	std::vector<cv::Point> getContour  ()           { return this->contour  ; };
	cv::Point              getCenter   ()           { return this->center   ; };
	/** if force is true, histogram always will be calculate from zero;
		if false, function return histogram without calculation (if was calculate before)
	*/
	std::vector<double>    getHistogram(bool force);
	// std::vector<double>    getHistogram()           { return this->histogram; };

	// setters
	void setContour( std::vector<cv::Point> _contour);
	void setCenter ( cv::Point              _center );

	// unnecessary
	// inline static float distance(cv::Point a, cv::Point b) { return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); };
	
	// constructors/destructors
	FoldingRule (std::vector<cv::Point> _contour, cv::Point _center) { this->contour = _contour; this->center = _center; };
	FoldingRule () {};
	~FoldingRule() {};
};

#endif