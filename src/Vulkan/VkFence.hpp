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
#include "System/Synchronization.hpp"

#include "marl/containers.h"
#include "marl/event.h"
#include "marl/waitgroup.h"

namespace vk {

class Fence : public Object<Fence, VkFence>, public sw::TaskEvents
{
public:
	Fence(const VkFenceCreateInfo *pCreateInfo, void *mem)
	    : event(marl::Event::Mode::Manual, (pCreateInfo->flags & VK_FENCE_CREATE_SIGNALED_BIT) != 0)
	{}

	static size_t ComputeRequiredAllocationSize(const VkFenceCreateInfo *pCreateInfo)
	{
		return 0;
	}

	void reset()
	{
		event.clear();
	}

	VkResult getStatus()
	{
		return event.isSignalled() ? VK_SUCCESS : VK_NOT_READY;
	}

	VkResult wait()
	{
		event.wait();
		return VK_SUCCESS;
	}

	template<class CLOCK, class DURATION>
	VkResult wait(const std::chrono::time_point<CLOCK, DURATION> &timeout)
	{
		return event.wait_until(timeout) ? VK_SUCCESS : VK_TIMEOUT;
	}

	const marl::Event &getEvent() const { return event; }

	// TaskEvents compliance
	void start() override
	{
		ASSERT(!event.isSignalled());
		wg.add();
	}

	void finish() override
	{
		ASSERT(!event.isSignalled());
		if(wg.done())
		{
			event.signal();
		}
	}

private:
	Fence(const Fence &) = delete;

	marl::WaitGroup wg;
	const marl::Event event;
};

static inline Fence *Cast(VkFence object)
{
	return Fence::Cast(object);
}

}  // namespace vk

#endif  // VK_FENCE_HPP_
