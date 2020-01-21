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

#include <spirv/unified1/spirv.hpp>

namespace sw {

struct SpirvShader::Impl::Group
{
	// Template function to perform a binary operation.
	// |TYPE| should be the type of the binary operation (as a SIMD::<ScalarType>).
	// |I| should be a type suitable to initialize the identity value.
	// |APPLY| should be a callable object that takes two RValue<TYPE> parameters
	// and returns a new RValue<TYPE> corresponding to the operation's result.
	template<typename TYPE, typename I, typename APPLY>
	static void BinaryOperation(
	    const SpirvShader *shader,
	    const SpirvShader::InsnIterator &insn,
	    const SpirvShader::EmitState *state,
	    Intermediate &dst,
	    const I identityValue,
	    APPLY &&apply)
	{
		SpirvShader::GenericValue value(shader, state, insn.word(5));
		auto &type = shader->getType(SpirvShader::Type::ID(insn.word(1)));
		for(auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto mask = As<SIMD::UInt>(state->activeLaneMask());
			auto identity = TYPE(identityValue);
			SIMD::UInt v_uint = (value.UInt(i) & mask) | (As<SIMD::UInt>(identity) & ~mask);
			TYPE v = As<TYPE>(v_uint);
			switch(spv::GroupOperation(insn.word(4)))
			{
				case spv::GroupOperationReduce:
				{
					// NOTE: floating-point add and multiply are not really commutative so
					//       ensure that all values in the final lanes are identical
					TYPE v2 = apply(v.xxzz, v.yyww);    // [xy]   [xy]   [zw]   [zw]
					TYPE v3 = apply(v2.xxxx, v2.zzzz);  // [xyzw] [xyzw] [xyzw] [xyzw]
					dst.move(i, v3);
					break;
				}
				case spv::GroupOperationInclusiveScan:
				{
					TYPE v2 = apply(v, Shuffle(v, identity, 0x4012) /* [id, v.y, v.z, v.w] */);      // [x] [xy] [yz]  [zw]
					TYPE v3 = apply(v2, Shuffle(v2, identity, 0x4401) /* [id,  id, v2.x, v2.y] */);  // [x] [xy] [xyz] [xyzw]
					dst.move(i, v3);
					break;
				}
				case spv::GroupOperationExclusiveScan:
				{
					TYPE v2 = apply(v, Shuffle(v, identity, 0x4012) /* [id, v.y, v.z, v.w] */);      // [x] [xy] [yz]  [zw]
					TYPE v3 = apply(v2, Shuffle(v2, identity, 0x4401) /* [id,  id, v2.x, v2.y] */);  // [x] [xy] [xyz] [xyzw]
					auto v4 = Shuffle(v3, identity, 0x4012 /* [id, v3.x, v3.y, v3.z] */);            // [i] [x]  [xy]  [xyz]
					dst.move(i, v4);
					break;
				}
				default:
					UNSUPPORTED("EmitGroupNonUniform op: %s Group operation: %d",
					            SpirvShader::OpcodeName(type.opcode()).c_str(), insn.word(4));
			}
		}
	}
};

SpirvShader::EmitResult SpirvShader::EmitGroupNonUniform(InsnIterator insn, EmitState *state) const
{
	static_assert(SIMD::Width == 4, "EmitGroupNonUniform makes many assumptions that the SIMD vector width is 4");

	auto &type = getType(Type::ID(insn.word(1)));
	Object::ID resultId = insn.word(2);
	auto scope = spv::Scope(GetConstScalarInt(insn.word(3)));
	ASSERT_MSG(scope == spv::ScopeSubgroup, "Scope for Non Uniform Group Operations must be Subgroup for Vulkan 1.1");

	auto &dst = state->createIntermediate(resultId, type.sizeInComponents);

	switch(insn.opcode())
	{
		case spv::OpGroupNonUniformElect:
		{
			// Result is true only in the active invocation with the lowest id
			// in the group, otherwise result is false.
			SIMD::Int active = state->activeLaneMask();
			// TODO: Would be nice if we could write this as:
			//   elect = active & ~(active.Oxyz | active.OOxy | active.OOOx)
			auto v0111 = SIMD::Int(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
			auto elect = active & ~(v0111 & (active.xxyz | active.xxxy | active.xxxx));
			dst.move(0, elect);
			break;
		}

		case spv::OpGroupNonUniformAll:
		{
			GenericValue predicate(this, state, insn.word(4));
			dst.move(0, AndAll(predicate.UInt(0) | ~As<SIMD::UInt>(state->activeLaneMask())));
			break;
		}

		case spv::OpGroupNonUniformAny:
		{
			GenericValue predicate(this, state, insn.word(4));
			dst.move(0, OrAll(predicate.UInt(0) & As<SIMD::UInt>(state->activeLaneMask())));
			break;
		}

		case spv::OpGroupNonUniformAllEqual:
		{
			GenericValue value(this, state, insn.word(4));
			auto res = SIMD::UInt(0xffffffff);
			SIMD::UInt active = As<SIMD::UInt>(state->activeLaneMask());
			SIMD::UInt inactive = ~active;
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				SIMD::UInt v = value.UInt(i) & active;
				SIMD::UInt filled = v;
				for(int j = 0; j < SIMD::Width - 1; j++)
				{
					filled |= filled.yzwx & inactive;  // Populate inactive 'holes' with a live value
				}
				res &= AndAll(CmpEQ(filled.xyzw, filled.yzwx));
			}
			dst.move(0, res);
			break;
		}

		case spv::OpGroupNonUniformBroadcast:
		{
			auto valueId = Object::ID(insn.word(4));
			auto id = SIMD::Int(GetConstScalarInt(insn.word(5)));
			GenericValue value(this, state, valueId);
			auto mask = CmpEQ(id, SIMD::Int(0, 1, 2, 3));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.move(i, OrAll(value.Int(i) & mask));
			}
			break;
		}

		case spv::OpGroupNonUniformBroadcastFirst:
		{
			auto valueId = Object::ID(insn.word(4));
			GenericValue value(this, state, valueId);
			// Result is true only in the active invocation with the lowest id
			// in the group, otherwise result is false.
			SIMD::Int active = state->activeLaneMask();
			// TODO: Would be nice if we could write this as:
			//   elect = active & ~(active.Oxyz | active.OOxy | active.OOOx)
			auto v0111 = SIMD::Int(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
			auto elect = active & ~(v0111 & (active.xxyz | active.xxxy | active.xxxx));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.move(i, OrAll(value.Int(i) & elect));
			}
			break;
		}

		case spv::OpGroupNonUniformBallot:
		{
			ASSERT(type.sizeInComponents == 4);
			GenericValue predicate(this, state, insn.word(4));
			dst.move(0, SIMD::Int(SignMask(state->activeLaneMask() & predicate.Int(0))));
			dst.move(1, SIMD::Int(0));
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(0));
			break;
		}

