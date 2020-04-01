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

#include "VkImage.hpp"

#include "VkBuffer.hpp"
#include "VkDevice.hpp"
#include "VkDeviceMemory.hpp"
#include "Device/ASTC_Decoder.hpp"
#include "Device/BC_Decoder.hpp"
#include "Device/Blitter.hpp"
#include "Device/ETC_Decoder.hpp"

#ifdef __ANDROID__
#	include "System/GrallocAndroid.hpp"
#endif

#include <cstring>

namespace {

ETC_Decoder::InputType GetInputType(const vk::Format &format)
{
	switch(format)
	{
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
			return ETC_Decoder::ETC_R_UNSIGNED;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK:
			return ETC_Decoder::ETC_R_SIGNED;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
			return ETC_Decoder::ETC_RG_UNSIGNED;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
			return ETC_Decoder::ETC_RG_SIGNED;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
			return ETC_Decoder::ETC_RGB;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
			return ETC_Decoder::ETC_RGB_PUNCHTHROUGH_ALPHA;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
			return ETC_Decoder::ETC_RGBA;
		default:
			UNSUPPORTED("format: %d", int(format));
			return ETC_Decoder::ETC_RGBA;
	}
}

int GetBCn(const vk::Format &format)
{
	switch(format)
	{
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
			return 1;
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
			return 2;
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
			return 3;
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
			return 4;
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
			return 5;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
			return 6;
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
			return 7;
		default:
			UNSUPPORTED("format: %d", int(format));
			return 0;
	}
}

// Returns true for BC1 if we have an RGB format, false for RGBA
// Returns true for BC4 and BC5 if we have an unsigned format, false for signed
// Ignored by BC2, BC3, BC6 and BC7
bool GetNoAlphaOrUnsigned(const vk::Format &format)
{
	switch(format)
	{
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
			return true;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
			return false;
		default:
			UNSUPPORTED("format: %d", int(format));
			return false;
	}
}

}  // anonymous namespace

namespace vk {

Image::Image(const VkImageCreateInfo *pCreateInfo, void *mem, Device *device)
    : device(device)
    , flags(pCreateInfo->flags)
    , imageType(pCreateInfo->imageType)
    , format(pCreateInfo->format)
    , extent(pCreateInfo->extent)
    , mipLevels(pCreateInfo->mipLevels)
    , arrayLayers(pCreateInfo->arrayLayers)
    , samples(pCreateInfo->samples)
    , tiling(pCreateInfo->tiling)
    , usage(pCreateInfo->usage)
{
	if(format.isCompressed())
	{
		VkImageCreateInfo compressedImageCreateInfo = *pCreateInfo;
		compressedImageCreateInfo.format = format.getDecompressedFormat();
		decompressedImage = new(mem) Image(&compressedImageCreateInfo, nullptr, device);
	}

	const auto *nextInfo = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext);
	for(; nextInfo != nullptr; nextInfo = nextInfo->pNext)
	{
		if(nextInfo->sType == VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO)
		{
			const auto *externalInfo = reinterpret_cast<const VkExternalMemoryImageCreateInfo *>(nextInfo);
			supportedExternalMemoryHandleTypes = externalInfo->handleTypes;
		}
	}
}

void Image::destroy(const VkAllocationCallbacks *pAllocator)
{
	if(decompressedImage)
	{
		vk::deallocate(decompressedImage, pAllocator);
	}
}

size_t Image::ComputeRequiredAllocationSize(const VkImageCreateInfo *pCreateInfo)
{
	return Format(pCreateInfo->format).isCompressed() ? sizeof(Image) : 0;
}

const VkMemoryRequirements Image::getMemoryRequirements() const
{
	VkMemoryRequirements memoryRequirements;
	memoryRequirements.alignment = vk::REQUIRED_MEMORY_ALIGNMENT;
	memoryRequirements.memoryTypeBits = vk::MEMORY_TYPE_GENERIC_BIT;
	memoryRequirements.size = getStorageSize(format.getAspects()) +
	                          (decompressedImage ? decompressedImage->getStorageSize(decompressedImage->format.getAspects()) : 0);
	return memoryRequirements;
}

size_t Image::getSizeInBytes(const VkImageSubresourceRange &subresourceRange) const
{
	size_t size = 0;
	uint32_t lastLayer = getLastLayerIndex(subresourceRange);
	uint32_t lastMipLevel = getLastMipLevel(subresourceRange);
	uint32_t layerCount = lastLayer - subresourceRange.baseArrayLayer + 1;
	uint32_t mipLevelCount = lastMipLevel - subresourceRange.baseMipLevel + 1;

	auto aspect = static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask);

	if(layerCount > 1)
	{
		if(mipLevelCount < mipLevels)  // Compute size for all layers except the last one, then add relevant mip level sizes only for last layer
		{
			size = (layerCount - 1) * getLayerSize(aspect);
			for(uint32_t mipLevel = subresourceRange.baseMipLevel; mipLevel <= lastMipLevel; ++mipLevel)
			{
				size += getMultiSampledLevelSize(aspect, mipLevel);
			}
		}
		else  // All mip levels used, compute full layer sizes
		{
			size = layerCount * getLayerSize(aspect);
		}
	}
	else  // Single layer, add all mip levels in the subresource range
	{
		for(uint32_t mipLevel = subresourceRange.baseMipLevel; mipLevel <= lastMipLevel; ++mipLevel)
		{
			size += getMultiSampledLevelSize(aspect, mipLevel);
		}
	}

	return size;
}

bool Image::canBindToMemory(DeviceMemory *pDeviceMemory) const
{
	return pDeviceMemory->checkExternalMemoryHandleType(supportedExternalMemoryHandleTypes);
}

