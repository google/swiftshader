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

#include "QuadRasterizer.hpp"

#include "Primitive.hpp"
#include "Renderer.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Debug.hpp"
#include "System/Math.hpp"

namespace sw {

QuadRasterizer::QuadRasterizer(const PixelProcessor::State &state, SpirvShader const *spirvShader)
    : state(state)
    , spirvShader{ spirvShader }
{
}

QuadRasterizer::~QuadRasterizer()
{
}

void QuadRasterizer::generate()
{
	constants = *Pointer<Pointer<Byte>>(data + OFFSET(DrawData, constants));
	occlusion = 0;

	Do
	{
		Int yMin = *Pointer<Int>(primitive + OFFSET(Primitive, yMin));
		Int yMax = *Pointer<Int>(primitive + OFFSET(Primitive, yMax));

		Int cluster2 = cluster + cluster;
		yMin += clusterCount * 2 - 2 - cluster2;
		yMin &= -clusterCount * 2;
		yMin += cluster2;

		If(yMin < yMax)
		{
			rasterize(yMin, yMax);
		}

		primitive += sizeof(Primitive) * state.multiSampleCount;
		count--;
	}
	Until(count == 0);

	if(state.occlusionEnabled)
	{
		UInt clusterOcclusion = *Pointer<UInt>(data + OFFSET(DrawData, occlusion) + 4 * cluster);
		clusterOcclusion += occlusion;
		*Pointer<UInt>(data + OFFSET(DrawData, occlusion) + 4 * cluster) = clusterOcclusion;
	}

	Return();
}

void QuadRasterizer::rasterize(Int &yMin, Int &yMax)
{
	Pointer<Byte> cBuffer[RENDERTARGETS];
	Pointer<Byte> zBuffer;
	Pointer<Byte> sBuffer;

	Int clusterCountLog2 = 31 - Ctlz(UInt(clusterCount), false);

	for(int index = 0; index < RENDERTARGETS; index++)
	{
		if(state.colorWriteActive(index))
		{
			cBuffer[index] = *Pointer<Pointer<Byte>>(data + OFFSET(DrawData, colorBuffer[index])) + yMin * *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));
		}
	}

	if(state.depthTestActive)
	{
		zBuffer = *Pointer<Pointer<Byte>>(data + OFFSET(DrawData, depthBuffer)) + yMin * *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));
	}

	if(state.stencilActive)
	{
		sBuffer = *Pointer<Pointer<Byte>>(data + OFFSET(DrawData, stencilBuffer)) + yMin * *Pointer<Int>(data + OFFSET(DrawData, stencilPitchB));
	}

	Int y = yMin;

	Do
	{
		Int x0a = Int(*Pointer<Short>(primitive + OFFSET(Primitive, outline->left) + (y + 0) * sizeof(Primitive::Span)));
		Int x0b = Int(*Pointer<Short>(primitive + OFFSET(Primitive, outline->left) + (y + 1) * sizeof(Primitive::Span)));
		Int x0 = Min(x0a, x0b);

		for(unsigned int q = 1; q < state.multiSampleCount; q++)
		{
			x0a = Int(*Pointer<Short>(primitive + q * sizeof(Primitive) + OFFSET(Primitive, outline->left) + (y + 0) * sizeof(Primitive::Span)));
			x0b = Int(*Pointer<Short>(primitive + q * sizeof(Primitive) + OFFSET(Primitive, outline->left) + (y + 1) * sizeof(Primitive::Span)));
			x0 = Min(x0, Min(x0a, x0b));
		}

		x0 &= 0xFFFFFFFE;

		Int x1a = Int(*Pointer<Short>(primitive + OFFSET(Primitive, outline->right) + (y + 0) * sizeof(Primitive::Span)));
		Int x1b = Int(*Pointer<Short>(primitive + OFFSET(Primitive, outline->right) + (y + 1) * sizeof(Primitive::Span)));
		Int x1 = Max(x1a, x1b);

		for(unsigned int q = 1; q < state.multiSampleCount; q++)
		{
			x1a = Int(*Pointer<Short>(primitive + q * sizeof(Primitive) + OFFSET(Primitive, outline->right) + (y + 0) * sizeof(Primitive::Span)));
			x1b = Int(*Pointer<Short>(primitive + q * sizeof(Primitive) + OFFSET(Primitive, outline->right) + (y + 1) * sizeof(Primitive::Span)));
			x1 = Max(x1, Max(x1a, x1b));
		}

		Float4 yyyy = Float4(Float(y)) + *Pointer<Float4>(primitive + OFFSET(Primitive, yQuad), 16);

		if(interpolateZ())
		{
			for(unsigned int q = 0; q < state.multiSampleCount; q++)
			{
				Float4 y = yyyy;

				if(state.enableMultiSampling)
				{
					y -= *Pointer<Float4>(constants + OFFSET(Constants, Y) + q * sizeof(float4));
				}

				Dz[q] = *Pointer<Float4>(primitive + OFFSET(Primitive, z.C), 16) + y * *Pointer<Float4>(primitive + OFFSET(Primitive, z.B), 16);
			}
		}

		If(x0 < x1)
		{
			if(interpolateW())
			{
				Dw = *Pointer<Float4>(primitive + OFFSET(Primitive, w.C), 16) + yyyy * *Pointer<Float4>(primitive + OFFSET(Primitive, w.B), 16);
			}

			if(spirvShader)
			{
				for(int interpolant = 0; interpolant < MAX_INTERFACE_COMPONENTS; interpolant++)
				{
					if(spirvShader->inputs[interpolant].Type == SpirvShader::ATTRIBTYPE_UNUSED)
						continue;

					Dv[interpolant] = *Pointer<Float4>(primitive + OFFSET(Primitive, V[interpolant].C), 16);
					if(!spirvShader->inputs[interpolant].Flat)
					{
						Dv[interpolant] +=
						    yyyy * *Pointer<Float4>(primitive + OFFSET(Primitive, V[interpolant].B), 16);
					}
				}

				for(unsigned int i = 0; i < state.numClipDistances; i++)
				{
					DclipDistance[i] = *Pointer<Float4>(primitive + OFFSET(Primitive, clipDistance[i].C), 16) +
					                   yyyy * *Pointer<Float4>(primitive + OFFSET(Primitive, clipDistance[i].B), 16);
				}

				for(unsigned int i = 0; i < state.numCullDistances; i++)
				{
					DcullDistance[i] = *Pointer<Float4>(primitive + OFFSET(Primitive, cullDistance[i].C), 16) +
					                   yyyy * *Pointer<Float4>(primitive + OFFSET(Primitive, cullDistance[i].B), 16);
				}
			}

			Short4 xLeft[4];
			Short4 xRight[4];

			for(unsigned int q = 0; q < state.multiSampleCount; q++)
			{
				xLeft[q] = *Pointer<Short4>(primitive + q * sizeof(Primitive) + OFFSET(Primitive, outline) + y * sizeof(Primitive::Span));
				xRight[q] = xLeft[q];

				xLeft[q] = Swizzle(xLeft[q], 0x0022) - Short4(1, 2, 1, 2);
				xRight[q] = Swizzle(xRight[q], 0x1133) - Short4(0, 1, 0, 1);
			}

			For(Int x = x0, x < x1, x += 2)
			{
				Short4 xxxx = Short4(x);
				Int cMask[4];

				for(unsigned int q = 0; q < state.multiSampleCount; q++)
				{
					if(state.multiSampleMask & (1 << q))
					{
						unsigned int i = state.enableMultiSampling ? q : 0;
						Short4 mask = CmpGT(xxxx, xLeft[i]) & CmpGT(xRight[i], xxxx);
						cMask[q] = SignMask(PackSigned(mask, mask)) & 0x0000000F;
					}
					else
					{
						cMask[q] = 0;
					}
				}

				quad(cBuffer, zBuffer, sBuffer, cMask, x, y);
			}
		}

		for(int index = 0; index < RENDERTARGETS; index++)
		{
			if(state.colorWriteActive(index))
			{
				cBuffer[index] += *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index])) << (1 + clusterCountLog2);  // FIXME: Precompute
			}
		}

		if(state.depthTestActive)
		{
			zBuffer += *Pointer<Int>(data + OFFSET(DrawData, depthPitchB)) << (1 + clusterCountLog2);  // FIXME: Precompute
		}

		if(state.stencilActive)
		{
			sBuffer += *Pointer<Int>(data + OFFSET(DrawData, stencilPitchB)) << (1 + clusterCountLog2);  // FIXME: Precompute
		}

		y += 2 * clusterCount;
	}
	Until(y >= yMax);
}

Float4 QuadRasterizer::interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective, bool clamp)
{
	Float4 interpolant = D;

	if(!flat)
	{
		interpolant += x * *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation, A), 16);

		if(perspective)
		{
			interpolant *= rhw;
		}
	}

	if(clamp)
	{
		interpolant = Min(Max(interpolant, Float4(0.0f)), Float4(1.0f));
	}

	return interpolant;
}

bool QuadRasterizer::interpolateZ() const
{
	return state.depthTestActive || (spirvShader && spirvShader->hasBuiltinInput(spv::BuiltInFragCoord));
}

bool QuadRasterizer::interpolateW() const
{
	// Note: could optimize cases where there is a fragment shader but it has no
	// perspective-correct inputs, but that's vanishingly rare.
	return spirvShader != nullptr;
}

}  // namespace sw
