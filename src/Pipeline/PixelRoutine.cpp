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

#include "PixelRoutine.hpp"

#include "Constants.hpp"
#include "SamplerCore.hpp"
#include "Device/Primitive.hpp"
#include "Device/QuadRasterizer.hpp"
#include "Device/Renderer.hpp"
#include "System/Debug.hpp"
#include "Vulkan/VkPipelineLayout.hpp"
#include "Vulkan/VkStringify.hpp"

namespace sw {

PixelRoutine::PixelRoutine(
    const PixelProcessor::State &state,
    vk::PipelineLayout const *pipelineLayout,
    SpirvShader const *spirvShader,
    const vk::DescriptorSet::Bindings &descriptorSets)
    : QuadRasterizer(state, spirvShader)
    , routine(pipelineLayout)
    , descriptorSets(descriptorSets)
    , shaderContainsInterpolation(spirvShader && spirvShader->getUsedCapabilities().InterpolationFunction)
    , shaderContainsSampleQualifier(spirvShader && spirvShader->getAnalysis().ContainsSampleQualifier)
    , perSampleShading((state.sampleShadingEnabled && (state.minSampleShading * state.multiSampleCount > 1.0f)) ||
                       shaderContainsSampleQualifier || shaderContainsInterpolation)  // TODO(b/194714095)
    , invocationCount(perSampleShading ? state.multiSampleCount : 1)
{
	if(spirvShader)
	{
		spirvShader->emitProlog(&routine);

		// Clearing inputs to 0 is not demanded by the spec,
		// but it makes the undefined behavior deterministic.
		// TODO(b/155148722): Remove to detect UB.
		for(int i = 0; i < MAX_INTERFACE_COMPONENTS; i++)
		{
			routine.inputs[i] = Float4(0.0f);
		}
	}
}

PixelRoutine::~PixelRoutine()
{
}

PixelRoutine::SampleSet PixelRoutine::getSampleSet(int invocation) const
{
	unsigned int sampleBegin = perSampleShading ? invocation : 0;
	unsigned int sampleEnd = perSampleShading ? (invocation + 1) : state.multiSampleCount;

	SampleSet samples;

	for(unsigned int q = sampleBegin; q < sampleEnd; q++)
	{
		if(state.multiSampleMask & (1 << q))
		{
			samples.push_back(q);
		}
	}

	return samples;
}

void PixelRoutine::quad(Pointer<Byte> cBuffer[MAX_COLOR_BUFFERS], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y)
{
	const bool earlyFragmentTests = !spirvShader || spirvShader->getExecutionModes().EarlyFragmentTests;

	Int zMask[4];  // Depth mask
	Int sMask[4];  // Stencil mask
	Float4 unclampedZ[4];

	for(int invocation = 0; invocation < invocationCount; invocation++)
	{
		SampleSet samples = getSampleSet(invocation);

		if(samples.empty())
		{
			continue;
		}

		for(unsigned int q : samples)
		{
			zMask[q] = cMask[q];
			sMask[q] = cMask[q];
		}

		stencilTest(sBuffer, x, sMask, samples);

		Float4 f;
		Float4 rhwCentroid;

		Float4 xxxx = Float4(Float(x)) + *Pointer<Float4>(primitive + OFFSET(Primitive, xQuad), 16);

		if(interpolateZ())
		{
			for(unsigned int q : samples)
			{
				Float4 x = xxxx;

				if(state.enableMultiSampling)
				{
					x -= *Pointer<Float4>(constants + OFFSET(Constants, X) + q * sizeof(float4));
				}

				z[q] = interpolate(x, Dz[q], z[q], primitive + OFFSET(Primitive, z), false, false);

				if(state.depthBias)
				{
					z[q] += *Pointer<Float4>(primitive + OFFSET(Primitive, zBias), 16);
				}

				unclampedZ[q] = z[q];
			}
		}

		Bool depthPass = false;

		if(earlyFragmentTests)
		{
			for(unsigned int q : samples)
			{
				z[q] = clampDepth(z[q]);
				depthPass = depthPass || depthTest(zBuffer, q, x, z[q], sMask[q], zMask[q], cMask[q]);
				depthBoundsTest(zBuffer, q, x, zMask[q], cMask[q]);
			}
		}

		If(depthPass || !earlyFragmentTests)
		{
			if(earlyFragmentTests)
			{
				writeDepth(zBuffer, x, zMask, samples);
			}

			Float4 yyyy = Float4(Float(y)) + *Pointer<Float4>(primitive + OFFSET(Primitive, yQuad), 16);

			// Centroid locations
			Float4 XXXX = Float4(0.0f);
			Float4 YYYY = Float4(0.0f);

			if(state.centroid || shaderContainsInterpolation)  // TODO(b/194714095)
			{
				Float4 WWWW(1.0e-9f);

				for(unsigned int q : samples)
				{
					XXXX += *Pointer<Float4>(constants + OFFSET(Constants, sampleX[q]) + 16 * cMask[q]);
					YYYY += *Pointer<Float4>(constants + OFFSET(Constants, sampleY[q]) + 16 * cMask[q]);
					WWWW += *Pointer<Float4>(constants + OFFSET(Constants, weight) + 16 * cMask[q]);
				}

				WWWW = Rcp(WWWW, true /* relaxedPrecision */);
				XXXX *= WWWW;
				YYYY *= WWWW;

				XXXX += xxxx;
				YYYY += yyyy;
			}

			if(interpolateW())
			{
				w = interpolate(xxxx, Dw, rhw, primitive + OFFSET(Primitive, w), false, false);
				rhw = reciprocal(w, false, true);

				if(state.centroid || shaderContainsInterpolation)  // TODO(b/194714095)
				{
					rhwCentroid = reciprocal(SpirvRoutine::interpolateAtXY(XXXX, YYYY, rhwCentroid, primitive + OFFSET(Primitive, w), false, false));
				}
			}

			if(spirvShader)
			{
				if(shaderContainsInterpolation)  // TODO(b/194714095)
				{
					routine.interpolationData.primitive = primitive;

					routine.interpolationData.x = xxxx;
					routine.interpolationData.y = yyyy;
					routine.interpolationData.rhw = rhw;

					routine.interpolationData.xCentroid = XXXX;
					routine.interpolationData.yCentroid = YYYY;
					routine.interpolationData.rhwCentroid = rhwCentroid;
				}

				if(perSampleShading && (state.multiSampleCount > 1))
				{
					xxxx += Float4(Constants::SampleLocationsX[samples[0]]);
					yyyy += Float4(Constants::SampleLocationsY[samples[0]]);
				}

				int packedInterpolant = 0;
				for(int interfaceInterpolant = 0; interfaceInterpolant < MAX_INTERFACE_COMPONENTS; interfaceInterpolant++)
				{
					auto const &input = spirvShader->inputs[interfaceInterpolant];
					if(input.Type != SpirvShader::ATTRIBTYPE_UNUSED)
					{
						if(input.Centroid && state.enableMultiSampling)
						{
							routine.inputs[interfaceInterpolant] =
							    SpirvRoutine::interpolateAtXY(XXXX, YYYY, rhwCentroid,
							                                  primitive + OFFSET(Primitive, V[packedInterpolant]),
							                                  input.Flat, !input.NoPerspective);
						}
						else if(perSampleShading)
						{
							routine.inputs[interfaceInterpolant] =
							    SpirvRoutine::interpolateAtXY(xxxx, yyyy, rhw,
							                                  primitive + OFFSET(Primitive, V[packedInterpolant]),
							                                  input.Flat, !input.NoPerspective);
						}
						else
						{
							routine.inputs[interfaceInterpolant] =
							    interpolate(xxxx, Dv[interfaceInterpolant], rhw,
							                primitive + OFFSET(Primitive, V[packedInterpolant]),
							                input.Flat, !input.NoPerspective);
						}
						packedInterpolant++;
					}
				}

				setBuiltins(x, y, unclampedZ, w, cMask, samples);

				for(uint32_t i = 0; i < state.numClipDistances; i++)
				{
					auto distance = interpolate(xxxx, DclipDistance[i], rhw,
					                            primitive + OFFSET(Primitive, clipDistance[i]),
					                            false, true);

					auto clipMask = SignMask(CmpGE(distance, SIMD::Float(0)));
					for(unsigned int q : samples)
					{
						// FIXME(b/148105887): Fragments discarded by clipping do not exist at
						// all -- they should not be counted in queries or have their Z/S effects
						// performed when early fragment tests are enabled.
						cMask[q] &= clipMask;
					}

					if(spirvShader->getUsedCapabilities().ClipDistance)
					{
						auto it = spirvShader->inputBuiltins.find(spv::BuiltInClipDistance);
						if(it != spirvShader->inputBuiltins.end())
						{
							if(i < it->second.SizeInComponents)
							{
								routine.getVariable(it->second.Id)[it->second.FirstComponent + i] = distance;
							}
						}
					}
				}

				if(spirvShader->getUsedCapabilities().CullDistance)
				{
					auto it = spirvShader->inputBuiltins.find(spv::BuiltInCullDistance);
					if(it != spirvShader->inputBuiltins.end())
					{
						for(uint32_t i = 0; i < state.numCullDistances; i++)
						{
							if(i < it->second.SizeInComponents)
							{
								routine.getVariable(it->second.Id)[it->second.FirstComponent + i] =
								    interpolate(xxxx, DcullDistance[i], rhw,
								                primitive + OFFSET(Primitive, cullDistance[i]),
								                false, true);
							}
						}
					}
				}
			}

			if(spirvShader)
			{
				executeShader(cMask, earlyFragmentTests ? sMask : cMask, earlyFragmentTests ? zMask : cMask, samples);
			}

			Bool alphaPass = alphaTest(cMask, samples);

			if((spirvShader && spirvShader->getAnalysis().ContainsKill) || state.alphaToCoverage)
			{
				for(unsigned int q : samples)
				{
					zMask[q] &= cMask[q];
					sMask[q] &= cMask[q];
				}
			}

			If(alphaPass)
			{
				if(!earlyFragmentTests)
				{
					for(unsigned int q : samples)
					{
						z[q] = clampDepth(z[q]);
						depthPass = depthPass || depthTest(zBuffer, q, x, z[q], sMask[q], zMask[q], cMask[q]);
						depthBoundsTest(zBuffer, q, x, zMask[q], cMask[q]);
					}
				}

				If(depthPass)
				{
					if(!earlyFragmentTests)
					{
						writeDepth(zBuffer, x, zMask, samples);
					}

					blendColor(cBuffer, x, sMask, zMask, cMask, samples);

					occlusionSampleCount(zMask, sMask, samples);
				}
			}
		}

		writeStencil(sBuffer, x, sMask, zMask, cMask, samples);
	}
}

void PixelRoutine::stencilTest(const Pointer<Byte> &sBuffer, const Int &x, Int sMask[4], const SampleSet &samples)
{
	if(!state.stencilActive)
	{
		return;
	}

	for(unsigned int q : samples)
	{
		// (StencilRef & StencilMask) CompFunc (StencilBufferValue & StencilMask)

		Pointer<Byte> buffer = sBuffer + x;

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(data + OFFSET(DrawData, stencilSliceB));
		}

		Int pitch = *Pointer<Int>(data + OFFSET(DrawData, stencilPitchB));
		Byte8 value = *Pointer<Byte8>(buffer) & Byte8(-1, -1, 0, 0, 0, 0, 0, 0);
		value = value | (*Pointer<Byte8>(buffer + pitch - 2) & Byte8(0, 0, -1, -1, 0, 0, 0, 0));
		Byte8 valueBack = value;

		if(state.frontStencil.compareMask != 0xff)
		{
			value &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[0].testMaskQ));
		}

		stencilTest(value, state.frontStencil.compareOp, false);

		if(state.backStencil.compareMask != 0xff)
		{
			valueBack &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[1].testMaskQ));
		}

		stencilTest(valueBack, state.backStencil.compareOp, true);

		value &= *Pointer<Byte8>(primitive + OFFSET(Primitive, clockwiseMask));
		valueBack &= *Pointer<Byte8>(primitive + OFFSET(Primitive, invClockwiseMask));
		value |= valueBack;

		sMask[q] &= SignMask(value);
	}
}

void PixelRoutine::stencilTest(Byte8 &value, VkCompareOp stencilCompareMode, bool isBack)
{
	Byte8 equal;

	switch(stencilCompareMode)
	{
	case VK_COMPARE_OP_ALWAYS:
		value = Byte8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
		break;
	case VK_COMPARE_OP_NEVER:
		value = Byte8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
		break;
	case VK_COMPARE_OP_LESS:  // a < b ~ b > a
		value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
		value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedSignedQ)));
		break;
	case VK_COMPARE_OP_EQUAL:
		value = CmpEQ(value, *Pointer<Byte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedQ)));
		break;
	case VK_COMPARE_OP_NOT_EQUAL:  // a != b ~ !(a == b)
		value = CmpEQ(value, *Pointer<Byte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedQ)));
		value ^= Byte8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
		break;
	case VK_COMPARE_OP_LESS_OR_EQUAL:  // a <= b ~ (b > a) || (a == b)
		equal = value;
		equal = CmpEQ(equal, *Pointer<Byte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedQ)));
		value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
		value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedSignedQ)));
		value |= equal;
		break;
	case VK_COMPARE_OP_GREATER:  // a > b
		equal = *Pointer<Byte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedSignedQ));
		value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
		equal = CmpGT(As<SByte8>(equal), As<SByte8>(value));
		value = equal;
		break;
	case VK_COMPARE_OP_GREATER_OR_EQUAL:  // a >= b ~ !(a < b) ~ !(b > a)
		value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
		value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(data + OFFSET(DrawData, stencil[isBack].referenceMaskedSignedQ)));
		value ^= Byte8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
		break;
	default:
		UNSUPPORTED("VkCompareOp: %d", int(stencilCompareMode));
	}
}

