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

#ifndef VK_DESCRIPTOR_SET_LAYOUT_HPP_
#define VK_DESCRIPTOR_SET_LAYOUT_HPP_

#include "VkObject.hpp"

namespace vk
{

class DescriptorSetLayout;

struct DescriptorSet
{
	vk::DescriptorSetLayout* layout;
	uint8_t data[];
};

inline DescriptorSet* Cast(VkDescriptorSet object)
{
	return reinterpret_cast<DescriptorSet*>(object);
}

class DescriptorSetLayout : public Object<DescriptorSetLayout, VkDescriptorSetLayout>
{
public:
	DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, void* mem);
	~DescriptorSetLayout() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkDescriptorSetLayoutCreateInfo* pCreateInfo);

	static size_t GetDescriptorSize(VkDescriptorType type);
	static void WriteDescriptorSet(const VkWriteDescriptorSet& descriptorWrites);
	static void CopyDescriptorSet(const VkCopyDescriptorSet& descriptorCopies);

	void initialize(VkDescriptorSet descriptorSet);
	size_t getDescriptorSetAllocationSize() const;

	size_t getBindingOffset(uint32_t binding) const;
	uint8_t* getOffsetPointer(DescriptorSet *descriptorSet, uint32_t binding, uint32_t arrayElement, uint32_t count, size_t* typeSize) const;

private:
	size_t getDescriptorSetDataSize() const;
	uint32_t getBindingIndex(uint32_t binding) const;
	static const uint8_t* GetInputData(const VkWriteDescriptorSet& descriptorWrites);

	VkDescriptorSetLayoutCreateFlags flags;
	uint32_t                         bindingCount;
	VkDescriptorSetLayoutBinding*    bindings;
	size_t*                          bindingOffsets;
};

static inline DescriptorSetLayout* Cast(VkDescriptorSetLayout object)
{
	return reinterpret_cast<DescriptorSetLayout*>(object);
}

} // namespace vk

#endif // VK_DESCRIPTOR_SET_LAYOUT_HPP_
