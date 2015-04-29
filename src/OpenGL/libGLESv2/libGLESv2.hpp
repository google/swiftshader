#ifndef libGLESv2_hpp
#define libGLESv2_hpp

#include <GLES/gl.h>
#include <GLES/glext.h>
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

class LibGLESv2exports
{
public:
	LibGLESv2exports();

	egl::Context *(*es2CreateContext)(const egl::Config *config, const egl::Context *shareContext, int clientVersion);
	__eglMustCastToProperFunctionPointerType (*es2GetProcAddress)(const char *procname);
	egl::Image *(*createBackBuffer)(int width, int height, const egl::Config *config);
	egl::Image *(*createDepthStencil)(unsigned int width, unsigned int height, sw::Format format, int multiSampleDepth, bool discard);
	sw::FrameBuffer *(*createFrameBuffer)(EGLNativeDisplayType display, EGLNativeWindowType window, int width, int height);
};

class LibGLESv2
{
public:
	LibGLESv2()
	{
		libGLESv2 = nullptr;
		libGLESv2exports = nullptr;
	}

	~LibGLESv2()
	{
		freeLibrary(libGLESv2);
	}

	operator bool()
	{
		return loadExports();
	}

	LibGLESv2exports *operator->()
	{
		return loadExports();
	}

private:
	LibGLESv2exports *loadExports()
	{
		if(!libGLESv2)
		{
			#if defined(_WIN32)
			const char *libGLESv2_lib[] = {"libGLESv2.dll", "libGLES_V2_translator.dll"};
			#elif defined(__ANDROID__)
			const char *libGLESv2_lib[] = {"/vendor/lib/egl/libGLESv2_swiftshader.so"};
			#elif defined(__LP64__)
			const char *libGLESv2_lib[] = {"lib64GLES_V2_translator.so", "libGLESv2.so.2", "libGLESv2.so"};
			#else
			const char *libGLESv2_lib[] = {"libGLES_V2_translator.so", "libGLESv2.so.2", "libGLESv2.so"};
			#endif

			libGLESv2 = loadLibrary(libGLESv2_lib, "libGLESv2_swiftshader");

			if(libGLESv2)
			{
				auto libGLESv2_swiftshader = (LibGLESv2exports *(*)())getProcAddress(libGLESv2, "libGLESv2_swiftshader");
				libGLESv2exports = libGLESv2_swiftshader();
			}
		}

		return libGLESv2exports;
	}

	void *libGLESv2;
	LibGLESv2exports *libGLESv2exports;
};

#endif   // libGLESv2_hpp
