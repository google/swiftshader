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

#include "VkDescriptorSetLayout.hpp"

#include "VkBuffer.hpp"
#include "VkBufferView.hpp"
#include "VkDescriptorSet.hpp"
#include "VkImageView.hpp"
#include "VkSampler.hpp"
#include "System/Types.hpp"

#include <algorithm>
#include <cstring>

namespace {

static bool UsesImmutableSamplers(const VkDescriptorSetLayoutBinding &binding)
{
	return (((binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER) ||
	         (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)) &&
	        (binding.pImmutableSamplers != nullptr));
}

}  // anonymous namespace

namespace vk {

DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo *pCreateInfo, void *mem)
    : flags(pCreateInfo->flags)
    , bindingCount(pCreateInfo->bindingCount)
    , bindings(reinterpret_cast<VkDescriptorSetLayoutBinding *>(mem))
{
	uint8_t *hostMemory = static_cast<uint8_t *>(mem) + bindingCount * sizeof(VkDescriptorSetLayoutBinding);
	bindingOffsets = reinterpret_cast<size_t *>(hostMemory);
	hostMemory += bindingCount * sizeof(size_t);

	size_t offset = 0;
	for(uint32_t i = 0; i < bindingCount; i++)
	{
		bindings[i] = pCreateInfo->pBindings[i];
		if(UsesImmutableSamplers(bindings[i]))
		{
			size_t immutableSamplersSize = bindings[i].descriptorCount * sizeof(VkSampler);
			bindings[i].pImmutableSamplers = reinterpret_cast<const VkSampler *>(hostMemory);
			hostMemory += immutableSamplersSize;
			memcpy(const_cast<VkSampler *>(bindings[i].pImmutableSamplers),
			       pCreateInfo->pBindings[i].pImmutableSamplers,
			       immutableSamplersSize);
		}
		else
		{
			bindings[i].pImmutableSamplers = nullptr;
		}
		bindingOffsets[i] = offset;
		offset += bindings[i].descriptorCount * GetDescriptorSize(bindings[i].descriptorType);
	}
	ASSERT_MSG(offset == getDescriptorSetDataSize(), "offset: %d, size: %d", int(offset), int(getDescriptorSetDataSize()));
}

void DescriptorSetLayout::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(bindings, pAllocator);  // This allocation also contains pImmutableSamplers
}

size_t DescriptorSetLayout::ComputeRequiredAllocationSize(const VkDescriptorSetLayoutCreateInfo *pCreateInfo)
{
	size_t allocationSize = pCreateInfo->bindingCount * (sizeof(VkDescriptorSetLayoutBinding) + sizeof(size_t));

	for(uint32_t i = 0; i < pCreateInfo->bindingCount; i++)
	{
		if(UsesImmutableSamplers(pCreateInfo->pBindings[i]))
		{
			allocationSize += pCreateInfo->pBindings[i].descriptorCount * sizeof(VkSampler);
		}
	}

	return allocationSize;
}

size_t DescriptorSetLayout::GetDescriptorSize(VkDescriptorType type)
{
	switch(type)
	{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return sizeof(SampledImageDescriptor);
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return sizeof(StorageImageDescriptor);
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return sizeof(BufferDescriptor);
		default:
			UNSUPPORTED("Unsupported Descriptor Type");
			return 0;
	}
}

size_t DescriptorSetLayout::getDescriptorSetAllocationSize() const
{
	// vk::DescriptorSet has a layout member field.
	return sw::align<alignof(DescriptorSet)>(OFFSET(DescriptorSet, data) + getDescriptorSetDataSize());
}

size_t DescriptorSetLayout::getDescriptorSetDataSize() const
{
	size_t size = 0;
	for(uint32_t i = 0; i < bindingCount; i++)
	{
		size += bindings[i].descriptorCount * GetDescriptorSize(bindings[i].descriptorType);
	}

	return size;
}

uint32_t DescriptorSetLayout::getBindingIndex(uint32_t binding) const
{
	for(uint32_t i = 0; i < bindingCount; i++)
	{
		if(binding == bindings[i].binding)
		{
			return i;
		}
	}

	DABORT("Invalid DescriptorSetLayout binding: %d", int(binding));
	return 0;
}

