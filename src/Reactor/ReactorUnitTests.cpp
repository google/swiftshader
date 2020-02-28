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

#include "Coroutine.hpp"
#include "Print.hpp"
#include "Reactor.hpp"

#include "gtest/gtest.h"

#include <array>
#include <cmath>
#include <thread>
#include <tuple>

using namespace rr;

constexpr float PI = 3.141592653589793f;

using float4 = float[4];
using int4 = int[4];

// TODO: Move to Reactor.hpp
template<>
struct rr::CToReactor<int[4]>
{
	using type = Int4;
	static Int4 cast(float[4]);
};

// Value type wrapper around a <type>[4] (i.e. float4, int4)
template<typename T>
struct type4_value
{
	using E = typename std::remove_pointer_t<std::decay_t<T>>;

	type4_value() = default;
	explicit type4_value(E rep)
	    : v{ rep, rep, rep, rep }
	{}
	type4_value(E x, E y, E z, E w)
	    : v{ x, y, z, w }
	{}

	bool operator==(const type4_value &rhs) const
	{
		return std::equal(std::begin(v), std::end(v), rhs.v);
	}

	// For gtest printing
	friend std::ostream &operator<<(std::ostream &os, const type4_value &value)
	{
		return os << "[" << value.v[0] << ", " << value.v[1] << ", " << value.v[2] << ", " << value.v[3] << "]";
	}

	T v;
};

using float4_value = type4_value<float4>;
using int4_value = type4_value<int4>;

// Invoke a void(type4_value<T>*) routine on &v.v, returning wrapped result in v
template<typename RoutineType, typename T>
type4_value<T> invokeRoutine(RoutineType &routine, type4_value<T> v)
{
	routine(&v.v);
	return v;
}

// Invoke a void(type4_value<T>*, type4_value<T>*) routine on &v1.v, &v2.v returning wrapped result in v1
template<typename RoutineType, typename T>
type4_value<T> invokeRoutine(RoutineType &routine, type4_value<T> v1, type4_value<T> v2)
{
	routine(&v1.v, &v2.v);
	return v1;
}

// For gtest printing of pairs
namespace std {
template<typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::pair<T, U> &value)
{
	return os << "{ " << value.first << ", " << value.second << " }";
}
}  // namespace std

int reference(int *p, int y)
{
	int x = p[-1];
	int z = 4;

	for(int i = 0; i < 10; i++)
	{
		z += (2 << i) - (i / 3);
	}

	int sum = x + y + z;

	return sum;
}

class StdOutCapture
{
public:
	~StdOutCapture()
	{
		stopIfCapturing();
	}

	void start()
	{
		stopIfCapturing();
		capturing = true;
		testing::internal::CaptureStdout();
	}

	std::string stop()
	{
		assert(capturing);
		capturing = false;
		return testing::internal::GetCapturedStdout();
	}

private:
	void stopIfCapturing()
	{
		if(capturing)
		{
			// This stops the capture
			testing::internal::GetCapturedStdout();
		}
	}

	bool capturing = false;
};

std::vector<std::string> split(const std::string &s)
{
	std::vector<std::string> result;
	std::istringstream iss(s);
	for(std::string line; std::getline(iss, line);)
	{
		result.push_back(line);
	}
	return result;
}

static const std::vector<int> fibonacci = {
	0,
	1,
	1,
	2,
	3,
	5,
	8,
	13,
	21,
	34,
	55,
	89,
	144,
	233,
	377,
	610,
	987,
	1597,
	2584,
	4181,
	6765,
	10946,
	17711,
	28657,
	46368,
	75025,
	121393,
	196418,
	317811,
};

TEST(ReactorUnitTests, PrintPrimitiveTypes)
{
#if defined(ENABLE_RR_PRINT) && !defined(ENABLE_RR_EMIT_PRINT_LOCATION)
	FunctionT<void()> function;
	{
		bool b(true);
		int8_t i8(-1);
		uint8_t ui8(1);
		int16_t i16(-1);
		uint16_t ui16(1);
		int32_t i32(-1);
		uint32_t ui32(1);
		int64_t i64(-1);
		uint64_t ui64(1);
		float f(1);
		double d(2);
		const char *cstr = "const char*";
		std::string str = "std::string";
		int *p = nullptr;

		RR_WATCH(b);
		RR_WATCH(i8);
		RR_WATCH(ui8);
		RR_WATCH(i16);
		RR_WATCH(ui16);
		RR_WATCH(i32);
		RR_WATCH(ui32);
		RR_WATCH(i64);
		RR_WATCH(ui64);
		RR_WATCH(f);
		RR_WATCH(d);
		RR_WATCH(cstr);
		RR_WATCH(str);
		RR_WATCH(p);
	}

	auto routine = function("one");

	char pNullptr[64];
	snprintf(pNullptr, sizeof(pNullptr), "  p: %p", nullptr);

	const char *expected[] = {
		"  b: true",
		"  i8: -1",
		"  ui8: 1",
		"  i16: -1",
		"  ui16: 1",
		"  i32: -1",
		"  ui32: 1",
		"  i64: -1",
		"  ui64: 1",
		"  f: 1.000000",
		"  d: 2.000000",
		"  cstr: const char*",
		"  str: std::string",
		pNullptr,
	};
	constexpr size_t expectedSize = sizeof(expected) / sizeof(expected[0]);

	StdOutCapture capture;
	capture.start();
	routine();
	auto output = split(capture.stop());
	for(size_t i = 0, j = 1; i < expectedSize; ++i, j += 2)
	{
		ASSERT_EQ(expected[i], output[j]);
	}

#endif
}

TEST(ReactorUnitTests, PrintReactorTypes)
{
#if defined(ENABLE_RR_PRINT) && !defined(ENABLE_RR_EMIT_PRINT_LOCATION)
	FunctionT<void()> function;
	{
		Bool b(true);
		Int i(-1);
		Int2 i2(-1, -2);
		Int4 i4(-1, -2, -3, -4);
		UInt ui(1);
		UInt2 ui2(1, 2);
		UInt4 ui4(1, 2, 3, 4);
		Short s(-1);
		Short4 s4(-1, -2, -3, -4);
		UShort us(1);
		UShort4 us4(1, 2, 3, 4);
		Float f(1);
		Float4 f4(1, 2, 3, 4);
		Long l(i);
		Pointer<Int> pi = nullptr;
		RValue<Int> rvi = i;
		Byte by('a');
		Byte4 by4(i4);

		RR_WATCH(b);
		RR_WATCH(i);
		RR_WATCH(i2);
		RR_WATCH(i4);
		RR_WATCH(ui);
		RR_WATCH(ui2);
		RR_WATCH(ui4);
		RR_WATCH(s);
		RR_WATCH(s4);
		RR_WATCH(us);
		RR_WATCH(us4);
		RR_WATCH(f);
		RR_WATCH(f4);
		RR_WATCH(l);
		RR_WATCH(pi);
		RR_WATCH(rvi);
		RR_WATCH(by);
		RR_WATCH(by4);
	}

	auto routine = function("one");

	char piNullptr[64];
	snprintf(piNullptr, sizeof(piNullptr), "  pi: %p", nullptr);

	const char *expected[] = {
		"  b: true",
		"  i: -1",
		"  i2: [-1, -2]",
		"  i4: [-1, -2, -3, -4]",
		"  ui: 1",
		"  ui2: [1, 2]",
		"  ui4: [1, 2, 3, 4]",
		"  s: -1",
		"  s4: [-1, -2, -3, -4]",
		"  us: 1",
		"  us4: [1, 2, 3, 4]",
		"  f: 1.000000",
		"  f4: [1.000000, 2.000000, 3.000000, 4.000000]",
		"  l: -1",
		piNullptr,
		"  rvi: -1",
		"  by: 97",
		"  by4: [255, 254, 253, 252]",
	};
	constexpr size_t expectedSize = sizeof(expected) / sizeof(expected[0]);

	StdOutCapture capture;
	capture.start();
	routine();
	auto output = split(capture.stop());
	for(size_t i = 0, j = 1; i < expectedSize; ++i, j += 2)
	{
		ASSERT_EQ(expected[i], output[j]);
	}

#endif
}

TEST(ReactorUnitTests, Sample)
{
	FunctionT<int(int *, int)> function;
	{
		Pointer<Int> p = function.Arg<0>();
		Int x = p[-1];
		Int y = function.Arg<1>();
		Int z = 4;

		For(Int i = 0, i < 10, i++)
		{
			z += (2 << i) - (i / 3);
		}

		Float4 v;
		v.z = As<Float>(z);
		z = As<Int>(Float(Float4(v.xzxx).y));

		Int sum = x + y + z;

		Return(sum);
	}

	auto routine = function("one");

	int one[2] = { 1, 0 };
	int result = routine(&one[1], 2);
	EXPECT_EQ(result, reference(&one[1], 2));
}

TEST(ReactorUnitTests, Uninitialized)
{
	FunctionT<int()> function;
	{
		Int a;
		Int z = 4;
		Int q;
		Int c;
		Int p;
		Bool b;

		q += q;

		If(b)
		{
			c = p;
		}

		Return(a + z + q + c);
	}

	auto routine = function("one");

	int result = routine();
	EXPECT_EQ(result, result);  // Anything is fine, just don't crash
}

TEST(ReactorUnitTests, Unreachable)
{
	FunctionT<int(int)> function;
	{
		Int a = function.Arg<0>();
		Int z = 4;

		Return(a + z);

		// Code beyond this point is unreachable but should not cause any
		// compilation issues.

		z += a;
	}

	auto routine = function("one");

	int result = routine(16);
	EXPECT_EQ(result, 20);
}

TEST(ReactorUnitTests, VariableAddress)
{
	FunctionT<int(int)> function;
	{
		Int a = function.Arg<0>();
		Int z = 0;
		Pointer<Int> p = &z;
		*p = 4;

		Return(a + z);
	}

	auto routine = function("one");

	int result = routine(16);
	EXPECT_EQ(result, 20);
}

TEST(ReactorUnitTests, SubVectorLoadStore)
{
	FunctionT<int(void *, void *)> function;
	{
		Pointer<Byte> in = function.Arg<0>();
		Pointer<Byte> out = function.Arg<1>();

		*Pointer<Int4>(out + 16 * 0) = *Pointer<Int4>(in + 16 * 0);
		*Pointer<Short4>(out + 16 * 1) = *Pointer<Short4>(in + 16 * 1);
		*Pointer<Byte8>(out + 16 * 2) = *Pointer<Byte8>(in + 16 * 2);
		*Pointer<Byte4>(out + 16 * 3) = *Pointer<Byte4>(in + 16 * 3);
		*Pointer<Short2>(out + 16 * 4) = *Pointer<Short2>(in + 16 * 4);

		Return(0);
	}

	auto routine = function("one");

	int8_t in[16 * 5] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		                  17, 18, 19, 20, 21, 22, 23, 24, 0, 0, 0, 0, 0, 0, 0, 0,
		                  25, 26, 27, 28, 29, 30, 31, 32, 0, 0, 0, 0, 0, 0, 0, 0,
		                  33, 34, 35, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		                  37, 38, 39, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int8_t out[16 * 5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	routine(in, out);

	for(int row = 0; row < 5; row++)
	{
		for(int col = 0; col < 16; col++)
		{
			int i = row * 16 + col;

			if(in[i] == 0)
			{
				EXPECT_EQ(out[i], -1) << "Row " << row << " column " << col << " not left untouched.";
			}
			else
			{
				EXPECT_EQ(out[i], in[i]) << "Row " << row << " column " << col << " not equal to input.";
			}
		}
	}
}

TEST(ReactorUnitTests, VectorConstant)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Int4>(out + 16 * 0) = Int4(0x04030201, 0x08070605, 0x0C0B0A09, 0x100F0E0D);
		*Pointer<Short4>(out + 16 * 1) = Short4(0x1211, 0x1413, 0x1615, 0x1817);
		*Pointer<Byte8>(out + 16 * 2) = Byte8(0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20);
		*Pointer<Int2>(out + 16 * 3) = Int2(0x24232221, 0x28272625);

		Return(0);
	}

	auto routine = function("one");

	int8_t out[16 * 4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	int8_t exp[16 * 4] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		                   17, 18, 19, 20, 21, 22, 23, 24, -1, -1, -1, -1, -1, -1, -1, -1,
		                   25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1, -1, -1, -1,
		                   33, 34, 35, 36, 37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1 };

	routine(out);

	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 16; col++)
		{
			int i = row * 16 + col;

			EXPECT_EQ(out[i], exp[i]);
		}
	}
}

