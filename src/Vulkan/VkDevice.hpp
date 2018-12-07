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

#ifndef VK_DEVICE_HPP_
#define VK_DEVICE_HPP_

#include "VkObject.hpp"

namespace vk
{

class Queue;

class Device
{
public:
	struct CreateInfo
	{
		const VkDeviceCreateInfo* pCreateInfo;
		VkPhysicalDevice pPhysicalDevice;
	};

	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_DEVICE; }

	Device(const CreateInfo* info, void* mem);
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const CreateInfo* info);

	VkQueue getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
	void waitForFences(uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout);
	void getDescriptorSetLayoutSupport(const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
	                                   VkDescriptorSetLayoutSupport* pSupport) const;
	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }

private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	Queue* queues = nullptr;
	uint32_t queueCount = 0;
};

using DispatchableDevice = DispatchableObject<Device, VkDevice>;

static inline Device* Cast(VkDevice object)
{
	return DispatchableDevice::Cast(object);
}

} // namespace vk

#endif // VK_DEVICE_HPP_
