#ifndef libGLES_CM_hpp
#define libGLES_CM_hpp

#define GL_API
#include <GLES/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>
#define EGLAPI
#include <EGL/egl.h>

#include "Common/SharedLibrary.hpp"

namespace sw
{
class FrameBuffer;
enum Format : unsigned char;
}

namespace egl
{
class Context;
class Image;
class Config;
}

class LibGLES_CMexports
{
public:
	LibGLES_CMexports();

	egl::Context *(*es1CreateContext)(const egl::Config *config, const egl::Context *shareContext);
	__eglMustCastToProperFunctionPointerType (*es1GetProcAddress)(const char *procname);
	egl::Image *(*createBackBuffer)(int width, int height, const egl::Config *config);
	egl::Image *(*createDepthStencil)(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard);
	sw::FrameBuffer *(*createFrameBuffer)(EGLNativeDisplayType display, EGLNativeWindowType window, int width, int height);

	void (GL_APIENTRY *glEGLImageTargetTexture2DOES)(GLenum target, GLeglImageOES image);
};

class LibGLES_CM
{
public:
	LibGLES_CM()
	{
		libGLES_CM = nullptr;
		libGLES_CMexports = nullptr;
	}

	~LibGLES_CM()
	{
		freeLibrary(libGLES_CM);
	}

	operator bool()
	{
		return loadExports();
	}

	LibGLES_CMexports *operator->()
	{
		return loadExports();
	}

private:
	LibGLES_CMexports *loadExports()
	{
		if(!libGLES_CM)
		{
			#if defined(_WIN32)
			const char *libGLES_CM_lib[] = {"libGLES_CM.dll", "libGLES_CM_translator.dll"};
			#elif defined(__ANDROID__)
			const char *libGLES_CM_lib[] = {"/vendor/lib/egl/libGLESv1_CM_swiftshader.so"};
			#elif defined(__LP64__)
			const char *libGLES_CM_lib[] = {"lib64GLES_CM_translator.so", "libGLES_CM.so.1", "libGLES_CM.so"};
			#else
			const char *libGLES_CM_lib[] = {"libGLES_CM_translator.so", "libGLES_CM.so.1", "libGLES_CM.so"};
			#endif

			libGLES_CM = loadLibrary(libGLES_CM_lib);

			if(libGLES_CM)
			{
				auto libGLES_CMexportsProc = (LibGLES_CMexports *(*)())getProcAddress(libGLES_CM, "libGLES_CMexports");
				libGLES_CMexports = libGLES_CMexportsProc();
			}
		}

		return libGLES_CMexports;
	}

	void *libGLES_CM;
	LibGLES_CMexports *libGLES_CMexports;
};

#endif libGLES_CM_hpp
