#ifndef _T_SAFE_QUEUE_HPP
#define _T_SAFE_QUEUE_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class TSafeQueue
{
private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;

public:
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
		queue_.pop();
		
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
		queue_.pop();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	void push(T&& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(std::move(item));
		mlock.unlock();
		cond_.notify_one();
	}

	bool empty()
	{
		return queue_.empty();
	}

	int size()
	{
		return queue_.size();
	}
};

#endif
