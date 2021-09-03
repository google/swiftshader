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
	void (*instanceInit)(void *external, const DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo);
};

// Template function that parses a |pAllocateInfo.pNext| chain to verify that
// it asks for the creation or import of a memory type managed by implementation
// class T. On success, return true and sets |pTraits| accordingly. Otherwise
// return false.
template<typename T>
static bool parseCreateInfo(const DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo,
                            ExternalMemoryTraits *pTraits)
{
	if(T::SupportsAllocateInfo(extendedAllocationInfo))
	{
		pTraits->typeFlagBit = T::typeFlagBit;
		pTraits->instanceSize = sizeof(T);
		pTraits->instanceInit = [](void *external,
		                           const DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo) {
			new(external) T(extendedAllocationInfo);
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
	static bool SupportsAllocateInfo(const DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo)
	{
		return true;
	}

	DeviceMemoryHostExternalBase(const DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo) {}

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

		AllocateInfo(const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo)
		{
			if(extendedAllocationInfo.importMemoryHostPointerInfo)
			{
				if((extendedAllocationInfo.importMemoryHostPointerInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT) &&
				   (extendedAllocationInfo.importMemoryHostPointerInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT))
				{
					UNSUPPORTED("extendedAllocationInfo.importMemoryHostPointerInfo->handleType");
				}
				hostPointer = extendedAllocationInfo.importMemoryHostPointerInfo->pHostPointer;
				supported = true;
			}
		}
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = (VkExternalMemoryHandleTypeFlagBits)(VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT | VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT);

	static bool SupportsAllocateInfo(const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo)
	{
		AllocateInfo info(extendedAllocationInfo);
		return info.supported;
	}

	explicit ExternalMemoryHost(const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo)
	    : allocateInfo(extendedAllocationInfo)
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
	OpaqueFdAllocateInfo(const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo)
	{
		if(extendedAllocationInfo.importMemoryFdInfo)
		{
			if(extendedAllocationInfo.importMemoryFdInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
			{
				UNSUPPORTED("VkImportMemoryFdInfoKHR::handleType %d", int(extendedAllocationInfo.importMemoryFdInfo->handleType));
			}
			importFd = true;
			fd = extendedAllocationInfo.importMemoryFdInfo->fd;
		}

		if(extendedAllocationInfo.exportMemoryAllocateInfo)
		{
			if(extendedAllocationInfo.exportMemoryAllocateInfo->handleTypes != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
			{
				UNSUPPORTED("VkExportMemoryAllocateInfo::handleTypes %d", int(extendedAllocationInfo.exportMemoryAllocateInfo->handleTypes));
			}
			exportFd = true;
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

static void findTraits(const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo,
                       ExternalMemoryTraits *pTraits)
{
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	if(parseCreateInfo<AHardwareBufferExternalMemory>(extendedAllocationInfo, pTraits))
	{
		return;
	}
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	if(parseCreateInfo<OpaqueFdExternalMemory>(extendedAllocationInfo, pTraits))
	{
		return;
	}
#endif
#if VK_USE_PLATFORM_FUCHSIA
	if(parseCreateInfo<zircon::VmoExternalMemory>(extendedAllocationInfo, pTraits))
	{
		return;
	}
#endif
	if(parseCreateInfo<ExternalMemoryHost>(extendedAllocationInfo, pTraits))
	{
		return;
	}
	parseCreateInfo<DeviceMemoryHostExternalBase>(extendedAllocationInfo, pTraits);
}

DeviceMemory::DeviceMemory(const VkMemoryAllocateInfo *pAllocateInfo, void *mem, const vk::DeviceMemory::ExtendedAllocationInfo &extendedAllocationInfo, Device *pDevice)
    : size(pAllocateInfo->allocationSize)
    , memoryTypeIndex(pAllocateInfo->memoryTypeIndex)
    , device(pDevice)
{
	ASSERT(size);

	ExternalMemoryTraits traits;
	findTraits(extendedAllocationInfo, &traits);
	traits.instanceInit(mem, extendedAllocationInfo);
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
	ExtendedAllocationInfo extendedAllocationInfo;
	ParseAllocationInfo(pAllocateInfo, &extendedAllocationInfo);
	ExternalMemoryTraits traits;
	findTraits(extendedAllocationInfo, &traits);
	return traits.instanceSize;
}

VkResult DeviceMemory::ParseAllocationInfo(const VkMemoryAllocateInfo *pAllocateInfo, DeviceMemory::ExtendedAllocationInfo *extendedAllocationInfo)
{
	const VkBaseInStructure *allocationInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
	while(allocationInfo)
	{
		switch(allocationInfo->sType)
		{
		case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
			// This can safely be ignored on most platforms, as the Vulkan spec mentions:
			// "If the pNext chain includes a VkMemoryDedicatedAllocateInfo structure, then that structure
			//  includes a handle of the sole buffer or image resource that the memory *can* be bound to."
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
			extendedAllocationInfo->dedicatedAllocateInfo = reinterpret_cast<const VkMemoryDedicatedAllocateInfo *>(allocationInfo);
#endif
			break;
		case VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO:
			// This extension controls on which physical devices the memory gets allocated.
			// SwiftShader only has a single physical device, so this extension does nothing in this case.
			break;
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
		case VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR:
			extendedAllocationInfo->importMemoryFdInfo = reinterpret_cast<const VkImportMemoryFdInfoKHR *>(allocationInfo);
			if(extendedAllocationInfo->importMemoryFdInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT)
			{
				UNSUPPORTED("extendedAllocationInfo->importMemoryFdInfo->handleType %u", extendedAllocationInfo->importMemoryFdInfo->handleType);
				return VK_ERROR_INVALID_EXTERNAL_HANDLE;
			}
			break;
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
		case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
			extendedAllocationInfo->exportMemoryAllocateInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(allocationInfo);
			switch(extendedAllocationInfo->exportMemoryAllocateInfo->handleTypes)
			{
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
			case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
				break;
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
			case VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID:
				break;
#endif
#if VK_USE_PLATFORM_FUCHSIA
			case VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA:
				break;
#endif
			default:
				UNSUPPORTED("extendedAllocationInfo->exportMemoryAllocateInfo->handleTypes %u", extendedAllocationInfo->exportMemoryAllocateInfo->handleTypes);
				return VK_ERROR_INVALID_EXTERNAL_HANDLE;
			}
			break;
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
		case VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID:
			extendedAllocationInfo->importAndroidHardwareBufferInfo = reinterpret_cast<const VkImportAndroidHardwareBufferInfoANDROID *>(allocationInfo);
			break;
#endif  // SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
		case VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT:
			extendedAllocationInfo->importMemoryHostPointerInfo = reinterpret_cast<const VkImportMemoryHostPointerInfoEXT *>(allocationInfo);
			if((extendedAllocationInfo->importMemoryHostPointerInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT) &&
			   (extendedAllocationInfo->importMemoryHostPointerInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_MAPPED_FOREIGN_MEMORY_BIT_EXT))
			{
				UNSUPPORTED("extendedAllocationInfo->importMemoryHostPointerInfo->handleType %u", extendedAllocationInfo->importMemoryHostPointerInfo->handleType);
				return VK_ERROR_INVALID_EXTERNAL_HANDLE;
			}
			break;
#if VK_USE_PLATFORM_FUCHSIA
		case VK_STRUCTURE_TYPE_TEMP_IMPORT_MEMORY_ZIRCON_HANDLE_INFO_FUCHSIA:
			extendedAllocationInfo->importMemoryZirconHandleInfo = reinterpret_cast<const VkImportMemoryZirconHandleInfoFUCHSIA *>(allocationInfo);
			if(extendedAllocationInfo->importMemoryZirconHandleInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA)
			{
				UNSUPPORTED("extendedAllocationInfo->importMemoryZirconHandleInfo->handleType %u", extendedAllocationInfo->importMemoryZirconHandleInfo->handleType);
				return VK_ERROR_INVALID_EXTERNAL_HANDLE;
			}
			break;
#endif  // VK_USE_PLATFORM_FUCHSIA
		default:
			LOG_TRAP("pAllocateInfo->pNext sType = %s", vk::Stringify(allocationInfo->sType).c_str());
			break;
		}

		allocationInfo = allocationInfo->pNext;
	}

	return VK_SUCCESS;
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
		// This device memory is not external and can accommodate
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
