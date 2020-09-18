// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "OptimalIntrinsics.hpp"

namespace rr {
namespace {
Float4 Reciprocal(RValue<Float4> x, bool pp = false, bool finite = false, bool exactAtPow2 = false)
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

Float4 SinOrCos(RValue<Float4> x, bool sin)
{
	// Reduce to [-0.5, 0.5] range
	Float4 y = x * Float4(1.59154943e-1f);  // 1/2pi
	y = y - Round(y);

	// From the paper: "A Fast, Vectorizable Algorithm for Producing Single-Precision Sine-Cosine Pairs"
	// This implementation passes OpenGL ES 3.0 precision requirements, at the cost of more operations:
	// !pp : 17 mul, 7 add, 1 sub, 1 reciprocal
	//  pp : 4 mul, 2 add, 2 abs

	Float4 y2 = y * y;
	Float4 c1 = y2 * (y2 * (y2 * Float4(-0.0204391631f) + Float4(0.2536086171f)) + Float4(-1.2336977925f)) + Float4(1.0f);
	Float4 s1 = y * (y2 * (y2 * (y2 * Float4(-0.0046075748f) + Float4(0.0796819754f)) + Float4(-0.645963615f)) + Float4(1.5707963235f));
	Float4 c2 = (c1 * c1) - (s1 * s1);
	Float4 s2 = Float4(2.0f) * s1 * c1;
	Float4 r = Reciprocal(s2 * s2 + c2 * c2, false, true, false);

	if(sin)
	{
		return Float4(2.0f) * s2 * c2 * r;
	}
	else
	{
		return ((c2 * c2) - (s2 * s2)) * r;
	}
}

// Approximation of atan in [0..1]
Float4 Atan_01(Float4 x)
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
}  // namespace

namespace optimal {

Float4 Sin(RValue<Float4> x)
{
	return SinOrCos(x, true);
}

Float4 Cos(RValue<Float4> x)
{
	return SinOrCos(x, false);
}

Float4 Tan(RValue<Float4> x)
{
	return SinOrCos(x, true) / SinOrCos(x, false);
}

Float4 Asin_4_terms(RValue<Float4> x)
{
	// From 4.4.45, page 81 of the Handbook of Mathematical Functions, by Milton Abramowitz and Irene Stegun
	// |e(x)| <= 5e-8
	const Float4 half_pi(1.57079632f);
	const Float4 a0(1.5707288f);
	const Float4 a1(-0.2121144f);
	const Float4 a2(0.0742610f);
	const Float4 a3(-0.0187293f);
	Float4 absx = Abs(x);
	return As<Float4>(As<Int4>(half_pi - Sqrt(Float4(1.0f) - absx) * (a0 + absx * (a1 + absx * (a2 + absx * a3)))) ^
	                  (As<Int4>(x) & Int4(0x80000000)));
}

Float4 Asin_8_terms(RValue<Float4> x)
{
	// From 4.4.46, page 81 of the Handbook of Mathematical Functions, by Milton Abramowitz and Irene Stegun
	// |e(x)| <= 0e-8
	const Float4 half_pi(1.5707963268f);
	const Float4 a0(1.5707963050f);
	const Float4 a1(-0.2145988016f);
	const Float4 a2(0.0889789874f);
	const Float4 a3(-0.0501743046f);
	const Float4 a4(0.0308918810f);
	const Float4 a5(-0.0170881256f);
	const Float4 a6(0.006700901f);
	const Float4 a7(-0.0012624911f);
	Float4 absx = Abs(x);
	return As<Float4>(As<Int4>(half_pi - Sqrt(Float4(1.0f) - absx) * (a0 + absx * (a1 + absx * (a2 + absx * (a3 + absx * (a4 + absx * (a5 + absx * (a6 + absx * a7)))))))) ^
	                  (As<Int4>(x) & Int4(0x80000000)));
}

Float4 Acos_4_terms(RValue<Float4> x)
{
	// pi/2 - arcsin(x)
	return Float4(1.57079632e+0f) - Asin_4_terms(x);
}

Float4 Acos_8_terms(RValue<Float4> x)
{
	// pi/2 - arcsin(x)
	return Float4(1.57079632e+0f) - Asin_8_terms(x);
}

Float4 Atan(RValue<Float4> x)
{
	Float4 absx = Abs(x);
	Int4 O = CmpNLT(absx, Float4(1.0f));
	Float4 y = As<Float4>((O & As<Int4>(Float4(1.0f) / absx)) | (~O & As<Int4>(absx)));  // FIXME: Vector select

	const Float4 half_pi(1.57079632f);
	Float4 theta = Atan_01(y);
	return As<Float4>(((O & As<Int4>(half_pi - theta)) | (~O & As<Int4>(theta))) ^  // FIXME: Vector select
	                  (As<Int4>(x) & Int4(0x80000000)));
}

Float4 Atan2(RValue<Float4> y, RValue<Float4> x)
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
	Float4 atan2_theta = Atan_01(y2 / x2);
	theta += As<Float4>((~zero_x & ~inf_y & ((O & As<Int4>(half_pi - atan2_theta)) | (~O & (As<Int4>(atan2_theta))))) |  // FIXME: Vector select
	                    (inf_y & As<Int4>(quarter_pi)));

	// Recover loss of precision for tiny theta angles
	// This combination results in (-pi + half_pi + half_pi - atan2_theta) which is equivalent to -atan2_theta
	Int4 precision_loss = S & Q & O & ~inf_y;

	return As<Float4>((precision_loss & As<Int4>(-atan2_theta)) | (~precision_loss & As<Int4>(theta)));  // FIXME: Vector select
}

Float4 Exp2(RValue<Float4> x)
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

Float4 Log2(RValue<Float4> x)
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

Float4 Exp(RValue<Float4> x)
{
	// TODO: Propagate the constant
	return optimal::Exp2(Float4(1.44269504f) * x);  // 1/ln(2)
}

Float4 Log(RValue<Float4> x)
{
	// TODO: Propagate the constant
	return Float4(6.93147181e-1f) * optimal::Log2(x);  // ln(2)
}

Float4 Pow(RValue<Float4> x, RValue<Float4> y)
{
	Float4 log = optimal::Log2(x);
	log *= y;
	return optimal::Exp2(log);
}

Float4 Sinh(RValue<Float4> x)
{
	return (optimal::Exp(x) - optimal::Exp(-x)) * Float4(0.5f);
}

Float4 Cosh(RValue<Float4> x)
{
	return (optimal::Exp(x) + optimal::Exp(-x)) * Float4(0.5f);
}

Float4 Tanh(RValue<Float4> x)
{
	Float4 e_x = optimal::Exp(x);
	Float4 e_minus_x = optimal::Exp(-x);
	return (e_x - e_minus_x) / (e_x + e_minus_x);
}

Float4 Asinh(RValue<Float4> x)
{
	return optimal::Log(x + Sqrt(x * x + Float4(1.0f)));
}

Float4 Acosh(RValue<Float4> x)
{
	return optimal::Log(x + Sqrt(x + Float4(1.0f)) * Sqrt(x - Float4(1.0f)));
}

Float4 Atanh(RValue<Float4> x)
{
	return optimal::Log((Float4(1.0f) + x) / (Float4(1.0f) - x)) * Float4(0.5f);
}

}  // namespace optimal
}  // namespace rr