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

#include "Reactor.hpp"
#include "Coroutine.hpp"

#include "gtest/gtest.h"

#include <tuple>

using namespace rr;

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

TEST(ReactorUnitTests, Sample)
{
	{
		FunctionT<int(int*, int)> function;
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

		if(routine)
		{
			int one[2] = {1, 0};
			int result = routine(&one[1], 2);
			EXPECT_EQ(result, reference(&one[1], 2));
		}
	}

}

TEST(ReactorUnitTests, Uninitialized)
{
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

		if(routine)
		{
			int result = routine();
			EXPECT_EQ(result, result);   // Anything is fine, just don't crash
		}
	}

}

TEST(ReactorUnitTests, Unreachable)
{
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

		if(routine)
		{
			int result = routine(16);
			EXPECT_EQ(result, 20);
		}
	}

}

TEST(ReactorUnitTests, VariableAddress)
{
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

		if(routine)
		{
			int result = routine(16);
			EXPECT_EQ(result, 20);
		}
	}

}

TEST(ReactorUnitTests, SubVectorLoadStore)
{
	{
		FunctionT<int(void*, void*)> function;
		{
			Pointer<Byte> in = function.Arg<0>();
			Pointer<Byte> out = function.Arg<1>();

			*Pointer<Int4>(out + 16 * 0)   = *Pointer<Int4>(in + 16 * 0);
			*Pointer<Short4>(out + 16 * 1) = *Pointer<Short4>(in + 16 * 1);
			*Pointer<Byte8>(out + 16 * 2)  = *Pointer<Byte8>(in + 16 * 2);
			*Pointer<Byte4>(out + 16 * 3)  = *Pointer<Byte4>(in + 16 * 3);
			*Pointer<Short2>(out + 16 * 4) = *Pointer<Short2>(in + 16 * 4);

			Return(0);
		}

		auto routine = function("one");

		if(routine)
		{
			int8_t in[16 * 5] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
			                     17, 18, 19, 20, 21, 22, 23, 24,  0,  0,  0,  0,  0,  0,  0,  0,
			                     25, 26, 27, 28, 29, 30, 31, 32,  0,  0,  0,  0,  0,  0,  0,  0,
			                     33, 34, 35, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			                     37, 38, 39, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

			int8_t out[16 * 5] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

			routine(in, out);

			for(int row = 0; row < 5; row++)
			{
				for(int col = 0; col < 16; col++)
				{
					int i = row * 16 + col;

					if(in[i] ==  0)
					{
						EXPECT_EQ(out[i], -1) << "Row " << row << " column " << col <<  " not left untouched.";
					}
					else
					{
						EXPECT_EQ(out[i], in[i]) << "Row " << row << " column " << col << " not equal to input.";
					}
				}
			}
		}
	}

}

