//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Compiler.h"

#include "AnalyzeCallDepth.h"
#include "Initialize.h"
#include "InitializeParseContext.h"
#include "InitializeGlobals.h"
#include "ParseHelper.h"
#include "ValidateLimitations.h"

namespace 
{
class TScopedPoolAllocator {
public:
    TScopedPoolAllocator(TPoolAllocator* allocator, bool pushPop)
        : mAllocator(allocator), mPushPopAllocator(pushPop) {
        if (mPushPopAllocator) mAllocator->push();
        SetGlobalPoolAllocator(mAllocator);
    }
    ~TScopedPoolAllocator() {
        SetGlobalPoolAllocator(NULL);
        if (mPushPopAllocator) mAllocator->pop();
    }

private:
    TPoolAllocator* mAllocator;
    bool mPushPopAllocator;
};
}  // namespace

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

TCompiler::TCompiler(GLenum type)
    : shaderType(type),
      maxCallStackDepth(UINT_MAX)
{
	allocator.push();
    SetGlobalPoolAllocator(&allocator);
}

TCompiler::~TCompiler()
{
	SetGlobalPoolAllocator(NULL);
    allocator.popAll();
}

bool TCompiler::Init(const ShBuiltInResources& resources)
{
    shaderVersion = 100;
    maxCallStackDepth = resources.MaxCallStackDepth;
    TScopedPoolAllocator scopedAlloc(&allocator, false);

    // Generate built-in symbol table.
    if (!InitBuiltInSymbolTable(resources))
        return false;
    InitExtensionBehavior(resources, extensionBehavior);

    return true;
}

bool TCompiler::compile(const char* const shaderStrings[],
                        const int numStrings,
                        int compileOptions)
{
    TScopedPoolAllocator scopedAlloc(&allocator, true);
    clearResults();

    if (numStrings == 0)
        return true;

    // First string is path of source file if flag is set. The actual source follows.
    const char* sourcePath = NULL;
    int firstSource = 0;
    if (compileOptions & SH_SOURCE_PATH)
    {
        sourcePath = shaderStrings[0];
        ++firstSource;
    }

    TIntermediate intermediate(infoSink);
    TParseContext parseContext(symbolTable, extensionBehavior, intermediate,
                               shaderType, compileOptions, true,
                               sourcePath, infoSink);
    SetGlobalParseContext(&parseContext);

    // We preserve symbols at the built-in level from compile-to-compile.
    // Start pushing the user-defined symbols at global level.
    symbolTable.push();
    if (!symbolTable.atGlobalLevel())
        infoSink.info.message(EPrefixInternalError, "Wrong symbol table level");

    // Parse shader.
    bool success =
        (PaParseStrings(numStrings - firstSource, &shaderStrings[firstSource], NULL, &parseContext) == 0) &&
        (parseContext.treeRoot != NULL);

    shaderVersion = parseContext.getShaderVersion();

    if (success) {
        TIntermNode* root = parseContext.treeRoot;
        success = intermediate.postProcess(root);

        if (success)
            success = validateCallDepth(root, infoSink);

        if (success && (compileOptions & SH_VALIDATE_LOOP_INDEXING))
            success = validateLimitations(root);

        if (success && (compileOptions & SH_INTERMEDIATE_TREE))
            intermediate.outputTree(root);

        if (success && (compileOptions & SH_OBJECT_CODE))
            success = translate(root);
    }

    // Ensure symbol table is returned to the built-in level,
    // throwing away all but the built-ins.
    while (!symbolTable.atBuiltInLevel())
        symbolTable.pop();

    return success;
}

bool TCompiler::InitBuiltInSymbolTable(const ShBuiltInResources &resources)
{
    assert(symbolTable.isEmpty());
    
    //
    // Push the symbol table to give it an initial scope.  This
    // push should not have a corresponding pop, so that built-ins
    // are preserved, and the test for an empty table fails.
    //
    symbolTable.push();

	TPublicType integer;
	integer.type = EbtInt;
	integer.size = 1;
	integer.matrix = false;
	integer.array = false;

	TPublicType floatingPoint;
	floatingPoint.type = EbtFloat;
	floatingPoint.size = 1;
	floatingPoint.matrix = false;
	floatingPoint.array = false;

	switch(shaderType)
	{
    case GL_FRAGMENT_SHADER:
		symbolTable.setDefaultPrecision(integer, EbpMedium);
        break;
    case GL_VERTEX_SHADER:
		symbolTable.setDefaultPrecision(integer, EbpHigh);
		symbolTable.setDefaultPrecision(floatingPoint, EbpHigh);
        break;
    default: assert(false && "Language not supported");
    }

	InsertBuiltInFunctions(shaderType, resources, symbolTable);

    IdentifyBuiltIns(shaderType, resources, symbolTable);

    return true;
}

void TCompiler::clearResults()
{
    infoSink.info.erase();
    infoSink.obj.erase();
    infoSink.debug.erase();
}

bool TCompiler::validateCallDepth(TIntermNode *root, TInfoSink &infoSink)
{
    AnalyzeCallDepth validator(root);
    
	unsigned int depth = validator.analyzeCallDepth();
	
	if(depth == 0)
	{
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "Missing main()";
        return false;
	}
    else if(depth == UINT_MAX)
	{
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "Function recursion detected";
        return false;
	}
	else if(depth > maxCallStackDepth)
	{
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "Function call stack too deep";
        return false;
	}

    return true;
}

bool TCompiler::validateLimitations(TIntermNode* root) {
    ValidateLimitations validate(shaderType, infoSink.info);
    root->traverse(&validate);
    return validate.numErrors() == 0;
}

const TExtensionBehavior& TCompiler::getExtensionBehavior() const
{
    return extensionBehavior;
}

bool InitCompilerGlobals()
{
    if(!InitializePoolIndex())
	{
        assert(0 && "InitCompilerGlobals(): Failed to initalize global pool");
        return false;
    }

    if(!InitializeParseContextIndex())
	{
        assert(0 && "InitCompilerGlobals(): Failed to initalize parse context");
        return false;
    }

    return true;
}

void FreeCompilerGlobals()
{
    FreeParseContextIndex();
    FreePoolIndex();
}
