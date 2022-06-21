// Copyright 2022 The SwiftShader Authors. All Rights Reserved.
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

#ifndef rr_SIMD_hpp
#define rr_SIMD_hpp

#include "Reactor.hpp"

namespace rr {

namespace scalar {
using Int = rr::Int;
using UInt = rr::UInt;
using Float = rr::Float;
}  // namespace scalar

namespace SIMD {

extern const int Width;

class Int;
class UInt;
class Float;

class Int : public LValue<SIMD::Int>
{
public:
	explicit Int(RValue<SIMD::Float> cast);

	Int();
	Int(int broadcast);
	Int(RValue<SIMD::Int> rhs);
	Int(const Int &rhs);
	Int(const Reference<SIMD::Int> &rhs);
	Int(RValue<SIMD::UInt> rhs);
	Int(const UInt &rhs);
	Int(const Reference<SIMD::UInt> &rhs);
	Int(RValue<scalar::Int> rhs);
	Int(const scalar::Int &rhs);
	Int(const Reference<scalar::Int> &rhs);

	RValue<SIMD::Int> operator=(int broadcast);
	RValue<SIMD::Int> operator=(RValue<SIMD::Int> rhs);
	RValue<SIMD::Int> operator=(const Int &rhs);
	RValue<SIMD::Int> operator=(const Reference<SIMD::Int> &rhs);

	static Type *type();
	static int element_count() { return SIMD::Width; }
};

class UInt : public LValue<SIMD::UInt>
{
public:
	explicit UInt(RValue<SIMD::Float> cast);

	UInt();
	UInt(int broadcast);
	UInt(RValue<SIMD::UInt> rhs);
	UInt(const UInt &rhs);
	UInt(const Reference<SIMD::UInt> &rhs);
	UInt(RValue<SIMD::Int> rhs);
	UInt(const Int &rhs);
	UInt(const Reference<SIMD::Int> &rhs);
	UInt(RValue<scalar::UInt> rhs);
	UInt(const scalar::UInt &rhs);
	UInt(const Reference<scalar::UInt> &rhs);

	RValue<SIMD::UInt> operator=(RValue<SIMD::UInt> rhs);
	RValue<SIMD::UInt> operator=(const UInt &rhs);
	RValue<SIMD::UInt> operator=(const Reference<SIMD::UInt> &rhs);

	static Type *type();
	static int element_count() { return SIMD::Width; }
};

class Float : public LValue<SIMD::Float>
{
public:
	explicit Float(RValue<SIMD::Int> cast);
	explicit Float(RValue<SIMD::UInt> cast);

	Float();
	Float(float broadcast);
	Float(RValue<SIMD::Float> rhs);
	Float(const Float &rhs);
	Float(const Reference<SIMD::Float> &rhs);
	Float(RValue<scalar::Float> rhs);
	Float(const scalar::Float &rhs);
	Float(const Reference<scalar::Float> &rhs);

	RValue<SIMD::Float> operator=(float broadcast);
	RValue<SIMD::Float> operator=(RValue<SIMD::Float> rhs);
	RValue<SIMD::Float> operator=(const Float &rhs);
	RValue<SIMD::Float> operator=(const Reference<SIMD::Float> &rhs);
	RValue<SIMD::Float> operator=(RValue<scalar::Float> rhs);
	RValue<SIMD::Float> operator=(const scalar::Float &rhs);
	RValue<SIMD::Float> operator=(const Reference<scalar::Float> &rhs);

	static SIMD::Float infinity();

