// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