void DescriptorSetLayout::initialize(DescriptorSet *descriptorSet)
{
	// Use a pointer to this descriptor set layout as the descriptor set's header
	descriptorSet->header.layout = this;
	uint8_t *mem = descriptorSet->data;

	for(uint32_t i = 0; i < bindingCount; i++)
	{
		size_t typeSize = GetDescriptorSize(bindings[i].descriptorType);
		if(UsesImmutableSamplers(bindings[i]))
		{
			for(uint32_t j = 0; j < bindings[i].descriptorCount; j++)
			{
				SampledImageDescriptor *imageSamplerDescriptor = reinterpret_cast<SampledImageDescriptor *>(mem);
				imageSamplerDescriptor->updateSampler(bindings[i].pImmutableSamplers[j]);
				mem += typeSize;
			}
		}
		else
		{
			mem += bindings[i].descriptorCount * typeSize;
		}
	}
}

size_t DescriptorSetLayout::getBindingCount() const
{
	return bindingCount;
}

bool DescriptorSetLayout::hasBinding(uint32_t binding) const
{
	for(uint32_t i = 0; i < bindingCount; i++)
	{
		if(binding == bindings[i].binding)
		{
			return true;
		}
	}
	return false;
}

size_t DescriptorSetLayout::getBindingStride(uint32_t binding) const
{
	uint32_t index = getBindingIndex(binding);
	return GetDescriptorSize(bindings[index].descriptorType);
}

size_t DescriptorSetLayout::getBindingOffset(uint32_t binding, size_t arrayElement) const
{
	uint32_t index = getBindingIndex(binding);
	auto typeSize = GetDescriptorSize(bindings[index].descriptorType);
	return bindingOffsets[index] + OFFSET(DescriptorSet, data[0]) + (typeSize * arrayElement);
}

bool DescriptorSetLayout::isDynamic(VkDescriptorType type)
{
	return type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
	       type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
}

bool DescriptorSetLayout::isBindingDynamic(uint32_t binding) const
{
	uint32_t index = getBindingIndex(binding);
	return isDynamic(bindings[index].descriptorType);
}

uint32_t DescriptorSetLayout::getDynamicDescriptorCount() const
{
	uint32_t count = 0;
	for(size_t i = 0; i < bindingCount; i++)
	{
		if(isDynamic(bindings[i].descriptorType))
		{
			count += bindings[i].descriptorCount;
		}
	}
	return count;
}

uint32_t DescriptorSetLayout::getDynamicDescriptorOffset(uint32_t binding) const
{
	uint32_t n = getBindingIndex(binding);
	ASSERT(isDynamic(bindings[n].descriptorType));

	uint32_t index = 0;
	for(uint32_t i = 0; i < n; i++)
	{
		if(isDynamic(bindings[i].descriptorType))
		{
			index += bindings[i].descriptorCount;
		}
	}
	return index;
}

VkDescriptorSetLayoutBinding const &DescriptorSetLayout::getBindingLayout(uint32_t binding) const
{
	uint32_t index = getBindingIndex(binding);
	return bindings[index];
}

uint8_t *DescriptorSetLayout::getOffsetPointer(DescriptorSet *descriptorSet, uint32_t binding, uint32_t arrayElement, uint32_t count, size_t *typeSize) const
{
	uint32_t index = getBindingIndex(binding);
	*typeSize = GetDescriptorSize(bindings[index].descriptorType);
	size_t byteOffset = bindingOffsets[index] + (*typeSize * arrayElement);
	ASSERT(((*typeSize * count) + byteOffset) <= getDescriptorSetDataSize());  // Make sure the operation will not go out of bounds
	return &descriptorSet->data[byteOffset];
}

void SampledImageDescriptor::updateSampler(const VkSampler newSampler)
{
	memcpy(reinterpret_cast<void *>(&sampler), vk::Cast(newSampler), sizeof(sampler));
}

