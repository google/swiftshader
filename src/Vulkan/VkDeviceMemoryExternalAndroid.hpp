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

#include "VkStringify.hpp"

#include "System/Debug.hpp"

#include <android/hardware_buffer.h>

#include <errno.h>
#include <string.h>

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
		AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
		{
			const auto *createInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
			while(createInfo)
			{
				switch(createInfo->sType)
				{
					case VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID:
					{
						const auto *importInfo = reinterpret_cast<const VkImportAndroidHardwareBufferInfoANDROID *>(createInfo);
						importAhb = true;
						ahb = importInfo->buffer;
					}
					break;
					case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
					{
						const auto *exportInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(createInfo);

						if(exportInfo->handleTypes != VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID)
						{
							UNSUPPORTED("VkExportMemoryAllocateInfo::handleTypes %d", int(exportInfo->handleTypes));
						}
						exportAhb = true;
					}
					break;
					case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
					{
						// AHB requires dedicated allocation -- for images, the gralloc gets to decide the image layout,
						// not us.
						const auto *dedicatedAllocateInfo = reinterpret_cast<const VkMemoryDedicatedAllocateInfo *>(createInfo);
						imageHandle = vk::Cast(dedicatedAllocateInfo->image);
						bufferHandle = vk::Cast(dedicatedAllocateInfo->buffer);
					}
					break;

					default:
						WARN("VkMemoryAllocateInfo->pNext sType = %s", vk::Stringify(createInfo->sType).c_str());
				}
				createInfo = createInfo->pNext;
			}
		}
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;

	static bool supportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		AllocateInfo info(pAllocateInfo);
		return (info.importAhb || info.exportAhb) && (info.bufferHandle || info.imageHandle);
	}

	explicit AHardwareBufferExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo)
	    : allocateInfo(pAllocateInfo)
	{
	}

	~AHardwareBufferExternalMemory()
	{
		if(ahb)
			AHardwareBuffer_release(ahb);
	}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		if(allocateInfo.importAhb)
		{
			//ahb = allocateInfo.ahb;
			//AHardwareBuffer_acquire(ahb);
			// TODO: also allocate our internal shadow memory
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
		else
		{
			ASSERT(allocateInfo.exportAhb);
			// TODO: create and import the AHB
			return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}

		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		/*
		void *addr = memfd.mapReadWrite(0, size);
		if(!addr)
		{
			return VK_ERROR_MEMORY_MAP_FAILED;
		}
		*pBuffer = addr;
		return VK_SUCCESS;
		 */
	}

	void deallocate(void *buffer, size_t size) override
	{
		// FIXME
	}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

	VkResult exportAhb(struct AHardwareBuffer **pAhb) const override
	{
		// Each call to vkGetMemoryAndroidHardwareBufferANDROID *must* return an Android hardware buffer with a new reference
		// acquired in addition to the reference held by the VkDeviceMemory. To avoid leaking resources, the application *must*
		// release the reference by calling AHardwareBuffer_release when it is no longer needed.
		AHardwareBuffer_acquire(ahb);
		*pAhb = ahb;
		return VK_SUCCESS;
	}

	static VkResult getAhbProperties(const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
	{
		UNIMPLEMENTED("b/141698760: getAhbProperties");  // FIXME(b/141698760)
		return VK_SUCCESS;
	}

private:
	struct AHardwareBuffer *ahb = nullptr;
	AllocateInfo allocateInfo;
};
