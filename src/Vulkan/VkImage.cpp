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
#include "VkImage.hpp"
#include "Device/Blitter.hpp"
#include "Device/Surface.hpp"
#include <cstring>

namespace vk
{

Image::Image(const VkImageCreateInfo* pCreateInfo, void* mem) :
	flags(pCreateInfo->flags),
	imageType(pCreateInfo->imageType),
	format(pCreateInfo->format),
	extent(pCreateInfo->extent),
	mipLevels(pCreateInfo->mipLevels),
	arrayLayers(pCreateInfo->arrayLayers),
	samples(pCreateInfo->samples),
	tiling(pCreateInfo->tiling)
{
	blitter = new sw::Blitter();
}

void Image::destroy(const VkAllocationCallbacks* pAllocator)
{
	delete blitter;
}

size_t Image::ComputeRequiredAllocationSize(const VkImageCreateInfo* pCreateInfo)
{
	return 0;
}

const VkMemoryRequirements Image::getMemoryRequirements() const
{
	VkMemoryRequirements memoryRequirements;
	memoryRequirements.alignment = vk::REQUIRED_MEMORY_ALIGNMENT;
	memoryRequirements.memoryTypeBits = vk::MEMORY_TYPE_GENERIC_BIT;
	memoryRequirements.size = getStorageSize(flags);
	return memoryRequirements;
}

void Image::bind(VkDeviceMemory pDeviceMemory, VkDeviceSize pMemoryOffset)
{
	deviceMemory = Cast(pDeviceMemory);
	memoryOffset = pMemoryOffset;
}

void Image::copyTo(VkImage dstImage, const VkImageCopy& pRegion)
{
	// Image copy does not perform any conversion, it simply copies memory from
	// an image to another image that has the same number of bytes per pixel.
	Image* dst = Cast(dstImage);
	int srcBytesPerTexel = bytesPerTexel(pRegion.srcSubresource.aspectMask);
	ASSERT(srcBytesPerTexel == dst->bytesPerTexel(pRegion.dstSubresource.aspectMask));

	if(!((pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
		 (pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
		 (pRegion.srcSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)) ||
		 (pRegion.srcSubresource.baseArrayLayer != 0) ||
		 (pRegion.srcSubresource.layerCount != 1))
	{
		UNIMPLEMENTED();
	}

	if(!((pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
		 (pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
		 (pRegion.dstSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)) ||
		 (pRegion.dstSubresource.baseArrayLayer != 0) ||
		 (pRegion.dstSubresource.layerCount != 1))
	{
		UNIMPLEMENTED();
	}

	const char* srcMem = static_cast<const char*>(getTexelPointer(pRegion.srcOffset, pRegion.srcSubresource));
	char* dstMem = static_cast<char*>(dst->getTexelPointer(pRegion.dstOffset, pRegion.dstSubresource));

	int srcRowPitchBytes = rowPitchBytes(pRegion.srcSubresource.aspectMask, pRegion.srcSubresource.mipLevel);
	int srcSlicePitchBytes = slicePitchBytes(pRegion.srcSubresource.aspectMask, pRegion.srcSubresource.mipLevel);
	int dstRowPitchBytes = dst->rowPitchBytes(pRegion.dstSubresource.aspectMask, pRegion.dstSubresource.mipLevel);
	int dstSlicePitchBytes = dst->slicePitchBytes(pRegion.dstSubresource.aspectMask, pRegion.dstSubresource.mipLevel);

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
		memcpy(dstMem, srcMem, pRegion.extent.width * srcBytesPerTexel);
	}
	else if(isEntireLine && isSinglePlane) // Copy one plane
	{
		memcpy(dstMem, srcMem, pRegion.extent.height * srcRowPitchBytes);
	}
	else if(isEntirePlane) // Copy multiple planes
	{
		memcpy(dstMem, srcMem, pRegion.extent.depth * srcSlicePitchBytes);
	}
	else if(isEntireLine) // Copy plane by plane
	{
		for(uint32_t z = 0; z < pRegion.extent.depth; z++, dstMem += dstSlicePitchBytes, srcMem += srcSlicePitchBytes)
		{
			memcpy(dstMem, srcMem, pRegion.extent.height * srcRowPitchBytes);
		}
	}
	else // Copy line by line
	{
		for(uint32_t z = 0; z < pRegion.extent.depth; z++)
		{
			for(uint32_t y = 0; y < pRegion.extent.height; y++, dstMem += dstRowPitchBytes, srcMem += srcRowPitchBytes)
			{
				memcpy(dstMem, srcMem, pRegion.extent.width * srcBytesPerTexel);
			}
		}
	}
}

void Image::copy(VkBuffer buffer, const VkBufferImageCopy& region, bool bufferIsSource)
{
	if(!((region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
	     (region.imageSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)))
	{
		UNIMPLEMENTED();
	}

	VkExtent3D mipLevelExtent = getMipLevelExtent(region.imageSubresource.mipLevel);
	int imageBytesPerTexel = bytesPerTexel(region.imageSubresource.aspectMask);
	int imageRowPitchBytes = rowPitchBytes(region.imageSubresource.aspectMask, region.imageSubresource.mipLevel);
	int imageSlicePitchBytes = slicePitchBytes(region.imageSubresource.aspectMask, region.imageSubresource.mipLevel);
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

	VkDeviceSize layerSize = getLayerSize(flags);
	char* bufferMemory = static_cast<char*>(Cast(buffer)->getOffsetPointer(region.bufferOffset));
	char* imageMemory = static_cast<char*>(deviceMemory->getOffsetPointer(
	                    getMemoryOffset(region.imageSubresource.aspectMask, region.imageSubresource.mipLevel,
	                                    region.imageSubresource.baseArrayLayer) +
	                    texelOffsetBytesInStorage(region.imageOffset, region.imageSubresource)));
	char* srcMemory = bufferIsSource ? bufferMemory : imageMemory;
	char* dstMemory = bufferIsSource ? imageMemory : bufferMemory;

	VkDeviceSize copySize = 0;
	if(isSingleLine)
	{
		copySize = region.imageExtent.width * imageBytesPerTexel;
	}
	else if(isEntireLine && isSinglePlane)
	{
		copySize = region.imageExtent.height * imageRowPitchBytes;
	}
	else if(isEntirePlane)
	{
		copySize = region.imageExtent.depth * imageSlicePitchBytes; // Copy multiple planes
	}
	else if(isEntireLine) // Copy plane by plane
	{
		copySize = region.imageExtent.height * imageRowPitchBytes;
	}
	else // Copy line by line
	{
		copySize = region.imageExtent.width * imageBytesPerTexel;
	}

	for(uint32_t i = 0; i < region.imageSubresource.layerCount; i++)
	{
		if(isSingleLine || (isEntireLine && isSinglePlane) || isEntirePlane)
		{
			memcpy(dstMemory, srcMemory, copySize);
		}
		else if(isEntireLine) // Copy plane by plane
		{
			for(uint32_t z = 0; z < region.imageExtent.depth; z++)
			{
				memcpy(dstMemory, srcMemory, copySize);
				srcMemory += srcSlicePitchBytes;
				dstMemory += dstSlicePitchBytes;
			}
		}
		else // Copy line by line
		{
			for(uint32_t z = 0; z < region.imageExtent.depth; z++)
			{
				for(uint32_t y = 0; y < region.imageExtent.height; y++)
				{
					memcpy(dstMemory, srcMemory, copySize);
					srcMemory += srcRowPitchBytes;
					dstMemory += dstRowPitchBytes;
				}
			}
		}

		srcMemory += layerSize;
		dstMemory += layerSize;
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
	return deviceMemory->getOffsetPointer(texelOffsetBytesInStorage(offset, subresource) +
	       getMemoryOffset(flags, subresource.mipLevel, subresource.baseArrayLayer));
}

VkDeviceSize Image::texelOffsetBytesInStorage(const VkOffset3D& offset, const VkImageSubresourceLayers& subresource) const
{
	return offset.z * slicePitchBytes(flags, subresource.mipLevel) +
	       offset.y * rowPitchBytes(flags, subresource.mipLevel) +
	       offset.x * bytesPerTexel(flags);
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

int Image::rowPitchBytes(const VkImageAspectFlags& flags, uint32_t mipLevel) const
{
	// Depth and Stencil pitch should be computed separately
	ASSERT((flags & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	return sw::Surface::pitchB(getMipLevelExtent(mipLevel).width, isCube() ? 1 : 0, getFormat(flags), false);
}

int Image::slicePitchBytes(const VkImageAspectFlags& flags, uint32_t mipLevel) const
{
	// Depth and Stencil slice should be computed separately
	ASSERT((flags & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	VkExtent3D mipLevelExtent = getMipLevelExtent(mipLevel);
	return sw::Surface::sliceB(mipLevelExtent.width, mipLevelExtent.height, isCube() ? 1 : 0, getFormat(flags), false);
}

int Image::bytesPerTexel(const VkImageAspectFlags& flags) const
{
	// Depth and Stencil bytes should be computed separately
	ASSERT((flags & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	                (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));
	return sw::Surface::bytes(getFormat(flags));
}

VkFormat Image::getFormat(const VkImageAspectFlags& flags) const
{
	switch(flags)
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

VkDeviceSize Image::getMemoryOffset(const VkImageAspectFlags& flags) const
{
	switch(format)
	{
	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		if(flags == VK_IMAGE_ASPECT_STENCIL_BIT)
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

VkDeviceSize Image::getMemoryOffset(const VkImageAspectFlags& flags, uint32_t mipLevel) const
{
	VkDeviceSize offset = getMemoryOffset(flags);
	for(uint32_t i = 0; i < mipLevel; ++i)
	{
		offset += getMipLevelSize(flags, i);
	}
	return offset;
}

VkDeviceSize Image::getMemoryOffset(const VkImageAspectFlags& flags, uint32_t mipLevel, uint32_t layer) const
{
	return layer * getLayerSize(flags) + getMemoryOffset(flags, mipLevel);
}

VkDeviceSize Image::getMipLevelSize(const VkImageAspectFlags& flags, uint32_t mipLevel) const
{
	int slicePitchB = 0;
	if(sw::Surface::isDepth(format) && sw::Surface::isStencil(format))
	{
		switch(flags)
		{
		case VK_IMAGE_ASPECT_DEPTH_BIT:
		case VK_IMAGE_ASPECT_STENCIL_BIT:
			slicePitchB = slicePitchBytes(flags, mipLevel);
			break;
		default:
			// Allow allocating both depth and stencil contiguously
			slicePitchB = (slicePitchBytes(VK_IMAGE_ASPECT_DEPTH_BIT, mipLevel) +
			               slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, mipLevel));
			break;
		}
	}
	else
	{
		slicePitchB = slicePitchBytes(flags, mipLevel);
	}

	return getMipLevelExtent(mipLevel).depth * slicePitchB;
}

VkDeviceSize Image::getLayerSize(const VkImageAspectFlags& flags) const
{
	VkDeviceSize layerSize = 0;

	for(uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
	{
		layerSize += getMipLevelSize(flags, mipLevel);
	}

	return layerSize;
}

VkDeviceSize Image::getStorageSize(const VkImageAspectFlags& flags) const
{
	return arrayLayers * getLayerSize(flags);
}

sw::Surface* Image::asSurface(const VkImageAspectFlags& flags, uint32_t mipLevel, uint32_t layer) const
{
	VkExtent3D mipLevelExtent = getMipLevelExtent(mipLevel);
	return sw::Surface::create(mipLevelExtent.width, mipLevelExtent.height, mipLevelExtent.depth, getFormat(flags),
	                           deviceMemory->getOffsetPointer(getMemoryOffset(flags, mipLevel, layer)),
	                           rowPitchBytes(flags, mipLevel), slicePitchBytes(flags, mipLevel));
}

void Image::blit(VkImage dstImage, const VkImageBlit& region, VkFilter filter)
{
	VkImageAspectFlags srcFlags = region.srcSubresource.aspectMask;
	VkImageAspectFlags dstFlags = region.dstSubresource.aspectMask;
	if((region.srcSubresource.baseArrayLayer != 0) ||
	   (region.dstSubresource.baseArrayLayer != 0) ||
	   (region.srcSubresource.layerCount != 1) ||
	   (region.dstSubresource.layerCount != 1) ||
	   (srcFlags != dstFlags))
	{
		UNIMPLEMENTED();
	}

	int32_t numSlices = (region.srcOffsets[1].z - region.srcOffsets[0].z);
	ASSERT(numSlices == (region.dstOffsets[1].z - region.dstOffsets[0].z));

	sw::Surface* srcSurface = asSurface(srcFlags, region.srcSubresource.mipLevel, 0);
	sw::Surface* dstSurface = Cast(dstImage)->asSurface(dstFlags, region.dstSubresource.mipLevel, 0);

	sw::SliceRectF sRect(static_cast<float>(region.srcOffsets[0].x), static_cast<float>(region.srcOffsets[0].y),
	                     static_cast<float>(region.srcOffsets[1].x), static_cast<float>(region.srcOffsets[1].y),
	                     region.srcOffsets[0].z);

	sw::SliceRect dRect(region.dstOffsets[0].x, region.dstOffsets[0].y,
	                    region.dstOffsets[1].x, region.dstOffsets[1].y, region.dstOffsets[0].z);

	for(int i = 0; i < numSlices; i++)
	{
		blitter->blit(srcSurface, sRect, dstSurface, dRect,
		              {filter != VK_FILTER_NEAREST, srcFlags == VK_IMAGE_ASPECT_STENCIL_BIT, false});
		sRect.slice++;
		dRect.slice++;
	}

	delete srcSurface;
	delete dstSurface;
}

VkFormat Image::getClearFormat() const
{
	// Set the proper format for the clear value, as described here:
	// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#clears-values
	if(sw::Surface::isSignedNonNormalizedInteger(format))
	{
		return VK_FORMAT_R32G32B32A32_SINT;
	}
	else if(sw::Surface::isUnsignedNonNormalizedInteger(format))
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

void Image::clear(void* pixelData, VkFormat format, const VkImageSubresourceRange& subresourceRange, VkImageAspectFlags aspectMask)
{
	uint32_t firstLayer = subresourceRange.baseArrayLayer;
	uint32_t lastLayer = getLastLayerIndex(subresourceRange);
	for(uint32_t layer = firstLayer; layer <= lastLayer; ++layer)
	{
		uint32_t lastLevel = getLastMipLevel(subresourceRange);
		for(uint32_t mipLevel = subresourceRange.baseMipLevel; mipLevel <= lastLevel; ++mipLevel)
		{
			VkExtent3D mipLevelExtent = getMipLevelExtent(mipLevel);
			for(uint32_t s = 0; s < mipLevelExtent.depth; ++s)
			{
				const sw::SliceRect dRect(0, 0, mipLevelExtent.width, mipLevelExtent.height, s);
				sw::Surface* surface = asSurface(aspectMask, mipLevel, layer);
				blitter->clear(pixelData, format, surface, dRect, 0xF);
				delete surface;
			}
		}
	}
}

void Image::clear(const VkClearColorValue& color, const VkImageSubresourceRange& subresourceRange)
{
	if(!(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT))
	{
		UNIMPLEMENTED();
	}

	clear((void*)color.float32, getClearFormat(), subresourceRange, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Image::clear(const VkClearDepthStencilValue& color, const VkImageSubresourceRange& subresourceRange)
{
	if((subresourceRange.aspectMask & ~(VK_IMAGE_ASPECT_DEPTH_BIT |
	                                    VK_IMAGE_ASPECT_STENCIL_BIT)) != 0)
	{
		UNIMPLEMENTED();
	}

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
	{
		clear((void*)(&color.depth), VK_FORMAT_D32_SFLOAT, subresourceRange, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
	{
		clear((void*)(&color.stencil), VK_FORMAT_S8_UINT, subresourceRange, VK_IMAGE_ASPECT_STENCIL_BIT);
	}
}

void Image::clear(const VkClearValue& clearValue, const VkRect2D& renderArea, const VkImageSubresourceRange& subresourceRange)
{
	if(!((subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (subresourceRange.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT |
	                                     VK_IMAGE_ASPECT_STENCIL_BIT))) ||
	   (subresourceRange.baseMipLevel != 0) ||
	   (subresourceRange.levelCount != 1) ||
	   (renderArea.offset.x != 0) ||
	   (renderArea.offset.y != 0) ||
	   (renderArea.extent.width != extent.width) ||
	   (renderArea.extent.height != extent.height))
	{
		UNIMPLEMENTED();
	}

	if(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		clear(clearValue.color, subresourceRange);
	}
	else
	{
		clear(clearValue.depthStencil, subresourceRange);
	}
}

} // namespace vk