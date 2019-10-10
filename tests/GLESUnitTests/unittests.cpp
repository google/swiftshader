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

// OpenGL ES unit tests that provide coverage for functionality not tested by
// the dEQP test suite. Also used as a smoke test.

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <string.h>
#include <cstdint>

#define EXPECT_GLENUM_EQ(expected, actual) EXPECT_EQ(static_cast<GLenum>(expected), static_cast<GLenum>(actual))

#define EXPECT_NO_GL_ERROR() EXPECT_GLENUM_EQ(GL_NO_ERROR, glGetError())
#define EXPECT_NO_EGL_ERROR() EXPECT_EQ(EGL_SUCCESS, eglGetError())

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

	void expectFramebufferColor(const unsigned char referenceColor[4], GLint x = 0, GLint y = 0)
	{
		unsigned char color[4] = { 0 };
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
		EXPECT_NO_GL_ERROR();
		EXPECT_EQ(color[0], referenceColor[0]);
		EXPECT_EQ(color[1], referenceColor[1]);
		EXPECT_EQ(color[2], referenceColor[2]);
		EXPECT_EQ(color[3], referenceColor[3]);
	}

	void expectFramebufferColor(const float referenceColor[4], GLint x = 0, GLint y = 0)
	{
		float color[4] = { 0 };
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &color);
		EXPECT_NO_GL_ERROR();
		EXPECT_EQ(color[0], referenceColor[0]);
		EXPECT_EQ(color[1], referenceColor[1]);
		EXPECT_EQ(color[2], referenceColor[2]);
		EXPECT_EQ(color[3], referenceColor[3]);
	}

	void Initialize(int version, bool withChecks)
	{
		EXPECT_NO_EGL_ERROR();

		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		if(withChecks)
		{
			EXPECT_NO_EGL_ERROR();
			EXPECT_NE(EGL_NO_DISPLAY, display);

			eglQueryString(display, EGL_VENDOR);
			EXPECT_EQ(EGL_NOT_INITIALIZED, eglGetError());
		}

		EGLint major;
		EGLint minor;
		EGLBoolean initialized = eglInitialize(display, &major, &minor);

		if(withChecks)
		{
			EXPECT_NO_EGL_ERROR();
			EXPECT_EQ((EGLBoolean)EGL_TRUE, initialized);
			EXPECT_EQ(1, major);
			EXPECT_EQ(4, minor);

			const char *eglVendor = eglQueryString(display, EGL_VENDOR);
			EXPECT_NO_EGL_ERROR();
			EXPECT_STREQ("Google Inc.", eglVendor);

			const char *eglVersion = eglQueryString(display, EGL_VERSION);
			EXPECT_NO_EGL_ERROR();
			EXPECT_THAT(eglVersion, testing::HasSubstr("1.4 SwiftShader "));
		}

		eglBindAPI(EGL_OPENGL_ES_API);
		EXPECT_NO_EGL_ERROR();

		const EGLint configAttributes[] =
		{
			EGL_SURFACE_TYPE,		EGL_PBUFFER_BIT,
			EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
			EGL_ALPHA_SIZE,			8,
			EGL_NONE
		};

		EGLint num_config = -1;
		EGLBoolean success = eglChooseConfig(display, configAttributes, &config, 1, &num_config);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ(num_config, 1);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		if(withChecks)
		{
			EGLint conformant = 0;
			eglGetConfigAttrib(display, config, EGL_CONFORMANT, &conformant);
			EXPECT_NO_EGL_ERROR();
			EXPECT_TRUE(conformant & EGL_OPENGL_ES2_BIT);

			EGLint renderableType = 0;
			eglGetConfigAttrib(display, config, EGL_RENDERABLE_TYPE, &renderableType);
			EXPECT_NO_EGL_ERROR();
			EXPECT_TRUE(renderableType & EGL_OPENGL_ES2_BIT);

			EGLint surfaceType = 0;
			eglGetConfigAttrib(display, config, EGL_SURFACE_TYPE, &surfaceType);
			EXPECT_NO_EGL_ERROR();
			EXPECT_TRUE(surfaceType & EGL_WINDOW_BIT);
		}

		EGLint surfaceAttributes[] =
		{
			EGL_WIDTH, 1920,
			EGL_HEIGHT, 1080,
			EGL_NONE
		};

		surface = eglCreatePbufferSurface(display, config, surfaceAttributes);
		EXPECT_NO_EGL_ERROR();
		EXPECT_NE(EGL_NO_SURFACE, surface);

		EGLint contextAttributes[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, version,
			EGL_NONE
		};

		context = eglCreateContext(display, config, NULL, contextAttributes);
		EXPECT_NO_EGL_ERROR();
		EXPECT_NE(EGL_NO_CONTEXT, context);

		success = eglMakeCurrent(display, surface, surface, context);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		if(withChecks)
		{
			EGLDisplay currentDisplay = eglGetCurrentDisplay();
			EXPECT_NO_EGL_ERROR();
			EXPECT_EQ(display, currentDisplay);

			EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
			EXPECT_NO_EGL_ERROR();
			EXPECT_EQ(surface, currentDrawSurface);

			EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
			EXPECT_NO_EGL_ERROR();
			EXPECT_EQ(surface, currentReadSurface);

			EGLContext currentContext = eglGetCurrentContext();
			EXPECT_NO_EGL_ERROR();
			EXPECT_EQ(context, currentContext);
		}

		EXPECT_NO_GL_ERROR();
	}

	void Uninitialize()
	{
		EXPECT_NO_GL_ERROR();

		EGLBoolean success = eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		EGLDisplay currentDisplay = eglGetCurrentDisplay();
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ(EGL_NO_DISPLAY, currentDisplay);

		EGLSurface currentDrawSurface = eglGetCurrentSurface(EGL_DRAW);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ(EGL_NO_SURFACE, currentDrawSurface);

		EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ(EGL_NO_SURFACE, currentReadSurface);

		EGLContext currentContext = eglGetCurrentContext();
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ(EGL_NO_CONTEXT, currentContext);

		success = eglDestroyContext(display, context);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		success = eglDestroySurface(display, surface);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);

		success = eglTerminate(display);
		EXPECT_NO_EGL_ERROR();
		EXPECT_EQ((EGLBoolean)EGL_TRUE, success);
	}

	struct ProgramHandles
	{
		GLuint program;
		GLuint vertexShader;
		GLuint fragmentShader;
	};

	GLuint MakeShader(const std::string &source, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		const char *c_source[1] = { source.c_str() };
		glShaderSource(shader, 1, c_source, nullptr);
		glCompileShader(shader);
		EXPECT_NO_GL_ERROR();

		GLchar buf[1024];
		GLint compileStatus = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
		EXPECT_EQ(compileStatus, GL_TRUE) << "Compile status: " << std::endl << buf;

		return shader;
	}

	GLuint MakeProgram(GLuint vs, GLuint fs)
	{
		GLuint program;

		program = glCreateProgram();
		EXPECT_NO_GL_ERROR();

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		EXPECT_NO_GL_ERROR();

		return program;
	}

	void LinkProgram(GLuint program)
	{
		GLchar buf[1024];
		glLinkProgram(program);

		GLint linkStatus = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		glGetProgramInfoLog(program, sizeof(buf), nullptr, buf);
		EXPECT_NE(linkStatus, 0) << "Link status: " << std::endl << buf;

		EXPECT_NO_GL_ERROR();
	}


	ProgramHandles createProgram(const std::string& vs, const std::string& fs)
	{
		ProgramHandles ph;
		ph.vertexShader = MakeShader(vs, GL_VERTEX_SHADER);
		ph.fragmentShader = MakeShader(fs, GL_FRAGMENT_SHADER);
		ph.program = MakeProgram(ph.vertexShader, ph.fragmentShader);
		LinkProgram(ph.program);

		return ph;
	}

	void deleteProgram(const ProgramHandles& ph)
	{
		glDeleteShader(ph.fragmentShader);
		glDeleteShader(ph.vertexShader);
		glDeleteProgram(ph.program);

		EXPECT_NO_GL_ERROR();
	}

	void drawQuad(GLuint program, const char* textureName = nullptr)
	{
		GLint prevProgram = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

		glUseProgram(program);
		EXPECT_NO_GL_ERROR();

		GLint posLoc = glGetAttribLocation(program, "position");
		EXPECT_NO_GL_ERROR();

		if(textureName)
		{
			GLint location = glGetUniformLocation(program, textureName);
			ASSERT_NE(-1, location);
			glUniform1i(location, 0);
		}

		float vertices[18] = { -1.0f,  1.0f, 0.5f,
		                       -1.0f, -1.0f, 0.5f,
		                        1.0f, -1.0f, 0.5f,
		                       -1.0f,  1.0f, 0.5f,
		                        1.0f, -1.0f, 0.5f,
		                        1.0f,  1.0f, 0.5f };

		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(posLoc);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		EXPECT_NO_GL_ERROR();

		glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDisableVertexAttribArray(posLoc);
		glUseProgram(prevProgram);
		EXPECT_NO_GL_ERROR();
	}

	std::string replace(std::string str, const std::string& substr, const std::string& replacement)
	{
		size_t pos = 0;
		while((pos = str.find(substr, pos)) != std::string::npos) {
			str.replace(pos, substr.length(), replacement);
			pos += replacement.length();
		}
		return str;
	}

	void checkCompiles(std::string v, std::string f)
	{
		Initialize(3, false);

		std::string vs =
			R"(#version 300 es
			in vec4 position;
			out float unfoldable;
			$INSERT
			void main()
			{
			    unfoldable = position.x;
			    gl_Position = vec4(position.xy, 0.0, 1.0);
			    gl_Position.x += F(unfoldable);\
			})";

		std::string fs =
			R"(#version 300 es
			precision mediump float;
			in float unfoldable;
			out vec4 fragColor;
			$INSERT
			void main()
			{
			    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
			    fragColor.x += F(unfoldable);
			})";

		vs = replace(vs, "$INSERT", (v.length() > 0) ? v : "float F(float ignored) { return 0.0; }");
		fs = replace(fs, "$INSERT", (f.length() > 0) ? f : "float F(float ignored) { return 0.0; }");

		const ProgramHandles ph = createProgram(vs, fs);

		glUseProgram(ph.program);

		drawQuad(ph.program);

		deleteProgram(ph);

		EXPECT_NO_GL_ERROR();

		Uninitialize();
	}

	void checkCompiles(std::string s)
	{
		checkCompiles(s, "");
		checkCompiles("", s);
	}

	std::string checkCompileFails(std::string source, GLenum glShaderType)
	{
		Initialize(3, false);

		GLint compileStatus = 0;
		const char *c_source[1] = { source.c_str() };
		GLuint glShader = glCreateShader(glShaderType);

		glShaderSource(glShader, 1, c_source, nullptr);
		glCompileShader(glShader);
		EXPECT_NO_GL_ERROR();

		std::string log;
		char *buf;
		GLsizei length = 0;
		GLsizei written = 0;

		glGetShaderiv(glShader, GL_COMPILE_STATUS, &compileStatus);
		EXPECT_EQ(compileStatus, GL_FALSE);

		glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &length);
		EXPECT_NO_GL_ERROR();
		EXPECT_NE(length, 0);
		buf = new char[length];

		glGetShaderInfoLog(glShader, length, &written, buf);
		EXPECT_NO_GL_ERROR();
		EXPECT_EQ(length, written + 1);
		log.assign(buf, length);
		delete[] buf;

		glDeleteShader(glShader);

		Uninitialize();

		return log;
	}

	void checkCompileFails(std::string s)
	{
		std::string vs =
			R"(#version 300 es
			in vec4 position;
			out float unfoldable;
			$INSERT
			void main()
			{
			    unfoldable = position.x;
			    gl_Position = vec4(position.xy, 0.0, 1.0);
			    gl_Position.x += F(unfoldable);
			})";

		std::string fs =
			R"(#version 300 es
			precision mediump float;
			in float unfoldable;
			out vec4 fragColor;
			$INSERT
			void main()
			{
			    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
			    fragColor.x += F(unfoldable);
			})";

		vs = replace(vs, "$INSERT", s);
		fs = replace(fs, "$INSERT", s);

		checkCompileFails(vs, GL_VERTEX_SHADER);
		checkCompileFails(fs, GL_FRAGMENT_SHADER);
	}

	EGLDisplay getDisplay() const { return display; }
	EGLConfig getConfig() const { return config; }
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
	EXPECT_NO_GL_ERROR();
	EXPECT_STREQ("Google Inc.", (const char*)glVendor);

	const GLubyte *glRenderer = glGetString(GL_RENDERER);
	EXPECT_NO_GL_ERROR();
	EXPECT_STREQ("Google SwiftShader", (const char*)glRenderer);

	// SwiftShader return an OpenGL ES 3.0 context when a 2.0 context is requested, as allowed by the spec.
	const GLubyte *glVersion = glGetString(GL_VERSION);
	EXPECT_NO_GL_ERROR();
	EXPECT_THAT((const char*)glVersion, testing::HasSubstr("OpenGL ES 3.0 SwiftShader "));

	Uninitialize();
}

