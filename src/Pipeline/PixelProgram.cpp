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

#include "PixelProgram.hpp"

#include "SamplerCore.hpp"
#include "Device/Primitive.hpp"
#include "Device/Renderer.hpp"

namespace sw
{
	extern bool postBlendSRGB;

	void PixelProgram::setBuiltins(Int &x, Int &y, Float4(&z)[4], Float4 &w)
	{
		// TODO: wire up builtins correctly
	}

	void PixelProgram::applyShader(Int cMask[4])
	{
		routine.descriptorSets = data + OFFSET(DrawData, descriptorSets);
		routine.descriptorDynamicOffsets = data + OFFSET(DrawData, descriptorDynamicOffsets);
		routine.pushConstants = data + OFFSET(DrawData, pushConstants);

		auto activeLaneMask = SIMD::Int(0xFFFFFFFF); // TODO: Control this.
		spirvShader->emit(&routine, activeLaneMask, descriptorSets);
		spirvShader->emitEpilog(&routine);

		for(int i = 0; i < RENDERTARGETS; i++)
		{
			c[i].x = routine.outputs[i * 4];
			c[i].y = routine.outputs[i * 4 + 1];
			c[i].z = routine.outputs[i * 4 + 2];
			c[i].w = routine.outputs[i * 4 + 3];
		}

		clampColor(c);

		if(spirvShader->getModes().ContainsKill)
		{
			for (auto i = 0u; i < state.multiSample; i++)
			{
				cMask[i] &= ~routine.killMask;
			}
		}

		if(spirvShader->getModes().DepthReplacing)
		{
			oDepth = Min(Max(oDepth, Float4(0.0f)), Float4(1.0f));
		}
	}

	Bool PixelProgram::alphaTest(Int cMask[4])
	{
		if(!state.alphaTestActive())
		{
			return true;
		}

		if(state.transparencyAntialiasing == TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			alphaToCoverage(cMask, c[0].w);
		}
		else ASSERT(false);

		Int pass = cMask[0];

		for(unsigned int q = 1; q < state.multiSample; q++)
		{
			pass = pass | cMask[q];
		}

		return pass != 0x0;
	}

