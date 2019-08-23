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
#include <cstring>

namespace vk
{

RenderPass::RenderPass(const VkRenderPassCreateInfo* pCreateInfo, void* mem) :
	attachmentCount(pCreateInfo->attachmentCount),
	subpassCount(pCreateInfo->subpassCount),
	dependencyCount(pCreateInfo->dependencyCount)
{
	char* hostMemory = reinterpret_cast<char*>(mem);

	// subpassCount must be greater than 0
	ASSERT(pCreateInfo->subpassCount > 0);

	size_t subpassesSize = pCreateInfo->subpassCount * sizeof(VkSubpassDescription);
	subpasses = reinterpret_cast<VkSubpassDescription*>(hostMemory);
	memcpy(subpasses, pCreateInfo->pSubpasses, subpassesSize);
	hostMemory += subpassesSize;
	uint32_t *masks = reinterpret_cast<uint32_t *>(hostMemory);
	hostMemory += pCreateInfo->subpassCount * sizeof(uint32_t);

	if(pCreateInfo->attachmentCount > 0)
	{
		size_t attachmentSize = pCreateInfo->attachmentCount * sizeof(VkAttachmentDescription);
		attachments = reinterpret_cast<VkAttachmentDescription*>(hostMemory);
		memcpy(attachments, pCreateInfo->pAttachments, attachmentSize);
		hostMemory += attachmentSize;

		size_t firstUseSize = pCreateInfo->attachmentCount * sizeof(int);
		attachmentFirstUse = reinterpret_cast<int*>(hostMemory);
		hostMemory += firstUseSize;

		attachmentViewMasks = reinterpret_cast<uint32_t *>(hostMemory);
		hostMemory += pCreateInfo->attachmentCount * sizeof(uint32_t);
		for (auto i = 0u; i < pCreateInfo->attachmentCount; i++)
		{
			attachmentFirstUse[i] = -1;
			attachmentViewMasks[i] = 0;
		}
	}

	const VkBaseInStructure* extensionCreateInfo = reinterpret_cast<const VkBaseInStructure*>(pCreateInfo->pNext);
	while (extensionCreateInfo)
	{
		switch (extensionCreateInfo->sType)
		{
		case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO:
		{
			// Renderpass uses multiview if this structure is present AND some subpass specifies
			// a nonzero view mask
			auto const *multiviewCreateInfo = reinterpret_cast<VkRenderPassMultiviewCreateInfo const *>(extensionCreateInfo);
			for (auto i = 0u; i < pCreateInfo->subpassCount; i++)
			{
				masks[i] = multiviewCreateInfo->pViewMasks[i];
				// This is now a multiview renderpass, so make the masks available
				if (masks[i])
					viewMasks = masks;
			}

			break;
		}
		default:
			/* Unknown structure in pNext chain must be ignored */
			break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	// Deep copy subpasses
	for(uint32_t i = 0; i < pCreateInfo->subpassCount; ++i)
	{
		const auto& subpass = pCreateInfo->pSubpasses[i];
		subpasses[i].pInputAttachments = nullptr;
		subpasses[i].pColorAttachments = nullptr;
		subpasses[i].pResolveAttachments = nullptr;
		subpasses[i].pDepthStencilAttachment = nullptr;
		subpasses[i].pPreserveAttachments = nullptr;

		if(subpass.inputAttachmentCount > 0)
		{
			size_t inputAttachmentsSize = subpass.inputAttachmentCount * sizeof(VkAttachmentReference);
			subpasses[i].pInputAttachments = reinterpret_cast<VkAttachmentReference*>(hostMemory);
			memcpy(const_cast<VkAttachmentReference*>(subpasses[i].pInputAttachments),
			       pCreateInfo->pSubpasses[i].pInputAttachments, inputAttachmentsSize);
			hostMemory += inputAttachmentsSize;

			for (auto j = 0u; j < subpasses[i].inputAttachmentCount; j++)
			{
				if (subpass.pInputAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pInputAttachments[j].attachment, i);
			}
		}

		if(subpass.colorAttachmentCount > 0)
		{
			size_t colorAttachmentsSize = subpass.colorAttachmentCount * sizeof(VkAttachmentReference);
			subpasses[i].pColorAttachments = reinterpret_cast<VkAttachmentReference*>(hostMemory);
			memcpy(const_cast<VkAttachmentReference*>(subpasses[i].pColorAttachments),
			       subpass.pColorAttachments, colorAttachmentsSize);
			hostMemory += colorAttachmentsSize;

			if(subpass.pResolveAttachments)
			{
				subpasses[i].pResolveAttachments = reinterpret_cast<VkAttachmentReference*>(hostMemory);
				memcpy(const_cast<VkAttachmentReference*>(subpasses[i].pResolveAttachments),
				       subpass.pResolveAttachments, colorAttachmentsSize);
				hostMemory += colorAttachmentsSize;
			}

			for (auto j = 0u; j < subpasses[i].colorAttachmentCount; j++)
			{
				if (subpass.pColorAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pColorAttachments[j].attachment, i);
				if (subpass.pResolveAttachments &&
					subpass.pResolveAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pResolveAttachments[j].attachment, i);
			}
		}

		if(subpass.pDepthStencilAttachment)
		{
			subpasses[i].pDepthStencilAttachment = reinterpret_cast<VkAttachmentReference*>(hostMemory);
			memcpy(const_cast<VkAttachmentReference*>(subpasses[i].pDepthStencilAttachment),
				subpass.pDepthStencilAttachment, sizeof(VkAttachmentReference));
			hostMemory += sizeof(VkAttachmentReference);

			if (subpass.pDepthStencilAttachment->attachment != VK_ATTACHMENT_UNUSED)
				MarkFirstUse(subpass.pDepthStencilAttachment->attachment, i);
		}

		if(subpass.preserveAttachmentCount > 0)
		{
			size_t preserveAttachmentSize = subpass.preserveAttachmentCount * sizeof(uint32_t);
			subpasses[i].pPreserveAttachments = reinterpret_cast<uint32_t*>(hostMemory);
			memcpy(const_cast<uint32_t*>(subpasses[i].pPreserveAttachments),
			       pCreateInfo->pSubpasses[i].pPreserveAttachments, preserveAttachmentSize);
			hostMemory += preserveAttachmentSize;

			for (auto j = 0u; j < subpasses[i].preserveAttachmentCount; j++)
			{
				if (subpass.pPreserveAttachments[j] != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pPreserveAttachments[j], i);
			}
		}
	}

	if(pCreateInfo->dependencyCount > 0)
	{
		size_t dependenciesSize = pCreateInfo->dependencyCount * sizeof(VkSubpassDependency);
		dependencies = reinterpret_cast<VkSubpassDependency*>(hostMemory);
		memcpy(dependencies, pCreateInfo->pDependencies, dependenciesSize);
	}
}

void RenderPass::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::deallocate(subpasses, pAllocator); // attachments and dependencies are in the same allocation
}

size_t RenderPass::ComputeRequiredAllocationSize(const VkRenderPassCreateInfo* pCreateInfo)
{
	size_t attachmentSize = pCreateInfo->attachmentCount * sizeof(VkAttachmentDescription)
			+ pCreateInfo->attachmentCount * sizeof(int)			// first use
			+ pCreateInfo->attachmentCount * sizeof(uint32_t);		// union of subpass view masks, per attachment
	size_t subpassesSize = 0;
	for(uint32_t i = 0; i < pCreateInfo->subpassCount; ++i)
	{
		const auto& subpass = pCreateInfo->pSubpasses[i];
		uint32_t nbAttachments = subpass.inputAttachmentCount + subpass.colorAttachmentCount;
		if(subpass.pResolveAttachments)
		{
			nbAttachments += subpass.colorAttachmentCount;
		}
		if(subpass.pDepthStencilAttachment)
		{
			nbAttachments += 1;
		}
		subpassesSize += sizeof(VkSubpassDescription) +
		                 sizeof(VkAttachmentReference) * nbAttachments +
		                 sizeof(uint32_t) * subpass.preserveAttachmentCount +
		                 sizeof(uint32_t);			// view mask
	}
	size_t dependenciesSize = pCreateInfo->dependencyCount * sizeof(VkSubpassDependency);

	return attachmentSize + subpassesSize + dependenciesSize;
}

void RenderPass::getRenderAreaGranularity(VkExtent2D* pGranularity) const
{
	pGranularity->width = 1;
	pGranularity->height = 1;
}

void RenderPass::MarkFirstUse(int attachment, int subpass)
{
	// FIXME: we may not actually need to track attachmentFirstUse if we're going to eagerly
	//  clear attachments at the start of the renderpass; can use attachmentViewMasks always instead.

	if (attachmentFirstUse[attachment] == -1)
		attachmentFirstUse[attachment] = subpass;

	if (isMultiView())
		attachmentViewMasks[attachment] |= viewMasks[subpass];
}

} // namespace vk