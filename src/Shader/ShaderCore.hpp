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

#ifndef sw_ShaderCore_hpp
#define sw_ShaderCore_hpp

#include "Debug.hpp"
#include "Shader.hpp"
#include "Reactor/Reactor.hpp"

namespace sw
{
	class Vector4s
	{
	public:
		Vector4s();
		Vector4s(unsigned short x, unsigned short y, unsigned short z, unsigned short w);
		Vector4s(const Vector4s &rhs);

		Short4 &operator[](int i);
		Vector4s &operator=(const Vector4s &rhs);

		Short4 x;
		Short4 y;
		Short4 z;
		Short4 w;
	};

	class Vector4i
	{
	public:
		Vector4i();
		Vector4i(int x, int y, int z, int w);
		Vector4i(const Vector4i &rhs);

		Int4 &operator[](int i);
		Vector4i &operator=(const Vector4i &rhs);

		Int4 x;
		Int4 y;
		Int4 z;
		Int4 w;
	};

	class Vector4u
	{
	public:
		Vector4u();
		Vector4u(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
		Vector4u(const Vector4u &rhs);

		UInt4 &operator[](int i);
		Vector4u &operator=(const Vector4u &rhs);

		UInt4 x;
		UInt4 y;
		UInt4 z;
		UInt4 w;
	};

	class Vector4f
	{
	public:
		Vector4f();
		Vector4f(float x, float y, float z, float w);
		Vector4f(const Vector4f &rhs);

		Float4 &operator[](int i);
		Vector4f &operator=(const Vector4f &rhs);

		Float4 x;
		Float4 y;
		Float4 z;
		Float4 w;
	};

	Float4 exponential2(RValue<Float4> x, bool pp = false);
	Float4 logarithm2(RValue<Float4> x, bool abs, bool pp = false);
	Float4 exponential(RValue<Float4> x, bool pp = false);
	Float4 logarithm(RValue<Float4> x, bool abs, bool pp = false);
	Float4 power(RValue<Float4> x, RValue<Float4> y, bool pp = false);
	Float4 reciprocal(RValue<Float4> x, bool pp = false, bool finite = false);
	Float4 reciprocalSquareRoot(RValue<Float4> x, bool abs, bool pp = false);
	Float4 modulo(RValue<Float4> x, RValue<Float4> y);
	Float4 sine_pi(RValue<Float4> x, bool pp = false);     // limited to [-pi, pi] range
	Float4 cosine_pi(RValue<Float4> x, bool pp = false);   // limited to [-pi, pi] range
	Float4 sine(RValue<Float4> x, bool pp = false);
	Float4 cosine(RValue<Float4> x, bool pp = false);
	Float4 tangent(RValue<Float4> x, bool pp = false);
	Float4 arccos(RValue<Float4> x, bool pp = false);
	Float4 arcsin(RValue<Float4> x, bool pp = false);
	Float4 arctan(RValue<Float4> x, bool pp = false);
	Float4 arctan(RValue<Float4> y, RValue<Float4> x, bool pp = false);
	Float4 sineh(RValue<Float4> x, bool pp = false);
	Float4 cosineh(RValue<Float4> x, bool pp = false);
	Float4 tangenth(RValue<Float4> x, bool pp = false);
	Float4 arccosh(RValue<Float4> x, bool pp = false);  // Limited to x >= 1
	Float4 arcsinh(RValue<Float4> x, bool pp = false);
	Float4 arctanh(RValue<Float4> x, bool pp = false);  // Limited to ]-1, 1[ range

	Float4 dot2(const Vector4f &v0, const Vector4f &v1);
	Float4 dot3(const Vector4f &v0, const Vector4f &v1);
	Float4 dot4(const Vector4f &v0, const Vector4f &v1);