TEST(ReactorUnitTests, Concatenate)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Int4>(out + 16 * 0) = Int4(Int2(0x04030201, 0x08070605), Int2(0x0C0B0A09, 0x100F0E0D));
		*Pointer<Short8>(out + 16 * 1) = Short8(Short4(0x0201, 0x0403, 0x0605, 0x0807), Short4(0x0A09, 0x0C0B, 0x0E0D, 0x100F));

		Return(0);
	}

	auto routine = function("one");

	int8_t ref[16 * 5] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		                   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	int8_t out[16 * 5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	routine(out);

	for(int row = 0; row < 2; row++)
	{
		for(int col = 0; col < 16; col++)
		{
			int i = row * 16 + col;

			EXPECT_EQ(out[i], ref[i]) << "Row " << row << " column " << col << " not equal to reference.";
		}
	}
}

TEST(ReactorUnitTests, Cast)
{
	FunctionT<void(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		Int4 c = Int4(0x01020304, 0x05060708, 0x09101112, 0x13141516);
		*Pointer<Short4>(out + 16 * 0) = Short4(c);
		*Pointer<Byte4>(out + 16 * 1 + 0) = Byte4(c);
		*Pointer<Byte4>(out + 16 * 1 + 4) = Byte4(As<Byte8>(c));
		*Pointer<Byte4>(out + 16 * 1 + 8) = Byte4(As<Short4>(c));
	}

	auto routine = function("one");

	int out[2][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x07080304);
	EXPECT_EQ(out[0][1], 0x15161112);

	EXPECT_EQ(out[1][0], 0x16120804);
	EXPECT_EQ(out[1][1], 0x01020304);
	EXPECT_EQ(out[1][2], 0x06080204);
}

static uint16_t swizzleCode4(int i)
{
	auto x = (i >> 0) & 0x03;
	auto y = (i >> 2) & 0x03;
	auto z = (i >> 4) & 0x03;
	auto w = (i >> 6) & 0x03;
	return static_cast<uint16_t>((x << 12) | (y << 8) | (z << 4) | (w << 0));
}

TEST(ReactorUnitTests, Swizzle4)
{
	FunctionT<void(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		for(int i = 0; i < 256; i++)
		{
			*Pointer<Float4>(out + 16 * i) = Swizzle(Float4(1.0f, 2.0f, 3.0f, 4.0f), swizzleCode4(i));
		}

		for(int i = 0; i < 256; i++)
		{
			*Pointer<Float4>(out + 16 * (256 + i)) = ShuffleLowHigh(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f), swizzleCode4(i));
		}

		*Pointer<Float4>(out + 16 * (512 + 0)) = UnpackLow(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f));
		*Pointer<Float4>(out + 16 * (512 + 1)) = UnpackHigh(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f));
		*Pointer<Int2>(out + 16 * (512 + 2)) = UnpackLow(Short4(1, 2, 3, 4), Short4(5, 6, 7, 8));
		*Pointer<Int2>(out + 16 * (512 + 3)) = UnpackHigh(Short4(1, 2, 3, 4), Short4(5, 6, 7, 8));
		*Pointer<Short4>(out + 16 * (512 + 4)) = UnpackLow(Byte8(1, 2, 3, 4, 5, 6, 7, 8), Byte8(9, 10, 11, 12, 13, 14, 15, 16));
		*Pointer<Short4>(out + 16 * (512 + 5)) = UnpackHigh(Byte8(1, 2, 3, 4, 5, 6, 7, 8), Byte8(9, 10, 11, 12, 13, 14, 15, 16));

		for(int i = 0; i < 256; i++)
		{
			*Pointer<Short4>(out + 16 * (512 + 6) + (8 * i)) =
			    Swizzle(Short4(1, 2, 3, 4), swizzleCode4(i));
		}

		for(int i = 0; i < 256; i++)
		{
			*Pointer<Int4>(out + 16 * (512 + 6 + i) + (8 * 256)) =
			    Swizzle(Int4(1, 2, 3, 4), swizzleCode4(i));
		}
	}

	auto routine = function("one");

	struct
	{
		float f[256 + 256 + 2][4];
		int i[388][4];
	} out;

	memset(&out, 0, sizeof(out));

	routine(&out);

	for(int i = 0; i < 256; i++)
	{
		EXPECT_EQ(out.f[i][0], float((i >> 0) & 0x03) + 1.0f);
		EXPECT_EQ(out.f[i][1], float((i >> 2) & 0x03) + 1.0f);
		EXPECT_EQ(out.f[i][2], float((i >> 4) & 0x03) + 1.0f);
		EXPECT_EQ(out.f[i][3], float((i >> 6) & 0x03) + 1.0f);
	}

	for(int i = 0; i < 256; i++)
	{
		EXPECT_EQ(out.f[256 + i][0], float((i >> 0) & 0x03) + 1.0f);
		EXPECT_EQ(out.f[256 + i][1], float((i >> 2) & 0x03) + 1.0f);
		EXPECT_EQ(out.f[256 + i][2], float((i >> 4) & 0x03) + 5.0f);
		EXPECT_EQ(out.f[256 + i][3], float((i >> 6) & 0x03) + 5.0f);
	}

	EXPECT_EQ(out.f[512 + 0][0], 1.0f);
	EXPECT_EQ(out.f[512 + 0][1], 5.0f);
	EXPECT_EQ(out.f[512 + 0][2], 2.0f);
	EXPECT_EQ(out.f[512 + 0][3], 6.0f);

	EXPECT_EQ(out.f[512 + 1][0], 3.0f);
	EXPECT_EQ(out.f[512 + 1][1], 7.0f);
	EXPECT_EQ(out.f[512 + 1][2], 4.0f);
	EXPECT_EQ(out.f[512 + 1][3], 8.0f);

	EXPECT_EQ(out.i[0][0], 0x00050001);
	EXPECT_EQ(out.i[0][1], 0x00060002);
	EXPECT_EQ(out.i[0][2], 0x00000000);
	EXPECT_EQ(out.i[0][3], 0x00000000);

	EXPECT_EQ(out.i[1][0], 0x00070003);
	EXPECT_EQ(out.i[1][1], 0x00080004);
	EXPECT_EQ(out.i[1][2], 0x00000000);
	EXPECT_EQ(out.i[1][3], 0x00000000);

	EXPECT_EQ(out.i[2][0], 0x0A020901);
	EXPECT_EQ(out.i[2][1], 0x0C040B03);
	EXPECT_EQ(out.i[2][2], 0x00000000);
	EXPECT_EQ(out.i[2][3], 0x00000000);

	EXPECT_EQ(out.i[3][0], 0x0E060D05);
	EXPECT_EQ(out.i[3][1], 0x10080F07);
	EXPECT_EQ(out.i[3][2], 0x00000000);
	EXPECT_EQ(out.i[3][3], 0x00000000);

	for(int i = 0; i < 256; i++)
	{
		EXPECT_EQ(out.i[4 + i / 2][0 + (i % 2) * 2] & 0xFFFF,
		          ((i >> 0) & 0x03) + 1);
		EXPECT_EQ(out.i[4 + i / 2][0 + (i % 2) * 2] >> 16,
		          ((i >> 2) & 0x03) + 1);
		EXPECT_EQ(out.i[4 + i / 2][1 + (i % 2) * 2] & 0xFFFF,
		          ((i >> 4) & 0x03) + 1);
		EXPECT_EQ(out.i[4 + i / 2][1 + (i % 2) * 2] >> 16,
		          ((i >> 6) & 0x03) + 1);
	}

	for(int i = 0; i < 256; i++)
	{
		EXPECT_EQ(out.i[132 + i][0], ((i >> 0) & 0x03) + 1);
		EXPECT_EQ(out.i[132 + i][1], ((i >> 2) & 0x03) + 1);
		EXPECT_EQ(out.i[132 + i][2], ((i >> 4) & 0x03) + 1);
		EXPECT_EQ(out.i[132 + i][3], ((i >> 6) & 0x03) + 1);
	}
}

TEST(ReactorUnitTests, Swizzle)
{
	FunctionT<void(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		Int4 c = Int4(0x01020304, 0x05060708, 0x09101112, 0x13141516);
		*Pointer<Byte16>(out + 16 * 0) = Swizzle(As<Byte16>(c), 0xFEDCBA9876543210ull);
		*Pointer<Byte8>(out + 16 * 1) = Swizzle(As<Byte8>(c), 0x76543210u);
		*Pointer<UShort8>(out + 16 * 2) = Swizzle(As<UShort8>(c), 0x76543210u);
	}

	auto routine = function("one");

	int out[3][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x16151413);
	EXPECT_EQ(out[0][1], 0x12111009);
	EXPECT_EQ(out[0][2], 0x08070605);
	EXPECT_EQ(out[0][3], 0x04030201);

	EXPECT_EQ(out[1][0], 0x08070605);
	EXPECT_EQ(out[1][1], 0x04030201);

	EXPECT_EQ(out[2][0], 0x15161314);
	EXPECT_EQ(out[2][1], 0x11120910);
	EXPECT_EQ(out[2][2], 0x07080506);
	EXPECT_EQ(out[2][3], 0x03040102);
}

TEST(ReactorUnitTests, Shuffle)
{
	// |select| is [0aaa:0bbb:0ccc:0ddd] where |aaa|, |bbb|, |ccc|
	// and |ddd| are 7-bit selection indices. For a total (1 << 12)
	// possibilities.
	const int kSelectRange = 1 << 12;

	// Unfortunately, testing the whole kSelectRange results in a test
	// that is far too slow to run, because LLVM spends exponentially more
	// time optimizing the function below as the number of test cases
	// increases.
	//
	// To work-around the problem, only test a subset of the range by
	// skipping every kRangeIncrement value.
	//
	// Set this value to 1 if you want to test the whole implementation,
	// which will take a little less than 2 minutes on a fast workstation.
	//
	// The default value here takes about 1390ms, which is a little more than
	// what the Swizzle test takes (993 ms) on my machine. A non-power-of-2
	// value ensures a better spread over possible values.
	const int kRangeIncrement = 11;

	auto rangeIndexToSelect = [](int i) {
		return static_cast<unsigned short>(
		    (((i >> 9) & 7) << 0) |
		    (((i >> 6) & 7) << 4) |
		    (((i >> 3) & 7) << 8) |
		    (((i >> 0) & 7) << 12));
	};

	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		for(int i = 0; i < kSelectRange; i += kRangeIncrement)
		{
			unsigned short select = rangeIndexToSelect(i);

			*Pointer<Float4>(out + 16 * i) = Shuffle(Float4(1.0f, 2.0f, 3.0f, 4.0f),
			                                         Float4(5.0f, 6.0f, 7.0f, 8.0f),
			                                         select);

			*Pointer<Int4>(out + (kSelectRange + i) * 16) = Shuffle(Int4(10, 11, 12, 13),
			                                                        Int4(14, 15, 16, 17),
			                                                        select);

			*Pointer<UInt4>(out + (2 * kSelectRange + i) * 16) = Shuffle(UInt4(100, 101, 102, 103),
			                                                             UInt4(104, 105, 106, 107),
			                                                             select);
		}

		Return(0);
	}

	auto routine = function("one");

	struct
	{
		float f[kSelectRange][4];
		int i[kSelectRange][4];
		unsigned u[kSelectRange][4];
	} out;

	memset(&out, 0, sizeof(out));

	routine(&out);

	for(int i = 0; i < kSelectRange; i += kRangeIncrement)
	{
		EXPECT_EQ(out.f[i][0], float(1.0f + (i & 7)));
		EXPECT_EQ(out.f[i][1], float(1.0f + ((i >> 3) & 7)));
		EXPECT_EQ(out.f[i][2], float(1.0f + ((i >> 6) & 7)));
		EXPECT_EQ(out.f[i][3], float(1.0f + ((i >> 9) & 7)));
	}

	for(int i = 0; i < kSelectRange; i += kRangeIncrement)
	{
		EXPECT_EQ(out.i[i][0], int(10 + (i & 7)));
		EXPECT_EQ(out.i[i][1], int(10 + ((i >> 3) & 7)));
		EXPECT_EQ(out.i[i][2], int(10 + ((i >> 6) & 7)));
		EXPECT_EQ(out.i[i][3], int(10 + ((i >> 9) & 7)));
	}

	for(int i = 0; i < kSelectRange; i += kRangeIncrement)
	{
		EXPECT_EQ(out.u[i][0], unsigned(100 + (i & 7)));
		EXPECT_EQ(out.u[i][1], unsigned(100 + ((i >> 3) & 7)));
		EXPECT_EQ(out.u[i][2], unsigned(100 + ((i >> 6) & 7)));
		EXPECT_EQ(out.u[i][3], unsigned(100 + ((i >> 9) & 7)));
	}
}

