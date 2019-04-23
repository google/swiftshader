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

#ifndef VK_FENCE_HPP_
#define VK_FENCE_HPP_

#include "VkObject.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace vk
{

using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

class Fence : public Object<Fence, VkFence>
{
public:
	Fence() : status(VK_NOT_READY) {}

	Fence(const VkFenceCreateInfo* pCreateInfo, void* mem) :
		status((pCreateInfo->flags & VK_FENCE_CREATE_SIGNALED_BIT) ? VK_SUCCESS : VK_NOT_READY)
	{
	}

	static size_t ComputeRequiredAllocationSize(const VkFenceCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void add()
	{
		std::unique_lock<std::mutex> lock(mutex);
		++count;
	}

	void done()
	{
		std::unique_lock<std::mutex> lock(mutex);
		ASSERT(count > 0);
		--count;
		if(count == 0)
		{
			// signal the fence, without the unlock/lock required to call signal() here
			status = VK_SUCCESS;
			lock.unlock();
			condition.notify_all();
		}
	}

	void signal()
	{
		std::unique_lock<std::mutex> lock(mutex);
		status = VK_SUCCESS;
		lock.unlock();
		condition.notify_all();
	}

	void reset()
	{
		std::unique_lock<std::mutex> lock(mutex);
		ASSERT(count == 0);
		status = VK_NOT_READY;
	}

	VkResult getStatus()
	{
		std::unique_lock<std::mutex> lock(mutex);
		auto out = status;
		lock.unlock();
		return out;
	}

	VkResult wait()
	{
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this] { return status == VK_SUCCESS; });
		auto out = status;
		lock.unlock();
		return out;
	}

	VkResult waitUntil(const time_point& timeout_ns)
	{
		std::unique_lock<std::mutex> lock(mutex);
		return condition.wait_until(lock, timeout_ns, [this] { return status == VK_SUCCESS; }) ?
		       VK_SUCCESS : VK_TIMEOUT;
	}

private:
	VkResult status = VK_NOT_READY; // guarded by mutex
	int32_t count = 0; // guarded by mutex
	std::mutex mutex;
	std::condition_variable condition;
};

static inline Fence* Cast(VkFence object)
{
	return reinterpret_cast<Fence*>(object);
}

} // namespace vk

#endif // VK_FENCE_HPP_
