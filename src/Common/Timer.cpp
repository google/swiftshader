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

#include "Timer.hpp"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <intrin.h>

namespace sw
{
	Timer::Timer()
	{
	}

	Timer::~Timer()
	{
	}

	double Timer::seconds()
	{
		return (double)counter() / (double)frequency();
	}

	int64_t Timer::ticks()
	{
		return __rdtsc();
	}

	int64_t Timer::counter()
	{
		int64_t counter;

		QueryPerformanceCounter((LARGE_INTEGER*)&counter);

		return counter;
	}

	int64_t Timer::frequency()
	{
		int64_t frequency;

		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

		return frequency;
	}
}
