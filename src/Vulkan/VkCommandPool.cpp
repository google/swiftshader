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

#include "VkCommandPool.hpp"
#include "VkCommandBuffer.hpp"
#include "VkDestroy.h"
#include <algorithm>
#include <new>

namespace vk {

CommandPool::CommandPool(const VkCommandPoolCreateInfo *pCreateInfo, void *mem)
{
	// FIXME (b/119409619): use an allocator here so we can control all memory allocations
	void *deviceMemory = vk::allocate(sizeof(std::set<VkCommandBuffer>), REQUIRED_MEMORY_ALIGNMENT,
	                                  DEVICE_MEMORY, GetAllocationScope());
	ASSERT(deviceMemory);
	commandBuffers = new(deviceMemory) std::set<VkCommandBuffer>();
}

void CommandPool::destroy(const VkAllocationCallbacks *pAllocator)
{
	// Free command Buffers allocated in allocateCommandBuffers
	for(auto commandBuffer : *commandBuffers)
	{
		vk::destroy(commandBuffer, DEVICE_MEMORY);
	}

	// FIXME (b/119409619): use an allocator here so we can control all memory allocations
	vk::deallocate(commandBuffers, DEVICE_MEMORY);
}

size_t CommandPool::ComputeRequiredAllocationSize(const VkCommandPoolCreateInfo *pCreateInfo)
{
	return 0;
}

VkResult CommandPool::allocateCommandBuffers(Device *device, VkCommandBufferLevel level, uint32_t commandBufferCount, VkCommandBuffer *pCommandBuffers)
{
	for(uint32_t i = 0; i < commandBufferCount; i++)
	{
		// FIXME (b/119409619): use an allocator here so we can control all memory allocations
		void *deviceMemory = vk::allocate(sizeof(DispatchableCommandBuffer), REQUIRED_MEMORY_ALIGNMENT,
		                                  DEVICE_MEMORY, DispatchableCommandBuffer::GetAllocationScope());
		ASSERT(deviceMemory);
		DispatchableCommandBuffer *commandBuffer = new(deviceMemory) DispatchableCommandBuffer(device, level);
		if(commandBuffer)
		{
			pCommandBuffers[i] = *commandBuffer;
		}
		else
		{
			for(uint32_t j = 0; j < i; j++)
			{
				vk::destroy(pCommandBuffers[j], DEVICE_MEMORY);
			}
			for(uint32_t j = 0; j < commandBufferCount; j++)
			{
				pCommandBuffers[j] = VK_NULL_HANDLE;
			}
			return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}
	}

	commandBuffers->insert(pCommandBuffers, pCommandBuffers + commandBufferCount);

	return VK_SUCCESS;
}

void CommandPool::freeCommandBuffers(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
	for(uint32_t i = 0; i < commandBufferCount; ++i)
	{
		commandBuffers->erase(pCommandBuffers[i]);
		vk::destroy(pCommandBuffers[i], DEVICE_MEMORY);
	}
}

VkResult CommandPool::reset(VkCommandPoolResetFlags flags)
{
	// According the Vulkan 1.1 spec:
	// "All command buffers that have been allocated from
	//  the command pool are put in the initial state."
	for(auto commandBuffer : *commandBuffers)
	{
		vk::Cast(commandBuffer)->reset(flags);
	}

	// According the Vulkan 1.1 spec:
	// "Resetting a command pool recycles all of the
	//  resources from all of the command buffers allocated
	//  from the command pool back to the command pool."
	commandBuffers->clear();

	return VK_SUCCESS;
}

void CommandPool::trim(VkCommandPoolTrimFlags flags)
{
	// TODO (b/119827933): Optimize memory usage here
}

}  // namespace vk
