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

#include "gtest/gtest.h"

using namespace sw;

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

TEST(SubzeroReactorTest, Sample)
{
	Routine *routine = nullptr;

	{
		Function<Int(Pointer<Int>, Int)> function;
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

		routine = function(L"one");

		if(routine)
		{
			int (*callable)(int*, int) = (int(*)(int*,int))routine->getEntry();
			int one[2] = {1, 0};
			int result = callable(&one[1], 2);
			EXPECT_EQ(result, reference(&one[1], 2));
		}
	}

	delete routine;
}

TEST(SubzeroReactorTest, SubVectorLoadStore)
{
	Routine *routine = nullptr;

	{
		Function<Int(Pointer<Byte>, Pointer<Byte>)> function;
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

		routine = function(L"one");

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
		
			int (*callable)(void*, void*) = (int(*)(void*,void*))routine->getEntry();
			callable(in, out);

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

	delete routine;
}

TEST(SubzeroReactorTest, VectorConstant)
{
	Routine *routine = nullptr;

	{
		Function<Int(Pointer<Byte>)> function;
		{
			Pointer<Byte> out = function.Arg<0>();

			*Pointer<Int4>(out + 16 * 0) = Int4(0x04030201, 0x08070605, 0x0C0B0A09, 0x100F0E0D);
			*Pointer<Short4>(out + 16 * 1) = Short4(0x1211, 0x1413, 0x1615, 0x1817);
			*Pointer<Byte8>(out + 16 * 2) = Byte8(0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20);
			*Pointer<Int2>(out + 16 * 3) = Int2(0x24232221, 0x28272625);

			Return(0);
		}

		routine = function(L"one");

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

			int(*callable)(void*) = (int(*)(void*))routine->getEntry();
			callable(out);

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

	delete routine;
}

TEST(SubzeroReactorTest, Swizzle)
{
	Routine *routine = nullptr;

	{
		Function<Int(Pointer<Byte>)> function;
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

			Return(0);
		}

		routine = function(L"one");

		if(routine)
		{
			float out[256 + 256 + 2][4];
			memset(out, 0, sizeof(out));

			float exp[256 + 256 + 2][4];

			for(int i = 0; i < 256; i++)
			{
				exp[i][0] = float((i >> 0) & 0x03) + 1.0f;
				exp[i][1] = float((i >> 2) & 0x03) + 1.0f;
				exp[i][2] = float((i >> 4) & 0x03) + 1.0f;
				exp[i][3] = float((i >> 6) & 0x03) + 1.0f;
			}

			for(int i = 0; i < 256; i++)
			{
				exp[256 + i][0] = float((i >> 0) & 0x03) + 1.0f;
				exp[256 + i][1] = float((i >> 2) & 0x03) + 1.0f;
				exp[256 + i][2] = float((i >> 4) & 0x03) + 5.0f;
				exp[256 + i][3] = float((i >> 6) & 0x03) + 5.0f;
			}

			exp[512 + 0][0] = 1.0f;
			exp[512 + 0][1] = 5.0f;
			exp[512 + 0][2] = 2.0f;
			exp[512 + 0][3] = 6.0f;

			exp[512 + 1][0] = 3.0f;
			exp[512 + 1][1] = 7.0f;
			exp[512 + 1][2] = 4.0f;
			exp[512 + 1][3] = 8.0f;

			int(*callable)(void*) = (int(*)(void*))routine->getEntry();
			callable(out);

			for(int i = 0; i < 256 + 256 + 2; i++)
			{
				EXPECT_EQ(out[i][0], exp[i][0]);
				EXPECT_EQ(out[i][1], exp[i][1]);
				EXPECT_EQ(out[i][2], exp[i][2]);
				EXPECT_EQ(out[i][3], exp[i][3]);
			}
		}
	}

	delete routine;
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
