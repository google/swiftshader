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

#include "Context.hpp"

#include "Primitive.hpp"
#include "Surface.hpp"
#include "Pipeline/PixelShader.hpp"
#include "Pipeline/VertexShader.hpp"
#include "System/Memory.hpp"
#include "System/Debug.hpp"

#include <string.h>

namespace sw
{
	extern bool perspectiveCorrection;

	bool halfIntegerCoordinates = false;     // Pixel centers are not at integer coordinates
	bool symmetricNormalizedDepth = false;   // [-1, 1] instead of [0, 1]
	bool booleanFaceRegister = false;
	bool fullPixelPositionRegister = false;
	bool leadingVertexFirst = false;         // Flat shading uses first vertex, else last
	bool secondaryColor = false;             // Specular lighting is applied after texturing
	bool colorsDefaultToZero = false;

	bool forceWindowed = false;
	bool quadLayoutEnabled = false;
	bool veryEarlyDepthTest = true;
	bool complementaryDepthBuffer = false;
	bool postBlendSRGB = false;
	bool exactColorRounding = false;
	TransparencyAntialiasing transparencyAntialiasing = TRANSPARENCY_NONE;
	bool forceClearRegisters = false;

	Context::Context()
	{
		init();
	}

	Context::~Context()
	{
	}

	void *Context::operator new(size_t bytes)
	{
		return allocate((unsigned int)bytes);
	}

	void Context::operator delete(void *pointer, size_t bytes)
	{
		deallocate(pointer);
	}

	bool Context::isDrawPoint() const
	{
		switch(drawType)
		{
		case DRAW_POINTLIST:
		case DRAW_INDEXEDPOINTLIST16:
		case DRAW_INDEXEDPOINTLIST32:
			return true;
		case DRAW_LINELIST:
		case DRAW_LINESTRIP:
		case DRAW_INDEXEDLINELIST16:
		case DRAW_INDEXEDLINESTRIP16:
		case DRAW_INDEXEDLINELIST32:
		case DRAW_INDEXEDLINESTRIP32:
			return false;
		case DRAW_TRIANGLELIST:
		case DRAW_TRIANGLESTRIP:
		case DRAW_TRIANGLEFAN:
		case DRAW_INDEXEDTRIANGLELIST16:
		case DRAW_INDEXEDTRIANGLESTRIP16:
		case DRAW_INDEXEDTRIANGLEFAN16:
		case DRAW_INDEXEDTRIANGLELIST32:
		case DRAW_INDEXEDTRIANGLESTRIP32:
		case DRAW_INDEXEDTRIANGLEFAN32:
			return false;
		default:
			ASSERT(false);
		}

		return false;
	}

	bool Context::isDrawLine() const
	{
		switch(drawType)
		{
		case DRAW_POINTLIST:
		case DRAW_INDEXEDPOINTLIST16:
		case DRAW_INDEXEDPOINTLIST32:
			return false;
		case DRAW_LINELIST:
		case DRAW_LINESTRIP:
		case DRAW_INDEXEDLINELIST16:
		case DRAW_INDEXEDLINESTRIP16:
		case DRAW_INDEXEDLINELIST32:
		case DRAW_INDEXEDLINESTRIP32:
			return true;
		case DRAW_TRIANGLELIST:
		case DRAW_TRIANGLESTRIP:
		case DRAW_TRIANGLEFAN:
		case DRAW_INDEXEDTRIANGLELIST16:
		case DRAW_INDEXEDTRIANGLESTRIP16:
		case DRAW_INDEXEDTRIANGLEFAN16:
		case DRAW_INDEXEDTRIANGLELIST32:
		case DRAW_INDEXEDTRIANGLESTRIP32:
		case DRAW_INDEXEDTRIANGLEFAN32:
			return false;
		default:
			ASSERT(false);
		}

		return false;
	}

