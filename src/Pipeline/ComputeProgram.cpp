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
#include "Constants.hpp"

#include "System/Debug.hpp"
#include "Vulkan/VkPipelineLayout.hpp"

#include "marl/defer.h"
#include "marl/trace.h"
#include "marl/waitgroup.h"

#include <queue>

namespace {

enum
{
	X,
	Y,
	Z
};

}  // anonymous namespace

namespace sw {

ComputeProgram::ComputeProgram(SpirvShader const *shader, vk::PipelineLayout const *pipelineLayout, const vk::DescriptorSet::Bindings &descriptorSets)
    : shader(shader)
    , pipelineLayout(pipelineLayout)
    , descriptorSets(descriptorSets)
{
}

ComputeProgram::~ComputeProgram()
{
}

void ComputeProgram::generate()
{
	MARL_SCOPED_EVENT("ComputeProgram::generate");

	SpirvRoutine routine(pipelineLayout);
	shader->emitProlog(&routine);
	emit(&routine);
	shader->emitEpilog(&routine);
}

void ComputeProgram::setWorkgroupBuiltins(Pointer<Byte> data, SpirvRoutine *routine, Int workgroupID[3])
{
	// TODO(b/146486064): Consider only assigning these to the SpirvRoutine iff
	// they are ever going to be read.
	routine->numWorkgroups = *Pointer<Int4>(data + OFFSET(Data, numWorkgroups));
	routine->workgroupID = Insert(Insert(Insert(Int4(0), workgroupID[X], X), workgroupID[Y], Y), workgroupID[Z], Z);
	routine->workgroupSize = *Pointer<Int4>(data + OFFSET(Data, workgroupSize));
	routine->subgroupsPerWorkgroup = *Pointer<Int>(data + OFFSET(Data, subgroupsPerWorkgroup));
	routine->invocationsPerSubgroup = *Pointer<Int>(data + OFFSET(Data, invocationsPerSubgroup));

	routine->setInputBuiltin(shader, spv::BuiltInNumWorkgroups, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		for(uint32_t component = 0; component < builtin.SizeInComponents; component++)
		{
			value[builtin.FirstComponent + component] =
			    As<SIMD::Float>(SIMD::Int(Extract(routine->numWorkgroups, component)));
		}
	});

	routine->setInputBuiltin(shader, spv::BuiltInWorkgroupId, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		for(uint32_t component = 0; component < builtin.SizeInComponents; component++)
		{
			value[builtin.FirstComponent + component] =
			    As<SIMD::Float>(SIMD::Int(workgroupID[component]));
		}
	});

	routine->setInputBuiltin(shader, spv::BuiltInWorkgroupSize, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		for(uint32_t component = 0; component < builtin.SizeInComponents; component++)
		{
			value[builtin.FirstComponent + component] =
			    As<SIMD::Float>(SIMD::Int(Extract(routine->workgroupSize, component)));
		}
	});

	routine->setInputBuiltin(shader, spv::BuiltInNumSubgroups, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(routine->subgroupsPerWorkgroup));
	});

	routine->setInputBuiltin(shader, spv::BuiltInSubgroupSize, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(routine->invocationsPerSubgroup));
	});

	routine->setImmutableInputBuiltins(shader);
}

