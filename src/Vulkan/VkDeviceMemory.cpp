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

#include "VkDeviceMemory.hpp"

#include "VkConfig.h"

namespace vk
{

DeviceMemory::DeviceMemory(const VkMemoryAllocateInfo* pCreateInfo, void* mem) :
	size(pCreateInfo->allocationSize), memoryTypeIndex(pCreateInfo->memoryTypeIndex)
{
	ASSERT(size);
}

void DeviceMemory::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::deallocate(buffer, DEVICE_MEMORY);
}

size_t DeviceMemory::ComputeRequiredAllocationSize(const VkMemoryAllocateInfo* pCreateInfo)
{
	// buffer is "GPU memory", so we use device memory for it
	return 0;
}

VkResult DeviceMemory::allocate()
{
	if(!buffer)
	{
		buffer = vk::allocate(size, REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY);
	}

	if(!buffer)
	{
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	return VK_SUCCESS;
}

VkResult DeviceMemory::map(VkDeviceSize pOffset, VkDeviceSize pSize, void** ppData)
{
	*ppData = getOffsetPointer(pOffset);

	return VK_SUCCESS;
}

VkDeviceSize DeviceMemory::getCommittedMemoryInBytes() const
{
	return size;
}

void* DeviceMemory::getOffsetPointer(VkDeviceSize pOffset) const
{
	ASSERT(buffer);

	return reinterpret_cast<char*>(buffer) + pOffset;
}

} // namespace vk