TEST(ReactorUnitTests, Branching)
{
	FunctionT<int()> function;
	{
		Int x = 0;

		For(Int i = 0, i < 8, i++)
		{
			If(i < 2)
			{
				x += 1;
			}
			Else If(i < 4)
			{
				x += 10;
			}
			Else If(i < 6)
			{
				x += 100;
			}
			Else
			{
				x += 1000;
			}

			For(Int i = 0, i < 5, i++)
			    x += 10000;
		}

		For(Int i = 0, i < 10, i++) for(int i = 0; i < 10; i++)
		    For(Int i = 0, i < 10, i++)
		{
			x += 1000000;
		}

		For(Int i = 0, i < 2, i++)
		    If(x == 1000402222)
		{
			If(x != 1000402222)
			    x += 1000000000;
		}
		Else
		    x = -5;

		Return(x);
	}

	auto routine = function("one");

	int result = routine();

	EXPECT_EQ(result, 1000402222);
}

TEST(ReactorUnitTests, MinMax)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Float4>(out + 16 * 0) = Min(Float4(1.0f, 0.0f, -0.0f, +0.0f), Float4(0.0f, 1.0f, +0.0f, -0.0f));
		*Pointer<Float4>(out + 16 * 1) = Max(Float4(1.0f, 0.0f, -0.0f, +0.0f), Float4(0.0f, 1.0f, +0.0f, -0.0f));

		*Pointer<Int4>(out + 16 * 2) = Min(Int4(1, 0, -1, -0), Int4(0, 1, 0, +0));
		*Pointer<Int4>(out + 16 * 3) = Max(Int4(1, 0, -1, -0), Int4(0, 1, 0, +0));
		*Pointer<UInt4>(out + 16 * 4) = Min(UInt4(1, 0, -1, -0), UInt4(0, 1, 0, +0));
		*Pointer<UInt4>(out + 16 * 5) = Max(UInt4(1, 0, -1, -0), UInt4(0, 1, 0, +0));

		*Pointer<Short4>(out + 16 * 6) = Min(Short4(1, 0, -1, -0), Short4(0, 1, 0, +0));
		*Pointer<Short4>(out + 16 * 7) = Max(Short4(1, 0, -1, -0), Short4(0, 1, 0, +0));
		*Pointer<UShort4>(out + 16 * 8) = Min(UShort4(1, 0, -1, -0), UShort4(0, 1, 0, +0));
		*Pointer<UShort4>(out + 16 * 9) = Max(UShort4(1, 0, -1, -0), UShort4(0, 1, 0, +0));

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[10][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x00000000u);
	EXPECT_EQ(out[0][1], 0x00000000u);
	EXPECT_EQ(out[0][2], 0x00000000u);
	EXPECT_EQ(out[0][3], 0x80000000u);

	EXPECT_EQ(out[1][0], 0x3F800000u);
	EXPECT_EQ(out[1][1], 0x3F800000u);
	EXPECT_EQ(out[1][2], 0x00000000u);
	EXPECT_EQ(out[1][3], 0x80000000u);

	EXPECT_EQ(out[2][0], 0x00000000u);
	EXPECT_EQ(out[2][1], 0x00000000u);
	EXPECT_EQ(out[2][2], 0xFFFFFFFFu);
	EXPECT_EQ(out[2][3], 0x00000000u);

	EXPECT_EQ(out[3][0], 0x00000001u);
	EXPECT_EQ(out[3][1], 0x00000001u);
	EXPECT_EQ(out[3][2], 0x00000000u);
	EXPECT_EQ(out[3][3], 0x00000000u);

	EXPECT_EQ(out[4][0], 0x00000000u);
	EXPECT_EQ(out[4][1], 0x00000000u);
	EXPECT_EQ(out[4][2], 0x00000000u);
	EXPECT_EQ(out[4][3], 0x00000000u);

	EXPECT_EQ(out[5][0], 0x00000001u);
	EXPECT_EQ(out[5][1], 0x00000001u);
	EXPECT_EQ(out[5][2], 0xFFFFFFFFu);
	EXPECT_EQ(out[5][3], 0x00000000u);

	EXPECT_EQ(out[6][0], 0x00000000u);
	EXPECT_EQ(out[6][1], 0x0000FFFFu);
	EXPECT_EQ(out[6][2], 0x00000000u);
	EXPECT_EQ(out[6][3], 0x00000000u);

	EXPECT_EQ(out[7][0], 0x00010001u);
	EXPECT_EQ(out[7][1], 0x00000000u);
	EXPECT_EQ(out[7][2], 0x00000000u);
	EXPECT_EQ(out[7][3], 0x00000000u);

	EXPECT_EQ(out[8][0], 0x00000000u);
	EXPECT_EQ(out[8][1], 0x00000000u);
	EXPECT_EQ(out[8][2], 0x00000000u);
	EXPECT_EQ(out[8][3], 0x00000000u);

	EXPECT_EQ(out[9][0], 0x00010001u);
	EXPECT_EQ(out[9][1], 0x0000FFFFu);
	EXPECT_EQ(out[9][2], 0x00000000u);
	EXPECT_EQ(out[9][3], 0x00000000u);
}

TEST(ReactorUnitTests, NotNeg)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Int>(out + 16 * 0) = ~Int(0x55555555);
		*Pointer<Short>(out + 16 * 1) = ~Short(0x5555);
		*Pointer<Int4>(out + 16 * 2) = ~Int4(0x55555555, 0xAAAAAAAA, 0x00000000, 0xFFFFFFFF);
		*Pointer<Short4>(out + 16 * 3) = ~Short4(0x5555, 0xAAAA, 0x0000, 0xFFFF);

		*Pointer<Int>(out + 16 * 4) = -Int(0x55555555);
		*Pointer<Short>(out + 16 * 5) = -Short(0x5555);
		*Pointer<Int4>(out + 16 * 6) = -Int4(0x55555555, 0xAAAAAAAA, 0x00000000, 0xFFFFFFFF);
		*Pointer<Short4>(out + 16 * 7) = -Short4(0x5555, 0xAAAA, 0x0000, 0xFFFF);

		*Pointer<Float4>(out + 16 * 8) = -Float4(1.0f, -1.0f, 0.0f, -0.0f);

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[10][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0xAAAAAAAAu);
	EXPECT_EQ(out[0][1], 0x00000000u);
	EXPECT_EQ(out[0][2], 0x00000000u);
	EXPECT_EQ(out[0][3], 0x00000000u);

	EXPECT_EQ(out[1][0], 0x0000AAAAu);
	EXPECT_EQ(out[1][1], 0x00000000u);
	EXPECT_EQ(out[1][2], 0x00000000u);
	EXPECT_EQ(out[1][3], 0x00000000u);

	EXPECT_EQ(out[2][0], 0xAAAAAAAAu);
	EXPECT_EQ(out[2][1], 0x55555555u);
	EXPECT_EQ(out[2][2], 0xFFFFFFFFu);
	EXPECT_EQ(out[2][3], 0x00000000u);

	EXPECT_EQ(out[3][0], 0x5555AAAAu);
	EXPECT_EQ(out[3][1], 0x0000FFFFu);
	EXPECT_EQ(out[3][2], 0x00000000u);
	EXPECT_EQ(out[3][3], 0x00000000u);

	EXPECT_EQ(out[4][0], 0xAAAAAAABu);
	EXPECT_EQ(out[4][1], 0x00000000u);
	EXPECT_EQ(out[4][2], 0x00000000u);
	EXPECT_EQ(out[4][3], 0x00000000u);

	EXPECT_EQ(out[5][0], 0x0000AAABu);
	EXPECT_EQ(out[5][1], 0x00000000u);
	EXPECT_EQ(out[5][2], 0x00000000u);
	EXPECT_EQ(out[5][3], 0x00000000u);

	EXPECT_EQ(out[6][0], 0xAAAAAAABu);
	EXPECT_EQ(out[6][1], 0x55555556u);
	EXPECT_EQ(out[6][2], 0x00000000u);
	EXPECT_EQ(out[6][3], 0x00000001u);

	EXPECT_EQ(out[7][0], 0x5556AAABu);
	EXPECT_EQ(out[7][1], 0x00010000u);
	EXPECT_EQ(out[7][2], 0x00000000u);
	EXPECT_EQ(out[7][3], 0x00000000u);

	EXPECT_EQ(out[8][0], 0xBF800000u);
	EXPECT_EQ(out[8][1], 0x3F800000u);
	EXPECT_EQ(out[8][2], 0x80000000u);
	EXPECT_EQ(out[8][3], 0x00000000u);
}

TEST(ReactorUnitTests, FPtoUI)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<UInt>(out + 0) = UInt(Float(0xF0000000u));
		*Pointer<UInt>(out + 4) = UInt(Float(0xC0000000u));
		*Pointer<UInt>(out + 8) = UInt(Float(0x00000001u));
		*Pointer<UInt>(out + 12) = UInt(Float(0xF000F000u));

		*Pointer<UInt4>(out + 16) = UInt4(Float4(0xF0000000u, 0x80000000u, 0x00000000u, 0xCCCC0000u));

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[2][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0xF0000000u);
	EXPECT_EQ(out[0][1], 0xC0000000u);
	EXPECT_EQ(out[0][2], 0x00000001u);
	EXPECT_EQ(out[0][3], 0xF000F000u);

	EXPECT_EQ(out[1][0], 0xF0000000u);
	EXPECT_EQ(out[1][1], 0x80000000u);
	EXPECT_EQ(out[1][2], 0x00000000u);
	EXPECT_EQ(out[1][3], 0xCCCC0000u);
}

TEST(ReactorUnitTests, VectorCompare)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Int4>(out + 16 * 0) = CmpEQ(Float4(1.0f, 1.0f, -0.0f, +0.0f), Float4(0.0f, 1.0f, +0.0f, -0.0f));
		*Pointer<Int4>(out + 16 * 1) = CmpEQ(Int4(1, 0, -1, -0), Int4(0, 1, 0, +0));
		*Pointer<Byte8>(out + 16 * 2) = CmpEQ(SByte8(1, 2, 3, 4, 5, 6, 7, 8), SByte8(7, 6, 5, 4, 3, 2, 1, 0));

		*Pointer<Int4>(out + 16 * 3) = CmpNLT(Float4(1.0f, 1.0f, -0.0f, +0.0f), Float4(0.0f, 1.0f, +0.0f, -0.0f));
		*Pointer<Int4>(out + 16 * 4) = CmpNLT(Int4(1, 0, -1, -0), Int4(0, 1, 0, +0));
		*Pointer<Byte8>(out + 16 * 5) = CmpGT(SByte8(1, 2, 3, 4, 5, 6, 7, 8), SByte8(7, 6, 5, 4, 3, 2, 1, 0));

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[6][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x00000000u);
	EXPECT_EQ(out[0][1], 0xFFFFFFFFu);
	EXPECT_EQ(out[0][2], 0xFFFFFFFFu);
	EXPECT_EQ(out[0][3], 0xFFFFFFFFu);

	EXPECT_EQ(out[1][0], 0x00000000u);
	EXPECT_EQ(out[1][1], 0x00000000u);
	EXPECT_EQ(out[1][2], 0x00000000u);
	EXPECT_EQ(out[1][3], 0xFFFFFFFFu);

	EXPECT_EQ(out[2][0], 0xFF000000u);
	EXPECT_EQ(out[2][1], 0x00000000u);

	EXPECT_EQ(out[3][0], 0xFFFFFFFFu);
	EXPECT_EQ(out[3][1], 0xFFFFFFFFu);
	EXPECT_EQ(out[3][2], 0xFFFFFFFFu);
	EXPECT_EQ(out[3][3], 0xFFFFFFFFu);

	EXPECT_EQ(out[4][0], 0xFFFFFFFFu);
	EXPECT_EQ(out[4][1], 0x00000000u);
	EXPECT_EQ(out[4][2], 0x00000000u);
	EXPECT_EQ(out[4][3], 0xFFFFFFFFu);

	EXPECT_EQ(out[5][0], 0x00000000u);
	EXPECT_EQ(out[5][1], 0xFFFFFFFFu);
}

