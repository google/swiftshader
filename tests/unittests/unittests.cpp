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
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#define EXPECT_GLENUM_EQ(expected, actual) EXPECT_EQ(static_cast<GLenum>(expected), static_cast<GLenum>(actual))

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

	void compareColor(unsigned char referenceColor[4])
	{
		unsigned char color[4] = { 0 };
		glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
		EXPECT_EQ(color[0], referenceColor[0]);
		EXPECT_EQ(color[1], referenceColor[1]);
		EXPECT_EQ(color[2], referenceColor[2]);
		EXPECT_EQ(color[3], referenceColor[3]);
	}

	void Initialize(int version, bool withChecks)
	{
		EXPECT_EQ(EGL_SUCCESS, eglGetError());

		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		if(withChecks)
		{
			EXPECT_EQ(EGL_SUCCESS, eglGetError());
			EXPECT_NE(EGL_NO_DISPLAY, display);

			eglQueryString(display, EGL_VENDOR);
			EXPECT_EQ(EGL_NOT_INITIALIZED, eglGetError());
		}

		EGLint major;
		EGLint minor;
		EGLBoolean initialized = eglInitialize(display, &major, &minor);

		if(withChecks)
		{
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
		}

		eglBindAPI(EGL_OPENGL_ES_API);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());

		const EGLint configAttributes[] =
		{
			EGL_SURFACE_TYPE,		EGL_PBUFFER_BIT,
			EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
			EGL_ALPHA_SIZE,			8,
			EGL_NONE
		};
		EGLint num_config = -1;
		EGLBoolean success = eglChooseConfig(display, configAttributes, &config, 1, &num_config);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ(num_config, 1);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		if(withChecks)
		{
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
		}

		EGLint surfaceAttributes[] =
		{
			EGL_WIDTH, 1920,
			EGL_HEIGHT, 1080,
			EGL_NONE
		};

		surface = eglCreatePbufferSurface(display, config, surfaceAttributes);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_NE(EGL_NO_SURFACE, surface);

		EGLint contextAttributes[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, version,
			EGL_NONE
		};

		context = eglCreateContext(display, config, NULL, contextAttributes);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_NE(EGL_NO_CONTEXT, context);

		success = eglMakeCurrent(display, surface, surface, context);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		if(withChecks)
		{
			EGLDisplay currentDisplay = eglGetCurrentDisplay();
			EXPECT_EQ(EGL_SUCCESS, eglGetError());
			EXPECT_EQ(display, currentDisplay);

			EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
			EXPECT_EQ(EGL_SUCCESS, eglGetError());
			EXPECT_EQ(surface, currentDrawSurface);

			EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
			EXPECT_EQ(EGL_SUCCESS, eglGetError());
			EXPECT_EQ(surface, currentReadSurface);

			EGLContext currentContext = eglGetCurrentContext();
			EXPECT_EQ(EGL_SUCCESS, eglGetError());
			EXPECT_EQ(context, currentContext);
		}

		EXPECT_GLENUM_EQ(GL_NO_ERROR, glGetError());
	}

	void Uninitialize()
	{
		EGLBoolean success = eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		EGLDisplay currentDisplay = eglGetCurrentDisplay();
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ(EGL_NO_DISPLAY, currentDisplay);

		EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ(EGL_NO_SURFACE, currentDrawSurface);

		EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
		EXPECT_EQ(EGL_SUCCESS, eglGetError());
		EXPECT_EQ(EGL_NO_SURFACE, currentReadSurface);

		EGLContext currentContext = eglGetCurrentContext();
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

	GLuint createProgram(const std::string& vs, const std::string& fs)
	{
		GLuint program = glCreateProgram();
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vsSource[1] = { vs.c_str() };
		glShaderSource(vsShader, 1, vsSource, nullptr);
		glCompileShader(vsShader);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fsSource[1] = { fs.c_str() };
		glShaderSource(fsShader, 1, fsSource, nullptr);
		glCompileShader(fsShader);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		glAttachShader(program, vsShader);
		glAttachShader(program, fsShader);
		glLinkProgram(program);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		return program;
	}

	void drawQuad(GLuint program)
	{
		GLint prevProgram = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

		glUseProgram(program);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		GLint posLoc = glGetAttribLocation(program, "position");
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		GLint location = glGetUniformLocation(program, "tex");
		ASSERT_NE(-1, location);
		glUniform1i(location, 0);

		float vertices[18] = { -1.0f,  1.0f, 0.5f,
		                       -1.0f, -1.0f, 0.5f,
		                        1.0f, -1.0f, 0.5f,
		                       -1.0f,  1.0f, 0.5f,
		                        1.0f, -1.0f, 0.5f,
		                        1.0f,  1.0f, 0.5f };

		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(posLoc);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());

		glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDisableVertexAttribArray(posLoc);
		glUseProgram(prevProgram);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	}

	EGLDisplay getDisplay() const { return display; }
	EGLDisplay getConfig() const { return config; }
	EGLSurface getSurface() const { return surface; }
	EGLContext getContext() const { return context; }
