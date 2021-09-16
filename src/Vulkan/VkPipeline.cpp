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

#include "VkDestroy.hpp"
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
sw::SpirvBinary preprocessSpirv(
    sw::SpirvBinary const &code,
    VkSpecializationInfo const *specializationInfo,
    bool optimize)
{
	spvtools::Optimizer opt{ vk::SPIRV_VERSION };

	opt.SetMessageConsumer([](spv_message_level_t level, const char *source, const spv_position_t &position, const char *message) {
		switch(level)
		{
		case SPV_MSG_FATAL: sw::warn("SPIR-V FATAL: %d:%d %s\n", int(position.line), int(position.column), message);
		case SPV_MSG_INTERNAL_ERROR: sw::warn("SPIR-V INTERNAL_ERROR: %d:%d %s\n", int(position.line), int(position.column), message);
		case SPV_MSG_ERROR: sw::warn("SPIR-V ERROR: %d:%d %s\n", int(position.line), int(position.column), message);
		case SPV_MSG_WARNING: sw::warn("SPIR-V WARNING: %d:%d %s\n", int(position.line), int(position.column), message);
		case SPV_MSG_INFO: sw::trace("SPIR-V INFO: %d:%d %s\n", int(position.line), int(position.column), message);
		case SPV_MSG_DEBUG: sw::trace("SPIR-V DEBUG: %d:%d %s\n", int(position.line), int(position.column), message);
		default: sw::trace("SPIR-V MESSAGE: %d:%d %s\n", int(position.line), int(position.column), message);
		}
	});

	// If the pipeline uses specialization, apply the specializations before freezing
	if(specializationInfo)
	{
		std::unordered_map<uint32_t, std::vector<uint32_t>> specializations;
		const uint8_t *specializationData = static_cast<const uint8_t *>(specializationInfo->pData);

		for(uint32_t i = 0; i < specializationInfo->mapEntryCount; i++)
		{
			const VkSpecializationMapEntry &entry = specializationInfo->pMapEntries[i];
			const uint8_t *value_ptr = specializationData + entry.offset;
			std::vector<uint32_t> value(reinterpret_cast<const uint32_t *>(value_ptr),
			                            reinterpret_cast<const uint32_t *>(value_ptr + entry.size));
			specializations.emplace(entry.constantID, std::move(value));
		}

		opt.RegisterPass(spvtools::CreateSetSpecConstantDefaultValuePass(specializations));
	}

	if(optimize)
	{
		// Full optimization list taken from spirv-opt.
		opt.RegisterPerformancePasses();
	}

	spvtools::OptimizerOptions optimizerOptions = {};
#if defined(NDEBUG)
	optimizerOptions.set_run_validator(false);
#else
	optimizerOptions.set_run_validator(true);
	spvtools::ValidatorOptions validatorOptions = {};
	validatorOptions.SetScalarBlockLayout(true);            // VK_EXT_scalar_block_layout
	validatorOptions.SetUniformBufferStandardLayout(true);  // VK_KHR_uniform_buffer_standard_layout
	optimizerOptions.set_validator_options(validatorOptions);
#endif

	sw::SpirvBinary optimized;
	opt.Run(code.data(), code.size(), &optimized, optimizerOptions);

	if(false)
	{
		spvtools::SpirvTools core(vk::SPIRV_VERSION);
		std::string preOpt;
		core.Disassemble(code, &preOpt, SPV_BINARY_TO_TEXT_OPTION_NONE);
		std::string postOpt;
		core.Disassemble(optimized, &postOpt, SPV_BINARY_TO_TEXT_OPTION_NONE);
		std::cout << "PRE-OPT: " << preOpt << std::endl
		          << "POST-OPT: " << postOpt << std::endl;
	}

	return optimized;
}

sw::SpirvBinary optimizeSpirv(const vk::PipelineCache::SpirvShaderKey &key)
{
	auto code = preprocessSpirv(key.getInsns(), key.getSpecializationInfo(), key.getOptimization());
	ASSERT(code.size() > 0);

	return code;
}

std::shared_ptr<sw::ComputeProgram> createProgram(vk::Device *device, std::shared_ptr<sw::SpirvShader> shader, const vk::PipelineLayout *layout)
{
	MARL_SCOPED_EVENT("createProgram");

	vk::DescriptorSet::Bindings descriptorSets;  // TODO(b/129523279): Delay code generation until dispatch time.
	// TODO(b/119409619): use allocator.
	auto program = std::make_shared<sw::ComputeProgram>(device, shader, layout, descriptorSets);
	program->generate();
	program->finalize("ComputeProgram");

	return program;
}

}  // anonymous namespace

