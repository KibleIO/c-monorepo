//PLATFORMS: Linux, Windows, OSX

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <iostream>

template <typename T>
class Queue {
public:

	T pop() {
		T t;
		mutex_.lock();
		if (queue_.empty()) {
			mutex_.unlock();
			return t;
		}
		t = queue_.front();
		queue_.pop();
		mutex_.unlock();
		return t;
	}

	void pop(T& item) {
		mutex_.lock();
		if (queue_.empty()) {
			mutex_.unlock();
			return;
		}
		item = queue_.front();
		queue_.pop();
		mutex_.unlock();
	}

	void push(const T& item) {
		mutex_.lock();
		queue_.push(item);
		mutex_.unlock();
	}

	int size() {
		mutex_.lock();
		int n = queue_.size();
		mutex_.unlock();
		return n;
	}

	Queue()=default;
	Queue(const Queue&) = delete;            // disable copying
	Queue& operator=(const Queue&) = delete; // disable assignment

private:
	std::queue<T> queue_;
	std::mutex mutex_;
};

#endif
