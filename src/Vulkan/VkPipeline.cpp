// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#include <Pipeline/SpirvShader.hpp>
#include "VkPipeline.hpp"
#include "VkShaderModule.hpp"

namespace
{

sw::DrawType Convert(VkPrimitiveTopology topology)
{
	switch(topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return sw::DRAW_POINTLIST;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		return sw::DRAW_LINELIST;
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return sw::DRAW_LINESTRIP;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		return sw::DRAW_TRIANGLELIST;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		return sw::DRAW_TRIANGLESTRIP;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return sw::DRAW_TRIANGLEFAN;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
		// geometry shader specific
		ASSERT(false);
		break;
	case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST:
		// tesselation shader specific
		ASSERT(false);
		break;
	default:
		UNIMPLEMENTED();
	}

	return sw::DRAW_TRIANGLELIST;
}

sw::Rect Convert(const VkRect2D& rect)
{
	return sw::Rect(rect.offset.x, rect.offset.y, rect.offset.x + rect.extent.width, rect.offset.y + rect.extent.height);
}

sw::StreamType getStreamType(VkFormat format)
{
	switch(format)
	{
	case VK_FORMAT_R8_UNORM:
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8_UINT:
	case VK_FORMAT_R8G8_UINT:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		return sw::STREAMTYPE_BYTE;
	case VK_FORMAT_R8_SNORM:
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8G8_SNORM:
	case VK_FORMAT_R8G8_SINT:
	case VK_FORMAT_R8G8B8A8_SNORM:
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		return sw::STREAMTYPE_SBYTE;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		return sw::STREAMTYPE_2_10_10_10_UINT;
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_UINT:
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_UINT:
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_UINT:
		return sw::STREAMTYPE_USHORT;
	case VK_FORMAT_R16_SNORM:
	case VK_FORMAT_R16_SINT:
	case VK_FORMAT_R16G16_SNORM:
	case VK_FORMAT_R16G16_SINT:
	case VK_FORMAT_R16G16B16A16_SNORM:
	case VK_FORMAT_R16G16B16A16_SINT:
		return sw::STREAMTYPE_SHORT;
	case VK_FORMAT_R16_SFLOAT:
	case VK_FORMAT_R16G16_SFLOAT:
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return sw::STREAMTYPE_HALF;
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R32G32_UINT:
	case VK_FORMAT_R32G32B32_UINT:
	case VK_FORMAT_R32G32B32A32_UINT:
		return sw::STREAMTYPE_UINT;
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32B32_SINT:
	case VK_FORMAT_R32G32B32A32_SINT:
		return sw::STREAMTYPE_INT;
	case VK_FORMAT_R32_SFLOAT:
	case VK_FORMAT_R32G32_SFLOAT:
	case VK_FORMAT_R32G32B32_SFLOAT:
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return sw::STREAMTYPE_FLOAT;
	default:
		UNIMPLEMENTED();
	}

	return sw::STREAMTYPE_BYTE;
}

uint32_t getNumberOfChannels(VkFormat format)
{
	switch(format)
	{
	case VK_FORMAT_R8_UNORM:
	case VK_FORMAT_R8_SNORM:
	case VK_FORMAT_R8_UINT:
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_SNORM:
	case VK_FORMAT_R16_UINT:
	case VK_FORMAT_R16_SINT:
	case VK_FORMAT_R16_SFLOAT:
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_SFLOAT:
		return 1;
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8G8_SNORM:
	case VK_FORMAT_R8G8_UINT:
	case VK_FORMAT_R8G8_SINT:
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_SNORM:
	case VK_FORMAT_R16G16_UINT:
	case VK_FORMAT_R16G16_SINT:
	case VK_FORMAT_R16G16_SFLOAT:
	case VK_FORMAT_R32G32_UINT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_SFLOAT:
		return 2;
	case VK_FORMAT_R32G32B32_UINT:
	case VK_FORMAT_R32G32B32_SINT:
	case VK_FORMAT_R32G32B32_SFLOAT:
		return 3;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SNORM:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_SNORM:
	case VK_FORMAT_R16G16B16A16_UINT:
	case VK_FORMAT_R16G16B16A16_SINT:
	case VK_FORMAT_R16G16B16A16_SFLOAT:
	case VK_FORMAT_R32G32B32A32_UINT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4;
	default:
		UNIMPLEMENTED();
	}

	return 0;
}

}