void ComputeProgram::setSubgroupBuiltins(Pointer<Byte> data, SpirvRoutine *routine, Int workgroupID[3], SIMD::Int localInvocationIndex, Int subgroupIndex)
{
	Int4 numWorkgroups = *Pointer<Int4>(data + OFFSET(Data, numWorkgroups));
	Int4 workgroupSize = *Pointer<Int4>(data + OFFSET(Data, workgroupSize));

	// TODO: Fix Int4 swizzles so we can just use workgroupSize.x, workgroupSize.y.
	Int workgroupSizeX = Extract(workgroupSize, X);
	Int workgroupSizeY = Extract(workgroupSize, Y);

	SIMD::Int localInvocationID[3];
	{
		SIMD::Int idx = localInvocationIndex;
		localInvocationID[Z] = idx / SIMD::Int(workgroupSizeX * workgroupSizeY);
		idx -= localInvocationID[Z] * SIMD::Int(workgroupSizeX * workgroupSizeY);  // modulo
		localInvocationID[Y] = idx / SIMD::Int(workgroupSizeX);
		idx -= localInvocationID[Y] * SIMD::Int(workgroupSizeX);  // modulo
		localInvocationID[X] = idx;
	}

	Int4 wgID = Insert(Insert(Insert(SIMD::Int(0), workgroupID[X], X), workgroupID[Y], Y), workgroupID[Z], Z);
	auto localBase = workgroupSize * wgID;
	SIMD::Int globalInvocationID[3];
	globalInvocationID[X] = SIMD::Int(Extract(localBase, X)) + localInvocationID[X];
	globalInvocationID[Y] = SIMD::Int(Extract(localBase, Y)) + localInvocationID[Y];
	globalInvocationID[Z] = SIMD::Int(Extract(localBase, Z)) + localInvocationID[Z];

	routine->localInvocationIndex = localInvocationIndex;
	routine->subgroupIndex = subgroupIndex;
	routine->localInvocationID[X] = localInvocationID[X];
	routine->localInvocationID[Y] = localInvocationID[Y];
	routine->localInvocationID[Z] = localInvocationID[Z];
	routine->globalInvocationID[X] = globalInvocationID[X];
	routine->globalInvocationID[Y] = globalInvocationID[Y];
	routine->globalInvocationID[Z] = globalInvocationID[Z];

	routine->setInputBuiltin(shader, spv::BuiltInLocalInvocationIndex, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		value[builtin.FirstComponent] = As<SIMD::Float>(localInvocationIndex);
	});

	routine->setInputBuiltin(shader, spv::BuiltInSubgroupId, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(subgroupIndex));
	});

	routine->setInputBuiltin(shader, spv::BuiltInLocalInvocationId, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		for(uint32_t component = 0; component < builtin.SizeInComponents; component++)
		{
			value[builtin.FirstComponent + component] =
			    As<SIMD::Float>(localInvocationID[component]);
		}
	});

	routine->setInputBuiltin(shader, spv::BuiltInGlobalInvocationId, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		for(uint32_t component = 0; component < builtin.SizeInComponents; component++)
		{
			value[builtin.FirstComponent + component] =
			    As<SIMD::Float>(globalInvocationID[component]);
		}
	});
}

void ComputeProgram::emit(SpirvRoutine *routine)
{
	Pointer<Byte> data = Arg<0>();
	Int workgroupX = Arg<1>();
	Int workgroupY = Arg<2>();
	Int workgroupZ = Arg<3>();
	Pointer<Byte> workgroupMemory = Arg<4>();
	Int firstSubgroup = Arg<5>();
	Int subgroupCount = Arg<6>();

	routine->descriptorSets = data + OFFSET(Data, descriptorSets);
	routine->descriptorDynamicOffsets = data + OFFSET(Data, descriptorDynamicOffsets);
	routine->pushConstants = data + OFFSET(Data, pushConstants);
	routine->constants = *Pointer<Pointer<Byte>>(data + OFFSET(Data, constants));
	routine->workgroupMemory = workgroupMemory;

	Int invocationsPerWorkgroup = *Pointer<Int>(data + OFFSET(Data, invocationsPerWorkgroup));

	Int workgroupID[3] = { workgroupX, workgroupY, workgroupZ };
	setWorkgroupBuiltins(data, routine, workgroupID);

	For(Int i = 0, i < subgroupCount, i++)
	{
		auto subgroupIndex = firstSubgroup + i;

		// TODO: Replace SIMD::Int(0, 1, 2, 3) with SIMD-width equivalent
		auto localInvocationIndex = SIMD::Int(subgroupIndex * SIMD::Width) + SIMD::Int(0, 1, 2, 3);

		// Disable lanes where (invocationIDs >= invocationsPerWorkgroup)
		auto activeLaneMask = CmpLT(localInvocationIndex, SIMD::Int(invocationsPerWorkgroup));

		setSubgroupBuiltins(data, routine, workgroupID, localInvocationIndex, subgroupIndex);

		shader->emit(routine, activeLaneMask, activeLaneMask, descriptorSets);
	}
}