TEST(ReactorUnitTests, VectorConstant)
{
	{
		FunctionT<int(void*)> function;
		{
			Pointer<Byte> out = function.Arg<0>();

			*Pointer<Int4>(out + 16 * 0) = Int4(0x04030201, 0x08070605, 0x0C0B0A09, 0x100F0E0D);
			*Pointer<Short4>(out + 16 * 1) = Short4(0x1211, 0x1413, 0x1615, 0x1817);
			*Pointer<Byte8>(out + 16 * 2) = Byte8(0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20);
			*Pointer<Int2>(out + 16 * 3) = Int2(0x24232221, 0x28272625);

			Return(0);
		}

		auto routine = function("one");

		if(routine)
		{
			int8_t out[16 * 4] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

			int8_t exp[16 * 4] = {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
			                      17, 18, 19, 20, 21, 22, 23, 24, -1, -1, -1, -1, -1, -1, -1, -1,
			                      25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1, -1, -1, -1,
			                      33, 34, 35, 36, 37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1};

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
	}

}

TEST(ReactorUnitTests, Concatenate)
{
	{
		FunctionT<int(void*)> function;
		{
			Pointer<Byte> out = function.Arg<0>();

			*Pointer<Int4>(out + 16 * 0)   = Int4(Int2(0x04030201, 0x08070605), Int2(0x0C0B0A09, 0x100F0E0D));
			*Pointer<Short8>(out + 16 * 1) = Short8(Short4(0x0201, 0x0403, 0x0605, 0x0807), Short4(0x0A09, 0x0C0B, 0x0E0D, 0x100F));

			Return(0);
		}

		auto routine = function("one");

		if(routine)
		{
			int8_t ref[16 * 5] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
			                      1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16};

			int8_t out[16 * 5] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

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
	}

}

TEST(ReactorUnitTests, Swizzle)
{
	{
		FunctionT<int(void*)> function;
		{
			Pointer<Byte> out = function.Arg<0>();

			for(int i = 0; i < 256; i++)
			{
				*Pointer<Float4>(out + 16 * i) = Swizzle(Float4(1.0f, 2.0f, 3.0f, 4.0f), i);
			}

			for(int i = 0; i < 256; i++)
			{
				*Pointer<Float4>(out + 16 * (256 + i)) = ShuffleLowHigh(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f), i);
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
                                    Swizzle(Short4(1, 2, 3, 4), i);
			}

			for(int i = 0; i < 256; i++)
			{
				*Pointer<Int4>(out + 16 * (512 + 6 + i) + (8 * 256)) =
                                    Swizzle(Int4(1, 2, 3, 4), i);
			}

			Return(0);
		}

		auto routine = function("one");

		if(routine)
		{
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
				EXPECT_EQ(out.i[4 + i/2][0 + (i%2) * 2] & 0xFFFF,
                                          ((i >> 0) & 0x03) + 1);
				EXPECT_EQ(out.i[4 + i/2][0 + (i%2) * 2] >> 16,
                                          ((i >> 2) & 0x03) + 1);
				EXPECT_EQ(out.i[4 + i/2][1 + (i%2) * 2] & 0xFFFF,
                                          ((i >> 4) & 0x03) + 1);
				EXPECT_EQ(out.i[4 + i/2][1 + (i%2) * 2] >> 16,
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
	}

}

TEST(ReactorUnitTests, Branching)
{
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

			For(Int i = 0, i < 10, i++)
				for(int i = 0; i < 10; i++)
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

		if(routine)
		{
			int result = routine();

			EXPECT_EQ(result, 1000402222);
		}
	}

}

TEST(ReactorUnitTests, MinMax)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, NotNeg)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, VectorCompare)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, SaturatedAddAndSubtract)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, Unpack)
{
	{
		FunctionT<int(void*, void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, Pack)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, MulHigh)
{
	{
		FunctionT<int(void*)> function;
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

		if(routine)
		{
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
	}

}

TEST(ReactorUnitTests, MulAdd)
{
	{
		FunctionT<int(void*)> function;
		{
			Pointer<Byte> out = function.Arg<0>();

			*Pointer<Int2>(out + 8 * 0) =
				MulAdd(Short4(0x1aa, 0x2dd, 0x3ee, 0xF422),
				       Short4(0x1bb, 0x2cc, 0x3ff, 0xF411));

			// (U)Short8 variant is mentioned but unimplemented
			Return(0);
		}

		auto routine = function("one");

		if(routine)
		{
			unsigned int out[1][2];

			memset(&out, 0, sizeof(out));

			routine(&out);

			EXPECT_EQ(out[0][0], 0x000AE34Au);
			EXPECT_EQ(out[0][1], 0x009D5254u);
		}
	}

}

TEST(ReactorUnitTests, PointersEqual)
{
	FunctionT<int(void*, void*)> function;
	{
		Pointer<Byte> ptrA = function.Arg<0>();
		Pointer<Byte> ptrB = function.Arg<1>();
		If (ptrA == ptrB)
		{
			Return(1);
		}
		Else
		{
			Return(0);
		}
	}

	auto routine = function("one");
	int* a = reinterpret_cast<int*>(uintptr_t(0x0000000000000000));
	int* b = reinterpret_cast<int*>(uintptr_t(0x00000000F0000000));
	int* c = reinterpret_cast<int*>(uintptr_t(0xF000000000000000));
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

	if (auto routine = function("one"))
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

	if (auto routine = function("one"))
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

	if (auto routine = function("one"))
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

	if (auto routine = function("one"))
	{
		float result = routine(1, 2.f, 3, 4.f, 5, 6.f, 7, 8.f, 9, 10.f);
		EXPECT_EQ(result, 55.f);
	}
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

	FunctionT<int(void*)> function;
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

	FunctionT<int(void*)> function;
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
		static void Callback(Class *c, const char* s)
		{
			c->str = s;
		}
		std::string str;
	};

	FunctionT<void(Class *c, const char *s)> function;
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
		static float Func(void* p, float a, int b)
		{
			auto funcToCall = reinterpret_cast<float(*)(float, int)>(p);
			return funcToCall(a, b);
		}
	};

	auto routine1 = [] {
		FunctionT<float(void*, float, int)> function;
		{
			Pointer<Byte> funcToCall = function.Arg<0>();
			Float a = function.Arg<1>();
			Int b = function.Arg<2>();
			Float result = Call(Class::Func, funcToCall, a, b);
			Return(result);
		}
		return function("one");
	}();

	float result = routine1((void*)routine2.getEntry(), 12.f, 13);
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
    {
        FunctionT<void(void*, void*)> function;
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

        float input[64] = { 1.0f,  0.0f,   0.0f, 0.0f,
                           -1.0f,  1.0f,  -1.0f, 0.0f,
                            1.0f,  2.0f,  -2.0f, 0.0f,
                           -1.0f,  3.0f,  -3.0f, 0.0f,
                            1.0f,  4.0f,  -4.0f, 0.0f,
                           -1.0f,  5.0f,  -5.0f, 0.0f,
                            1.0f,  6.0f,  -6.0f, 0.0f,
                           -1.0f,  7.0f,  -7.0f, 0.0f,
                            1.0f,  8.0f,  -8.0f, 0.0f,
                           -1.0f,  9.0f,  -9.0f, 0.0f,
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

}

template <typename T>
class CToReactorTCastTest : public ::testing::Test
{
public:
	using CType = typename std::tuple_element<0, T>::type;
	using ReactorType = typename std::tuple_element<1, T>::type;
};

using CToReactorTCastTestTypes = ::testing::Types
	< // Subset of types that can be used as arguments.
	//	std::pair<bool,         Bool>,    FIXME(capn): Not supported as argument type by Subzero.
	//	std::pair<uint8_t,      Byte>,    FIXME(capn): Not supported as argument type by Subzero.
	//	std::pair<int8_t,       SByte>,   FIXME(capn): Not supported as argument type by Subzero.
	//	std::pair<int16_t,      Short>,   FIXME(capn): Not supported as argument type by Subzero.
	//	std::pair<uint16_t,     UShort>,  FIXME(capn): Not supported as argument type by Subzero.
		std::pair<int,          Int>,
		std::pair<unsigned int, UInt>,
		std::pair<float,        Float>
	>;

TYPED_TEST_SUITE(CToReactorTCastTest, CToReactorTCastTestTypes);

TYPED_TEST(CToReactorTCastTest, Casts)
{
	using CType = typename TestFixture::CType;
	using ReactorType = typename TestFixture::ReactorType;

	std::shared_ptr<Routine> routine;

	{
		Function< Int(ReactorType) > function;
		{
			ReactorType a = function.template Arg<0>();
			ReactorType b = CType{};
			RValue<ReactorType> c = RValue<ReactorType>(CType{});
			Bool same = (a == b) && (a == c);
			Return(IfThenElse(same, Int(1), Int(0))); // TODO: Ability to use Bools as return values.
		}

		routine = function("one");

		if(routine)
		{
			auto callable = (int(*)(CType))routine->getEntry();
			CType in = {};
			EXPECT_EQ(callable(in), 1);
		}
	}

}

template <typename T>
class GEPTest : public ::testing::Test
{
public:
	using CType = typename std::tuple_element<0, T>::type;
	using ReactorType = typename std::tuple_element<1, T>::type;
};

using GEPTestTypes = ::testing::Types
	<
		std::pair<bool,        Bool>,
		std::pair<int8_t,      Byte>,
		std::pair<int8_t,      SByte>,
		std::pair<int8_t[4],   Byte4>,
		std::pair<int8_t[4],   SByte4>,
		std::pair<int8_t[8],   Byte8>,
		std::pair<int8_t[8],   SByte8>,
		std::pair<int8_t[16],  Byte16>,
		std::pair<int8_t[16],  SByte16>,
		std::pair<int16_t,     Short>,
		std::pair<int16_t,     UShort>,
		std::pair<int16_t[2],  Short2>,
		std::pair<int16_t[2],  UShort2>,
		std::pair<int16_t[4],  Short4>,
		std::pair<int16_t[4],  UShort4>,
		std::pair<int16_t[8],  Short8>,
		std::pair<int16_t[8],  UShort8>,
		std::pair<int,         Int>,
		std::pair<int,         UInt>,
		std::pair<int[2],      Int2>,
		std::pair<int[2],      UInt2>,
		std::pair<int[4],      Int4>,
		std::pair<int[4],      UInt4>,
		std::pair<int64_t,     Long>,
		std::pair<int16_t,     Half>,
		std::pair<float,       Float>,
		std::pair<float[2],    Float2>,
		std::pair<float[4],    Float4>
	>;

TYPED_TEST_SUITE(GEPTest, GEPTestTypes);

TYPED_TEST(GEPTest, PtrOffsets)
{
	using CType = typename TestFixture::CType;
	using ReactorType = typename TestFixture::ReactorType;

	std::shared_ptr<Routine> routine;

	{
		Function< Pointer<ReactorType>(Pointer<ReactorType>, Int) > function;
		{
			Pointer<ReactorType> pointer = function.template Arg<0>();
			Int index = function.template Arg<1>();
			Return(&pointer[index]);
		}

		routine = function("one");

		if(routine)
		{
			auto callable = (CType*(*)(CType*, unsigned int))routine->getEntry();

			union PtrInt {
				CType* p;
				size_t i;
			};

			PtrInt base;
			base.i = 0x10000;

			for (int i = 0; i < 5; i++)
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

}

TEST(ReactorUnitTests, Coroutines_Fibonacci)
{
	if (!rr::Caps.CoroutinesSupported)
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
		While (true) {
			Yield(next);
			auto tmp = current + next;
			current = next;
			next = tmp;
		}
	}

	auto coroutine = function();

	int32_t expected[] =
	{
		0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597,
		2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418,
		317811,
	};

	auto count = sizeof(expected) / sizeof(expected[0]);

	for (size_t i = 0; i < count; i++)
	{
		int out = 0;
		EXPECT_EQ(coroutine->await(out), true);
		EXPECT_EQ(out, expected[i]);
	}
}

TEST(ReactorUnitTests, Coroutines_Parameters)
{
	if (!rr::Caps.CoroutinesSupported)
	{
		SUCCEED() << "Coroutines not supported";
		return;
	}

	Coroutine<uint8_t(uint8_t* data, int count)> function;
	{
		Pointer<Byte> data = function.Arg<0>();
		Int count = function.Arg<1>();

		For(Int i = 0, i < count, i++)
		{
			Yield(data[i]);
		}
	}

	uint8_t data[] = {10, 20, 30};
	auto coroutine = function(&data[0], 3);

	uint8_t out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 10); out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 20); out = 0;
	EXPECT_EQ(coroutine->await(out), true);
	EXPECT_EQ(out, 30); out = 99;
	EXPECT_EQ(coroutine->await(out), false);
	EXPECT_EQ(out, 99);
	EXPECT_EQ(coroutine->await(out), false);
	EXPECT_EQ(out, 99);
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
static_assert(std::is_same<CToReactorT<void>,     Void>::value, "");
static_assert(std::is_same<CToReactorT<bool>,     Bool>::value, "");
static_assert(std::is_same<CToReactorT<uint8_t>,  Byte>::value, "");
static_assert(std::is_same<CToReactorT<int8_t>,   SByte>::value, "");
static_assert(std::is_same<CToReactorT<int16_t>,  Short>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t>, UShort>::value, "");
static_assert(std::is_same<CToReactorT<int32_t>,  Int>::value, "");
static_assert(std::is_same<CToReactorT<uint64_t>, Long>::value, "");
static_assert(std::is_same<CToReactorT<uint32_t>, UInt>::value, "");
static_assert(std::is_same<CToReactorT<float>,    Float>::value, "");

// Assert CToReactorT for known pointer types resolves to expected types.
static_assert(std::is_same<CToReactorT<void*>,     Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<bool*>,     Pointer<Bool>>::value, "");
static_assert(std::is_same<CToReactorT<uint8_t*>,  Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<int8_t*>,   Pointer<SByte>>::value, "");
static_assert(std::is_same<CToReactorT<int16_t*>,  Pointer<Short>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t*>, Pointer<UShort>>::value, "");
static_assert(std::is_same<CToReactorT<int32_t*>,  Pointer<Int>>::value, "");
static_assert(std::is_same<CToReactorT<uint64_t*>, Pointer<Long>>::value, "");
static_assert(std::is_same<CToReactorT<uint32_t*>, Pointer<UInt>>::value, "");
static_assert(std::is_same<CToReactorT<float*>,    Pointer<Float>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t**>, Pointer<Pointer<UShort>>>::value, "");
static_assert(std::is_same<CToReactorT<uint16_t***>, Pointer<Pointer<Pointer<UShort>>>>::value, "");

// Assert CToReactorT for unknown pointer types resolves to Pointer<Byte>.
struct S{};
static_assert(std::is_same<CToReactorT<S*>, Pointer<Byte>>::value, "");
static_assert(std::is_same<CToReactorT<S**>, Pointer<Pointer<Byte>>>::value, "");
static_assert(std::is_same<CToReactorT<S***>, Pointer<Pointer<Pointer<Byte>>>>::value, "");

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
