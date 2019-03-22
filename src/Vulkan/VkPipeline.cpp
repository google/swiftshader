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

#include "VkPipeline.hpp"
#include "VkPipelineLayout.hpp"
#include "VkShaderModule.hpp"
#include "Pipeline/ComputeProgram.hpp"
#include "Pipeline/SpirvShader.hpp"

#include "spirv-tools/optimizer.hpp"

#include <iostream>

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
		UNIMPLEMENTED("topology");
	}

	return sw::DRAW_TRIANGLELIST;
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
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		return sw::STREAMTYPE_BYTE;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return sw::STREAMTYPE_COLOR;
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
		UNIMPLEMENTED("format");
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
		UNIMPLEMENTED("format");
	}

	return 0;
}

// preprocessSpirv applies and freezes specializations into constants, inlines
// all functions and performs constant folding.
std::vector<uint32_t> preprocessSpirv(
		std::vector<uint32_t> const &code,
		VkSpecializationInfo const *specializationInfo)
{
	spvtools::Optimizer opt{SPV_ENV_VULKAN_1_1};

	opt.SetMessageConsumer([](spv_message_level_t level, const char*, const spv_position_t& p, const char* m) {
		const char* category = "";
		switch (level)
		{
		case SPV_MSG_FATAL:          category = "FATAL";          break;
		case SPV_MSG_INTERNAL_ERROR: category = "INTERNAL_ERROR"; break;
		case SPV_MSG_ERROR:          category = "ERROR";          break;
		case SPV_MSG_WARNING:        category = "WARNING";        break;
		case SPV_MSG_INFO:           category = "INFO";           break;
		case SPV_MSG_DEBUG:          category = "DEBUG";          break;
		}
		vk::trace("%s: %d:%d %s", category, p.line, p.column, m);
	});

	opt.RegisterPass(spvtools::CreateInlineExhaustivePass());
	opt.RegisterPass(spvtools::CreateEliminateDeadFunctionsPass());

	// If the pipeline uses specialization, apply the specializations before freezing
	if (specializationInfo)
	{
		std::unordered_map<uint32_t, std::vector<uint32_t>> specializations;
		for (auto i = 0u; i < specializationInfo->mapEntryCount; ++i)
		{
			auto const &e = specializationInfo->pMapEntries[i];
			auto value_ptr =
					static_cast<uint32_t const *>(specializationInfo->pData) + e.offset / sizeof(uint32_t);
			specializations.emplace(e.constantID,
									std::vector<uint32_t>{value_ptr, value_ptr + e.size / sizeof(uint32_t)});
		}
		opt.RegisterPass(spvtools::CreateSetSpecConstantDefaultValuePass(specializations));
	}
	// Freeze specialization constants into normal constants, and propagate through
	opt.RegisterPass(spvtools::CreateFreezeSpecConstantValuePass());
	opt.RegisterPass(spvtools::CreateFoldSpecConstantOpAndCompositePass());

	std::vector<uint32_t> optimized;
	opt.Run(code.data(), code.size(), &optimized);

	if (false) {
		spvtools::SpirvTools core(SPV_ENV_VULKAN_1_1);
		std::string preOpt;
		core.Disassemble(code, &preOpt);
		std::string postOpt;
		core.Disassemble(optimized, &postOpt);
		std::cout << "PRE-OPT: " << preOpt << std::endl
		 		<< "POST-OPT: " << postOpt << std::endl;
	}

	return optimized;
}

} // anonymous namespace

