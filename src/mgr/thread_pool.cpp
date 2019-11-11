#include "thread_pool.hpp"

TSafeQueue<store_queue> *ThreadPool::sQueue;
TSafeQueue<Entity>      *ThreadPool::entities;
std::string              ThreadPool::_out_path;
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
	std::string *_path = (std::string *)path;
	std::string path_ = std::move(*_path);
	
	formatted_inf("Start working for: %s", path_.c_str());

	cv::Mat image_ = cv::imread(path_.c_str(), CV_LOAD_IMAGE_COLOR);

	if (image_.empty())
	{
		formatted_err("Could not open or find the image \"%s\"", path_.c_str());

		return;
	}
	
//////////////////////////////////////////////////////////////////

	Cartographer *cartographer = new Cartographer();
	cartographer->setSrcImg(image_.clone());
	cartographer->makeBorder(true);

	std::vector<cv::Point> contour = cartographer->getContour();
	cv::Mat image = cartographer->getCuted().clone();

//////////////////////////////////////////////////////////////////

	float rho_divs   = hypotf( image.rows, image.cols ) + 1;
	float theta_divs = 180.0;

	SymmetryDetector detector( image.size(), Size(rho_divs, theta_divs), 1 );
	pair<cv::Point, cv::Point> symmetry = detector.getResult(image.clone());

	straight_t sym = createStraightFrom2Point(symmetry.first, symmetry.second);

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

///////////////////////////////////////////////////////////////////
	const double angle = 0.5;
	unsigned long long id;

	formatted_log("Start folding_rule_worker() for %s", path_.c_str());

	FoldingRule *foldingRule = new FoldingRule();

	foldingRule->setContour(contour);
	foldingRule->setCenter(starting);

	std::vector<double> histogram = foldingRule->getHistogram(angle, false);
	std::string file_name = extract_name(path_);
	
	ThreadPool::sQueue->getId(&id);

	std::string tmp_name = delete_last_slash(ThreadPool::_out_path);

	foldingRule->saveHistogram( tmp_name, 
		file_name + "(" + std::to_string(id) + ")", angle );

	formatted_inf("End working for: %s", path_.c_str());

	// path_ = NULL;
	delete foldingRule;
	delete cartographer;
}

void ThreadPool::folding_rule_worker(void *path, void *not_use)
{
	const double angle = 0.5;
	Entity *e;
	
	std::string *path_ = (std::string *)path;
	formatted_log("Start folding_rule_worker() for %s", (*path_).c_str());

	for (size_t i = 0; i < entities->size(); i++) {
		if ((e = ThreadPool::entities->getItem(i).isThisEntityE(*path_)) != NULL ) 
		{
			break;
		}
	}

	if (e == NULL) 
	{
		formatted_err("LOST ENTITY! %s", (*path_).c_str());
		return;
	}

	FoldingRule *foldingRule = new FoldingRule();

	foldingRule->setContour(e->getContour());
	foldingRule->setCenter(e->getPointZero());

	std::vector<double> histogram = foldingRule->getHistogram(angle, false);
	std::string file_name = extract_name(*path_);
	unsigned long long id;
	
	ThreadPool::sQueue->getId(&id);

	std::string tmp = delete_last_slash(ThreadPool::_out_path);

	foldingRule->saveHistogram( tmp, 
		file_name + "(" + std::to_string(id) + ")", angle );

	formatted_inf("End working for: %s", (*path_).c_str());

	path_ = NULL;
	delete foldingRule;
}

void ThreadPool::cartographer_worker(void *path, void *not_use) 
{
	Entity *e;
	
	std::string *path_ = (std::string *)path;

	formatted_log("Start cartographer_worker() for %s", (*path_).c_str());

	for (size_t i = 0; i < entities->size(); i++) {
		if ((e = ThreadPool::entities->getItem(i).isThisEntityE(*path_)) != NULL ) 
		{
			break;
		}
	}

	if (e == NULL) 
	{
		formatted_err("LOST ENTITY! %s", (*path_).c_str());
		return;
	}

	Rectification rec = e->getRect();
	cv::Mat image = cv::imread((*path_).c_str(), CV_LOAD_IMAGE_COLOR);

	if (image.empty())
	{
		formatted_err("Could not open or find the image \"%s\"", (*path_).c_str());

		return;
	}

	Cartographer *cartographer = new Cartographer();
	cartographer->setSrcImg(image);
	cartographer->makeBorder(false);

	std::vector<cv::Point> contour = cartographer->getContour();

	// tmp solution
	for (size_t i = 0; i < contour.size(); i++)
	{
		Point2f p = contour[i];
		rec.straightenPoint(p);
		contour[i] = p;
	}

	formatted_log("CONTURY: %d, first: %d:%d", contour.size(), contour[0].x, contour[0].y);

	// TODO poprawic kiedys
	Entity *e2 = new Entity();

	e2->setId(std::move(e->getId()));
	e2->setName(std::move(e->getName()));
	e2->setContour(std::move(contour));
	e2->setHierarchy(std::move(e->getHierarchy()));
	e2->setPointZero(std::move(e->getPointZero()));
	e2->setRec(std::move(e->getRect()));

	for (size_t i = 0; i < ThreadPool::entities->size(); i++) {
		if ((ThreadPool::entities->getItem(i).isThisEntity(*path_))) 
		{
			ThreadPool::entities->update(i, *e2);
			break;
		}
	}

	ThreadPool::sQueue->push( { ThreadPool::folding_rule_worker, path_, NULL } );

	path_ = NULL;
	delete cartographer;
}

// TODO dodaj opis co tu się dzieje
void ThreadPool::symmetry_worker(void *path, void *not_use)
{
	cv::Mat tmp;
	std::string *path_ = (std::string *)path;
	
	formatted_log("Start symmetry_worker() for %s", (*path_).c_str());

	cv::Mat image = cv::imread((*path_).c_str(), CV_LOAD_IMAGE_COLOR);

	if (image.empty())
	{
		formatted_err("Could not open or find the image \"%s\"", (*path_).c_str());

		return;
	}

	float rho_divs   = hypotf( image.rows, image.cols ) + 1;
	float theta_divs = 180.0;

	SymmetryDetector detector( image.size(), Size(rho_divs, theta_divs), 1 );
	pair<cv::Point, cv::Point> symmetry = detector.getResult(image);

	straight_t sym = createStraightFrom2Point(symmetry.first, symmetry.second);

	StartingPoint sp(std::move(image), sym);
	cv::Point2f starting = sp.getStartingPoint(0.5);

	Rectification rec(std::move(image), sym);

	rec.straightenPoint(starting);
	tmp = rec.straightenImg();

	Entity *e = new Entity( (*path_), std::move(rec), std::move(starting) );

	ThreadPool::entities->push(*e);
	ThreadPool::sQueue->push( { ThreadPool::cartographer_worker, path_, NULL } );
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