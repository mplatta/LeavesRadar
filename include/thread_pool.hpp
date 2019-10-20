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

#define NUM_THREADS std::thread::hardware_concurrency()

class ThreadPool
{
private:
	static TSafeQueue<store_queue> *sQueue;
	static TSafeQueue<Entity>      *entities;

	static bool stop_flag;
	static bool	*stop_flags;
	
	static void worker              ( int   id                                       ) ;
	static void folding_rule_worker ( void *contour, void *size   , void *point_zero ) ;
	static void cartographer_worker ( void *path   , void *not_use, void *not_use_2  ) ;
	static void	set_starts_value	();

public:
	static void start();
	static void stop ();

	// getters/setters
	static TSafeQueue<store_queue> *getSQueue() { return ThreadPool::sQueue; };

	ThreadPool()  { ThreadPool::stop_flag = false; ThreadPool::sQueue = new TSafeQueue<store_queue>();
	                ThreadPool::entities = new TSafeQueue<Entity>(); stop_flags = new bool[NUM_THREADS];
	                ThreadPool::set_starts_value(); };
	~ThreadPool() {};
	
};

#endif
