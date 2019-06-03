#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main( int argc, char** argv ) 
{
	Mat image, src_gray;
	Mat dst, detected_edges;

	image = imread("../res/sample.jpg", CV_LOAD_IMAGE_COLOR);
	
	if(image.empty()) 
	{
		std::cout <<  "Could not open or find the image" << std::endl;
		return -1;
	}

	dst.create(image.size(), image.type());
	
	cvtColor(image, src_gray, CV_BGR2GRAY);

	namedWindow( "Display window", cv::WINDOW_AUTOSIZE );

	blur(src_gray, detected_edges, Size(3, 3));
	Canny(detected_edges, detected_edges, 100, 3, 3);
	
	dst = Scalar::all(0);

	image.copyTo(dst, detected_edges);

// 
	// threshold(src_gray, image, 0, 255, 0);
	// int threshold_type = 3;
	// int const max_type = 4;

	// createTrackbar( 3, "AAA", )

	imshow( "Display window", dst );
	
	waitKey(0);
	return 0;
}