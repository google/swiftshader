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

#include "Shell.hpp"

namespace sw
{
	TranscendentalPrecision logPrecision = ACCURATE;
	TranscendentalPrecision expPrecision = ACCURATE;
	TranscendentalPrecision rcpPrecision = ACCURATE;
	TranscendentalPrecision rsqPrecision = ACCURATE;
	bool perspectiveCorrection = true;

	Color4i::Color4i() : x(r), y(g), z(b), w(a)
	{
	}

	Color4i::Color4i(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) : x(r), y(g), z(b), w(a)
	{
		r = Short4(red);
		g = Short4(green);
		b = Short4(blue);
		a = Short4(alpha);
	}

	Color4i::Color4i(const Color4i &rhs) : x(r), y(g), z(b), w(a)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
	}

	Color4i &Color4i::operator=(const Color4i &rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;

		return *this;
	}

	Short4 &Color4i::operator[](int i)
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}

		return x;
	}

	Color4f::Color4f() : x(r), y(g), z(b), w(a), u(r), v(g), s(b), t(a)
	{
	}

	Color4f::Color4f(float red, float green, float blue, float alpha) : x(r), y(g), z(b), w(a), u(r), v(g), s(b), t(a)
	{
		r = Float4(red);
		g = Float4(green);
		b = Float4(blue);
		a = Float4(alpha);
	}

	Color4f::Color4f(const Color4f &rhs) : x(r), y(g), z(b), w(a), u(r), v(g), s(b), t(a)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
	}

	Color4f &Color4f::operator=(const Color4f &rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;

		return *this;
	}

	Float4 &Color4f::operator[](int i)
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}

		return x;
	}

	Float4 exponential(Float4 &src, bool pp)
	{
		Float4 x0;
		Float4 x1;
		Int4 x2;
	
		x0 = src;

		x0 = Min(x0, As<Float4>(Int4(0x43010000, 0x43010000, 0x43010000, 0x43010000)));   // 129.00000e+0f
		x0 = Max(x0, As<Float4>(Int4(0xC2FDFFFF, 0xC2FDFFFF, 0xC2FDFFFF, 0xC2FDFFFF)));   // -126.99999e+0f
		x1 = x0;
		x1 -= Float4(0.5f, 0.5f, 0.5f, 0.5f);
		x2 = RoundInt(x1);
		x1 = Float4(x2);
		x2 += Int4(0x0000007F, 0x0000007F, 0x0000007F, 0x0000007F);   // 127
		x2 = x2 << 23;
		x0 -= x1;
		x1 = As<Float4>(Int4(0x3AF61905, 0x3AF61905, 0x3AF61905, 0x3AF61905));   // 1.8775767e-3f
		x1 *= x0;
		x1 += As<Float4>(Int4(0x3C134806, 0x3C134806, 0x3C134806, 0x3C134806));   // 8.9893397e-3f
		x1 *= x0;
		x1 += As<Float4>(Int4(0x3D64AA23, 0x3D64AA23, 0x3D64AA23, 0x3D64AA23));   // 5.5826318e-2f
		x1 *= x0;
		x1 += As<Float4>(Int4(0x3E75EAD4, 0x3E75EAD4, 0x3E75EAD4, 0x3E75EAD4));   // 2.4015361e-1f
		x1 *= x0;
		x1 += As<Float4>(Int4(0x3F31727B, 0x3F31727B, 0x3F31727B, 0x3F31727B));   // 6.9315308e-1f
		x1 *= x0;
		x1 += As<Float4>(Int4(0x3F7FFFFF, 0x3F7FFFFF, 0x3F7FFFFF, 0x3F7FFFFF));   // 9.9999994e-1f
		x1 *= As<Float4>(x2);
			
		return x1;
	}

	Float4 logarithm(Float4 &src, bool absolute, bool pp)
	{
		Float4 x0;
		Float4 x1;
		Float4 x2;
		Float4 x3;
		
		x0 = src;
		
		x1 = As<Float4>(As<Int4>(x0) & Int4(0x7F800000));
		x1 = As<Float4>(As<UInt4>(x1) >> 8);
		x1 = As<Float4>(As<Int4>(x1) | As<Int4>(Float4(1.0f)));
		x1 = (x1 - Float4(1.4960938f)) * Float4(256.0f);   // FIXME: (x1 - 1.4960938f) * 256.0f;
		x0 = As<Float4>((As<Int4>(x0) & Int4(0x007FFFFF)) | As<Int4>(Float4(1.0f)));

		x2 = (Float4(9.5428179e-2f) * x0 + Float4(4.7779095e-1f)) * x0 + Float4(1.9782813e-1f);
		x3 = ((Float4(1.6618466e-2f) * x0 + Float4(2.0350508e-1f)) * x0 + Float4(2.7382900e-1f)) * x0 + Float4(4.0496687e-2f);
		x2 /= x3;

		x1 += (x0 - Float4(1.0f)) * x2;
				
		return x1;
	}

	Float4 power(Float4 &src0, Float4 &src1, bool pp)
	{
		Float4 log = logarithm(src0, true, pp);
		log *= src1;
		return exponential(log, pp);
	}

	Float4 reciprocal(Float4 &src, bool pp, bool finite)
	{
		Float4 dst;

		if(!pp && rcpPrecision >= WHQL)
		{
			dst = Float4(1, 1, 1, 1) / src;
		}
		else
		{
			dst = Rcp_pp(src);

			if(!pp)
			{
				dst = (dst + dst) - (src * dst * dst);
			}
		}

		if(finite)
		{
			int x = 0x7F7FFFFF;
			dst = Min(dst, Float4((float&)x, (float&)x, (float&)x, (float&)x));
		}

		return dst;
	}

	Float4 reciprocalSquareRoot(Float4 &src, bool absolute, bool pp)
	{
		Float4 abs = src;

		if(absolute)
		{
			abs = Abs(abs);
		}

		Float4 rsq;

		if(!pp && rsqPrecision >= IEEE)
		{
			rsq = Float4(1.0f, 1.0f, 1.0f, 1.0f) / Sqrt(abs);
		}
		else
		{
			rsq = RcpSqrt_pp(abs);

			if(!pp)
			{
				rsq = rsq * (Float4(3.0f) - rsq * rsq * abs) * Float4(0.5f);
			}
		}

		int x = 0x7F7FFFFF;
		rsq = Min(rsq, Float4((float&)x, (float&)x, (float&)x, (float&)x));

		return rsq;
	}

	Float4 sine(Float4 &src, bool pp)
	{
		const Float4 A = Float4(-4.05284734e-1f);   // -4/pi^2
		const Float4 B = Float4(1.27323954e+0f);    // 4/pi
		const Float4 C = Float4(7.75160950e-1f);
		const Float4 D = Float4(2.24839049e-1f);

		// Parabola approximating sine
		Float4 sin = src * (Abs(src) * A + B);

		// Improve precision from 0.06 to 0.001
		if(true)
		{
			sin = sin * (Abs(sin) * D + C);
		}

		return sin;
	}

	Float4 dot3(Color4f &src0, Color4f &src1)
	{
		return src0.x * src1.x + src0.y * src1.y + src0.z * src1.z;
	}

	Float4 dot4(Color4f &src0, Color4f &src1)
	{
		return src0.x * src1.x + src0.y * src1.y + src0.z * src1.z + src0.w * src1.w;
	}

	void transpose4x4(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3)
	{
		Int2 tmp0 = UnpackHigh(row0, row1);
		Int2 tmp1 = UnpackHigh(row2, row3);
		Int2 tmp2 = UnpackLow(row0, row1);
		Int2 tmp3 = UnpackLow(row2, row3);

		row0 = As<Short4>(UnpackLow(tmp2, tmp3));
		row1 = As<Short4>(UnpackHigh(tmp2, tmp3));
		row2 = As<Short4>(UnpackLow(tmp0, tmp1));
		row3 = As<Short4>(UnpackHigh(tmp0, tmp1));
	}

	void transpose4x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		Float4 tmp0 = UnpackLow(row0, row1);
		Float4 tmp1 = UnpackLow(row2, row3);
		Float4 tmp2 = UnpackHigh(row0, row1);
		Float4 tmp3 = UnpackHigh(row2, row3);

		row0 = Float4(tmp0.xy, tmp1.xy);
		row1 = Float4(tmp0.zw, tmp1.zw);
		row2 = Float4(tmp2.xy, tmp3.xy);
		row3 = Float4(tmp2.zw, tmp3.zw);
	}

	void transpose4x3(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		Float4 tmp0 = UnpackLow(row0, row1);
		Float4 tmp1 = UnpackLow(row2, row3);
		Float4 tmp2 = UnpackHigh(row0, row1);
		Float4 tmp3 = UnpackHigh(row2, row3);

		row0 = Float4(tmp0.xy, tmp1.xy);
		row1 = Float4(tmp0.zw, tmp1.zw);
		row2 = Float4(tmp2.xy, tmp3.xy);
	}

	void transpose4x2(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		Float4 tmp0 = UnpackLow(row0, row1);
		Float4 tmp1 = UnpackLow(row2, row3);

		row0 = Float4(tmp0.xy, tmp1.xy);
		row1 = Float4(tmp0.zw, tmp1.zw);
	}

	void transpose4x1(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		Float4 tmp0 = UnpackLow(row0, row1);
		Float4 tmp1 = UnpackLow(row2, row3);

		row0 = Float4(tmp0.xy, tmp1.xy);
	}

	void transpose2x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		row0 = UnpackLow(row0, row1);
		row1 = Float4(row0.zw, row1.zw);
		row2 = UnpackHigh(row0, row1);
		row3 = Float4(row2.zw, row3.zw);
	}

	void transpose2x4h(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3)
	{
		row0 = UnpackLow(row2, row3);
		row1 = Float4(row0.zw, row1.zw);
		row2 = UnpackHigh(row2, row3);
		row3 = Float4(row2.zw, row3.zw);
	}

	void transpose4xN(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3, int N)
	{
		switch(N)
		{
		case 1: transpose4x1(row0, row1, row2, row3); break;
		case 2: transpose4x2(row0, row1, row2, row3); break;
		case 3: transpose4x3(row0, row1, row2, row3); break;
		case 4: transpose4x4(row0, row1, row2, row3); break;
		}
	}
}
