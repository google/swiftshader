// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "TranslatorASM.h"

#include "InitializeParseContext.h"

TranslatorASM::TranslatorASM(glsl::Shader *shaderObject, ShShaderType type, ShShaderSpec spec) : TCompiler(type, spec), shaderObject(shaderObject)
{
}

bool TranslatorASM::translate(TIntermNode* root)
{
    TParseContext& parseContext = *GetGlobalParseContext();
    glsl::OutputASM outputASM(parseContext, shaderObject);

	outputASM.output();

	return parseContext.numErrors() == 0;
}
