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

#include "SetupProcessor.hpp"

#include "Context.hpp"
#include "Polygon.hpp"
#include "Primitive.hpp"
#include "Renderer.hpp"
#include "Pipeline/Constants.hpp"
#include "Pipeline/SetupRoutine.hpp"
#include "Pipeline/SpirvShader.hpp"
#include "System/Debug.hpp"

#include <cstring>

namespace sw {

uint32_t SetupProcessor::States::computeHash()
{
	uint32_t *state = reinterpret_cast<uint32_t *>(this);
	uint32_t hash = 0;

	for(unsigned int i = 0; i < sizeof(States) / sizeof(uint32_t); i++)
	{
		hash ^= state[i];
	}

	return hash;
}

bool SetupProcessor::State::operator==(const State &state) const
{
	if(hash != state.hash)
	{
		return false;
	}

	return *static_cast<const States *>(this) == static_cast<const States &>(state);
}

SetupProcessor::SetupProcessor()
{
	routineCache = nullptr;
	setRoutineCacheSize(1024);
}

SetupProcessor::~SetupProcessor()
{
	delete routineCache;
	routineCache = nullptr;
}

SetupProcessor::State SetupProcessor::update(const sw::Context *context) const
{
	State state;

	bool vPosZW = (context->pixelShader && context->pixelShader->hasBuiltinInput(spv::BuiltInFragCoord));

	state.isDrawPoint = context->isDrawPoint(true);
	state.isDrawLine = context->isDrawLine(true);
	state.isDrawTriangle = context->isDrawTriangle(true);
	state.applySlopeDepthBias = context->isDrawTriangle(false) && (context->slopeDepthBias != 0.0f);
	state.interpolateZ = context->depthBufferActive() || vPosZW;
	state.interpolateW = context->pixelShader != nullptr;
	state.frontFace = context->frontFace;
	state.cullMode = context->cullMode;

	state.multiSampleCount = context->sampleCount;
	state.enableMultiSampling = (state.multiSampleCount > 1) &&
	                            !(context->isDrawLine(true) && (context->lineRasterizationMode == VK_LINE_RASTERIZATION_MODE_BRESENHAM_EXT));
	state.rasterizerDiscard = context->rasterizerDiscard;

	state.numClipDistances = context->vertexShader->getNumOutputClipDistances();
	state.numCullDistances = context->vertexShader->getNumOutputCullDistances();

	if(context->pixelShader)
	{
		for(int interpolant = 0; interpolant < MAX_INTERFACE_COMPONENTS; interpolant++)
		{
			state.gradient[interpolant] = context->pixelShader->inputs[interpolant];
		}
	}

	state.hash = state.computeHash();

	return state;
}

SetupProcessor::RoutineType SetupProcessor::routine(const State &state)
{
	auto routine = routineCache->query(state);

	if(!routine)
	{
		SetupRoutine *generator = new SetupRoutine(state);
		generator->generate();
		routine = generator->getRoutine();
		delete generator;

		routineCache->add(state, routine);
	}

	return routine;
}

void SetupProcessor::setRoutineCacheSize(int cacheSize)
{
	delete routineCache;
	routineCache = new RoutineCacheType(clamp(cacheSize, 1, 65536));
}

}  // namespace sw
