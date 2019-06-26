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
#include "VkPipelineCache.hpp"
#include "VkPipelineLayout.hpp"
#include "VkShaderModule.hpp"
#include "VkRenderPass.hpp"
#include "Pipeline/ComputeProgram.hpp"
#include "Pipeline/SpirvShader.hpp"

#include "spirv-tools/optimizer.hpp"

#include <iostream>

namespace
{

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

unsigned char getNumberOfChannels(VkFormat format)
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

// preprocessSpirv applies and freezes specializations into constants, and inlines all functions.
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
		vk::trace("%s: %d:%d %s", category, int(p.line), int(p.column), m);
	});


	opt.RegisterPass(spvtools::CreateDeadBranchElimPass()); // Required for MergeReturnPass
	opt.RegisterPass(spvtools::CreateMergeReturnPass());
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

	// Basic optimization passes to primarily address glslang's love of loads &
	// stores. Significantly reduces time spent in LLVM passes and codegen.
	opt.RegisterPass(spvtools::CreateLocalAccessChainConvertPass());
	opt.RegisterPass(spvtools::CreateLocalSingleBlockLoadStoreElimPass());
	opt.RegisterPass(spvtools::CreateLocalSingleStoreElimPass());
	opt.RegisterPass(spvtools::CreateBlockMergePass());
	opt.RegisterPass(spvtools::CreateLocalMultiStoreElimPass());
	opt.RegisterPass(spvtools::CreateSSARewritePass());

	std::vector<uint32_t> optimized;
	opt.Run(code.data(), code.size(), &optimized);

	if (false) {
		spvtools::SpirvTools core(SPV_ENV_VULKAN_1_1);
		std::string preOpt;
		core.Disassemble(code, &preOpt, SPV_BINARY_TO_TEXT_OPTION_NONE);
		std::string postOpt;
		core.Disassemble(optimized, &postOpt, SPV_BINARY_TO_TEXT_OPTION_NONE);
		std::cout << "PRE-OPT: " << preOpt << std::endl
		 		<< "POST-OPT: " << postOpt << std::endl;
	}

	return optimized;
}

std::shared_ptr<sw::SpirvShader> createShader(const vk::PipelineCache::SpirvShaderKey& key, const vk::ShaderModule *module)
{
	auto code = preprocessSpirv(key.getInsns(), key.getSpecializationInfo());
	ASSERT(code.size() > 0);

	// If the pipeline has specialization constants, assume they're unique and
	// use a new serial ID so the shader gets recompiled.
	uint32_t codeSerialID = (key.getSpecializationInfo() ? vk::ShaderModule::nextSerialID() : module->getSerialID());

	// TODO(b/119409619): use allocator.
	return std::make_shared<sw::SpirvShader>(codeSerialID, key.getPipelineStage(), key.getEntryPointName().c_str(),
		code, key.getRenderPass(), key.getSubpassIndex());
}

std::shared_ptr<sw::ComputeProgram> createProgram(const vk::PipelineCache::ComputeProgramKey& key)
{
	vk::DescriptorSet::Bindings descriptorSets;  // FIXME(b/129523279): Delay code generation until invoke time.
	// TODO(b/119409619): use allocator.
	auto program = std::make_shared<sw::ComputeProgram>(key.getShader(), key.getLayout(), descriptorSets);
	program->generate();
	program->finalize();
	return program;
}

} // anonymous namespace

