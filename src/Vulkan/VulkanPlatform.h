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

template<typename HandleType> class VkHandle
{
public:
	VkHandle(HandleType handle)
	{
		u.dummy = 0;
		u.handle = handle;
	}

	HandleType get() const
	{
		return u.handle;
	}

	operator HandleType() const
	{
		return u.handle;
	}

protected:
	HandleType set(HandleType handle)
	{
		return (u.handle = handle);
	}

private:
	union PointerHandleUnion
	{
		HandleType handle;
		uint64_t dummy; // VkNonDispatchableHandle's size must always be 64 bits even when void* is 32 bits
	};
	PointerHandleUnion u;
};

template<typename T> class VkNonDispatchableHandleBase : public VkHandle<T>
{
public:
	using HandleType = T;

	VkNonDispatchableHandleBase(HandleType handle) : VkHandle<T>(handle)
	{
	}

	void operator=(HandleType handle)
	{
		this->set(handle);
	}
};

// VkDescriptorSet objects are really just memory in the VkDescriptorPool
// object, so define different/more convenient operators for this object.
struct VkDescriptorSet_T;
template<> class VkNonDispatchableHandleBase<VkDescriptorSet_T*> : public VkHandle<uint8_t*>
{
public:
	using HandleType = uint8_t*;

	VkNonDispatchableHandleBase(HandleType handle) : VkHandle<uint8_t*>(handle)
	{
	}

	HandleType operator+(ptrdiff_t rhs) const
	{
		return get() + rhs;
	}

	HandleType operator+=(ptrdiff_t rhs)
	{
		return this->set(get() + rhs);
	}

	ptrdiff_t operator-(const HandleType rhs) const
	{
		return get() - rhs;
	}
};

template<typename T> class VkNonDispatchableHandle : public VkNonDispatchableHandleBase<T>
{
public:
	VkNonDispatchableHandle() : VkNonDispatchableHandleBase<T>(nullptr)
	{
	}

	VkNonDispatchableHandle(typename VkNonDispatchableHandleBase<T>::HandleType handle) : VkNonDispatchableHandleBase<T>(handle)
	{
		static_assert(sizeof(VkNonDispatchableHandle) == sizeof(uint64_t), "Size is not 64 bits!");
	}
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
