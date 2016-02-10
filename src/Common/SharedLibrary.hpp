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

#ifndef SharedLibrary_hpp
#define SharedLibrary_hpp

#if defined(_WIN32)
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

void *getLibraryHandle(const char *path);
void *loadLibrary(const char *path);
void freeLibrary(void *library);
void *getProcAddress(void *library, const char *name);

template<int n>
void *loadLibrary(const char *(&names)[n], const char *mustContainSymbol = nullptr)
{
	for(int i = 0; i < n; i++)
	{
		void *library = getLibraryHandle(names[i]);

		if(library)
		{
			if(!mustContainSymbol || getProcAddress(library, mustContainSymbol))
			{
				return library;
			}

			freeLibrary(library);
		}
	}

	for(int i = 0; i < n; i++)
	{
		void *library = loadLibrary(names[i]);

		if(library)
		{
			if(!mustContainSymbol || getProcAddress(library, mustContainSymbol))
			{
				return library;
			}

			freeLibrary(library);
		}
	}

	return 0;
}

#if defined(_WIN32)
	inline void *loadLibrary(const char *path)
	{
		return (void*)LoadLibrary(path);
	}

	inline void *getLibraryHandle(const char *path)
	{
		HMODULE module = 0;
		GetModuleHandleEx(0, path, &module);
		return (void*)module;
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
		return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
	}

	inline void *getLibraryHandle(const char *path)
	{
		#ifdef __ANDROID__
			// bionic doesn't support RTLD_NOLOAD before L
			return dlopen(path, RTLD_NOW | RTLD_LOCAL);
		#else
			void *resident = dlopen(path, RTLD_LAZY | RTLD_NOLOAD | RTLD_LOCAL);

			if(resident)
			{
				return dlopen(path, RTLD_LAZY | RTLD_LOCAL);   // Increment reference count
			}

			return 0;
		#endif
	}

    inline void freeLibrary(void *library)
    {
        if(library)
        {
            dlclose(library);
        }
    }

	inline void *getProcAddress(void *library, const char *name)
	{
		void *symbol = dlsym(library, name);

		if(!symbol)
		{
			const char *reason = dlerror();   // Silence the error
			(void)reason;
		}

		return symbol;
    }
#endif

#endif   // SharedLibrary_hpp