	bool Context::isDrawTriangle() const
	{
		switch(drawType)
		{
		case DRAW_POINTLIST:
		case DRAW_INDEXEDPOINTLIST16:
		case DRAW_INDEXEDPOINTLIST32:
			return false;
		case DRAW_LINELIST:
		case DRAW_LINESTRIP:
		case DRAW_INDEXEDLINELIST16:
		case DRAW_INDEXEDLINESTRIP16:
		case DRAW_INDEXEDLINELIST32:
		case DRAW_INDEXEDLINESTRIP32:
			return false;
		case DRAW_TRIANGLELIST:
		case DRAW_TRIANGLESTRIP:
		case DRAW_TRIANGLEFAN:
		case DRAW_INDEXEDTRIANGLELIST16:
		case DRAW_INDEXEDTRIANGLESTRIP16:
		case DRAW_INDEXEDTRIANGLEFAN16:
		case DRAW_INDEXEDTRIANGLELIST32:
		case DRAW_INDEXEDTRIANGLESTRIP32:
		case DRAW_INDEXEDTRIANGLEFAN32:
			return true;
		default:
			ASSERT(false);
		}

		return true;
	}

	void Context::init()
	{
		// Set vertex streams to null stream
		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			input[i].defaults();
		}

		for(int i = 0; i < RENDERTARGETS; ++i)
		{
			renderTarget[i] = nullptr;
		}
		depthBuffer = nullptr;
		stencilBuffer = nullptr;

		stencilEnable = false;
		stencilCompareMode = VK_COMPARE_OP_ALWAYS;
		stencilReference = 0;
		stencilMask = 0xFFFFFFFF;
		stencilFailOperation = VK_STENCIL_OP_KEEP;
		stencilPassOperation = VK_STENCIL_OP_KEEP;
		stencilZFailOperation = VK_STENCIL_OP_KEEP;
		stencilWriteMask = 0xFFFFFFFF;

		twoSidedStencil = false;
		stencilCompareModeCCW = VK_COMPARE_OP_ALWAYS;
		stencilReferenceCCW = 0;
		stencilMaskCCW = 0xFFFFFFFF;
		stencilFailOperationCCW = VK_STENCIL_OP_KEEP;
		stencilPassOperationCCW = VK_STENCIL_OP_KEEP;
		stencilZFailOperationCCW = VK_STENCIL_OP_KEEP;
		stencilWriteMaskCCW = 0xFFFFFFFF;

		alphaCompareMode = VK_COMPARE_OP_ALWAYS;
		alphaTestEnable = false;

		rasterizerDiscard = false;

		depthCompareMode = VK_COMPARE_OP_LESS;
		depthBufferEnable = true;
		depthWriteEnable = true;

		alphaBlendEnable = false;
		sourceBlendFactorState = BLEND_ONE;
		destBlendFactorState = BLEND_ZERO;
		blendOperationState = BLENDOP_ADD;

		separateAlphaBlendEnable = false;
		sourceBlendFactorStateAlpha = BLEND_ONE;
		destBlendFactorStateAlpha = BLEND_ZERO;
		blendOperationStateAlpha = BLENDOP_ADD;

		cullMode = CULL_CLOCKWISE;
		frontFacingCCW = true;
		alphaReference = 0.0f;

		depthBias = 0.0f;
		slopeDepthBias = 0.0f;

		for(int i = 0; i < RENDERTARGETS; i++)
		{
			colorWriteMask[i] = 0x0000000F;
		}

		pixelShader = nullptr;
		vertexShader = nullptr;

		instanceID = 0;

		occlusionEnabled = false;
		transformFeedbackQueryEnabled = false;
		transformFeedbackEnabled = 0;

		lineWidth = 1.0f;

		writeSRGB = false;
		sampleMask = 0xFFFFFFFF;

