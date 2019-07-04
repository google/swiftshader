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
	Context::Context()
	{
		init();
	}

	bool Context::isDrawPoint() const
	{
		switch(topology)
		{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
			return true;
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			break;
		default:
			UNIMPLEMENTED("topology %d", int(topology));
		}
		return false;
	}

	bool Context::isDrawLine() const
	{
		switch(topology)
		{
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			return true;
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			break;
		default:
			UNIMPLEMENTED("topology %d", int(topology));
		}
		return false;
	}

	bool Context::isDrawTriangle() const
	{
		switch(topology)
		{
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			return true;
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			break;
		default:
			UNIMPLEMENTED("topology %d", int(topology));
		}
		return false;
	}

	void Context::init()
	{
		// Set vertex streams to null stream
		for(int i = 0; i < MAX_INTERFACE_COMPONENTS/4; i++)
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
		frontStencil = {};
		backStencil = {};

		rasterizerDiscard = false;

		depthCompareMode = VK_COMPARE_OP_LESS;
		depthBoundsTestEnable = false;
		depthBufferEnable = false;
		depthWriteEnable = false;

		alphaBlendEnable = false;
		sourceBlendFactorState = VK_BLEND_FACTOR_ONE;
		destBlendFactorState = VK_BLEND_FACTOR_ZERO;
		blendOperationState = VK_BLEND_OP_ADD;

		sourceBlendFactorStateAlpha = VK_BLEND_FACTOR_ONE;
		destBlendFactorStateAlpha = VK_BLEND_FACTOR_ZERO;
		blendOperationStateAlpha = VK_BLEND_OP_ADD;

		cullMode = VK_CULL_MODE_FRONT_BIT;
		frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

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

	bool Context::alphaBlendActive() const
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
		bool alphaBlend = !(blendOperationAlpha() == VK_BLEND_OP_SRC_EXT && sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE);

		return colorBlend || alphaBlend;
	}

	VkBlendFactor Context::sourceBlendFactor() const
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

	VkBlendFactor Context::destBlendFactor() const
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

	bool Context::allTargetsColorClamp() const
	{
		// TODO: remove all of this and support VkPhysicalDeviceFeatures::independentBlend instead
		for (int i = 0; i < RENDERTARGETS; i++)
		{
			if (renderTarget[i] && renderTarget[i]->getFormat().isFloatFormat())
			{
				return false;
			}
		}

		return true;
	}

	VkBlendOp Context::blendOperation() const
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
			if(sourceBlendFactor() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
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
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
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
				if(destBlendFactor() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
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

	VkBlendFactor Context::sourceBlendFactorAlpha() const
	{
		switch (blendOperationStateAlpha)
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

	VkBlendFactor Context::destBlendFactorAlpha() const
	{
		switch (blendOperationStateAlpha)
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

	VkBlendOp Context::blendOperationAlpha() const
	{
		switch (blendOperationStateAlpha)
		{
		case VK_BLEND_OP_ADD:
			if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
			{
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
			{
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
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
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_ADD;
				}
			}
		case VK_BLEND_OP_SUBTRACT:
			if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
			{
				return VK_BLEND_OP_ZERO_EXT;   // Negative, clamped to zero
			}
			else if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
			{
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
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
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_SRC_EXT;
				}
				else
				{
					return VK_BLEND_OP_SUBTRACT;
				}
			}
		case VK_BLEND_OP_REVERSE_SUBTRACT:
			if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
			{
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO)
				{
					return VK_BLEND_OP_ZERO_EXT;
				}
				else
				{
					return VK_BLEND_OP_DST_EXT;
				}
			}
			else if (sourceBlendFactorAlpha() == VK_BLEND_FACTOR_ONE)
			{
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
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
				if (destBlendFactorAlpha() == VK_BLEND_FACTOR_ZERO && allTargetsColorClamp())
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

		return blendOperationStateAlpha;
	}

	VkFormat Context::renderTargetInternalFormat(int index) const
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

	bool Context::colorWriteActive() const
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

	int Context::colorWriteActive(int index) const
	{
		if(!renderTarget[index] || renderTarget[index]->getFormat() == VK_FORMAT_UNDEFINED)
		{
			return 0;
		}

		if(blendOperation() == VK_BLEND_OP_DST_EXT && destBlendFactor() == VK_BLEND_FACTOR_ONE &&
		   (blendOperationAlpha() == VK_BLEND_OP_DST_EXT && destBlendFactorAlpha() == VK_BLEND_FACTOR_ONE))
		{
			return 0;
		}

		return colorWriteMask[index];
	}

	bool Context::colorUsed() const
	{
		return colorWriteActive() || (pixelShader && pixelShader->getModes().ContainsKill);
	}
}
