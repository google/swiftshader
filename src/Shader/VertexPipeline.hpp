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

#ifndef sw_VertexPipeline_hpp
#define sw_VertexPipeline_hpp

#include "VertexRoutine.hpp"

#include "Context.hpp"
#include "VertexProcessor.hpp"

namespace sw
{
	class VertexPipeline : public VertexRoutine
	{
	public:
		VertexPipeline(const VertexProcessor::State &state);

		virtual ~VertexPipeline();

	private:
		void pipeline(Registers &r);
		void processTextureCoordinate(Registers &r, int stage, Color4f &normal, Color4f &position);
		void processPointSize(Registers &r);

		Color4f transformBlend(Registers &r, Color4f &src, Pointer<Byte> &matrix, bool homogenous);
		Color4f transform(Color4f &src, Pointer<Byte> &matrix, bool homogenous);
		Color4f transform(Color4f &src, Pointer<Byte> &matrix, UInt index[4], bool homogenous);
		Color4f normalize(Color4f &src);
		Float4 power(Float4 &src0, Float4 &src1);
	};
};

#endif   // sw_VertexPipeline_hpp