void Image::bind(DeviceMemory *pDeviceMemory, VkDeviceSize pMemoryOffset)
{
	deviceMemory = pDeviceMemory;
	memoryOffset = pMemoryOffset;
	if(decompressedImage)
	{
		decompressedImage->deviceMemory = deviceMemory;
		decompressedImage->memoryOffset = memoryOffset + getStorageSize(format.getAspects());
	}
}

#ifdef __ANDROID__
VkResult Image::prepareForExternalUseANDROID() const
{
	void *nativeBuffer = nullptr;
	VkExtent3D extent = getMipLevelExtent(VK_IMAGE_ASPECT_COLOR_BIT, 0);

	if(GrallocModule::getInstance()->lock(backingMemory.nativeHandle, GRALLOC_USAGE_SW_WRITE_OFTEN, 0, 0, extent.width, extent.height, &nativeBuffer) != 0)
	{
		return VK_ERROR_OUT_OF_DATE_KHR;
	}

	if(!nativeBuffer)
	{
		return VK_ERROR_OUT_OF_DATE_KHR;
	}

	int imageRowBytes = rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
	int bufferRowBytes = backingMemory.stride * getFormat().bytes();
	ASSERT(imageRowBytes <= bufferRowBytes);

	uint8_t *srcBuffer = static_cast<uint8_t *>(deviceMemory->getOffsetPointer(0));
	uint8_t *dstBuffer = static_cast<uint8_t *>(nativeBuffer);
	for(uint32_t i = 0; i < extent.height; i++)
	{
		memcpy(dstBuffer + (i * bufferRowBytes), srcBuffer + (i * imageRowBytes), imageRowBytes);
	}

	if(GrallocModule::getInstance()->unlock(backingMemory.nativeHandle) != 0)
	{
		return VK_ERROR_OUT_OF_DATE_KHR;
	}

	return VK_SUCCESS;
}

VkDeviceMemory Image::getExternalMemory() const
{
	return backingMemory.externalMemory ? *deviceMemory : VkDeviceMemory{ VK_NULL_HANDLE };
}
#endif

