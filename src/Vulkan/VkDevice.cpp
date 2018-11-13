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

#include "VkConfig.h"
#include "VkDebug.hpp"
#include "VkDevice.hpp"
#include "VkQueue.hpp"
#include <new> // Must #include this to use "placement new"

namespace vk
{

Device::Device(const Device::CreateInfo* info, void* mem)
	: physicalDevice(info->pPhysicalDevice), queues(reinterpret_cast<Queue*>(mem))
{
	const auto* pCreateInfo = info->pCreateInfo;
	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo& queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];
		queueCount += queueCreateInfo.queueCount;
	}

	uint32_t queueID = 0;
	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo& queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];

		for(uint32_t j = 0; j < queueCreateInfo.queueCount; j++, queueID++)
		{
			new (queues + queueID) Queue(queueCreateInfo.queueFamilyIndex, queueCreateInfo.pQueuePriorities[j]);
		}
	}
}

void Device::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::deallocate(queues, pAllocator);
}

size_t Device::ComputeRequiredAllocationSize(const Device::CreateInfo* info)
{
	uint32_t queueCount = 0;
	for(uint32_t i = 0; i < info->pCreateInfo->queueCreateInfoCount; i++)
	{
		queueCount += info->pCreateInfo->pQueueCreateInfos[i].queueCount;
	}

	return sizeof(Queue) * queueCount;
}

VkQueue Device::getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
{
	ASSERT(queueFamilyIndex == 0);

	return queues[queueIndex];
}

void Device::getDescriptorSetLayoutSupport(const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                           VkDescriptorSetLayoutSupport* pSupport) const
{
	// Mark everything as unsupported
	pSupport->supported = VK_FALSE;
}

} // namespace vk
