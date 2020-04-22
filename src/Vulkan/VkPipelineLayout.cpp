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

#include "VkPipelineLayout.hpp"

#include <atomic>
#include <cstring>

namespace vk {

static std::atomic<uint32_t> layoutIdentifierSerial = { 1 };  // Start at 1. 0 is invalid/void layout.

PipelineLayout::PipelineLayout(const VkPipelineLayoutCreateInfo *pCreateInfo, void *mem)
    : identifier(layoutIdentifierSerial++)
    , descriptorSetCount(pCreateInfo->setLayoutCount)
    , pushConstantRangeCount(pCreateInfo->pushConstantRangeCount)
{
	char *hostMem = reinterpret_cast<char *>(mem);

	size_t setLayoutsSize = pCreateInfo->setLayoutCount * sizeof(DescriptorSetLayout *);
	descriptorSetLayouts = reinterpret_cast<const DescriptorSetLayout **>(hostMem);
	for(uint32_t i = 0; i < pCreateInfo->setLayoutCount; i++)
	{
		descriptorSetLayouts[i] = vk::Cast(pCreateInfo->pSetLayouts[i]);
	}
	hostMem += setLayoutsSize;

	size_t pushConstantRangesSize = pCreateInfo->pushConstantRangeCount * sizeof(VkPushConstantRange);
	pushConstantRanges = reinterpret_cast<VkPushConstantRange *>(hostMem);
	memcpy(pushConstantRanges, pCreateInfo->pPushConstantRanges, pushConstantRangesSize);
	hostMem += pushConstantRangesSize;

	dynamicOffsetBaseIndices = reinterpret_cast<uint32_t *>(hostMem);
	uint32_t dynamicOffsetBase = 0;
	for(uint32_t i = 0; i < descriptorSetCount; i++)
	{
		uint32_t dynamicDescriptorCount = descriptorSetLayouts[i]->getDynamicDescriptorCount();
		ASSERT_OR_RETURN((dynamicOffsetBase + dynamicDescriptorCount) <= MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC);
		dynamicOffsetBaseIndices[i] = dynamicOffsetBase;
		dynamicOffsetBase += dynamicDescriptorCount;
	}
}

void PipelineLayout::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(descriptorSetLayouts, pAllocator);  // pushConstantRanges are in the same allocation
}

size_t PipelineLayout::ComputeRequiredAllocationSize(const VkPipelineLayoutCreateInfo *pCreateInfo)
{
	return (pCreateInfo->setLayoutCount * sizeof(DescriptorSetLayout *)) +        // descriptorSetLayouts[]
	       (pCreateInfo->pushConstantRangeCount * sizeof(VkPushConstantRange)) +  // pushConstantRanges[]
	       (pCreateInfo->setLayoutCount * sizeof(uint32_t));                      // dynamicOffsetBaseIndices[]
}

size_t PipelineLayout::getDescriptorSetCount() const
{
	return descriptorSetCount;
}

uint32_t PipelineLayout::getDynamicDescriptorCount(uint32_t setNumber) const
{
	return getDescriptorSetLayout(setNumber)->getDynamicDescriptorCount();
}

uint32_t PipelineLayout::getDynamicOffsetBaseIndex(uint32_t setNumber) const
{
	return dynamicOffsetBaseIndices[setNumber];
}

uint32_t PipelineLayout::getDynamicOffsetIndex(uint32_t setNumber, uint32_t bindingNumber) const
{
	return getDynamicOffsetBaseIndex(setNumber) + getDescriptorSetLayout(setNumber)->getDynamicOffsetIndex(bindingNumber);
}

uint32_t PipelineLayout::getBindingOffset(uint32_t setNumber, uint32_t bindingNumber) const
{
	return getDescriptorSetLayout(setNumber)->getBindingOffset(bindingNumber);
}

VkDescriptorType PipelineLayout::getDescriptorType(uint32_t setNumber, uint32_t bindingNumber) const
{
	return getDescriptorSetLayout(setNumber)->getDescriptorType(bindingNumber);
}

uint32_t PipelineLayout::getDescriptorSize(uint32_t setNumber, uint32_t bindingNumber) const
{
	return DescriptorSetLayout::GetDescriptorSize(getDescriptorType(setNumber, bindingNumber));
}

bool PipelineLayout::isDescriptorDynamic(uint32_t setNumber, uint32_t bindingNumber) const
{
	return DescriptorSetLayout::IsDescriptorDynamic(getDescriptorType(setNumber, bindingNumber));
}

DescriptorSetLayout const *PipelineLayout::getDescriptorSetLayout(size_t descriptorSet) const
{
	ASSERT(descriptorSet < descriptorSetCount);
	return descriptorSetLayouts[descriptorSet];
}

}  // namespace vk