Bool PixelRoutine::depthTest32F(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask)
{
	Float4 Z = z;

	Pointer<Byte> buffer = zBuffer + 4 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Float4 zValue;

	if(state.depthCompareMode != VK_COMPARE_OP_NEVER || (state.depthCompareMode != VK_COMPARE_OP_ALWAYS && !state.depthWriteEnable))
	{
		zValue = Float4(*Pointer<Float2>(buffer), *Pointer<Float2>(buffer + pitch));
	}

	Int4 zTest;

	switch(state.depthCompareMode)
	{
	case VK_COMPARE_OP_ALWAYS:
		// Optimized
		break;
	case VK_COMPARE_OP_NEVER:
		// Optimized
		break;
	case VK_COMPARE_OP_EQUAL:
		zTest = CmpEQ(zValue, Z);
		break;
	case VK_COMPARE_OP_NOT_EQUAL:
		zTest = CmpNEQ(zValue, Z);
		break;
	case VK_COMPARE_OP_LESS:
		zTest = CmpNLE(zValue, Z);
		break;
	case VK_COMPARE_OP_GREATER_OR_EQUAL:
		zTest = CmpLE(zValue, Z);
		break;
	case VK_COMPARE_OP_LESS_OR_EQUAL:
		zTest = CmpNLT(zValue, Z);
		break;
	case VK_COMPARE_OP_GREATER:
		zTest = CmpLT(zValue, Z);
		break;
	default:
		UNSUPPORTED("VkCompareOp: %d", int(state.depthCompareMode));
	}

	switch(state.depthCompareMode)
	{
	case VK_COMPARE_OP_ALWAYS:
		zMask = cMask;
		break;
	case VK_COMPARE_OP_NEVER:
		zMask = 0x0;
		break;
	default:
		zMask = SignMask(zTest) & cMask;
		break;
	}

	if(state.stencilActive)
	{
		zMask &= sMask;
	}

	return zMask != 0;
}

Bool PixelRoutine::depthTest16(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask)
{
	Short4 Z = convertFixed16(z, true);

	Pointer<Byte> buffer = zBuffer + 2 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Short4 zValue;

	if(state.depthCompareMode != VK_COMPARE_OP_NEVER || (state.depthCompareMode != VK_COMPARE_OP_ALWAYS && !state.depthWriteEnable))
	{
		zValue = As<Short4>(Insert(As<Int2>(zValue), *Pointer<Int>(buffer), 0));
		zValue = As<Short4>(Insert(As<Int2>(zValue), *Pointer<Int>(buffer + pitch), 1));
	}

	Int4 zTest;

	// Bias values to make unsigned compares out of Reactor's (due SSE's) signed compares only
	zValue = zValue - Short4(0x8000u);
	Z = Z - Short4(0x8000u);

	switch(state.depthCompareMode)
	{
	case VK_COMPARE_OP_ALWAYS:
		// Optimized
		break;
	case VK_COMPARE_OP_NEVER:
		// Optimized
		break;
	case VK_COMPARE_OP_EQUAL:
		zTest = Int4(CmpEQ(zValue, Z));
		break;
	case VK_COMPARE_OP_NOT_EQUAL:
		zTest = ~Int4(CmpEQ(zValue, Z));
		break;
	case VK_COMPARE_OP_LESS:
		zTest = Int4(CmpGT(zValue, Z));
		break;
	case VK_COMPARE_OP_GREATER_OR_EQUAL:
		zTest = ~Int4(CmpGT(zValue, Z));
		break;
	case VK_COMPARE_OP_LESS_OR_EQUAL:
		zTest = ~Int4(CmpGT(Z, zValue));
		break;
	case VK_COMPARE_OP_GREATER:
		zTest = Int4(CmpGT(Z, zValue));
		break;
	default:
		UNSUPPORTED("VkCompareOp: %d", int(state.depthCompareMode));
	}

	switch(state.depthCompareMode)
	{
	case VK_COMPARE_OP_ALWAYS:
		zMask = cMask;
		break;
	case VK_COMPARE_OP_NEVER:
		zMask = 0x0;
		break;
	default:
		zMask = SignMask(zTest) & cMask;
		break;
	}

	if(state.stencilActive)
	{
		zMask &= sMask;
	}

	return zMask != 0;
}

Float4 PixelRoutine::clampDepth(const Float4 &z)
{
	if(!state.depthClamp)
	{
		return z;
	}

	return Min(Max(z, Float4(state.minDepthClamp)), Float4(state.maxDepthClamp));
}

Bool PixelRoutine::depthTest(const Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &sMask, Int &zMask, const Int &cMask)
{
	if(!state.depthTestActive)
	{
		return true;
	}

	switch(state.depthFormat)
	{
	case VK_FORMAT_D16_UNORM:
		return depthTest16(zBuffer, q, x, z, sMask, zMask, cMask);
	case VK_FORMAT_D32_SFLOAT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return depthTest32F(zBuffer, q, x, z, sMask, zMask, cMask);
	default:
		UNSUPPORTED("Depth format: %d", int(state.depthFormat));
		return false;
	}
}

Int4 PixelRoutine::depthBoundsTest16(const Pointer<Byte> &zBuffer, int q, const Int &x)
{
	Pointer<Byte> buffer = zBuffer + 2 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Float4 minDepthBound(state.minDepthBounds);
	Float4 maxDepthBound(state.maxDepthBounds);

	Int2 z;
	z = Insert(z, *Pointer<Int>(buffer), 0);
	z = Insert(z, *Pointer<Int>(buffer + pitch), 1);

	Float4 zValue = convertFloat32(As<UShort4>(z));
	return Int4(CmpLE(minDepthBound, zValue) & CmpLE(zValue, maxDepthBound));
}

Int4 PixelRoutine::depthBoundsTest32F(const Pointer<Byte> &zBuffer, int q, const Int &x)
{
	Pointer<Byte> buffer = zBuffer + 4 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Float4 zValue = Float4(*Pointer<Float2>(buffer), *Pointer<Float2>(buffer + pitch));
	return Int4(CmpLE(Float4(state.minDepthBounds), zValue) & CmpLE(zValue, Float4(state.maxDepthBounds)));
}

void PixelRoutine::depthBoundsTest(const Pointer<Byte> &zBuffer, int q, const Int &x, Int &zMask, Int &cMask)
{
	if(!state.depthBoundsTestActive)
	{
		return;
	}

	Int4 zTest;
	switch(state.depthFormat)
	{
	case VK_FORMAT_D16_UNORM:
		zTest = depthBoundsTest16(zBuffer, q, x);
		break;
	case VK_FORMAT_D32_SFLOAT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		zTest = depthBoundsTest32F(zBuffer, q, x);
		break;
	default:
		UNSUPPORTED("Depth format: %d", int(state.depthFormat));
		break;
	}

	if(!state.depthTestActive)
	{
		cMask &= zMask & SignMask(zTest);
	}
	else
	{
		zMask &= cMask & SignMask(zTest);
	}
}

void PixelRoutine::alphaToCoverage(Int cMask[4], const Float4 &alpha, const SampleSet &samples)
{
	static const int a2c[4] = {
		OFFSET(DrawData, a2c0),
		OFFSET(DrawData, a2c1),
		OFFSET(DrawData, a2c2),
		OFFSET(DrawData, a2c3),
	};

	for(unsigned int q : samples)
	{
		Int4 coverage = CmpNLT(alpha, *Pointer<Float4>(data + a2c[q]));
		Int aMask = SignMask(coverage);
		cMask[q] &= aMask;
	}
}

void PixelRoutine::writeDepth32F(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask)
{
	Float4 Z = z;

	Pointer<Byte> buffer = zBuffer + 4 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Float4 zValue;

	if(state.depthCompareMode != VK_COMPARE_OP_NEVER || (state.depthCompareMode != VK_COMPARE_OP_ALWAYS && !state.depthWriteEnable))
	{
		zValue = Float4(*Pointer<Float2>(buffer), *Pointer<Float2>(buffer + pitch));
	}

	Z = As<Float4>(As<Int4>(Z) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X) + zMask * 16, 16));
	zValue = As<Float4>(As<Int4>(zValue) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X) + zMask * 16, 16));
	Z = As<Float4>(As<Int4>(Z) | As<Int4>(zValue));

	*Pointer<Float2>(buffer) = Float2(Z.xy);
	*Pointer<Float2>(buffer + pitch) = Float2(Z.zw);
}

void PixelRoutine::writeDepth16(Pointer<Byte> &zBuffer, int q, const Int &x, const Float4 &z, const Int &zMask)
{
	Short4 Z = As<Short4>(convertFixed16(z, true));

	Pointer<Byte> buffer = zBuffer + 2 * x;
	Int pitch = *Pointer<Int>(data + OFFSET(DrawData, depthPitchB));

	if(q > 0)
	{
		buffer += q * *Pointer<Int>(data + OFFSET(DrawData, depthSliceB));
	}

	Short4 zValue;

	if(state.depthCompareMode != VK_COMPARE_OP_NEVER || (state.depthCompareMode != VK_COMPARE_OP_ALWAYS && !state.depthWriteEnable))
	{
		zValue = As<Short4>(Insert(As<Int2>(zValue), *Pointer<Int>(buffer), 0));
		zValue = As<Short4>(Insert(As<Int2>(zValue), *Pointer<Int>(buffer + pitch), 1));
	}

	Z = Z & *Pointer<Short4>(constants + OFFSET(Constants, maskW4Q) + zMask * 8, 8);
	zValue = zValue & *Pointer<Short4>(constants + OFFSET(Constants, invMaskW4Q) + zMask * 8, 8);
	Z = Z | zValue;

	*Pointer<Int>(buffer) = Extract(As<Int2>(Z), 0);
	*Pointer<Int>(buffer + pitch) = Extract(As<Int2>(Z), 1);
}

void PixelRoutine::writeDepth(Pointer<Byte> &zBuffer, const Int &x, const Int zMask[4], const SampleSet &samples)
{
	if(!state.depthWriteEnable)
	{
		return;
	}

	for(unsigned int q : samples)
	{
		switch(state.depthFormat)
		{
		case VK_FORMAT_D16_UNORM:
			writeDepth16(zBuffer, q, x, z[q], zMask[q]);
			break;
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			writeDepth32F(zBuffer, q, x, z[q], zMask[q]);
			break;
		default:
			UNSUPPORTED("Depth format: %d", int(state.depthFormat));
			break;
		}
	}
}

void PixelRoutine::occlusionSampleCount(const Int zMask[4], const Int sMask[4], const SampleSet &samples)
{
	if(!state.occlusionEnabled)
	{
		return;
	}

	for(unsigned int q : samples)
	{
		occlusion += *Pointer<UInt>(constants + OFFSET(Constants, occlusionCount) + 4 * (zMask[q] & sMask[q]));
	}
}

void PixelRoutine::writeStencil(Pointer<Byte> &sBuffer, const Int &x, const Int sMask[4], const Int zMask[4], const Int cMask[4], const SampleSet &samples)
{
	if(!state.stencilActive)
	{
		return;
	}

	if(state.frontStencil.passOp == VK_STENCIL_OP_KEEP && state.frontStencil.depthFailOp == VK_STENCIL_OP_KEEP && state.frontStencil.failOp == VK_STENCIL_OP_KEEP)
	{
		if(state.backStencil.passOp == VK_STENCIL_OP_KEEP && state.backStencil.depthFailOp == VK_STENCIL_OP_KEEP && state.backStencil.failOp == VK_STENCIL_OP_KEEP)
		{
			return;
		}
	}

	if((state.frontStencil.writeMask == 0) && (state.backStencil.writeMask == 0))
	{
		return;
	}

	for(unsigned int q : samples)
	{
		Pointer<Byte> buffer = sBuffer + x;

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(data + OFFSET(DrawData, stencilSliceB));
		}

		Int pitch = *Pointer<Int>(data + OFFSET(DrawData, stencilPitchB));
		Byte8 bufferValue = *Pointer<Byte8>(buffer) & Byte8(-1, -1, 0, 0, 0, 0, 0, 0);
		bufferValue = bufferValue | (*Pointer<Byte8>(buffer + pitch - 2) & Byte8(0, 0, -1, -1, 0, 0, 0, 0));
		Byte8 newValue;
		stencilOperation(newValue, bufferValue, state.frontStencil, false, zMask[q], sMask[q]);

		if((state.frontStencil.writeMask & 0xFF) != 0xFF)  // Assume 8-bit stencil buffer
		{
			Byte8 maskedValue = bufferValue;
			newValue &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[0].writeMaskQ));
			maskedValue &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[0].invWriteMaskQ));
			newValue |= maskedValue;
		}

		Byte8 newValueBack;

		stencilOperation(newValueBack, bufferValue, state.backStencil, true, zMask[q], sMask[q]);

		if((state.backStencil.writeMask & 0xFF) != 0xFF)  // Assume 8-bit stencil buffer
		{
			Byte8 maskedValue = bufferValue;
			newValueBack &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[1].writeMaskQ));
			maskedValue &= *Pointer<Byte8>(data + OFFSET(DrawData, stencil[1].invWriteMaskQ));
			newValueBack |= maskedValue;
		}

		newValue &= *Pointer<Byte8>(primitive + OFFSET(Primitive, clockwiseMask));
		newValueBack &= *Pointer<Byte8>(primitive + OFFSET(Primitive, invClockwiseMask));
		newValue |= newValueBack;

		newValue &= *Pointer<Byte8>(constants + OFFSET(Constants, maskB4Q) + 8 * cMask[q]);
		bufferValue &= *Pointer<Byte8>(constants + OFFSET(Constants, invMaskB4Q) + 8 * cMask[q]);
		newValue |= bufferValue;

		*Pointer<Short>(buffer) = Extract(As<Short4>(newValue), 0);
		*Pointer<Short>(buffer + pitch) = Extract(As<Short4>(newValue), 1);
	}
}

void PixelRoutine::stencilOperation(Byte8 &newValue, const Byte8 &bufferValue, const PixelProcessor::States::StencilOpState &ops, bool isBack, const Int &zMask, const Int &sMask)
{
	Byte8 &pass = newValue;
	Byte8 fail;
	Byte8 zFail;

	stencilOperation(pass, bufferValue, ops.passOp, isBack);

	if(ops.depthFailOp != ops.passOp)
	{
		stencilOperation(zFail, bufferValue, ops.depthFailOp, isBack);
	}

	if(ops.failOp != ops.passOp || ops.failOp != ops.depthFailOp)
	{
		stencilOperation(fail, bufferValue, ops.failOp, isBack);
	}

	if(ops.failOp != ops.passOp || ops.failOp != ops.depthFailOp)
	{
		if(state.depthTestActive && ops.depthFailOp != ops.passOp)  // zMask valid and values not the same
		{
			pass &= *Pointer<Byte8>(constants + OFFSET(Constants, maskB4Q) + 8 * zMask);
			zFail &= *Pointer<Byte8>(constants + OFFSET(Constants, invMaskB4Q) + 8 * zMask);
			pass |= zFail;
		}

		pass &= *Pointer<Byte8>(constants + OFFSET(Constants, maskB4Q) + 8 * sMask);
		fail &= *Pointer<Byte8>(constants + OFFSET(Constants, invMaskB4Q) + 8 * sMask);
		pass |= fail;
	}
}

