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

#include "Config.hpp"

#include "System/Timer.hpp"

namespace sw {

Profiler profiler;

Profiler::Profiler()
{
	reset();
}

void Profiler::reset()
{
	framesSec = 0;
	framesTotal = 0;
	FPS = 0;
}

void Profiler::nextFrame()
{
	static double fpsTime = sw::Timer::seconds();

	double time = sw::Timer::seconds();
	double delta = time - fpsTime;
	framesSec++;

	if(delta > 1.0)
	{
		FPS = framesSec / delta;

		fpsTime = time;
		framesTotal += framesSec;
		framesSec = 0;
	}
}

}  // namespace sw