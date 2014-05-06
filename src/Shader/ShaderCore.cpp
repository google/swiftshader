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

#include "ShaderCore.hpp"

#include "Debug.hpp"

namespace sw
{
	void ShaderCore::mov(Color4f &dst, Color4f &src, bool floorToInteger)
	{
		if(floorToInteger)
		{
			dst.x = Floor(src.x);
		}
		else
		{
			dst = src;
		}
	}

	void ShaderCore::add(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = src0.x + src1.x;
		dst.y = src0.y + src1.y;
		dst.z = src0.z + src1.z;
		dst.w = src0.w + src1.w;
	}

	void ShaderCore::sub(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = src0.x - src1.x;
		dst.y = src0.y - src1.y;
		dst.z = src0.z - src1.z;
		dst.w = src0.w - src1.w;
	}

	void ShaderCore::mad(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2)
	{
		dst.x = src0.x * src1.x + src2.x;
		dst.y = src0.y * src1.y + src2.y;
		dst.z = src0.z * src1.z + src2.z;
		dst.w = src0.w * src1.w + src2.w;
	}

	void ShaderCore::mul(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = src0.x * src1.x;
		dst.y = src0.y * src1.y;
		dst.z = src0.z * src1.z;
		dst.w = src0.w * src1.w;
	}

	void ShaderCore::rcp(Color4f &dst, Color4f &src, bool pp)
	{
		Float4 rcp = reciprocal(src.x, pp, true);

		dst.x = rcp;
		dst.y = rcp;
		dst.z = rcp;
		dst.w = rcp;
	}

	void ShaderCore::rsq(Color4f &dst, Color4f &src, bool pp)
	{
		Float4 rsq = reciprocalSquareRoot(src.x, true, pp);

		dst.r = rsq;
		dst.g = rsq;
		dst.b = rsq;
		dst.a = rsq;
	}

	void ShaderCore::dp3(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		Float4 dot = dot3(src0, src1);

		dst.x = dot;
		dst.y = dot;
		dst.z = dot;
		dst.w = dot;
	}

	void ShaderCore::dp4(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		Float4 dot = dot4(src0, src1);

		dst.x = dot;
		dst.y = dot;
		dst.z = dot;
		dst.w = dot;
	}

	void ShaderCore::min(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = Min(src0.x, src1.x);
		dst.y = Min(src0.y, src1.y);
		dst.z = Min(src0.z, src1.z);
		dst.w = Min(src0.w, src1.w);
	}

	void ShaderCore::max(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = Max(src0.x, src1.x);
		dst.y = Max(src0.y, src1.y);
		dst.z = Max(src0.z, src1.z);
		dst.w = Max(src0.w, src1.w);
	}

	void ShaderCore::slt(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		Int4 xMask = As<Int4>(CmpLT(src0.x, src1.x));
		Int4 yMask = As<Int4>(CmpLT(src0.y, src1.y));
		Int4 zMask = As<Int4>(CmpLT(src0.z, src1.z));
		Int4 wMask = As<Int4>(CmpLT(src0.w, src1.w));

		Int4 iOne = As<Int4>(Float4(1, 1, 1, 1));

		dst.x = As<Float4>(xMask & iOne);
		dst.y = As<Float4>(yMask & iOne);
		dst.z = As<Float4>(zMask & iOne);
		dst.w = As<Float4>(wMask & iOne);
	}

	void ShaderCore::sge(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		Int4 xMask = As<Int4>(CmpNLT(src0.x, src1.x));
		Int4 yMask = As<Int4>(CmpNLT(src0.y, src1.y));
		Int4 zMask = As<Int4>(CmpNLT(src0.z, src1.z));
		Int4 wMask = As<Int4>(CmpNLT(src0.w, src1.w));

		Int4 iOne = As<Int4>(Float4(1, 1, 1, 1));

		dst.x = As<Float4>(xMask & iOne);
		dst.y = As<Float4>(yMask & iOne);
		dst.z = As<Float4>(zMask & iOne);
		dst.w = As<Float4>(wMask & iOne);
	}

	void ShaderCore::exp(Color4f &dst, Color4f &src, bool pp)
	{ 
		Float4 exp = exponential(src.x, pp);

		dst.x = exp;
		dst.y = exp;
		dst.z = exp;
		dst.w = exp;
	}

	void ShaderCore::log(Color4f &dst, Color4f &src, bool pp)
	{
		Float4 log = logarithm(src.x, true, pp);

		dst.x = log;
		dst.y = log;
		dst.z = log;
		dst.w = log;
	}

