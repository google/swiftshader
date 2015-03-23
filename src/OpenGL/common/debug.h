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

// debug.h: Debugging utilities.

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#ifdef __ANDROID__
#include <cutils/log.h>
#endif

#include <stdio.h>
#include <assert.h>

#if !defined(TRACE_OUTPUT_FILE)
#define TRACE_OUTPUT_FILE "debug.txt"
#endif

namespace es
{
    // Outputs text to the debugging log
    void trace(const char *format, ...);
}

// A macro to output a trace of a function call and its arguments to the debugging log
#if defined(ANGLE_DISABLE_TRACE)
#define TRACE(message, ...) (void(0))
#else
#define TRACE(message, ...) es::trace("trace: %s(%d): " message "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

// A macro to output a function call and its arguments to the debugging log, to denote an item in need of fixing.
#if defined(ANGLE_DISABLE_TRACE)
#define FIXME(message, ...) (void(0))
#else
#define FIXME(message, ...) do {es::trace("fixme: %s(%d): " message "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); assert(false);} while(false)
#endif

// A macro to output a function call and its arguments to the debugging log, in case of error.
#if defined(ANGLE_DISABLE_TRACE)
#define ERR(message, ...) (void(0))
#else
#define ERR(message, ...) do {es::trace("err: %s(%d): " message "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); assert(false);} while(false)
#endif

// A macro asserting a condition and outputting failures to the debug log
#if !defined(NDEBUG)
#define ASSERT(expression) do { \
    if(!(expression)) \
        ERR("\t! Assert failed in %s(%d): "#expression"\n", __FUNCTION__, __LINE__); \
        assert(expression); \
    } while(0)
#else
#define ASSERT(expression) (void(0))
#endif

// A macro to indicate unimplemented functionality
#ifdef __ANDROID__
	// On Android Virtual Devices we heavily depend on logging, even in
	// production builds. We do this because AVDs are components of larger
	// systems, and may be configured in ways that are difficult to
	// reproduce locally. For example some system run tests against
	// third-party code that we cannot access.  Aborting (cf. assert) on
	// unimplemented functionality creates two problems. First, it produces
	// a service failure where none is needed. Second, it puts the
	// customer on the critical path for notifying us of a problem.
	// The alternative, skipping unimplemented functionality silently, is
	// arguably worse: neither the service provider nor the customer will
	// learn that unimplemented functionality may have compromised the test
	// results.
	// Logging invocations of unimplemented functionality is useful to both
	// service provider and the customer. The service provider can learn
	// that the functionality is needed. The customer learns that the test
	// results may be compromised.
	#define UNIMPLEMENTED() {ALOGE("Unimplemented: %s %s:%d", __FUNCTION__, __FILE__, __LINE__); }
#else
	#if !defined(NDEBUG)
		#define UNIMPLEMENTED() do { \
			FIXME("\t! Unimplemented: %s(%d)\n", __FUNCTION__, __LINE__); \
			assert(false); \
		} while(0)
	#else
	    #define UNIMPLEMENTED() FIXME("\t! Unimplemented: %s(%d)\n", __FUNCTION__, __LINE__)
	#endif
#endif

// A macro for code which is not expected to be reached under valid assumptions

#if !defined(NDEBUG)
#define UNREACHABLE() do { \
    ERR("\t! Unreachable reached: %s(%d)\n", __FUNCTION__, __LINE__); \
    assert(false); \
    } while(0)
#else
    #define UNREACHABLE() ERR("\t! Unreachable reached: %s(%d)\n", __FUNCTION__, __LINE__)
#endif

// A macro functioning as a compile-time assert to validate constant conditions
#define META_ASSERT(condition) typedef int COMPILE_TIME_ASSERT_##__LINE__[static_cast<bool>(condition) ? 1 : -1]

#endif   // COMMON_DEBUG_H_
