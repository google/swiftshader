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

#include "Blitter.hpp"

#include "Pipeline/ShaderCore.hpp"
#include "Reactor/Reactor.hpp"
#include "System/Memory.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkImage.hpp"

#include <utility>

namespace sw
{
	Blitter::Blitter()
	{
		blitCache = new RoutineCache<State>(1024);
	}

	Blitter::~Blitter()
	{
		delete blitCache;
	}

	void Blitter::clear(void *pixel, vk::Format format, vk::Image *dest, const VkImageSubresourceRange& subresourceRange, const VkRect2D* renderArea)
	{
		VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask);
		if(dest->getFormat(aspect) == VK_FORMAT_UNDEFINED)
		{
			return;
		}

		if(fastClear(pixel, format, dest, subresourceRange, renderArea))
		{
			return;
		}

		State state(format, dest->getFormat(aspect), 1, dest->getSampleCountFlagBits(), { 0xF });
		Routine *blitRoutine = getRoutine(state);
		if(!blitRoutine)
		{
			return;
		}

		void(*blitFunction)(const BlitData *data) = (void(*)(const BlitData*))blitRoutine->getEntry();

		VkImageSubresourceLayers subresLayers =
		{
			subresourceRange.aspectMask,
			subresourceRange.baseMipLevel,
			subresourceRange.baseArrayLayer,
			1
		};

		uint32_t lastMipLevel = dest->getLastMipLevel(subresourceRange);
		uint32_t lastLayer = dest->getLastLayerIndex(subresourceRange);

		VkRect2D area = { { 0, 0 }, { 0, 0 } };
		if(renderArea)
		{
			ASSERT(subresourceRange.levelCount == 1);
			area = *renderArea;
		}