TEST(ReactorUnitTests, SaturatedAddAndSubtract)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Byte8>(out + 8 * 0) =
		    AddSat(Byte8(1, 2, 3, 4, 5, 6, 7, 8),
		           Byte8(7, 6, 5, 4, 3, 2, 1, 0));
		*Pointer<Byte8>(out + 8 * 1) =
		    AddSat(Byte8(0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE),
		           Byte8(7, 6, 5, 4, 3, 2, 1, 0));
		*Pointer<Byte8>(out + 8 * 2) =
		    SubSat(Byte8(1, 2, 3, 4, 5, 6, 7, 8),
		           Byte8(7, 6, 5, 4, 3, 2, 1, 0));

		*Pointer<SByte8>(out + 8 * 3) =
		    AddSat(SByte8(1, 2, 3, 4, 5, 6, 7, 8),
		           SByte8(7, 6, 5, 4, 3, 2, 1, 0));
		*Pointer<SByte8>(out + 8 * 4) =
		    AddSat(SByte8(0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E),
		           SByte8(7, 6, 5, 4, 3, 2, 1, 0));
		*Pointer<SByte8>(out + 8 * 5) =
		    AddSat(SByte8(0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88),
		           SByte8(-7, -6, -5, -4, -3, -2, -1, -0));
		*Pointer<SByte8>(out + 8 * 6) =
		    SubSat(SByte8(0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88),
		           SByte8(7, 6, 5, 4, 3, 2, 1, 0));

		*Pointer<Short4>(out + 8 * 7) =
		    AddSat(Short4(1, 2, 3, 4), Short4(3, 2, 1, 0));
		*Pointer<Short4>(out + 8 * 8) =
		    AddSat(Short4(0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE),
		           Short4(3, 2, 1, 0));
		*Pointer<Short4>(out + 8 * 9) =
		    AddSat(Short4(0x8001, 0x8002, 0x8003, 0x8004),
		           Short4(-3, -2, -1, -0));
		*Pointer<Short4>(out + 8 * 10) =
		    SubSat(Short4(0x8001, 0x8002, 0x8003, 0x8004),
		           Short4(3, 2, 1, 0));

		*Pointer<UShort4>(out + 8 * 11) =
		    AddSat(UShort4(1, 2, 3, 4), UShort4(3, 2, 1, 0));
		*Pointer<UShort4>(out + 8 * 12) =
		    AddSat(UShort4(0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE),
		           UShort4(3, 2, 1, 0));
		*Pointer<UShort4>(out + 8 * 13) =
		    SubSat(UShort4(1, 2, 3, 4), UShort4(3, 2, 1, 0));

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[14][2];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x08080808u);
	EXPECT_EQ(out[0][1], 0x08080808u);

	EXPECT_EQ(out[1][0], 0xFFFFFFFFu);
	EXPECT_EQ(out[1][1], 0xFEFFFFFFu);

	EXPECT_EQ(out[2][0], 0x00000000u);
	EXPECT_EQ(out[2][1], 0x08060402u);

	EXPECT_EQ(out[3][0], 0x08080808u);
	EXPECT_EQ(out[3][1], 0x08080808u);

	EXPECT_EQ(out[4][0], 0x7F7F7F7Fu);
	EXPECT_EQ(out[4][1], 0x7E7F7F7Fu);

	EXPECT_EQ(out[5][0], 0x80808080u);
	EXPECT_EQ(out[5][1], 0x88868482u);

	EXPECT_EQ(out[6][0], 0x80808080u);
	EXPECT_EQ(out[6][1], 0x88868482u);

	EXPECT_EQ(out[7][0], 0x00040004u);
	EXPECT_EQ(out[7][1], 0x00040004u);

	EXPECT_EQ(out[8][0], 0x7FFF7FFFu);
	EXPECT_EQ(out[8][1], 0x7FFE7FFFu);

	EXPECT_EQ(out[9][0], 0x80008000u);
	EXPECT_EQ(out[9][1], 0x80048002u);

	EXPECT_EQ(out[10][0], 0x80008000u);
	EXPECT_EQ(out[10][1], 0x80048002u);

	EXPECT_EQ(out[11][0], 0x00040004u);
	EXPECT_EQ(out[11][1], 0x00040004u);

	EXPECT_EQ(out[12][0], 0xFFFFFFFFu);
	EXPECT_EQ(out[12][1], 0xFFFEFFFFu);

	EXPECT_EQ(out[13][0], 0x00000000u);
	EXPECT_EQ(out[13][1], 0x00040002u);
}

TEST(ReactorUnitTests, Unpack)
{
	FunctionT<int(void *, void *)> function;
	{
		Pointer<Byte> in = function.Arg<0>();
		Pointer<Byte> out = function.Arg<1>();

		Byte4 test_byte_a = *Pointer<Byte4>(in + 4 * 0);
		Byte4 test_byte_b = *Pointer<Byte4>(in + 4 * 1);

		*Pointer<Short4>(out + 8 * 0) =
		    Unpack(test_byte_a, test_byte_b);

		*Pointer<Short4>(out + 8 * 1) = Unpack(test_byte_a);

		Return(0);
	}

	auto routine = function("one");

	unsigned int in[1][2];
	unsigned int out[2][2];

	memset(&out, 0, sizeof(out));

	in[0][0] = 0xABCDEF12u;
	in[0][1] = 0x34567890u;

	routine(&in, &out);

	EXPECT_EQ(out[0][0], 0x78EF9012u);
	EXPECT_EQ(out[0][1], 0x34AB56CDu);

	EXPECT_EQ(out[1][0], 0xEFEF1212u);
	EXPECT_EQ(out[1][1], 0xABABCDCDu);
}

TEST(ReactorUnitTests, Pack)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<SByte8>(out + 8 * 0) =
		    PackSigned(Short4(-1, -2, 1, 2),
		               Short4(3, 4, -3, -4));

		*Pointer<Byte8>(out + 8 * 1) =
		    PackUnsigned(Short4(-1, -2, 1, 2),
		                 Short4(3, 4, -3, -4));

		*Pointer<Short8>(out + 8 * 2) =
		    PackSigned(Int4(-1, -2, 1, 2),
		               Int4(3, 4, -3, -4));

		*Pointer<UShort8>(out + 8 * 4) =
		    PackUnsigned(Int4(-1, -2, 1, 2),
		                 Int4(3, 4, -3, -4));

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[6][2];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x0201FEFFu);
	EXPECT_EQ(out[0][1], 0xFCFD0403u);

	EXPECT_EQ(out[1][0], 0x02010000u);
	EXPECT_EQ(out[1][1], 0x00000403u);

	EXPECT_EQ(out[2][0], 0xFFFEFFFFu);
	EXPECT_EQ(out[2][1], 0x00020001u);

	EXPECT_EQ(out[3][0], 0x00040003u);
	EXPECT_EQ(out[3][1], 0xFFFCFFFDu);

	EXPECT_EQ(out[4][0], 0x00000000u);
	EXPECT_EQ(out[4][1], 0x00020001u);

	EXPECT_EQ(out[5][0], 0x00040003u);
	EXPECT_EQ(out[5][1], 0x00000000u);
}

TEST(ReactorUnitTests, MulHigh)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Short4>(out + 16 * 0) =
		    MulHigh(Short4(0x01AA, 0x02DD, 0x03EE, 0xF422),
		            Short4(0x01BB, 0x02CC, 0x03FF, 0xF411));
		*Pointer<UShort4>(out + 16 * 1) =
		    MulHigh(UShort4(0x01AA, 0x02DD, 0x03EE, 0xF422),
		            UShort4(0x01BB, 0x02CC, 0x03FF, 0xF411));

		*Pointer<Int4>(out + 16 * 2) =
		    MulHigh(Int4(0x000001AA, 0x000002DD, 0xC8000000, 0xF8000000),
		            Int4(0x000001BB, 0x84000000, 0x000003EE, 0xD7000000));
		*Pointer<UInt4>(out + 16 * 3) =
		    MulHigh(UInt4(0x000001AAu, 0x000002DDu, 0xC8000000u, 0xD8000000u),
		            UInt4(0x000001BBu, 0x84000000u, 0x000003EEu, 0xD7000000u));

		*Pointer<Int4>(out + 16 * 4) =
		    MulHigh(Int4(0x7FFFFFFF, 0x7FFFFFFF, 0x80008000, 0xFFFFFFFF),
		            Int4(0x7FFFFFFF, 0x80000000, 0x80008000, 0xFFFFFFFF));
		*Pointer<UInt4>(out + 16 * 5) =
		    MulHigh(UInt4(0x7FFFFFFFu, 0x7FFFFFFFu, 0x80008000u, 0xFFFFFFFFu),
		            UInt4(0x7FFFFFFFu, 0x80000000u, 0x80008000u, 0xFFFFFFFFu));

		// (U)Short8 variants currently unimplemented.

		Return(0);
	}

	auto routine = function("one");

	unsigned int out[6][4];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x00080002u);
	EXPECT_EQ(out[0][1], 0x008D000Fu);

	EXPECT_EQ(out[1][0], 0x00080002u);
	EXPECT_EQ(out[1][1], 0xE8C0000Fu);

	EXPECT_EQ(out[2][0], 0x00000000u);
	EXPECT_EQ(out[2][1], 0xFFFFFE9Cu);
	EXPECT_EQ(out[2][2], 0xFFFFFF23u);
	EXPECT_EQ(out[2][3], 0x01480000u);

	EXPECT_EQ(out[3][0], 0x00000000u);
	EXPECT_EQ(out[3][1], 0x00000179u);
	EXPECT_EQ(out[3][2], 0x00000311u);
	EXPECT_EQ(out[3][3], 0xB5680000u);

	EXPECT_EQ(out[4][0], 0x3FFFFFFFu);
	EXPECT_EQ(out[4][1], 0xC0000000u);
	EXPECT_EQ(out[4][2], 0x3FFF8000u);
	EXPECT_EQ(out[4][3], 0x00000000u);

	EXPECT_EQ(out[5][0], 0x3FFFFFFFu);
	EXPECT_EQ(out[5][1], 0x3FFFFFFFu);
	EXPECT_EQ(out[5][2], 0x40008000u);
	EXPECT_EQ(out[5][3], 0xFFFFFFFEu);
}

TEST(ReactorUnitTests, MulAdd)
{
	FunctionT<int(void *)> function;
	{
		Pointer<Byte> out = function.Arg<0>();

		*Pointer<Int2>(out + 8 * 0) =
		    MulAdd(Short4(0x1aa, 0x2dd, 0x3ee, 0xF422),
		           Short4(0x1bb, 0x2cc, 0x3ff, 0xF411));

		// (U)Short8 variant is mentioned but unimplemented
		Return(0);
	}

	auto routine = function("one");

	unsigned int out[1][2];

	memset(&out, 0, sizeof(out));

	routine(&out);

	EXPECT_EQ(out[0][0], 0x000AE34Au);
	EXPECT_EQ(out[0][1], 0x009D5254u);
}

TEST(ReactorUnitTests, PointersEqual)
{
	FunctionT<int(void *, void *)> function;
	{
		Pointer<Byte> ptrA = function.Arg<0>();
		Pointer<Byte> ptrB = function.Arg<1>();
		If(ptrA == ptrB)
		{
			Return(1);
		}
		Else
		{
			Return(0);
		}
	}

	auto routine = function("one");
	int *a = reinterpret_cast<int *>(uintptr_t(0x0000000000000000));
	int *b = reinterpret_cast<int *>(uintptr_t(0x00000000F0000000));
	int *c = reinterpret_cast<int *>(uintptr_t(0xF000000000000000));
	EXPECT_EQ(routine(&a, &a), 1);
	EXPECT_EQ(routine(&b, &b), 1);
	EXPECT_EQ(routine(&c, &c), 1);

	EXPECT_EQ(routine(&a, &b), 0);
	EXPECT_EQ(routine(&b, &a), 0);
	EXPECT_EQ(routine(&b, &c), 0);
	EXPECT_EQ(routine(&c, &b), 0);
	EXPECT_EQ(routine(&c, &a), 0);
	EXPECT_EQ(routine(&a, &c), 0);
}

TEST(ReactorUnitTests, Args_2Mixed)
{
	// 2 mixed type args
	FunctionT<float(int, float)> function;
	{
		Int a = function.Arg<0>();
		Float b = function.Arg<1>();
		Return(Float(a) + b);
	}

	if(auto routine = function("one"))
	{
		float result = routine(1, 2.f);
		EXPECT_EQ(result, 3.f);
	}
}

TEST(ReactorUnitTests, Args_4Mixed)
{
	// 4 mixed type args (max register allocation on Windows)
	FunctionT<float(int, float, int, float)> function;
	{
		Int a = function.Arg<0>();
		Float b = function.Arg<1>();
		Int c = function.Arg<2>();
		Float d = function.Arg<3>();
		Return(Float(a) + b + Float(c) + d);
	}

	if(auto routine = function("one"))
	{
		float result = routine(1, 2.f, 3, 4.f);
		EXPECT_EQ(result, 10.f);
	}
}

TEST(ReactorUnitTests, Args_5Mixed)
{
	// 5 mixed type args (5th spills over to stack on Windows)
	FunctionT<float(int, float, int, float, int)> function;
	{
		Int a = function.Arg<0>();
		Float b = function.Arg<1>();
		Int c = function.Arg<2>();
		Float d = function.Arg<3>();
		Int e = function.Arg<4>();
		Return(Float(a) + b + Float(c) + d + Float(e));
	}

	if(auto routine = function("one"))
	{
		float result = routine(1, 2.f, 3, 4.f, 5);
		EXPECT_EQ(result, 15.f);
	}
}

