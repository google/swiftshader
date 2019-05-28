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

#include "Device/Color.hpp"
#include "Device/Config.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkFormat.h"

namespace vk
{
	class Image;
}

namespace sw
{
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
	};

	struct Texture
	{
		Mipmap mipmap[MIPMAP_LEVELS];

		float4 widthWidthHeightHeight;
		float4 width;
		float4 height;
		float4 depth;
	};

	enum SamplerType
	{
		SAMPLER_PIXEL,
		SAMPLER_VERTEX
	};

	enum TextureType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		TEXTURE_NULL,       // TODO(b/129523279): Eliminate
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_CUBE,
		TEXTURE_1D_ARRAY,   // Treated as 2D texture with second coordinate 0. TODO(b/134669567)
		TEXTURE_2D_ARRAY,
		TEXTURE_CUBE_ARRAY,

		TEXTURE_LAST = TEXTURE_CUBE_ARRAY
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
		ADDRESSING_BORDER,     // Single color
		ADDRESSING_SEAMLESS,   // Border of pixels
		ADDRESSING_CUBEFACE,   // Cube face layer
		ADDRESSING_LAYER,      // Array layer
		ADDRESSING_TEXELFETCH,

		ADDRESSING_LAST = ADDRESSING_TEXELFETCH
	};

	enum CompareFunc ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		COMPARE_BYPASS,
		COMPARE_LESSEQUAL,
		COMPARE_GREATEREQUAL,
		COMPARE_LESS,
		COMPARE_GREATER,
		COMPARE_EQUAL,
		COMPARE_NOTEQUAL,
		COMPARE_ALWAYS,
		COMPARE_NEVER,

		COMPARE_LAST = COMPARE_NEVER
	};

	enum SwizzleType ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		SWIZZLE_RED,
		SWIZZLE_GREEN,
		SWIZZLE_BLUE,
		SWIZZLE_ALPHA,
		SWIZZLE_ZERO,
		SWIZZLE_ONE,

		SWIZZLE_LAST = SWIZZLE_ONE
	};

	struct Sampler
	{
		TextureType textureType;
		vk::Format textureFormat;
		FilterType textureFilter;
		AddressingMode addressingModeU;
		AddressingMode addressingModeV;
		AddressingMode addressingModeW;
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
	};
}

#endif   // sw_Sampler_hpp