void DescriptorSetLayout::WriteDescriptorSet(Device *device, DescriptorSet *dstSet, VkDescriptorUpdateTemplateEntry const &entry, char const *src)
{
	DescriptorSetLayout *dstLayout = dstSet->header.layout;
	auto &binding = dstLayout->bindings[dstLayout->getBindingIndex(entry.dstBinding)];
	ASSERT(dstLayout);
	ASSERT(binding.descriptorType == entry.descriptorType);

	size_t typeSize = 0;
	uint8_t *memToWrite = dstLayout->getOffsetPointer(dstSet, entry.dstBinding, entry.dstArrayElement, entry.descriptorCount, &typeSize);

	ASSERT(reinterpret_cast<intptr_t>(memToWrite) % 16 == 0);  // Each descriptor must be 16-byte aligned.

	if(entry.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
	{
		SampledImageDescriptor *imageSampler = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);
			// "All consecutive bindings updated via a single VkWriteDescriptorSet structure, except those with a
			//  descriptorCount of zero, must all either use immutable samplers or must all not use immutable samplers."
			if(!binding.pImmutableSamplers)
			{
				imageSampler[i].updateSampler(update->sampler);
			}
			imageSampler[i].device = device;
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
	{
		SampledImageDescriptor *imageSampler = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkBufferView const *>(src + entry.offset + entry.stride * i);
			auto bufferView = vk::Cast(*update);

			imageSampler[i].type = VK_IMAGE_VIEW_TYPE_1D;
			imageSampler[i].imageViewId = bufferView->id;
			imageSampler[i].swizzle = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			imageSampler[i].format = bufferView->getFormat();

			auto numElements = bufferView->getElementCount();
			imageSampler[i].extent = { numElements, 1, 1 };
			imageSampler[i].arrayLayers = 1;
			imageSampler[i].mipLevels = 1;
			imageSampler[i].sampleCount = 1;
			imageSampler[i].texture.widthWidthHeightHeight = sw::float4(static_cast<float>(numElements), static_cast<float>(numElements), 1, 1);
			imageSampler[i].texture.width = sw::float4(static_cast<float>(numElements));
			imageSampler[i].texture.height = sw::float4(1);
			imageSampler[i].texture.depth = sw::float4(1);
			imageSampler[i].device = device;

			sw::Mipmap &mipmap = imageSampler[i].texture.mipmap[0];
			mipmap.buffer = bufferView->getPointer();
			mipmap.width[0] = mipmap.width[1] = mipmap.width[2] = mipmap.width[3] = numElements;
			mipmap.height[0] = mipmap.height[1] = mipmap.height[2] = mipmap.height[3] = 1;
			mipmap.depth[0] = mipmap.depth[1] = mipmap.depth[2] = mipmap.depth[3] = 1;
			mipmap.pitchP.x = mipmap.pitchP.y = mipmap.pitchP.z = mipmap.pitchP.w = numElements;
			mipmap.sliceP.x = mipmap.sliceP.y = mipmap.sliceP.z = mipmap.sliceP.w = 0;
			mipmap.onePitchP[0] = mipmap.onePitchP[2] = 1;
			mipmap.onePitchP[1] = mipmap.onePitchP[3] = static_cast<short>(numElements);
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
	{
		SampledImageDescriptor *imageSampler = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);

			vk::ImageView *imageView = vk::Cast(update->imageView);
			Format format = imageView->getFormat(ImageView::SAMPLING);

			sw::Texture *texture = &imageSampler[i].texture;

			if(entry.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				// "All consecutive bindings updated via a single VkWriteDescriptorSet structure, except those with a
				//  descriptorCount of zero, must all either use immutable samplers or must all not use immutable samplers."
				if(!binding.pImmutableSamplers)
				{
					imageSampler[i].updateSampler(update->sampler);
				}
			}

			imageSampler[i].imageViewId = imageView->id;
			imageSampler[i].extent = imageView->getMipLevelExtent(0);
			imageSampler[i].arrayLayers = imageView->getSubresourceRange().layerCount;
			imageSampler[i].mipLevels = imageView->getSubresourceRange().levelCount;
			imageSampler[i].sampleCount = imageView->getSampleCount();
			imageSampler[i].type = imageView->getType();
			imageSampler[i].swizzle = imageView->getComponentMapping();
			imageSampler[i].format = format;
			imageSampler[i].device = device;

			auto &subresourceRange = imageView->getSubresourceRange();

			if(format.isYcbcrFormat())
			{
				ASSERT(subresourceRange.levelCount == 1);

				// YCbCr images can only have one level, so we can store parameters for the
				// different planes in the descriptor's mipmap levels instead.

				const int level = 0;
				VkOffset3D offset = { 0, 0, 0 };
				texture->mipmap[0].buffer = imageView->getOffsetPointer(offset, VK_IMAGE_ASPECT_PLANE_0_BIT, level, 0, ImageView::SAMPLING);
				texture->mipmap[1].buffer = imageView->getOffsetPointer(offset, VK_IMAGE_ASPECT_PLANE_1_BIT, level, 0, ImageView::SAMPLING);
				if(format.getAspects() & VK_IMAGE_ASPECT_PLANE_2_BIT)
				{
					texture->mipmap[2].buffer = imageView->getOffsetPointer(offset, VK_IMAGE_ASPECT_PLANE_2_BIT, level, 0, ImageView::SAMPLING);
				}

				VkExtent3D extent = imageView->getMipLevelExtent(0);

				int width = extent.width;
				int height = extent.height;
				int pitchP0 = imageView->rowPitchBytes(VK_IMAGE_ASPECT_PLANE_0_BIT, level, ImageView::SAMPLING) /
				              imageView->getFormat(VK_IMAGE_ASPECT_PLANE_0_BIT).bytes();

				// Write plane 0 parameters to mipmap level 0.
				WriteTextureLevelInfo(texture, 0, width, height, 1, pitchP0, 0, 0, 0);

				// Plane 2, if present, has equal parameters to plane 1, so we use mipmap level 1 for both.
				int pitchP1 = imageView->rowPitchBytes(VK_IMAGE_ASPECT_PLANE_1_BIT, level, ImageView::SAMPLING) /
				              imageView->getFormat(VK_IMAGE_ASPECT_PLANE_1_BIT).bytes();

				WriteTextureLevelInfo(texture, 1, width / 2, height / 2, 1, pitchP1, 0, 0, 0);
			}
			else
			{
				for(int mipmapLevel = 0; mipmapLevel < sw::MIPMAP_LEVELS; mipmapLevel++)
				{
					int level = sw::clamp(mipmapLevel, 0, (int)subresourceRange.levelCount - 1);  // Level within the image view

					VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(imageView->getSubresourceRange().aspectMask);
					sw::Mipmap &mipmap = texture->mipmap[mipmapLevel];

					if((imageView->getType() == VK_IMAGE_VIEW_TYPE_CUBE) ||
					   (imageView->getType() == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY))
					{
						// Obtain the pointer to the corner of the level including the border, for seamless sampling.
						// This is taken into account in the sampling routine, which can't handle negative texel coordinates.
						VkOffset3D offset = { -1, -1, 0 };
						mipmap.buffer = imageView->getOffsetPointer(offset, aspect, level, 0, ImageView::SAMPLING);
					}
					else
					{
						VkOffset3D offset = { 0, 0, 0 };
						mipmap.buffer = imageView->getOffsetPointer(offset, aspect, level, 0, ImageView::SAMPLING);
					}

					VkExtent3D extent = imageView->getMipLevelExtent(level);

					int width = extent.width;
					int height = extent.height;
					int bytes = format.bytes();
					int layers = imageView->getSubresourceRange().layerCount;  // TODO(b/129523279): Untangle depth vs layers throughout the sampler
					int depth = layers > 1 ? layers : extent.depth;
					int pitchP = imageView->rowPitchBytes(aspect, level, ImageView::SAMPLING) / bytes;
					int sliceP = (layers > 1 ? imageView->layerPitchBytes(aspect, ImageView::SAMPLING) : imageView->slicePitchBytes(aspect, level, ImageView::SAMPLING)) / bytes;
					int samplePitchP = imageView->getMipLevelSize(aspect, level, ImageView::SAMPLING) / bytes;
					int sampleMax = imageView->getSampleCount() - 1;

					WriteTextureLevelInfo(texture, mipmapLevel, width, height, depth, pitchP, sliceP, samplePitchP, sampleMax);
				}
			}
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
	{
		auto descriptor = reinterpret_cast<StorageImageDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);
			auto imageView = vk::Cast(update->imageView);
			descriptor[i].ptr = imageView->getOffsetPointer({ 0, 0, 0 }, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0);
			descriptor[i].extent = imageView->getMipLevelExtent(0);
			descriptor[i].rowPitchBytes = imageView->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			descriptor[i].samplePitchBytes = imageView->getSubresourceRange().layerCount > 1
			                                     ? imageView->layerPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT)
			                                     : imageView->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			descriptor[i].slicePitchBytes = descriptor[i].samplePitchBytes * imageView->getSampleCount();
			descriptor[i].arrayLayers = imageView->getSubresourceRange().layerCount;
			descriptor[i].sampleCount = imageView->getSampleCount();
			descriptor[i].sizeInBytes = static_cast<int>(imageView->getSizeInBytes());

			if(imageView->getFormat().isStencil())
			{
				descriptor[i].stencilPtr = imageView->getOffsetPointer({ 0, 0, 0 }, VK_IMAGE_ASPECT_STENCIL_BIT, 0, 0);
				descriptor[i].stencilRowPitchBytes = imageView->rowPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
				descriptor[i].stencilSamplePitchBytes = (imageView->getSubresourceRange().layerCount > 1)
				                                            ? imageView->layerPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT)
				                                            : imageView->slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
				descriptor[i].stencilSlicePitchBytes = descriptor[i].stencilSamplePitchBytes * imageView->getSampleCount();
			}
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)
	{
		auto descriptor = reinterpret_cast<StorageImageDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkBufferView const *>(src + entry.offset + entry.stride * i);
			auto bufferView = vk::Cast(*update);
			descriptor[i].ptr = bufferView->getPointer();
			descriptor[i].extent = { bufferView->getElementCount(), 1, 1 };
			descriptor[i].rowPitchBytes = 0;
			descriptor[i].slicePitchBytes = 0;
			descriptor[i].samplePitchBytes = 0;
			descriptor[i].arrayLayers = 1;
			descriptor[i].sampleCount = 1;
			descriptor[i].sizeInBytes = bufferView->getRangeInBytes();
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
	{
		auto descriptor = reinterpret_cast<BufferDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorBufferInfo const *>(src + entry.offset + entry.stride * i);
			auto buffer = vk::Cast(update->buffer);
			descriptor[i].ptr = buffer->getOffsetPointer(update->offset);
			descriptor[i].sizeInBytes = static_cast<int>((update->range == VK_WHOLE_SIZE) ? buffer->getSize() - update->offset : update->range);
			descriptor[i].robustnessSize = static_cast<int>(buffer->getSize() - update->offset);
		}
	}
}

