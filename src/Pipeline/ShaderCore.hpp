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
#include "Vulkan/VkDebug.hpp"

namespace sw
{
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

	Float4 exponential2(RValue<Float4> x, bool pp = false);
	Float4 logarithm2(RValue<Float4> x, bool pp = false);
	Float4 exponential(RValue<Float4> x, bool pp = false);
	Float4 logarithm(RValue<Float4> x, bool pp = false);
	Float4 power(RValue<Float4> x, RValue<Float4> y, bool pp = false);
	Float4 reciprocal(RValue<Float4> x, bool pp = false, bool finite = false, bool exactAtPow2 = false);
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
	void transpose4x3(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3);
	void transpose4x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x3(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x2(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x1(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose2x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4xN(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3, int N);

	UInt4 halfToFloatBits(UInt4 halfBits);
}

#ifdef ENABLE_RR_PRINT
namespace rr {
	template <> struct PrintValue::Ty<sw::Vector4f>
	{
		static std::string fmt(const sw::Vector4f& v)
		{
			return "[x: " + PrintValue::fmt(v.x) + ","
			       " y: " + PrintValue::fmt(v.y) + ","
			       " z: " + PrintValue::fmt(v.z) + ","
			       " w: " + PrintValue::fmt(v.w) + "]";
		}

		static std::vector<rr::Value*> val(const sw::Vector4f& v)
		{
			return PrintValue::vals(v.x, v.y, v.z, v.w);
		}
	};
	template <> struct PrintValue::Ty<sw::Vector4s>
	{
		static std::string fmt(const sw::Vector4s& v)
		{
			return "[x: " + PrintValue::fmt(v.x) + ","
			       " y: " + PrintValue::fmt(v.y) + ","
			       " z: " + PrintValue::fmt(v.z) + ","
			       " w: " + PrintValue::fmt(v.w) + "]";
		}

		static std::vector<rr::Value*> val(const sw::Vector4s& v)
		{
			return PrintValue::vals(v.x, v.y, v.z, v.w);
		}
	};
}
#endif // ENABLE_RR_PRINT

#endif   // sw_ShaderCore_hpp
