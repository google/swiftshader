{
global:
	# EGL core functions
	eglBindAPI;
	eglBindTexImage;
	eglChooseConfig;
	eglClientWaitSync;
	eglCopyBuffers;
	eglCreateContext;
	eglCreateImage;
	eglCreatePbufferFromClientBuffer;
	eglCreatePbufferSurface;
	eglCreatePixmapSurface;
	eglCreatePlatformPixmapSurface;
	eglCreatePlatformWindowSurface;
	eglCreateSync;
	eglCreateWindowSurface;
	eglDestroyContext;
	eglDestroyImage;
	eglDestroySurface;
	eglDestroySync;
	eglGetConfigAttrib;
	eglGetConfigs;
	eglGetCurrentContext;
	eglGetCurrentDisplay;
	eglGetCurrentSurface;
	eglGetDisplay;
	eglGetError;
	eglGetPlatformDisplay;
	eglGetProcAddress;
	eglGetSyncAttrib;
	eglInitialize;
	eglMakeCurrent;
	eglQueryAPI;
	eglQueryContext;
	eglQueryString;
	eglQuerySurface;
	eglReleaseTexImage;
	eglReleaseThread;
	eglSurfaceAttrib;
	eglSwapBuffers;
	eglSwapInterval;
	eglTerminate;
	eglWaitClient;
	eglWaitGL;
	eglWaitNative;
	eglWaitSync;

	# Extensions
	eglCreateImageKHR;
	eglDestroyImageKHR;
	eglGetPlatformDisplayEXT;
	eglCreatePlatformWindowSurfaceEXT;
	eglCreatePlatformPixmapSurfaceEXT;
	eglCreateSyncKHR;
	eglDestroySyncKHR;
	eglClientWaitSyncKHR;
	eglGetSyncAttribKHR;

	# Table of function pointers to disambiguate between libraries
	libEGL_swiftshader;

	# Type-strings and type-infos required by sanitizers
	_ZTS*;
	_ZTI*;

local:
	*;
};
