#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cartographer.hpp"
#include "formatted_log.hpp"
#include "thread_pool.hpp"

using namespace cv;

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
				formatted_inf("%s\n", image_names[i].c_str());
				pool->getSQueue()->push( { NULL, &image_names[i], NULL } );
			}

			pool->start();
		}

	}
	
	formatted_inf("PROGRAM EXECUTED");

	return 0;
}
