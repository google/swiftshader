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
#include "System/Math.hpp"

#include <climits>

namespace {

VkComponentMapping ResolveComponentMapping(VkComponentMapping m, vk::Format format)
{
	m = vk::ResolveIdentityMapping(m);

	// Replace non-present components with zero/one swizzles so that the sampler
	// will give us correct interactions between channel replacement and texel replacement,
	// where we've had to invent new channels behind the app's back (eg transparent decompression
	// of ETC2 RGB -> BGRA8)
	VkComponentSwizzle table[] = {
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_ZERO,
		VK_COMPONENT_SWIZZLE_ONE,
		VK_COMPONENT_SWIZZLE_R,
		format.componentCount() < 2 ? VK_COMPONENT_SWIZZLE_ZERO : VK_COMPONENT_SWIZZLE_G,
		format.componentCount() < 3 ? VK_COMPONENT_SWIZZLE_ZERO : VK_COMPONENT_SWIZZLE_B,
		format.componentCount() < 4 ? VK_COMPONENT_SWIZZLE_ONE : VK_COMPONENT_SWIZZLE_A,
	};

	return { table[m.r], table[m.g], table[m.b], table[m.a] };
}

VkImageSubresourceRange ResolveRemainingLevelsLayers(VkImageSubresourceRange range, const vk::Image *image)
{
	return {
		range.aspectMask,
		range.baseMipLevel,
		(range.levelCount == VK_REMAINING_MIP_LEVELS) ? (image->getMipLevels() - range.baseMipLevel) : range.levelCount,
		range.baseArrayLayer,
		(range.layerCount == VK_REMAINING_ARRAY_LAYERS) ? (image->getArrayLayers() - range.baseArrayLayer) : range.layerCount,
	};
}

}  // anonymous namespace

namespace vk {

Identifier::Identifier(const Image *image, VkImageViewType type, VkFormat fmt, VkComponentMapping mapping)
{
	imageViewType = type;
	format = Format::mapTo8bit(fmt);
	r = mapping.r;
	g = mapping.g;
	b = mapping.b;
	a = mapping.a;

	// TODO(b/152224843): eliminate
	auto extent = image->getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);
	large = (extent.width > SHRT_MAX) ||
	        (extent.height > SHRT_MAX) ||
	        (extent.depth > SHRT_MAX);
}

Identifier::Identifier(VkFormat fmt)
{
	static_assert(VK_IMAGE_VIEW_TYPE_END_RANGE == 6, "VkImageViewType does not allow using 7 to indicate buffer view");
	imageViewType = 7;  // Still fits in 3-bit field
	format = Format::mapTo8bit(fmt);
}

ImageView::ImageView(const VkImageViewCreateInfo *pCreateInfo, void *mem, const vk::SamplerYcbcrConversion *ycbcrConversion)
    : image(vk::Cast(pCreateInfo->image))
    , viewType(pCreateInfo->viewType)
    , format(pCreateInfo->format)
    , components(ResolveComponentMapping(pCreateInfo->components, format))
    , subresourceRange(ResolveRemainingLevelsLayers(pCreateInfo->subresourceRange, image))
    , ycbcrConversion(ycbcrConversion)
    , id(image, viewType, format.getAspectFormat(subresourceRange.aspectMask), components)
{
}

size_t ImageView::ComputeRequiredAllocationSize(const VkImageViewCreateInfo *pCreateInfo)
{
	return 0;
}

void ImageView::destroy(const VkAllocationCallbacks *pAllocator)
{
}

// Vulkan 1.2 Table 8. Image and image view parameter compatibility requirements
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

void ImageView::clear(const VkClearValue &clearValue, const VkImageAspectFlags aspectMask, const VkRect2D &renderArea)
{
	// Note: clearing ignores swizzling, so components is ignored.

	ASSERT(imageTypesMatch(image->getImageType()));
	ASSERT(format.isCompatible(image->getFormat()));

	VkImageSubresourceRange sr = subresourceRange;
	sr.aspectMask = aspectMask;
	image->clear(clearValue, format, renderArea, sr);
}

void ImageView::clear(const VkClearValue &clearValue, const VkImageAspectFlags aspectMask, const VkClearRect &renderArea)
{
	// Note: clearing ignores swizzling, so components is ignored.

	ASSERT(imageTypesMatch(image->getImageType()));
	ASSERT(format.isCompatible(image->getFormat()));

	VkImageSubresourceRange sr;
	sr.aspectMask = aspectMask;
	sr.baseMipLevel = subresourceRange.baseMipLevel;
	sr.levelCount = subresourceRange.levelCount;
	sr.baseArrayLayer = renderArea.baseArrayLayer + subresourceRange.baseArrayLayer;
	sr.layerCount = renderArea.layerCount;

	image->clear(clearValue, format, renderArea.rect, sr);
}