// Test attempting to clear an incomplete framebuffer
TEST_F(SwiftShaderTest, ClearIncomplete)
{
	Initialize(3, false);

	GLfloat zero_float = 0;
	GLuint renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	EXPECT_NO_GL_ERROR();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_R8I, 43, 27);
	EXPECT_NO_GL_ERROR();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	EXPECT_NO_GL_ERROR();
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	EXPECT_NO_GL_ERROR();
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	EXPECT_NO_GL_ERROR();
	glClearBufferfv(GL_DEPTH, 0, &zero_float);
	EXPECT_GLENUM_EQ(GL_INVALID_FRAMEBUFFER_OPERATION, glGetError());

	Uninitialize();
}

// Test unrolling of a loop
TEST_F(SwiftShaderTest, UnrollLoop)
{
	Initialize(3, false);

	unsigned char green[4] = { 0, 255, 0, 255 };

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		out vec4 color;
		void main()
		{
		   for(int i = 0; i < 4; i++)
		   {
		       color[i] = (i % 2 == 0) ? 0.0 : 1.0;
		   }
			gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		in vec4 color;
		out vec4 fragColor;
		void main()
		{
			fragColor = color;
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	// Expect the info log to contain "unrolled". This is not a spec requirement.
	GLsizei length = 0;
	glGetShaderiv(ph.vertexShader, GL_INFO_LOG_LENGTH, &length);
	EXPECT_NO_GL_ERROR();
	EXPECT_NE(length, 0);
	char *log = new char[length];
	GLsizei written = 0;
	glGetShaderInfoLog(ph.vertexShader, length, &written, log);
	EXPECT_NO_GL_ERROR();
	EXPECT_EQ(length, written + 1);
	EXPECT_NE(strstr(log, "unrolled"), nullptr);
	delete[] log;

	glUseProgram(ph.program);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program);

	deleteProgram(ph);

	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test non-canonical or non-deterministic loops do not get unrolled
TEST_F(SwiftShaderTest, DynamicLoop)
{
	Initialize(3, false);

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		out vec4 color;
		void main()
		{
		   for(int i = 0; i < 4; )
		   {
		       color[i] = (i % 2 == 0) ? 0.0 : 1.0;
		       i++;
		   }
			gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		in vec4 color;
		out vec4 fragColor;
		void main()
		{
		   vec4 temp;
		   for(int i = 0; i < 4; i++)
		   {
		       if(color.x < 0.0) return;
		       temp[i] = color[i];
		   }
			fragColor = vec4(temp[0], temp[1], temp[2], temp[3]);
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	// Expect the info logs to be empty. This is not a spec requirement.
	GLsizei length = 0;
	glGetShaderiv(ph.vertexShader, GL_INFO_LOG_LENGTH, &length);
	EXPECT_NO_GL_ERROR();
	EXPECT_EQ(length, 0);
	glGetShaderiv(ph.fragmentShader, GL_INFO_LOG_LENGTH, &length);
	EXPECT_NO_GL_ERROR();
	EXPECT_EQ(length, 0);

	glUseProgram(ph.program);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program);

	deleteProgram(ph);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test dynamic indexing
TEST_F(SwiftShaderTest, DynamicIndexing)
{
	Initialize(3, false);

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		out float color[4];
		void main()
		{
		   for(int i = 0; i < 4; )
		   {
		       int j = (gl_VertexID + i) % 4;
		       color[j] = (j % 2 == 0) ? 0.0 : 1.0;
		       i++;
		   }
			gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		in float color[4];
		out vec4 fragColor;
		void main()
		{
		   float temp[4];
		   for(int i = 0; i < 4; )
		   {
		       temp[i] = color[i];
		       i++;
		   }
			fragColor = vec4(temp[0], temp[1], temp[2], temp[3]);
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	glUseProgram(ph.program);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program);

	deleteProgram(ph);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test vertex attribute location linking
TEST_F(SwiftShaderTest, AttributeLocation)
{
	Initialize(3, false);

	const std::string vs =
		R"(#version 300 es
		layout(location = 0) in vec4 a0;   // Explicitly bound in GLSL
		layout(location = 2) in vec4 a2;   // Explicitly bound in GLSL
		in vec4 a5;                        // Bound to location 5 by API
		in mat2 a3;                        // Implicit location
		in vec4 a1;                        // Implicit location
		in vec4 a6;                        // Implicit location
		out vec4 color;
		void main()
		{
		   vec4 a34 = vec4(a3[0], a3[1]);
			gl_Position = a0;
		   color = (a2 == vec4(1.0, 2.0, 3.0, 4.0) &&
		            a34 == vec4(5.0, 6.0, 7.0, 8.0) &&
		            a5 == vec4(9.0, 10.0, 11.0, 12.0) &&
		            a1 == vec4(13.0, 14.0, 15.0, 16.0) &&
		            a6 == vec4(17.0, 18.0, 19.0, 20.0)) ?
		           vec4(0.0, 1.0, 0.0, 1.0) :
		           vec4(1.0, 0.0, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		in vec4 color;
		out vec4 fragColor;
		void main()
		{
			fragColor = color;
		})";

	ProgramHandles ph;
	ph.vertexShader = MakeShader(vs, GL_VERTEX_SHADER);
	ph.fragmentShader = MakeShader(fs, GL_FRAGMENT_SHADER);
	ph.program = glCreateProgram();
	EXPECT_NO_GL_ERROR();

	// Not assigned a layout location in GLSL. Bind it explicitly with the API.
	glBindAttribLocation(ph.program, 5, "a5");
	EXPECT_NO_GL_ERROR();

	// Should not override GLSL layout location qualifier
	glBindAttribLocation(ph.program, 8, "a2");
	EXPECT_NO_GL_ERROR();

	glAttachShader(ph.program, ph.vertexShader);
	glAttachShader(ph.program, ph.fragmentShader);
	glLinkProgram(ph.program);
	EXPECT_NO_GL_ERROR();

	// Changes after linking should have no effect
	glBindAttribLocation(ph.program, 0, "a1");
	glBindAttribLocation(ph.program, 6, "a2");
	glBindAttribLocation(ph.program, 2, "a6");

	GLint linkStatus = 0;
	glGetProgramiv(ph.program, GL_LINK_STATUS, &linkStatus);
	EXPECT_NE(linkStatus, 0);
	EXPECT_NO_GL_ERROR();

	float vertices[6][3] = { { -1.0f,  1.0f, 0.5f },
	                         { -1.0f, -1.0f, 0.5f },
	                         {  1.0f, -1.0f, 0.5f },
	                         { -1.0f,  1.0f, 0.5f },
	                         {  1.0f, -1.0f, 0.5f },
	                         {  1.0f,  1.0f, 0.5f } };

	float attributes[5][4] = { { 1.0f, 2.0f, 3.0f, 4.0f },
	                           { 5.0f, 6.0f, 7.0f, 8.0f },
	                           { 9.0f, 10.0f, 11.0f, 12.0f },
	                           { 13.0f, 14.0f, 15.0f, 16.0f },
	                           { 17.0f, 18.0f, 19.0f, 20.0f } };

	GLint a0 = glGetAttribLocation(ph.program, "a0");
	EXPECT_EQ(a0, 0);
	glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(a0);
	EXPECT_NO_GL_ERROR();

	GLint a2 = glGetAttribLocation(ph.program, "a2");
	EXPECT_EQ(a2, 2);
	glVertexAttribPointer(a2, 4, GL_FLOAT, GL_FALSE, 0, attributes[0]);
	glVertexAttribDivisor(a2, 1);
	glEnableVertexAttribArray(a2);
	EXPECT_NO_GL_ERROR();

	GLint a3 = glGetAttribLocation(ph.program, "a3");
	EXPECT_EQ(a3, 3);   // Note: implementation specific
	glVertexAttribPointer(a3 + 0, 2, GL_FLOAT, GL_FALSE, 0, &attributes[1][0]);
	glVertexAttribPointer(a3 + 1, 2, GL_FLOAT, GL_FALSE, 0, &attributes[1][2]);
	glVertexAttribDivisor(a3 + 0, 1);
	glVertexAttribDivisor(a3 + 1, 1);
	glEnableVertexAttribArray(a3 + 0);
	glEnableVertexAttribArray(a3 + 1);
	EXPECT_NO_GL_ERROR();

	GLint a5 = glGetAttribLocation(ph.program, "a5");
	EXPECT_EQ(a5, 5);
	glVertexAttribPointer(a5, 4, GL_FLOAT, GL_FALSE, 0, attributes[2]);
	glVertexAttribDivisor(a5, 1);
	glEnableVertexAttribArray(a5);
	EXPECT_NO_GL_ERROR();

	GLint a1 = glGetAttribLocation(ph.program, "a1");
	EXPECT_EQ(a1, 1);   // Note: implementation specific
	glVertexAttribPointer(a1, 4, GL_FLOAT, GL_FALSE, 0, attributes[3]);
	glVertexAttribDivisor(a1, 1);
	glEnableVertexAttribArray(a1);
	EXPECT_NO_GL_ERROR();

	GLint a6 = glGetAttribLocation(ph.program, "a6");
	EXPECT_EQ(a6, 6);   // Note: implementation specific
	glVertexAttribPointer(a6, 4, GL_FLOAT, GL_FALSE, 0, attributes[4]);
	glVertexAttribDivisor(a6, 1);
	glEnableVertexAttribArray(a6);
	EXPECT_NO_GL_ERROR();

	glUseProgram(ph.program);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	EXPECT_NO_GL_ERROR();

	deleteProgram(ph);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test negative layout locations
TEST_F(SwiftShaderTest, NegativeLocation)
{
	const std::string vs =
		R"(#version 300 es
		layout(location = 0x86868686u) in vec4 a0;   // Explicitly bound in GLSL
		layout(location = 0x96969696u) in vec4 a2;   // Explicitly bound in GLSL
		in vec4 a5;                        // Bound to location 5 by API
		in mat2 a3;                        // Implicit location
		in vec4 a1;                        // Implicit location
		in vec4 a6;                        // Implicit location
		out vec4 color;
		float F(float f)
		{
		   vec4 a34 = vec4(a3[0], a3[1]);\n"
			gl_Position = a0;\n"
		   color = (a2 == vec4(1.0, 2.0, 3.0, 4.0) &&
		            a34 == vec4(5.0, 6.0, 7.0, 8.0) &&
		            a5 == vec4(9.0, 10.0, 11.0, 12.0) &&
		            a1 == vec4(13.0, 14.0, 15.0, 16.0) &&
		            a6 == vec4(17.0, 18.0, 19.0, 20.0)) ?
		           vec4(0.0, 1.0, 0.0, 1.0) :
		           vec4(1.0, 0.0, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		in vec4 color;
		layout(location = 0xA6A6A6A6u) out vec4 fragColor;
		float F main()
		{
			fragColor = color;
		})";

	{
		std::string log = checkCompileFails(vs, GL_VERTEX_SHADER);
		EXPECT_NE(strstr(log.c_str(), "out of range: location must be non-negative"), nullptr);
	}

	{
		std::string log = checkCompileFails(fs, GL_FRAGMENT_SHADER);
		EXPECT_NE(strstr(log.c_str(), "out of range: location must be non-negative"), nullptr);
	}
}

// Tests clearing of a texture with 'dirty' content.
TEST_F(SwiftShaderTest, ClearDirtyTexture)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, 256, 256, 0, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, nullptr);
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	EXPECT_NO_GL_ERROR();
	EXPECT_GLENUM_EQ(GL_FRAMEBUFFER_COMPLETE, glCheckFramebufferStatus(GL_FRAMEBUFFER));

	float dirty_color[3] = { 128 / 255.0f, 64 / 255.0f, 192 / 255.0f };
	GLint dirty_x = 8;
	GLint dirty_y = 12;
	glTexSubImage2D(GL_TEXTURE_2D, 0, dirty_x, dirty_y, 1, 1, GL_RGB, GL_FLOAT, dirty_color);

	const float clear_color[4] = { 1.0f, 32.0f, 0.5f, 1.0f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	expectFramebufferColor(clear_color, dirty_x, dirty_y);

	Uninitialize();
}

// Tests copying between textures of different floating-point formats using a framebuffer object.
TEST_F(SwiftShaderTest, CopyTexImage)
{
	Initialize(3, false);

	GLuint tex1 = 1;
	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 16, 16);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 5, 10, 1, 1, GL_RGBA, GL_FLOAT, &green);
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);
	EXPECT_NO_GL_ERROR();

	GLuint tex2 = 2;
	glBindTexture(GL_TEXTURE_2D, tex2);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 6, 8, 8, 0);
	EXPECT_NO_GL_ERROR();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex2, 0);
	expectFramebufferColor(green, 3, 4);
	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Tests copying to a texture from a pixel buffer object
TEST_F(SwiftShaderTest, CopyTexImageFromPixelBuffer)
{
	Initialize(3, false);
	const GLuint red   = 0xff0000ff;
	const GLuint green = 0x00ff00ff;
	const GLuint blue  = 0x0000ffff;
	// Set up texture
	GLuint texture = 0;
	glGenTextures(1, &texture);
	EXPECT_NO_GL_ERROR();
	GLuint tex_data[4][4] = {
		{red, red, red, red},
		{red, red, red, red},
		{red, red, red, red},
		{red, red, red, red}
	};
	glBindTexture(GL_TEXTURE_2D, texture);
	EXPECT_NO_GL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) tex_data[0]);
	EXPECT_NO_GL_ERROR();
	// Set up Pixel Buffer Object
	GLuint pixelBuffer = 0;
	glGenBuffers(1, &pixelBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBuffer);
	EXPECT_NO_GL_ERROR();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	EXPECT_NO_GL_ERROR();
	GLuint pixel_data[4][4] = {
		{blue, blue, green, green},
		{blue, blue, green, green},
		{blue, blue, green, green},
		{blue, blue, green, green},
	};
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(pixel_data), (void *) pixel_data, GL_STREAM_DRAW);
	// Should set the 2-rightmost columns of the currently bound texture to the
	// 2-rightmost columns of the PBO;
	GLintptr offset = 2 * sizeof(GLuint);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 2, 0, 2, 4, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(offset));
	EXPECT_NO_GL_ERROR();
	// Create an off-screen framebuffer to render the texture data to.
	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	EXPECT_NO_GL_ERROR();
	unsigned int color[4][4] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};
	glReadPixels(0, 0, 4, 4, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	EXPECT_NO_GL_ERROR();
	bool allEqual = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			allEqual = allEqual && (color[i][j] == tex_data[i][j]);
			allEqual = allEqual && (color[i][j+2] == pixel_data[i][j+2]);
			if (!allEqual)
				break;
		}
		if (!allEqual)
			break;
	}
	EXPECT_EQ(allEqual, true);
	// We can't use an offset of 3 GLuints or more, because the PBO is not large
	// enough to satisfy such a request with the current GL_UNPACK_ROW_LENGTH.
	offset = 3 * sizeof(GLuint);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 2, 0, 2, 4, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void *>(offset));
	GLenum error = glGetError();
	EXPECT_GLENUM_EQ(GL_INVALID_OPERATION, error);
	Uninitialize();
}

