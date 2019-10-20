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
				st.func(st.fir_data, st.sec_data, st.thr_data);
			}
			else if (st.func == NULL && st.fir_data != NULL)
			{
				// ThreadPool::folding_rule_worker(st.fir_data, st.sec_data);
				ThreadPool::cartographer_worker(st.fir_data, NULL, NULL);	
			}
		}
		else 
		{
			formatted_log("sleep for %d", id);
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

void ThreadPool::folding_rule_worker(void *contour, void *size, void *point_zero)
{
	// std::vector<cv::Point> *contour_ = (std::vector<cv::Point> *)contour;
	// size_t *s = (size_t *)size;
	// cv::Point *contour_ = (cv::Point *)contour;
	// cv::Point *point = (cv::Point *)point_zero;

	int *i = (int *)contour;

	// std::vector<cv::Point> vec_contour;
	// formatted_log("sfdsfsdfsfsdfsdfdfsdfsd");
	// vec_contour.assign(contour_, contour_ + (750));

	// formatted_log("DDDDDD: %d", (*contour_).x);

	// FoldingRule *foldingRule = new FoldingRule();

	// foldingRule->setContour(vec_contour);
	// foldingRule->setCenter(cv::Point(0,0));

	// std::vector<double> histogram = foldingRule->getHistogram(0.5, false);
	formatted_log("SSSSSSSSSSSSSSSSS %d", *i);
	// TODO: save histogram in file

	formatted_log("End foldingRule");

	// delete contour_;
	// delete foldingRule;
	// delete point;
}

// std::vector<std::vector<cv::Point> > ThreadPool::cartographer_worker_non_static()

void ThreadPool::cartographer_worker(void *path, void *not_use, void *not_use_2) 
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

	// TSafeQueue<Entity> q = (TSafeQueue<Entity>)entities->getQueue();

	int index = -1;

	for (size_t i = 0; i < entities->size(); i++) {
		if (ThreadPool::entities->getItem(i).isThisEntity(*path_)) 
		{
			index = i;
			break;
		}
	}

	if (index == -1) {
		formatted_err("KUPA");
	}
	// std::vector<cv::Point> c = cartographer->getContour();
	// size = c.size();
	// memcpy( arr_contour, &c[0], sizeof( cv::Point ) * size );
	
	formatted_log("Add %s to foldingRule", (*path_).c_str());
	// formatted_log("AddS %d to foldingRule", size);
	// formatted_log("AddC %d to foldingRule", arr_contour[0].x);
	// formatted_log("AddC %d to foldingRule", p_z.x);

	ThreadPool::sQueue->push( { ThreadPool::folding_rule_worker, &index, NULL, NULL } );

	formatted_log("Queue size %d", sQueue->size());

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