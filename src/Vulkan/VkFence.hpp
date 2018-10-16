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

namespace vk
{

class Fence : public Object<Fence, VkFence>
{
public:
	Fence(const VkFenceCreateInfo* pCreateInfo, void* mem) :
		status((pCreateInfo->flags & VK_FENCE_CREATE_SIGNALED_BIT) ? VK_SUCCESS : VK_NOT_READY)
	{
	}

	~Fence() = delete;

	static size_t ComputeRequiredAllocationSize(const VkFenceCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void signal()
	{
		status = VK_SUCCESS;
	}

	void reset()
	{
		status = VK_NOT_READY;
	}

	VkResult getStatus() const
	{
		return status;
	}

private:
	VkResult status = VK_NOT_READY;
};

static inline Fence* Cast(VkFence object)
{
	return reinterpret_cast<Fence*>(object);
}

} // namespace vk

#endif // VK_FENCE_HPP_
