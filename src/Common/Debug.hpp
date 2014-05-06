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

#ifndef Debug_hpp
#define Debug_hpp

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <guiddef.h>

#undef min
#undef max

void trace(const char *format, ...);

#ifndef NDEBUG
	#define TRACE(format, ...) trace("[0x%0.8X]%s("format")\n", this, __FUNCTION__, __VA_ARGS__)
#else
	#define TRACE(...) ((void)0)
#endif

#ifndef NDEBUG
	#define UNIMPLEMENTED() {trace("\t! Unimplemented: %s(%d)\n", __FUNCTION__, __LINE__); ASSERT(false);}
#else
	#define UNIMPLEMENTED() ((void)0)
#endif

#ifndef NDEBUG
	#define NOINTERFACE(iid) _NOINTERFACE(__FUNCTION__, iid)

	inline long _NOINTERFACE(const char *function, const IID &iid)
	{
		trace("\t! No interface {0x%0.8X, 0x%0.4X, 0x%0.4X, 0x%0.2X, 0x%0.2X, 0x%0.2X, 0x%0.2X, 0x%0.2X, 0x%0.2X, 0x%0.2X, 0x%0.2X} for %s\n", iid.Data1, iid.Data2, iid.Data3, iid.Data4[0], iid.Data4[1], iid.Data4[2], iid.Data4[3], iid.Data4[4], iid.Data4[5], iid.Data4[6], iid.Data4[7], function);

		return  E_NOINTERFACE;
	}
#else
	#define NOINTERFACE() E_NOINTERFACE
#endif

#ifndef NDEBUG
	#define ASSERT(expression) {if(!(expression)) trace("\t! Assert failed in %s(%d): "#expression"\n", __FUNCTION__, __LINE__); assert(expression);}
#else
	#define ASSERT assert
#endif

#endif   // Debug_hpp