void DescriptorSetLayout::WriteTextureLevelInfo(sw::Texture *texture, int level, int width, int height, int depth, int pitchP, int sliceP, int samplePitchP, int sampleMax)
{
	if(level == 0)
	{
		texture->widthWidthHeightHeight[0] =
		    texture->widthWidthHeightHeight[1] = static_cast<float>(width);
		texture->widthWidthHeightHeight[2] =
		    texture->widthWidthHeightHeight[3] = static_cast<float>(height);

		texture->width[0] =
		    texture->width[1] =
		        texture->width[2] =
		            texture->width[3] = static_cast<float>(width);

		texture->height[0] =
		    texture->height[1] =
		        texture->height[2] =
		            texture->height[3] = static_cast<float>(height);

		texture->depth[0] =
		    texture->depth[1] =
		        texture->depth[2] =
		            texture->depth[3] = static_cast<float>(depth);
	}

	sw::Mipmap &mipmap = texture->mipmap[level];

	short halfTexelU = 0x8000 / width;
	short halfTexelV = 0x8000 / height;
	short halfTexelW = 0x8000 / depth;

	mipmap.uHalf[0] =
	    mipmap.uHalf[1] =
	        mipmap.uHalf[2] =
	            mipmap.uHalf[3] = halfTexelU;

	mipmap.vHalf[0] =
	    mipmap.vHalf[1] =
	        mipmap.vHalf[2] =
	            mipmap.vHalf[3] = halfTexelV;

	mipmap.wHalf[0] =
	    mipmap.wHalf[1] =
	        mipmap.wHalf[2] =
	            mipmap.wHalf[3] = halfTexelW;

	mipmap.width[0] =
	    mipmap.width[1] =
	        mipmap.width[2] =
	            mipmap.width[3] = width;

	mipmap.height[0] =
	    mipmap.height[1] =
	        mipmap.height[2] =
	            mipmap.height[3] = height;

	mipmap.depth[0] =
	    mipmap.depth[1] =
	        mipmap.depth[2] =
	            mipmap.depth[3] = depth;

	mipmap.onePitchP[0] = 1;
	mipmap.onePitchP[1] = static_cast<short>(pitchP);
	mipmap.onePitchP[2] = 1;
	mipmap.onePitchP[3] = static_cast<short>(pitchP);

	mipmap.pitchP[0] = pitchP;
	mipmap.pitchP[1] = pitchP;
	mipmap.pitchP[2] = pitchP;
	mipmap.pitchP[3] = pitchP;

	mipmap.sliceP[0] = sliceP;
	mipmap.sliceP[1] = sliceP;
	mipmap.sliceP[2] = sliceP;
	mipmap.sliceP[3] = sliceP;

	mipmap.samplePitchP[0] = samplePitchP;
	mipmap.samplePitchP[1] = samplePitchP;
	mipmap.samplePitchP[2] = samplePitchP;
	mipmap.samplePitchP[3] = samplePitchP;

	mipmap.sampleMax[0] = sampleMax;
	mipmap.sampleMax[1] = sampleMax;
	mipmap.sampleMax[2] = sampleMax;
	mipmap.sampleMax[3] = sampleMax;
}

