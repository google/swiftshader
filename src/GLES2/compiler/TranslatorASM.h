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

#ifndef COMPILER_TRANSLATORASM_H_
#define COMPILER_TRANSLATORASM_H_

#include "ShHandle.h"
#include "OutputASM.h"
#include "Shader/PixelShader.hpp"
#include "Shader/VertexShader.hpp"

namespace gl2
{
	class Shader;
}

class TranslatorASM : public TCompiler
{
public:
    TranslatorASM(gl2::Shader *shaderObject, ShShaderType type, ShShaderSpec spec);

protected:
    virtual bool translate(TIntermNode* root);

private:
	gl2::Shader *const shaderObject;
};

#endif  // COMPILER_TRANSLATORASM_H_
