#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <thread>

#include "TSafeQueue.hpp"
#include "store_queue.hpp"
#include "opencv_headers.hpp"
#include "cartographer.hpp"
#include "folding_rule.hpp"
#include "Entity.hpp"
#include "formatted_log.hpp"
#include "rectification.hpp"
#include "symmetry_detector.hpp"
#include "straight.hpp"
#include "starting_point.hpp"
#include "dir_functions.hpp"

#define NUM_THREADS std::thread::hardware_concurrency()

class ThreadPool
{
private:
	static TSafeQueue<store_queue> *sQueue;
	// NOT USED, SAVE FOR FUTURE
	// static TSafeQueue<Entity>      *entities;

	static std::string _out_path;

	static bool stop_flag;
	static bool	*stop_flags;
	
	static void worker              ( int   id                     ) ;
	// NOT USED, SAVE FOR FUTURE
	// static void folding_rule_worker ( void *contour, void *size    ) ;
	// static void cartographer_worker ( void *path   , void *not_use ) ;
	// static void symmetry_worker     ( void *path   , void *not_use ) ;
	static void all_in_one          ( void *path   , void *not_use ) ;

	static void	set_starts_value	();

public:
	static void start();
	static void stop ();

	// getters/setters
	static TSafeQueue<store_queue> *getSQueue() { return ThreadPool::sQueue;    };
	static std::string             getOutPath() { return ThreadPool::_out_path; };

	static void setOutPath ( std::string out_path ) { ThreadPool::_out_path = out_path; };

	ThreadPool()  { ThreadPool::stop_flag = false; ThreadPool::sQueue = new TSafeQueue<store_queue>();
	                stop_flags = new bool[NUM_THREADS];
	                ThreadPool::set_starts_value(); ThreadPool::_out_path = "../out"; }; //ThreadPool::entities = new TSafeQueue<Entity>(); 
	~ThreadPool() {};
	
};

#endif
