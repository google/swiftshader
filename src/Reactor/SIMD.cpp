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

#include "SIMD.hpp"

#include "Debug.hpp"

#include <cmath>

namespace rr {

SIMD::Int::Int()
{
}

SIMD::Int::Int(RValue<SIMD::Float> cast)
{
	Value *xyzw = Nucleus::createFPToSI(cast.value(), SIMD::Int::type());

	storeValue(xyzw);
}

SIMD::Int::Int(int broadcast)
{
	std::vector<int64_t> constantVector = { broadcast };
	storeValue(Nucleus::createConstantVector(constantVector, type()));
}

SIMD::Int::Int(RValue<SIMD::Int> rhs)
{
	store(rhs);
}

SIMD::Int::Int(const SIMD::Int &rhs)
{
	store(rhs.load());
}

SIMD::Int::Int(const Reference<SIMD::Int> &rhs)
{
	store(rhs.load());
}

SIMD::Int::Int(RValue<SIMD::UInt> rhs)
{
	storeValue(rhs.value());
}

SIMD::Int::Int(const SIMD::UInt &rhs)
{
	storeValue(rhs.loadValue());
}

SIMD::Int::Int(const Reference<SIMD::UInt> &rhs)
{
	storeValue(rhs.loadValue());
}

SIMD::Int::Int(const scalar::Int &rhs)
{
	*this = RValue<scalar::Int>(rhs.loadValue());
}

SIMD::Int::Int(const Reference<scalar::Int> &rhs)
{
	*this = RValue<scalar::Int>(rhs.loadValue());
}

RValue<SIMD::Int> SIMD::Int::operator=(int x)
{
	return *this = SIMD::Int(x);
}

RValue<SIMD::Int> SIMD::Int::operator=(RValue<SIMD::Int> rhs)
{
	return store(rhs);
}

RValue<SIMD::Int> SIMD::Int::operator=(const SIMD::Int &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::Int> SIMD::Int::operator=(const Reference<SIMD::Int> &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::Int> operator+(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createAdd(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator-(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createSub(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator*(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createMul(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator/(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createSDiv(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator%(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createSRem(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator&(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createAnd(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator|(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createOr(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator^(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createXor(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator<<(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createShl(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator>>(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createAShr(lhs.value(), rhs.value()));
}

RValue<SIMD::Int> operator+=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs + rhs;
}

RValue<SIMD::Int> operator-=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs - rhs;
}

RValue<SIMD::Int> operator*=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs * rhs;
}

//	RValue<SIMD::Int> operator/=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<SIMD::Int> operator%=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

RValue<SIMD::Int> operator&=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs & rhs;
}

RValue<SIMD::Int> operator|=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs | rhs;
}

RValue<SIMD::Int> operator^=(SIMD::Int &lhs, RValue<SIMD::Int> rhs)
{
	return lhs = lhs ^ rhs;
}

RValue<SIMD::Int> operator<<=(SIMD::Int &lhs, unsigned char rhs)
{
	return lhs = lhs << rhs;
}

RValue<SIMD::Int> operator>>=(SIMD::Int &lhs, unsigned char rhs)
{
	return lhs = lhs >> rhs;
}

RValue<SIMD::Int> operator+(RValue<SIMD::Int> val)
{
	return val;
}

RValue<SIMD::Int> operator-(RValue<SIMD::Int> val)
{
	return RValue<SIMD::Int>(Nucleus::createNeg(val.value()));
}

RValue<SIMD::Int> operator~(RValue<SIMD::Int> val)
{
	return RValue<SIMD::Int>(Nucleus::createNot(val.value()));
}

RValue<scalar::Int> Extract(RValue<SIMD::Int> x, int i)
{
	return RValue<scalar::Int>(Nucleus::createExtractElement(x.value(), scalar::Int::type(), i));
}

RValue<SIMD::Int> Insert(RValue<SIMD::Int> x, RValue<scalar::Int> element, int i)
{
	return RValue<SIMD::Int>(Nucleus::createInsertElement(x.value(), element.value(), i));
}

SIMD::UInt::UInt()
{
}

SIMD::UInt::UInt(int broadcast)
{
	std::vector<int64_t> constantVector = { broadcast };
	storeValue(Nucleus::createConstantVector(constantVector, type()));
}

SIMD::UInt::UInt(RValue<SIMD::UInt> rhs)
{
	store(rhs);
}

SIMD::UInt::UInt(const SIMD::UInt &rhs)
{
	store(rhs.load());
}

SIMD::UInt::UInt(const Reference<SIMD::UInt> &rhs)
{
	store(rhs.load());
}

SIMD::UInt::UInt(RValue<SIMD::Int> rhs)
{
	storeValue(rhs.value());
}

SIMD::UInt::UInt(const SIMD::Int &rhs)
{
	storeValue(rhs.loadValue());
}

SIMD::UInt::UInt(const Reference<SIMD::Int> &rhs)
{
	storeValue(rhs.loadValue());
}

SIMD::UInt::UInt(const scalar::UInt &rhs)
{
	*this = RValue<scalar::UInt>(rhs.loadValue());
}

SIMD::UInt::UInt(const Reference<scalar::UInt> &rhs)
{
	*this = RValue<scalar::UInt>(rhs.loadValue());
}

RValue<SIMD::UInt> SIMD::UInt::operator=(RValue<SIMD::UInt> rhs)
{
	return store(rhs);
}

RValue<SIMD::UInt> SIMD::UInt::operator=(const SIMD::UInt &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::UInt> SIMD::UInt::operator=(const Reference<SIMD::UInt> &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::UInt> operator+(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createAdd(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator-(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createSub(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator*(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createMul(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator/(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createUDiv(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator%(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createURem(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator&(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createAnd(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator|(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createOr(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator^(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createXor(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator<<(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createShl(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator>>(RValue<SIMD::UInt> lhs, RValue<SIMD::UInt> rhs)
{
	return RValue<SIMD::UInt>(Nucleus::createLShr(lhs.value(), rhs.value()));
}

RValue<SIMD::UInt> operator+=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs + rhs;
}

RValue<SIMD::UInt> operator-=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs - rhs;
}

RValue<SIMD::UInt> operator*=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs * rhs;
}

//	RValue<SIMD::UInt> operator/=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<SIMD::UInt> operator%=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

RValue<SIMD::UInt> operator&=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs & rhs;
}

RValue<SIMD::UInt> operator|=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs | rhs;
}

RValue<SIMD::UInt> operator^=(SIMD::UInt &lhs, RValue<SIMD::UInt> rhs)
{
	return lhs = lhs ^ rhs;
}

RValue<SIMD::UInt> operator<<=(SIMD::UInt &lhs, unsigned char rhs)
{
	return lhs = lhs << rhs;
}

RValue<SIMD::UInt> operator>>=(SIMD::UInt &lhs, unsigned char rhs)
{
	return lhs = lhs >> rhs;
}

RValue<SIMD::UInt> operator+(RValue<SIMD::UInt> val)
{
	return val;
}

RValue<SIMD::UInt> operator-(RValue<SIMD::UInt> val)
{
	return RValue<SIMD::UInt>(Nucleus::createNeg(val.value()));
}

RValue<SIMD::UInt> operator~(RValue<SIMD::UInt> val)
{
	return RValue<SIMD::UInt>(Nucleus::createNot(val.value()));
}

RValue<scalar::UInt> Extract(RValue<SIMD::UInt> x, int i)
{
	return RValue<scalar::UInt>(Nucleus::createExtractElement(x.value(), scalar::Int::type(), i));
}

RValue<SIMD::UInt> Insert(RValue<SIMD::UInt> x, RValue<scalar::UInt> element, int i)
{
	return RValue<SIMD::UInt>(Nucleus::createInsertElement(x.value(), element.value(), i));
}

SIMD::Float::Float(RValue<SIMD::Int> cast)
{
	Value *xyzw = Nucleus::createSIToFP(cast.value(), SIMD::Float::type());

	storeValue(xyzw);
}

SIMD::Float::Float(RValue<SIMD::UInt> cast)
{
	RValue<SIMD::Float> result = SIMD::Float(SIMD::Int(cast & SIMD::UInt(0x7FFFFFFF))) +
	                             As<SIMD::Float>((As<SIMD::Int>(cast) >> 31) & As<SIMD::Int>(SIMD::Float(0x80000000u)));

	storeValue(result.value());
}

SIMD::Float::Float()
{
}

SIMD::Float::Float(float broadcast)
{
	// See rr::Float(float) constructor for the rationale behind this assert.
	ASSERT(std::isfinite(broadcast));

	std::vector<double> constantVector = { broadcast };
	storeValue(Nucleus::createConstantVector(constantVector, type()));
}

SIMD::Float SIMD::Float::infinity()
{
	SIMD::Float result;

	constexpr double inf = std::numeric_limits<double>::infinity();
	std::vector<double> constantVector = { inf };
	result.storeValue(Nucleus::createConstantVector(constantVector, type()));

	return result;
}

SIMD::Float::Float(RValue<SIMD::Float> rhs)
{
	store(rhs);
}

SIMD::Float::Float(const SIMD::Float &rhs)
{
	store(rhs.load());
}

SIMD::Float::Float(const Reference<SIMD::Float> &rhs)
{
	store(rhs.load());
}

SIMD::Float::Float(const scalar::Float &rhs)
{
	*this = RValue<scalar::Float>(rhs.loadValue());
}

SIMD::Float::Float(const Reference<scalar::Float> &rhs)
{
	*this = RValue<scalar::Float>(rhs.loadValue());
}

RValue<SIMD::Float> SIMD::Float::operator=(float x)
{
	return *this = SIMD::Float(x);
}

RValue<SIMD::Float> SIMD::Float::operator=(RValue<SIMD::Float> rhs)
{
	return store(rhs);
}

RValue<SIMD::Float> SIMD::Float::operator=(const SIMD::Float &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::Float> SIMD::Float::operator=(const Reference<SIMD::Float> &rhs)
{
	return store(rhs.load());
}

RValue<SIMD::Float> SIMD::Float::operator=(RValue<scalar::Float> rhs)
{
	return *this = SIMD::Float(rhs);
}

RValue<SIMD::Float> SIMD::Float::operator=(const scalar::Float &rhs)
{
	return *this = SIMD::Float(rhs);
}

RValue<SIMD::Float> SIMD::Float::operator=(const Reference<scalar::Float> &rhs)
{
	return *this = SIMD::Float(rhs);
}

RValue<SIMD::Float> operator+(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs)
{
	return RValue<SIMD::Float>(Nucleus::createFAdd(lhs.value(), rhs.value()));
}

RValue<SIMD::Float> operator-(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs)
{
	return RValue<SIMD::Float>(Nucleus::createFSub(lhs.value(), rhs.value()));
}

RValue<SIMD::Float> operator*(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs)
{
	return RValue<SIMD::Float>(Nucleus::createFMul(lhs.value(), rhs.value()));
}

RValue<SIMD::Float> operator/(RValue<SIMD::Float> lhs, RValue<SIMD::Float> rhs)
{
	return RValue<SIMD::Float>(Nucleus::createFDiv(lhs.value(), rhs.value()));
}

RValue<SIMD::Float> operator+=(SIMD::Float &lhs, RValue<SIMD::Float> rhs)
{
	return lhs = lhs + rhs;
}

RValue<SIMD::Float> operator-=(SIMD::Float &lhs, RValue<SIMD::Float> rhs)
{
	return lhs = lhs - rhs;
}

RValue<SIMD::Float> operator*=(SIMD::Float &lhs, RValue<SIMD::Float> rhs)
{
	return lhs = lhs * rhs;
}

RValue<SIMD::Float> operator/=(SIMD::Float &lhs, RValue<SIMD::Float> rhs)
{
	return lhs = lhs / rhs;
}

RValue<SIMD::Float> operator%=(SIMD::Float &lhs, RValue<SIMD::Float> rhs)
{
	return lhs = lhs % rhs;
}

RValue<SIMD::Float> operator+(RValue<SIMD::Float> val)
{
	return val;
}

RValue<SIMD::Float> operator-(RValue<SIMD::Float> val)
{
	return RValue<SIMD::Float>(Nucleus::createFNeg(val.value()));
}

RValue<SIMD::Float> Insert(RValue<SIMD::Float> x, RValue<scalar::Float> element, int i)
{
	return RValue<SIMD::Float>(Nucleus::createInsertElement(x.value(), element.value(), i));
}

RValue<scalar::Float> Extract(RValue<SIMD::Float> x, int i)
{
	return RValue<scalar::Float>(Nucleus::createExtractElement(x.value(), scalar::Float::type(), i));
}

RValue<SIMD::Int> IsInf(RValue<SIMD::Float> x)
{
	return CmpEQ(As<SIMD::Int>(x) & SIMD::Int(0x7FFFFFFF), SIMD::Int(0x7F800000));
}

RValue<SIMD::Int> IsNan(RValue<SIMD::Float> x)
{
	return ~CmpEQ(x, x);
}

RValue<SIMD::Float> Sin(RValue<SIMD::Float> x)
{
	return ScalarizeCall(sinf, x);
}

RValue<SIMD::Float> Cos(RValue<SIMD::Float> x)
{
	return ScalarizeCall(cosf, x);
}

RValue<SIMD::Float> Tan(RValue<SIMD::Float> x)
{
	return ScalarizeCall(tanf, x);
}

RValue<SIMD::Float> Asin(RValue<SIMD::Float> x)
{
	return ScalarizeCall(asinf, x);
}

RValue<SIMD::Float> Acos(RValue<SIMD::Float> x)
{
	return ScalarizeCall(acosf, x);
}

RValue<SIMD::Float> Atan(RValue<SIMD::Float> x)
{
	return ScalarizeCall(atanf, x);
}

RValue<SIMD::Float> Sinh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(sinhf, x);
}

RValue<SIMD::Float> Cosh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(coshf, x);
}

RValue<SIMD::Float> Tanh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(tanhf, x);
}

RValue<SIMD::Float> Asinh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(asinhf, x);
}

RValue<SIMD::Float> Acosh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(acoshf, x);
}

RValue<SIMD::Float> Atanh(RValue<SIMD::Float> x)
{
	return ScalarizeCall(atanhf, x);
}

RValue<SIMD::Float> Atan2(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return ScalarizeCall(atan2f, x, y);
}

RValue<SIMD::Float> Pow(RValue<SIMD::Float> x, RValue<SIMD::Float> y)
{
	return ScalarizeCall(powf, x, y);
}

RValue<SIMD::Float> Exp(RValue<SIMD::Float> x)
{
	return ScalarizeCall(expf, x);
}

RValue<SIMD::Float> Log(RValue<SIMD::Float> x)
{
	return ScalarizeCall(logf, x);
}

RValue<SIMD::Float> Exp2(RValue<SIMD::Float> x)
{
	return ScalarizeCall(exp2f, x);
}

RValue<SIMD::Float> Log2(RValue<SIMD::Float> x)
{
	return ScalarizeCall(log2f, x);
}

}  // namespace rr
