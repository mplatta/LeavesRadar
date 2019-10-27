#include <iostream>
#include <pthread.h>

#include "dir_functions.hpp"
#include "formatted_log.hpp"
#include "thread_pool.hpp"

using namespace cv;

void all_image_mode(std::string file, ThreadPool *pool) 
{
	std::vector<string> image_names;

	if (dir_exists(file.c_str()))
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
		1 - run program for all img in dir
		2 - run program for one img
	*/
	int flag = 0;
	std::string file = "";
	std::string out_file = "";

	if (argc > 1) 
	{
		for (int i = 0; i < argc; ++i) 
		{
			if ( (strcmp(argv[i], "--path") == 0 ) &&
				 (argc >= i) && (flag != 2) )
			{
				formatted_inf("All image mode");
				file = argv[i + 1];
				flag = flag | 1;
			}
			if ( (strcmp(argv[i], "--file") == 0 ) &&
				 (argc >= i) && (flag != 1) )
			{
				formatted_inf("One image mode");
				file = argv[i + 1];
				flag = flag | 2;
			}
			if ( (strcmp(argv[i], "--out") == 0 ) &&
				 (argc >= i) )
			{
				out_file = argv[i + 1];
				flag = flag | 4;
			}

			if ( strcmp(argv[i], "-g") ) {
				// TODO add flag to run in graphic mode
				;
			}
		}
	}

	if (flag == 0)
	{
		formatted_inf("RUN PROGRAM COMMAND:\nRun all image mode: ./mgr --path ../dir/ [--out] [/path/to/out/dir]\nRun one image mode: ./mgr --file ../dir/image.jpg [--out] [/path/to/out/dir]");
	}
	else 
	{

		if (!out_file.empty())
		{
			if (!dir_exists(out_file.c_str())) return -1;

			pool->setOutPath(out_file);
		}

		switch(flag) {
			case 1 : case 5:
				all_image_mode(file, pool);
				break;
			case 2 : case 6:
				one_image_mode(file, pool);
				break;
			default :
				formatted_err("SOMTHING IS NO YES");
		}
	}
	
	formatted_inf("PROGRAM EXECUTED");

	return 0;
}