		case spv::OpGroupNonUniformInverseBallot:
		{
			auto valueId = Object::ID(insn.word(4));
			ASSERT(type.sizeInComponents == 1);
			ASSERT(getType(getObject(valueId).type).sizeInComponents == 4);
			GenericValue value(this, state, valueId);
			auto bit = (value.Int(0) >> SIMD::Int(0, 1, 2, 3)) & SIMD::Int(1);
			dst.move(0, -bit);
			break;
		}

		case spv::OpGroupNonUniformBallotBitExtract:
		{
			auto valueId = Object::ID(insn.word(4));
			auto indexId = Object::ID(insn.word(5));
			ASSERT(type.sizeInComponents == 1);
			ASSERT(getType(getObject(valueId).type).sizeInComponents == 4);
			ASSERT(getType(getObject(indexId).type).sizeInComponents == 1);
			GenericValue value(this, state, valueId);
			GenericValue index(this, state, indexId);
			auto vecIdx = index.Int(0) / SIMD::Int(32);
			auto bitIdx = index.Int(0) & SIMD::Int(31);
			auto bits = (value.Int(0) & CmpEQ(vecIdx, SIMD::Int(0))) |
			            (value.Int(1) & CmpEQ(vecIdx, SIMD::Int(1))) |
			            (value.Int(2) & CmpEQ(vecIdx, SIMD::Int(2))) |
			            (value.Int(3) & CmpEQ(vecIdx, SIMD::Int(3)));
			dst.move(0, -((bits >> bitIdx) & SIMD::Int(1)));
			break;
		}

