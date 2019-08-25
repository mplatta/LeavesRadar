#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cartographer.hpp"
#include "formatted_log.hpp"
#include "thread_pool.hpp"
// #include "TSafeQueue.hpp"
// #include "store_queue.hpp"

using namespace cv;

// #define NUM_THREADS std::thread::hardware_concurrency()

static bool dirExists(const char *path)
{
	struct stat info;

	if( stat( path, &info ) != 0 )
    	formatted_err( "Cannot access %s", path );
	else if( info.st_mode & S_IFDIR )
	{
    	formatted_log( "%s directory exists", path );

    	return true;
	}
	else
    	formatted_err( "%s is no directory", path );
	
	return false;
}

int main( int argc, char** argv ) 
{
	ThreadPool *pool = new ThreadPool();

	string file = "";

	if (argc > 1) 
	{
		for (int i = 0; i < argc; ++i) 
		{
			if ( (strcmp(argv[i], "--file") == 0 ) &&
				 (argc >= i + 1 ) )
			{
				file = argv[i + 1];
			}

			if ( strcmp(argv[i], "-g") ) {
				// TODO add flag to run in graphic mode
				;
			}
		}
	}

	// tmp
	if (file == "")
	{
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
	}
	else 
	{
		std::vector<string> image_names;

		if (dirExists(file.c_str()))
		{
			glob(file + "/*.jpg", image_names);

			for (size_t i = 0; i < image_names.size(); ++i)
			{
				printf("%s\n", image_names[i].c_str());


				// cv::Point p_z = cv::Point(0, 0);

				// Cartographer *cartographer = new Cartographer();
				// // std::string *path_ = (std::string *)path;

				// cv::Mat image = cv::imread(image_names[i], CV_LOAD_IMAGE_COLOR);
				// cartographer->setSrcImg(image);
				// cartographer->makeBorder(false);
				
				// std::vector<cv::Point> c = cartographer->getContour();
				// // formatted_log("Add %s to foldingRule", (*path_).c_str());

				// pool->getSQueue()->push( { NULL, &c, &p_z } );
				pool->getSQueue()->push( { NULL, &image_names[i], NULL } );
				// sQueue->push({&cartographer_worker, &image_names[i]});
			}

			pool->start();
		}

	}
	
	return 0;
}