	void ShaderCore::lit(Color4f &dst, Color4f &src)
	{
		dst.x = Float4(1.0f, 1.0f, 1.0f, 1.0f);
		dst.y = Max(src.x, Float4(0.0f, 0.0f, 0.0f, 0.0f));

		Float4 pow;

		pow = src.w;
		pow = Min(pow, Float4(127.9961f, 127.9961f, 127.9961f, 127.9961f));
		pow = Max(pow, Float4(-127.9961f, -127.9961f, -127.9961f, -127.9961f));

		dst.z = power(src.y, pow);
		dst.z = As<Float4>(As<Int4>(dst.z) & CmpNLT(src.x, Float4(0.0f, 0.0f, 0.0f, 0.0f)));
		dst.z = As<Float4>(As<Int4>(dst.z) & CmpNLT(src.y, Float4(0.0f, 0.0f, 0.0f, 0.0f)));

		dst.w = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void ShaderCore::dst(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = 1;
		dst.y = src0.y * src1.y;
		dst.z = src0.z;
		dst.w = src1.w;
	}

	void ShaderCore::lrp(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2)
	{
		dst.x = src0.x * (src1.x - src2.x) + src2.x;
		dst.y = src0.y * (src1.y - src2.y) + src2.y;
		dst.z = src0.z * (src1.z - src2.z) + src2.z;
		dst.w = src0.w * (src1.w - src2.w) + src2.w;
	}

	void ShaderCore::frc(Color4f &dst, Color4f &src)
	{
		dst.x = Fraction(src.x);
		dst.y = Fraction(src.y);
		dst.z = Fraction(src.z);
		dst.w = Fraction(src.w);
	}

	void ShaderCore::pow(Color4f &dst, Color4f &src0, Color4f &src1, bool pp)
	{
		Float4 pow = power(src0.x, src1.x, pp);

		dst.x = pow;
		dst.y = pow;
		dst.z = pow;
		dst.w = pow;
	}

	void ShaderCore::crs(Color4f &dst, Color4f &src0, Color4f &src1)
	{
		dst.x = src0.y * src1.z - src0.z * src1.y;
		dst.y = src0.z * src1.x - src0.x * src1.z;
		dst.z = src0.x * src1.y - src0.y * src1.x;
	}

	void ShaderCore::sgn(Color4f &dst, Color4f &src)
	{
		sgn(dst.x, src.x);
		sgn(dst.y, src.y);
		sgn(dst.z, src.z);
		sgn(dst.w, src.w);
	}

	void ShaderCore::abs(Color4f &dst, Color4f &src)
	{
		dst.x = Abs(src.x);
		dst.y = Abs(src.y);
		dst.z = Abs(src.z);
		dst.w = Abs(src.w);
	}

	void ShaderCore::nrm(Color4f &dst, Color4f &src, bool pp)
	{
		Float4 dot = dot3(src, src);
		Float4 rsq = reciprocalSquareRoot(dot, false, pp);

		dst.x = src.x * rsq;
		dst.y = src.y * rsq;
		dst.z = src.z * rsq;
		dst.w = src.w * rsq;
	}
	
	void ShaderCore::sincos(Color4f &dst, Color4f &src, bool pp)
	{
		Float4 tmp0;
		Float4 tmp1;

		tmp0 = src.x;

		// cos(x) = sin(x + pi/2)
		tmp0 += Float4(1.57079632e+0f);
		tmp1 = As<Float4>(CmpNLT(tmp0, Float4(3.14159265e+0f)) & As<Int4>(Float4(6.28318530e+0f)));
		tmp0 -= tmp1;

		dst.x = sine(tmp0, pp);
		dst.y = sine(src.x, pp);
	}

	void ShaderCore::expp(Color4f &dst, Color4f &src, unsigned short version)
	{
		if(version < 0x0200)
		{
			Float4 frc = Fraction(src.x);
			Float4 floor = src.x - frc;

			dst.x = exponential(floor, true);
			dst.y = frc;
			dst.z = exponential(src.x, true);
			dst.w = Float4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else   // Version >= 2.0
		{
			exp(dst, src, true);   // FIXME: 10-bit precision suffices
		}
	}
	
	void ShaderCore::logp(Color4f &dst, Color4f &src, unsigned short version)
	{
		if(version < 0x0200)
		{
			Float4 tmp0;
			Float4 tmp1;
			Float4 t;
			Int4 r;

			tmp0 = Abs(src.x);
			tmp1 = tmp0;

			// X component
			r = As<Int4>(As<UInt4>(tmp0) >> 23) - Int4(127, 127, 127, 127);
			dst.x = Float4(r);

			// Y component
			dst.y = As<Float4>((As<Int4>(tmp1) & Int4(0x007FFFFF)) | As<Int4>(Float4(1.0f)));

			// Z component
			dst.z = logarithm(src.x, true, true);

			// W component
			dst.w = 1.0f;
		}
		else
		{
			log(dst, src, true);
		}
	}
	
	void ShaderCore::cmp(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2)
	{
		cmp(dst.x, src0.x, src1.x, src2.x);
		cmp(dst.y, src0.y, src1.y, src2.y);
		cmp(dst.z, src0.z, src1.z, src2.z);
		cmp(dst.w, src0.w, src1.w, src2.w);
	}
	
	void ShaderCore::dp2add(Color4f &dst, Color4f &src0, Color4f &src1, Color4f &src2)
	{
		Float4 t = src0.x * src1.x + src0.y * src1.y + src2.x;

		dst.x = t;
		dst.y = t;
		dst.z = t;
		dst.w = t;
	}

	void ShaderCore::sgn(Float4 &dst, Float4 &src)
	{
		Int4 neg = As<Int4>(CmpLT(src, Float4(0, 0, 0, 0))) & As<Int4>(Float4(-1, -1, -1, -1));
		Int4 pos = As<Int4>(CmpNLT(src, Float4(0, 0, 0, 0))) & As<Int4>(Float4(1, 1, 1, 1));
		dst = As<Float4>(neg | pos);
	}

	void ShaderCore::cmp(Float4 &dst, Float4 &src0, Float4 &src1, Float4 &src2)
	{
		Int4 pos = CmpNLE(Float4(0.0f, 0.0f, 0.0f, 0.0f), src0);
		Int4 t0 = pos & As<Int4>(src2);
		Int4 t1 = ~pos & As<Int4>(src1);
		dst = As<Float4>(t0 | t1);
	}

	void ShaderCore::setp(Color4f &dst, Color4f &src0, Color4f &src1, Control control)
	{
		switch(control)
		{
		case Op::CONTROL_GT:
			dst.x = As<Float4>(CmpNLE(src0.x, src1.x));
			dst.y = As<Float4>(CmpNLE(src0.y, src1.y));
			dst.z = As<Float4>(CmpNLE(src0.z, src1.z));
			dst.w = As<Float4>(CmpNLE(src0.w, src1.w));
			break;
		case Op::CONTROL_EQ:
			dst.x = As<Float4>(CmpEQ(src0.x, src1.x));
			dst.y = As<Float4>(CmpEQ(src0.y, src1.y));
			dst.z = As<Float4>(CmpEQ(src0.z, src1.z));
			dst.w = As<Float4>(CmpEQ(src0.w, src1.w));
			break;
		case Op::CONTROL_GE:
			dst.x = As<Float4>(CmpNLT(src0.x, src1.x));
			dst.y = As<Float4>(CmpNLT(src0.y, src1.y));
			dst.z = As<Float4>(CmpNLT(src0.z, src1.z));
			dst.w = As<Float4>(CmpNLT(src0.w, src1.w));
			break;
		case Op::CONTROL_LT:
			dst.x = As<Float4>(CmpLT(src0.x, src1.x));
			dst.y = As<Float4>(CmpLT(src0.y, src1.y));
			dst.z = As<Float4>(CmpLT(src0.z, src1.z));
			dst.w = As<Float4>(CmpLT(src0.w, src1.w));
			break;
		case Op::CONTROL_NE:
			dst.x = As<Float4>(CmpNEQ(src0.x, src1.x));
			dst.y = As<Float4>(CmpNEQ(src0.y, src1.y));
			dst.z = As<Float4>(CmpNEQ(src0.z, src1.z));
			dst.w = As<Float4>(CmpNEQ(src0.w, src1.w));
			break;
		case Op::CONTROL_LE:
			dst.x = As<Float4>(CmpLE(src0.x, src1.x));
			dst.y = As<Float4>(CmpLE(src0.y, src1.y));
			dst.z = As<Float4>(CmpLE(src0.z, src1.z));
			dst.w = As<Float4>(CmpLE(src0.w, src1.w));
			break;
		default:
			ASSERT(false);
		}
	}
}
