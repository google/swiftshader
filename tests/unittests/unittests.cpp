// Copyright 2017 The SwiftShader Authors. All Rights Reserved.
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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

class SwiftShaderTest : public testing::Test
{
protected:
	void SetUp() override
	{
		#if defined(_WIN32) && !defined(STANDALONE)
			// The DLLs are delay loaded (see BUILD.gn), so we can load
			// the correct ones from Chrome's swiftshader subdirectory.
			HMODULE libEGL = LoadLibraryA("swiftshader\\libEGL.dll");
			EXPECT_NE((HMODULE)NULL, libEGL);

			HMODULE libGLESv2 = LoadLibraryA("swiftshader\\libGLESv2.dll");
			EXPECT_NE((HMODULE)NULL, libGLESv2);
		#endif
	}
};

TEST_F(SwiftShaderTest, Initalization)
{
	EXPECT_EQ(EGL_SUCCESS, eglGetError());

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_DISPLAY, display);

	eglQueryString(display, EGL_VENDOR);
	EXPECT_EQ(EGL_NOT_INITIALIZED, eglGetError());

	EGLint major;
	EGLint minor;
	EGLBoolean initialized = eglInitialize(display, &major, &minor);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, initialized);
	EXPECT_EQ(1, major);
	EXPECT_EQ(4, minor);

	const char *eglVendor = eglQueryString(display, EGL_VENDOR);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_STREQ("Google Inc.", eglVendor);

	const char *eglVersion = eglQueryString(display, EGL_VERSION);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_THAT(eglVersion, testing::HasSubstr("1.4 SwiftShader "));

	eglBindAPI(EGL_OPENGL_ES_API);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());

	const EGLint configAttributes[] =
	{
		EGL_SURFACE_TYPE,		EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLConfig config;
	EGLint num_config = -1;
	EGLBoolean success = eglChooseConfig(display, configAttributes, &config, 1, &num_config);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ(num_config, 1);
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

	EGLint conformant = 0;
	eglGetConfigAttrib(display, config, EGL_CONFORMANT, &conformant);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_TRUE(conformant & EGL_OPENGL_ES2_BIT);

	EGLint renderableType = 0;
	eglGetConfigAttrib(display, config, EGL_RENDERABLE_TYPE, &renderableType);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_TRUE(renderableType & EGL_OPENGL_ES2_BIT);

	EGLint surfaceType = 0;
	eglGetConfigAttrib(display, config, EGL_RENDERABLE_TYPE, &surfaceType);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_TRUE(surfaceType & EGL_WINDOW_BIT);

	EGLint surfaceAttributes[] =
	{
		EGL_WIDTH, 1920,
		EGL_HEIGHT, 1080,
		EGL_NONE
	};

	EGLSurface surface = eglCreatePbufferSurface(display, config, surfaceAttributes);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_SURFACE, surface);

	EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLContext context = eglCreateContext(display, config, NULL, contextAttributes);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_SURFACE, surface);

	success = eglMakeCurrent(display, surface, surface, context);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

	EGLDisplay currentDisplay = eglGetCurrentDisplay();
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_DISPLAY, currentDisplay);

	EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_SURFACE, currentDrawSurface);

	EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_SURFACE, currentReadSurface);

	EGLContext currentContext = eglGetCurrentContext();
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_NE(EGL_NO_CONTEXT, currentContext);

	EXPECT_EQ((GLenum)GL_NO_ERROR, glGetError());

	const GLubyte *glVendor = glGetString(GL_VENDOR);
	EXPECT_EQ((GLenum)GL_NO_ERROR, glGetError());
	EXPECT_STREQ("Google Inc.", (const char*)glVendor);

	const GLubyte *glRenderer = glGetString(GL_RENDERER);
	EXPECT_EQ((GLenum)GL_NO_ERROR, glGetError());
	EXPECT_STREQ("Google SwiftShader", (const char*)glRenderer);

	const GLubyte *glVersion = glGetString(GL_VERSION);
	EXPECT_EQ((GLenum)GL_NO_ERROR, glGetError());
	EXPECT_THAT((const char*)glVersion, testing::HasSubstr("OpenGL ES 2.0 SwiftShader "));

	success = eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

	currentDisplay = eglGetCurrentDisplay();
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ(EGL_NO_DISPLAY, currentDisplay);

	currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ(EGL_NO_SURFACE, currentDrawSurface);

	currentReadSurface = eglGetCurrentSurface(EGL_READ);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ(EGL_NO_SURFACE, currentReadSurface);

	currentContext = eglGetCurrentContext();
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ(EGL_NO_CONTEXT, currentContext);

	success = eglDestroyContext(display, context);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

	success = eglDestroySurface(display, surface);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

	success = eglTerminate(display);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_EQ((EGLBoolean)EGL_TRUE, success);
}
