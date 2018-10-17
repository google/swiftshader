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

#include "VkConfig.h"
#include "VkDebug.hpp"
#include "VkMemory.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_icd.h>

namespace vk
{
// For use in the placement new to make it verbose that we're allocating an object using device memory
static constexpr VkAllocationCallbacks* DEVICE_MEMORY = nullptr;

template<typename T, typename VkT, typename CreateInfo>
static VkResult Create(const VkAllocationCallbacks* pAllocator, const CreateInfo* pCreateInfo, VkT* outObject)
{
	*outObject = VK_NULL_HANDLE;

	size_t size = T::ComputeRequiredAllocationSize(pCreateInfo);
	void* memory = nullptr;
	if(size)
	{
		memory = vk::allocate(size, REQUIRED_MEMORY_ALIGNMENT, pAllocator, T::GetAllocationScope());
		if(!memory)
		{
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}
	}

	auto object = new (pAllocator) T(pCreateInfo, memory);

	if(!object)
	{
		vk::deallocate(memory, pAllocator);
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	*outObject = *object;

	return VK_SUCCESS;
}

template<typename T, typename VkT>
class ObjectBase
{
public:
	using VkType = VkT;

	void destroy(const VkAllocationCallbacks* pAllocator) {} // Method defined by objects to delete their content, if necessary

	void* operator new(size_t count, const VkAllocationCallbacks* pAllocator)
	{
		return vk::allocate(count, alignof(T), pAllocator, T::GetAllocationScope());
	}

	void operator delete(void* ptr, const VkAllocationCallbacks* pAllocator)
	{
		// Should never happen
		ASSERT(false);
	}

	template<typename CreateInfo>
	static VkResult Create(const VkAllocationCallbacks* pAllocator, const CreateInfo* pCreateInfo, VkT* outObject)
	{
		return vk::Create<T, VkT, CreateInfo>(pAllocator, pCreateInfo, outObject);
	}

	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_OBJECT; }

protected:
	// All derived classes should have deleted destructors
	~ObjectBase() {}
};

template<typename T, typename VkT>
class Object : public ObjectBase<T, VkT>
{
public:
	operator VkT()
	{
		return reinterpret_cast<VkT>(this);
	}
};

template<typename T, typename VkT>
class DispatchableObject
{
	VK_LOADER_DATA loaderData = { ICD_LOADER_MAGIC };

	T object;
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return T::GetAllocationScope(); }

	template<typename ...Args>
	DispatchableObject(Args... args) : object(args...)
	{
	}

	~DispatchableObject() = delete;

	void destroy(const VkAllocationCallbacks* pAllocator)
	{
		object.destroy(pAllocator);
	}

	void* operator new(size_t count, const VkAllocationCallbacks* pAllocator)
	{
		return vk::allocate(count, alignof(T), pAllocator, T::GetAllocationScope());
	}

	void operator delete(void* ptr, const VkAllocationCallbacks* pAllocator)
	{
		// Should never happen
		ASSERT(false);
	}

	template<typename CreateInfo>
	static VkResult Create(const VkAllocationCallbacks* pAllocator, const CreateInfo* pCreateInfo, VkT* outObject)
	{
		return vk::Create<DispatchableObject<T, VkT>, VkT, CreateInfo>(pAllocator, pCreateInfo, outObject);
	}

	template<typename CreateInfo>
	static size_t ComputeRequiredAllocationSize(const CreateInfo* pCreateInfo)
	{
		return T::ComputeRequiredAllocationSize(pCreateInfo);
	}

	static inline T* Cast(VkT vkObject)
	{
		return &(reinterpret_cast<DispatchableObject<T, VkT>*>(vkObject)->object);
	}

	operator VkT()
	{
		return reinterpret_cast<VkT>(this);
	}
};

} // namespace vk

#endif // VK_OBJECT_HPP_
