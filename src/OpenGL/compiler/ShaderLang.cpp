//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

//
// Implement the top-level of interface to the compiler,
// as defined in ShaderLang.h
//

#include "GLSLANG/ShaderLang.h"

#include "InitializeDll.h"
#include "preprocessor/length_limits.h"
#include "Compiler.h"

#include <limits.h>

//
// Driver must call this first, once, before doing any other
// compiler operations.
//
int ShInitialize()
{
    return InitProcess() ? 1 : 0;
}

//
// Cleanup symbol tables
//
int ShFinalize()
{
    DetachProcess();
    return 1;
}

//
// Initialize built-in resources with minimum expected values.
//
void ShInitBuiltInResources(ShBuiltInResources* resources)
{
    // Constants.
    resources->MaxVertexAttribs = 8;
    resources->MaxVertexUniformVectors = 128;
    resources->MaxVaryingVectors = 8;
    resources->MaxVertexTextureImageUnits = 0;
    resources->MaxCombinedTextureImageUnits = 8;
    resources->MaxTextureImageUnits = 8;
    resources->MaxFragmentUniformVectors = 16;
    resources->MaxDrawBuffers = 1;

    // Extensions.
    resources->OES_standard_derivatives = 0;
	resources->OES_fragment_precision_high = 0;
    resources->OES_EGL_image_external = 0;

	resources->MaxCallStackDepth = UINT_MAX;
}

//
// Driver calls these to create and destroy compiler objects.
//
ShHandle ShConstructCompiler(ShShaderType type, ShShaderSpec spec,
                             const ShBuiltInResources* resources)
{
    TShHandleBase* base = static_cast<TShHandleBase*>(ConstructCompiler(type, spec));
    TCompiler* compiler = base->getAsCompiler();
    if (compiler == 0)
        return 0;

    // Generate built-in symbol table.
    if (!compiler->Init(*resources)) {
        ShDestruct(base);
        return 0;
    }

    return reinterpret_cast<void*>(base);
}

void ShDestruct(ShHandle handle)
{
    if (handle == 0)
        return;

    TShHandleBase* base = static_cast<TShHandleBase*>(handle);

    if (base->getAsCompiler())
        DeleteCompiler(base->getAsCompiler());
}

//
// Do an actual compile on the given strings.  The result is left 
// in the given compile object.
//
// Return:  The return value of ShCompile is really boolean, indicating
// success or failure.
//
int ShCompile(
    const ShHandle handle,
    const char* const shaderStrings[],
    const int numStrings,
    int compileOptions)
{
    if (handle == 0)
        return 0;

    TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (compiler == 0)
        return 0;

    bool success = compiler->compile(shaderStrings, numStrings, compileOptions);
    return success ? 1 : 0;
}

void ShGetInfo(const ShHandle handle, ShShaderInfo pname, int* params)
{
    if (!handle || !params)
        return;

    TShHandleBase* base = static_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (!compiler) return;

    switch(pname)
    {
    case SH_INFO_LOG_LENGTH:
        *params = compiler->getInfoSink().info.size() + 1;
        break;
    case SH_OBJECT_CODE_LENGTH:
        *params = compiler->getInfoSink().obj.size() + 1;
        break;
    case SH_ACTIVE_UNIFORM_MAX_LENGTH:
        *params = 1 +  MAX_SYMBOL_NAME_LEN;
        break;
    case SH_ACTIVE_ATTRIBUTE_MAX_LENGTH:
        *params = 1 + MAX_SYMBOL_NAME_LEN;
        break;
    default: UNREACHABLE();
    }
}

//
// Return any compiler log of messages for the application.
//
void ShGetInfoLog(const ShHandle handle, char* infoLog)
{
    if (!handle || !infoLog)
        return;

    TShHandleBase* base = static_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (!compiler) return;

    TInfoSink& infoSink = compiler->getInfoSink();
    strcpy(infoLog, infoSink.info.c_str());
}

//
// Return any object code.
//
void ShGetObjectCode(const ShHandle handle, char* objCode)
{
    if (!handle || !objCode)
        return;

    TShHandleBase* base = static_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (!compiler) return;

    TInfoSink& infoSink = compiler->getInfoSink();
    strcpy(objCode, infoSink.obj.c_str());
}
