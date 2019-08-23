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

#ifndef VK_IMAGE_VIEW_HPP_
#define VK_IMAGE_VIEW_HPP_

#include "VkDebug.hpp"
#include "VkFormat.h"
#include "VkObject.hpp"
#include "VkImage.hpp"

#include <atomic>

namespace vk
{
class SamplerYcbcrConversion;

class ImageView : public Object<ImageView, VkImageView>
{
public:
	// Image usage:
	// RAW: Use the base image as is
	// SAMPLING: Image used for texture sampling
	enum Usage { RAW, SAMPLING };

	ImageView(const VkImageViewCreateInfo* pCreateInfo, void* mem, const vk::SamplerYcbcrConversion *ycbcrConversion);
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkImageViewCreateInfo* pCreateInfo);

	void clear(const VkClearValue& clearValues, VkImageAspectFlags aspectMask, const VkRect2D& renderArea);
	void clear(const VkClearValue& clearValue, VkImageAspectFlags aspectMask, const VkClearRect& renderArea);
	void clearWithLayerMask(const VkClearValue &clearValue, VkImageAspectFlags aspectMask, const VkRect2D &renderArea, uint32_t layerMask);
	void resolve(ImageView* resolveAttachment);
	void resolve(ImageView* resolveAttachment, int layer);
	void resolveWithLayerMask(ImageView *resolveAttachment, uint32_t layerMask);

	VkImageViewType getType() const { return viewType; }
	Format getFormat(Usage usage = RAW) const;
	Format getFormat(VkImageAspectFlagBits aspect) const { return image->getFormat(aspect); }
	int rowPitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel, Usage usage = RAW) const;
	int slicePitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel, Usage usage = RAW) const;
	int layerPitchBytes(VkImageAspectFlagBits aspect, Usage usage = RAW) const;
	VkExtent3D getMipLevelExtent(uint32_t mipLevel) const;

	int getSampleCount() const
	{
		switch (image->getSampleCountFlagBits())
		{
		case VK_SAMPLE_COUNT_1_BIT: return 1;
		case VK_SAMPLE_COUNT_4_BIT: return 4;
		default:
			UNIMPLEMENTED("Sample count flags %d", image->getSampleCountFlagBits());
			return 1;
		}
	}

	void *getOffsetPointer(const VkOffset3D& offset, VkImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t layer, Usage usage = RAW) const;
	bool hasDepthAspect() const { return (subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) != 0; }
	bool hasStencilAspect() const { return (subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) != 0; }

	void prepareForSampling() const { image->prepareForSampling(subresourceRange); }

	const VkComponentMapping &getComponentMapping() const { return components; }
	const VkImageSubresourceRange &getSubresourceRange() const { return subresourceRange; }
	size_t getImageSizeInBytes() const { return image->getMemoryRequirements().size; }

	const uint32_t id = nextID++;

private:
	static std::atomic<uint32_t> nextID;
	friend class BufferView;	// ImageView/BufferView share the ID space above.

	bool                          imageTypesMatch(VkImageType imageType) const;
	const Image*                  getImage(Usage usage) const;

	Image *const                  image = nullptr;
	const VkImageViewType         viewType = VK_IMAGE_VIEW_TYPE_2D;
	const Format                  format;
	const VkComponentMapping      components = {};
	const VkImageSubresourceRange subresourceRange = {};

	const vk::SamplerYcbcrConversion *ycbcrConversion = nullptr;
};

// TODO(b/132437008): Also used by SamplerYcbcrConversion. Move somewhere centrally?
inline VkComponentMapping ResolveIdentityMapping(VkComponentMapping m)
{
	return {
			(m.r == VK_COMPONENT_SWIZZLE_IDENTITY) ? VK_COMPONENT_SWIZZLE_R : m.r,
			(m.g == VK_COMPONENT_SWIZZLE_IDENTITY) ? VK_COMPONENT_SWIZZLE_G : m.g,
			(m.b == VK_COMPONENT_SWIZZLE_IDENTITY) ? VK_COMPONENT_SWIZZLE_B : m.b,
			(m.a == VK_COMPONENT_SWIZZLE_IDENTITY) ? VK_COMPONENT_SWIZZLE_A : m.a,
		};
}

static inline ImageView* Cast(VkImageView object)
{
	return ImageView::Cast(object);
}

} // namespace vk

#endif // VK_IMAGE_VIEW_HPP_