// Tests reading of half-float textures.
TEST_F(SwiftShaderTest, ReadHalfFloat)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 256, 256, 0, GL_RGB, GL_HALF_FLOAT, nullptr);
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	EXPECT_NO_GL_ERROR();
	EXPECT_GLENUM_EQ(GL_FRAMEBUFFER_COMPLETE, glCheckFramebufferStatus(GL_FRAMEBUFFER));

	const float clear_color[4] = { 1.0f, 32.0f, 0.5f, 1.0f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	uint16_t pixel[3] = { 0x1234, 0x3F80, 0xAAAA };
	GLint x = 6;
	GLint y = 3;
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_HALF_FLOAT, pixel);

	// This relies on GL_HALF_FLOAT being a valid type for read-back,
	// which isn't guaranteed by the spec but is supported by SwiftShader.
	uint16_t read_color[3] = { 0, 0, 0 };
	glReadPixels(x, y, 1, 1, GL_RGB, GL_HALF_FLOAT, &read_color);
	EXPECT_NO_GL_ERROR();
	EXPECT_EQ(read_color[0], pixel[0]);
	EXPECT_EQ(read_color[1], pixel[1]);
	EXPECT_EQ(read_color[2], pixel[2]);

	Uninitialize();
}

// Tests construction of a structure containing a single matrix
TEST_F(SwiftShaderTest, MatrixInStruct)
{
	Initialize(2, false);

	const std::string fs =
		R"(#version 100
		precision mediump float;
		struct S
		{
			mat2 rotation;
		};
		void main(void)
		{
			float angle = 1.0;
			S(mat2(1.0, angle, 1.0, 1.0));
		})";

	MakeShader(fs, GL_FRAGMENT_SHADER);
	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test sampling from a sampler in a struct as a function argument
