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

SpirvShader::ImageSampler *SpirvShader::getImageSampler(const vk::ImageView *imageView, const vk::Sampler *sampler)
{
	// TODO: Move somewhere sensible.
	static std::unordered_map<uintptr_t, ImageSampler*> cache;
	static std::mutex mutex;

	// TODO: Don't use pointers they can be deleted and reused, combine some two
	// unique ids.
	auto key = reinterpret_cast<uintptr_t>(imageView) ^ reinterpret_cast<uintptr_t>(sampler);

	std::unique_lock<std::mutex> lock(mutex);
	auto it = cache.find(key);
	if (it != cache.end()) { return it->second; }

	// TODO: Hold a separate mutex lock for the sampler being built.
	auto function = rr::Function<Void(Pointer<Byte> image, Pointer<SIMD::Float>, Pointer<SIMD::Float>)>();
	Pointer<Byte> image = function.Arg<0>();
	Pointer<SIMD::Float> in = function.Arg<1>();
	Pointer<SIMD::Float> out = function.Arg<2>();
	emitSamplerFunction(imageView, sampler, image, in, out);
	auto fptr = reinterpret_cast<ImageSampler*>((void *)function("sampler")->getEntry());
	cache.emplace(key, fptr);
	return fptr;
}

void SpirvShader::emitSamplerFunction(
        const vk::ImageView *imageView, const vk::Sampler *sampler,
        Pointer<Byte> image, Pointer<SIMD::Float> in, Pointer<Byte> out)
{
	SIMD::Float u = in[0];
	SIMD::Float v = in[1];

	Pointer<Byte> constants;  // FIXME(b/129523279)

	Sampler::State samplerState;
	samplerState.textureType = TEXTURE_2D;                  ASSERT(imageView->getType() == VK_IMAGE_VIEW_TYPE_2D);  // TODO(b/129523279)
	samplerState.textureFormat = imageView->getFormat();
	samplerState.textureFilter = convertFilterMode(sampler);

	samplerState.addressingModeU = convertAddressingMode(sampler->addressModeU);
	samplerState.addressingModeV = convertAddressingMode(sampler->addressModeV);
	samplerState.addressingModeW = convertAddressingMode(sampler->addressModeW);
	samplerState.mipmapFilter = convertMipmapMode(sampler);
	samplerState.sRGB = false;                              ASSERT(imageView->getFormat().isSRGBformat() == false);  // TODO(b/129523279)
	samplerState.swizzleR = SWIZZLE_RED;                    ASSERT(imageView->getComponentMapping().r == VK_COMPONENT_SWIZZLE_R);  // TODO(b/129523279)
	samplerState.swizzleG = SWIZZLE_GREEN;                  ASSERT(imageView->getComponentMapping().g == VK_COMPONENT_SWIZZLE_G);  // TODO(b/129523279)
	samplerState.swizzleB = SWIZZLE_BLUE;                   ASSERT(imageView->getComponentMapping().b == VK_COMPONENT_SWIZZLE_B);  // TODO(b/129523279)
	samplerState.swizzleA = SWIZZLE_ALPHA;                  ASSERT(imageView->getComponentMapping().a == VK_COMPONENT_SWIZZLE_A);  // TODO(b/129523279)
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
	SIMD::Float w(0);     // TODO(b/129523279)
	SIMD::Float q(0);     // TODO(b/129523279)
	SIMD::Float bias(0);  // TODO(b/129523279)
	Vector4f dsx;         // TODO(b/129523279)
	Vector4f dsy;         // TODO(b/129523279)
	Vector4f offset;      // TODO(b/129523279)
	SamplerFunction samplerFunction = { Implicit, None };  // ASSERT(insn.wordCount() == 5);  // TODO(b/129523279)

	Vector4f sample = s.sampleTexture(texture, u, v, w, q, bias, dsx, dsy, offset, samplerFunction);

	Pointer<SIMD::Float> rgba = out;
	rgba[0] = sample.x;
	rgba[1] = sample.y;
	rgba[2] = sample.z;
	rgba[3] = sample.w;
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

sw::AddressingMode SpirvShader::convertAddressingMode(VkSamplerAddressMode addressMode)
{
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