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

enum class OutOfBoundsBehavior
{
	Nullify,             // Loads become zero, stores are elided.
	RobustBufferAccess,  // As defined by the Vulkan spec (in short: access anywhere within bounds, or zeroing).
	UndefinedValue,      // Only for load operations. Not secure. No program termination.
	UndefinedBehavior,   // Program may terminate.
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

struct Pointer
{
	Pointer(rr::Pointer<Byte> base, rr::Int limit);
	Pointer(rr::Pointer<Byte> base, unsigned int limit);
	Pointer(rr::Pointer<Byte> base, rr::Int limit, SIMD::Int offset);
	Pointer(rr::Pointer<Byte> base, unsigned int limit, SIMD::Int offset);

	Pointer &operator+=(Int i);
	Pointer &operator*=(Int i);

	Pointer operator+(SIMD::Int i);
	Pointer operator*(SIMD::Int i);

	Pointer &operator+=(int i);
	Pointer &operator*=(int i);

	Pointer operator+(int i);
	Pointer operator*(int i);

	SIMD::Int offsets() const;

	SIMD::Int isInBounds(unsigned int accessSize, OutOfBoundsBehavior robustness) const;

	bool isStaticallyInBounds(unsigned int accessSize, OutOfBoundsBehavior robustness) const;

	Int limit() const;

	// Returns true if all offsets are sequential
	// (N+0*step, N+1*step, N+2*step, N+3*step)
	rr::Bool hasSequentialOffsets(unsigned int step) const;

	// Returns true if all offsets are are compile-time static and
	// sequential (N+0*step, N+1*step, N+2*step, N+3*step)
	bool hasStaticSequentialOffsets(unsigned int step) const;

	// Returns true if all offsets are equal (N, N, N, N)
	rr::Bool hasEqualOffsets() const;

	// Returns true if all offsets are compile-time static and are equal
	// (N, N, N, N)
	bool hasStaticEqualOffsets() const;

	template<typename T>
	inline T Load(OutOfBoundsBehavior robustness, Int mask, bool atomic = false, std::memory_order order = std::memory_order_relaxed, int alignment = sizeof(float));

	template<typename T>
	inline void Store(T val, OutOfBoundsBehavior robustness, Int mask, bool atomic = false, std::memory_order order = std::memory_order_relaxed);

	template<typename T>
	inline void Store(RValue<T> val, OutOfBoundsBehavior robustness, Int mask, bool atomic = false, std::memory_order order = std::memory_order_relaxed);

	// Base address for the pointer, common across all lanes.
	rr::Pointer<rr::Byte> base;

	// Upper (non-inclusive) limit for offsets from base.
	rr::Int dynamicLimit;  // If hasDynamicLimit is false, dynamicLimit is zero.
	unsigned int staticLimit;

	// Per lane offsets from base.
	SIMD::Int dynamicOffsets;  // If hasDynamicOffsets is false, all dynamicOffsets are zero.
	std::array<int32_t, SIMD::Width> staticOffsets;

	bool hasDynamicLimit;    // True if dynamicLimit is non-zero.
	bool hasDynamicOffsets;  // True if any dynamicOffsets are non-zero.
};

template<typename T>
struct Element
{};
template<>
struct Element<Float>
{
	using type = rr::Float;
};
template<>
struct Element<Int>
{
	using type = rr::Int;
};
template<>
struct Element<UInt>
{
	using type = rr::UInt;
};

}  // namespace SIMD

Float4 exponential2(RValue<Float4> x, bool pp = false);
Float4 logarithm2(RValue<Float4> x, bool pp = false);
Float4 exponential(RValue<Float4> x, bool pp = false);
Float4 logarithm(RValue<Float4> x, bool pp = false);
Float4 power(RValue<Float4> x, RValue<Float4> y, bool pp = false);
Float4 reciprocal(RValue<Float4> x, bool pp = false, bool finite = false, bool exactAtPow2 = false);
Float4 reciprocalSquareRoot(RValue<Float4> x, bool abs, bool pp = false);
Float4 modulo(RValue<Float4> x, RValue<Float4> y);
Float4 sine_pi(RValue<Float4> x, bool pp = false);    // limited to [-pi, pi] range
Float4 cosine_pi(RValue<Float4> x, bool pp = false);  // limited to [-pi, pi] range
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
Vector4s a2b10g10r10Unpack(const Int4 &value);
Vector4s a2r10g10b10Unpack(const Int4 &value);