TEST_F(SwiftShaderTest, SamplerArrayInStructArrayAsFunctionArg)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_2D, tex);
	EXPECT_NO_GL_ERROR();

	unsigned char green[4] = { 0, 255, 0, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
	EXPECT_NO_GL_ERROR();

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		void main()
		{
			gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		struct SamplerStruct{ sampler2D tex[2]; };
		vec4 doSample(in SamplerStruct s[2])
		{
			return texture(s[1].tex[1], vec2(0.0));
		}
		uniform SamplerStruct samplerStruct[2];
		out vec4 fragColor;
		void main()
		{
			fragColor = doSample(samplerStruct);
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	glUseProgram(ph.program);
	GLint location = glGetUniformLocation(ph.program, "samplerStruct[1].tex[1]");
	ASSERT_NE(-1, location);
	glUniform1i(location, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program, "samplerStruct[1].tex[1]");

	deleteProgram(ph);

	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test sampling from a sampler in a struct as a function argument
TEST_F(SwiftShaderTest, AtanCornerCases)
{
	Initialize(3, false);

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		void main()
		{
			gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		precision mediump float;
		const float kPI = 3.14159265358979323846;
		uniform float positive_value;
		uniform float negative_value;
		out vec4 fragColor;
		void main()
		{
			// Should yield vec4(0, pi, pi/2, -pi/2)
			vec4 result = atan(vec4(0.0, 0.0, positive_value, negative_value),
			                   vec4(positive_value, negative_value, 0.0, 0.0));
			fragColor = (result / vec4(kPI)) + vec4(0.5, -0.5, 0.0, 1.0) + vec4(0.5 / 255.0);
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	glUseProgram(ph.program);
	GLint positive_value = glGetUniformLocation(ph.program, "positive_value");
	ASSERT_NE(-1, positive_value);
	GLint negative_value = glGetUniformLocation(ph.program, "negative_value");
	ASSERT_NE(-1, negative_value);

	float value = 1.0f;
	glUniform1fv(positive_value, 1, &value);
	value = -1.0f;
	glUniform1fv(negative_value, 1, &value);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program, nullptr);

	deleteProgram(ph);

	unsigned char grey[4] = { 128, 128, 128, 128 };
	expectFramebufferColor(grey);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

TEST_F(SwiftShaderTest, TransformFeedback_DrawArraysInstanced)
{
	Initialize(3, false);

	std::string fs =
		R"(#version 300 es
		in mediump vec2 vary;
		out mediump vec4 color;
		void main()
		{
			color = vec4(vary, 0.0, 1.0);
		})";
	std::string vs =
		R"(#version 300 es
		layout(location=0) in mediump vec2 pos;
		out mediump vec2 vary;
		void main()
		{
			vary = pos;
			gl_Position = vec4(pos, 0.0, 1.0);
		})";

	GLuint vert = MakeShader(vs, GL_VERTEX_SHADER);
	GLuint frag = MakeShader(fs, GL_FRAGMENT_SHADER);
	GLuint program = MakeProgram(vert, frag);
	LinkProgram(program);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArraysInstanced(GL_POINTS, 0, 1, 1);

	Uninitialize();
}

TEST_F(SwiftShaderTest, TransformFeedback_BadViewport)
{
	Initialize(3, false);

	GLuint tfBuffer;
	glGenBuffers(1, &tfBuffer);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 1 << 12, nullptr, GL_STATIC_DRAW);

	std::string vsSource =
		R"(#version 300 es
		in vec4 a_position;
		void main()
		{
			gl_Position = a_position;
		})";
	std::string fsSource =
		R"(#version 300 es
		precision highp float;
		out vec4 my_FragColor;
		void main()
		{
			my_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		})";

	const char *varyings[] = { "gl_Position" };

	GLuint vs = MakeShader(vsSource, GL_VERTEX_SHADER);
	GLuint fs = MakeShader(fsSource, GL_FRAGMENT_SHADER);
	GLuint program = MakeProgram(vs, fs);

	glTransformFeedbackVaryings(program, 1,
			&varyings[0], GL_INTERLEAVED_ATTRIBS);
	LinkProgram(program);
	glUseProgram(program);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer);
	glBeginTransformFeedback(GL_TRIANGLES);

	GLuint primitivesWrittenQuery = 0;
	glGenQueries(1, &primitivesWrittenQuery);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, primitivesWrittenQuery);

	glViewport(0, 10000000, 300, 300);

	GLint positionLocation = glGetAttribLocation(program, "a_position");
	GLfloat quadVertices[] = {
		-1.0f,  1.0f, 0.5f,
		-1.0f, -1.0f, 0.5f,
		 1.0f, -1.0f, 0.5f,
		-1.0f,  1.0f, 0.5f,
		 1.0f, -1.0f, 0.5f,
		 1.0f,  1.0f, 0.5f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, &quadVertices[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glEndTransformFeedback();

	GLuint primitivesWritten = 0;
	glGetQueryObjectuiv(primitivesWrittenQuery, GL_QUERY_RESULT_EXT, &primitivesWritten);
	EXPECT_NO_GL_ERROR();

	EXPECT_EQ(2u, primitivesWritten);

	Uninitialize();
}

// Test conditions that should result in a GL_OUT_OF_MEMORY and not crash
TEST_F(SwiftShaderTest, OutOfMemory)
{
	// Image sizes are assumed to fit in a 32-bit signed integer by the renderer,
	// so test that we can't create a 2+ GiB image.
	{
		Initialize(3, false);

		GLuint tex = 1;
		glBindTexture(GL_TEXTURE_3D, tex);

		const int width = 0xC2;
		const int height = 0x541;
		const int depth = 0x404;
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, nullptr);
		EXPECT_GLENUM_EQ(GL_OUT_OF_MEMORY, glGetError());

		// The spec states that the GL is in an undefined state when GL_OUT_OF_MEMORY
		// is returned, and the context must be recreated before attempting more rendering.
		Uninitialize();
	}
}

