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

#ifndef VK_PHYSICAL_DEVICE_HPP_
#define VK_PHYSICAL_DEVICE_HPP_

#include "VkFormat.h"
#include "VkObject.hpp"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#	include <vulkan/vk_android_native_buffer.h>
#endif

namespace vk {

class PhysicalDevice
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE; }

	PhysicalDevice(const void *, void *mem);
	void destroy(const VkAllocationCallbacks *pAllocator) {}

	static size_t ComputeRequiredAllocationSize(const void *) { return 0; }

	const VkPhysicalDeviceFeatures &getFeatures() const;
	void getFeatures(VkPhysicalDeviceSamplerYcbcrConversionFeatures *features) const;
	void getFeatures(VkPhysicalDevice16BitStorageFeatures *features) const;
	void getFeatures(VkPhysicalDeviceVariablePointerFeatures *features) const;
	void getFeatures(VkPhysicalDevice8BitStorageFeaturesKHR *features) const;
	void getFeatures(VkPhysicalDeviceMultiviewFeatures *features) const;
	void getFeatures(VkPhysicalDeviceProtectedMemoryFeatures *features) const;
	void getFeatures(VkPhysicalDeviceShaderDrawParameterFeatures *features) const;
	void getFeatures(VkPhysicalDeviceLineRasterizationFeaturesEXT *features) const;
	void getFeatures(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *features) const;
	void getFeatures(VkPhysicalDeviceProvokingVertexFeaturesEXT *features) const;
	bool hasFeatures(const VkPhysicalDeviceFeatures &requestedFeatures) const;

	const VkPhysicalDeviceProperties &getProperties() const;
	void getProperties(VkPhysicalDeviceIDProperties *properties) const;
	void getProperties(VkPhysicalDeviceMaintenance3Properties *properties) const;
	void getProperties(VkPhysicalDeviceMultiviewProperties *properties) const;
	void getProperties(VkPhysicalDevicePointClippingProperties *properties) const;
	void getProperties(VkPhysicalDeviceProtectedMemoryProperties *properties) const;
	void getProperties(VkPhysicalDeviceSubgroupProperties *properties) const;
	void getProperties(const VkExternalMemoryHandleTypeFlagBits *handleType, VkExternalImageFormatProperties *properties) const;
	void getProperties(VkSamplerYcbcrConversionImageFormatProperties *properties) const;
#ifdef __ANDROID__
	void getProperties(VkPhysicalDevicePresentationPropertiesANDROID *properties) const;
#endif
	void getProperties(const VkPhysicalDeviceExternalBufferInfo *pExternalBufferInfo, VkExternalBufferProperties *pExternalBufferProperties) const;
	void getProperties(const VkPhysicalDeviceExternalFenceInfo *pExternalFenceInfo, VkExternalFenceProperties *pExternalFenceProperties) const;
	void getProperties(const VkPhysicalDeviceExternalSemaphoreInfo *pExternalSemaphoreInfo, VkExternalSemaphoreProperties *pExternalSemaphoreProperties) const;
	void getProperties(VkPhysicalDeviceExternalMemoryHostPropertiesEXT *properties) const;
	void getProperties(VkPhysicalDeviceDriverPropertiesKHR *properties) const;
	void getProperties(VkPhysicalDeviceLineRasterizationPropertiesEXT *properties) const;
	void getProperties(VkPhysicalDeviceProvokingVertexPropertiesEXT *properties) const;

	void getFormatProperties(Format format, VkFormatProperties *pFormatProperties) const;
	void getImageFormatProperties(Format format, VkImageType type, VkImageTiling tiling,
	                              VkImageUsageFlags usage, VkImageCreateFlags flags,
	                              VkImageFormatProperties *pImageFormatProperties) const;
	uint32_t getQueueFamilyPropertyCount() const;
	void getQueueFamilyProperties(uint32_t pQueueFamilyPropertyCount,
	                              VkQueueFamilyProperties *pQueueFamilyProperties) const;
	void getQueueFamilyProperties(uint32_t pQueueFamilyPropertyCount,
	                              VkQueueFamilyProperties2 *pQueueFamilyProperties) const;
	const VkPhysicalDeviceMemoryProperties &getMemoryProperties() const;

private:
	const VkPhysicalDeviceLimits &getLimits() const;
	VkSampleCountFlags getSampleCounts() const;
};

using DispatchablePhysicalDevice = DispatchableObject<PhysicalDevice, VkPhysicalDevice>;

static inline PhysicalDevice *Cast(VkPhysicalDevice object)
{
	return DispatchablePhysicalDevice::Cast(object);
}

}  // namespace vk

#endif  // VK_PHYSICAL_DEVICE_HPP_