Byte8 PixelRoutine::stencilReplaceRef(bool isBack)
{
	if(spirvShader)
	{
		auto it = spirvShader->outputBuiltins.find(spv::BuiltInFragStencilRefEXT);
		if(it != spirvShader->outputBuiltins.end())
		{
			UInt4 sRef = As<UInt4>(routine.getVariable(it->second.Id)[it->second.FirstComponent]) & UInt4(0xff);
			// TODO (b/148295813): Could be done with a single pshufb instruction. Optimize the
			//                     following line by either adding a rr::Shuffle() variant to do
			//                     it explicitly or adding a Byte4(Int4) constructor would work.
			sRef.x = rr::UInt(sRef.x) | (rr::UInt(sRef.y) << 8) | (rr::UInt(sRef.z) << 16) | (rr::UInt(sRef.w) << 24);

			UInt2 sRefDuplicated;
			sRefDuplicated = Insert(sRefDuplicated, sRef.x, 0);
			sRefDuplicated = Insert(sRefDuplicated, sRef.x, 1);
			return As<Byte8>(sRefDuplicated);
		}
	}

	return *Pointer<Byte8>(data + OFFSET(DrawData, stencil[isBack].referenceQ));
}

void PixelRoutine::stencilOperation(Byte8 &output, const Byte8 &bufferValue, VkStencilOp operation, bool isBack)
{
	switch(operation)
	{
	case VK_STENCIL_OP_KEEP:
		output = bufferValue;
		break;
	case VK_STENCIL_OP_ZERO:
		output = Byte8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
		break;
	case VK_STENCIL_OP_REPLACE:
		output = stencilReplaceRef(isBack);
		break;
	case VK_STENCIL_OP_INCREMENT_AND_CLAMP:
		output = AddSat(bufferValue, Byte8(1, 1, 1, 1, 1, 1, 1, 1));
		break;
	case VK_STENCIL_OP_DECREMENT_AND_CLAMP:
		output = SubSat(bufferValue, Byte8(1, 1, 1, 1, 1, 1, 1, 1));
		break;
	case VK_STENCIL_OP_INVERT:
		output = bufferValue ^ Byte8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
		break;
	case VK_STENCIL_OP_INCREMENT_AND_WRAP:
		output = bufferValue + Byte8(1, 1, 1, 1, 1, 1, 1, 1);
		break;
	case VK_STENCIL_OP_DECREMENT_AND_WRAP:
		output = bufferValue - Byte8(1, 1, 1, 1, 1, 1, 1, 1);
		break;
	default:
		UNSUPPORTED("VkStencilOp: %d", int(operation));
	}
}

bool PixelRoutine::isSRGB(int index) const
{
	return vk::Format(state.colorFormat[index]).isSRGBformat();
}

void PixelRoutine::readPixel(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &pixel)
{
	Short4 c01;
	Short4 c23;
	Pointer<Byte> buffer = cBuffer;
	Pointer<Byte> buffer2;

	Int pitchB = *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));

	switch(state.colorFormat[index])
	{
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.x = (c01 & Short4(0xF000u));
		pixel.y = (c01 & Short4(0x0F00u)) << 4;
		pixel.z = (c01 & Short4(0x00F0u)) << 8;
		pixel.w = (c01 & Short4(0x000Fu)) << 12;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 4);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 8);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 4);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 8);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 4);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 8);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.z = (c01 & Short4(0xF000u));
		pixel.y = (c01 & Short4(0x0F00u)) << 4;
		pixel.x = (c01 & Short4(0x00F0u)) << 8;
		pixel.w = (c01 & Short4(0x000Fu)) << 12;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 4);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 8);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 4);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 8);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 4);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 8);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		break;
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.w = (c01 & Short4(0xF000u));
		pixel.z = (c01 & Short4(0x0F00u)) << 4;
		pixel.y = (c01 & Short4(0x00F0u)) << 8;
		pixel.x = (c01 & Short4(0x000Fu)) << 12;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 4);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 8);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 4);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 8);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 4);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 8);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		break;
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.w = (c01 & Short4(0xF000u));
		pixel.x = (c01 & Short4(0x0F00u)) << 4;
		pixel.y = (c01 & Short4(0x00F0u)) << 8;
		pixel.z = (c01 & Short4(0x000Fu)) << 12;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 4);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 8);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 4);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 8);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 4);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 8);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
		pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.x = (c01 & Short4(0xF800u));
		pixel.y = (c01 & Short4(0x07C0u)) << 5;
		pixel.z = (c01 & Short4(0x003Eu)) << 10;
		pixel.w = ((c01 & Short4(0x0001u)) << 15) >> 15;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 5);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 5);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 10);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 5);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.z = (c01 & Short4(0xF800u));
		pixel.y = (c01 & Short4(0x07C0u)) << 5;
		pixel.x = (c01 & Short4(0x003Eu)) << 10;
		pixel.w = ((c01 & Short4(0x0001u)) << 15) >> 15;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 5);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 5);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 10);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 5);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.x = (c01 & Short4(0x7C00u)) << 1;
		pixel.y = (c01 & Short4(0x03E0u)) << 6;
		pixel.z = (c01 & Short4(0x001Fu)) << 11;
		pixel.w = (c01 & Short4(0x8000u)) >> 15;

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 5);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 5);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 10);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 5);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.x = c01 & Short4(0xF800u);
		pixel.y = (c01 & Short4(0x07E0u)) << 5;
		pixel.z = (c01 & Short4(0x001Fu)) << 11;
		pixel.w = Short4(0xFFFFu);

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 5);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 6);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 12);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 5);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		buffer += 2 * x;
		buffer2 = buffer + pitchB;
		c01 = As<Short4>(Int2(*Pointer<Int>(buffer), *Pointer<Int>(buffer2)));

		pixel.z = c01 & Short4(0xF800u);
		pixel.y = (c01 & Short4(0x07E0u)) << 5;
		pixel.x = (c01 & Short4(0x001Fu)) << 11;
		pixel.w = Short4(0xFFFFu);

		// Expand to 16 bit range
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 5);
		pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 6);
		pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 12);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 5);
		pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
		buffer += 4 * x;
		c01 = *Pointer<Short4>(buffer);
		buffer += pitchB;
		c23 = *Pointer<Short4>(buffer);
		pixel.z = c01;
		pixel.y = c01;
		pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(c23));
		pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(c23));
		pixel.x = pixel.z;
		pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.y));
		pixel.x = UnpackHigh(As<Byte8>(pixel.x), As<Byte8>(pixel.y));
		pixel.y = pixel.z;
		pixel.w = pixel.x;
		pixel.x = UnpackLow(As<Byte8>(pixel.x), As<Byte8>(pixel.x));
		pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(pixel.y));
		pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.z));
		pixel.w = UnpackHigh(As<Byte8>(pixel.w), As<Byte8>(pixel.w));
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SRGB:
		buffer += 4 * x;
		c01 = *Pointer<Short4>(buffer);
		buffer += pitchB;
		c23 = *Pointer<Short4>(buffer);
		pixel.z = c01;
		pixel.y = c01;
		pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(c23));
		pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(c23));
		pixel.x = pixel.z;
		pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.y));
		pixel.x = UnpackHigh(As<Byte8>(pixel.x), As<Byte8>(pixel.y));
		pixel.y = pixel.z;
		pixel.w = pixel.x;
		pixel.x = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.z));
		pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(pixel.y));
		pixel.z = UnpackLow(As<Byte8>(pixel.w), As<Byte8>(pixel.w));
		pixel.w = UnpackHigh(As<Byte8>(pixel.w), As<Byte8>(pixel.w));
		break;
	case VK_FORMAT_R8_UNORM:
		buffer += 1 * x;
		pixel.x = Insert(pixel.x, *Pointer<Short>(buffer), 0);
		buffer += pitchB;
		pixel.x = Insert(pixel.x, *Pointer<Short>(buffer), 1);
		pixel.x = UnpackLow(As<Byte8>(pixel.x), As<Byte8>(pixel.x));
		pixel.y = Short4(0x0000);
		pixel.z = Short4(0x0000);
		pixel.w = Short4(0xFFFFu);
		break;
	case VK_FORMAT_R8G8_UNORM:
		buffer += 2 * x;
		c01 = As<Short4>(Insert(As<Int2>(c01), *Pointer<Int>(buffer), 0));
		buffer += pitchB;
		c01 = As<Short4>(Insert(As<Int2>(c01), *Pointer<Int>(buffer), 1));
		pixel.x = (c01 & Short4(0x00FFu)) | (c01 << 8);
		pixel.y = (c01 & Short4(0xFF00u)) | As<Short4>(As<UShort4>(c01) >> 8);
		pixel.z = Short4(0x0000u);
		pixel.w = Short4(0xFFFFu);
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		{
			Int4 v = Int4(0);
			buffer += 4 * x;
			v = Insert(v, *Pointer<Int>(buffer + 0), 0);
			v = Insert(v, *Pointer<Int>(buffer + 4), 1);
			buffer += pitchB;
			v = Insert(v, *Pointer<Int>(buffer + 0), 2);
			v = Insert(v, *Pointer<Int>(buffer + 4), 3);

			pixel.x = Short4(v << 6) & Short4(0xFFC0u);
			pixel.y = Short4(v >> 4) & Short4(0xFFC0u);
			pixel.z = Short4(v >> 14) & Short4(0xFFC0u);
			pixel.w = Short4(v >> 16) & Short4(0xC000u);

			// Expand to 16 bit range
			pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
			pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 10);
			pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 2);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		}
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		{
			Int4 v = Int4(0);
			v = Insert(v, *Pointer<Int>(buffer + 4 * x), 0);
			v = Insert(v, *Pointer<Int>(buffer + 4 * x + 4), 1);
			buffer += *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));
			v = Insert(v, *Pointer<Int>(buffer + 4 * x), 2);
			v = Insert(v, *Pointer<Int>(buffer + 4 * x + 4), 3);

			pixel.x = Short4(v >> 14) & Short4(0xFFC0u);
			pixel.y = Short4(v >> 4) & Short4(0xFFC0u);
			pixel.z = Short4(v << 6) & Short4(0xFFC0u);
			pixel.w = Short4(v >> 16) & Short4(0xC000u);

			// Expand to 16 bit range
			pixel.x |= As<Short4>(As<UShort4>(pixel.x) >> 10);
			pixel.y |= As<Short4>(As<UShort4>(pixel.y) >> 10);
			pixel.z |= As<Short4>(As<UShort4>(pixel.z) >> 10);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 2);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 4);
			pixel.w |= As<Short4>(As<UShort4>(pixel.w) >> 8);
		}
		break;
	default:
		UNSUPPORTED("VkFormat %d", int(state.colorFormat[index]));
	}

	if(isSRGB(index))
	{
		sRGBtoLinear16_12_16(pixel);
	}
}

