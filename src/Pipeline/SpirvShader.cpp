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
#include <spirv/unified1/GLSL.std.450.h>
#include "SpirvShader.hpp"
#include "System/Math.hpp"
#include "Vulkan/VkBuffer.hpp"
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

		Block::ID currentBlock;
		InsnIterator blockStart;

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
				Type::ID targetId = insn.word(1);
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
			{
				ASSERT(currentBlock.value() == 0);
				currentBlock = Block::ID(insn.word(1));
				blockStart = insn;
				break;
			}

			// Branch Instructions (subset of Termination Instructions):
			case spv::OpBranch:
			case spv::OpBranchConditional:
			case spv::OpSwitch:
			case spv::OpReturn:
			// fallthrough

			// Termination instruction:
			case spv::OpKill:
			case spv::OpUnreachable:
			{
				ASSERT(currentBlock.value() != 0);
				auto blockEnd = insn; blockEnd++;
				blocks[currentBlock] = Block(blockStart, blockEnd);
				currentBlock = Block::ID(0);

				if (insn.opcode() == spv::OpKill)
				{
					modes.ContainsKill = true;
				}
				break;
			}

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
				Type::ID typeId = insn.word(1);
				Object::ID resultId = insn.word(2);
				auto storageClass = static_cast<spv::StorageClass>(insn.word(3));
				if (insn.wordCount() > 4)
					UNIMPLEMENTED("Variable initializers not yet supported");

				auto &object = defs[resultId];
				object.kind = Object::Kind::Variable;
				object.definition = insn;
				object.type = typeId;
				object.pointerBase = insn.word(2);	// base is itself

				ASSERT(getType(typeId).storageClass == storageClass);

				switch (storageClass)
				{
				case spv::StorageClassInput:
				case spv::StorageClassOutput:
					ProcessInterfaceVariable(object);
					break;
				case spv::StorageClassUniform:
				case spv::StorageClassStorageBuffer:
				case spv::StorageClassPushConstant:
					object.kind = Object::Kind::PhysicalPointer;
					break;

				case spv::StorageClassPrivate:
				case spv::StorageClassFunction:
					break; // Correctly handled.

				case spv::StorageClassUniformConstant:
				case spv::StorageClassWorkgroup:
				case spv::StorageClassCrossWorkgroup:
				case spv::StorageClassGeneric:
				case spv::StorageClassAtomicCounter:
				case spv::StorageClassImage:
					UNIMPLEMENTED("StorageClass %d not yet implemented", (int)storageClass);
					break;

				default:
					UNREACHABLE("Unexpected StorageClass"); // See Appendix A of the Vulkan spec.
					break;
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
			case spv::OpUndef:
			{
				// TODO: consider a real LLVM-level undef. For now, zero is a perfectly good value.
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

				auto objectId = Object::ID(insn.word(2));
				auto decorationsIt = decorations.find(objectId);
				if (decorationsIt != decorations.end() &&
					decorationsIt->second.BuiltIn == spv::BuiltInWorkgroupSize)
				{
					// https://www.khronos.org/registry/vulkan/specs/1.1/html/vkspec.html#interfaces-builtin-variables :
					// Decorating an object with the WorkgroupSize built-in
					// decoration will make that object contain the dimensions
					// of a local workgroup. If an object is decorated with the
					// WorkgroupSize decoration, this must take precedence over
					// any execution mode set for LocalSize.
					// The object decorated with WorkgroupSize must be declared
					// as a three-component vector of 32-bit integers.
					ASSERT(getType(object.type).sizeInComponents == 3);
					modes.WorkgroupSizeX = object.constantValue[0];
					modes.WorkgroupSizeY = object.constantValue[1];
					modes.WorkgroupSizeZ = object.constantValue[2];
				}
				break;
			}

			case spv::OpCapability:
				break; // Various capabilities will be declared, but none affect our code generation at this point.
			case spv::OpMemoryModel:
				break; // Memory model does not affect our code generation until we decide to do Vulkan Memory Model support.

			case spv::OpEntryPoint:
				break;
			case spv::OpFunction:
				ASSERT(mainBlockId.value() == 0); // Multiple functions found
				// Scan forward to find the function's label.
				for (auto it = insn; it != end() && mainBlockId.value() == 0; it++)
				{
					switch (it.opcode())
					{
					case spv::OpFunction:
					case spv::OpFunctionParameter:
						break;
					case spv::OpLabel:
						mainBlockId = Block::ID(it.word(1));
						break;
					default:
						WARN("Unexpected opcode '%s' following OpFunction", OpcodeName(it.opcode()).c_str());
					}
				}
				ASSERT(mainBlockId.value() != 0); // Function's OpLabel not found
				break;
			case spv::OpFunctionEnd:
				// Due to preprocessing, the entrypoint and its function provide no value.
				break;
			case spv::OpExtInstImport:
				// We will only support the GLSL 450 extended instruction set, so no point in tracking the ID we assign it.
				// Valid shaders will not attempt to import any other instruction sets.
				if (0 != strcmp("GLSL.std.450", reinterpret_cast<char const *>(insn.wordPointer(2))))
				{
					UNIMPLEMENTED("Only GLSL extended instruction set is supported");
				}
				break;
			case spv::OpName:
			case spv::OpMemberName:
			case spv::OpSource:
			case spv::OpSourceContinued:
			case spv::OpSourceExtension:
			case spv::OpLine:
			case spv::OpNoLine:
			case spv::OpModuleProcessed:
			case spv::OpString:
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
			case spv::OpInBoundsAccessChain:
			case spv::OpCompositeConstruct:
			case spv::OpCompositeInsert:
			case spv::OpCompositeExtract:
			case spv::OpVectorShuffle:
			case spv::OpVectorTimesScalar:
			case spv::OpVectorExtractDynamic:
			case spv::OpVectorInsertDynamic:
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
			case spv::OpFMul:
			case spv::OpFDiv:
			case spv::OpFMod:
			case spv::OpFRem:
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
			case spv::OpSMod:
			case spv::OpSRem:
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
			case spv::OpLogicalEqual:
			case spv::OpLogicalNotEqual:
			case spv::OpUMulExtended:
			case spv::OpSMulExtended:
			case spv::OpDot:
			case spv::OpConvertFToU:
			case spv::OpConvertFToS:
			case spv::OpConvertSToF:
			case spv::OpConvertUToF:
			case spv::OpBitcast:
			case spv::OpSelect:
			case spv::OpExtInst:
			case spv::OpIsInf:
			case spv::OpIsNan:
			case spv::OpAny:
			case spv::OpAll:
				// Instructions that yield an intermediate value
			{
				Type::ID typeId = insn.word(1);
				Object::ID resultId = insn.word(2);
				auto &object = defs[resultId];
				object.type = typeId;
				object.kind = Object::Kind::Value;
				object.definition = insn;

				if (insn.opcode() == spv::OpAccessChain || insn.opcode() == spv::OpInBoundsAccessChain)
				{
					// interior ptr has two parts:
					// - logical base ptr, common across all lanes and known at compile time
					// - per-lane offset
					Object::ID baseId = insn.word(3);
					object.pointerBase = getObject(baseId).pointerBase;
				}
				break;
			}

			case spv::OpStore:
				// Don't need to do anything during analysis pass
				break;

			default:
				UNIMPLEMENTED("%s", OpcodeName(insn.opcode()).c_str());
			}
		}
	}

	void SpirvShader::DeclareType(InsnIterator insn)
	{
		Type::ID resultId = insn.word(1);

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
			Type::ID elementTypeId = insn.word(3);
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
			Type::ID elementTypeId = insn.word(2);
			type.element = elementTypeId;
			break;
		}
		default:
			break;
		}
	}

	SpirvShader::Object& SpirvShader::CreateConstant(InsnIterator insn)
	{
		Type::ID typeId = insn.word(1);
		Object::ID resultId = insn.word(2);
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
		Object::ID resultId = object.definition.word(2);

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
			modes.WorkgroupSizeX = insn.word(3);
			modes.WorkgroupSizeY = insn.word(4);
			modes.WorkgroupSizeZ = insn.word(5);
			break;
		case spv::ExecutionModeOriginUpperLeft:
			// This is always the case for a Vulkan shader. Do nothing.
			break;
		default:
			UNIMPLEMENTED("No other execution modes are permitted");
		}
	}

	uint32_t SpirvShader::ComputeTypeSize(InsnIterator insn)
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

	bool SpirvShader::IsStorageInterleavedByLane(spv::StorageClass storageClass)
	{
		switch (storageClass)
		{
		case spv::StorageClassUniform:
		case spv::StorageClassStorageBuffer:
		case spv::StorageClassPushConstant:
			return false;
		default:
			return true;
		}
	}

	template<typename F>
	int SpirvShader::VisitInterfaceInner(Type::ID id, Decorations d, F f) const
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
	void SpirvShader::VisitInterface(Object::ID id, F f) const
	{
		// Walk a variable definition and call f for each component in it.
		Decorations d{};
		ApplyDecorationsForId(&d, id);

		auto def = getObject(id).definition;
		ASSERT(def.opcode() == spv::OpVariable);
		VisitInterfaceInner<F>(def.word(1), d, f);
	}

	SIMD::Int SpirvShader::WalkExplicitLayoutAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const
	{
		// Produce a offset into external memory in sizeof(float) units

		int constantOffset = 0;
		SIMD::Int dynamicOffset = SIMD::Int(0);
		auto &baseObject = getObject(id);
		Type::ID typeId = getType(baseObject.type).element;
		Decorations d{};
		ApplyDecorationsForId(&d, baseObject.type);

		// The <base> operand is an intermediate value itself, ie produced by a previous OpAccessChain.
		// Start with its offset and build from there.
		if (baseObject.kind == Object::Kind::Value)
		{
			dynamicOffset += routine->getIntermediate(id).Int(0);
		}

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			switch (type.definition.opcode())
			{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstantInt(indexIds[i]);
				ApplyDecorationsForIdMember(&d, typeId, memberIndex);
				ASSERT(d.HasOffset);
				constantOffset += d.Offset / sizeof(float);
				typeId = type.definition.word(2u + memberIndex);
				break;
			}
			case spv::OpTypeArray:
			case spv::OpTypeRuntimeArray:
			{
				// TODO: b/127950082: Check bounds.
				ApplyDecorationsForId(&d, typeId);
				ASSERT(d.HasArrayStride);
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					constantOffset += d.ArrayStride/sizeof(float) * GetConstantInt(indexIds[i]);
				else
					dynamicOffset += SIMD::Int(d.ArrayStride / sizeof(float)) * routine->getIntermediate(indexIds[i]).Int(0);
				typeId = type.element;
				break;
			}
			case spv::OpTypeMatrix:
			{
				// TODO: b/127950082: Check bounds.
				ApplyDecorationsForId(&d, typeId);
				ASSERT(d.HasMatrixStride);
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					constantOffset += d.MatrixStride/sizeof(float) * GetConstantInt(indexIds[i]);
				else
					dynamicOffset += SIMD::Int(d.MatrixStride / sizeof(float)) * routine->getIntermediate(indexIds[i]).Int(0);
				typeId = type.element;
				break;
			}
			case spv::OpTypeVector:
			{
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					constantOffset += GetConstantInt(indexIds[i]);
				else
					dynamicOffset += routine->getIntermediate(indexIds[i]).Int(0);
				typeId = type.element;
				break;
			}
			default:
				UNIMPLEMENTED("Unexpected type '%s' in WalkExplicitLayoutAccessChain", OpcodeName(type.definition.opcode()).c_str());
			}
		}

		return dynamicOffset + SIMD::Int(constantOffset);
	}

	SIMD::Int SpirvShader::WalkAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const
	{
		// TODO: avoid doing per-lane work in some cases if we can?
		// Produce a *component* offset into location-oriented memory

		int constantOffset = 0;
		SIMD::Int dynamicOffset = SIMD::Int(0);
		auto &baseObject = getObject(id);
		Type::ID typeId = getType(baseObject.type).element;

		// The <base> operand is an intermediate value itself, ie produced by a previous OpAccessChain.
		// Start with its offset and build from there.
		if (baseObject.kind == Object::Kind::Value)
		{
			dynamicOffset += routine->getIntermediate(id).Int(0);
		}

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
			case spv::OpTypeRuntimeArray:
			{
				// TODO: b/127950082: Check bounds.
				auto stride = getType(type.element).sizeInComponents;
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					constantOffset += stride * GetConstantInt(indexIds[i]);
				else
					dynamicOffset += SIMD::Int(stride) * routine->getIntermediate(indexIds[i]).Int(0);
				typeId = type.element;
				break;
			}

			default:
				UNIMPLEMENTED("Unexpected type '%s' in WalkAccessChain", OpcodeName(type.definition.opcode()).c_str());
			}
		}

		return dynamicOffset + SIMD::Int(constantOffset);
	}

	uint32_t SpirvShader::WalkLiteralAccessChain(Type::ID typeId, uint32_t numIndexes, uint32_t const *indexes) const
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
		case spv::DecorationOffset:
			HasOffset = true;
			Offset = static_cast<int32_t>(arg);
			break;
		case spv::DecorationArrayStride:
			HasArrayStride = true;
			ArrayStride = static_cast<int32_t>(arg);
			break;
		case spv::DecorationMatrixStride:
			HasMatrixStride = true;
			MatrixStride = static_cast<int32_t>(arg);
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

		if (src.HasOffset)
		{
			HasOffset = true;
			Offset = src.Offset;
		}

		if (src.HasArrayStride)
		{
			HasArrayStride = true;
			ArrayStride = src.ArrayStride;
		}

		if (src.HasMatrixStride)
		{
			HasMatrixStride = true;
			MatrixStride = src.MatrixStride;
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

	void SpirvShader::ApplyDecorationsForIdMember(Decorations *d, Type::ID id, uint32_t member) const
	{
		auto it = memberDecorations.find(id);
		if (it != memberDecorations.end() && member < it->second.size())
		{
			d->Apply(it->second[member]);
		}
	}

	uint32_t SpirvShader::GetConstantInt(Object::ID id) const
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
				Object::ID resultId = insn.word(2);
				auto &object = getObject(resultId);
				auto &objectTy = getType(object.type);
				auto &pointeeTy = getType(objectTy.element);
				// TODO: what to do about zero-slot objects?
				if (pointeeTy.sizeInComponents > 0)
				{
					routine->createLvalue(resultId, pointeeTy.sizeInComponents);
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
		// Emit everything up to the first label
		// TODO: Separate out dispatch of block from non-block instructions?
		for (auto insn : *this)
		{
			if (insn.opcode() == spv::OpLabel)
			{
				break;
			}
			EmitInstruction(routine, insn);
		}

		// Emit the main function block
		EmitBlock(routine, getBlock(mainBlockId));
	}

	void SpirvShader::EmitBlock(SpirvRoutine *routine, Block const &block) const
	{
		for (auto insn : block)
		{
			EmitInstruction(routine, insn);
		}
	}

	void SpirvShader::EmitInstruction(SpirvRoutine *routine, InsnIterator insn) const
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
		case spv::OpUndef:
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
		case spv::OpLine:
		case spv::OpNoLine:
		case spv::OpModuleProcessed:
		case spv::OpString:
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
		case spv::OpInBoundsAccessChain:
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

		case spv::OpVectorExtractDynamic:
			EmitVectorExtractDynamic(insn, routine);
			break;

		case spv::OpVectorInsertDynamic:
			EmitVectorInsertDynamic(insn, routine);
			break;

		case spv::OpVectorTimesScalar:
			EmitVectorTimesScalar(insn, routine);
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
		case spv::OpIsInf:
		case spv::OpIsNan:
			EmitUnaryOp(insn, routine);
			break;

		case spv::OpIAdd:
		case spv::OpISub:
		case spv::OpIMul:
		case spv::OpSDiv:
		case spv::OpUDiv:
		case spv::OpFAdd:
		case spv::OpFSub:
		case spv::OpFMul:
		case spv::OpFDiv:
		case spv::OpFMod:
		case spv::OpFRem:
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
		case spv::OpSMod:
		case spv::OpSRem:
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
		case spv::OpLogicalEqual:
		case spv::OpLogicalNotEqual:
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

		case spv::OpExtInst:
			EmitExtendedInstruction(insn, routine);
			break;

		case spv::OpAny:
			EmitAny(insn, routine);
			break;

		case spv::OpAll:
			EmitAll(insn, routine);
			break;

		case spv::OpBranch:
			EmitBranch(insn, routine);
			break;

		default:
			UNIMPLEMENTED("opcode: %s", OpcodeName(insn.opcode()).c_str());
			break;
		}
	}

	void SpirvShader::EmitVariable(InsnIterator insn, SpirvRoutine *routine) const
	{
		Object::ID resultId = insn.word(2);
		auto &object = getObject(resultId);
		auto &objectTy = getType(object.type);
		switch (objectTy.storageClass)
		{
		case spv::StorageClassInput:
		{
			if (object.kind == Object::Kind::InterfaceVariable)
			{
				auto &dst = routine->getValue(resultId);
				int offset = 0;
				VisitInterface(resultId,
								[&](Decorations const &d, AttribType type) {
									auto scalarSlot = d.Location << 2 | d.Component;
									dst[offset++] = routine->inputs[scalarSlot];
								});
			}
			break;
		}
		case spv::StorageClassUniform:
		case spv::StorageClassStorageBuffer:
		{
			Decorations d{};
			ApplyDecorationsForId(&d, resultId);
			ASSERT(d.DescriptorSet >= 0);
			ASSERT(d.Binding >= 0);

			size_t bindingOffset = routine->pipelineLayout->getBindingOffset(d.DescriptorSet, d.Binding);

			Pointer<Byte> set = routine->descriptorSets[d.DescriptorSet]; // DescriptorSet*
			Pointer<Byte> binding = Pointer<Byte>(set + bindingOffset); // VkDescriptorBufferInfo*
			Pointer<Byte> buffer = *Pointer<Pointer<Byte>>(binding + OFFSET(VkDescriptorBufferInfo, buffer)); // vk::Buffer*
			Pointer<Byte> data = *Pointer<Pointer<Byte>>(buffer + vk::Buffer::DataOffset); // void*
			Int offset = *Pointer<Int>(binding + OFFSET(VkDescriptorBufferInfo, offset));
			Pointer<Byte> address = data + offset;
			routine->physicalPointers[resultId] = address;
			break;
		}
		case spv::StorageClassPushConstant:
		{
			routine->physicalPointers[resultId] = routine->pushConstants;
			break;
		}
		default:
			break;
		}
	}

	void SpirvShader::EmitLoad(InsnIterator insn, SpirvRoutine *routine) const
	{
		Object::ID objectId = insn.word(2);
		Object::ID pointerId = insn.word(3);
		auto &object = getObject(objectId);
		auto &objectTy = getType(object.type);
		auto &pointer = getObject(pointerId);
		auto &pointerBase = getObject(pointer.pointerBase);
		auto &pointerBaseTy = getType(pointerBase.type);

		ASSERT(getType(pointer.type).element == object.type);
		ASSERT(Type::ID(insn.word(1)) == object.type);

		if (pointerBaseTy.storageClass == spv::StorageClassImage)
		{
			UNIMPLEMENTED("StorageClassImage load not yet implemented");
		}

		Pointer<Float> ptrBase;
		if (pointerBase.kind == Object::Kind::PhysicalPointer)
		{
			ptrBase = routine->getPhysicalPointer(pointer.pointerBase);
		}
		else
		{
			ptrBase = &routine->getValue(pointer.pointerBase)[0];
		}

		bool interleavedByLane = IsStorageInterleavedByLane(pointerBaseTy.storageClass);
		auto anyInactiveLanes = SignMask(~routine->activeLaneMask) != 0;

		auto load = SpirvRoutine::Value(objectTy.sizeInComponents);

		If(pointer.kind == Object::Kind::Value || anyInactiveLanes)
		{
			// Divergent offsets or masked lanes.
			auto offsets = pointer.kind == Object::Kind::Value ?
					As<SIMD::Int>(routine->getIntermediate(pointerId).Int(0)) :
					RValue<SIMD::Int>(SIMD::Int(0));
			for (auto i = 0u; i < objectTy.sizeInComponents; i++)
			{
				// i wish i had a Float,Float,Float,Float constructor here..
				for (int j = 0; j < SIMD::Width; j++)
				{
					If(Extract(routine->activeLaneMask, j) != 0)
					{
						Int offset = Int(i) + Extract(offsets, j);
						if (interleavedByLane) { offset = offset * SIMD::Width + j; }
						load[i] = Insert(load[i], ptrBase[offset], j);
					}
				}
			}
		}
		Else
		{
			// No divergent offsets or masked lanes.
			if (interleavedByLane)
			{
				// Lane-interleaved data.
				Pointer<SIMD::Float> src = ptrBase;
				for (auto i = 0u; i < objectTy.sizeInComponents; i++)
				{
					load[i] = src[i];
				}
			}
			else
			{
				// Non-interleaved data.
				for (auto i = 0u; i < objectTy.sizeInComponents; i++)
				{
					load[i] = RValue<SIMD::Float>(ptrBase[i]);
				}
			}
		}

		auto &dst = routine->createIntermediate(objectId, objectTy.sizeInComponents);
		for (auto i = 0u; i < objectTy.sizeInComponents; i++)
		{
			dst.emplace(i, load[i]);
		}
	}

	void SpirvShader::EmitAccessChain(InsnIterator insn, SpirvRoutine *routine) const
	{
		Type::ID typeId = insn.word(1);
		Object::ID objectId = insn.word(2);
		Object::ID baseId = insn.word(3);
		auto &type = getType(typeId);
		ASSERT(type.sizeInComponents == 1);
		ASSERT(getObject(baseId).pointerBase == getObject(objectId).pointerBase);

		auto &dst = routine->createIntermediate(objectId, type.sizeInComponents);

		if (type.storageClass == spv::StorageClassPushConstant ||
			type.storageClass == spv::StorageClassUniform ||
			type.storageClass == spv::StorageClassStorageBuffer)
		{
			dst.emplace(0, WalkExplicitLayoutAccessChain(baseId, insn.wordCount() - 4, insn.wordPointer(4), routine));
		}
		else
		{
			dst.emplace(0, WalkAccessChain(baseId, insn.wordCount() - 4, insn.wordPointer(4), routine));
		}
	}

	void SpirvShader::EmitStore(InsnIterator insn, SpirvRoutine *routine) const
	{
		Object::ID pointerId = insn.word(1);
		Object::ID objectId = insn.word(2);
		auto &object = getObject(objectId);
		auto &pointer = getObject(pointerId);
		auto &pointerTy = getType(pointer.type);
		auto &elementTy = getType(pointerTy.element);
		auto &pointerBase = getObject(pointer.pointerBase);
		auto &pointerBaseTy = getType(pointerBase.type);

		if (pointerBaseTy.storageClass == spv::StorageClassImage)
		{
			UNIMPLEMENTED("StorageClassImage store not yet implemented");
		}

		Pointer<Float> ptrBase;
		if (pointerBase.kind == Object::Kind::PhysicalPointer)
		{
			ptrBase = routine->getPhysicalPointer(pointer.pointerBase);
		}
		else
		{
			ptrBase = &routine->getValue(pointer.pointerBase)[0];
		}

		bool interleavedByLane = IsStorageInterleavedByLane(pointerBaseTy.storageClass);
		auto anyInactiveLanes = SignMask(~routine->activeLaneMask) != 0;

		if (object.kind == Object::Kind::Constant)
		{
			// Constant source data.
			auto src = reinterpret_cast<float *>(object.constantValue.get());
			If(pointer.kind == Object::Kind::Value || anyInactiveLanes)
			{
				// Divergent offsets or masked lanes.
				auto offsets = pointer.kind == Object::Kind::Value ?
						As<SIMD::Int>(routine->getIntermediate(pointerId).Int(0)) :
						RValue<SIMD::Int>(SIMD::Int(0));
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					for (int j = 0; j < SIMD::Width; j++)
					{
						If(Extract(routine->activeLaneMask, j) != 0)
						{
							Int offset = Int(i) + Extract(offsets, j);
							if (interleavedByLane) { offset = offset * SIMD::Width + j; }
							ptrBase[offset] = RValue<Float>(src[i]);
						}
					}
				}
			}
			Else
			{
				// Constant source data.
				// No divergent offsets or masked lanes.
				Pointer<SIMD::Float> dst = ptrBase;
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					dst[i] = RValue<SIMD::Float>(src[i]);
				}
			}
		}
		else
		{
			// Intermediate source data.
			auto &src = routine->getIntermediate(objectId);
			If(pointer.kind == Object::Kind::Value || anyInactiveLanes)
			{
				// Divergent offsets or masked lanes.
				auto offsets = pointer.kind == Object::Kind::Value ?
						As<SIMD::Int>(routine->getIntermediate(pointerId).Int(0)) :
						RValue<SIMD::Int>(SIMD::Int(0));
				for (auto i = 0u; i < elementTy.sizeInComponents; i++)
				{
					for (int j = 0; j < SIMD::Width; j++)
					{
						If(Extract(routine->activeLaneMask, j) != 0)
						{
							Int offset = Int(i) + Extract(offsets, j);
							if (interleavedByLane) { offset = offset * SIMD::Width + j; }
							ptrBase[offset] = Extract(src.Float(i), j);
						}
					}
				}
			}
			Else
			{
				// No divergent offsets or masked lanes.
				if (interleavedByLane)
				{
					// Lane-interleaved data.
					Pointer<SIMD::Float> dst = ptrBase;
					for (auto i = 0u; i < elementTy.sizeInComponents; i++)
					{
						dst[i] = src.Float(i);
					}
				}
				else
				{
					// Intermediate source data. Non-interleaved data.
					Pointer<SIMD::Float> dst = ptrBase;
					for (auto i = 0u; i < elementTy.sizeInComponents; i++)
					{
						dst[i] = SIMD::Float(src.Float(i));
					}
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
			Object::ID srcObjectId = insn.word(3u + i);
			auto & srcObject = getObject(srcObjectId);
			auto & srcObjectTy = getType(srcObject.type);
			GenericValue srcObjectAccess(this, routine, srcObjectId);

			for (auto j = 0u; j < srcObjectTy.sizeInComponents; j++)
			{
				dst.emplace(offset++, srcObjectAccess.Float(j));
			}
		}
	}

	void SpirvShader::EmitCompositeInsert(InsnIterator insn, SpirvRoutine *routine) const
	{
		Type::ID resultTypeId = insn.word(1);
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
			dst.emplace(i, srcObjectAccess.Float(i));
		}
		// new part
		for (auto i = 0u; i < newPartObjectTy.sizeInComponents; i++)
		{
			dst.emplace(firstNewComponent + i, newPartObjectAccess.Float(i));
		}
		// old components after
		for (auto i = firstNewComponent + newPartObjectTy.sizeInComponents; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, srcObjectAccess.Float(i));
		}
	}

	void SpirvShader::EmitCompositeExtract(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &compositeObject = getObject(insn.word(3));
		Type::ID compositeTypeId = compositeObject.definition.word(1);
		auto firstComponent = WalkLiteralAccessChain(compositeTypeId, insn.wordCount() - 4, insn.wordPointer(4));

		GenericValue compositeObjectAccess(this, routine, insn.word(3));
		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, compositeObjectAccess.Float(firstComponent + i));
		}
	}

	void SpirvShader::EmitVectorShuffle(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);

		// Note: number of components in result type, first half type, and second
		// half type are all independent.
		auto &firstHalfType = getType(getObject(insn.word(3)).type);

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
			else if (selector < firstHalfType.sizeInComponents)
			{
				dst.emplace(i, firstHalfAccess.Float(selector));
			}
			else
			{
				dst.emplace(i, secondHalfAccess.Float(selector - firstHalfType.sizeInComponents));
			}
		}
	}

	void SpirvShader::EmitVectorExtractDynamic(sw::SpirvShader::InsnIterator insn, sw::SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &srcType = getType(getObject(insn.word(3)).type);

		GenericValue src(this, routine, insn.word(3));
		GenericValue index(this, routine, insn.word(4));

		SIMD::UInt v = SIMD::UInt(0);

		for (auto i = 0u; i < srcType.sizeInComponents; i++)
		{
			v |= CmpEQ(index.UInt(0), SIMD::UInt(i)) & src.UInt(i);
		}

		dst.emplace(0, v);
	}

	void SpirvShader::EmitVectorInsertDynamic(sw::SpirvShader::InsnIterator insn, sw::SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);

		GenericValue src(this, routine, insn.word(3));
		GenericValue component(this, routine, insn.word(4));
		GenericValue index(this, routine, insn.word(5));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::UInt mask = CmpEQ(SIMD::UInt(i), index.UInt(0));
			dst.emplace(i, (src.UInt(i) & ~mask) | (component.UInt(0) & mask));
		}
	}

	void SpirvShader::EmitVectorTimesScalar(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto lhs = GenericValue(this, routine, insn.word(3));
		auto rhs = GenericValue(this, routine, insn.word(4));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, lhs.Float(i) * rhs.Float(0));
		}
	}

	void SpirvShader::EmitUnaryOp(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto src = GenericValue(this, routine, insn.word(3));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			switch (insn.opcode())
			{
			case spv::OpNot:
			case spv::OpLogicalNot:		// logical not == bitwise not due to all-bits boolean representation
				dst.emplace(i, ~src.UInt(i));
				break;
			case spv::OpSNegate:
				dst.emplace(i, -src.Int(i));
				break;
			case spv::OpFNegate:
				dst.emplace(i, -src.Float(i));
				break;
			case spv::OpConvertFToU:
				dst.emplace(i, SIMD::UInt(src.Float(i)));
				break;
			case spv::OpConvertFToS:
				dst.emplace(i, SIMD::Int(src.Float(i)));
				break;
			case spv::OpConvertSToF:
				dst.emplace(i, SIMD::Float(src.Int(i)));
				break;
			case spv::OpConvertUToF:
				dst.emplace(i, SIMD::Float(src.UInt(i)));
				break;
			case spv::OpBitcast:
				dst.emplace(i, src.Float(i));
				break;
			case spv::OpIsInf:
				dst.emplace(i, IsInf(src.Float(i)));
				break;
			case spv::OpIsNan:
				dst.emplace(i, IsNan(src.Float(i)));
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
		auto lhs = GenericValue(this, routine, insn.word(3));
		auto rhs = GenericValue(this, routine, insn.word(4));

		for (auto i = 0u; i < lhsType.sizeInComponents; i++)
		{
			switch (insn.opcode())
			{
			case spv::OpIAdd:
				dst.emplace(i, lhs.Int(i) + rhs.Int(i));
				break;
			case spv::OpISub:
				dst.emplace(i, lhs.Int(i) - rhs.Int(i));
				break;
			case spv::OpIMul:
				dst.emplace(i, lhs.Int(i) * rhs.Int(i));
				break;
			case spv::OpSDiv:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0)); // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1))); // prevent integer overflow
				dst.emplace(i, a / b);
				break;
			}
			case spv::OpUDiv:
			{
				auto zeroMask = As<SIMD::UInt>(CmpEQ(rhs.Int(i), SIMD::Int(0)));
				dst.emplace(i, lhs.UInt(i) / (rhs.UInt(i) | zeroMask));
				break;
			}
			case spv::OpSRem:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0)); // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1))); // prevent integer overflow
				dst.emplace(i, a % b);
				break;
			}
			case spv::OpSMod:
			{
				SIMD::Int a = lhs.Int(i);
				SIMD::Int b = rhs.Int(i);
				b = b | CmpEQ(b, SIMD::Int(0)); // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1))); // prevent integer overflow
				auto mod = a % b;
				// If a and b have opposite signs, the remainder operation takes
				// the sign from a but OpSMod is supposed to take the sign of b.
				// Adding b will ensure that the result has the correct sign and
				// that it is still congruent to a modulo b.
				//
				// See also http://mathforum.org/library/drmath/view/52343.html
				auto signDiff = CmpNEQ(CmpGE(a, SIMD::Int(0)), CmpGE(b, SIMD::Int(0)));
				auto fixedMod = mod + (b & CmpNEQ(mod, SIMD::Int(0)) & signDiff);
				dst.emplace(i, As<SIMD::Float>(fixedMod));
				break;
			}
			case spv::OpUMod:
			{
				auto zeroMask = As<SIMD::UInt>(CmpEQ(rhs.Int(i), SIMD::Int(0)));
				dst.emplace(i, lhs.UInt(i) % (rhs.UInt(i) | zeroMask));
				break;
			}
			case spv::OpIEqual:
			case spv::OpLogicalEqual:
				dst.emplace(i, CmpEQ(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpINotEqual:
			case spv::OpLogicalNotEqual:
				dst.emplace(i, CmpNEQ(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUGreaterThan:
				dst.emplace(i, CmpGT(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSGreaterThan:
				dst.emplace(i, CmpGT(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUGreaterThanEqual:
				dst.emplace(i, CmpGE(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSGreaterThanEqual:
				dst.emplace(i, CmpGE(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpULessThan:
				dst.emplace(i, CmpLT(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSLessThan:
				dst.emplace(i, CmpLT(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpULessThanEqual:
				dst.emplace(i, CmpLE(lhs.UInt(i), rhs.UInt(i)));
				break;
			case spv::OpSLessThanEqual:
				dst.emplace(i, CmpLE(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpFAdd:
				dst.emplace(i, lhs.Float(i) + rhs.Float(i));
				break;
			case spv::OpFSub:
				dst.emplace(i, lhs.Float(i) - rhs.Float(i));
				break;
			case spv::OpFMul:
				dst.emplace(i, lhs.Float(i) * rhs.Float(i));
				break;
			case spv::OpFDiv:
				dst.emplace(i, lhs.Float(i) / rhs.Float(i));
				break;
			case spv::OpFMod:
				// TODO(b/126873455): inaccurate for values greater than 2^24
				dst.emplace(i, lhs.Float(i) - rhs.Float(i) * Floor(lhs.Float(i) / rhs.Float(i)));
				break;
			case spv::OpFRem:
				dst.emplace(i, lhs.Float(i) % rhs.Float(i));
				break;
			case spv::OpFOrdEqual:
				dst.emplace(i, CmpEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordEqual:
				dst.emplace(i, CmpUEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdNotEqual:
				dst.emplace(i, CmpNEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordNotEqual:
				dst.emplace(i, CmpUNEQ(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdLessThan:
				dst.emplace(i, CmpLT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordLessThan:
				dst.emplace(i, CmpULT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdGreaterThan:
				dst.emplace(i, CmpGT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordGreaterThan:
				dst.emplace(i, CmpUGT(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdLessThanEqual:
				dst.emplace(i, CmpLE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordLessThanEqual:
				dst.emplace(i, CmpULE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFOrdGreaterThanEqual:
				dst.emplace(i, CmpGE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpFUnordGreaterThanEqual:
				dst.emplace(i, CmpUGE(lhs.Float(i), rhs.Float(i)));
				break;
			case spv::OpShiftRightLogical:
				dst.emplace(i, lhs.UInt(i) >> rhs.UInt(i));
				break;
			case spv::OpShiftRightArithmetic:
				dst.emplace(i, lhs.Int(i) >> rhs.Int(i));
				break;
			case spv::OpShiftLeftLogical:
				dst.emplace(i, lhs.UInt(i) << rhs.UInt(i));
				break;
			case spv::OpBitwiseOr:
			case spv::OpLogicalOr:
				dst.emplace(i, lhs.UInt(i) | rhs.UInt(i));
				break;
			case spv::OpBitwiseXor:
				dst.emplace(i, lhs.UInt(i) ^ rhs.UInt(i));
				break;
			case spv::OpBitwiseAnd:
			case spv::OpLogicalAnd:
				dst.emplace(i, lhs.UInt(i) & rhs.UInt(i));
				break;
			case spv::OpSMulExtended:
				// Extended ops: result is a structure containing two members of the same type as lhs & rhs.
				// In our flat view then, component i is the i'th component of the first member;
				// component i + N is the i'th component of the second member.
				dst.emplace(i, lhs.Int(i) * rhs.Int(i));
				dst.emplace(i + lhsType.sizeInComponents, MulHigh(lhs.Int(i), rhs.Int(i)));
				break;
			case spv::OpUMulExtended:
				dst.emplace(i, lhs.UInt(i) * rhs.UInt(i));
				dst.emplace(i + lhsType.sizeInComponents, MulHigh(lhs.UInt(i), rhs.UInt(i)));
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
		auto lhs = GenericValue(this, routine, insn.word(3));
		auto rhs = GenericValue(this, routine, insn.word(4));

		dst.emplace(0, Dot(lhsType.sizeInComponents, lhs, rhs));
	}

	void SpirvShader::EmitSelect(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto cond = GenericValue(this, routine, insn.word(3));
		auto lhs = GenericValue(this, routine, insn.word(4));
		auto rhs = GenericValue(this, routine, insn.word(5));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			dst.emplace(i, (cond.Int(i) & lhs.Int(i)) | (~cond.Int(i) & rhs.Int(i)));   // FIXME: IfThenElse()
		}
	}

	void SpirvShader::EmitExtendedInstruction(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto extInstIndex = static_cast<GLSLstd450>(insn.word(4));

		switch (extInstIndex)
		{
		case GLSLstd450FAbs:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Abs(src.Float(i)));
			}
			break;
		}
		case GLSLstd450SAbs:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Abs(src.Int(i)));
			}
			break;
		}
		case GLSLstd450Cross:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			dst.emplace(0, lhs.Float(1) * rhs.Float(2) - rhs.Float(1) * lhs.Float(2));
			dst.emplace(1, lhs.Float(2) * rhs.Float(0) - rhs.Float(2) * lhs.Float(0));
			dst.emplace(2, lhs.Float(0) * rhs.Float(1) - rhs.Float(0) * lhs.Float(1));
			break;
		}
		case GLSLstd450Floor:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Floor(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Trunc:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Trunc(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Ceil:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Ceil(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Fract:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Frac(src.Float(i)));
			}
			break;
		}
		case GLSLstd450Round:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Round(src.Float(i)));
			}
			break;
		}
		case GLSLstd450RoundEven:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				auto x = Round(src.Float(i));
				// dst = round(src) + ((round(src) < src) * 2 - 1) * (fract(src) == 0.5) * isOdd(round(src));
				dst.emplace(i, x + ((SIMD::Float(CmpLT(x, src.Float(i)) & SIMD::Int(1)) * SIMD::Float(2.0f)) - SIMD::Float(1.0f)) *
						SIMD::Float(CmpEQ(Frac(src.Float(i)), SIMD::Float(0.5f)) & SIMD::Int(1)) * SIMD::Float(Int4(x) & SIMD::Int(1)));
			}
			break;
		}
		case GLSLstd450FMin:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(lhs.Float(i), rhs.Float(i)));
			}
			break;
		}
		case GLSLstd450FMax:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Max(lhs.Float(i), rhs.Float(i)));
			}
			break;
		}
		case GLSLstd450SMin:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(lhs.Int(i), rhs.Int(i)));
			}
			break;
		}
		case GLSLstd450SMax:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Max(lhs.Int(i), rhs.Int(i)));
			}
			break;
		}
		case GLSLstd450UMin:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(lhs.UInt(i), rhs.UInt(i)));
			}
			break;
		}
		case GLSLstd450UMax:
		{
			auto lhs = GenericValue(this, routine, insn.word(5));
			auto rhs = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Max(lhs.UInt(i), rhs.UInt(i)));
			}
			break;
		}
		case GLSLstd450Step:
		{
			auto edge = GenericValue(this, routine, insn.word(5));
			auto x = GenericValue(this, routine, insn.word(6));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, CmpNLT(x.Float(i), edge.Float(i)) & As<SIMD::Int>(SIMD::Float(1.0f)));
			}
			break;
		}
		case GLSLstd450SmoothStep:
		{
			auto edge0 = GenericValue(this, routine, insn.word(5));
			auto edge1 = GenericValue(this, routine, insn.word(6));
			auto x = GenericValue(this, routine, insn.word(7));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				auto tx = Min(Max((x.Float(i) - edge0.Float(i)) /
						(edge1.Float(i) - edge0.Float(i)), SIMD::Float(0.0f)), SIMD::Float(1.0f));
				dst.emplace(i, tx * tx * (Float4(3.0f) - Float4(2.0f) * tx));
			}
			break;
		}
		case GLSLstd450FMix:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			auto y = GenericValue(this, routine, insn.word(6));
			auto a = GenericValue(this, routine, insn.word(7));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, a.Float(i) * (y.Float(i) - x.Float(i)) + x.Float(i));
			}
			break;
		}
		case GLSLstd450FClamp:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			auto minVal = GenericValue(this, routine, insn.word(6));
			auto maxVal = GenericValue(this, routine, insn.word(7));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(Max(x.Float(i), minVal.Float(i)), maxVal.Float(i)));
			}
			break;
		}
		case GLSLstd450SClamp:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			auto minVal = GenericValue(this, routine, insn.word(6));
			auto maxVal = GenericValue(this, routine, insn.word(7));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(Max(x.Int(i), minVal.Int(i)), maxVal.Int(i)));
			}
			break;
		}
		case GLSLstd450UClamp:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			auto minVal = GenericValue(this, routine, insn.word(6));
			auto maxVal = GenericValue(this, routine, insn.word(7));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, Min(Max(x.UInt(i), minVal.UInt(i)), maxVal.UInt(i)));
			}
			break;
		}
		case GLSLstd450FSign:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				auto neg = As<SIMD::Int>(CmpLT(src.Float(i), SIMD::Float(-0.0f))) & As<SIMD::Int>(SIMD::Float(-1.0f));
				auto pos = As<SIMD::Int>(CmpNLE(src.Float(i), SIMD::Float(+0.0f))) & As<SIMD::Int>(SIMD::Float(1.0f));
				dst.emplace(i, neg | pos);
			}
			break;
		}
		case GLSLstd450SSign:
		{
			auto src = GenericValue(this, routine, insn.word(5));
			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				auto neg = CmpLT(src.Int(i), SIMD::Int(0)) & SIMD::Int(-1);
				auto pos = CmpNLE(src.Int(i), SIMD::Int(0)) & SIMD::Int(1);
				dst.emplace(i, neg | pos);
			}
			break;
		}
		case GLSLstd450Reflect:
		{
			auto I = GenericValue(this, routine, insn.word(5));
			auto N = GenericValue(this, routine, insn.word(6));

			SIMD::Float d = Dot(type.sizeInComponents, I, N);

			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, I.Float(i) - SIMD::Float(2.0f) * d * N.Float(i));
			}
			break;
		}
		case GLSLstd450Refract:
		{
			auto I = GenericValue(this, routine, insn.word(5));
			auto N = GenericValue(this, routine, insn.word(6));
			auto eta = GenericValue(this, routine, insn.word(7));

			SIMD::Float d = Dot(type.sizeInComponents, I, N);
			SIMD::Float k = SIMD::Float(1.0f) - eta.Float(0) * eta.Float(0) * (SIMD::Float(1.0f) - d * d);
			SIMD::Int pos = CmpNLT(k, SIMD::Float(0.0f));
			SIMD::Float t = (eta.Float(0) * d + Sqrt(k));

			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, pos & As<SIMD::Int>(eta.Float(0) * I.Float(i) - t * N.Float(i)));
			}
			break;
		}
		case GLSLstd450FaceForward:
		{
			auto N = GenericValue(this, routine, insn.word(5));
			auto I = GenericValue(this, routine, insn.word(6));
			auto Nref = GenericValue(this, routine, insn.word(7));

			SIMD::Float d = Dot(type.sizeInComponents, I, Nref);
			SIMD::Int neg = CmpLT(d, SIMD::Float(0.0f));

			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				auto n = N.Float(i);
				dst.emplace(i, (neg & As<SIMD::Int>(n)) | (~neg & As<SIMD::Int>(-n)));
			}
			break;
		}
		case GLSLstd450Length:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			SIMD::Float d = Dot(getType(getObject(insn.word(5)).type).sizeInComponents, x, x);

			dst.emplace(0, Sqrt(d));
			break;
		}
		case GLSLstd450Normalize:
		{
			auto x = GenericValue(this, routine, insn.word(5));
			SIMD::Float d = Dot(getType(getObject(insn.word(5)).type).sizeInComponents, x, x);
			SIMD::Float invLength = SIMD::Float(1.0f) / Sqrt(d);

			for (auto i = 0u; i < type.sizeInComponents; i++)
			{
				dst.emplace(i, invLength * x.Float(i));
			}
			break;
		}
		case GLSLstd450Distance:
		{
			auto p0 = GenericValue(this, routine, insn.word(5));
			auto p1 = GenericValue(this, routine, insn.word(6));
			auto p0Type = getType(getObject(insn.word(5)).type);

			// sqrt(dot(p0-p1, p0-p1))
			SIMD::Float d = (p0.Float(0) - p1.Float(0)) * (p0.Float(0) - p1.Float(0));

			for (auto i = 1u; i < p0Type.sizeInComponents; i++)
			{
				d += (p0.Float(i) - p1.Float(i)) * (p0.Float(i) - p1.Float(i));
			}

			dst.emplace(0, Sqrt(d));
			break;
		}
		default:
			UNIMPLEMENTED("Unhandled ExtInst %d", extInstIndex);
		}
	}

	SIMD::Float SpirvShader::Dot(unsigned numComponents, GenericValue const & x, GenericValue const & y) const
	{
		SIMD::Float d = x.Float(0) * y.Float(0);

		for (auto i = 1u; i < numComponents; i++)
		{
			d += x.Float(i) * y.Float(i);
		}

		return d;
	}

	void SpirvShader::EmitAny(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		assert(type.sizeInComponents == 1);
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &srcType = getType(getObject(insn.word(3)).type);
		auto src = GenericValue(this, routine, insn.word(3));

		SIMD::UInt result = src.UInt(0);

		for (auto i = 1u; i < srcType.sizeInComponents; i++)
		{
			result |= src.UInt(i);
		}

		dst.emplace(0, result);
	}

	void SpirvShader::EmitAll(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto &type = getType(insn.word(1));
		assert(type.sizeInComponents == 1);
		auto &dst = routine->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &srcType = getType(getObject(insn.word(3)).type);
		auto src = GenericValue(this, routine, insn.word(3));

		SIMD::UInt result = src.UInt(0);

		for (auto i = 1u; i < srcType.sizeInComponents; i++)
		{
			result &= src.UInt(i);
		}

		dst.emplace(0, result);
	}

	void SpirvShader::EmitBranch(InsnIterator insn, SpirvRoutine *routine) const
	{
		auto blockId = Block::ID(insn.word(1));
		EmitBlock(routine, getBlock(blockId));
	}

	void SpirvShader::emitEpilog(SpirvRoutine *routine) const
	{
		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpVariable:
			{
				Object::ID resultId = insn.word(2);
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
