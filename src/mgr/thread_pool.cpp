#include "thread_pool.hpp"

TSafeQueue<store_queue> *ThreadPool::sQueue;
//TSafeQueue<Entity>      *ThreadPool::entities;
String              	 ThreadPool::_out_path;
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

			formatted_inf("QUEUE size: %d", sQueue->size());
			store_queue st = ThreadPool::sQueue->pop();
			if (st.func != NULL && st.fir_data != NULL) 
			{
				st.func(st.fir_data, st.sec_data);
			}
			else if (st.func == NULL && st.fir_data != NULL)
			{
				// if first data is empty, start first worker func
				// ThreadPool::symmetry_worker(st.fir_data, st.sec_data);	
				ThreadPool::all_in_one(st.fir_data, st.sec_data);	
			}
		}
		else 
		{
			// threads wait 500ms 4 times, after it they stopped  
			// flag - is variable to count times of waiting
			formatted_inf("Waiting for new job...");
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

void ThreadPool::all_in_one(void *path, void *not_use)
{	
	const int frame = 0;

	cv::Mat tmp;
	String *_path = (String *)path;
	String path_ = std::move(*_path);
	
	formatted_inf("Start working for: %s", path_.c_str());

	cv::Mat image_ = cv::imread	(path_.c_str(), CV_LOAD_IMAGE_COLOR);

	if (image_.empty())
	{
		formatted_err("Could not open or find the image \"%s\"", path_.c_str());

		return;
	}
	
/////////////////////////// CARTOGRAPHER ///////////////////////////////////////

	Cartographer *cartographer = new Cartographer();
	cartographer->setSrcImg(image_.clone());
	cartographer->makeBorder(true);

	std::vector<cv::Point> contour = cartographer->getContour();
	cv::Mat image = cartographer->getCuted().clone();

///////////////////////// SYMMETRY DETECTOR ////////////////////////////////////

	SymmetryDetector detector( image.clone() );
	straight_t sym = detector.getResult();

	StartingPoint sp(image.clone(), sym);
	cv::Point2f starting = sp.getStartingPoint(0.5);

	Rectification rec(image.clone(), sym);

	tmp = rec.straightenImg();
	rec.straightenPoint(starting);

//////////////////////////////////////////////////////////////////

	// tmp solution
	for (size_t i = 0; i < contour.size(); i++)
	{
		Point2f p = contour[i];
		rec.straightenPoint(p);
		contour[i] = p;
	}

//////////////////////////// FOLDING RULE /////////////////////////////////////
	const double angle = 0.5;
	unsigned long long id;

	formatted_log("Start folding_rule_worker() for %s", path_.c_str());

	FoldingRule *foldingRule = new FoldingRule();

	foldingRule->setContour(contour);
	foldingRule->setCenter(starting);

	std::vector<double> histogram = foldingRule->getHistogram(angle, false);
	String file_name = extract_name(path_);
	
	ThreadPool::sQueue->getId(&id);

	String tmp_name = delete_last_slash(ThreadPool::_out_path);

	foldingRule->saveHistogram( tmp_name, 
		file_name + "(" + std::to_string(id) + ")", angle );

	formatted_inf("End working for: %s", path_.c_str());

	// path_ = NULL;
	delete foldingRule;
	delete cartographer;
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