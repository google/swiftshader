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

#include "Pipeline/VertexProgram.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Math.hpp"
#include "Vulkan/VkDebug.hpp"

#include <string.h>

namespace sw
{
	bool precacheVertex = false;

	void VertexCache::clear()
	{
		for(int i = 0; i < 16; i++)
		{
			tag[i] = 0x80000000;
		}
	}

	unsigned int VertexProcessor::States::computeHash()
	{
		unsigned int *state = (unsigned int*)this;
		unsigned int hash = 0;

		for(unsigned int i = 0; i < sizeof(States) / 4; i++)
		{
			hash ^= state[i];
		}

		return hash;
	}

	VertexProcessor::State::State()
	{
		memset(this, 0, sizeof(State));
	}

	bool VertexProcessor::State::operator==(const State &state) const
	{
		if(hash != state.hash)
		{
			return false;
		}

		return memcmp(static_cast<const States*>(this), static_cast<const States*>(&state), sizeof(States)) == 0;
	}

	VertexProcessor::VertexProcessor(Context *context) : context(context)
	{
		routineCache = nullptr;
		setRoutineCacheSize(1024);
	}

	VertexProcessor::~VertexProcessor()
	{
		delete routineCache;
		routineCache = nullptr;
	}

	void VertexProcessor::setInputStream(int index, const Stream &stream)
	{
		context->input[index] = stream;
	}

	void VertexProcessor::resetInputStreams()
	{
		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			context->input[i].defaults();
		}
	}

	void VertexProcessor::setInstanceID(int instanceID)
	{
		context->instanceID = instanceID;
	}

	void VertexProcessor::setRoutineCacheSize(int cacheSize)
	{
		delete routineCache;
		routineCache = new RoutineCache<State>(clamp(cacheSize, 1, 65536));
	}

	const VertexProcessor::State VertexProcessor::update(VkPrimitiveTopology topology)
	{
		State state;

		state.shaderID = context->vertexShader->getSerialID();

		switch(topology)
		{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
			state.verticesPerPrimitive = 1;
			break;
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			state.verticesPerPrimitive = 2;
			break;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			state.verticesPerPrimitive = 3;
			break;
		default:
			UNIMPLEMENTED("topology %d", int(topology));
		}

		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			state.input[i].type = context->input[i].type;
			state.input[i].count = context->input[i].count;
			state.input[i].normalized = context->input[i].normalized;
			// TODO: get rid of attribType -- just keep the VK format all the way through, this fully determines
			// how to handle the attribute.
			state.input[i].attribType = context->vertexShader->inputs[i*4].Type;
		}

		state.hash = state.computeHash();

		return state;
	}

	Routine *VertexProcessor::routine(const State &state)
	{
		Routine *routine = routineCache->query(state);

		if(!routine)   // Create one
		{
			VertexRoutine *generator = new VertexProgram(state, context->pipelineLayout, context->vertexShader);
			generator->generate();
			routine = (*generator)("VertexRoutine_%0.8X", state.shaderID);
			delete generator;

			routineCache->add(state, routine);
		}

		return routine;
	}
}
