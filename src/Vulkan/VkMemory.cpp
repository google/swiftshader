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

#include "VkMemory.hpp"

#include "VkConfig.hpp"
#include "System/Memory.hpp"

namespace vk {

void *allocateDeviceMemory(size_t count, size_t alignment)
{
	// TODO(b/140991626): Use allocateZeroOrPoison() instead of allocateZero() to detect MemorySanitizer errors.
	// TODO(b/140991626): Use allocateUninitialized() instead of allocateZeroOrPoison() to improve startup peformance.
	return sw::allocateZero(count, alignment);
}

void deallocateDeviceMemory(void *ptr)
{
	sw::deallocate(ptr);
}

void *allocate(size_t count, size_t alignment, const VkAllocationCallbacks *pAllocator, VkSystemAllocationScope allocationScope)
{
	// TODO(b/140991626): Use allocateZeroOrPoison() instead of allocateZero() to detect MemorySanitizer errors.
	// TODO(b/140991626): Use allocateUninitialized() instead of allocateZeroOrPoison() to improve startup peformance.
	return pAllocator ? pAllocator->pfnAllocation(pAllocator->pUserData, count, alignment, allocationScope)
	                  : sw::allocateZero(count, alignment);
}

void deallocate(void *ptr, const VkAllocationCallbacks *pAllocator)
{
	pAllocator ? pAllocator->pfnFree(pAllocator->pUserData, ptr) : sw::deallocate(ptr);
}

}  // namespace vk

#endif  // VK_OBJECT_HPP_