private:
	EGLDisplay display;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;
};

TEST_F(SwiftShaderTest, Initalization)
{
	Initialize(2, true);

	const GLubyte *glVendor = glGetString(GL_VENDOR);
	EXPECT_GLENUM_EQ(GL_NO_ERROR, glGetError());
	EXPECT_STREQ("Google Inc.", (const char*)glVendor);

	const GLubyte *glRenderer = glGetString(GL_RENDERER);
	EXPECT_GLENUM_EQ(GL_NO_ERROR, glGetError());
	EXPECT_STREQ("Google SwiftShader", (const char*)glRenderer);

	const GLubyte *glVersion = glGetString(GL_VERSION);
	EXPECT_GLENUM_EQ(GL_NO_ERROR, glGetError());
	EXPECT_THAT((const char*)glVersion, testing::HasSubstr("OpenGL ES 2.0 SwiftShader "));

	Uninitialize();
}

// Note: GL_ARB_texture_rectangle is part of gl2extchromium.h in the Chromium repo
// GL_ARB_texture_rectangle
#ifndef GL_ARB_texture_rectangle
#define GL_ARB_texture_rectangle 1

#ifndef GL_SAMPLER_2D_RECT_ARB
#define GL_SAMPLER_2D_RECT_ARB 0x8B63
#endif

#ifndef GL_TEXTURE_BINDING_RECTANGLE_ARB
#define GL_TEXTURE_BINDING_RECTANGLE_ARB 0x84F6
#endif

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#ifndef GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB 0x84F8
#endif

#endif  // GL_ARB_texture_rectangle

// Test using TexImage2D to define a rectangle texture

TEST_F(SwiftShaderTest, TextureRectangle_TexImage2D)
{
	Initialize(2, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	// Defining level 0 is allowed
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	// Defining level other than 0 is not allowed
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &maxSize);

	// Defining a texture of the max size is allowed
	{
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, maxSize, maxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		GLenum error = glGetError();
		ASSERT_TRUE(error == GL_NO_ERROR || error == GL_OUT_OF_MEMORY);
	}

	// Defining a texture larger than the max size is disallowed
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, maxSize + 1, maxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, maxSize, maxSize + 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	Uninitialize();
}

// Test using CompressedTexImage2D cannot be used on a retangle texture
TEST_F(SwiftShaderTest, TextureRectangle_CompressedTexImage2DDisallowed)
{
	Initialize(2, false);

	const char data[128] = { 0 };

	// Control case: 2D texture
	{
		GLuint tex = 1;
		glBindTexture(GL_TEXTURE_2D, tex);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 16, 16, 0, 128, data);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	}

	// Rectangle textures cannot be compressed
	{
		GLuint tex = 2;
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glCompressedTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 16, 16, 0, 128, data);
		EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	}

	Uninitialize();
}

// Test using TexStorage2D to define a rectangle texture (ES3)
TEST_F(SwiftShaderTest, TextureRectangle_TexStorage2D)
{
	Initialize(3, false);

	// Defining one level is allowed
	{
		GLuint tex = 1;
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8UI, 16, 16);
		EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	}

	// Having more than one level is not allowed
	{
		GLuint tex = 2;
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		// Use 5 levels because the EXT_texture_storage extension requires a mip chain all the way
		// to a 1x1 mip.
		glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 5, GL_RGBA8UI, 16, 16);
		EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());
	}

	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &maxSize);

	// Defining a texture of the max size is allowed but still allow for OOM
	{
		GLuint tex = 3;
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8UI, maxSize, maxSize);
		GLenum error = glGetError();
		ASSERT_TRUE(error == GL_NO_ERROR || error == GL_OUT_OF_MEMORY);
	}

	// Defining a texture larger than the max size is disallowed
	{
		GLuint tex = 4;
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8UI, maxSize + 1, maxSize);
		EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());
		glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8UI, maxSize, maxSize + 1);
		EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());
	}

	// Compressed formats are disallowed
	GLuint tex = 5;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glTexStorage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 16, 16);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	Uninitialize();
}

// Test validation of disallowed texture parameters
TEST_F(SwiftShaderTest, TextureRectangle_TexParameterRestriction)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	// Only wrap mode CLAMP_TO_EDGE is supported
	// Wrap S
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	// Wrap T
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	// Min filter has to be nearest or linear
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	// Base level has to be 0
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_BASE_LEVEL, 0);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_BASE_LEVEL, 1);
	EXPECT_GLENUM_EQ(GL_INVALID_OPERATION, glGetError());

	Uninitialize();
}

