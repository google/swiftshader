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

#ifndef sw_Synchronization_hpp
#define sw_Synchronization_hpp

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace sw
{

// Event is a synchronization mechanism used to indicate to waiting threads
// when a boolean condition has become true.
class Event
{
public:
	enum class ClearMode
	{
		// The event signal will be automatically reset when a call to wait()
		// returns.
		// A single call to signal() will only unblock a single (possibly
		// pending) call to wait().
		Auto,

		// The event will remain in the signaled state when calling signal().
		// While the event is in the signaled state, any calls to wait() will
		// unblock without automatically reseting the signaled state.
		// The signaled state can be reset with a call to clear().
		Manual
	};


	Event(ClearMode mode = ClearMode::Auto, bool initialState = false)
			: mode(mode), signaled(initialState) {}

	// signal() signals the event, unblocking any calls to wait().
	void signal()
	{
		std::unique_lock<std::mutex> lock(mutex);
		signaled = true;
		if (mode == ClearMode::Auto)
		{
			condition.notify_one();
		}
		else
		{
			condition.notify_all();
		}
	}

	// clear() sets the event signal to false.
	void clear()
	{
		std::unique_lock<std::mutex> lock(mutex);
		signaled = false;
	}

	// wait() blocks until all the event signal is set to true.
	// If the event was constructed with the Auto ClearMode, then the signal
	// is cleared before returning.
	void wait()
	{
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this] { return signaled; });
		if (mode == ClearMode::Auto)
		{
			signaled = false;
		}
	}

	// wait() blocks until the event signal is set to true or the timeout
	// has been reached, returning true if signal was raised, or false if the
	// timeout has been reached.
	// If the event was constructed with the Auto ClearMode and the wait did
	// not time out, then the signal is cleared before returning.
	template <class CLOCK, class DURATION>
	bool wait(const std::chrono::time_point<CLOCK, DURATION>& timeout)
	{
		std::unique_lock<std::mutex> lock(mutex);
		if (!condition.wait_until(lock, timeout, [this] { return signaled; }))
		{
			return false;
		}
		if (mode == ClearMode::Auto)
		{
			signaled = false;
		}
		return true;
	}

	// bool() returns true if the event is signaled, otherwise false.
	// Note: No lock is held after bool() returns, so the event state may
	// immediately change after returning.
	operator bool()
	{
		std::unique_lock<std::mutex> lock(mutex);
		return signaled;
	}

public:
	const ClearMode mode;
	bool signaled; // guarded by mutex
	std::mutex mutex;
	std::condition_variable condition;
};

// Chan is a thread-safe FIFO queue of type T.
// Chan takes its name after Golang's chan.
template <typename T>
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

template <typename T>
Chan<T>::Chan() {}

template <typename T>
T Chan<T>::take()
{
	std::unique_lock<std::mutex> lock(mutex);
	// Wait for item to be added.
	added.wait(lock, [this] { return queue.size() > 0; });
	T out = queue.front();
	queue.pop();
	return out;
}

template <typename T>
std::pair<T, bool> Chan<T>::tryTake()
{
	std::unique_lock<std::mutex> lock(mutex);
	if (queue.size() == 0)
	{
		return std::make_pair(T{}, false);
	}
	T out = queue.front();
	queue.pop();
	return std::make_pair(out, true);
}

template <typename T>
void Chan<T>::put(const T &item)
{
	std::unique_lock<std::mutex> lock(mutex);
	queue.push(item);
	lock.unlock();
	added.notify_one();
}

template <typename T>
size_t Chan<T>::count()
{
	std::unique_lock<std::mutex> lock(mutex);
	return queue.size();
}

} // namespace sw

#endif // sw_Synchronization_hpp
