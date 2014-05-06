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

#if defined(_WIN32)
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

#if defined(_WIN32)
	inline void *loadLibrary(const char *path)
	{
		return (void*)LoadLibrary(path);
	}

	inline void freeLibrary(void *library)
	{
		FreeLibrary((HMODULE)library);
	}

	inline void *getProcAddress(void *library, const char *name)
	{
		return (void*)GetProcAddress((HMODULE)library, name);
	}
#else
	inline void *loadLibrary(const char *path)
	{
		return dlopen(path, RTLD_LAZY);
	}

	inline void freeLibrary(void *library)
	{
		dlclose(library);
	}

	inline void *getProcAddress(void *library, const char *name)
	{
		return dlsym(library, name);
	}
#endif
