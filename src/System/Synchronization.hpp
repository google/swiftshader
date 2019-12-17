// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file contains a number of synchronization primitives for concurrency.
//
// You may be tempted to change this code to unlock the mutex before calling
// std::condition_variable::notify_[one,all]. Please read
// https://issuetracker.google.com/issues/133135427 before making this sort of
// change.

#ifndef sw_Synchronization_hpp
#define sw_Synchronization_hpp

#include <assert.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace sw {

// TaskEvents is an interface for notifying when tasks begin and end.
// Tasks can be nested and/or overlapping.
// TaskEvents is used for task queue synchronization.
class TaskEvents
{
public:
	// start() is called before a task begins.
	virtual void start() = 0;
	// finish() is called after a task ends. finish() must only be called after
	// a corresponding call to start().
	virtual void finish() = 0;
	// complete() is a helper for calling start() followed by finish().
	inline void complete()
	{
		start();
		finish();
	}

protected:
	virtual ~TaskEvents() = default;
};

// WaitGroup is a synchronization primitive that allows you to wait for
// collection of asynchronous tasks to finish executing.
// Call add() before each task begins, and then call done() when after each task
// is finished.
// At the same time, wait() can be used to block until all tasks have finished.
// WaitGroup takes its name after Golang's sync.WaitGroup.
class WaitGroup : public TaskEvents
{
public:
	// add() begins a new task.
	void add()
	{
		std::unique_lock<std::mutex> lock(mutex);
		++count_;
	}

	// done() is called when a task of the WaitGroup has been completed.
	// Returns true if there are no more tasks currently running in the
	// WaitGroup.
	bool done()
	{
		std::unique_lock<std::mutex> lock(mutex);
		assert(count_ > 0);
		--count_;
		if(count_ == 0)
		{
			condition.notify_all();
		}
		return count_ == 0;
	}

	// wait() blocks until all the tasks have been finished.
	void wait()
	{
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this] { return count_ == 0; });
	}

	// wait() blocks until all the tasks have been finished or the timeout
	// has been reached, returning true if all tasks have been completed, or
	// false if the timeout has been reached.
	template<class CLOCK, class DURATION>
	bool wait(const std::chrono::time_point<CLOCK, DURATION> &timeout)
	{
		std::unique_lock<std::mutex> lock(mutex);
		return condition.wait_until(lock, timeout, [this] { return count_ == 0; });
	}

	// count() returns the number of times add() has been called without a call
	// to done().
	// Note: No lock is held after count() returns, so the count may immediately
	// change after returning.
	int32_t count()
	{
		std::unique_lock<std::mutex> lock(mutex);
		return count_;
	}

	// TaskEvents compliance
	void start() override { add(); }
	void finish() override { done(); }

private:
	int32_t count_ = 0;  // guarded by mutex
	std::mutex mutex;
	std::condition_variable condition;
};

// Chan is a thread-safe FIFO queue of type T.
// Chan takes its name after Golang's chan.
template<typename T>
class Chan
{
public:
	Chan();

	// take returns the next item in the chan, blocking until an item is
	// available.
	T take();

	// tryTake returns a <T, bool> pair.
	// If the chan is not empty, then the next item and true are returned.
	// If the chan is empty, then a default-initialized T and false are returned.
	std::pair<T, bool> tryTake();

	// put places an item into the chan, blocking if the chan is bounded and
	// full.
	void put(const T &v);

	// Returns the number of items in the chan.
	// Note: that this may change as soon as the function returns, so should
	// only be used for debugging.
	size_t count();

private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable added;
};

template<typename T>
Chan<T>::Chan()
{}

template<typename T>
T Chan<T>::take()
{
	std::unique_lock<std::mutex> lock(mutex);
	// Wait for item to be added.
	added.wait(lock, [this] { return queue.size() > 0; });
	T out = queue.front();
	queue.pop();
	return out;
}

template<typename T>
std::pair<T, bool> Chan<T>::tryTake()
{
	std::unique_lock<std::mutex> lock(mutex);
	if(queue.size() == 0)
	{
		return std::make_pair(T{}, false);
	}
	T out = queue.front();
	queue.pop();
	return std::make_pair(out, true);
}

template<typename T>
void Chan<T>::put(const T &item)
{
	std::unique_lock<std::mutex> lock(mutex);
	queue.push(item);
	added.notify_one();
}

template<typename T>
size_t Chan<T>::count()
{
	std::unique_lock<std::mutex> lock(mutex);
	return queue.size();
}

}  // namespace sw

#endif  // sw_Synchronization_hpp
