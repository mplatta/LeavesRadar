#include "folding_rule.hpp"

void FoldingRule::scaling(double min, double max)
{
	formatted_log("Scaling histogram");

	for (int i = 0; i < histogram.size(); ++i) 
	{
		histogram[i] = ((histogram[i] - min) / (max - min));
	}
}

void FoldingRule::createHistogram(double angle) 
{	
	formatted_log("Creating histogram");

	double max = 0.0;

	if (histogram.size() != 0) histogram.clear();

	// for (int i = 0; i < this->contour.size(); ++i)
	// {
	// 	double tmp = cv::norm(this->contour[i] - this->center);

	// 	if (tmp > max) max = tmp;

	// 	histogram.push_back(tmp);
	// }

	cv::Point *p = new cv::Point();
	cv::Point w = cv::Point(this->center.x, this->center.y + 100);

	for (double i = 0.; i < 360.; i += angle)
	{
		double rad = (i * M_PI) / 180;
		
		w = this->rotatePoint(w, this->center, rad);
		p = this->findIntersect(this->center, w);

		if (p == NULL) formatted_err("DUPA");

		// formatted_log("x: %d, y:%d", p->x, p->y);

		if (p) 
		{
			this->points.push_back(*p);
			double tmp = cv::norm(*p - this->center);
			
			if (tmp > max) max = tmp;

			histogram.push_back(tmp);
		}
		else histogram.push_back(0.0);	
	}

	this->scaling(0.0, max);
}

bool FoldingRule::isIntersect(cv::Point A, cv::Point B, cv::Point C, cv::Point D)
{
	float x1 = C.x;
	float y1 = C.y;
	float x2 = D.y;
	float y2 = D.y;

	float x3 = A.x;
	float y3 = A.y;
	float x4 = B.y;
	float y4 = B.y;

	float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	if (den == 0) return false;

	float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den; 
	float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

	return (t > 0 && t < 1 && u > 0);
}

cv::Point *FoldingRule::getIntersect(cv::Point A, cv::Point B, cv::Point C, cv::Point D) 
{
	if (!isIntersect(A, B, C, D)) return NULL;

	cv::Point *p = new cv::Point();
	straight_t a = createStraightFrom2Point(A, B);
	straight_t b = createStraightFrom2Point(C, D);

	p = isIntersectStright_t(a, b);

	return p;
}

cv::Point *FoldingRule::findIntersect(cv::Point A, cv::Point B) 
{
	// brute force temporarily
	cv::Point *p = new cv::Point();

	for (size_t c = 0; c < this->contour.size(); ++c)
	{

		int d = (c + 1) % this->contour.size();
		p = this->getIntersect(A, B, this->contour[c], this->contour[d]);

		if (p) return p;
	}

	// delete p;

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

std::vector<double> FoldingRule::getHistogram(double angle, bool force) 
{
	if (force || this->histogram.size() == 0) 
		this->createHistogram(angle);

	return this->histogram;
}

void FoldingRule::saveHistogram(std::string path, std::string name)
{
	std::string full_path = path + "/" + name;

	std::ofstream file;
	file.open(full_path);

	for (size_t i = 0; i < this->histogram.size(); ++i)
	{
		file << std::to_string(this->histogram[i]) << ";" << std::endl;
	}

	file.close();
}