namespace vk {

Pipeline::Pipeline(PipelineLayout *layout, Device *device)
    : layout(layout)
    , device(device)
    , robustBufferAccess(device->getEnabledFeatures().robustBufferAccess)
{
	layout->incRefCount();
}

void Pipeline::destroy(const VkAllocationCallbacks *pAllocator)
{
	destroyPipeline(pAllocator);

	vk::release(static_cast<VkPipelineLayout>(*layout), pAllocator);
}

GraphicsPipeline::GraphicsPipeline(const VkGraphicsPipelineCreateInfo *pCreateInfo, void *mem, Device *device)
    : Pipeline(vk::Cast(pCreateInfo->layout), device)
    , state(device, pCreateInfo, layout, robustBufferAccess)
    , inputs(pCreateInfo->pVertexInputState)
{
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

void GraphicsPipeline::getIndexBuffers(uint32_t count, uint32_t first, bool indexed, std::vector<std::pair<uint32_t, void *>> *indexBuffers) const
{
	indexBuffer.getIndexBuffers(state.getTopology(), count, first, indexed, state.hasPrimitiveRestartEnable(), indexBuffers);
}

bool GraphicsPipeline::containsImageWrite() const
{
	return (vertexShader.get() && vertexShader->containsImageWrite()) ||
	       (fragmentShader.get() && fragmentShader->containsImageWrite());
}

void GraphicsPipeline::setShader(const VkShaderStageFlagBits &stage, const std::shared_ptr<sw::SpirvShader> spirvShader)
{
	switch(stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		ASSERT(vertexShader.get() == nullptr);
		vertexShader = spirvShader;
		break;

	case VK_SHADER_STAGE_FRAGMENT_BIT:
		ASSERT(fragmentShader.get() == nullptr);
		fragmentShader = spirvShader;
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

		auto dbgctx = device->getDebuggerContext();
		// Do not optimize the shader if we have a debugger context.
		// Optimization passes are likely to damage debug information, and reorder
		// instructions.
		const bool optimize = !dbgctx;

		const ShaderModule *module = vk::Cast(pStage->module);
		const PipelineCache::SpirvShaderKey key(module->getCode(), pStage->pSpecializationInfo, optimize);

		sw::SpirvBinary spirv;

		if(pPipelineCache)
		{
			spirv = pPipelineCache->getOrOptimizeSpirv(key, [&] {
				return optimizeSpirv(key);
			});
		}
		else
		{
			spirv = optimizeSpirv(key);
		}

		// If the pipeline has specialization constants, assume they're unique and
		// use a new serial ID so the shader gets recompiled.
		uint32_t codeSerialID = (key.getSpecializationInfo() ? vk::ShaderModule::nextSerialID() : module->getSerialID());

		// TODO(b/119409619): use allocator.
		auto shader = std::make_shared<sw::SpirvShader>(codeSerialID, pStage->stage, pStage->pName, spirv,
		                                                vk::Cast(pCreateInfo->renderPass), pCreateInfo->subpass, robustBufferAccess, dbgctx);

		setShader(pStage->stage, shader);
	}
}

ComputePipeline::ComputePipeline(const VkComputePipelineCreateInfo *pCreateInfo, void *mem, Device *device)
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

	auto dbgctx = device->getDebuggerContext();
	// Do not optimize the shader if we have a debugger context.
	// Optimization passes are likely to damage debug information, and reorder
	// instructions.
	const bool optimize = !dbgctx;

	const PipelineCache::SpirvShaderKey shaderKey(module->getCode(), stage.pSpecializationInfo, optimize);

	sw::SpirvBinary spirv;

	if(pPipelineCache)
	{
		spirv = pPipelineCache->getOrOptimizeSpirv(shaderKey, [&] {
			return optimizeSpirv(shaderKey);
		});
	}
	else
	{
		spirv = optimizeSpirv(shaderKey);
	}

	// If the pipeline has specialization constants, assume they're unique and
	// use a new serial ID so the shader gets recompiled.
	uint32_t codeSerialID = (stage.pSpecializationInfo ? vk::ShaderModule::nextSerialID() : module->getSerialID());

	// TODO(b/119409619): use allocator.
	shader = std::make_shared<sw::SpirvShader>(codeSerialID, stage.stage, stage.pName, spirv,
	                                           nullptr, 0, robustBufferAccess, dbgctx);

	const PipelineCache::ComputeProgramKey programKey(shader->getSerialID(), layout->identifier);

	if(pPipelineCache)
	{
		program = pPipelineCache->getOrCreateComputeProgram(programKey, [&] {
			return createProgram(device, shader, layout);
		});
	}
	else
	{
		program = createProgram(device, shader, layout);
	}
}

void ComputePipeline::run(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                          uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ,
                          vk::DescriptorSet::Array const &descriptorSetObjects,
                          vk::DescriptorSet::Bindings const &descriptorSets,
                          vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
                          vk::Pipeline::PushConstantStorage const &pushConstants)
{
	ASSERT_OR_RETURN(program != nullptr);
	program->run(
	    descriptorSetObjects, descriptorSets, descriptorDynamicOffsets, pushConstants,
	    baseGroupX, baseGroupY, baseGroupZ,
	    groupCountX, groupCountY, groupCountZ);
}

}  // namespace vk
