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

#include "marl/blockingcall.h"
#include "marl/conditionvariable.h"

#include <functional>
#include <memory>
#include <utility>

namespace vk {

// This is a base abstract class for all external semaphore implementations
// used in this source file.
class Semaphore::External
{
public:
	virtual ~External() = default;

	// Initialize new instance with a given initial state.
	virtual VkResult init(bool initialState) = 0;

	virtual bool tryWait() = 0;
	virtual void wait() = 0;
	virtual void signal() = 0;

	// For VK_KHR_external_semaphore_fd
	virtual VkResult importOpaqueFd(int fd)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	virtual VkResult exportOpaqueFd(int *pFd)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

#if VK_USE_PLATFORM_FUCHSIA
	// For VK_FUCHSIA_external_semaphore
	virtual VkResult importHandle(zx_handle_t handle)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	virtual VkResult exportHandle(zx_handle_t *pHandle)
	{
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
#endif
	// Pointer to previous temporary external instanc,e used for |tempExternal| only.
	External *previous = nullptr;
};

}  // namespace vk

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
#	if defined(__linux__) || defined(__ANDROID__)
#		include "VkSemaphoreExternalLinux.hpp"
#	else
#		error "Missing VK_KHR_external_semaphore_fd implementation for this platform!"
#	endif
#elif VK_USE_PLATFORM_FUCHSIA
#	include "VkSemaphoreExternalFuchsia.hpp"
#endif

namespace vk {

// The bitmask of all external semaphore handle types supported by this source file.
static const VkExternalSemaphoreHandleTypeFlags kSupportedTypes =
#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
    VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT |
#endif
#if VK_USE_PLATFORM_FUCHSIA
    VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA |
#endif
    0;

namespace {

struct SemaphoreCreateInfo
{
	bool exportSemaphore = false;
	VkExternalSemaphoreHandleTypeFlags exportHandleTypes = 0;

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
					exportHandleTypes = exportInfo->handleTypes;
					if((exportHandleTypes & ~kSupportedTypes) != 0)
					{
						UNSUPPORTED("exportInfo->handleTypes 0x%X (supports 0x%X)",
						            int(exportHandleTypes),
						            int(kSupportedTypes));
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
	std::unique_lock<std::mutex> lock(mutex);
	External *ext = tempExternal ? tempExternal : external;
	if(ext)
	{
		if(!ext->tryWait())
		{
			// Dispatch the ext wait to a background thread.
			// Even if this creates a new thread on each
			// call, it is assumed that this is negligible
			// compared with the actual semaphore wait()
			// operation.
			marl::blocking_call([ext, &lock]() {
				lock.unlock();
				ext->wait();
				lock.lock();
			});
		}

		// If the import was temporary, reset the semaphore to its previous state.
		// See "6.4.5. Importing Semaphore Payloads" in Vulkan 1.1 spec.
		if(ext == tempExternal)
		{
			tempExternal = ext->previous;
			deallocateExternal(ext);
		}
	}
	else
	{
		internal.wait();
	}
}

void Semaphore::signal()
{
	std::unique_lock<std::mutex> lock(mutex);
	External *ext = tempExternal ? tempExternal : external;
	if(ext)
	{
		// Assumes that signalling an external semaphore is non-blocking,
		// so it can be performed directly either from a fiber or thread.
		ext->signal();
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
	exportableHandleTypes = info.exportHandleTypes;
}

void Semaphore::destroy(const VkAllocationCallbacks *pAllocator)
{
	while(tempExternal)
	{
		External *ext = tempExternal;
		tempExternal = ext->previous;
		deallocateExternal(ext);
	}
	if(external)
	{
		deallocateExternal(external);
		external = nullptr;
	}
}

size_t Semaphore::ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo *pCreateInfo)
{
	// Semaphore::External instance is created and destroyed on demand so return 0 here.
	return 0;
}

template<class EXTERNAL>
Semaphore::External *Semaphore::allocateExternal()
{
	auto *ext = reinterpret_cast<Semaphore::External *>(
	    vk::allocate(sizeof(EXTERNAL), alignof(EXTERNAL), allocator));
	new(ext) EXTERNAL();
	return ext;
}

void Semaphore::deallocateExternal(Semaphore::External *ext)
{
	ext->~External();
	vk::deallocate(ext, allocator);
}

template<typename ALLOC_FUNC, typename IMPORT_FUNC>
VkResult Semaphore::importPayload(bool temporaryImport,
                                  ALLOC_FUNC alloc_func,
                                  IMPORT_FUNC import_func)
{
	std::unique_lock<std::mutex> lock(mutex);

	// Create new External instance if needed.
	External *ext = external;
	if(temporaryImport || !ext)
	{
		ext = alloc_func();
	}
	VkResult result = import_func(ext);
	if(result != VK_SUCCESS)
	{
		if(temporaryImport || !external)
		{
			deallocateExternal(ext);
		}
		return result;
	}

	if(temporaryImport)
	{
		ext->previous = tempExternal;
		tempExternal = ext;
	}
	else if(!external)
	{
		external = ext;
	}
	return VK_SUCCESS;
}

template<typename ALLOC_FUNC, typename EXPORT_FUNC>
VkResult Semaphore::exportPayload(ALLOC_FUNC alloc_func, EXPORT_FUNC export_func)
{
	std::unique_lock<std::mutex> lock(mutex);
	// Sanity check, do not try to export a semaphore that has a temporary import.
	if(tempExternal != nullptr)
	{
		TRACE("Cannot export semaphore with a temporary import!");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	// Allocate |external| if it doesn't exist yet.
	if(!external)
	{
		External *ext = alloc_func();
		VkResult result = ext->init(internal.isSignalled());
		if(result != VK_SUCCESS)
		{
			deallocateExternal(ext);
			return result;
		}
		external = ext;
	}
	return export_func(external);
}

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
VkResult Semaphore::importFd(int fd, bool temporaryImport)
{
	return importPayload(
	    temporaryImport,
	    [this]() {
		    return allocateExternal<OpaqueFdExternalSemaphore>();
	    },
	    [fd](External *ext) {
		    return ext->importOpaqueFd(fd);
	    });
}

VkResult Semaphore::exportFd(int *pFd)
{
	if((exportableHandleTypes & VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT) == 0)
	{
		TRACE("Cannot export semaphore as opaque FD (exportableHandleType = 0x%X, want 0x%X)",
		      exportableHandleTypes,
		      VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT);

		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	return exportPayload([this]() { return allocateExternal<OpaqueFdExternalSemaphore>(); },
	                     [pFd](External *ext) {
		                     return ext->exportOpaqueFd(pFd);
	                     });
}
#endif  // SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD

#if VK_USE_PLATFORM_FUCHSIA
VkResult Semaphore::importHandle(zx_handle_t handle, bool temporaryImport)
{
	return importPayload(
	    temporaryImport,
	    [this]() {
		    return allocateExternal<ZirconEventExternalSemaphore>();
	    },
	    [handle](External *ext) {
		    return ext->importHandle(handle);
	    });
}

VkResult Semaphore::exportHandle(zx_handle_t *pHandle)
{
	if((exportableHandleTypes & VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA) == 0)
	{
		TRACE("Cannot export semaphore as Zircon handle (exportableHandleType = 0x%X, want 0x%X)",
		      exportableHandleTypes,
		      VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TEMP_ZIRCON_EVENT_BIT_FUCHSIA);

		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}

	return exportPayload([this]() { return allocateExternal<ZirconEventExternalSemaphore>(); },
	                     [pHandle](External *ext) {
		                     return ext->exportHandle(pHandle);
	                     });
}
#endif  // VK_USE_PLATFORM_FUCHSIA

}  // namespace vk
