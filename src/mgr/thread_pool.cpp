#include "thread_pool.hpp"

TSafeQueue<store_queue> *ThreadPool::sQueue;
TSafeQueue<Entity>      *ThreadPool::entities;
bool					 ThreadPool::stop_flag;
bool					*ThreadPool::stop_flags;

void ThreadPool::worker(int id) 
{
	int flag = 0;

	formatted_log("From %d, queue size %d", id, sQueue->size());

	do
	{	
		if (!sQueue->empty()) 
		{
			flag = 0;

			store_queue st = ThreadPool::sQueue->pop();
			if (st.func != NULL && st.fir_data != NULL) 
			{
				st.func(st.fir_data, st.sec_data);
			}
			else if (st.func == NULL && st.fir_data != NULL)
			{
				// ThreadPool::folding_rule_worker(st.fir_data, st.sec_data);
				ThreadPool::cartographer_worker(st.fir_data, NULL);	
			}
		}
		else 
		{
			// formatted_log("sleep for %d", id);
			std::this_thread::sleep_for( std::chrono::milliseconds(500) ); 
			flag++;
		}
		
		bool stop = false;

		if (flag >= 4) 
		{
			ThreadPool::stop_flags[id] = true;

			for (unsigned int i = 0; i < NUM_THREADS; ++i)
			{
				if (!stop_flags[i])
				{
					stop = false;
					break;
				}
				else
				{
					stop = true;
				}
			}
		}

		if (stop) ThreadPool::stop_flag = true;

	} while (!ThreadPool::stop_flag);
}

void ThreadPool::folding_rule_worker(void *path, void *not_use)
{
	Entity *e;
	int index = -1;
	
	std::string *path_ = (std::string *)path;


	for (size_t i = 0; i < entities->size(); i++) {
		if ((e = ThreadPool::entities->getItem(i).isThisEntityE(*path_)) != NULL ) 
		{
			index = 0;
			break;
		}
	}

	if (index == -1) {
		formatted_err("SOMTHING IS NO YES");
	}

	FoldingRule *foldingRule = new FoldingRule();

	foldingRule->setContour(e->getContour());
	foldingRule->setCenter(cv::Point(0,0));

	std::vector<double> histogram = foldingRule->getHistogram(0.5, false);

	// TODO: save histogram in file

	formatted_log("End foldingRule");

	path_ = NULL;
	delete foldingRule;
	// delete point;
}

void ThreadPool::cartographer_worker(void *path, void *not_use) 
{
	// tmp
	cv::Point p_z = cv::Point(0, 0);

	cv::Point *arr_contour = new cv::Point();
	size_t size;

	Cartographer *cartographer = new Cartographer();
	std::string *path_ = (std::string *)path;

	cv::Mat image = cv::imread((*path_).c_str(), CV_LOAD_IMAGE_COLOR);
	cartographer->setSrcImg(image);
	cartographer->makeBorder(false);

	Entity *e = new Entity( (*path_), cartographer->getContour() );

	ThreadPool::entities->push(*e);
	
	formatted_log("Add %s to foldingRule", (*path_).c_str());

	ThreadPool::sQueue->push( { ThreadPool::folding_rule_worker, path_, NULL } );

	// formatted_log("Queue size %d", sQueue->size());

	cartographer = NULL;
	path_ = NULL;
	arr_contour = NULL;
	// p_z = NULL;
	// delete cartographer;
	// delete path_;
}

void ThreadPool::set_starts_value() 
{
	for (unsigned int i = 0; i < NUM_THREADS; ++i )
	{
		ThreadPool::stop_flags[i] = false;
	}
}

/*-----------------------PUBLIC---------------------------- */

void ThreadPool::start() 
{
	formatted_log("Starts %d threads", NUM_THREADS);

	stop_flag = false;
	std::thread threads[NUM_THREADS];
	
	for (unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		threads[i] = std::thread(&ThreadPool::worker, i);
	}

	for (unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		threads[i].join();
	}
}

void ThreadPool::stop()
{
	ThreadPool::stop_flag = true;
}