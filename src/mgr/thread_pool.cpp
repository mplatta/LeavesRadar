#include "thread_pool.hpp"

TSafeQueue<store_queue> *ThreadPool::sQueue;
bool ThreadPool::stop_flag;

void ThreadPool::worker(int id) 
{
	// TODO: add smth to stop thread if finish all jobs
	// add if NULL do nothing
	do
	{
		store_queue st = ThreadPool::sQueue->pop();
		if (st.func != NULL && st.fir_data != NULL) 
			st.func(st.fir_data, st.sec_data);
		else if (st.func == NULL && st.fir_data != NULL)
			ThreadPool::cartographer_worker(st.fir_data, NULL);

	} while (!ThreadPool::stop_flag);
}

void ThreadPool::folding_rule_worker(void *contour, void *point_zero)
{
	FoldingRule *foldingRule = new FoldingRule();
	
	std::vector<cv::Point> *contour_ = (std::vector<cv::Point> *)contour;
	cv::Point *point = (cv::Point *)point_zero;
	foldingRule->setContour(*contour_);
	foldingRule->setCenter(*point);

	std::vector<double> histogram = foldingRule->getHistogram(0.5, false);
	// TODO: save histogram in file

	delete contour_;
	delete foldingRule;
}

// std::vector<std::vector<cv::Point> > ThreadPool::cartographer_worker_non_static()

void ThreadPool::cartographer_worker(void *path, void *not_use) 
{
	// tmp
	cv::Point p_z = cv::Point(0, 0);

	Cartographer *cartographer = new Cartographer();
	std::string *path_ = (std::string *)path;

	cv::Mat image = cv::imread((*path_).c_str(), CV_LOAD_IMAGE_COLOR);
	cartographer->setSrcImg(image);
	cartographer->makeBorder(false);
	// TODO: change to one contour
	std::vector<std::vector<cv::Point> > c = cartographer->getContours();

	ThreadPool::sQueue->push( {ThreadPool::folding_rule_worker, &c, &p_z} );

	delete cartographer;
	delete path_;
}

/*-----------------------PUBLIC---------------------------- */

void ThreadPool::start() 
{
	stop_flag = false;
	std::thread threads[NUM_THREADS];
	
	for ( int i = 0; i < NUM_THREADS; ++i)
	{
		threads[i] = std::thread(&ThreadPool::worker, i);
	}
}

void ThreadPool::stop()
{
	ThreadPool::stop_flag = true;
}