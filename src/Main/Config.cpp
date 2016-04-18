// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Config.hpp"

#include "Thread.hpp"
#include "Timer.hpp"

namespace sw
{
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

		#if PERF_PROFILE
			for(int i = 0; i < PERF_TIMERS; i++)
			{
				cycles[i] = 0;
			}

			ropOperations = 0;
			ropOperationsTotal = 0;
			ropOperationsFrame = 0;

			texOperations = 0;
			texOperationsTotal = 0;
			texOperationsFrame = 0;

			compressedTex = 0;
			compressedTexTotal = 0;
			compressedTexFrame = 0;
		#endif
	};

	void Profiler::nextFrame()
	{
		#if PERF_PROFILE
			ropOperationsFrame = sw::atomicExchange(&ropOperations, 0);
			texOperationsFrame = sw::atomicExchange(&texOperations, 0);
			compressedTexFrame = sw::atomicExchange(&compressedTex, 0);

			ropOperationsTotal += ropOperationsFrame;
			texOperationsTotal += texOperationsFrame;
			compressedTexTotal += compressedTexFrame;
		#endif

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
}