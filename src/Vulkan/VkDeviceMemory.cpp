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
#include "VkImage.hpp"

#include "VkConfig.h"

namespace vk {

// Base abstract interface for a device memory implementation.
class DeviceMemory::ExternalBase
{
public:
	virtual ~ExternalBase() = default;

	// Allocate the memory according to |size|. On success return VK_SUCCESS
	// and sets |*pBuffer|.
	virtual VkResult allocate(size_t size, void **pBuffer) = 0;

	// Deallocate previously allocated memory at |buffer|.
	virtual void deallocate(void *buffer, size_t size) = 0;

	// Return the handle type flag bit supported by this implementation.
	// A value of 0 corresponds to non-external memory.
	virtual VkExternalMemoryHandleTypeFlagBits getFlagBit() const = 0;

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	virtual VkResult exportFd(int *pFd) const
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
#endif

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	virtual VkResult exportAhb(struct AHardwareBuffer **pAhb) const
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
#endif

protected:
	ExternalBase() = default;
};

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
	if(T::supportsAllocateInfo(pAllocateInfo))
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
	static bool supportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		return true;
	}

	DeviceMemoryHostExternalBase(const VkMemoryAllocateInfo *pAllocateInfo) {}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		void *buffer = vk::allocate(size, REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY);
		if(!buffer)
		{
			return VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}

		*pBuffer = buffer;
		return VK_SUCCESS;
	}

	void deallocate(void *buffer, size_t size) override
	{
		vk::deallocate(buffer, DEVICE_MEMORY);
	}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}
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

	static bool supportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
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
#	if defined(__linux__) || defined(__ANDROID__)
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

namespace vk {

static void findTraits(const VkMemoryAllocateInfo *pAllocateInfo,
                       ExternalMemoryTraits *pTraits)
{
#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
	if(parseCreateInfo<OpaqueFdExternalMemory>(pAllocateInfo, pTraits))
	{
		return;
	}
#endif
#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
	if(parseCreateInfo<AHardwareBufferExternalMemory>(pAllocateInfo, pTraits))
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

DeviceMemory::DeviceMemory(const VkMemoryAllocateInfo *pAllocateInfo, void *mem)
    : size(pAllocateInfo->allocationSize)
    , memoryTypeIndex(pAllocateInfo->memoryTypeIndex)
    , external(reinterpret_cast<ExternalBase *>(mem))
{
	ASSERT(size);

	ExternalMemoryTraits traits;
	findTraits(pAllocateInfo, &traits);
	traits.instanceInit(external, pAllocateInfo);
}

void DeviceMemory::destroy(const VkAllocationCallbacks *pAllocator)
{
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
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	VkResult result = VK_SUCCESS;
	if(!buffer)
	{
		result = external->allocate(size, &buffer);
	}
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

#if SWIFTSHADER_EXTERNAL_MEMORY_OPAQUE_FD
VkResult DeviceMemory::exportFd(int *pFd) const
{
	return external->exportFd(pFd);
}
#endif

#if SWIFTSHADER_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER
VkResult DeviceMemory::exportAhb(struct AHardwareBuffer **pAhb) const
{
	return external->exportAhb(pAhb);
}

VkResult DeviceMemory::getAhbProperties(const struct AHardwareBuffer *buffer, VkAndroidHardwareBufferPropertiesANDROID *pProperties)
{
	return AHardwareBufferExternalMemory::getAhbProperties(buffer, pProperties);
}
#endif

}  // namespace vk
