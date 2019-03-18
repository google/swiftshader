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
#include "System/Memory.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Pipeline/SpirvShader.hpp"

#include <string.h>

namespace sw
{
	extern bool perspectiveCorrection;

	bool booleanFaceRegister = false;
	bool fullPixelPositionRegister = false;
	bool leadingVertexFirst = false;         // Flat shading uses first vertex, else last
	bool secondaryColor = false;             // Specular lighting is applied after texturing
	bool colorsDefaultToZero = false;

	bool forceWindowed = false;
	bool quadLayoutEnabled = false;
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
		twoSidedStencil = false;
		frontStencil = {};
		backStencil = {};

		rasterizerDiscard = false;

		depthCompareMode = VK_COMPARE_OP_LESS;
		depthBufferEnable = false;
		depthWriteEnable = false;

		alphaBlendEnable = false;
		sourceBlendFactorState = VK_BLEND_FACTOR_ONE;
		destBlendFactorState = VK_BLEND_FACTOR_ZERO;
		blendOperationState = VK_BLEND_OP_ADD;

		separateAlphaBlendEnable = false;
		sourceBlendFactorStateAlpha = VK_BLEND_FACTOR_ONE;
		destBlendFactorStateAlpha = VK_BLEND_FACTOR_ZERO;
		blendOperationStateAlpha = VK_BLEND_OP_ADD;

		cullMode = CULL_CLOCKWISE;
		frontFacingCCW = true;
		alphaReference = 0.0f;

		depthBias = 0.0f;
		slopeDepthBias = 0.0f;

		for(int i = 0; i < RENDERTARGETS; i++)
		{
			colorWriteMask[i] = 0x0000000F;
		}

		pipelineLayout = nullptr;

		pixelShader = nullptr;
		vertexShader = nullptr;

		instanceID = 0;

		occlusionEnabled = false;

		lineWidth = 1.0f;

		writeSRGB = false;
		sampleMask = 0xFFFFFFFF;
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

	bool Context::setSourceBlendFactor(VkBlendFactor sourceBlendFactor)
	{
		bool modified = (Context::sourceBlendFactorState != sourceBlendFactor);
		Context::sourceBlendFactorState = sourceBlendFactor;
		return modified;
	}

	bool Context::setDestBlendFactor(VkBlendFactor destBlendFactor)
	{
		bool modified = (Context::destBlendFactorState != destBlendFactor);
		Context::destBlendFactorState = destBlendFactor;
		return modified;
	}

	bool Context::setBlendOperation(VkBlendOp blendOperation)
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

	bool Context::setSourceBlendFactorAlpha(VkBlendFactor sourceBlendFactorAlpha)
	{
		bool modified = (Context::sourceBlendFactorStateAlpha != sourceBlendFactorAlpha);
		Context::sourceBlendFactorStateAlpha = sourceBlendFactorAlpha;
		return modified;
	}

	bool Context::setDestBlendFactorAlpha(VkBlendFactor destBlendFactorAlpha)
	{
		bool modified = (Context::destBlendFactorStateAlpha != destBlendFactorAlpha);
		Context::destBlendFactorStateAlpha = destBlendFactorAlpha;
		return modified;
	}

	bool Context::setBlendOperationAlpha(VkBlendOp blendOperationAlpha)
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

	bool Context::depthWriteActive()
	{
		if(!depthBufferActive()) return false;

		return depthWriteEnable;
	}

	bool Context::alphaTestActive()
	{
		return transparencyAntialiasing != TRANSPARENCY_NONE;
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

		bool colorBlend = !(blendOperation() == VK_BLEND_OP_SRC_EXT && sourceBlendFactor() == VK_BLEND_FACTOR_ONE);
		bool alphaBlend = separateAlphaBlendEnable ? !(blendOperationAlpha() == VK_BLEND_OP_SRC_EXT && sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE) : colorBlend;

		return colorBlend || alphaBlend;
	}

	VkBlendFactor Context::sourceBlendFactor()
	{
		if(!alphaBlendEnable) return VK_BLEND_FACTOR_ONE;

		switch(blendOperationState)
		{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return sourceBlendFactorState;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
		}

		return sourceBlendFactorState;
	}

	VkBlendFactor Context::destBlendFactor()
	{
		if(!alphaBlendEnable) return VK_BLEND_FACTOR_ONE;

		switch(blendOperationState)
		{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return destBlendFactorState;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
		}

		return destBlendFactorState;
	}