TEST_F(SwiftShaderTest, ViewportBounds)
{
	auto doRenderWithViewportSettings = [&](GLint x, GLint y, GLsizei w, GLsizei h)
	{
		Initialize(3, false);

		std::string vs =
			R"(#version 300 es
			in vec4 position;
			out float unfoldable;
			void main()
			{
			    unfoldable = position.x;
			    gl_Position = vec4(position.xy, 0.0, 1.0);
			})";

		std::string fs =
			R"(#version 300 es
			precision mediump float;
			in float unfoldable;
			out vec4 fragColor;
			void main()
			{
			    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
			})";

		const ProgramHandles ph = createProgram(vs, fs);

		glUseProgram(ph.program);

		glViewport(x, y, w, h);

		drawQuad(ph.program);
		EXPECT_NO_GL_ERROR();

		deleteProgram(ph);
		Uninitialize();
	};

	GLsizei w = 100;
	GLsizei h = 100;
	GLint minPos = -2000;

	doRenderWithViewportSettings(0, 0, 0, 0);
	doRenderWithViewportSettings(0, 0, w, h);

	// Negative positions
	doRenderWithViewportSettings(minPos, 0, w, h);
	doRenderWithViewportSettings(0, minPos, w, h);
	doRenderWithViewportSettings(minPos, minPos, w, h);
}

// Test using TexImage2D to define a rectangle texture

TEST_F(SwiftShaderTest, TextureRectangle_TexImage2D)
{
	Initialize(2, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

	// Defining level 0 is allowed
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	EXPECT_NO_GL_ERROR();

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
		EXPECT_NO_GL_ERROR();
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
		EXPECT_NO_GL_ERROR();
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
	EXPECT_NO_GL_ERROR();
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	// Wrap T
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	EXPECT_NO_GL_ERROR();
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	// Min filter has to be nearest or linear
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	EXPECT_NO_GL_ERROR();
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	EXPECT_NO_GL_ERROR();
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
	EXPECT_NO_GL_ERROR();
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
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Using level 0 of a rectangle texture is valid.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);
	EXPECT_GLENUM_EQ(GL_FRAMEBUFFER_COMPLETE, glCheckFramebufferStatus(GL_FRAMEBUFFER));
	EXPECT_NO_GL_ERROR();

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
	EXPECT_NO_GL_ERROR();

	unsigned char green[4] = { 0, 255, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
	EXPECT_NO_GL_ERROR();

	const std::string vs =
		R"(attribute vec4 position;
		void main()
		{
		    gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#extension GL_ARB_texture_rectangle : require
		precision mediump float;
		uniform sampler2DRect tex;
		void main()
		{
		    gl_FragColor = texture2DRect(tex, vec2(0, 0));
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	glUseProgram(ph.program);
	GLint location = glGetUniformLocation(ph.program, "tex");
	ASSERT_NE(-1, location);
	glUniform1i(location, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program, "tex");

	deleteProgram(ph);

	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

// Test sampling from a rectangle texture
TEST_F(SwiftShaderTest, TextureRectangle_SamplingFromRectangleESSL3)
{
	Initialize(3, false);

	GLuint tex = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
	EXPECT_NO_GL_ERROR();

	unsigned char green[4] = { 0, 255, 0, 255 };
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
	EXPECT_NO_GL_ERROR();

	const std::string vs =
		R"(#version 300 es
		in vec4 position;
		void main()
		{
		    gl_Position = vec4(position.xy, 0.0, 1.0);
		})";

	const std::string fs =
		R"(#version 300 es
		#extension GL_ARB_texture_rectangle : require
		precision mediump float;
		uniform sampler2DRect tex;
		out vec4 fragColor;
		void main()
		{
		    fragColor = texture(tex, vec2(0, 0));
		})";

	const ProgramHandles ph = createProgram(vs, fs);

	glUseProgram(ph.program);
	GLint location = glGetUniformLocation(ph.program, "tex");
	ASSERT_NE(-1, location);
	glUniform1i(location, 0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	EXPECT_NO_GL_ERROR();

	drawQuad(ph.program, "tex");

	deleteProgram(ph);

	expectFramebufferColor(green);

	EXPECT_NO_GL_ERROR();

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
	EXPECT_NO_GL_ERROR();

	// Clearing a texture is just as good as checking we can render to it, right?
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);
	EXPECT_NO_GL_ERROR();

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
	EXPECT_NO_GL_ERROR();

	// Error case: level != 0
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 1, GL_RGBA8, 0, 0, 1, 1, 0);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	// level = 0 works and defines the texture.
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, 0, 0, 1, 1, 0);
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);
	EXPECT_NO_GL_ERROR();

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
	EXPECT_NO_GL_ERROR();

	// Error case: level != 0
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 1, 0, 0, 0, 0, 1, 1);
	EXPECT_GLENUM_EQ(GL_INVALID_VALUE, glGetError());

	// level = 0 works and defines the texture.
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, 0, 0, 1, 1);
	EXPECT_NO_GL_ERROR();

	GLuint fbo = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, tex, 0);

	unsigned char green[4] = { 0, 255, 0, 255 };
	expectFramebufferColor(green);
	EXPECT_NO_GL_ERROR();

	Uninitialize();
}

