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

#include "PixelProcessor.hpp"

#include "Surface.hpp"
#include "Primitive.hpp"
#include "Pipeline/PixelProgram.hpp"
#include "Pipeline/PixelShader.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Debug.hpp"

#include <string.h>

namespace sw
{
	extern bool complementaryDepthBuffer;
	extern TransparencyAntialiasing transparencyAntialiasing;
	extern bool perspectiveCorrection;

	bool precachePixel = false;

	unsigned int PixelProcessor::States::computeHash()
	{
		unsigned int *state = (unsigned int*)this;
		unsigned int hash = 0;

		for(unsigned int i = 0; i < sizeof(States) / 4; i++)
		{
			hash ^= state[i];
		}

		return hash;
	}

	PixelProcessor::State::State()
	{
		memset(this, 0, sizeof(State));
	}

	bool PixelProcessor::State::operator==(const State &state) const
	{
		if(hash != state.hash)
		{
			return false;
		}

		return memcmp(static_cast<const States*>(this), static_cast<const States*>(&state), sizeof(States)) == 0;
	}

	PixelProcessor::UniformBufferInfo::UniformBufferInfo()
	{
		buffer = nullptr;
		offset = 0;
	}

	PixelProcessor::PixelProcessor(Context *context) : context(context)
	{
		routineCache = nullptr;
		setRoutineCacheSize(1024);
	}

	PixelProcessor::~PixelProcessor()
	{
		delete routineCache;
		routineCache = nullptr;
	}

	void PixelProcessor::setFloatConstant(unsigned int index, const float value[4])
	{
		if(index < FRAGMENT_UNIFORM_VECTORS)
		{
			c[index][0] = value[0];
			c[index][1] = value[1];
			c[index][2] = value[2];
			c[index][3] = value[3];
		}
		else ASSERT(false);
	}

	void PixelProcessor::setIntegerConstant(unsigned int index, const int value[4])
	{
		if(index < 16)
		{
			i[index][0] = value[0];
			i[index][1] = value[1];
			i[index][2] = value[2];
			i[index][3] = value[3];
		}
		else ASSERT(false);
	}

	void PixelProcessor::setBooleanConstant(unsigned int index, int boolean)
	{
		if(index < 16)
		{
			b[index] = boolean != 0;
		}
		else ASSERT(false);
	}

	void PixelProcessor::setUniformBuffer(int index, sw::Resource* buffer, int offset)
	{
		uniformBufferInfo[index].buffer = buffer;
		uniformBufferInfo[index].offset = offset;
	}

	void PixelProcessor::lockUniformBuffers(byte** u, sw::Resource* uniformBuffers[])
	{
		for(int i = 0; i < MAX_UNIFORM_BUFFER_BINDINGS; ++i)
		{
			u[i] = uniformBufferInfo[i].buffer ? static_cast<byte*>(uniformBufferInfo[i].buffer->lock(PUBLIC, PRIVATE)) + uniformBufferInfo[i].offset : nullptr;
			uniformBuffers[i] = uniformBufferInfo[i].buffer;
		}
	}

	void PixelProcessor::setRenderTarget(int index, Surface *renderTarget, unsigned int layer)
	{
		context->renderTarget[index] = renderTarget;
		context->renderTargetLayer[index] = layer;
	}

	void PixelProcessor::setDepthBuffer(Surface *depthBuffer, unsigned int layer)
	{
		context->depthBuffer = depthBuffer;
		context->depthBufferLayer = layer;
	}

	void PixelProcessor::setStencilBuffer(Surface *stencilBuffer, unsigned int layer)
	{
		context->stencilBuffer = stencilBuffer;
		context->stencilBufferLayer = layer;
	}

