//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef _COMPILER_INTERFACE_INCLUDED_
#define _COMPILER_INTERFACE_INCLUDED_

//
// This is the platform independent interface between an OGL driver
// and the shading language compiler.
//

#ifdef __cplusplus
extern "C" {
#endif

// Version number for shader translation API.
// It is incremented everytime the API changes.
#define SH_VERSION 105

//
// The names of the following enums have been derived by replacing GL prefix
// with SH. For example, SH_INFO_LOG_LENGTH is equivalent to GL_INFO_LOG_LENGTH.
// The enum values are also equal to the values of their GL counterpart. This
// is done to make it easier for applications to use the shader library.
//
typedef enum {
  SH_FRAGMENT_SHADER = 0x8B30,
  SH_VERTEX_SHADER   = 0x8B31
} ShShaderType;

typedef enum {
  SH_GLES2_SPEC = 0x8B40,
  SH_WEBGL_SPEC = 0x8B41
} ShShaderSpec;

typedef enum {
  SH_INFO_LOG_LENGTH             =  0x8B84,
  SH_OBJECT_CODE_LENGTH          =  0x8B88,  // GL_SHADER_SOURCE_LENGTH
  SH_ACTIVE_UNIFORM_MAX_LENGTH   =  0x8B87,
  SH_ACTIVE_ATTRIBUTE_MAX_LENGTH =  0x8B8A
} ShShaderInfo;

// Compile options.
typedef enum {
  SH_VALIDATE                = 0,
  SH_VALIDATE_LOOP_INDEXING  = 0x0001,
  SH_INTERMEDIATE_TREE       = 0x0002,
  SH_OBJECT_CODE             = 0x0004,
  SH_ATTRIBUTES_UNIFORMS     = 0x0008,
  SH_LINE_DIRECTIVES         = 0x0010,
  SH_SOURCE_PATH             = 0x0020
} ShCompileOptions;

//
// Driver must call this first, once, before doing any other
// compiler operations.
// If the function succeeds, the return value is nonzero, else zero.
//
int ShInitialize();
//
// Driver should call this at shutdown.
// If the function succeeds, the return value is nonzero, else zero.
//
int ShFinalize();

//
// Implementation dependent built-in resources (constants and extensions).
// The names for these resources has been obtained by stripping gl_/GL_.
//
struct ShBuiltInResources
{
	ShBuiltInResources();

    // Constants.
    int MaxVertexAttribs;
    int MaxVertexUniformVectors;
    int MaxVaryingVectors;
    int MaxVertexTextureImageUnits;
    int MaxCombinedTextureImageUnits;
    int MaxTextureImageUnits;
    int MaxFragmentUniformVectors;
    int MaxDrawBuffers;

    // Extensions.
    // Set to 1 to enable the extension, else 0.
    int OES_standard_derivatives;
	int OES_fragment_precision_high;
    int OES_EGL_image_external;

    unsigned int MaxCallStackDepth;
};

#ifdef __cplusplus
}
#endif

#endif // _COMPILER_INTERFACE_INCLUDED_
