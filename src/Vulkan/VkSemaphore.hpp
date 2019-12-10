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

#ifndef VK_SEMAPHORE_HPP_
#define VK_SEMAPHORE_HPP_

#include "VkConfig.h"
#include "VkObject.hpp"

#include "marl/event.h"
#include <mutex>

#if VK_USE_PLATFORM_FUCHSIA
#	include <zircon/types.h>
#endif

namespace vk {

class Semaphore : public Object<Semaphore, VkSemaphore>
{
public:
	Semaphore(const VkSemaphoreCreateInfo *pCreateInfo, void *mem, const VkAllocationCallbacks *pAllocator);
	void destroy(const VkAllocationCallbacks *pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo *pCreateInfo);

	void wait();

	void wait(const VkPipelineStageFlags &flag)
	{
		// NOTE: not sure what else to do here?
		wait();
	}

	void signal();

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_OPAQUE_FD
	VkResult importFd(int fd, bool temporaryImport);
	VkResult exportFd(int *pFd);
#endif

#if VK_USE_PLATFORM_FUCHSIA
	VkResult importHandle(zx_handle_t handle, bool temporaryImport);
	VkResult exportHandle(zx_handle_t *pHandle);
#endif

	class External;

private:
	void allocateExternal();
	void deallocateExternal();

	const VkAllocationCallbacks *allocator = nullptr;
	marl::Event internal;
	std::mutex mutex;
	External *external = nullptr;
	bool temporaryImport = false;
};

static inline Semaphore *Cast(VkSemaphore object)
{
	return Semaphore::Cast(object);
}

}  // namespace vk

#endif  // VK_SEMAPHORE_HPP_