	void PixelProcessor::setTextureFilter(unsigned int sampler, FilterType textureFilter)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setTextureFilter(textureFilter);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMipmapFilter(mipmapFilter);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setGatherEnable(unsigned int sampler, bool enable)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setGatherEnable(enable);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setAddressingModeU(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setAddressingModeU(addressMode);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setAddressingModeV(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setAddressingModeV(addressMode);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setAddressingModeW(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setAddressingModeW(addressMode);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setReadSRGB(unsigned int sampler, bool sRGB)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setReadSRGB(sRGB);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMipmapLOD(unsigned int sampler, float bias)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMipmapLOD(bias);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setBorderColor(unsigned int sampler, const Color<float> &borderColor)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setBorderColor(borderColor);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMaxAnisotropy(unsigned int sampler, float maxAnisotropy)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMaxAnisotropy(maxAnisotropy);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setHighPrecisionFiltering(unsigned int sampler, bool highPrecisionFiltering)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setHighPrecisionFiltering(highPrecisionFiltering);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setSwizzleR(unsigned int sampler, SwizzleType swizzleR)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setSwizzleR(swizzleR);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setSwizzleG(unsigned int sampler, SwizzleType swizzleG)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setSwizzleG(swizzleG);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setSwizzleB(unsigned int sampler, SwizzleType swizzleB)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setSwizzleB(swizzleB);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setSwizzleA(unsigned int sampler, SwizzleType swizzleA)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setSwizzleA(swizzleA);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setCompareFunc(unsigned int sampler, CompareFunc compFunc)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setCompareFunc(compFunc);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setBaseLevel(unsigned int sampler, int baseLevel)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setBaseLevel(baseLevel);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMaxLevel(unsigned int sampler, int maxLevel)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMaxLevel(maxLevel);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMinLod(unsigned int sampler, float minLod)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMinLod(minLod);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setMaxLod(unsigned int sampler, float maxLod)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setMaxLod(maxLod);
		}
		else ASSERT(false);
	}

	void PixelProcessor::setWriteSRGB(bool sRGB)
	{
		context->setWriteSRGB(sRGB);
	}

	void PixelProcessor::setColorLogicOpEnabled(bool colorLogicOpEnabled)
	{
		context->setColorLogicOpEnabled(colorLogicOpEnabled);
	}

	void PixelProcessor::setLogicalOperation(VkLogicOp logicalOperation)
	{
		context->setLogicalOperation(logicalOperation);
	}

	void PixelProcessor::setDepthBufferEnable(bool depthBufferEnable)
	{
		context->setDepthBufferEnable(depthBufferEnable);
	}

	void PixelProcessor::setDepthCompare(VkCompareOp depthCompareMode)
	{
		context->depthCompareMode = depthCompareMode;
	}

	void PixelProcessor::setAlphaCompare(VkCompareOp alphaCompareMode)
	{
		context->alphaCompareMode = alphaCompareMode;
	}

	void PixelProcessor::setDepthWriteEnable(bool depthWriteEnable)
	{
		context->depthWriteEnable = depthWriteEnable;
	}

	void PixelProcessor::setAlphaTestEnable(bool alphaTestEnable)
	{
		context->alphaTestEnable = alphaTestEnable;
	}

	void PixelProcessor::setCullMode(CullMode cullMode, bool frontFacingCCW)
	{
		context->cullMode = cullMode;
		context->frontFacingCCW = frontFacingCCW;
	}

	void PixelProcessor::setColorWriteMask(int index, int rgbaMask)
	{
		context->setColorWriteMask(index, rgbaMask);
	}

	void PixelProcessor::setStencilEnable(bool stencilEnable)
	{
		context->stencilEnable = stencilEnable;
	}

	void PixelProcessor::setStencilCompare(VkCompareOp stencilCompareMode)
	{
		context->stencilCompareMode = stencilCompareMode;
	}

	void PixelProcessor::setStencilReference(int stencilReference)
	{
		context->stencilReference = stencilReference;
		stencil.set(stencilReference, context->stencilMask, context->stencilWriteMask);
	}

	void PixelProcessor::setStencilReferenceCCW(int stencilReferenceCCW)
	{
		context->stencilReferenceCCW = stencilReferenceCCW;
		stencilCCW.set(stencilReferenceCCW, context->stencilMaskCCW, context->stencilWriteMaskCCW);
	}

	void PixelProcessor::setStencilMask(int stencilMask)
	{
		context->stencilMask = stencilMask;
		stencil.set(context->stencilReference, stencilMask, context->stencilWriteMask);
	}

	void PixelProcessor::setStencilMaskCCW(int stencilMaskCCW)
	{
		context->stencilMaskCCW = stencilMaskCCW;
		stencilCCW.set(context->stencilReferenceCCW, stencilMaskCCW, context->stencilWriteMaskCCW);
	}

	void PixelProcessor::setStencilFailOperation(VkStencilOp stencilFailOperation)
	{
		context->stencilFailOperation = stencilFailOperation;
	}

	void PixelProcessor::setStencilPassOperation(VkStencilOp stencilPassOperation)
	{
		context->stencilPassOperation = stencilPassOperation;
	}

	void PixelProcessor::setStencilZFailOperation(VkStencilOp stencilZFailOperation)
	{
		context->stencilZFailOperation = stencilZFailOperation;
	}

	void PixelProcessor::setStencilWriteMask(int stencilWriteMask)
	{
		context->stencilWriteMask = stencilWriteMask;
		stencil.set(context->stencilReference, context->stencilMask, stencilWriteMask);
	}

	void PixelProcessor::setStencilWriteMaskCCW(int stencilWriteMaskCCW)
	{
		context->stencilWriteMaskCCW = stencilWriteMaskCCW;
		stencilCCW.set(context->stencilReferenceCCW, context->stencilMaskCCW, stencilWriteMaskCCW);
	}

	void PixelProcessor::setTwoSidedStencil(bool enable)
	{
		context->twoSidedStencil = enable;
	}

	void PixelProcessor::setStencilCompareCCW(VkCompareOp stencilCompareMode)
	{
		context->stencilCompareModeCCW = stencilCompareMode;
	}

	void PixelProcessor::setStencilFailOperationCCW(VkStencilOp stencilFailOperation)
	{
		context->stencilFailOperationCCW = stencilFailOperation;
	}

	void PixelProcessor::setStencilPassOperationCCW(VkStencilOp stencilPassOperation)
	{
		context->stencilPassOperationCCW = stencilPassOperation;
	}

	void PixelProcessor::setStencilZFailOperationCCW(VkStencilOp stencilZFailOperation)
	{
		context->stencilZFailOperationCCW = stencilZFailOperation;
	}

	void PixelProcessor::setBlendConstant(const Color<float> &blendConstant)
	{
		// FIXME: Compact into generic function   // FIXME: Clamp
		short blendConstantR = iround(65535 * blendConstant.r);
		short blendConstantG = iround(65535 * blendConstant.g);
		short blendConstantB = iround(65535 * blendConstant.b);
		short blendConstantA = iround(65535 * blendConstant.a);

		factor.blendConstant4W[0][0] = blendConstantR;
		factor.blendConstant4W[0][1] = blendConstantR;
		factor.blendConstant4W[0][2] = blendConstantR;
		factor.blendConstant4W[0][3] = blendConstantR;

		factor.blendConstant4W[1][0] = blendConstantG;
		factor.blendConstant4W[1][1] = blendConstantG;
		factor.blendConstant4W[1][2] = blendConstantG;
		factor.blendConstant4W[1][3] = blendConstantG;

		factor.blendConstant4W[2][0] = blendConstantB;
		factor.blendConstant4W[2][1] = blendConstantB;
		factor.blendConstant4W[2][2] = blendConstantB;
		factor.blendConstant4W[2][3] = blendConstantB;

		factor.blendConstant4W[3][0] = blendConstantA;
		factor.blendConstant4W[3][1] = blendConstantA;
		factor.blendConstant4W[3][2] = blendConstantA;
		factor.blendConstant4W[3][3] = blendConstantA;

		// FIXME: Compact into generic function   // FIXME: Clamp
		short invBlendConstantR = iround(65535 * (1 - blendConstant.r));
		short invBlendConstantG = iround(65535 * (1 - blendConstant.g));
		short invBlendConstantB = iround(65535 * (1 - blendConstant.b));
		short invBlendConstantA = iround(65535 * (1 - blendConstant.a));

		factor.invBlendConstant4W[0][0] = invBlendConstantR;
		factor.invBlendConstant4W[0][1] = invBlendConstantR;
		factor.invBlendConstant4W[0][2] = invBlendConstantR;
		factor.invBlendConstant4W[0][3] = invBlendConstantR;

		factor.invBlendConstant4W[1][0] = invBlendConstantG;
		factor.invBlendConstant4W[1][1] = invBlendConstantG;
		factor.invBlendConstant4W[1][2] = invBlendConstantG;
		factor.invBlendConstant4W[1][3] = invBlendConstantG;

		factor.invBlendConstant4W[2][0] = invBlendConstantB;
		factor.invBlendConstant4W[2][1] = invBlendConstantB;
		factor.invBlendConstant4W[2][2] = invBlendConstantB;
		factor.invBlendConstant4W[2][3] = invBlendConstantB;

		factor.invBlendConstant4W[3][0] = invBlendConstantA;
		factor.invBlendConstant4W[3][1] = invBlendConstantA;
		factor.invBlendConstant4W[3][2] = invBlendConstantA;
		factor.invBlendConstant4W[3][3] = invBlendConstantA;

		factor.blendConstant4F[0][0] = blendConstant.r;
		factor.blendConstant4F[0][1] = blendConstant.r;
		factor.blendConstant4F[0][2] = blendConstant.r;
		factor.blendConstant4F[0][3] = blendConstant.r;

		factor.blendConstant4F[1][0] = blendConstant.g;
		factor.blendConstant4F[1][1] = blendConstant.g;
		factor.blendConstant4F[1][2] = blendConstant.g;
		factor.blendConstant4F[1][3] = blendConstant.g;

		factor.blendConstant4F[2][0] = blendConstant.b;
		factor.blendConstant4F[2][1] = blendConstant.b;
		factor.blendConstant4F[2][2] = blendConstant.b;
		factor.blendConstant4F[2][3] = blendConstant.b;

		factor.blendConstant4F[3][0] = blendConstant.a;
		factor.blendConstant4F[3][1] = blendConstant.a;
		factor.blendConstant4F[3][2] = blendConstant.a;
		factor.blendConstant4F[3][3] = blendConstant.a;

		factor.invBlendConstant4F[0][0] = 1 - blendConstant.r;
		factor.invBlendConstant4F[0][1] = 1 - blendConstant.r;
		factor.invBlendConstant4F[0][2] = 1 - blendConstant.r;
		factor.invBlendConstant4F[0][3] = 1 - blendConstant.r;

		factor.invBlendConstant4F[1][0] = 1 - blendConstant.g;
		factor.invBlendConstant4F[1][1] = 1 - blendConstant.g;
		factor.invBlendConstant4F[1][2] = 1 - blendConstant.g;
		factor.invBlendConstant4F[1][3] = 1 - blendConstant.g;

		factor.invBlendConstant4F[2][0] = 1 - blendConstant.b;
		factor.invBlendConstant4F[2][1] = 1 - blendConstant.b;
		factor.invBlendConstant4F[2][2] = 1 - blendConstant.b;
		factor.invBlendConstant4F[2][3] = 1 - blendConstant.b;

		factor.invBlendConstant4F[3][0] = 1 - blendConstant.a;
		factor.invBlendConstant4F[3][1] = 1 - blendConstant.a;
		factor.invBlendConstant4F[3][2] = 1 - blendConstant.a;
		factor.invBlendConstant4F[3][3] = 1 - blendConstant.a;
	}

	void PixelProcessor::setAlphaBlendEnable(bool alphaBlendEnable)
	{
		context->setAlphaBlendEnable(alphaBlendEnable);
	}

	void PixelProcessor::setSourceBlendFactor(VkBlendFactor sourceBlendFactor)
	{
		context->setSourceBlendFactor(sourceBlendFactor);
	}

	void PixelProcessor::setDestBlendFactor(VkBlendFactor destBlendFactor)
	{
		context->setDestBlendFactor(destBlendFactor);
	}

	void PixelProcessor::setBlendOperation(VkBlendOp blendOperation)
	{
		context->setBlendOperation(blendOperation);
	}

	void PixelProcessor::setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable)
	{
		context->setSeparateAlphaBlendEnable(separateAlphaBlendEnable);
	}