void ComputeProgram::run(
    vk::DescriptorSet::Bindings const &descriptorSets,
    vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
    PushConstantStorage const &pushConstants,
    uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
    uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	auto &modes = shader->getModes();

	auto invocationsPerSubgroup = SIMD::Width;
	auto invocationsPerWorkgroup = modes.WorkgroupSizeX * modes.WorkgroupSizeY * modes.WorkgroupSizeZ;
	auto subgroupsPerWorkgroup = (invocationsPerWorkgroup + invocationsPerSubgroup - 1) / invocationsPerSubgroup;

	Data data;
	data.descriptorSets = descriptorSets;
	data.descriptorDynamicOffsets = descriptorDynamicOffsets;
	data.numWorkgroups[X] = groupCountX;
	data.numWorkgroups[Y] = groupCountY;
	data.numWorkgroups[Z] = groupCountZ;
	data.numWorkgroups[3] = 0;
	data.workgroupSize[X] = modes.WorkgroupSizeX;
	data.workgroupSize[Y] = modes.WorkgroupSizeY;
	data.workgroupSize[Z] = modes.WorkgroupSizeZ;
	data.workgroupSize[3] = 0;
	data.invocationsPerSubgroup = invocationsPerSubgroup;
	data.invocationsPerWorkgroup = invocationsPerWorkgroup;
	data.subgroupsPerWorkgroup = subgroupsPerWorkgroup;
	data.pushConstants = pushConstants;
	data.constants = &sw::constants;

	marl::WaitGroup wg;
	const uint32_t batchCount = 16;

	auto groupCount = groupCountX * groupCountY * groupCountZ;

	for(uint32_t batchID = 0; batchID < batchCount && batchID < groupCount; batchID++)
	{
		wg.add(1);
		marl::schedule([=, &data] {
			defer(wg.done());
			std::vector<uint8_t> workgroupMemory(shader->workgroupMemory.size());

			for(uint32_t groupIndex = batchID; groupIndex < groupCount; groupIndex += batchCount)
			{
				auto modulo = groupIndex;
				auto groupOffsetZ = modulo / (groupCountX * groupCountY);
				modulo -= groupOffsetZ * (groupCountX * groupCountY);
				auto groupOffsetY = modulo / groupCountX;
				modulo -= groupOffsetY * groupCountX;
				auto groupOffsetX = modulo;

				auto groupZ = baseGroupZ + groupOffsetZ;
				auto groupY = baseGroupY + groupOffsetY;
				auto groupX = baseGroupX + groupOffsetX;
				MARL_SCOPED_EVENT("groupX: %d, groupY: %d, groupZ: %d", groupX, groupY, groupZ);

				using Coroutine = std::unique_ptr<rr::Stream<SpirvShader::YieldResult>>;
				std::queue<Coroutine> coroutines;

				if(modes.ContainsControlBarriers)
				{
					// Make a function call per subgroup so each subgroup
					// can yield, bringing all subgroups to the barrier
					// together.
					for(int subgroupIndex = 0; subgroupIndex < subgroupsPerWorkgroup; subgroupIndex++)
					{
						auto coroutine = (*this)(&data, groupX, groupY, groupZ, workgroupMemory.data(), subgroupIndex, 1);
						coroutines.push(std::move(coroutine));
					}
				}
				else
				{
					auto coroutine = (*this)(&data, groupX, groupY, groupZ, workgroupMemory.data(), 0, subgroupsPerWorkgroup);
					coroutines.push(std::move(coroutine));
				}

				while(coroutines.size() > 0)
				{
					auto coroutine = std::move(coroutines.front());
					coroutines.pop();

					SpirvShader::YieldResult result;
					if(coroutine->await(result))
					{
						// TODO: Consider result (when the enum is more than 1 entry).
						coroutines.push(std::move(coroutine));
					}
				}
			}
		});
	}

	wg.wait();
}

}  // namespace sw
