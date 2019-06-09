#include "cartographer.hpp"
#include "formatted_log.hpp"

using namespace cv;

int main( int argc, char** argv ) 
{
	// for tests
	formatted_log("aaa%d", 2);
	formatted_log("aaa");

	Cartographer *cartographer = new Cartographer();

	Mat image, src_gray;
	Mat dst, detected_edges;

	image = imread("../res/oak.jpg", CV_LOAD_IMAGE_COLOR);

	if(image.empty()) 
	{
		formatted_err("Could not open or find the image");
		
		return -1;
	}

	cartographer->setSrcImg(image);
	dst = cartographer->getBordered(false);

	namedWindow( "Display window", cv::WINDOW_AUTOSIZE );

	imshow( "Display window", dst);
	
	waitKey(0);

	delete cartographer;
	
	return 0;
}