	void PixelProcessor::setSourceBlendFactorAlpha(VkBlendFactor sourceBlendFactorAlpha)
	{
		context->setSourceBlendFactorAlpha(sourceBlendFactorAlpha);
	}

	void PixelProcessor::setDestBlendFactorAlpha(VkBlendFactor destBlendFactorAlpha)
	{
		context->setDestBlendFactorAlpha(destBlendFactorAlpha);
	}

	void PixelProcessor::setBlendOperationAlpha(VkBlendOp blendOperationAlpha)
	{
		context->setBlendOperationAlpha(blendOperationAlpha);
	}

	void PixelProcessor::setAlphaReference(float alphaReference)
	{
		context->alphaReference = alphaReference;

		factor.alphaReference4[0] = (word)iround(alphaReference * 0x1000 / 0xFF);
		factor.alphaReference4[1] = (word)iround(alphaReference * 0x1000 / 0xFF);
		factor.alphaReference4[2] = (word)iround(alphaReference * 0x1000 / 0xFF);
		factor.alphaReference4[3] = (word)iround(alphaReference * 0x1000 / 0xFF);
	}

	void PixelProcessor::setPerspectiveCorrection(bool perspectiveEnable)
	{
		perspectiveCorrection = perspectiveEnable;
	}

	void PixelProcessor::setOcclusionEnabled(bool enable)
	{
		context->occlusionEnabled = enable;
	}

