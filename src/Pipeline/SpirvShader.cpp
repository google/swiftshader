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
				auto targetId = insn.word(1);
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
				auto targetId = insn.word(1);
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
				auto resultId = insn.word(1);
				auto &object = types[resultId];
				object.kind = Object::Kind::Type;
				object.definition = insn;
				object.sizeInComponents = ComputeTypeSize(insn);

				// A structure is a builtin block if it has a builtin
				// member. All members of such a structure are builtins.
				if (insn.opcode() == spv::OpTypeStruct)
				{
					auto d = memberDecorations.find(resultId);
					if (d != memberDecorations.end())
					{
						for (auto &m : d->second)
						{
							if (m.HasBuiltIn)
							{
								object.isBuiltInBlock = true;
								break;
							}
						}
					}
				}
				else if (insn.opcode() == spv::OpTypePointer)
				{
					auto pointeeType = insn.word(3);
					object.isBuiltInBlock = getType(pointeeType).isBuiltInBlock;
				}
				break;
			}

			case spv::OpVariable:
			{
				auto typeId = insn.word(1);
				auto resultId = insn.word(2);
				auto storageClass = static_cast<spv::StorageClass>(insn.word(3));
				if (insn.wordCount() > 4)
					UNIMPLEMENTED("Variable initializers not yet supported");

				auto &object = defs[resultId];
				object.kind = Object::Kind::Variable;
				object.definition = insn;
				object.storageClass = storageClass;

				auto &type = getType(typeId);
				auto &pointeeType = getType(type.definition.word(3));

				// OpVariable's "size" is the size of the allocation required (the size of the pointee)
				object.sizeInComponents = pointeeType.sizeInComponents;
				object.isBuiltInBlock = type.isBuiltInBlock;
				object.pointerBase = insn.word(2);	// base is itself

				// Register builtins

				if (storageClass == spv::StorageClassInput || storageClass == spv::StorageClassOutput)
				{
					ProcessInterfaceVariable(object);
				}
				break;
			}

			case spv::OpConstant:
			case spv::OpConstantComposite:
			case spv::OpConstantFalse:
			case spv::OpConstantTrue:
			case spv::OpConstantNull:
			{
				auto typeId = insn.word(1);
				auto resultId = insn.word(2);
				auto &object = defs[resultId];
				object.kind = Object::Kind::Constant;
				object.definition = insn;
				object.sizeInComponents = getType(typeId).sizeInComponents;
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
				auto typeId = insn.word(1);
				auto resultId = insn.word(2);
				auto &object = defs[resultId];
				object.kind = Object::Kind::Value;
				object.definition = insn;
				object.sizeInComponents = getType(typeId).sizeInComponents;

				if (insn.opcode() == spv::OpAccessChain)
				{
					// interior ptr has two parts:
					// - logical base ptr, common across all lanes and known at compile time
					// - per-lane offset
					object.pointerBase = getObject(insn.word(3)).pointerBase;
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

	void SpirvShader::ProcessInterfaceVariable(Object &object)
	{
		assert(object.storageClass == spv::StorageClassInput || object.storageClass == spv::StorageClassOutput);

		auto &builtinInterface = (object.storageClass == spv::StorageClassInput) ? inputBuiltins : outputBuiltins;
		auto &userDefinedInterface = (object.storageClass == spv::StorageClassInput) ? inputs : outputs;

		auto resultId = object.definition.word(2);
		if (object.isBuiltInBlock)
		{
			// walk the builtin block, registering each of its members separately.
			auto ptrType = getType(object.definition.word(1)).definition;
			assert(ptrType.opcode() == spv::OpTypePointer);
			auto pointeeType = ptrType.word(3);
			auto m = memberDecorations.find(pointeeType);
			assert(m != memberDecorations.end());        // otherwise we wouldn't have marked the type chain
			auto &structType = getType(pointeeType).definition;
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
			builtinInterface[d->second.BuiltIn] = {resultId, 0, object.sizeInComponents};
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
	int SpirvShader::VisitInterfaceInner(uint32_t id, Decorations d, F f) const
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
	void SpirvShader::VisitInterface(uint32_t id, F f) const
	{
		// Walk a variable definition and call f for each component in it.
		Decorations d{};
		ApplyDecorationsForId(&d, id);

		auto def = getObject(id).definition;
		assert(def.opcode() == spv::OpVariable);
		VisitInterfaceInner<F>(def.word(1), d, f);
	}

	Int4 SpirvShader::WalkAccessChain(uint32_t id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const
	{
		// TODO: think about decorations, to make this work on location based interfaces
		// TODO: think about explicit layout (UBO/SSBO) storage classes
		// TODO: avoid doing per-lane work in some cases if we can?

		Int4 res = Int4(0);
		auto & baseObject = getObject(id);
		auto typeId = baseObject.definition.word(1);

		if (baseObject.kind == Object::Kind::Value)
			res += As<Int4>(routine->getValue(id)[0]);

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
					offsetIntoStruct += getType(type.definition.word(2 + memberIndex)).sizeInComponents;
				}
				res += Int4(offsetIntoStruct);
				break;
			}

			case spv::OpTypeVector:
			case spv::OpTypeMatrix:
			case spv::OpTypeArray:
			{
				auto stride = getType(type.definition.word(2)).sizeInComponents;
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
					res += Int4(stride * GetConstantInt(indexIds[i]));
				else
					res += Int4(stride) * As<Int4>(routine->getValue(indexIds[i])[0]);
				break;
			}

			default:
				UNIMPLEMENTED("Unexpected type in WalkAccessChain");
			}
		}

		return res;
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

	void SpirvShader::ApplyDecorationsForId(Decorations *d, uint32_t id) const
	{
		auto it = decorations.find(id);
		if (it != decorations.end())
			d->Apply(it->second);
	}

	void SpirvShader::ApplyDecorationsForIdMember(Decorations *d, uint32_t id, uint32_t member) const
	{
		auto it = memberDecorations.find(id);
		if (it != memberDecorations.end() && member < it->second.size())
		{
			d->Apply(it->second[member]);
		}
	}

	uint32_t SpirvShader::GetConstantInt(uint32_t id) const
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

	void SpirvShader::emitEarly(SpirvRoutine *routine) const
	{
		for (auto insn : *this)
		{
			switch (insn.opcode())
			{
			case spv::OpVariable:
			{
				auto &object = getObject(insn.word(2));
				// Want to exclude: location-oriented interface variables; special things that consume zero slots.
				// TODO: what to do about zero-slot objects?
				if (object.kind != Object::Kind::InterfaceVariable && object.sizeInComponents > 0)
				{
					// any variable not in a location-oriented interface
					routine->createLvalue(insn.word(2), object.sizeInComponents);
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
			case spv::OpLoad:
			{
				auto &object = getObject(insn.word(2));
				auto &type = getType(insn.word(1));
				auto &pointer = getObject(insn.word(3));
				routine->createLvalue(insn.word(2), type.sizeInComponents);		// TODO: this should be an ssavalue!
				auto &pointerBase = getObject(pointer.pointerBase);

				if (pointerBase.kind == Object::Kind::InterfaceVariable)
				{
					UNIMPLEMENTED("Location-based load not yet implemented");
				}

				if (pointerBase.storageClass == spv::StorageClassImage ||
					pointerBase.storageClass == spv::StorageClassUniform ||
					pointerBase.storageClass == spv::StorageClassUniformConstant)
				{
					UNIMPLEMENTED("Descriptor-backed load not yet implemented");
				}

				SpirvRoutine::Value& ptrBase = routine->getValue(pointer.pointerBase);
				auto & dst = routine->getValue(insn.word(2));

				if (pointer.kind == Object::Kind::Value)
				{
					auto offsets = As<Int4>(routine->getValue(insn.word(3)));
					for (auto i = 0u; i < object.sizeInComponents; i++)
					{
						// i wish i had a Float,Float,Float,Float constructor here..
						Float4 v;
						for (int j = 0; j < 4; j++)
							v = Insert(v, Extract(ptrBase[Int(i) + Extract(offsets, j)], j), j);
						dst[i] = v;
					}
				}
				else
				{
					// no divergent offsets to worry about
					for (auto i = 0u; i < object.sizeInComponents; i++)
					{
						dst[i] = ptrBase[i];
					}
				}
				break;
			}
			case spv::OpAccessChain:
			{
				auto &object = getObject(insn.word(2));
				auto &type = getType(insn.word(1));
				auto &base = getObject(insn.word(3));
				routine->createLvalue(insn.word(2), type.sizeInComponents);		// TODO: this should be an ssavalue!
				auto &pointerBase = getObject(object.pointerBase);
				assert(type.sizeInComponents == 1);
				assert(base.pointerBase == object.pointerBase);

				if (pointerBase.kind == Object::Kind::InterfaceVariable)
				{
					UNIMPLEMENTED("Location-based OpAccessChain not yet implemented");
				}

				if (pointerBase.storageClass == spv::StorageClassImage ||
					pointerBase.storageClass == spv::StorageClassUniform ||
					pointerBase.storageClass == spv::StorageClassUniformConstant)
				{
					UNIMPLEMENTED("Descriptor-backed OpAccessChain not yet implemented");
				}

				auto & dst = routine->getValue(insn.word(2));
				dst[0] = As<Float4>(WalkAccessChain(insn.word(3), insn.wordCount() - 4, insn.wordPointer(4), routine));
				break;
			}
			case spv::OpStore:
			{
				auto &object = getObject(insn.word(2));
				auto &pointer = getObject(insn.word(1));
				auto &pointerBase = getObject(pointer.pointerBase);

				if (pointerBase.kind == Object::Kind::InterfaceVariable)
				{
					UNIMPLEMENTED("Location-based store not yet implemented");
				}

				if (pointerBase.storageClass == spv::StorageClassImage ||
					pointerBase.storageClass == spv::StorageClassUniform ||
					pointerBase.storageClass == spv::StorageClassUniformConstant)
				{
					UNIMPLEMENTED("Descriptor-backed store not yet implemented");
				}

				SpirvRoutine::Value& ptrBase = routine->getValue(pointer.pointerBase);
				auto & src = routine->getValue(insn.word(2));;

				if (pointer.kind == Object::Kind::Value)
				{
					auto offsets = As<Int4>(routine->getValue(insn.word(1)));
					for (auto i = 0u; i < object.sizeInComponents; i++)
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
					for (auto i = 0u; i < object.sizeInComponents; i++)
					{
						ptrBase[i] = src[i];
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
}
