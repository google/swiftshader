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
#include "System/Debug.hpp"
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
				decorations[targetId].Apply(
						static_cast<spv::Decoration>(insn.word(2)),
						insn.wordCount() > 3 ? insn.word(3) : 0);
				break;
			}

			case spv::OpMemberDecorate:
			{
				auto targetId = insn.word(1);
				auto memberIndex = insn.word(2);
				auto &d = memberDecorations[targetId];
				if (memberIndex >= d.size())
					d.resize(memberIndex + 1);    // on demand; exact size would require another pass...
				d[memberIndex].Apply(
						static_cast<spv::Decoration>(insn.word(3)),
						insn.wordCount() > 4 ? insn.word(4) : 0);
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

				object.sizeInComponents = type.sizeInComponents;
				object.isBuiltInBlock = type.isBuiltInBlock;

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
				// Due to preprocessing, the entrypoint provides no value.
				break;

			default:
				break;    // This is OK, these passes are intentionally partial
			}
		}
	}

	void SpirvShader::ProcessInterfaceVariable(Object const &object)
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
			PopulateInterface(&userDefinedInterface, resultId);
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
			// Pointer 'size' is just pointee size
			// TODO: this isn't really correct. we should look through pointers as appropriate.
			return getType(insn.word(3)).sizeInComponents;

		default:
			// Some other random insn.
			UNIMPLEMENTED("Only types are supported");
		}
	}

	void SpirvShader::PopulateInterfaceSlot(std::vector<InterfaceComponent> *iface, Decorations const &d, AttribType type)
	{
		// Populate a single scalar slot in the interface from a collection of decorations and the intended component type.
		auto scalarSlot = (d.Location << 2) | d.Component;

		auto &slot = (*iface)[scalarSlot];
		slot.Type = type;
		slot.Flat = d.Flat;
		slot.NoPerspective = d.NoPerspective;
		slot.Centroid = d.Centroid;
	}

	int SpirvShader::PopulateInterfaceInner(std::vector<InterfaceComponent> *iface, uint32_t id, Decorations d)
	{
		// Recursively walks variable definition and its type tree, taking into account
		// any explicit Location or Component decorations encountered; where explicit
		// Locations or Components are not specified, assigns them sequentially.
		// Collected decorations are carried down toward the leaves and across
		// siblings; Effect of decorations intentionally does not flow back up the tree.
		//
		// Returns the next available location.

		// This covers the rules in Vulkan 1.1 spec, 14.1.4 Location Assignment.

		auto const it = decorations.find(id);
		if (it != decorations.end())
		{
			d.Apply(it->second);
		}

		auto const &obj = getType(id);
		switch (obj.definition.opcode())
		{
		case spv::OpVariable:
			return PopulateInterfaceInner(iface, obj.definition.word(1), d);
		case spv::OpTypePointer:
			return PopulateInterfaceInner(iface, obj.definition.word(3), d);
		case spv::OpTypeMatrix:
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Location++)
			{
				// consumes same components of N consecutive locations
				PopulateInterfaceInner(iface, obj.definition.word(2), d);
			}
			return d.Location;
		case spv::OpTypeVector:
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Component++)
			{
				// consumes N consecutive components in the same location
				PopulateInterfaceInner(iface, obj.definition.word(2), d);
			}
			return d.Location + 1;
		case spv::OpTypeFloat:
			PopulateInterfaceSlot(iface, d, ATTRIBTYPE_FLOAT);
			return d.Location + 1;
		case spv::OpTypeInt:
			PopulateInterfaceSlot(iface, d, obj.definition.word(3) ? ATTRIBTYPE_INT : ATTRIBTYPE_UINT);
			return d.Location + 1;
		case spv::OpTypeBool:
			PopulateInterfaceSlot(iface, d, ATTRIBTYPE_UINT);
			return d.Location + 1;
		case spv::OpTypeStruct:
		{
			auto const memberDecorationsIt = memberDecorations.find(id);
			// iterate over members, which may themselves have Location/Component decorations
			for (auto i = 0u; i < obj.definition.wordCount() - 2; i++)
			{
				// Apply any member decorations for this member to the carried state.
				if (memberDecorationsIt != memberDecorations.end() && i < memberDecorationsIt->second.size())
				{
					d.Apply(memberDecorationsIt->second[i]);
				}
				d.Location = PopulateInterfaceInner(iface, obj.definition.word(i + 2), d);
				d.Component = 0;    // Implicit locations always have component=0
			}
			return d.Location;
		}
		case spv::OpTypeArray:
		{
			auto arraySize = GetConstantInt(obj.definition.word(3));
			for (auto i = 0u; i < arraySize; i++)
			{
				d.Location = PopulateInterfaceInner(iface, obj.definition.word(2), d);
			}
			return d.Location;
		}
		default:
			// Intentionally partial; most opcodes do not participate in type hierarchies
			return 0;
		}
	}

	void SpirvShader::PopulateInterface(std::vector<InterfaceComponent> *iface, uint32_t id)
	{
		// Walk a variable definition and populate the interface from it.
		Decorations d{};
		PopulateInterfaceInner(iface, id, d);
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

	uint32_t SpirvShader::GetConstantInt(uint32_t id)
	{
		// Slightly hackish access to constants very early in translation.
		// General consumption of constants by other instructions should
		// probably be just lowered to Reactor.

		// TODO: not encountered yet since we only use this for array sizes etc,
		// but is possible to construct integer constant 0 via OpConstantNull.
		auto insn = defs[id].definition;
		assert(insn.opcode() == spv::OpConstant);
		assert(getType(insn.word(1)).definition.opcode() == spv::OpTypeInt);
		return insn.word(3);
	}
}
