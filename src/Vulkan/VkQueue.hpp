// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_QUEUE_HPP_
#define VK_QUEUE_HPP_

#include "VkObject.hpp"
#include "Device/Renderer.hpp"
#include <queue>
#include <thread>
#include <vulkan/vk_icd.h>

namespace sw
{
	class Context;
	class Renderer;
}

namespace vk
{

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
	std::condition_variable removed;
};

template <typename T>
Chan<T>::Chan() {}

template <typename T>
T Chan<T>::take()
{
	std::unique_lock<std::mutex> lock(mutex);
	if (queue.size() == 0)
	{
		// Chan empty. Wait for item to be added.
		added.wait(lock, [this] { return queue.size() > 0; });
	}
	T out = queue.front();
	queue.pop();
	lock.unlock();
	removed.notify_one();
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
	lock.unlock();
	removed.notify_one();
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
	auto out = queue.size();
	lock.unlock();
	return out;
}

class Queue
{
	VK_LOADER_DATA loaderData = { ICD_LOADER_MAGIC };

public:
	Queue();
	~Queue() = delete;

	operator VkQueue()
	{
		return reinterpret_cast<VkQueue>(this);
	}

	void destroy();
	VkResult submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence);
	VkResult waitIdle();
#ifndef __ANDROID__
	void present(const VkPresentInfoKHR* presentInfo);
#endif

private:
	struct Task
	{
		uint32_t submitCount = 0;
		VkSubmitInfo* pSubmits = nullptr;
		Fence* fence = nullptr;

		enum Type { KILL_THREAD, SUBMIT_QUEUE };
		Type type = SUBMIT_QUEUE;
	};

	static void TaskLoop(vk::Queue* queue);
	void taskLoop();
	void garbageCollect();
	void submitQueue(const Task& task);

	std::unique_ptr<sw::Renderer> renderer;
	Chan<Task> pending;
	Chan<VkSubmitInfo*> toDelete;
	std::thread queueThread;
};

static inline Queue* Cast(VkQueue object)
{
	return reinterpret_cast<Queue*>(object);
}

} // namespace vk

#endif // VK_QUEUE_HPP_
