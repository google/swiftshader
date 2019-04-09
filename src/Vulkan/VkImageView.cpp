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

#include "VkImageView.hpp"
#include "VkImage.hpp"

namespace vk
{

ImageView::ImageView(const VkImageViewCreateInfo* pCreateInfo, void* mem) :
	image(Cast(pCreateInfo->image)), viewType(pCreateInfo->viewType), format(pCreateInfo->format),
	components(pCreateInfo->components), subresourceRange(pCreateInfo->subresourceRange)
{
}

size_t ImageView::ComputeRequiredAllocationSize(const VkImageViewCreateInfo* pCreateInfo)
{
	return 0;
}

void ImageView::destroy(const VkAllocationCallbacks* pAllocator)
{
}

bool ImageView::imageTypesMatch(VkImageType imageType) const
{
	uint32_t imageArrayLayers = image->getArrayLayers();

	switch(viewType)
	{
	case VK_IMAGE_VIEW_TYPE_1D:
		return (imageType == VK_IMAGE_TYPE_1D) &&
		       (subresourceRange.layerCount == 1);
	case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
		return imageType == VK_IMAGE_TYPE_1D;
	case VK_IMAGE_VIEW_TYPE_2D:
		return ((imageType == VK_IMAGE_TYPE_2D) ||
		        ((imageType == VK_IMAGE_TYPE_3D) &&
		         (imageArrayLayers == 1))) &&
		       (subresourceRange.layerCount == 1);
	case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
		return (imageType == VK_IMAGE_TYPE_2D) ||
		       ((imageType == VK_IMAGE_TYPE_3D) &&
		        (imageArrayLayers == 1));
	case VK_IMAGE_VIEW_TYPE_CUBE:
		return image->isCube() &&
		       (imageArrayLayers >= subresourceRange.layerCount) &&
		       (subresourceRange.layerCount == 6);
	case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
		return image->isCube() &&
		       (imageArrayLayers >= subresourceRange.layerCount) &&
		       (subresourceRange.layerCount >= 6);
	case VK_IMAGE_VIEW_TYPE_3D:
		return (imageType == VK_IMAGE_TYPE_3D) &&
		       (imageArrayLayers == 1) &&
		       (subresourceRange.layerCount == 1);
	default:
		UNREACHABLE("Unexpected viewType %d", (int)viewType);
	}

	return false;
}

void ImageView::clear(const VkClearValue& clearValue, const VkImageAspectFlags aspectMask, const VkRect2D& renderArea)
{
	// Note: clearing ignores swizzling, so components is ignored.

	if(!imageTypesMatch(image->getImageType()))
	{
		UNIMPLEMENTED("imageTypesMatch");
	}

	if(!format.isCompatible(image->getFormat()))
	{
		UNIMPLEMENTED("incompatible formats");
	}

	VkImageSubresourceRange sr = subresourceRange;
	sr.aspectMask = aspectMask;
	image->clear(clearValue, format, renderArea, sr);
}

void ImageView::clear(const VkClearValue& clearValue, const VkImageAspectFlags aspectMask, const VkClearRect& renderArea)
{
	// Note: clearing ignores swizzling, so components is ignored.

	if(!imageTypesMatch(image->getImageType()))
	{
		UNIMPLEMENTED("imageTypesMatch");
	}

	if(!format.isCompatible(image->getFormat()))
	{
		UNIMPLEMENTED("incompatible formats");
	}

	VkImageSubresourceRange sr;
	sr.aspectMask = aspectMask;
	sr.baseMipLevel = subresourceRange.baseMipLevel;
	sr.levelCount = subresourceRange.levelCount;
	sr.baseArrayLayer = renderArea.baseArrayLayer + subresourceRange.baseArrayLayer;
	sr.layerCount = renderArea.layerCount;

	image->clear(clearValue, format, renderArea.rect, sr);
}

void ImageView::resolve(ImageView* resolveAttachment)
{
	if((subresourceRange.levelCount != 1) || (resolveAttachment->subresourceRange.levelCount != 1))
	{
		UNIMPLEMENTED("levelCount");
	}

	VkImageCopy region;
	region.srcSubresource =
	{
		subresourceRange.aspectMask,
		subresourceRange.baseMipLevel,
		subresourceRange.baseArrayLayer,
		subresourceRange.layerCount
	};
	region.srcOffset = { 0, 0, 0 };
	region.dstSubresource =
	{
		resolveAttachment->subresourceRange.aspectMask,
		resolveAttachment->subresourceRange.baseMipLevel,
		resolveAttachment->subresourceRange.baseArrayLayer,
		resolveAttachment->subresourceRange.layerCount
	};
	region.dstOffset = { 0, 0, 0 };
	region.extent = image->getMipLevelExtent(subresourceRange.baseMipLevel);

	image->copyTo(*(resolveAttachment->image), region);
}

void *ImageView::getOffsetPointer(const VkOffset3D& offset, VkImageAspectFlagBits aspect) const
{
	VkImageSubresourceLayers imageSubresourceLayers =
	{
		static_cast<VkImageAspectFlags>(aspect),
		subresourceRange.baseMipLevel,
		subresourceRange.baseArrayLayer,
		subresourceRange.layerCount
	};
	return image->getTexelPointer(offset, imageSubresourceLayers);
}

}
