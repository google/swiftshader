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
#include "VkStringify.hpp"
#include <cstring>

namespace {

template<class T>
size_t ComputeRequiredAllocationSizeT(const T *pCreateInfo)
{
	size_t attachmentSize = pCreateInfo->attachmentCount * sizeof(VkAttachmentDescription) + pCreateInfo->attachmentCount * sizeof(int)  // first use
	                        + pCreateInfo->attachmentCount * sizeof(uint32_t);                                                           // union of subpass view masks, per attachment
	size_t subpassesSize = 0;
	for(uint32_t i = 0; i < pCreateInfo->subpassCount; ++i)
	{
		const auto &subpass = pCreateInfo->pSubpasses[i];
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
		                 sizeof(uint32_t);  // view mask
	}
	size_t dependenciesSize = pCreateInfo->dependencyCount * sizeof(VkSubpassDependency);

	return attachmentSize + subpassesSize + dependenciesSize;
}

template<class T>
void CopySubpasses(VkSubpassDescription *dst, const T *src, uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
	{
		dst[i].flags = src[i].flags;
		dst[i].pipelineBindPoint = src[i].pipelineBindPoint;
		dst[i].inputAttachmentCount = src[i].inputAttachmentCount;
		dst[i].pInputAttachments = nullptr;
		dst[i].colorAttachmentCount = src[i].colorAttachmentCount;
		dst[i].pColorAttachments = nullptr;
		dst[i].pResolveAttachments = nullptr;
		dst[i].pDepthStencilAttachment = nullptr;
		dst[i].preserveAttachmentCount = src[i].preserveAttachmentCount;
		dst[i].pPreserveAttachments = nullptr;
	}
}

template<class T>
void CopyAttachmentDescriptions(VkAttachmentDescription *dst, const T *src, uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
	{
		dst[i].flags = src[i].flags;
		dst[i].format = src[i].format;
		dst[i].samples = src[i].samples;
		dst[i].loadOp = src[i].loadOp;
		dst[i].storeOp = src[i].storeOp;
		dst[i].stencilLoadOp = src[i].stencilLoadOp;
		dst[i].stencilStoreOp = src[i].stencilStoreOp;
		dst[i].initialLayout = src[i].initialLayout;
		dst[i].finalLayout = src[i].finalLayout;
	}
}

template<class T>
void CopyAttachmentReferences(VkAttachmentReference *dst, const T *src, uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
	{
		dst[i].attachment = src[i].attachment;
		dst[i].layout = src[i].layout;
	}
}

template<class T>
void CopySubpassDependencies(VkSubpassDependency *dst, const T *src, uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
	{
		dst[i].srcSubpass = src[i].srcSubpass;
		dst[i].dstSubpass = src[i].dstSubpass;
		dst[i].srcStageMask = src[i].srcStageMask;
		dst[i].dstStageMask = src[i].dstStageMask;
		dst[i].srcAccessMask = src[i].srcAccessMask;
		dst[i].dstAccessMask = src[i].dstAccessMask;
		dst[i].dependencyFlags = src[i].dependencyFlags;
	}
}

bool GetViewMasks(const VkRenderPassCreateInfo *pCreateInfo, uint32_t *masks)
{
	return false;
}

bool GetViewMasks(const VkRenderPassCreateInfo2KHR *pCreateInfo, uint32_t *masks)
{
	for(uint32_t i = 0; i < pCreateInfo->subpassCount; ++i)
	{
		masks[i] = pCreateInfo->pSubpasses[i].viewMask;
	}
	return true;
}

}  // namespace

