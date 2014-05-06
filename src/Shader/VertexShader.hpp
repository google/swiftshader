// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_VertexShader_hpp
#define sw_VertexShader_hpp

#include "Shader.hpp"

namespace sw
{
	class VertexShader : public Shader
	{
	public:
		VertexShader(const unsigned long *token);

		virtual ~VertexShader();

		static int validate(const unsigned long *const token);   // Returns number of instructions if valid
		bool containsTexldl() const;
		
		int positionRegister;     // FIXME: Private
		int pointSizeRegister;    // FIXME: Private

		Semantic input[16];       // FIXME: Private
		Semantic output[12][4];   // FIXME: Private

	private:
		void parse(const unsigned long *token);

		void analyzeInput();
		void analyzeOutput();
		void analyzeTexldl();

		bool texldl;
	};

	typedef VertexShader::Instruction VertexShaderInstruction;
}

#endif   // sw_VertexShader_hpp
