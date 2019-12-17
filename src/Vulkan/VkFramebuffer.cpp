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
#include <memory.h>
#include <algorithm>

namespace vk {

Framebuffer::Framebuffer(const VkFramebufferCreateInfo *pCreateInfo, void *mem)
    : attachmentCount(pCreateInfo->attachmentCount)
    , attachments(reinterpret_cast<ImageView **>(mem))
    , extent{ pCreateInfo->width, pCreateInfo->height, pCreateInfo->layers }
{
	for(uint32_t i = 0; i < attachmentCount; i++)
	{
		attachments[i] = vk::Cast(pCreateInfo->pAttachments[i]);
	}
}

void Framebuffer::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(attachments, pAllocator);
}

void Framebuffer::clear(const RenderPass *renderPass, uint32_t clearValueCount, const VkClearValue *pClearValues, const VkRect2D &renderArea)
{
	ASSERT(attachmentCount == renderPass->getAttachmentCount());

	const uint32_t count = std::min(clearValueCount, attachmentCount);
	for(uint32_t i = 0; i < count; i++)
	{
		const VkAttachmentDescription attachment = renderPass->getAttachment(i);

		VkImageAspectFlags aspectMask = Format(attachment.format).getAspects();
		if(attachment.loadOp != VK_ATTACHMENT_LOAD_OP_CLEAR)
			aspectMask &= VK_IMAGE_ASPECT_STENCIL_BIT;
		if(attachment.stencilLoadOp != VK_ATTACHMENT_LOAD_OP_CLEAR)
			aspectMask &= ~VK_IMAGE_ASPECT_STENCIL_BIT;

		if(!aspectMask || !renderPass->isAttachmentUsed(i))
		{
			continue;
		}

		if(renderPass->isMultiView())
		{
			attachments[i]->clearWithLayerMask(pClearValues[i], aspectMask, renderArea,
			                                   renderPass->getAttachmentViewMask(i));
		}
		else
		{
			attachments[i]->clear(pClearValues[i], aspectMask, renderArea);
		}
	}
}

void Framebuffer::clearAttachment(const RenderPass *renderPass, uint32_t subpassIndex, const VkClearAttachment &attachment, const VkClearRect &rect)
{
	VkSubpassDescription subpass = renderPass->getSubpass(subpassIndex);

	if(attachment.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		ASSERT(attachment.colorAttachment < subpass.colorAttachmentCount);
		uint32_t attachmentIndex = subpass.pColorAttachments[attachment.colorAttachment].attachment;

		if(attachmentIndex != VK_ATTACHMENT_UNUSED)
		{
			ASSERT(attachmentIndex < attachmentCount);
			ImageView *imageView = attachments[attachmentIndex];

			if(renderPass->isMultiView())
			{
				imageView->clearWithLayerMask(attachment.clearValue, attachment.aspectMask, rect.rect,
				                              renderPass->getViewMask(subpassIndex));
			}
			else
			{
				imageView->clear(attachment.clearValue, attachment.aspectMask, rect);
			}
		}
	}
	else if(attachment.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		uint32_t attachmentIndex = subpass.pDepthStencilAttachment->attachment;

		if(attachmentIndex != VK_ATTACHMENT_UNUSED)
		{
			ASSERT(attachmentIndex < attachmentCount);
			ImageView *imageView = attachments[attachmentIndex];

			if(renderPass->isMultiView())
			{
				imageView->clearWithLayerMask(attachment.clearValue, attachment.aspectMask, rect.rect,
				                              renderPass->getViewMask(subpassIndex));
			}
			else
			{
				imageView->clear(attachment.clearValue, attachment.aspectMask, rect);
			}
		}
	}
}

ImageView *Framebuffer::getAttachment(uint32_t index) const
{
	return attachments[index];
}

void Framebuffer::resolve(const RenderPass *renderPass, uint32_t subpassIndex)
{
	auto const &subpass = renderPass->getSubpass(subpassIndex);
	if(subpass.pResolveAttachments)
	{
		for(uint32_t i = 0; i < subpass.colorAttachmentCount; i++)
		{
			uint32_t resolveAttachment = subpass.pResolveAttachments[i].attachment;
			if(resolveAttachment != VK_ATTACHMENT_UNUSED)
			{
				ImageView *imageView = attachments[subpass.pColorAttachments[i].attachment];
				if(renderPass->isMultiView())
				{
					imageView->resolveWithLayerMask(attachments[resolveAttachment],
					                                renderPass->getViewMask(subpassIndex));
				}
				else
				{
					imageView->resolve(attachments[resolveAttachment]);
				}
			}
		}
	}
}

size_t Framebuffer::ComputeRequiredAllocationSize(const VkFramebufferCreateInfo *pCreateInfo)
{
	return pCreateInfo->attachmentCount * sizeof(void *);
}

}  // namespace vk
