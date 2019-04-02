// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "ComputeProgram.hpp"

#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkPipelineLayout.hpp"

namespace
{
	enum { X, Y, Z };
} // anonymous namespace

namespace sw
{
	ComputeProgram::ComputeProgram(SpirvShader const *shader, vk::PipelineLayout const *pipelineLayout)
		: data(Arg<0>()),
		  routine(pipelineLayout),
		  shader(shader),
		  pipelineLayout(pipelineLayout)
	{
	}

	ComputeProgram::~ComputeProgram()
	{
	}

	void ComputeProgram::generate()
	{
		shader->emitProlog(&routine);
		emit();
		shader->emitEpilog(&routine);
	}

	void ComputeProgram::emit()
	{
		routine.descriptorSets = data + OFFSET(Data, descriptorSets);
		routine.descriptorDynamicOffsets = data + OFFSET(Data, descriptorDynamicOffsets);
		routine.pushConstants = data + OFFSET(Data, pushConstants);

		auto &modes = shader->getModes();

		int localSize[3] = {modes.WorkgroupSizeX, modes.WorkgroupSizeY, modes.WorkgroupSizeZ};

		const int subgroupSize = SIMD::Width;

		// Total number of invocations required to execute this workgroup.
		int numInvocations = localSize[X] * localSize[Y] * localSize[Z];

		Int4 numWorkgroups = *Pointer<Int4>(data + OFFSET(Data, numWorkgroups));
		Int4 workgroupID = *Pointer<Int4>(data + OFFSET(Data, workgroupID));
		Int4 workgroupSize = Int4(localSize[X], localSize[Y], localSize[Z], 0);
		Int numSubgroups = (numInvocations + subgroupSize - 1) / subgroupSize;

		setInputBuiltin(spv::BuiltInNumWorkgroups, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
			{
				value[builtin.FirstComponent + component] =
					As<SIMD::Float>(SIMD::Int(Extract(numWorkgroups, component)));
			}
		});

		setInputBuiltin(spv::BuiltInWorkgroupId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
			{
				value[builtin.FirstComponent + component] =
						As<SIMD::Float>(SIMD::Int(Extract(workgroupID, component)));
			}
		});

		setInputBuiltin(spv::BuiltInWorkgroupSize, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
			{
				value[builtin.FirstComponent + component] =
					As<SIMD::Float>(SIMD::Int(Extract(workgroupSize, component)));
			}
		});

		setInputBuiltin(spv::BuiltInNumSubgroups, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 1);
			value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(numSubgroups));
		});

		setInputBuiltin(spv::BuiltInSubgroupSize, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 1);
			value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(subgroupSize));
		});

		setInputBuiltin(spv::BuiltInSubgroupLocalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 1);
			value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(0, 1, 2, 3));
		});

		For(Int subgroupIndex = 0, subgroupIndex < numSubgroups, subgroupIndex++)
		{
			// TODO: Replace SIMD::Int(0, 1, 2, 3) with SIMD-width equivalent
			auto localInvocationIndex = SIMD::Int(subgroupIndex * SIMD::Width) + SIMD::Int(0, 1, 2, 3);

			// Disable lanes where (invocationIDs >= numInvocations)
			auto activeLaneMask = CmpLT(localInvocationIndex, SIMD::Int(numInvocations));

			SIMD::Int localInvocationID[3];
			{
				SIMD::Int idx = localInvocationIndex;
				localInvocationID[Z] = idx / SIMD::Int(localSize[X] * localSize[Y]);
				idx -= localInvocationID[Z] * SIMD::Int(localSize[X] * localSize[Y]); // modulo
				localInvocationID[Y] = idx / SIMD::Int(localSize[X]);
				idx -= localInvocationID[Y] * SIMD::Int(localSize[X]); // modulo
				localInvocationID[X] = idx;
			}

			setInputBuiltin(spv::BuiltInLocalInvocationIndex, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
			{
				ASSERT(builtin.SizeInComponents == 1);
				value[builtin.FirstComponent] = As<SIMD::Float>(localInvocationIndex);
			});

			setInputBuiltin(spv::BuiltInSubgroupId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
			{
				ASSERT(builtin.SizeInComponents == 1);
				value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(subgroupIndex));
			});

			setInputBuiltin(spv::BuiltInLocalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
			{
				for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
				{
					value[builtin.FirstComponent + component] = As<SIMD::Float>(localInvocationID[component]);
				}
			});

			setInputBuiltin(spv::BuiltInGlobalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
			{
				auto localBase = workgroupID * workgroupSize;
				for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
				{
					auto globalInvocationID = SIMD::Int(Extract(localBase, component)) + localInvocationID[component];
					value[builtin.FirstComponent + component] = As<SIMD::Float>(globalInvocationID);
				}
			});

			// Process numLanes of the workgroup.
			shader->emit(&routine, activeLaneMask);
		}
	}

	void ComputeProgram::setInputBuiltin(spv::BuiltIn id, std::function<void(const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)> cb)
	{
		auto it = shader->inputBuiltins.find(id);
		if (it != shader->inputBuiltins.end())
		{
			const auto& builtin = it->second;
			auto &value = routine.getValue(builtin.Id);
			cb(builtin, value);
		}
	}

	void ComputeProgram::run(
		Routine *routine,
		vk::DescriptorSet::Bindings const &descriptorSets,
		vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
		PushConstantStorage const &pushConstants,
		uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		auto runWorkgroup = (void(*)(void*))(routine->getEntry());

		Data data;
		data.descriptorSets = descriptorSets;
		data.descriptorDynamicOffsets = descriptorDynamicOffsets;
		data.numWorkgroups[X] = groupCountX;
		data.numWorkgroups[Y] = groupCountY;
		data.numWorkgroups[Z] = groupCountZ;
		data.numWorkgroups[3] = 0;
		data.pushConstants = pushConstants;

		// TODO(bclayton): Split work across threads.
		for (uint32_t groupZ = 0; groupZ < groupCountZ; groupZ++)
		{
			data.workgroupID[Z] = groupZ;
			for (uint32_t groupY = 0; groupY < groupCountY; groupY++)
			{
				data.workgroupID[Y] = groupY;
				for (uint32_t groupX = 0; groupX < groupCountX; groupX++)
				{
					data.workgroupID[X] = groupX;
					runWorkgroup(&data);
				}
			}
		}
	}
}
