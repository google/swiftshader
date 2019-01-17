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

#ifndef VK_IMAGE_HPP_
#define VK_IMAGE_HPP_

#include "VkObject.hpp"

namespace sw
{
	class Blitter;
	class Surface;
};

namespace vk
{

class DeviceMemory;

class Image : public Object<Image, VkImage>
{
public:
	Image(const VkImageCreateInfo* pCreateInfo, void* mem);
	~Image() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkImageCreateInfo* pCreateInfo);

	const VkMemoryRequirements getMemoryRequirements() const;
	void bind(VkDeviceMemory pDeviceMemory, VkDeviceSize pMemoryOffset);
	void copyTo(VkImage dstImage, const VkImageCopy& pRegion);
	void copyTo(VkBuffer dstBuffer, const VkBufferImageCopy& region);
	void copyFrom(VkBuffer srcBuffer, const VkBufferImageCopy& region);

	void blit(VkImage dstImage, const VkImageBlit& region, VkFilter filter);
	void clear(const VkClearValue& clearValue, const VkRect2D& renderArea, const VkImageSubresourceRange& subresourceRange);
	void clear(const VkClearColorValue& color, const VkImageSubresourceRange& subresourceRange);
	void clear(const VkClearDepthStencilValue& color, const VkImageSubresourceRange& subresourceRange);

	VkImageType              getImageType() const { return imageType; }
	VkFormat                 getFormat() const { return format; }
	uint32_t                 getArrayLayers() const { return arrayLayers; }
	bool                     isCube() const;

private:
	void copy(VkBuffer buffer, const VkBufferImageCopy& region, bool bufferIsSource);
	VkDeviceSize getStorageSize(const VkImageAspectFlags& flags) const;
	VkDeviceSize getMipLevelSize(const VkImageAspectFlags& flags, uint32_t mipLevel) const;
	VkDeviceSize getLayerSize(const VkImageAspectFlags& flags) const;
	VkDeviceSize getMemoryOffset(const VkImageAspectFlags& flags, uint32_t mipLevel) const;
	VkDeviceSize getMemoryOffset(const VkImageAspectFlags& flags, uint32_t mipLevel, uint32_t layer) const;
	void* getTexelPointer(const VkOffset3D& offset, const VkImageSubresourceLayers& subresource) const;
	VkDeviceSize texelOffsetBytesInStorage(const VkOffset3D& offset, const VkImageSubresourceLayers& subresource) const;
	VkDeviceSize getMemoryOffset(const VkImageAspectFlags& flags) const;
	int rowPitchBytes(const VkImageAspectFlags& flags, uint32_t mipLevel) const;
	int slicePitchBytes(const VkImageAspectFlags& flags, uint32_t mipLevel) const;
	int bytesPerTexel(const VkImageAspectFlags& flags) const;
	VkExtent3D getMipLevelExtent(uint32_t mipLevel) const;
	VkFormat getFormat(const VkImageAspectFlags& flags) const;
	uint32_t getLastLayerIndex(const VkImageSubresourceRange& subresourceRange) const;
	uint32_t getLastMipLevel(const VkImageSubresourceRange& subresourceRange) const;
	VkFormat getClearFormat() const;
	void clear(void* pixelData, VkFormat format, const VkImageSubresourceRange& subresourceRange, VkImageAspectFlags aspectMask);
	sw::Surface* asSurface(const VkImageAspectFlags& flags, uint32_t mipLevel, uint32_t layer) const;

	DeviceMemory*            deviceMemory = nullptr;
	VkDeviceSize             memoryOffset = 0;
	VkImageCreateFlags       flags = 0;
	VkImageType              imageType = VK_IMAGE_TYPE_2D;
	VkFormat                 format = VK_FORMAT_UNDEFINED;
	VkExtent3D               extent = {0, 0, 0};
	uint32_t                 mipLevels = 0;
	uint32_t                 arrayLayers = 0;
	VkSampleCountFlagBits    samples = VK_SAMPLE_COUNT_1_BIT;
	VkImageTiling            tiling = VK_IMAGE_TILING_OPTIMAL;
	sw::Blitter*             blitter = nullptr;
};

static inline Image* Cast(VkImage object)
{
	return reinterpret_cast<Image*>(object);
}

} // namespace vk

#endif // VK_IMAGE_HPP_