void DescriptorSetLayout::WriteDescriptorSet(Device *device, const VkWriteDescriptorSet &writeDescriptorSet)
{
	DescriptorSet *dstSet = vk::Cast(writeDescriptorSet.dstSet);
	VkDescriptorUpdateTemplateEntry e;
	e.descriptorType = writeDescriptorSet.descriptorType;
	e.dstBinding = writeDescriptorSet.dstBinding;
	e.dstArrayElement = writeDescriptorSet.dstArrayElement;
	e.descriptorCount = writeDescriptorSet.descriptorCount;
	e.offset = 0;
	void const *ptr = nullptr;
	switch(writeDescriptorSet.descriptorType)
	{
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			ptr = writeDescriptorSet.pTexelBufferView;
			e.stride = sizeof(VkBufferView);
			break;

		case VK_DESCRIPTOR_TYPE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			ptr = writeDescriptorSet.pImageInfo;
			e.stride = sizeof(VkDescriptorImageInfo);
			break;

		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			ptr = writeDescriptorSet.pBufferInfo;
			e.stride = sizeof(VkDescriptorBufferInfo);
			break;

		default:
			UNSUPPORTED("descriptor type %u", writeDescriptorSet.descriptorType);
	}

	WriteDescriptorSet(device, dstSet, e, reinterpret_cast<char const *>(ptr));
}

void DescriptorSetLayout::CopyDescriptorSet(const VkCopyDescriptorSet &descriptorCopies)
{
	DescriptorSet *srcSet = vk::Cast(descriptorCopies.srcSet);
	DescriptorSetLayout *srcLayout = srcSet->header.layout;
	ASSERT(srcLayout);

	DescriptorSet *dstSet = vk::Cast(descriptorCopies.dstSet);
	DescriptorSetLayout *dstLayout = dstSet->header.layout;
	ASSERT(dstLayout);

	size_t srcTypeSize = 0;
	uint8_t *memToRead = srcLayout->getOffsetPointer(srcSet, descriptorCopies.srcBinding, descriptorCopies.srcArrayElement, descriptorCopies.descriptorCount, &srcTypeSize);

	size_t dstTypeSize = 0;
	uint8_t *memToWrite = dstLayout->getOffsetPointer(dstSet, descriptorCopies.dstBinding, descriptorCopies.dstArrayElement, descriptorCopies.descriptorCount, &dstTypeSize);

	ASSERT(srcTypeSize == dstTypeSize);
	size_t writeSize = dstTypeSize * descriptorCopies.descriptorCount;
	memcpy(memToWrite, memToRead, writeSize);
}

}  // namespace vk
