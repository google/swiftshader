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
#include <vulkan/vk_icd.h>

namespace sw
{
	class Context;
	class Renderer;
}

namespace vk
{

class Queue
{
	VK_LOADER_DATA loaderData = { ICD_LOADER_MAGIC };

public:
	Queue(uint32_t pFamilyIndex, float pPriority);
	~Queue() = delete;

	operator VkQueue()
	{
		return reinterpret_cast<VkQueue>(this);
	}

	void destroy();
	void submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence);

private:
	sw::Context* context = nullptr;
	sw::Renderer* renderer = nullptr;
	uint32_t familyIndex = 0;
	float    priority = 0.0f;
};

static inline Queue* Cast(VkQueue object)
{
	return reinterpret_cast<Queue*>(object);
}

} // namespace vk

#endif // VK_QUEUE_HPP_
