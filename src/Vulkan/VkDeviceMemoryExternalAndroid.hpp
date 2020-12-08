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

#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "VkDeviceMemory.hpp"
#include "VkDeviceMemoryExternalBase.hpp"
#include "VkImage.hpp"

#include <vndk/hardware_buffer.h>

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
		AHardwareBuffer *ahb = nullptr;
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

	explicit AHardwareBufferExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo);
	~AHardwareBufferExternalMemory();

	VkResult allocate(size_t size, void **pBuffer) override;
	void deallocate(void *buffer, size_t size) override;

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override { return typeFlagBit; }

	VkResult exportAndroidHardwareBuffer(AHardwareBuffer **pAhb) const;

	void setDevicePtr(vk::Device *pDevice) override { device = pDevice; }

	static VkFormat GetVkFormatFromAHBFormat(uint32_t ahbFormat);
	static VkResult GetAndroidHardwareBufferFormatProperties(const AHardwareBuffer_Desc &ahbDesc, VkAndroidHardwareBufferFormatPropertiesANDROID *pFormat);
	static VkResult GetAndroidHardwareBufferProperties(VkDevice &device, const AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties);

	bool hasExternalImageProperties() const override final { return true; }
	int externalImageRowPitchBytes(VkImageAspectFlagBits aspect) const override final;
	VkDeviceSize externalImageMemoryOffset(VkImageAspectFlagBits aspect) const override final;

#ifdef SWIFTSHADER_DEVICE_MEMORY_REPORT
	bool isImport() const override
	{
		return allocateInfo.importAhb;
	}
	uint64_t getMemoryObjectId() const override;
#endif  // SWIFTSHADER_DEVICE_MEMORY_REPORT

private:
	VkResult importAndroidHardwareBuffer(AHardwareBuffer *buffer, void **pBuffer);
	VkResult allocateAndroidHardwareBuffer(void **pBuffer);
	VkResult lockAndroidHardwareBuffer(void **pBuffer);
	VkResult unlockAndroidHardwareBuffer();

	AHardwareBuffer *ahb = nullptr;
	AHardwareBuffer_Desc ahbDesc = {};
	AHardwareBuffer_Planes ahbPlanes = {};
	vk::Device *device = nullptr;
	AllocateInfo allocateInfo;
};

#endif  // VK_DEVICE_MEMORY_EXTERNAL_ANDROID_HPP_
