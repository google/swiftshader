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
#include "Pipeline/SpirvShader.hpp"
#include "System/Debug.hpp"
#include "System/Memory.hpp"
#include "Vulkan/VkImageView.hpp"

#include <string.h>

namespace sw {

Context::Context()
{
	init();
}

bool Context::isDrawPoint(bool polygonModeAware) const
{
	switch(topology)
	{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
			return true;
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			return false;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_POINT) : false;
		default:
			UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

bool Context::isDrawLine(bool polygonModeAware) const
{
	switch(topology)
	{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
			return false;
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			return true;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_LINE) : false;
		default:
			UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

bool Context::isDrawTriangle(bool polygonModeAware) const
{
	switch(topology)
	{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			return false;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_FILL) : true;
		default:
			UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

void Context::init()
{
	for(int i = 0; i < RENDERTARGETS; ++i)
	{
		renderTarget[i] = nullptr;
	}

	depthBuffer = nullptr;
	stencilBuffer = nullptr;

	stencilEnable = false;
	frontStencil = {};
	backStencil = {};

	robustBufferAccess = false;

	rasterizerDiscard = false;

	depthCompareMode = VK_COMPARE_OP_LESS;
	depthBoundsTestEnable = false;
	depthBufferEnable = false;
	depthWriteEnable = false;

	cullMode = VK_CULL_MODE_FRONT_BIT;
	frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	provokingVertexMode = VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT;
	lineRasterizationMode = VK_LINE_RASTERIZATION_MODE_DEFAULT_EXT;

	depthBias = 0.0f;
	slopeDepthBias = 0.0f;

	for(int i = 0; i < RENDERTARGETS; i++)
	{
		colorWriteMask[i] = 0x0000000F;
	}

	pipelineLayout = nullptr;

	pixelShader = nullptr;
	vertexShader = nullptr;

	occlusionEnabled = false;

	lineWidth = 1.0f;

	sampleMask = 0xFFFFFFFF;
	alphaToCoverage = false;
}

bool Context::depthWriteActive() const
{
	if(!depthBufferActive()) return false;

	return depthWriteEnable;
}

bool Context::depthBufferActive() const
{
	return depthBuffer && depthBufferEnable;
}

bool Context::stencilActive() const
{
	return stencilBuffer && stencilEnable;
}

void Context::setBlendState(int index, BlendState state)
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	blendState[index] = state;
}

BlendState Context::getBlendState(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	BlendState activeBlendState;
	activeBlendState.alphaBlendEnable = alphaBlendActive(index);
	activeBlendState.sourceBlendFactor = sourceBlendFactor(index);
	activeBlendState.destBlendFactor = destBlendFactor(index);
	activeBlendState.blendOperation = blendOperation(index);
	activeBlendState.sourceBlendFactorAlpha = sourceBlendFactorAlpha(index);
	activeBlendState.destBlendFactorAlpha = destBlendFactorAlpha(index);
	activeBlendState.blendOperationAlpha = blendOperationAlpha(index);
	return activeBlendState;
}

bool Context::alphaBlendActive(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(!blendState[index].alphaBlendEnable)
	{
		return false;
	}

	if(!colorUsed())
	{
		return false;
	}

	bool colorBlend = !(blendOperation(index) == VK_BLEND_OP_SRC_EXT && sourceBlendFactor(index) == VK_BLEND_FACTOR_ONE);
	bool alphaBlend = !(blendOperationAlpha(index) == VK_BLEND_OP_SRC_EXT && sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ONE);

	return colorBlend || alphaBlend;
}

VkBlendFactor Context::sourceBlendFactor(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(!blendState[index].alphaBlendEnable) return VK_BLEND_FACTOR_ONE;

	switch(blendState[index].blendOperation)
	{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return blendState[index].sourceBlendFactor;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
	}

	return blendState[index].sourceBlendFactor;
}

VkBlendFactor Context::destBlendFactor(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(!blendState[index].alphaBlendEnable) return VK_BLEND_FACTOR_ONE;

	switch(blendState[index].blendOperation)
	{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return blendState[index].destBlendFactor;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
	}

	return blendState[index].destBlendFactor;
}

bool Context::allTargetsColorClamp() const
{
	// TODO: remove all of this and support VkPhysicalDeviceFeatures::independentBlend instead
	for(int i = 0; i < RENDERTARGETS; i++)
	{
		if(renderTarget[i] && renderTarget[i]->getFormat().isFloatFormat())
		{
			return false;
		}
	}

	return true;
}

VkBlendOp Context::blendOperation(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(!blendState[index].alphaBlendEnable) return VK_BLEND_OP_SRC_EXT;

	switch(blendState[index].blendOperation)
	{
		case VK_BLEND_OP_ADD:
			if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
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
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_ADD;
				}
			}
		case VK_BLEND_OP_SUBTRACT:
			if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
			{
				return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
			}
			else if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
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
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_SUBTRACT;
				}
			}
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactor(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
				{
					return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
				}
				else
				{
					return VK_BLEND_OP_REVERSE_SUBTRACT;
				}
			}
			else
			{
				if(destBlendFactor(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
				{
					return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
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

	return blendState[index].blendOperation;
}

VkBlendFactor Context::sourceBlendFactorAlpha(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	switch(blendState[index].blendOperationAlpha)
	{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return blendState[index].sourceBlendFactorAlpha;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
	}

	return blendState[index].sourceBlendFactorAlpha;
}

VkBlendFactor Context::destBlendFactorAlpha(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	switch(blendState[index].blendOperationAlpha)
	{
		case VK_BLEND_OP_ADD:
		case VK_BLEND_OP_SUBTRACT:
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			return blendState[index].destBlendFactorAlpha;
		case VK_BLEND_OP_MIN:
			return VK_BLEND_FACTOR_ONE;
		case VK_BLEND_OP_MAX:
			return VK_BLEND_FACTOR_ONE;
		default:
			ASSERT(false);
	}

	return blendState[index].destBlendFactorAlpha;
}

VkBlendOp Context::blendOperationAlpha(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	switch(blendState[index].blendOperationAlpha)
	{
		case VK_BLEND_OP_ADD:
			if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
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
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_ADD;
				}
			}
		case VK_BLEND_OP_SUBTRACT:
			if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
			{
				return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
			}
			else if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
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
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_SUBTRACT;
				}
			}
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if(sourceBlendFactorAlpha(index) == VK_BLEND_FACTOR_ONE)
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
				{
					return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
				}
				else
				{
					return VK_BLEND_OP_REVERSE_SUBTRACT;
				}
			}
			else
			{
				if(destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
				{
					return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
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

	return blendState[index].blendOperationAlpha;
}

VkFormat Context::renderTargetInternalFormat(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(renderTarget[index])
	{
		return renderTarget[index]->getFormat();
	}
	else
	{
		return VK_FORMAT_UNDEFINED;
	}
}

bool Context::colorWriteActive() const
{
	for(int i = 0; i < RENDERTARGETS; i++)
	{
		if(colorWriteActive(i))
		{
			return true;
		}
	}

	return false;
}

int Context::colorWriteActive(int index) const
{
	ASSERT((index >= 0) && (index < RENDERTARGETS));

	if(!renderTarget[index] || renderTarget[index]->getFormat() == VK_FORMAT_UNDEFINED)
	{
		return 0;
	}

	if(blendOperation(index) == VK_BLEND_OP_DST_EXT && destBlendFactor(index) == VK_BLEND_FACTOR_ONE &&
	   (blendOperationAlpha(index) == VK_BLEND_OP_DST_EXT && destBlendFactorAlpha(index) == VK_BLEND_FACTOR_ONE))
	{
		return 0;
	}

	return colorWriteMask[index];
}

bool Context::colorUsed() const
{
	return colorWriteActive() || (pixelShader && pixelShader->getModes().ContainsKill);
}

}  // namespace sw
