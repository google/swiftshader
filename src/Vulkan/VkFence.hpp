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

namespace vk
{

class Fence : public Object<Fence, VkFence>, public sw::TaskEvents
{
public:
	Fence(const VkFenceCreateInfo* pCreateInfo, void* mem) :
		signaled(sw::Event::ClearMode::Manual, (pCreateInfo->flags & VK_FENCE_CREATE_SIGNALED_BIT) != 0) {}

	static size_t ComputeRequiredAllocationSize(const VkFenceCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void reset()
	{
		ASSERT_MSG(wg.count() == 0, "Fence::reset() called when work is in flight");
		signaled.clear();
	}

	VkResult getStatus()
	{
		return signaled ? VK_SUCCESS : VK_NOT_READY;
	}

	VkResult wait()
	{
		signaled.wait();
		return VK_SUCCESS;
	}

    template <class CLOCK, class DURATION>
	VkResult wait(const std::chrono::time_point<CLOCK, DURATION>& timeout)
	{
		return signaled.wait(timeout) ? VK_SUCCESS : VK_TIMEOUT;
	}

	// TaskEvents compliance
	void start() override
	{
		ASSERT(!signaled);
		wg.add();
	}

	void finish() override
	{
		ASSERT(!signaled);
		if (wg.done())
		{
			signaled.signal();
		}
	}

private:
	Fence(const Fence&) = delete;

	sw::WaitGroup wg;
	sw::Event signaled;
};

static inline Fence* Cast(VkFence object)
{
	return Fence::Cast(object);
}

} // namespace vk

#endif // VK_FENCE_HPP_