TEST_F(SwiftShaderTest, BlitTest)
{
	Initialize(3, false);

	GLuint fbos[] = {0, 0};
	glGenFramebuffers(2, fbos);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]);

	GLuint textures[] = {0, 0};
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	unsigned char red[4][4] = {
		{255, 0, 0, 255},
		{255, 0, 0, 255},
		{255, 0, 0, 255},
		{255, 0, 0, 255}
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, red);
	EXPECT_NO_GL_ERROR();

	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
	EXPECT_NO_GL_ERROR();

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	unsigned char black[4][4] = {
		{0, 0, 0, 255},
		{0, 0, 0, 255},
		{0, 0, 0, 255},
		{0, 0, 0, 255}
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);
	EXPECT_NO_GL_ERROR();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[1], 0);

	// Test that glBlitFramebuffer works as expected for the normal case.
	glBlitFramebuffer(0, 0, 1, 1, 0, 0, 1, 1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	EXPECT_NO_GL_ERROR();
	EXPECT_EQ(red[0][1], black[0][1]);

	// Check that glBlitFramebuffer doesn't crash with ugly input.
	const int big = (int) 2e9;
	const int small = 200;
	const int neg_small = -small;
	const int neg_big = -big;
	int max = 0x7fffffff;
	int data[][8] = {
		// sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1
		{0, 0, 0, 0, 0, 0, 0, 0},
		{-1, -1, -1, -1, -1, -1, -1, -1},
		{1, 1, 1, 1, 1, 1, 1, 1},
		{-1, -1, 1, 1, -1, -1, 1, 1},
		{0, 0, 127, (int) 2e9, 10, 10, 200, 200},
		{-2, -2, 127, 2147483470, 10, 10, 200, 200},
		{big, small, small, big, big, big, small, small},
		{neg_small, small, neg_small, neg_small, neg_small, big, small},
		{big, big-1, big-2, big-3, big-4, big-5, big-6, big-7},
		{big, neg_big, neg_big, big, small, big, 0, neg_small},
		{323479648, 21931, 1769809195, 32733, 0, 0, -161640504, 32766},
		{0, 0, max, max, 0, 0, 8, 8},
		{0, 0, 8, 8, 0, 0, max, max},
		{0, 0, max, max, 0, 0, max, max},
		{-1, -1, max, max, 0, 0, 8, 8},
		{0, 0, 8, 8, -1, -1, max, max},
		{-1, -1, max, max, -1, -1, max, max},
		{-max-1, -max-1, max, max, -max-1, -max-1, max, max}
	};

	for (int i = 0; i < (int) (sizeof(data)/sizeof(data[0])); i++)
	{
		glBlitFramebuffer(
				data[i][0], data[i][1], data[i][2], data[i][3],
				data[i][4], data[i][5], data[i][6], data[i][7],
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		// Ignore error state, just make sure that we don't crash on these inputs.
	}

	// Clear the error state before uninitializing test.
	glGetError();

	glDeleteFramebuffers(2, fbos);
	glDeleteTextures(2, textures);
	Uninitialize();
}

TEST_F(SwiftShaderTest, InvalidEnum_TexImage2D)
{
	Initialize(3, false);

	const GLenum invalidTarget = GL_TEXTURE_3D;

	glTexImage2D(invalidTarget, 0, GL_R11F_G11F_B10F, 256, 256, 0, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, nullptr);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	float pixels[3] = { 0.0f, 0.0f, 0.0f };
	glTexSubImage2D(invalidTarget, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, pixels);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	glCopyTexImage2D(invalidTarget, 0, GL_RGB, 2, 6, 8, 8, 0);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	glCopyTexSubImage2D(invalidTarget, 0, 0, 0, 0, 0, 1, 1);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	const char data[128] = { 0 };
	glCompressedTexImage2D(invalidTarget, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 16, 16, 0, 128, data);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	glCompressedTexSubImage2D(invalidTarget, 0, 0, 0, 0, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 0, 0);
	EXPECT_GLENUM_EQ(GL_INVALID_ENUM, glGetError());

	Uninitialize();
}

TEST_F(SwiftShaderTest, CompilerLimits_DeepNestedIfs)
{
	std::string body = "return 1.0;";
	for (int i = 0; i < 16; i++)
	{
		body = "  if (f > " + std::to_string(i * 0.1f) + ") {\n" + body + "}\n";
	}

	checkCompiles(
		"float F(float f) {\n" + body + "  return 0.0f;\n}\n"
	);
}

TEST_F(SwiftShaderTest, CompilerLimits_DeepNestedSwitches)
{
	std::string body = "return 1.0;";
	for (int i = 0; i < 16; i++)
	{
		body = "  switch (int(f)) {\n case 1:\n  f *= 2.0;\n" + body + "}\n";
	}

	checkCompiles("float F(float f) {\n" + body + "  return 0.0f;\n}\n");
}

TEST_F(SwiftShaderTest, CompilerLimits_DeepNestedLoops)
{
	std::string loops = "f = f + f * 2.0;";
	for (int i = 0; i < 16; i++)
	{
		auto it = "l" + std::to_string(i);
		loops = "  for (int " + it + " = 0; " + it + " < i; " + it + "++) {\n" + loops + "}\n";
	}

	checkCompiles(
		"float F(float f) {\n"
		"  int i = (f > 0.0) ? 1 : 0;\n" + loops +
		"  return f;\n"
		"}\n"
	);
}

TEST_F(SwiftShaderTest, CompilerLimits_DeepNestedCalls)
{
	std::string funcs = "float E(float f) { return f * 2.0f; }\n";
	std::string last = "E";
	for (int i = 0; i < 16; i++)
	{
		std::string f = "C" + std::to_string(i);
		funcs += "float " + f + "(float f) { return " + last + "(f) + 1.0f; }\n";
		last = f;
	}

	checkCompiles(funcs +
		"float F(float f) { return " + last + "(f); }\n"
	);
}

TEST_F(SwiftShaderTest, CompilerLimits_ManyCallSites)
{
	std::string calls;
	for (int i = 0; i < 256; i++)
	{
		calls += "  f += C(f);\n";
	}

	checkCompiles(
		"float C(float f) { return f * 2.0f; }\n"
		"float F(float f) {\n" + calls + "  return f;\n}\n"
	);
}

TEST_F(SwiftShaderTest, CompilerLimits_DeepNestedCallsInUnusedFunction)
{
	std::string funcs = "float E(float f) { return f * 2.0f; }\n";
	std::string last = "E";
	for (int i = 0; i < 16; i++)
	{
		std::string f = "C" + std::to_string(i);
		funcs += "float " + f + "(float f) { return " + last + "(f) + 1.0f; }\n";
		last = f;
	}

	checkCompiles(funcs +
		"float F(float f) { return f; }\n"
	);
}

// Test that the compiler correctly handles functions being stripped.
// The frontend will strip the Dead functions, but may keep the their function
// labels reserved. This produces labels that are greater than the number of
// live functions.
TEST_F(SwiftShaderTest, CompilerLimits_SparseLabels)
{
	checkCompiles(
		R"(void Dead1() {}
		void Dead2() {}
		void Dead3() {}
		void Dead4() {}
		void Dead5() { Dead1(); Dead2(); Dead3(); Dead4(); }
		float F(float f) { for(int i = 0; i < -1; ++i) { Dead5(); } return f; })"
	);
}

// Test that the compiler doesn't compile arrays larger than
// GL_MAX_{VERTEX/FRAGMENT}_UNIFORM_VECTOR.
TEST_F(SwiftShaderTest, CompilerLimits_ArraySize)
{
	checkCompileFails(
		R"(uniform float u_var[100000000];
		float F(float f) { return u_var[2]; })");
	checkCompileFails(
		R"(struct structType { mediump sampler2D m0; mediump samplerCube m1; };
		uniform structType u_var[100000000];
		float F(float f) { return texture(u_var[2].m1, vec3(0.0)), vec4(0.26, 1.72, 0.60, 0.12).x; })");
}

// Test that the compiler rejects negations of things that can't be negated.
TEST_F(SwiftShaderTest, BadNegation)
{
	checkCompileFails(
		R"(uniform samplerCube m;
		float F (float f) { vec4 ret = texture(-m, vec3(f)); return ret.x; })"
	);
	checkCompileFails(
		R"(uniform sampler2D m[9];
		vec4 G (sampler2D X[9]) { return texture(X[0], vec2(0.0f)); }
		float F (float f) { vec4 ret = G(-m); return ret.x; })"
	);
	checkCompileFails(
		R"(struct structType { int a; float b; };
		uniform structType m;
		float F (float f) { structType n = -m; return f; })"
	);
	checkCompileFails(
		R"(struct structType { int a; float b; };
		uniform structType m[4];
		float F (float f) { structType n[4] = -m; return f; })"
	);
	checkCompileFails(
		R"(uniform float m[4];
		float G (float f[4]) { return f[0]; }
		float F (float f) { return G(-m); })"
	);
}

