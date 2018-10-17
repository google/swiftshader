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

#include "VkInstance.hpp"
#include "VkDestroy.h"

namespace vk
{

Instance::Instance(const CreateInfo* pCreateInfo, void* mem)
{
	if(pCreateInfo->pPhysicalDevice)
	{
		physicalDevice = pCreateInfo->pPhysicalDevice;
		physicalDeviceCount = 1;
	}
}

void Instance::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::destroy(physicalDevice, pAllocator);
}

uint32_t Instance::getPhysicalDeviceCount() const
{
	return physicalDeviceCount;
}

void Instance::getPhysicalDevices(uint32_t pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) const
{
	ASSERT(pPhysicalDeviceCount == 1);

	*pPhysicalDevices = physicalDevice;
}

uint32_t Instance::getPhysicalDeviceGroupCount() const
{
	return physicalDeviceGroupCount;
}

void Instance::getPhysicalDeviceGroups(uint32_t pPhysicalDeviceGroupCount,
                                       VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) const
{
	ASSERT(pPhysicalDeviceGroupCount == 1);

	pPhysicalDeviceGroupProperties->physicalDeviceCount = physicalDeviceCount;
	pPhysicalDeviceGroupProperties->physicalDevices[0] = physicalDevice;
	pPhysicalDeviceGroupProperties->subsetAllocation = VK_FALSE;
}

} // namespace vk
