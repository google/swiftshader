// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_STRUCT_CONVERSION_HPP_
#define VK_STRUCT_CONVERSION_HPP_

#include "Vulkan/VulkanPlatform.hpp"
#include <vector>

namespace vk {

struct CopyBufferInfo : public VkCopyBufferInfo2
{
	CopyBufferInfo(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions)
	    : VkCopyBufferInfo2{
		    VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
		    nullptr,
		    srcBuffer,
		    dstBuffer,
		    regionCount,
		    nullptr
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_BUFFER_COPY_2,
				nullptr,
				pRegions[i].srcOffset,
				pRegions[i].dstOffset,
				pRegions[i].size
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkBufferCopy2> regions;
};

struct CopyImageInfo : public VkCopyImageInfo2
{
	CopyImageInfo(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy *pRegions)
	    : VkCopyImageInfo2{
		    VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
		    nullptr,
		    srcImage,
		    srcImageLayout,
		    dstImage,
		    dstImageLayout,
		    regionCount,
		    nullptr
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_IMAGE_COPY_2,
				nullptr,
				pRegions[i].srcSubresource,
				pRegions[i].srcOffset,
				pRegions[i].dstSubresource,
				pRegions[i].dstOffset,
				pRegions[i].extent
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkImageCopy2> regions;
};

struct BlitImageInfo : public VkBlitImageInfo2
{
	BlitImageInfo(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter)
	    : VkBlitImageInfo2{
		    VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
		    nullptr,
		    srcImage,
		    srcImageLayout,
		    dstImage,
		    dstImageLayout,
		    regionCount,
		    nullptr,
		    filter
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
				nullptr,
				pRegions[i].srcSubresource,
				{ pRegions[i].srcOffsets[0], pRegions[i].srcOffsets[1] },
				pRegions[i].dstSubresource,
				{ pRegions[i].dstOffsets[0], pRegions[i].dstOffsets[1] }
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkImageBlit2> regions;
};

struct CopyBufferToImageInfo : public VkCopyBufferToImageInfo2
{
	CopyBufferToImageInfo(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy *pRegions)
	    : VkCopyBufferToImageInfo2{
		    VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
		    nullptr,
		    srcBuffer,
		    dstImage,
		    dstImageLayout,
		    regionCount,
		    nullptr
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
				nullptr,
				pRegions[i].bufferOffset,
				pRegions[i].bufferRowLength,
				pRegions[i].bufferImageHeight,
				pRegions[i].imageSubresource,
				pRegions[i].imageOffset,
				pRegions[i].imageExtent
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkBufferImageCopy2> regions;
};

struct CopyImageToBufferInfo : public VkCopyImageToBufferInfo2
{
	CopyImageToBufferInfo(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy *pRegions)
	    : VkCopyImageToBufferInfo2{
		    VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
		    nullptr,
		    srcImage,
		    srcImageLayout,
		    dstBuffer,
		    regionCount,
		    nullptr
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
				nullptr,
				pRegions[i].bufferOffset,
				pRegions[i].bufferRowLength,
				pRegions[i].bufferImageHeight,
				pRegions[i].imageSubresource,
				pRegions[i].imageOffset,
				pRegions[i].imageExtent
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkBufferImageCopy2> regions;
};

struct ResolveImageInfo : public VkResolveImageInfo2
{
	ResolveImageInfo(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve *pRegions)
	    : VkResolveImageInfo2{
		    VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2,
		    nullptr,
		    srcImage,
		    srcImageLayout,
		    dstImage,
		    dstImageLayout,
		    regionCount,
		    nullptr
	    }
	{
		regions.resize(regionCount);
		for(uint32_t i = 0; i < regionCount; i++)
		{
			regions[i] = {
				VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2,
				nullptr,
				pRegions[i].srcSubresource,
				pRegions[i].srcOffset,
				pRegions[i].dstSubresource,
				pRegions[i].dstOffset,
				pRegions[i].extent
			};
		}

		this->pRegions = &regions.front();
	}

private:
	std::vector<VkImageResolve2> regions;
};

struct ImageSubresource : VkImageSubresource
{
	ImageSubresource(const VkImageSubresourceLayers &subresourceLayers)
	    : VkImageSubresource{
		    subresourceLayers.aspectMask,
		    subresourceLayers.mipLevel,
		    subresourceLayers.baseArrayLayer
	    }
	{}
};

struct ImageSubresourceRange : VkImageSubresourceRange
{
	ImageSubresourceRange(const VkImageSubresourceLayers &subresourceLayers)
	    : VkImageSubresourceRange{
		    subresourceLayers.aspectMask,
		    subresourceLayers.mipLevel,
		    1,
		    subresourceLayers.baseArrayLayer,
		    subresourceLayers.layerCount
	    }
	{}
};

struct Extent2D : VkExtent2D
{
	Extent2D(const VkExtent3D &extent3D)
	    : VkExtent2D{ extent3D.width, extent3D.height }
	{}
};

}  // namespace vk

#endif  // VK_STRUCT_CONVERSION_HPP_