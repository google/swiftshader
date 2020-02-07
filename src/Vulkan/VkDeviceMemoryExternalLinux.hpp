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
#include "System/Linux/MemFd.hpp"

#include <errno.h>
#include <string.h>
#include <sys/mman.h>

class OpaqueFdExternalMemory : public vk::DeviceMemory::ExternalBase
{
public:
	// Helper struct to parse the VkMemoryAllocateInfo.pNext chain and
	// extract relevant information related to the handle type supported
	// by this DeviceMemory;:ExternalBase subclass.
	struct AllocateInfo
	{
		bool importFd = false;
		bool exportFd = false;
		int fd = -1;

		AllocateInfo() = default;

		// Parse the VkMemoryAllocateInfo.pNext chain to initialize an AllocateInfo.
		AllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
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

					default:
						WARN("VkMemoryAllocateInfo->pNext sType = %s", vk::Stringify(createInfo->sType).c_str());
				}
				createInfo = createInfo->pNext;
			}
		}
	};

	static const VkExternalMemoryHandleTypeFlagBits typeFlagBit = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;

	static bool supportsAllocateInfo(const VkMemoryAllocateInfo *pAllocateInfo)
	{
		AllocateInfo info(pAllocateInfo);
		return info.importFd || info.exportFd;
	}

	explicit OpaqueFdExternalMemory(const VkMemoryAllocateInfo *pAllocateInfo)
	    : allocateInfo(pAllocateInfo)
	{
	}

	~OpaqueFdExternalMemory()
	{
		memfd.close();
	}

	VkResult allocate(size_t size, void **pBuffer) override
	{
		if(allocateInfo.importFd)
		{
			memfd.importFd(allocateInfo.fd);
			if(!memfd.isValid())
			{
				return VK_ERROR_INVALID_EXTERNAL_HANDLE;
			}
		}
		else
		{
			ASSERT(allocateInfo.exportFd);
			static int counter = 0;
			char name[40];
			snprintf(name, sizeof(name), "SwiftShader.Memory.%d", ++counter);
			if(!memfd.allocate(name, size))
			{
				TRACE("memfd.allocate() returned %s", strerror(errno));
				return VK_ERROR_OUT_OF_DEVICE_MEMORY;
			}
		}
		void *addr = memfd.mapReadWrite(0, size);
		if(!addr)
		{
			return VK_ERROR_MEMORY_MAP_FAILED;
		}
		*pBuffer = addr;
		return VK_SUCCESS;
	}

	void deallocate(void *buffer, size_t size) override
	{
		memfd.unmap(buffer, size);
	}

	VkExternalMemoryHandleTypeFlagBits getFlagBit() const override
	{
		return typeFlagBit;
	}

	VkResult exportFd(int *pFd) const override
	{
		int fd = memfd.exportFd();
		if(fd < 0)
		{
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
		*pFd = fd;
		return VK_SUCCESS;
	}

private:
	LinuxMemFd memfd;
	AllocateInfo allocateInfo;
};
