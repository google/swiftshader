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

#ifndef VK_PIPELINE_LAYOUT_HPP_
#define VK_PIPELINE_LAYOUT_HPP_

#include "VkDescriptorSetLayout.hpp"

namespace vk {

class PipelineLayout : public Object<PipelineLayout, VkPipelineLayout>
{
public:
	PipelineLayout(const VkPipelineLayoutCreateInfo *pCreateInfo, void *mem);
	void destroy(const VkAllocationCallbacks *pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkPipelineLayoutCreateInfo *pCreateInfo);

	size_t getDescriptorSetCount() const;
	uint32_t getDynamicDescriptorCount(uint32_t setNumber) const;

	// Returns the index into the pipeline's dynamic offsets array for
	// the given descriptor set (and binding number).
	uint32_t getDynamicOffsetBaseIndex(uint32_t setNumber) const;
	uint32_t getDynamicOffsetIndex(uint32_t setNumber, uint32_t bindingNumber) const;

	uint32_t getBindingOffset(uint32_t setNumber, uint32_t bindingNumber) const;
	VkDescriptorType getDescriptorType(uint32_t setNumber, uint32_t bindingNumber) const;
	uint32_t getDescriptorSize(uint32_t setNumber, uint32_t bindingNumber) const;
	bool isDescriptorDynamic(uint32_t setNumber, uint32_t bindingNumber) const;

	const uint32_t identifier;

private:
	DescriptorSetLayout const *getDescriptorSetLayout(size_t descriptorSet) const;

	const uint32_t descriptorSetCount = 0;
	const DescriptorSetLayout **descriptorSetLayouts = nullptr;
	const uint32_t pushConstantRangeCount = 0;
	VkPushConstantRange *pushConstantRanges = nullptr;
	uint32_t *dynamicOffsetBaseIndices = nullptr;  // Base index per descriptor set for dynamic buffer offsets.
};

static inline PipelineLayout *Cast(VkPipelineLayout object)
{
	return PipelineLayout::Cast(object);
}

}  // namespace vk

#endif  // VK_PIPELINE_LAYOUT_HPP_
