#ifndef _T_SAFE_QUEUE_HPP
#define _T_SAFE_QUEUE_HPP

// #include <queue>
// #include <list>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class TSafeQueue
{
private:
	std::deque<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
	unsigned long long id_;

public:
	// not safe
	std::deque<T> getQueue() 
	{
		return queue_;
	}
	// --------


	T getItem(int i) 
	{
		std::unique_lock<std::mutex> mlock(mutex_);

		int flag = 0;

		while (queue_.empty())
		{
			cond_.wait(mlock);
			flag++;

			if (flag >= 4) break;
		}	

		return queue_[i];
	}

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);

		int flag = 0;

		while (queue_.empty())
		{
			cond_.wait(mlock);
			flag++;

			if (flag >= 4) break;
		}

		auto item = queue_.front();
		queue_.pop_front();
		
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);

		int flag = 0;

		while (queue_.empty())
	 	{
			cond_.wait(mlock);
			flag++;

			if (flag >= 4) break;
		}

		item = queue_.front();
		queue_.pop_front();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push_back(item);
		mlock.unlock();
		cond_.notify_one();
	}

	void push(T&& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push_back(std::move(item));
		mlock.unlock();
		cond_.notify_one();
	}

	bool empty()
	{
		return queue_.empty();
	}

	size_t size()
	{
		return queue_.size();
	}

	void getId(unsigned long long *id_to_save) {
		std::unique_lock<std::mutex> mlock(mutex_);
		
		id_++;
		*id_to_save = id_;
		
		mlock.unlock();
		cond_.notify_one();
	}

	TSafeQueue() {
		id_ = 0;
	}

	// void update(int item)
	// {
	// 	std::unique_lock<std::mutex> mlock(mutex_);



	// 	mlock.unlock();
	// 	cond_.notify_one();
	// }
};

#endif
