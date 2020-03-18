// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_FORMAT_HPP_
#define VK_FORMAT_HPP_

#include "System/Types.hpp"

#include <Vulkan/VulkanPlatform.h>

namespace vk {

class Format
{
public:
	Format() {}
	Format(VkFormat format)
	    : format(format)
	{}
	inline operator VkFormat() const { return format; }

	bool isUnsignedNormalized() const;
	bool isSignedNormalized() const;
	bool isSignedUnnormalizedInteger() const;
	bool isUnsignedUnnormalizedInteger() const;
	bool isUnnormalizedInteger() const;

	VkImageAspectFlags getAspects() const;
	Format getAspectFormat(VkImageAspectFlags aspect) const;
	bool isStencil() const;
	bool isDepth() const;
	bool isSRGBformat() const;
	bool isFloatFormat() const;
	bool isYcbcrFormat() const;

	bool isCompatible(const Format &other) const;
	bool isCompressed() const;
	VkFormat getDecompressedFormat() const;
	int blockWidth() const;
	int blockHeight() const;
	int bytesPerBlock() const;

	int componentCount() const;
	bool isUnsignedComponent(int component) const;

	int bytes() const;
	int pitchB(int width, int border, bool target) const;
	int sliceB(int width, int height, int border, bool target) const;

	sw::float4 getScale() const;

	// Texture sampling utilities
	bool has16bitTextureFormat() const;
	bool has8bitTextureComponents() const;
	bool has16bitTextureComponents() const;
	bool has32bitIntegerTextureComponents() const;
	bool isRGBComponent(int component) const;

	static uint8_t mapTo8bit(VkFormat format);

private:
	VkFormat compatibleFormat() const;
	int sliceBUnpadded(int width, int height, int border, bool target) const;

	VkFormat format = VK_FORMAT_UNDEFINED;
};

}  // namespace vk

#endif  // VK_FORMAT_HPP_