// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef sw_Sampler_hpp
#define sw_Sampler_hpp

#include "Device/Config.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkFormat.h"

namespace vk {
class Image;
}

namespace sw {

struct Mipmap
{
	const void *buffer;

	short4 uHalf;
	short4 vHalf;
	short4 wHalf;
	int4 width;
	int4 height;
	int4 depth;
	short4 onePitchP;
	int4 pitchP;
	int4 sliceP;
	int4 samplePitchP;
	int4 sampleMax;
};

struct Texture
{
	Mipmap mipmap[MIPMAP_LEVELS];

	float4 widthWidthHeightHeight;
	float4 width;
	float4 height;
	float4 depth;
};

enum FilterType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
{
	FILTER_POINT,
	FILTER_GATHER,
	FILTER_MIN_POINT_MAG_LINEAR,
	FILTER_MIN_LINEAR_MAG_POINT,
	FILTER_LINEAR,
	FILTER_ANISOTROPIC,

	FILTER_LAST = FILTER_ANISOTROPIC
};

enum MipmapType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
{
	MIPMAP_NONE,
	MIPMAP_POINT,
	MIPMAP_LINEAR,

	MIPMAP_LAST = MIPMAP_LINEAR
};

enum AddressingMode ENUM_UNDERLYING_TYPE_UNSIGNED_INT
{
	ADDRESSING_UNUSED,
	ADDRESSING_WRAP,
	ADDRESSING_CLAMP,
	ADDRESSING_MIRROR,
	ADDRESSING_MIRRORONCE,
	ADDRESSING_BORDER,    // Single color
	ADDRESSING_SEAMLESS,  // Border of pixels
	ADDRESSING_CUBEFACE,  // Cube face layer
	ADDRESSING_LAYER,     // Array layer
	ADDRESSING_TEXELFETCH,

	ADDRESSING_LAST = ADDRESSING_TEXELFETCH
};

struct Sampler
{
	VkImageViewType textureType;
	vk::Format textureFormat;
	FilterType textureFilter;
	AddressingMode addressingModeU;
	AddressingMode addressingModeV;
	AddressingMode addressingModeW;
	AddressingMode addressingModeY;
	MipmapType mipmapFilter;
	VkComponentMapping swizzle;
	int gatherComponent;
	bool highPrecisionFiltering;
	bool compareEnable;
	VkCompareOp compareOp;
	VkBorderColor border;
	bool unnormalizedCoordinates;
	bool largeTexture;

	VkSamplerYcbcrModelConversion ycbcrModel;
	bool studioSwing;    // Narrow range
	bool swappedChroma;  // Cb/Cr components in reverse order

	float mipLodBias = 0.0f;
	float maxAnisotropy = 0.0f;
	float minLod = 0.0f;
	float maxLod = 0.0f;
};

}  // namespace sw

#endif  // sw_Sampler_hpp
