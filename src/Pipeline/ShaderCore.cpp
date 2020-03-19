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

#include "ShaderCore.hpp"

#include "Device/Renderer.hpp"
#include "System/Debug.hpp"

#include <limits.h>

namespace sw {

Vector4s::Vector4s()
{
}

Vector4s::Vector4s(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
{
	this->x = Short4(x);
	this->y = Short4(y);
	this->z = Short4(z);
	this->w = Short4(w);
}

Vector4s::Vector4s(const Vector4s &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
}

Vector4s &Vector4s::operator=(const Vector4s &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}

Short4 &Vector4s::operator[](int i)
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

Vector4f::Vector4f()
{
}

Vector4f::Vector4f(float x, float y, float z, float w)
{
	this->x = Float4(x);
	this->y = Float4(y);
	this->z = Float4(z);
	this->w = Float4(w);
}

Vector4f::Vector4f(const Vector4f &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
}

Vector4f &Vector4f::operator=(const Vector4f &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}

Float4 &Vector4f::operator[](int i)
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

Float4 exponential2(RValue<Float4> x, bool pp)
{
	// This implementation is based on 2^(i + f) = 2^i * 2^f,
	// where i is the integer part of x and f is the fraction.

	// For 2^i we can put the integer part directly in the exponent of
	// the IEEE-754 floating-point number. Clamp to prevent overflow
	// past the representation of infinity.
	Float4 x0 = x;
	x0 = Min(x0, As<Float4>(Int4(0x43010000)));  // 129.00000e+0f
	x0 = Max(x0, As<Float4>(Int4(0xC2FDFFFF)));  // -126.99999e+0f

	Int4 i = RoundInt(x0 - Float4(0.5f));
	Float4 ii = As<Float4>((i + Int4(127)) << 23);  // Add single-precision bias, and shift into exponent.

	// For the fractional part use a polynomial
	// which approximates 2^f in the 0 to 1 range.
	Float4 f = x0 - Float4(i);
	Float4 ff = As<Float4>(Int4(0x3AF61905));    // 1.8775767e-3f
	ff = ff * f + As<Float4>(Int4(0x3C134806));  // 8.9893397e-3f
	ff = ff * f + As<Float4>(Int4(0x3D64AA23));  // 5.5826318e-2f
	ff = ff * f + As<Float4>(Int4(0x3E75EAD4));  // 2.4015361e-1f
	ff = ff * f + As<Float4>(Int4(0x3F31727B));  // 6.9315308e-1f
	ff = ff * f + Float4(1.0f);

	return ii * ff;
}

Float4 logarithm2(RValue<Float4> x, bool pp)
{
	Float4 x0;
	Float4 x1;
	Float4 x2;
	Float4 x3;

	x0 = x;

	x1 = As<Float4>(As<Int4>(x0) & Int4(0x7F800000));
	x1 = As<Float4>(As<UInt4>(x1) >> 8);
	x1 = As<Float4>(As<Int4>(x1) | As<Int4>(Float4(1.0f)));
	x1 = (x1 - Float4(1.4960938f)) * Float4(256.0f);  // FIXME: (x1 - 1.4960938f) * 256.0f;
	x0 = As<Float4>((As<Int4>(x0) & Int4(0x007FFFFF)) | As<Int4>(Float4(1.0f)));

	x2 = (Float4(9.5428179e-2f) * x0 + Float4(4.7779095e-1f)) * x0 + Float4(1.9782813e-1f);
	x3 = ((Float4(1.6618466e-2f) * x0 + Float4(2.0350508e-1f)) * x0 + Float4(2.7382900e-1f)) * x0 + Float4(4.0496687e-2f);
	x2 /= x3;

	x1 += (x0 - Float4(1.0f)) * x2;

	Int4 pos_inf_x = CmpEQ(As<Int4>(x), Int4(0x7F800000));
	return As<Float4>((pos_inf_x & As<Int4>(x)) | (~pos_inf_x & As<Int4>(x1)));
}

Float4 exponential(RValue<Float4> x, bool pp)
{
	// TODO: Propagate the constant
	return exponential2(Float4(1.44269504f) * x, pp);  // 1/ln(2)
}

Float4 logarithm(RValue<Float4> x, bool pp)
{
	// TODO: Propagate the constant
	return Float4(6.93147181e-1f) * logarithm2(x, pp);  // ln(2)
}

Float4 power(RValue<Float4> x, RValue<Float4> y, bool pp)
{
	Float4 log = logarithm2(x, pp);
	log *= y;
	return exponential2(log, pp);
}

Float4 reciprocal(RValue<Float4> x, bool pp, bool finite, bool exactAtPow2)
{
	Float4 rcp = Rcp_pp(x, exactAtPow2);

	if(!pp)
	{
		rcp = (rcp + rcp) - (x * rcp * rcp);
	}

	if(finite)
	{
		int big = 0x7F7FFFFF;
		rcp = Min(rcp, Float4((float &)big));
	}

	return rcp;
}

Float4 reciprocalSquareRoot(RValue<Float4> x, bool absolute, bool pp)
{
	Float4 abs = x;

	if(absolute)
	{
		abs = Abs(abs);
	}

	Float4 rsq;

	if(!pp)
	{
		rsq = Float4(1.0f) / Sqrt(abs);
	}
	else
	{
		rsq = RcpSqrt_pp(abs);

		if(!pp)
		{
			rsq = rsq * (Float4(3.0f) - rsq * rsq * abs) * Float4(0.5f);
		}

		rsq = As<Float4>(CmpNEQ(As<Int4>(abs), Int4(0x7F800000)) & As<Int4>(rsq));
	}

	return rsq;
}

Float4 modulo(RValue<Float4> x, RValue<Float4> y)
{
	return x - y * Floor(x / y);
}

Float4 sine_pi(RValue<Float4> x, bool pp)
{
	const Float4 A = Float4(-4.05284734e-1f);  // -4/pi^2
	const Float4 B = Float4(1.27323954e+0f);   // 4/pi
	const Float4 C = Float4(7.75160950e-1f);
	const Float4 D = Float4(2.24839049e-1f);

	// Parabola approximating sine
	Float4 sin = x * (Abs(x) * A + B);

	// Improve precision from 0.06 to 0.001
	if(true)
	{
		sin = sin * (Abs(sin) * D + C);
	}

	return sin;
}

Float4 cosine_pi(RValue<Float4> x, bool pp)
{
	// cos(x) = sin(x + pi/2)
	Float4 y = x + Float4(1.57079632e+0f);

	// Wrap around
	y -= As<Float4>(CmpNLT(y, Float4(3.14159265e+0f)) & As<Int4>(Float4(6.28318530e+0f)));

	return sine_pi(y, pp);
}

Float4 sine(RValue<Float4> x, bool pp)
{
	// Reduce to [-0.5, 0.5] range
	Float4 y = x * Float4(1.59154943e-1f);  // 1/2pi
	y = y - Round(y);

	if(!pp)
	{
		// From the paper: "A Fast, Vectorizable Algorithm for Producing Single-Precision Sine-Cosine Pairs"
		// This implementation passes OpenGL ES 3.0 precision requirements, at the cost of more operations:
		// !pp : 17 mul, 7 add, 1 sub, 1 reciprocal
		//  pp : 4 mul, 2 add, 2 abs

		Float4 y2 = y * y;
		Float4 c1 = y2 * (y2 * (y2 * Float4(-0.0204391631f) + Float4(0.2536086171f)) + Float4(-1.2336977925f)) + Float4(1.0f);
		Float4 s1 = y * (y2 * (y2 * (y2 * Float4(-0.0046075748f) + Float4(0.0796819754f)) + Float4(-0.645963615f)) + Float4(1.5707963235f));
		Float4 c2 = (c1 * c1) - (s1 * s1);
		Float4 s2 = Float4(2.0f) * s1 * c1;
		return Float4(2.0f) * s2 * c2 * reciprocal(s2 * s2 + c2 * c2, pp, true);
	}

	const Float4 A = Float4(-16.0f);
	const Float4 B = Float4(8.0f);
	const Float4 C = Float4(7.75160950e-1f);
	const Float4 D = Float4(2.24839049e-1f);

	// Parabola approximating sine
	Float4 sin = y * (Abs(y) * A + B);

	// Improve precision from 0.06 to 0.001
	if(true)
	{
		sin = sin * (Abs(sin) * D + C);
	}

	return sin;
}

Float4 cosine(RValue<Float4> x, bool pp)
{
	// cos(x) = sin(x + pi/2)
	Float4 y = x + Float4(1.57079632e+0f);
	return sine(y, pp);
}

Float4 tangent(RValue<Float4> x, bool pp)
{
	return sine(x, pp) / cosine(x, pp);
}

Float4 arccos(RValue<Float4> x, bool pp)
{
	// pi/2 - arcsin(x)
	return Float4(1.57079632e+0f) - arcsin(x);
}

Float4 arcsin(RValue<Float4> x, bool pp)
{
	if(false)  // Simpler implementation fails even lowp precision tests
	{
		// x*(pi/2-sqrt(1-x*x)*pi/5)
		return x * (Float4(1.57079632e+0f) - Sqrt(Float4(1.0f) - x * x) * Float4(6.28318531e-1f));
	}
	else
	{
		// From 4.4.45, page 81 of the Handbook of Mathematical Functions, by Milton Abramowitz and Irene Stegun
		const Float4 half_pi(1.57079632f);
		const Float4 a0(1.5707288f);
		const Float4 a1(-0.2121144f);
		const Float4 a2(0.0742610f);
		const Float4 a3(-0.0187293f);
		Float4 absx = Abs(x);
		return As<Float4>(As<Int4>(half_pi - Sqrt(Float4(1.0f) - absx) * (a0 + absx * (a1 + absx * (a2 + absx * a3)))) ^
		                  (As<Int4>(x) & Int4(0x80000000)));
	}
}

// Approximation of atan in [0..1]
Float4 arctan_01(Float4 x, bool pp)
{
	if(pp)
	{
		return x * (Float4(-0.27f) * x + Float4(1.05539816f));
	}
	else
	{
		// From 4.4.49, page 81 of the Handbook of Mathematical Functions, by Milton Abramowitz and Irene Stegun
		const Float4 a2(-0.3333314528f);
		const Float4 a4(0.1999355085f);
		const Float4 a6(-0.1420889944f);
		const Float4 a8(0.1065626393f);
		const Float4 a10(-0.0752896400f);
		const Float4 a12(0.0429096138f);
		const Float4 a14(-0.0161657367f);
		const Float4 a16(0.0028662257f);
		Float4 x2 = x * x;
		return (x + x * (x2 * (a2 + x2 * (a4 + x2 * (a6 + x2 * (a8 + x2 * (a10 + x2 * (a12 + x2 * (a14 + x2 * a16)))))))));
	}
}

Float4 arctan(RValue<Float4> x, bool pp)
{
	Float4 absx = Abs(x);
	Int4 O = CmpNLT(absx, Float4(1.0f));
	Float4 y = As<Float4>((O & As<Int4>(Float4(1.0f) / absx)) | (~O & As<Int4>(absx)));  // FIXME: Vector select

	const Float4 half_pi(1.57079632f);
	Float4 theta = arctan_01(y, pp);
	return As<Float4>(((O & As<Int4>(half_pi - theta)) | (~O & As<Int4>(theta))) ^  // FIXME: Vector select
	                  (As<Int4>(x) & Int4(0x80000000)));
}

Float4 arctan(RValue<Float4> y, RValue<Float4> x, bool pp)
{
	const Float4 pi(3.14159265f);             // pi
	const Float4 minus_pi(-3.14159265f);      // -pi
	const Float4 half_pi(1.57079632f);        // pi/2
	const Float4 quarter_pi(7.85398163e-1f);  // pi/4

	// Rotate to upper semicircle when in lower semicircle
	Int4 S = CmpLT(y, Float4(0.0f));
	Float4 theta = As<Float4>(S & As<Int4>(minus_pi));
	Float4 x0 = As<Float4>((As<Int4>(y) & Int4(0x80000000)) ^ As<Int4>(x));
	Float4 y0 = Abs(y);

	// Rotate to right quadrant when in left quadrant
	Int4 Q = CmpLT(x0, Float4(0.0f));
	theta += As<Float4>(Q & As<Int4>(half_pi));
	Float4 x1 = As<Float4>((Q & As<Int4>(y0)) | (~Q & As<Int4>(x0)));   // FIXME: Vector select
	Float4 y1 = As<Float4>((Q & As<Int4>(-x0)) | (~Q & As<Int4>(y0)));  // FIXME: Vector select

	// Mirror to first octant when in second octant
	Int4 O = CmpNLT(y1, x1);
	Float4 x2 = As<Float4>((O & As<Int4>(y1)) | (~O & As<Int4>(x1)));  // FIXME: Vector select
	Float4 y2 = As<Float4>((O & As<Int4>(x1)) | (~O & As<Int4>(y1)));  // FIXME: Vector select

	// Approximation of atan in [0..1]
	Int4 zero_x = CmpEQ(x2, Float4(0.0f));
	Int4 inf_y = IsInf(y2);  // Since x2 >= y2, this means x2 == y2 == inf, so we use 45 degrees or pi/4
	Float4 atan2_theta = arctan_01(y2 / x2, pp);
	theta += As<Float4>((~zero_x & ~inf_y & ((O & As<Int4>(half_pi - atan2_theta)) | (~O & (As<Int4>(atan2_theta))))) |  // FIXME: Vector select
	                    (inf_y & As<Int4>(quarter_pi)));

	// Recover loss of precision for tiny theta angles
	Int4 precision_loss = S & Q & O & ~inf_y;                                                            // This combination results in (-pi + half_pi + half_pi - atan2_theta) which is equivalent to -atan2_theta
	return As<Float4>((precision_loss & As<Int4>(-atan2_theta)) | (~precision_loss & As<Int4>(theta)));  // FIXME: Vector select
}

Float4 sineh(RValue<Float4> x, bool pp)
{
	return (exponential(x, pp) - exponential(-x, pp)) * Float4(0.5f);
}

Float4 cosineh(RValue<Float4> x, bool pp)
{
	return (exponential(x, pp) + exponential(-x, pp)) * Float4(0.5f);
}

Float4 tangenth(RValue<Float4> x, bool pp)
{
	Float4 e_x = exponential(x, pp);
	Float4 e_minus_x = exponential(-x, pp);
	return (e_x - e_minus_x) / (e_x + e_minus_x);
}

Float4 arccosh(RValue<Float4> x, bool pp)
{
	return logarithm(x + Sqrt(x + Float4(1.0f)) * Sqrt(x - Float4(1.0f)), pp);
}

Float4 arcsinh(RValue<Float4> x, bool pp)
{
	return logarithm(x + Sqrt(x * x + Float4(1.0f)), pp);
}

Float4 arctanh(RValue<Float4> x, bool pp)
{
	return logarithm((Float4(1.0f) + x) / (Float4(1.0f) - x), pp) * Float4(0.5f);
}

Float4 dot2(const Vector4f &v0, const Vector4f &v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}

Float4 dot3(const Vector4f &v0, const Vector4f &v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

Float4 dot4(const Vector4f &v0, const Vector4f &v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
}

void transpose4x4(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3)
{
	Int2 tmp0 = UnpackHigh(row0, row1);
	Int2 tmp1 = UnpackHigh(row2, row3);
	Int2 tmp2 = UnpackLow(row0, row1);
	Int2 tmp3 = UnpackLow(row2, row3);

	row0 = UnpackLow(tmp2, tmp3);
	row1 = UnpackHigh(tmp2, tmp3);
	row2 = UnpackLow(tmp0, tmp1);
	row3 = UnpackHigh(tmp0, tmp1);
}

void transpose4x3(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3)
{
	Int2 tmp0 = UnpackHigh(row0, row1);
	Int2 tmp1 = UnpackHigh(row2, row3);
	Int2 tmp2 = UnpackLow(row0, row1);
	Int2 tmp3 = UnpackLow(row2, row3);

	row0 = UnpackLow(tmp2, tmp3);
	row1 = UnpackHigh(tmp2, tmp3);
	row2 = UnpackLow(tmp0, tmp1);
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
	Float4 tmp01 = UnpackLow(row0, row1);
	Float4 tmp23 = UnpackHigh(row0, row1);

	row0 = tmp01;
	row1 = Float4(tmp01.zw, row1.zw);
	row2 = tmp23;
	row3 = Float4(tmp23.zw, row3.zw);
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

SIMD::UInt halfToFloatBits(SIMD::UInt halfBits)
{
	auto magic = SIMD::UInt(126 << 23);

	auto sign16 = halfBits & SIMD::UInt(0x8000);
	auto man16 = halfBits & SIMD::UInt(0x03FF);
	auto exp16 = halfBits & SIMD::UInt(0x7C00);

	auto isDnormOrZero = CmpEQ(exp16, SIMD::UInt(0));
	auto isInfOrNaN = CmpEQ(exp16, SIMD::UInt(0x7C00));

	auto sign32 = sign16 << 16;
	auto man32 = man16 << 13;
	auto exp32 = (exp16 + SIMD::UInt(0x1C000)) << 13;
	auto norm32 = (man32 | exp32) | (isInfOrNaN & SIMD::UInt(0x7F800000));

	auto denorm32 = As<SIMD::UInt>(As<SIMD::Float>(magic + man16) - As<SIMD::Float>(magic));

	return sign32 | (norm32 & ~isDnormOrZero) | (denorm32 & isDnormOrZero);
}

SIMD::UInt floatToHalfBits(SIMD::UInt floatBits, bool storeInUpperBits)
{
	static const uint32_t mask_sign = 0x80000000u;
	static const uint32_t mask_round = ~0xfffu;
	static const uint32_t c_f32infty = 255 << 23;
	static const uint32_t c_magic = 15 << 23;
	static const uint32_t c_nanbit = 0x200;
	static const uint32_t c_infty_as_fp16 = 0x7c00;
	static const uint32_t c_clamp = (31 << 23) - 0x1000;

	SIMD::UInt justsign = SIMD::UInt(mask_sign) & floatBits;
	SIMD::UInt absf = floatBits ^ justsign;
	SIMD::UInt b_isnormal = CmpNLE(SIMD::UInt(c_f32infty), absf);

	// Note: this version doesn't round to the nearest even in case of a tie as defined by IEEE 754-2008, it rounds to +inf
	//       instead of nearest even, since that's fine for GLSL ES 3.0's needs (see section 2.1.1 Floating-Point Computation)
	SIMD::UInt joined = ((((As<SIMD::UInt>(Min(As<SIMD::Float>(absf & SIMD::UInt(mask_round)) * As<SIMD::Float>(SIMD::UInt(c_magic)),
	                                           As<SIMD::Float>(SIMD::UInt(c_clamp))))) -
	                       SIMD::UInt(mask_round)) >>
	                      13) &
	                     b_isnormal) |
	                    ((b_isnormal ^ SIMD::UInt(0xFFFFFFFF)) &
	                     ((CmpNLE(absf, SIMD::UInt(c_f32infty)) & SIMD::UInt(c_nanbit)) | SIMD::UInt(c_infty_as_fp16)));

	return storeInUpperBits ? ((joined << 16) | justsign) : joined | (justsign >> 16);
}

Float4 r11g11b10Unpack(UInt r11g11b10bits)
{
	// 10 (or 11) bit float formats are unsigned formats with a 5 bit exponent and a 5 (or 6) bit mantissa.
	// Since the Half float format also has a 5 bit exponent, we can convert these formats to half by
	// copy/pasting the bits so the the exponent bits and top mantissa bits are aligned to the half format.
	// In this case, we have:
	// MSB | B B B B B B B B B B G G G G G G G G G G G R R R R R R R R R R R | LSB
	UInt4 halfBits;
	halfBits = Insert(halfBits, (r11g11b10bits & UInt(0x000007FFu)) << 4, 0);
	halfBits = Insert(halfBits, (r11g11b10bits & UInt(0x003FF800u)) >> 7, 1);
	halfBits = Insert(halfBits, (r11g11b10bits & UInt(0xFFC00000u)) >> 17, 2);
	halfBits = Insert(halfBits, UInt(0x00003C00u), 3);
	return As<Float4>(halfToFloatBits(halfBits));
}

UInt r11g11b10Pack(const Float4 &value)
{
	// 10 and 11 bit floats are unsigned, so their minimal value is 0
	auto halfBits = floatToHalfBits(As<UInt4>(Max(value, Float4(0.0f))), true);
	// Truncates instead of rounding. See b/147900455
	UInt4 truncBits = halfBits & UInt4(0x7FF00000, 0x7FF00000, 0x7FE00000, 0);
	return (UInt(truncBits.x) >> 20) | (UInt(truncBits.y) >> 9) | (UInt(truncBits.z) << 1);
}

Vector4s a2b10g10r10Unpack(const Int4 &value)
{
	Vector4s result;

	result.x = Short4(value << 6) & Short4(0xFFC0u);
	result.y = Short4(value >> 4) & Short4(0xFFC0u);
	result.z = Short4(value >> 14) & Short4(0xFFC0u);
	result.w = Short4(value >> 16) & Short4(0xC000u);

	// Expand to 16 bit range
	result.x |= As<Short4>(As<UShort4>(result.x) >> 10);
	result.y |= As<Short4>(As<UShort4>(result.y) >> 10);
	result.z |= As<Short4>(As<UShort4>(result.z) >> 10);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 2);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 4);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 8);

	return result;
}

Vector4s a2r10g10b10Unpack(const Int4 &value)
{
	Vector4s result;

	result.x = Short4(value >> 14) & Short4(0xFFC0u);
	result.y = Short4(value >> 4) & Short4(0xFFC0u);
	result.z = Short4(value << 6) & Short4(0xFFC0u);
	result.w = Short4(value >> 16) & Short4(0xC000u);

	// Expand to 16 bit range
	result.x |= As<Short4>(As<UShort4>(result.x) >> 10);
	result.y |= As<Short4>(As<UShort4>(result.y) >> 10);
	result.z |= As<Short4>(As<UShort4>(result.z) >> 10);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 2);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 4);
	result.w |= As<Short4>(As<UShort4>(result.w) >> 8);

	return result;
}