namespace vk
{

Pipeline::Pipeline(PipelineLayout const *layout) : layout(layout) {}

GraphicsPipeline::GraphicsPipeline(const VkGraphicsPipelineCreateInfo* pCreateInfo, void* mem)
	: Pipeline(Cast(pCreateInfo->layout))
{
	if(((pCreateInfo->flags & ~(VK_PIPELINE_CREATE_DERIVATIVE_BIT | VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)) != 0) ||
	   (pCreateInfo->stageCount != 2) ||
	   (pCreateInfo->pTessellationState != nullptr) ||
	   (pCreateInfo->pDynamicState != nullptr))
	{
		UNIMPLEMENTED("pCreateInfo settings");
	}

	const VkPipelineVertexInputStateCreateInfo* vertexInputState = pCreateInfo->pVertexInputState;
	if(vertexInputState->flags != 0)
	{
		UNIMPLEMENTED("vertexInputState->flags");
	}

	// Context must always have a PipelineLayout set.
	context.pipelineLayout = layout;

	// Temporary in-binding-order representation of buffer strides, to be consumed below
	// when considering attributes. TODO: unfuse buffers from attributes in backend, is old GL model.
	uint32_t vertexStrides[MAX_VERTEX_INPUT_BINDINGS];
	uint32_t instanceStrides[MAX_VERTEX_INPUT_BINDINGS];
	for(uint32_t i = 0; i < vertexInputState->vertexBindingDescriptionCount; i++)
	{
		auto const & desc = vertexInputState->pVertexBindingDescriptions[i];
		vertexStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_VERTEX ? desc.stride : 0;
		instanceStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_INSTANCE ? desc.stride : 0;
	}

	for(uint32_t i = 0; i < vertexInputState->vertexAttributeDescriptionCount; i++)
	{
		auto const & desc = vertexInputState->pVertexAttributeDescriptions[i];
		sw::Stream& input = context.input[desc.location];
		input.count = getNumberOfChannels(desc.format);
		input.type = getStreamType(desc.format);
		input.normalized = !vk::Format(desc.format).isNonNormalizedInteger();
		input.offset = desc.offset;
		input.binding = desc.binding;
		input.vertexStride = vertexStrides[desc.binding];
		input.instanceStride = instanceStrides[desc.binding];
	}

	const VkPipelineInputAssemblyStateCreateInfo* assemblyState = pCreateInfo->pInputAssemblyState;
	if((assemblyState->flags != 0) ||
	   (assemblyState->primitiveRestartEnable != 0))
	{
		UNIMPLEMENTED("pCreateInfo->pInputAssemblyState settings");
	}

	context.drawType = Convert(assemblyState->topology);

	const VkPipelineViewportStateCreateInfo* viewportState = pCreateInfo->pViewportState;
	if(viewportState)
	{
		if((viewportState->flags != 0) ||
			(viewportState->viewportCount != 1) ||
			(viewportState->scissorCount != 1))
		{
			UNIMPLEMENTED("pCreateInfo->pViewportState settings");
		}

		scissor = viewportState->pScissors[0];
		viewport = viewportState->pViewports[0];
	}

	const VkPipelineRasterizationStateCreateInfo* rasterizationState = pCreateInfo->pRasterizationState;
	if((rasterizationState->flags != 0) ||
	   (rasterizationState->depthClampEnable != 0) ||
	   (rasterizationState->polygonMode != VK_POLYGON_MODE_FILL))
	{
		UNIMPLEMENTED("pCreateInfo->pRasterizationState settings");
	}

	context.rasterizerDiscard = rasterizationState->rasterizerDiscardEnable;
	context.cullMode = rasterizationState->cullMode;
	context.frontFacingCCW = rasterizationState->frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE;
	context.depthBias = (rasterizationState->depthBiasEnable ? rasterizationState->depthBiasConstantFactor : 0.0f);
	context.slopeDepthBias = (rasterizationState->depthBiasEnable ? rasterizationState->depthBiasSlopeFactor : 0.0f);

	const VkPipelineMultisampleStateCreateInfo* multisampleState = pCreateInfo->pMultisampleState;
	if(multisampleState)
	{
		switch (multisampleState->rasterizationSamples) {
		case VK_SAMPLE_COUNT_1_BIT:
			context.sampleCount = 1;
			break;
		case VK_SAMPLE_COUNT_4_BIT:
			context.sampleCount = 4;
			break;
		default:
			UNIMPLEMENTED("Unsupported sample count");
		}

		if (multisampleState->pSampleMask)
			context.sampleMask = multisampleState->pSampleMask[0];

		if((multisampleState->flags != 0) ||
			(multisampleState->sampleShadingEnable != 0) ||
				(multisampleState->alphaToCoverageEnable != 0) ||
			(multisampleState->alphaToOneEnable != 0))
		{
			UNIMPLEMENTED("multisampleState");
		}
	}
	else
	{
		context.sampleCount = 1;
	}

	const VkPipelineDepthStencilStateCreateInfo* depthStencilState = pCreateInfo->pDepthStencilState;
	if(depthStencilState)
	{
		if((depthStencilState->flags != 0) ||
		   (depthStencilState->depthBoundsTestEnable != 0))
		{
			UNIMPLEMENTED("depthStencilState");
		}

		context.depthBufferEnable = depthStencilState->depthTestEnable;
		context.depthWriteEnable = depthStencilState->depthWriteEnable;
		context.depthCompareMode = depthStencilState->depthCompareOp;

		context.stencilEnable = context.twoSidedStencil = depthStencilState->stencilTestEnable;
		if(context.stencilEnable)
		{
			context.frontStencil = depthStencilState->front;
			context.backStencil = depthStencilState->back;
		}
	}

	const VkPipelineColorBlendStateCreateInfo* colorBlendState = pCreateInfo->pColorBlendState;
	if(colorBlendState)
	{
		if((colorBlendState->flags != 0) ||
		   ((colorBlendState->logicOpEnable != 0) &&
			(colorBlendState->attachmentCount > 1)))
		{
			UNIMPLEMENTED("colorBlendState");
		}

		blendConstants.r = colorBlendState->blendConstants[0];
		blendConstants.g = colorBlendState->blendConstants[1];
		blendConstants.b = colorBlendState->blendConstants[2];
		blendConstants.a = colorBlendState->blendConstants[3];

		if(colorBlendState->attachmentCount == 1)
		{
			const VkPipelineColorBlendAttachmentState& attachment = colorBlendState->pAttachments[0];
			if(attachment.colorWriteMask != (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT))
			{
				UNIMPLEMENTED("colorWriteMask");
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
	for (auto pStage = pCreateInfo->pStages; pStage != pCreateInfo->pStages + pCreateInfo->stageCount; pStage++)
	{
		if (pStage->flags != 0)
		{
			UNIMPLEMENTED("pStage->flags");
		}

		auto module = Cast(pStage->module);
		auto code = preprocessSpirv(module->getCode(), pStage->pSpecializationInfo);

		// TODO: also pass in any pipeline state which will affect shader compilation
		auto spirvShader = new sw::SpirvShader{code};

		switch (pStage->stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:
			context.vertexShader = vertexShader = spirvShader;
			break;

		case VK_SHADER_STAGE_FRAGMENT_BIT:
			context.pixelShader = fragmentShader = spirvShader;
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
		UNIMPLEMENTED("drawType");
	}

	return 0;
}

const sw::Context& GraphicsPipeline::getContext() const
{
	return context;
}

const VkRect2D& GraphicsPipeline::getScissor() const
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
	: Pipeline(Cast(pCreateInfo->layout))
{
}

void ComputePipeline::destroyPipeline(const VkAllocationCallbacks* pAllocator)
{
	delete shader;
}

size_t ComputePipeline::ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo* pCreateInfo)
{
	return 0;
}

void ComputePipeline::compileShaders(const VkAllocationCallbacks* pAllocator, const VkComputePipelineCreateInfo* pCreateInfo)
{
	auto module = Cast(pCreateInfo->stage.module);

	auto code = preprocessSpirv(module->getCode(), pCreateInfo->stage.pSpecializationInfo);

	ASSERT_OR_RETURN(code.size() > 0);

	ASSERT(shader == nullptr);

	// FIXME (b/119409619): use allocator.
	shader = new sw::SpirvShader(code);

	sw::ComputeProgram program(shader, layout);

	program.generate();

	// TODO(bclayton): Cache program
	routine = program("ComputeRoutine");
}

void ComputePipeline::run(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ,
	size_t numDescriptorSets, VkDescriptorSet *descriptorSets, sw::PushConstantStorage const &pushConstants)
{
	ASSERT_OR_RETURN(routine != nullptr);
	sw::ComputeProgram::run(
		routine, reinterpret_cast<void**>(descriptorSets), pushConstants,
		groupCountX, groupCountY, groupCountZ);
}

} // namespace vk