#ifndef EGL_ANGLE_iosurface_client_buffer
#define EGL_ANGLE_iosurface_client_buffer 1
#define EGL_IOSURFACE_ANGLE 0x3454
#define EGL_IOSURFACE_PLANE_ANGLE 0x345A
#define EGL_TEXTURE_RECTANGLE_ANGLE 0x345B
#define EGL_TEXTURE_TYPE_ANGLE 0x345C
#define EGL_TEXTURE_INTERNAL_FORMAT_ANGLE 0x345D
#endif /* EGL_ANGLE_iosurface_client_buffer */

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <IOSurface/IOSurface.h>

namespace
{
	void AddIntegerValue(CFMutableDictionaryRef dictionary, const CFStringRef key, int32_t value)
	{
		CFNumberRef number = CFNumberCreate(nullptr, kCFNumberSInt32Type, &value);
		CFDictionaryAddValue(dictionary, key, number);
		CFRelease(number);
	}
}  // anonymous namespace

class EGLClientBufferWrapper
{
public:
	EGLClientBufferWrapper(int width = 1, int height = 1)
	{
		// Create a 1 by 1 BGRA8888 IOSurface
		ioSurface = nullptr;

		CFMutableDictionaryRef dict = CFDictionaryCreateMutable(
			kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		AddIntegerValue(dict, kIOSurfaceWidth, width);
		AddIntegerValue(dict, kIOSurfaceHeight, height);
		AddIntegerValue(dict, kIOSurfacePixelFormat, 'BGRA');
		AddIntegerValue(dict, kIOSurfaceBytesPerElement, 4);

		ioSurface = IOSurfaceCreate(dict);
		CFRelease(dict);

		EXPECT_NE(nullptr, ioSurface);
	}

	~EGLClientBufferWrapper()
	{
		IOSurfaceUnlock(ioSurface, kIOSurfaceLockReadOnly, nullptr);

		CFRelease(ioSurface);
	}

	EGLClientBuffer getClientBuffer() const
	{
		return ioSurface;
	}

	const unsigned char* lockColor()
	{
		IOSurfaceLock(ioSurface, kIOSurfaceLockReadOnly, nullptr);
		return reinterpret_cast<const unsigned char*>(IOSurfaceGetBaseAddress(ioSurface));
	}

	void unlockColor()
	{
		IOSurfaceUnlock(ioSurface, kIOSurfaceLockReadOnly, nullptr);
	}

	void writeColor(void* data, size_t dataSize)
	{
		// Write the data to the IOSurface
		IOSurfaceLock(ioSurface, 0, nullptr);
		memcpy(IOSurfaceGetBaseAddress(ioSurface), data, dataSize);
		IOSurfaceUnlock(ioSurface, 0, nullptr);
	}
private:
	IOSurfaceRef ioSurface;
};

#else // __APPLE__

class EGLClientBufferWrapper
{
public:
	EGLClientBufferWrapper(int width = 1, int height = 1)
	{
		clientBuffer = new unsigned char[4 * width * height];
	}

	~EGLClientBufferWrapper()
	{
		delete[] clientBuffer;
	}

	EGLClientBuffer getClientBuffer() const
	{
		return clientBuffer;
	}

	const unsigned char* lockColor()
	{
		return clientBuffer;
	}

	void unlockColor()
	{
	}

	void writeColor(void* data, size_t dataSize)
	{
		memcpy(clientBuffer, data, dataSize);
	}
private:
	unsigned char* clientBuffer;
};

#endif

class IOSurfaceClientBufferTest : public SwiftShaderTest
{
protected:
	EGLSurface createIOSurfacePbuffer(EGLClientBuffer buffer, EGLint width, EGLint height, EGLint plane, GLenum internalFormat, GLenum type) const
	{
		// Make a PBuffer from it using the EGL_ANGLE_iosurface_client_buffer extension
		const EGLint attribs[] = {
			EGL_WIDTH,                         width,
			EGL_HEIGHT,                        height,
			EGL_IOSURFACE_PLANE_ANGLE,         plane,
			EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
			EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, (EGLint)internalFormat,
			EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
			EGL_TEXTURE_TYPE_ANGLE,            (EGLint)type,
			EGL_NONE,                          EGL_NONE,
		};

		EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, buffer, getConfig(), attribs);
		EXPECT_NE(EGL_NO_SURFACE, pbuffer);
		return pbuffer;
	}

	void bindIOSurfaceToTexture(EGLClientBuffer buffer, EGLint width, EGLint height, EGLint plane, GLenum internalFormat, GLenum type, EGLSurface *pbuffer, GLuint *texture) const
	{
		*pbuffer = createIOSurfacePbuffer(buffer, width, height, plane, internalFormat, type);

		// Bind the pbuffer
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, *texture);
		EGLBoolean result = eglBindTexImage(getDisplay(), *pbuffer, EGL_BACK_BUFFER);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
		EXPECT_NO_EGL_ERROR();
	}

	void doClear(GLenum internalFormat, bool clearToZero)
	{
		if(internalFormat == GL_R16UI)
		{
			GLuint color = clearToZero ? 0 : 257;
			glClearBufferuiv(GL_COLOR, 0, &color);
			EXPECT_NO_GL_ERROR();
		}
		else
		{
			glClearColor(clearToZero ? 0.0f : 1.0f / 255.0f,
				clearToZero ? 0.0f : 2.0f / 255.0f,
				clearToZero ? 0.0f : 3.0f / 255.0f,
				clearToZero ? 0.0f : 4.0f / 255.0f);
			EXPECT_NO_GL_ERROR();
			glClear(GL_COLOR_BUFFER_BIT);
			EXPECT_NO_GL_ERROR();
		}
	}

	void doClearTest(EGLClientBufferWrapper& clientBufferWrapper, GLenum internalFormat, GLenum type, void *data, size_t dataSize)
	{
		ASSERT_TRUE(dataSize <= 4);

		// Bind the IOSurface to a texture and clear it.
		GLuint texture = 1;
		EGLSurface pbuffer;
		bindIOSurfaceToTexture(clientBufferWrapper.getClientBuffer(), 1, 1, 0, internalFormat, type, &pbuffer, &texture);

		// glClear the pbuffer
		GLuint fbo = 2;
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		EXPECT_NO_GL_ERROR();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, texture, 0);
		EXPECT_NO_GL_ERROR();
		EXPECT_GLENUM_EQ(glCheckFramebufferStatus(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);
		EXPECT_NO_GL_ERROR();

		doClear(internalFormat, false);

		// Unbind pbuffer and check content.
		EGLBoolean result = eglReleaseTexImage(getDisplay(), pbuffer, EGL_BACK_BUFFER);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
		EXPECT_NO_EGL_ERROR();

		const unsigned char* color = clientBufferWrapper.lockColor();
		for(size_t i = 0; i < dataSize; ++i)
		{
			EXPECT_EQ(color[i], reinterpret_cast<unsigned char*>(data)[i]);
		}

		result = eglDestroySurface(getDisplay(), pbuffer);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
		EXPECT_NO_EGL_ERROR();
	}

	void doSampleTest(EGLClientBufferWrapper& clientBufferWrapper, GLenum internalFormat, GLenum type, void *data, size_t dataSize)
	{
		ASSERT_TRUE(dataSize <= 4);

		clientBufferWrapper.writeColor(data, dataSize);

		// Bind the IOSurface to a texture and clear it.
		GLuint texture = 1;
		EGLSurface pbuffer;
		bindIOSurfaceToTexture(clientBufferWrapper.getClientBuffer(), 1, 1, 0, internalFormat, type, &pbuffer, &texture);

		doClear(internalFormat, true);

		// Create program and draw quad using it
		const std::string vs =
			R"(attribute vec4 position;
			void main()
			{
			    gl_Position = vec4(position.xy, 0.0, 1.0);
			})";

		const std::string fs =
			R"(#extension GL_ARB_texture_rectangle : require
			precision mediump float;
			uniform sampler2DRect tex;
			void main()
			{
			    gl_FragColor = texture2DRect(tex, vec2(0, 0));
			})";

		const ProgramHandles ph = createProgram(vs, fs);

		drawQuad(ph.program, "tex");

		deleteProgram(ph);
		EXPECT_NO_GL_ERROR();

		// Unbind pbuffer and check content.
		EGLBoolean result = eglReleaseTexImage(getDisplay(), pbuffer, EGL_BACK_BUFFER);
		EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
		EXPECT_NO_EGL_ERROR();

		const unsigned char* color = clientBufferWrapper.lockColor();
		for(size_t i = 0; i < dataSize; ++i)
		{
			EXPECT_EQ(color[i], reinterpret_cast<unsigned char*>(data)[i]);
		}
		clientBufferWrapper.unlockColor();
	}
};