	VkBlendOp Context::blendOperation()
	{
		if(!alphaBlendEnable) return VK_BLEND_OP_SRC_EXT;

		switch(blendOperationState)
		{
		case VK_BLEND_OP_ADD:
			if(sourceBlendFactor() == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactor() == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_ADD;
				}
			}
			else
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_ADD;
				}
			}
		case VK_BLEND_OP_SUBTRACT:
			if(sourceBlendFactor() == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
			}
			else if(sourceBlendFactor() == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_SUBTRACT;
				}
			}
			else
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_SUBTRACT;
				}
			}
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			if(sourceBlendFactor() == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactor() == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
				}
				else
				{
					return VK_BLEND_OP_REVERSE_SUBTRACT;
				}
			}
			else
			{
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
				}
				else
				{
					return VK_BLEND_OP_REVERSE_SUBTRACT;
				}
			}
		case VK_BLEND_OP_MIN:
			return VK_BLEND_OP_MIN;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_OP_MAX;
		default:
			ASSERT(false);
		}

		return blendOperationState;
	}

	VkBlendFactor Context::sourceBlendFactorAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return sourceBlendFactor();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case VK_BLEND_OP_ADD:
			case VK_BLEND_OP_SUBTRACT:
			case VK_BLEND_OP_REVERSE_SUBTRACT:
				return sourceBlendFactorStateAlpha;
			case VK_BLEND_OP_MIN:
				return VK_BLEND_FACTOR_ONE;
			case VK_BLEND_OP_MAX:
				return VK_BLEND_FACTOR_ONE;
			default:
				ASSERT(false);
			}

			return sourceBlendFactorStateAlpha;
		}
	}

	VkBlendFactor Context::destBlendFactorAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return destBlendFactor();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case VK_BLEND_OP_ADD:
			case VK_BLEND_OP_SUBTRACT:
			case VK_BLEND_OP_REVERSE_SUBTRACT:
				return destBlendFactorStateAlpha;
			case VK_BLEND_OP_MIN:
				return VK_BLEND_FACTOR_ONE;
			case VK_BLEND_OP_MAX:
				return VK_BLEND_FACTOR_ONE;
			default:
				ASSERT(false);
			}

			return destBlendFactorStateAlpha;
		}
	}

	VkBlendOp Context::blendOperationAlpha()
	{
		if(!separateAlphaBlendEnable)
		{
			return blendOperation();
		}
		else
		{
			switch(blendOperationStateAlpha)
			{
			case VK_BLEND_OP_ADD:
				if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_ZERO_EXT;
					}
					else
					{
						return VK_BLEND_OP_DST_EXT;
					}
				}
				else if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_SRC_EXT;
					}
					else
					{
						return VK_BLEND_OP_ADD;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_SRC_EXT;
					}
					else
					{
						return VK_BLEND_OP_ADD;
					}
				}
			case VK_BLEND_OP_SUBTRACT:
				if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
				}
				else if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_SRC_EXT;
					}
					else
					{
						return VK_BLEND_OP_SUBTRACT;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_SRC_EXT;
					}
					else
					{
						return VK_BLEND_OP_SUBTRACT;
					}
				}
			case VK_BLEND_OP_REVERSE_SUBTRACT:
				if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_ZERO_EXT;
					}
					else
					{
						return VK_BLEND_OP_DST_EXT;
					}
				}
				else if(sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
					}
					else
					{
						return VK_BLEND_OP_REVERSE_SUBTRACT;
					}
				}
				else
				{
					if(destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
					{
						return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
					}
					else
					{
						return VK_BLEND_OP_ZERO_EXT;
					}
				}
			case VK_BLEND_OP_MIN:
				return VK_BLEND_OP_MIN;
			case VK_BLEND_OP_MAX:
				return VK_BLEND_OP_MAX;
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

	VkFormat Context::renderTargetInternalFormat(int index)
	{
		if(renderTarget[index])
		{
			return renderTarget[index]->getFormat();
		}
		else
		{
			return VK_FORMAT_UNDEFINED;
		}
	}

	bool Context::colorWriteActive()
	{
		for (int i = 0; i < RENDERTARGETS; i++)
		{
			if (colorWriteActive(i))
			{
				return true;
			}
		}

		return false;
	}

	int Context::colorWriteActive(int index)
	{
		if(!renderTarget[index] || renderTarget[index]->getFormat() == VK_FORMAT_UNDEFINED)
		{
			return 0;
		}

		if(blendOperation() == VK_BLEND_OP_DST_EXT && destBlendFactor() == VK_BLEND_FACTOR_ONE &&
		   (!separateAlphaBlendEnable || (blendOperationAlpha() == VK_BLEND_OP_DST_EXT && destBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)))
		{
			return 0;
		}

		return colorWriteMask[index];
	}

	bool Context::colorUsed()
	{
		return colorWriteActive() || alphaTestActive() || (pixelShader && pixelShader->getModes().ContainsKill);
	}
}