namespace vk {

RenderPass::RenderPass(const VkRenderPassCreateInfo *pCreateInfo, void *mem)
    : attachmentCount(pCreateInfo->attachmentCount)
    , subpassCount(pCreateInfo->subpassCount)
    , dependencyCount(pCreateInfo->dependencyCount)
{
	init(pCreateInfo, mem);
}

RenderPass::RenderPass(const VkRenderPassCreateInfo2KHR *pCreateInfo, void *mem)
    : attachmentCount(pCreateInfo->attachmentCount)
    , subpassCount(pCreateInfo->subpassCount)
    , dependencyCount(pCreateInfo->dependencyCount)
{
	init(pCreateInfo, mem);
	// Note: the init function above ignores:
	// - pCorrelatedViewMasks: This provides a potential performance optimization
	// - VkAttachmentReference2::aspectMask : This specifies which aspects may be used
	// - VkSubpassDependency2::viewOffset : This is the same as VkRenderPassMultiviewCreateInfo::pViewOffsets, which is currently ignored
	// - Any pNext pointer in VkRenderPassCreateInfo2KHR's internal structures
}

template<class T>
void RenderPass::init(const T *pCreateInfo, void *mem)
{
	char *hostMemory = reinterpret_cast<char *>(mem);

	// subpassCount must be greater than 0
	ASSERT(pCreateInfo->subpassCount > 0);

	size_t subpassesSize = pCreateInfo->subpassCount * sizeof(VkSubpassDescription);
	subpasses = reinterpret_cast<VkSubpassDescription *>(hostMemory);
	CopySubpasses(subpasses, pCreateInfo->pSubpasses, pCreateInfo->subpassCount);
	hostMemory += subpassesSize;
	uint32_t *masks = reinterpret_cast<uint32_t *>(hostMemory);
	hostMemory += subpassCount * sizeof(uint32_t);

	if(attachmentCount > 0)
	{
		size_t attachmentSize = pCreateInfo->attachmentCount * sizeof(VkAttachmentDescription);
		attachments = reinterpret_cast<VkAttachmentDescription *>(hostMemory);
		CopyAttachmentDescriptions(attachments, pCreateInfo->pAttachments, pCreateInfo->attachmentCount);
		hostMemory += attachmentSize;

		size_t firstUseSize = pCreateInfo->attachmentCount * sizeof(int);
		attachmentFirstUse = reinterpret_cast<int *>(hostMemory);
		hostMemory += firstUseSize;

		attachmentViewMasks = reinterpret_cast<uint32_t *>(hostMemory);
		hostMemory += pCreateInfo->attachmentCount * sizeof(uint32_t);
		for(auto i = 0u; i < pCreateInfo->attachmentCount; i++)
		{
			attachmentFirstUse[i] = -1;
			attachmentViewMasks[i] = 0;
		}
	}

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
			case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO:
			{
				// Renderpass uses multiview if this structure is present AND some subpass specifies
				// a nonzero view mask
				auto const *multiviewCreateInfo = reinterpret_cast<VkRenderPassMultiviewCreateInfo const *>(extensionCreateInfo);
				for(auto i = 0u; i < pCreateInfo->subpassCount; i++)
				{
					masks[i] = multiviewCreateInfo->pViewMasks[i];
					// This is now a multiview renderpass, so make the masks available
					if(masks[i])
					{
						viewMasks = masks;
					}
				}

				break;
			}
			default:
				WARN("pCreateInfo->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	if(!viewMasks && (GetViewMasks(pCreateInfo, masks)))
	{
		for(auto i = 0u; i < pCreateInfo->subpassCount; i++)
		{
			if(masks[i])
			{
				viewMasks = masks;
			}
		}
	}

	// Deep copy subpasses
	for(uint32_t i = 0; i < pCreateInfo->subpassCount; ++i)
	{
		const auto &subpass = pCreateInfo->pSubpasses[i];

		if(subpass.inputAttachmentCount > 0)
		{
			size_t inputAttachmentsSize = subpass.inputAttachmentCount * sizeof(VkAttachmentReference);
			subpasses[i].pInputAttachments = reinterpret_cast<VkAttachmentReference *>(hostMemory);
			CopyAttachmentReferences(const_cast<VkAttachmentReference *>(subpasses[i].pInputAttachments),
			                         pCreateInfo->pSubpasses[i].pInputAttachments, subpass.inputAttachmentCount);
			hostMemory += inputAttachmentsSize;

			for(auto j = 0u; j < subpasses[i].inputAttachmentCount; j++)
			{
				if(subpass.pInputAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pInputAttachments[j].attachment, i);
			}
		}

		if(subpass.colorAttachmentCount > 0)
		{
			size_t colorAttachmentsSize = subpass.colorAttachmentCount * sizeof(VkAttachmentReference);
			subpasses[i].pColorAttachments = reinterpret_cast<VkAttachmentReference *>(hostMemory);
			CopyAttachmentReferences(const_cast<VkAttachmentReference *>(subpasses[i].pColorAttachments),
			                         subpass.pColorAttachments, subpass.colorAttachmentCount);
			hostMemory += colorAttachmentsSize;

			if(subpass.pResolveAttachments)
			{
				subpasses[i].pResolveAttachments = reinterpret_cast<VkAttachmentReference *>(hostMemory);
				CopyAttachmentReferences(const_cast<VkAttachmentReference *>(subpasses[i].pResolveAttachments),
				                         subpass.pResolveAttachments, subpass.colorAttachmentCount);
				hostMemory += colorAttachmentsSize;
			}

			for(auto j = 0u; j < subpasses[i].colorAttachmentCount; j++)
			{
				if(subpass.pColorAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pColorAttachments[j].attachment, i);
				if(subpass.pResolveAttachments &&
				   subpass.pResolveAttachments[j].attachment != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pResolveAttachments[j].attachment, i);
			}
		}

		if(subpass.pDepthStencilAttachment)
		{
			subpasses[i].pDepthStencilAttachment = reinterpret_cast<VkAttachmentReference *>(hostMemory);
			CopyAttachmentReferences(const_cast<VkAttachmentReference *>(subpasses[i].pDepthStencilAttachment),
			                         subpass.pDepthStencilAttachment, 1);
			hostMemory += sizeof(VkAttachmentReference);

			if(subpass.pDepthStencilAttachment->attachment != VK_ATTACHMENT_UNUSED)
				MarkFirstUse(subpass.pDepthStencilAttachment->attachment, i);
		}

		if(subpass.preserveAttachmentCount > 0)
		{
			size_t preserveAttachmentSize = subpass.preserveAttachmentCount * sizeof(uint32_t);
			subpasses[i].pPreserveAttachments = reinterpret_cast<uint32_t *>(hostMemory);
			for(uint32_t j = 0u; j < subpass.preserveAttachmentCount; j++)
			{
				const_cast<uint32_t *>(subpasses[i].pPreserveAttachments)[j] = pCreateInfo->pSubpasses[i].pPreserveAttachments[j];
			}
			hostMemory += preserveAttachmentSize;

			for(auto j = 0u; j < subpasses[i].preserveAttachmentCount; j++)
			{
				if(subpass.pPreserveAttachments[j] != VK_ATTACHMENT_UNUSED)
					MarkFirstUse(subpass.pPreserveAttachments[j], i);
			}
		}
	}

	if(pCreateInfo->dependencyCount > 0)
	{
		dependencies = reinterpret_cast<VkSubpassDependency *>(hostMemory);
		CopySubpassDependencies(dependencies, pCreateInfo->pDependencies, pCreateInfo->dependencyCount);
	}
}

void RenderPass::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(subpasses, pAllocator);  // attachments and dependencies are in the same allocation
}

size_t RenderPass::ComputeRequiredAllocationSize(const VkRenderPassCreateInfo *pCreateInfo)
{
	return ComputeRequiredAllocationSizeT(pCreateInfo);
}

size_t RenderPass::ComputeRequiredAllocationSize(const VkRenderPassCreateInfo2KHR *pCreateInfo)
{
	return ComputeRequiredAllocationSizeT(pCreateInfo);
}

void RenderPass::getRenderAreaGranularity(VkExtent2D *pGranularity) const
{
	pGranularity->width = 1;
	pGranularity->height = 1;
}

void RenderPass::MarkFirstUse(int attachment, int subpass)
{
	// FIXME: we may not actually need to track attachmentFirstUse if we're going to eagerly
	//  clear attachments at the start of the renderpass; can use attachmentViewMasks always instead.

	if(attachmentFirstUse[attachment] == -1)
		attachmentFirstUse[attachment] = subpass;

	if(isMultiView())
		attachmentViewMasks[attachment] |= viewMasks[subpass];
}

}  // namespace vk