rr::RValue<rr::Bool> AnyTrue(rr::RValue<sw::SIMD::Int> const &ints);

rr::RValue<rr::Bool> AnyFalse(rr::RValue<sw::SIMD::Int> const &ints);

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

// Performs a fused-multiply add, returning a * b + c.
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
inline T SIMD::Pointer::Load(OutOfBoundsBehavior robustness, Int mask, bool atomic /* = false */, std::memory_order order /* = std::memory_order_relaxed */, int alignment /* = sizeof(float) */)
{
	using EL = typename Element<T>::type;

	if(isStaticallyInBounds(sizeof(float), robustness))
	{
		// All elements are statically known to be in-bounds.
		// We can avoid costly conditional on masks.

		if(hasStaticSequentialOffsets(sizeof(float)))
		{
			// Offsets are sequential. Perform regular load.
			return rr::Load(rr::Pointer<T>(base + staticOffsets[0]), alignment, atomic, order);
		}
		if(hasStaticEqualOffsets())
		{
			// Load one, replicate.
			return T(*rr::Pointer<EL>(base + staticOffsets[0], alignment));
		}
	}
	else
	{
		switch(robustness)
		{
			case OutOfBoundsBehavior::Nullify:
			case OutOfBoundsBehavior::RobustBufferAccess:
			case OutOfBoundsBehavior::UndefinedValue:
				mask &= isInBounds(sizeof(float), robustness);  // Disable out-of-bounds reads.
				break;
			case OutOfBoundsBehavior::UndefinedBehavior:
				// Nothing to do. Application/compiler must guarantee no out-of-bounds accesses.
				break;
		}
	}

	auto offs = offsets();

	if(!atomic && order == std::memory_order_relaxed)
	{
		if(hasStaticEqualOffsets())
		{
			// Load one, replicate.
			// Be careful of the case where the post-bounds-check mask
			// is 0, in which case we must not load.
			T out = T(0);
			If(AnyTrue(mask))
			{
				EL el = *rr::Pointer<EL>(base + staticOffsets[0], alignment);
				out = T(el);
			}
			return out;
		}

		bool zeroMaskedLanes = true;
		switch(robustness)
		{
			case OutOfBoundsBehavior::Nullify:
			case OutOfBoundsBehavior::RobustBufferAccess:  // Must either return an in-bounds value, or zero.
				zeroMaskedLanes = true;
				break;
			case OutOfBoundsBehavior::UndefinedValue:
			case OutOfBoundsBehavior::UndefinedBehavior:
				zeroMaskedLanes = false;
				break;
		}

		if(hasStaticSequentialOffsets(sizeof(float)))
		{
			return rr::MaskedLoad(rr::Pointer<T>(base + staticOffsets[0]), mask, alignment, zeroMaskedLanes);
		}

		return rr::Gather(rr::Pointer<EL>(base), offs, mask, alignment, zeroMaskedLanes);
	}
	else
	{
		T out;
		auto anyLanesDisabled = AnyFalse(mask);
		If(hasEqualOffsets() && !anyLanesDisabled)
		{
			// Load one, replicate.
			auto offset = Extract(offs, 0);
			out = T(rr::Load(rr::Pointer<EL>(&base[offset]), alignment, atomic, order));
		}
		Else If(hasSequentialOffsets(sizeof(float)) && !anyLanesDisabled)
		{
			// Load all elements in a single SIMD instruction.
			auto offset = Extract(offs, 0);
			out = rr::Load(rr::Pointer<T>(&base[offset]), alignment, atomic, order);
		}
		Else
		{
			// Divergent offsets or masked lanes.
			out = T(0);
			for(int i = 0; i < SIMD::Width; i++)
			{
				If(Extract(mask, i) != 0)
				{
					auto offset = Extract(offs, i);
					auto el = rr::Load(rr::Pointer<EL>(&base[offset]), alignment, atomic, order);
					out = Insert(out, el, i);
				}
			}
		}
		return out;
	}
}

