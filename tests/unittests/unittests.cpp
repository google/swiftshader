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
		#if defined(_WIN32)
			// The DLLs are delay loaded (see BUILD.gn), so we can load
			// the correct ones from the swiftshader subdirectory.
			HMODULE libEGL = LoadLibraryA("swiftshader\\libEGL.dll");
			EXPECT_NE((HMODULE)NULL, libEGL);

			HMODULE libGLESv2 = LoadLibraryA("swiftshader\\libGLESv2.dll");
			EXPECT_NE((HMODULE)NULL, libGLESv2);
		#endif
	}
};

TEST_F(SwiftShaderTest, CompilationOnly)
{
	// Empty test to trigger compilation of SwiftShader on build bots
}

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

	const char *vendor = eglQueryString(display, EGL_VENDOR);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_STREQ("Google Inc.", vendor);

	const char *version = eglQueryString(display, EGL_VERSION);
	EXPECT_EQ(EGL_SUCCESS, eglGetError());
	EXPECT_THAT(version, testing::HasSubstr("1.4 SwiftShader "));
}
