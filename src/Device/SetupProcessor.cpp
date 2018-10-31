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

#include "Primitive.hpp"
#include "Polygon.hpp"
#include "Context.hpp"
#include "Renderer.hpp"
#include "Pipeline/SetupRoutine.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Debug.hpp"

namespace sw
{
	extern bool complementaryDepthBuffer;
	extern bool fullPixelPositionRegister;

	bool precacheSetup = false;

	unsigned int SetupProcessor::States::computeHash()
	{
		unsigned int *state = (unsigned int*)this;
		unsigned int hash = 0;

		for(unsigned int i = 0; i < sizeof(States) / 4; i++)
		{
			hash ^= state[i];
		}

		return hash;
	}

	SetupProcessor::State::State(int i)
	{
		memset(this, 0, sizeof(State));
	}

	bool SetupProcessor::State::operator==(const State &state) const
	{
		if(hash != state.hash)
		{
			return false;
		}

		return memcmp(static_cast<const States*>(this), static_cast<const States*>(&state), sizeof(States)) == 0;
	}

	SetupProcessor::SetupProcessor(Context *context) : context(context)
	{
		routineCache = nullptr;
		setRoutineCacheSize(1024);
	}

	SetupProcessor::~SetupProcessor()
	{
		delete routineCache;
		routineCache = nullptr;
	}

	SetupProcessor::State SetupProcessor::update() const
	{
		State state;

		bool vPosZW = (context->pixelShader && context->pixelShader->isVPosDeclared() && fullPixelPositionRegister);

		state.isDrawPoint = context->isDrawPoint();
		state.isDrawLine = context->isDrawLine();
		state.isDrawTriangle = context->isDrawTriangle();
		state.interpolateZ = context->depthBufferActive() || vPosZW;
		state.interpolateW = context->perspectiveActive() || vPosZW;
		state.perspective = context->perspectiveActive();
		state.cullMode = context->cullMode;
		state.twoSidedStencil = context->stencilActive() && context->twoSidedStencil;
		state.slopeDepthBias = context->slopeDepthBias != 0.0f;
		state.vFace = context->pixelShader && context->pixelShader->isVFaceDeclared();

		state.positionRegister = Pos;
		state.pointSizeRegister = Unused;

		state.multiSample = context->getMultiSampleCount();
		state.rasterizerDiscard = context->rasterizerDiscard;

		state.positionRegister = context->vertexShader->getPositionRegister();
		state.pointSizeRegister = context->vertexShader->getPointSizeRegister();

		for(int interpolant = 0; interpolant < MAX_FRAGMENT_INPUTS; interpolant++)
		{
			for(int component = 0; component < 4; component++)
			{
				state.gradient[interpolant][component].attribute = Unused;
				state.gradient[interpolant][component].flat = false;
				state.gradient[interpolant][component].wrap = false;
			}
		}

		const bool point = context->isDrawPoint();

		for(int interpolant = 0; interpolant < MAX_FRAGMENT_INPUTS; interpolant++)
		{
			for(int component = 0; component < 4; component++)
			{
				const Shader::Semantic& semantic = context->pixelShader->getInput(interpolant, component);

				if(semantic.active())
				{
					int input = interpolant;
					for(int i = 0; i < MAX_VERTEX_OUTPUTS; i++)
					{
						if(semantic == context->vertexShader->getOutput(i, component))
						{
							input = i;
							break;
						}
					}

					bool flat = point;

					switch(semantic.usage)
					{
					case Shader::USAGE_TEXCOORD: flat = false;                  break;
					case Shader::USAGE_COLOR:    flat = semantic.flat || point; break;
					}

					state.gradient[interpolant][component].attribute = input;
					state.gradient[interpolant][component].flat = flat;
				}
			}
		}

		state.hash = state.computeHash();

		return state;
	}

	Routine *SetupProcessor::routine(const State &state)
	{
		Routine *routine = routineCache->query(state);

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
		routineCache = new RoutineCache<State>(clamp(cacheSize, 1, 65536), precacheSetup ? "sw-setup" : 0);
	}
}
