#ifndef libEGL_hpp
#define libEGL_hpp

#define EGLAPI
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#include "Common/SharedLibrary.hpp"

class LibEGLexports
{
public:
	LibEGLexports();

	EGLint (EGLAPIENTRY *eglGetError)(void);
	EGLDisplay (EGLAPIENTRY *eglGetDisplay)(EGLNativeDisplayType display_id);
	EGLBoolean (EGLAPIENTRY *eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor);
	EGLBoolean (EGLAPIENTRY *eglTerminate)(EGLDisplay dpy);
	const char *(EGLAPIENTRY *eglQueryString)(EGLDisplay dpy, EGLint name);
	EGLBoolean (EGLAPIENTRY *eglGetConfigs)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);
	EGLBoolean (EGLAPIENTRY *eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
	EGLBoolean (EGLAPIENTRY *eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
	EGLSurface (EGLAPIENTRY *eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType window, const EGLint *attrib_list);
	EGLSurface (EGLAPIENTRY *eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list);
	EGLSurface (EGLAPIENTRY *eglCreatePixmapSurface)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY *eglDestroySurface)(EGLDisplay dpy, EGLSurface surface);
	EGLBoolean (EGLAPIENTRY *eglQuerySurface)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
	EGLBoolean (EGLAPIENTRY *eglBindAPI)(EGLenum api);
	EGLenum (EGLAPIENTRY *eglQueryAPI)(void);
	EGLBoolean (EGLAPIENTRY *eglWaitClient)(void);
	EGLBoolean (EGLAPIENTRY *eglReleaseThread)(void);
	EGLSurface (EGLAPIENTRY *eglCreatePbufferFromClientBuffer)(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY *eglSurfaceAttrib)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
	EGLBoolean (EGLAPIENTRY *eglBindTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
	EGLBoolean (EGLAPIENTRY *eglReleaseTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
	EGLBoolean (EGLAPIENTRY *eglSwapInterval)(EGLDisplay dpy, EGLint interval);
	EGLContext (EGLAPIENTRY *eglCreateContext)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY *eglDestroyContext)(EGLDisplay dpy, EGLContext ctx);
	EGLBoolean (EGLAPIENTRY *eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
	EGLContext (EGLAPIENTRY *eglGetCurrentContext)(void);
	EGLSurface (EGLAPIENTRY *eglGetCurrentSurface)(EGLint readdraw);
	EGLDisplay (EGLAPIENTRY *eglGetCurrentDisplay)(void);
	EGLBoolean (EGLAPIENTRY *eglQueryContext)(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);
	EGLBoolean (EGLAPIENTRY *eglWaitGL)(void);
	EGLBoolean (EGLAPIENTRY *eglWaitNative)(EGLint engine);
	EGLBoolean (EGLAPIENTRY *eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
	EGLBoolean (EGLAPIENTRY *eglCopyBuffers)(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
	EGLImageKHR (EGLAPIENTRY *eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
	EGLBoolean (EGLAPIENTRY *eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image);
	__eglMustCastToProperFunctionPointerType (EGLAPIENTRY *eglGetProcAddress)(const char*);

	// Functions that don't change the error code, for use by client APIs
	egl::Context *(*clientGetCurrentContext)();
	egl::Display *(*clientGetCurrentDisplay)();
};

class LibEGL
{
public:
	LibEGL()
	{
		libEGL = nullptr;
		libEGLexports = nullptr;
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
		if(!libEGL)
		{
			#if defined(_WIN32)
			const char *libEGL_lib[] = {"libEGL.dll", "libEGL_translator.dll"};
			#elif defined(__ANDROID__)
			const char *libEGL_lib[] = {"/vendor/lib/egl/libEGL_swiftshader.so"};
			#elif defined(__LP64__)
			const char *libEGL_lib[] = {"lib64EGL_translator.so", "libEGL.so.1", "libEGL.so"};
			#else
			const char *libEGL_lib[] = {"libEGL_translator.so", "libEGL.so.1", "libEGL.so"};
			#endif

			libEGL = loadLibrary(libEGL_lib, "libEGL_swiftshader");

			if(libEGL)
			{
				auto libEGL_swiftshader = (LibEGLexports *(*)())getProcAddress(libEGL, "libEGL_swiftshader");
				libEGLexports = libEGL_swiftshader();
			}
		}

		return libEGLexports;
	}

	void *libEGL;
	LibEGLexports *libEGLexports;
};

#endif   // libEGL_hpp
