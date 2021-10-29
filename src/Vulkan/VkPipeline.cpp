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

// optimizeSpirv() applies and freezes specializations into constants, and runs spirv-opt.
sw::SpirvBinary optimizeSpirv(const vk::PipelineCache::SpirvBinaryKey &key)
{
	const sw::SpirvBinary &code = key.getBinary();
	const VkSpecializationInfo *specializationInfo = key.getSpecializationInfo();
	bool optimize = key.getOptimization();

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
	ASSERT(optimized.size() > 0);

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

class PipelineCreationFeedback
{
public:
	PipelineCreationFeedback(const VkGraphicsPipelineCreateInfo *pCreateInfo)
	    : pipelineCreationFeedback(GetPipelineCreationFeedback(pCreateInfo->pNext))
	{
		pipelineCreationBegins();
	}

	PipelineCreationFeedback(const VkComputePipelineCreateInfo *pCreateInfo)
	    : pipelineCreationFeedback(GetPipelineCreationFeedback(pCreateInfo->pNext))
	{
		pipelineCreationBegins();
	}

	~PipelineCreationFeedback()
	{
		pipelineCreationEnds();
	}

	void stageCreationBegins(uint32_t stage)
	{
		if(pipelineCreationFeedback)
		{
			// Record stage creation begin time
			pipelineCreationFeedback->pPipelineStageCreationFeedbacks[stage].duration = now();
		}
	}

	void cacheHit(uint32_t stage)
	{
		if(pipelineCreationFeedback)
		{
			pipelineCreationFeedback->pPipelineCreationFeedback->flags |=
			    VK_PIPELINE_CREATION_FEEDBACK_APPLICATION_PIPELINE_CACHE_HIT_BIT;
			pipelineCreationFeedback->pPipelineStageCreationFeedbacks[stage].flags |=
			    VK_PIPELINE_CREATION_FEEDBACK_APPLICATION_PIPELINE_CACHE_HIT_BIT;
		}
	}

	void stageCreationEnds(uint32_t stage)
	{
		if(pipelineCreationFeedback)
		{
			pipelineCreationFeedback->pPipelineStageCreationFeedbacks[stage].flags |=
			    VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT;
			pipelineCreationFeedback->pPipelineStageCreationFeedbacks[stage].duration =
			    now() - pipelineCreationFeedback->pPipelineStageCreationFeedbacks[stage].duration;
		}
	}

	void pipelineCreationError()
	{
		clear();
		pipelineCreationFeedback = nullptr;
	}

private:
	static const VkPipelineCreationFeedbackCreateInfo *GetPipelineCreationFeedback(const void *pNext)
	{
		const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(pNext);
		while(extensionCreateInfo)
		{
			if(extensionCreateInfo->sType == VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO)
			{
				return reinterpret_cast<const VkPipelineCreationFeedbackCreateInfo *>(extensionCreateInfo);
			}

			extensionCreateInfo = extensionCreateInfo->pNext;
		}

		return nullptr;
	}

	void pipelineCreationBegins()
	{
		if(pipelineCreationFeedback)
		{
			clear();

			// Record pipeline creation begin time
			pipelineCreationFeedback->pPipelineCreationFeedback->duration = now();
		}
	}

	void pipelineCreationEnds()
	{
		if(pipelineCreationFeedback)
		{
			pipelineCreationFeedback->pPipelineCreationFeedback->flags |=
			    VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT;
			pipelineCreationFeedback->pPipelineCreationFeedback->duration =
			    now() - pipelineCreationFeedback->pPipelineCreationFeedback->duration;
		}
	}

	void clear()
	{
		if(pipelineCreationFeedback)
		{
			// Clear all flags and durations
			pipelineCreationFeedback->pPipelineCreationFeedback->flags = 0;
			pipelineCreationFeedback->pPipelineCreationFeedback->duration = 0;
			for(uint32_t i = 0; i < pipelineCreationFeedback->pipelineStageCreationFeedbackCount; i++)
			{
				pipelineCreationFeedback->pPipelineStageCreationFeedbacks[i].flags = 0;
				pipelineCreationFeedback->pPipelineStageCreationFeedbacks[i].duration = 0;
			}
		}
	}

	uint64_t now()
	{
		return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	}

	const VkPipelineCreationFeedbackCreateInfo *pipelineCreationFeedback = nullptr;
};

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

VkResult GraphicsPipeline::compileShaders(const VkAllocationCallbacks *pAllocator, const VkGraphicsPipelineCreateInfo *pCreateInfo, PipelineCache *pPipelineCache)
{
	PipelineCreationFeedback pipelineCreationFeedback(pCreateInfo);

	for(uint32_t stageIndex = 0; stageIndex < pCreateInfo->stageCount; stageIndex++)
	{
		const VkPipelineShaderStageCreateInfo &stageInfo = pCreateInfo->pStages[stageIndex];

		pipelineCreationFeedback.stageCreationBegins(stageIndex);

		if((stageInfo.flags &
		    ~(VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT |
		      VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT)) != 0)
		{
			UNSUPPORTED("pStage->flags %d", int(stageInfo.flags));
		}

		auto dbgctx = device->getDebuggerContext();
		// Do not optimize the shader if we have a debugger context.
		// Optimization passes are likely to damage debug information, and reorder
		// instructions.
		const bool optimize = !dbgctx;

		const ShaderModule *module = vk::Cast(stageInfo.module);
		const PipelineCache::SpirvBinaryKey key(module->getBinary(), stageInfo.pSpecializationInfo, optimize);

		if((pCreateInfo->flags & VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT_EXT) &&
		   (!pPipelineCache || !pPipelineCache->contains(key)))
		{
			pipelineCreationFeedback.pipelineCreationError();
			return VK_PIPELINE_COMPILE_REQUIRED_EXT;
		}

		sw::SpirvBinary spirv;

		if(pPipelineCache)
		{
			auto onCacheMiss = [&] { return optimizeSpirv(key); };
			auto onCacheHit = [&] { pipelineCreationFeedback.cacheHit(stageIndex); };
			spirv = pPipelineCache->getOrOptimizeSpirv(key, onCacheMiss, onCacheHit);
		}
		else
		{
			spirv = optimizeSpirv(key);

			// If the pipeline does not have specialization constants, there's a 1-to-1 mapping between the unoptimized and optimized SPIR-V,
			// so we should use a 1-to-1 mapping of the identifiers to avoid JIT routine recompiles.
			if(!key.getSpecializationInfo())
			{
				spirv.mapOptimizedIdentifier(key.getBinary());
			}
		}

		// TODO(b/201798871): use allocator.
		auto shader = std::make_shared<sw::SpirvShader>(stageInfo.stage, stageInfo.pName, spirv,
		                                                vk::Cast(pCreateInfo->renderPass), pCreateInfo->subpass, robustBufferAccess, dbgctx);

		setShader(stageInfo.stage, shader);

		pipelineCreationFeedback.stageCreationEnds(stageIndex);
	}

	return VK_SUCCESS;
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

VkResult ComputePipeline::compileShaders(const VkAllocationCallbacks *pAllocator, const VkComputePipelineCreateInfo *pCreateInfo, PipelineCache *pPipelineCache)
{
	PipelineCreationFeedback pipelineCreationFeedback(pCreateInfo);
	pipelineCreationFeedback.stageCreationBegins(0);

	auto &stage = pCreateInfo->stage;
	const ShaderModule *module = vk::Cast(stage.module);

	ASSERT(shader.get() == nullptr);
	ASSERT(program.get() == nullptr);

	auto dbgctx = device->getDebuggerContext();
	// Do not optimize the shader if we have a debugger context.
	// Optimization passes are likely to damage debug information, and reorder
	// instructions.
	const bool optimize = !dbgctx;

	const PipelineCache::SpirvBinaryKey shaderKey(module->getBinary(), stage.pSpecializationInfo, optimize);

	if((pCreateInfo->flags & VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT_EXT) &&
	   (!pPipelineCache || !pPipelineCache->contains(shaderKey)))
	{
		pipelineCreationFeedback.pipelineCreationError();
		return VK_PIPELINE_COMPILE_REQUIRED_EXT;
	}

	sw::SpirvBinary spirv;

	if(pPipelineCache)
	{
		auto onCacheMiss = [&] { return optimizeSpirv(shaderKey); };
		auto onCacheHit = [&] { pipelineCreationFeedback.cacheHit(0); };
		spirv = pPipelineCache->getOrOptimizeSpirv(shaderKey, onCacheMiss, onCacheHit);
	}
	else
	{
		spirv = optimizeSpirv(shaderKey);

		// If the pipeline does not have specialization constants, there's a 1-to-1 mapping between the unoptimized and optimized SPIR-V,
		// so we should use a 1-to-1 mapping of the identifiers to avoid JIT routine recompiles.
		if(!shaderKey.getSpecializationInfo())
		{
			spirv.mapOptimizedIdentifier(shaderKey.getBinary());
		}
	}

	// TODO(b/201798871): use allocator.
	shader = std::make_shared<sw::SpirvShader>(stage.stage, stage.pName, spirv,
	                                           nullptr, 0, robustBufferAccess, dbgctx);

	const PipelineCache::ComputeProgramKey programKey(shader->getIdentifier(), layout->identifier);

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

	pipelineCreationFeedback.stageCreationEnds(0);

	return VK_SUCCESS;
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