		case spv::OpGroupNonUniformBallotBitCount:
		{
			auto operation = spv::GroupOperation(insn.word(4));
			auto valueId = Object::ID(insn.word(5));
			ASSERT(type.sizeInComponents == 1);
			ASSERT(getType(getObject(valueId).type).sizeInComponents == 4);
			GenericValue value(this, state, valueId);
			switch(operation)
			{
				case spv::GroupOperationReduce:
					dst.move(0, CountBits(value.UInt(0) & SIMD::UInt(15)));
					break;
				case spv::GroupOperationInclusiveScan:
					dst.move(0, CountBits(value.UInt(0) & SIMD::UInt(1, 3, 7, 15)));
					break;
				case spv::GroupOperationExclusiveScan:
					dst.move(0, CountBits(value.UInt(0) & SIMD::UInt(0, 1, 3, 7)));
					break;
				default:
					UNSUPPORTED("GroupOperation %d", int(operation));
			}
			break;
		}

		case spv::OpGroupNonUniformBallotFindLSB:
		{
			auto valueId = Object::ID(insn.word(4));
			ASSERT(type.sizeInComponents == 1);
			ASSERT(getType(getObject(valueId).type).sizeInComponents == 4);
			GenericValue value(this, state, valueId);
			dst.move(0, Cttz(value.UInt(0) & SIMD::UInt(15), true));
			break;
		}

		case spv::OpGroupNonUniformBallotFindMSB:
		{
			auto valueId = Object::ID(insn.word(4));
			ASSERT(type.sizeInComponents == 1);
			ASSERT(getType(getObject(valueId).type).sizeInComponents == 4);
			GenericValue value(this, state, valueId);
			dst.move(0, SIMD::UInt(31) - Ctlz(value.UInt(0) & SIMD::UInt(15), false));
			break;
		}

