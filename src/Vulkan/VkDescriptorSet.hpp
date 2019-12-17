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

#ifndef VK_DESCRIPTOR_SET_HPP_
#define VK_DESCRIPTOR_SET_HPP_

// Intentionally not including VkObject.hpp here due to b/127920555

#include <array>
#include <memory>

namespace vk {

class DescriptorSetLayout;

struct alignas(16) DescriptorSetHeader
{
	DescriptorSetLayout *layout;
};

class alignas(16) DescriptorSet
{
public:
	static inline DescriptorSet *Cast(VkDescriptorSet object)
	{
		return static_cast<DescriptorSet *>(static_cast<void *>(object));
	}

	using Bindings = std::array<vk::DescriptorSet *, vk::MAX_BOUND_DESCRIPTOR_SETS>;
	using DynamicOffsets = std::array<uint32_t, vk::MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC>;

	DescriptorSetHeader header;
	alignas(16) uint8_t data[1];
};

inline DescriptorSet *Cast(VkDescriptorSet object)
{
	return DescriptorSet::Cast(object);
}

}  // namespace vk

#endif  // VK_DESCRIPTOR_SET_HPP_
