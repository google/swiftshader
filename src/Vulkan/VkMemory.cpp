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

#ifndef VK_OBJECT_HPP_
#define VK_OBJECT_HPP_

#include "VkMemory.h"
#include "VkConfig.h"
#include "System/Memory.hpp"

namespace vk {

void *allocate(size_t count, size_t alignment, const VkAllocationCallbacks *pAllocator, VkSystemAllocationScope allocationScope)
{
	return pAllocator ? pAllocator->pfnAllocation(pAllocator->pUserData, count, alignment, allocationScope) : sw::allocate(count, alignment);
}

void deallocate(void *ptr, const VkAllocationCallbacks *pAllocator)
{
	pAllocator ? pAllocator->pfnFree(pAllocator->pUserData, ptr) : sw::deallocate(ptr);
}

}  // namespace vk

#endif  // VK_OBJECT_HPP_
