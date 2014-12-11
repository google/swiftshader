// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Timer.hpp"

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <x86intrin.h>
	#include <features.h>
#endif

#include <assert.h>

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
		#if defined(_WIN32)
			return (double)counter() / (double)frequency();
		#else
			timeval t;
			gettimeofday(&t, 0);
			return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
		#endif
	}

	int64_t Timer::ticks()
	{
		#if defined(_WIN32)
			return __rdtsc();
		#else
			int64_t tsc;
			__asm volatile("rdtsc": "=A" (tsc));
			return tsc;
		#endif
	}

	int64_t Timer::counter()
	{
		#if defined(_WIN32)
			int64_t counter;
			QueryPerformanceCounter((LARGE_INTEGER*)&counter);
			return counter;
		#else
			timeval t;
			gettimeofday(&t, 0);
			return t.tv_sec * 1000000 + t.tv_usec;
		#endif
	}

	int64_t Timer::frequency()
	{
		#if defined(_WIN32)
			int64_t frequency;
			QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
			return frequency;
		#else
			return 1000000;   // gettimeofday uses microsecond resolution
		#endif
	}
}
