// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#include <spirv/unified1/spirv.hpp>
#include "SpirvShader.hpp"
#include "System/Math.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkPipelineLayout.hpp"
#include "Device/Config.hpp"

namespace sw
{
	volatile int SpirvShader::serialCounter = 1;    // Start at 1, 0 is invalid shader.

	SpirvShader::SpirvShader(InsnStore const &insns)
			: insns{insns}, inputs{MAX_INTERFACE_COMPONENTS},
			  outputs{MAX_INTERFACE_COMPONENTS},
			  serialID{serialCounter++}, modes{}
	{
		ASSERT(insns.size() > 0);

		// Simplifying assumptions (to be satisfied by earlier transformations)
		// - There is exactly one entrypoint in the module, and it's the one we want
		// - The only input/output OpVariables present are those used by the entrypoint

		// TODO: Add real support for control flow. For now, track whether we've seen
		// a label or a return already (if so, the shader does things we will mishandle).
		// We expect there to be one of each in a simple shader -- the first and last instruction
		// of the entrypoint function.
		bool seenLabel = false;
		bool seenReturn = false;

		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpExecutionMode:
				ProcessExecutionMode(insn);
				break;

			case spv::OpDecorate:
			{
				TypeOrObjectID targetId = insn.word(1);
				auto decoration = static_cast<spv::Decoration>(insn.word(2));
				decorations[targetId].Apply(
						decoration,
						insn.wordCount() > 3 ? insn.word(3) : 0);

				if (decoration == spv::DecorationCentroid)
					modes.NeedsCentroid = true;
				break;
			}

			case spv::OpMemberDecorate:
			{
				TypeID targetId = insn.word(1);
				auto memberIndex = insn.word(2);
				auto &d = memberDecorations[targetId];
				if (memberIndex >= d.size())
					d.resize(memberIndex + 1);    // on demand; exact size would require another pass...
				auto decoration = static_cast<spv::Decoration>(insn.word(3));
				d[memberIndex].Apply(
						decoration,
						insn.wordCount() > 4 ? insn.word(4) : 0);

				if (decoration == spv::DecorationCentroid)
					modes.NeedsCentroid = true;
				break;
			}

			case spv::OpDecorationGroup:
				// Nothing to do here. We don't need to record the definition of the group; we'll just have
				// the bundle of decorations float around. If we were to ever walk the decorations directly,
				// we might think about introducing this as a real Object.
				break;

			case spv::OpGroupDecorate:
			{
				auto const &srcDecorations = decorations[insn.word(1)];
				for (auto i = 2u; i < insn.wordCount(); i++)
				{
					// remaining operands are targets to apply the group to.
					decorations[insn.word(i)].Apply(srcDecorations);
				}
				break;
			}

			case spv::OpGroupMemberDecorate:
			{
				auto const &srcDecorations = decorations[insn.word(1)];
				for (auto i = 2u; i < insn.wordCount(); i += 2)
				{
					// remaining operands are pairs of <id>, literal for members to apply to.
					auto &d = memberDecorations[insn.word(i)];
					auto memberIndex = insn.word(i + 1);
					if (memberIndex >= d.size())
						d.resize(memberIndex + 1);    // on demand resize, see above...
					d[memberIndex].Apply(srcDecorations);
				}
				break;
			}

			case spv::OpLabel:
				if (seenLabel)
					UNIMPLEMENTED("Shader contains multiple labels, has control flow");
				seenLabel = true;
				break;

			case spv::OpReturn:
				if (seenReturn)
					UNIMPLEMENTED("Shader contains multiple returns, has control flow");
				seenReturn = true;
				break;

			case spv::OpTypeVoid:
			case spv::OpTypeBool:
			case spv::OpTypeInt:
			case spv::OpTypeFloat:
			case spv::OpTypeVector:
			case spv::OpTypeMatrix:
			case spv::OpTypeImage:
			case spv::OpTypeSampler:
			case spv::OpTypeSampledImage:
			case spv::OpTypeArray:
			case spv::OpTypeRuntimeArray:
			case spv::OpTypeStruct:
			case spv::OpTypePointer:
			case spv::OpTypeFunction:
				DeclareType(insn);
				break;

			case spv::OpVariable:
			{
				TypeID typeId = insn.word(1);
				ObjectID resultId = insn.word(2);
				auto storageClass = static_cast<spv::StorageClass>(insn.word(3));
				if (insn.wordCount() > 4)
					UNIMPLEMENTED("Variable initializers not yet supported");

				auto &object = defs[resultId];
				object.kind = Object::Kind::Variable;
				object.definition = insn;
				object.type = typeId;
				object.pointerBase = insn.word(2);	// base is itself

				// Register builtins
				if (storageClass == spv::StorageClassInput || storageClass == spv::StorageClassOutput)
				{
					ProcessInterfaceVariable(object);
				}
				break;
			}

			case spv::OpConstant:
				CreateConstant(insn).constantValue[0] = insn.word(3);
				break;
			case spv::OpConstantFalse:
				CreateConstant(insn).constantValue[0] = 0;		// represent boolean false as zero
				break;
			case spv::OpConstantTrue:
				CreateConstant(insn).constantValue[0] = ~0u;	// represent boolean true as all bits set
				break;
			case spv::OpConstantNull:
			{
				// OpConstantNull forms a constant of arbitrary type, all zeros.
				auto &object = CreateConstant(insn);
				auto &objectTy = getType(object.type);
				for (auto i = 0u; i < objectTy.sizeInComponents; i++)
				{
					object.constantValue[i] = 0;
				}
				break;
			}
			case spv::OpConstantComposite:
			{
				auto &object = CreateConstant(insn);
				auto offset = 0u;
				for (auto i = 0u; i < insn.wordCount() - 3; i++)
				{
					auto &constituent = getObject(insn.word(i + 3));
					auto &constituentTy = getType(constituent.type);
					for (auto j = 0u; j < constituentTy.sizeInComponents; j++)
						object.constantValue[offset++] = constituent.constantValue[j];
				}
				break;
			}

			case spv::OpCapability:
				// Various capabilities will be declared, but none affect our code generation at this point.
			case spv::OpMemoryModel:
				// Memory model does not affect our code generation until we decide to do Vulkan Memory Model support.
			case spv::OpEntryPoint:
			case spv::OpFunction:
			case spv::OpFunctionEnd:
				// Due to preprocessing, the entrypoint and its function provide no value.
				break;
			case spv::OpExtInstImport:
				// We will only support the GLSL 450 extended instruction set, so no point in tracking the ID we assign it.
				// Valid shaders will not attempt to import any other instruction sets.
			case spv::OpName:
			case spv::OpMemberName:
			case spv::OpSource:
			case spv::OpSourceContinued:
			case spv::OpSourceExtension:
				// No semantic impact
				break;

			case spv::OpFunctionParameter:
			case spv::OpFunctionCall:
			case spv::OpSpecConstant:
			case spv::OpSpecConstantComposite:
			case spv::OpSpecConstantFalse:
			case spv::OpSpecConstantOp:
			case spv::OpSpecConstantTrue:
				// These should have all been removed by preprocessing passes. If we see them here,
				// our assumptions are wrong and we will probably generate wrong code.
				UNIMPLEMENTED("These instructions should have already been lowered.");
				break;

			case spv::OpFConvert:
			case spv::OpSConvert:
			case spv::OpUConvert:
				UNIMPLEMENTED("No valid uses for Op*Convert until we support multiple bit widths");
				break;

			case spv::OpLoad:
			case spv::OpAccessChain:
			case spv::OpCompositeConstruct:
			case spv::OpCompositeInsert:
			case spv::OpCompositeExtract:
			case spv::OpVectorShuffle:
			case spv::OpNot: // Unary ops
			case spv::OpSNegate:
			case spv::OpFNegate:
			case spv::OpLogicalNot:
			case spv::OpIAdd: // Binary ops
			case spv::OpISub:
			case spv::OpIMul:
			case spv::OpSDiv:
			case spv::OpUDiv:
			case spv::OpFAdd:
			case spv::OpFSub:
			case spv::OpFDiv:
			case spv::OpFOrdEqual:
			case spv::OpFUnordEqual:
			case spv::OpFOrdNotEqual:
			case spv::OpFUnordNotEqual:
			case spv::OpFOrdLessThan:
			case spv::OpFUnordLessThan:
			case spv::OpFOrdGreaterThan:
			case spv::OpFUnordGreaterThan:
			case spv::OpFOrdLessThanEqual:
			case spv::OpFUnordLessThanEqual:
			case spv::OpFOrdGreaterThanEqual:
			case spv::OpFUnordGreaterThanEqual:
			case spv::OpUMod:
			case spv::OpIEqual:
			case spv::OpINotEqual:
			case spv::OpUGreaterThan:
			case spv::OpSGreaterThan:
			case spv::OpUGreaterThanEqual:
			case spv::OpSGreaterThanEqual:
			case spv::OpULessThan:
			case spv::OpSLessThan:
			case spv::OpULessThanEqual:
			case spv::OpSLessThanEqual:
			case spv::OpShiftRightLogical:
			case spv::OpShiftRightArithmetic:
			case spv::OpShiftLeftLogical:
			case spv::OpBitwiseOr:
			case spv::OpBitwiseXor:
			case spv::OpBitwiseAnd:
			case spv::OpLogicalOr:
			case spv::OpLogicalAnd:
			case spv::OpUMulExtended:
			case spv::OpSMulExtended:
			case spv::OpDot:
			case spv::OpConvertFToU:
			case spv::OpConvertFToS:
			case spv::OpConvertSToF:
			case spv::OpConvertUToF:
			case spv::OpBitcast:
			case spv::OpSelect:
				// Instructions that yield an intermediate value
			{
				TypeID typeId = insn.word(1);
				ObjectID resultId = insn.word(2);
				auto &object = defs[resultId];
				object.type = typeId;
				object.kind = Object::Kind::Value;
				object.definition = insn;

				if (insn.opcode() == spv::OpAccessChain)
				{
					// interior ptr has two parts:
					// - logical base ptr, common across all lanes and known at compile time
					// - per-lane offset
					ObjectID baseId = insn.word(3);
					object.pointerBase = getObject(baseId).pointerBase;
				}
				break;
			}

			case spv::OpStore:
				// Don't need to do anything during analysis pass
				break;

			case spv::OpKill:
				modes.ContainsKill = true;
				break;

			default:
				UNIMPLEMENTED(OpcodeName(insn.opcode()).c_str());
			}
		}
	}

	void SpirvShader::DeclareType(InsnIterator insn)
	{
		TypeID resultId = insn.word(1);

		auto &type = types[resultId];
		type.definition = insn;
		type.sizeInComponents = ComputeTypeSize(insn);

		// A structure is a builtin block if it has a builtin
		// member. All members of such a structure are builtins.
		switch (insn.opcode())
		{
		case spv::OpTypeStruct:
		{
			auto d = memberDecorations.find(resultId);
			if (d != memberDecorations.end())
			{
				for (auto &m : d->second)
				{
					if (m.HasBuiltIn)
					{
						type.isBuiltInBlock = true;
						break;
					}
				}
			}
			break;
		}
		case spv::OpTypePointer:
		{
			TypeID elementTypeId = insn.word(3);
			type.element = elementTypeId;
			type.isBuiltInBlock = getType(elementTypeId).isBuiltInBlock;
			type.storageClass = static_cast<spv::StorageClass>(insn.word(2));
			break;
		}
		case spv::OpTypeVector:
		case spv::OpTypeMatrix:
		case spv::OpTypeArray:
		case spv::OpTypeRuntimeArray:
		{
			TypeID elementTypeId = insn.word(2);
			type.element = elementTypeId;
			break;
		}
		default:
			break;
		}
	}

	SpirvShader::Object& SpirvShader::CreateConstant(InsnIterator insn)
	{
		TypeID typeId = insn.word(1);
		ObjectID resultId = insn.word(2);
		auto &object = defs[resultId];
		auto &objectTy = getType(typeId);
		object.type = typeId;
		object.kind = Object::Kind::Constant;
		object.definition = insn;
		object.constantValue = std::unique_ptr<uint32_t[]>(new uint32_t[objectTy.sizeInComponents]);
		return object;
	}

	void SpirvShader::ProcessInterfaceVariable(Object &object)
	{
		auto &objectTy = getType(object.type);
		ASSERT(objectTy.storageClass == spv::StorageClassInput || objectTy.storageClass == spv::StorageClassOutput);

		ASSERT(objectTy.definition.opcode() == spv::OpTypePointer);
		auto pointeeTy = getType(objectTy.element);

		auto &builtinInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputBuiltins : outputBuiltins;
		auto &userDefinedInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputs : outputs;

		ASSERT(object.definition.opcode() == spv::OpVariable);
		ObjectID resultId = object.definition.word(2);

		if (objectTy.isBuiltInBlock)
		{
			// walk the builtin block, registering each of its members separately.
			auto m = memberDecorations.find(objectTy.element);
			ASSERT(m != memberDecorations.end());        // otherwise we wouldn't have marked the type chain
			auto &structType = pointeeTy.definition;
			auto offset = 0u;
			auto word = 2u;
			for (auto &member : m->second)
			{
				auto &memberType = getType(structType.word(word));

				if (member.HasBuiltIn)
				{
					builtinInterface[member.BuiltIn] = {resultId, offset, memberType.sizeInComponents};
				}

				offset += memberType.sizeInComponents;
				++word;
			}
			return;
		}

		auto d = decorations.find(resultId);
		if (d != decorations.end() && d->second.HasBuiltIn)
		{
			builtinInterface[d->second.BuiltIn] = {resultId, 0, pointeeTy.sizeInComponents};
		}
		else
		{
			object.kind = Object::Kind::InterfaceVariable;
			VisitInterface(resultId,
						   [&userDefinedInterface](Decorations const &d, AttribType type) {
							   // Populate a single scalar slot in the interface from a collection of decorations and the intended component type.
							   auto scalarSlot = (d.Location << 2) | d.Component;
							   ASSERT(scalarSlot >= 0 &&
									  scalarSlot < static_cast<int32_t>(userDefinedInterface.size()));

							   auto &slot = userDefinedInterface[scalarSlot];
							   slot.Type = type;
							   slot.Flat = d.Flat;
							   slot.NoPerspective = d.NoPerspective;
							   slot.Centroid = d.Centroid;
						   });
		}
	}

	void SpirvShader::ProcessExecutionMode(InsnIterator insn)
	{
		auto mode = static_cast<spv::ExecutionMode>(insn.word(2));
		switch (mode)
		{
		case spv::ExecutionModeEarlyFragmentTests:
			modes.EarlyFragmentTests = true;
			break;
		case spv::ExecutionModeDepthReplacing:
			modes.DepthReplacing = true;
			break;
		case spv::ExecutionModeDepthGreater:
			modes.DepthGreater = true;
			break;
		case spv::ExecutionModeDepthLess:
			modes.DepthLess = true;
			break;
		case spv::ExecutionModeDepthUnchanged:
			modes.DepthUnchanged = true;
			break;
		case spv::ExecutionModeLocalSize:
			modes.LocalSizeX = insn.word(3);
			modes.LocalSizeZ = insn.word(5);
			modes.LocalSizeY = insn.word(4);
			break;
		case spv::ExecutionModeOriginUpperLeft:
			// This is always the case for a Vulkan shader. Do nothing.
			break;
		default:
			UNIMPLEMENTED("No other execution modes are permitted");
		}
	}

	uint32_t SpirvShader::ComputeTypeSize(sw::SpirvShader::InsnIterator insn)
	{
		// Types are always built from the bottom up (with the exception of forward ptrs, which
		// don't appear in Vulkan shaders. Therefore, we can always assume our component parts have
		// already been described (and so their sizes determined)
		switch (insn.opcode())
		{
		case spv::OpTypeVoid:
		case spv::OpTypeSampler:
		case spv::OpTypeImage:
		case spv::OpTypeSampledImage:
		case spv::OpTypeFunction:
		case spv::OpTypeRuntimeArray:
			// Objects that don't consume any space.
			// Descriptor-backed objects currently only need exist at compile-time.
			// Runtime arrays don't appear in places where their size would be interesting
			return 0;

		case spv::OpTypeBool:
		case spv::OpTypeFloat:
		case spv::OpTypeInt:
			// All the fundamental types are 1 component. If we ever add support for 8/16/64-bit components,
			// we might need to change this, but only 32 bit components are required for Vulkan 1.1.
			return 1;

		case spv::OpTypeVector:
		case spv::OpTypeMatrix:
			// Vectors and matrices both consume element count * element size.
			return getType(insn.word(2)).sizeInComponents * insn.word(3);

		case spv::OpTypeArray:
		{
			// Element count * element size. Array sizes come from constant ids.
			auto arraySize = GetConstantInt(insn.word(3));
			return getType(insn.word(2)).sizeInComponents * arraySize;
		}

		case spv::OpTypeStruct:
		{
			uint32_t size = 0;
			for (uint32_t i = 2u; i < insn.wordCount(); i++)
			{
				size += getType(insn.word(i)).sizeInComponents;
			}
			return size;
		}

		case spv::OpTypePointer:
			// Runtime representation of a pointer is a per-lane index.
			// Note: clients are expected to look through the pointer if they want the pointee size instead.
			return 1;

		default:
			// Some other random insn.
			UNIMPLEMENTED("Only types are supported");
			return 0;
		}
	}

	template<typename F>
	int SpirvShader::VisitInterfaceInner(TypeID id, Decorations d, F f) const
	{
		// Recursively walks variable definition and its type tree, taking into account
		// any explicit Location or Component decorations encountered; where explicit
		// Locations or Components are not specified, assigns them sequentially.
		// Collected decorations are carried down toward the leaves and across
		// siblings; Effect of decorations intentionally does not flow back up the tree.
		//
		// F is a functor to be called with the effective decoration set for every component.
		//
		// Returns the next available location, and calls f().

		// This covers the rules in Vulkan 1.1 spec, 14.1.4 Location Assignment.

		ApplyDecorationsForId(&d, id);

		auto const &obj = getType(id);
		switch (obj.definition.opcode())
		{
		case spv::OpTypePointer:
			return VisitInterfaceInner<F>(obj.definition.word(3), d, f);
		case spv::OpTypeMatrix:
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Location++)
			{
				// consumes same components of N consecutive locations
				VisitInterfaceInner<F>(obj.definition.word(2), d, f);
			}
			return d.Location;
		case spv::OpTypeVector:
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Component++)
			{
				// consumes N consecutive components in the same location
				VisitInterfaceInner<F>(obj.definition.word(2), d, f);
			}
			return d.Location + 1;
		case spv::OpTypeFloat:
			f(d, ATTRIBTYPE_FLOAT);
			return d.Location + 1;
		case spv::OpTypeInt:
			f(d, obj.definition.word(3) ? ATTRIBTYPE_INT : ATTRIBTYPE_UINT);
			return d.Location + 1;
		case spv::OpTypeBool:
			f(d, ATTRIBTYPE_UINT);
			return d.Location + 1;
		case spv::OpTypeStruct:
		{
			// iterate over members, which may themselves have Location/Component decorations
			for (auto i = 0u; i < obj.definition.wordCount() - 2; i++)
			{
				ApplyDecorationsForIdMember(&d, id, i);
				d.Location = VisitInterfaceInner<F>(obj.definition.word(i + 2), d, f);
				d.Component = 0;    // Implicit locations always have component=0
			}
			return d.Location;
		}
		case spv::OpTypeArray:
		{
			auto arraySize = GetConstantInt(obj.definition.word(3));
			for (auto i = 0u; i < arraySize; i++)
			{
				d.Location = VisitInterfaceInner<F>(obj.definition.word(2), d, f);
			}
			return d.Location;
		}
		default:
			// Intentionally partial; most opcodes do not participate in type hierarchies
			return 0;
		}
	}

	template<typename F>
	void SpirvShader::VisitInterface(ObjectID id, F f) const
	{
		// Walk a variable definition and call f for each component in it.
		Decorations d{};
		ApplyDecorationsForId(&d, id);

		auto def = getObject(id).definition;
		ASSERT(def.opcode() == spv::OpVariable);
		VisitInterfaceInner<F>(def.word(1), d, f);
	}

	SIMD::Int SpirvShader::WalkAccessChain(ObjectID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const
	{
		// TODO: think about explicit layout (UBO/SSBO) storage classes
		// TODO: avoid doing per-lane work in some cases if we can?

		int constantOffset = 0;
		SIMD::Int dynamicOffset = SIMD::Int(0);
		auto &baseObject = getObject(id);
		TypeID typeId = getType(baseObject.type).element;

		// The <base> operand is an intermediate value itself, ie produced by a previous OpAccessChain.
		// Start with its offset and build from there.
		if (baseObject.kind == Object::Kind::Value)
			dynamicOffset += As<SIMD::Int>(routine->getIntermediate(id)[0]);

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			switch (type.definition.opcode())
			{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstantInt(indexIds[i]);
				int offsetIntoStruct = 0;
				for (auto j = 0; j < memberIndex; j++) {
					auto memberType = type.definition.word(2u + j);
					offsetIntoStruct += getType(memberType).sizeInComponents;
				}
				constantOffset += offsetIntoStruct;
				typeId = type.definition.word(2u + memberIndex);
				break;
			}

			case spv::OpTypeVector:
			case spv::OpTypeMatrix:
			case spv::OpTypeArray:
			{
				auto stride = getType(type.element).sizeInComponents;
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					constantOffset += stride * GetConstantInt(indexIds[i]);
				else
					dynamicOffset += SIMD::Int(stride) * As<SIMD::Int>(routine->getIntermediate(indexIds[i])[0]);
				typeId = type.element;
				break;
			}

			default:
				UNIMPLEMENTED("Unexpected type '%s' in WalkAccessChain", OpcodeName(type.definition.opcode()).c_str());
			}
		}

		return dynamicOffset + SIMD::Int(constantOffset);
	}

	uint32_t SpirvShader::WalkLiteralAccessChain(TypeID typeId, uint32_t numIndexes, uint32_t const *indexes) const
	{
		uint32_t constantOffset = 0;

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			switch (type.definition.opcode())
			{
			case spv::OpTypeStruct:
			{
				int memberIndex = indexes[i];
				int offsetIntoStruct = 0;
				for (auto j = 0; j < memberIndex; j++) {
					auto memberType = type.definition.word(2u + j);
					offsetIntoStruct += getType(memberType).sizeInComponents;
				}
				constantOffset += offsetIntoStruct;
				typeId = type.definition.word(2u + memberIndex);
				break;
			}

			case spv::OpTypeVector:
			case spv::OpTypeMatrix:
			case spv::OpTypeArray:
			{
				auto elementType = type.definition.word(2);
				auto stride = getType(elementType).sizeInComponents;
				constantOffset += stride * indexes[i];
				typeId = elementType;
				break;
			}

			default:
				UNIMPLEMENTED("Unexpected type in WalkLiteralAccessChain");
			}
		}

		return constantOffset;
	}

	void SpirvShader::Decorations::Apply(spv::Decoration decoration, uint32_t arg)
	{
		switch (decoration)
		{
		case spv::DecorationLocation:
			HasLocation = true;
			Location = static_cast<int32_t>(arg);
			break;
		case spv::DecorationComponent:
			HasComponent = true;
			Component = arg;
			break;
		case spv::DecorationDescriptorSet:
			HasDescriptorSet = true;
			DescriptorSet = arg;
			break;
		case spv::DecorationBinding:
			HasBinding = true;
			Binding = arg;
			break;
		case spv::DecorationBuiltIn:
			HasBuiltIn = true;
			BuiltIn = static_cast<spv::BuiltIn>(arg);
			break;
		case spv::DecorationFlat:
			Flat = true;
			break;
		case spv::DecorationNoPerspective:
			NoPerspective = true;
			break;
		case spv::DecorationCentroid:
			Centroid = true;
			break;
		case spv::DecorationBlock:
			Block = true;
			break;
		case spv::DecorationBufferBlock:
			BufferBlock = true;
			break;
		default:
			// Intentionally partial, there are many decorations we just don't care about.
			break;
		}
	}

	void SpirvShader::Decorations::Apply(const sw::SpirvShader::Decorations &src)
	{
		// Apply a decoration group to this set of decorations
		if (src.HasBuiltIn)
		{
			HasBuiltIn = true;
			BuiltIn = src.BuiltIn;
		}

		if (src.HasLocation)
		{
			HasLocation = true;
			Location = src.Location;
		}

		if (src.HasComponent)
		{
			HasComponent = true;
			Component = src.Component;
		}

		if (src.HasDescriptorSet)
		{
			HasDescriptorSet = true;
			DescriptorSet = src.DescriptorSet;
		}

		if (src.HasBinding)
		{
			HasBinding = true;
			Binding = src.Binding;
		}

		Flat |= src.Flat;
		NoPerspective |= src.NoPerspective;
		Centroid |= src.Centroid;
		Block |= src.Block;
		BufferBlock |= src.BufferBlock;
	}

	void SpirvShader::ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const
	{
		auto it = decorations.find(id);
		if (it != decorations.end())
			d->Apply(it->second);
	}

	void SpirvShader::ApplyDecorationsForIdMember(Decorations *d, TypeID id, uint32_t member) const
	{
		auto it = memberDecorations.find(id);
		if (it != memberDecorations.end() && member < it->second.size())
		{
			d->Apply(it->second[member]);
		}
	}

	uint32_t SpirvShader::GetConstantInt(ObjectID id) const
	{
		// Slightly hackish access to constants very early in translation.
		// General consumption of constants by other instructions should
		// probably be just lowered to Reactor.

		// TODO: not encountered yet since we only use this for array sizes etc,
		// but is possible to construct integer constant 0 via OpConstantNull.
		auto insn = getObject(id).definition;
		ASSERT(insn.opcode() == spv::OpConstant);
		ASSERT(getType(insn.word(1)).definition.opcode() == spv::OpTypeInt);
		return insn.word(3);
	}

	// emit-time

	void SpirvShader::emitProlog(SpirvRoutine *routine) const
	{
		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpVariable:
			{
				ObjectID resultId = insn.word(2);
				auto &object = getObject(resultId);
				auto &objectTy = getType(object.type);
				auto &pointeeTy = getType(objectTy.element);
				// TODO: what to do about zero-slot objects?
				if (pointeeTy.sizeInComponents > 0)
				{
					routine->createLvalue(insn.word(2), pointeeTy.sizeInComponents);
				}
				break;
			}
			default:
				// Nothing else produces interface variables, so can all be safely ignored.
				break;
			}
		}
	}

	void SpirvShader::emit(SpirvRoutine *routine) const
	{
		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpTypeVoid:
			case spv::OpTypeInt:
			case spv::OpTypeFloat:
			case spv::OpTypeBool:
			case spv::OpTypeVector:
			case spv::OpTypeArray:
			case spv::OpTypeRuntimeArray:
			case spv::OpTypeMatrix:
			case spv::OpTypeStruct:
			case spv::OpTypePointer:
			case spv::OpTypeFunction:
			case spv::OpExecutionMode:
			case spv::OpMemoryModel:
			case spv::OpFunction:
			case spv::OpFunctionEnd:
			case spv::OpConstant:
			case spv::OpConstantNull:
			case spv::OpConstantTrue:
			case spv::OpConstantFalse:
			case spv::OpConstantComposite:
			case spv::OpExtension:
			case spv::OpCapability:
			case spv::OpEntryPoint:
			case spv::OpExtInstImport:
			case spv::OpDecorate:
			case spv::OpMemberDecorate:
			case spv::OpGroupDecorate:
			case spv::OpGroupMemberDecorate:
			case spv::OpDecorationGroup:
			case spv::OpName:
			case spv::OpMemberName:
			case spv::OpSource:
			case spv::OpSourceContinued:
			case spv::OpSourceExtension:
				// Nothing to do at emit time. These are either fully handled at analysis time,
				// or don't require any work at all.
				break;

			case spv::OpLabel:
			case spv::OpReturn:
				// TODO: when we do control flow, will need to do some work here.
				// Until then, there is nothing to do -- we expect there to be an initial OpLabel
				// in the entrypoint function, for which we do nothing; and a final OpReturn at the
				// end of the entrypoint function, for which we do nothing.
				break;

			case spv::OpVariable:
				EmitVariable(insn, routine);
				break;

			case spv::OpLoad:
				EmitLoad(insn, routine);
				break;

			case spv::OpStore:
				EmitStore(insn, routine);
				break;

			case spv::OpAccessChain:
				EmitAccessChain(insn, routine);
				break;

			case spv::OpCompositeConstruct:
				EmitCompositeConstruct(insn, routine);
				break;

			case spv::OpCompositeInsert:
				EmitCompositeInsert(insn, routine);
				break;

			case spv::OpCompositeExtract:
				EmitCompositeExtract(insn, routine);
				break;

			case spv::OpVectorShuffle:
				EmitVectorShuffle(insn, routine);
				break;

			case spv::OpNot:
			case spv::OpSNegate:
			case spv::OpFNegate:
			case spv::OpLogicalNot:
			case spv::OpConvertFToU:
			case spv::OpConvertFToS:
			case spv::OpConvertSToF:
			case spv::OpConvertUToF:
			case spv::OpBitcast:
				EmitUnaryOp(insn, routine);
				break;

			case spv::OpIAdd:
			case spv::OpISub:
			case spv::OpIMul:
			case spv::OpSDiv:
			case spv::OpUDiv:
			case spv::OpFAdd:
			case spv::OpFSub:
			case spv::OpFDiv:
			case spv::OpFOrdEqual:
			case spv::OpFUnordEqual:
			case spv::OpFOrdNotEqual:
			case spv::OpFUnordNotEqual:
			case spv::OpFOrdLessThan:
			case spv::OpFUnordLessThan:
			case spv::OpFOrdGreaterThan:
			case spv::OpFUnordGreaterThan:
			case spv::OpFOrdLessThanEqual:
			case spv::OpFUnordLessThanEqual:
			case spv::OpFOrdGreaterThanEqual:
			case spv::OpFUnordGreaterThanEqual:
			case spv::OpUMod:
			case spv::OpIEqual:
			case spv::OpINotEqual:
			case spv::OpUGreaterThan:
			case spv::OpSGreaterThan:
			case spv::OpUGreaterThanEqual:
			case spv::OpSGreaterThanEqual:
			case spv::OpULessThan:
			case spv::OpSLessThan:
			case spv::OpULessThanEqual:
			case spv::OpSLessThanEqual:
			case spv::OpShiftRightLogical:
			case spv::OpShiftRightArithmetic:
			case spv::OpShiftLeftLogical:
			case spv::OpBitwiseOr:
			case spv::OpBitwiseXor:
			case spv::OpBitwiseAnd:
			case spv::OpLogicalOr:
			case spv::OpLogicalAnd:
			case spv::OpUMulExtended:
			case spv::OpSMulExtended:
				EmitBinaryOp(insn, routine);
				break;

			case spv::OpDot:
				EmitDot(insn, routine);
				break;

			case spv::OpSelect:
				EmitSelect(insn, routine);
				break;

			default:
				UNIMPLEMENTED(OpcodeName(insn.opcode()).c_str());
				break;
			}
		}
	}

	void SpirvShader::EmitVariable(InsnIterator insn, SpirvRoutine *routine) const
	{
		ObjectID resultId = insn.word(2);
		auto &object = getObject(resultId);
		auto &objectTy = getType(object.type);
		if (object.kind == Object::Kind::InterfaceVariable && objectTy.storageClass == spv::StorageClassInput)
		{
			auto &dst = routine->getValue(resultId);
			int offset = 0;
			VisitInterface(resultId,
							[&](Decorations const &d, AttribType type) {
								auto scalarSlot = d.Location << 2 | d.Component;
								dst[offset++] = routine->inputs[scalarSlot];
							});
		}
	}

	void SpirvShader::EmitLoad(InsnIterator insn, SpirvRoutine *routine) const
	{
		ObjectID objectId = insn.word(2);
		ObjectID pointerId = insn.word(3);
		auto &object = getObject(objectId);
		auto &objectTy = getType(object.type);
		auto &pointer = getObject(pointerId);
		auto &pointerBase = getObject(pointer.pointerBase);
		auto &pointerBaseTy = getType(pointerBase.type);

		ASSERT(getType(pointer.type).element == object.type);
		ASSERT(TypeID(insn.word(1)) == object.type);

		if (pointerBaseTy.storageClass == spv::StorageClassImage ||
			pointerBaseTy.storageClass == spv::StorageClassUniform ||
			pointerBaseTy.storageClass == spv::StorageClassUniformConstant)
		{
			UNIMPLEMENTED("Descriptor-backed load not yet implemented");
		}

		auto &ptrBase = routine->getValue(pointer.pointerBase);
		auto &dst = routine->createIntermediate(objectId, objectTy.sizeInComponents);

		if (pointer.kind == Object::Kind::Value)
		{
			auto offsets = As<SIMD::Int>(routine->getIntermediate(insn.word(3))[0]);
			for (auto i = 0u; i < objectTy.sizeInComponents; i++)
			{
				// i wish i had a Float,Float,Float,Float constructor here..
				SIMD::Float v;
				for (int j = 0; j < SIMD::Width; j++)
				{
					Int offset = Int(i) + Extract(offsets, j);
					v = Insert(v, Extract(ptrBase[offset], j), j);
				}
				dst.emplace(i, v);
			}
		}
		else
		{
			// no divergent offsets to worry about
			for (auto i = 0u; i < objectTy.sizeInComponents; i++)
			{
				dst.emplace(i, ptrBase[i]);
			}
		}
	}

	void SpirvShader::EmitAccessChain(InsnIterator insn, SpirvRoutine *routine) const
	{
		TypeID typeId = insn.word(1);
		ObjectID objectId = insn.word(2);
		ObjectID baseId = insn.word(3);
		auto &object = getObject(objectId);
		auto &type = getType(typeId);
		auto &pointerBase = getObject(object.pointerBase);
		auto &pointerBaseTy = getType(pointerBase.type);
		ASSERT(type.sizeInComponents == 1);
		ASSERT(getObject(baseId).pointerBase == object.pointerBase);

		if (pointerBaseTy.storageClass == spv::StorageClassImage ||
			pointerBaseTy.storageClass == spv::StorageClassUniform ||
			pointerBaseTy.storageClass == spv::StorageClassUniformConstant)
		{
			UNIMPLEMENTED("Descriptor-backed OpAccessChain not yet implemented");
		}
		auto &dst = routine->createIntermediate(objectId, type.sizeInComponents);
		dst.emplace(0, As<SIMD::Float>(WalkAccessChain(baseId, insn.wordCount() - 4, insn.wordPointer(4), routine)));
	}

	void SpirvShader::EmitStore(InsnIterator insn, SpirvRoutine *routine) const
	{
		ObjectID pointerId = insn.word(1);
		ObjectID objectId = insn.word(2);
		auto &object = getObject(objectId);
		auto &pointer = getObject(pointerId);
		auto &pointerTy = getType(pointer.type);
		auto &elementTy = getType(pointerTy.element);
		auto &pointerBase = getObject(pointer.pointerBase);
		auto &pointerBaseTy = getType(pointerBase.type);

		if (pointerBaseTy.storageClass == spv::StorageClassImage ||
			pointerBaseTy.storageClass == spv::StorageClassUniform ||
			pointerBaseTy.storageClass == spv::StorageClassUniformConstant)
		{
			UNIMPLEMENTED("Descriptor-backed store not yet implemented");
		}

		auto &ptrBase = routine->getValue(pointer.pointerBase);

		if (object.kind == Object::Kind::Constant)
		{
			auto src = reinterpret_cast<float *>(object.constantValue.get());

			if (pointer.kind == Object::Kind::Value)
			{
				auto offsets = As<SIMD::Int>(routine->getIntermediate(pointerId)[0]);
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					// Scattered store
					for (int j = 0; j < SIMD::Width; j++)
					{
						auto dst = ptrBase[Int(i) + Extract(offsets, j)];
						dst = Insert(dst, Float(src[i]), j);
					}
				}
			}
			else
			{
				// no divergent offsets
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					ptrBase[i] = RValue<SIMD::Float>(src[i]);
				}
			}
		}
		else
		{
			auto &src = routine->getIntermediate(objectId);

			if (pointer.kind == Object::Kind::Value)
			{
				auto offsets = As<SIMD::Int>(routine->getIntermediate(pointerId)[0]);
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					// Scattered store
					for (int j = 0; j < SIMD::Width; j++)
					{
						auto dst = ptrBase[Int(i) + Extract(offsets, j)];
						dst = Insert(dst, Extract(src[i], j), j);
					}
				}
			}
			else
			{
				// no divergent offsets
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					ptrBase[i] = src[i];
				}
			}
		}
	}

	void SpirvShader::EmitCompositeConstruct(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto offset = 0u;

		for (auto i = 0u; i < insn.wordCount() - 3; i++)
		{
			ObjectID srcObjectId = insn.word(3u + i);
			auto & srcObject = getObject(srcObjectId);
			auto & srcObjectTy = getType(srcObject.type);
			GenericValue srcObjectAccess(this, routine, srcObjectId);

			for (auto j = 0u; j < srcObjectTy.sizeInComponents; j++)
				dst.emplace(offset++, srcObjectAccess[j]);
		}
	}

	void SpirvShader::EmitCompositeInsert(InsnIterator insn, SpirvRoutine *routine) const
	{
		TypeID resultTypeId = insn.word(1);
		auto &type = getType(resultTypeId);
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &newPartObject = getObject(insn.word(3));
		auto &newPartObjectTy = getType(newPartObject.type);
		auto firstNewComponent = WalkLiteralAccessChain(resultTypeId, insn.wordCount() - 5, insn.wordPointer(5));

		GenericValue srcObjectAccess(this, routine, insn.word(4));
		GenericValue newPartObjectAccess(this, routine, insn.word(3));

		// old components before
		for (auto i = 0u; i < firstNewComponent; i++)
		{
			dst.emplace(i, srcObjectAccess[i]);
		}
		// new part
		for (auto i = 0u; i < newPartObjectTy.sizeInComponents; i++)
		{
			dst.emplace(firstNewComponent + i, newPartObjectAccess[i]);
		}
		// old components after
		for (auto i = firstNewComponent + newPartObjectTy.sizeInComponents; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, srcObjectAccess[i]);
		}
	}

	void SpirvShader::EmitCompositeExtract(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &compositeObject = getObject(insn.word(3));
		TypeID compositeTypeId = compositeObject.definition.word(1);
		auto firstComponent = WalkLiteralAccessChain(compositeTypeId, insn.wordCount() - 4, insn.wordPointer(4));

		GenericValue compositeObjectAccess(this, routine, insn.word(3));
		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, compositeObjectAccess[firstComponent + i]);
		}
	}

	void SpirvShader::EmitVectorShuffle(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);

		GenericValue firstHalfAccess(this, routine, insn.word(3));
		GenericValue secondHalfAccess(this, routine, insn.word(4));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto selector = insn.word(5 + i);
			if (selector == static_cast<uint32_t>(-1))
			{
				// Undefined value. Until we decide to do real undef values, zero is as good
				// a value as any
				dst.emplace(i, RValue<SIMD::Float>(0.0f));
			}
			else if (selector < type.sizeInComponents)
			{
				dst.emplace(i, firstHalfAccess[selector]);
			}
			else
			{
				dst.emplace(i, secondHalfAccess[selector - type.sizeInComponents]);
			}
		}
	}

	void SpirvShader::EmitUnaryOp(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto src = GenericValue(this, routine, insn.word(3));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto val = src[i];

			switch (insn.opcode())
			{
			case spv::OpNot:
			case spv::OpLogicalNot:		// logical not == bitwise not due to all-bits boolean representation
				dst.emplace(i, As<SIMD::Float>(~As<SIMD::UInt>(val)));
				break;
			case spv::OpSNegate:
				dst.emplace(i, As<SIMD::Float>(-As<SIMD::Int>(val)));
				break;
			case spv::OpFNegate:
				dst.emplace(i, -val);
				break;
			case spv::OpConvertFToU:
				dst.emplace(i, As<SIMD::Float>(SIMD::UInt(val)));
				break;
			case spv::OpConvertFToS:
				dst.emplace(i, As<SIMD::Float>(SIMD::Int(val)));
				break;
			case spv::OpConvertSToF:
				dst.emplace(i, SIMD::Float(As<SIMD::Int>(val)));
				break;
			case spv::OpConvertUToF:
				dst.emplace(i, SIMD::Float(As<SIMD::UInt>(val)));
				break;
			case spv::OpBitcast:
				dst.emplace(i, val);
				break;
			default:
				UNIMPLEMENTED("Unhandled unary operator %s", OpcodeName(insn.opcode()).c_str());
			}
		}
	}

	void SpirvShader::EmitBinaryOp(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &lhsType = getType(getObject(insn.word(3)).type);
		auto srcLHS = GenericValue(this, routine, insn.word(3));
		auto srcRHS = GenericValue(this, routine, insn.word(4));

		for (auto i = 0u; i < lhsType.sizeInComponents; i++)
		{
			auto lhs = srcLHS[i];
			auto rhs = srcRHS[i];

			switch (insn.opcode())
			{
			case spv::OpIAdd:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) + As<SIMD::Int>(rhs)));
				break;
			case spv::OpISub:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) - As<SIMD::Int>(rhs)));
				break;
			case spv::OpIMul:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) * As<SIMD::Int>(rhs)));
				break;
			case spv::OpSDiv:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) / As<SIMD::Int>(rhs)));
				break;
			case spv::OpUDiv:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) / As<SIMD::UInt>(rhs)));
				break;
			case spv::OpUMod:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) % As<SIMD::UInt>(rhs)));
				break;
			case spv::OpIEqual:
				dst.emplace(i, As<SIMD::Float>(CmpEQ(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpINotEqual:
				dst.emplace(i, As<SIMD::Float>(CmpNEQ(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpUGreaterThan:
				dst.emplace(i, As<SIMD::Float>(CmpGT(As<SIMD::UInt>(lhs), As<SIMD::UInt>(rhs))));
				break;
			case spv::OpSGreaterThan:
				dst.emplace(i, As<SIMD::Float>(CmpGT(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpUGreaterThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpGE(As<SIMD::UInt>(lhs), As<SIMD::UInt>(rhs))));
				break;
			case spv::OpSGreaterThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpGE(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpULessThan:
				dst.emplace(i, As<SIMD::Float>(CmpLT(As<SIMD::UInt>(lhs), As<SIMD::UInt>(rhs))));
				break;
			case spv::OpSLessThan:
				dst.emplace(i, As<SIMD::Float>(CmpLT(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpULessThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpLE(As<SIMD::UInt>(lhs), As<SIMD::UInt>(rhs))));
				break;
			case spv::OpSLessThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpLE(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpFAdd:
				dst.emplace(i, lhs + rhs);
				break;
			case spv::OpFSub:
				dst.emplace(i, lhs - rhs);
				break;
			case spv::OpFDiv:
				dst.emplace(i, lhs / rhs);
				break;
			case spv::OpFOrdEqual:
				dst.emplace(i, As<SIMD::Float>(CmpEQ(lhs, rhs)));
				break;
			case spv::OpFUnordEqual:
				dst.emplace(i, As<SIMD::Float>(CmpUEQ(lhs, rhs)));
				break;
			case spv::OpFOrdNotEqual:
				dst.emplace(i, As<SIMD::Float>(CmpNEQ(lhs, rhs)));
				break;
			case spv::OpFUnordNotEqual:
				dst.emplace(i, As<SIMD::Float>(CmpUNEQ(lhs, rhs)));
				break;
			case spv::OpFOrdLessThan:
				dst.emplace(i, As<SIMD::Float>(CmpLT(lhs, rhs)));
				break;
			case spv::OpFUnordLessThan:
				dst.emplace(i, As<SIMD::Float>(CmpULT(lhs, rhs)));
				break;
			case spv::OpFOrdGreaterThan:
				dst.emplace(i, As<SIMD::Float>(CmpGT(lhs, rhs)));
				break;
			case spv::OpFUnordGreaterThan:
				dst.emplace(i, As<SIMD::Float>(CmpUGT(lhs, rhs)));
				break;
			case spv::OpFOrdLessThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpLE(lhs, rhs)));
				break;
			case spv::OpFUnordLessThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpULE(lhs, rhs)));
				break;
			case spv::OpFOrdGreaterThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpGE(lhs, rhs)));
				break;
			case spv::OpFUnordGreaterThanEqual:
				dst.emplace(i, As<SIMD::Float>(CmpUGE(lhs, rhs)));
				break;
			case spv::OpShiftRightLogical:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) >> As<SIMD::UInt>(rhs)));
				break;
			case spv::OpShiftRightArithmetic:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) >> As<SIMD::Int>(rhs)));
				break;
			case spv::OpShiftLeftLogical:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) << As<SIMD::UInt>(rhs)));
				break;
			case spv::OpBitwiseOr:
			case spv::OpLogicalOr:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) | As<SIMD::UInt>(rhs)));
				break;
			case spv::OpBitwiseXor:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) ^ As<SIMD::UInt>(rhs)));
				break;
			case spv::OpBitwiseAnd:
			case spv::OpLogicalAnd:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) & As<SIMD::UInt>(rhs)));
				break;
			case spv::OpSMulExtended:
				// Extended ops: result is a structure containing two members of the same type as lhs & rhs.
				// In our flat view then, component i is the i'th component of the first member;
				// component i + N is the i'th component of the second member.
				dst.emplace(i, As<SIMD::Float>(As<SIMD::Int>(lhs) * As<SIMD::Int>(rhs)));
				dst.emplace(i + lhsType.sizeInComponents, As<SIMD::Float>(MulHigh(As<SIMD::Int>(lhs), As<SIMD::Int>(rhs))));
				break;
			case spv::OpUMulExtended:
				dst.emplace(i, As<SIMD::Float>(As<SIMD::UInt>(lhs) * As<SIMD::UInt>(rhs)));
				dst.emplace(i + lhsType.sizeInComponents, As<SIMD::Float>(MulHigh(As<SIMD::UInt>(lhs), As<SIMD::UInt>(rhs))));
				break;
			default:
				UNIMPLEMENTED("Unhandled binary operator %s", OpcodeName(insn.opcode()).c_str());
			}
		}
	}

	void SpirvShader::EmitDot(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		assert(type.sizeInComponents == 1);
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &lhsType = getType(getObject(insn.word(3)).type);
		auto srcLHS = GenericValue(this, routine, insn.word(3));
		auto srcRHS = GenericValue(this, routine, insn.word(4));

		SIMD::Float result = srcLHS[0] * srcRHS[0];

		for (auto i = 1u; i < lhsType.sizeInComponents; i++)
		{
			result += srcLHS[i] * srcRHS[i];
		}

		dst.emplace(0, result);
	}

	void SpirvShader::EmitSelect(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto srcCond = GenericValue(this, routine, insn.word(3));
		auto srcLHS = GenericValue(this, routine, insn.word(4));
		auto srcRHS = GenericValue(this, routine, insn.word(5));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto cond = As<SIMD::Int>(srcCond[i]);
			auto lhs = srcLHS[i];
			auto rhs = srcRHS[i];
			auto out = (cond & As<Int4>(lhs)) | (~cond & As<Int4>(rhs));   // FIXME: IfThenElse()
			dst.emplace(i, As<SIMD::Float>(out));
		}
	}

	void SpirvShader::emitEpilog(SpirvRoutine *routine) const
	{
		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpVariable:
			{
				ObjectID resultId = insn.word(2);
				auto &object = getObject(resultId);
				auto &objectTy = getType(object.type);
				if (object.kind == Object::Kind::InterfaceVariable && objectTy.storageClass == spv::StorageClassOutput)
				{
					auto &dst = routine->getValue(resultId);
					int offset = 0;
					VisitInterface(resultId,
								   [&](Decorations const &d, AttribType type) {
									   auto scalarSlot = d.Location << 2 | d.Component;
									   routine->outputs[scalarSlot] = dst[offset++];
								   });
				}
				break;
			}
			default:
				break;
			}
		}
	}

	SpirvRoutine::SpirvRoutine(vk::PipelineLayout const *pipelineLayout) :
		pipelineLayout(pipelineLayout)
	{
	}

}