template<typename T>
inline void SIMD::Pointer::Store(T val, OutOfBoundsBehavior robustness, Int mask, bool atomic /* = false */, std::memory_order order /* = std::memory_order_relaxed */)
{
	using EL = typename Element<T>::type;
	constexpr size_t alignment = sizeof(float);
	auto offs = offsets();

	switch(robustness)
	{
		case OutOfBoundsBehavior::Nullify:
		case OutOfBoundsBehavior::RobustBufferAccess:       // TODO: Allows writing anywhere within bounds. Could be faster than masking.
		case OutOfBoundsBehavior::UndefinedValue:           // Should not be used for store operations. Treat as robust buffer access.
			mask &= isInBounds(sizeof(float), robustness);  // Disable out-of-bounds writes.
			break;
		case OutOfBoundsBehavior::UndefinedBehavior:
			// Nothing to do. Application/compiler must guarantee no out-of-bounds accesses.
			break;
	}

	if(!atomic && order == std::memory_order_relaxed)
	{
		if(hasStaticEqualOffsets())
		{
			If(AnyTrue(mask))
			{
				// All equal. One of these writes will win -- elect the winning lane.
				auto v0111 = SIMD::Int(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				auto elect = mask & ~(v0111 & (mask.xxyz | mask.xxxy | mask.xxxx));
				auto maskedVal = As<SIMD::Int>(val) & elect;
				auto scalarVal = Extract(maskedVal, 0) |
				                 Extract(maskedVal, 1) |
				                 Extract(maskedVal, 2) |
				                 Extract(maskedVal, 3);
				*rr::Pointer<EL>(base + staticOffsets[0], alignment) = As<EL>(scalarVal);
			}
		}
		else if(hasStaticSequentialOffsets(sizeof(float)))
		{
			if(isStaticallyInBounds(sizeof(float), robustness))
			{
				// Pointer has no elements OOB, and the store is not atomic.
				// Perform a RMW.
				auto p = rr::Pointer<SIMD::Int>(base + staticOffsets[0], alignment);
				auto prev = *p;
				*p = (prev & ~mask) | (As<SIMD::Int>(val) & mask);
			}
			else
			{
				rr::MaskedStore(rr::Pointer<T>(base + staticOffsets[0]), val, mask, alignment);
			}
		}
		else
		{
			rr::Scatter(rr::Pointer<EL>(base), val, offs, mask, alignment);
		}
	}
	else
	{
		auto anyLanesDisabled = AnyFalse(mask);
		If(hasSequentialOffsets(sizeof(float)) && !anyLanesDisabled)
		{
			// Store all elements in a single SIMD instruction.
			auto offset = Extract(offs, 0);
			rr::Store(val, rr::Pointer<T>(&base[offset]), alignment, atomic, order);
		}
		Else
		{
			// Divergent offsets or masked lanes.
			for(int i = 0; i < SIMD::Width; i++)
			{
				If(Extract(mask, i) != 0)
				{
					auto offset = Extract(offs, i);
					rr::Store(Extract(val, i), rr::Pointer<EL>(&base[offset]), alignment, atomic, order);
				}
			}
		}
	}
}

template<typename T>
inline void SIMD::Pointer::Store(RValue<T> val, OutOfBoundsBehavior robustness, Int mask, bool atomic /* = false */, std::memory_order order /* = std::memory_order_relaxed */)
{
	Store(T(val), robustness, mask, atomic, order);
}

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
