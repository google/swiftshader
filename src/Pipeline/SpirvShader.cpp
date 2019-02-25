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
#include "Device/Config.hpp"

namespace sw
{
	volatile int SpirvShader::serialCounter = 1;    // Start at 1, 0 is invalid shader.

	SpirvShader::SpirvShader(InsnStore const &insns)
			: insns{insns}, inputs{MAX_INTERFACE_COMPONENTS},
			  outputs{MAX_INTERFACE_COMPONENTS},
			  serialID{serialCounter++}, modes{}
	{
		// Simplifying assumptions (to be satisfied by earlier transformations)
		// - There is exactly one entrypoint in the module, and it's the one we want
		// - The only input/output OpVariables present are those used by the entrypoint

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
				break;
			}

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
				switch (storageClass)
				{
					case spv::StorageClassInput:
					case spv::StorageClassOutput:
						ProcessInterfaceVariable(object);
						break;
					default:
						UNIMPLEMENTED("Unhandled storage class %d for OpVariable", (int)storageClass);
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

			case spv::OpLoad:
			case spv::OpAccessChain:
				// Instructions that yield an ssavalue.
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
			case spv::OpReturn:
				// Don't need to do anything during analysis pass
				break;

			case spv::OpKill:
				modes.ContainsKill = true;
				break;

			default:
				printf("Warning: ignored opcode %u\n", insn.opcode());
				break;    // This is OK, these passes are intentionally partial
			}
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
		assert(objectTy.storageClass == spv::StorageClassInput || objectTy.storageClass == spv::StorageClassOutput);

		assert(objectTy.definition.opcode() == spv::OpTypePointer);
		auto pointeeTy = getType(objectTy.element);

		auto &builtinInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputBuiltins : outputBuiltins;
		auto &userDefinedInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputs : outputs;

		assert(object.definition.opcode() == spv::OpVariable);
		ObjectID resultId = object.definition.word(2);

		if (objectTy.isBuiltInBlock)
		{
			// walk the builtin block, registering each of its members separately.
			auto m = memberDecorations.find(objectTy.element);
			assert(m != memberDecorations.end());        // otherwise we wouldn't have marked the type chain
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
							   assert(scalarSlot >= 0 &&
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
		assert(def.opcode() == spv::OpVariable);
		VisitInterfaceInner<F>(def.word(1), d, f);
	}

	Int4 SpirvShader::WalkAccessChain(ObjectID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const
	{
		// TODO: think about explicit layout (UBO/SSBO) storage classes
		// TODO: avoid doing per-lane work in some cases if we can?

		int constantOffset = 0;
		Int4 dynamicOffset = Int4(0);
		auto &baseObject = getObject(id);
		TypeID typeId = getType(baseObject.type).element;

		// The <base> operand is an intermediate value itself, ie produced by a previous OpAccessChain.
		// Start with its offset and build from there.
		if (baseObject.kind == Object::Kind::Value)
			dynamicOffset += As<Int4>(routine->getIntermediate(id)[0]);

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
					dynamicOffset += Int4(stride) * As<Int4>(routine->getIntermediate(indexIds[i])[0]);
				typeId = type.element;
				break;
			}

			case spv::OpTypePointer:
				typeId = type.element;
				break;

			default:
				UNIMPLEMENTED("Unexpected type in WalkAccessChain");
			}
		}

		return dynamicOffset + Int4(constantOffset);
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
		assert(insn.opcode() == spv::OpConstant);
		assert(getType(insn.word(1)).definition.opcode() == spv::OpTypeInt);
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

			case spv::OpVariable:
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
				break;
			}
			case spv::OpLoad:
			{
				ObjectID objectId = insn.word(2);
				ObjectID pointerId = insn.word(3);
				auto &object = getObject(objectId);
				auto &objectTy = getType(object.type);
				auto &pointer = getObject(pointerId);
				auto &pointerTy = getType(pointer.type);
				routine->createIntermediate(objectId, objectTy.sizeInComponents);
				auto &pointerBase = getObject(pointer.pointerBase);
				auto &pointerBaseTy = getType(pointerBase.type);

				assert(pointerTy.element == object.type);
				assert(TypeID(insn.word(1)) == object.type);

				if (pointerBaseTy.storageClass == spv::StorageClassImage ||
					pointerBaseTy.storageClass == spv::StorageClassUniform ||
					pointerBaseTy.storageClass == spv::StorageClassUniformConstant)
				{
					UNIMPLEMENTED("Descriptor-backed load not yet implemented");
				}

				auto &ptrBase = routine->getValue(pointer.pointerBase);
				auto &dst = routine->getIntermediate(objectId);

				if (pointer.kind == Object::Kind::Value)
				{
					auto offsets = As<Int4>(routine->getIntermediate(insn.word(3))[0]);
					for (auto i = 0u; i < objectTy.sizeInComponents; i++)
					{
						// i wish i had a Float,Float,Float,Float constructor here..
						Float4 v;
						for (int j = 0; j < 4; j++)
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
				break;
			}
			case spv::OpAccessChain:
			{
				TypeID typeId = insn.word(1);
				ObjectID objectId = insn.word(2);
				ObjectID baseId = insn.word(3);
				auto &object = getObject(objectId);
				auto &type = getType(typeId);
				auto &base = getObject(baseId);
				routine->createIntermediate(objectId, type.sizeInComponents);
				auto &pointerBase = getObject(object.pointerBase);
				auto &pointerBaseTy = getType(pointerBase.type);
				assert(type.sizeInComponents == 1);
				assert(base.pointerBase == object.pointerBase);

				if (pointerBaseTy.storageClass == spv::StorageClassImage ||
					pointerBaseTy.storageClass == spv::StorageClassUniform ||
					pointerBaseTy.storageClass == spv::StorageClassUniformConstant)
				{
					UNIMPLEMENTED("Descriptor-backed OpAccessChain not yet implemented");
				}
				auto &dst = routine->getIntermediate(objectId);
				dst.emplace(0, As<Float4>(WalkAccessChain(baseId, insn.wordCount() - 4, insn.wordPointer(4), routine)));
				break;
			}
			case spv::OpStore:
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
						auto offsets = As<Int4>(routine->getIntermediate(pointerId)[0]);
						for (auto i = 0u; i < elementTy.sizeInComponents; i++)
						{
							// Scattered store
							for (int j = 0; j < 4; j++)
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
							ptrBase[i] = RValue<Float4>(src[i]);
						}
					}
				}
				else
				{
					auto &src = routine->getIntermediate(objectId);

					if (pointer.kind == Object::Kind::Value)
					{
						auto offsets = As<Int4>(routine->getIntermediate(pointerId)[0]);
						for (auto i = 0u; i < elementTy.sizeInComponents; i++)
						{
							// Scattered store
							for (int j = 0; j < 4; j++)
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
				break;
			}
			default:
				printf("emit: ignoring opcode %d\n", insn.opcode());
				break;
			}
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
}