		case spv::OpGroupNonUniformShuffle:
		{
			GenericValue value(this, state, insn.word(4));
			GenericValue id(this, state, insn.word(5));
			auto x = CmpEQ(SIMD::Int(0), id.Int(0));
			auto y = CmpEQ(SIMD::Int(1), id.Int(0));
			auto z = CmpEQ(SIMD::Int(2), id.Int(0));
			auto w = CmpEQ(SIMD::Int(3), id.Int(0));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				SIMD::Int v = value.Int(i);
				dst.move(i, (x & v.xxxx) | (y & v.yyyy) | (z & v.zzzz) | (w & v.wwww));
			}
			break;
		}

		case spv::OpGroupNonUniformShuffleXor:
		{
			GenericValue value(this, state, insn.word(4));
			GenericValue mask(this, state, insn.word(5));
			auto x = CmpEQ(SIMD::Int(0), SIMD::Int(0, 1, 2, 3) ^ mask.Int(0));
			auto y = CmpEQ(SIMD::Int(1), SIMD::Int(0, 1, 2, 3) ^ mask.Int(0));
			auto z = CmpEQ(SIMD::Int(2), SIMD::Int(0, 1, 2, 3) ^ mask.Int(0));
			auto w = CmpEQ(SIMD::Int(3), SIMD::Int(0, 1, 2, 3) ^ mask.Int(0));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				SIMD::Int v = value.Int(i);
				dst.move(i, (x & v.xxxx) | (y & v.yyyy) | (z & v.zzzz) | (w & v.wwww));
			}
			break;
		}

		case spv::OpGroupNonUniformShuffleUp:
		{
			GenericValue value(this, state, insn.word(4));
			GenericValue delta(this, state, insn.word(5));
			auto d0 = CmpEQ(SIMD::Int(0), delta.Int(0));
			auto d1 = CmpEQ(SIMD::Int(1), delta.Int(0));
			auto d2 = CmpEQ(SIMD::Int(2), delta.Int(0));
			auto d3 = CmpEQ(SIMD::Int(3), delta.Int(0));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				SIMD::Int v = value.Int(i);
				dst.move(i, (d0 & v.xyzw) | (d1 & v.xxyz) | (d2 & v.xxxy) | (d3 & v.xxxx));
			}
			break;
		}

		case spv::OpGroupNonUniformShuffleDown:
		{
			GenericValue value(this, state, insn.word(4));
			GenericValue delta(this, state, insn.word(5));
			auto d0 = CmpEQ(SIMD::Int(0), delta.Int(0));
			auto d1 = CmpEQ(SIMD::Int(1), delta.Int(0));
			auto d2 = CmpEQ(SIMD::Int(2), delta.Int(0));
			auto d3 = CmpEQ(SIMD::Int(3), delta.Int(0));
			for(auto i = 0u; i < type.sizeInComponents; i++)
			{
				SIMD::Int v = value.Int(i);
				dst.move(i, (d0 & v.xyzw) | (d1 & v.yzww) | (d2 & v.zwww) | (d3 & v.wwww));
			}
			break;
		}

		case spv::OpGroupNonUniformIAdd:
			Impl::Group::BinaryOperation<SIMD::Int>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) { return a + b; });
			break;

		case spv::OpGroupNonUniformFAdd:
			Impl::Group::BinaryOperation<SIMD::Float>(
			    this, insn, state, dst, 0.0f,
			    [](auto a, auto b) { return a + b; });
			break;

		case spv::OpGroupNonUniformIMul:
			Impl::Group::BinaryOperation<SIMD::Int>(
			    this, insn, state, dst, 1,
			    [](auto a, auto b) { return a * b; });
			break;

		case spv::OpGroupNonUniformFMul:
			Impl::Group::BinaryOperation<SIMD::Float>(
			    this, insn, state, dst, 1.0f,
			    [](auto a, auto b) { return a * b; });
			break;

		case spv::OpGroupNonUniformBitwiseAnd:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, ~0u,
			    [](auto a, auto b) { return a & b; });
			break;

		case spv::OpGroupNonUniformBitwiseOr:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) { return a | b; });
			break;

		case spv::OpGroupNonUniformBitwiseXor:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) { return a ^ b; });
			break;

		case spv::OpGroupNonUniformSMin:
			Impl::Group::BinaryOperation<SIMD::Int>(
			    this, insn, state, dst, INT32_MAX,
			    [](auto a, auto b) { return Min(a, b); });
			break;

		case spv::OpGroupNonUniformUMin:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, ~0u,
			    [](auto a, auto b) { return Min(a, b); });
			break;

		case spv::OpGroupNonUniformFMin:
			Impl::Group::BinaryOperation<SIMD::Float>(
			    this, insn, state, dst, SIMD::Float::infinity(),
			    [](auto a, auto b) { return NMin(a, b); });
			break;

		case spv::OpGroupNonUniformSMax:
			Impl::Group::BinaryOperation<SIMD::Int>(
			    this, insn, state, dst, INT32_MIN,
			    [](auto a, auto b) { return Max(a, b); });
			break;

		case spv::OpGroupNonUniformUMax:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) { return Max(a, b); });
			break;

		case spv::OpGroupNonUniformFMax:
			Impl::Group::BinaryOperation<SIMD::Float>(
			    this, insn, state, dst, -SIMD::Float::infinity(),
			    [](auto a, auto b) { return NMax(a, b); });
			break;

		case spv::OpGroupNonUniformLogicalAnd:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, ~0u,
			    [](auto a, auto b) {
				    SIMD::UInt zero = SIMD::UInt(0);
				    return CmpNEQ(a, zero) & CmpNEQ(b, zero);
			    });
			break;

		case spv::OpGroupNonUniformLogicalOr:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) {
				    SIMD::UInt zero = SIMD::UInt(0);
				    return CmpNEQ(a, zero) | CmpNEQ(b, zero);
			    });
			break;

		case spv::OpGroupNonUniformLogicalXor:
			Impl::Group::BinaryOperation<SIMD::UInt>(
			    this, insn, state, dst, 0,
			    [](auto a, auto b) {
				    SIMD::UInt zero = SIMD::UInt(0);
				    return CmpNEQ(a, zero) ^ CmpNEQ(b, zero);
			    });
			break;

		default:
			UNSUPPORTED("EmitGroupNonUniform op: %s", OpcodeName(type.opcode()).c_str());
	}
	return EmitResult::Continue;
}

}  // namespace sw
