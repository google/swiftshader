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

#include "VkDeviceMemory.hpp"
#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "VkImage.hpp"
#include "Device/Blitter.hpp"
#include <cstring>

namespace
{
	VkImageAspectFlags GetAspects(vk::Format format)
	{
		// TODO: probably just flatten this out to a full format list, and alter
		// isDepth / isStencil etc to check for their aspect

		VkImageAspectFlags aspects = 0;
		if (format.isDepth()) aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (format.isStencil()) aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;

		// TODO: YCbCr planar formats have different aspects

		// Anything else is "color".
		if (!aspects) aspects |= VK_IMAGE_ASPECT_COLOR_BIT;
		return aspects;
	}
}

namespace vk
{

Image::Image(const Image::CreateInfo* pCreateInfo, void* mem) :
	device(Cast(pCreateInfo->device)),
	flags(pCreateInfo->pCreateInfo->flags),
	imageType(pCreateInfo->pCreateInfo->imageType),
	format(pCreateInfo->pCreateInfo->format),
	extent(pCreateInfo->pCreateInfo->extent),
	mipLevels(pCreateInfo->pCreateInfo->mipLevels),
	arrayLayers(pCreateInfo->pCreateInfo->arrayLayers),
	samples(pCreateInfo->pCreateInfo->samples),
	tiling(pCreateInfo->pCreateInfo->tiling)
{
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		UNIMPLEMENTED("Multisample images not yet supported");
	}
}

void Image::destroy(const VkAllocationCallbacks* pAllocator)
{
}

size_t Image::ComputeRequiredAllocationSize(const Image::CreateInfo* pCreateInfo)
{
	return 0;
}

const VkMemoryRequirements Image::getMemoryRequirements() const
{
	VkMemoryRequirements memoryRequirements;
	memoryRequirements.alignment = vk::REQUIRED_MEMORY_ALIGNMENT;
	memoryRequirements.memoryTypeBits = vk::MEMORY_TYPE_GENERIC_BIT;
	memoryRequirements.size = getStorageSize(GetAspects(format));
	return memoryRequirements;
}

void Image::bind(VkDeviceMemory pDeviceMemory, VkDeviceSize pMemoryOffset)
{
	deviceMemory = Cast(pDeviceMemory);
	memoryOffset = pMemoryOffset;
}