		for(; subresLayers.mipLevel <= lastMipLevel; subresLayers.mipLevel++)
		{
			VkExtent3D extent = dest->getMipLevelExtent(subresLayers.mipLevel);
			if(!renderArea)
			{
				area.extent.width = extent.width;
				area.extent.height = extent.height;
			}

			BlitData data =
			{
				pixel, nullptr, // source, dest

				format.bytes(),                                       // sPitchB
				dest->rowPitchBytes(aspect, subresLayers.mipLevel),   // dPitchB
				0,                                                    // sSliceB (unused in clear operations)
				dest->slicePitchBytes(aspect, subresLayers.mipLevel), // dSliceB

				0.5f, 0.5f, 0.0f, 0.0f, // x0, y0, w, h

				area.offset.y, static_cast<int>(area.offset.y + area.extent.height), // y0d, y1d
				area.offset.x, static_cast<int>(area.offset.x + area.extent.width),  // x0d, x1d

				0, 0, // sWidth, sHeight
			};

			for(subresLayers.baseArrayLayer = subresourceRange.baseArrayLayer; subresLayers.baseArrayLayer <= lastLayer; subresLayers.baseArrayLayer++)
			{
				for(uint32_t depth = 0; depth < extent.depth; depth++)
				{
					data.dest = dest->getTexelPointer({ 0, 0, static_cast<int32_t>(depth) }, subresLayers);

					blitFunction(&data);
				}
			}
		}
	}

	bool Blitter::fastClear(void *pixel, vk::Format format, vk::Image *dest, const VkImageSubresourceRange& subresourceRange, const VkRect2D* renderArea)
	{
		if(format != VK_FORMAT_R32G32B32A32_SFLOAT)
		{
			return false;
		}

		float *color = (float*)pixel;
		float r = color[0];
		float g = color[1];
		float b = color[2];
		float a = color[3];

		uint32_t packed;

		VkImageAspectFlagBits aspect = static_cast<VkImageAspectFlagBits>(subresourceRange.aspectMask);
		switch(dest->getFormat(aspect))
		{
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
			packed = ((uint16_t)(31 * b + 0.5f) << 0) |
			         ((uint16_t)(63 * g + 0.5f) << 5) |
			         ((uint16_t)(31 * r + 0.5f) << 11);
			break;
		case VK_FORMAT_B5G6R5_UNORM_PACK16:
			packed = ((uint16_t)(31 * r + 0.5f) << 0) |
			         ((uint16_t)(63 * g + 0.5f) << 5) |
			         ((uint16_t)(31 * b + 0.5f) << 11);
			break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_R8G8B8A8_UNORM:
			packed = ((uint32_t)(255 * a + 0.5f) << 24) |
			         ((uint32_t)(255 * b + 0.5f) << 16) |
			         ((uint32_t)(255 * g + 0.5f) << 8) |
			         ((uint32_t)(255 * r + 0.5f) << 0);
			break;
		case VK_FORMAT_B8G8R8A8_UNORM:
			packed = ((uint32_t)(255 * a + 0.5f) << 24) |
			         ((uint32_t)(255 * r + 0.5f) << 16) |
			         ((uint32_t)(255 * g + 0.5f) << 8) |
			         ((uint32_t)(255 * b + 0.5f) << 0);
			break;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
			packed = R11G11B10F(color);
			break;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
			packed = RGB9E5(color);
			break;
		default:
			return false;
		}

		VkImageSubresourceLayers subresLayers =
		{
			subresourceRange.aspectMask,
			subresourceRange.baseMipLevel,
			subresourceRange.baseArrayLayer,
			1
		};
		uint32_t lastMipLevel = dest->getLastMipLevel(subresourceRange);
		uint32_t lastLayer = dest->getLastLayerIndex(subresourceRange);

		VkRect2D area = { { 0, 0 }, { 0, 0 } };
		if(renderArea)
		{
			ASSERT(subresourceRange.levelCount == 1);
			area = *renderArea;
		}

		for(; subresLayers.mipLevel <= lastMipLevel; subresLayers.mipLevel++)
		{
			int rowPitchBytes = dest->rowPitchBytes(aspect, subresLayers.mipLevel);
			int slicePitchBytes = dest->slicePitchBytes(aspect, subresLayers.mipLevel);
			VkExtent3D extent = dest->getMipLevelExtent(subresLayers.mipLevel);
			if(!renderArea)
			{
				area.extent.width = extent.width;
				area.extent.height = extent.height;
			}

			for(subresLayers.baseArrayLayer = subresourceRange.baseArrayLayer; subresLayers.baseArrayLayer <= lastLayer; subresLayers.baseArrayLayer++)
			{
				for(uint32_t depth = 0; depth < extent.depth; depth++)
				{
					uint8_t *slice = (uint8_t*)dest->getTexelPointer(
						{ area.offset.x, area.offset.y, static_cast<int32_t>(depth) }, subresLayers);

					for(int j = 0; j < dest->getSampleCountFlagBits(); j++)
					{
						uint8_t *d = slice;

						switch(dest->getFormat(aspect).bytes())
						{
						case 2:
							for(uint32_t i = 0; i < area.extent.height; i++)
							{
								sw::clear((uint16_t*)d, packed, area.extent.width);
								d += rowPitchBytes;
							}
							break;
						case 4:
							for(uint32_t i = 0; i < area.extent.height; i++)
							{
								sw::clear((uint32_t*)d, packed, area.extent.width);
								d += rowPitchBytes;
							}
							break;
						default:
							assert(false);
						}

						slice += slicePitchBytes;
					}
				}
			}
		}

		return true;
	}

	bool Blitter::read(Float4 &c, Pointer<Byte> element, const State &state)
	{
		c = Float4(0.0f, 0.0f, 0.0f, 1.0f);

		switch(state.sourceFormat)
		{
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
			c.w = Float(Int(*Pointer<Byte>(element)) & Int(0xF));
			c.x = Float((Int(*Pointer<Byte>(element)) >> 4) & Int(0xF));
			c.y = Float(Int(*Pointer<Byte>(element + 1)) & Int(0xF));
			c.z = Float((Int(*Pointer<Byte>(element + 1)) >> 4) & Int(0xF));
			break;
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SNORM:
			c.x = Float(Int(*Pointer<SByte>(element)));
			c.w = float(0x7F);
			break;
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_UINT:
			c.x = Float(Int(*Pointer<Byte>(element)));
			c.w = float(0xFF);
			break;
		case VK_FORMAT_R16_SINT:
			c.x = Float(Int(*Pointer<Short>(element)));
			c.w = float(0x7FFF);
			break;
		case VK_FORMAT_R16_UINT:
			c.x = Float(Int(*Pointer<UShort>(element)));
			c.w = float(0xFFFF);
			break;
		case VK_FORMAT_R32_SINT:
			c.x = Float(*Pointer<Int>(element));
			c.w = float(0x7FFFFFFF);
			break;
		case VK_FORMAT_R32_UINT:
			c.x = Float(*Pointer<UInt>(element));
			c.w = float(0xFFFFFFFF);
			break;
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
			c = Float4(*Pointer<Byte4>(element)).zyxw;
			break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_R8G8B8A8_SNORM:
			c = Float4(*Pointer<SByte4>(element));
			break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VK_FORMAT_R8G8B8A8_SRGB:
			c = Float4(*Pointer<Byte4>(element));
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
			c = Float4(*Pointer<Short4>(element));
			break;
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_UINT:
			c = Float4(*Pointer<UShort4>(element));
			break;
		case VK_FORMAT_R32G32B32A32_SINT:
			c = Float4(*Pointer<Int4>(element));
			break;
		case VK_FORMAT_R32G32B32A32_UINT:
			c = Float4(*Pointer<UInt4>(element));
			break;
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_SNORM:
			c.x = Float(Int(*Pointer<SByte>(element + 0)));
			c.y = Float(Int(*Pointer<SByte>(element + 1)));
			c.w = float(0x7F);
			break;
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_UINT:
			c.x = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			c.w = float(0xFF);
			break;
		case VK_FORMAT_R16G16_SINT:
			c.x = Float(Int(*Pointer<Short>(element + 0)));
			c.y = Float(Int(*Pointer<Short>(element + 2)));
			c.w = float(0x7FFF);
			break;
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_UINT:
			c.x = Float(Int(*Pointer<UShort>(element + 0)));
			c.y = Float(Int(*Pointer<UShort>(element + 2)));
			c.w = float(0xFFFF);
			break;
		case VK_FORMAT_R32G32_SINT:
			c.x = Float(*Pointer<Int>(element + 0));
			c.y = Float(*Pointer<Int>(element + 4));
			c.w = float(0x7FFFFFFF);
			break;
		case VK_FORMAT_R32G32_UINT:
			c.x = Float(*Pointer<UInt>(element + 0));
			c.y = Float(*Pointer<UInt>(element + 4));
			c.w = float(0xFFFFFFFF);
			break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			c = *Pointer<Float4>(element);
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			c.x = *Pointer<Float>(element + 0);
			c.y = *Pointer<Float>(element + 4);
			break;
		case VK_FORMAT_R32_SFLOAT:
			c.x = *Pointer<Float>(element);
			break;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			c.w = Float(*Pointer<Half>(element + 6));
		case VK_FORMAT_R16G16B16_SFLOAT:
			c.z = Float(*Pointer<Half>(element + 4));
		case VK_FORMAT_R16G16_SFLOAT:
			c.y = Float(*Pointer<Half>(element + 2));
		case VK_FORMAT_R16_SFLOAT:
			c.x = Float(*Pointer<Half>(element));
			break;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
			// 10 (or 11) bit float formats are unsigned formats with a 5 bit exponent and a 5 (or 6) bit mantissa.
			// Since the Half float format also has a 5 bit exponent, we can convert these formats to half by
			// copy/pasting the bits so the the exponent bits and top mantissa bits are aligned to the half format.
			// In this case, we have:
			//              B B B B B B B B B B G G G G G G G G G G G R R R R R R R R R R R
			// 1st Short:                                  |xxxxxxxxxx---------------------|
			// 2nd Short:                  |xxxx---------------------xxxxxx|
			// 3rd Short: |--------------------xxxxxxxxxxxx|
			// These memory reads overlap, but each of them contains an entire channel, so we can read this without
			// any int -> short conversion.
			c.x = Float(As<Half>((*Pointer<UShort>(element + 0) & UShort(0x07FF)) << UShort(4)));
			c.y = Float(As<Half>((*Pointer<UShort>(element + 1) & UShort(0x3FF8)) << UShort(1)));
			c.z = Float(As<Half>((*Pointer<UShort>(element + 2) & UShort(0xFFC0)) >> UShort(1)));
			break;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
			// This type contains a common 5 bit exponent (E) and a 9 bit the mantissa for R, G and B.
			c.x = Float(*Pointer<UInt>(element) & UInt(0x000001FF));         // R's mantissa (bits 0-8)
			c.y = Float((*Pointer<UInt>(element) & UInt(0x0003FE00)) >> 9);  // G's mantissa (bits 9-17)
			c.z = Float((*Pointer<UInt>(element) & UInt(0x07FC0000)) >> 18); // B's mantissa (bits 18-26)
			c *= Float4(
				// 2^E, using the exponent (bits 27-31) and treating it as an unsigned integer value
				Float(UInt(1) << ((*Pointer<UInt>(element) & UInt(0xF8000000)) >> 27)) *
				// Since the 9 bit mantissa values currently stored in RGB were converted straight
				// from int to float (in the [0, 1<<9] range instead of the [0, 1] range), they
				// are (1 << 9) times too high.
				// Also, the exponent has 5 bits and we compute the exponent bias of floating point
				// formats using "2^(k-1) - 1", so, in this case, the exponent bias is 2^(5-1)-1 = 15
				// Exponent bias (15) + number of mantissa bits per component (9) = 24
				Float(1.0f / (1 << 24)));
			c.w = 1.0f;
			break;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
			c.x = Float(Int((*Pointer<UShort>(element) & UShort(0xF800)) >> UShort(11)));
			c.y = Float(Int((*Pointer<UShort>(element) & UShort(0x07E0)) >> UShort(5)));
			c.z = Float(Int(*Pointer<UShort>(element) & UShort(0x001F)));
			break;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
			c.w = Float(Int((*Pointer<UShort>(element) & UShort(0x8000)) >> UShort(15)));
			c.x = Float(Int((*Pointer<UShort>(element) & UShort(0x7C00)) >> UShort(10)));
			c.y = Float(Int((*Pointer<UShort>(element) & UShort(0x03E0)) >> UShort(5)));
			c.z = Float(Int(*Pointer<UShort>(element) & UShort(0x001F)));
			break;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
			c.x = Float(Int((*Pointer<UInt>(element) & UInt(0x000003FF))));
			c.y = Float(Int((*Pointer<UInt>(element) & UInt(0x000FFC00)) >> 10));
			c.z = Float(Int((*Pointer<UInt>(element) & UInt(0x3FF00000)) >> 20));
			c.w = Float(Int((*Pointer<UInt>(element) & UInt(0xC0000000)) >> 30));
			break;
		case VK_FORMAT_D16_UNORM:
			c.x = Float(Int((*Pointer<UShort>(element))));
			break;
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			c.x = Float(Int((*Pointer<UInt>(element) & UInt(0xFFFFFF00)) >> 8));
			break;
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			c.x = *Pointer<Float>(element);
			break;
		case VK_FORMAT_S8_UINT:
			c.x = Float(Int(*Pointer<Byte>(element)));
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Float4 &c, Pointer<Byte> element, const State &state)
	{
		bool writeR = state.writeRed;
		bool writeG = state.writeGreen;
		bool writeB = state.writeBlue;
		bool writeA = state.writeAlpha;
		bool writeRGBA = writeR && writeG && writeB && writeA;

		switch(state.destFormat)
		{
		case VK_FORMAT_R4G4_UNORM_PACK8:
			if(writeR | writeG)
			{
				if(!writeR)
				{
					*Pointer<Byte>(element) = (Byte(RoundInt(Float(c.y))) & Byte(0xF)) |
				                              (*Pointer<Byte>(element) & Byte(0xF0));
				}
				else if(!writeG)
				{
					*Pointer<Byte>(element) = (*Pointer<Byte>(element) & Byte(0xF)) |
				                              (Byte(RoundInt(Float(c.x))) << Byte(4));
				}
				else
				{
					*Pointer<Byte>(element) = (Byte(RoundInt(Float(c.y))) & Byte(0xF)) |
				                              (Byte(RoundInt(Float(c.x))) << Byte(4));
				}
			}
			break;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
			if(writeR || writeG || writeB || writeA)
			{
				*Pointer<UShort>(element) = (writeR ? ((UShort(RoundInt(Float(c.x))) & UShort(0xF)) << UShort(12)) :
				                                      (*Pointer<UShort>(element) & UShort(0x000F))) |
				                            (writeG ? ((UShort(RoundInt(Float(c.y))) & UShort(0xF)) << UShort(8)) :
				                                      (*Pointer<UShort>(element) & UShort(0x00F0))) |
				                            (writeB ? ((UShort(RoundInt(Float(c.z))) & UShort(0xF)) << UShort(4)) :
			                                          (*Pointer<UShort>(element) & UShort(0x0F00))) |
			                                (writeA ? (UShort(RoundInt(Float(c.w))) & UShort(0xF)) :
			                                          (*Pointer<UShort>(element) & UShort(0xF000)));
			}
			break;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
			if(writeRGBA)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.w)) & Int(0xF)) |
				                            UShort((RoundInt(Float(c.x)) & Int(0xF)) << 4) |
				                            UShort((RoundInt(Float(c.y)) & Int(0xF)) << 8) |
				                            UShort((RoundInt(Float(c.z)) & Int(0xF)) << 12);
			}
			else
			{
				unsigned short mask = (writeA ? 0x000F : 0x0000) |
				                      (writeR ? 0x00F0 : 0x0000) |
				                      (writeG ? 0x0F00 : 0x0000) |
				                      (writeB ? 0xF000 : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            ((UShort(RoundInt(Float(c.w)) & Int(0xF)) |
				                              UShort((RoundInt(Float(c.x)) & Int(0xF)) << 4) |
				                              UShort((RoundInt(Float(c.y)) & Int(0xF)) << 8) |
				                              UShort((RoundInt(Float(c.z)) & Int(0xF)) << 12)) & UShort(mask));
			}
			break;
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
			if(writeRGBA)
			{
				Short4 c0 = RoundShort4(c.zyxw);
				*Pointer<Byte4>(element) = Byte4(PackUnsigned(c0, c0));
			}
			else
			{
				if(writeB) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.z))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeR) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.x))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(RoundInt(Float(c.w))); }
			}
			break;
		case VK_FORMAT_B8G8R8_SNORM:
			if(writeB) { *Pointer<SByte>(element + 0) = SByte(RoundInt(Float(c.z))); }
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(RoundInt(Float(c.y))); }
			if(writeR) { *Pointer<SByte>(element + 2) = SByte(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_B8G8R8_UNORM:
		case VK_FORMAT_B8G8R8_SRGB:
			if(writeB) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.z))); }
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
			if(writeR) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_USCALED:
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
			if(writeRGBA)
			{
				Short4 c0 = RoundShort4(c);
				*Pointer<Byte4>(element) = Byte4(PackUnsigned(c0, c0));
			}
			else
			{
				if(writeR) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(RoundInt(Float(c.w))); }
			}
			break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			if(writeRGBA)
			{
				*Pointer<Float4>(element) = c;
			}
			else
			{
				if(writeR) { *Pointer<Float>(element) = c.x; }
				if(writeG) { *Pointer<Float>(element + 4) = c.y; }
				if(writeB) { *Pointer<Float>(element + 8) = c.z; }
				if(writeA) { *Pointer<Float>(element + 12) = c.w; }
			}
			break;
		case VK_FORMAT_R32G32B32_SFLOAT:
			if(writeR) { *Pointer<Float>(element) = c.x; }
			if(writeG) { *Pointer<Float>(element + 4) = c.y; }
			if(writeB) { *Pointer<Float>(element + 8) = c.z; }
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			if(writeR && writeG)
			{
				*Pointer<Float2>(element) = Float2(c);
			}
			else
			{
				if(writeR) { *Pointer<Float>(element) = c.x; }
				if(writeG) { *Pointer<Float>(element + 4) = c.y; }
			}
			break;
		case VK_FORMAT_R32_SFLOAT:
			if(writeR) { *Pointer<Float>(element) = c.x; }
			break;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			if(writeA) { *Pointer<Half>(element + 6) = Half(c.w); }
		case VK_FORMAT_R16G16B16_SFLOAT:
			if(writeB) { *Pointer<Half>(element + 4) = Half(c.z); }
		case VK_FORMAT_R16G16_SFLOAT:
			if(writeG) { *Pointer<Half>(element + 2) = Half(c.y); }
		case VK_FORMAT_R16_SFLOAT:
			if(writeR) { *Pointer<Half>(element) = Half(c.x); }
			break;
		case VK_FORMAT_B8G8R8A8_SNORM:
			if(writeB) { *Pointer<SByte>(element) = SByte(RoundInt(Float(c.z))); }
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(RoundInt(Float(c.y))); }
			if(writeR) { *Pointer<SByte>(element + 2) = SByte(RoundInt(Float(c.x))); }
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(RoundInt(Float(c.w))); }
			break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_SSCALED:
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(RoundInt(Float(c.w))); }
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R8G8B8_SNORM:
		case VK_FORMAT_R8G8B8_SSCALED:
		case VK_FORMAT_R8G8B8_SRGB:
			if(writeB) { *Pointer<SByte>(element + 2) = SByte(RoundInt(Float(c.z))); }
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_SSCALED:
		case VK_FORMAT_R8G8_SRGB:
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(RoundInt(Float(c.y))); }
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_SSCALED:
		case VK_FORMAT_R8_SRGB:
			if(writeR) { *Pointer<SByte>(element) = SByte(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8_USCALED:
			if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_USCALED:
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_USCALED:
			if(writeR) { *Pointer<Byte>(element) = Byte(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16B16A16_SSCALED:
			if(writeRGBA)
			{
				*Pointer<Short4>(element) = Short4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Short>(element + 4) = Short(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<Short>(element + 6) = Short(RoundInt(Float(c.w))); }
			}
			break;
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_SNORM:
		case VK_FORMAT_R16G16B16_SSCALED:
			if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
			if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
			if(writeB) { *Pointer<Short>(element + 4) = Short(RoundInt(Float(c.z))); }
			break;
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R16G16_SSCALED:
			if(writeR && writeG)
			{
				*Pointer<Short2>(element) = Short2(Short4(RoundInt(c)));
			}
			else
			{
				if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
			}
			break;
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R16_SSCALED:
			if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_USCALED:
			if(writeRGBA)
			{
				*Pointer<UShort4>(element) = UShort4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UShort>(element + 4) = UShort(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<UShort>(element + 6) = UShort(RoundInt(Float(c.w))); }
			}
			break;
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16_USCALED:
			if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
			if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
			if(writeB) { *Pointer<UShort>(element + 4) = UShort(RoundInt(Float(c.z))); }
			break;
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_USCALED:
			if(writeR && writeG)
			{
				*Pointer<UShort2>(element) = UShort2(UShort4(RoundInt(c)));
			}
			else
			{
				if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
			}
			break;
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_USCALED:
			if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_R32G32B32A32_SINT:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = RoundInt(c);
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = RoundInt(Float(c.x)); }
				if(writeG) { *Pointer<Int>(element + 4) = RoundInt(Float(c.y)); }
				if(writeB) { *Pointer<Int>(element + 8) = RoundInt(Float(c.z)); }
				if(writeA) { *Pointer<Int>(element + 12) = RoundInt(Float(c.w)); }
			}
			break;
		case VK_FORMAT_R32G32B32_SINT:
			if(writeB) { *Pointer<Int>(element + 8) = RoundInt(Float(c.z)); }
		case VK_FORMAT_R32G32_SINT:
			if(writeG) { *Pointer<Int>(element + 4) = RoundInt(Float(c.y)); }
		case VK_FORMAT_R32_SINT:
			if(writeR) { *Pointer<Int>(element) = RoundInt(Float(c.x)); }
			break;
		case VK_FORMAT_R32G32B32A32_UINT:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = UInt4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<UInt>(element + 12) = As<UInt>(RoundInt(Float(c.w))); }
			}
			break;
		case VK_FORMAT_R32G32B32_UINT:
			if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(RoundInt(Float(c.z))); }
		case VK_FORMAT_R32G32_UINT:
			if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(RoundInt(Float(c.y))); }
		case VK_FORMAT_R32_UINT:
			if(writeR) { *Pointer<UInt>(element) = As<UInt>(RoundInt(Float(c.x))); }
			break;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
			if(writeR && writeG && writeB)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.z)) |
				                                  (RoundInt(Float(c.y)) << Int(5)) |
				                                  (RoundInt(Float(c.x)) << Int(11)));
			}
			else
			{
				unsigned short mask = (writeB ? 0x001F : 0x0000) | (writeG ? 0x07E0 : 0x0000) | (writeR ? 0xF800 : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            (UShort(RoundInt(Float(c.z)) |
				                                   (RoundInt(Float(c.y)) << Int(5)) |
				                                   (RoundInt(Float(c.x)) << Int(11))) & UShort(mask));
			}
			break;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
			if(writeRGBA)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.w)) |
				                                  (RoundInt(Float(c.z)) << Int(1)) |
				                                  (RoundInt(Float(c.y)) << Int(6)) |
				                                  (RoundInt(Float(c.x)) << Int(11)));
			}
			else
			{
				unsigned short mask = (writeA ? 0x8000 : 0x0000) |
				                      (writeR ? 0x7C00 : 0x0000) |
				                      (writeG ? 0x03E0 : 0x0000) |
				                      (writeB ? 0x001F : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            (UShort(RoundInt(Float(c.w)) |
				                                   (RoundInt(Float(c.z)) << Int(1)) |
				                                   (RoundInt(Float(c.y)) << Int(6)) |
				                                   (RoundInt(Float(c.x)) << Int(11))) & UShort(mask));
			}
			break;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
			if(writeRGBA)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.w)) |
				                                  (RoundInt(Float(c.x)) << Int(1)) |
				                                  (RoundInt(Float(c.y)) << Int(6)) |
				                                  (RoundInt(Float(c.z)) << Int(11)));
			}
			else
			{
				unsigned short mask = (writeA ? 0x8000 : 0x0000) |
				                      (writeR ? 0x7C00 : 0x0000) |
				                      (writeG ? 0x03E0 : 0x0000) |
				                      (writeB ? 0x001F : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            (UShort(RoundInt(Float(c.w)) |
				                                   (RoundInt(Float(c.x)) << Int(1)) |
				                                   (RoundInt(Float(c.y)) << Int(6)) |
				                                   (RoundInt(Float(c.z)) << Int(11))) & UShort(mask));
			}
			break;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
			if(writeRGBA)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.z)) |
				                                  (RoundInt(Float(c.y)) << Int(5)) |
				                                  (RoundInt(Float(c.x)) << Int(10)) |
				                                  (RoundInt(Float(c.w)) << Int(15)));
			}
			else
			{
				unsigned short mask = (writeA ? 0x8000 : 0x0000) |
				                      (writeR ? 0x7C00 : 0x0000) |
				                      (writeG ? 0x03E0 : 0x0000) |
				                      (writeB ? 0x001F : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            (UShort(RoundInt(Float(c.z)) |
				                                   (RoundInt(Float(c.y)) << Int(5)) |
				                                   (RoundInt(Float(c.x)) << Int(10)) |
				                                   (RoundInt(Float(c.w)) << Int(15))) & UShort(mask));
			}
			break;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
			if(writeRGBA)
			{
				*Pointer<UInt>(element) = UInt(RoundInt(Float(c.x)) |
				                              (RoundInt(Float(c.y)) << 10) |
				                              (RoundInt(Float(c.z)) << 20) |
				                              (RoundInt(Float(c.w)) << 30));
			}
			else
			{
				unsigned int mask = (writeA ? 0xC0000000 : 0x0000) |
				                    (writeB ? 0x3FF00000 : 0x0000) |
				                    (writeG ? 0x000FFC00 : 0x0000) |
				                    (writeR ? 0x000003FF : 0x0000);
				unsigned int unmask = ~mask;
				*Pointer<UInt>(element) = (*Pointer<UInt>(element) & UInt(unmask)) |
				                            (UInt(RoundInt(Float(c.x)) |
				                                 (RoundInt(Float(c.y)) << 10) |
				                                 (RoundInt(Float(c.z)) << 20) |
				                                 (RoundInt(Float(c.w)) << 30)) & UInt(mask));
			}
			break;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
			if(writeRGBA)
			{
				*Pointer<UInt>(element) = UInt(RoundInt(Float(c.z)) |
				                              (RoundInt(Float(c.y)) << 10) |
				                              (RoundInt(Float(c.x)) << 20) |
				                              (RoundInt(Float(c.w)) << 30));
			}
			else
			{
				unsigned int mask = (writeA ? 0xC0000000 : 0x0000) |
				                    (writeR ? 0x3FF00000 : 0x0000) |
				                    (writeG ? 0x000FFC00 : 0x0000) |
				                    (writeB ? 0x000003FF : 0x0000);
				unsigned int unmask = ~mask;
				*Pointer<UInt>(element) = (*Pointer<UInt>(element) & UInt(unmask)) |
				                            (UInt(RoundInt(Float(c.z)) |
				                                 (RoundInt(Float(c.y)) << 10) |
				                                 (RoundInt(Float(c.x)) << 20) |
				                                 (RoundInt(Float(c.w)) << 30)) & UInt(mask));
			}
			break;
		case VK_FORMAT_D16_UNORM:
			*Pointer<UShort>(element) = UShort(RoundInt(Float(c.x)));
			break;
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			*Pointer<UInt>(element) = UInt(RoundInt(Float(c.x)) << 8);
			break;
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			*Pointer<Float>(element) = c.x;
			break;
		case VK_FORMAT_S8_UINT:
			*Pointer<Byte>(element) = Byte(RoundInt(Float(c.x)));
			break;
		default:
			return false;
		}
		return true;
	}

	bool Blitter::read(Int4 &c, Pointer<Byte> element, const State &state)
	{
		c = Int4(0, 0, 0, 1);

		switch(state.sourceFormat)
		{
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R8G8B8A8_SINT:
			c = Insert(c, Int(*Pointer<SByte>(element + 3)), 3);
			c = Insert(c, Int(*Pointer<SByte>(element + 2)), 2);
		case VK_FORMAT_R8G8_SINT:
			c = Insert(c, Int(*Pointer<SByte>(element + 1)), 1);
		case VK_FORMAT_R8_SINT:
			c = Insert(c, Int(*Pointer<SByte>(element)), 0);
			break;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
			c = Insert(c, Int((*Pointer<UInt>(element) & UInt(0x000003FF))), 0);
			c = Insert(c, Int((*Pointer<UInt>(element) & UInt(0x000FFC00)) >> 10), 1);
			c = Insert(c, Int((*Pointer<UInt>(element) & UInt(0x3FF00000)) >> 20), 2);
			c = Insert(c, Int((*Pointer<UInt>(element) & UInt(0xC0000000)) >> 30), 3);
			break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_R8G8B8A8_UINT:
			c = Insert(c, Int(*Pointer<Byte>(element + 3)), 3);
			c = Insert(c, Int(*Pointer<Byte>(element + 2)), 2);
		case VK_FORMAT_R8G8_UINT:
			c = Insert(c, Int(*Pointer<Byte>(element + 1)), 1);
		case VK_FORMAT_R8_UINT:
			c = Insert(c, Int(*Pointer<Byte>(element)), 0);
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
			c = Insert(c, Int(*Pointer<Short>(element + 6)), 3);
			c = Insert(c, Int(*Pointer<Short>(element + 4)), 2);
		case VK_FORMAT_R16G16_SINT:
			c = Insert(c, Int(*Pointer<Short>(element + 2)), 1);
		case VK_FORMAT_R16_SINT:
			c = Insert(c, Int(*Pointer<Short>(element)), 0);
			break;
		case VK_FORMAT_R16G16B16A16_UINT:
			c = Insert(c, Int(*Pointer<UShort>(element + 6)), 3);
			c = Insert(c, Int(*Pointer<UShort>(element + 4)), 2);
		case VK_FORMAT_R16G16_UINT:
			c = Insert(c, Int(*Pointer<UShort>(element + 2)), 1);
		case VK_FORMAT_R16_UINT:
			c = Insert(c, Int(*Pointer<UShort>(element)), 0);
			break;
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
			c = *Pointer<Int4>(element);
			break;
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_UINT:
			c = Insert(c, *Pointer<Int>(element + 4), 1);
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_UINT:
			c = Insert(c, *Pointer<Int>(element), 0);
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Int4 &c, Pointer<Byte> element, const State &state)
	{
		bool writeR = state.writeRed;
		bool writeG = state.writeGreen;
		bool writeB = state.writeBlue;
		bool writeA = state.writeAlpha;
		bool writeRGBA = writeR && writeG && writeB && writeA;

		switch(state.destFormat)
		{
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
			c = Min(As<UInt4>(c), UInt4(0x03FF, 0x03FF, 0x03FF, 0x0003));
			break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8G8B8A8_USCALED:
		case VK_FORMAT_R8G8B8_USCALED:
		case VK_FORMAT_R8G8_USCALED:
		case VK_FORMAT_R8_USCALED:
			c = Min(As<UInt4>(c), UInt4(0xFF));
			break;
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16G16B16A16_USCALED:
		case VK_FORMAT_R16G16B16_USCALED:
		case VK_FORMAT_R16G16_USCALED:
		case VK_FORMAT_R16_USCALED:
			c = Min(As<UInt4>(c), UInt4(0xFFFF));
			break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8G8B8A8_SSCALED:
		case VK_FORMAT_R8G8B8_SSCALED:
		case VK_FORMAT_R8G8_SSCALED:
		case VK_FORMAT_R8_SSCALED:
			c = Min(Max(c, Int4(-0x80)), Int4(0x7F));
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16G16B16A16_SSCALED:
		case VK_FORMAT_R16G16B16_SSCALED:
		case VK_FORMAT_R16G16_SSCALED:
		case VK_FORMAT_R16_SSCALED:
			c = Min(Max(c, Int4(-0x8000)), Int4(0x7FFF));
			break;
		default:
			break;
		}

		switch(state.destFormat)
		{
		case VK_FORMAT_B8G8R8A8_SINT:
		case VK_FORMAT_B8G8R8A8_SSCALED:
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(Extract(c, 3)); }
		case VK_FORMAT_B8G8R8_SINT:
		case VK_FORMAT_B8G8R8_SRGB:
		case VK_FORMAT_B8G8R8_SSCALED:
			if(writeB) { *Pointer<SByte>(element) = SByte(Extract(c, 2)); }
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(Extract(c, 1)); }
			if(writeR) { *Pointer<SByte>(element + 2) = SByte(Extract(c, 0)); }
			break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8B8A8_SSCALED:
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(Extract(c, 3)); }
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R8G8B8_SSCALED:
			if(writeB) { *Pointer<SByte>(element + 2) = SByte(Extract(c, 2)); }
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_SSCALED:
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(Extract(c, 1)); }
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SSCALED:
			if(writeR) { *Pointer<SByte>(element) = SByte(Extract(c, 0)); }
			break;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
			if(writeRGBA)
			{
				*Pointer<UInt>(element) =
					UInt((Extract(c, 0)) | (Extract(c, 1) << 10) | (Extract(c, 2) << 20) | (Extract(c, 3) << 30));
			}
			else
			{
				unsigned int mask = (writeA ? 0xC0000000 : 0x0000) |
				                    (writeB ? 0x3FF00000 : 0x0000) |
				                    (writeG ? 0x000FFC00 : 0x0000) |
				                    (writeR ? 0x000003FF : 0x0000);
				unsigned int unmask = ~mask;
				*Pointer<UInt>(element) = (*Pointer<UInt>(element) & UInt(unmask)) |
					(UInt(Extract(c, 0) | (Extract(c, 1) << 10) | (Extract(c, 2) << 20) | (Extract(c, 3) << 30)) & UInt(mask));
			}
			break;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
			if(writeRGBA)
			{
				*Pointer<UInt>(element) =
					UInt((Extract(c, 2)) | (Extract(c, 1) << 10) | (Extract(c, 0) << 20) | (Extract(c, 3) << 30));
			}
			else
			{
				unsigned int mask = (writeA ? 0xC0000000 : 0x0000) |
				                    (writeR ? 0x3FF00000 : 0x0000) |
				                    (writeG ? 0x000FFC00 : 0x0000) |
				                    (writeB ? 0x000003FF : 0x0000);
				unsigned int unmask = ~mask;
				*Pointer<UInt>(element) = (*Pointer<UInt>(element) & UInt(unmask)) |
					(UInt(Extract(c, 2) | (Extract(c, 1) << 10) | (Extract(c, 0) << 20) | (Extract(c, 3) << 30)) & UInt(mask));
			}
			break;
		case VK_FORMAT_B8G8R8A8_UINT:
		case VK_FORMAT_B8G8R8A8_USCALED:
			if(writeA) { *Pointer<Byte>(element + 3) = Byte(Extract(c, 3)); }
		case VK_FORMAT_B8G8R8_UINT:
		case VK_FORMAT_B8G8R8_USCALED:
			if(writeB) { *Pointer<Byte>(element) = Byte(Extract(c, 2)); }
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(Extract(c, 1)); }
			if(writeR) { *Pointer<Byte>(element + 2) = Byte(Extract(c, 0)); }
			break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_USCALED:
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
			if(writeA) { *Pointer<Byte>(element + 3) = Byte(Extract(c, 3)); }
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8B8_USCALED:
			if(writeB) { *Pointer<Byte>(element + 2) = Byte(Extract(c, 2)); }
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_USCALED:
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(Extract(c, 1)); }
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_USCALED:
			if(writeR) { *Pointer<Byte>(element) = Byte(Extract(c, 0)); }
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SSCALED:
			if(writeA) { *Pointer<Short>(element + 6) = Short(Extract(c, 3)); }
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_SSCALED:
			if(writeB) { *Pointer<Short>(element + 4) = Short(Extract(c, 2)); }
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SSCALED:
			if(writeG) { *Pointer<Short>(element + 2) = Short(Extract(c, 1)); }
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SSCALED:
			if(writeR) { *Pointer<Short>(element) = Short(Extract(c, 0)); }
			break;
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_USCALED:
			if(writeA) { *Pointer<UShort>(element + 6) = UShort(Extract(c, 3)); }
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16B16_USCALED:
			if(writeB) { *Pointer<UShort>(element + 4) = UShort(Extract(c, 2)); }
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_USCALED:
			if(writeG) { *Pointer<UShort>(element + 2) = UShort(Extract(c, 1)); }
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_USCALED:
			if(writeR) { *Pointer<UShort>(element) = UShort(Extract(c, 0)); }
			break;
		case VK_FORMAT_R32G32B32A32_SINT:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = c;
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
				if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
				if(writeB) { *Pointer<Int>(element + 8) = Extract(c, 2); }
				if(writeA) { *Pointer<Int>(element + 12) = Extract(c, 3); }
			}
			break;
		case VK_FORMAT_R32G32B32_SINT:
			if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
			if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
			if(writeB) { *Pointer<Int>(element + 8) = Extract(c, 2); }
			break;
		case VK_FORMAT_R32G32_SINT:
			if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
			if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
			break;
		case VK_FORMAT_R32_SINT:
			if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
			break;
		case VK_FORMAT_R32G32B32A32_UINT:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = As<UInt4>(c);
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(Extract(c, 1)); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(Extract(c, 2)); }
				if(writeA) { *Pointer<UInt>(element + 12) = As<UInt>(Extract(c, 3)); }
			}
			break;
		case VK_FORMAT_R32G32B32_UINT:
			if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(Extract(c, 2)); }
		case VK_FORMAT_R32G32_UINT:
			if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(Extract(c, 1)); }
		case VK_FORMAT_R32_UINT:
			if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::ApplyScaleAndClamp(Float4 &value, const State &state, bool preScaled)
	{
		float4 scale, unscale;
		if(state.clearOperation &&
		   state.sourceFormat.isNonNormalizedInteger() &&
		   !state.destFormat.isNonNormalizedInteger())
		{
			// If we're clearing a buffer from an int or uint color into a normalized color,
			// then the whole range of the int or uint color must be scaled between 0 and 1.
			switch(state.sourceFormat)
			{
			case VK_FORMAT_R32G32B32A32_SINT:
				unscale = replicate(static_cast<float>(0x7FFFFFFF));
				break;
			case VK_FORMAT_R32G32B32A32_UINT:
				unscale = replicate(static_cast<float>(0xFFFFFFFF));
				break;
			default:
				return false;
			}
		}
		else if(!state.sourceFormat.getScale(unscale))
		{
			return false;
		}

		if(!state.destFormat.getScale(scale))
		{
			return false;
		}

		bool srcSRGB = state.sourceFormat.isSRGBformat();
		bool dstSRGB = state.destFormat.isSRGBformat();

		if(state.convertSRGB && ((srcSRGB && !preScaled) || dstSRGB))   // One of the formats is sRGB encoded.
		{
			value *= preScaled ? Float4(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z, 1.0f / scale.w) : // Unapply scale
			                     Float4(1.0f / unscale.x, 1.0f / unscale.y, 1.0f / unscale.z, 1.0f / unscale.w); // Apply unscale
			value = (srcSRGB && !preScaled) ? sRGBtoLinear(value) : LinearToSRGB(value);
			value *= Float4(scale.x, scale.y, scale.z, scale.w); // Apply scale
		}
		else if(unscale != scale)
		{
			value *= Float4(scale.x / unscale.x, scale.y / unscale.y, scale.z / unscale.z, scale.w / unscale.w);
		}

		if(state.sourceFormat.isFloatFormat() && !state.destFormat.isFloatFormat())
		{
			value = Min(value, Float4(scale.x, scale.y, scale.z, scale.w));

			value = Max(value, Float4(state.destFormat.isUnsignedComponent(0) ? 0.0f : -scale.x,
			                          state.destFormat.isUnsignedComponent(1) ? 0.0f : -scale.y,
			                          state.destFormat.isUnsignedComponent(2) ? 0.0f : -scale.z,
			                          state.destFormat.isUnsignedComponent(3) ? 0.0f : -scale.w));
		}

		return true;
	}

	Int Blitter::ComputeOffset(Int &x, Int &y, Int &pitchB, int bytes, bool quadLayout)
	{
		if(!quadLayout)
		{
			return y * pitchB + x * bytes;
		}
		else
		{
			// (x & ~1) * 2 + (x & 1) == (x - (x & 1)) * 2 + (x & 1) == x * 2 - (x & 1) * 2 + (x & 1) == x * 2 - (x & 1)
			return (y & Int(~1)) * pitchB +
			       ((y & Int(1)) * 2 + x * 2 - (x & Int(1))) * bytes;
		}
	}

	Float4 Blitter::LinearToSRGB(Float4 &c)
	{
		Float4 lc = Min(c, Float4(0.0031308f)) * Float4(12.92f);
		Float4 ec = Float4(1.055f) * power(c, Float4(1.0f / 2.4f)) - Float4(0.055f);

		Float4 s = c;
		s.xyz = Max(lc, ec);

		return s;
	}

	Float4 Blitter::sRGBtoLinear(Float4 &c)
	{
		Float4 lc = c * Float4(1.0f / 12.92f);
		Float4 ec = power((c + Float4(0.055f)) * Float4(1.0f / 1.055f), Float4(2.4f));

		Int4 linear = CmpLT(c, Float4(0.04045f));

		Float4 s = c;
		s.xyz = As<Float4>((linear & As<Int4>(lc)) | (~linear & As<Int4>(ec)));   // FIXME: IfThenElse()

		return s;
	}

	Routine *Blitter::generate(const State &state)
	{
		Function<Void(Pointer<Byte>)> function;
		{
			Pointer<Byte> blit(function.Arg<0>());

			Pointer<Byte> source = *Pointer<Pointer<Byte>>(blit + OFFSET(BlitData,source));
			Pointer<Byte> dest = *Pointer<Pointer<Byte>>(blit + OFFSET(BlitData,dest));
			Int sPitchB = *Pointer<Int>(blit + OFFSET(BlitData,sPitchB));
			Int dPitchB = *Pointer<Int>(blit + OFFSET(BlitData,dPitchB));

			Float x0 = *Pointer<Float>(blit + OFFSET(BlitData,x0));
			Float y0 = *Pointer<Float>(blit + OFFSET(BlitData,y0));
			Float w = *Pointer<Float>(blit + OFFSET(BlitData,w));
			Float h = *Pointer<Float>(blit + OFFSET(BlitData,h));

			Int x0d = *Pointer<Int>(blit + OFFSET(BlitData,x0d));
			Int x1d = *Pointer<Int>(blit + OFFSET(BlitData,x1d));
			Int y0d = *Pointer<Int>(blit + OFFSET(BlitData,y0d));
			Int y1d = *Pointer<Int>(blit + OFFSET(BlitData,y1d));

			Int sWidth = *Pointer<Int>(blit + OFFSET(BlitData,sWidth));
			Int sHeight = *Pointer<Int>(blit + OFFSET(BlitData,sHeight));

			bool intSrc = state.sourceFormat.isNonNormalizedInteger();
			bool intDst = state.destFormat.isNonNormalizedInteger();
			bool intBoth = intSrc && intDst;
			bool srcQuadLayout = state.sourceFormat.hasQuadLayout();
			bool dstQuadLayout = state.destFormat.hasQuadLayout();
			int srcBytes = state.sourceFormat.bytes();
			int dstBytes = state.destFormat.bytes();

			bool hasConstantColorI = false;
			Int4 constantColorI;
			bool hasConstantColorF = false;
			Float4 constantColorF;
			if(state.clearOperation)
			{
				if(intBoth) // Integer types
				{
					if(!read(constantColorI, source, state))
					{
						return nullptr;
					}
					hasConstantColorI = true;
				}
				else
				{
					if(!read(constantColorF, source, state))
					{
						return nullptr;
					}
					hasConstantColorF = true;

					if(!ApplyScaleAndClamp(constantColorF, state))
					{
						return nullptr;
					}
				}
			}

			For(Int j = y0d, j < y1d, j++)
			{
				Float y = state.clearOperation ? RValue<Float>(y0) : y0 + Float(j) * h;
				Pointer<Byte> destLine = dest + (dstQuadLayout ? j & Int(~1) : RValue<Int>(j)) * dPitchB;

				For(Int i = x0d, i < x1d, i++)
				{
					Float x = state.clearOperation ? RValue<Float>(x0) : x0 + Float(i) * w;
					Pointer<Byte> d = destLine + (dstQuadLayout ? (((j & Int(1)) << 1) + (i * 2) - (i & Int(1))) : RValue<Int>(i)) * dstBytes;

					if(hasConstantColorI)
					{
						if(!write(constantColorI, d, state))
						{
							return nullptr;
						}
					}
					else if(hasConstantColorF)
					{
						for(int s = 0; s < state.destSamples; s++)
						{
							if(!write(constantColorF, d, state))
							{
								return nullptr;
							}

							d += *Pointer<Int>(blit + OFFSET(BlitData, dSliceB));
						}
					}
					else if(intBoth) // Integer types do not support filtering
					{
						Int4 color; // When both formats are true integer types, we don't go to float to avoid losing precision
						Int X = Int(x);
						Int Y = Int(y);

						if(state.clampToEdge)
						{
							X = Clamp(X, 0, sWidth - 1);
							Y = Clamp(Y, 0, sHeight - 1);
						}

						Pointer<Byte> s = source + ComputeOffset(X, Y, sPitchB, srcBytes, srcQuadLayout);

						if(!read(color, s, state))
						{
							return nullptr;
						}

						if(!write(color, d, state))
						{
							return nullptr;
						}
					}
					else
					{
						Float4 color;

						bool preScaled = false;
						if(!state.filter || intSrc)
						{
							Int X = Int(x);
							Int Y = Int(y);

							if(state.clampToEdge)
							{
								X = Clamp(X, 0, sWidth - 1);
								Y = Clamp(Y, 0, sHeight - 1);
							}

							Pointer<Byte> s = source + ComputeOffset(X, Y, sPitchB, srcBytes, srcQuadLayout);

							if(!read(color, s, state))
							{
								return nullptr;
							}

							if(state.srcSamples > 1) // Resolve multisampled source
							{
								Float4 accum = color;
								for(int i = 1; i < state.srcSamples; i++)
								{
									s += *Pointer<Int>(blit + OFFSET(BlitData, sSliceB));
									if(!read(color, s, state))
									{
										return nullptr;
									}
									accum += color;
								}
								color = accum * Float4(1.0f / static_cast<float>(state.srcSamples));
							}
						}
						else   // Bilinear filtering
						{
							Float X = x;
							Float Y = y;

							if(state.clampToEdge)
							{
								X = Min(Max(x, 0.5f), Float(sWidth) - 0.5f);
								Y = Min(Max(y, 0.5f), Float(sHeight) - 0.5f);
							}

							Float x0 = X - 0.5f;
							Float y0 = Y - 0.5f;

							Int X0 = Max(Int(x0), 0);
							Int Y0 = Max(Int(y0), 0);

							Int X1 = X0 + 1;
							Int Y1 = Y0 + 1;
							X1 = IfThenElse(X1 >= sWidth, X0, X1);
							Y1 = IfThenElse(Y1 >= sHeight, Y0, Y1);

							Pointer<Byte> s00 = source + ComputeOffset(X0, Y0, sPitchB, srcBytes, srcQuadLayout);
							Pointer<Byte> s01 = source + ComputeOffset(X1, Y0, sPitchB, srcBytes, srcQuadLayout);
							Pointer<Byte> s10 = source + ComputeOffset(X0, Y1, sPitchB, srcBytes, srcQuadLayout);
							Pointer<Byte> s11 = source + ComputeOffset(X1, Y1, sPitchB, srcBytes, srcQuadLayout);

							Float4 c00; if(!read(c00, s00, state)) return nullptr;
							Float4 c01; if(!read(c01, s01, state)) return nullptr;
							Float4 c10; if(!read(c10, s10, state)) return nullptr;
							Float4 c11; if(!read(c11, s11, state)) return nullptr;

							if(state.convertSRGB && state.sourceFormat.isSRGBformat()) // sRGB -> RGB
							{
								if(!ApplyScaleAndClamp(c00, state)) return nullptr;
								if(!ApplyScaleAndClamp(c01, state)) return nullptr;
								if(!ApplyScaleAndClamp(c10, state)) return nullptr;
								if(!ApplyScaleAndClamp(c11, state)) return nullptr;
								preScaled = true;
							}

							Float4 fx = Float4(x0 - Float(X0));
							Float4 fy = Float4(y0 - Float(Y0));
							Float4 ix = Float4(1.0f) - fx;
							Float4 iy = Float4(1.0f) - fy;

							color = (c00 * ix + c01 * fx) * iy +
							        (c10 * ix + c11 * fx) * fy;
						}

						if(!ApplyScaleAndClamp(color, state, preScaled))
						{
							return nullptr;
						}

						for(int s = 0; s < state.destSamples; s++)
						{
							if(!write(color, d, state))
							{
								return nullptr;
							}

							d += *Pointer<Int>(blit + OFFSET(BlitData,dSliceB));
						}
					}
				}
			}
		}

		return function("BlitRoutine");
	}

	Routine *Blitter::getRoutine(const State &state)
	{
		criticalSection.lock();
		Routine *blitRoutine = blitCache->query(state);

		if(!blitRoutine)
		{
			blitRoutine = generate(state);

			if(!blitRoutine)
			{
				criticalSection.unlock();
				UNIMPLEMENTED("blitRoutine");
				return nullptr;
			}

			blitCache->add(state, blitRoutine);
		}

		criticalSection.unlock();

		return blitRoutine;
	}

	void Blitter::blit(vk::Image *src, vk::Image *dst, VkImageBlit region, VkFilter filter)
	{
		if(dst->getFormat() == VK_FORMAT_UNDEFINED)
		{
			return;
		}

		if((region.srcSubresource.layerCount != region.dstSubresource.layerCount) ||
		   (region.srcSubresource.aspectMask != region.dstSubresource.aspectMask))
		{
			UNIMPLEMENTED("region");
		}

		if(region.dstOffsets[0].x > region.dstOffsets[1].x)
		{
			std::swap(region.srcOffsets[0].x, region.srcOffsets[1].x);
			std::swap(region.dstOffsets[0].x, region.dstOffsets[1].x);
		}

		if(region.dstOffsets[0].y > region.dstOffsets[1].y)
		{
			std::swap(region.srcOffsets[0].y, region.srcOffsets[1].y);
			std::swap(region.dstOffsets[0].y, region.dstOffsets[1].y);
		}

		VkExtent3D srcExtent = src->getMipLevelExtent(region.srcSubresource.mipLevel);

		int32_t numSlices = (region.srcOffsets[1].z - region.srcOffsets[0].z);
		ASSERT(numSlices == (region.dstOffsets[1].z - region.dstOffsets[0].z));

		VkImageAspectFlagBits srcAspect = static_cast<VkImageAspectFlagBits>(region.srcSubresource.aspectMask);
		VkImageAspectFlagBits dstAspect = static_cast<VkImageAspectFlagBits>(region.dstSubresource.aspectMask);

		float widthRatio = static_cast<float>(region.srcOffsets[1].x - region.srcOffsets[0].x) /
		                   static_cast<float>(region.dstOffsets[1].x - region.dstOffsets[0].x);
		float heightRatio = static_cast<float>(region.srcOffsets[1].y - region.srcOffsets[0].y) /
		                    static_cast<float>(region.dstOffsets[1].y - region.dstOffsets[0].y);
		float x0 = region.srcOffsets[0].x + (0.5f - region.dstOffsets[0].x) * widthRatio;
		float y0 = region.srcOffsets[0].y + (0.5f - region.dstOffsets[0].y) * heightRatio;

		bool doFilter = (filter != VK_FILTER_NEAREST);
		State state(src->getFormat(srcAspect), dst->getFormat(dstAspect), src->getSampleCountFlagBits(), dst->getSampleCountFlagBits(),
		            { doFilter, srcAspect == VK_IMAGE_ASPECT_STENCIL_BIT, doFilter });
		state.clampToEdge = (region.srcOffsets[0].x < 0) ||
		                    (region.srcOffsets[0].y < 0) ||
		                    (static_cast<uint32_t>(region.srcOffsets[1].x) > srcExtent.width) ||
		                    (static_cast<uint32_t>(region.srcOffsets[1].y) > srcExtent.height) ||
		                    (doFilter && ((x0 < 0.5f) || (y0 < 0.5f)));

		Routine *blitRoutine = getRoutine(state);
		if(!blitRoutine)
		{
			return;
		}

		void(*blitFunction)(const BlitData *data) = (void(*)(const BlitData*))blitRoutine->getEntry();

		BlitData data =
		{
			nullptr, // source
			nullptr, // dest
			src->rowPitchBytes(srcAspect, region.srcSubresource.mipLevel),   // sPitchB
			dst->rowPitchBytes(dstAspect, region.dstSubresource.mipLevel),   // dPitchB
			src->slicePitchBytes(srcAspect, region.srcSubresource.mipLevel), // sSliceB
			dst->slicePitchBytes(dstAspect, region.dstSubresource.mipLevel), // dSliceB

			x0,
			y0,
			widthRatio,
			heightRatio,

			region.dstOffsets[0].y, // y0d
			region.dstOffsets[1].y, // y1d
			region.dstOffsets[0].x, // x0d
			region.dstOffsets[1].x, // x1d

			static_cast<int>(srcExtent.width), // sWidth
			static_cast<int>(srcExtent.height) // sHeight;
		};

		VkOffset3D srcOffset = { 0, 0, region.srcOffsets[0].z };
		VkOffset3D dstOffset = { 0, 0, region.dstOffsets[0].z };

		VkImageSubresourceLayers srcSubresLayers =
		{
			region.srcSubresource.aspectMask,
			region.srcSubresource.mipLevel,
			region.srcSubresource.baseArrayLayer,
			1
		};

		VkImageSubresourceLayers dstSubresLayers =
		{
			region.dstSubresource.aspectMask,
			region.dstSubresource.mipLevel,
			region.dstSubresource.baseArrayLayer,
			1
		};

		VkImageSubresourceRange srcSubresRange =
		{
			region.srcSubresource.aspectMask,
			region.srcSubresource.mipLevel,
			1,
			region.srcSubresource.baseArrayLayer,
			region.srcSubresource.layerCount
		};

		uint32_t lastLayer = src->getLastLayerIndex(srcSubresRange);

		for(; srcSubresLayers.baseArrayLayer <= lastLayer; srcSubresLayers.baseArrayLayer++, dstSubresLayers.baseArrayLayer++)
		{
			srcOffset.z = region.srcOffsets[0].z;
			dstOffset.z = region.dstOffsets[0].z;

			for(int i = 0; i < numSlices; i++)
			{
				data.source = src->getTexelPointer(srcOffset, srcSubresLayers);
				data.dest = dst->getTexelPointer(dstOffset, dstSubresLayers);

				ASSERT(data.source < src->end());
				ASSERT(data.dest < dst->end());

				blitFunction(&data);
				srcOffset.z++;
				dstOffset.z++;
			}
		}
	}
}
