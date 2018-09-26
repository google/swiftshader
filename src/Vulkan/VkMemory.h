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

#include "System/Memory.hpp"
#include <vulkan/vulkan.h>

namespace vk
{

void* allocate(size_t count, size_t alignment, const VkAllocationCallbacks* pAllocator,
	           VkSystemAllocationScope allocationScope = VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
void deallocate(void* ptr, const VkAllocationCallbacks* pAllocator);

template <typename T>
T* allocate(size_t count, const VkAllocationCallbacks* pAllocator)
{
	return reinterpret_cast<T*>(allocate(count, alignof(T), pAllocator, T::GetAllocationScope()));
}

// Because Vulkan uses optional allocation callbacks, we use them in a custom
// placement new operator in the VkObjectBase class for simplicity.
// Unfortunately, since we use a placement new to allocate VkObjectBase derived
// classes objects, the corresponding deletion operator is a placement delete,
// which does nothing. In order to properly dispose of these objects' memory,
// we use this function, which calls the proper T:destroy() function
// prior to releasing the object (by default, VkObjectBase::destroy does nothing).
template<typename VkT>
inline void destroy(VkT vkObject, const VkAllocationCallbacks* pAllocator)
{
	auto object = Cast(vkObject);
	if(object)
	{
		object->destroy(pAllocator);
		// object may not point to the same pointer as vkObject, for dispatchable objects,
		// for example, so make sure to deallocate based on the vkObject pointer, which
		// should always point to the beginning of the allocated memory
		vk::deallocate(vkObject, pAllocator);
	}
}

} // namespace vk

#endif // VK_MEMORY_HPP_