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

#include "VkDevice.hpp"
#include "VkPipelineCache.hpp"
#include "VkPipelineLayout.hpp"
#include "VkRenderPass.hpp"
#include "VkShaderModule.hpp"
#include "VkStringify.hpp"
#include "Pipeline/ComputeProgram.hpp"
#include "Pipeline/SpirvShader.hpp"

#include "marl/trace.h"

#include "spirv-tools/optimizer.hpp"

#include <iostream>

namespace {

// preprocessSpirv applies and freezes specializations into constants, and inlines all functions.
std::vector<uint32_t> preprocessSpirv(
    std::vector<uint32_t> const &code,
    VkSpecializationInfo const *specializationInfo,
    bool optimize)
{
	spvtools::Optimizer opt{ SPV_ENV_VULKAN_1_1 };

	opt.SetMessageConsumer([](spv_message_level_t level, const char *, const spv_position_t &p, const char *m) {
		switch(level)
		{
			case SPV_MSG_FATAL: sw::warn("SPIR-V FATAL: %d:%d %s\n", int(p.line), int(p.column), m);
			case SPV_MSG_INTERNAL_ERROR: sw::warn("SPIR-V INTERNAL_ERROR: %d:%d %s\n", int(p.line), int(p.column), m);
			case SPV_MSG_ERROR: sw::warn("SPIR-V ERROR: %d:%d %s\n", int(p.line), int(p.column), m);
			case SPV_MSG_WARNING: sw::warn("SPIR-V WARNING: %d:%d %s\n", int(p.line), int(p.column), m);
			case SPV_MSG_INFO: sw::trace("SPIR-V INFO: %d:%d %s\n", int(p.line), int(p.column), m);
			case SPV_MSG_DEBUG: sw::trace("SPIR-V DEBUG: %d:%d %s\n", int(p.line), int(p.column), m);
			default: sw::trace("SPIR-V MESSAGE: %d:%d %s\n", int(p.line), int(p.column), m);
		}
	});

	// If the pipeline uses specialization, apply the specializations before freezing
	if(specializationInfo)
	{
		std::unordered_map<uint32_t, std::vector<uint32_t>> specializations;
		for(auto i = 0u; i < specializationInfo->mapEntryCount; ++i)
		{
			auto const &e = specializationInfo->pMapEntries[i];
			auto value_ptr =
			    static_cast<uint32_t const *>(specializationInfo->pData) + e.offset / sizeof(uint32_t);
			specializations.emplace(e.constantID,
			                        std::vector<uint32_t>{ value_ptr, value_ptr + e.size / sizeof(uint32_t) });
		}
		opt.RegisterPass(spvtools::CreateSetSpecConstantDefaultValuePass(specializations));
	}

	if(optimize)
	{
		// Full optimization list taken from spirv-opt.
		opt.RegisterPerformancePasses();
	}

	std::vector<uint32_t> optimized;
	opt.Run(code.data(), code.size(), &optimized);

	if(false)
	{
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

std::shared_ptr<sw::SpirvShader> createShader(
    const vk::PipelineCache::SpirvShaderKey &key,
    const vk::ShaderModule *module,
    bool robustBufferAccess,
    const std::shared_ptr<vk::dbg::Context> &dbgctx)
{
	// Do not optimize the shader if we have a debugger context.
	// Optimization passes are likely to damage debug information, and reorder
	// instructions.
	const bool optimize = !dbgctx;

	// TODO(b/147726513): Do not preprocess the shader if we have a debugger
	// context.
	// This is a work-around for the SPIR-V tools incorrectly reporting errors
	// when debug information is provided. This can be removed once the
	// following SPIR-V tools bugs are fixed:
	// https://github.com/KhronosGroup/SPIRV-Tools/issues/3102
	// https://github.com/KhronosGroup/SPIRV-Tools/issues/3103
	// https://github.com/KhronosGroup/SPIRV-Tools/issues/3118
	auto code = dbgctx ? key.getInsns() : preprocessSpirv(key.getInsns(), key.getSpecializationInfo(), optimize);
	ASSERT(code.size() > 0);

	// If the pipeline has specialization constants, assume they're unique and
	// use a new serial ID so the shader gets recompiled.
	uint32_t codeSerialID = (key.getSpecializationInfo() ? vk::ShaderModule::nextSerialID() : module->getSerialID());

	// TODO(b/119409619): use allocator.
	return std::make_shared<sw::SpirvShader>(codeSerialID, key.getPipelineStage(), key.getEntryPointName().c_str(),
	                                         code, key.getRenderPass(), key.getSubpassIndex(), robustBufferAccess, dbgctx);
}

std::shared_ptr<sw::ComputeProgram> createProgram(const vk::PipelineCache::ComputeProgramKey &key)
{
	MARL_SCOPED_EVENT("createProgram");

	vk::DescriptorSet::Bindings descriptorSets;  // FIXME(b/129523279): Delay code generation until invoke time.
	// TODO(b/119409619): use allocator.
	auto program = std::make_shared<sw::ComputeProgram>(key.getShader(), key.getLayout(), descriptorSets);
	program->generate();
	program->finalize();
	return program;
}

}  // anonymous namespace

namespace vk {

Pipeline::Pipeline(PipelineLayout const *layout, const Device *device)
    : layout(layout)
    , device(device)
    , robustBufferAccess(device->getEnabledFeatures().robustBufferAccess)
{
}

GraphicsPipeline::GraphicsPipeline(const VkGraphicsPipelineCreateInfo *pCreateInfo, void *mem, const Device *device)
    : Pipeline(vk::Cast(pCreateInfo->layout), device)
{
	context.robustBufferAccess = robustBufferAccess;

	if((pCreateInfo->flags &
	    ~(VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT |
	      VK_PIPELINE_CREATE_DERIVATIVE_BIT |
	      VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)) != 0)
	{
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	if(pCreateInfo->pTessellationState != nullptr)
	{
		UNSUPPORTED("pCreateInfo->pTessellationState");
	}

	if(pCreateInfo->pDynamicState)
	{
		if(pCreateInfo->pDynamicState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pDynamicState->flags %d", int(pCreateInfo->pDynamicState->flags));
		}

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
					UNSUPPORTED("VkDynamicState %d", int(dynamicState));
			}
		}
	}

	const VkPipelineVertexInputStateCreateInfo *vertexInputState = pCreateInfo->pVertexInputState;

	if(vertexInputState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("vertexInputState->flags");
	}

	// Context must always have a PipelineLayout set.
	context.pipelineLayout = layout;

	// Temporary in-binding-order representation of buffer strides, to be consumed below
	// when considering attributes. TODO: unfuse buffers from attributes in backend, is old GL model.
	uint32_t vertexStrides[MAX_VERTEX_INPUT_BINDINGS];
	uint32_t instanceStrides[MAX_VERTEX_INPUT_BINDINGS];
	for(uint32_t i = 0; i < vertexInputState->vertexBindingDescriptionCount; i++)
	{
		auto const &desc = vertexInputState->pVertexBindingDescriptions[i];
		vertexStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_VERTEX ? desc.stride : 0;
		instanceStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_INSTANCE ? desc.stride : 0;
	}

	for(uint32_t i = 0; i < vertexInputState->vertexAttributeDescriptionCount; i++)
	{
		auto const &desc = vertexInputState->pVertexAttributeDescriptions[i];
		sw::Stream &input = context.input[desc.location];
		input.format = desc.format;
		input.offset = desc.offset;
		input.binding = desc.binding;
		input.vertexStride = vertexStrides[desc.binding];
		input.instanceStride = instanceStrides[desc.binding];
	}

	const VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState = pCreateInfo->pInputAssemblyState;

	if(inputAssemblyState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->pInputAssemblyState->flags %d", int(pCreateInfo->pInputAssemblyState->flags));
	}

	primitiveRestartEnable = (inputAssemblyState->primitiveRestartEnable != VK_FALSE);
	context.topology = inputAssemblyState->topology;

	const VkPipelineViewportStateCreateInfo *viewportState = pCreateInfo->pViewportState;
	if(viewportState)
	{
		if(viewportState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pViewportState->flags %d", int(pCreateInfo->pViewportState->flags));
		}

		if((viewportState->viewportCount != 1) ||
		   (viewportState->scissorCount != 1))
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::multiViewport");
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

	const VkPipelineRasterizationStateCreateInfo *rasterizationState = pCreateInfo->pRasterizationState;

	if(rasterizationState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->pRasterizationState->flags %d", int(pCreateInfo->pRasterizationState->flags));
	}

	if(rasterizationState->depthClampEnable != VK_FALSE)
	{
		UNSUPPORTED("VkPhysicalDeviceFeatures::depthClamp");
	}

	context.rasterizerDiscard = (rasterizationState->rasterizerDiscardEnable != VK_FALSE);
	context.cullMode = rasterizationState->cullMode;
	context.frontFace = rasterizationState->frontFace;
	context.polygonMode = rasterizationState->polygonMode;
	context.depthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasConstantFactor : 0.0f;
	context.slopeDepthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasSlopeFactor : 0.0f;

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(rasterizationState->pNext);
	while(extensionCreateInfo)
	{
		// Casting to a long since some structures, such as
		// VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT
		// are not enumerated in the official Vulkan header
		switch((long)(extensionCreateInfo->sType))
		{
			case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO_EXT:
			{
				const VkPipelineRasterizationLineStateCreateInfoEXT *lineStateCreateInfo = reinterpret_cast<const VkPipelineRasterizationLineStateCreateInfoEXT *>(extensionCreateInfo);
				context.lineRasterizationMode = lineStateCreateInfo->lineRasterizationMode;
			}
			break;
			case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_PROVOKING_VERTEX_STATE_CREATE_INFO_EXT:
			{
				const VkPipelineRasterizationProvokingVertexStateCreateInfoEXT *provokingVertexModeCreateInfo =
				    reinterpret_cast<const VkPipelineRasterizationProvokingVertexStateCreateInfoEXT *>(extensionCreateInfo);
				context.provokingVertexMode = provokingVertexModeCreateInfo->provokingVertexMode;
			}
			break;
			default:
				WARN("pCreateInfo->pRasterizationState->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
				break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	const VkPipelineMultisampleStateCreateInfo *multisampleState = pCreateInfo->pMultisampleState;
	if(multisampleState)
	{
		if(multisampleState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pMultisampleState->flags %d", int(pCreateInfo->pMultisampleState->flags));
		}

		if(multisampleState->sampleShadingEnable != VK_FALSE)
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::sampleRateShading");
		}

		if(multisampleState->alphaToOneEnable != VK_FALSE)
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::alphaToOne");
		}

		switch(multisampleState->rasterizationSamples)
		{
			case VK_SAMPLE_COUNT_1_BIT:
				context.sampleCount = 1;
				break;
			case VK_SAMPLE_COUNT_4_BIT:
				context.sampleCount = 4;
				break;
			default:
				UNSUPPORTED("Unsupported sample count");
		}

		if(multisampleState->pSampleMask)
		{
			context.sampleMask = multisampleState->pSampleMask[0];
		}

		context.alphaToCoverage = (multisampleState->alphaToCoverageEnable != VK_FALSE);
	}
	else
	{
		context.sampleCount = 1;
	}

	const VkPipelineDepthStencilStateCreateInfo *depthStencilState = pCreateInfo->pDepthStencilState;
	if(depthStencilState)
	{
		if(depthStencilState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pDepthStencilState->flags %d", int(pCreateInfo->pDepthStencilState->flags));
		}

		if(depthStencilState->depthBoundsTestEnable != VK_FALSE)
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::depthBounds");
		}

		context.depthBoundsTestEnable = (depthStencilState->depthBoundsTestEnable != VK_FALSE);
		context.depthBufferEnable = (depthStencilState->depthTestEnable != VK_FALSE);
		context.depthWriteEnable = (depthStencilState->depthWriteEnable != VK_FALSE);
		context.depthCompareMode = depthStencilState->depthCompareOp;

		context.stencilEnable = (depthStencilState->stencilTestEnable != VK_FALSE);
		if(context.stencilEnable)
		{
			context.frontStencil = depthStencilState->front;
			context.backStencil = depthStencilState->back;
		}
	}

	const VkPipelineColorBlendStateCreateInfo *colorBlendState = pCreateInfo->pColorBlendState;
	if(colorBlendState)
	{
		if(pCreateInfo->pColorBlendState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pColorBlendState->flags %d", int(pCreateInfo->pColorBlendState->flags));
		}

		if(colorBlendState->logicOpEnable != VK_FALSE)
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::logicOp");
		}

		if(!hasDynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS))
		{
			blendConstants.x = colorBlendState->blendConstants[0];
			blendConstants.y = colorBlendState->blendConstants[1];
			blendConstants.z = colorBlendState->blendConstants[2];
			blendConstants.w = colorBlendState->blendConstants[3];
		}

		for(auto i = 0u; i < colorBlendState->attachmentCount; i++)
		{
			const VkPipelineColorBlendAttachmentState &attachment = colorBlendState->pAttachments[i];
			context.colorWriteMask[i] = attachment.colorWriteMask;

			context.setBlendState(i, { (attachment.blendEnable != VK_FALSE),
			                           attachment.srcColorBlendFactor, attachment.dstColorBlendFactor, attachment.colorBlendOp,
			                           attachment.srcAlphaBlendFactor, attachment.dstAlphaBlendFactor, attachment.alphaBlendOp });
		}
	}

	context.multiSampleMask = context.sampleMask & ((unsigned)0xFFFFFFFF >> (32 - context.sampleCount));
}

void GraphicsPipeline::destroyPipeline(const VkAllocationCallbacks *pAllocator)
{
	vertexShader.reset();
	fragmentShader.reset();
}

size_t GraphicsPipeline::ComputeRequiredAllocationSize(const VkGraphicsPipelineCreateInfo *pCreateInfo)
{
	return 0;
}

void GraphicsPipeline::setShader(const VkShaderStageFlagBits &stage, const std::shared_ptr<sw::SpirvShader> spirvShader)
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

const std::shared_ptr<sw::SpirvShader> GraphicsPipeline::getShader(const VkShaderStageFlagBits &stage) const
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

void GraphicsPipeline::compileShaders(const VkAllocationCallbacks *pAllocator, const VkGraphicsPipelineCreateInfo *pCreateInfo, PipelineCache *pPipelineCache)
{
	for(auto pStage = pCreateInfo->pStages; pStage != pCreateInfo->pStages + pCreateInfo->stageCount; pStage++)
	{
		if(pStage->flags != 0)
		{
			// Vulkan 1.2: "flags must be 0"
			UNSUPPORTED("pStage->flags %d", int(pStage->flags));
		}

		const ShaderModule *module = vk::Cast(pStage->module);
		const PipelineCache::SpirvShaderKey key(pStage->stage, pStage->pName, module->getCode(),
		                                        vk::Cast(pCreateInfo->renderPass), pCreateInfo->subpass,
		                                        pStage->pSpecializationInfo);
		auto pipelineStage = key.getPipelineStage();

		if(pPipelineCache)
		{
			PipelineCache &pipelineCache = *pPipelineCache;
			{
				std::unique_lock<std::mutex> lock(pipelineCache.getShaderMutex());
				const std::shared_ptr<sw::SpirvShader> *spirvShader = pipelineCache[key];
				if(!spirvShader)
				{
					auto shader = createShader(key, module, robustBufferAccess, device->getDebuggerContext());
					setShader(pipelineStage, shader);
					pipelineCache.insert(key, getShader(pipelineStage));
				}
				else
				{
					setShader(pipelineStage, *spirvShader);
				}
			}
		}
		else
		{
			auto shader = createShader(key, module, robustBufferAccess, device->getDebuggerContext());
			setShader(pipelineStage, shader);
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
			UNSUPPORTED("VkPrimitiveTopology %d", int(context.topology));
	}

	return 0;
}

const sw::Context &GraphicsPipeline::getContext() const
{
	return context;
}

const VkRect2D &GraphicsPipeline::getScissor() const
{
	return scissor;
}

const VkViewport &GraphicsPipeline::getViewport() const
{
	return viewport;
}

const sw::float4 &GraphicsPipeline::getBlendConstants() const
{
	return blendConstants;
}

bool GraphicsPipeline::hasDynamicState(VkDynamicState dynamicState) const
{
	return (dynamicStateFlags & (1 << dynamicState)) != 0;
}

ComputePipeline::ComputePipeline(const VkComputePipelineCreateInfo *pCreateInfo, void *mem, const Device *device)
    : Pipeline(vk::Cast(pCreateInfo->layout), device)
{
}

void ComputePipeline::destroyPipeline(const VkAllocationCallbacks *pAllocator)
{
	shader.reset();
	program.reset();
}

size_t ComputePipeline::ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo *pCreateInfo)
{
	return 0;
}

void ComputePipeline::compileShaders(const VkAllocationCallbacks *pAllocator, const VkComputePipelineCreateInfo *pCreateInfo, PipelineCache *pPipelineCache)
{
	auto &stage = pCreateInfo->stage;
	const ShaderModule *module = vk::Cast(stage.module);

	ASSERT(shader.get() == nullptr);
	ASSERT(program.get() == nullptr);

	const PipelineCache::SpirvShaderKey shaderKey(
	    stage.stage, stage.pName, module->getCode(), nullptr, 0, stage.pSpecializationInfo);
	if(pPipelineCache)
	{
		PipelineCache &pipelineCache = *pPipelineCache;
		{
			std::unique_lock<std::mutex> lock(pipelineCache.getShaderMutex());
			const std::shared_ptr<sw::SpirvShader> *spirvShader = pipelineCache[shaderKey];
			if(!spirvShader)
			{
				shader = createShader(shaderKey, module, robustBufferAccess, device->getDebuggerContext());
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
			const std::shared_ptr<sw::ComputeProgram> *computeProgram = pipelineCache[programKey];
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
		shader = createShader(shaderKey, module, robustBufferAccess, device->getDebuggerContext());
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

}  // namespace vk
