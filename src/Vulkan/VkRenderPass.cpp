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

#include "VkRenderPass.hpp"

namespace vk
{

RenderPass::RenderPass(const VkRenderPassCreateInfo* pCreateInfo, void* mem)
{
}

void RenderPass::destroy(const VkAllocationCallbacks* pAllocator)
{
}

size_t RenderPass::ComputeRequiredAllocationSize(const VkRenderPassCreateInfo* pCreateInfo)
{
	return 0;
}

void RenderPass::begin()
{
	// FIXME (b/119620965): noop
}

void RenderPass::end()
{
	// FIXME (b/119620965): noop
}

} // namespace vk