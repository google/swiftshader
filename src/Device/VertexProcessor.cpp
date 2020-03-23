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

#include "VertexProcessor.hpp"

#include "Pipeline/Constants.hpp"
#include "Pipeline/VertexProgram.hpp"
#include "System/Debug.hpp"
#include "System/Math.hpp"

#include <cstring>

namespace sw {

void VertexCache::clear()
{
	for(uint32_t i = 0; i < SIZE; i++)
	{
		tag[i] = 0xFFFFFFFF;
	}
}

uint32_t VertexProcessor::States::computeHash()
{
	uint32_t *state = reinterpret_cast<uint32_t *>(this);
	uint32_t hash = 0;

	for(unsigned int i = 0; i < sizeof(States) / sizeof(uint32_t); i++)
	{
		hash ^= state[i];
	}

	return hash;
}

bool VertexProcessor::State::operator==(const State &state) const
{
	if(hash != state.hash)
	{
		return false;
	}

	return *static_cast<const States *>(this) == static_cast<const States &>(state);
}

VertexProcessor::VertexProcessor()
{
	routineCache = nullptr;
	setRoutineCacheSize(1024);
}

VertexProcessor::~VertexProcessor()
{
	delete routineCache;
	routineCache = nullptr;
}

void VertexProcessor::setRoutineCacheSize(int cacheSize)
{
	delete routineCache;
	routineCache = new RoutineCacheType(clamp(cacheSize, 1, 65536));
}

const VertexProcessor::State VertexProcessor::update(const sw::Context *context)
{
	State state;

	state.shaderID = context->vertexShader->getSerialID();
	state.robustBufferAccess = context->robustBufferAccess;
	state.isPoint = context->topology == VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

	for(int i = 0; i < MAX_INTERFACE_COMPONENTS / 4; i++)
	{
		state.input[i].format = context->input[i].format;
		// TODO: get rid of attribType -- just keep the VK format all the way through, this fully determines
		// how to handle the attribute.
		state.input[i].attribType = context->vertexShader->inputs[i * 4].Type;
	}

	state.hash = state.computeHash();

	return state;
}

VertexProcessor::RoutineType VertexProcessor::routine(const State &state,
                                                      vk::PipelineLayout const *pipelineLayout,
                                                      SpirvShader const *vertexShader,
                                                      const vk::DescriptorSet::Bindings &descriptorSets)
{
	auto routine = routineCache->query(state);

	if(!routine)  // Create one
	{
		VertexRoutine *generator = new VertexProgram(state, pipelineLayout, vertexShader, descriptorSets);
		generator->generate();
		routine = (*generator)("VertexRoutine_%0.8X", state.shaderID);
		delete generator;

		routineCache->add(state, routine);
	}

	return routine;
}

}  // namespace sw
