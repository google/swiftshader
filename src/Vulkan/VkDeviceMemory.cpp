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
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "VkDeviceMemoryExternalBase.hpp"
#include "VkImage.hpp"
#include "VkStringify.hpp"

#include "VkConfig.hpp"

namespace vk {

// Small class describing a given DeviceMemory::ExternalBase derived class.
// |typeFlagBit| corresponds to the external memory handle type.
// |instanceSize| is the size of each class instance in bytes.
// |instanceInit| is a function pointer used to initialize an instance inplace
// according to a |pAllocateInfo| parameter.
class ExternalMemoryTraits
{
public:
	VkExternalMemoryHandleTypeFlagBits typeFlagBit;
	size_t instanceSize;
	void (*instanceInit)(void *external, const VkMemoryAllocateInfo *pAllocateInfo);
};

// Template function that parses a |pAllocateInfo.pNext| chain to verify that
// it asks for the creation or import of a memory type managed by implementation
// class T. On success, return true and sets |pTraits| accordingly. Otherwise
// return false.
template<typename T>
static bool parseCreateInfo(const VkMemoryAllocateInfo *pAllocateInfo,
                            ExternalMemoryTraits *pTraits)
{
	if(T::SupportsAllocateInfo(pAllocateInfo))
	{
		pTraits->typeFlagBit = T::typeFlagBit;
		pTraits->instanceSize = sizeof(T);
		pTraits->instanceInit = [](void *external,
		                           const VkMemoryAllocateInfo *pAllocateInfo) {
			new(external) T(pAllocateInfo);
		};
		return true;
	}
	return false;
}

// DeviceMemory::ExternalBase implementation that uses host memory.
// Not really external, but makes everything simpler.
class DeviceMemoryHostExternalBase : public DeviceMemory::ExternalBase
{
public:
	// Does not support any external memory type at all.
	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = (VkExternalMemoryHandleTypeFlagBits)0;

	// Always return true as is used as a fallback in findTraits() below.
	static bool SupportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		return true;
	}

	DeviceMemoryHostExternalBase(const VkMemoryAllocateInfo *pAllocateInfo) {}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		buffer = vk::allocate(size, REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY);
		if(!buffer)
		{
			return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}

		*pBuffer = buffer;
		return VK_SUCCESS;
	}

	void deallocate(void * /* buffer */, size_t size) override
	{
		vk::deallocate(buffer, DEVICE_MEMORY);
		buffer = nullptr;
	}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

#ifdef SWIFTSHADER_DEVICE_MEMORY_REPORT
	uint64_t getMemoryObjectId() const override
	{
		return (uint64_t)buffer;
	}
#endif  // SWIFTSHADER_DEVICE_MEMORY_REPORT

private:
	void *buffer = nullptr;
};

}  // namespace vk

// Host-allocated memory and host-mapped foreign memory
class ExternalMemoryHost : public vk::DeviceMemory::ExternalBase
{
public:
	struct AllocateInfo
	{
		bool supported = false;
		void *hostPointer = nullptr;

		AllocateInfo() = default;

		AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
		{
			const auto *createInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
			while(createInfo)
			{
				switch(createInfo->sType)
				{
					case VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT:
					{
						const auto *importInfo = reinterpret_cast<const VkImportMemoryHostPointerInfoEXT *>(createInfo);

						if(importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT && importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT)
						{
							UNSUPPORTED("importInfo->handleType");
						}
						hostPointer = importInfo->pHostPointer;
						supported = true;
						break;
					}
					default:
						break;
				}
				createInfo = createInfo->pNext;
			}
		}
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = (VkExternalMemoryHandleTypeFlagBits)(VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT | VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT);

	static bool SupportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		AllocateInfo info(pAllocateInfo);
		return info.supported;
	}

	explicit ExternalMemoryHost(const VkMemoryAllocateInfo *pAllocateInfo)
	    : allocateInfo(pAllocateInfo)
	{
	}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		if(allocateInfo.supported)
		{
			*pBuffer = allocateInfo.hostPointer;
			return VK_SUCCESS;
		}
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	void deallocate(void *buffer, size_t size) override
	{}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

private:
	AllocateInfo allocateInfo;
};

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD

// Helper struct to parse the VkMemoryAllocateInfo.pNext chain and
// extract relevant information related to the handle type supported
// by this DeviceMemory;:ExternalBase subclass.
struct OpaqueFdAllocateInfo
{
	bool importFd = false;
	bool exportFd = false;
	int fd = -1;

