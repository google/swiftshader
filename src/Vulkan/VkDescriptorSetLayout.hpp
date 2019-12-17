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

#ifndef VK_DESCRIPTOR_SET_LAYOUT_HPP_
#define VK_DESCRIPTOR_SET_LAYOUT_HPP_

#include "VkObject.hpp"

#include "Device/Sampler.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Vulkan/VkSampler.hpp"

namespace vk {

class DescriptorSet;
class Device;

// TODO(b/129523279): Move to the Device or Pipeline layer.
struct alignas(16) SampledImageDescriptor
{
	~SampledImageDescriptor() = delete;

	void updateSampler(VkSampler sampler);

	// TODO(b/129523279): Minimize to the data actually needed.
	vk::Sampler sampler;
	vk::Device *device;

	uint32_t imageViewId;
	VkImageViewType type;
	VkFormat format;
	VkComponentMapping swizzle;
	alignas(16) sw::Texture texture;
	VkExtent3D extent;  // Of base mip-level.
	int arrayLayers;
	int mipLevels;
	int sampleCount;
};

struct alignas(16) StorageImageDescriptor
{
	~StorageImageDescriptor() = delete;

	void *ptr;
	VkExtent3D extent;
	int rowPitchBytes;
	int slicePitchBytes;
	int samplePitchBytes;
	int arrayLayers;
	int sampleCount;
	int sizeInBytes;

	void *stencilPtr;
	int stencilRowPitchBytes;
	int stencilSlicePitchBytes;
	int stencilSamplePitchBytes;
};

struct alignas(16) BufferDescriptor
{
	~BufferDescriptor() = delete;

	void *ptr;
	int sizeInBytes;     // intended size of the bound region -- slides along with dynamic offsets
	int robustnessSize;  // total accessible size from static offset -- does not move with dynamic offset
};

class DescriptorSetLayout : public Object<DescriptorSetLayout, VkDescriptorSetLayout>
{
public:
	DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo *pCreateInfo, void *mem);
	void destroy(const VkAllocationCallbacks *pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkDescriptorSetLayoutCreateInfo *pCreateInfo);

	static size_t GetDescriptorSize(VkDescriptorType type);
	static void WriteDescriptorSet(Device *device, const VkWriteDescriptorSet &descriptorWrites);
	static void CopyDescriptorSet(const VkCopyDescriptorSet &descriptorCopies);

	static void WriteDescriptorSet(Device *device, DescriptorSet *dstSet, VkDescriptorUpdateTemplateEntry const &entry, char const *src);
	static void WriteTextureLevelInfo(sw::Texture *texture, int level, int width, int height, int depth, int pitchP, int sliceP, int samplePitchP, int sampleMax);

	void initialize(DescriptorSet *descriptorSet);

	// Returns the total size of the descriptor set in bytes.
	size_t getDescriptorSetAllocationSize() const;

	// Returns the number of bindings in the descriptor set.
	size_t getBindingCount() const;

	// Returns true iff the given binding exists.
	bool hasBinding(uint32_t binding) const;

	// Returns the byte offset from the base address of the descriptor set for
	// the given binding and array element within that binding.
	size_t getBindingOffset(uint32_t binding, size_t arrayElement) const;

	// Returns the stride of an array of descriptors
	size_t getBindingStride(uint32_t binding) const;

	// Returns the number of descriptors across all bindings that are dynamic
	// (see isBindingDynamic).
	uint32_t getDynamicDescriptorCount() const;

	// Returns the relative offset into the pipeline's dynamic offsets array for
	// the given binding. This offset should be added to the base offset
	// returned by PipelineLayout::getDynamicOffsetBase() to produce the
	// starting index for dynamic descriptors.
	uint32_t getDynamicDescriptorOffset(uint32_t binding) const;

	// Returns true if the given binding is of type:
	//  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC or
	//  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
	bool isBindingDynamic(uint32_t binding) const;

	// Returns the VkDescriptorSetLayoutBinding for the given binding.
	VkDescriptorSetLayoutBinding const &getBindingLayout(uint32_t binding) const;

	uint8_t *getOffsetPointer(DescriptorSet *descriptorSet, uint32_t binding, uint32_t arrayElement, uint32_t count, size_t *typeSize) const;

private:
	size_t getDescriptorSetDataSize() const;
	uint32_t getBindingIndex(uint32_t binding) const;
	static bool isDynamic(VkDescriptorType type);

	VkDescriptorSetLayoutCreateFlags flags;
	uint32_t bindingCount;
	VkDescriptorSetLayoutBinding *bindings;
	size_t *bindingOffsets;
};

static inline DescriptorSetLayout *Cast(VkDescriptorSetLayout object)
{
	return DescriptorSetLayout::Cast(object);
}

}  // namespace vk

#endif  // VK_DESCRIPTOR_SET_LAYOUT_HPP_