void PixelRoutine::writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4s &current, const Int &sMask, const Int &zMask, const Int &cMask)
{
	if(isSRGB(index))
	{
		linearToSRGB16_12_16(current);
	}

	switch(state.colorFormat[index])
	{
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SRGB:
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8_UNORM:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 8) + Short4(0x0080);
		current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 8) + Short4(0x0080);
		current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 8) + Short4(0x0080);
		current.w = current.w - As<Short4>(As<UShort4>(current.w) >> 8) + Short4(0x0080);
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 10) + Short4(0x0020);
		current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 10) + Short4(0x0020);
		current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 10) + Short4(0x0020);
		current.w = current.w - As<Short4>(As<UShort4>(current.w) >> 2) + Short4(0x2000);
		break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
		current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 4) + Short4(0x0800);
		current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 4) + Short4(0x0800);
		current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 4) + Short4(0x0800);
		current.w = current.w - As<Short4>(As<UShort4>(current.w) >> 4) + Short4(0x0800);
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 5) + Short4(0x0400);
		current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 5) + Short4(0x0400);
		current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 5) + Short4(0x0400);
		current.w = current.w - As<Short4>(As<UShort4>(current.w) >> 1) + Short4(0x4000);
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 5) + Short4(0x0400);
		current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 6) + Short4(0x0200);
		current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 5) + Short4(0x0400);
		break;
	default:
		break;
	}

	int rgbaWriteMask = state.colorWriteActive(index);
	int bgraWriteMask = (rgbaWriteMask & 0x0000000A) | (rgbaWriteMask & 0x00000001) << 2 | (rgbaWriteMask & 0x00000004) >> 2;

	switch(state.colorFormat[index])
	{
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		{
			current.x = As<UShort4>(current.x & Short4(0xF000));
			current.y = As<UShort4>(current.y & Short4(0xF000)) >> 4;
			current.z = As<UShort4>(current.z & Short4(0xF000)) >> 8;
			current.w = As<UShort4>(current.w & Short4(0xF000u)) >> 12;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		{
			current.z = As<UShort4>(current.z & Short4(0xF000));
			current.y = As<UShort4>(current.y & Short4(0xF000)) >> 4;
			current.x = As<UShort4>(current.x & Short4(0xF000)) >> 8;
			current.w = As<UShort4>(current.w & Short4(0xF000u)) >> 12;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
		{
			current.w = As<UShort4>(current.w & Short4(0xF000));
			current.x = As<UShort4>(current.x & Short4(0xF000)) >> 4;
			current.y = As<UShort4>(current.y & Short4(0xF000)) >> 8;
			current.z = As<UShort4>(current.z & Short4(0xF000u)) >> 12;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
		{
			current.w = As<UShort4>(current.w & Short4(0xF000));
			current.z = As<UShort4>(current.z & Short4(0xF000)) >> 4;
			current.y = As<UShort4>(current.y & Short4(0xF000)) >> 8;
			current.x = As<UShort4>(current.x & Short4(0xF000u)) >> 12;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		{
			current.x = As<UShort4>(current.x & Short4(0xF800));
			current.y = As<UShort4>(current.y & Short4(0xF800)) >> 5;
			current.z = As<UShort4>(current.z & Short4(0xF800)) >> 10;
			current.w = As<UShort4>(current.w & Short4(0x8000u)) >> 15;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		{
			current.z = As<UShort4>(current.z & Short4(0xF800));
			current.y = As<UShort4>(current.y & Short4(0xF800)) >> 5;
			current.x = As<UShort4>(current.x & Short4(0xF800)) >> 10;
			current.w = As<UShort4>(current.w & Short4(0x8000u)) >> 15;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		{
			current.w = current.w & Short4(0x8000u);
			current.x = As<UShort4>(current.x & Short4(0xF800)) >> 1;
			current.y = As<UShort4>(current.y & Short4(0xF800)) >> 6;
			current.z = As<UShort4>(current.z & Short4(0xF800)) >> 11;

			current.x = current.x | current.y | current.z | current.w;
		}
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		{
			current.x = current.x & Short4(0xF800u);
			current.y = As<UShort4>(current.y & Short4(0xFC00u)) >> 5;
			current.z = As<UShort4>(current.z) >> 11;

			current.x = current.x | current.y | current.z;
		}
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		{
			current.z = current.z & Short4(0xF800u);
			current.y = As<UShort4>(current.y & Short4(0xFC00u)) >> 5;
			current.x = As<UShort4>(current.x) >> 11;

			current.x = current.x | current.y | current.z;
		}
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
		if(rgbaWriteMask == 0x7)
		{
			current.x = As<Short4>(As<UShort4>(current.x) >> 8);
			current.y = As<Short4>(As<UShort4>(current.y) >> 8);
			current.z = As<Short4>(As<UShort4>(current.z) >> 8);

			current.z = As<Short4>(PackUnsigned(current.z, current.x));
			current.y = As<Short4>(PackUnsigned(current.y, current.y));

			current.x = current.z;
			current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
			current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
			current.y = current.z;
			current.z = As<Short4>(UnpackLow(current.z, current.x));
			current.y = As<Short4>(UnpackHigh(current.y, current.x));
		}
		else
		{
			current.x = As<Short4>(As<UShort4>(current.x) >> 8);
			current.y = As<Short4>(As<UShort4>(current.y) >> 8);
			current.z = As<Short4>(As<UShort4>(current.z) >> 8);
			current.w = As<Short4>(As<UShort4>(current.w) >> 8);

			current.z = As<Short4>(PackUnsigned(current.z, current.x));
			current.y = As<Short4>(PackUnsigned(current.y, current.w));

			current.x = current.z;
			current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
			current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
			current.y = current.z;
			current.z = As<Short4>(UnpackLow(current.z, current.x));
			current.y = As<Short4>(UnpackHigh(current.y, current.x));
		}
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SRGB:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		if(rgbaWriteMask == 0x7)
		{
			current.x = As<Short4>(As<UShort4>(current.x) >> 8);
			current.y = As<Short4>(As<UShort4>(current.y) >> 8);
			current.z = As<Short4>(As<UShort4>(current.z) >> 8);

			current.z = As<Short4>(PackUnsigned(current.x, current.z));
			current.y = As<Short4>(PackUnsigned(current.y, current.y));

			current.x = current.z;
			current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
			current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
			current.y = current.z;
			current.z = As<Short4>(UnpackLow(current.z, current.x));
			current.y = As<Short4>(UnpackHigh(current.y, current.x));
		}
		else
		{
			current.x = As<Short4>(As<UShort4>(current.x) >> 8);
			current.y = As<Short4>(As<UShort4>(current.y) >> 8);
			current.z = As<Short4>(As<UShort4>(current.z) >> 8);
			current.w = As<Short4>(As<UShort4>(current.w) >> 8);

			current.z = As<Short4>(PackUnsigned(current.x, current.z));
			current.y = As<Short4>(PackUnsigned(current.y, current.w));

			current.x = current.z;
			current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
			current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
			current.y = current.z;
			current.z = As<Short4>(UnpackLow(current.z, current.x));
			current.y = As<Short4>(UnpackHigh(current.y, current.x));
		}
		break;
	case VK_FORMAT_R8G8_UNORM:
		current.x = As<Short4>(As<UShort4>(current.x) >> 8);
		current.y = As<Short4>(As<UShort4>(current.y) >> 8);
		current.x = As<Short4>(PackUnsigned(current.x, current.x));
		current.y = As<Short4>(PackUnsigned(current.y, current.y));
		current.x = UnpackLow(As<Byte8>(current.x), As<Byte8>(current.y));
		break;
	case VK_FORMAT_R8_UNORM:
		current.x = As<Short4>(As<UShort4>(current.x) >> 8);
		current.x = As<Short4>(PackUnsigned(current.x, current.x));
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		{
			auto r = (Int4(current.x) >> 6) & Int4(0x3ff);
			auto g = (Int4(current.y) >> 6) & Int4(0x3ff);
			auto b = (Int4(current.z) >> 6) & Int4(0x3ff);
			auto a = (Int4(current.w) >> 14) & Int4(0x3);
			Int4 packed = (a << 30) | (b << 20) | (g << 10) | r;
			auto c02 = As<Int2>(Int4(packed.xzzz));  // TODO: auto c02 = packed.xz;
			auto c13 = As<Int2>(Int4(packed.ywww));  // TODO: auto c13 = packed.yw;
			current.x = UnpackLow(c02, c13);
			current.y = UnpackHigh(c02, c13);
		}
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		{
			auto r = (Int4(current.x) >> 6) & Int4(0x3ff);
			auto g = (Int4(current.y) >> 6) & Int4(0x3ff);
			auto b = (Int4(current.z) >> 6) & Int4(0x3ff);
			auto a = (Int4(current.w) >> 14) & Int4(0x3);
			Int4 packed = (a << 30) | (r << 20) | (g << 10) | b;
			auto c02 = As<Int2>(Int4(packed.xzzz));  // TODO: auto c02 = packed.xz;
			auto c13 = As<Int2>(Int4(packed.ywww));  // TODO: auto c13 = packed.yw;
			current.x = UnpackLow(c02, c13);
			current.y = UnpackHigh(c02, c13);
		}
		break;
	default:
		UNSUPPORTED("VkFormat: %d", int(state.colorFormat[index]));
	}

	Short4 c01 = current.z;
	Short4 c23 = current.y;

	Int xMask;  // Combination of all masks

	if(state.depthTestActive)
	{
		xMask = zMask;
	}
	else
	{
		xMask = cMask;
	}

	if(state.stencilActive)
	{
		xMask &= sMask;
	}

	Pointer<Byte> buffer = cBuffer;
	Int pitchB = *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));

	switch(state.colorFormat[index])
	{
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
		{
			buffer += 2 * x;
			Int value = *Pointer<Int>(buffer);

			Int channelMask;
			switch(state.colorFormat[index])
			{
			case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
				channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask4rgbaQ[bgraWriteMask & 0xF][0]));
				break;
			case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
				channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask4bgraQ[bgraWriteMask & 0xF][0]));
				break;
			case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
				channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask4argbQ[bgraWriteMask & 0xF][0]));
				break;
			case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
				channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask4abgrQ[bgraWriteMask & 0xF][0]));
				break;
			default:
				UNREACHABLE("Format: %s", vk::Stringify(state.colorFormat[index]).c_str());
			}

			Int c01 = Extract(As<Int2>(current.x), 0);
			Int mask01 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][0]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Int>(buffer);

			Int c23 = Extract(As<Int2>(current.x), 1);
			Int mask23 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][2]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		{
			buffer += 2 * x;
			Int value = *Pointer<Int>(buffer);

			Int channelMask = *Pointer<Int>(constants + OFFSET(Constants, maskr5g5b5a1Q[bgraWriteMask & 0xF][0]));

			Int c01 = Extract(As<Int2>(current.x), 0);
			Int mask01 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][0]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Int>(buffer);

			Int c23 = Extract(As<Int2>(current.x), 1);
			Int mask23 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][2]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		{
			buffer += 2 * x;
			Int value = *Pointer<Int>(buffer);

			Int channelMask = *Pointer<Int>(constants + OFFSET(Constants, maskb5g5r5a1Q[bgraWriteMask & 0xF][0]));

			Int c01 = Extract(As<Int2>(current.x), 0);
			Int mask01 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][0]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Int>(buffer);

			Int c23 = Extract(As<Int2>(current.x), 1);
			Int mask23 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][2]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		{
			buffer += 2 * x;
			Int value = *Pointer<Int>(buffer);

			Int channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask5551Q[bgraWriteMask & 0xF][0]));

			Int c01 = Extract(As<Int2>(current.x), 0);
			Int mask01 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][0]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Int>(buffer);

			Int c23 = Extract(As<Int2>(current.x), 1);
			Int mask23 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][2]) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		{
			buffer += 2 * x;
			Int value = *Pointer<Int>(buffer);

			Int channelMask = *Pointer<Int>(constants + OFFSET(Constants, mask565Q[bgraWriteMask & 0x7][0]));

			Int c01 = Extract(As<Int2>(current.x), 0);
			Int mask01 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][0]) + xMask * 8);
			if((bgraWriteMask & 0x00000007) != 0x00000007)
			{
				mask01 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Int>(buffer);

			Int c23 = Extract(As<Int2>(current.x), 1);
			Int mask23 = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[0][2]) + xMask * 8);
			if((bgraWriteMask & 0x00000007) != 0x00000007)
			{
				mask23 &= channelMask;
			}
			*Pointer<Int>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
		{
			buffer += x * 4;
			Short4 value = *Pointer<Short4>(buffer);
			Short4 channelMask = *Pointer<Short4>(constants + OFFSET(Constants, maskB4Q[bgraWriteMask][0]));

			Short4 mask01 = *Pointer<Short4>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Short4>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Short4>(buffer);

			Short4 mask23 = *Pointer<Short4>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(bgraWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Short4>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SRGB:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		{
			buffer += x * 4;
			Short4 value = *Pointer<Short4>(buffer);
			Short4 channelMask = *Pointer<Short4>(constants + OFFSET(Constants, maskB4Q[rgbaWriteMask][0]));

			Short4 mask01 = *Pointer<Short4>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(rgbaWriteMask != 0x0000000F)
			{
				mask01 &= channelMask;
			}
			*Pointer<Short4>(buffer) = (c01 & mask01) | (value & ~mask01);

			buffer += pitchB;
			value = *Pointer<Short4>(buffer);

			Short4 mask23 = *Pointer<Short4>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(rgbaWriteMask != 0x0000000F)
			{
				mask23 &= channelMask;
			}
			*Pointer<Short4>(buffer) = (c23 & mask23) | (value & ~mask23);
		}
		break;
	case VK_FORMAT_R8G8_UNORM:
		if((rgbaWriteMask & 0x00000003) != 0x0)
		{
			buffer += 2 * x;
			Int2 value;
			value = Insert(value, *Pointer<Int>(buffer), 0);
			value = Insert(value, *Pointer<Int>(buffer + pitchB), 1);

			Int2 packedCol = As<Int2>(current.x);

			UInt2 mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskW4Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				Int tmpMask = *Pointer<Int>(constants + OFFSET(Constants, maskB4Q[5 * (rgbaWriteMask & 0x3)][0]));
				UInt2 rgbaMask = As<UInt2>(Int2(tmpMask, tmpMask));
				mergedMask &= rgbaMask;
			}

			packedCol = As<Int2>((As<UInt2>(packedCol) & mergedMask) | (As<UInt2>(value) & ~mergedMask));

			*Pointer<UInt>(buffer) = As<UInt>(Extract(packedCol, 0));
			*Pointer<UInt>(buffer + pitchB) = As<UInt>(Extract(packedCol, 1));
		}
		break;
	case VK_FORMAT_R8_UNORM:
		if(rgbaWriteMask & 0x00000001)
		{
			buffer += 1 * x;
			Short4 value;
			value = Insert(value, *Pointer<Short>(buffer), 0);
			value = Insert(value, *Pointer<Short>(buffer + pitchB), 1);

			current.x &= *Pointer<Short4>(constants + OFFSET(Constants, maskB4Q) + 8 * xMask);
			value &= *Pointer<Short4>(constants + OFFSET(Constants, invMaskB4Q) + 8 * xMask);
			current.x |= value;

			*Pointer<Short>(buffer) = Extract(current.x, 0);
			*Pointer<Short>(buffer + pitchB) = Extract(current.x, 1);
		}
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		rgbaWriteMask = bgraWriteMask;
		// [[fallthrough]]
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		{
			buffer += 4 * x;

			Int2 value = *Pointer<Int2>(buffer, 16);
			Int2 mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[rgbaWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(current.x) & mergedMask) | (value & ~mergedMask);

			buffer += pitchB;

			value = *Pointer<Int2>(buffer, 16);
			mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[rgbaWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(current.y) & mergedMask) | (value & ~mergedMask);
		}
		break;
	default:
		UNSUPPORTED("VkFormat: %d", int(state.colorFormat[index]));
	}
}

Float PixelRoutine::blendConstant(vk::Format format, int component, BlendFactorModifier modifier)
{
	bool inverse = (modifier == OneMinus);

	if(format.isUnsignedNormalized())
	{
		return inverse ? *Pointer<Float>(data + OFFSET(DrawData, factor.invBlendConstantU[component]))
		               : *Pointer<Float>(data + OFFSET(DrawData, factor.blendConstantU[component]));
	}
	else if(format.isSignedNormalized())
	{
		return inverse ? *Pointer<Float>(data + OFFSET(DrawData, factor.invBlendConstantS[component]))
		               : *Pointer<Float>(data + OFFSET(DrawData, factor.blendConstantS[component]));
	}
	else  // Floating-point format
	{
		ASSERT(format.isFloatFormat());
		return inverse ? *Pointer<Float>(data + OFFSET(DrawData, factor.invBlendConstantF[component]))
		               : *Pointer<Float>(data + OFFSET(DrawData, factor.blendConstantF[component]));
	}
}

