#include "cartographer.hpp"
#include "formatted_log.hpp"

using namespace cv;

int main( int argc, char** argv ) 
{
	formatted_log("adsadasdas %d", 12);
	formatted_err("adsadasdas %d", 12);
	// LOG("aaaaaaa%d", 33);
	Cartographer *cartographer = new Cartographer();

	Mat image, src_gray;
	Mat dst, detected_edges;

	image = imread("../res/oak.jpg", CV_LOAD_IMAGE_COLOR);

	if(image.empty()) 
	{
		std::cout <<  "Could not open or find the image" << std::endl;
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