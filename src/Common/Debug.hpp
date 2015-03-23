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

#ifndef Debug_hpp
#define Debug_hpp

#ifdef __ANDROID__
#include <cutils/log.h>
#endif

#include <assert.h>
#include <stdio.h>

#undef min
#undef max

void trace(const char *format, ...);

#ifndef NDEBUG
	#define TRACE(format, ...) trace("[0x%0.8X]%s(" format ")\n", this, __FUNCTION__, ##__VA_ARGS__)
#else
	#define TRACE(...) ((void)0)
#endif

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
	#ifndef NDEBUG
		#define UNIMPLEMENTED() {trace("\t! Unimplemented: %s(%d)\n", __FUNCTION__, __LINE__); ASSERT(false);}
	#else
		#define UNIMPLEMENTED() ((void)0)
	#endif
#endif

#ifndef NDEBUG
	#define ASSERT(expression) {if(!(expression)) trace("\t! Assert failed in %s(%d): " #expression "\n", __FUNCTION__, __LINE__); assert(expression);}
#else
	#define ASSERT assert
#endif

#endif   // Debug_hpp