		colorLogicOpEnabled = false;
		logicalOperation = VK_LOGIC_OP_COPY;
	}

	bool Context::setDepthBufferEnable(bool depthBufferEnable)
	{
		bool modified = (Context::depthBufferEnable != depthBufferEnable);
		Context::depthBufferEnable = depthBufferEnable;
		return modified;
	}

	bool Context::setAlphaBlendEnable(bool alphaBlendEnable)
	{
		bool modified = (Context::alphaBlendEnable != alphaBlendEnable);
		Context::alphaBlendEnable = alphaBlendEnable;
		return modified;
	}

	bool Context::setSourceBlendFactor(BlendFactor sourceBlendFactor)
	{
		bool modified = (Context::sourceBlendFactorState != sourceBlendFactor);
		Context::sourceBlendFactorState = sourceBlendFactor;
		return modified;
	}

	bool Context::setDestBlendFactor(BlendFactor destBlendFactor)
	{
		bool modified = (Context::destBlendFactorState != destBlendFactor);
		Context::destBlendFactorState = destBlendFactor;
		return modified;
	}

	bool Context::setBlendOperation(BlendOperation blendOperation)
	{
		bool modified = (Context::blendOperationState != blendOperation);
		Context::blendOperationState = blendOperation;
		return modified;
	}

	bool Context::setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable)
	{
		bool modified = (Context::separateAlphaBlendEnable != separateAlphaBlendEnable);
		Context::separateAlphaBlendEnable = separateAlphaBlendEnable;
		return modified;
	}

	bool Context::setSourceBlendFactorAlpha(BlendFactor sourceBlendFactorAlpha)
	{
		bool modified = (Context::sourceBlendFactorStateAlpha != sourceBlendFactorAlpha);
		Context::sourceBlendFactorStateAlpha = sourceBlendFactorAlpha;
		return modified;
	}

	bool Context::setDestBlendFactorAlpha(BlendFactor destBlendFactorAlpha)
	{
		bool modified = (Context::destBlendFactorStateAlpha != destBlendFactorAlpha);
		Context::destBlendFactorStateAlpha = destBlendFactorAlpha;
		return modified;
	}

	bool Context::setBlendOperationAlpha(BlendOperation blendOperationAlpha)
	{
		bool modified = (Context::blendOperationStateAlpha != blendOperationAlpha);
		Context::blendOperationStateAlpha = blendOperationAlpha;
		return modified;
	}

	bool Context::setColorWriteMask(int index, int colorWriteMask)
	{
		bool modified = (Context::colorWriteMask[index] != colorWriteMask);
		Context::colorWriteMask[index] = colorWriteMask;
		return modified;
	}

	bool Context::setWriteSRGB(bool sRGB)
	{
		bool modified = (Context::writeSRGB != sRGB);
		Context::writeSRGB = sRGB;
		return modified;
	}

	bool Context::setColorLogicOpEnabled(bool enabled)
	{
		bool modified = (Context::colorLogicOpEnabled != enabled);
		Context::colorLogicOpEnabled = enabled;
		return modified;
	}

	bool Context::setLogicalOperation(VkLogicOp logicalOperation)
	{
		bool modified = (Context::logicalOperation != logicalOperation);
		Context::logicalOperation = logicalOperation;
		return modified;
	}

	bool Context::depthWriteActive()
	{
		if(!depthBufferActive()) return false;

		return depthWriteEnable;
	}

	bool Context::alphaTestActive()
	{
		if(transparencyAntialiasing != TRANSPARENCY_NONE) return true;
		if(!alphaTestEnable) return false;
		if(alphaCompareMode == VK_COMPARE_OP_ALWAYS) return false;
		if(alphaReference == 0.0f && alphaCompareMode == VK_COMPARE_OP_GREATER_OR_EQUAL) return false;

		return true;
	}

	bool Context::depthBufferActive()
	{
		return depthBuffer && depthBufferEnable;
	}

	bool Context::stencilActive()
	{
		return stencilBuffer && stencilEnable;
	}

	bool Context::alphaBlendActive()
	{
		if(!alphaBlendEnable)
		{
			return false;
		}

		if(!colorUsed())
		{
			return false;
		}

		bool colorBlend = !(blendOperation() == BLENDOP_SOURCE && sourceBlendFactor() == BLEND_ONE);
		bool alphaBlend = separateAlphaBlendEnable ? !(blendOperationAlpha() == BLENDOP_SOURCE && sourceBlendFactorAlpha() == BLEND_ONE) : colorBlend;

		return colorBlend || alphaBlend;
	}

	VkLogicOp Context::colorLogicOp()
	{
		return colorLogicOpEnabled ? logicalOperation : VK_LOGIC_OP_COPY;
	}

	BlendFactor Context::sourceBlendFactor()
	{
		if(!alphaBlendEnable) return BLEND_ONE;

		switch(blendOperationState)
		{
		case BLENDOP_ADD:
		case BLENDOP_SUB:
		case BLENDOP_INVSUB:
			return sourceBlendFactorState;
		case BLENDOP_MIN:
			return BLEND_ONE;
		case BLENDOP_MAX:
			return BLEND_ONE;
		default:
			ASSERT(false);
		}

		return sourceBlendFactorState;
	}

	BlendFactor Context::destBlendFactor()
	{
		if(!alphaBlendEnable) return BLEND_ZERO;

		switch(blendOperationState)
		{
		case BLENDOP_ADD:
		case BLENDOP_SUB:
		case BLENDOP_INVSUB:
			return destBlendFactorState;
		case BLENDOP_MIN:
			return BLEND_ONE;
		case BLENDOP_MAX:
			return BLEND_ONE;
		default:
			ASSERT(false);
		}

		return destBlendFactorState;
	}

	BlendOperation Context::blendOperation()
	{
		if(!alphaBlendEnable) return BLENDOP_SOURCE;

		switch(blendOperationState)
		{
		case BLENDOP_ADD:
			if(sourceBlendFactor() == BLEND_ZERO)
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_NULL;
				}
				else
				{
					return BLENDOP_DEST;
				}
			}
			else if(sourceBlendFactor() == BLEND_ONE)
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_SOURCE;
				}
				else
				{
					return BLENDOP_ADD;
				}
			}
			else
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_SOURCE;
				}
				else
				{
					return BLENDOP_ADD;
				}
			}
		case BLENDOP_SUB:
			if(sourceBlendFactor() == BLEND_ZERO)
			{
				return BLENDOP_NULL;   // Negative, clamped to zero
			}
			else if(sourceBlendFactor() == BLEND_ONE)
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_SOURCE;
				}
				else
				{
					return BLENDOP_SUB;
				}
			}
			else
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_SOURCE;
				}
				else
				{
					return BLENDOP_SUB;
				}
			}
		case BLENDOP_INVSUB:
			if(sourceBlendFactor() == BLEND_ZERO)
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_NULL;
				}
				else
				{
					return BLENDOP_DEST;
				}
			}
			else if(sourceBlendFactor() == BLEND_ONE)
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_NULL;   // Negative, clamped to zero
				}
				else
				{
					return BLENDOP_INVSUB;
				}
			}
			else
			{
				if(destBlendFactor() == BLEND_ZERO)
				{
					return BLENDOP_NULL;   // Negative, clamped to zero
				}
				else
				{
					return BLENDOP_INVSUB;
				}
			}
		case BLENDOP_MIN:
			return BLENDOP_MIN;
		case BLENDOP_MAX:
			return BLENDOP_MAX;
		default:
			ASSERT(false);
		}

		return blendOperationState;
	}

	BlendFactor Context::sourceBlendFactorAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return sourceBlendFactor();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case BLENDOP_ADD:
			case BLENDOP_SUB:
			case BLENDOP_INVSUB:
				return sourceBlendFactorStateAlpha;
			case BLENDOP_MIN:
				return BLEND_ONE;
			case BLENDOP_MAX:
				return BLEND_ONE;
			default:
				ASSERT(false);
			}

			return sourceBlendFactorStateAlpha;
		}
	}

	BlendFactor Context::destBlendFactorAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return destBlendFactor();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case BLENDOP_ADD:
			case BLENDOP_SUB:
			case BLENDOP_INVSUB:
				return destBlendFactorStateAlpha;
			case BLENDOP_MIN:
				return BLEND_ONE;
			case BLENDOP_MAX:
				return BLEND_ONE;
			default:
				ASSERT(false);
			}

			return destBlendFactorStateAlpha;
		}
	}

	BlendOperation Context::blendOperationAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return blendOperation();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case BLENDOP_ADD:
				if(sourceBlendFactorAlpha() == BLEND_ZERO)
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_NULL;
					}
					else
					{
						return BLENDOP_DEST;
					}
				}
				else if(sourceBlendFactorAlpha() == BLEND_ONE)
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_SOURCE;
					}
					else
					{
						return BLENDOP_ADD;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_SOURCE;
					}
					else
					{
						return BLENDOP_ADD;
					}
				}
			case BLENDOP_SUB:
				if(sourceBlendFactorAlpha() == BLEND_ZERO)
				{
					return BLENDOP_NULL;   // Negative, clamped to zero
				}
				else if(sourceBlendFactorAlpha() == BLEND_ONE)
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_SOURCE;
					}
					else
					{
						return BLENDOP_SUB;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_SOURCE;
					}
					else
					{
						return BLENDOP_SUB;
					}
				}
			case BLENDOP_INVSUB:
				if(sourceBlendFactorAlpha() == BLEND_ZERO)
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_NULL;
					}
					else
					{
						return BLENDOP_DEST;
					}
				}
				else if(sourceBlendFactorAlpha() == BLEND_ONE)
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_NULL;   // Negative, clamped to zero
					}
					else
					{
						return BLENDOP_INVSUB;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == BLEND_ZERO)
					{
						return BLENDOP_NULL;   // Negative, clamped to zero
					}
					else
					{
						return BLENDOP_INVSUB;
					}
				}
			case BLENDOP_MIN:
				return BLENDOP_MIN;
			case BLENDOP_MAX:
				return BLENDOP_MAX;
			default:
				ASSERT(false);
			}

			return blendOperationStateAlpha;
		}
	}

	bool Context::perspectiveActive()
	{
		if(!colorUsed())
		{
			return false;
		}

		if(!perspectiveCorrection)
		{
			return false;
		}

		if(isDrawPoint())
		{
			return false;
		}

		return true;
	}

	unsigned short Context::pixelShaderModel() const
	{
		return pixelShader ? pixelShader->getShaderModel() : 0x0000;
	}

	unsigned short Context::vertexShaderModel() const
	{
		return vertexShader ? vertexShader->getShaderModel() : 0x0000;
	}

	int Context::getMultiSampleCount() const
	{
		return renderTarget[0] ? renderTarget[0]->getMultiSampleCount() : 1;
	}

	VkFormat Context::renderTargetInternalFormat(int index)
	{
		if(renderTarget[index])
		{
			return renderTarget[index]->getInternalFormat();
		}
		else
		{
			return VK_FORMAT_UNDEFINED;
		}
	}

	int Context::colorWriteActive()
	{
		return colorWriteActive(0) | colorWriteActive(1) | colorWriteActive(2) | colorWriteActive(3);
	}

	int Context::colorWriteActive(int index)
	{
		if(!renderTarget[index] || renderTarget[index]->getInternalFormat() == VK_FORMAT_UNDEFINED)
		{
			return 0;
		}

		if(blendOperation() == BLENDOP_DEST && destBlendFactor() == BLEND_ONE &&
		   (!separateAlphaBlendEnable || (blendOperationAlpha() == BLENDOP_DEST && destBlendFactorAlpha() == BLEND_ONE)))
		{
			return 0;
		}

		return colorWriteMask[index];
	}

	bool Context::colorUsed()
	{
		return colorWriteActive() || alphaTestActive() || (pixelShader && pixelShader->containsKill());
	}
}