rr::RValue<rr::Bool> AnyTrue(rr::RValue<sw::SIMD::Int> const &ints)
{
	return rr::SignMask(ints) != 0;
}

rr::RValue<rr::Bool> AnyFalse(rr::RValue<sw::SIMD::Int> const &ints)
{
	return rr::SignMask(~ints) != 0;
}

rr::RValue<sw::SIMD::Float> Sign(rr::RValue<sw::SIMD::Float> const &val)
{
	return rr::As<sw::SIMD::Float>((rr::As<sw::SIMD::UInt>(val) & sw::SIMD::UInt(0x80000000)) | sw::SIMD::UInt(0x3f800000));
}

// Returns the <whole, frac> of val.
// Both whole and frac will have the same sign as val.
std::pair<rr::RValue<sw::SIMD::Float>, rr::RValue<sw::SIMD::Float>>
Modf(rr::RValue<sw::SIMD::Float> const &val)
{
	auto abs = Abs(val);
	auto sign = Sign(val);
	auto whole = Floor(abs) * sign;
	auto frac = Frac(abs) * sign;
	return std::make_pair(whole, frac);
}

// Returns the number of 1s in bits, per lane.
sw::SIMD::UInt CountBits(rr::RValue<sw::SIMD::UInt> const &bits)
{
	// TODO: Add an intrinsic to reactor. Even if there isn't a
	// single vector instruction, there may be target-dependent
	// ways to make this faster.
	// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	sw::SIMD::UInt c = bits - ((bits >> 1) & sw::SIMD::UInt(0x55555555));
	c = ((c >> 2) & sw::SIMD::UInt(0x33333333)) + (c & sw::SIMD::UInt(0x33333333));
	c = ((c >> 4) + c) & sw::SIMD::UInt(0x0F0F0F0F);
	c = ((c >> 8) + c) & sw::SIMD::UInt(0x00FF00FF);
	c = ((c >> 16) + c) & sw::SIMD::UInt(0x0000FFFF);
	return c;
}