void PixelRoutine::blendFactorRGB(Vector4f &blendFactor, const Vector4f &sourceColor, const Vector4f &destColor, VkBlendFactor colorBlendFactor, vk::Format format)
{
	switch(colorBlendFactor)
	{
	case VK_BLEND_FACTOR_ZERO:
		blendFactor.x = Float4(0);
		blendFactor.y = Float4(0);
		blendFactor.z = Float4(0);
		break;
	case VK_BLEND_FACTOR_ONE:
		blendFactor.x = Float4(1);
		blendFactor.y = Float4(1);
		blendFactor.z = Float4(1);
		break;
	case VK_BLEND_FACTOR_SRC_COLOR:
		blendFactor.x = sourceColor.x;
		blendFactor.y = sourceColor.y;
		blendFactor.z = sourceColor.z;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
		blendFactor.x = Float4(1.0f) - sourceColor.x;
		blendFactor.y = Float4(1.0f) - sourceColor.y;
		blendFactor.z = Float4(1.0f) - sourceColor.z;
		break;
	case VK_BLEND_FACTOR_DST_COLOR:
		blendFactor.x = destColor.x;
		blendFactor.y = destColor.y;
		blendFactor.z = destColor.z;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
		blendFactor.x = Float4(1.0f) - destColor.x;
		blendFactor.y = Float4(1.0f) - destColor.y;
		blendFactor.z = Float4(1.0f) - destColor.z;
		break;
	case VK_BLEND_FACTOR_SRC_ALPHA:
		blendFactor.x = sourceColor.w;
		blendFactor.y = sourceColor.w;
		blendFactor.z = sourceColor.w;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		blendFactor.x = Float4(1.0f) - sourceColor.w;
		blendFactor.y = Float4(1.0f) - sourceColor.w;
		blendFactor.z = Float4(1.0f) - sourceColor.w;
		break;
	case VK_BLEND_FACTOR_DST_ALPHA:
		blendFactor.x = destColor.w;
		blendFactor.y = destColor.w;
		blendFactor.z = destColor.w;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		blendFactor.x = Float4(1.0f) - destColor.w;
		blendFactor.y = Float4(1.0f) - destColor.w;
		blendFactor.z = Float4(1.0f) - destColor.w;
		break;
	case VK_BLEND_FACTOR_SRC_ALPHA_SATURATE:
		blendFactor.x = Float4(1.0f) - destColor.w;
		blendFactor.x = Min(blendFactor.x, sourceColor.w);
		blendFactor.y = blendFactor.x;
		blendFactor.z = blendFactor.x;
		break;
	case VK_BLEND_FACTOR_CONSTANT_COLOR:
		blendFactor.x = Float4(blendConstant(format, 0));
		blendFactor.y = Float4(blendConstant(format, 1));
		blendFactor.z = Float4(blendConstant(format, 2));
		break;
	case VK_BLEND_FACTOR_CONSTANT_ALPHA:
		blendFactor.x = Float4(blendConstant(format, 3));
		blendFactor.y = Float4(blendConstant(format, 3));
		blendFactor.z = Float4(blendConstant(format, 3));
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
		blendFactor.x = Float4(blendConstant(format, 0, OneMinus));
		blendFactor.y = Float4(blendConstant(format, 1, OneMinus));
		blendFactor.z = Float4(blendConstant(format, 2, OneMinus));
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
		blendFactor.x = Float4(blendConstant(format, 3, OneMinus));
		blendFactor.y = Float4(blendConstant(format, 3, OneMinus));
		blendFactor.z = Float4(blendConstant(format, 3, OneMinus));
		break;

	default:
		UNSUPPORTED("VkBlendFactor: %d", int(colorBlendFactor));
	}

	// "If the color attachment is fixed-point, the components of the source and destination values and blend factors are each clamped
	//  to [0,1] or [-1,1] respectively for an unsigned normalized or signed normalized color attachment prior to evaluating the blend
	//  operations. If the color attachment is floating-point, no clamping occurs."
	if(blendFactorCanExceedFormatRange(colorBlendFactor, format))
	{
		if(format.isUnsignedNormalized())
		{
			blendFactor.x = Min(Max(blendFactor.x, Float4(0.0f)), Float4(1.0f));
			blendFactor.y = Min(Max(blendFactor.y, Float4(0.0f)), Float4(1.0f));
			blendFactor.z = Min(Max(blendFactor.z, Float4(0.0f)), Float4(1.0f));
		}
		else if(format.isSignedNormalized())
		{
			blendFactor.x = Min(Max(blendFactor.x, Float4(-1.0f)), Float4(1.0f));
			blendFactor.y = Min(Max(blendFactor.y, Float4(-1.0f)), Float4(1.0f));
			blendFactor.z = Min(Max(blendFactor.z, Float4(-1.0f)), Float4(1.0f));
		}
	}
}

void PixelRoutine::blendFactorAlpha(Float4 &blendFactorAlpha, const Float4 &sourceAlpha, const Float4 &destAlpha, VkBlendFactor alphaBlendFactor, vk::Format format)
{
	switch(alphaBlendFactor)
	{
	case VK_BLEND_FACTOR_ZERO:
		blendFactorAlpha = Float4(0);
		break;
	case VK_BLEND_FACTOR_ONE:
		blendFactorAlpha = Float4(1);
		break;
	case VK_BLEND_FACTOR_SRC_COLOR:
		blendFactorAlpha = sourceAlpha;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
		blendFactorAlpha = Float4(1.0f) - sourceAlpha;
		break;
	case VK_BLEND_FACTOR_DST_COLOR:
		blendFactorAlpha = destAlpha;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
		blendFactorAlpha = Float4(1.0f) - destAlpha;
		break;
	case VK_BLEND_FACTOR_SRC_ALPHA:
		blendFactorAlpha = sourceAlpha;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		blendFactorAlpha = Float4(1.0f) - sourceAlpha;
		break;
	case VK_BLEND_FACTOR_DST_ALPHA:
		blendFactorAlpha = destAlpha;
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		blendFactorAlpha = Float4(1.0f) - destAlpha;
		break;
	case VK_BLEND_FACTOR_SRC_ALPHA_SATURATE:
		blendFactorAlpha = Float4(1.0f);
		break;
	case VK_BLEND_FACTOR_CONSTANT_COLOR:
	case VK_BLEND_FACTOR_CONSTANT_ALPHA:
		blendFactorAlpha = Float4(blendConstant(format, 3));
		break;
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
		blendFactorAlpha = Float4(blendConstant(format, 3, OneMinus));
		break;
	default:
		UNSUPPORTED("VkBlendFactor: %d", int(alphaBlendFactor));
	}

	// "If the color attachment is fixed-point, the components of the source and destination values and blend factors are each clamped
	//  to [0,1] or [-1,1] respectively for an unsigned normalized or signed normalized color attachment prior to evaluating the blend
	//  operations. If the color attachment is floating-point, no clamping occurs."
	if(blendFactorCanExceedFormatRange(alphaBlendFactor, format))
	{
		if(format.isUnsignedNormalized())
		{
			blendFactorAlpha = Min(Max(blendFactorAlpha, Float4(0.0f)), Float4(1.0f));
		}
		else if(format.isSignedNormalized())
		{
			blendFactorAlpha = Min(Max(blendFactorAlpha, Float4(-1.0f)), Float4(1.0f));
		}
	}
}

Float4 PixelRoutine::blendOpOverlay(Float4 &src, Float4 &dst)
{
	Int4 largeDst = CmpGT(dst, Float4(0.5f));
	return As<Float4>(
	    (~largeDst &
	     As<Int4>(Float4(2.0f) * src * dst)) |
	    (largeDst &
	     As<Int4>(Float4(1.0f) - (Float4(2.0f) * (Float4(1.0f) - src) * (Float4(1.0f) - dst)))));
}

Float4 PixelRoutine::blendOpColorDodge(Float4 &src, Float4 &dst)
{
	Int4 srcBelowOne = CmpLT(src, Float4(1.0f));
	Int4 positiveDst = CmpGT(dst, Float4(0.0f));
	return As<Float4>(positiveDst & ((~srcBelowOne &
	                                  As<Int4>(Float4(1.0f))) |
	                                 (srcBelowOne &
	                                  As<Int4>(Min(Float4(1.0f), (dst / (Float4(1.0f) - src)))))));
}

Float4 PixelRoutine::blendOpColorBurn(Float4 &src, Float4 &dst)
{
	Int4 dstBelowOne = CmpLT(dst, Float4(1.0f));
	Int4 positiveSrc = CmpGT(src, Float4(0.0f));
	return As<Float4>(
	    (~dstBelowOne &
	     As<Int4>(Float4(1.0f))) |
	    (dstBelowOne & positiveSrc &
	     As<Int4>(Float4(1.0f) - Min(Float4(1.0f), (Float4(1.0f) - dst) / src))));
}

Float4 PixelRoutine::blendOpHardlight(Float4 &src, Float4 &dst)
{
	Int4 largeSrc = CmpGT(src, Float4(0.5f));
	return As<Float4>(
	    (~largeSrc &
	     As<Int4>(Float4(2.0f) * src * dst)) |
	    (largeSrc &
	     As<Int4>(Float4(1.0f) - (Float4(2.0f) * (Float4(1.0f) - src) * (Float4(1.0f) - dst)))));
}

Float4 PixelRoutine::blendOpSoftlight(Float4 &src, Float4 &dst)
{
	Int4 largeSrc = CmpGT(src, Float4(0.5f));
	Int4 largeDst = CmpGT(dst, Float4(0.25f));

	return As<Float4>(
	    (~largeSrc &
	     As<Int4>(dst - ((Float4(1.0f) - (Float4(2.0f) * src)) * dst * (Float4(1.0f) - dst)))) |
	    (largeSrc & ((~largeDst &
	                  As<Int4>(dst + (((Float4(2.0f) * src) - Float4(1.0f)) * dst * ((((Float4(16.0f) * dst) - Float4(12.0f)) * dst) + Float4(3.0f))))) |
	                 (largeDst &
	                  As<Int4>(dst + (((Float4(2.0f) * src) - Float4(1.0f)) * (Sqrt(dst) - dst)))))));
}

Float4 PixelRoutine::maxRGB(Vector4f &c)
{
	return Max(Max(c.x, c.y), c.z);
}

Float4 PixelRoutine::minRGB(Vector4f &c)
{
	return Min(Min(c.x, c.y), c.z);
}

void PixelRoutine::setLumSat(Vector4f &cbase, Vector4f &csat, Vector4f &clum, Float4 &x, Float4 &y, Float4 &z)
{
	Float4 minbase = minRGB(cbase);
	Float4 sbase = maxRGB(cbase) - minbase;
	Float4 ssat = maxRGB(csat) - minRGB(csat);
	Int4 isNonZero = CmpGT(sbase, Float4(0.0f));
	Vector4f color;
	color.x = As<Float4>(isNonZero & As<Int4>((cbase.x - minbase) * ssat / sbase));
	color.y = As<Float4>(isNonZero & As<Int4>((cbase.y - minbase) * ssat / sbase));
	color.z = As<Float4>(isNonZero & As<Int4>((cbase.z - minbase) * ssat / sbase));
	setLum(color, clum, x, y, z);
}

Float4 PixelRoutine::lumRGB(Vector4f &c)
{
	return c.x * Float4(0.3f) + c.y * Float4(0.59f) + c.z * Float4(0.11f);
}

Float4 PixelRoutine::computeLum(Float4 &color, Float4 &lum, Float4 &mincol, Float4 &maxcol, Int4 &negative, Int4 &aboveOne)
{
	return As<Float4>(
	    (negative &
	     As<Int4>(lum + ((color - lum) * lum) / (lum - mincol))) |
	    (~negative &
	     ((aboveOne &
	       As<Int4>(lum + ((color - lum) * (Float4(1.0f) - lum)) / (Float4(maxcol) - lum))) |
	      (~aboveOne &
	       As<Int4>(color)))));
}

void PixelRoutine::setLum(Vector4f &cbase, Vector4f &clum, Float4 &x, Float4 &y, Float4 &z)
{
	Float4 lbase = lumRGB(cbase);
	Float4 llum = lumRGB(clum);
	Float4 ldiff = llum - lbase;

	Vector4f color;
	color.x = cbase.x + ldiff;
	color.y = cbase.y + ldiff;
	color.z = cbase.z + ldiff;

	Float4 lum = lumRGB(color);
	Float4 mincol = minRGB(color);
	Float4 maxcol = maxRGB(color);

	Int4 negative = CmpLT(mincol, Float4(0.0f));
	Int4 aboveOne = CmpGT(maxcol, Float4(1.0f));

	x = computeLum(color.x, lum, mincol, maxcol, negative, aboveOne);
	y = computeLum(color.y, lum, mincol, maxcol, negative, aboveOne);
	z = computeLum(color.z, lum, mincol, maxcol, negative, aboveOne);
}

void PixelRoutine::premultiply(Vector4f &c)
{
	Int4 nonZeroAlpha = CmpNEQ(c.w, Float4(0.0f));
	c.x = As<Float4>(nonZeroAlpha & As<Int4>(c.x / c.w));
	c.y = As<Float4>(nonZeroAlpha & As<Int4>(c.y / c.w));
	c.z = As<Float4>(nonZeroAlpha & As<Int4>(c.z / c.w));
}

