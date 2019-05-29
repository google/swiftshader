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

// We can't directly use alignas(uint64_t) because on some platforms a uint64_t
// has an alignment of 8 outside of a struct but inside it has an alignment of
// 4. We use this dummy struct to figure out the alignment of uint64_t inside a
// struct.
struct DummyUInt64Wrapper {
	uint64_t dummy;
};

static constexpr size_t kNativeVkHandleAlignment = alignof(DummyUInt64Wrapper);

template<typename HandleType> class alignas(kNativeVkHandleAlignment) VkWrapperBase
{
public:
	VkWrapperBase(HandleType handle)
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
		uint64_t dummy; // VkWrapper's size must always be 64 bits even when void* is 32 bits
	};
	PointerHandleUnion u;
};

template<typename T> class alignas(kNativeVkHandleAlignment) VkWrapper : public VkWrapperBase<T>
{
public:
	using HandleType = T;

	VkWrapper() : VkWrapperBase<T>(nullptr)
	{
	}

	VkWrapper(HandleType handle) : VkWrapperBase<T>(handle)
	{
		static_assert(sizeof(VkWrapper) == sizeof(uint64_t), "Size is not 64 bits!");
	}

	void operator=(HandleType handle)
	{
		this->set(handle);
	}
};

// VkDescriptorSet objects are really just memory in the VkDescriptorPool
// object, so define different/more convenient operators for this object.
struct VkDescriptorSet_T;
template<> class alignas(kNativeVkHandleAlignment) VkWrapper<VkDescriptorSet_T*> : public VkWrapperBase<uint8_t*>
{
public:
	using HandleType = uint8_t*;

	VkWrapper(HandleType handle) : VkWrapperBase<uint8_t*>(handle)
	{
		static_assert(sizeof(VkWrapper) == sizeof(uint64_t), "Size is not 64 bits!");
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

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) \
	typedef struct object##_T *object##Ptr; \
	typedef VkWrapper<object##Ptr> object;

#include <vulkan/vulkan.h>

#ifdef Bool
#undef Bool // b/127920555
#undef None
#endif

#endif // VULKAN_PLATFORM
