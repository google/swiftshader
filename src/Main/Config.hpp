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

// Worker thread count when not set by SwiftConfig
// 0 = process affinity count (recommended)
// 1 = rendering on main thread (no worker threads), useful for debugging
#ifndef DEFAULT_THREAD_COUNT
#define DEFAULT_THREAD_COUNT 0
#endif

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

	#if PERF_PROFILE
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
	#endif
};

extern Profiler profiler;

enum
{
	OUTLINE_RESOLUTION = 4096,   // Maximum vertical resolution of the render target
	MIPMAP_LEVELS = 14,
	MAX_COLOR_ATTACHMENTS = 8,
	VERTEX_ATTRIBUTES = 16,
	TEXTURE_IMAGE_UNITS = 16,
	VERTEX_TEXTURE_IMAGE_UNITS = 16,
	TOTAL_IMAGE_UNITS = TEXTURE_IMAGE_UNITS + VERTEX_TEXTURE_IMAGE_UNITS,
	FRAGMENT_UNIFORM_VECTORS = 224,
	VERTEX_UNIFORM_VECTORS = 256,
	MAX_FRAGMENT_UNIFORM_COMPONENTS = FRAGMENT_UNIFORM_VECTORS * 4,
	MAX_VERTEX_UNIFORM_COMPONENTS = VERTEX_UNIFORM_VECTORS * 4,
	MAX_FRAGMENT_UNIFORM_BLOCKS = 12,
	MAX_VERTEX_UNIFORM_BLOCKS = 12,
	MAX_UNIFORM_BLOCK_SIZE = 16384,
	MAX_FRAGMENT_UNIFORM_BLOCKS_COMPONENTS = MAX_FRAGMENT_UNIFORM_BLOCKS * MAX_UNIFORM_BLOCK_SIZE / 4,
	MAX_VERTEX_UNIFORM_BLOCKS_COMPONENTS = MAX_VERTEX_UNIFORM_BLOCKS * MAX_UNIFORM_BLOCK_SIZE / 4,
	MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = MAX_FRAGMENT_UNIFORM_BLOCKS_COMPONENTS + MAX_FRAGMENT_UNIFORM_COMPONENTS,
	MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = MAX_VERTEX_UNIFORM_BLOCKS_COMPONENTS + MAX_VERTEX_UNIFORM_COMPONENTS,
	MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 4,
	MAX_UNIFORM_BUFFER_BINDINGS = MAX_FRAGMENT_UNIFORM_BLOCKS + MAX_VERTEX_UNIFORM_BLOCKS, // Limited to 127 by SourceParameter.bufferIndex in Shader.hpp
	MAX_CLIP_PLANES = 6,
	RENDERTARGETS = 4,
};

#endif   // sw_Config_hpp
