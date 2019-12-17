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

	size_t getNumDescriptorSets() const;
	DescriptorSetLayout const *getDescriptorSetLayout(size_t descriptorSet) const;

	// Returns the starting index into the pipeline's dynamic offsets array for
	// the given descriptor set.
	uint32_t getDynamicOffsetBase(size_t descriptorSet) const;

private:
	uint32_t setLayoutCount = 0;
	DescriptorSetLayout **setLayouts = nullptr;
	uint32_t pushConstantRangeCount = 0;
	VkPushConstantRange *pushConstantRanges = nullptr;
	uint32_t *dynamicOffsetBases = nullptr;  // Base offset per set layout.
};

static inline PipelineLayout *Cast(VkPipelineLayout object)
{
	return PipelineLayout::Cast(object);
}

}  // namespace vk

#endif  // VK_PIPELINE_LAYOUT_HPP_