void Image::getSubresourceLayout(const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) const
{
	// By spec, aspectMask has a single bit set.
	if (!((pSubresource->aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
		  (pSubresource->aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
		  (pSubresource->aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)))
	{
		UNIMPLEMENTED("aspectMask");
	}
	auto aspect = static_cast<VkImageAspectFlagBits>(pSubresource->aspectMask);
	pLayout->offset = getMemoryOffset(aspect, pSubresource->mipLevel, pSubresource->arrayLayer);
	pLayout->size = getMipLevelSize(aspect, pSubresource->mipLevel);
	pLayout->rowPitch = rowPitchBytes(aspect, pSubresource->mipLevel);
	pLayout->depthPitch = slicePitchBytes(aspect, pSubresource->mipLevel);
	pLayout->arrayPitch = getLayerSize(aspect);
}

void Image::copyTo(VkImage dstImage, const VkImageCopy& pRegion)
{
	// Image copy does not perform any conversion, it simply copies memory from
	// an image to another image that has the same number of bytes per pixel.
	Image* dst = Cast(dstImage);

	if(!((pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
		 (pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
		 (pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)) ||
		 (pRegion.srcSubresource.baseArrayLayer != 0) ||
		 (pRegion.srcSubresource.layerCount != 1))
	{
		UNIMPLEMENTED("srcSubresource");
	}

	if(!((pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
		 (pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
		 (pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)) ||
		 (pRegion.dstSubresource.baseArrayLayer != 0) ||
		 (pRegion.dstSubresource.layerCount != 1))
	{
		UNIMPLEMENTED("dstSubresource");
	}

	VkImageAspectFlagBits srcAspect = static_cast<VkImageAspectFlagBits>(pRegion.srcSubresource.aspectMask);
	VkImageAspectFlagBits dstAspect = static_cast<VkImageAspectFlagBits>(pRegion.dstSubresource.aspectMask);

	int srcBytesPerTexel = bytesPerTexel(srcAspect);
	ASSERT(srcBytesPerTexel == dst->bytesPerTexel(dstAspect));

	const uint8_t* srcMem = static_cast<const uint8_t*>(getTexelPointer(pRegion.srcOffset, pRegion.srcSubresource));
	uint8_t* dstMem = static_cast<uint8_t*>(dst->getTexelPointer(pRegion.dstOffset, pRegion.dstSubresource));

	int srcRowPitchBytes = rowPitchBytes(srcAspect, pRegion.srcSubresource.mipLevel);
	int srcSlicePitchBytes = slicePitchBytes(srcAspect, pRegion.srcSubresource.mipLevel);
	int dstRowPitchBytes = dst->rowPitchBytes(dstAspect, pRegion.dstSubresource.mipLevel);
	int dstSlicePitchBytes = dst->slicePitchBytes(dstAspect, pRegion.dstSubresource.mipLevel);

	VkExtent3D srcExtent = getMipLevelExtent(pRegion.srcSubresource.mipLevel);
	VkExtent3D dstExtent = dst->getMipLevelExtent(pRegion.dstSubresource.mipLevel);

	bool isSinglePlane = (pRegion.extent.depth == 1);
	bool isSingleLine  = (pRegion.extent.height == 1) && isSinglePlane;
	// In order to copy multiple lines using a single memcpy call, we
	// have to make sure that we need to copy the entire line and that
	// both source and destination lines have the same length in bytes
	bool isEntireLine  = (pRegion.extent.width == srcExtent.width) &&
	                     (pRegion.extent.width == dstExtent.width) &&
	                     (srcRowPitchBytes == dstRowPitchBytes);
	// In order to copy multiple planes using a single memcpy call, we
	// have to make sure that we need to copy the entire plane and that
	// both source and destination planes have the same length in bytes
	bool isEntirePlane = isEntireLine &&
	                     (pRegion.extent.height == srcExtent.height) &&
	                     (pRegion.extent.height == dstExtent.height) &&
	                     (srcSlicePitchBytes == dstSlicePitchBytes);

	if(isSingleLine) // Copy one line
	{
		size_t copySize = pRegion.extent.width * srcBytesPerTexel;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dst->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntireLine && isSinglePlane) // Copy one plane
	{
		size_t copySize = pRegion.extent.height * srcRowPitchBytes;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dst->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntirePlane) // Copy multiple planes
	{
		size_t copySize = pRegion.extent.depth * srcSlicePitchBytes;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dst->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntireLine) // Copy plane by plane
	{
		size_t copySize = pRegion.extent.height * srcRowPitchBytes;

		for(uint32_t z = 0; z < pRegion.extent.depth; z++, dstMem += dstSlicePitchBytes, srcMem += srcSlicePitchBytes)
		{
			ASSERT((srcMem + copySize) < end());
			ASSERT((dstMem + copySize) < dst->end());
			memcpy(dstMem, srcMem, copySize);
		}
	}
	else // Copy line by line
	{
		size_t copySize = pRegion.extent.width * srcBytesPerTexel;

		for(uint32_t z = 0; z < pRegion.extent.depth; z++)
		{
			for(uint32_t y = 0; y < pRegion.extent.height; y++, dstMem += dstRowPitchBytes, srcMem += srcRowPitchBytes)
			{
				ASSERT((srcMem + copySize) < end());
				ASSERT((dstMem + copySize) < dst->end());
				memcpy(dstMem, srcMem, copySize);
			}
		}
	}
}

void Image::copy(VkBuffer buf, const VkBufferImageCopy& region, bool bufferIsSource)
{
	if(!((region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
	     (region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)))
	{
		UNIMPLEMENTED("imageSubresource");
	}

	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(region.imageSubresource.aspectMask);

	VkExtent3D mipLevelExtent = getMipLevelExtent(region.imageSubresource.mipLevel);
	int imageBytesPerTexel = bytesPerTexel(aspect);
	int imageRowPitchBytes = rowPitchBytes(aspect, region.imageSubresource.mipLevel);
	int imageSlicePitchBytes = slicePitchBytes(aspect, region.imageSubresource.mipLevel);
	int bufferRowPitchBytes = ((region.bufferRowLength == 0) ? region.imageExtent.width : region.bufferRowLength) *
	                          imageBytesPerTexel;
	int bufferSlicePitchBytes = (((region.bufferImageHeight == 0) || (region.bufferRowLength == 0))) ?
                                region.imageExtent.height * bufferRowPitchBytes :
	                            (region.bufferImageHeight * region.bufferRowLength) * imageBytesPerTexel;

	int srcSlicePitchBytes = bufferIsSource ? bufferSlicePitchBytes : imageSlicePitchBytes;
	int dstSlicePitchBytes = bufferIsSource ? imageSlicePitchBytes : bufferSlicePitchBytes;
	int srcRowPitchBytes = bufferIsSource ? bufferRowPitchBytes : imageRowPitchBytes;
	int dstRowPitchBytes = bufferIsSource ? imageRowPitchBytes : bufferRowPitchBytes;

	bool isSinglePlane = (region.imageExtent.depth == 1);
	bool isSingleLine  = (region.imageExtent.height == 1) && isSinglePlane;
	bool isEntireLine  = (region.imageExtent.width == mipLevelExtent.width) &&
	                     (imageRowPitchBytes == bufferRowPitchBytes);
	bool isEntirePlane = isEntireLine && (region.imageExtent.height == mipLevelExtent.height) &&
	                     (imageSlicePitchBytes == bufferSlicePitchBytes);

	Buffer* buffer = Cast(buf);
	uint8_t* bufferMemory = static_cast<uint8_t*>(buffer->getOffsetPointer(region.bufferOffset));
	uint8_t* imageMemory = static_cast<uint8_t*>(deviceMemory->getOffsetPointer(
	                       getMemoryOffset(aspect, region.imageSubresource.mipLevel,
	                                       region.imageSubresource.baseArrayLayer) +
	                       texelOffsetBytesInStorage(region.imageOffset, region.imageSubresource)));
	uint8_t* srcMemory = bufferIsSource ? bufferMemory : imageMemory;
	uint8_t* dstMemory = bufferIsSource ? imageMemory : bufferMemory;

	VkDeviceSize copySize = 0;
	VkDeviceSize bufferLayerSize = 0;
	if(isSingleLine)
	{
		copySize = region.imageExtent.width * imageBytesPerTexel;
		bufferLayerSize = copySize;
	}
	else if(isEntireLine && isSinglePlane)
	{
		copySize = region.imageExtent.height * imageRowPitchBytes;
		bufferLayerSize = copySize;
	}
	else if(isEntirePlane)
	{
		copySize = region.imageExtent.depth * imageSlicePitchBytes; // Copy multiple planes
		bufferLayerSize = copySize;
	}
	else if(isEntireLine) // Copy plane by plane
	{
		copySize = region.imageExtent.height * imageRowPitchBytes;
		bufferLayerSize = copySize * region.imageExtent.depth;
	}
	else // Copy line by line
	{
		copySize = region.imageExtent.width * imageBytesPerTexel;
		bufferLayerSize = copySize * region.imageExtent.depth * region.imageExtent.height;
	}

	VkDeviceSize imageLayerSize = getLayerSize(aspect);
	VkDeviceSize srcLayerSize = bufferIsSource ? bufferLayerSize : imageLayerSize;
	VkDeviceSize dstLayerSize = bufferIsSource ? imageLayerSize : bufferLayerSize;

	for(uint32_t i = 0; i < region.imageSubresource.layerCount; i++)
	{
		if(isSingleLine || (isEntireLine && isSinglePlane) || isEntirePlane)
		{
			ASSERT(((bufferIsSource ? dstMemory : srcMemory) + copySize) < end());
			ASSERT(((bufferIsSource ? srcMemory : dstMemory) + copySize) < buffer->end());
			memcpy(dstMemory, srcMemory, copySize);
		}
		else if(isEntireLine) // Copy plane by plane
		{
			uint8_t* srcPlaneMemory = srcMemory;
			uint8_t* dstPlaneMemory = dstMemory;
			for(uint32_t z = 0; z < region.imageExtent.depth; z++)
			{
				ASSERT(((bufferIsSource ? dstPlaneMemory : srcPlaneMemory) + copySize) < end());
				ASSERT(((bufferIsSource ? srcPlaneMemory : dstPlaneMemory) + copySize) < buffer->end());
				memcpy(dstPlaneMemory, srcPlaneMemory, copySize);
				srcPlaneMemory += srcSlicePitchBytes;
				dstPlaneMemory += dstSlicePitchBytes;
			}
		}
		else // Copy line by line
		{
			uint8_t* srcLayerMemory = srcMemory;
			uint8_t* dstLayerMemory = dstMemory;
			for(uint32_t z = 0; z < region.imageExtent.depth; z++)
			{
				uint8_t* srcPlaneMemory = srcLayerMemory;
				uint8_t* dstPlaneMemory = dstLayerMemory;
				for(uint32_t y = 0; y < region.imageExtent.height; y++)
				{
					ASSERT(((bufferIsSource ? dstPlaneMemory : srcPlaneMemory) + copySize) < end());
					ASSERT(((bufferIsSource ? srcPlaneMemory : dstPlaneMemory) + copySize) < buffer->end());
					memcpy(dstPlaneMemory, srcPlaneMemory, copySize);
					srcPlaneMemory += srcRowPitchBytes;
					dstPlaneMemory += dstRowPitchBytes;
				}
				srcLayerMemory += srcSlicePitchBytes;
				dstLayerMemory += dstSlicePitchBytes;
			}
		}

		srcMemory += srcLayerSize;
		dstMemory += dstLayerSize;
	}
}

void Image::copyTo(VkBuffer dstBuffer, const VkBufferImageCopy& region)
{
	copy(dstBuffer, region, false);
}

void Image::copyFrom(VkBuffer srcBuffer, const VkBufferImageCopy& region)
{
	copy(srcBuffer, region, true);
}

void* Image::getTexelPointer(const VkOffset3D& offset, const VkImageSubresourceLayers& subresource) const
{
	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresource.aspectMask);
	return deviceMemory->getOffsetPointer(texelOffsetBytesInStorage(offset, subresource) +
	       getMemoryOffset(aspect, subresource.mipLevel, subresource.baseArrayLayer));
}

VkDeviceSize Image::texelOffsetBytesInStorage(const VkOffset3D& offset, const VkImageSubresourceLayers& subresource) const
{
	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresource.aspectMask);
	return offset.z * slicePitchBytes(aspect, subresource.mipLevel) +
	       (offset.y + (isCube() ? 1 : 0)) * rowPitchBytes(aspect, subresource.mipLevel) +
	       (offset.x + (isCube() ? 1 : 0)) * bytesPerTexel(aspect);
}

VkExtent3D Image::getMipLevelExtent(uint32_t mipLevel) const
{
	VkExtent3D mipLevelExtent;
	mipLevelExtent.width = extent.width >> mipLevel;
	mipLevelExtent.height = extent.height >> mipLevel;
	mipLevelExtent.depth = extent.depth >> mipLevel;

	if(mipLevelExtent.width == 0)
	{
		mipLevelExtent.width = 1;
	}
	if(mipLevelExtent.height == 0)
	{
		mipLevelExtent.height = 1;
	}
	if(mipLevelExtent.depth == 0)
	{
		mipLevelExtent.depth = 1;
	}
	return mipLevelExtent;
}

int Image::rowPitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	// Depth and Stencil pitch should be computed separately
	ASSERT((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	return getFormat(aspect).pitchB(getMipLevelExtent(mipLevel).width, isCube() ? 1 : 0, false);
}

int Image::slicePitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	// Depth and Stencil slice should be computed separately
	ASSERT((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	VkExtent3D mipLevelExtent = getMipLevelExtent(mipLevel);
	return getFormat(aspect).sliceB(mipLevelExtent.width, mipLevelExtent.height, isCube() ? 1 : 0, false);
}

int Image::bytesPerTexel(VkImageAspectFlagBits aspect) const
{
	// Depth and Stencil bytes should be computed separately
	ASSERT((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	return getFormat(aspect).bytes();
}

Format Image::getFormat(VkImageAspectFlagBits aspect) const
{
	switch(aspect)
	{
	case VK_IMAGE_ASPECT_DEPTH_BIT:
		switch(format)
		{
		case VK_FORMAT_D16_UNORM_S8_UINT:
			return VK_FORMAT_D16_UNORM;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return VK_FORMAT_X8_D24_UNORM_PACK32; // FIXME: This will allocate an extra byte per pixel
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_FORMAT_D32_SFLOAT;
		default:
			break;
		}
		break;
	case VK_IMAGE_ASPECT_STENCIL_BIT:
		switch(format)
		{
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_FORMAT_S8_UINT;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return format;
}

bool Image::isCube() const
{
	return (flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) && (imageType == VK_IMAGE_TYPE_2D);
}

uint8_t* Image::end() const
{
	return reinterpret_cast<uint8_t*>(deviceMemory->getOffsetPointer(deviceMemory->getCommittedMemoryInBytes() + 1));
}

VkDeviceSize Image::getMemoryOffset(VkImageAspectFlagBits aspect) const
{
	switch(format)
	{
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		if(aspect == VK_IMAGE_ASPECT_STENCIL_BIT)
		{
			// Offset by depth buffer to get to stencil buffer
			return memoryOffset + getStorageSize(VK_IMAGE_ASPECT_DEPTH_BIT);
		}
		break;
	default:
		break;
	}

	return memoryOffset;
}

VkDeviceSize Image::getMemoryOffset(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	VkDeviceSize offset = getMemoryOffset(aspect);
	for(uint32_t i = 0; i < mipLevel; ++i)
	{
		offset += getMipLevelSize(aspect, i);
	}
	return offset;
}

VkDeviceSize Image::getMemoryOffset(VkImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t layer) const
{
	return layer * getLayerSize(aspect) + getMemoryOffset(aspect, mipLevel);
}

VkDeviceSize Image::getMipLevelSize(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	return getMipLevelExtent(mipLevel).depth * slicePitchBytes(aspect, mipLevel);
}

VkDeviceSize Image::getLayerSize(VkImageAspectFlagBits aspect) const
{
	VkDeviceSize layerSize = 0;

	for(uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
	{
		layerSize += getMipLevelSize(aspect, mipLevel);
	}

	return layerSize;
}

VkDeviceSize Image::getStorageSize(VkImageAspectFlags aspectMask) const
{
	if (aspectMask == (VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		return arrayLayers * (getLayerSize(VK_IMAGE_ASPECT_DEPTH_BIT) + getLayerSize(VK_IMAGE_ASPECT_STENCIL_BIT));
	}
	return arrayLayers * getLayerSize(static_cast<VkImageAspectFlagBits>(aspectMask));
}

void Image::blit(VkImage dstImage, const VkImageBlit& region, VkFilter filter)
{
	device->getBlitter()->blit(this, Cast(dstImage), region, filter);
}

VkFormat Image::getClearFormat() const
{
	// Set the proper format for the clear value, as described here:
	// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#clears-values
	if(format.isSignedNonNormalizedInteger())
	{
		return VK_FORMAT_R32G32B32A32_SINT;
	}
	else if(format.isUnsignedNonNormalizedInteger())
	{
		return VK_FORMAT_R32G32B32A32_UINT;
	}

	return VK_FORMAT_R32G32B32A32_SFLOAT;
}

uint32_t Image::getLastLayerIndex(const VkImageSubresourceRange& subresourceRange) const
{
	return ((subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS) ?
	        arrayLayers : (subresourceRange.baseArrayLayer + subresourceRange.layerCount)) - 1;
}

uint32_t Image::getLastMipLevel(const VkImageSubresourceRange& subresourceRange) const
{
	return ((subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS) ?
	        mipLevels : (subresourceRange.baseMipLevel + subresourceRange.levelCount)) - 1;
}

void Image::clear(void* pixelData, VkFormat format, const VkImageSubresourceRange& subresourceRange, const VkRect2D& renderArea)
{
	if((subresourceRange.baseMipLevel != 0) ||
	   (subresourceRange.levelCount != 1))
	{
		UNIMPLEMENTED("subresourceRange");
	}

	device->getBlitter()->clear(pixelData, format, this, subresourceRange, &renderArea);
}

void Image::clear(const VkClearColorValue& color, const VkImageSubresourceRange& subresourceRange)
{
	if(!(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT))
	{
		UNIMPLEMENTED("aspectMask");
	}

	device->getBlitter()->clear((void*)color.float32, getClearFormat(), this, subresourceRange);
}

void Image::clear(const VkClearDepthStencilValue& color, const VkImageSubresourceRange& subresourceRange)
{
	if((subresourceRange.aspectMask & ~(VK_IMAGE_ASPECT_DEPTH_BIT |
	                                    VK_IMAGE_ASPECT_STENCIL_BIT)) != 0)
	{
		UNIMPLEMENTED("aspectMask");
	}

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
	{
		VkImageSubresourceRange depthSubresourceRange = subresourceRange;
		depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		device->getBlitter()->clear((void*)(&color.depth), VK_FORMAT_D32_SFLOAT, this, depthSubresourceRange);
	}

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
	{
		VkImageSubresourceRange stencilSubresourceRange = subresourceRange;
		stencilSubresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		device->getBlitter()->clear((void*)(&color.stencil), VK_FORMAT_S8_UINT, this, stencilSubresourceRange);
	}
}

void Image::clear(const VkClearValue& clearValue, const VkRect2D& renderArea, const VkImageSubresourceRange& subresourceRange)
{
	if(!((subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (subresourceRange.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT |
	                                     VK_IMAGE_ASPECT_STENCIL_BIT))) ||
	   (subresourceRange.baseMipLevel != 0) ||
	   (subresourceRange.levelCount != 1))
	{
		UNIMPLEMENTED("subresourceRange");
	}

	if(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		clear((void*)(clearValue.color.float32), getClearFormat(), subresourceRange, renderArea);
	}
	else
	{
		if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
		{
			VkImageSubresourceRange depthSubresourceRange = subresourceRange;
			depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			clear((void*)(&clearValue.depthStencil.depth), VK_FORMAT_D32_SFLOAT, depthSubresourceRange, renderArea);
		}

		if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
		{
			VkImageSubresourceRange stencilSubresourceRange = subresourceRange;
			stencilSubresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
			clear((void*)(&clearValue.depthStencil.stencil), VK_FORMAT_S8_UINT, stencilSubresourceRange, renderArea);
		}
	}
}

} // namespace vk