namespace vk
{

Pipeline::Pipeline(PipelineLayout const *layout) : layout(layout) {}

GraphicsPipeline::GraphicsPipeline(const VkGraphicsPipelineCreateInfo* pCreateInfo, void* mem)
	: Pipeline(vk::Cast(pCreateInfo->layout))
{
	if(((pCreateInfo->flags &
		~(VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT |
	      VK_PIPELINE_CREATE_DERIVATIVE_BIT |
	      VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)) != 0) ||
	   (pCreateInfo->pTessellationState != nullptr))
	{
		UNIMPLEMENTED("pCreateInfo settings");
	}

	if(pCreateInfo->pDynamicState)
	{
		for(uint32_t i = 0; i < pCreateInfo->pDynamicState->dynamicStateCount; i++)
		{
			VkDynamicState dynamicState = pCreateInfo->pDynamicState->pDynamicStates[i];
			switch(dynamicState)
			{
			case VK_DYNAMIC_STATE_VIEWPORT:
			case VK_DYNAMIC_STATE_SCISSOR:
			case VK_DYNAMIC_STATE_LINE_WIDTH:
			case VK_DYNAMIC_STATE_DEPTH_BIAS:
			case VK_DYNAMIC_STATE_BLEND_CONSTANTS:
			case VK_DYNAMIC_STATE_DEPTH_BOUNDS:
			case VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK:
			case VK_DYNAMIC_STATE_STENCIL_WRITE_MASK:
			case VK_DYNAMIC_STATE_STENCIL_REFERENCE:
				ASSERT(dynamicState < (sizeof(dynamicStateFlags) * 8));
				dynamicStateFlags |= (1 << dynamicState);
				break;
			default:
				UNIMPLEMENTED("dynamic state");
			}
		}
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
	if(assemblyState->flags != 0)
	{
		UNIMPLEMENTED("pCreateInfo->pInputAssemblyState settings");
	}

	primitiveRestartEnable = (assemblyState->primitiveRestartEnable != VK_FALSE);
	context.topology = assemblyState->topology;

	const VkPipelineViewportStateCreateInfo* viewportState = pCreateInfo->pViewportState;
	if(viewportState)
	{
		if((viewportState->flags != 0) ||
			(viewportState->viewportCount != 1) ||
			(viewportState->scissorCount != 1))
		{
			UNIMPLEMENTED("pCreateInfo->pViewportState settings");
		}

		if(!hasDynamicState(VK_DYNAMIC_STATE_SCISSOR))
		{
			scissor = viewportState->pScissors[0];
		}

		if(!hasDynamicState(VK_DYNAMIC_STATE_VIEWPORT))
		{
			viewport = viewportState->pViewports[0];
		}
	}

	const VkPipelineRasterizationStateCreateInfo* rasterizationState = pCreateInfo->pRasterizationState;
	if((rasterizationState->flags != 0) ||
	   (rasterizationState->depthClampEnable != VK_FALSE) ||
	   (rasterizationState->polygonMode != VK_POLYGON_MODE_FILL))
	{
		UNIMPLEMENTED("pCreateInfo->pRasterizationState settings");
	}

	context.rasterizerDiscard = (rasterizationState->rasterizerDiscardEnable == VK_TRUE);
	context.cullMode = rasterizationState->cullMode;
	context.frontFacingCCW = rasterizationState->frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE;
	context.depthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasConstantFactor : 0.0f;
	context.slopeDepthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasSlopeFactor : 0.0f;

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

		context.alphaToCoverage = (multisampleState->alphaToCoverageEnable == VK_TRUE);

		if((multisampleState->flags != 0) ||
			(multisampleState->sampleShadingEnable != VK_FALSE) ||
			(multisampleState->alphaToOneEnable != VK_FALSE))
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
		   (depthStencilState->depthBoundsTestEnable != VK_FALSE))
		{
			UNIMPLEMENTED("depthStencilState");
		}

		context.depthBoundsTestEnable = (depthStencilState->depthBoundsTestEnable == VK_TRUE);
		context.depthBufferEnable = (depthStencilState->depthTestEnable == VK_TRUE);
		context.depthWriteEnable = (depthStencilState->depthWriteEnable == VK_TRUE);
		context.depthCompareMode = depthStencilState->depthCompareOp;

		context.stencilEnable = context.twoSidedStencil = (depthStencilState->stencilTestEnable == VK_TRUE);
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
		   ((colorBlendState->logicOpEnable != VK_FALSE)))
		{
			UNIMPLEMENTED("colorBlendState");
		}

		if(!hasDynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS))
		{
			blendConstants.r = colorBlendState->blendConstants[0];
			blendConstants.g = colorBlendState->blendConstants[1];
			blendConstants.b = colorBlendState->blendConstants[2];
			blendConstants.a = colorBlendState->blendConstants[3];
		}

		for (auto i = 0u; i < colorBlendState->attachmentCount; i++)
		{
			const VkPipelineColorBlendAttachmentState& attachment = colorBlendState->pAttachments[i];
			context.setColorWriteMask(i, attachment.colorWriteMask);
		}

		if(colorBlendState->attachmentCount > 0)
		{
			const VkPipelineColorBlendAttachmentState& attachment = colorBlendState->pAttachments[0];
			context.alphaBlendEnable = (attachment.blendEnable == VK_TRUE);
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
	vertexShader.reset();
	fragmentShader.reset();
}

size_t GraphicsPipeline::ComputeRequiredAllocationSize(const VkGraphicsPipelineCreateInfo* pCreateInfo)
{
	return 0;
}

void GraphicsPipeline::setShader(const VkShaderStageFlagBits& stage, const std::shared_ptr<sw::SpirvShader> spirvShader)
{
	switch(stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		ASSERT(vertexShader.get() == nullptr);
		vertexShader = spirvShader;
		context.vertexShader = vertexShader.get();
		break;

	case VK_SHADER_STAGE_FRAGMENT_BIT:
		ASSERT(fragmentShader.get() == nullptr);
		fragmentShader = spirvShader;
		context.pixelShader = fragmentShader.get();
		break;

	default:
		UNSUPPORTED("Unsupported stage");
		break;
	}
}

