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

#ifndef VULKAN_PLATFORM
#define VULKAN_PLATFORM

#include <cstddef>
#include <cstdint>

template<typename T> class VkNonDispatchableHandle
{
public:
	VkNonDispatchableHandle(uint64_t h) : handle(h)
	{
		static_assert(sizeof(VkNonDispatchableHandle) == sizeof(uint64_t), "Size is not 64 bits!");
	}

	void* get() const
	{
		return reinterpret_cast<void*>(static_cast<uintptr_t>(handle));
	}

	operator void*() const
	{
		return get();
	}

private:
	uint64_t handle;
};

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) \
	typedef struct object##_T *object##Ptr; \
	typedef VkNonDispatchableHandle<object##Ptr> object; \
    template class VkNonDispatchableHandle<object##Ptr>;

#include <vulkan/vulkan.h>

#ifdef Bool
#undef Bool // b/127920555
#undef None
#endif

#endif // VULKAN_PLATFORM