// Returns 1 << bits.
// If the resulting bit overflows a 32 bit integer, 0 is returned.
rr::RValue<sw::SIMD::UInt> NthBit32(rr::RValue<sw::SIMD::UInt> const &bits)
{
	return ((sw::SIMD::UInt(1) << bits) & rr::CmpLT(bits, sw::SIMD::UInt(32)));
}

// Returns bitCount number of of 1's starting from the LSB.
rr::RValue<sw::SIMD::UInt> Bitmask32(rr::RValue<sw::SIMD::UInt> const &bitCount)
{
	return NthBit32(bitCount) - sw::SIMD::UInt(1);
}

// Performs a fused-multiply add, returning a * b + c.
rr::RValue<sw::SIMD::Float> FMA(
    rr::RValue<sw::SIMD::Float> const &a,
    rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c)
{
	return a * b + c;
}

// Returns the exponent of the floating point number f.
// Assumes IEEE 754
rr::RValue<sw::SIMD::Int> Exponent(rr::RValue<sw::SIMD::Float> f)
{
	auto v = rr::As<sw::SIMD::UInt>(f);
	return (sw::SIMD::Int((v >> sw::SIMD::UInt(23)) & sw::SIMD::UInt(0xFF)) - sw::SIMD::Int(126));
}

// Returns y if y < x; otherwise result is x.
// If one operand is a NaN, the other operand is the result.
// If both operands are NaN, the result is a NaN.
rr::RValue<sw::SIMD::Float> NMin(rr::RValue<sw::SIMD::Float> const &x, rr::RValue<sw::SIMD::Float> const &y)
{
	using namespace rr;
	auto xIsNan = IsNan(x);
	auto yIsNan = IsNan(y);
	return As<sw::SIMD::Float>(
	    // If neither are NaN, return min
	    ((~xIsNan & ~yIsNan) & As<sw::SIMD::Int>(Min(x, y))) |
	    // If one operand is a NaN, the other operand is the result
	    // If both operands are NaN, the result is a NaN.
	    ((~xIsNan & yIsNan) & As<sw::SIMD::Int>(x)) |
	    (xIsNan & As<sw::SIMD::Int>(y)));
}