	void transpose4x4(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3);
	void transpose4x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x3(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x2(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x1(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose2x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose2x4h(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4xN(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3, int N);

	class Register
	{
	public:
		Register(const Reference<Float4> &x, const Reference<Float4> &y, const Reference<Float4> &z, const Reference<Float4> &w) : x(x), y(y), z(z), w(w)
		{
		}

		Reference<Float4> &operator[](int i)
		{
			switch(i)
			{
			default:
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			}
		}

		Register &operator=(const Register &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;

			return *this;
		}

		Register &operator=(const Vector4f &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;

			return *this;
		}

		operator Vector4f()
		{
			Vector4f v;

			v.x = x;
			v.y = y;
			v.z = z;
			v.w = w;

			return v;
		}

		Reference<Float4> x;
		Reference<Float4> y;
		Reference<Float4> z;
		Reference<Float4> w;
	};

	template<int S, bool D = false>
	class RegisterArray
	{
	public:
		RegisterArray(bool dynamic = D) : dynamic(dynamic)
		{
			if(dynamic)
			{
				x = new Array<Float4>(S);
				y = new Array<Float4>(S);
				z = new Array<Float4>(S);
				w = new Array<Float4>(S);
			}
			else
			{
				x = new Array<Float4>[S];
				y = new Array<Float4>[S];
				z = new Array<Float4>[S];
				w = new Array<Float4>[S];
			}
		}

		~RegisterArray()
		{
			if(dynamic)
			{
				delete x;
				delete y;
				delete z;
				delete w;
			}
			else
			{
				delete[] x;
				delete[] y;
				delete[] z;
				delete[] w;
			}
		}

		Register operator[](int i)
		{
			if(dynamic)
			{
				return Register(x[0][i], y[0][i], z[0][i], w[0][i]);
			}
			else
			{
				return Register(x[i][0], y[i][0], z[i][0], w[i][0]);
			}
		}

		Register operator[](RValue<Int> i)
		{
			ASSERT(dynamic);

			return Register(x[0][i], y[0][i], z[0][i], w[0][i]);
		}

	private:
		const bool dynamic;
		Array<Float4> *x;
		Array<Float4> *y;
		Array<Float4> *z;
		Array<Float4> *w;
	};

	class ShaderCore
	{
		typedef Shader::Control Control;

	public:
		void mov(Vector4f &dst, const Vector4f &src, bool integerDestination = false);
		void neg(Vector4f &dst, const Vector4f &src);
		void ineg(Vector4f &dst, const Vector4f &src);
		void f2b(Vector4f &dst, const Vector4f &src);
		void b2f(Vector4f &dst, const Vector4f &src);
		void f2i(Vector4f &dst, const Vector4f &src);
		void i2f(Vector4f &dst, const Vector4f &src);
		void f2u(Vector4f &dst, const Vector4f &src);
		void u2f(Vector4f &dst, const Vector4f &src);
		void i2b(Vector4f &dst, const Vector4f &src);
		void b2i(Vector4f &dst, const Vector4f &src);
		void u2b(Vector4f &dst, const Vector4f &src);
		void b2u(Vector4f &dst, const Vector4f &src);
		void add(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void iadd(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void sub(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void isub(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void mad(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void imad(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void mul(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void imul(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void rcpx(Vector4f &dst, const Vector4f &src, bool pp = false);
		void div(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void idiv(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void udiv(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void mod(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void imod(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void umod(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void shl(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void ishr(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void ushr(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void rsqx(Vector4f &dst, const Vector4f &src, bool pp = false);
		void sqrt(Vector4f &dst, const Vector4f &src, bool pp = false);
		void rsq(Vector4f &dst, const Vector4f &src, bool pp = false);
		void len2(Float4 &dst, const Vector4f &src, bool pp = false);
		void len3(Float4 &dst, const Vector4f &src, bool pp = false);
		void len4(Float4 &dst, const Vector4f &src, bool pp = false);
		void dist1(Float4 &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void dist2(Float4 &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void dist3(Float4 &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void dist4(Float4 &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void dp1(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void dp2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void dp2add(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void dp3(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void dp4(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void det2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void det3(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void det4(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2, const Vector4f &src3);
		void min(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void imin(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void umin(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void max(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void imax(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void umax(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void slt(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void step(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void exp2x(Vector4f &dst, const Vector4f &src, bool pp = false);
		void exp2(Vector4f &dst, const Vector4f &src, bool pp = false);
		void exp(Vector4f &dst, const Vector4f &src, bool pp = false);
		void log2x(Vector4f &dst, const Vector4f &src, bool pp = false);
		void log2(Vector4f &dst, const Vector4f &src, bool pp = false);
		void log(Vector4f &dst, const Vector4f &src, bool pp = false);
		void lit(Vector4f &dst, const Vector4f &src);
		void att(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void lrp(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void smooth(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void frc(Vector4f &dst, const Vector4f &src);
		void trunc(Vector4f &dst, const Vector4f &src);
		void floor(Vector4f &dst, const Vector4f &src);
		void round(Vector4f &dst, const Vector4f &src);
		void roundEven(Vector4f &dst, const Vector4f &src);
		void ceil(Vector4f &dst, const Vector4f &src);
		void powx(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void pow(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void crs(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void forward1(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void forward2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void forward3(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void forward4(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void reflect1(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void reflect2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void reflect3(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void reflect4(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void refract1(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Float4 &src2);
		void refract2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Float4 &src2);
		void refract3(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Float4 &src2);
		void refract4(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Float4 &src2);
		void sgn(Vector4f &dst, const Vector4f &src);
		void abs(Vector4f &dst, const Vector4f &src);
		void nrm2(Vector4f &dst, const Vector4f &src, bool pp = false);
		void nrm3(Vector4f &dst, const Vector4f &src, bool pp = false);
		void nrm4(Vector4f &dst, const Vector4f &src, bool pp = false);
		void sincos(Vector4f &dst, const Vector4f &src, bool pp = false);
		void cos(Vector4f &dst, const Vector4f &src, bool pp = false);
		void sin(Vector4f &dst, const Vector4f &src, bool pp = false);
		void tan(Vector4f &dst, const Vector4f &src, bool pp = false);
		void acos(Vector4f &dst, const Vector4f &src, bool pp = false);
		void asin(Vector4f &dst, const Vector4f &src, bool pp = false);
		void atan(Vector4f &dst, const Vector4f &src, bool pp = false);
		void atan2(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, bool pp = false);
		void cosh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void sinh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void tanh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void acosh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void asinh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void atanh(Vector4f &dst, const Vector4f &src, bool pp = false);
		void expp(Vector4f &dst, const Vector4f &src, unsigned short version);
		void logp(Vector4f &dst, const Vector4f &src, unsigned short version);
		void cmp0(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void cmp(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, Control control);
		void icmp(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, Control control);
		void ucmp(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, Control control);
		void select(Vector4f &dst, const Vector4f &src0, const Vector4f &src1, const Vector4f &src2);
		void extract(Float4 &dst, const Vector4f &src0, const Float4 &src1);
		void insert(Vector4f &dst, const Vector4f &src, const Float4 &element, const Float4 &index);
		void all(Float4 &dst, const Vector4f &src);
		void any(Float4 &dst, const Vector4f &src);
		void not(Vector4f &dst, const Vector4f &src);
		void or(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void xor(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void and(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void equal(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);
		void notEqual(Vector4f &dst, const Vector4f &src0, const Vector4f &src1);

	private:
		void sgn(Float4 &dst, const Float4 &src);
		void cmp0(Float4 &dst, const Float4 &src0, const Float4 &src1, const Float4 &src2);
		void cmp0i(Float4 &dst, const Float4 &src0, const Float4 &src1, const Float4 &src2);
		void select(Float4 &dst, RValue<Int4> src0, const Float4 &src1, const Float4 &src2);
	};
}

#endif   // sw_ShaderCore_hpp
