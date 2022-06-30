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

#ifndef sw_ShaderCore_hpp
#define sw_ShaderCore_hpp

#include "Reactor/Print.hpp"
#include "Reactor/Reactor.hpp"
#include "Reactor/SIMD.hpp"
#include "System/Debug.hpp"

#include <array>
#include <atomic>   // std::memory_order
#include <utility>  // std::pair

namespace sw {

using namespace rr;

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

// SIMD contains types that represent multiple scalars packed into a single
// vector data type. Types in the SIMD namespace provide a semantic hint
// that the data should be treated as a per-execution-lane scalar instead of
// a typical euclidean-style vector type.
namespace SIMD {

// Width is the number of per-lane scalars packed into each SIMD vector.
static constexpr int Width = 4;

using Float = rr::Float4;
using Int = rr::Int4;
using UInt = rr::UInt4;
using Pointer = rr::Pointer4;

}  // namespace SIMD

// Vulkan 'SPIR-V Extended Instructions for GLSL' (GLSL.std.450) compliant transcendental functions
RValue<Float4> Sin(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Cos(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Tan(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Asin(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Acos(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Atan(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Atan2(RValue<Float4> y, RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Exp2(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Log2(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Exp(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Log(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y, bool relaxedPrecision);
RValue<Float4> Sinh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Cosh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Tanh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Asinh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Acosh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Atanh(RValue<Float4> x, bool relaxedPrecision);
RValue<Float4> Sqrt(RValue<Float4> x, bool relaxedPrecision);

// Math functions with uses outside of shaders can be invoked using a verbose template argument instead
// of a Boolean argument to indicate precision. For example Sqrt<Mediump>(x) equals Sqrt(x, true).
enum Precision
{
	Highp,
	Relaxed,
	Mediump = Relaxed,  // GLSL defines mediump and lowp as corresponding with SPIR-V's RelaxedPrecision
};

// clang-format off
template<Precision precision> RValue<Float4> Sqrt(RValue<Float4> x);
template<> inline RValue<Float4> Sqrt<Highp>(RValue<Float4> x) { return Sqrt(x, false); }
template<> inline RValue<Float4> Sqrt<Mediump>(RValue<Float4> x) { return Sqrt(x, true); }

template<Precision precision> RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y);
template<> inline RValue<Float4> Pow<Highp>(RValue<Float4> x, RValue<Float4> y) { return Pow(x, y, false); }
template<> inline RValue<Float4> Pow<Mediump>(RValue<Float4> x, RValue<Float4> y) { return Pow(x, y, true); }
// clang-format on

RValue<Float4> reciprocal(RValue<Float4> x, bool pp = false, bool exactAtPow2 = false);
RValue<Float4> reciprocalSquareRoot(RValue<Float4> x, bool abs, bool pp = false);

RValue<Float4> mulAdd(RValue<Float4> x, RValue<Float4> y, RValue<Float4> z);  // TODO(chromium:1299047)

void transpose4x4(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3);
void transpose4x3(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3);
void transpose4x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
void transpose4x3(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
void transpose4x2(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
void transpose4x1(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
void transpose2x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
void transpose4xN(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3, int N);

sw::SIMD::UInt halfToFloatBits(sw::SIMD::UInt halfBits);
sw::SIMD::UInt floatToHalfBits(sw::SIMD::UInt floatBits, bool storeInUpperBits);
Float4 r11g11b10Unpack(UInt r11g11b10bits);
UInt r11g11b10Pack(const Float4 &value);
Float4 linearToSRGB(const Float4 &c);
Float4 sRGBtoLinear(const Float4 &c);

template<typename T>
inline rr::RValue<T> AndAll(rr::RValue<T> const &mask);

template<typename T>
inline rr::RValue<T> OrAll(rr::RValue<T> const &mask);

rr::RValue<sw::SIMD::Float> Sign(rr::RValue<sw::SIMD::Float> const &val);

// Returns the <whole, frac> of val.
// Both whole and frac will have the same sign as val.
std::pair<rr::RValue<sw::SIMD::Float>, rr::RValue<sw::SIMD::Float>>
Modf(rr::RValue<sw::SIMD::Float> const &val);

// Returns the number of 1s in bits, per lane.
sw::SIMD::UInt CountBits(rr::RValue<sw::SIMD::UInt> const &bits);

// Returns 1 << bits.
// If the resulting bit overflows a 32 bit integer, 0 is returned.
rr::RValue<sw::SIMD::UInt> NthBit32(rr::RValue<sw::SIMD::UInt> const &bits);

// Returns bitCount number of of 1's starting from the LSB.
rr::RValue<sw::SIMD::UInt> Bitmask32(rr::RValue<sw::SIMD::UInt> const &bitCount);

// Computes `a * b + c`, which may be fused into one operation to produce a higher-precision result.
rr::RValue<sw::SIMD::Float> FMA(
    rr::RValue<sw::SIMD::Float> const &a,
    rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c);

// Returns the exponent of the floating point number f.
// Assumes IEEE 754
rr::RValue<sw::SIMD::Int> Exponent(rr::RValue<sw::SIMD::Float> f);

// Returns y if y < x; otherwise result is x.
// If one operand is a NaN, the other operand is the result.
// If both operands are NaN, the result is a NaN.
rr::RValue<sw::SIMD::Float> NMin(rr::RValue<sw::SIMD::Float> const &x, rr::RValue<sw::SIMD::Float> const &y);

// Returns y if y > x; otherwise result is x.
// If one operand is a NaN, the other operand is the result.
// If both operands are NaN, the result is a NaN.
rr::RValue<sw::SIMD::Float> NMax(rr::RValue<sw::SIMD::Float> const &x, rr::RValue<sw::SIMD::Float> const &y);

// Returns the determinant of a 2x2 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d);

// Returns the determinant of a 3x3 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c,
    rr::RValue<sw::SIMD::Float> const &d, rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f,
    rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h, rr::RValue<sw::SIMD::Float> const &i);

// Returns the determinant of a 4x4 matrix.
rr::RValue<sw::SIMD::Float> Determinant(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d,
    rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f, rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h,
    rr::RValue<sw::SIMD::Float> const &i, rr::RValue<sw::SIMD::Float> const &j, rr::RValue<sw::SIMD::Float> const &k, rr::RValue<sw::SIMD::Float> const &l,
    rr::RValue<sw::SIMD::Float> const &m, rr::RValue<sw::SIMD::Float> const &n, rr::RValue<sw::SIMD::Float> const &o, rr::RValue<sw::SIMD::Float> const &p);

// Returns the inverse of a 2x2 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 4> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b,
    rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d);

// Returns the inverse of a 3x3 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 9> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c,
    rr::RValue<sw::SIMD::Float> const &d, rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f,
    rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h, rr::RValue<sw::SIMD::Float> const &i);

// Returns the inverse of a 4x4 matrix.
std::array<rr::RValue<sw::SIMD::Float>, 16> MatrixInverse(
    rr::RValue<sw::SIMD::Float> const &a, rr::RValue<sw::SIMD::Float> const &b, rr::RValue<sw::SIMD::Float> const &c, rr::RValue<sw::SIMD::Float> const &d,
    rr::RValue<sw::SIMD::Float> const &e, rr::RValue<sw::SIMD::Float> const &f, rr::RValue<sw::SIMD::Float> const &g, rr::RValue<sw::SIMD::Float> const &h,
    rr::RValue<sw::SIMD::Float> const &i, rr::RValue<sw::SIMD::Float> const &j, rr::RValue<sw::SIMD::Float> const &k, rr::RValue<sw::SIMD::Float> const &l,
    rr::RValue<sw::SIMD::Float> const &m, rr::RValue<sw::SIMD::Float> const &n, rr::RValue<sw::SIMD::Float> const &o, rr::RValue<sw::SIMD::Float> const &p);

////////////////////////////////////////////////////////////////////////////
// Inline functions
////////////////////////////////////////////////////////////////////////////

template<typename T>
inline rr::RValue<T> AndAll(rr::RValue<T> const &mask)
{
	T v1 = mask;               // [x]    [y]    [z]    [w]
	T v2 = v1.xzxz & v1.ywyw;  // [xy]   [zw]   [xy]   [zw]
	return v2.xxxx & v2.yyyy;  // [xyzw] [xyzw] [xyzw] [xyzw]
}

template<typename T>
inline rr::RValue<T> OrAll(rr::RValue<T> const &mask)
{
	T v1 = mask;               // [x]    [y]    [z]    [w]
	T v2 = v1.xzxz | v1.ywyw;  // [xy]   [zw]   [xy]   [zw]
	return v2.xxxx | v2.yyyy;  // [xyzw] [xyzw] [xyzw] [xyzw]
}

}  // namespace sw

#ifdef ENABLE_RR_PRINT
namespace rr {
template<>
struct PrintValue::Ty<sw::Vector4f>
{
	static std::string fmt(const sw::Vector4f &v)
	{
		return "[x: " + PrintValue::fmt(v.x) +
		       ", y: " + PrintValue::fmt(v.y) +
		       ", z: " + PrintValue::fmt(v.z) +
		       ", w: " + PrintValue::fmt(v.w) + "]";
	}

	static std::vector<rr::Value *> val(const sw::Vector4f &v)
	{
		return PrintValue::vals(v.x, v.y, v.z, v.w);
	}
};
template<>
struct PrintValue::Ty<sw::Vector4s>
{
	static std::string fmt(const sw::Vector4s &v)
	{
		return "[x: " + PrintValue::fmt(v.x) +
		       ", y: " + PrintValue::fmt(v.y) +
		       ", z: " + PrintValue::fmt(v.z) +
		       ", w: " + PrintValue::fmt(v.w) + "]";
	}

	static std::vector<rr::Value *> val(const sw::Vector4s &v)
	{
		return PrintValue::vals(v.x, v.y, v.z, v.w);
	}
};
}  // namespace rr
#endif  // ENABLE_RR_PRINT

#endif  // sw_ShaderCore_hpp
