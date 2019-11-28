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

SpirvShader::EmitResult SpirvShader::EmitGroupNonUniform(InsnIterator insn, EmitState *state) const
{
	static_assert(SIMD::Width == 4, "EmitGroupNonUniform makes many assumptions that the SIMD vector width is 4");

	auto &type = getType(Type::ID(insn.word(1)));
	Object::ID resultId = insn.word(2);
	auto scope = spv::Scope(GetConstScalarInt(insn.word(3)));
	ASSERT_MSG(scope == spv::ScopeSubgroup, "Scope for Non Uniform Group Operations must be Subgroup for Vulkan 1.1");

	auto &dst = state->createIntermediate(resultId, type.sizeInComponents);

	switch (insn.opcode())
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::UInt v = value.UInt(i) & active;
			SIMD::UInt filled = v;
			for (int j = 0; j < SIMD::Width - 1; j++)
			{
				filled |= filled.yzwx & inactive; // Populate inactive 'holes' with a live value
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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
		auto bits =	(value.Int(0) & CmpEQ(vecIdx, SIMD::Int(0))) |
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
		switch (operation)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::Int v = value.Int(i);
			dst.move(i, (d0 & v.xyzw) | (d1 & v.yzww) | (d2 & v.zwww) | (d3 & v.wwww));
		}
		break;
	}

	default:
		UNIMPLEMENTED("EmitGroupNonUniform op: %s", OpcodeName(type.opcode()).c_str());
	}
	return EmitResult::Continue;
}

}  // namespace sw