TEST(ReactorUnitTests, Args_GreaterThan5Mixed)
{
	// >5 mixed type args
	FunctionT<float(int, float, int, float, int, float, int, float, int, float)> function;
	{
		Int a = function.Arg<0>();
		Float b = function.Arg<1>();
		Int c = function.Arg<2>();
		Float d = function.Arg<3>();
		Int e = function.Arg<4>();
		Float f = function.Arg<5>();
		Int g = function.Arg<6>();
		Float h = function.Arg<7>();
		Int i = function.Arg<8>();
		Float j = function.Arg<9>();
		Return(Float(a) + b + Float(c) + d + Float(e) + f + Float(g) + h + Float(i) + j);
	}

	if(auto routine = function("one"))
	{
		float result = routine(1, 2.f, 3, 4.f, 5, 6.f, 7, 8.f, 9, 10.f);
		EXPECT_EQ(result, 55.f);
	}
}

// This test was written because on Windows with Subzero, we would get a crash when executing a function
// with a large number of local variables. The problem was that on Windows, 4K pages are allocated as
// needed for the stack whenever an access is made in a "guard page", at which point the page is committed,
// and the next 4K page becomes the guard page. If a stack access is made that's beyond the guard page,
// a regular page fault occurs. To fix this, Subzero (and any compiler) now emits a call to __chkstk with
// the stack size in EAX, so that it can probe the stack in 4K increments up to that size, committing the
// required pages. See https://docs.microsoft.com/en-us/windows/win32/devnotes/-win32-chkstk.
TEST(ReactorUnitTests, LargeStack)
{
#if defined(_WIN32)
	// An empirically large enough value to access outside the guard pages
	constexpr int ArrayByteSize = 24 * 1024;
	constexpr int ArraySize = ArrayByteSize / sizeof(int32_t);

	FunctionT<void(int32_t * v)> function;
	{
		// Allocate a stack array large enough that writing to the first element will reach beyond
		// the guard page.
		Array<Int, ArraySize> largeStackArray;
		for(int i = 0; i < ArraySize; ++i)
		{
			largeStackArray[i] = i;
		}

		Pointer<Int> in = function.Arg<0>();
		for(int i = 0; i < ArraySize; ++i)
		{
			in[i] = largeStackArray[i];
		}
	}

	auto routine = function("one");
	std::array<int32_t, ArraySize> v;

	// Run this in a thread, so that we get the default reserved stack size (8K on Win64).
	std::thread t([&] {
		routine(v.data());
	});
	t.join();

	for(int i = 0; i < ArraySize; ++i)
	{
		EXPECT_EQ(v[i], i);
	}
#endif
}

TEST(ReactorUnitTests, Call)
{
	struct Class
	{
		static int Callback(Class *p, int i, float f)
		{
			p->i = i;
			p->f = f;
			return i + int(f);
		}

		int i = 0;
		float f = 0.0f;
	};

	FunctionT<int(void *)> function;
	{
		Pointer<Byte> c = function.Arg<0>();
		auto res = Call(Class::Callback, c, 10, 20.0f);
		Return(res);
	}

	auto routine = function("one");

	Class c;
	int res = routine(&c);
	EXPECT_EQ(res, 30);
	EXPECT_EQ(c.i, 10);
	EXPECT_EQ(c.f, 20.0f);
}

TEST(ReactorUnitTests, CallMemberFunction)
{
	struct Class
	{
		int Callback(int argI, float argF)
		{
			i = argI;
			f = argF;
			return i + int(f);
		}

		int i = 0;
		float f = 0.0f;
	};

	Class c;

	FunctionT<int()> function;
	{
		auto res = Call(&Class::Callback, &c, 10, 20.0f);
		Return(res);
	}

	auto routine = function("one");

	int res = routine();
	EXPECT_EQ(res, 30);
	EXPECT_EQ(c.i, 10);
	EXPECT_EQ(c.f, 20.0f);
}

TEST(ReactorUnitTests, CallMemberFunctionIndirect)
{
	struct Class
	{
		int Callback(int argI, float argF)
		{
			i = argI;
			f = argF;
			return i + int(f);
		}

		int i = 0;
		float f = 0.0f;
	};

	FunctionT<int(void *)> function;
	{
		Pointer<Byte> c = function.Arg<0>();
		auto res = Call(&Class::Callback, c, 10, 20.0f);
		Return(res);
	}

	auto routine = function("one");

	Class c;
	int res = routine(&c);
	EXPECT_EQ(res, 30);
	EXPECT_EQ(c.i, 10);
	EXPECT_EQ(c.f, 20.0f);
}

TEST(ReactorUnitTests, CallImplicitCast)
{
	struct Class
	{
		static void Callback(Class *c, const char *s)
		{
			c->str = s;
		}
		std::string str;
	};

	FunctionT<void(Class * c, const char *s)> function;
	{
		Pointer<Byte> c = function.Arg<0>();
		Pointer<Byte> s = function.Arg<1>();
		Call(Class::Callback, c, s);
	}

	auto routine = function("one");

	Class c;
	routine(&c, "hello world");
	EXPECT_EQ(c.str, "hello world");
}

TEST(ReactorUnitTests, CallBoolReturnFunction)
{
	struct Class
	{
		static bool IsEven(int a)
		{
			return a % 2 == 0;
		}
	};

	FunctionT<int(int)> function;
	{
		Int a = function.Arg<0>();
		Bool res = Call(Class::IsEven, a);
		If(res)
		{
			Return(1);
		}
		Return(0);
	}

	auto routine = function("one");

	for(int i = 0; i < 10; ++i)
	{
		EXPECT_EQ(routine(i), i % 2 == 0);
	}
}

TEST(ReactorUnitTests, Call_Args4)
{
	struct Class
	{
		static int Func(int a, int b, int c, int d)
		{
			return a + b + c + d;
		}
	};

	{
		FunctionT<int()> function;
		{
			auto res = Call(Class::Func, 1, 2, 3, 4);
			Return(res);
		}

		auto routine = function("one");

		int res = routine();
		EXPECT_EQ(res, 1 + 2 + 3 + 4);
	}
}

TEST(ReactorUnitTests, Call_Args5)
{
	struct Class
	{
		static int Func(int a, int b, int c, int d, int e)
		{
			return a + b + c + d + e;
		}
	};

	{
		FunctionT<int()> function;
		{
			auto res = Call(Class::Func, 1, 2, 3, 4, 5);
			Return(res);
		}

		auto routine = function("one");

		int res = routine();
		EXPECT_EQ(res, 1 + 2 + 3 + 4 + 5);
	}
}

TEST(ReactorUnitTests, Call_ArgsMany)
{
	struct Class
	{
		static int Func(int a, int b, int c, int d, int e, int f, int g, int h)
		{
			return a + b + c + d + e + f + g + h;
		}
	};

	{
		FunctionT<int()> function;
		{
			auto res = Call(Class::Func, 1, 2, 3, 4, 5, 6, 7, 8);
			Return(res);
		}

		auto routine = function("one");

		int res = routine();
		EXPECT_EQ(res, 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);
	}
}

TEST(ReactorUnitTests, Call_ArgsMixed)
{
	struct Class
	{
		static int Func(int a, float b, int *c, float *d, int e, float f, int *g, float *h)
		{
			return a + b + *c + *d + e + f + *g + *h;
		}
	};

	{
		FunctionT<int()> function;
		{
			Int c(3);
			Float d(4);
			Int g(7);
			Float h(8);
			auto res = Call(Class::Func, 1, 2.f, &c, &d, 5, 6.f, &g, &h);
			Return(res);
		}

		auto routine = function("one");

		int res = routine();
		EXPECT_EQ(res, 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);
	}
}

TEST(ReactorUnitTests, Call_ArgsPointer)
{
	struct Class
	{
		static int Func(int *a)
		{
			return *a;
		}
	};

	{
		FunctionT<int()> function;
		{
			Int a(12345);
			auto res = Call(Class::Func, &a);
			Return(res);
		}

		auto routine = function("one");

		int res = routine();
		EXPECT_EQ(res, 12345);
	}
}

TEST(ReactorUnitTests, CallExternalCallRoutine)
{
	// routine1 calls Class::Func, passing it a pointer to routine2, and Class::Func calls routine2

	auto routine2 = [] {
		FunctionT<float(float, int)> function;
		{
			Float a = function.Arg<0>();
			Int b = function.Arg<1>();
			Return(a + Float(b));
		}
		return function("two");
	}();

	struct Class
	{
		static float Func(void *p, float a, int b)
		{
			auto funcToCall = reinterpret_cast<float (*)(float, int)>(p);
			return funcToCall(a, b);
		}
	};

	auto routine1 = [] {
		FunctionT<float(void *, float, int)> function;
		{
			Pointer<Byte> funcToCall = function.Arg<0>();
			Float a = function.Arg<1>();
			Int b = function.Arg<2>();
			Float result = Call(Class::Func, funcToCall, a, b);
			Return(result);
		}
		return function("one");
	}();

	float result = routine1((void *)routine2.getEntry(), 12.f, 13);
	EXPECT_EQ(result, 25.f);
}

// Check that a complex generated function which utilizes all 8 or 16 XMM
// registers computes the correct result.
// (Note that due to MSC's lack of support for inline assembly in x64,
// this test does not actually check that the register contents are
// preserved, just that the generated function computes the correct value.
// It's necessary to inspect the registers in a debugger to actually verify.)
TEST(ReactorUnitTests, PreserveXMMRegisters)
{
	FunctionT<void(void *, void *)> function;
	{
		Pointer<Byte> in = function.Arg<0>();
		Pointer<Byte> out = function.Arg<1>();

		Float4 a = *Pointer<Float4>(in + 16 * 0);
		Float4 b = *Pointer<Float4>(in + 16 * 1);
		Float4 c = *Pointer<Float4>(in + 16 * 2);
		Float4 d = *Pointer<Float4>(in + 16 * 3);
		Float4 e = *Pointer<Float4>(in + 16 * 4);
		Float4 f = *Pointer<Float4>(in + 16 * 5);
		Float4 g = *Pointer<Float4>(in + 16 * 6);
		Float4 h = *Pointer<Float4>(in + 16 * 7);
		Float4 i = *Pointer<Float4>(in + 16 * 8);
		Float4 j = *Pointer<Float4>(in + 16 * 9);
		Float4 k = *Pointer<Float4>(in + 16 * 10);
		Float4 l = *Pointer<Float4>(in + 16 * 11);
		Float4 m = *Pointer<Float4>(in + 16 * 12);
		Float4 n = *Pointer<Float4>(in + 16 * 13);
		Float4 o = *Pointer<Float4>(in + 16 * 14);
		Float4 p = *Pointer<Float4>(in + 16 * 15);

		Float4 ab = a + b;
		Float4 cd = c + d;
		Float4 ef = e + f;
		Float4 gh = g + h;
		Float4 ij = i + j;
		Float4 kl = k + l;
		Float4 mn = m + n;
		Float4 op = o + p;

		Float4 abcd = ab + cd;
		Float4 efgh = ef + gh;
		Float4 ijkl = ij + kl;
		Float4 mnop = mn + op;

		Float4 abcdefgh = abcd + efgh;
		Float4 ijklmnop = ijkl + mnop;
		Float4 sum = abcdefgh + ijklmnop;
		*Pointer<Float4>(out) = sum;
		Return();
	}

	auto routine = function("one");
	assert(routine);

	float input[64] = { 1.0f, 0.0f, 0.0f, 0.0f,
		                -1.0f, 1.0f, -1.0f, 0.0f,
		                1.0f, 2.0f, -2.0f, 0.0f,
		                -1.0f, 3.0f, -3.0f, 0.0f,
		                1.0f, 4.0f, -4.0f, 0.0f,
		                -1.0f, 5.0f, -5.0f, 0.0f,
		                1.0f, 6.0f, -6.0f, 0.0f,
		                -1.0f, 7.0f, -7.0f, 0.0f,
		                1.0f, 8.0f, -8.0f, 0.0f,
		                -1.0f, 9.0f, -9.0f, 0.0f,
		                1.0f, 10.0f, -10.0f, 0.0f,
		                -1.0f, 11.0f, -11.0f, 0.0f,
		                1.0f, 12.0f, -12.0f, 0.0f,
		                -1.0f, 13.0f, -13.0f, 0.0f,
		                1.0f, 14.0f, -14.0f, 0.0f,
		                -1.0f, 15.0f, -15.0f, 0.0f };

	float result[4];

	routine(input, result);

	EXPECT_EQ(result[0], 0.0f);
	EXPECT_EQ(result[1], 120.0f);
	EXPECT_EQ(result[2], -120.0f);
	EXPECT_EQ(result[3], 0.0f);
}

template<typename T>
class CToReactorTCastTest : public ::testing::Test
{
public:
	using CType = typename std::tuple_element<0, T>::type;
	using ReactorType = typename std::tuple_element<1, T>::type;
};