void ImageView::clearWithLayerMask(const VkClearValue &clearValue, VkImageAspectFlags aspectMask, const VkRect2D &renderArea, uint32_t layerMask)
{
	while(layerMask)
	{
		uint32_t layer = sw::log2i(layerMask);
		layerMask &= ~(1 << layer);
		VkClearRect r = { renderArea, layer, 1 };
		r.baseArrayLayer = layer;
		clear(clearValue, aspectMask, r);
	}
}

void ImageView::resolve(ImageView *resolveAttachment, int layer)
{
	if((subresourceRange.levelCount != 1) || (resolveAttachment->subresourceRange.levelCount != 1))
	{
		UNIMPLEMENTED("b/148242443: levelCount != 1");  // FIXME(b/148242443)
	}

	VkImageCopy region;
	region.srcSubresource = {
		subresourceRange.aspectMask,
		subresourceRange.baseMipLevel,
		subresourceRange.baseArrayLayer + layer,
		1
	};
	region.srcOffset = { 0, 0, 0 };
	region.dstSubresource = {
		resolveAttachment->subresourceRange.aspectMask,
		resolveAttachment->subresourceRange.baseMipLevel,
		resolveAttachment->subresourceRange.baseArrayLayer + layer,
		1
	};
	region.dstOffset = { 0, 0, 0 };
	region.extent = image->getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask),
	                                         subresourceRange.baseMipLevel);

	image->copyTo(resolveAttachment->image, region);
}

void ImageView::resolve(ImageView *resolveAttachment)
{
	if((subresourceRange.levelCount != 1) || (resolveAttachment->subresourceRange.levelCount != 1))
	{
		UNIMPLEMENTED("b/148242443: levelCount != 1");  // FIXME(b/148242443)
	}

	VkImageCopy region;
	region.srcSubresource = {
		subresourceRange.aspectMask,
		subresourceRange.baseMipLevel,
		subresourceRange.baseArrayLayer,
		subresourceRange.layerCount
	};
	region.srcOffset = { 0, 0, 0 };
	region.dstSubresource = {
		resolveAttachment->subresourceRange.aspectMask,
		resolveAttachment->subresourceRange.baseMipLevel,
		resolveAttachment->subresourceRange.baseArrayLayer,
		resolveAttachment->subresourceRange.layerCount
	};
	region.dstOffset = { 0, 0, 0 };
	region.extent = image->getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask),
	                                         subresourceRange.baseMipLevel);

	image->copyTo(resolveAttachment->image, region);
}

void ImageView::resolveWithLayerMask(ImageView *resolveAttachment, uint32_t layerMask)
{
	while(layerMask)
	{
		int layer = sw::log2i(layerMask);
		layerMask &= ~(1 << layer);
		resolve(resolveAttachment, layer);
	}
}

const Image *ImageView::getImage(Usage usage) const
{
	switch(usage)
	{
		case RAW:
			return image;
		case SAMPLING:
			return image->getSampledImage(format);
		default:
			UNREACHABLE("usage %d", int(usage));
			return nullptr;
	}
}

Format ImageView::getFormat(Usage usage) const
{
	Format imageFormat = ((usage == RAW) || (getImage(usage) == image)) ? format : getImage(usage)->getFormat();
	return imageFormat.getAspectFormat(subresourceRange.aspectMask);
}

int ImageView::rowPitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel, Usage usage) const
{
	return getImage(usage)->rowPitchBytes(aspect, subresourceRange.baseMipLevel + mipLevel);
}

int ImageView::slicePitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel, Usage usage) const
{
	return getImage(usage)->slicePitchBytes(aspect, subresourceRange.baseMipLevel + mipLevel);
}

int ImageView::getMipLevelSize(VkImageAspectFlagBits aspect, uint32_t mipLevel, Usage usage) const
{
	return getImage(usage)->getMipLevelSize(aspect, subresourceRange.baseMipLevel + mipLevel);
}

int ImageView::layerPitchBytes(VkImageAspectFlagBits aspect, Usage usage) const
{
	return static_cast<int>(getImage(usage)->getLayerSize(aspect));
}

VkExtent3D ImageView::getMipLevelExtent(uint32_t mipLevel) const
{
	return image->getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask),
	                                subresourceRange.baseMipLevel + mipLevel);
}

void *ImageView::getOffsetPointer(const VkOffset3D &offset, VkImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t layer, Usage usage) const
{
	ASSERT(mipLevel < subresourceRange.levelCount);

	VkImageSubresourceLayers imageSubresourceLayers = {
		static_cast<VkImageAspectFlags>(aspect),
		subresourceRange.baseMipLevel + mipLevel,
		subresourceRange.baseArrayLayer + layer,
		subresourceRange.layerCount
	};

	return getImage(usage)->getTexelPointer(offset, imageSubresourceLayers);
}

}  // namespace vk
