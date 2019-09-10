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

#ifndef libEGL_hpp
#define libEGL_hpp

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "Common/SharedLibrary.hpp"

class LibEGLexports
{
public:
	LibEGLexports();

	EGLint (EGLAPIENTRY* eglGetError)(void);
	EGLDisplay (EGLAPIENTRY* eglGetDisplay)(EGLNativeDisplayType display_id);
	EGLBoolean (EGLAPIENTRY* eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor);
	EGLBoolean (EGLAPIENTRY* eglTerminate)(EGLDisplay dpy);
	const char *(EGLAPIENTRY* eglQueryString)(EGLDisplay dpy, EGLint name);
	EGLBoolean (EGLAPIENTRY* eglGetConfigs)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);
	EGLBoolean (EGLAPIENTRY* eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
	EGLBoolean (EGLAPIENTRY* eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
	EGLSurface (EGLAPIENTRY* eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType window, const EGLint *attrib_list);
	EGLSurface (EGLAPIENTRY* eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list);
	EGLSurface (EGLAPIENTRY* eglCreatePixmapSurface)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY* eglDestroySurface)(EGLDisplay dpy, EGLSurface surface);
	EGLBoolean (EGLAPIENTRY* eglQuerySurface)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
	EGLBoolean (EGLAPIENTRY* eglBindAPI)(EGLenum api);
	EGLenum (EGLAPIENTRY* eglQueryAPI)(void);
	EGLBoolean (EGLAPIENTRY* eglWaitClient)(void);
	EGLBoolean (EGLAPIENTRY* eglReleaseThread)(void);
	EGLSurface (EGLAPIENTRY* eglCreatePbufferFromClientBuffer)(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY* eglSurfaceAttrib)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
	EGLBoolean (EGLAPIENTRY* eglBindTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
	EGLBoolean (EGLAPIENTRY* eglReleaseTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
	EGLBoolean (EGLAPIENTRY* eglSwapInterval)(EGLDisplay dpy, EGLint interval);
	EGLContext (EGLAPIENTRY* eglCreateContext)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY* eglDestroyContext)(EGLDisplay dpy, EGLContext ctx);
	EGLBoolean (EGLAPIENTRY* eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
	EGLContext (EGLAPIENTRY* eglGetCurrentContext)(void);
	EGLSurface (EGLAPIENTRY* eglGetCurrentSurface)(EGLint readdraw);
	EGLDisplay (EGLAPIENTRY* eglGetCurrentDisplay)(void);
	EGLBoolean (EGLAPIENTRY* eglQueryContext)(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);
	EGLBoolean (EGLAPIENTRY* eglWaitGL)(void);
	EGLBoolean (EGLAPIENTRY* eglWaitNative)(EGLint engine);
	EGLBoolean (EGLAPIENTRY* eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
	EGLBoolean (EGLAPIENTRY* eglCopyBuffers)(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
	EGLImageKHR (EGLAPIENTRY* eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY* eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image);
	__eglMustCastToProperFunctionPointerType (EGLAPIENTRY* eglGetProcAddress)(const char*);
	EGLSyncKHR (EGLAPIENTRY* eglCreateSyncKHR)(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY* eglDestroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
	EGLint (EGLAPIENTRY* eglClientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
	EGLBoolean (EGLAPIENTRY* eglGetSyncAttribKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);

	// Functions that don't change the error code, for use by client APIs
	egl::Context *(*clientGetCurrentContext)();
};

class LibEGL
{
public:
	LibEGL()
	{
	}

	~LibEGL()
	{
		freeLibrary(libEGL);
	}

	LibEGLexports *operator->()
	{
		return loadExports();
	}

private:
	LibEGLexports *loadExports()
	{
		if(!loadLibraryAttempted && !libEGL)
		{
			#if defined(_WIN32)
				#if defined(__LP64__)
					const char *libEGL_lib[] = {"libswiftshader_libEGL.dll", "libEGL.dll", "lib64EGL_translator.dll"};
				#else
					const char *libEGL_lib[] = {"libswiftshader_libEGL.dll", "libEGL.dll", "libEGL_translator.dll"};
				#endif
			#elif defined(__ANDROID__)
				const char *libEGL_lib[] = {"libEGL_swiftshader.so", "libEGL_swiftshader.so"};
			#elif defined(__linux__)
				#if defined(__LP64__)
					const char *libEGL_lib[] = {"lib64EGL_translator.so", "libEGL.so.1", "libEGL.so"};
				#else
					const char *libEGL_lib[] = {"libEGL_translator.so", "libEGL.so.1", "libEGL.so"};
				#endif
			#elif defined(__APPLE__)
				#if defined(__LP64__)
					const char *libEGL_lib[] = {"libswiftshader_libEGL.dylib", "lib64EGL_translator.dylib", "libEGL.so", "libEGL.dylib"};
				#else
					const char *libEGL_lib[] = {"libswiftshader_libEGL.dylib", "libEGL_translator.dylib", "libEGL.so", "libEGL.dylib"};
				#endif
			#elif defined(__Fuchsia__)
				const char *libEGL_lib[] = {"libswiftshader_libEGL.so", "libEGL.so"};
			#else
				#error "libEGL::loadExports unimplemented for this platform"
			#endif

			std::string directory = getModuleDirectory();
			libEGL = loadLibrary(directory, libEGL_lib, "libEGL_swiftshader");

			if(libEGL)
			{
				auto libEGL_swiftshader = (LibEGLexports *(*)())getProcAddress(libEGL, "libEGL_swiftshader");
				libEGLexports = libEGL_swiftshader();
			}

			loadLibraryAttempted = true;
		}

		return libEGLexports;
	}

	void *libEGL = nullptr;
	LibEGLexports *libEGLexports = nullptr;
	bool loadLibraryAttempted = false;
};

#endif   // libEGL_hpp
