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
		Pointer<Pointer<Byte>> descriptorSetsIn = *Pointer<Pointer<Pointer<Byte>>>(data + OFFSET(Data, descriptorSets));
		size_t numDescriptorSets = routine.pipelineLayout->getNumDescriptorSets();
		for(unsigned int i = 0; i < numDescriptorSets; i++)
		{
			routine.descriptorSets[i] = descriptorSetsIn[i];
		}

		auto &modes = shader->getModes();

		Int4 numWorkgroups = *Pointer<Int4>(data + OFFSET(Data, numWorkgroups));
		Int4 workgroupID = *Pointer<Int4>(data + OFFSET(Data, workgroupID));
		Int4 workgroupSize = Int4(modes.LocalSizeX, modes.LocalSizeY, modes.LocalSizeZ, 0);

		setInputBuiltin(spv::BuiltInNumWorkgroups, [&](const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)
		{
			for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
			{
				value[builtin.FirstComponent + component] =
					As<Float4>(Int4(Extract(numWorkgroups, component)));
			}
		});

		setInputBuiltin(spv::BuiltInWorkgroupSize, [&](const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)
		{
			for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
			{
				value[builtin.FirstComponent + component] =
					As<Float4>(Int4(Extract(workgroupSize, component)));
			}
		});

		// Total number of invocations required to execute this workgroup.
		const int numInvocations = modes.LocalSizeX * modes.LocalSizeY * modes.LocalSizeZ;

		enum { XXXX, YYYY, ZZZZ };

		For(Int invocationIndex = 0, invocationIndex < numInvocations, invocationIndex += SIMD::Width)
		{
			Int4 localInvocationIndex = Int4(invocationIndex) + Int4(0, 1, 2, 3);

			Int4 localInvocationID[3];
			{
				Int4 idx = localInvocationIndex;
				localInvocationID[ZZZZ] = idx / Int4(modes.LocalSizeX * modes.LocalSizeY);
				idx -= localInvocationID[ZZZZ] * Int4(modes.LocalSizeX * modes.LocalSizeY); // modulo
				localInvocationID[YYYY] = idx / Int4(modes.LocalSizeX);
				idx -= localInvocationID[YYYY] * Int4(modes.LocalSizeX); // modulo
				localInvocationID[XXXX] = idx;
			}

			setInputBuiltin(spv::BuiltInLocalInvocationIndex, [&](const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)
			{
				ASSERT(builtin.SizeInComponents == 1);
				value[builtin.FirstComponent] = As<Float4>(localInvocationIndex);
			});

			setInputBuiltin(spv::BuiltInLocalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)
			{
				for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
				{
					value[builtin.FirstComponent + component] = As<Float4>(localInvocationID[component]);
				}
			});

			setInputBuiltin(spv::BuiltInGlobalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)
			{
				for (uint32_t component = 0; component < builtin.SizeInComponents; component++)
				{
					Int4 globalInvocationID =
						Int4(Extract(workgroupID, component)) *
						Int4(Extract(workgroupSize, component)) +
						localInvocationID[component];
					value[builtin.FirstComponent + component] = As<Float4>(globalInvocationID);
					// RR_WATCH(component, globalInvocationID);
				}
			});

			// TODO(bclayton): Disable lanes where (invocationIDs >= numInvocations)
			// Int4 enabledLanes = invocationIDs < Int4(numInvocations);

			// Process numLanes of the workgroup.
			shader->emit(&routine);
		}
	}

	void ComputeProgram::setInputBuiltin(spv::BuiltIn id, std::function<void(const SpirvShader::BuiltinMapping& builtin, Array<Float4>& value)> cb)
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
		uint32_t numDescriptorSets, void** descriptorSets,
		uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		auto runWorkgroup = (void(*)(void*))(routine->getEntry());

		Data data;
		data.descriptorSets = descriptorSets;
		data.numWorkgroups[0] = groupCountX;
		data.numWorkgroups[1] = groupCountY;
		data.numWorkgroups[2] = groupCountZ;
		data.numWorkgroups[3] = 0;

		// TODO(bclayton): Split work across threads.
		for (uint32_t groupZ = 0; groupZ < groupCountZ; groupZ++)
		{
			data.workgroupID[2] = groupZ;
			for (uint32_t groupY = 0; groupY < groupCountY; groupY++)
			{
				data.workgroupID[1] = groupY;
				for (uint32_t groupX = 0; groupX < groupCountX; groupX++)
				{
					data.workgroupID[0] = groupX;
					runWorkgroup(&data);
				}
			}
		}
	}
}
