// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "SpirvShader.hpp"

#include "ShaderCore.hpp"

#include <spirv/unified1/GLSL.std.450.h>
#include <spirv/unified1/spirv.hpp>

namespace {
constexpr float PI = 3.141592653589793f;
}

namespace sw {

SpirvShader::EmitResult SpirvShader::EmitExtGLSLstd450(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.resultTypeId());
	auto &dst = state->createIntermediate(insn.resultId(), type.componentCount);
	auto extInstIndex = static_cast<GLSLstd450>(insn.word(4));

	switch(extInstIndex)
	{
		case GLSLstd450FAbs:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Abs(src.Float(i)));
			}
			break;
		}
		case GLSLstd450SAbs:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Abs(src.Int(i)));
			}
			break;
		}
		case GLSLstd450Cross:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			dst.move(0, lhs.Float(1) * rhs.Float(2) - rhs.Float(1) * lhs.Float(2));
			dst.move(1, lhs.Float(2) * rhs.Float(0) - rhs.Float(2) * lhs.Float(0));
			dst.move(2, lhs.Float(0) * rhs.Float(1) - rhs.Float(0) * lhs.Float(1));
			break;
		}
		case GLSLstd450Floor:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Floor(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Trunc:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Trunc(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Ceil:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Ceil(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Fract:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Frac(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Round:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Round(src.Float(i)));
			}
			break;
		}
		case GLSLstd450RoundEven:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto x = Round(src.Float(i));
				// dst = round(src) + ((round(src) < src) * 2 - 1) * (fract(src) == 0.5) * isOdd(round(src));
				dst.move(i, x + ((SIMD::Float(CmpLT(x, src.Float(i)) & SIMD::Int(1)) * SIMD::Float(2.0f)) - SIMD::Float(1.0f)) *
				                    SIMD::Float(CmpEQ(Frac(src.Float(i)), SIMD::Float(0.5f)) & SIMD::Int(1)) * SIMD::Float(Int4(x) & SIMD::Int(1)));
			}
			break;
		}
		case GLSLstd450FMin:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(lhs.Float(i), rhs.Float(i)));
			}
			break;
		}
		case GLSLstd450FMax:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Max(lhs.Float(i), rhs.Float(i)));
			}
			break;
		}
		case GLSLstd450SMin:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(lhs.Int(i), rhs.Int(i)));
			}
			break;
		}
		case GLSLstd450SMax:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Max(lhs.Int(i), rhs.Int(i)));
			}
			break;
		}
		case GLSLstd450UMin:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(lhs.UInt(i), rhs.UInt(i)));
			}
			break;
		}
		case GLSLstd450UMax:
		{
			auto lhs = Operand(this, state, insn.word(5));
			auto rhs = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Max(lhs.UInt(i), rhs.UInt(i)));
			}
			break;
		}
		case GLSLstd450Step:
		{
			auto edge = Operand(this, state, insn.word(5));
			auto x = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, CmpNLT(x.Float(i), edge.Float(i)) & As<SIMD::Int>(SIMD::Float(1.0f)));
			}
			break;
		}
		case GLSLstd450SmoothStep:
		{
			auto edge0 = Operand(this, state, insn.word(5));
			auto edge1 = Operand(this, state, insn.word(6));
			auto x = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto tx = Min(Max((x.Float(i) - edge0.Float(i)) /
				                      (edge1.Float(i) - edge0.Float(i)),
				                  SIMD::Float(0.0f)),
				              SIMD::Float(1.0f));
				dst.move(i, tx * tx * (Float4(3.0f) - Float4(2.0f) * tx));
			}
			break;
		}
		case GLSLstd450FMix:
		{
			auto x = Operand(this, state, insn.word(5));
			auto y = Operand(this, state, insn.word(6));
			auto a = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, a.Float(i) * (y.Float(i) - x.Float(i)) + x.Float(i));
			}
			break;
		}
		case GLSLstd450FClamp:
		{
			auto x = Operand(this, state, insn.word(5));
			auto minVal = Operand(this, state, insn.word(6));
			auto maxVal = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(Max(x.Float(i), minVal.Float(i)), maxVal.Float(i)));
			}
			break;
		}
		case GLSLstd450SClamp:
		{
			auto x = Operand(this, state, insn.word(5));
			auto minVal = Operand(this, state, insn.word(6));
			auto maxVal = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(Max(x.Int(i), minVal.Int(i)), maxVal.Int(i)));
			}
			break;
		}
		case GLSLstd450UClamp:
		{
			auto x = Operand(this, state, insn.word(5));
			auto minVal = Operand(this, state, insn.word(6));
			auto maxVal = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Min(Max(x.UInt(i), minVal.UInt(i)), maxVal.UInt(i)));
			}
			break;
		}
		case GLSLstd450FSign:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto neg = As<SIMD::Int>(CmpLT(src.Float(i), SIMD::Float(-0.0f))) & As<SIMD::Int>(SIMD::Float(-1.0f));
				auto pos = As<SIMD::Int>(CmpNLE(src.Float(i), SIMD::Float(+0.0f))) & As<SIMD::Int>(SIMD::Float(1.0f));
				dst.move(i, neg | pos);
			}
			break;
		}
		case GLSLstd450SSign:
		{
			auto src = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto neg = CmpLT(src.Int(i), SIMD::Int(0)) & SIMD::Int(-1);
				auto pos = CmpNLE(src.Int(i), SIMD::Int(0)) & SIMD::Int(1);
				dst.move(i, neg | pos);
			}
			break;
		}
		case GLSLstd450Reflect:
		{
			auto I = Operand(this, state, insn.word(5));
			auto N = Operand(this, state, insn.word(6));

			SIMD::Float d = Dot(type.componentCount, I, N);

			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, I.Float(i) - SIMD::Float(2.0f) * d * N.Float(i));
			}
			break;
		}
		case GLSLstd450Refract:
		{
			auto I = Operand(this, state, insn.word(5));
			auto N = Operand(this, state, insn.word(6));
			auto eta = Operand(this, state, insn.word(7));

			SIMD::Float d = Dot(type.componentCount, I, N);
			SIMD::Float k = SIMD::Float(1.0f) - eta.Float(0) * eta.Float(0) * (SIMD::Float(1.0f) - d * d);
			SIMD::Int pos = CmpNLT(k, SIMD::Float(0.0f));
			SIMD::Float t = (eta.Float(0) * d + Sqrt(k));

			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, pos & As<SIMD::Int>(eta.Float(0) * I.Float(i) - t * N.Float(i)));
			}
			break;
		}
		case GLSLstd450FaceForward:
		{
			auto N = Operand(this, state, insn.word(5));
			auto I = Operand(this, state, insn.word(6));
			auto Nref = Operand(this, state, insn.word(7));

			SIMD::Float d = Dot(type.componentCount, I, Nref);
			SIMD::Int neg = CmpLT(d, SIMD::Float(0.0f));

			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto n = N.Float(i);
				dst.move(i, (neg & As<SIMD::Int>(n)) | (~neg & As<SIMD::Int>(-n)));
			}
			break;
		}
		case GLSLstd450Length:
		{
			auto x = Operand(this, state, insn.word(5));
			SIMD::Float d = Dot(getType(getObject(insn.word(5))).componentCount, x, x);

			dst.move(0, Sqrt(d));
			break;
		}
		case GLSLstd450Normalize:
		{
			auto x = Operand(this, state, insn.word(5));
			SIMD::Float d = Dot(getType(getObject(insn.word(5))).componentCount, x, x);
			SIMD::Float invLength = SIMD::Float(1.0f) / Sqrt(d);

			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, invLength * x.Float(i));
			}
			break;
		}
		case GLSLstd450Distance:
		{
			auto p0 = Operand(this, state, insn.word(5));
			auto p1 = Operand(this, state, insn.word(6));

			// sqrt(dot(p0-p1, p0-p1))
			SIMD::Float d = (p0.Float(0) - p1.Float(0)) * (p0.Float(0) - p1.Float(0));

			for(auto i = 1u; i < p0.componentCount; i++)
			{
				d += (p0.Float(i) - p1.Float(i)) * (p0.Float(i) - p1.Float(i));
			}

			dst.move(0, Sqrt(d));
			break;
		}
		case GLSLstd450Modf:
		{
			auto val = Operand(this, state, insn.word(5));
			auto ptrId = Object::ID(insn.word(6));

			Intermediate whole(type.componentCount);

			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto wholeAndFrac = Modf(val.Float(i));
				dst.move(i, wholeAndFrac.second);
				whole.move(i, wholeAndFrac.first);
			}

			Store(ptrId, whole, false, std::memory_order_relaxed, state);
			break;
		}
		case GLSLstd450ModfStruct:
		{
			auto val = Operand(this, state, insn.word(5));

			for(auto i = 0u; i < val.componentCount; i++)
			{
				auto wholeAndFrac = Modf(val.Float(i));
				dst.move(i, wholeAndFrac.second);
				dst.move(val.componentCount + i, wholeAndFrac.first);
			}
			break;
		}
		case GLSLstd450PackSnorm4x8:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, (SIMD::Int(Round(Min(Max(val.Float(0), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
			             SIMD::Int(0xFF)) |
			                ((SIMD::Int(Round(Min(Max(val.Float(1), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
			                  SIMD::Int(0xFF))
			                 << 8) |
			                ((SIMD::Int(Round(Min(Max(val.Float(2), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
			                  SIMD::Int(0xFF))
			                 << 16) |
			                ((SIMD::Int(Round(Min(Max(val.Float(3), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
			                  SIMD::Int(0xFF))
			                 << 24));
			break;
		}
		case GLSLstd450PackUnorm4x8:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, (SIMD::UInt(Round(Min(Max(val.Float(0), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) |
			                ((SIMD::UInt(Round(Min(Max(val.Float(1), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 8) |
			                ((SIMD::UInt(Round(Min(Max(val.Float(2), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 16) |
			                ((SIMD::UInt(Round(Min(Max(val.Float(3), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 24));
			break;
		}
		case GLSLstd450PackSnorm2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, (SIMD::Int(Round(Min(Max(val.Float(0), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(32767.0f))) &
			             SIMD::Int(0xFFFF)) |
			                ((SIMD::Int(Round(Min(Max(val.Float(1), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(32767.0f))) &
			                  SIMD::Int(0xFFFF))
			                 << 16));
			break;
		}
		case GLSLstd450PackUnorm2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, (SIMD::UInt(Round(Min(Max(val.Float(0), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(65535.0f))) &
			             SIMD::UInt(0xFFFF)) |
			                ((SIMD::UInt(Round(Min(Max(val.Float(1), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(65535.0f))) &
			                  SIMD::UInt(0xFFFF))
			                 << 16));
			break;
		}
		case GLSLstd450PackHalf2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, floatToHalfBits(val.UInt(0), false) | floatToHalfBits(val.UInt(1), true));
			break;
		}
		case GLSLstd450UnpackSnorm4x8:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, Min(Max(SIMD::Float(((val.Int(0) << 24) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(1, Min(Max(SIMD::Float(((val.Int(0) << 16) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(2, Min(Max(SIMD::Float(((val.Int(0) << 8) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(3, Min(Max(SIMD::Float(((val.Int(0)) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			break;
		}
		case GLSLstd450UnpackUnorm4x8:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, SIMD::Float((val.UInt(0) & SIMD::UInt(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(1, SIMD::Float(((val.UInt(0) >> 8) & SIMD::UInt(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(2, SIMD::Float(((val.UInt(0) >> 16) & SIMD::UInt(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(3, SIMD::Float(((val.UInt(0) >> 24) & SIMD::UInt(0xFF))) * SIMD::Float(1.0f / 255.f));
			break;
		}
		case GLSLstd450UnpackSnorm2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			// clamp(f / 32767.0, -1.0, 1.0)
			dst.move(0, Min(Max(SIMD::Float(As<SIMD::Int>((val.UInt(0) & SIMD::UInt(0x0000FFFF)) << 16)) *
			                        SIMD::Float(1.0f / float(0x7FFF0000)),
			                    SIMD::Float(-1.0f)),
			                SIMD::Float(1.0f)));
			dst.move(1, Min(Max(SIMD::Float(As<SIMD::Int>(val.UInt(0) & SIMD::UInt(0xFFFF0000))) * SIMD::Float(1.0f / float(0x7FFF0000)),
			                    SIMD::Float(-1.0f)),
			                SIMD::Float(1.0f)));
			break;
		}
		case GLSLstd450UnpackUnorm2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			// f / 65535.0
			dst.move(0, SIMD::Float((val.UInt(0) & SIMD::UInt(0x0000FFFF)) << 16) * SIMD::Float(1.0f / float(0xFFFF0000)));
			dst.move(1, SIMD::Float(val.UInt(0) & SIMD::UInt(0xFFFF0000)) * SIMD::Float(1.0f / float(0xFFFF0000)));
			break;
		}
		case GLSLstd450UnpackHalf2x16:
		{
			auto val = Operand(this, state, insn.word(5));
			dst.move(0, halfToFloatBits(val.UInt(0) & SIMD::UInt(0x0000FFFF)));
			dst.move(1, halfToFloatBits((val.UInt(0) & SIMD::UInt(0xFFFF0000)) >> 16));
			break;
		}
		case GLSLstd450Fma:
		{
			auto a = Operand(this, state, insn.word(5));
			auto b = Operand(this, state, insn.word(6));
			auto c = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, FMA(a.Float(i), b.Float(i), c.Float(i)));
			}
			break;
		}
		case GLSLstd450Frexp:
		{
			auto val = Operand(this, state, insn.word(5));
			auto ptrId = Object::ID(insn.word(6));

			Intermediate exp(type.componentCount);

			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto significandAndExponent = Frexp(val.Float(i));
				dst.move(i, significandAndExponent.first);
				exp.move(i, significandAndExponent.second);
			}

			Store(ptrId, exp, false, std::memory_order_relaxed, state);
			break;
		}
		case GLSLstd450FrexpStruct:
		{
			auto val = Operand(this, state, insn.word(5));

			for(auto i = 0u; i < val.componentCount; i++)
			{
				auto significandAndExponent = Frexp(val.Float(i));
				dst.move(i, significandAndExponent.first);
				dst.move(val.componentCount + i, significandAndExponent.second);
			}
			break;
		}
		case GLSLstd450Ldexp:
		{
			auto significand = Operand(this, state, insn.word(5));
			auto exponent = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				// Assumes IEEE 754
				auto in = significand.Float(i);
				auto significandExponent = Exponent(in);
				auto combinedExponent = exponent.Int(i) + significandExponent;
				auto isSignificandZero = SIMD::UInt(CmpEQ(significand.Int(i), SIMD::Int(0)));
				auto isSignificandInf = SIMD::UInt(IsInf(in));
				auto isSignificandNaN = SIMD::UInt(IsNan(in));
				auto isExponentNotTooSmall = SIMD::UInt(CmpGE(combinedExponent, SIMD::Int(-126)));
				auto isExponentNotTooLarge = SIMD::UInt(CmpLE(combinedExponent, SIMD::Int(128)));
				auto isExponentInBounds = isExponentNotTooSmall & isExponentNotTooLarge;

				SIMD::UInt v;
				v = significand.UInt(i) & SIMD::UInt(0x7FFFFF);                          // Add significand.
				v |= (SIMD::UInt(combinedExponent + SIMD::Int(126)) << SIMD::UInt(23));  // Add exponent.
				v &= isExponentInBounds;                                                 // Clear v if the exponent is OOB.

				v |= significand.UInt(i) & SIMD::UInt(0x80000000);     // Add sign bit.
				v |= ~isExponentNotTooLarge & SIMD::UInt(0x7F800000);  // Mark as inf if the exponent is too great.

				// If the input significand is zero, inf or nan, just return the
				// input significand.
				auto passthrough = isSignificandZero | isSignificandInf | isSignificandNaN;
				v = (v & ~passthrough) | (significand.UInt(i) & passthrough);

				dst.move(i, As<SIMD::Float>(v));
			}
			break;
		}
		case GLSLstd450Radians:
		{
			auto degrees = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, degrees.Float(i) * SIMD::Float(PI / 180.0f));
			}
			break;
		}
		case GLSLstd450Degrees:
		{
			auto radians = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, radians.Float(i) * SIMD::Float(180.0f / PI));
			}
			break;
		}
		case GLSLstd450Sin:
		{
			auto radians = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Sin(radians.Float(i)));
			}
			break;
		}
		case GLSLstd450Cos:
		{
			auto radians = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Cos(radians.Float(i)));
			}
			break;
		}
		case GLSLstd450Tan:
		{
			auto radians = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Tan(radians.Float(i)));
			}
			break;
		}
		case GLSLstd450Asin:
		{
			auto val = Operand(this, state, insn.word(5));
			Decorations d;
			ApplyDecorationsForId(&d, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Asin(val.Float(i), d.RelaxedPrecision ? Precision::Relaxed : Precision::Full));
			}
			break;
		}
		case GLSLstd450Acos:
		{
			auto val = Operand(this, state, insn.word(5));
			Decorations d;
			ApplyDecorationsForId(&d, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Acos(val.Float(i), d.RelaxedPrecision ? Precision::Relaxed : Precision::Full));
			}
			break;
		}
		case GLSLstd450Atan:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Atan(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Sinh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Sinh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Cosh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Cosh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Tanh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Tanh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Asinh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Asinh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Acosh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Acosh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Atanh:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Atanh(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Atan2:
		{
			auto x = Operand(this, state, insn.word(5));
			auto y = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Atan2(x.Float(i), y.Float(i)));
			}
			break;
		}
		case GLSLstd450Pow:
		{
			auto x = Operand(this, state, insn.word(5));
			auto y = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Pow(x.Float(i), y.Float(i)));
			}
			break;
		}
		case GLSLstd450Exp:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Exp(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Log:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Log(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Exp2:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Exp2(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Log2:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Log2(val.Float(i)));
			}
			break;
		}
		case GLSLstd450Sqrt:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, Sqrt(val.Float(i)));
			}
			break;
		}
		case GLSLstd450InverseSqrt:
		{
			auto val = Operand(this, state, insn.word(5));
			Decorations d;
			ApplyDecorationsForId(&d, insn.word(5));
			if(d.RelaxedPrecision)
			{
				for(auto i = 0u; i < type.componentCount; i++)
				{
					dst.move(i, RcpSqrt_pp(val.Float(i)));
				}
			}
			else
			{
				for(auto i = 0u; i < type.componentCount; i++)
				{
					dst.move(i, SIMD::Float(1.0f) / Sqrt(val.Float(i)));
				}
			}
			break;
		}
		case GLSLstd450Determinant:
		{
			auto mat = Operand(this, state, insn.word(5));

			switch(mat.componentCount)
			{
				case 4:  // 2x2
					dst.move(0, Determinant(
					                mat.Float(0), mat.Float(1),
					                mat.Float(2), mat.Float(3)));
					break;
				case 9:  // 3x3
					dst.move(0, Determinant(
					                mat.Float(0), mat.Float(1), mat.Float(2),
					                mat.Float(3), mat.Float(4), mat.Float(5),
					                mat.Float(6), mat.Float(7), mat.Float(8)));
					break;
				case 16:  // 4x4
					dst.move(0, Determinant(
					                mat.Float(0), mat.Float(1), mat.Float(2), mat.Float(3),
					                mat.Float(4), mat.Float(5), mat.Float(6), mat.Float(7),
					                mat.Float(8), mat.Float(9), mat.Float(10), mat.Float(11),
					                mat.Float(12), mat.Float(13), mat.Float(14), mat.Float(15)));
					break;
				default:
					UNREACHABLE("GLSLstd450Determinant can only operate with square matrices. Got %d elements", int(mat.componentCount));
			}
			break;
		}
		case GLSLstd450MatrixInverse:
		{
			auto mat = Operand(this, state, insn.word(5));

			switch(mat.componentCount)
			{
				case 4:  // 2x2
				{
					auto inv = MatrixInverse(
					    mat.Float(0), mat.Float(1),
					    mat.Float(2), mat.Float(3));
					for(uint32_t i = 0; i < inv.size(); i++)
					{
						dst.move(i, inv[i]);
					}
					break;
				}
				case 9:  // 3x3
				{
					auto inv = MatrixInverse(
					    mat.Float(0), mat.Float(1), mat.Float(2),
					    mat.Float(3), mat.Float(4), mat.Float(5),
					    mat.Float(6), mat.Float(7), mat.Float(8));
					for(uint32_t i = 0; i < inv.size(); i++)
					{
						dst.move(i, inv[i]);
					}
					break;
				}
				case 16:  // 4x4
				{
					auto inv = MatrixInverse(
					    mat.Float(0), mat.Float(1), mat.Float(2), mat.Float(3),
					    mat.Float(4), mat.Float(5), mat.Float(6), mat.Float(7),
					    mat.Float(8), mat.Float(9), mat.Float(10), mat.Float(11),
					    mat.Float(12), mat.Float(13), mat.Float(14), mat.Float(15));
					for(uint32_t i = 0; i < inv.size(); i++)
					{
						dst.move(i, inv[i]);
					}
					break;
				}
				default:
					UNREACHABLE("GLSLstd450MatrixInverse can only operate with square matrices. Got %d elements", int(mat.componentCount));
			}
			break;
		}
		case GLSLstd450IMix:
		{
			UNREACHABLE("GLSLstd450IMix has been removed from the specification");
			break;
		}
		case GLSLstd450PackDouble2x32:
		{
			UNSUPPORTED("SPIR-V Float64 Capability (GLSLstd450PackDouble2x32)");
			break;
		}
		case GLSLstd450UnpackDouble2x32:
		{
			UNSUPPORTED("SPIR-V Float64 Capability (GLSLstd450UnpackDouble2x32)");
			break;
		}
		case GLSLstd450FindILsb:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto v = val.UInt(i);
				dst.move(i, Cttz(v, true) | CmpEQ(v, SIMD::UInt(0)));
			}
			break;
		}
		case GLSLstd450FindSMsb:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto v = val.UInt(i) ^ As<SIMD::UInt>(CmpLT(val.Int(i), SIMD::Int(0)));
				dst.move(i, SIMD::UInt(31) - Ctlz(v, false));
			}
			break;
		}
		case GLSLstd450FindUMsb:
		{
			auto val = Operand(this, state, insn.word(5));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, SIMD::UInt(31) - Ctlz(val.UInt(i), false));
			}
			break;
		}
		case GLSLstd450InterpolateAtCentroid:
		{
			UNSUPPORTED("SPIR-V SampleRateShading Capability (GLSLstd450InterpolateAtCentroid)");
			break;
		}
		case GLSLstd450InterpolateAtSample:
		{
			UNSUPPORTED("SPIR-V SampleRateShading Capability (GLSLstd450InterpolateAtCentroid)");
			break;
		}
		case GLSLstd450InterpolateAtOffset:
		{
			UNSUPPORTED("SPIR-V SampleRateShading Capability (GLSLstd450InterpolateAtCentroid)");
			break;
		}
		case GLSLstd450NMin:
		{
			auto x = Operand(this, state, insn.word(5));
			auto y = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, NMin(x.Float(i), y.Float(i)));
			}
			break;
		}
		case GLSLstd450NMax:
		{
			auto x = Operand(this, state, insn.word(5));
			auto y = Operand(this, state, insn.word(6));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				dst.move(i, NMax(x.Float(i), y.Float(i)));
			}
			break;
		}
		case GLSLstd450NClamp:
		{
			auto x = Operand(this, state, insn.word(5));
			auto minVal = Operand(this, state, insn.word(6));
			auto maxVal = Operand(this, state, insn.word(7));
			for(auto i = 0u; i < type.componentCount; i++)
			{
				auto clamp = NMin(NMax(x.Float(i), minVal.Float(i)), maxVal.Float(i));
				dst.move(i, clamp);
			}
			break;
		}
		default:
			UNREACHABLE("ExtInst %d", int(extInstIndex));
			break;
	}

	return EmitResult::Continue;
}

}  // namespace sw