Vector4f PixelRoutine::computeAdvancedBlendMode(int index, const Vector4f &src, const Vector4f &dst, const Vector4f &srcFactor, const Vector4f &dstFactor)
{
	Vector4f srcColor = src;
	srcColor.x *= srcFactor.x;
	srcColor.y *= srcFactor.y;
	srcColor.z *= srcFactor.z;
	srcColor.w *= srcFactor.w;

	Vector4f dstColor = dst;
	dstColor.x *= dstFactor.x;
	dstColor.y *= dstFactor.y;
	dstColor.z *= dstFactor.z;
	dstColor.w *= dstFactor.w;

	premultiply(srcColor);
	premultiply(dstColor);

	Vector4f blendedColor;

	switch(state.blendState[index].blendOperation)
	{
	case VK_BLEND_OP_MULTIPLY_EXT:
		blendedColor.x = (srcColor.x * dstColor.x);
		blendedColor.y = (srcColor.y * dstColor.y);
		blendedColor.z = (srcColor.z * dstColor.z);
		break;
	case VK_BLEND_OP_SCREEN_EXT:
		blendedColor.x = srcColor.x + dstColor.x - (srcColor.x * dstColor.x);
		blendedColor.y = srcColor.y + dstColor.y - (srcColor.y * dstColor.y);
		blendedColor.z = srcColor.z + dstColor.z - (srcColor.z * dstColor.z);
		break;
	case VK_BLEND_OP_OVERLAY_EXT:
		blendedColor.x = blendOpOverlay(srcColor.x, dstColor.x);
		blendedColor.y = blendOpOverlay(srcColor.y, dstColor.y);
		blendedColor.z = blendOpOverlay(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_DARKEN_EXT:
		blendedColor.x = Min(srcColor.x, dstColor.x);
		blendedColor.y = Min(srcColor.y, dstColor.y);
		blendedColor.z = Min(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_LIGHTEN_EXT:
		blendedColor.x = Max(srcColor.x, dstColor.x);
		blendedColor.y = Max(srcColor.y, dstColor.y);
		blendedColor.z = Max(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_COLORDODGE_EXT:
		blendedColor.x = blendOpColorDodge(srcColor.x, dstColor.x);
		blendedColor.y = blendOpColorDodge(srcColor.y, dstColor.y);
		blendedColor.z = blendOpColorDodge(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_COLORBURN_EXT:
		blendedColor.x = blendOpColorBurn(srcColor.x, dstColor.x);
		blendedColor.y = blendOpColorBurn(srcColor.y, dstColor.y);
		blendedColor.z = blendOpColorBurn(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_HARDLIGHT_EXT:
		blendedColor.x = blendOpHardlight(srcColor.x, dstColor.x);
		blendedColor.y = blendOpHardlight(srcColor.y, dstColor.y);
		blendedColor.z = blendOpHardlight(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_SOFTLIGHT_EXT:
		blendedColor.x = blendOpSoftlight(srcColor.x, dstColor.x);
		blendedColor.y = blendOpSoftlight(srcColor.y, dstColor.y);
		blendedColor.z = blendOpSoftlight(srcColor.z, dstColor.z);
		break;
	case VK_BLEND_OP_DIFFERENCE_EXT:
		blendedColor.x = Abs(srcColor.x - dstColor.x);
		blendedColor.y = Abs(srcColor.y - dstColor.y);
		blendedColor.z = Abs(srcColor.z - dstColor.z);
		break;
	case VK_BLEND_OP_EXCLUSION_EXT:
		blendedColor.x = srcColor.x + dstColor.x - (srcColor.x * dstColor.x * Float4(2.0f));
		blendedColor.y = srcColor.y + dstColor.y - (srcColor.y * dstColor.y * Float4(2.0f));
		blendedColor.z = srcColor.z + dstColor.z - (srcColor.z * dstColor.z * Float4(2.0f));
		break;
	case VK_BLEND_OP_HSL_HUE_EXT:
		setLumSat(srcColor, dstColor, dstColor, blendedColor.x, blendedColor.y, blendedColor.z);
		break;
	case VK_BLEND_OP_HSL_SATURATION_EXT:
		setLumSat(dstColor, srcColor, dstColor, blendedColor.x, blendedColor.y, blendedColor.z);
		break;
	case VK_BLEND_OP_HSL_COLOR_EXT:
		setLum(srcColor, dstColor, blendedColor.x, blendedColor.y, blendedColor.z);
		break;
	case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
		setLum(dstColor, srcColor, blendedColor.x, blendedColor.y, blendedColor.z);
		break;
	default:
		UNSUPPORTED("Unsupported advanced VkBlendOp: %d", int(state.blendState[index].blendOperation));
		break;
	}

	Float4 p = srcColor.w * dstColor.w;
	blendedColor.x *= p;
	blendedColor.y *= p;
	blendedColor.z *= p;

	p = srcColor.w * (Float4(1.0f) - dstColor.w);
	blendedColor.x += srcColor.x * p;
	blendedColor.y += srcColor.y * p;
	blendedColor.z += srcColor.z * p;

	p = dstColor.w * (Float4(1.0f) - srcColor.w);
	blendedColor.x += dstColor.x * p;
	blendedColor.y += dstColor.y * p;
	blendedColor.z += dstColor.z * p;

	return blendedColor;
}

bool PixelRoutine::blendFactorCanExceedFormatRange(VkBlendFactor blendFactor, vk::Format format)
{
	switch(blendFactor)
	{
	case VK_BLEND_FACTOR_ZERO:
	case VK_BLEND_FACTOR_ONE:
		return false;
	case VK_BLEND_FACTOR_SRC_COLOR:
	case VK_BLEND_FACTOR_SRC_ALPHA:
		// Source values have been clamped after fragment shader execution if the attachment format is normalized.
		return false;
	case VK_BLEND_FACTOR_DST_COLOR:
	case VK_BLEND_FACTOR_DST_ALPHA:
		// Dest values have a valid range due to being read from the attachment.
		return false;
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
	case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
	case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
	case VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		// For signed formats, negative values cause the result to exceed 1.0.
		return format.isSignedNormalized();
	case VK_BLEND_FACTOR_SRC_ALPHA_SATURATE:
		// min(As, 1 - Ad)
		return false;
	case VK_BLEND_FACTOR_CONSTANT_COLOR:
	case VK_BLEND_FACTOR_CONSTANT_ALPHA:
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
	case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
		return false;

	default:
		UNSUPPORTED("VkBlendFactor: %d", int(blendFactor));
		return false;
	}
}

Vector4f PixelRoutine::alphaBlend(int index, const Pointer<Byte> &cBuffer, const Vector4f &sourceColor, const Int &x)
{
	if(!state.blendState[index].alphaBlendEnable)
	{
		return sourceColor;
	}

	vk::Format format = state.colorFormat[index];
	ASSERT(format.supportsColorAttachmentBlend());

	Pointer<Byte> buffer = cBuffer;
	Int pitchB = *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));

	// destColor holds four texel color values.
	// Note: Despite the type being Vector4f, the colors may be stored as
	// integers. Half-floats are stored as full 32-bit floats.
	// Non-float and non-fixed point formats are not alpha blended.
	Vector4f destColor;

	switch(format)
	{
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R32_SFLOAT:
		// FIXME: movlps
		buffer += 4 * x;
		destColor.x.x = *Pointer<Float>(buffer + 0);
		destColor.x.y = *Pointer<Float>(buffer + 4);
		buffer += pitchB;
		// FIXME: movhps
		destColor.x.z = *Pointer<Float>(buffer + 0);
		destColor.x.w = *Pointer<Float>(buffer + 4);
		destColor.y = destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_UINT:
	case VK_FORMAT_R32G32_SFLOAT:
		buffer += 8 * x;
		destColor.x = *Pointer<Float4>(buffer, 16);
		buffer += pitchB;
		destColor.y = *Pointer<Float4>(buffer, 16);
		destColor.z = destColor.x;
		destColor.x = ShuffleLowHigh(destColor.x, destColor.y, 0x0202);
		destColor.z = ShuffleLowHigh(destColor.z, destColor.y, 0x1313);
		destColor.y = destColor.z;
		destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_UINT:
		buffer += 16 * x;
		destColor.x = *Pointer<Float4>(buffer + 0, 16);
		destColor.y = *Pointer<Float4>(buffer + 16, 16);
		buffer += pitchB;
		destColor.z = *Pointer<Float4>(buffer + 0, 16);
		destColor.w = *Pointer<Float4>(buffer + 16, 16);
		transpose4x4(destColor.x, destColor.y, destColor.z, destColor.w);
		break;
	case VK_FORMAT_R16_UNORM:
		buffer += 2 * x;
		destColor.x.x = Float(Int(*Pointer<UShort>(buffer + 0)));
		destColor.x.y = Float(Int(*Pointer<UShort>(buffer + 2)));
		buffer += pitchB;
		destColor.x.z = Float(Int(*Pointer<UShort>(buffer + 0)));
		destColor.x.w = Float(Int(*Pointer<UShort>(buffer + 2)));
		destColor.x *= Float4(1.0f / 0xFFFF);
		destColor.y = destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R16_SFLOAT:
		buffer += 2 * x;
		destColor.x.x = Float(*Pointer<Half>(buffer + 0));
		destColor.x.y = Float(*Pointer<Half>(buffer + 2));
		buffer += pitchB;
		destColor.x.z = Float(*Pointer<Half>(buffer + 0));
		destColor.x.w = Float(*Pointer<Half>(buffer + 2));
		destColor.y = destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R16G16_UNORM:
		buffer += 4 * x;
		destColor.x.x = Float(Int(*Pointer<UShort>(buffer + 0)));
		destColor.y.x = Float(Int(*Pointer<UShort>(buffer + 2)));
		destColor.x.y = Float(Int(*Pointer<UShort>(buffer + 4)));
		destColor.y.y = Float(Int(*Pointer<UShort>(buffer + 6)));
		buffer += pitchB;
		destColor.x.z = Float(Int(*Pointer<UShort>(buffer + 0)));
		destColor.y.z = Float(Int(*Pointer<UShort>(buffer + 2)));
		destColor.x.w = Float(Int(*Pointer<UShort>(buffer + 4)));
		destColor.y.w = Float(Int(*Pointer<UShort>(buffer + 6)));
		destColor.x *= Float4(1.0f / 0xFFFF);
		destColor.y *= Float4(1.0f / 0xFFFF);
		destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		buffer += 4 * x;
		destColor.x.x = Float(*Pointer<Half>(buffer + 0));
		destColor.y.x = Float(*Pointer<Half>(buffer + 2));
		destColor.x.y = Float(*Pointer<Half>(buffer + 4));
		destColor.y.y = Float(*Pointer<Half>(buffer + 6));
		buffer += pitchB;
		destColor.x.z = Float(*Pointer<Half>(buffer + 0));
		destColor.y.z = Float(*Pointer<Half>(buffer + 2));
		destColor.x.w = Float(*Pointer<Half>(buffer + 4));
		destColor.y.w = Float(*Pointer<Half>(buffer + 6));
		destColor.z = destColor.w = Float4(1.0f);
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		buffer += 8 * x;
		destColor.x.x = Float(Int(*Pointer<UShort>(buffer + 0x0)));
		destColor.y.x = Float(Int(*Pointer<UShort>(buffer + 0x2)));
		destColor.z.x = Float(Int(*Pointer<UShort>(buffer + 0x4)));
		destColor.w.x = Float(Int(*Pointer<UShort>(buffer + 0x6)));
		destColor.x.y = Float(Int(*Pointer<UShort>(buffer + 0x8)));
		destColor.y.y = Float(Int(*Pointer<UShort>(buffer + 0xa)));
		destColor.z.y = Float(Int(*Pointer<UShort>(buffer + 0xc)));
		destColor.w.y = Float(Int(*Pointer<UShort>(buffer + 0xe)));
		buffer += pitchB;
		destColor.x.z = Float(Int(*Pointer<UShort>(buffer + 0x0)));
		destColor.y.z = Float(Int(*Pointer<UShort>(buffer + 0x2)));
		destColor.z.z = Float(Int(*Pointer<UShort>(buffer + 0x4)));
		destColor.w.z = Float(Int(*Pointer<UShort>(buffer + 0x6)));
		destColor.x.w = Float(Int(*Pointer<UShort>(buffer + 0x8)));
		destColor.y.w = Float(Int(*Pointer<UShort>(buffer + 0xa)));
		destColor.z.w = Float(Int(*Pointer<UShort>(buffer + 0xc)));
		destColor.w.w = Float(Int(*Pointer<UShort>(buffer + 0xe)));
		destColor.x *= Float4(1.0f / 0xFFFF);
		destColor.y *= Float4(1.0f / 0xFFFF);
		destColor.z *= Float4(1.0f / 0xFFFF);
		destColor.w *= Float4(1.0f / 0xFFFF);
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		buffer += 8 * x;
		destColor.x.x = Float(*Pointer<Half>(buffer + 0x0));
		destColor.y.x = Float(*Pointer<Half>(buffer + 0x2));
		destColor.z.x = Float(*Pointer<Half>(buffer + 0x4));
		destColor.w.x = Float(*Pointer<Half>(buffer + 0x6));
		destColor.x.y = Float(*Pointer<Half>(buffer + 0x8));
		destColor.y.y = Float(*Pointer<Half>(buffer + 0xa));
		destColor.z.y = Float(*Pointer<Half>(buffer + 0xc));
		destColor.w.y = Float(*Pointer<Half>(buffer + 0xe));
		buffer += pitchB;
		destColor.x.z = Float(*Pointer<Half>(buffer + 0x0));
		destColor.y.z = Float(*Pointer<Half>(buffer + 0x2));
		destColor.z.z = Float(*Pointer<Half>(buffer + 0x4));
		destColor.w.z = Float(*Pointer<Half>(buffer + 0x6));
		destColor.x.w = Float(*Pointer<Half>(buffer + 0x8));
		destColor.y.w = Float(*Pointer<Half>(buffer + 0xa));
		destColor.z.w = Float(*Pointer<Half>(buffer + 0xc));
		destColor.w.w = Float(*Pointer<Half>(buffer + 0xe));
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		buffer += 4 * x;
		destColor.x = r11g11b10Unpack(*Pointer<UInt>(buffer + 0));
		destColor.y = r11g11b10Unpack(*Pointer<UInt>(buffer + 4));
		buffer += pitchB;
		destColor.z = r11g11b10Unpack(*Pointer<UInt>(buffer + 0));
		destColor.w = r11g11b10Unpack(*Pointer<UInt>(buffer + 4));
		transpose4x3(destColor.x, destColor.y, destColor.z, destColor.w);
		destColor.w = Float4(1.0f);
		break;
	default:
		{
			// Attempt to read an integer based format and convert it to float
			Vector4s color;
			readPixel(index, cBuffer, x, color);
			destColor.x = convertFloat32(As<UShort4>(color.x));
			destColor.y = convertFloat32(As<UShort4>(color.y));
			destColor.z = convertFloat32(As<UShort4>(color.z));
			destColor.w = convertFloat32(As<UShort4>(color.w));
		}
		break;
	}

	Vector4f sourceFactor;
	Vector4f destFactor;

	blendFactorRGB(sourceFactor, sourceColor, destColor, state.blendState[index].sourceBlendFactor, format);
	blendFactorRGB(destFactor, sourceColor, destColor, state.blendState[index].destBlendFactor, format);
	blendFactorAlpha(sourceFactor.w, sourceColor.w, destColor.w, state.blendState[index].sourceBlendFactorAlpha, format);
	blendFactorAlpha(destFactor.w, sourceColor.w, destColor.w, state.blendState[index].destBlendFactorAlpha, format);

	Vector4f blendedColor;

	switch(state.blendState[index].blendOperation)
	{
	case VK_BLEND_OP_ADD:
		blendedColor.x = sourceColor.x * sourceFactor.x + destColor.x * destFactor.x;
		blendedColor.y = sourceColor.y * sourceFactor.y + destColor.y * destFactor.y;
		blendedColor.z = sourceColor.z * sourceFactor.z + destColor.z * destFactor.z;
		break;
	case VK_BLEND_OP_SUBTRACT:
		blendedColor.x = sourceColor.x * sourceFactor.x - destColor.x * destFactor.x;
		blendedColor.y = sourceColor.y * sourceFactor.y - destColor.y * destFactor.y;
		blendedColor.z = sourceColor.z * sourceFactor.z - destColor.z * destFactor.z;
		break;
	case VK_BLEND_OP_REVERSE_SUBTRACT:
		blendedColor.x = destColor.x * destFactor.x - sourceColor.x * sourceFactor.x;
		blendedColor.y = destColor.y * destFactor.y - sourceColor.y * sourceFactor.y;
		blendedColor.z = destColor.z * destFactor.z - sourceColor.z * sourceFactor.z;
		break;
	case VK_BLEND_OP_MIN:
		blendedColor.x = Min(sourceColor.x, destColor.x);
		blendedColor.y = Min(sourceColor.y, destColor.y);
		blendedColor.z = Min(sourceColor.z, destColor.z);
		break;
	case VK_BLEND_OP_MAX:
		blendedColor.x = Max(sourceColor.x, destColor.x);
		blendedColor.y = Max(sourceColor.y, destColor.y);
		blendedColor.z = Max(sourceColor.z, destColor.z);
		break;
	case VK_BLEND_OP_SRC_EXT:
		blendedColor.x = sourceColor.x;
		blendedColor.y = sourceColor.y;
		blendedColor.z = sourceColor.z;
		break;
	case VK_BLEND_OP_DST_EXT:
		blendedColor.x = destColor.x;
		blendedColor.y = destColor.y;
		blendedColor.z = destColor.z;
		break;
	case VK_BLEND_OP_ZERO_EXT:
		blendedColor.x = Float4(0.0f);
		blendedColor.y = Float4(0.0f);
		blendedColor.z = Float4(0.0f);
		break;
	case VK_BLEND_OP_MULTIPLY_EXT:
	case VK_BLEND_OP_SCREEN_EXT:
	case VK_BLEND_OP_OVERLAY_EXT:
	case VK_BLEND_OP_DARKEN_EXT:
	case VK_BLEND_OP_LIGHTEN_EXT:
	case VK_BLEND_OP_COLORDODGE_EXT:
	case VK_BLEND_OP_COLORBURN_EXT:
	case VK_BLEND_OP_HARDLIGHT_EXT:
	case VK_BLEND_OP_SOFTLIGHT_EXT:
	case VK_BLEND_OP_DIFFERENCE_EXT:
	case VK_BLEND_OP_EXCLUSION_EXT:
	case VK_BLEND_OP_HSL_HUE_EXT:
	case VK_BLEND_OP_HSL_SATURATION_EXT:
	case VK_BLEND_OP_HSL_COLOR_EXT:
	case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
		blendedColor = computeAdvancedBlendMode(index, sourceColor, destColor, sourceFactor, destFactor);
		break;
	default:
		UNSUPPORTED("VkBlendOp: %d", int(state.blendState[index].blendOperation));
	}

	switch(state.blendState[index].blendOperationAlpha)
	{
	case VK_BLEND_OP_ADD:
		blendedColor.w = sourceColor.w * sourceFactor.w + destColor.w * destFactor.w;
		break;
	case VK_BLEND_OP_SUBTRACT:
		blendedColor.w = sourceColor.w * sourceFactor.w - destColor.w * destFactor.w;
		break;
	case VK_BLEND_OP_REVERSE_SUBTRACT:
		blendedColor.w = destColor.w * destFactor.w - sourceColor.w * sourceFactor.w;
		break;
	case VK_BLEND_OP_MIN:
		blendedColor.w = Min(sourceColor.w, destColor.w);
		break;
	case VK_BLEND_OP_MAX:
		blendedColor.w = Max(sourceColor.w, destColor.w);
		break;
	case VK_BLEND_OP_SRC_EXT:
		blendedColor.w = sourceColor.w;
		break;
	case VK_BLEND_OP_DST_EXT:
		blendedColor.w = destColor.w;
		break;
	case VK_BLEND_OP_ZERO_EXT:
		blendedColor.w = Float4(0.0f);
		break;
	case VK_BLEND_OP_MULTIPLY_EXT:
	case VK_BLEND_OP_SCREEN_EXT:
	case VK_BLEND_OP_OVERLAY_EXT:
	case VK_BLEND_OP_DARKEN_EXT:
	case VK_BLEND_OP_LIGHTEN_EXT:
	case VK_BLEND_OP_COLORDODGE_EXT:
	case VK_BLEND_OP_COLORBURN_EXT:
	case VK_BLEND_OP_HARDLIGHT_EXT:
	case VK_BLEND_OP_SOFTLIGHT_EXT:
	case VK_BLEND_OP_DIFFERENCE_EXT:
	case VK_BLEND_OP_EXCLUSION_EXT:
	case VK_BLEND_OP_HSL_HUE_EXT:
	case VK_BLEND_OP_HSL_SATURATION_EXT:
	case VK_BLEND_OP_HSL_COLOR_EXT:
	case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
		// All of the currently supported 'advanced blend modes' compute the alpha the same way.
		blendedColor.w = sourceColor.w + destColor.w - (sourceColor.w * destColor.w);
		break;
	default:
		UNSUPPORTED("VkBlendOp: %d", int(state.blendState[index].blendOperationAlpha));
	}

	return blendedColor;
}

void PixelRoutine::writeColor(int index, const Pointer<Byte> &cBuffer, const Int &x, Vector4f &color, const Int &sMask, const Int &zMask, const Int &cMask)
{
	vk::Format format = state.colorFormat[index];
	switch(format)
	{
	case VK_FORMAT_R16G16B16A16_UNORM:
		color.w = Min(Max(color.w, Float4(0.0f)), Float4(1.0f));  // TODO(b/204560089): Omit clamp if redundant
		color.w = As<Float4>(RoundInt(color.w * Float4(0xFFFF)));
		color.z = Min(Max(color.z, Float4(0.0f)), Float4(1.0f));  // TODO(b/204560089): Omit clamp if redundant
		color.z = As<Float4>(RoundInt(color.z * Float4(0xFFFF)));
		// [[fallthrough]]
	case VK_FORMAT_R16G16_UNORM:
		color.y = Min(Max(color.y, Float4(0.0f)), Float4(1.0f));  // TODO(b/204560089): Omit clamp if redundant
		color.y = As<Float4>(RoundInt(color.y * Float4(0xFFFF)));
		//[[fallthrough]]
	case VK_FORMAT_R16_UNORM:
		color.x = Min(Max(color.x, Float4(0.0f)), Float4(1.0f));  // TODO(b/204560089): Omit clamp if redundant
		color.x = As<Float4>(RoundInt(color.x * Float4(0xFFFF)));
		break;
	default:
		// TODO(b/204560089): Omit clamp if redundant
		if(format.isUnsignedNormalized())
		{
			color.x = Min(Max(color.x, Float4(0.0f)), Float4(1.0f));
			color.y = Min(Max(color.y, Float4(0.0f)), Float4(1.0f));
			color.z = Min(Max(color.z, Float4(0.0f)), Float4(1.0f));
			color.w = Min(Max(color.w, Float4(0.0f)), Float4(1.0f));
		}
		else if(format.isSignedNormalized())
		{
			color.x = Min(Max(color.x, Float4(-1.0f)), Float4(1.0f));
			color.y = Min(Max(color.y, Float4(-1.0f)), Float4(1.0f));
			color.z = Min(Max(color.z, Float4(-1.0f)), Float4(1.0f));
			color.w = Min(Max(color.w, Float4(-1.0f)), Float4(1.0f));
		}
	}

	switch(format)
	{
	case VK_FORMAT_R16_SFLOAT:
	case VK_FORMAT_R32_SFLOAT:
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_SINT:
	case VK_FORMAT_R16_UINT:
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UINT:
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		break;
	case VK_FORMAT_R16G16_SFLOAT:
	case VK_FORMAT_R32G32_SFLOAT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_UINT:
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_SINT:
	case VK_FORMAT_R16G16_UINT:
	case VK_FORMAT_R8G8_SINT:
	case VK_FORMAT_R8G8_UINT:
		color.z = color.x;
		color.x = UnpackLow(color.x, color.y);
		color.z = UnpackHigh(color.z, color.y);
		color.y = color.z;
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
	case VK_FORMAT_R32G32B32A32_SFLOAT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_UINT:
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_SINT:
	case VK_FORMAT_R16G16B16A16_UINT:
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		transpose4x4(color.x, color.y, color.z, color.w);
		break;
	default:
		UNSUPPORTED("VkFormat: %d", int(format));
	}

	int rgbaWriteMask = state.colorWriteActive(index);
	int bgraWriteMask = (rgbaWriteMask & 0x0000000A) | (rgbaWriteMask & 0x00000001) << 2 | (rgbaWriteMask & 0x00000004) >> 2;

	Int xMask;  // Combination of all masks

	if(state.depthTestActive)
	{
		xMask = zMask;
	}
	else
	{
		xMask = cMask;
	}

	if(state.stencilActive)
	{
		xMask &= sMask;
	}

	Pointer<Byte> buffer = cBuffer;
	Int pitchB = *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));
	Float4 value;

	switch(format)
	{
	case VK_FORMAT_R32_SFLOAT:
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_UINT:
		if(rgbaWriteMask & 0x00000001)
		{
			buffer += 4 * x;

			// FIXME: movlps
			value.x = *Pointer<Float>(buffer + 0);
			value.y = *Pointer<Float>(buffer + 4);

			buffer += pitchB;

			// FIXME: movhps
			value.z = *Pointer<Float>(buffer + 0);
			value.w = *Pointer<Float>(buffer + 4);

			color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X) + xMask * 16, 16));
			color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(value));

			// FIXME: movhps
			*Pointer<Float>(buffer + 0) = color.x.z;
			*Pointer<Float>(buffer + 4) = color.x.w;

			buffer -= pitchB;

			// FIXME: movlps
			*Pointer<Float>(buffer + 0) = color.x.x;
			*Pointer<Float>(buffer + 4) = color.x.y;
		}
		break;
	case VK_FORMAT_R16_SFLOAT:
		if(rgbaWriteMask & 0x00000001)
		{
			buffer += 2 * x;

			value = Insert(value, Float(*Pointer<Half>(buffer + 0)), 0);
			value = Insert(value, Float(*Pointer<Half>(buffer + 2)), 1);

			buffer += pitchB;

			value = Insert(value, Float(*Pointer<Half>(buffer + 0)), 2);
			value = Insert(value, Float(*Pointer<Half>(buffer + 2)), 3);

			color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X) + xMask * 16, 16));
			color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(value));

			*Pointer<Half>(buffer + 0) = Half(color.x.z);
			*Pointer<Half>(buffer + 2) = Half(color.x.w);

			buffer -= pitchB;

			*Pointer<Half>(buffer + 0) = Half(color.x.x);
			*Pointer<Half>(buffer + 2) = Half(color.x.y);
		}
		break;
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_SINT:
	case VK_FORMAT_R16_UINT:
		if(rgbaWriteMask & 0x00000001)
		{
			buffer += 2 * x;

			UShort4 xyzw;
			xyzw = As<UShort4>(Insert(As<Int2>(xyzw), *Pointer<Int>(buffer), 0));

			buffer += pitchB;

			xyzw = As<UShort4>(Insert(As<Int2>(xyzw), *Pointer<Int>(buffer), 1));
			value = As<Float4>(Int4(xyzw));

			color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X) + xMask * 16, 16));
			color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(value));

			Float component = color.x.z;
			*Pointer<UShort>(buffer + 0) = UShort(As<Int>(component));
			component = color.x.w;
			*Pointer<UShort>(buffer + 2) = UShort(As<Int>(component));

			buffer -= pitchB;

			component = color.x.x;
			*Pointer<UShort>(buffer + 0) = UShort(As<Int>(component));
			component = color.x.y;
			*Pointer<UShort>(buffer + 2) = UShort(As<Int>(component));
		}
		break;
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UINT:
		if(rgbaWriteMask & 0x00000001)
		{
			buffer += x;

			UInt xyzw, packedCol;

			xyzw = UInt(*Pointer<UShort>(buffer)) & 0xFFFF;
			buffer += pitchB;
			xyzw |= UInt(*Pointer<UShort>(buffer)) << 16;

			Short4 tmpCol = Short4(As<Int4>(color.x));
			if(format == VK_FORMAT_R8_SINT)
			{
				tmpCol = As<Short4>(PackSigned(tmpCol, tmpCol));
			}
			else
			{
				tmpCol = As<Short4>(PackUnsigned(tmpCol, tmpCol));
			}
			packedCol = Extract(As<Int2>(tmpCol), 0);

			packedCol = (packedCol & *Pointer<UInt>(constants + OFFSET(Constants, maskB4Q) + 8 * xMask)) |
			            (xyzw & *Pointer<UInt>(constants + OFFSET(Constants, invMaskB4Q) + 8 * xMask));

			*Pointer<UShort>(buffer) = UShort(packedCol >> 16);
			buffer -= pitchB;
			*Pointer<UShort>(buffer) = UShort(packedCol);
		}
		break;
	case VK_FORMAT_R32G32_SFLOAT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_UINT:
		buffer += 8 * x;

		value = *Pointer<Float4>(buffer);

		if((rgbaWriteMask & 0x00000003) != 0x00000003)
		{
			Float4 masked = value;
			color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskD01X[rgbaWriteMask & 0x3][0])));
			masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, maskD01X[~rgbaWriteMask & 0x3][0])));
			color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(masked));
		}

		color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskQ01X) + xMask * 16, 16));
		value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskQ01X) + xMask * 16, 16));
		color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(value));
		*Pointer<Float4>(buffer) = color.x;

		buffer += pitchB;

		value = *Pointer<Float4>(buffer);

		if((rgbaWriteMask & 0x00000003) != 0x00000003)
		{
			Float4 masked;

			masked = value;
			color.y = As<Float4>(As<Int4>(color.y) & *Pointer<Int4>(constants + OFFSET(Constants, maskD01X[rgbaWriteMask & 0x3][0])));
			masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, maskD01X[~rgbaWriteMask & 0x3][0])));
			color.y = As<Float4>(As<Int4>(color.y) | As<Int4>(masked));
		}

		color.y = As<Float4>(As<Int4>(color.y) & *Pointer<Int4>(constants + OFFSET(Constants, maskQ23X) + xMask * 16, 16));
		value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskQ23X) + xMask * 16, 16));
		color.y = As<Float4>(As<Int4>(color.y) | As<Int4>(value));
		*Pointer<Float4>(buffer) = color.y;
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		if((rgbaWriteMask & 0x00000003) != 0x0)
		{
			buffer += 4 * x;

			UInt2 rgbaMask;
			UInt2 packedCol;
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.x.y))) << 16) | UInt(As<UShort>(Half(color.x.x))), 0);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.x.w))) << 16) | UInt(As<UShort>(Half(color.x.z))), 1);

			UShort4 value = *Pointer<UShort4>(buffer);
			UInt2 mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				Int tmpMask = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[rgbaWriteMask & 0x3][0]));
				rgbaMask = As<UInt2>(Int2(tmpMask, tmpMask));
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt2>(buffer) = (packedCol & mergedMask) | (As<UInt2>(value) & ~mergedMask);

			buffer += pitchB;

			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.y.y))) << 16) | UInt(As<UShort>(Half(color.y.x))), 0);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.y.w))) << 16) | UInt(As<UShort>(Half(color.y.z))), 1);
			value = *Pointer<UShort4>(buffer);
			mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt2>(buffer) = (packedCol & mergedMask) | (As<UInt2>(value) & ~mergedMask);
		}
		break;
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_SINT:
	case VK_FORMAT_R16G16_UINT:
		if((rgbaWriteMask & 0x00000003) != 0x0)
		{
			buffer += 4 * x;

			UInt2 rgbaMask;
			UShort4 packedCol = UShort4(As<Int4>(color.x));
			UShort4 value = *Pointer<UShort4>(buffer);
			UInt2 mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				Int tmpMask = *Pointer<Int>(constants + OFFSET(Constants, maskW4Q[rgbaWriteMask & 0x3][0]));
				rgbaMask = As<UInt2>(Int2(tmpMask, tmpMask));
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt2>(buffer) = (As<UInt2>(packedCol) & mergedMask) | (As<UInt2>(value) & ~mergedMask);

			buffer += pitchB;

			packedCol = UShort4(As<Int4>(color.y));
			value = *Pointer<UShort4>(buffer);
			mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt2>(buffer) = (As<UInt2>(packedCol) & mergedMask) | (As<UInt2>(value) & ~mergedMask);
		}
		break;
	case VK_FORMAT_R8G8_SINT:
	case VK_FORMAT_R8G8_UINT:
		if((rgbaWriteMask & 0x00000003) != 0x0)
		{
			buffer += 2 * x;

			Int2 xyzw, packedCol;

			xyzw = Insert(xyzw, *Pointer<Int>(buffer), 0);
			buffer += pitchB;
			xyzw = Insert(xyzw, *Pointer<Int>(buffer), 1);

			if(format == VK_FORMAT_R8G8_SINT)
			{
				packedCol = As<Int2>(PackSigned(Short4(As<Int4>(color.x)), Short4(As<Int4>(color.y))));
			}
			else
			{
				packedCol = As<Int2>(PackUnsigned(Short4(As<Int4>(color.x)), Short4(As<Int4>(color.y))));
			}

			UInt2 mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskW4Q) + xMask * 8);
			if((rgbaWriteMask & 0x3) != 0x3)
			{
				Int tmpMask = *Pointer<Int>(constants + OFFSET(Constants, maskB4Q[5 * (rgbaWriteMask & 0x3)][0]));
				UInt2 rgbaMask = As<UInt2>(Int2(tmpMask, tmpMask));
				mergedMask &= rgbaMask;
			}

			packedCol = As<Int2>((As<UInt2>(packedCol) & mergedMask) | (As<UInt2>(xyzw) & ~mergedMask));

			*Pointer<UInt>(buffer) = As<UInt>(Extract(packedCol, 1));
			buffer -= pitchB;
			*Pointer<UInt>(buffer) = As<UInt>(Extract(packedCol, 0));
		}
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_UINT:
		buffer += 16 * x;

		{
			value = *Pointer<Float4>(buffer, 16);

			if(rgbaWriteMask != 0x0000000F)
			{
				Float4 masked = value;
				color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X[rgbaWriteMask][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X[rgbaWriteMask][0])));
				color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(masked));
			}

			color.x = As<Float4>(As<Int4>(color.x) & *Pointer<Int4>(constants + OFFSET(Constants, maskX0X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskX0X) + xMask * 16, 16));
			color.x = As<Float4>(As<Int4>(color.x) | As<Int4>(value));
			*Pointer<Float4>(buffer, 16) = color.x;
		}

		{
			value = *Pointer<Float4>(buffer + 16, 16);

			if(rgbaWriteMask != 0x0000000F)
			{
				Float4 masked = value;
				color.y = As<Float4>(As<Int4>(color.y) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X[rgbaWriteMask][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X[rgbaWriteMask][0])));
				color.y = As<Float4>(As<Int4>(color.y) | As<Int4>(masked));
			}

			color.y = As<Float4>(As<Int4>(color.y) & *Pointer<Int4>(constants + OFFSET(Constants, maskX1X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskX1X) + xMask * 16, 16));
			color.y = As<Float4>(As<Int4>(color.y) | As<Int4>(value));
			*Pointer<Float4>(buffer + 16, 16) = color.y;
		}

		buffer += pitchB;

		{
			value = *Pointer<Float4>(buffer, 16);

			if(rgbaWriteMask != 0x0000000F)
			{
				Float4 masked = value;
				color.z = As<Float4>(As<Int4>(color.z) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X[rgbaWriteMask][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X[rgbaWriteMask][0])));
				color.z = As<Float4>(As<Int4>(color.z) | As<Int4>(masked));
			}

			color.z = As<Float4>(As<Int4>(color.z) & *Pointer<Int4>(constants + OFFSET(Constants, maskX2X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskX2X) + xMask * 16, 16));
			color.z = As<Float4>(As<Int4>(color.z) | As<Int4>(value));
			*Pointer<Float4>(buffer, 16) = color.z;
		}

		{
			value = *Pointer<Float4>(buffer + 16, 16);

			if(rgbaWriteMask != 0x0000000F)
			{
				Float4 masked = value;
				color.w = As<Float4>(As<Int4>(color.w) & *Pointer<Int4>(constants + OFFSET(Constants, maskD4X[rgbaWriteMask][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskD4X[rgbaWriteMask][0])));
				color.w = As<Float4>(As<Int4>(color.w) | As<Int4>(masked));
			}

			color.w = As<Float4>(As<Int4>(color.w) & *Pointer<Int4>(constants + OFFSET(Constants, maskX3X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(constants + OFFSET(Constants, invMaskX3X) + xMask * 16, 16));
			color.w = As<Float4>(As<Int4>(color.w) | As<Int4>(value));
			*Pointer<Float4>(buffer + 16, 16) = color.w;
		}
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		if((rgbaWriteMask & 0x0000000F) != 0x0)
		{
			buffer += 8 * x;

			UInt4 rgbaMask;
			UInt4 value = *Pointer<UInt4>(buffer);
			UInt4 packedCol;
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.x.y))) << 16) | UInt(As<UShort>(Half(color.x.x))), 0);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.x.w))) << 16) | UInt(As<UShort>(Half(color.x.z))), 1);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.y.y))) << 16) | UInt(As<UShort>(Half(color.y.x))), 2);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.y.w))) << 16) | UInt(As<UShort>(Half(color.y.z))), 3);
			UInt4 mergedMask = *Pointer<UInt4>(constants + OFFSET(Constants, maskQ01X) + xMask * 16);
			if((rgbaWriteMask & 0xF) != 0xF)
			{
				UInt2 tmpMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskW4Q[rgbaWriteMask][0]));
				rgbaMask = UInt4(tmpMask, tmpMask);
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt4>(buffer) = (packedCol & mergedMask) | (As<UInt4>(value) & ~mergedMask);

			buffer += pitchB;

			value = *Pointer<UInt4>(buffer);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.z.y))) << 16) | UInt(As<UShort>(Half(color.z.x))), 0);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.z.w))) << 16) | UInt(As<UShort>(Half(color.z.z))), 1);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.w.y))) << 16) | UInt(As<UShort>(Half(color.w.x))), 2);
			packedCol = Insert(packedCol, (UInt(As<UShort>(Half(color.w.w))) << 16) | UInt(As<UShort>(Half(color.w.z))), 3);
			mergedMask = *Pointer<UInt4>(constants + OFFSET(Constants, maskQ23X) + xMask * 16);
			if((rgbaWriteMask & 0xF) != 0xF)
			{
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt4>(buffer) = (packedCol & mergedMask) | (As<UInt4>(value) & ~mergedMask);
		}
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		if((rgbaWriteMask & 0x7) != 0x0)
		{
			buffer += 4 * x;

			UInt4 packedCol;
			packedCol = Insert(packedCol, r11g11b10Pack(color.x), 0);
			packedCol = Insert(packedCol, r11g11b10Pack(color.y), 1);
			packedCol = Insert(packedCol, r11g11b10Pack(color.z), 2);
			packedCol = Insert(packedCol, r11g11b10Pack(color.w), 3);

			UInt4 value;
			value = Insert(value, *Pointer<UInt>(buffer + 0), 0);
			value = Insert(value, *Pointer<UInt>(buffer + 4), 1);
			buffer += pitchB;
			value = Insert(value, *Pointer<UInt>(buffer + 0), 2);
			value = Insert(value, *Pointer<UInt>(buffer + 4), 3);

			UInt4 mask = *Pointer<UInt4>(constants + OFFSET(Constants, maskD4X[0][0]) + xMask * 16, 16);
			if((rgbaWriteMask & 0x7) != 0x7)
			{
				mask &= *Pointer<UInt4>(constants + OFFSET(Constants, mask11X[rgbaWriteMask & 0x7][0]), 16);
			}
			value = (packedCol & mask) | (value & ~mask);

			*Pointer<UInt>(buffer + 0) = value.z;
			*Pointer<UInt>(buffer + 4) = value.w;
			buffer -= pitchB;
			*Pointer<UInt>(buffer + 0) = value.x;
			*Pointer<UInt>(buffer + 4) = value.y;
		}
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_SINT:
	case VK_FORMAT_R16G16B16A16_UINT:
		if((rgbaWriteMask & 0x0000000F) != 0x0)
		{
			buffer += 8 * x;

			UInt4 rgbaMask;
			UShort8 value = *Pointer<UShort8>(buffer);
			UShort8 packedCol = UShort8(UShort4(As<Int4>(color.x)), UShort4(As<Int4>(color.y)));
			UInt4 mergedMask = *Pointer<UInt4>(constants + OFFSET(Constants, maskQ01X) + xMask * 16);
			if((rgbaWriteMask & 0xF) != 0xF)
			{
				UInt2 tmpMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskW4Q[rgbaWriteMask][0]));
				rgbaMask = UInt4(tmpMask, tmpMask);
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt4>(buffer) = (As<UInt4>(packedCol) & mergedMask) | (As<UInt4>(value) & ~mergedMask);

			buffer += pitchB;

			value = *Pointer<UShort8>(buffer);
			packedCol = UShort8(UShort4(As<Int4>(color.z)), UShort4(As<Int4>(color.w)));
			mergedMask = *Pointer<UInt4>(constants + OFFSET(Constants, maskQ23X) + xMask * 16);
			if((rgbaWriteMask & 0xF) != 0xF)
			{
				mergedMask &= rgbaMask;
			}
			*Pointer<UInt4>(buffer) = (As<UInt4>(packedCol) & mergedMask) | (As<UInt4>(value) & ~mergedMask);
		}
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		if((rgbaWriteMask & 0x0000000F) != 0x0)
		{
			UInt2 value, packedCol, mergedMask;

			buffer += 4 * x;

			bool isSigned = (format == VK_FORMAT_R8G8B8A8_SINT) || (format == VK_FORMAT_A8B8G8R8_SINT_PACK32);

			if(isSigned)
			{
				packedCol = As<UInt2>(PackSigned(Short4(As<Int4>(color.x)), Short4(As<Int4>(color.y))));
			}
			else
			{
				packedCol = As<UInt2>(PackUnsigned(Short4(As<Int4>(color.x)), Short4(As<Int4>(color.y))));
			}
			value = *Pointer<UInt2>(buffer, 16);
			mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<UInt2>(constants + OFFSET(Constants, maskB4Q[rgbaWriteMask][0]));
			}
			*Pointer<UInt2>(buffer) = (packedCol & mergedMask) | (value & ~mergedMask);

			buffer += pitchB;

			if(isSigned)
			{
				packedCol = As<UInt2>(PackSigned(Short4(As<Int4>(color.z)), Short4(As<Int4>(color.w))));
			}
			else
			{
				packedCol = As<UInt2>(PackUnsigned(Short4(As<Int4>(color.z)), Short4(As<Int4>(color.w))));
			}
			value = *Pointer<UInt2>(buffer, 16);
			mergedMask = *Pointer<UInt2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<UInt2>(constants + OFFSET(Constants, maskB4Q[rgbaWriteMask][0]));
			}
			*Pointer<UInt2>(buffer) = (packedCol & mergedMask) | (value & ~mergedMask);
		}
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		if((rgbaWriteMask & 0x0000000F) != 0x0)
		{
			Int2 mergedMask, packedCol, value;
			Int4 packed = ((As<Int4>(color.w) & Int4(0x3)) << 30) |
			              ((As<Int4>(color.z) & Int4(0x3ff)) << 20) |
			              ((As<Int4>(color.y) & Int4(0x3ff)) << 10) |
			              ((As<Int4>(color.x) & Int4(0x3ff)));

			buffer += 4 * x;
			value = *Pointer<Int2>(buffer, 16);
			mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[rgbaWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(packed) & mergedMask) | (value & ~mergedMask);

			buffer += pitchB;

			value = *Pointer<Int2>(buffer, 16);
			mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(rgbaWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[rgbaWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(Int4(packed.zwww)) & mergedMask) | (value & ~mergedMask);
		}
		break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		if((bgraWriteMask & 0x0000000F) != 0x0)
		{
			Int2 mergedMask, packedCol, value;
			Int4 packed = ((As<Int4>(color.w) & Int4(0x3)) << 30) |
			              ((As<Int4>(color.x) & Int4(0x3ff)) << 20) |
			              ((As<Int4>(color.y) & Int4(0x3ff)) << 10) |
			              ((As<Int4>(color.z) & Int4(0x3ff)));

			buffer += 4 * x;
			value = *Pointer<Int2>(buffer, 16);
			mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD01Q) + xMask * 8);
			if(bgraWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[bgraWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(packed) & mergedMask) | (value & ~mergedMask);

			buffer += *Pointer<Int>(data + OFFSET(DrawData, colorPitchB[index]));

			value = *Pointer<Int2>(buffer, 16);
			mergedMask = *Pointer<Int2>(constants + OFFSET(Constants, maskD23Q) + xMask * 8);
			if(bgraWriteMask != 0xF)
			{
				mergedMask &= *Pointer<Int2>(constants + OFFSET(Constants, mask10Q[bgraWriteMask][0]));
			}
			*Pointer<Int2>(buffer) = (As<Int2>(Int4(packed.zwww)) & mergedMask) | (value & ~mergedMask);
		}
		break;
	default:
		UNSUPPORTED("VkFormat: %d", int(format));
	}
}