const std::shared_ptr<sw::SpirvShader> GraphicsPipeline::getShader(const VkShaderStageFlagBits& stage) const
{
	switch(stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return vertexShader;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return fragmentShader;
	default:
		UNSUPPORTED("Unsupported stage");
		return fragmentShader;
	}
}

void GraphicsPipeline::compileShaders(const VkAllocationCallbacks* pAllocator, const VkGraphicsPipelineCreateInfo* pCreateInfo, PipelineCache* pPipelineCache)
{
	for (auto pStage = pCreateInfo->pStages; pStage != pCreateInfo->pStages + pCreateInfo->stageCount; pStage++)
	{
		if (pStage->flags != 0)
		{
			UNIMPLEMENTED("pStage->flags");
		}

		const ShaderModule *module = vk::Cast(pStage->module);
		const PipelineCache::SpirvShaderKey key(pStage->stage, pStage->pName, module->getCode(),
		                                        vk::Cast(pCreateInfo->renderPass), pCreateInfo->subpass,
		                                        pStage->pSpecializationInfo);
		if(pPipelineCache)
		{
			PipelineCache& pipelineCache = *pPipelineCache;
			{
				std::unique_lock<std::mutex> lock(pipelineCache.getShaderMutex());
				const std::shared_ptr<sw::SpirvShader>* spirvShader = pipelineCache[key];
				if(!spirvShader)
				{
					setShader(key.getPipelineStage(), createShader(key, module));
					pipelineCache.insert(key, getShader(key.getPipelineStage()));
				}
				else
				{
					setShader(key.getPipelineStage(), *spirvShader);
				}
			}
		}
		else
		{
			setShader(key.getPipelineStage(), createShader(key, module));
		}
	}
}

uint32_t GraphicsPipeline::computePrimitiveCount(uint32_t vertexCount) const
{
	switch(context.topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return vertexCount;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		return vertexCount / 2;
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return std::max<uint32_t>(vertexCount, 1) - 1;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		return vertexCount / 3;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		return std::max<uint32_t>(vertexCount, 2) - 2;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return std::max<uint32_t>(vertexCount, 2) - 2;
	default:
		UNIMPLEMENTED("context.topology %d", int(context.topology));
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

bool GraphicsPipeline::hasDynamicState(VkDynamicState dynamicState) const
{
	return (dynamicStateFlags & (1 << dynamicState)) != 0;
}

ComputePipeline::ComputePipeline(const VkComputePipelineCreateInfo* pCreateInfo, void* mem)
	: Pipeline(vk::Cast(pCreateInfo->layout))
{
}

void ComputePipeline::destroyPipeline(const VkAllocationCallbacks* pAllocator)
{
	shader.reset();
	program.reset();
}

size_t ComputePipeline::ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo* pCreateInfo)
{
	return 0;
}

void ComputePipeline::compileShaders(const VkAllocationCallbacks* pAllocator, const VkComputePipelineCreateInfo* pCreateInfo, PipelineCache* pPipelineCache)
{
	auto &stage = pCreateInfo->stage;
	const ShaderModule *module = vk::Cast(stage.module);

	ASSERT(shader.get() == nullptr);
	ASSERT(program.get() == nullptr);

	const PipelineCache::SpirvShaderKey shaderKey(
		stage.stage, stage.pName, module->getCode(), nullptr, 0, stage.pSpecializationInfo);
	if(pPipelineCache)
	{
		PipelineCache& pipelineCache = *pPipelineCache;
		{
			std::unique_lock<std::mutex> lock(pipelineCache.getShaderMutex());
			const std::shared_ptr<sw::SpirvShader>* spirvShader = pipelineCache[shaderKey];
			if(!spirvShader)
			{
				shader = createShader(shaderKey, module);
				pipelineCache.insert(shaderKey, shader);
			}
			else
			{
				shader = *spirvShader;
			}
		}

		{
			const PipelineCache::ComputeProgramKey programKey(shader.get(), layout);
			std::unique_lock<std::mutex> lock(pipelineCache.getProgramMutex());
			const std::shared_ptr<sw::ComputeProgram>* computeProgram = pipelineCache[programKey];
			if(!computeProgram)
			{
				program = createProgram(programKey);
				pipelineCache.insert(programKey, program);
			}
			else
			{
				program = *computeProgram;
			}
		}
	}
	else
	{
		shader = createShader(shaderKey, module);
		const PipelineCache::ComputeProgramKey programKey(shader.get(), layout);
		program = createProgram(programKey);
	}
}

void ComputePipeline::run(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
	uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ,
	vk::DescriptorSet::Bindings const &descriptorSets,
	vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
	sw::PushConstantStorage const &pushConstants)
{
	ASSERT_OR_RETURN(program != nullptr);
	program->run(
		descriptorSets, descriptorDynamicOffsets, pushConstants,
		baseGroupX, baseGroupY, baseGroupZ,
		groupCountX, groupCountY, groupCountZ);
}

} // namespace vk