// Tests for the EGL_ANGLE_iosurface_client_buffer extension
TEST_F(IOSurfaceClientBufferTest, RenderToBGRA8888IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[4] = { 3, 2, 1, 4 };
		doClearTest(clientBufferWrapper, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data, 4);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test reading from BGRA8888 IOSurfaces
TEST_F(IOSurfaceClientBufferTest, ReadFromBGRA8888IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[4] = { 3, 2, 1, 4 };
		doSampleTest(clientBufferWrapper, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data, 4);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test using RGBX8888 IOSurfaces for rendering
TEST_F(IOSurfaceClientBufferTest, RenderToRGBX8888IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[3] = { 1, 2, 3 };
		doClearTest(clientBufferWrapper, GL_RGB, GL_UNSIGNED_BYTE, data, 3);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test reading from RGBX8888 IOSurfaces
TEST_F(IOSurfaceClientBufferTest, ReadFromRGBX8888IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[3] = { 1, 2, 3 };
		doSampleTest(clientBufferWrapper, GL_RGB, GL_UNSIGNED_BYTE, data, 3);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test using RG88 IOSurfaces for rendering
TEST_F(IOSurfaceClientBufferTest, RenderToRG88IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[2] = { 1, 2 };
		doClearTest(clientBufferWrapper, GL_RG, GL_UNSIGNED_BYTE, data, 2);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test reading from RG88 IOSurfaces
TEST_F(IOSurfaceClientBufferTest, ReadFromRG88IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[2] = { 1, 2 };
		doSampleTest(clientBufferWrapper, GL_RG, GL_UNSIGNED_BYTE, data, 2);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test using R8 IOSurfaces for rendering
TEST_F(IOSurfaceClientBufferTest, RenderToR8IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[1] = { 1 };
		doClearTest(clientBufferWrapper, GL_RED, GL_UNSIGNED_BYTE, data, 1);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test reading from R8 IOSurfaces
TEST_F(IOSurfaceClientBufferTest, ReadFromR8IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		unsigned char data[1] = { 1 };
		doSampleTest(clientBufferWrapper, GL_RED, GL_UNSIGNED_BYTE, data, 1);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test using R16 IOSurfaces for rendering
TEST_F(IOSurfaceClientBufferTest, RenderToR16IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		uint16_t data[1] = { 257 };
		doClearTest(clientBufferWrapper, GL_R16UI, GL_UNSIGNED_SHORT, data, 2);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test reading from R8 IOSurfaces
TEST_F(IOSurfaceClientBufferTest, ReadFromR16IOSurface)
{
	Initialize(3, false);

	{ // EGLClientBufferWrapper scope
		EGLClientBufferWrapper clientBufferWrapper;
		uint16_t data[1] = { 257 };
		doSampleTest(clientBufferWrapper, GL_R16UI, GL_UNSIGNED_SHORT, data, 1);
	} // end of EGLClientBufferWrapper scope

	Uninitialize();
}

// Test the validation errors for missing attributes for eglCreatePbufferFromClientBuffer with
// IOSurface
TEST_F(IOSurfaceClientBufferTest, NegativeValidationMissingAttributes)
{
	Initialize(3, false);

	{
		EGLClientBufferWrapper clientBufferWrapper(10, 10);

		// Success case
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_NE(EGL_NO_SURFACE, pbuffer);

			EGLBoolean result = eglDestroySurface(getDisplay(), pbuffer);
			EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
			EXPECT_NO_EGL_ERROR();
		}

		// Missing EGL_WIDTH
		{
			const EGLint attribs[] = {
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_PARAMETER, eglGetError());
		}

		// Missing EGL_HEIGHT
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_PARAMETER, eglGetError());
		}

		// Missing EGL_IOSURFACE_PLANE_ANGLE
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_PARAMETER, eglGetError());
		}

		// Missing EGL_TEXTURE_TARGET - EGL_BAD_MATCH from the base spec of
		// eglCreatePbufferFromClientBuffer
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_MATCH, eglGetError());
		}

		// Missing EGL_TEXTURE_INTERNAL_FORMAT_ANGLE
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_PARAMETER, eglGetError());
		}

		// Missing EGL_TEXTURE_FORMAT - EGL_BAD_MATCH from the base spec of
		// eglCreatePbufferFromClientBuffer
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_MATCH, eglGetError());
		}

		// Missing EGL_TEXTURE_TYPE_ANGLE
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_PARAMETER, eglGetError());
		}
	}

	Uninitialize();
}

// Test the validation errors for bad parameters for eglCreatePbufferFromClientBuffer with IOSurface
TEST_F(IOSurfaceClientBufferTest, NegativeValidationBadAttributes)
{
	Initialize(3, false);

	{
		EGLClientBufferWrapper clientBufferWrapper(10, 10);

		// Success case
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_NE(EGL_NO_SURFACE, pbuffer);

			EGLBoolean result = eglDestroySurface(getDisplay(), pbuffer);
			EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
			EXPECT_NO_EGL_ERROR();
		}

		// EGL_TEXTURE_FORMAT must be EGL_TEXTURE_RGBA
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGB,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// EGL_WIDTH must be at least 1
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         0,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// EGL_HEIGHT must be at least 1
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        0,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

#if defined(__APPLE__)
		// EGL_WIDTH must be at most the width of the IOSurface
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         11,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// EGL_HEIGHT must be at most the height of the IOSurface
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        11,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// EGL_IOSURFACE_PLANE_ANGLE must less than the number of planes of the IOSurface
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         1,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}
#endif

		// EGL_TEXTURE_FORMAT must be at EGL_TEXTURE_RECTANGLE_ANGLE
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_2D,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// EGL_IOSURFACE_PLANE_ANGLE must be at least 0
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         -1,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_BGRA_EXT,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}

		// The internal format / type most be listed in the table
		{
			const EGLint attribs[] = {
				EGL_WIDTH,                         10,
				EGL_HEIGHT,                        10,
				EGL_IOSURFACE_PLANE_ANGLE,         0,
				EGL_TEXTURE_TARGET,                EGL_TEXTURE_RECTANGLE_ANGLE,
				EGL_TEXTURE_INTERNAL_FORMAT_ANGLE, GL_RGBA,
				EGL_TEXTURE_FORMAT,                EGL_TEXTURE_RGBA,
				EGL_TEXTURE_TYPE_ANGLE,            GL_UNSIGNED_BYTE,
				EGL_NONE,                          EGL_NONE,
			};

			EGLSurface pbuffer = eglCreatePbufferFromClientBuffer(getDisplay(), EGL_IOSURFACE_ANGLE, clientBufferWrapper.getClientBuffer(), getConfig(), attribs);
			EXPECT_EQ(EGL_NO_SURFACE, pbuffer);
			EXPECT_EQ(EGL_BAD_ATTRIBUTE, eglGetError());
		}
	}

	Uninitialize();
}

// Test IOSurface pbuffers can be made current
TEST_F(IOSurfaceClientBufferTest, MakeCurrentAllowed)
{
	Initialize(3, false);

	{
		EGLClientBufferWrapper clientBufferWrapper(10, 10);

		EGLSurface pbuffer = createIOSurfacePbuffer(clientBufferWrapper.getClientBuffer(), 10, 10, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE);

		EGLBoolean result = eglMakeCurrent(getDisplay(), pbuffer, pbuffer, getContext());
		EXPECT_EQ((EGLBoolean)EGL_TRUE, result);
		EXPECT_NO_EGL_ERROR();
	}

	Uninitialize();
}

