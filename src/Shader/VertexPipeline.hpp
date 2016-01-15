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
		void pipeline() override;
		void processTextureCoordinate(int stage, Vector4f &normal, Vector4f &position);
		void processPointSize();

		Vector4f transformBlend(const Register &src, const Pointer<Byte> &matrix, bool homogenous);
		Vector4f transform(const Register &src, const Pointer<Byte> &matrix, bool homogenous);
		Vector4f transform(const Register &src, const Pointer<Byte> &matrix, UInt index[4], bool homogenous);
		Vector4f normalize(Vector4f &src);
		Float4 power(Float4 &src0, Float4 &src1);
	};
};

#endif   // sw_VertexPipeline_hpp