	static Type *type();
	static int element_count() { return SIMD::Width; }
};

}  // namespace SIMD

RValue<SIMD::Int> operator+(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator-(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator*(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator/(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator%(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator&(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator|(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator^(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator<<(RValue<SIMD::Int> lhs, unsigned char rhs);
RValue<SIMD::Int> operator>>(RValue<SIMD::Int> lhs, unsigned char rhs);
RValue<SIMD::Int> operator<<(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator>>(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator+=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator-=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator*=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
//	RValue<SIMD::Int> operator/=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
//	RValue<SIMD::Int> operator%=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator&=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator|=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator^=(SIMD::Int &lhs, RValue<SIMD::Int> rhs);
RValue<SIMD::Int> operator<<=(SIMD::Int &lhs, unsigned char rhs);
RValue<SIMD::Int> operator>>=(SIMD::Int &lhs, unsigned char rhs);
RValue<SIMD::Int> operator+(RValue<SIMD::Int> val);
RValue<SIMD::Int> operator-(RValue<SIMD::Int> val);
RValue<SIMD::Int> operator~(RValue<SIMD::Int> val);
//	RValue<SIMD::Int> operator++(SIMD::Int &val, int);   // Post-increment
//	const Int &operator++(SIMD::Int &val);   // Pre-increment
//	RValue<SIMD::Int> operator--(SIMD::Int &val, int);   // Post-decrement
//	const Int &operator--(SIMD::Int &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
//	RValue<Bool> operator<=(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
//	RValue<Bool> operator>(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
//	RValue<Bool> operator>=(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
//	RValue<Bool> operator!=(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);
//	RValue<Bool> operator==(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs);

RValue<SIMD::Int> CmpEQ(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> CmpLT(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> CmpLE(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> CmpNEQ(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> CmpNLT(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> CmpNLE(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
inline RValue<SIMD::Int> CmpGT(RValue<SIMD::Int> x, RValue<SIMD::Int> y)
{
	return CmpNLE(x, y);
}
inline RValue<SIMD::Int> CmpGE(RValue<SIMD::Int> x, RValue<SIMD::Int> y)
{
	return CmpNLT(x, y);
}
RValue<SIMD::Int> Abs(RValue<SIMD::Int> x);
RValue<SIMD::Int> Max(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
RValue<SIMD::Int> Min(RValue<SIMD::Int> x, RValue<SIMD::Int> y);
// Convert to nearest integer. If a converted value is outside of the integer
// range, the returned result is undefined.
RValue<SIMD::Int> RoundInt(RValue<SIMD::Float> cast);
// Rounds to the nearest integer, but clamps very large values to an
// implementation-dependent range.
// Specifically, on x86, values larger than 2147483583.0 are converted to
// 2147483583 (0x7FFFFFBF) instead of producing 0x80000000.
RValue<SIMD::Int> RoundIntClamped(RValue<SIMD::Float> cast);
RValue<scalar::Int> Extract(RValue<SIMD::Int> val, int i);
RValue<SIMD::Int> Insert(RValue<SIMD::Int> val, RValue<scalar::Int> element, int i);

RValue<SIMD::UInt> operator+(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator-(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator*(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator/(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator%(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator&(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator|(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator^(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator<<(RValue<SIMD::UInt> lhs, unsigned char rhs);
RValue<SIMD::UInt> operator>>(RValue<SIMD::UInt> lhs, unsigned char rhs);
RValue<SIMD::UInt> operator<<(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator>>(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator+=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator-=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator*=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
//	RValue<SIMD::UInt> operator/=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
//	RValue<SIMD::UInt> operator%=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator&=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator|=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator^=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs);
RValue<SIMD::UInt> operator<<=(SIMD::UInt &lhs, unsigned char rhs);
RValue<SIMD::UInt> operator>>=(SIMD::UInt &lhs, unsigned char rhs);
RValue<SIMD::UInt> operator+(RValue<SIMD::UInt> val);
RValue<SIMD::UInt> operator-(RValue<SIMD::UInt> val);
RValue<SIMD::UInt> operator~(RValue<SIMD::UInt> val);
//	RValue<SIMD::UInt> operator++(SIMD::UInt &val, int);   // Post-increment
//	const UInt &operator++(SIMD::UInt &val);   // Pre-increment
//	RValue<SIMD::UInt> operator--(SIMD::UInt &val, int);   // Post-decrement
//	const UInt &operator--(SIMD::UInt &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
//	RValue<Bool> operator<=(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
//	RValue<Bool> operator>(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
//	RValue<Bool> operator>=(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
//	RValue<Bool> operator!=(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);
//	RValue<Bool> operator==(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs);

RValue<SIMD::UInt> CmpEQ(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> CmpLT(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> CmpLE(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> CmpNEQ(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> CmpNLT(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> CmpNLE(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
inline RValue<SIMD::UInt> CmpGT(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y)
{
	return CmpNLE(x, y);
}
inline RValue<SIMD::UInt> CmpGE(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y)
{
	return CmpNLT(x, y);
}
RValue<SIMD::UInt> Max(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<SIMD::UInt> Min(RValue<SIMD::UInt> x, RValue<SIMD::UInt> y);
RValue<scalar::UInt> Extract(RValue<SIMD::UInt> val, int i);
RValue<SIMD::UInt> Insert(RValue<SIMD::UInt> val, RValue<scalar::UInt> element, int i);
//	RValue<SIMD::UInt> RoundInt(RValue<SIMD::Float> cast);

RValue<SIMD::Float> operator+(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator-(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator*(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator/(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator%(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator+=(SIMD::Float &lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator-=(SIMD::Float &lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator*=(SIMD::Float &lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator/=(SIMD::Float &lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator%=(SIMD::Float &lhs, RValue<SIMD::Float> rhs);
RValue<SIMD::Float> operator+(RValue<SIMD::Float> val);
RValue<SIMD::Float> operator-(RValue<SIMD::Float> val);

// Computes `x * y + z`, which may be fused into one operation to produce a higher-precision result.
RValue<SIMD::Float> MulAdd(RValue<SIMD::Float> x, RValue<SIMD::Float> y, RValue<SIMD::Float> z);
// Computes a fused `x * y + z` operation. Caps::fmaIsFast indicates whether it emits an FMA instruction.
RValue<SIMD::Float> FMA(RValue<SIMD::Float> x, RValue<SIMD::Float> y, RValue<SIMD::Float> z);

RValue<SIMD::Float> Abs(RValue<SIMD::Float> x);
RValue<SIMD::Float> Max(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Float> Min(RValue<SIMD::Float> x, RValue<SIMD::Float> y);

RValue<SIMD::Float> Rcp(RValue<SIMD::Float> x, bool relaxedPrecision, bool exactAtPow2 = false);
RValue<SIMD::Float> RcpSqrt(RValue<SIMD::Float> x, bool relaxedPrecision);
RValue<SIMD::Float> Sqrt(RValue<SIMD::Float> x);
RValue<SIMD::Float> Insert(RValue<SIMD::Float> val, RValue<rr ::Float> element, int i);
RValue<rr ::Float> Extract(RValue<SIMD::Float> x, int i);

// Ordered comparison functions
RValue<SIMD::Int> CmpEQ(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpLT(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpLE(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpNEQ(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpNLT(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpNLE(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
inline RValue<SIMD::Int> CmpGT(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return CmpNLE(x, y);
}
inline RValue<SIMD::Int> CmpGE(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return CmpNLT(x, y);
}

// Unordered comparison functions
RValue<SIMD::Int> CmpUEQ(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpULT(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpULE(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpUNEQ(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpUNLT(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Int> CmpUNLE(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
inline RValue<SIMD::Int> CmpUGT(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return CmpUNLE(x, y);
}
inline RValue<SIMD::Int> CmpUGE(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return CmpUNLT(x, y);
}

RValue<SIMD::Int> IsInf(RValue<SIMD::Float> x);
RValue<SIMD::Int> IsNan(RValue<SIMD::Float> x);
RValue<SIMD::Float> Round(RValue<SIMD::Float> x);
RValue<SIMD::Float> Trunc(RValue<SIMD::Float> x);
RValue<SIMD::Float> Frac(RValue<SIMD::Float> x);
RValue<SIMD::Float> Floor(RValue<SIMD::Float> x);
RValue<SIMD::Float> Ceil(RValue<SIMD::Float> x);

// Trigonometric functions
RValue<SIMD::Float> Sin(RValue<SIMD::Float> x);
RValue<SIMD::Float> Cos(RValue<SIMD::Float> x);
RValue<SIMD::Float> Tan(RValue<SIMD::Float> x);
RValue<SIMD::Float> Asin(RValue<SIMD::Float> x);
RValue<SIMD::Float> Acos(RValue<SIMD::Float> x);
RValue<SIMD::Float> Atan(RValue<SIMD::Float> x);
RValue<SIMD::Float> Sinh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Cosh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Tanh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Asinh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Acosh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Atanh(RValue<SIMD::Float> x);
RValue<SIMD::Float> Atan2(RValue<SIMD::Float> x, RValue<SIMD::Float> y);

// Exponential functions
RValue<SIMD::Float> Pow(RValue<SIMD::Float> x, RValue<SIMD::Float> y);
RValue<SIMD::Float> Exp(RValue<SIMD::Float> x);
RValue<SIMD::Float> Log(RValue<SIMD::Float> x);
RValue<SIMD::Float> Exp2(RValue<SIMD::Float> x);
RValue<SIMD::Float> Log2(RValue<SIMD::Float> x);

template<>
inline RValue<SIMD::Int>::RValue(int i)
    : val(broadcast(i, SIMD::Int::type()))
{
	RR_DEBUG_INFO_EMIT_VAR(val);
}

template<>
inline RValue<SIMD::UInt>::RValue(unsigned int i)
    : val(broadcast(int(i), SIMD::UInt::type()))
{
	RR_DEBUG_INFO_EMIT_VAR(val);
}

template<>
inline RValue<SIMD::Float>::RValue(float f)
    : val(broadcast(f, SIMD::Float::type()))
{
	RR_DEBUG_INFO_EMIT_VAR(val);
}

}  // namespace rr

#endif  // rr_SIMD_hpp
