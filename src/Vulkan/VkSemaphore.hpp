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

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_ZIRCON_EVENT
#include <zircon/types.h>
#endif

namespace vk
{

class Semaphore : public Object<Semaphore, VkSemaphore>
{
public:
	Semaphore(const VkSemaphoreCreateInfo* pCreateInfo, void* mem);
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo* pCreateInfo);

	void wait();

	void wait(const VkPipelineStageFlags& flag)
	{
		// NOTE: not sure what else to do here?
		wait();
	}

	void signal();

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_LINUX_MEMFD
	VkResult importFd(int fd, bool temporaryImport);
	VkResult exportFd(int* pFd) const;
#endif

#if SWIFTSHADER_EXTERNAL_SEMAPHORE_ZIRCON_EVENT
	VkResult importHandle(zx_handle_t handle, bool temporaryImport);
	VkResult exportHandle(zx_handle_t *pHandle) const;
#endif

private:
	class External;
	class Impl;
	Impl* impl = nullptr;
};

static inline Semaphore* Cast(VkSemaphore object)
{
	return Semaphore::Cast(object);
}

} // namespace vk

#endif // VK_SEMAPHORE_HPP_