namespace vk
{

GraphicsPipeline::GraphicsPipeline(const VkGraphicsPipelineCreateInfo* pCreateInfo, void* mem)
{
	if((pCreateInfo->flags != 0) ||
	   (pCreateInfo->stageCount != 2) ||
	   (pCreateInfo->pTessellationState != nullptr) ||
	   (pCreateInfo->pDynamicState != nullptr) ||
	   (pCreateInfo->subpass != 0) ||
	   (pCreateInfo->basePipelineHandle != VK_NULL_HANDLE) ||
	   (pCreateInfo->basePipelineIndex != 0))
	{
		UNIMPLEMENTED();
	}

	const VkPipelineShaderStageCreateInfo& vertexStage = pCreateInfo->pStages[0];
	if((vertexStage.stage != VK_SHADER_STAGE_VERTEX_BIT) ||
	   (vertexStage.flags != 0) ||
	   !((vertexStage.pSpecializationInfo == nullptr) ||
	     ((vertexStage.pSpecializationInfo->mapEntryCount == 0) &&
	      (vertexStage.pSpecializationInfo->dataSize == 0))))
	{
		UNIMPLEMENTED();
	}

	const VkPipelineShaderStageCreateInfo& fragmentStage = pCreateInfo->pStages[1];
	if((fragmentStage.stage != VK_SHADER_STAGE_FRAGMENT_BIT) ||
	   (fragmentStage.flags != 0) ||
	   !((fragmentStage.pSpecializationInfo == nullptr) ||
	     ((fragmentStage.pSpecializationInfo->mapEntryCount == 0) &&
	      (fragmentStage.pSpecializationInfo->dataSize == 0))))
	{
		UNIMPLEMENTED();
	}

	const VkPipelineVertexInputStateCreateInfo* vertexInputState = pCreateInfo->pVertexInputState;
	if(vertexInputState->flags != 0)
	{
		UNIMPLEMENTED();
	}

	for(uint32_t i = 0; i < vertexInputState->vertexBindingDescriptionCount; i++)
	{
		const VkVertexInputBindingDescription* vertexBindingDescription = vertexInputState->pVertexBindingDescriptions;
		context.input[vertexBindingDescription->binding].stride = vertexBindingDescription->stride;
		if(vertexBindingDescription->inputRate != VK_VERTEX_INPUT_RATE_VERTEX)
		{
			UNIMPLEMENTED();
		}
	}

	for(uint32_t i = 0; i < vertexInputState->vertexAttributeDescriptionCount; i++)
	{
		const VkVertexInputAttributeDescription* vertexAttributeDescriptions = vertexInputState->pVertexAttributeDescriptions;
		sw::Stream& input = context.input[vertexAttributeDescriptions->binding];
		input.count = getNumberOfChannels(vertexAttributeDescriptions->format);
		input.type = getStreamType(vertexAttributeDescriptions->format);
		input.normalized = !sw::Surface::isNonNormalizedInteger(vertexAttributeDescriptions->format);

		if(vertexAttributeDescriptions->location != vertexAttributeDescriptions->binding)
		{
			UNIMPLEMENTED();
		}
		if(vertexAttributeDescriptions->offset != 0)
		{
			UNIMPLEMENTED();
		}
	}

	const VkPipelineInputAssemblyStateCreateInfo* assemblyState = pCreateInfo->pInputAssemblyState;
	if((assemblyState->flags != 0) ||
	   (assemblyState->primitiveRestartEnable != 0))
	{
		UNIMPLEMENTED();
	}

	context.drawType = Convert(assemblyState->topology);

	const VkPipelineViewportStateCreateInfo* viewportState = pCreateInfo->pViewportState;
	if((viewportState->flags != 0) ||
	   (viewportState->viewportCount != 1) ||
	   (viewportState->scissorCount	!= 1))
	{
		UNIMPLEMENTED();
	}

	scissor = Convert(viewportState->pScissors[0]);
	viewport = viewportState->pViewports[0];

	const VkPipelineRasterizationStateCreateInfo* rasterizationState = pCreateInfo->pRasterizationState;
	if((rasterizationState->flags != 0) ||
	   (rasterizationState->depthClampEnable != 0) ||
	   (rasterizationState->polygonMode != VK_POLYGON_MODE_FILL))
	{
		UNIMPLEMENTED();
	}

	context.rasterizerDiscard = rasterizationState->rasterizerDiscardEnable;
	context.frontFacingCCW = rasterizationState->frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE;
	context.depthBias = (rasterizationState->depthBiasEnable ? rasterizationState->depthBiasConstantFactor : 0.0f);
	context.slopeDepthBias = (rasterizationState->depthBiasEnable ? rasterizationState->depthBiasSlopeFactor : 0.0f);

	const VkPipelineMultisampleStateCreateInfo* multisampleState = pCreateInfo->pMultisampleState;
	if((multisampleState->flags != 0) ||
	   (multisampleState->rasterizationSamples != VK_SAMPLE_COUNT_1_BIT) ||
	   (multisampleState->sampleShadingEnable != 0) ||
	   !((multisampleState->pSampleMask == nullptr) ||
	     (*(multisampleState->pSampleMask) == 0xFFFFFFFFu)) ||
	   (multisampleState->alphaToCoverageEnable != 0) ||
	   (multisampleState->alphaToOneEnable != 0))
	{
		UNIMPLEMENTED();
	}

	const VkPipelineDepthStencilStateCreateInfo* depthStencilState = pCreateInfo->pDepthStencilState;
	if((depthStencilState->flags != 0) ||
	   (depthStencilState->depthBoundsTestEnable != 0) ||
	   (depthStencilState->minDepthBounds != 0.0f) ||
	   (depthStencilState->maxDepthBounds != 1.0f))
	{
		UNIMPLEMENTED();
	}

	context.depthBufferEnable = depthStencilState->depthTestEnable;
	context.depthWriteEnable = depthStencilState->depthWriteEnable;
	context.depthCompareMode = depthStencilState->depthCompareOp;

	context.stencilEnable = context.twoSidedStencil = depthStencilState->stencilTestEnable;
	if(context.stencilEnable)
	{
		context.stencilMask = depthStencilState->front.compareMask;
		context.stencilCompareMode = depthStencilState->front.compareOp;
		context.stencilZFailOperation = depthStencilState->front.depthFailOp;
		context.stencilFailOperation = depthStencilState->front.failOp;
		context.stencilPassOperation = depthStencilState->front.passOp;
		context.stencilReference = depthStencilState->front.reference;
		context.stencilWriteMask = depthStencilState->front.writeMask;

		context.stencilMaskCCW = depthStencilState->back.compareMask;
		context.stencilCompareModeCCW = depthStencilState->back.compareOp;
		context.stencilZFailOperationCCW = depthStencilState->back.depthFailOp;
		context.stencilFailOperationCCW = depthStencilState->back.failOp;
		context.stencilPassOperationCCW = depthStencilState->back.passOp;
		context.stencilReferenceCCW = depthStencilState->back.reference;
		context.stencilWriteMaskCCW = depthStencilState->back.writeMask;
	}

	const VkPipelineColorBlendStateCreateInfo* colorBlendState = pCreateInfo->pColorBlendState;
	if((colorBlendState->flags != 0) ||
	   ((colorBlendState->logicOpEnable != 0) &&
	    (colorBlendState->attachmentCount > 1)))
	{
		UNIMPLEMENTED();
	}

	context.colorLogicOpEnabled = colorBlendState->logicOpEnable;
	context.logicalOperation = colorBlendState->logicOp;
	blendConstants.r = colorBlendState->blendConstants[0];
	blendConstants.g = colorBlendState->blendConstants[1];
	blendConstants.b = colorBlendState->blendConstants[2];
	blendConstants.a = colorBlendState->blendConstants[3];

	if(colorBlendState->attachmentCount == 1)
	{
		const VkPipelineColorBlendAttachmentState& attachment = colorBlendState->pAttachments[0];
		if(attachment.colorWriteMask != (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT))
		{
			UNIMPLEMENTED();
		}

		context.alphaBlendEnable = attachment.blendEnable;
		context.separateAlphaBlendEnable = (attachment.alphaBlendOp != attachment.colorBlendOp) ||
		                                   (attachment.dstAlphaBlendFactor != attachment.dstColorBlendFactor) ||
		                                   (attachment.srcAlphaBlendFactor != attachment.srcColorBlendFactor);
		context.blendOperationStateAlpha = attachment.alphaBlendOp;
		context.blendOperationState = attachment.colorBlendOp;
		context.destBlendFactorStateAlpha = attachment.dstAlphaBlendFactor;
		context.destBlendFactorState = attachment.dstColorBlendFactor;
		context.sourceBlendFactorStateAlpha = attachment.srcAlphaBlendFactor;
		context.sourceBlendFactorState = attachment.srcColorBlendFactor;
	}
}

void GraphicsPipeline::destroyPipeline(const VkAllocationCallbacks* pAllocator)
{
	delete vertexShader;
	delete fragmentShader;
}

size_t GraphicsPipeline::ComputeRequiredAllocationSize(const VkGraphicsPipelineCreateInfo* pCreateInfo)
{
	return 0;
}

void GraphicsPipeline::compileShaders(const VkAllocationCallbacks* pAllocator, const VkGraphicsPipelineCreateInfo* pCreateInfo)
{
	for (auto pStage = pCreateInfo->pStages; pStage != pCreateInfo->pStages + pCreateInfo->stageCount; pStage++) {
		auto module = Cast(pStage->module);

		// TODO: apply prep passes using SPIRV-Opt here.
		// - Apply and freeze specializations, etc.
		auto code = module->getCode();

		// TODO: pass in additional information here:
		// - any NOS from pCreateInfo which we'll actually need
		auto spirvShader = new sw::SpirvShader{code};

		switch (pStage->stage) {
			case VK_SHADER_STAGE_VERTEX_BIT:
				vertexShader = spirvShader;
				break;

			case VK_SHADER_STAGE_FRAGMENT_BIT:
				fragmentShader = spirvShader;
				break;

			default:
				UNIMPLEMENTED("Unsupported stage");
		}
	}
}

uint32_t GraphicsPipeline::computePrimitiveCount(uint32_t vertexCount) const
{
	switch(context.drawType)
	{
	case sw::DRAW_POINTLIST:
		return vertexCount;
	case sw::DRAW_LINELIST:
		return vertexCount / 2;
	case sw::DRAW_LINESTRIP:
		return vertexCount - 1;
	case sw::DRAW_TRIANGLELIST:
		return vertexCount / 3;
	case sw::DRAW_TRIANGLESTRIP:
		return vertexCount - 2;
	case sw::DRAW_TRIANGLEFAN:
		return vertexCount - 2;
	default:
		UNIMPLEMENTED();
	}

	return 0;
}

const sw::Context& GraphicsPipeline::getContext() const
{
	return context;
}

const sw::Rect& GraphicsPipeline::getScissor() const
{
	return scissor;
}

const VkViewport& GraphicsPipeline::getViewport() const
{
	return viewport;
}

const sw::Color<float>& GraphicsPipeline::getBlendConstants() const
{
	return blendConstants;
}

ComputePipeline::ComputePipeline(const VkComputePipelineCreateInfo* pCreateInfo, void* mem)
{
}

void ComputePipeline::destroyPipeline(const VkAllocationCallbacks* pAllocator)
{
}

size_t ComputePipeline::ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo* pCreateInfo)
{
	return 0;
}

} // namespace vk