	void PixelProgram::rasterOperation(Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		for(int index = 0; index < RENDERTARGETS; index++)
		{
			if(!state.colorWriteActive(index))
			{
				continue;
			}

			if(!postBlendSRGB && state.writeSRGB && !isSRGB(index))
			{
				c[index].x = linearToSRGB(c[index].x);
				c[index].y = linearToSRGB(c[index].y);
				c[index].z = linearToSRGB(c[index].z);
			}

			switch(state.targetFormat[index])
			{
			case VK_FORMAT_R5G6B5_UNORM_PACK16:
			case VK_FORMAT_B8G8R8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_SRGB:
			case VK_FORMAT_R8G8_UNORM:
			case VK_FORMAT_R8_UNORM:
			case VK_FORMAT_R16G16_UNORM:
			case VK_FORMAT_R16G16B16A16_UNORM:
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Pointer<Byte> buffer = cBuffer[index] + q * *Pointer<Int>(data + OFFSET(DrawData, colorSliceB[index]));
					Vector4s color;

					if(state.targetFormat[index] == VK_FORMAT_R5G6B5_UNORM_PACK16)
					{
						color.x = UShort4(c[index].x * Float4(0xFBFF), false);
						color.y = UShort4(c[index].y * Float4(0xFDFF), false);
						color.z = UShort4(c[index].z * Float4(0xFBFF), false);
						color.w = UShort4(c[index].w * Float4(0xFFFF), false);
					}
					else
					{
						color.x = convertFixed16(c[index].x, false);
						color.y = convertFixed16(c[index].y, false);
						color.z = convertFixed16(c[index].z, false);
						color.w = convertFixed16(c[index].w, false);
					}

					if(state.multiSampleMask & (1 << q))
					{
						alphaBlend(index, buffer, color, x);
						writeColor(index, buffer, x, color, sMask[q], zMask[q], cMask[q]);
					}
				}
				break;
			case VK_FORMAT_R32_SFLOAT:
			case VK_FORMAT_R32G32_SFLOAT:
			case VK_FORMAT_R32G32B32A32_SFLOAT:
			case VK_FORMAT_R32_SINT:
			case VK_FORMAT_R32G32_SINT:
			case VK_FORMAT_R32G32B32A32_SINT:
			case VK_FORMAT_R32_UINT:
			case VK_FORMAT_R32G32_UINT:
			case VK_FORMAT_R32G32B32A32_UINT:
			case VK_FORMAT_R16_SINT:
			case VK_FORMAT_R16G16_SINT:
			case VK_FORMAT_R16G16B16A16_SINT:
			case VK_FORMAT_R16_UINT:
			case VK_FORMAT_R16G16_UINT:
			case VK_FORMAT_R16G16B16A16_UINT:
			case VK_FORMAT_R8_SINT:
			case VK_FORMAT_R8G8_SINT:
			case VK_FORMAT_R8G8B8A8_SINT:
			case VK_FORMAT_R8_UINT:
			case VK_FORMAT_R8G8_UINT:
			case VK_FORMAT_R8G8B8A8_UINT:
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Pointer<Byte> buffer = cBuffer[index] + q * *Pointer<Int>(data + OFFSET(DrawData, colorSliceB[index]));
					Vector4f color = c[index];

					if(state.multiSampleMask & (1 << q))
					{
						alphaBlend(index, buffer, color, x);
						writeColor(index, buffer, x, color, sMask[q], zMask[q], cMask[q]);
					}
				}
				break;
			default:
				UNIMPLEMENTED("VkFormat: %d", int(state.targetFormat[index]));
			}
		}
	}

	void PixelProgram::clampColor(Vector4f oC[RENDERTARGETS])
	{
		for(int index = 0; index < RENDERTARGETS; index++)
		{
			if(!state.colorWriteActive(index) && !(index == 0 && state.alphaTestActive()))
			{
				continue;
			}

			switch(state.targetFormat[index])
			{
			case VK_FORMAT_UNDEFINED:
				break;
			case VK_FORMAT_R5G6B5_UNORM_PACK16:
			case VK_FORMAT_B8G8R8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_SRGB:
			case VK_FORMAT_R8G8_UNORM:
			case VK_FORMAT_R8_UNORM:
			case VK_FORMAT_R16G16_UNORM:
			case VK_FORMAT_R16G16B16A16_UNORM:
				oC[index].x = Max(oC[index].x, Float4(0.0f)); oC[index].x = Min(oC[index].x, Float4(1.0f));
				oC[index].y = Max(oC[index].y, Float4(0.0f)); oC[index].y = Min(oC[index].y, Float4(1.0f));
				oC[index].z = Max(oC[index].z, Float4(0.0f)); oC[index].z = Min(oC[index].z, Float4(1.0f));
				oC[index].w = Max(oC[index].w, Float4(0.0f)); oC[index].w = Min(oC[index].w, Float4(1.0f));
				break;
			case VK_FORMAT_R32_SFLOAT:
			case VK_FORMAT_R32G32_SFLOAT:
			case VK_FORMAT_R32G32B32A32_SFLOAT:
			case VK_FORMAT_R32_SINT:
			case VK_FORMAT_R32G32_SINT:
			case VK_FORMAT_R32G32B32A32_SINT:
			case VK_FORMAT_R32_UINT:
			case VK_FORMAT_R32G32_UINT:
			case VK_FORMAT_R32G32B32A32_UINT:
			case VK_FORMAT_R16_SINT:
			case VK_FORMAT_R16G16_SINT:
			case VK_FORMAT_R16G16B16A16_SINT:
			case VK_FORMAT_R16_UINT:
			case VK_FORMAT_R16G16_UINT:
			case VK_FORMAT_R16G16B16A16_UINT:
			case VK_FORMAT_R8_SINT:
			case VK_FORMAT_R8G8_SINT:
			case VK_FORMAT_R8G8B8A8_SINT:
			case VK_FORMAT_R8_UINT:
			case VK_FORMAT_R8G8_UINT:
			case VK_FORMAT_R8G8B8A8_UINT:
				break;
			default:
				UNIMPLEMENTED("VkFormat: %d", int(state.targetFormat[index]));
			}
		}
	}

	Float4 PixelProgram::linearToSRGB(const Float4 &x)   // Approximates x^(1.0/2.2)
	{
		Float4 sqrtx = Rcp_pp(RcpSqrt_pp(x));
		Float4 sRGB = sqrtx * Float4(1.14f) - x * Float4(0.14f);

		return Min(Max(sRGB, Float4(0.0f)), Float4(1.0f));
	}
}
