#include "folding_rule.hpp"

void FoldingRule::scaling(double min, double max)
{
	for (int i = 0; i < histogram.size(); ++i) {
		histogram[i] = ((histogram[i] - min) / (max - min));
	}
}

void FoldingRule::createHistogram() 
{	
	double max = 0.0;

	if (histogram.size != 0) histogram.clear();

	for (int i = 0; i < this->contour.size(); ++i)
	{
		double tmp = cv::norm(this->contour[i] - this->center);

		if (tmp > max) max = tmp;

		histogram.push_back(tmp);
	}

	this->scaling(0.0, max);
}

std::vector<double> FoldingRule::getHistogram(bool force) 
{
	if (force || this->histogram.size() == 0) 
		this->createHistogram();

	return this->histogram;
}