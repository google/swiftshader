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

#ifndef VK_COMMAND_BUFFER_HPP_
#define VK_COMMAND_BUFFER_HPP_

#include "VkConfig.h"
#include "VkObject.hpp"

namespace vk
{

class CommandBuffer
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_OBJECT; }

	CommandBuffer(VkCommandBufferLevel pLevel);

private:
	VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	VkPipeline pipelines[VK_PIPELINE_BIND_POINT_RANGE_SIZE];
};

using DispatchableCommandBuffer = DispatchableObject<CommandBuffer, VkCommandBuffer>;

static inline CommandBuffer* Cast(VkCommandBuffer object)
{
	return DispatchableCommandBuffer::Cast(object);
}

} // namespace vk

#endif // VK_COMMAND_BUFFER_HPP_
