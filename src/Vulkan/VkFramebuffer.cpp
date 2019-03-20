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

#include "VkFramebuffer.hpp"
#include "VkImageView.hpp"
#include "VkRenderPass.hpp"
#include <algorithm>
#include <memory.h>

namespace vk
{

Framebuffer::Framebuffer(const VkFramebufferCreateInfo* pCreateInfo, void* mem) :
	attachmentCount(pCreateInfo->attachmentCount),
	attachments(reinterpret_cast<ImageView**>(mem))
{
	for(uint32_t i = 0; i < attachmentCount; i++)
	{
		attachments[i] = Cast(pCreateInfo->pAttachments[i]);
	}
}

void Framebuffer::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::deallocate(attachments, pAllocator);
}

void Framebuffer::clear(const RenderPass* renderPass, uint32_t clearValueCount, const VkClearValue* pClearValues, const VkRect2D& renderArea)
{
	ASSERT(attachmentCount == renderPass->getAttachmentCount());

	const uint32_t count = std::min(clearValueCount, attachmentCount);
	for(uint32_t i = 0; i < count; i++)
	{
		const VkAttachmentDescription attachment = renderPass->getAttachment(i);
		const Format format(attachment.format);
		bool isDepth = format.isDepth();
		bool isStencil = format.isStencil();

		if(isDepth || isStencil)
		{
			bool clearDepth = (isDepth && (attachment.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR));
			bool clearStencil = (isStencil && (attachment.stencilLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR));

			if(clearDepth || clearStencil)
			{
				attachments[i]->clear(pClearValues[i],
				                      (clearDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0) |
				                      (clearStencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0),
				                      renderArea);
			}
		}
		else if(attachment.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
		{
			attachments[i]->clear(pClearValues[i], VK_IMAGE_ASPECT_COLOR_BIT, renderArea);
		}
	}
}

void Framebuffer::clear(const RenderPass* renderPass, const VkClearAttachment& attachment, const VkClearRect& rect)
{
	if(attachment.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		if(attachment.colorAttachment != VK_ATTACHMENT_UNUSED)
		{
			VkSubpassDescription subpass = renderPass->getCurrentSubpass();

			ASSERT(attachment.colorAttachment < subpass.colorAttachmentCount);
			ASSERT(subpass.pColorAttachments[attachment.colorAttachment].attachment < attachmentCount);

			attachments[subpass.pColorAttachments[attachment.colorAttachment].attachment]->clear(
				attachment.clearValue, attachment.aspectMask, rect);
		}
	}
	else if(attachment.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		VkSubpassDescription subpass = renderPass->getCurrentSubpass();

		ASSERT(subpass.pDepthStencilAttachment->attachment < attachmentCount);

		attachments[subpass.pDepthStencilAttachment->attachment]->clear(attachment.clearValue, attachment.aspectMask, rect);
	}
}

ImageView *Framebuffer::getAttachment(uint32_t index) const
{
	return attachments[index];
}

void Framebuffer::resolve(const RenderPass* renderPass)
{
	VkSubpassDescription subpass = renderPass->getCurrentSubpass();
	if(subpass.pResolveAttachments)
	{
		for(uint32_t i = 0; i < subpass.colorAttachmentCount; i++)
		{
			uint32_t resolveAttachment = subpass.pResolveAttachments[i].attachment;
			if(resolveAttachment != VK_ATTACHMENT_UNUSED)
			{
				attachments[subpass.pColorAttachments[i].attachment]->resolve(attachments[resolveAttachment]);
			}
		}
	}
}

size_t Framebuffer::ComputeRequiredAllocationSize(const VkFramebufferCreateInfo* pCreateInfo)
{
	return pCreateInfo->attachmentCount * sizeof(void*);
}

} // namespace vk