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
ShBuiltInResources::ShBuiltInResources()
{
    // Constants.
    MaxVertexAttribs = 8;
    MaxVertexUniformVectors = 128;
    MaxVaryingVectors = 8;
    MaxVertexTextureImageUnits = 0;
    MaxCombinedTextureImageUnits = 8;
    MaxTextureImageUnits = 8;
    MaxFragmentUniformVectors = 16;
    MaxDrawBuffers = 1;

    // Extensions.
    OES_standard_derivatives = 0;
	OES_fragment_precision_high = 0;
    OES_EGL_image_external = 0;

	MaxCallStackDepth = UINT_MAX;
}
