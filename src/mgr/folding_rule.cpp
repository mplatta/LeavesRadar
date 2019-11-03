#include "folding_rule.hpp"

void FoldingRule::scaling(double min, double max)
{
	formatted_log("Scaling histogram");

	for (size_t i = 0; i < this->histogram.size(); ++i) 
	{
		if (this->histogram[i] < 0.)
		{
			if (this->histogram[this->histogram.size() - 1 - i] > 0.)
				this->histogram[i] = this->histogram[this->histogram.size() - 1 - i];
			else if (this->histogram[(i + 1) % this->histogram.size()] > 0.)
				this->histogram[i] = this->histogram[i + 1];
			else if (this->histogram[(i - 1 + this->histogram.size()) % this->histogram.size()] > 0.)
				this->histogram[i] = this->histogram[i - 1];
			else histogram[i] = 0.;
		}
	}

	for (size_t i = 0; i < this->histogram.size(); ++i) 
	{
		this->histogram[i] = ((this->histogram[i] - min) / (max - min));
	}
}

void FoldingRule::createHistogram(double angle) 
{	
	formatted_log("Creating histogram");

	double max = 0.0;

	if (histogram.size() != 0) histogram.clear();

	cv::Point *p = new cv::Point();
	cv::Point w = cv::Point(this->center.x, this->center.y + 100);
	
	double rad = (angle * M_PI) / 180;
	
	for (double i = 0.; i < 360.; i += angle)
	{
		w = this->rotatePoint(w, this->center, rad);
		p = this->findIntersect(this->center, w);

		if (p) 
		{
			this->points.push_back(*p);
			double tmp = cv::norm(*p - this->center);
			
			if (tmp > max) max = tmp;

			histogram.push_back(tmp);
		}
		else 
		{
			histogram.push_back(-1.0);
			formatted_war("Not found intersecting point for angle: %f\nDirection point (%d:%d)", i, w.x - this->center.x, w.y - this->center.y);
		}
	}

	formatted_log("histogram size: %d", this->histogram.size());

	this->scaling(0.0, max);
}

// void FoldingRule::createHistogram2(double angle, cv::Mat img, std::string name) 
// {	
// 	formatted_log("Creating histogram");

// 	double max = 0.0;

// 	if (histogram.size() != 0) histogram.clear();

// 	// for (int i = 0; i < this->contour.size(); ++i)
// 	// {
// 	// 	double tmp = cv::norm(this->contour[i] - this->center);

// 	// 	if (tmp > max) max = tmp;

// 	// 	histogram.push_back(tmp);
// 	// }
	
// 	cv::Point *p = new cv::Point();
// 	cv::Point w = cv::Point(this->center.x, this->center.y + 100);

// 	double rad = (angle * M_PI) / 180;
	

// 	for (double i = 0.; i < 360.1; i += angle)
// 	{
// 		cv::Mat ttt = img.clone();
// 		cv::line(ttt, this->center, w, cv::Scalar(0, 0, 255), 1, 1);
//     	imshow( name, ttt );

// 		// formatted_log("D: %f - R: %f", i, rad);
		
// 		w = this->rotatePoint(w, this->center, rad);
// 		// getRotationMatrix2D( Point2f(0.0, 0.0), angle, 1.0);
// 		// p = this->findIntersect(this->center, w);
// 		p = this->findIntersectA(this->center, w, ttt, name);

// 		// circle(img, contour[i], 2, Scalar(0, 0, 255), 1, 1);

// 		if (p == NULL) {
// 			formatted_war("Not found intersecting point for angle: %f\nDirection point (%d:%d)", i, w.x, w.y);
// 		}

// 		// formatted_log("x: %d, y:%d", p->x, p->y);

// 		if (p) 
// 		{
// 			this->points.push_back(*p);
// 			double tmp = cv::norm(*p - this->center);
			
// 			if (tmp > max) max = tmp;

// 			histogram.push_back(tmp);
// 		}
// 		else histogram.push_back(0.0);	
//     	cvWaitKey(30);
// 	}

// 	this->scaling(0.0, max);
// }

bool FoldingRule::isIntersect(cv::Point A, cv::Point B, cv::Point C, cv::Point D)
{
	float x1 = C.x;
	float y1 = C.y;
	float x2 = D.x;
	float y2 = D.y;

	float x3 = A.x;
	float y3 = A.y;
	float x4 = B.x;
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

	return NULL;
}

// cv::Point *FoldingRule::findIntersectA(cv::Point A, cv::Point B, cv::Mat img, std::string name ) 
// {
// 	// brute force temporarily
// 	cv::Point *p = new cv::Point();

// 	// formatted_log("A: (%d:%d)  B: (%d:%d)", A.x, A.y, B.x, B.y);

// 	for (size_t c = 0; c < this->contour.size(); ++c)
// 	{
// 		cv::Mat ttt = img.clone();
// 		int d = (c + 1) % this->contour.size();

// 		p = this->getIntersect(A, B, this->contour[c], this->contour[d]);

// 		circle(ttt, this->contour[c], 2, cv::Scalar(0, 0, 255), 1, 1);
// 		circle(ttt, this->contour[d], 2, cv::Scalar(255, 0, 0), 1, 1);
// imshow( name, ttt );
// 		// formatted_log("%d - %d,   (%d:%d) - (%d:%d)", c, d, this->contour[c].x, this->contour[c].y, this->contour[d].x, this->contour[d].y);

// 		cvWaitKey(50);
// 		if (p) return p;
// 	}

// 	// delete p;

// 	return NULL;
// }

cv::Point FoldingRule::rotatePoint(cv::Point rotated, cv::Point center, double angle )
{
	cv::Point r;

	double ox = center.x;
	double oy = center.y;

	double px = rotated.x;
	double py = rotated.y;

	double qx = ox + cos(angle) * (px - ox) - sin(angle) * (py - oy);
	double qy = oy + sin(angle) * (px - ox) + cos(angle) * (py - oy);

	r.x = (int)round(qx);
	r.y = (int)round(qy);

	return r;
}

std::vector<double> FoldingRule::getHistogram(double angle, bool force) 
{
	if (force || this->histogram.size() == 0) 
		this->createHistogram(angle);

	return this->histogram;
}

// std::vector<double> FoldingRule::getHistogramAnim(double angle, bool force, cv::Mat img, std::string name)
// {
// 	if (force || this->histogram.size() == 0) 
// 		this->createHistogram2(angle, img, name);

// 	return this->histogram;
// }

void FoldingRule::saveHistogram(std::string path, std::string name, const double angle)
{
	std::string full_path = path + "/" + name;

	std::ofstream file;
	file.open(full_path);

	file << name << ";" << std::endl;
	file << this->center.x << ":" << this->center.y << ";" << std::endl;

	for (size_t i = 0; i < this->histogram.size(); ++i)
	{	
		double angle_ = angle * i; 
		file << std::to_string(angle_) << ";" << std::to_string(this->histogram[i]) << ";" << std::endl;
	}

	file.close();
}