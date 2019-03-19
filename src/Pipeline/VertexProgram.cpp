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

namespace sw
{
	VertexProgram::VertexProgram(
			const VertexProcessor::State &state,
			vk::PipelineLayout const *pipelineLayout,
			SpirvShader const *spirvShader)
		: VertexRoutine(state, pipelineLayout, spirvShader)
	{
		ifDepth = 0;
		loopRepDepth = 0;
		currentLabel = -1;
		whileTest = false;

		enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

		auto it = spirvShader->inputBuiltins.find(spv::BuiltInInstanceIndex);
		if (it != spirvShader->inputBuiltins.end())
		{
			// TODO: we could do better here; we know InstanceIndex is uniform across all lanes
			assert(it->second.SizeInComponents == 1);
			routine.getValue(it->second.Id)[it->second.FirstComponent] =
					As<Float4>(Int4((*Pointer<Int>(data + OFFSET(DrawData, instanceID)))));
		}

		routine.pushConstants = data + OFFSET(DrawData, pushConstants);
	}

	VertexProgram::~VertexProgram()
	{
	}

	void VertexProgram::program(UInt &index)
	{
		//	shader->print("VertexShader-%0.8X.txt", state.shaderID);

		enableIndex = 0;

		auto it = spirvShader->inputBuiltins.find(spv::BuiltInVertexIndex);
		if (it != spirvShader->inputBuiltins.end())
		{
			assert(it->second.SizeInComponents == 1);
			routine.getValue(it->second.Id)[it->second.FirstComponent] =
					As<Float4>(Int4(index) + Int4(0, 1, 2, 3));
		}

		spirvShader->emit(&routine);

		if(currentLabel != -1)
		{
			Nucleus::setInsertBlock(returnBlock);
		}

		spirvShader->emitEpilog(&routine);
	}

	RValue<Pointer<Byte>> VertexProgram::uniformAddress(int bufferIndex, unsigned int index)
	{
		if(bufferIndex == -1)
		{
			return data + OFFSET(DrawData, vs.c[index]);
		}
		else
		{
			return *Pointer<Pointer<Byte>>(data + OFFSET(DrawData, vs.u[bufferIndex])) + index;
		}
	}

	RValue<Pointer<Byte>> VertexProgram::uniformAddress(int bufferIndex, unsigned int index, Int &offset)
	{
		return uniformAddress(bufferIndex, index) + offset * sizeof(float4);
	}

	Int4 VertexProgram::enableMask()
	{
		Int4 enable = true ? Int4(enableStack[enableIndex]) : Int4(0xFFFFFFFF);
		return enable;
	}

}
