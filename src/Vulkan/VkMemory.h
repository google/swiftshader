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

#ifndef VK_MEMORY_HPP_
#define VK_MEMORY_HPP_

#include <Vulkan/VulkanPlatform.h>

namespace vk {

void* allocate(size_t count, size_t alignment, const VkAllocationCallbacks* pAllocator,
               VkSystemAllocationScope allocationScope = VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
void deallocate(void* ptr, const VkAllocationCallbacks* pAllocator);

template <typename T>
T* allocate(size_t count, const VkAllocationCallbacks* pAllocator)
{
	return static_cast<T*>(allocate(count, alignof(T), pAllocator, T::GetAllocationScope()));
}

}  // namespace vk

#endif // VK_MEMORY_HPP_
