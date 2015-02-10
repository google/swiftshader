//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef _COMPILER_INCLUDED_
#define _COMPILER_INCLUDED_

#include "ExtensionBehavior.h"
#include "InfoSink.h"
#include "SymbolTable.h"

//
// The names of the following enums have been derived by replacing GL prefix
// with SH. For example, SH_INFO_LOG_LENGTH is equivalent to GL_INFO_LOG_LENGTH.
// The enum values are also equal to the values of their GL counterpart. This
// is done to make it easier for applications to use the shader library.
//
enum ShShaderType
{
  SH_FRAGMENT_SHADER = 0x8B30,
  SH_VERTEX_SHADER   = 0x8B31
};

enum ShShaderSpec
{
  SH_GLES2_SPEC = 0x8B40,
  SH_WEBGL_SPEC = 0x8B41
};

 enum ShShaderInfo
{
  SH_INFO_LOG_LENGTH             =  0x8B84,
  SH_OBJECT_CODE_LENGTH          =  0x8B88,  // GL_SHADER_SOURCE_LENGTH
  SH_ACTIVE_UNIFORM_MAX_LENGTH   =  0x8B87,
  SH_ACTIVE_ATTRIBUTE_MAX_LENGTH =  0x8B8A
};

enum ShCompileOptions
{
  SH_VALIDATE                = 0,
  SH_VALIDATE_LOOP_INDEXING  = 0x0001,
  SH_INTERMEDIATE_TREE       = 0x0002,
  SH_OBJECT_CODE             = 0x0004,
  SH_ATTRIBUTES_UNIFORMS     = 0x0008,
  SH_LINE_DIRECTIVES         = 0x0010,
  SH_SOURCE_PATH             = 0x0020
};

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

//
// The base class for the machine dependent compiler to derive from
// for managing object code from the compile.
//
class TCompiler
{
public:
    TCompiler(ShShaderType type, ShShaderSpec spec);
    virtual ~TCompiler();
    virtual TCompiler* getAsCompiler() { return this; }

    bool Init(const ShBuiltInResources& resources);
    bool compile(const char* const shaderStrings[],
                 const int numStrings,
                 int compileOptions);

    // Get results of the last compilation.
    TInfoSink& getInfoSink() { return infoSink; }

protected:
    ShShaderType getShaderType() const { return shaderType; }
    ShShaderSpec getShaderSpec() const { return shaderSpec; }
    // Initialize symbol-table with built-in symbols.
    bool InitBuiltInSymbolTable(const ShBuiltInResources& resources);
    // Clears the results from the previous compilation.
    void clearResults();
    // Return true if function recursion is detected or call depth exceeded.
    bool validateCallDepth(TIntermNode *root, TInfoSink &infoSink);
    // Returns true if the given shader does not exceed the minimum
    // functionality mandated in GLSL 1.0 spec Appendix A.
    bool validateLimitations(TIntermNode *root);
    // Translate to object code.
    virtual bool translate(TIntermNode *root) = 0;
    // Get built-in extensions with default behavior.
    const TExtensionBehavior& getExtensionBehavior() const;

private:
    ShShaderType shaderType;
    ShShaderSpec shaderSpec;

    unsigned int maxCallStackDepth;

    // Built-in symbol table for the given language, spec, and resources.
    // It is preserved from compile-to-compile.
    TSymbolTable symbolTable;
    // Built-in extensions with default behavior.
    TExtensionBehavior extensionBehavior;

    // Results of compilation.
    TInfoSink infoSink;  // Output sink.

    // Memory allocator. Allocates and tracks memory required by the compiler.
    // Deallocates all memory when compiler is destructed.
    TPoolAllocator allocator;
};

bool InitCompilerGlobals();
void FreeCompilerGlobals();

#endif // _COMPILER_INCLUDED_
