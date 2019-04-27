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

#include "SpirvShader.hpp"

#include "SamplerCore.hpp" // TODO: Figure out what's needed.
#include "System/Math.hpp"
#include "Vulkan/VkBuffer.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkDescriptorSet.hpp"
#include "Vulkan/VkPipelineLayout.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Vulkan/VkSampler.hpp"
#include "Vulkan/VkDescriptorSetLayout.hpp"
#include "Device/Config.hpp"

#include <spirv/unified1/spirv.hpp>
#include <spirv/unified1/GLSL.std.450.h>

#include <mutex>

#ifdef Bool
#undef Bool // b/127920555
#undef None
#endif

namespace sw {

SpirvShader::ImageSampler *SpirvShader::getImageSampler(uint32_t instruction, const vk::ImageView *imageView, const vk::Sampler *sampler)
{
	// TODO(b/129523279): Move somewhere sensible.
	static std::unordered_map<uint64_t, ImageSampler*> cache;
	static std::mutex mutex;

	// FIXME(b/129523279): Take instruction opcode and optional parameters into acount (SamplerMethod / SamplerOption).
	auto key = (static_cast<uint64_t>(imageView->id) << 32) | static_cast<uint64_t>(sampler->id);

	std::unique_lock<std::mutex> lock(mutex);
	auto it = cache.find(key);
	if (it != cache.end()) { return it->second; }

	// TODO: Hold a separate mutex lock for the sampler being built.
	auto function = rr::Function<Void(Pointer<Byte> image, Pointer<SIMD::Float>, Pointer<SIMD::Float>, Pointer<Byte>)>();
	Pointer<Byte> image = function.Arg<0>();
	Pointer<SIMD::Float> in = function.Arg<1>();
	Pointer<SIMD::Float> out = function.Arg<2>();
	Pointer<Byte> constants = function.Arg<3>();

	emitSamplerFunction({instruction}, imageView, sampler, image, in, out, constants);

	auto fptr = reinterpret_cast<ImageSampler*>((void *)function("sampler")->getEntry());
	cache.emplace(key, fptr);
	return fptr;
}

void SpirvShader::emitSamplerFunction(
        ImageInstruction instruction,
        const vk::ImageView *imageView, const vk::Sampler *sampler,
        Pointer<Byte> image, Pointer<SIMD::Float> in, Pointer<Byte> out, Pointer<Byte> constants)
{
	Sampler::State samplerState = {};
	samplerState.textureType = convertTextureType(imageView->getType());
	samplerState.textureFormat = imageView->getFormat();
	samplerState.textureFilter = convertFilterMode(sampler);
	samplerState.border = sampler->borderColor;

	samplerState.addressingModeU = convertAddressingMode(0, sampler->addressModeU, imageView->getType());
	samplerState.addressingModeV = convertAddressingMode(1, sampler->addressModeV, imageView->getType());
	samplerState.addressingModeW = convertAddressingMode(2, sampler->addressModeW, imageView->getType());

	samplerState.mipmapFilter = convertMipmapMode(sampler);
	samplerState.sRGB = imageView->getFormat().isSRGBformat();
	samplerState.swizzle = imageView->getComponentMapping();
	samplerState.highPrecisionFiltering = false;
	samplerState.compare = COMPARE_BYPASS;                  ASSERT(sampler->compareEnable == VK_FALSE);  // TODO(b/129523279)

//	minLod  // TODO(b/129523279)
//	maxLod  // TODO(b/129523279)
//	borderColor  // TODO(b/129523279)
	ASSERT(sampler->mipLodBias == 0.0f);  // TODO(b/129523279)
	ASSERT(sampler->anisotropyEnable == VK_FALSE);  // TODO(b/129523279)
	ASSERT(sampler->unnormalizedCoordinates == VK_FALSE);  // TODO(b/129523279)

	SamplerCore s(constants, samplerState);

	Pointer<Byte> texture = image + OFFSET(vk::SampledImageDescriptor, texture);  // sw::Texture*
	SIMD::Float uvw[3];
	SIMD::Float q(0);     // TODO(b/129523279)
	SIMD::Float bias(0);  // Bias added to the implicit level-of-detail, or explicit level-of-detail (depending on samplerMethod).
	Vector4f dsx;
	Vector4f dsy;
	Vector4f offset;
	SamplerFunction samplerFunction = instruction.getSamplerFunction();

	uint32_t i = 0;
	for( ; i < instruction.coordinates; i++)
	{
		uvw[i] = in[i];
	}

	// TODO(b/129523279): Currently 1D textures are treated as 2D by setting the second coordinate to 0.
	// Implement optimized 1D sampling.
	if(imageView->getType() == VK_IMAGE_VIEW_TYPE_1D ||
	   imageView->getType() == VK_IMAGE_VIEW_TYPE_1D_ARRAY)
	{
		uvw[1] = SIMD::Float(0);
	}

	// Lod and Grad are explicit-lod image operands, and always come after the coordinates.
	if(instruction.samplerMethod == Lod)
	{
		bias = in[instruction.coordinates];
	}
	else if(instruction.samplerMethod == Grad)
	{
		for(uint32_t j = 0; j < instruction.gradComponents; j++, i++)
		{
			dsx[j] = in[i];
		}

		for(uint32_t j = 0; j < instruction.gradComponents; j++, i++)
		{
			dsy[j] = in[i];
		}
	}

	if(instruction.samplerOption == Offset)
	{
		for(uint32_t j = 0; j < instruction.offsetComponents; j++, i++)
		{
			offset[j] = in[i];
		}
	}

	Vector4f sample = s.sampleTexture(texture, uvw[0], uvw[1], uvw[2], q, bias, dsx, dsy, offset, samplerFunction);

	Pointer<SIMD::Float> rgba = out;
	rgba[0] = sample.x;
	rgba[1] = sample.y;
	rgba[2] = sample.z;
	rgba[3] = sample.w;
}

sw::TextureType SpirvShader::convertTextureType(VkImageViewType imageViewType)
{
	switch(imageViewType)
	{
	case VK_IMAGE_VIEW_TYPE_1D:         return TEXTURE_1D;
	case VK_IMAGE_VIEW_TYPE_2D:         return TEXTURE_2D;
//	case VK_IMAGE_VIEW_TYPE_3D:         return TEXTURE_3D;
	case VK_IMAGE_VIEW_TYPE_CUBE:       return TEXTURE_CUBE;
//	case VK_IMAGE_VIEW_TYPE_1D_ARRAY:   return TEXTURE_1D_ARRAY;
	case VK_IMAGE_VIEW_TYPE_2D_ARRAY:   return TEXTURE_2D_ARRAY;
//	case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: return TEXTURE_CUBE_ARRAY;
	default:
		UNIMPLEMENTED("imageViewType %d", imageViewType);
		return TEXTURE_2D;
	}
}

sw::FilterType SpirvShader::convertFilterMode(const vk::Sampler *sampler)
{
	switch(sampler->magFilter)
	{
	case VK_FILTER_NEAREST:
		switch(sampler->minFilter)
		{
		case VK_FILTER_NEAREST: return FILTER_POINT;
		case VK_FILTER_LINEAR:  return FILTER_MIN_LINEAR_MAG_POINT;
		default:
			UNIMPLEMENTED("minFilter %d", sampler->minFilter);
			return FILTER_POINT;
		}
		break;
	case VK_FILTER_LINEAR:
		switch(sampler->minFilter)
		{
		case VK_FILTER_NEAREST: return FILTER_MIN_POINT_MAG_LINEAR;
		case VK_FILTER_LINEAR:  return FILTER_LINEAR;
		default:
			UNIMPLEMENTED("minFilter %d", sampler->minFilter);
			return FILTER_POINT;
		}
		break;
	default:
		UNIMPLEMENTED("magFilter %d", sampler->magFilter);
		return FILTER_POINT;
	}
}

sw::MipmapType SpirvShader::convertMipmapMode(const vk::Sampler *sampler)
{
	switch(sampler->mipmapMode)
	{
	case VK_SAMPLER_MIPMAP_MODE_NEAREST: return MIPMAP_POINT;
	case VK_SAMPLER_MIPMAP_MODE_LINEAR:  return MIPMAP_LINEAR;
	default:
		UNIMPLEMENTED("mipmapMode %d", sampler->mipmapMode);
		return MIPMAP_POINT;
	}
}

sw::AddressingMode SpirvShader::convertAddressingMode(int coordinateIndex, VkSamplerAddressMode addressMode, VkImageViewType imageViewType)
{
	switch(imageViewType)
	{
	case VK_IMAGE_VIEW_TYPE_CUBE:
		break;
	case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
		UNSUPPORTED("ImageCubeArray");
		if(coordinateIndex == 3)
		{
			return ADDRESSING_LAYER;
		}
		break;
	case VK_IMAGE_VIEW_TYPE_1D:
	case VK_IMAGE_VIEW_TYPE_2D:
//	case VK_IMAGE_VIEW_TYPE_3D:
		break;
//	case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
		break;
	case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
		if(coordinateIndex == 2)
		{
			return ADDRESSING_LAYER;
		}
		break;
	default:
		UNIMPLEMENTED("imageViewType %d", imageViewType);
		return ADDRESSING_WRAP;
	}

	// Vulkan 1.1 spec:
	// "Cube images ignore the wrap modes specified in the sampler. Instead, if VK_FILTER_NEAREST is used within a mip level then
	//  VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE is used, and if VK_FILTER_LINEAR is used within a mip level then sampling at the edges
	//  is performed as described earlier in the Cube map edge handling section."
	// This corresponds with our 'seamless' addressing mode.
	switch(imageViewType)
	{
	case VK_IMAGE_VIEW_TYPE_CUBE:
		return ADDRESSING_SEAMLESS;
//	case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
		UNSUPPORTED("ImageCubeArray");
		return ADDRESSING_SEAMLESS;
	case VK_IMAGE_VIEW_TYPE_1D:
	case VK_IMAGE_VIEW_TYPE_2D:
//	case VK_IMAGE_VIEW_TYPE_3D:
//	case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
	case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
		break;
	default:
		UNIMPLEMENTED("imageViewType %d", imageViewType);
		return ADDRESSING_WRAP;
	}

	switch(addressMode)
	{
	case VK_SAMPLER_ADDRESS_MODE_REPEAT:               return ADDRESSING_WRAP;
	case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:      return ADDRESSING_MIRROR;
	case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:        return ADDRESSING_CLAMP;
	case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:      return ADDRESSING_BORDER;
	case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: return ADDRESSING_MIRRORONCE;
	default:
		UNIMPLEMENTED("addressMode %d", addressMode);
		return ADDRESSING_WRAP;
	}
}

} // namespace sw
