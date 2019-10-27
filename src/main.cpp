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

void all_image_mode(string file, ThreadPool *pool) 
{
	std::vector<string> image_names;

	if (dirExists(file.c_str()))
	{
		glob(file + "/*.jpg", image_names);

		for (size_t i = 0; i < image_names.size(); ++i)
		{
			formatted_inf("Add file to queue: %s", image_names[i].c_str());
			pool->getSQueue()->push( { NULL, &image_names[i], NULL } );
		}

		formatted_inf("WORKING");
		pool->start();
	} 
	else 
	{
		formatted_err("Direction: \"%s\" not exists", file.c_str());
	}
}

void one_image_mode(string file, ThreadPool *pool) 
{
	formatted_inf("%s", file.c_str());
	pool->getSQueue()->push( { NULL, &file, NULL } );
	formatted_inf("WORKING");
	pool->start();
}

int main( int argc, char** argv ) 
{
	ThreadPool *pool = new ThreadPool();

	/*	type of start program
		0 - run program for all img in dir
		1 - run program for one img
	*/
	int flag = -1;
	string file = "";

	if (argc > 1) 
	{
		for (int i = 0; i < argc; ++i) 
		{
			if ( (strcmp(argv[i], "--path") == 0 ) &&
				 (argc >= i) && (flag != 1) )
			{
				file = argv[i + 1];
				flag = 0;
				formatted_inf("All image mode");
			}
			if ( (strcmp(argv[i], "--file") == 0 ) &&
				 (argc >= i) && (flag != 0) )
			{
				file = argv[i + 1];
				flag = 1;
				formatted_inf("One image mode");
			}

			if ( strcmp(argv[i], "-g") ) {
				// TODO add flag to run in graphic mode
				;
			}
		}
	}

	// tmp
	if (flag == -1)
	{
		formatted_inf("RUN PROGRAM COMMAND:\nRun all image mode: ./mgr --path ../dir/\nRun one image mode: ./mgr --file ../dir/image.jpg");
	}
	else 
	{
		switch(flag) {
			case 0 :
				all_image_mode(file, pool);
				break;
			case 1 :
				one_image_mode(file, pool);
				break;
			default :
				formatted_err("SOMTHING IS NO YES");
		}
	}
	
	formatted_inf("PROGRAM EXECUTED");

	return 0;
}
