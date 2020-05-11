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

#include <zircon/process.h>
#include <zircon/syscalls.h>

namespace zircon {

class VmoExternalMemory : public vk::DeviceMemory::ExternalBase
{
public:
	// Helper struct to parse the VkMemoryAllocateInfo.pNext chain and
	// extract relevant information related to the handle type supported
	// by this DeviceMemory::ExternalBase subclass.
	struct AllocateInfo
	{
		bool importHandle = false;
		bool exportHandle = false;
		zx_handle_t handle = ZX_HANDLE_INVALID;

		AllocateInfo() = default;

		// Parse the VkMemoryAllocateInfo->pNext chain to initialize a AllocateInfo.
		AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
		{
			const auto *extInfo = reinterpret_cast<const VkBaseInStructure *>(pAllocateInfo->pNext);
			while(extInfo)
			{
				switch(extInfo->sType)
				{
					case VK_STRUCTURE_TYPE_TEMP_IMPORT_MEMORY_ZIRCON_HANDLE_INFO_FUCHSIA:
					{
						const auto *importInfo = reinterpret_cast<const VkImportMemoryZirconHandleInfoFUCHSIA *>(extInfo);

						if(importInfo->handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA)
						{
							UNSUPPORTED("importInfo->handleType");
						}
						importHandle = true;
						handle = importInfo->handle;
						break;
					}
					case VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO:
					{
						const auto *exportInfo = reinterpret_cast<const VkExportMemoryAllocateInfo *>(extInfo);

						if(exportInfo->handleTypes != VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA)
						{
							UNSUPPORTED("exportInfo->handleTypes");
						}
						exportHandle = true;
						break;
					}
					case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO:
						// This can safely be ignored, as the Vulkan spec mentions:
						// "If the pNext chain includes a VkMemoryDedicatedAllocateInfo structure, then that structure
						//  includes a handle of the sole buffer or image resource that the memory *can* be bound to."
						break;

					default:
						WARN("VkMemoryAllocateInfo->pNext sType = %s", vk::Stringify(extInfo->sType).c_str());
				}
				extInfo = extInfo->pNext;
			}
		}
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = VK_EXTERNAL_MEMORY_HANDLE_TYPE_TEMP_ZIRCON_VMO_BIT_FUCHSIA;

	static bool supportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		AllocateInfo info(pAllocateInfo);
		return info.importHandle || info.exportHandle;
	}

	explicit VmoExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo)
	    : allocateInfo(pAllocateInfo)
	{
	}

	~VmoExternalMemory()
	{
		closeVmo();
	}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		if(allocateInfo.importHandle)
		{
			// NOTE: handle ownership is passed to the VkDeviceMemory.
			vmoHandle = allocateInfo.handle;
		}
		else
		{
			ASSERT(allocateInfo.exportHandle);
			zx_status_t status = zx_vmo_create(size, 0, &vmoHandle);
			if(status != ZX_OK)
			{
				TRACE("zx_vmo_create() returned %d", status);
				return VK_ERROR_OUT_OF_DEVICE_MEMORY;
			}
		}

		// Now map it directly.
		zx_vaddr_t addr = 0;
		zx_status_t status = zx_vmar_map(zx_vmar_root_self(),
		                                 ZX_VM_PERM_READ | ZX_VM_PERM_WRITE,
		                                 0,  // vmar_offset
		                                 vmoHandle,
		                                 0,  // vmo_offset
		                                 size,
		                                 &addr);
		if(status != ZX_OK)
		{
			TRACE("zx_vmar_map() failed with %d", status);
			return VK_ERROR_MEMORY_MAP_FAILED;
		}
		*pBuffer = reinterpret_cast<void *>(addr);
		return VK_SUCCESS;
	}

	void deallocate(void *buffer, size_t size) override
	{
		zx_status_t status = zx_vmar_unmap(zx_vmar_root_self(),
		                                   reinterpret_cast<zx_vaddr_t>(buffer),
		                                   size);
		if(status != ZX_OK)
		{
			TRACE("zx_vmar_unmap() failed with %d", status);
		}
		closeVmo();
	}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

	VkResult exportHandle(zx_handle_t *pHandle) const override
	{
		if(vmoHandle == ZX_HANDLE_INVALID)
		{
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
		zx_status_t status = zx_handle_duplicate(vmoHandle, ZX_RIGHT_SAME_RIGHTS, pHandle);
		if(status != ZX_OK)
		{
			TRACE("zx_handle_duplicate() returned %d", status);
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
		return VK_SUCCESS;
	}

private:
	void closeVmo()
	{
		if(vmoHandle != ZX_HANDLE_INVALID)
		{
			zx_handle_close(vmoHandle);
			vmoHandle = ZX_HANDLE_INVALID;
		}
	}

	zx_handle_t vmoHandle = ZX_HANDLE_INVALID;
	AllocateInfo allocateInfo;
};

}  // namespace zircon