	OpaqueFdAllocateInfo() = default;

	// Parse the VkMemoryAllocateInfo.pNext chain to initialize an OpaqueFdAllocateInfo.
	OpaqueFdAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		const auto *createInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
		while(createInfo)
		{
			switch(createInfo->sType)
			{
				case VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR:
				{
					const auto *importInfo = reinterpret_cast<const VkImportMemoryFdInfoKHR *>(createInfo);

					if(importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
					{
						UNSUPPORTED("VkImportMemoryFdInfoKHR::handleType %d", int(importInfo->handleType));
					}
					importFd = true;
					fd = importInfo->fd;
				}
				break;
				case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
				{
					const auto *exportInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(createInfo);

					if(exportInfo->handleTypes != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
					{
						UNSUPPORTED("VkExportMemoryAllocateInfo::handleTypes %d", int(exportInfo->handleTypes));
					}
					exportFd = true;
				}
				break;
				case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
					// This can safely be ignored, as the Vulkan spec mentions:
					// "If the pNext chain includes a VkMemoryDedicatedAllocateInfo structure, then that structure
					//  includes a handle of the sole buffer or image resource that the memory *can* be bound to."
					break;
				default:
					WARN("VkMemoryAllocateInfo->pNext sType = %s", vk::Stringify(createInfo->sType).c_str());
			}
			createInfo = createInfo->pNext;
		}
	}
};

#	if defined(__APPLE__)
#		include "VkDeviceMemoryExternalMac.hpp"
#	elif defined(__linux__) && !defined(__ANDROID__)
#		include "VkDeviceMemoryExternalLinux.hpp"
#	else
#		error "Missing VK_KHR_external_memory_fd implementation for this platform!"
#	endif
#endif

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
#	if defined(__ANDROID__)
#		include "VkDeviceMemoryExternalAndroid.hpp"
#	else
#		error "Missing VK_ANDROID_external_memory_android_hardware_buffer implementation for this platform!"
#	endif
#endif

#if VK_USE_PLATFORM_FUCHSIA
#	include "VkDeviceMemoryExternalFuchsia.hpp"
#endif

namespace vk {

static void findTraits(const VkMemoryAllocateInfo *pAllocateInfo,
                       ExternalMemoryTraits *pTraits)
{
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	if(parseCreateInfo<AHardwareBufferExternalMemory>(pAllocateInfo, pTraits))
	{
		return;
	}
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	if(parseCreateInfo<OpaqueFdExternalMemory>(pAllocateInfo, pTraits))
	{
		return;
	}
#endif
#if VK_USE_PLATFORM_FUCHSIA
	if(parseCreateInfo<zircon::VmoExternalMemory>(pAllocateInfo, pTraits))
	{
		return;
	}
#endif
	if(parseCreateInfo<ExternalMemoryHost>(pAllocateInfo, pTraits))
	{
		return;
	}
	parseCreateInfo<DeviceMemoryHostExternalBase>(pAllocateInfo, pTraits);
}

DeviceMemory::DeviceMemory(const VkMemoryAllocateInfo *pAllocateInfo, void *mem, Device *pDevice)
    : size(pAllocateInfo->allocationSize)
    , memoryTypeIndex(pAllocateInfo->memoryTypeIndex)
    , device(pDevice)
{
	ASSERT(size);

	ExternalMemoryTraits traits;
	findTraits(pAllocateInfo, &traits);
	traits.instanceInit(mem, pAllocateInfo);
	external = reinterpret_cast<ExternalBase *>(mem);
	external->setDevicePtr(device);
}

void DeviceMemory::destroy(const VkAllocationCallbacks *pAllocator)
{
#ifdef SWIFTSHADER_DEVICE_MEMORY_REPORT
	device->emitDeviceMemoryReport(external->isImport() ? VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_UNIMPORT_EXT : VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_FREE_EXT, external->getMemoryObjectId(), 0 /* size */, VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)(void *)VkDeviceMemory(*this));
#endif  // SWIFTSHADER_DEVICE_MEMORY_REPORT
	if(buffer)
	{
		external->deallocate(buffer, size);
		buffer = nullptr;
	}
	external->~ExternalBase();  // Call virtual destructor in place.
	vk::deallocate(external, pAllocator);
}

size_t DeviceMemory::ComputeRequiredAllocationSize(const VkMemoryAllocateInfo *pAllocateInfo)
{
	ExternalMemoryTraits traits;
	findTraits(pAllocateInfo, &traits);
	return traits.instanceSize;
}

VkResult DeviceMemory::allocate()
{
	if(size > MAX_MEMORY_ALLOCATION_SIZE)
	{
#ifdef SWIFTSHADER_DEVICE_MEMORY_REPORT
		device->emitDeviceMemoryReport(VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATION_FAILED_EXT, 0 /* memoryObjectId */, size, VK_OBJECT_TYPE_DEVICE_MEMORY, 0 /* objectHandle */);
#endif  // SWIFTSHADER_DEVICE_MEMORY_REPORT
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	VkResult result = VK_SUCCESS;
	if(!buffer)
	{
		result = external->allocate(size, &buffer);
	}
#ifdef SWIFTSHADER_DEVICE_MEMORY_REPORT
	if(result == VK_SUCCESS)
	{
		device->emitDeviceMemoryReport(external->isImport() ? VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_IMPORT_EXT : VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATE_EXT, external->getMemoryObjectId(), size, VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)(void *)VkDeviceMemory(*this));
	}
	else
	{
		device->emitDeviceMemoryReport(VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATION_FAILED_EXT, 0 /* memoryObjectId */, size, VK_OBJECT_TYPE_DEVICE_MEMORY, 0 /* objectHandle */);
	}
#endif  // SWIFTSHADER_DEVICE_MEMORY_REPORT
	return result;
}

VkResult DeviceMemory::map(VkDeviceSize pOffset, VkDeviceSize pSize, void **ppData)
{
	*ppData = getOffsetPointer(pOffset);

	return VK_SUCCESS;
}

VkDeviceSize DeviceMemory::getCommittedMemoryInBytes() const
{
	return size;
}

void *DeviceMemory::getOffsetPointer(VkDeviceSize pOffset) const
{
	ASSERT(buffer);
	return reinterpret_cast<char *>(buffer) + pOffset;
}

bool DeviceMemory::checkExternalMemoryHandleType(
    VkExternalMemoryHandleTypeFlags supportedHandleTypes) const
{
	if(!supportedHandleTypes)
	{
		// This image or buffer does not need to be stored on external
		// memory, so this check should always pass.
		return true;
	}
	VkExternalMemoryHandleTypeFlagBits handle_type_bit = external->getFlagBit();
	if(!handle_type_bit)
	{
		// This device memory is not external and can accomodate
		// any image or buffer as well.
		return true;
	}
	// Return true only if the external memory type is compatible with the
	// one specified during VkCreate{Image,Buffer}(), through a
	// VkExternalMemory{Image,Buffer}AllocateInfo struct.
	return (supportedHandleTypes & handle_type_bit) != 0;
}

bool DeviceMemory::hasExternalImageProperties() const
{
	return external && external->hasExternalImageProperties();
}

int DeviceMemory::externalImageRowPitchBytes(VkImageAspectFlagBits aspect) const
{
	if(external)
	{
		return external->externalImageRowPitchBytes(aspect);
	}

	// This function should never be called on non-external memory.
	ASSERT(false);
	return -1;
}

VkDeviceSize DeviceMemory::externalImageMemoryOffset(VkImageAspectFlagBits aspect) const
{
	if(external)
	{
		return external->externalImageMemoryOffset(aspect);
	}

	// This function should never be called on non-external memory.
	ASSERT(false);
	return -1;
}

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
VkResult DeviceMemory::exportFd(int *pFd) const
{
	return external->exportFd(pFd);
}
#endif

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
VkResult DeviceMemory::exportAndroidHardwareBuffer(struct AHardwareBuffer **pAhb) const
{
	if(external->getFlagBit() != VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID)
	{
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}
	return static_cast<AHardwareBufferExternalMemory *>(external)->exportAndroidHardwareBuffer(pAhb);
}

VkResult DeviceMemory::GetAndroidHardwareBufferProperties(VkDevice &ahbDevice, const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
{
	return AHardwareBufferExternalMemory::GetAndroidHardwareBufferProperties(ahbDevice, buffer, pProperties);
}
#endif

#if VK_USE_PLATFORM_FUCHSIA
VkResult DeviceMemory::exportHandle(zx_handle_t *pHandle) const
{
	return external->exportHandle(pHandle);
}
#endif

}  // namespace vk