// Returns y if y > x; otherwise result is x.
// If one operand is a NaN, the other operand is the result.
// If both operands are NaN, the result is a NaN.
rr::RValue<sw::SIMD::Float> NMax(rr::RValue<sw::SIMD::Float> const &x, rr::RValue<sw::SIMD::Float> const &y)
{
	using namespace rr;
	auto xIsNan = IsNan(x);
	auto yIsNan = IsNan(y);
	return As<sw::SIMD::Float>(
	    // If neither are NaN, return max
	    ((~xIsNan & ~yIsNan) & As<sw::SIMD::Int>(Max(x, y))) |
	    // If one operand is a NaN, the other operand is the result
	    // If both operands are NaN, the result is a NaN.
	    ((~xIsNan & yIsNan) & As<sw::SIMD::Int>(x)) |
	    (xIsNan & As<sw::SIMD::Int>(y)));
}

// Returns the determinant of a 2x2 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d)
{
	return a * d - b * c;
}

// Returns the determinant of a 3x3 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c,
    rr::RValue<sw::SIMD::Float> const &d, rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f,
    rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h, rr::RValue<sw::SIMD::Float> const &i)
{
	return a * e * i + b * f * g + c * d * h - c * e * g - b * d * i - a * f * h;
}

// Returns the determinant of a 4x4 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d,
    rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f, rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h,
    rr::RValue<sw::SIMD::Float> const &i, rr::RValue<sw::SIMD::Float> const &j, rr::RValue<sw::SIMD::Float> const &k, rr::RValue<sw::SIMD::Float> const &l,
    rr::RValue<sw::SIMD::Float> const &m, rr::RValue<sw::SIMD::Float> const &n, rr::RValue<sw::SIMD::Float> const &o, rr::RValue<sw::SIMD::Float> const &p)
{
	return a * Determinant(f, g, h,
	                       j, k, l,
	                       n, o, p) -
	       b * Determinant(e, g, h,
	                       i, k, l,
	                       m, o, p) +
	       c * Determinant(e, f, h,
	                       i, j, l,
	                       m, n, p) -
	       d * Determinant(e, f, g,
	                       i, j, k,
	                       m, n, o);
}

