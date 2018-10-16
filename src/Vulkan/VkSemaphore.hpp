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

#ifndef VK_SEMAPHORE_HPP_
#define VK_SEMAPHORE_HPP_

#include "VkObject.hpp"

namespace vk
{

class Semaphore : public Object<Semaphore, VkSemaphore>
{
public:
	Semaphore(const VkSemaphoreCreateInfo* pCreateInfo, void* mem) {}

	~Semaphore() = delete;

	static size_t ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void wait()
	{
		// Semaphores are noop for now
	}

	void wait(const VkPipelineStageFlags& flag)
	{
		// VkPipelineStageFlags is the pipeline stage at which the semaphore wait will occur

		// Semaphores are noop for now
	}

	void signal()
	{
		// Semaphores are noop for now
	}

private:
};

static inline Semaphore* Cast(VkSemaphore object)
{
	return reinterpret_cast<Semaphore*>(object);
}

} // namespace vk

#endif // VK_SEMAPHORE_HPP_
