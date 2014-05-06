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

#ifndef sw_ShaderCore_hpp
#define sw_ShaderCore_hpp

#include "Shader.hpp"
#include "Reactor/Reactor.hpp"

namespace sw
{
	class ShaderCore
	{
		typedef Shader::Instruction::Operation::Control Control;
		typedef Shader::Instruction::Operation Op;

	public:
		void mov(Color4f &dst, Color4f &src, bool floorToInteger = false);
		void add(Color4f &dst, Color4f &src0, Color4f &src1);
		void sub(Color4f &dst, Color4f &src0, Color4f &src1);
		void mad(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2);
		void mul(Color4f &dst, Color4f &src0, Color4f &src1);
		void rcp(Color4f &dst, Color4f &src, bool pp = false);
		void rsq(Color4f &dst, Color4f &src, bool pp = false);
		void dp3(Color4f &dst, Color4f &src0, Color4f &src1);
		void dp4(Color4f &dst, Color4f &src0, Color4f &src1);
		void min(Color4f &dst, Color4f &src0, Color4f &src1);
		void max(Color4f &dst, Color4f &src0, Color4f &src1);
		void slt(Color4f &dst, Color4f &src0, Color4f &src1);
		void sge(Color4f &dst, Color4f &src0, Color4f &src1);
		void exp(Color4f &dst, Color4f &src, bool pp = false);
		void log(Color4f &dst, Color4f &src, bool pp = false);
		void lit(Color4f &dst, Color4f &src);
		void dst(Color4f &dst, Color4f &src0, Color4f &src1);
		void lrp(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2);
		void frc(Color4f &dst, Color4f &src);
		void pow(Color4f &dst, Color4f &src0, Color4f &src1, bool pp = false);
		void crs(Color4f &dst, Color4f &src0, Color4f &src1);
		void sgn(Color4f &dst, Color4f &src);
		void abs(Color4f &dst, Color4f &src);
		void nrm(Color4f &dst, Color4f &src, bool pp = false);
		void sincos(Color4f &dst, Color4f &src, bool pp = false);
		void expp(Color4f &dst, Color4f &src, unsigned short version);
		void logp(Color4f &dst, Color4f &src, unsigned short version);
		void cmp(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2);
		void dp2add(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2);
		void setp(Color4f &dst, Color4f &src0, Color4f &src1, Control control);

	private:
		void sgn(Float4 &dst, Float4 &src);
		void cmp(Float4 &dst, Float4 &src0, Float4 &src1, Float4 &src2);
	};
}

#endif   // sw_ShaderCore_hpp