// Returns the inverse of a 2x2 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 4> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d)
{
	auto s = sw::SIMD::Float(1.0f) / Determinant(a, b, c, d);
	return { { s * d, -s * b, -s * c, s * a } };
}

// Returns the inverse of a 3x3 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 9> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c,
    rr::RValue<sw::SIMD::Float> const &d, rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f,
    rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h, rr::RValue<sw::SIMD::Float> const &i)
{
	auto s = sw::SIMD::Float(1.0f) / Determinant(
	                                     a, b, c,
	                                     d, e, f,
	                                     g, h, i);  // TODO: duplicate arithmetic calculating the det and below.

	return { {
		s * (e * i - f * h),
		s * (c * h - b * i),
		s * (b * f - c * e),
		s * (f * g - d * i),
		s * (a * i - c * g),
		s * (c * d - a * f),
		s * (d * h - e * g),
		s * (b * g - a * h),
		s * (a * e - b * d),
	} };
}

// Returns the inverse of a 4x4 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 16> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d,
    rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f, rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h,
    rr::RValue<sw::SIMD::Float> const &i, rr::RValue<sw::SIMD::Float> const &j, rr::RValue<sw::SIMD::Float> const &k, rr::RValue<sw::SIMD::Float> const &l,
    rr::RValue<sw::SIMD::Float> const &m, rr::RValue<sw::SIMD::Float> const &n, rr::RValue<sw::SIMD::Float> const &o, rr::RValue<sw::SIMD::Float> const &p)
{
	auto s = sw::SIMD::Float(1.0f) / Determinant(
	                                     a, b, c, d,
	                                     e, f, g, h,
	                                     i, j, k, l,
	                                     m, n, o, p);  // TODO: duplicate arithmetic calculating the det and below.

	auto kplo = k * p - l * o, jpln = j * p - l * n, jokn = j * o - k * n;
	auto gpho = g * p - h * o, fphn = f * p - h * n, fogn = f * o - g * n;
	auto glhk = g * l - h * k, flhj = f * l - h * j, fkgj = f * k - g * j;
	auto iplm = i * p - l * m, iokm = i * o - k * m, ephm = e * p - h * m;
	auto eogm = e * o - g * m, elhi = e * l - h * i, ekgi = e * k - g * i;
	auto injm = i * n - j * m, enfm = e * n - f * m, ejfi = e * j - f * i;

	return { {
		s * (f * kplo - g * jpln + h * jokn),
		s * (-b * kplo + c * jpln - d * jokn),
		s * (b * gpho - c * fphn + d * fogn),
		s * (-b * glhk + c * flhj - d * fkgj),

		s * (-e * kplo + g * iplm - h * iokm),
		s * (a * kplo - c * iplm + d * iokm),
		s * (-a * gpho + c * ephm - d * eogm),
		s * (a * glhk - c * elhi + d * ekgi),

		s * (e * jpln - f * iplm + h * injm),
		s * (-a * jpln + b * iplm - d * injm),
		s * (a * fphn - b * ephm + d * enfm),
		s * (-a * flhj + b * elhi - d * ejfi),

		s * (-e * jokn + f * iokm - g * injm),
		s * (a * jokn - b * iokm + c * injm),
		s * (-a * fogn + b * eogm - c * enfm),
		s * (a * fkgj - b * ekgi + c * ejfi),
	} };
}