// Test validation of "level" in FramebufferTexture2D
TEST_F(SwiftShaderTest, TextureRectangle_FramebufferTexture2DLevel)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Using level 0 of a rectangle texture is valid.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);
	EXPECT_GLENUM_EQ(GL_FRAMEBUFFER_COMPLETE, glCheckFramebufferStatus(GL_FRAMEBUFFER));
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	// Setting level != 0 is invalid
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 1);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	Uninitialize();
}

// Test sampling from a rectangle texture
TEST_F(SwiftShaderTest, TextureRectangle_SamplingFromRectangle)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	unsigned char green[4] = { 0, 255, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	const std::string vs =
		"attribute vec4 position;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(position.xy, 0.0, 1.0);\n"
		"}\n";

	const std::string fs =
		"#extension GL_ARB_texture_rectangle : require\n"
		"precision mediump float;\n"
		"uniform sampler2DRect tex;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = texture2DRect(tex, vec2(0, 0));\n"
		"}\n";

	GLuint program = createProgram(vs, fs);

	glUseProgram(program);
	GLint location = glGetUniformLocation(program, "tex");
	ASSERT_NE(-1, location);
	glUniform1i(location, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	drawQuad(program);

	compareColor(green);

	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	Uninitialize();
}

// Test sampling from a rectangle texture
TEST_F(SwiftShaderTest, TextureRectangle_SamplingFromRectangleESSL3)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	unsigned char green[4] = { 0, 255, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	const std::string vs =
		"#version 300 es\n"
		"in vec4 position;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(position.xy, 0.0, 1.0);\n"
		"}\n";

	const std::string fs =
		"#version 300 es\n"
		"#extension GL_ARB_texture_rectangle : require\n"
		"precision mediump float;\n"
		"uniform sampler2DRect tex;\n"
		"out vec4 fragColor;\n"
		"void main()\n"
		"{\n"
		"    fragColor = texture(tex, vec2(0, 0));\n"
		"}\n";

	GLuint program = createProgram(vs, fs);
	glUseProgram(program);
	GLint location = glGetUniformLocation(program, "tex");
	ASSERT_NE(-1, location);
	glUniform1i(location, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	drawQuad(program);

	compareColor(green);

	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	Uninitialize();
}

// Test attaching a rectangle texture and rendering to it.
TEST_F(SwiftShaderTest, TextureRectangle_RenderToRectangle)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	unsigned char black[4] = { 0, 0, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);
	EXPECT_GLENUM_EQ(GL_FRAMEBUFFER_COMPLETE, glCheckFramebufferStatus(GL_FRAMEBUFFER));
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	// Clearing a texture is just as good as checking we can render to it, right?
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	unsigned char green[4] = { 0, 255, 0, 255 };
	compareColor(green);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	Uninitialize();
}

TEST_F(SwiftShaderTest, TextureRectangle_DefaultSamplerParameters)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	GLint minFilter = 0;
	glGetTexParameteriv(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, &minFilter);
	EXPECT_GLENUM_EQ(GL_LINEAR, minFilter);

	GLint wrapS = 0;
	glGetTexParameteriv(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, &wrapS);
	EXPECT_GLENUM_EQ(GL_CLAMP_TO_EDGE, wrapS);

	GLint wrapT = 0;
	glGetTexParameteriv(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, &wrapT);
	EXPECT_GLENUM_EQ(GL_CLAMP_TO_EDGE, wrapT);

	Uninitialize();
}

// Test glCopyTexImage with rectangle textures (ES3)
TEST_F(SwiftShaderTest, TextureRectangle_CopyTexImage)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	// Error case: level != 0
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8, 0, 0, 1, 1, 0);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	// level = 0 works and defines the texture.
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, 0, 0, 1, 1, 0);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);

	unsigned char green[4] = { 0, 255, 0, 255 };
	compareColor(green);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	Uninitialize();
}

// Test glCopyTexSubImage with rectangle textures (ES3)
TEST_F(SwiftShaderTest, TextureRectangle_CopyTexSubImage)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	unsigned char black[4] = { 0, 0, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	// Error case: level != 0
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 1, 0, 0, 0, 0, 1, 1);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	// level = 0 works and defines the texture.
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, 0, 0, 1, 1);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);

	unsigned char green[4] = { 0, 255, 0, 255 };
	compareColor(green);
	EXPECT_GLENUM_EQ(GL_NONE, glGetError());

	Uninitialize();
}
