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
#include "System/Types.hpp"

#include <algorithm>
#include <cstring>

namespace
{

static bool UsesImmutableSamplers(const VkDescriptorSetLayoutBinding& binding)
{
	return (((binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER) ||
	        (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)) &&
	        (binding.pImmutableSamplers != nullptr));
}

}

namespace vk
{

DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, void* mem) :
	flags(pCreateInfo->flags), bindingCount(pCreateInfo->bindingCount), bindings(reinterpret_cast<VkDescriptorSetLayoutBinding*>(mem))
{
	uint8_t* hostMemory = static_cast<uint8_t*>(mem) + bindingCount * sizeof(VkDescriptorSetLayoutBinding);
	bindingOffsets = reinterpret_cast<size_t*>(hostMemory);
	hostMemory += bindingCount * sizeof(size_t);

	size_t offset = 0;
	for(uint32_t i = 0; i < bindingCount; i++)
	{
		bindings[i] = pCreateInfo->pBindings[i];
		if(UsesImmutableSamplers(bindings[i]))
		{
			size_t immutableSamplersSize = bindings[i].descriptorCount * sizeof(VkSampler);
			bindings[i].pImmutableSamplers = reinterpret_cast<const VkSampler*>(hostMemory);
			hostMemory += immutableSamplersSize;
			memcpy(const_cast<VkSampler*>(bindings[i].pImmutableSamplers),
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
}

void DescriptorSetLayout::destroy(const VkAllocationCallbacks* pAllocator)
{
	vk::deallocate(bindings, pAllocator); // This allocation also contains pImmutableSamplers
}

size_t DescriptorSetLayout::ComputeRequiredAllocationSize(const VkDescriptorSetLayoutCreateInfo* pCreateInfo)
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
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
	case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
		return sizeof(VkDescriptorImageInfo);
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		return sizeof(VkBufferView);
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
		return sizeof(VkDescriptorBufferInfo);
	default:
		UNIMPLEMENTED("Unsupported Descriptor Type");
	}

	return 0;
}

size_t DescriptorSetLayout::getDescriptorSetAllocationSize() const
{
	// vk::DescriptorSet has a layout member field.
	return sizeof(vk::DescriptorSetLayout*) + getDescriptorSetDataSize();
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

	ASSERT(false); // Bindings should always be found
	return 0;
}

void DescriptorSetLayout::initialize(VkDescriptorSet vkDescriptorSet)
{
	// Use a pointer to this descriptor set layout as the descriptor set's header
	DescriptorSet* descriptorSet = vk::Cast(vkDescriptorSet);
	descriptorSet->layout = this;
	uint8_t* mem = descriptorSet->data;

	for(uint32_t i = 0; i < bindingCount; i++)
	{
		size_t typeSize = GetDescriptorSize(bindings[i].descriptorType);
		if(UsesImmutableSamplers(bindings[i]))
		{
			for(uint32_t j = 0; j < bindings[i].descriptorCount; j++)
			{
				VkDescriptorImageInfo* imageInfo = reinterpret_cast<VkDescriptorImageInfo*>(mem);
				imageInfo->sampler = bindings[i].pImmutableSamplers[j];
				mem += typeSize;
			}
		}
		else
		{
			mem += bindings[i].descriptorCount * typeSize;
		}
	}
}

size_t DescriptorSetLayout::getBindingOffset(uint32_t binding) const
{
	uint32_t index = getBindingIndex(binding);
	return bindingOffsets[index] + OFFSET(DescriptorSet, data[0]);
}

uint8_t* DescriptorSetLayout::getOffsetPointer(DescriptorSet *descriptorSet, uint32_t binding, uint32_t arrayElement, uint32_t count, size_t* typeSize) const
{
	uint32_t index = getBindingIndex(binding);
	*typeSize = GetDescriptorSize(bindings[index].descriptorType);
	size_t byteOffset = bindingOffsets[index] + (*typeSize * arrayElement);
	ASSERT(((*typeSize * count) + byteOffset) <= getDescriptorSetDataSize()); // Make sure the operation will not go out of bounds
	return &descriptorSet->data[byteOffset];
}

const uint8_t* DescriptorSetLayout::GetInputData(const VkWriteDescriptorSet& descriptorWrites)
{
	switch(descriptorWrites.descriptorType)
	{
	case VK_DESCRIPTOR_TYPE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
	case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
		return reinterpret_cast<const uint8_t*>(descriptorWrites.pImageInfo);
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		return reinterpret_cast<const uint8_t*>(descriptorWrites.pTexelBufferView);
		break;
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
		return reinterpret_cast<const uint8_t*>(descriptorWrites.pBufferInfo);
		break;
	default:
		UNIMPLEMENTED("descriptorType");
		return nullptr;
	}
}

void DescriptorSetLayout::WriteDescriptorSet(const VkWriteDescriptorSet& descriptorWrites)
{
	DescriptorSet* dstSet = vk::Cast(descriptorWrites.dstSet);
	DescriptorSetLayout* dstLayout = dstSet->layout;
	ASSERT(dstLayout);
	ASSERT(dstLayout->bindings[dstLayout->getBindingIndex(descriptorWrites.dstBinding)].descriptorType == descriptorWrites.descriptorType);

	size_t typeSize = 0;
	uint8_t* memToWrite = dstLayout->getOffsetPointer(dstSet, descriptorWrites.dstBinding, descriptorWrites.dstArrayElement, descriptorWrites.descriptorCount, &typeSize);

	// If the dstBinding has fewer than descriptorCount array elements remaining
	// starting from dstArrayElement, then the remainder will be used to update
	// the subsequent binding - dstBinding+1 starting at array element zero. If
	// a binding has a descriptorCount of zero, it is skipped. This behavior
	// applies recursively, with the update affecting consecutive bindings as
	// needed to update all descriptorCount descriptors.
	size_t writeSize = typeSize * descriptorWrites.descriptorCount;
	memcpy(memToWrite, DescriptorSetLayout::GetInputData(descriptorWrites), writeSize);
}

void DescriptorSetLayout::CopyDescriptorSet(const VkCopyDescriptorSet& descriptorCopies)
{
	DescriptorSet* srcSet = vk::Cast(descriptorCopies.srcSet);
	DescriptorSetLayout* srcLayout = srcSet->layout;
	ASSERT(srcLayout);

	DescriptorSet* dstSet = vk::Cast(descriptorCopies.dstSet);
	DescriptorSetLayout* dstLayout = dstSet->layout;
	ASSERT(dstLayout);

	size_t srcTypeSize = 0;
	uint8_t* memToRead = srcLayout->getOffsetPointer(srcSet, descriptorCopies.srcBinding, descriptorCopies.srcArrayElement, descriptorCopies.descriptorCount, &srcTypeSize);

	size_t dstTypeSize = 0;
	uint8_t* memToWrite = dstLayout->getOffsetPointer(dstSet, descriptorCopies.dstBinding, descriptorCopies.dstArrayElement, descriptorCopies.descriptorCount, &dstTypeSize);

	ASSERT(srcTypeSize == dstTypeSize);
	size_t writeSize = dstTypeSize * descriptorCopies.descriptorCount;
	memcpy(memToWrite, memToRead, writeSize);
}

} // namespace vk