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

#include "VkSemaphore.hpp"

#include "VkConfig.h"
#include "VkStringify.hpp"

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
#	if defined(__linux__) || defined(__ANDROID__)
#		include "VkSemaphoreExternalLinux.hpp"
#	else
#		error "Missing VK_KHR_external_semaphore_fd implementation for this platform!"
#	endif
#elif VK_USE_PLATFORM_FUCHSIA
#	include "VkSemaphoreExternalFuchsia.hpp"
#else
#	include "VkSemaphoreExternalNone.hpp"
#endif

#include "marl/blockingcall.h"
#include "marl/conditionvariable.h"

#include <functional>
#include <memory>
#include <utility>

namespace vk {

namespace {

struct SemaphoreCreateInfo
{
	bool exportSemaphore = false;

	// Create a new instance. The external instance will be allocated only
	// the pCreateInfo->pNext chain indicates it needs to be exported.
	SemaphoreCreateInfo(const VkSemaphoreCreateInfo *pCreateInfo)
	{
		for(const auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
		    nextInfo != nullptr; nextInfo = nextInfo->pNext)
		{
			switch(nextInfo->sType)
			{
				case VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO:
				{
					const auto *exportInfo = reinterpret_cast<const VkExportSemaphoreCreateInfo *>(nextInfo);
					exportSemaphore = true;
					if(exportInfo->handleTypes != Semaphore::External::kExternalSemaphoreHandleType)
					{
						UNSUPPORTED("exportInfo->handleTypes %d", int(exportInfo->handleTypes));
					}
				}
				break;
				default:
					WARN("nextInfo->sType = %s", vk::Stringify(nextInfo->sType).c_str());
			}
		}
	}
};

}  // namespace

void Semaphore::wait()
{
	if(external)
	{
		if(!external->tryWait())
		{
			// Dispatch the external wait to a background thread.
			// Even if this creates a new thread on each
			// call, it is assumed that this is negligible
			// compared with the actual semaphore wait()
			// operation.
			marl::blocking_call([this]() {
				external->wait();
			});
		}

		// If the import was temporary, reset the semaphore to its
		// permanent state by getting rid of |external|.
		// See "6.4.5. Importing Semaphore Payloads" in Vulkan 1.1 spec.
		if(temporaryImport)
		{
			deallocateExternal();
			temporaryImport = false;
		}
	}
	else
	{
		internal.wait();
	}
}

void Semaphore::signal()
{
	if(external)
	{
		// Assumes that signalling an external semaphore is non-blocking,
		// so it can be performed directly either from a fiber or thread.
		external->signal();
	}
	else
	{
		internal.signal();
	}
}

Semaphore::Semaphore(const VkSemaphoreCreateInfo *pCreateInfo, void *mem, const VkAllocationCallbacks *pAllocator)
    : allocator(pAllocator)
{
	SemaphoreCreateInfo info(pCreateInfo);
	if(info.exportSemaphore)
	{
		allocateExternal();
		external->init();
	}
}

void Semaphore::destroy(const VkAllocationCallbacks *pAllocator)
{
	deallocateExternal();
}

size_t Semaphore::ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo *pCreateInfo)
{
	// Semaphore::External instance is created and destroyed on demand so return 0 here.
	return 0;
}

void Semaphore::allocateExternal()
{
	ASSERT(external == nullptr);
	external = reinterpret_cast<Semaphore::External *>(
	    vk::allocate(sizeof(Semaphore::External), vk::REQUIRED_MEMORY_ALIGNMENT, allocator));
	new(external) Semaphore::External();
}

void Semaphore::deallocateExternal()
{
	if(external)
	{
		vk::deallocate(external, allocator);
		external = nullptr;
	}
}

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
VkResult Semaphore::importFd(int fd, bool tempImport)
{
	std::unique_lock<std::mutex> lock(mutex);
	if(!external)
	{
		allocateExternal();
	}
	VkResult result = external->importFd(fd);
	if(result != VK_SUCCESS)
	{
		deallocateExternal();
	}
	else
	{
		temporaryImport = tempImport;
	}
	return result;
}

VkResult Semaphore::exportFd(int *pFd)
{
	std::unique_lock<std::mutex> lock(mutex);
	if(!external)
	{
		TRACE("Cannot export non-external semaphore");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	return external->exportFd(pFd);
}
#endif  // SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD

#if VK_USE_PLATFORM_FUCHSIA
VkResult Semaphore::importHandle(zx_handle_t handle, bool tempImport)
{
	std::unique_lock<std::mutex> lock(mutex);
	if(!external)
	{
		allocateExternal();
	}
	// NOTE: Imports are just moving a handle so cannot fail.
	external->importHandle(handle);
	temporaryImport = tempImport;
	return VK_SUCCESS;
}

VkResult Semaphore::exportHandle(zx_handle_t *pHandle)
{
	std::unique_lock<std::mutex> lock(mutex);
	if(!external)
	{
		TRACE("Cannot export non-external semaphore");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	return external->exportHandle(pHandle);
}
#endif  // VK_USE_PLATFORM_FUCHSIA

}  // namespace vk