using CToReactorTCastTestTypes = ::testing::Types<  // Subset of types that can be used as arguments.
                                                    //	std::pair<bool,         Bool>,    FIXME(capn): Not supported as argument type by Subzero.
                                                    //	std::pair<uint8_t,      Byte>,    FIXME(capn): Not supported as argument type by Subzero.
                                                    //	std::pair<int8_t,       SByte>,   FIXME(capn): Not supported as argument type by Subzero.
                                                    //	std::pair<int16_t,      Short>,   FIXME(capn): Not supported as argument type by Subzero.
                                                    //	std::pair<uint16_t,     UShort>,  FIXME(capn): Not supported as argument type by Subzero.
    std::pair<int, Int>,
    std::pair<unsigned int, UInt>,
    std::pair<float, Float>>;

TYPED_TEST_SUITE(CToReactorTCastTest, CToReactorTCastTestTypes);

TYPED_TEST(CToReactorTCastTest, Casts)
{
	using CType = typename TestFixture::CType;
	using ReactorType = typename TestFixture::ReactorType;

	std::shared_ptr<Routine> routine;

	{
		Function<Int(ReactorType)> function;
		{
			ReactorType a = function.template Arg<0>();
			ReactorType b = CType{};
			RValue<ReactorType> c = RValue<ReactorType>(CType{});
			Bool same = (a == b) && (a == c);
			Return(IfThenElse(same, Int(1), Int(0)));  // TODO: Ability to use Bools as return values.
		}

		routine = function("one");

		auto callable = (int (*)(CType))routine->getEntry();
		CType in = {};
		EXPECT_EQ(callable(in), 1);
	}
}

template<typename T>
class GEPTest : public ::testing::Test
{
public:
	using CType = typename std::tuple_element<0, T>::type;
	using ReactorType = typename std::tuple_element<1, T>::type;
};

using GEPTestTypes = ::testing::Types<
    std::pair<bool, Bool>,
    std::pair<int8_t, Byte>,
    std::pair<int8_t, SByte>,
    std::pair<int8_t[4], Byte4>,
    std::pair<int8_t[4], SByte4>,
    std::pair<int8_t[8], Byte8>,
    std::pair<int8_t[8], SByte8>,
    std::pair<int8_t[16], Byte16>,
    std::pair<int8_t[16], SByte16>,
    std::pair<int16_t, Short>,
    std::pair<int16_t, UShort>,
    std::pair<int16_t[2], Short2>,
    std::pair<int16_t[2], UShort2>,
    std::pair<int16_t[4], Short4>,
    std::pair<int16_t[4], UShort4>,
    std::pair<int16_t[8], Short8>,
    std::pair<int16_t[8], UShort8>,
    std::pair<int, Int>,
    std::pair<int, UInt>,
    std::pair<int[2], Int2>,
    std::pair<int[2], UInt2>,
    std::pair<int[4], Int4>,
    std::pair<int[4], UInt4>,
    std::pair<int64_t, Long>,
    std::pair<int16_t, Half>,
    std::pair<float, Float>,
    std::pair<float[2], Float2>,
    std::pair<float[4], Float4>>;

TYPED_TEST_SUITE(GEPTest, GEPTestTypes);

TYPED_TEST(GEPTest, PtrOffsets)
{
	using CType = typename TestFixture::CType;
	using ReactorType = typename TestFixture::ReactorType;

	std::shared_ptr<Routine> routine;

	{
		Function<Pointer<ReactorType>(Pointer<ReactorType>, Int)> function;
		{
			Pointer<ReactorType> pointer = function.template Arg<0>();
			Int index = function.template Arg<1>();
			Return(&pointer[index]);
		}

		routine = function("one");

		auto callable = (CType * (*)(CType *, unsigned int)) routine->getEntry();

		union PtrInt
		{
			CType *p;
			size_t i;
		};

		PtrInt base;
		base.i = 0x10000;

		for(int i = 0; i < 5; i++)
		{
			PtrInt reference;
			reference.p = &base.p[i];

			PtrInt result;
			result.p = callable(base.p, i);

			auto expect = reference.i - base.i;
			auto got = result.i - base.i;

			EXPECT_EQ(got, expect) << "i:" << i;
		}
	}
}

TEST(ReactorUnitTests, Fibonacci)
{
	FunctionT<int(int)> function;
	{
		Int n = function.Arg<0>();
		Int current = 0;
		Int next = 1;
		For(Int i = 0, i < n, i++)
		{
			auto tmp = current + next;
			current = next;
			next = tmp;
		}
		Return(current);
	}

	auto routine = function("one");

	for(size_t i = 0; i < fibonacci.size(); i++)
	{
		EXPECT_EQ(routine(i), fibonacci[i]);
	}
}

TEST(ReactorUnitTests, Coroutines_Fibonacci)
{
	if(!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	Coroutine<int()> function;
	{
		Yield(Int(0));
		Yield(Int(1));
		Int current = 1;
		Int next = 1;
		While(true)
		{
			Yield(next);
			auto tmp = current + next;
			current = next;
			next = tmp;
		}
	}

	auto coroutine = function();

	for(size_t i = 0; i < fibonacci.size(); i++)
	{
		int out = 0;
		EXPECT_EQ(coroutine->await(out), true);
		EXPECT_EQ(out, fibonacci[i]);
	}
}

TEST(ReactorUnitTests, Coroutines_Parameters)
{
	if(!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	Coroutine<uint8_t(uint8_t * data, int count)> function;
	{
		Pointer<Byte> data = function.Arg<0>();
		Int count = function.Arg<1>();

		For(Int i = 0, i < count, i++)
		{
			Yield(data[i]);
		}
	}

	uint8_t data[] = { 10, 20, 30 };
	auto coroutine = function(&data[0], 3);

	uint8_t out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 10);
	out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 20);
	out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 30);
	out = 99;
	EXPECT_EQ(coroutine->await(out), false);
	EXPECT_EQ(out, 99);
	EXPECT_EQ(coroutine->await(out), false);
	EXPECT_EQ(out, 99);
}

// This test was written because Subzero's handling of vector types
// failed when more than one function is generated, as is the case
// with coroutines.
TEST(ReactorUnitTests, Coroutines_Vectors)
{
	if(!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	Coroutine<int()> function;
	{
		Int4 a{ 1, 2, 3, 4 };
		Yield(rr::Extract(a, 2));
		Int4 b{ 5, 6, 7, 8 };
		Yield(rr::Extract(b, 1));
		Int4 c{ 9, 10, 11, 12 };
		Yield(rr::Extract(c, 1));
	}

	auto coroutine = function();

	int out;
	coroutine->await(out);
	EXPECT_EQ(out, 3);
	coroutine->await(out);
	EXPECT_EQ(out, 6);
	coroutine->await(out);
	EXPECT_EQ(out, 10);
}

// This test was written to make sure a coroutine without a Yield()
// works correctly, by executing like a regular function with no
// return (the return type is ignored).
// We also run it twice to ensure per instance and/or global state
// is properly cleaned up in between.
TEST(ReactorUnitTests, Coroutines_NoYield)
{
	if(!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	for(int i = 0; i < 2; ++i)
	{
		Coroutine<int()> function;
		{
			Int a;
			a = 4;
		}

		auto coroutine = function();
		int out;
		EXPECT_EQ(coroutine->await(out), false);
	}
}

// Test generating one coroutine, and executing it on multiple threads. This makes
// sure the implementation manages per-call instance data correctly.
TEST(ReactorUnitTests, Coroutines_Parallel)
{
	if(!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	Coroutine<int()> function;
	{
		Yield(Int(0));
		Yield(Int(1));
		Int current = 1;
		Int next = 1;
		While(true)
		{
			Yield(next);
			auto tmp = current + next;
			current = next;
			next = tmp;
		}
	}

	// Must call on same thread that creates the coroutine
	function.finalize();

	std::vector<std::thread> threads;
	const size_t numThreads = 100;

	for(size_t t = 0; t < numThreads; ++t)
	{
		threads.emplace_back([&] {
			auto coroutine = function();

			for(size_t i = 0; i < fibonacci.size(); i++)
			{
				int out = 0;
				EXPECT_EQ(coroutine->await(out), true);
				EXPECT_EQ(out, fibonacci[i]);
			}
		});
	}

	for(auto &t : threads)
	{
		t.join();
	}
}

template<typename TestFuncType, typename RefFuncType, typename TestValueType>
struct IntrinsicTestParams
{
	std::function<TestFuncType> testFunc;   // Function we're testing (Reactor)
	std::function<RefFuncType> refFunc;     // Reference function to test against (C)
	std::vector<TestValueType> testValues;  // Values to input to functions
};

using IntrinsicTestParams_Float = IntrinsicTestParams<RValue<Float>(RValue<Float>), float(float), float>;
using IntrinsicTestParams_Float4 = IntrinsicTestParams<RValue<Float4>(RValue<Float4>), float(float), float>;
using IntrinsicTestParams_Float4_Float4 = IntrinsicTestParams<RValue<Float4>(RValue<Float4>, RValue<Float4>), float(float, float), std::pair<float, float>>;

struct IntrinsicTest_Float : public testing::TestWithParam<IntrinsicTestParams_Float>
{
	void test()
	{
		FunctionT<float(float)> function;
		{
			Return(GetParam().testFunc((Float(function.Arg<0>()))));
		}

		auto routine = function("one");

		for(auto &&v : GetParam().testValues)
		{
			SCOPED_TRACE(v);
			EXPECT_FLOAT_EQ(routine(v), GetParam().refFunc(v));
		}
	}
};

struct IntrinsicTest_Float4 : public testing::TestWithParam<IntrinsicTestParams_Float4>
{
	void test()
	{
		FunctionT<void(float4 *)> function;
		{
			Pointer<Float4> a = function.Arg<0>();
			*a = GetParam().testFunc(*a);
			Return();
		}

		auto routine = function("one");

		for(auto &&v : GetParam().testValues)
		{
			SCOPED_TRACE(v);
			float4_value result = invokeRoutine(routine, float4_value{ v });
			float4_value expected = float4_value{ GetParam().refFunc(v) };
			EXPECT_FLOAT_EQ(result.v[0], expected.v[0]);
			EXPECT_FLOAT_EQ(result.v[1], expected.v[1]);
			EXPECT_FLOAT_EQ(result.v[2], expected.v[2]);
			EXPECT_FLOAT_EQ(result.v[3], expected.v[3]);
		}
	}
};

struct IntrinsicTest_Float4_Float4 : public testing::TestWithParam<IntrinsicTestParams_Float4_Float4>
{
	void test()
	{
		FunctionT<void(float4 *, float4 *)> function;
		{
			Pointer<Float4> a = function.Arg<0>();
			Pointer<Float4> b = function.Arg<1>();
			*a = GetParam().testFunc(*a, *b);
			Return();
		}

		auto routine = function("one");

		for(auto &&v : GetParam().testValues)
		{
			SCOPED_TRACE(v);
			float4_value result = invokeRoutine(routine, float4_value{ v.first }, float4_value{ v.second });
			float4_value expected = float4_value{ GetParam().refFunc(v.first, v.second) };
			EXPECT_FLOAT_EQ(result.v[0], expected.v[0]);
			EXPECT_FLOAT_EQ(result.v[1], expected.v[1]);
			EXPECT_FLOAT_EQ(result.v[2], expected.v[2]);
			EXPECT_FLOAT_EQ(result.v[3], expected.v[3]);
		}
	}
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(IntrinsicTestParams_Float, IntrinsicTest_Float, testing::Values(
	IntrinsicTestParams_Float{ [](Float v) { return rr::Exp2(v); }, exp2f, {0.f, 1.f, 12345.f} },
	IntrinsicTestParams_Float{ [](Float v) { return rr::Log2(v); }, log2f, {0.f, 1.f, 12345.f} },
	IntrinsicTestParams_Float{ [](Float v) { return rr::Sqrt(v); }, sqrtf, {0.f, 1.f, 12345.f} }
));
// clang-format on

// TODO(b/149110874) Use coshf/sinhf when we've implemented SpirV versions at the SpirV level
float vulkan_sinhf(float a)
{
	return ((expf(a) - expf(-a)) / 2);
}
float vulkan_coshf(float a)
{
	return ((expf(a) + expf(-a)) / 2);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(IntrinsicTestParams_Float4, IntrinsicTest_Float4, testing::Values(
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Sin(v); },   sinf,   {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Cos(v); },   cosf,   {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Tan(v); },   tanf,   {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Asin(v); },  asinf,  {0.f, 1.f, -1.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Acos(v); },  acosf,  {0.f, 1.f, -1.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Atan(v); },  atanf,  {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Sinh(v); },  vulkan_sinhf,  {0.f, 1.f, PI, 12345.f, 0x1.65a84ep6}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Cosh(v); },  vulkan_coshf,  {0.f, 1.f, PI, 12345.f, 0x1.65a84ep6} },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Tanh(v); },  tanhf,  {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Asinh(v); }, asinhf, {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Acosh(v); }, acoshf, {     1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Atanh(v); }, atanhf, {0.f, 1.f, -1.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Exp(v); },   expf,   {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Log(v); },   logf,   {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Exp2(v); },  exp2f,  {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Log2(v); },  log2f,  {0.f, 1.f, PI, 12345.f}  },
	IntrinsicTestParams_Float4{ [](RValue<Float4> v) { return rr::Sqrt(v); },  sqrtf,  {0.f, 1.f, PI, 12345.f}  }
));
// clang-format on