void Image::getSubresourceLayout(const VkImageSubresource *pSubresource, VkSubresourceLayout *pLayout) const
{
	// By spec, aspectMask has a single bit set.
	if(!((pSubresource->aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (pSubresource->aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
	     (pSubresource->aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) ||
	     (pSubresource->aspectMask == VK_IMAGE_ASPECT_PLANE_0_BIT) ||
	     (pSubresource->aspectMask == VK_IMAGE_ASPECT_PLANE_1_BIT) ||
	     (pSubresource->aspectMask == VK_IMAGE_ASPECT_PLANE_2_BIT)))
	{
		UNSUPPORTED("aspectMask %X", pSubresource->aspectMask);
	}

	auto aspect = static_cast<VkImageAspectFlagBits>(pSubresource->aspectMask);
	pLayout->offset = getMemoryOffset(aspect, pSubresource->mipLevel, pSubresource->arrayLayer);
	pLayout->size = getMultiSampledLevelSize(aspect, pSubresource->mipLevel);
	pLayout->rowPitch = rowPitchBytes(aspect, pSubresource->mipLevel);
	pLayout->depthPitch = slicePitchBytes(aspect, pSubresource->mipLevel);
	pLayout->arrayPitch = getLayerSize(aspect);
}

void Image::copyTo(Image *dstImage, const VkImageCopy &region) const
{
	// Image copy does not perform any conversion, it simply copies memory from
	// an image to another image that has the same number of bytes per pixel.

	if(!((region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
	     (region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) ||
	     (region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_0_BIT) ||
	     (region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_1_BIT) ||
	     (region.srcSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_2_BIT)))
	{
		UNSUPPORTED("srcSubresource.aspectMask %X", region.srcSubresource.aspectMask);
	}

	if(!((region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	     (region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) ||
	     (region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) ||
	     (region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_0_BIT) ||
	     (region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_1_BIT) ||
	     (region.dstSubresource.aspectMask == VK_IMAGE_ASPECT_PLANE_2_BIT)))
	{
		UNSUPPORTED("dstSubresource.aspectMask %X", region.dstSubresource.aspectMask);
	}

	VkImageAspectFlagBits srcAspect = static_cast<VkImageAspectFlagBits>(region.srcSubresource.aspectMask);
	VkImageAspectFlagBits dstAspect = static_cast<VkImageAspectFlagBits>(region.dstSubresource.aspectMask);

	Format srcFormat = getFormat(srcAspect);
	Format dstFormat = dstImage->getFormat(dstAspect);

	if((samples > VK_SAMPLE_COUNT_1_BIT) && (imageType == VK_IMAGE_TYPE_2D) && !format.isUnnormalizedInteger())
	{
		// Requires multisampling resolve
		VkImageBlit blitRegion;
		blitRegion.srcSubresource = region.srcSubresource;
		blitRegion.srcOffsets[0] = region.srcOffset;
		blitRegion.srcOffsets[1].x = blitRegion.srcOffsets[0].x + region.extent.width;
		blitRegion.srcOffsets[1].y = blitRegion.srcOffsets[0].y + region.extent.height;
		blitRegion.srcOffsets[1].z = blitRegion.srcOffsets[0].z + region.extent.depth;

		blitRegion.dstSubresource = region.dstSubresource;
		blitRegion.dstOffsets[0] = region.dstOffset;
		blitRegion.dstOffsets[1].x = blitRegion.dstOffsets[0].x + region.extent.width;
		blitRegion.dstOffsets[1].y = blitRegion.dstOffsets[0].y + region.extent.height;
		blitRegion.dstOffsets[1].z = blitRegion.dstOffsets[0].z + region.extent.depth;

		return device->getBlitter()->blit(this, dstImage, blitRegion, VK_FILTER_NEAREST);
	}

	int srcBytesPerBlock = srcFormat.bytesPerBlock();
	ASSERT(srcBytesPerBlock == dstFormat.bytesPerBlock());

	const uint8_t *srcMem = static_cast<const uint8_t *>(getTexelPointer(region.srcOffset, region.srcSubresource));
	uint8_t *dstMem = static_cast<uint8_t *>(dstImage->getTexelPointer(region.dstOffset, region.dstSubresource));

	int srcRowPitchBytes = rowPitchBytes(srcAspect, region.srcSubresource.mipLevel);
	int srcSlicePitchBytes = slicePitchBytes(srcAspect, region.srcSubresource.mipLevel);
	int dstRowPitchBytes = dstImage->rowPitchBytes(dstAspect, region.dstSubresource.mipLevel);
	int dstSlicePitchBytes = dstImage->slicePitchBytes(dstAspect, region.dstSubresource.mipLevel);

	VkExtent3D srcExtent = getMipLevelExtent(srcAspect, region.srcSubresource.mipLevel);
	VkExtent3D dstExtent = dstImage->getMipLevelExtent(dstAspect, region.dstSubresource.mipLevel);
	VkExtent3D copyExtent = imageExtentInBlocks(region.extent, srcAspect);

	bool isSinglePlane = (copyExtent.depth == 1);
	bool isSingleLine = (copyExtent.height == 1) && isSinglePlane;
	// In order to copy multiple lines using a single memcpy call, we
	// have to make sure that we need to copy the entire line and that
	// both source and destination lines have the same length in bytes
	bool isEntireLine = (region.extent.width == srcExtent.width) &&
	                    (region.extent.width == dstExtent.width) &&
	                    // For non compressed formats, blockWidth is 1. For compressed
	                    // formats, rowPitchBytes returns the number of bytes for a row of
	                    // blocks, so we have to divide by the block height, which means:
	                    // srcRowPitchBytes / srcBlockWidth == dstRowPitchBytes / dstBlockWidth
	                    // And, to avoid potential non exact integer division, for example if a
	                    // block has 16 bytes and represents 5 lines, we change the equation to:
	                    // srcRowPitchBytes * dstBlockWidth == dstRowPitchBytes * srcBlockWidth
	                    ((srcRowPitchBytes * dstFormat.blockWidth()) ==
	                     (dstRowPitchBytes * srcFormat.blockWidth()));
	// In order to copy multiple planes using a single memcpy call, we
	// have to make sure that we need to copy the entire plane and that
	// both source and destination planes have the same length in bytes
	bool isEntirePlane = isEntireLine &&
	                     (copyExtent.height == srcExtent.height) &&
	                     (copyExtent.height == dstExtent.height) &&
	                     (srcSlicePitchBytes == dstSlicePitchBytes);

	if(isSingleLine)  // Copy one line
	{
		size_t copySize = copyExtent.width * srcBytesPerBlock;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dstImage->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntireLine && isSinglePlane)  // Copy one plane
	{
		size_t copySize = copyExtent.height * srcRowPitchBytes;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dstImage->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntirePlane)  // Copy multiple planes
	{
		size_t copySize = copyExtent.depth * srcSlicePitchBytes;
		ASSERT((srcMem + copySize) < end());
		ASSERT((dstMem + copySize) < dstImage->end());
		memcpy(dstMem, srcMem, copySize);
	}
	else if(isEntireLine)  // Copy plane by plane
	{
		size_t copySize = copyExtent.height * srcRowPitchBytes;

		for(uint32_t z = 0; z < copyExtent.depth; z++, dstMem += dstSlicePitchBytes, srcMem += srcSlicePitchBytes)
		{
			ASSERT((srcMem + copySize) < end());
			ASSERT((dstMem + copySize) < dstImage->end());
			memcpy(dstMem, srcMem, copySize);
		}
	}
	else  // Copy line by line
	{
		size_t copySize = copyExtent.width * srcBytesPerBlock;

		for(uint32_t z = 0; z < copyExtent.depth; z++, dstMem += dstSlicePitchBytes, srcMem += srcSlicePitchBytes)
		{
			const uint8_t *srcSlice = srcMem;
			uint8_t *dstSlice = dstMem;
			for(uint32_t y = 0; y < copyExtent.height; y++, dstSlice += dstRowPitchBytes, srcSlice += srcRowPitchBytes)
			{
				ASSERT((srcSlice + copySize) < end());
				ASSERT((dstSlice + copySize) < dstImage->end());
				memcpy(dstSlice, srcSlice, copySize);
			}
		}
	}

	dstImage->prepareForSampling({ region.dstSubresource.aspectMask, region.dstSubresource.mipLevel, 1,
	                               region.dstSubresource.baseArrayLayer, region.dstSubresource.layerCount });
}

void Image::copy(Buffer *buffer, const VkBufferImageCopy &region, bool bufferIsSource)
{
	switch(region.imageSubresource.aspectMask)
	{
		case VK_IMAGE_ASPECT_COLOR_BIT:
		case VK_IMAGE_ASPECT_DEPTH_BIT:
		case VK_IMAGE_ASPECT_STENCIL_BIT:
		case VK_IMAGE_ASPECT_PLANE_0_BIT:
		case VK_IMAGE_ASPECT_PLANE_1_BIT:
		case VK_IMAGE_ASPECT_PLANE_2_BIT:
			break;
		default:
			UNSUPPORTED("aspectMask %x", int(region.imageSubresource.aspectMask));
			break;
	}

	auto aspect = static_cast<VkImageAspectFlagBits>(region.imageSubresource.aspectMask);
	Format copyFormat = getFormat(aspect);

	VkExtent3D imageExtent = imageExtentInBlocks(region.imageExtent, aspect);
	VkExtent2D bufferExtent = bufferExtentInBlocks({ imageExtent.width, imageExtent.height }, region);
	int bytesPerBlock = copyFormat.bytesPerBlock();
	int bufferRowPitchBytes = bufferExtent.width * bytesPerBlock;
	int bufferSlicePitchBytes = bufferExtent.height * bufferRowPitchBytes;

	uint8_t *bufferMemory = static_cast<uint8_t *>(buffer->getOffsetPointer(region.bufferOffset));
	uint8_t *imageMemory = static_cast<uint8_t *>(getTexelPointer(region.imageOffset, region.imageSubresource));
	uint8_t *srcMemory = bufferIsSource ? bufferMemory : imageMemory;
	uint8_t *dstMemory = bufferIsSource ? imageMemory : bufferMemory;
	int imageRowPitchBytes = rowPitchBytes(aspect, region.imageSubresource.mipLevel);
	int imageSlicePitchBytes = slicePitchBytes(aspect, region.imageSubresource.mipLevel);

	int srcSlicePitchBytes = bufferIsSource ? bufferSlicePitchBytes : imageSlicePitchBytes;
	int dstSlicePitchBytes = bufferIsSource ? imageSlicePitchBytes : bufferSlicePitchBytes;
	int srcRowPitchBytes = bufferIsSource ? bufferRowPitchBytes : imageRowPitchBytes;
	int dstRowPitchBytes = bufferIsSource ? imageRowPitchBytes : bufferRowPitchBytes;

	VkExtent3D mipLevelExtent = getMipLevelExtent(aspect, region.imageSubresource.mipLevel);
	bool isSinglePlane = (imageExtent.depth == 1);
	bool isSingleLine = (imageExtent.height == 1) && isSinglePlane;
	bool isEntireLine = (imageExtent.width == mipLevelExtent.width) &&
	                    (imageRowPitchBytes == bufferRowPitchBytes);
	bool isEntirePlane = isEntireLine && (imageExtent.height == mipLevelExtent.height) &&
	                     (imageSlicePitchBytes == bufferSlicePitchBytes);

	VkDeviceSize copySize = 0;
	VkDeviceSize bufferLayerSize = 0;
	if(isSingleLine)
	{
		copySize = imageExtent.width * bytesPerBlock;
		bufferLayerSize = copySize;
	}
	else if(isEntireLine && isSinglePlane)
	{
		copySize = imageExtent.height * imageRowPitchBytes;
		bufferLayerSize = copySize;
	}
	else if(isEntirePlane)
	{
		copySize = imageExtent.depth * imageSlicePitchBytes;  // Copy multiple planes
		bufferLayerSize = copySize;
	}
	else if(isEntireLine)  // Copy plane by plane
	{
		copySize = imageExtent.height * imageRowPitchBytes;
		bufferLayerSize = copySize * imageExtent.depth;
	}
	else  // Copy line by line
	{
		copySize = imageExtent.width * bytesPerBlock;
		bufferLayerSize = copySize * imageExtent.depth * imageExtent.height;
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
		else if(isEntireLine)  // Copy plane by plane
		{
			uint8_t *srcPlaneMemory = srcMemory;
			uint8_t *dstPlaneMemory = dstMemory;
			for(uint32_t z = 0; z < imageExtent.depth; z++)
			{
				ASSERT(((bufferIsSource ? dstPlaneMemory : srcPlaneMemory) + copySize) < end());
				ASSERT(((bufferIsSource ? srcPlaneMemory : dstPlaneMemory) + copySize) < buffer->end());
				memcpy(dstPlaneMemory, srcPlaneMemory, copySize);
				srcPlaneMemory += srcSlicePitchBytes;
				dstPlaneMemory += dstSlicePitchBytes;
			}
		}
		else  // Copy line by line
		{
			uint8_t *srcLayerMemory = srcMemory;
			uint8_t *dstLayerMemory = dstMemory;
			for(uint32_t z = 0; z < imageExtent.depth; z++)
			{
				uint8_t *srcPlaneMemory = srcLayerMemory;
				uint8_t *dstPlaneMemory = dstLayerMemory;
				for(uint32_t y = 0; y < imageExtent.height; y++)
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

	if(bufferIsSource)
	{
		prepareForSampling({ region.imageSubresource.aspectMask, region.imageSubresource.mipLevel, 1,
		                     region.imageSubresource.baseArrayLayer, region.imageSubresource.layerCount });
	}
}

void Image::copyTo(Buffer *dstBuffer, const VkBufferImageCopy &region)
{
	copy(dstBuffer, region, false);
}

void Image::copyFrom(Buffer *srcBuffer, const VkBufferImageCopy &region)
{
	copy(srcBuffer, region, true);
}

void *Image::getTexelPointer(const VkOffset3D &offset, const VkImageSubresourceLayers &subresource) const
{
	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresource.aspectMask);
	return deviceMemory->getOffsetPointer(texelOffsetBytesInStorage(offset, subresource) +
	                                      getMemoryOffset(aspect, subresource.mipLevel, subresource.baseArrayLayer));
}

VkExtent3D Image::imageExtentInBlocks(const VkExtent3D &extent, VkImageAspectFlagBits aspect) const
{
	VkExtent3D adjustedExtent = extent;
	Format usedFormat = getFormat(aspect);
	if(usedFormat.isCompressed())
	{
		// When using a compressed format, we use the block as the base unit, instead of the texel
		int blockWidth = usedFormat.blockWidth();
		int blockHeight = usedFormat.blockHeight();

		// Mip level allocations will round up to the next block for compressed texture
		adjustedExtent.width = ((adjustedExtent.width + blockWidth - 1) / blockWidth);
		adjustedExtent.height = ((adjustedExtent.height + blockHeight - 1) / blockHeight);
	}
	return adjustedExtent;
}

VkOffset3D Image::imageOffsetInBlocks(const VkOffset3D &offset, VkImageAspectFlagBits aspect) const
{
	VkOffset3D adjustedOffset = offset;
	Format usedFormat = getFormat(aspect);
	if(usedFormat.isCompressed())
	{
		// When using a compressed format, we use the block as the base unit, instead of the texel
		int blockWidth = usedFormat.blockWidth();
		int blockHeight = usedFormat.blockHeight();

		ASSERT(((offset.x % blockWidth) == 0) && ((offset.y % blockHeight) == 0));  // We can't offset within a block

		adjustedOffset.x /= blockWidth;
		adjustedOffset.y /= blockHeight;
	}
	return adjustedOffset;
}

VkExtent2D Image::bufferExtentInBlocks(const VkExtent2D &extent, const VkBufferImageCopy &region) const
{
	VkExtent2D adjustedExtent = extent;
	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(region.imageSubresource.aspectMask);
	Format usedFormat = getFormat(aspect);
	if(region.bufferRowLength != 0)
	{
		adjustedExtent.width = region.bufferRowLength;

		if(usedFormat.isCompressed())
		{
			int blockWidth = usedFormat.blockWidth();
			ASSERT((adjustedExtent.width % blockWidth) == 0);
			adjustedExtent.width /= blockWidth;
		}
	}
	if(region.bufferImageHeight != 0)
	{
		adjustedExtent.height = region.bufferImageHeight;

		if(usedFormat.isCompressed())
		{
			int blockHeight = usedFormat.blockHeight();
			ASSERT((adjustedExtent.height % blockHeight) == 0);
			adjustedExtent.height /= blockHeight;
		}
	}
	return adjustedExtent;
}

int Image::borderSize() const
{
	// We won't add a border to compressed cube textures, we'll add it when we decompress the texture
	return (isCube() && !format.isCompressed()) ? 1 : 0;
}

VkDeviceSize Image::texelOffsetBytesInStorage(const VkOffset3D &offset, const VkImageSubresourceLayers &subresource) const
{
	VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresource.aspectMask);
	VkOffset3D adjustedOffset = imageOffsetInBlocks(offset, aspect);
	int border = borderSize();
	return adjustedOffset.z * slicePitchBytes(aspect, subresource.mipLevel) +
	       (adjustedOffset.y + border) * rowPitchBytes(aspect, subresource.mipLevel) +
	       (adjustedOffset.x + border) * getFormat(aspect).bytesPerBlock();
}

VkExtent3D Image::getMipLevelExtent(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	VkExtent3D mipLevelExtent;
	mipLevelExtent.width = extent.width >> mipLevel;
	mipLevelExtent.height = extent.height >> mipLevel;
	mipLevelExtent.depth = extent.depth >> mipLevel;

	if(mipLevelExtent.width == 0) { mipLevelExtent.width = 1; }
	if(mipLevelExtent.height == 0) { mipLevelExtent.height = 1; }
	if(mipLevelExtent.depth == 0) { mipLevelExtent.depth = 1; }

	switch(aspect)
	{
		case VK_IMAGE_ASPECT_COLOR_BIT:
		case VK_IMAGE_ASPECT_DEPTH_BIT:
		case VK_IMAGE_ASPECT_STENCIL_BIT:
		case VK_IMAGE_ASPECT_PLANE_0_BIT:  // Vulkan 1.1 Table 31. Plane Format Compatibility Table: plane 0 of all defined formats is full resolution.
			break;
		case VK_IMAGE_ASPECT_PLANE_1_BIT:
		case VK_IMAGE_ASPECT_PLANE_2_BIT:
			switch(format)
			{
				case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
				case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
					ASSERT(mipLevelExtent.width % 2 == 0 && mipLevelExtent.height % 2 == 0);  // Vulkan 1.1: "Images in this format must be defined with a width and height that is a multiple of two."
					// Vulkan 1.1 Table 31. Plane Format Compatibility Table:
					// Half-resolution U and V planes.
					mipLevelExtent.width /= 2;
					mipLevelExtent.height /= 2;
					break;
				default:
					UNSUPPORTED("format %d", int(format));
			}
			break;
		default:
			UNSUPPORTED("aspect %x", int(aspect));
	}

	return mipLevelExtent;
}

int Image::rowPitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	// Depth and Stencil pitch should be computed separately
	ASSERT((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	       (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));

	VkExtent3D mipLevelExtent = getMipLevelExtent(aspect, mipLevel);
	Format usedFormat = getFormat(aspect);
	if(usedFormat.isCompressed())
	{
		VkExtent3D extentInBlocks = imageExtentInBlocks(mipLevelExtent, aspect);
		return extentInBlocks.width * usedFormat.bytesPerBlock();
	}

	return usedFormat.pitchB(mipLevelExtent.width, borderSize(), true);
}

int Image::slicePitchBytes(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	// Depth and Stencil slice should be computed separately
	ASSERT((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) !=
	       (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));

	VkExtent3D mipLevelExtent = getMipLevelExtent(aspect, mipLevel);
	Format usedFormat = getFormat(aspect);
	if(usedFormat.isCompressed())
	{
		VkExtent3D extentInBlocks = imageExtentInBlocks(mipLevelExtent, aspect);
		return extentInBlocks.height * extentInBlocks.width * usedFormat.bytesPerBlock();
	}

	return usedFormat.sliceB(mipLevelExtent.width, mipLevelExtent.height, borderSize(), true);
}

Format Image::getFormat(VkImageAspectFlagBits aspect) const
{
	return format.getAspectFormat(aspect);
}

bool Image::isCube() const
{
	return (flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) && (imageType == VK_IMAGE_TYPE_2D);
}

uint8_t *Image::end() const
{
	return reinterpret_cast<uint8_t *>(deviceMemory->getOffsetPointer(deviceMemory->getCommittedMemoryInBytes() + 1));
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

		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
			if(aspect == VK_IMAGE_ASPECT_PLANE_2_BIT)
			{
				return memoryOffset + getStorageSize(VK_IMAGE_ASPECT_PLANE_1_BIT) + getStorageSize(VK_IMAGE_ASPECT_PLANE_0_BIT);
			}
			// Fall through to 2PLANE case:
		case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
			if(aspect == VK_IMAGE_ASPECT_PLANE_1_BIT)
			{
				return memoryOffset + getStorageSize(VK_IMAGE_ASPECT_PLANE_0_BIT);
			}
			else
			{
				ASSERT(aspect == VK_IMAGE_ASPECT_PLANE_0_BIT);

				return memoryOffset;
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
		offset += getMultiSampledLevelSize(aspect, i);
	}
	return offset;
}

VkDeviceSize Image::getMemoryOffset(VkImageAspectFlagBits aspect, uint32_t mipLevel, uint32_t layer) const
{
	return layer * getLayerOffset(aspect, mipLevel) + getMemoryOffset(aspect, mipLevel);
}

VkDeviceSize Image::getMipLevelSize(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	return getMipLevelExtent(aspect, mipLevel).depth * slicePitchBytes(aspect, mipLevel);
}

VkDeviceSize Image::getMultiSampledLevelSize(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	return getMipLevelSize(aspect, mipLevel) * samples;
}

bool Image::is3DSlice() const
{
	return ((imageType == VK_IMAGE_TYPE_3D) && (flags & VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT));
}

VkDeviceSize Image::getLayerOffset(VkImageAspectFlagBits aspect, uint32_t mipLevel) const
{
	if(is3DSlice())
	{
		// When the VkImageSubresourceRange structure is used to select a subset of the slices of a 3D
		// image's mip level in order to create a 2D or 2D array image view of a 3D image created with
		// VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT, baseArrayLayer and layerCount specify the first
		// slice index and the number of slices to include in the created image view.
		ASSERT(samples == VK_SAMPLE_COUNT_1_BIT);

		// Offset to the proper slice of the 3D image's mip level
		return slicePitchBytes(aspect, mipLevel);
	}

	return getLayerSize(aspect);
}

VkDeviceSize Image::getLayerSize(VkImageAspectFlagBits aspect) const
{
	VkDeviceSize layerSize = 0;

	for(uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
	{
		layerSize += getMultiSampledLevelSize(aspect, mipLevel);
	}

	return layerSize;
}

VkDeviceSize Image::getStorageSize(VkImageAspectFlags aspectMask) const
{
	if((aspectMask & ~(VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
	                   VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT)) != 0)
	{
		UNSUPPORTED("aspectMask %x", int(aspectMask));
	}

	VkDeviceSize storageSize = 0;

	if(aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_COLOR_BIT);
	if(aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_DEPTH_BIT);
	if(aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_STENCIL_BIT);
	if(aspectMask & VK_IMAGE_ASPECT_PLANE_0_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_PLANE_0_BIT);
	if(aspectMask & VK_IMAGE_ASPECT_PLANE_1_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_PLANE_1_BIT);
	if(aspectMask & VK_IMAGE_ASPECT_PLANE_2_BIT) storageSize += getLayerSize(VK_IMAGE_ASPECT_PLANE_2_BIT);

	return arrayLayers * storageSize;
}

const Image *Image::getSampledImage(const vk::Format &imageViewFormat) const
{
	bool isImageViewCompressed = imageViewFormat.isCompressed();
	if(decompressedImage && !isImageViewCompressed)
	{
		ASSERT(flags & VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT);
		ASSERT(format.bytesPerBlock() == imageViewFormat.bytesPerBlock());
	}
	// If the ImageView's format is compressed, then we do need to decompress the image so that
	// it may be sampled properly by texture sampling functions, which don't support compressed
	// textures. If the ImageView's format is NOT compressed, then we reinterpret cast the
	// compressed image into the ImageView's format, so we must return the compressed image as is.
	return (decompressedImage && isImageViewCompressed) ? decompressedImage : this;
}

void Image::blit(Image *dstImage, const VkImageBlit &region, VkFilter filter) const
{
	device->getBlitter()->blit(this, dstImage, region, filter);
}

void Image::blitToBuffer(VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D extent, uint8_t *dst, int bufferRowPitch, int bufferSlicePitch) const
{
	device->getBlitter()->blitToBuffer(this, subresource, offset, extent, dst, bufferRowPitch, bufferSlicePitch);
}

void Image::resolve(Image *dstImage, const VkImageResolve &region) const
{
	VkImageBlit blitRegion;

	blitRegion.srcOffsets[0] = blitRegion.srcOffsets[1] = region.srcOffset;
	blitRegion.srcOffsets[1].x += region.extent.width;
	blitRegion.srcOffsets[1].y += region.extent.height;
	blitRegion.srcOffsets[1].z += region.extent.depth;

	blitRegion.dstOffsets[0] = blitRegion.dstOffsets[1] = region.dstOffset;
	blitRegion.dstOffsets[1].x += region.extent.width;
	blitRegion.dstOffsets[1].y += region.extent.height;
	blitRegion.dstOffsets[1].z += region.extent.depth;

	blitRegion.srcSubresource = region.srcSubresource;
	blitRegion.dstSubresource = region.dstSubresource;

	device->getBlitter()->blit(this, dstImage, blitRegion, VK_FILTER_NEAREST);
}

VkFormat Image::getClearFormat() const
{
	// Set the proper format for the clear value, as described here:
	// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#clears-values
	if(format.isSignedUnnormalizedInteger())
	{
		return VK_FORMAT_R32G32B32A32_SINT;
	}
	else if(format.isUnsignedUnnormalizedInteger())
	{
		return VK_FORMAT_R32G32B32A32_UINT;
	}

	return VK_FORMAT_R32G32B32A32_SFLOAT;
}

uint32_t Image::getLastLayerIndex(const VkImageSubresourceRange &subresourceRange) const
{
	return ((subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS) ? arrayLayers : (subresourceRange.baseArrayLayer + subresourceRange.layerCount)) - 1;
}

uint32_t Image::getLastMipLevel(const VkImageSubresourceRange &subresourceRange) const
{
	return ((subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS) ? mipLevels : (subresourceRange.baseMipLevel + subresourceRange.levelCount)) - 1;
}

void Image::clear(void *pixelData, VkFormat pixelFormat, const vk::Format &viewFormat, const VkImageSubresourceRange &subresourceRange, const VkRect2D &renderArea)
{
	device->getBlitter()->clear(pixelData, pixelFormat, this, viewFormat, subresourceRange, &renderArea);
}

void Image::clear(const VkClearColorValue &color, const VkImageSubresourceRange &subresourceRange)
{
	ASSERT(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT);

	device->getBlitter()->clear((void *)color.float32, getClearFormat(), this, format, subresourceRange);
}

void Image::clear(const VkClearDepthStencilValue &color, const VkImageSubresourceRange &subresourceRange)
{
	ASSERT((subresourceRange.aspectMask & ~(VK_IMAGE_ASPECT_DEPTH_BIT |
	                                        VK_IMAGE_ASPECT_STENCIL_BIT)) == 0);

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
	{
		VkImageSubresourceRange depthSubresourceRange = subresourceRange;
		depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		device->getBlitter()->clear((void *)(&color.depth), VK_FORMAT_D32_SFLOAT, this, format, depthSubresourceRange);
	}

	if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
	{
		VkImageSubresourceRange stencilSubresourceRange = subresourceRange;
		stencilSubresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		device->getBlitter()->clear((void *)(&color.stencil), VK_FORMAT_S8_UINT, this, format, stencilSubresourceRange);
	}
}

void Image::clear(const VkClearValue &clearValue, const vk::Format &viewFormat, const VkRect2D &renderArea, const VkImageSubresourceRange &subresourceRange)
{
	ASSERT((subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) ||
	       (subresourceRange.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT |
	                                       VK_IMAGE_ASPECT_STENCIL_BIT)));

	if(subresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		clear((void *)(clearValue.color.float32), getClearFormat(), viewFormat, subresourceRange, renderArea);
	}
	else
	{
		if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
		{
			VkImageSubresourceRange depthSubresourceRange = subresourceRange;
			depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			clear((void *)(&clearValue.depthStencil.depth), VK_FORMAT_D32_SFLOAT, viewFormat, depthSubresourceRange, renderArea);
		}

		if(subresourceRange.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
		{
			VkImageSubresourceRange stencilSubresourceRange = subresourceRange;
			stencilSubresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
			clear((void *)(&clearValue.depthStencil.stencil), VK_FORMAT_S8_UINT, viewFormat, stencilSubresourceRange, renderArea);
		}
	}
}

void Image::prepareForSampling(const VkImageSubresourceRange &subresourceRange)
{
	if(decompressedImage)
	{
		switch(format)
		{
			case VK_FORMAT_EAC_R11_UNORM_BLOCK:
			case VK_FORMAT_EAC_R11_SNORM_BLOCK:
			case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
			case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
			case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
				decodeETC2(subresourceRange);
				break;
			case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
			case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
			case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
			case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
			case VK_FORMAT_BC2_UNORM_BLOCK:
			case VK_FORMAT_BC2_SRGB_BLOCK:
			case VK_FORMAT_BC3_UNORM_BLOCK:
			case VK_FORMAT_BC3_SRGB_BLOCK:
			case VK_FORMAT_BC4_UNORM_BLOCK:
			case VK_FORMAT_BC4_SNORM_BLOCK:
			case VK_FORMAT_BC5_UNORM_BLOCK:
			case VK_FORMAT_BC5_SNORM_BLOCK:
			case VK_FORMAT_BC6H_UFLOAT_BLOCK:
			case VK_FORMAT_BC6H_SFLOAT_BLOCK:
			case VK_FORMAT_BC7_UNORM_BLOCK:
			case VK_FORMAT_BC7_SRGB_BLOCK:
				decodeBC(subresourceRange);
				break;
			case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
			case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
			case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
			case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
			case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
			case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
			case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
			case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
			case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
			case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
			case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
			case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
			case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
			case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
			case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
			case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
			case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
			case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
			case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
			case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
			case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
			case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
			case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
			case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
			case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
			case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
			case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
			case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
			case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
			case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
				decodeASTC(subresourceRange);
				break;
			default:
				break;
		}
	}

	if(isCube() && (arrayLayers >= 6))
	{
		VkImageSubresourceLayers subresourceLayers = {
			subresourceRange.aspectMask,
			subresourceRange.baseMipLevel,
			subresourceRange.baseArrayLayer,
			6
		};

		// Update the borders of all the groups of 6 layers that can be part of a cubemaps but don't
		// touch leftover layers that cannot be part of cubemaps.
		uint32_t lastMipLevel = getLastMipLevel(subresourceRange);
		for(; subresourceLayers.mipLevel <= lastMipLevel; subresourceLayers.mipLevel++)
		{
			for(subresourceLayers.baseArrayLayer = 0;
			    subresourceLayers.baseArrayLayer < arrayLayers - 5;
			    subresourceLayers.baseArrayLayer += 6)
			{
				device->getBlitter()->updateBorders(decompressedImage ? decompressedImage : this, subresourceLayers);
			}
		}
	}
}

void Image::decodeETC2(const VkImageSubresourceRange &subresourceRange) const
{
	ASSERT(decompressedImage);

	ETC_Decoder::InputType inputType = GetInputType(format);

	uint32_t lastLayer = getLastLayerIndex(subresourceRange);
	uint32_t lastMipLevel = getLastMipLevel(subresourceRange);

	int bytes = decompressedImage->format.bytes();
	bool fakeAlpha = (format == VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK) || (format == VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK);
	size_t sizeToWrite = 0;

	VkImageSubresourceLayers subresourceLayers = { subresourceRange.aspectMask, subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, 1 };
	for(; subresourceLayers.baseArrayLayer <= lastLayer; subresourceLayers.baseArrayLayer++)
	{
		for(; subresourceLayers.mipLevel <= lastMipLevel; subresourceLayers.mipLevel++)
		{
			VkExtent3D mipLevelExtent = getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceLayers.aspectMask), subresourceLayers.mipLevel);

			int pitchB = decompressedImage->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, subresourceLayers.mipLevel);

			if(fakeAlpha)
			{
				// To avoid overflow in case of cube textures, which are offset in memory to account for the border,
				// compute the size from the first pixel to the last pixel, excluding any padding or border before
				// the first pixel or after the last pixel.
				sizeToWrite = ((mipLevelExtent.height - 1) * pitchB) + (mipLevelExtent.width * bytes);
			}

			for(int32_t depth = 0; depth < static_cast<int32_t>(mipLevelExtent.depth); depth++)
			{
				uint8_t *source = static_cast<uint8_t *>(getTexelPointer({ 0, 0, depth }, subresourceLayers));
				uint8_t *dest = static_cast<uint8_t *>(decompressedImage->getTexelPointer({ 0, 0, depth }, subresourceLayers));

				if(fakeAlpha)
				{
					ASSERT((dest + sizeToWrite) < decompressedImage->end());
					memset(dest, 0xFF, sizeToWrite);
				}

				ETC_Decoder::Decode(source, dest, mipLevelExtent.width, mipLevelExtent.height,
				                    pitchB, bytes, inputType);
			}
		}
	}
}

void Image::decodeBC(const VkImageSubresourceRange &subresourceRange) const
{
	ASSERT(decompressedImage);

	int n = GetBCn(format);
	int noAlphaU = GetNoAlphaOrUnsigned(format);

	uint32_t lastLayer = getLastLayerIndex(subresourceRange);
	uint32_t lastMipLevel = getLastMipLevel(subresourceRange);

	int bytes = decompressedImage->format.bytes();

	VkImageSubresourceLayers subresourceLayers = { subresourceRange.aspectMask, subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, 1 };
	for(; subresourceLayers.baseArrayLayer <= lastLayer; subresourceLayers.baseArrayLayer++)
	{
		for(; subresourceLayers.mipLevel <= lastMipLevel; subresourceLayers.mipLevel++)
		{
			VkExtent3D mipLevelExtent = getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceLayers.aspectMask), subresourceLayers.mipLevel);

			int pitchB = decompressedImage->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, subresourceLayers.mipLevel);

			for(int32_t depth = 0; depth < static_cast<int32_t>(mipLevelExtent.depth); depth++)
			{
				uint8_t *source = static_cast<uint8_t *>(getTexelPointer({ 0, 0, depth }, subresourceLayers));
				uint8_t *dest = static_cast<uint8_t *>(decompressedImage->getTexelPointer({ 0, 0, depth }, subresourceLayers));

				BC_Decoder::Decode(source, dest, mipLevelExtent.width, mipLevelExtent.height,
				                   pitchB, bytes, n, noAlphaU);
			}
		}
	}
}

