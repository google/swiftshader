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
