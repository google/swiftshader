//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

//
// Create strings that declare built-in definitions, add built-ins that
// cannot be expressed in the files, and establish mappings between 
// built-in functions and operators.
//

#include "Initialize.h"

#include "intermediate.h"

void InsertBuiltInFunctions(GLenum type, const ShBuiltInResources &resources, TSymbolTable &symbolTable)
{
	TType *float1 = new TType(EbtFloat);
	TType *float2 = new TType(EbtFloat, 2);
	TType *float3 = new TType(EbtFloat, 3);
	TType *float4 = new TType(EbtFloat, 4);
	TType *genType = new TType(EbtGenType);

    //
    // Angle and Trigonometric Functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpRadians, genType, "radians", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpDegrees, genType, "degrees", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpSin, genType, "sin", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpCos, genType, "cos", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpTan, genType, "tan", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAsin, genType, "asin", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAcos, genType, "acos", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAtan, genType, "atan", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAtan, genType, "atan", genType);

    //
    // Exponential Functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpPow, genType, "pow", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpExp, genType, "exp", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLog, genType, "log", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpExp2, genType, "exp2", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLog2, genType, "log2", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpSqrt, genType, "sqrt", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpInverseSqrt, genType, "inversesqrt", genType);

    //
    // Common Functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAbs, genType, "abs", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpSign, genType, "sign", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpFloor, genType, "floor", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpCeil, genType, "ceil", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpFract, genType, "fract", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMod, genType, "mod", genType, float1);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMod, genType, "mod", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMin, genType, "min", genType, float1);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMin, genType, "min", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMax, genType, "max", genType, float1);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMax, genType, "max", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpClamp, genType, "clamp", genType, float1, float1);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpClamp, genType, "clamp", genType, genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMix, genType, "mix", genType, genType, float1);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMix, genType, "mix", genType, genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpStep, genType, "step", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpStep, genType, "step", float1, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpSmoothStep, genType, "smoothstep", genType, genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpSmoothStep, genType, "smoothstep", float1, float1, genType);

    //
    // Geometric Functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLength, float1, "length", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpDistance, float1, "distance", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpDot, float1, "dot", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpCross, float3, "cross", float3, float3);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpNormalize, genType, "normalize", genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpFaceForward, genType, "faceforward", genType, genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpReflect, genType, "reflect", genType, genType);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpRefract, genType, "refract", genType, genType, float1);

	TType *mat2 = new TType(EbtFloat, 2, true);
	TType *mat3 = new TType(EbtFloat, 3, true);
	TType *mat4 = new TType(EbtFloat, 4, true);

    //
    // Matrix Functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMul, mat2, "matrixCompMult", mat2, mat2);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMul, mat3, "matrixCompMult", mat3, mat3);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpMul, mat4, "matrixCompMult", mat4, mat4);

	TType *bool1 = new TType(EbtBool);
	TType *vec = new TType(EbtVec);
	TType *ivec = new TType(EbtIVec);
	TType *bvec = new TType(EbtBVec);

    //
    // Vector relational functions.
    //
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLessThan, bvec, "lessThan", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLessThan, bvec, "lessThan", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLessThanEqual, bvec, "lessThanEqual", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpLessThanEqual, bvec, "lessThanEqual", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpGreaterThan, bvec, "greaterThan", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpGreaterThan, bvec, "greaterThan", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpGreaterThanEqual, bvec, "greaterThanEqual", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpGreaterThanEqual, bvec, "greaterThanEqual", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorEqual, bvec, "equal", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorEqual, bvec, "equal", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorEqual, bvec, "equal", bvec, bvec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorNotEqual, bvec, "notEqual", vec, vec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorNotEqual, bvec, "notEqual", ivec, ivec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorNotEqual, bvec, "notEqual", bvec, bvec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAny, bool1, "any", bvec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpAll, bool1, "all", bvec);
    symbolTable.insertBuiltIn(COMMON_BUILTINS, EOpVectorLogicalNot, bvec, "not", bvec);

	TType *sampler2D = new TType(EbtSampler2D);
	TType *samplerCube = new TType(EbtSamplerCube);
	TType *sampler3D = new TType(EbtSampler3D);

    //
    // Texture Functions for GLSL ES 1.0
    //
    symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2D", sampler2D, float2);
    symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", sampler2D, float3);
    symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", sampler2D, float4);
    symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "textureCube", samplerCube, float3);
	symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture3D", sampler3D, float3);

	if(resources.OES_EGL_image_external)
    {
        TType *samplerExternalOES = new TType(EbtSamplerExternalOES);

        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2D", samplerExternalOES, float2);
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", samplerExternalOES, float3);
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", samplerExternalOES, float4);
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture3D", samplerExternalOES, float3);
    }

	if(type == GL_FRAGMENT_SHADER)
	{
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2D", sampler2D, float2, float1);
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", sampler2D, float3, float1);
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProj", sampler2D, float4, float1);
        symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "textureCube", samplerCube, float3, float1);

		if(resources.OES_standard_derivatives)
		{
			symbolTable.insertBuiltIn(ESSL1_BUILTINS, EOpDFdx, "GL_OES_standard_derivatives", genType, "dFdx", genType);
			symbolTable.insertBuiltIn(ESSL1_BUILTINS, EOpDFdy, "GL_OES_standard_derivatives", genType, "dFdy", genType);
			symbolTable.insertBuiltIn(ESSL1_BUILTINS, EOpFwidth,"GL_OES_standard_derivatives", genType, "fwidth", genType);
		}
	}

	if(type == GL_VERTEX_SHADER)
	{
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DLod", sampler2D, float2, float1);
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProjLod", sampler2D, float3, float1);
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture2DProjLod", sampler2D, float4, float1);
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "textureCubeLod", samplerCube, float3, float1);
		symbolTable.insertBuiltIn(ESSL1_BUILTINS, float4, "texture3DLod", sampler3D, float3, float1);
	}

    TType *gvec4 = new TType(EbtGVec4);

    TType *gsampler2D = new TType(EbtGSampler2D);
    TType *gsamplerCube = new TType(EbtGSamplerCube);
    TType *gsampler3D = new TType(EbtGSampler3D);
    TType *gsampler2DArray = new TType(EbtGSampler2DArray);

    //
    // Texture Functions for GLSL ES 3.0
    //
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler2D, float2);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler3D, float3);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsamplerCube, float3);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler2DArray, float3);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler2D, float3);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler2D, float4);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler3D, float4);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureLod", gsampler2D, float2, float1);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureLod", gsampler3D, float3, float1);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureLod", gsamplerCube, float3, float1);
    symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureLod", gsampler2DArray, float3, float1);

    if(type == GL_FRAGMENT_SHADER)
    {
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler2D, float2, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler3D, float3, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsamplerCube, float3, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "texture", gsampler2DArray, float3, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler2D, float3, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler2D, float4, float1);
        symbolTable.insertBuiltIn(ESSL3_BUILTINS, gvec4, "textureProj", gsampler3D, float4, float1);
    }

    //
    // Depth range in window coordinates
    //
	TTypeList *members = NewPoolTTypeList();
	TTypeLine near = {new TType(EbtFloat, EbpHigh, EvqGlobal, 1), 0};
	TTypeLine far = {new TType(EbtFloat, EbpHigh, EvqGlobal, 1), 0};
	TTypeLine diff = {new TType(EbtFloat, EbpHigh, EvqGlobal, 1), 0};
	near.type->setFieldName("near");
	far.type->setFieldName("far");
	diff.type->setFieldName("diff");
	members->push_back(near);
	members->push_back(far);
	members->push_back(diff);
	TVariable *depthRangeParameters = new TVariable(NewPoolTString("gl_DepthRangeParameters"), TType(members, "gl_DepthRangeParameters"), true);
	symbolTable.insert(COMMON_BUILTINS, *depthRangeParameters);
	TVariable *depthRange = new TVariable(NewPoolTString("gl_DepthRange"), TType(members, "gl_DepthRangeParameters"));
	depthRange->setQualifier(EvqUniform);
	symbolTable.insert(COMMON_BUILTINS, *depthRange);

    //
    // Implementation dependent built-in constants.
    //
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxVertexAttribs", resources.MaxVertexAttribs);
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxVertexUniformVectors", resources.MaxVertexUniformVectors);
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxVertexTextureImageUnits", resources.MaxVertexTextureImageUnits);
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxCombinedTextureImageUnits", resources.MaxCombinedTextureImageUnits);
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxTextureImageUnits", resources.MaxTextureImageUnits);
    symbolTable.insertConstInt(COMMON_BUILTINS, "gl_MaxFragmentUniformVectors", resources.MaxFragmentUniformVectors);
    symbolTable.insertConstInt(ESSL1_BUILTINS, "gl_MaxVaryingVectors", resources.MaxVaryingVectors);
    symbolTable.insertConstInt(ESSL1_BUILTINS, "gl_MaxDrawBuffers", resources.MaxDrawBuffers);
}