	void PixelProcessor::setRoutineCacheSize(int cacheSize)
	{
		delete routineCache;
		routineCache = new RoutineCache<State>(clamp(cacheSize, 1, 65536), precachePixel ? "sw-pixel" : 0);
	}

	const PixelProcessor::State PixelProcessor::update() const
	{
		State state;

		if(context->pixelShader)
		{
			state.shaderID = context->pixelShader->getSerialID();
		}
		else
		{
			state.shaderID = 0;
		}

		state.depthOverride = context->pixelShader && context->pixelShader->depthOverride();
		state.shaderContainsKill = context->pixelShader ? context->pixelShader->containsKill() : false;

		if(context->alphaTestActive())
		{
			state.alphaCompareMode = context->alphaCompareMode;

			state.transparencyAntialiasing = context->getMultiSampleCount() > 1 ? transparencyAntialiasing : TRANSPARENCY_NONE;
		}

		state.depthWriteEnable = context->depthWriteActive();

		if(context->stencilActive())
		{
			state.stencilActive = true;
			state.stencilCompareMode = context->stencilCompareMode;
			state.stencilFailOperation = context->stencilFailOperation;
			state.stencilPassOperation = context->stencilPassOperation;
			state.stencilZFailOperation = context->stencilZFailOperation;
			state.noStencilMask = (context->stencilMask == 0xFF);
			state.noStencilWriteMask = (context->stencilWriteMask == 0xFF);
			state.stencilWriteMasked = (context->stencilWriteMask == 0x00);

			state.twoSidedStencil = context->twoSidedStencil;
			state.stencilCompareModeCCW = context->twoSidedStencil ? context->stencilCompareModeCCW : state.stencilCompareMode;
			state.stencilFailOperationCCW = context->twoSidedStencil ? context->stencilFailOperationCCW : state.stencilFailOperation;
			state.stencilPassOperationCCW = context->twoSidedStencil ? context->stencilPassOperationCCW : state.stencilPassOperation;
			state.stencilZFailOperationCCW = context->twoSidedStencil ? context->stencilZFailOperationCCW : state.stencilZFailOperation;
			state.noStencilMaskCCW = context->twoSidedStencil ? (context->stencilMaskCCW == 0xFF) : state.noStencilMask;
			state.noStencilWriteMaskCCW = context->twoSidedStencil ? (context->stencilWriteMaskCCW == 0xFF) : state.noStencilWriteMask;
			state.stencilWriteMaskedCCW = context->twoSidedStencil ? (context->stencilWriteMaskCCW == 0x00) : state.stencilWriteMasked;
		}

		if(context->depthBufferActive())
		{
			state.depthTestActive = true;
			state.depthCompareMode = context->depthCompareMode;
			state.quadLayoutDepthBuffer = Surface::hasQuadLayout(context->depthBuffer->getInternalFormat());
		}

		state.occlusionEnabled = context->occlusionEnabled;

		state.perspective = context->perspectiveActive();
		state.depthClamp = (context->depthBias != 0.0f) || (context->slopeDepthBias != 0.0f);

		if(context->alphaBlendActive())
		{
			state.alphaBlendActive = true;
			state.sourceBlendFactor = context->sourceBlendFactor();
			state.destBlendFactor = context->destBlendFactor();
			state.blendOperation = context->blendOperation();
			state.sourceBlendFactorAlpha = context->sourceBlendFactorAlpha();
			state.destBlendFactorAlpha = context->destBlendFactorAlpha();
			state.blendOperationAlpha = context->blendOperationAlpha();
		}

		state.logicalOperation = context->colorLogicOp();

		for(int i = 0; i < RENDERTARGETS; i++)
		{
			state.colorWriteMask |= context->colorWriteActive(i) << (4 * i);
			state.targetFormat[i] = context->renderTargetInternalFormat(i);
		}

		state.writeSRGB	= context->writeSRGB && context->renderTarget[0] && Surface::isSRGBwritable(context->renderTarget[0]->getExternalFormat());
		state.multiSample = context->getMultiSampleCount();
		state.multiSampleMask = context->multiSampleMask;

		if(state.multiSample > 1 && context->pixelShader)
		{
			state.centroid = context->pixelShader->containsCentroid();
		}

		state.frontFaceCCW = context->frontFacingCCW;


		for(unsigned int i = 0; i < 16; i++)
		{
			if(context->pixelShader)
			{
				if(context->pixelShader->usesSampler(i))
				{
					state.sampler[i] = context->sampler[i].samplerState();
				}
			}
		}

		const bool point = context->isDrawPoint();

		for(int interpolant = 0; interpolant < MAX_FRAGMENT_INPUTS; interpolant++)
		{
			for(int component = 0; component < 4; component++)
			{
				const Shader::Semantic &semantic = context->pixelShader->getInput(interpolant, component);

				if(semantic.active())
				{
					bool flat = point;

					switch(semantic.usage)
					{
					case Shader::USAGE_TEXCOORD: flat = false;                  break;
					case Shader::USAGE_COLOR:    flat = semantic.flat || point; break;
					}

					state.interpolant[interpolant].component |= 1 << component;

					if(flat)
					{
						state.interpolant[interpolant].flat |= 1 << component;
					}
				}
			}
		}

		if(state.centroid)
		{
			for(int interpolant = 0; interpolant < MAX_FRAGMENT_INPUTS; interpolant++)
			{
				for(int component = 0; component < 4; component++)
				{
					state.interpolant[interpolant].centroid = context->pixelShader->getInput(interpolant, 0).centroid;
				}
			}
		}

		state.hash = state.computeHash();

		return state;
	}

	Routine *PixelProcessor::routine(const State &state)
	{
		Routine *routine = routineCache->query(state);

		if(!routine)
		{
			const bool integerPipeline = (context->pixelShaderModel() <= 0x0104);
			QuadRasterizer *generator = new PixelProgram(state, context->pixelShader);
			generator->generate();
			routine = (*generator)(L"PixelRoutine_%0.8X", state.shaderID);
			delete generator;

			routineCache->add(state, routine);
		}

		return routine;
	}
}
