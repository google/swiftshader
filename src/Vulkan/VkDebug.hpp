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

// debug.h: Debugging utilities.

#ifndef VK_DEBUG_H_
#define VK_DEBUG_H_

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#if !defined(TRACE_OUTPUT_FILE)
#define TRACE_OUTPUT_FILE "debug.txt"
#endif

#if defined(__GNUC__) || defined(__clang__)
#define CHECK_PRINTF_ARGS __attribute__((format(printf, 1, 2)))
#else
#define CHECK_PRINTF_ARGS
#endif

namespace vk
{
	// Outputs text to the debugging log
	void trace(const char *format, ...) CHECK_PRINTF_ARGS;
	inline void trace() {}

	// Outputs text to the debugging log and prints to stderr.
	void warn(const char *format, ...) CHECK_PRINTF_ARGS;
	inline void warn() {}

	// Outputs the message to the debugging log and stderr, and calls abort().
	void abort(const char *format, ...) CHECK_PRINTF_ARGS;
}

// A macro to output a trace of a function call and its arguments to the
// debugging log. Disabled if SWIFTSHADER_DISABLE_TRACE is defined.
#if defined(SWIFTSHADER_DISABLE_TRACE)
#define TRACE(message, ...) (void(0))
#else
#define TRACE(message, ...) vk::trace("%s:%d TRACE: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// A macro to print a warning message to the debugging log and stderr to denote
// an issue that needs fixing.
#define FIXME(message, ...) vk::warn("%s:%d FIXME: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__);

// A macro to print a warning message to the debugging log and stderr.
#define WARN(message, ...) vk::warn("%s:%d WARNING: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__);

// A macro that prints the message to the debugging log and stderr and
// immediately aborts execution of the application.
//
// Note: This will terminate the application regardless of build flags!
//       Use with extreme caution!
#undef ABORT
#define ABORT(message, ...) vk::abort("%s:%d ABORT: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// A macro that delegates to:
//   ABORT() in debug builds (!NDEBUG || DCHECK_ALWAYS_ON)
// or
//   WARN() in release builds (NDEBUG && !DCHECK_ALWAYS_ON)
#undef DABORT
#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
#define DABORT(message, ...) ABORT(message, ##__VA_ARGS__)
#else
#define DABORT(message, ...) WARN(message, ##__VA_ARGS__)
#endif

// A macro asserting a condition.
// If the condition fails, the condition and message is passed to DABORT().
#undef ASSERT_MSG
#define ASSERT_MSG(expression, format, ...) do { \
	if(!(expression)) { \
		DABORT("ASSERT(%s): " format "\n", #expression, ##__VA_ARGS__); \
	} } while(0)

// A macro asserting a condition.
// If the condition fails, the condition is passed to DABORT().
#undef ASSERT
#define ASSERT(expression) do { \
	if(!(expression)) { \
		DABORT("ASSERT(%s)\n", #expression); \
	} } while(0)

// A macro to indicate functionality currently unimplemented for a feature
// advertised as supported. For unsupported features not advertised as supported
// use UNSUPPORTED().
#undef UNIMPLEMENTED
#define UNIMPLEMENTED(format, ...) DABORT("UNIMPLEMENTED: " format, ##__VA_ARGS__)

// A macro to indicate unsupported functionality.
// This should be called when a Vulkan / SPIR-V feature is attempted to be used,
// but is not currently implemented by SwiftShader.
// Note that in a well-behaved application these should not be reached as the
// application should be respecting the advertised features / limits.
#undef UNSUPPORTED
#define UNSUPPORTED(format, ...) DABORT("UNSUPPORTED: " format, ##__VA_ARGS__)

// A macro for code which should never be reached, even with misbehaving
// applications.
#undef UNREACHABLE
#define UNREACHABLE(format, ...) DABORT("UNREACHABLE: " format, ##__VA_ARGS__)

// A macro asserting a condition and performing a return.
#undef ASSERT_OR_RETURN
#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
#define ASSERT_OR_RETURN(expression) ASSERT(expression)
#else
#define ASSERT_OR_RETURN(expression) do { \
	if(!(expression)) { \
		return; \
	} } while(0)
#endif

#endif   // VK_DEBUG_H_
