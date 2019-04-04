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

#ifndef Debug_hpp
#define Debug_hpp

#if defined(__ANDROID__) && !defined(ANDROID_HOST_BUILD) && !defined(ANDROID_NDK_BUILD)
#include "DebugAndroid.hpp"
#else

#include <assert.h>
#include <stdio.h>

#undef min
#undef max

namespace sw
{
void trace(const char *format, ...);
inline void trace() {}
}

#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
	#define TRACE(format, ...) sw::trace("[0x%0.8X]%s(" format ")\n", this, __FUNCTION__, ##__VA_ARGS__)
#else
	#define TRACE(...) ((void)0)
#endif

#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
	#define UNIMPLEMENTED(...) do { \
		sw::trace("\t! Unimplemented: %s(%d): ", __FUNCTION__, __LINE__); \
		sw::trace(__VA_ARGS__); \
		sw::trace("\n"); \
		ASSERT(false); \
	} while(0)
#else
	#define UNIMPLEMENTED(...) ((void)0)
#endif

#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
	#define ASSERT(expression) {if(!(expression)) sw::trace("\t! Assert failed in %s(%d): " #expression "\n", __FUNCTION__, __LINE__); assert(expression);}
#else
	#define ASSERT assert
#endif

#endif   // !__ANDROID__
#endif   // Debug_hpp