// clang-format off
INSTANTIATE_TEST_SUITE_P(IntrinsicTestParams_Float4_Float4, IntrinsicTest_Float4_Float4, testing::Values(
	IntrinsicTestParams_Float4_Float4{ [](RValue<Float4> v1, RValue<Float4> v2) { return Atan2(v1, v2); }, atan2f, { {0.f, 0.f}, {0.f, -1.f}, {-1.f, 0.f}, {12345.f, 12345.f} } },
	IntrinsicTestParams_Float4_Float4{ [](RValue<Float4> v1, RValue<Float4> v2) { return Pow(v1, v2); },   powf,   { {0.f, 0.f}, {0.f, -1.f}, {-1.f, 0.f}, {12345.f, 12345.f} } }
));
// clang-format on

TEST_P(IntrinsicTest_Float, Test)
{
	test();
}
TEST_P(IntrinsicTest_Float4, Test)
{
	test();
}
TEST_P(IntrinsicTest_Float4_Float4, Test)
{
	test();
}

TEST(ReactorUnitTests, Intrinsics_Ctlz)
{
	// ctlz: counts number of leading zeros

	{
		Function<UInt(UInt x)> function;
		{
			UInt x = function.Arg<0>();
			Return(rr::Ctlz(x, false));
		}
		auto routine = function("one");
		auto callable = (uint32_t(*)(uint32_t))routine->getEntry();

		for(uint32_t i = 0; i < 31; ++i)
		{
			uint32_t result = callable(1 << i);
			EXPECT_EQ(result, 31 - i);
		}

		// Input 0 should return 32 for isZeroUndef == false
		{
			uint32_t result = callable(0);
			EXPECT_EQ(result, 32u);
		}
	}

	{
		Function<Void(Pointer<UInt4>, UInt x)> function;
		{
			Pointer<UInt4> out = function.Arg<0>();
			UInt x = function.Arg<1>();
			*out = rr::Ctlz(UInt4(x), false);
		}
		auto routine = function("one");
		auto callable = (void (*)(uint32_t *, uint32_t))routine->getEntry();

		uint32_t x[4];

		for(uint32_t i = 0; i < 31; ++i)
		{
			callable(x, 1 << i);
			EXPECT_EQ(x[0], 31 - i);
			EXPECT_EQ(x[1], 31 - i);
			EXPECT_EQ(x[2], 31 - i);
			EXPECT_EQ(x[3], 31 - i);
		}

		// Input 0 should return 32 for isZeroUndef == false
		{
			callable(x, 0);
			EXPECT_EQ(x[0], 32u);
			EXPECT_EQ(x[1], 32u);
			EXPECT_EQ(x[2], 32u);
			EXPECT_EQ(x[3], 32u);
		}
	}
}

TEST(ReactorUnitTests, Intrinsics_Cttz)
{
	// cttz: counts number of trailing zeros

	{
		Function<UInt(UInt x)> function;
		{
			UInt x = function.Arg<0>();
			Return(rr::Cttz(x, false));
		}
		auto routine = function("one");
		auto callable = (uint32_t(*)(uint32_t))routine->getEntry();

		for(uint32_t i = 0; i < 31; ++i)
		{
			uint32_t result = callable(1 << i);
			EXPECT_EQ(result, i);
		}

		// Input 0 should return 32 for isZeroUndef == false
		{
			uint32_t result = callable(0);
			EXPECT_EQ(result, 32u);
		}
	}

	{
		Function<Void(Pointer<UInt4>, UInt x)> function;
		{
			Pointer<UInt4> out = function.Arg<0>();
			UInt x = function.Arg<1>();
			*out = rr::Cttz(UInt4(x), false);
		}
		auto routine = function("one");
		auto callable = (void (*)(uint32_t *, uint32_t))routine->getEntry();

		uint32_t x[4];

		for(uint32_t i = 0; i < 31; ++i)
		{
			callable(x, 1 << i);
			EXPECT_EQ(x[0], i);
			EXPECT_EQ(x[1], i);
			EXPECT_EQ(x[2], i);
			EXPECT_EQ(x[3], i);
		}

		// Input 0 should return 32 for isZeroUndef == false
		{
			callable(x, 0);
			EXPECT_EQ(x[0], 32u);
			EXPECT_EQ(x[1], 32u);
			EXPECT_EQ(x[2], 32u);
			EXPECT_EQ(x[3], 32u);
		}
	}
}

TEST(ReactorUnitTests, Intrinsics_Scatter)
{
	Function<Void(Pointer<Float> base, Pointer<Float4> val, Pointer<Int4> offsets)> function;
	{
		Pointer<Float> base = function.Arg<0>();
		Pointer<Float4> val = function.Arg<1>();
		Pointer<Int4> offsets = function.Arg<2>();

		auto mask = Int4(~0, ~0, ~0, ~0);
		unsigned int alignment = 1;
		Scatter(base, *val, *offsets, mask, alignment);
	}

	float buffer[16] = { 0 };

	constexpr auto elemSize = sizeof(buffer[0]);

	int offsets[] = {
		1 * elemSize,
		6 * elemSize,
		11 * elemSize,
		13 * elemSize
	};

	float val[4] = { 10, 60, 110, 130 };

	auto routine = function("one");
	auto entry = (void (*)(float *, float *, int *))routine->getEntry();

	entry(buffer, val, offsets);

	EXPECT_EQ(buffer[offsets[0] / sizeof(buffer[0])], 10);
	EXPECT_EQ(buffer[offsets[1] / sizeof(buffer[0])], 60);
	EXPECT_EQ(buffer[offsets[2] / sizeof(buffer[0])], 110);
	EXPECT_EQ(buffer[offsets[3] / sizeof(buffer[0])], 130);
}

TEST(ReactorUnitTests, Intrinsics_Gather)
{
	Function<Void(Pointer<Float> base, Pointer<Int4> offsets, Pointer<Float4> result)> function;
	{
		Pointer<Float> base = function.Arg<0>();
		Pointer<Int4> offsets = function.Arg<1>();
		Pointer<Float4> result = function.Arg<2>();

		auto mask = Int4(~0, ~0, ~0, ~0);
		unsigned int alignment = 1;
		bool zeroMaskedLanes = true;
		*result = Gather(base, *offsets, mask, alignment, zeroMaskedLanes);
	}

	float buffer[] = {
		0, 10, 20, 30,
		40, 50, 60, 70,
		80, 90, 100, 110,
		120, 130, 140, 150
	};

	constexpr auto elemSize = sizeof(buffer[0]);

	int offsets[] = {
		1 * elemSize,
		6 * elemSize,
		11 * elemSize,
		13 * elemSize
	};

	auto routine = function("one");
	auto entry = (void (*)(float *, int *, float *))routine->getEntry();

	float result[4] = {};
	entry(buffer, offsets, result);

	EXPECT_EQ(result[0], 10);
	EXPECT_EQ(result[1], 60);
	EXPECT_EQ(result[2], 110);
	EXPECT_EQ(result[3], 130);
}

TEST(ReactorUnitTests, ExtractFromRValue)
{
	Function<Void(Pointer<Int4> values, Pointer<Int4> result)> function;
	{
		Pointer<Int4> vIn = function.Arg<0>();
		Pointer<Int4> resultIn = function.Arg<1>();

		RValue<Int4> v = *vIn;

		Int4 result(678);

		If(Extract(v, 0) == 42)
		{
			result = Insert(result, 1, 0);
		}

		If(Extract(v, 1) == 42)
		{
			result = Insert(result, 1, 1);
		}

		*resultIn = result;

		Return();
	}

	auto routine = function("one");
	auto entry = (void (*)(int *, int *))routine->getEntry();

	int v[4] = { 42, 42, 42, 42 };
	int result[4] = { 99, 99, 99, 99 };
	entry(v, result);
	EXPECT_EQ(result[0], 1);
	EXPECT_EQ(result[1], 1);
	EXPECT_EQ(result[2], 678);
	EXPECT_EQ(result[3], 678);
}

TEST(ReactorUnitTests, AddAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::AddAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 123;
	uint32_t y = 456;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 123u);
	EXPECT_EQ(x, 579u);
}

TEST(ReactorUnitTests, SubAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::SubAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 456;
	uint32_t y = 123;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 456u);
	EXPECT_EQ(x, 333u);
}

TEST(ReactorUnitTests, AndAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::AndAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 0b1111'0000;
	uint32_t y = 0b1010'1100;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 0b1111'0000u);
	EXPECT_EQ(x, 0b1010'0000u);
}

TEST(ReactorUnitTests, OrAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::OrAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 0b1111'0000;
	uint32_t y = 0b1010'1100;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 0b1111'0000u);
	EXPECT_EQ(x, 0b1111'1100u);
}

TEST(ReactorUnitTests, XorAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::XorAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 0b1111'0000;
	uint32_t y = 0b1010'1100;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 0b1111'0000u);
	EXPECT_EQ(x, 0b0101'1100u);
}

TEST(ReactorUnitTests, MinAtomic)
{
	{
		FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
		{
			Pointer<UInt> p = function.Arg<0>();
			UInt a = function.Arg<1>();
			UInt r = rr::MinAtomic(p, a, std::memory_order_relaxed);
			Return(r);
		}

		auto routine = function("one");
		uint32_t x = 123;
		uint32_t y = 100;
		uint32_t prevX = routine(&x, y);
		EXPECT_EQ(prevX, 123u);
		EXPECT_EQ(x, 100u);
	}

	{
		FunctionT<int32_t(int32_t * p, int32_t a)> function;
		{
			Pointer<Int> p = function.Arg<0>();
			Int a = function.Arg<1>();
			Int r = rr::MinAtomic(p, a, std::memory_order_relaxed);
			Return(r);
		}

		auto routine = function("one");
		int32_t x = -123;
		int32_t y = -200;
		int32_t prevX = routine(&x, y);
		EXPECT_EQ(prevX, -123);
		EXPECT_EQ(x, -200);
	}
}

TEST(ReactorUnitTests, MaxAtomic)
{
	{
		FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
		{
			Pointer<UInt> p = function.Arg<0>();
			UInt a = function.Arg<1>();
			UInt r = rr::MaxAtomic(p, a, std::memory_order_relaxed);
			Return(r);
		}

		auto routine = function("one");
		uint32_t x = 123;
		uint32_t y = 100;
		uint32_t prevX = routine(&x, y);
		EXPECT_EQ(prevX, 123u);
		EXPECT_EQ(x, 123u);
	}

	{
		FunctionT<int32_t(int32_t * p, int32_t a)> function;
		{
			Pointer<Int> p = function.Arg<0>();
			Int a = function.Arg<1>();
			Int r = rr::MaxAtomic(p, a, std::memory_order_relaxed);
			Return(r);
		}

		auto routine = function("one");
		int32_t x = -123;
		int32_t y = -200;
		int32_t prevX = routine(&x, y);
		EXPECT_EQ(prevX, -123);
		EXPECT_EQ(x, -123);
	}
}

