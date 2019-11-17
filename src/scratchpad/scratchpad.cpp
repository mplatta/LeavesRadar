/**
 * place to test ideas and solutions
*/

#include "TSafeQueue.hpp"
#include "store_queue.hpp"
#include "opencv_headers.hpp"
#include "cartographer.hpp"
#include "folding_rule.hpp"
#include "Entity.hpp"
#include "formatted_log.hpp"
#include "rectification.hpp"
#include "symmetry_detector.hpp"
#include "straight.hpp"
#include "starting_point.hpp"
#include "dir_functions.hpp"

using namespace cv;

int main( int argc, char** argv ) 
{
	formatted_log("Place for your fun! :*");

		
	namedWindow("z1", WINDOW_NORMAL);

	cv::Mat tmp;
	std::string path_ = "../img/Lonicera_henryi_12.png";

	cv::Mat image_ = cv::imread(path_.c_str(), CV_LOAD_IMAGE_COLOR);

	if (image_.empty())
	{
		formatted_err("Could not open or find the image \"%s\"", path_.c_str());

		// return;
	}
//////////////////////////////////////////////////////////////////////////////
	Cartographer *cartographer = new Cartographer();
	cartographer->setSrcImg(image_.clone());
	cartographer->makeBorder(true);

	std::vector<cv::Point> contour = cartographer->getContour();
	Mat image = cartographer->getCuted().clone();

	imshow("z1", image);

///////////////////////////////////////////////////////////////////////////////

	SymmetryDetector detector( image );
	straight_t sym = detector.getResult();

	StartingPoint sp(image, sym);
	cv::Point2f starting = sp.getStartingPoint(0.5);

	Rectification rec(image, sym);

	tmp = rec.straightenImg();
	rec.straightenPoint(starting);

	// circle(tmp, Point(maxX2, maxY2), 4, Scalar(0,0,255), 3, 1, 0);
	circle(tmp, starting, 2, Scalar(0,0,255), 2, 1, 0);


//////////////////////////////////////////////////////////////////

	// Cartographer *cartographer = new Cartographer();
	// cartographer->setSrcImg(rec.getImg());
	// cartographer->makeBorder(true);

	// std::vector<cv::Point> contour = cartographer->getContour();

	// tmp solution
	for (size_t i = 0; i < contour.size(); i++)
	{
		Point2f p = contour[i];
		rec.straightenPoint(p);
		contour[i] = p;
		circle(tmp, contour[i], 15, Scalar(0,0,255), 1, 1, 0);
	}

///////////////////////////////////////////////////////////////////
	const double angle = 0.5;
	unsigned long long id;

	formatted_log("Start folding_rule_worker() for %s", path_.c_str());

	FoldingRule *foldingRule = new FoldingRule();

	foldingRule->setContour(contour);
	foldingRule->setCenter(starting);

	std::vector<double> histogram = foldingRule->getHistogram(angle, false);

	for (size_t i = 0; i < foldingRule->getPoints().size(); i++)
	{
		line(tmp, starting, foldingRule->getPoints()[i], Scalar(0,255,0), 1,1,0);
		circle(tmp, foldingRule->getPoints()[i], 1, Scalar(0,255,0), 1, 1, 0);

		imshow("z1", tmp);
		waitKey(10);
	}

	std::string file_name = extract_name(path_);
	std::string tmp_name = delete_last_slash("../out/");

	foldingRule->saveHistogram( tmp_name, 
		file_name + "(test)", angle );

	formatted_inf("End working for: %s", path_.c_str());

	resizeWindow("z1", 600, 600);
	// resizeWindow("z2", 600, 600);;
	// imshow("z2", tmp);

	// path_ = NULL;
	delete foldingRule;
	delete cartographer;

	waitKey(0);

	return 0;
}