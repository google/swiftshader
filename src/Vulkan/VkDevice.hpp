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

namespace sw
{
	class Blitter;
}

namespace vk
{

class PhysicalDevice;
class Queue;

class Device
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_DEVICE; }

	Device(const VkDeviceCreateInfo* pCreateInfo, void* mem, PhysicalDevice *physicalDevice, const VkPhysicalDeviceFeatures *enabledFeatures);
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkDeviceCreateInfo* pCreateInfo);

	bool hasExtension(const char* extensionName) const;
	VkQueue getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
	VkResult waitForFences(uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout);
	VkResult waitIdle();
	void getDescriptorSetLayoutSupport(const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
	                                   VkDescriptorSetLayoutSupport* pSupport) const;
	PhysicalDevice *getPhysicalDevice() const { return physicalDevice; }
	void updateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites,
	                          uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies);
	const VkPhysicalDeviceFeatures &getEnabledFeatures() const { return enabledFeatures; }
	sw::Blitter* getBlitter() const { return blitter; }

private:
	PhysicalDevice *const physicalDevice = nullptr;
	Queue *const queues = nullptr;
	uint32_t queueCount = 0;

	const uint32_t enabledExtensionCount = 0;
	typedef char ExtensionName[VK_MAX_EXTENSION_NAME_SIZE];
	ExtensionName* extensions = nullptr;
	const VkPhysicalDeviceFeatures enabledFeatures = {};

	sw::Blitter* blitter = nullptr;
};

using DispatchableDevice = DispatchableObject<Device, VkDevice>;

static inline Device* Cast(VkDevice object)
{
	return DispatchableDevice::Cast(object);
}

} // namespace vk

#endif // VK_DEVICE_HPP_
