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

#ifndef VK_RENDER_PASS_HPP_
#define VK_RENDER_PASS_HPP_

#include "VkObject.hpp"

#include <vector>

namespace vk
{

class RenderPass : public Object<RenderPass, VkRenderPass>
{
public:
	RenderPass(const VkRenderPassCreateInfo* pCreateInfo, void* mem);
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkRenderPassCreateInfo* pCreateInfo);

	void getRenderAreaGranularity(VkExtent2D* pGranularity) const;

	void begin();
	void nextSubpass();
	void end();

	uint32_t getAttachmentCount() const
	{
		return attachmentCount;
	}

	VkAttachmentDescription getAttachment(uint32_t i) const
	{
		return attachments[i];
	}

	uint32_t getSubpassCount() const
	{
		return subpassCount;
	}

	VkSubpassDescription getSubpass(uint32_t i) const
	{
		return subpasses[i];
	}

	VkSubpassDescription getCurrentSubpass() const
	{
		return subpasses[currentSubpass];
	}

	uint32_t getDependencyCount() const
	{
		return dependencyCount;
	}

	VkSubpassDependency getDependency(uint32_t i) const
	{
		return dependencies[i];
	}

	bool isAttachmentUsed(uint32_t i) const
	{
		return attachmentFirstUse[i] >= 0;
	}

private:
	uint32_t                 attachmentCount = 0;
	VkAttachmentDescription* attachments = nullptr;
	uint32_t                 subpassCount = 0;
	VkSubpassDescription*    subpasses = nullptr;
	uint32_t                 dependencyCount = 0;
	VkSubpassDependency*     dependencies = nullptr;
	uint32_t                 currentSubpass = 0;
	int*                     attachmentFirstUse = nullptr;
};

static inline RenderPass* Cast(VkRenderPass object)
{
	return RenderPass::Cast(object);
}

} // namespace vk

#endif // VK_RENDER_PASS_HPP_