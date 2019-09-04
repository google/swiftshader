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

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_LINUX_MEMFD
#include "VkSemaphoreExternalLinux.hpp"
#elif SWIFTSHADER_EXTERNAL_SEMAPHORE_ZIRCON_EVENT
#include "VkSemaphoreExternalFuchsia.hpp"
#else
#include "VkSemaphoreExternalNone.hpp"
#endif

#include "marl/blockingcall.h"
#include "marl/conditionvariable.h"

#include <functional>
#include <memory>
#include <mutex>
#include <utility>

namespace vk
{

// An implementation of VkSemaphore based on Marl primitives.
class Semaphore::Impl
{
public:
	// Create a new instance. The external instance will be allocated only
	// the pCreateInfo->pNext chain indicates it needs to be exported.
	Impl(const VkSemaphoreCreateInfo* pCreateInfo) {
		bool exportSemaphore = false;
		for (const auto* nextInfo = reinterpret_cast<const VkBaseInStructure*>(pCreateInfo->pNext);
			 nextInfo != nullptr; nextInfo = nextInfo->pNext)
		{
			if (nextInfo->sType == VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO)
			{
				const auto* exportInfo = reinterpret_cast<const VkExportSemaphoreCreateInfo *>(nextInfo);
				if (exportInfo->handleTypes != External::kExternalSemaphoreHandleType)
				{
					UNIMPLEMENTED("exportInfo->handleTypes");
				}
				exportSemaphore = true;
				break;
			}
		}

		if (exportSemaphore)
		{
			allocateExternalNoInit();
			external->init();
		}
	}

	~Impl() {
		deallocateExternal();
	}

	// Deallocate the External semaphore if any.
	void deallocateExternal()
	{
		if (external)
		{
			external->~External();
			external = nullptr;
		}
	}

	// Allocate the external semaphore.
	// Note that this does not allocate the internal resource, which must be
	// performed by calling external->init(), or importing one using
	// a platform-specific external->importXXX(...) method.
	void allocateExternalNoInit()
	{
		external = new (externalStorage) External();
	}

	void wait()
	{
		if (external)
		{
			if (!external->tryWait())
			{
				// Dispatch the external wait to a background thread.
				// Even if this creates a new thread on each
				// call, it is assumed that this is negligible
				// compared with the actual semaphore wait()
				// operation.
				marl::blocking_call([this](){
					external->wait();
				});
			}

			// If the import was temporary, reset the semaphore to its
			// permanent state by getting rid of |external|.
			// See "6.4.5. Importing Semaphore Payloads" in Vulkan 1.1 spec.
			if (temporaryImport)
			{
				deallocateExternal();
				temporaryImport = false;
			}
		}
		else
		{
			waitInternal();
		}
	}

	void signal()
	{
		if (external)
		{
			// Assumes that signalling an external semaphore is non-blocking,
			// so it can be performed directly either from a fiber or thread.
			external->signal();
		}
		else
		{
			signalInternal();
		}
	}

private:
	// Necessary to make ::importXXX() and ::exportXXX() simpler.
	friend Semaphore;

	void waitInternal()
	{
		// Wait on the marl condition variable only.
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this]{ return this->signaled; });
		signaled = false;  // Vulkan requires resetting after waiting.
	}

	void signalInternal()
	{
		// Signal the marl condition variable only.
		std::unique_lock<std::mutex> lock(mutex);
		if (!signaled)
		{
			signaled = true;
			condition.notify_one();
		}
	}

	// Implementation of a non-external semaphore based on Marl.
	std::mutex mutex;
	marl::ConditionVariable condition;
	bool signaled = false;

	// Optional external semaphore data might be referenced and stored here.
	External* external = nullptr;

	// Set to true if |external| comes from a temporary import.
	bool temporaryImport = false;

	alignas(External) char externalStorage[sizeof(External)];
};

Semaphore::Semaphore(const VkSemaphoreCreateInfo* pCreateInfo, void* mem)
{
	impl = new (mem) Impl(pCreateInfo);
}

void Semaphore::destroy(const VkAllocationCallbacks* pAllocator)
{
	impl->~Impl();
	vk::deallocate(impl, pAllocator);
}

size_t Semaphore::ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo* pCreateInfo)
{
	return sizeof(Semaphore::Impl);
}

void Semaphore::wait()
{
	impl->wait();
}

void Semaphore::signal()
{
	impl->signal();
}

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_LINUX_MEMFD
VkResult Semaphore::importFd(int fd, bool temporaryImport)
{
	std::unique_lock<std::mutex> lock(impl->mutex);
	if (!impl->external)
	{
		impl->allocateExternalNoInit();
	}
	VkResult result = impl->external->importFd(fd);
	if (result != VK_SUCCESS)
	{
		impl->deallocateExternal();
	}
	else
	{
		impl->temporaryImport = temporaryImport;
	}
	return result;
}

VkResult Semaphore::exportFd(int* pFd) const
{
	std::unique_lock<std::mutex> lock(impl->mutex);
	if (!impl->external)
	{
		TRACE("Cannot export non-external semaphore");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	return impl->external->exportFd(pFd);
}
#endif  // SWIFTSHADER_EXTERNAL_SEMAPHORE_LINUX_MEMFD

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_ZIRCON_EVENT
VkResult Semaphore::importHandle(zx_handle_t handle, bool temporaryImport)
{
	std::unique_lock<std::mutex> lock(impl->mutex);
	if (!impl->external)
	{
		impl->allocateExternalNoInit();
	}
	// NOTE: Imports are just moving a handle so cannot fail.
	impl->external->importHandle(handle);
	impl->temporaryImport = temporaryImport;
	return VK_SUCCESS;
}

VkResult Semaphore::exportHandle(zx_handle_t *pHandle) const
{
	std::unique_lock<std::mutex> lock(impl->mutex);
	if (!impl->external)
	{
		TRACE("Cannot export non-external semaphore");
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	}
	return impl->external->exportHandle(pHandle);
}
#endif  // SWIFTSHADER_EXTERNAL_SEMAPHORE_ZIRCON_EVENT

}  // namespace vk
