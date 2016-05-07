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

#ifndef sw_PixelShader_hpp
#define sw_PixelShader_hpp

#include "Shader.hpp"

namespace sw
{
	class PixelShader : public Shader
	{
	public:
		explicit PixelShader(const PixelShader *ps = 0);
		explicit PixelShader(const unsigned long *token);

		virtual ~PixelShader();

		static int validate(const unsigned long *const token);   // Returns number of instructions if valid
		bool depthOverride() const;
		bool containsKill() const;
		bool containsCentroid() const;
		bool usesDiffuse(int component) const;
		bool usesSpecular(int component) const;
		bool usesTexture(int coordinate, int component) const;

		virtual void analyze();

		enum {MAX_INPUT_VARYINGS = 10};
		Semantic semantic[MAX_INPUT_VARYINGS][4];   // FIXME: Private

		bool vPosDeclared;
		bool vFaceDeclared;

	private:
		void analyzeZOverride();
		void analyzeKill();
		void analyzeInterpolants();

		bool zOverride;
		bool kill;
		bool centroid;
	};
}

#endif   // sw_PixelShader_hpp
