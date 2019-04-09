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

#include "VertexProgram.hpp"

#include "SamplerCore.hpp"
#include "Device/Renderer.hpp"
#include "Device/Vertex.hpp"
#include "System/Half.hpp"
#include "Vulkan/VkDebug.hpp"

#include "Vulkan/VkPipelineLayout.hpp"

namespace sw
{
	VertexProgram::VertexProgram(
			const VertexProcessor::State &state,
			vk::PipelineLayout const *pipelineLayout,
			SpirvShader const *spirvShader,
			const vk::DescriptorSet::Bindings &descriptorSets)
		: VertexRoutine(state, pipelineLayout, spirvShader),
		  descriptorSets(descriptorSets)
	{
		auto it = spirvShader->inputBuiltins.find(spv::BuiltInInstanceIndex);
		if (it != spirvShader->inputBuiltins.end())
		{
			// TODO: we could do better here; we know InstanceIndex is uniform across all lanes
			assert(it->second.SizeInComponents == 1);
			routine.getVariable(it->second.Id)[it->second.FirstComponent] =
					As<Float4>(Int4((*Pointer<Int>(data + OFFSET(DrawData, instanceID)))));
		}

		routine.descriptorSets = data + OFFSET(DrawData, descriptorSets);
		routine.descriptorDynamicOffsets = data + OFFSET(DrawData, descriptorDynamicOffsets);
		routine.pushConstants = data + OFFSET(DrawData, pushConstants);
	}

	VertexProgram::~VertexProgram()
	{
	}

	void VertexProgram::program(UInt &index)
	{
		auto it = spirvShader->inputBuiltins.find(spv::BuiltInVertexIndex);
		if (it != spirvShader->inputBuiltins.end())
		{
			assert(it->second.SizeInComponents == 1);
			routine.getVariable(it->second.Id)[it->second.FirstComponent] =
					As<Float4>(Int4(index) + Int4(0, 1, 2, 3));
		}

		auto activeLaneMask = SIMD::Int(0xFFFFFFFF); // TODO: Control this.
		spirvShader->emit(&routine, activeLaneMask, descriptorSets);

		spirvShader->emitEpilog(&routine);
	}
}
