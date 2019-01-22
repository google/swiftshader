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

class DescriptorSetLayout : public Object<DescriptorSetLayout, VkDescriptorSetLayout>
{
public:
	DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, void* mem);
	~DescriptorSetLayout() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkDescriptorSetLayoutCreateInfo* pCreateInfo);

	static size_t GetDescriptorSize(VkDescriptorType type);

	size_t getSize() const;

private:
	VkDescriptorSetLayoutCreateFlags flags;
	uint32_t                         bindingCount;
	VkDescriptorSetLayoutBinding*    bindings;
};

static inline DescriptorSetLayout* Cast(VkDescriptorSetLayout object)
{
	return reinterpret_cast<DescriptorSetLayout*>(object);
}

} // namespace vk

#endif // VK_DESCRIPTOR_SET_LAYOUT_HPP_
