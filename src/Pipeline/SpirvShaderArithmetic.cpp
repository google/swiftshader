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

#include <spirv/unified1/spirv.hpp>

namespace sw {

SpirvShader::EmitResult SpirvShader::EmitVectorTimesScalar(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		dst.move(i, lhs.Float(i) * rhs.Float(0));
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitMatrixTimesVector(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));
	auto rhsType = getType(rhs.type);

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		SIMD::Float v = lhs.Float(i) * rhs.Float(0);
		for(auto j = 1u; j < rhsType.sizeInComponents; j++)
		{
			v += lhs.Float(i + type.sizeInComponents * j) * rhs.Float(j);
		}
		dst.move(i, v);
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitVectorTimesMatrix(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));
	auto lhsType = getType(lhs.type);

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		SIMD::Float v = lhs.Float(0) * rhs.Float(i * lhsType.sizeInComponents);
		for(auto j = 1u; j < lhsType.sizeInComponents; j++)
		{
			v += lhs.Float(j) * rhs.Float(i * lhsType.sizeInComponents + j);
		}
		dst.move(i, v);
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitMatrixTimesMatrix(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));

	auto numColumns = type.definition.word(3);
	auto numRows = getType(type.definition.word(2)).definition.word(3);
	auto numAdds = getType(getObject(insn.word(3)).type).definition.word(3);

	for(auto row = 0u; row < numRows; row++)
	{
		for(auto col = 0u; col < numColumns; col++)
		{
			SIMD::Float v = SIMD::Float(0);
			for(auto i = 0u; i < numAdds; i++)
			{
				v += lhs.Float(i * numRows + row) * rhs.Float(col * numAdds + i);
			}
			dst.move(numRows * col + row, v);
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitOuterProduct(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));
	auto &lhsType = getType(lhs.type);
	auto &rhsType = getType(rhs.type);

	ASSERT(type.definition.opcode() == spv::OpTypeMatrix);
	ASSERT(lhsType.definition.opcode() == spv::OpTypeVector);
	ASSERT(rhsType.definition.opcode() == spv::OpTypeVector);
	ASSERT(getType(lhsType.element).opcode() == spv::OpTypeFloat);
	ASSERT(getType(rhsType.element).opcode() == spv::OpTypeFloat);

	auto numRows = lhsType.definition.word(3);
	auto numCols = rhsType.definition.word(3);

	for(auto col = 0u; col < numCols; col++)
	{
		for(auto row = 0u; row < numRows; row++)
		{
			dst.move(col * numRows + row, lhs.Float(row) * rhs.Float(col));
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitTranspose(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto mat = GenericValue(this, state, insn.word(3));

	auto numCols = type.definition.word(3);
	auto numRows = getType(type.definition.word(2)).sizeInComponents;

	for(auto col = 0u; col < numCols; col++)
	{
		for(auto row = 0u; row < numRows; row++)
		{
			dst.move(col * numRows + row, mat.Float(row * numCols + col));
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitUnaryOp(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto src = GenericValue(this, state, insn.word(3));

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		switch(insn.opcode())
		{
			case spv::OpNot:
			case spv::OpLogicalNot:  // logical not == bitwise not due to all-bits boolean representation
				dst.move(i, ~src.UInt(i));
				break;
			case spv::OpBitFieldInsert:
			{
				auto insert = GenericValue(this, state, insn.word(4)).UInt(i);
				auto offset = GenericValue(this, state, insn.word(5)).UInt(0);
				auto count = GenericValue(this, state, insn.word(6)).UInt(0);
				auto one = SIMD::UInt(1);
				auto v = src.UInt(i);
				auto mask = Bitmask32(offset + count) ^ Bitmask32(offset);
				dst.move(i, (v & ~mask) | ((insert << offset) & mask));
				break;
			}
			case spv::OpBitFieldSExtract:
			case spv::OpBitFieldUExtract:
			{
				auto offset = GenericValue(this, state, insn.word(4)).UInt(0);
				auto count = GenericValue(this, state, insn.word(5)).UInt(0);
				auto one = SIMD::UInt(1);
				auto v = src.UInt(i);
				SIMD::UInt out = (v >> offset) & Bitmask32(count);
				if(insn.opcode() == spv::OpBitFieldSExtract)
				{
					auto sign = out & NthBit32(count - one);
					auto sext = ~(sign - one);
					out |= sext;
				}
				dst.move(i, out);
				break;
			}
			case spv::OpBitReverse:
			{
				// TODO: Add an intrinsic to reactor. Even if there isn't a
				// single vector instruction, there may be target-dependent
				// ways to make this faster.
				// https://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
				SIMD::UInt v = src.UInt(i);
				v = ((v >> 1) & SIMD::UInt(0x55555555)) | ((v & SIMD::UInt(0x55555555)) << 1);
				v = ((v >> 2) & SIMD::UInt(0x33333333)) | ((v & SIMD::UInt(0x33333333)) << 2);
				v = ((v >> 4) & SIMD::UInt(0x0F0F0F0F)) | ((v & SIMD::UInt(0x0F0F0F0F)) << 4);
				v = ((v >> 8) & SIMD::UInt(0x00FF00FF)) | ((v & SIMD::UInt(0x00FF00FF)) << 8);
				v = (v >> 16) | (v << 16);
				dst.move(i, v);
				break;
			}
			case spv::OpBitCount:
				dst.move(i, CountBits(src.UInt(i)));
				break;
			case spv::OpSNegate:
				dst.move(i, -src.Int(i));
				break;
			case spv::OpFNegate:
				dst.move(i, -src.Float(i));
				break;
			case spv::OpConvertFToU:
				dst.move(i, SIMD::UInt(src.Float(i)));
				break;
			case spv::OpConvertFToS:
				dst.move(i, SIMD::Int(src.Float(i)));
				break;
			case spv::OpConvertSToF:
				dst.move(i, SIMD::Float(src.Int(i)));
				break;
			case spv::OpConvertUToF:
				dst.move(i, SIMD::Float(src.UInt(i)));
				break;
			case spv::OpBitcast:
				dst.move(i, src.Float(i));
				break;
			case spv::OpIsInf:
				dst.move(i, IsInf(src.Float(i)));
				break;
			case spv::OpIsNan:
				dst.move(i, IsNan(src.Float(i)));
				break;
			case spv::OpDPdx:
			case spv::OpDPdxCoarse:
				// Derivative instructions: FS invocations are laid out like so:
				//    0 1
				//    2 3
				static_assert(SIMD::Width == 4, "All cross-lane instructions will need care when using a different width");
				dst.move(i, SIMD::Float(Extract(src.Float(i), 1) - Extract(src.Float(i), 0)));
				break;
			case spv::OpDPdy:
			case spv::OpDPdyCoarse:
				dst.move(i, SIMD::Float(Extract(src.Float(i), 2) - Extract(src.Float(i), 0)));
				break;
			case spv::OpFwidth:
			case spv::OpFwidthCoarse:
				dst.move(i, SIMD::Float(Abs(Extract(src.Float(i), 1) - Extract(src.Float(i), 0)) + Abs(Extract(src.Float(i), 2) - Extract(src.Float(i), 0))));
				break;
			case spv::OpDPdxFine:
			{
				auto firstRow = Extract(src.Float(i), 1) - Extract(src.Float(i), 0);
				auto secondRow = Extract(src.Float(i), 3) - Extract(src.Float(i), 2);
				SIMD::Float v = SIMD::Float(firstRow);
				v = Insert(v, secondRow, 2);
				v = Insert(v, secondRow, 3);
				dst.move(i, v);
				break;
			}
			case spv::OpDPdyFine:
			{
				auto firstColumn = Extract(src.Float(i), 2) - Extract(src.Float(i), 0);
				auto secondColumn = Extract(src.Float(i), 3) - Extract(src.Float(i), 1);
				SIMD::Float v = SIMD::Float(firstColumn);
				v = Insert(v, secondColumn, 1);
				v = Insert(v, secondColumn, 3);
				dst.move(i, v);
				break;
			}
			case spv::OpFwidthFine:
			{
				auto firstRow = Extract(src.Float(i), 1) - Extract(src.Float(i), 0);
				auto secondRow = Extract(src.Float(i), 3) - Extract(src.Float(i), 2);
				SIMD::Float dpdx = SIMD::Float(firstRow);
				dpdx = Insert(dpdx, secondRow, 2);
				dpdx = Insert(dpdx, secondRow, 3);
				auto firstColumn = Extract(src.Float(i), 2) - Extract(src.Float(i), 0);
				auto secondColumn = Extract(src.Float(i), 3) - Extract(src.Float(i), 1);
				SIMD::Float dpdy = SIMD::Float(firstColumn);
				dpdy = Insert(dpdy, secondColumn, 1);
				dpdy = Insert(dpdy, secondColumn, 3);
				dst.move(i, Abs(dpdx) + Abs(dpdy));
				break;
			}
			case spv::OpQuantizeToF16:
			{
				// Note: keep in sync with the specialization constant version in EvalSpecConstantUnaryOp
				auto abs = Abs(src.Float(i));
				auto sign = src.Int(i) & SIMD::Int(0x80000000);
				auto isZero = CmpLT(abs, SIMD::Float(0.000061035f));
				auto isInf = CmpGT(abs, SIMD::Float(65504.0f));
				auto isNaN = IsNan(abs);
				auto isInfOrNan = isInf | isNaN;
				SIMD::Int v = src.Int(i) & SIMD::Int(0xFFFFE000);
				v &= ~isZero | SIMD::Int(0x80000000);
				v = sign | (isInfOrNan & SIMD::Int(0x7F800000)) | (~isInfOrNan & v);
				v |= isNaN & SIMD::Int(0x400000);
				dst.move(i, v);
				break;
			}
			default:
				UNREACHABLE("%s", OpcodeName(insn.opcode()).c_str());
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitBinaryOp(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &lhsType = getType(getObject(insn.word(3)).type);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));

	for(auto i = 0u; i < lhsType.sizeInComponents; i++)
	{
		switch(insn.opcode())
		{
			case spv::OpIAdd:
				dst.move(i, lhs.Int(i) + rhs.Int(i));
				break;
			case spv::OpISub:
				dst.move(i, lhs.Int(i) - rhs.Int(i));
				break;
			case spv::OpIMul:
				dst.move(i, lhs.Int(i) * rhs.Int(i));
				break;
			case spv::OpSDiv:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0));                                       // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1)));  // prevent integer overflow
				dst.move(i, a / b);
				break;
			}
			case spv::OpUDiv:
			{
				auto zeroMask = As<SIMD::UInt>(CmpEQ(rhs.Int(i), SIMD::Int(0)));
				dst.move(i, lhs.UInt(i) / (rhs.UInt(i) | zeroMask));
				break;
			}
			case spv::OpSRem:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0));                                       // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1)));  // prevent integer overflow
				dst.move(i, a % b);
				break;
			}
			case spv::OpSMod:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0));                                       // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1)));  // prevent integer overflow
				auto mod = a % b;
				// If a and b have opposite signs, the remainder operation takes
				// the sign from a but OpSMod is supposed to take the sign of b.
				// Adding b will ensure that the result has the correct sign and
				// that it is still congruent to a modulo b.
				//
				// See also http://mathforum.org/library/drmath/view/52343.html
				auto signDiff = CmpNEQ(CmpGE(a, SIMD::Int(0)), CmpGE(b, SIMD::Int(0)));
				auto fixedMod = mod + (b & CmpNEQ(mod, SIMD::Int(0)) & signDiff);
				dst.move(i, As<SIMD::Float>(fixedMod));
				break;
			}
			case spv::OpUMod:
			{
				auto zeroMask = As<SIMD::UInt>(CmpEQ(rhs.Int(i), SIMD::Int(0)));
				dst.move(i, lhs.UInt(i) % (rhs.UInt(i) | zeroMask));
				break;
			}
			case spv::OpIEqual:
			case spv::OpLogicalEqual:
				dst.move(i, CmpEQ(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpINotEqual:
			case spv::OpLogicalNotEqual:
				dst.move(i, CmpNEQ(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUGreaterThan:
				dst.move(i, CmpGT(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSGreaterThan:
				dst.move(i, CmpGT(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUGreaterThanEqual:
				dst.move(i, CmpGE(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSGreaterThanEqual:
				dst.move(i, CmpGE(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpULessThan:
				dst.move(i, CmpLT(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSLessThan:
				dst.move(i, CmpLT(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpULessThanEqual:
				dst.move(i, CmpLE(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSLessThanEqual:
				dst.move(i, CmpLE(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpFAdd:
				dst.move(i, lhs.Float(i) + rhs.Float(i));
				break;
			case spv::OpFSub:
				dst.move(i, lhs.Float(i) - rhs.Float(i));
				break;
			case spv::OpFMul:
				dst.move(i, lhs.Float(i) * rhs.Float(i));
				break;
			case spv::OpFDiv:
				dst.move(i, lhs.Float(i) / rhs.Float(i));
				break;
			case spv::OpFMod:
				// TODO(b/126873455): inaccurate for values greater than 2^24
				dst.move(i, lhs.Float(i) - rhs.Float(i) * Floor(lhs.Float(i) / rhs.Float(i)));
				break;
			case spv::OpFRem:
				dst.move(i, lhs.Float(i) % rhs.Float(i));
				break;
			case spv::OpFOrdEqual:
				dst.move(i, CmpEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordEqual:
				dst.move(i, CmpUEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdNotEqual:
				dst.move(i, CmpNEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordNotEqual:
				dst.move(i, CmpUNEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdLessThan:
				dst.move(i, CmpLT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordLessThan:
				dst.move(i, CmpULT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdGreaterThan:
				dst.move(i, CmpGT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordGreaterThan:
				dst.move(i, CmpUGT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdLessThanEqual:
				dst.move(i, CmpLE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordLessThanEqual:
				dst.move(i, CmpULE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdGreaterThanEqual:
				dst.move(i, CmpGE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordGreaterThanEqual:
				dst.move(i, CmpUGE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpShiftRightLogical:
				dst.move(i, lhs.UInt(i) >> rhs.UInt(i));
				break;
			case spv::OpShiftRightArithmetic:
				dst.move(i, lhs.Int(i) >> rhs.Int(i));
				break;
			case spv::OpShiftLeftLogical:
				dst.move(i, lhs.UInt(i) << rhs.UInt(i));
				break;
			case spv::OpBitwiseOr:
			case spv::OpLogicalOr:
				dst.move(i, lhs.UInt(i) | rhs.UInt(i));
				break;
			case spv::OpBitwiseXor:
				dst.move(i, lhs.UInt(i) ^ rhs.UInt(i));
				break;
			case spv::OpBitwiseAnd:
			case spv::OpLogicalAnd:
				dst.move(i, lhs.UInt(i) & rhs.UInt(i));
				break;
			case spv::OpSMulExtended:
				// Extended ops: result is a structure containing two members of the same type as lhs & rhs.
				// In our flat view then, component i is the i'th component of the first member;
				// component i + N is the i'th component of the second member.
				dst.move(i, lhs.Int(i) * rhs.Int(i));
				dst.move(i + lhsType.sizeInComponents, MulHigh(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUMulExtended:
				dst.move(i, lhs.UInt(i) * rhs.UInt(i));
				dst.move(i + lhsType.sizeInComponents, MulHigh(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpIAddCarry:
				dst.move(i, lhs.UInt(i) + rhs.UInt(i));
				dst.move(i + lhsType.sizeInComponents, CmpLT(dst.UInt(i), lhs.UInt(i)) >> 31);
				break;
			case spv::OpISubBorrow:
				dst.move(i, lhs.UInt(i) - rhs.UInt(i));
				dst.move(i + lhsType.sizeInComponents, CmpLT(lhs.UInt(i), rhs.UInt(i)) >> 31);
				break;
			default:
				UNREACHABLE("%s", OpcodeName(insn.opcode()).c_str());
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitDot(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	ASSERT(type.sizeInComponents == 1);
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &lhsType = getType(getObject(insn.word(3)).type);
	auto lhs = GenericValue(this, state, insn.word(3));
	auto rhs = GenericValue(this, state, insn.word(4));

	dst.move(0, Dot(lhsType.sizeInComponents, lhs, rhs));
	return EmitResult::Continue;
}

SIMD::Float SpirvShader::Dot(unsigned numComponents, GenericValue const &x, GenericValue const &y) const
{
	SIMD::Float d = x.Float(0) * y.Float(0);

	for(auto i = 1u; i < numComponents; i++)
	{
		d += x.Float(i) * y.Float(i);
	}

	return d;
}

std::pair<SIMD::Float, SIMD::Int> SpirvShader::Frexp(RValue<SIMD::Float> val) const
{
	// Assumes IEEE 754
	auto v = As<SIMD::UInt>(val);
	auto isNotZero = CmpNEQ(v & SIMD::UInt(0x7FFFFFFF), SIMD::UInt(0));
	auto zeroSign = v & SIMD::UInt(0x80000000) & ~isNotZero;
	auto significand = As<SIMD::Float>((((v & SIMD::UInt(0x807FFFFF)) | SIMD::UInt(0x3F000000)) & isNotZero) | zeroSign);
	auto exponent = Exponent(val) & SIMD::Int(isNotZero);
	return std::make_pair(significand, exponent);
}

}  // namespace sw