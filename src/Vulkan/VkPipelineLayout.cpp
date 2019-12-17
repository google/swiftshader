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
#include <cstring>

namespace vk {

PipelineLayout::PipelineLayout(const VkPipelineLayoutCreateInfo *pCreateInfo, void *mem)
    : setLayoutCount(pCreateInfo->setLayoutCount)
    , pushConstantRangeCount(pCreateInfo->pushConstantRangeCount)
{
	char *hostMem = reinterpret_cast<char *>(mem);

	size_t setLayoutsSize = pCreateInfo->setLayoutCount * sizeof(DescriptorSetLayout *);
	setLayouts = reinterpret_cast<DescriptorSetLayout **>(hostMem);
	for(uint32_t i = 0; i < pCreateInfo->setLayoutCount; i++)
	{
		setLayouts[i] = vk::Cast(pCreateInfo->pSetLayouts[i]);
	}
	hostMem += setLayoutsSize;

	size_t pushConstantRangesSize = pCreateInfo->pushConstantRangeCount * sizeof(VkPushConstantRange);
	pushConstantRanges = reinterpret_cast<VkPushConstantRange *>(hostMem);
	memcpy(pushConstantRanges, pCreateInfo->pPushConstantRanges, pushConstantRangesSize);
	hostMem += pushConstantRangesSize;

	dynamicOffsetBases = reinterpret_cast<uint32_t *>(hostMem);
	uint32_t dynamicOffsetBase = 0;
	for(uint32_t i = 0; i < setLayoutCount; i++)
	{
		uint32_t dynamicDescriptorCount = setLayouts[i]->getDynamicDescriptorCount();
		ASSERT_OR_RETURN((dynamicOffsetBase + dynamicDescriptorCount) <= MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC);
		dynamicOffsetBases[i] = dynamicOffsetBase;
		dynamicOffsetBase += dynamicDescriptorCount;
	}
}

void PipelineLayout::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(setLayouts, pAllocator);  // pushConstantRanges are in the same allocation
}

size_t PipelineLayout::ComputeRequiredAllocationSize(const VkPipelineLayoutCreateInfo *pCreateInfo)
{
	return (pCreateInfo->setLayoutCount * sizeof(DescriptorSetLayout *)) +
	       (pCreateInfo->pushConstantRangeCount * sizeof(VkPushConstantRange)) +
	       (pCreateInfo->setLayoutCount * sizeof(uint32_t));  // dynamicOffsetBases
}

size_t PipelineLayout::getNumDescriptorSets() const
{
	return setLayoutCount;
}

DescriptorSetLayout const *PipelineLayout::getDescriptorSetLayout(size_t descriptorSet) const
{
	ASSERT(descriptorSet < setLayoutCount);
	return setLayouts[descriptorSet];
}

uint32_t PipelineLayout::getDynamicOffsetBase(size_t descriptorSet) const
{
	ASSERT(descriptorSet < setLayoutCount);
	return dynamicOffsetBases[descriptorSet];
}

}  // namespace vk