void Image::decodeASTC(const VkImageSubresourceRange &subresourceRange) const
{
	ASSERT(decompressedImage);

	int xBlockSize = format.blockWidth();
	int yBlockSize = format.blockHeight();
	int zBlockSize = 1;
	bool isUnsigned = format.isUnsignedComponent(0);

	uint32_t lastLayer = getLastLayerIndex(subresourceRange);
	uint32_t lastMipLevel = getLastMipLevel(subresourceRange);

	int bytes = decompressedImage->format.bytes();

	VkImageSubresourceLayers subresourceLayers = { subresourceRange.aspectMask, subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, 1 };
	for(; subresourceLayers.baseArrayLayer <= lastLayer; subresourceLayers.baseArrayLayer++)
	{
		for(; subresourceLayers.mipLevel <= lastMipLevel; subresourceLayers.mipLevel++)
		{
			VkExtent3D mipLevelExtent = getMipLevelExtent(static_cast<VkImageAspectFlagBits>(subresourceLayers.aspectMask), subresourceLayers.mipLevel);

			int xblocks = (mipLevelExtent.width + xBlockSize - 1) / xBlockSize;
			int yblocks = (mipLevelExtent.height + yBlockSize - 1) / yBlockSize;
			int zblocks = (zBlockSize > 1) ? (mipLevelExtent.depth + zBlockSize - 1) / zBlockSize : 1;

			if(xblocks <= 0 || yblocks <= 0 || zblocks <= 0)
			{
				continue;
			}

			int pitchB = decompressedImage->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, subresourceLayers.mipLevel);
			int sliceB = decompressedImage->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, subresourceLayers.mipLevel);

			for(int32_t depth = 0; depth < static_cast<int32_t>(mipLevelExtent.depth); depth++)
			{
				uint8_t *source = static_cast<uint8_t *>(getTexelPointer({ 0, 0, depth }, subresourceLayers));
				uint8_t *dest = static_cast<uint8_t *>(decompressedImage->getTexelPointer({ 0, 0, depth }, subresourceLayers));

				ASTC_Decoder::Decode(source, dest, mipLevelExtent.width, mipLevelExtent.height, mipLevelExtent.depth, bytes, pitchB, sliceB,
				                     xBlockSize, yBlockSize, zBlockSize, xblocks, yblocks, zblocks, isUnsigned);
			}
		}
	}
}

}  // namespace vk
