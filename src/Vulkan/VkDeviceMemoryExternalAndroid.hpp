// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_DEVICE_MEMORY_EXTERNAL_ANDROID_HPP_
#define VK_DEVICE_MEMORY_EXTERNAL_ANDROID_HPP_

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER

#	include "VkDevice.hpp"
#	include "VkDeviceMemory.hpp"
#	include "VkDeviceMemoryExternalBase.hpp"

#	include "VkBuffer.hpp"
#	include "VkDeviceMemory.hpp"
#	include "VkImage.hpp"
#	include "VkStringify.hpp"

#	include "System/Debug.hpp"
#	include "System/Linux/MemFd.hpp"

#	include <android/hardware_buffer.h>

#	include <cros_gralloc/cros_gralloc_handle.h>
#	include <unistd.h>
#	include <virtgpu_drm.h>
#	include <xf86drm.h>

#	include <errno.h>
#	include <string.h>
#	include <map>

class AHardwareBufferExternalMemory : public vk::DeviceMemory::ExternalBase
{
public:
	// Helper struct to parse the VkMemoryAllocateInfo.pNext chain and
	// extract relevant information related to the handle type supported
	// by this DeviceMemory::ExternalBase subclass.
	struct AllocateInfo
	{
		bool importAhb = false;
		bool exportAhb = false;
		struct AHardwareBuffer *ahb = nullptr;
		vk::Image *imageHandle = nullptr;
		vk::Buffer *bufferHandle = nullptr;

		AllocateInfo() = default;

		// Parse the VkMemoryAllocateInfo.pNext chain to initialize an AllocateInfo.
		AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo);
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;

	static bool SupportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		AllocateInfo info(pAllocateInfo);
		return (info.importAhb || info.exportAhb) && (info.bufferHandle || info.imageHandle);
	}

	explicit AHardwareBufferExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo)
	    : allocateInfo(pAllocateInfo)
	{
	}

	~AHardwareBufferExternalMemory();
	VkResult allocate(size_t size, void **pBuffer) override;
	void deallocate(void *buffer, size_t size) override;
	VkResult allocateAndroidHardwareBuffer(size_t size, void **pBuffer);

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

	VkResult exportAndroidHardwareBuffer(struct AHardwareBuffer **pAhb) const override;

	void setDevicePtr(vk::Device *pDevice) override
	{
		device = pDevice;
	}

	bool isAndroidHardwareBuffer() override
	{
		return true;
	}

	static uint32_t GetAndroidHardwareBufferDescFormat(VkFormat format);
	static VkFormat GetVkFormat(uint32_t ahbFormat);
	static VkFormatFeatureFlags GetVkFormatFeatures(VkFormat format);
	static VkResult GetAndroidHardwareBufferFormatProperties(const AHardwareBuffer_Desc &ahbDesc, VkAndroidHardwareBufferFormatPropertiesANDROID *pFormat);
	static VkResult GetAndroidHardwareBufferProperties(VkDevice &device, const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties);
	static VkResult GetAndroidHardwareBufferUsageFromVkUsage(const VkImageCreateFlags createFlags, const VkImageUsageFlags usageFlags, uint64_t &ahbDescUsage);

	// All reliance on minigbm and DRM is contained in these functions
	VkResult getPrimeHandle(const native_handle_t *h, uint32_t &primeHandle);
	void createRenderNodeFD();
	VkResult mapMemory(uint32_t &primeHandle, void **ptr);
	VkResult closeMemory(uint32_t &primeHandle);

private:
	struct AHardwareBuffer *ahb = nullptr;
	int rendernodeFD;
	vk::Device *device = nullptr;
	AllocateInfo allocateInfo;
};

#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
#endif  // VK_DEVICE_MEMORY_EXTERNAL_ANDROID_HPP_