void IdentifyBuiltIns(GLenum shaderType,
                      const ShBuiltInResources &resources,
                      TSymbolTable &symbolTable)
{
    //
    // First, insert some special built-in variables that are not in 
    // the built-in header files.
    //
    switch(shaderType)
	{
    case GL_FRAGMENT_SHADER:
        symbolTable.insert(COMMON_BUILTINS, *new TVariable(NewPoolTString("gl_FragCoord"), TType(EbtFloat, EbpMedium, EvqFragCoord,   4)));
        symbolTable.insert(COMMON_BUILTINS, *new TVariable(NewPoolTString("gl_FrontFacing"), TType(EbtBool,  EbpUndefined, EvqFrontFacing, 1)));
        symbolTable.insert(COMMON_BUILTINS, *new TVariable(NewPoolTString("gl_PointCoord"), TType(EbtFloat, EbpMedium, EvqPointCoord,  2)));
        symbolTable.insert(ESSL1_BUILTINS, *new TVariable(NewPoolTString("gl_FragColor"), TType(EbtFloat, EbpMedium, EvqFragColor,   4)));
        symbolTable.insert(ESSL1_BUILTINS, *new TVariable(NewPoolTString("gl_FragData[gl_MaxDrawBuffers]"), TType(EbtFloat, EbpMedium, EvqFragData,    4)));
        break;
    case GL_VERTEX_SHADER:
        symbolTable.insert(COMMON_BUILTINS, *new TVariable(NewPoolTString("gl_Position"), TType(EbtFloat, EbpHigh, EvqPosition,    4)));
        symbolTable.insert(COMMON_BUILTINS, *new TVariable(NewPoolTString("gl_PointSize"), TType(EbtFloat, EbpMedium, EvqPointSize,   1)));
        break;
    default: assert(false && "Language not supported");
    }

    // Finally add resource-specific variables.
    switch(shaderType)
    {
    case GL_FRAGMENT_SHADER:
		{
            // Set up gl_FragData.  The array size.
            TType fragData(EbtFloat, EbpMedium, EvqFragData, 4, false, true);
            fragData.setArraySize(resources.MaxDrawBuffers);
            symbolTable.insert(ESSL1_BUILTINS, *new TVariable(NewPoolTString("gl_FragData"), fragData));
		}
		break;
    default: break;
    }
}

void InitExtensionBehavior(const ShBuiltInResources& resources,
                           TExtensionBehavior& extBehavior)
{
    if(resources.OES_standard_derivatives)
        extBehavior["GL_OES_standard_derivatives"] = EBhUndefined;
	if(resources.OES_fragment_precision_high)
        extBehavior["GL_FRAGMENT_PRECISION_HIGH"] = EBhUndefined;
    if(resources.OES_EGL_image_external)
        extBehavior["GL_OES_EGL_image_external"] = EBhUndefined;
}