TEST(ReactorUnitTests, ExchangeAtomic)
{
	FunctionT<uint32_t(uint32_t * p, uint32_t a)> function;
	{
		Pointer<UInt> p = function.Arg<0>();
		UInt a = function.Arg<1>();
		UInt r = rr::ExchangeAtomic(p, a, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 123;
	uint32_t y = 456;
	uint32_t prevX = routine(&x, y);
	EXPECT_EQ(prevX, 123u);
	EXPECT_EQ(x, y);
}

TEST(ReactorUnitTests, CompareExchangeAtomic)
{
	FunctionT<uint32_t(uint32_t * x, uint32_t y, uint32_t compare)> function;
	{
		Pointer<UInt> x = function.Arg<0>();
		UInt y = function.Arg<1>();
		UInt compare = function.Arg<2>();
		UInt r = rr::CompareExchangeAtomic(x, y, compare, std::memory_order_relaxed, std::memory_order_relaxed);
		Return(r);
	}

	auto routine = function("one");
	uint32_t x = 123;
	uint32_t y = 456;
	uint32_t compare = 123;
	uint32_t prevX = routine(&x, y, compare);
	EXPECT_EQ(prevX, 123u);
	EXPECT_EQ(x, y);

	x = 123;
	y = 456;
	compare = 456;
	prevX = routine(&x, y, compare);
	EXPECT_EQ(prevX, 123u);
	EXPECT_EQ(x, 123u);
}

TEST(ReactorUnitTests, SRem)
{
	FunctionT<void(int4 *, int4 *)> function;
	{
		Pointer<Int4> a = function.Arg<0>();
		Pointer<Int4> b = function.Arg<1>();
		*a = *a % *b;
	}

	auto routine = function("one");

	int4_value result = invokeRoutine(routine, int4_value{ 10, 11, 12, 13 }, int4_value{ 3, 3, 3, 3 });
	int4_value expected = int4_value{ 10 % 3, 11 % 3, 12 % 3, 13 % 3 };
	EXPECT_FLOAT_EQ(result.v[0], expected.v[0]);
	EXPECT_FLOAT_EQ(result.v[1], expected.v[1]);
	EXPECT_FLOAT_EQ(result.v[2], expected.v[2]);
	EXPECT_FLOAT_EQ(result.v[3], expected.v[3]);
}

TEST(ReactorUnitTests, FRem)
{
	FunctionT<void(float4 *, float4 *)> function;
	{
		Pointer<Float4> a = function.Arg<0>();
		Pointer<Float4> b = function.Arg<1>();
		*a = *a % *b;
	}

	auto routine = function("one");

	float4_value result = invokeRoutine(routine, float4_value{ 10.1f, 11.2f, 12.3f, 13.4f }, float4_value{ 3.f, 3.f, 3.f, 3.f });
	float4_value expected = float4_value{ fmodf(10.1f, 3.f), fmodf(11.2f, 3.f), fmodf(12.3f, 3.f), fmodf(13.4f, 3.f) };
	EXPECT_FLOAT_EQ(result.v[0], expected.v[0]);
	EXPECT_FLOAT_EQ(result.v[1], expected.v[1]);
	EXPECT_FLOAT_EQ(result.v[2], expected.v[2]);
	EXPECT_FLOAT_EQ(result.v[3], expected.v[3]);
}

// Subzero's load instruction assumes that a Constant ptr value is an offset, rather than an absolute
// pointer, and would fail during codegen. This was fixed by casting the constant to a non-const
// variable, and loading from it instead. This test makes sure this works.
TEST(ReactorUnitTests, LoadFromConstantData)
{
	const int value = 123;

	FunctionT<int()> function;
	{
		auto p = Pointer<Int>{ ConstantData(&value, sizeof(value)) };
		Int v = *p;
		Return(v);
	}

	const int result = function("one")();
	EXPECT_EQ(result, value);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

////////////////////////////////
// Trait compile time checks. //
////////////////////////////////

// Assert CToReactorT resolves to expected types.
static_assert(std::is_same<CToReactorT<void>, Void>::value, "");
static_assert(std::is_same<CToReactorT<bool>, Bool>::value, "");
static_assert(std::is_same<CToReactorT<uint8_t>, Byte>::value, "");
static_assert(std::is_same<CToReactorT<int8_t>, SByte>::value, "");
static_assert(std::is_same<CToReactorT<int16_t>, Short>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t>, UShort>::value, "");
static_assert(std::is_same<CToReactorT<int32_t>, Int>::value, "");
static_assert(std::is_same<CToReactorT<uint64_t>, Long>::value, "");
static_assert(std::is_same<CToReactorT<uint32_t>, UInt>::value, "");
static_assert(std::is_same<CToReactorT<float>, Float>::value, "");

// Assert CToReactorT for known pointer types resolves to expected types.
static_assert(std::is_same<CToReactorT<void *>, Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<bool *>, Pointer<Bool>>::value, "");
static_assert(std::is_same<CToReactorT<uint8_t *>, Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<int8_t *>, Pointer<SByte>>::value, "");
static_assert(std::is_same<CToReactorT<int16_t *>, Pointer<Short>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t *>, Pointer<UShort>>::value, "");
static_assert(std::is_same<CToReactorT<int32_t *>, Pointer<Int>>::value, "");
static_assert(std::is_same<CToReactorT<uint64_t *>, Pointer<Long>>::value, "");
static_assert(std::is_same<CToReactorT<uint32_t *>, Pointer<UInt>>::value, "");
static_assert(std::is_same<CToReactorT<float *>, Pointer<Float>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t **>, Pointer<Pointer<UShort>>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t ***>, Pointer<Pointer<Pointer<UShort>>>>::value, "");

// Assert CToReactorT for unknown pointer types resolves to Pointer<Byte>.
struct S
{};
static_assert(std::is_same<CToReactorT<S *>, Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<S **>, Pointer<Pointer<Byte>>>::value, "");
static_assert(std::is_same<CToReactorT<S ***>, Pointer<Pointer<Pointer<Byte>>>>::value, "");

// Assert IsRValue<> resolves true for RValue<> types.
static_assert(IsRValue<RValue<Void>>::value, "");
static_assert(IsRValue<RValue<Bool>>::value, "");
static_assert(IsRValue<RValue<Byte>>::value, "");
static_assert(IsRValue<RValue<SByte>>::value, "");
static_assert(IsRValue<RValue<Short>>::value, "");
static_assert(IsRValue<RValue<UShort>>::value, "");
static_assert(IsRValue<RValue<Int>>::value, "");
static_assert(IsRValue<RValue<Long>>::value, "");
static_assert(IsRValue<RValue<UInt>>::value, "");
static_assert(IsRValue<RValue<Float>>::value, "");

// Assert IsLValue<> resolves true for LValue types.
static_assert(IsLValue<Bool>::value, "");
static_assert(IsLValue<Byte>::value, "");
static_assert(IsLValue<SByte>::value, "");
static_assert(IsLValue<Short>::value, "");
static_assert(IsLValue<UShort>::value, "");
static_assert(IsLValue<Int>::value, "");
static_assert(IsLValue<Long>::value, "");
static_assert(IsLValue<UInt>::value, "");
static_assert(IsLValue<Float>::value, "");

// Assert IsReference<> resolves true for Reference types.
static_assert(IsReference<Reference<Bool>>::value, "");
static_assert(IsReference<Reference<Byte>>::value, "");
static_assert(IsReference<Reference<SByte>>::value, "");
static_assert(IsReference<Reference<Short>>::value, "");
static_assert(IsReference<Reference<UShort>>::value, "");
static_assert(IsReference<Reference<Int>>::value, "");
static_assert(IsReference<Reference<Long>>::value, "");
static_assert(IsReference<Reference<UInt>>::value, "");
static_assert(IsReference<Reference<Float>>::value, "");

// Assert IsRValue<> resolves false for LValue types.
static_assert(!IsRValue<Void>::value, "");
static_assert(!IsRValue<Bool>::value, "");
static_assert(!IsRValue<Byte>::value, "");
static_assert(!IsRValue<SByte>::value, "");
static_assert(!IsRValue<Short>::value, "");
static_assert(!IsRValue<UShort>::value, "");
static_assert(!IsRValue<Int>::value, "");
static_assert(!IsRValue<Long>::value, "");
static_assert(!IsRValue<UInt>::value, "");
static_assert(!IsRValue<Float>::value, "");

// Assert IsRValue<> resolves false for Reference types.
static_assert(!IsRValue<Reference<Void>>::value, "");
static_assert(!IsRValue<Reference<Bool>>::value, "");
static_assert(!IsRValue<Reference<Byte>>::value, "");
static_assert(!IsRValue<Reference<SByte>>::value, "");
static_assert(!IsRValue<Reference<Short>>::value, "");
static_assert(!IsRValue<Reference<UShort>>::value, "");
static_assert(!IsRValue<Reference<Int>>::value, "");
static_assert(!IsRValue<Reference<Long>>::value, "");
static_assert(!IsRValue<Reference<UInt>>::value, "");
static_assert(!IsRValue<Reference<Float>>::value, "");

// Assert IsRValue<> resolves false for C types.
static_assert(!IsRValue<void>::value, "");
static_assert(!IsRValue<bool>::value, "");
static_assert(!IsRValue<uint8_t>::value, "");
static_assert(!IsRValue<int8_t>::value, "");
static_assert(!IsRValue<int16_t>::value, "");
static_assert(!IsRValue<uint16_t>::value, "");
static_assert(!IsRValue<int32_t>::value, "");
static_assert(!IsRValue<uint64_t>::value, "");
static_assert(!IsRValue<uint32_t>::value, "");
static_assert(!IsRValue<float>::value, "");

// Assert IsLValue<> resolves false for RValue<> types.
static_assert(!IsLValue<RValue<Void>>::value, "");
static_assert(!IsLValue<RValue<Bool>>::value, "");
static_assert(!IsLValue<RValue<Byte>>::value, "");
static_assert(!IsLValue<RValue<SByte>>::value, "");
static_assert(!IsLValue<RValue<Short>>::value, "");
static_assert(!IsLValue<RValue<UShort>>::value, "");
static_assert(!IsLValue<RValue<Int>>::value, "");
static_assert(!IsLValue<RValue<Long>>::value, "");
static_assert(!IsLValue<RValue<UInt>>::value, "");
static_assert(!IsLValue<RValue<Float>>::value, "");

// Assert IsLValue<> resolves false for Void type.
static_assert(!IsLValue<Void>::value, "");

// Assert IsLValue<> resolves false for Reference<> types.
static_assert(!IsLValue<Reference<Void>>::value, "");
static_assert(!IsLValue<Reference<Bool>>::value, "");
static_assert(!IsLValue<Reference<Byte>>::value, "");
static_assert(!IsLValue<Reference<SByte>>::value, "");
static_assert(!IsLValue<Reference<Short>>::value, "");
static_assert(!IsLValue<Reference<UShort>>::value, "");
static_assert(!IsLValue<Reference<Int>>::value, "");
static_assert(!IsLValue<Reference<Long>>::value, "");
static_assert(!IsLValue<Reference<UInt>>::value, "");
static_assert(!IsLValue<Reference<Float>>::value, "");

// Assert IsLValue<> resolves false for C types.
static_assert(!IsLValue<void>::value, "");
static_assert(!IsLValue<bool>::value, "");
static_assert(!IsLValue<uint8_t>::value, "");
static_assert(!IsLValue<int8_t>::value, "");
static_assert(!IsLValue<int16_t>::value, "");
static_assert(!IsLValue<uint16_t>::value, "");
static_assert(!IsLValue<int32_t>::value, "");
static_assert(!IsLValue<uint64_t>::value, "");
static_assert(!IsLValue<uint32_t>::value, "");
static_assert(!IsLValue<float>::value, "");

// Assert IsDefined<> resolves true for RValue<> types.
static_assert(IsDefined<RValue<Void>>::value, "");
static_assert(IsDefined<RValue<Bool>>::value, "");
static_assert(IsDefined<RValue<Byte>>::value, "");
static_assert(IsDefined<RValue<SByte>>::value, "");
static_assert(IsDefined<RValue<Short>>::value, "");
static_assert(IsDefined<RValue<UShort>>::value, "");
static_assert(IsDefined<RValue<Int>>::value, "");
static_assert(IsDefined<RValue<Long>>::value, "");
static_assert(IsDefined<RValue<UInt>>::value, "");
static_assert(IsDefined<RValue<Float>>::value, "");

// Assert IsDefined<> resolves true for LValue types.
static_assert(IsDefined<Void>::value, "");
static_assert(IsDefined<Bool>::value, "");
static_assert(IsDefined<Byte>::value, "");
static_assert(IsDefined<SByte>::value, "");
static_assert(IsDefined<Short>::value, "");
static_assert(IsDefined<UShort>::value, "");
static_assert(IsDefined<Int>::value, "");
static_assert(IsDefined<Long>::value, "");
static_assert(IsDefined<UInt>::value, "");
static_assert(IsDefined<Float>::value, "");

// Assert IsDefined<> resolves true for Reference<> types.
static_assert(IsDefined<Reference<Bool>>::value, "");
static_assert(IsDefined<Reference<Byte>>::value, "");
static_assert(IsDefined<Reference<SByte>>::value, "");
static_assert(IsDefined<Reference<Short>>::value, "");
static_assert(IsDefined<Reference<UShort>>::value, "");
static_assert(IsDefined<Reference<Int>>::value, "");
static_assert(IsDefined<Reference<Long>>::value, "");
static_assert(IsDefined<Reference<UInt>>::value, "");
static_assert(IsDefined<Reference<Float>>::value, "");

// Assert IsDefined<> resolves true for C types.
static_assert(IsDefined<void>::value, "");
static_assert(IsDefined<bool>::value, "");
static_assert(IsDefined<uint8_t>::value, "");
static_assert(IsDefined<int8_t>::value, "");
static_assert(IsDefined<int16_t>::value, "");
static_assert(IsDefined<uint16_t>::value, "");
static_assert(IsDefined<int32_t>::value, "");
static_assert(IsDefined<uint64_t>::value, "");
static_assert(IsDefined<uint32_t>::value, "");
static_assert(IsDefined<float>::value, "");