UShort4 PixelRoutine::convertFixed16(const Float4 &cf, bool saturate)
{
	return UShort4(cf * Float4(0xFFFF), saturate);
}

Float4 PixelRoutine::convertFloat32(const UShort4 &cf)
{
	return Float4(cf) * Float4(1.0f / 65535.0f);
}

void PixelRoutine::sRGBtoLinear16_12_16(Vector4s &c)
{
	Pointer<Byte> LUT = constants + OFFSET(Constants, sRGBtoLinear12_16);

	c.x = AddSat(As<UShort4>(c.x), UShort4(0x0007)) >> 4;
	c.y = AddSat(As<UShort4>(c.y), UShort4(0x0007)) >> 4;
	c.z = AddSat(As<UShort4>(c.z), UShort4(0x0007)) >> 4;

	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 0))), 0);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 1))), 1);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 2))), 2);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 3))), 3);

	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 0))), 0);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 1))), 1);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 2))), 2);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 3))), 3);

	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 0))), 0);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 1))), 1);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 2))), 2);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 3))), 3);
}

void PixelRoutine::linearToSRGB16_12_16(Vector4s &c)
{
	c.x = AddSat(As<UShort4>(c.x), UShort4(0x0007)) >> 4;
	c.y = AddSat(As<UShort4>(c.y), UShort4(0x0007)) >> 4;
	c.z = AddSat(As<UShort4>(c.z), UShort4(0x0007)) >> 4;

	linearToSRGB12_16(c);
}

void PixelRoutine::linearToSRGB12_16(Vector4s &c)
{
	Pointer<Byte> LUT = constants + OFFSET(Constants, linearToSRGB12_16);

	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 0))), 0);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 1))), 1);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 2))), 2);
	c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 3))), 3);

	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 0))), 0);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 1))), 1);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 2))), 2);
	c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 3))), 3);

	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 0))), 0);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 1))), 1);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 2))), 2);
	c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 3))), 3);
}

Float4 PixelRoutine::sRGBtoLinear(const Float4 &x)  // Approximates x^2.2
{
	Float4 linear = x * x;
	linear = linear * Float4(0.73f) + linear * x * Float4(0.27f);

	return Min(Max(linear, Float4(0.0f)), Float4(1.0f));
}

}  // namespace sw
