// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
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
		explicit VertexShader(const VertexShader *vs = 0);
		explicit VertexShader(const unsigned long *token);

		virtual ~VertexShader();

		static int validate(const unsigned long *const token);   // Returns number of instructions if valid
		bool containsTextureSampling() const;

		virtual void analyze();

		int positionRegister;     // FIXME: Private
		int pointSizeRegister;    // FIXME: Private

		bool instanceIdDeclared;

		enum {MAX_INPUT_ATTRIBUTES = 16};
		Semantic input[MAX_INPUT_ATTRIBUTES];       // FIXME: Private

		enum {MAX_OUTPUT_VARYINGS = 12};
		Semantic output[MAX_OUTPUT_VARYINGS][4];   // FIXME: Private

	private:
		void analyzeInput();
		void analyzeOutput();
		void analyzeTextureSampling();

		bool textureSampling;
	};
}

#endif   // sw_VertexShader_hpp
