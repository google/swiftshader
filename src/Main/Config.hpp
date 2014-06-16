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

#ifndef sw_Config_hpp
#define sw_Config_hpp

#include "Common/Types.hpp"

#define PERF_HUD 0       // Display time spent on vertex, setup and pixel processing for each thread
#define PERF_PROFILE 0   // Profile various pipeline stages and display the timing in SwiftConfig
#if defined(_WIN32)
#define S3TC_SUPPORT 1
#else
#define S3TC_SUPPORT 0
#endif

#if PERF_PROFILE
enum
{
	PERF_PIXEL,
	PERF_PIPE,
	PERF_INTERP,
	PERF_SHADER,
	PERF_TEX,
	PERF_ROP,

	PERF_TIMERS
};

struct Profiler
{
	Profiler();

	void reset();
	void nextFrame();

	int framesSec;
	int framesTotal;
	double FPS;
	double cycles[PERF_TIMERS];

	int64_t ropOperations;
	int64_t ropOperationsTotal;
	int64_t ropOperationsFrame;

	int64_t texOperations;
	int64_t texOperationsTotal;
	int64_t texOperationsFrame;

	int64_t compressedTex;
	int64_t compressedTexTotal;
	int64_t compressedTexFrame;
};

extern Profiler profiler;
#endif

enum
{
	OUTLINE_RESOLUTION = 4096,   // Maximum vertical resolution of the render target
	MIPMAP_LEVELS = 14
};

#endif   // sw_Config_hpp
