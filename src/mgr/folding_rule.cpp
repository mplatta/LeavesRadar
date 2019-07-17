#include "folding_rule.hpp"

void FoldingRule::scaling(double min, double max)
{
	formatted_log("Scaling histogram");

	for (int i = 0; i < histogram.size(); ++i) {
		histogram[i] = ((histogram[i] - min) / (max - min));
	}
}

void FoldingRule::createHistogram() 
{	
	formatted_log("Creating histogram");

	double max = 0.0;

	if (histogram.size() != 0) histogram.clear();

	for (int i = 0; i < this->contour.size(); ++i)
	{
		double tmp = cv::norm(this->contour[i] - this->center);

		if (tmp > max) max = tmp;

		histogram.push_back(tmp);
	}

	// div by 2
	// ..

	this->scaling(0.0, max);
}

cv::Point *FoldingRule::isIntersect(cv::Point A, cv::Point B, cv::Point C, cv::Point D) 
{
	cv::Point *p = new cv::Point();
	straight_t a = createStraightFrom2Point(A, B);
	straight_t b = createStraightFrom2Point(C, D);

	formatted_log("Check intersecting");

	p = isIntersectStright_t(a, b);

	if (!p) return NULL;

	if ( min(C.x, D.x) <= p->x && p->x <= max(C.x, D.x) && 
		 min(C.y, D.y) <= p->y && p->y <= max(C.y, D.y) )
		return p;

	formatted_log("Point is not in line segment!");

	return NULL; 
}

cv::Point FoldingRule::findIntersect(cv::Point A, cv::Point B) 
{
	// brute force temporarily
	cv::Point *p = new cv::Point();

	for (int c = 0; c < this->contour.size(); ++c)
	{

		int d = (c + 1) % this->contour.size();

		p = this->isIntersect(A, B, this->contour[c], this->contour[d]);

		if (p) return p
	}

	delete p;

	return NULL;
}

cv::Point FoldingRule::rotatePoint(cv::Point rotated, cv::Point center, double angle )
{
	cv::Point r;
	cv::Point tmp = cv::Point(rotated.x - center.x, rotated.y - center.y);

	double tx = ((double)tmp.x * cos(angle) - (double)tmp.y * sin(angle));
	double ty = ((double)tmp.y * cos(angle) + (double)tmp.x * sin(angle));

	r.x = (int)(tx + (double)center.x);
	r.y = (int)(ty + (double)center.y);

	return r;
}

std::vector<double> FoldingRule::getHistogram(bool force) 
{
	if (force || this->histogram.size() == 0) 
		this->createHistogram();

	return this->histogram;
}