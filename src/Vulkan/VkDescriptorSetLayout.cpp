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

#include "Reactor/Reactor.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>

namespace vk {

static bool UsesImmutableSamplers(const VkDescriptorSetLayoutBinding &binding)
{
	return (((binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER) ||
	         (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)) &&
	        (binding.pImmutableSamplers != nullptr));
}

DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo *pCreateInfo, void *mem)
    : flags(pCreateInfo->flags)
    , bindings(reinterpret_cast<Binding *>(mem))
{
	// The highest binding number determines the size of the direct-indexed array.
	bindingsArraySize = 0;
	for(uint32_t i = 0; i < pCreateInfo->bindingCount; i++)
	{
		bindingsArraySize = std::max(bindingsArraySize, pCreateInfo->pBindings[i].binding + 1);
	}

	uint8_t *immutableSamplersStorage = static_cast<uint8_t *>(mem) + bindingsArraySize * sizeof(Binding);

	// pCreateInfo->pBindings[] can have gaps in the binding numbers, so first initialize the entire bindings array.
	// "Bindings that are not specified have a descriptorCount and stageFlags of zero, and the value of descriptorType is undefined."
	for(uint32_t i = 0; i < bindingsArraySize; i++)
	{
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		bindings[i].descriptorCount = 0;
		bindings[i].immutableSamplers = nullptr;
	}

	for(uint32_t i = 0; i < pCreateInfo->bindingCount; i++)
	{
		const auto &srcBinding = pCreateInfo->pBindings[i];
		auto &dstBinding = bindings[srcBinding.binding];

		dstBinding.descriptorType = srcBinding.descriptorType;
		dstBinding.descriptorCount = srcBinding.descriptorCount;

		if(UsesImmutableSamplers(srcBinding))
		{
			size_t immutableSamplersSize = dstBinding.descriptorCount * sizeof(VkSampler);
			dstBinding.immutableSamplers = reinterpret_cast<const vk::Sampler **>(immutableSamplersStorage);
			immutableSamplersStorage += immutableSamplersSize;

			for(uint32_t i = 0; i < dstBinding.descriptorCount; i++)
			{
				dstBinding.immutableSamplers[i] = vk::Cast(srcBinding.pImmutableSamplers[i]);
			}
		}
	}

	uint32_t offset = 0;
	for(uint32_t i = 0; i < bindingsArraySize; i++)
	{
		bindings[i].offset = offset;
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
	uint32_t bindingsArraySize = 0;
	uint32_t immutableSamplerCount = 0;
	for(uint32_t i = 0; i < pCreateInfo->bindingCount; i++)
	{
		bindingsArraySize = std::max(bindingsArraySize, pCreateInfo->pBindings[i].binding + 1);

		if(UsesImmutableSamplers(pCreateInfo->pBindings[i]))
		{
			immutableSamplerCount += pCreateInfo->pBindings[i].descriptorCount;
		}
	}

	return bindingsArraySize * sizeof(Binding) +
	       immutableSamplerCount * sizeof(VkSampler);
}

uint32_t DescriptorSetLayout::GetDescriptorSize(VkDescriptorType type)
{
	switch(type)
	{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return static_cast<uint32_t>(sizeof(SampledImageDescriptor));
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return static_cast<uint32_t>(sizeof(StorageImageDescriptor));
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return static_cast<uint32_t>(sizeof(BufferDescriptor));
		default:
			UNSUPPORTED("Unsupported Descriptor Type: %d", int(type));
			return 0;
	}
}

bool DescriptorSetLayout::IsDescriptorDynamic(VkDescriptorType type)
{
	return type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
	       type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
}

size_t DescriptorSetLayout::getDescriptorSetAllocationSize() const
{
	// vk::DescriptorSet has a header with a pointer to the layout.
	return sw::align<alignof(DescriptorSet)>(OFFSET(DescriptorSet, data) + getDescriptorSetDataSize());
}

size_t DescriptorSetLayout::getDescriptorSetDataSize() const
{
	size_t size = 0;
	for(uint32_t i = 0; i < bindingsArraySize; i++)
	{
		size += bindings[i].descriptorCount * GetDescriptorSize(bindings[i].descriptorType);
	}

	return size;
}

void DescriptorSetLayout::initialize(DescriptorSet *descriptorSet)
{
	ASSERT(descriptorSet->header.layout == nullptr);

	// Use a pointer to this descriptor set layout as the descriptor set's header
	descriptorSet->header.layout = this;
	uint8_t *mem = descriptorSet->data;

	for(uint32_t i = 0; i < bindingsArraySize; i++)
	{
		size_t descriptorSize = GetDescriptorSize(bindings[i].descriptorType);

		if(bindings[i].immutableSamplers)
		{
			for(uint32_t j = 0; j < bindings[i].descriptorCount; j++)
			{
				SampledImageDescriptor *imageSamplerDescriptor = reinterpret_cast<SampledImageDescriptor *>(mem);
				imageSamplerDescriptor->updateSampler(bindings[i].immutableSamplers[j]);
				mem += descriptorSize;
			}
		}
		else
		{
			mem += bindings[i].descriptorCount * descriptorSize;
		}
	}
}

uint32_t DescriptorSetLayout::getBindingOffset(uint32_t bindingNumber) const
{
	ASSERT(bindingNumber < bindingsArraySize);
	return bindings[bindingNumber].offset;
}

uint32_t DescriptorSetLayout::getDescriptorCount(uint32_t bindingNumber) const
{
	ASSERT(bindingNumber < bindingsArraySize);
	return bindings[bindingNumber].descriptorCount;
}

uint32_t DescriptorSetLayout::getDynamicDescriptorCount() const
{
	uint32_t count = 0;
	for(size_t i = 0; i < bindingsArraySize; i++)
	{
		if(IsDescriptorDynamic(bindings[i].descriptorType))
		{
			count += bindings[i].descriptorCount;
		}
	}

	return count;
}

uint32_t DescriptorSetLayout::getDynamicOffsetIndex(uint32_t bindingNumber) const
{
	ASSERT(bindingNumber < bindingsArraySize);
	ASSERT(IsDescriptorDynamic(bindings[bindingNumber].descriptorType));

	uint32_t index = 0;
	for(uint32_t i = 0; i < bindingNumber; i++)
	{
		if(IsDescriptorDynamic(bindings[i].descriptorType))
		{
			index += bindings[i].descriptorCount;
		}
	}

	return index;
}

VkDescriptorType DescriptorSetLayout::getDescriptorType(uint32_t bindingNumber) const
{
	ASSERT(bindingNumber < bindingsArraySize);
	return bindings[bindingNumber].descriptorType;
}

uint8_t *DescriptorSetLayout::getDescriptorPointer(DescriptorSet *descriptorSet, uint32_t bindingNumber, uint32_t arrayElement, uint32_t count, size_t *typeSize) const
{
	ASSERT(bindingNumber < bindingsArraySize);
	*typeSize = GetDescriptorSize(bindings[bindingNumber].descriptorType);
	size_t byteOffset = bindings[bindingNumber].offset + (*typeSize * arrayElement);
	ASSERT(((*typeSize * count) + byteOffset) <= getDescriptorSetDataSize());  // Make sure the operation will not go out of bounds

	return &descriptorSet->data[byteOffset];
}

void SampledImageDescriptor::updateSampler(const vk::Sampler *newSampler)
{
	memcpy(reinterpret_cast<void *>(&sampler), newSampler, sizeof(sampler));
}

void DescriptorSetLayout::WriteDescriptorSet(Device *device, DescriptorSet *dstSet, VkDescriptorUpdateTemplateEntry const &entry, char const *src)
{
	DescriptorSetLayout *dstLayout = dstSet->header.layout;
	auto &binding = dstLayout->bindings[entry.dstBinding];
	ASSERT(dstLayout);
	ASSERT(binding.descriptorType == entry.descriptorType);

	size_t typeSize = 0;
	uint8_t *memToWrite = dstLayout->getDescriptorPointer(dstSet, entry.dstBinding, entry.dstArrayElement, entry.descriptorCount, &typeSize);

	ASSERT(reinterpret_cast<intptr_t>(memToWrite) % 16 == 0);  // Each descriptor must be 16-byte aligned.

	if(entry.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
	{
		SampledImageDescriptor *sampledImage = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);
			// "All consecutive bindings updated via a single VkWriteDescriptorSet structure, except those with a
			//  descriptorCount of zero, must all either use immutable samplers or must all not use immutable samplers."
			if(!binding.immutableSamplers)
			{
				sampledImage[i].updateSampler(vk::Cast(update->sampler));
			}
			sampledImage[i].device = device;
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
	{
		SampledImageDescriptor *sampledImage = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkBufferView const *>(src + entry.offset + entry.stride * i);
			auto bufferView = vk::Cast(*update);

			sampledImage[i].type = VK_IMAGE_VIEW_TYPE_1D;
			sampledImage[i].imageViewId = bufferView->id;
			constexpr VkComponentMapping identityMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			sampledImage[i].swizzle = ResolveComponentMapping(identityMapping, bufferView->getFormat());
			sampledImage[i].format = bufferView->getFormat();

			auto numElements = bufferView->getElementCount();
			sampledImage[i].width = numElements;
			sampledImage[i].height = 1;
			sampledImage[i].depth = 1;
			sampledImage[i].mipLevels = 1;
			sampledImage[i].sampleCount = 1;
			sampledImage[i].texture.widthWidthHeightHeight = sw::float4(static_cast<float>(numElements), static_cast<float>(numElements), 1, 1);
			sampledImage[i].texture.width = sw::float4(static_cast<float>(numElements));
			sampledImage[i].texture.height = sw::float4(1);
			sampledImage[i].texture.depth = sw::float4(1);
			sampledImage[i].device = device;

			sw::Mipmap &mipmap = sampledImage[i].texture.mipmap[0];
			mipmap.buffer = bufferView->getPointer();
			mipmap.width[0] = mipmap.width[1] = mipmap.width[2] = mipmap.width[3] = numElements;
			mipmap.height[0] = mipmap.height[1] = mipmap.height[2] = mipmap.height[3] = 1;
			mipmap.depth[0] = mipmap.depth[1] = mipmap.depth[2] = mipmap.depth[3] = 1;
			mipmap.pitchP.x = mipmap.pitchP.y = mipmap.pitchP.z = mipmap.pitchP.w = numElements;
			mipmap.sliceP.x = mipmap.sliceP.y = mipmap.sliceP.z = mipmap.sliceP.w = 0;
			mipmap.onePitchP[0] = mipmap.onePitchP[2] = 1;
			mipmap.onePitchP[1] = mipmap.onePitchP[3] = 0;
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
	{
		SampledImageDescriptor *sampledImage = reinterpret_cast<SampledImageDescriptor *>(memToWrite);

		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto *update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);

			vk::ImageView *imageView = vk::Cast(update->imageView);
			Format format = imageView->getFormat(ImageView::SAMPLING);

			sw::Texture *texture = &sampledImage[i].texture;

			if(entry.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				// "All consecutive bindings updated via a single VkWriteDescriptorSet structure, except those with a
				//  descriptorCount of zero, must all either use immutable samplers or must all not use immutable samplers."
				if(!binding.immutableSamplers)
				{
					sampledImage[i].updateSampler(vk::Cast(update->sampler));
				}
			}

			const auto &extent = imageView->getMipLevelExtent(0);

			sampledImage[i].imageViewId = imageView->id;
			sampledImage[i].width = extent.width;
			sampledImage[i].height = extent.height;
			sampledImage[i].depth = imageView->getDepthOrLayerCount(0);
			sampledImage[i].mipLevels = imageView->getSubresourceRange().levelCount;
			sampledImage[i].sampleCount = imageView->getSampleCount();
			sampledImage[i].type = imageView->getType();
			sampledImage[i].swizzle = imageView->getComponentMapping();
			sampledImage[i].format = format;
			sampledImage[i].device = device;
			sampledImage[i].memoryOwner = imageView;

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

				VkExtent2D extent = imageView->getMipLevelExtent(0);

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

					VkExtent2D extent = imageView->getMipLevelExtent(level);

					int width = extent.width;
					int height = extent.height;
					int layerCount = imageView->getSubresourceRange().layerCount;
					int depth = imageView->getDepthOrLayerCount(level);
					int bytes = format.bytes();
					int pitchP = imageView->rowPitchBytes(aspect, level, ImageView::SAMPLING) / bytes;
					int sliceP = (layerCount > 1 ? imageView->layerPitchBytes(aspect, ImageView::SAMPLING) : imageView->slicePitchBytes(aspect, level, ImageView::SAMPLING)) / bytes;
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
		auto storageImage = reinterpret_cast<StorageImageDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto *update = reinterpret_cast<VkDescriptorImageInfo const *>(src + entry.offset + entry.stride * i);
			auto *imageView = vk::Cast(update->imageView);
			const auto &extent = imageView->getMipLevelExtent(0);
			auto layerCount = imageView->getSubresourceRange().layerCount;

			storageImage[i].ptr = imageView->getOffsetPointer({ 0, 0, 0 }, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0);
			storageImage[i].width = extent.width;
			storageImage[i].height = extent.height;
			storageImage[i].depth = imageView->getDepthOrLayerCount(0);
			storageImage[i].rowPitchBytes = imageView->rowPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			storageImage[i].samplePitchBytes = imageView->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			storageImage[i].slicePitchBytes = layerCount > 1
			                                      ? imageView->layerPitchBytes(VK_IMAGE_ASPECT_COLOR_BIT)
			                                      : imageView->slicePitchBytes(VK_IMAGE_ASPECT_COLOR_BIT, 0);
			storageImage[i].sampleCount = imageView->getSampleCount();
			storageImage[i].sizeInBytes = static_cast<int>(imageView->getSizeInBytes());
			storageImage[i].memoryOwner = imageView;

			if(imageView->getFormat().isStencil())
			{
				storageImage[i].stencilPtr = imageView->getOffsetPointer({ 0, 0, 0 }, VK_IMAGE_ASPECT_STENCIL_BIT, 0, 0);
				storageImage[i].stencilRowPitchBytes = imageView->rowPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
				storageImage[i].stencilSamplePitchBytes = imageView->slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
				storageImage[i].stencilSlicePitchBytes = (imageView->getSubresourceRange().layerCount > 1)
				                                             ? imageView->layerPitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT)
				                                             : imageView->slicePitchBytes(VK_IMAGE_ASPECT_STENCIL_BIT, 0);
			}
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)
	{
		auto *storageImage = reinterpret_cast<StorageImageDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkBufferView const *>(src + entry.offset + entry.stride * i);
			auto bufferView = vk::Cast(*update);
			storageImage[i].ptr = bufferView->getPointer();
			storageImage[i].width = bufferView->getElementCount();
			storageImage[i].height = 1;
			storageImage[i].depth = 1;
			storageImage[i].rowPitchBytes = 0;
			storageImage[i].slicePitchBytes = 0;
			storageImage[i].samplePitchBytes = 0;
			storageImage[i].sampleCount = 1;
			storageImage[i].sizeInBytes = bufferView->getRangeInBytes();
		}
	}
	else if(entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
	        entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
	{
		auto *bufferDescriptor = reinterpret_cast<BufferDescriptor *>(memToWrite);
		for(uint32_t i = 0; i < entry.descriptorCount; i++)
		{
			auto update = reinterpret_cast<VkDescriptorBufferInfo const *>(src + entry.offset + entry.stride * i);
			auto buffer = vk::Cast(update->buffer);
			bufferDescriptor[i].ptr = buffer->getOffsetPointer(update->offset);
			bufferDescriptor[i].sizeInBytes = static_cast<int>((update->range == VK_WHOLE_SIZE) ? buffer->getSize() - update->offset : update->range);
			bufferDescriptor[i].robustnessSize = static_cast<int>(buffer->getSize() - update->offset);
		}
	}
}

void DescriptorSetLayout::WriteTextureLevelInfo(sw::Texture *texture, int level, int width, int height, int depth, int pitchP, int sliceP, int samplePitchP, int sampleMax)
{
	if(level == 0)
	{
		texture->widthWidthHeightHeight[0] = static_cast<float>(width);
		texture->widthWidthHeightHeight[1] = static_cast<float>(width);
		texture->widthWidthHeightHeight[2] = static_cast<float>(height);
		texture->widthWidthHeightHeight[3] = static_cast<float>(height);

		texture->width = sw::float4(static_cast<float>(width));
		texture->height = sw::float4(static_cast<float>(height));
		texture->depth = sw::float4(static_cast<float>(depth));
	}

	sw::Mipmap &mipmap = texture->mipmap[level];

	short halfTexelU = 0x8000 / width;
	short halfTexelV = 0x8000 / height;
	short halfTexelW = 0x8000 / depth;

	mipmap.uHalf = sw::short4(halfTexelU);
	mipmap.vHalf = sw::short4(halfTexelV);
	mipmap.wHalf = sw::short4(halfTexelW);

	mipmap.width = sw::int4(width);
	mipmap.height = sw::int4(height);
	mipmap.depth = sw::int4(depth);

	mipmap.onePitchP[0] = 1;
	mipmap.onePitchP[1] = static_cast<short>(pitchP);
	mipmap.onePitchP[2] = 1;
	mipmap.onePitchP[3] = static_cast<short>(pitchP);

	mipmap.pitchP = sw::int4(pitchP);
	mipmap.sliceP = sw::int4(sliceP);
	mipmap.samplePitchP = sw::int4(samplePitchP);
	mipmap.sampleMax = sw::int4(sampleMax);
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
	uint8_t *memToRead = srcLayout->getDescriptorPointer(srcSet, descriptorCopies.srcBinding, descriptorCopies.srcArrayElement, descriptorCopies.descriptorCount, &srcTypeSize);

	size_t dstTypeSize = 0;
	uint8_t *memToWrite = dstLayout->getDescriptorPointer(dstSet, descriptorCopies.dstBinding, descriptorCopies.dstArrayElement, descriptorCopies.descriptorCount, &dstTypeSize);

	ASSERT(srcTypeSize == dstTypeSize);
	size_t writeSize = dstTypeSize * descriptorCopies.descriptorCount;
	memcpy(memToWrite, memToRead, writeSize);
}

}  // namespace vk