namespace SIMD {

Pointer::Pointer(rr::Pointer<Byte> base, rr::Int limit)
    : base(base)
    , dynamicLimit(limit)
    , staticLimit(0)
    , dynamicOffsets(0)
    , staticOffsets{}
    , hasDynamicLimit(true)
    , hasDynamicOffsets(false)
{}

Pointer::Pointer(rr::Pointer<Byte> base, unsigned int limit)
    : base(base)
    , dynamicLimit(0)
    , staticLimit(limit)
    , dynamicOffsets(0)
    , staticOffsets{}
    , hasDynamicLimit(false)
    , hasDynamicOffsets(false)
{}

Pointer::Pointer(rr::Pointer<Byte> base, rr::Int limit, SIMD::Int offset)
    : base(base)
    , dynamicLimit(limit)
    , staticLimit(0)
    , dynamicOffsets(offset)
    , staticOffsets{}
    , hasDynamicLimit(true)
    , hasDynamicOffsets(true)
{}

Pointer::Pointer(rr::Pointer<Byte> base, unsigned int limit, SIMD::Int offset)
    : base(base)
    , dynamicLimit(0)
    , staticLimit(limit)
    , dynamicOffsets(offset)
    , staticOffsets{}
    , hasDynamicLimit(false)
    , hasDynamicOffsets(true)
{}

Pointer &Pointer::operator+=(Int i)
{
	dynamicOffsets += i;
	hasDynamicOffsets = true;
	return *this;
}

Pointer &Pointer::operator*=(Int i)
{
	dynamicOffsets = offsets() * i;
	staticOffsets = {};
	hasDynamicOffsets = true;
	return *this;
}

Pointer Pointer::operator+(SIMD::Int i)
{
	Pointer p = *this;
	p += i;
	return p;
}
Pointer Pointer::operator*(SIMD::Int i)
{
	Pointer p = *this;
	p *= i;
	return p;
}

Pointer &Pointer::operator+=(int i)
{
	for(int el = 0; el < SIMD::Width; el++) { staticOffsets[el] += i; }
	return *this;
}

Pointer &Pointer::operator*=(int i)
{
	for(int el = 0; el < SIMD::Width; el++) { staticOffsets[el] *= i; }
	if(hasDynamicOffsets)
	{
		dynamicOffsets *= SIMD::Int(i);
	}
	return *this;
}

Pointer Pointer::operator+(int i)
{
	Pointer p = *this;
	p += i;
	return p;
}
Pointer Pointer::operator*(int i)
{
	Pointer p = *this;
	p *= i;
	return p;
}

SIMD::Int Pointer::offsets() const
{
	static_assert(SIMD::Width == 4, "Expects SIMD::Width to be 4");
	return dynamicOffsets + SIMD::Int(staticOffsets[0], staticOffsets[1], staticOffsets[2], staticOffsets[3]);
}

SIMD::Int Pointer::isInBounds(unsigned int accessSize, OutOfBoundsBehavior robustness) const
{
	ASSERT(accessSize > 0);

	if(isStaticallyInBounds(accessSize, robustness))
	{
		return SIMD::Int(0xffffffff);
	}

	if(!hasDynamicOffsets && !hasDynamicLimit)
	{
		// Common fast paths.
		static_assert(SIMD::Width == 4, "Expects SIMD::Width to be 4");
		return SIMD::Int(
		    (staticOffsets[0] + accessSize - 1 < staticLimit) ? 0xffffffff : 0,
		    (staticOffsets[1] + accessSize - 1 < staticLimit) ? 0xffffffff : 0,
		    (staticOffsets[2] + accessSize - 1 < staticLimit) ? 0xffffffff : 0,
		    (staticOffsets[3] + accessSize - 1 < staticLimit) ? 0xffffffff : 0);
	}

	return CmpLT(offsets() + SIMD::Int(accessSize - 1), SIMD::Int(limit()));
}

bool Pointer::isStaticallyInBounds(unsigned int accessSize, OutOfBoundsBehavior robustness) const
{
	if(hasDynamicOffsets)
	{
		return false;
	}

	if(hasDynamicLimit)
	{
		if(hasStaticEqualOffsets() || hasStaticSequentialOffsets(accessSize))
		{
			switch(robustness)
			{
				case OutOfBoundsBehavior::UndefinedBehavior:
					// With this robustness setting the application/compiler guarantees in-bounds accesses on active lanes,
					// but since it can't know in advance which branches are taken this must be true even for inactives lanes.
					return true;
				case OutOfBoundsBehavior::Nullify:
				case OutOfBoundsBehavior::RobustBufferAccess:
				case OutOfBoundsBehavior::UndefinedValue:
					return false;
			}
		}
	}

	for(int i = 0; i < SIMD::Width; i++)
	{
		if(staticOffsets[i] + accessSize - 1 >= staticLimit)
		{
			return false;
		}
	}

	return true;
}

Int Pointer::limit() const
{
	return dynamicLimit + staticLimit;
}

// Returns true if all offsets are sequential
// (N+0*step, N+1*step, N+2*step, N+3*step)
rr::Bool Pointer::hasSequentialOffsets(unsigned int step) const
{
	if(hasDynamicOffsets)
	{
		auto o = offsets();
		static_assert(SIMD::Width == 4, "Expects SIMD::Width to be 4");
		return rr::SignMask(~CmpEQ(o.yzww, o + SIMD::Int(1 * step, 2 * step, 3 * step, 0))) == 0;
	}
	return hasStaticSequentialOffsets(step);
}

// Returns true if all offsets are are compile-time static and
// sequential (N+0*step, N+1*step, N+2*step, N+3*step)
bool Pointer::hasStaticSequentialOffsets(unsigned int step) const
{
	if(hasDynamicOffsets)
	{
		return false;
	}
	for(int i = 1; i < SIMD::Width; i++)
	{
		if(staticOffsets[i - 1] + int32_t(step) != staticOffsets[i]) { return false; }
	}
	return true;
}

// Returns true if all offsets are equal (N, N, N, N)
rr::Bool Pointer::hasEqualOffsets() const
{
	if(hasDynamicOffsets)
	{
		auto o = offsets();
		static_assert(SIMD::Width == 4, "Expects SIMD::Width to be 4");
		return rr::SignMask(~CmpEQ(o, o.yzwx)) == 0;
	}
	return hasStaticEqualOffsets();
}

// Returns true if all offsets are compile-time static and are equal
// (N, N, N, N)
bool Pointer::hasStaticEqualOffsets() const
{
	if(hasDynamicOffsets)
	{
		return false;
	}
	for(int i = 1; i < SIMD::Width; i++)
	{
		if(staticOffsets[i - 1] != staticOffsets[i]) { return false; }
	}
	return true;
}

}  // namespace SIMD

}  // namespace sw
