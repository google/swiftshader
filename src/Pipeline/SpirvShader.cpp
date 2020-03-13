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

#include "SpirvShader.hpp"

#include "System/Debug.hpp"
#include "Vulkan/VkPipelineLayout.hpp"
#include "Vulkan/VkRenderPass.hpp"

#include "marl/defer.h"

#include <spirv/unified1/spirv.hpp>

namespace sw {

SpirvShader::SpirvShader(
    uint32_t codeSerialID,
    VkShaderStageFlagBits pipelineStage,
    const char *entryPointName,
    InsnStore const &insns,
    const vk::RenderPass *renderPass,
    uint32_t subpassIndex,
    bool robustBufferAccess,
    const std::shared_ptr<vk::dbg::Context> &dbgctx)
    : insns{ insns }
    , inputs{ MAX_INTERFACE_COMPONENTS }
    , outputs{ MAX_INTERFACE_COMPONENTS }
    , codeSerialID(codeSerialID)
    , robustBufferAccess(robustBufferAccess)
{
	ASSERT(insns.size() > 0);

	if(dbgctx)
	{
		dbgInit(dbgctx);
	}

	if(renderPass)
	{
		// capture formats of any input attachments present
		auto subpass = renderPass->getSubpass(subpassIndex);
		inputAttachmentFormats.reserve(subpass.inputAttachmentCount);
		for(auto i = 0u; i < subpass.inputAttachmentCount; i++)
		{
			auto attachmentIndex = subpass.pInputAttachments[i].attachment;
			inputAttachmentFormats.push_back(attachmentIndex != VK_ATTACHMENT_UNUSED
			                                     ? renderPass->getAttachment(attachmentIndex).format
			                                     : VK_FORMAT_UNDEFINED);
		}
	}

	// Simplifying assumptions (to be satisfied by earlier transformations)
	// - The only input/output OpVariables present are those used by the entrypoint

	Function::ID currentFunction;
	Block::ID currentBlock;
	InsnIterator blockStart;

	for(auto insn : *this)
	{
		spv::Op opcode = insn.opcode();

		switch(opcode)
		{
			case spv::OpEntryPoint:
			{
				executionModel = spv::ExecutionModel(insn.word(1));
				auto id = Function::ID(insn.word(2));
				auto name = insn.string(3);
				auto stage = executionModelToStage(executionModel);
				if(stage == pipelineStage && strcmp(name, entryPointName) == 0)
				{
					ASSERT_MSG(entryPoint == 0, "Duplicate entry point with name '%s' and stage %d", name, int(stage));
					entryPoint = id;
				}
				break;
			}

			case spv::OpExecutionMode:
				ProcessExecutionMode(insn);
				break;

			case spv::OpDecorate:
			{
				TypeOrObjectID targetId = insn.word(1);
				auto decoration = static_cast<spv::Decoration>(insn.word(2));
				uint32_t value = insn.wordCount() > 3 ? insn.word(3) : 0;

				decorations[targetId].Apply(decoration, value);

				switch(decoration)
				{
					case spv::DecorationDescriptorSet:
						descriptorDecorations[targetId].DescriptorSet = value;
						break;
					case spv::DecorationBinding:
						descriptorDecorations[targetId].Binding = value;
						break;
					case spv::DecorationInputAttachmentIndex:
						descriptorDecorations[targetId].InputAttachmentIndex = value;
						break;
					default:
						// Only handling descriptor decorations here.
						break;
				}

				if(decoration == spv::DecorationCentroid)
					modes.NeedsCentroid = true;
				break;
			}

			case spv::OpMemberDecorate:
			{
				Type::ID targetId = insn.word(1);
				auto memberIndex = insn.word(2);
				auto decoration = static_cast<spv::Decoration>(insn.word(3));
				uint32_t value = insn.wordCount() > 4 ? insn.word(4) : 0;

				auto &d = memberDecorations[targetId];
				if(memberIndex >= d.size())
					d.resize(memberIndex + 1);  // on demand; exact size would require another pass...

				d[memberIndex].Apply(decoration, value);

				if(decoration == spv::DecorationCentroid)
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
				uint32_t group = insn.word(1);
				auto const &groupDecorations = decorations[group];
				auto const &descriptorGroupDecorations = descriptorDecorations[group];
				for(auto i = 2u; i < insn.wordCount(); i++)
				{
					// Remaining operands are targets to apply the group to.
					uint32_t target = insn.word(i);
					decorations[target].Apply(groupDecorations);
					descriptorDecorations[target].Apply(descriptorGroupDecorations);
				}

				break;
			}

			case spv::OpGroupMemberDecorate:
			{
				auto const &srcDecorations = decorations[insn.word(1)];
				for(auto i = 2u; i < insn.wordCount(); i += 2)
				{
					// remaining operands are pairs of <id>, literal for members to apply to.
					auto &d = memberDecorations[insn.word(i)];
					auto memberIndex = insn.word(i + 1);
					if(memberIndex >= d.size())
						d.resize(memberIndex + 1);  // on demand resize, see above...
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
				ASSERT(currentFunction.value() != 0);

				auto blockEnd = insn;
				blockEnd++;
				functions[currentFunction].blocks[currentBlock] = Block(blockStart, blockEnd);
				currentBlock = Block::ID(0);

				if(opcode == spv::OpKill)
				{
					modes.ContainsKill = true;
				}
				break;
			}

			case spv::OpLoopMerge:
			case spv::OpSelectionMerge:
				break;  // Nothing to do in analysis pass.

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

				auto &object = defs[resultId];
				object.kind = Object::Kind::Pointer;
				object.definition = insn;
				object.type = typeId;

				ASSERT(getType(typeId).definition.opcode() == spv::OpTypePointer);
				ASSERT(getType(typeId).storageClass == storageClass);

				switch(storageClass)
				{
					case spv::StorageClassInput:
					case spv::StorageClassOutput:
						ProcessInterfaceVariable(object);
						break;

					case spv::StorageClassUniform:
					case spv::StorageClassStorageBuffer:
						object.kind = Object::Kind::DescriptorSet;
						break;

					case spv::StorageClassPushConstant:
					case spv::StorageClassPrivate:
					case spv::StorageClassFunction:
					case spv::StorageClassUniformConstant:
						break;  // Correctly handled.

					case spv::StorageClassWorkgroup:
					{
						auto &elTy = getType(getType(typeId).element);
						auto sizeInBytes = elTy.sizeInComponents * static_cast<uint32_t>(sizeof(float));
						workgroupMemory.allocate(resultId, sizeInBytes);
						object.kind = Object::Kind::Pointer;
						break;
					}
					case spv::StorageClassAtomicCounter:
					case spv::StorageClassImage:
						UNSUPPORTED("StorageClass %d not yet supported", (int)storageClass);
						break;

					case spv::StorageClassCrossWorkgroup:
						UNSUPPORTED("SPIR-V OpenCL Execution Model (StorageClassCrossWorkgroup)");
						break;

					case spv::StorageClassGeneric:
						UNSUPPORTED("SPIR-V GenericPointer Capability (StorageClassGeneric)");
						break;

					default:
						UNREACHABLE("Unexpected StorageClass %d", storageClass);  // See Appendix A of the Vulkan spec.
						break;
				}
				break;
			}

			case spv::OpConstant:
			case spv::OpSpecConstant:
				CreateConstant(insn).constantValue[0] = insn.word(3);
				break;
			case spv::OpConstantFalse:
			case spv::OpSpecConstantFalse:
				CreateConstant(insn).constantValue[0] = 0;  // Represent Boolean false as zero.
				break;
			case spv::OpConstantTrue:
			case spv::OpSpecConstantTrue:
				CreateConstant(insn).constantValue[0] = ~0u;  // Represent Boolean true as all bits set.
				break;
			case spv::OpConstantNull:
			case spv::OpUndef:
			{
				// TODO: consider a real LLVM-level undef. For now, zero is a perfectly good value.
				// OpConstantNull forms a constant of arbitrary type, all zeros.
				auto &object = CreateConstant(insn);
				auto &objectTy = getType(object.type);
				for(auto i = 0u; i < objectTy.sizeInComponents; i++)
				{
					object.constantValue[i] = 0;
				}
				break;
			}
			case spv::OpConstantComposite:
			case spv::OpSpecConstantComposite:
			{
				auto &object = CreateConstant(insn);
				auto offset = 0u;
				for(auto i = 0u; i < insn.wordCount() - 3; i++)
				{
					auto &constituent = getObject(insn.word(i + 3));
					auto &constituentTy = getType(constituent.type);
					for(auto j = 0u; j < constituentTy.sizeInComponents; j++)
					{
						object.constantValue[offset++] = constituent.constantValue[j];
					}
				}

				auto objectId = Object::ID(insn.word(2));
				auto decorationsIt = decorations.find(objectId);
				if(decorationsIt != decorations.end() &&
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
			case spv::OpSpecConstantOp:
				EvalSpecConstantOp(insn);
				break;

			case spv::OpCapability:
			{
				auto capability = static_cast<spv::Capability>(insn.word(1));
				switch(capability)
				{
					case spv::CapabilityMatrix: capabilities.Matrix = true; break;
					case spv::CapabilityShader: capabilities.Shader = true; break;
					case spv::CapabilityClipDistance: capabilities.ClipDistance = true; break;
					case spv::CapabilityCullDistance: capabilities.CullDistance = true; break;
					case spv::CapabilityInputAttachment: capabilities.InputAttachment = true; break;
					case spv::CapabilitySampled1D: capabilities.Sampled1D = true; break;
					case spv::CapabilityImage1D: capabilities.Image1D = true; break;
					case spv::CapabilityImageCubeArray: capabilities.ImageCubeArray = true; break;
					case spv::CapabilitySampledBuffer: capabilities.SampledBuffer = true; break;
					case spv::CapabilitySampledCubeArray: capabilities.SampledCubeArray = true; break;
					case spv::CapabilityImageBuffer: capabilities.ImageBuffer = true; break;
					case spv::CapabilityStorageImageExtendedFormats: capabilities.StorageImageExtendedFormats = true; break;
					case spv::CapabilityImageQuery: capabilities.ImageQuery = true; break;
					case spv::CapabilityDerivativeControl: capabilities.DerivativeControl = true; break;
					case spv::CapabilityGroupNonUniform: capabilities.GroupNonUniform = true; break;
					case spv::CapabilityGroupNonUniformVote: capabilities.GroupNonUniformVote = true; break;
					case spv::CapabilityGroupNonUniformArithmetic: capabilities.GroupNonUniformArithmetic = true; break;
					case spv::CapabilityGroupNonUniformBallot: capabilities.GroupNonUniformBallot = true; break;
					case spv::CapabilityGroupNonUniformShuffle: capabilities.GroupNonUniformShuffle = true; break;
					case spv::CapabilityGroupNonUniformShuffleRelative: capabilities.GroupNonUniformShuffleRelative = true; break;
					case spv::CapabilityDeviceGroup: capabilities.DeviceGroup = true; break;
					case spv::CapabilityMultiView: capabilities.MultiView = true; break;
					case spv::CapabilityStencilExportEXT: capabilities.StencilExportEXT = true; break;
					default:
						UNSUPPORTED("Unsupported capability %u", insn.word(1));
				}
				break;  // Various capabilities will be declared, but none affect our code generation at this point.
			}

			case spv::OpMemoryModel:
				break;  // Memory model does not affect our code generation until we decide to do Vulkan Memory Model support.

			case spv::OpFunction:
			{
				auto functionId = Function::ID(insn.word(2));
				ASSERT_MSG(currentFunction == 0, "Functions %d and %d overlap", currentFunction.value(), functionId.value());
				currentFunction = functionId;
				auto &function = functions[functionId];
				function.result = Type::ID(insn.word(1));
				function.type = Type::ID(insn.word(4));
				// Scan forward to find the function's label.
				for(auto it = insn; it != end(); it++)
				{
					if(it.opcode() == spv::OpLabel)
					{
						function.entry = Block::ID(it.word(1));
						break;
					}
				}
				ASSERT_MSG(function.entry != 0, "Function<%d> has no label", currentFunction.value());
				break;
			}

			case spv::OpFunctionEnd:
				currentFunction = 0;
				break;

			case spv::OpExtInstImport:
			{
				static constexpr std::pair<const char *, Extension::Name> extensionsByName[] = {
					{ "GLSL.std.450", Extension::GLSLstd450 },
					{ "OpenCL.DebugInfo.100", Extension::OpenCLDebugInfo100 },
				};
				static constexpr auto extensionCount = sizeof(extensionsByName) / sizeof(extensionsByName[0]);

				auto id = Extension::ID(insn.word(1));
				auto name = insn.string(2);
				auto ext = Extension{ Extension::Unknown };
				for(size_t i = 0; i < extensionCount; i++)
				{
					if(0 == strcmp(name, extensionsByName[i].first))
					{
						ext = Extension{ extensionsByName[i].second };
						break;
					}
				}
				if(ext.name == Extension::Unknown)
				{
					UNSUPPORTED("SPIR-V Extension: %s", name);
					break;
				}
				extensionsByID.emplace(id, ext);
				extensionsImported.emplace(ext.name);
				break;
			}
			case spv::OpName:
			case spv::OpMemberName:
			case spv::OpSource:
			case spv::OpSourceContinued:
			case spv::OpSourceExtension:
			case spv::OpLine:
			case spv::OpNoLine:
			case spv::OpModuleProcessed:
				// No semantic impact
				break;

			case spv::OpString:
				strings.emplace(insn.word(1), insn.string(2));
				break;

			case spv::OpFunctionParameter:
				// These should have all been removed by preprocessing passes. If we see them here,
				// our assumptions are wrong and we will probably generate wrong code.
				UNREACHABLE("%s should have already been lowered.", OpcodeName(opcode).c_str());
				break;

			case spv::OpFunctionCall:
				// TODO(b/141246700): Add full support for spv::OpFunctionCall
				break;

			case spv::OpFConvert:
				UNSUPPORTED("SPIR-V Float16 or Float64 Capability (OpFConvert)");
				break;

			case spv::OpSConvert:
				UNSUPPORTED("SPIR-V Int16 or Int64 Capability (OpSConvert)");
				break;

			case spv::OpUConvert:
				UNSUPPORTED("SPIR-V Int16 or Int64 Capability (OpUConvert)");
				break;

			case spv::OpLoad:
			case spv::OpAccessChain:
			case spv::OpInBoundsAccessChain:
			case spv::OpSampledImage:
			case spv::OpImage:
			{
				// Propagate the descriptor decorations to the result.
				Object::ID resultId = insn.word(2);
				Object::ID pointerId = insn.word(3);
				const auto &d = descriptorDecorations.find(pointerId);

				if(d != descriptorDecorations.end())
				{
					descriptorDecorations[resultId] = d->second;
				}

				DefineResult(insn);

				if(opcode == spv::OpAccessChain || opcode == spv::OpInBoundsAccessChain)
				{
					Decorations dd{};
					ApplyDecorationsForAccessChain(&dd, &descriptorDecorations[resultId], pointerId, insn.wordCount() - 4, insn.wordPointer(4));
					// Note: offset is the one thing that does *not* propagate, as the access chain accounts for it.
					dd.HasOffset = false;
					decorations[resultId].Apply(dd);
				}
			}
			break;

			case spv::OpCompositeConstruct:
			case spv::OpCompositeInsert:
			case spv::OpCompositeExtract:
			case spv::OpVectorShuffle:
			case spv::OpVectorTimesScalar:
			case spv::OpMatrixTimesScalar:
			case spv::OpMatrixTimesVector:
			case spv::OpVectorTimesMatrix:
			case spv::OpMatrixTimesMatrix:
			case spv::OpOuterProduct:
			case spv::OpTranspose:
			case spv::OpVectorExtractDynamic:
			case spv::OpVectorInsertDynamic:
			// Unary ops
			case spv::OpNot:
			case spv::OpBitFieldInsert:
			case spv::OpBitFieldSExtract:
			case spv::OpBitFieldUExtract:
			case spv::OpBitReverse:
			case spv::OpBitCount:
			case spv::OpSNegate:
			case spv::OpFNegate:
			case spv::OpLogicalNot:
			case spv::OpQuantizeToF16:
			// Binary ops
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
			case spv::OpIAddCarry:
			case spv::OpISubBorrow:
			case spv::OpDot:
			case spv::OpConvertFToU:
			case spv::OpConvertFToS:
			case spv::OpConvertSToF:
			case spv::OpConvertUToF:
			case spv::OpBitcast:
			case spv::OpSelect:
			case spv::OpIsInf:
			case spv::OpIsNan:
			case spv::OpAny:
			case spv::OpAll:
			case spv::OpDPdx:
			case spv::OpDPdxCoarse:
			case spv::OpDPdy:
			case spv::OpDPdyCoarse:
			case spv::OpFwidth:
			case spv::OpFwidthCoarse:
			case spv::OpDPdxFine:
			case spv::OpDPdyFine:
			case spv::OpFwidthFine:
			case spv::OpAtomicLoad:
			case spv::OpAtomicIAdd:
			case spv::OpAtomicISub:
			case spv::OpAtomicSMin:
			case spv::OpAtomicSMax:
			case spv::OpAtomicUMin:
			case spv::OpAtomicUMax:
			case spv::OpAtomicAnd:
			case spv::OpAtomicOr:
			case spv::OpAtomicXor:
			case spv::OpAtomicIIncrement:
			case spv::OpAtomicIDecrement:
			case spv::OpAtomicExchange:
			case spv::OpAtomicCompareExchange:
			case spv::OpPhi:
			case spv::OpImageSampleImplicitLod:
			case spv::OpImageSampleExplicitLod:
			case spv::OpImageSampleDrefImplicitLod:
			case spv::OpImageSampleDrefExplicitLod:
			case spv::OpImageSampleProjImplicitLod:
			case spv::OpImageSampleProjExplicitLod:
			case spv::OpImageSampleProjDrefImplicitLod:
			case spv::OpImageSampleProjDrefExplicitLod:
			case spv::OpImageGather:
			case spv::OpImageDrefGather:
			case spv::OpImageFetch:
			case spv::OpImageQuerySizeLod:
			case spv::OpImageQuerySize:
			case spv::OpImageQueryLod:
			case spv::OpImageQueryLevels:
			case spv::OpImageQuerySamples:
			case spv::OpImageRead:
			case spv::OpImageTexelPointer:
			case spv::OpGroupNonUniformElect:
			case spv::OpGroupNonUniformAll:
			case spv::OpGroupNonUniformAny:
			case spv::OpGroupNonUniformAllEqual:
			case spv::OpGroupNonUniformBroadcast:
			case spv::OpGroupNonUniformBroadcastFirst:
			case spv::OpGroupNonUniformBallot:
			case spv::OpGroupNonUniformInverseBallot:
			case spv::OpGroupNonUniformBallotBitExtract:
			case spv::OpGroupNonUniformBallotBitCount:
			case spv::OpGroupNonUniformBallotFindLSB:
			case spv::OpGroupNonUniformBallotFindMSB:
			case spv::OpGroupNonUniformShuffle:
			case spv::OpGroupNonUniformShuffleXor:
			case spv::OpGroupNonUniformShuffleUp:
			case spv::OpGroupNonUniformShuffleDown:
			case spv::OpGroupNonUniformIAdd:
			case spv::OpGroupNonUniformFAdd:
			case spv::OpGroupNonUniformIMul:
			case spv::OpGroupNonUniformFMul:
			case spv::OpGroupNonUniformSMin:
			case spv::OpGroupNonUniformUMin:
			case spv::OpGroupNonUniformFMin:
			case spv::OpGroupNonUniformSMax:
			case spv::OpGroupNonUniformUMax:
			case spv::OpGroupNonUniformFMax:
			case spv::OpGroupNonUniformBitwiseAnd:
			case spv::OpGroupNonUniformBitwiseOr:
			case spv::OpGroupNonUniformBitwiseXor:
			case spv::OpGroupNonUniformLogicalAnd:
			case spv::OpGroupNonUniformLogicalOr:
			case spv::OpGroupNonUniformLogicalXor:
			case spv::OpCopyObject:
			case spv::OpArrayLength:
				// Instructions that yield an intermediate value or divergent pointer
				DefineResult(insn);
				break;

			case spv::OpExtInst:
				switch(getExtension(insn.word(3)).name)
				{
					case Extension::GLSLstd450:
						DefineResult(insn);
						break;
					case Extension::OpenCLDebugInfo100:
						DefineOpenCLDebugInfo100(insn);
						break;
					default:
						UNREACHABLE("Unexpected Extension name %d", int(getExtension(insn.word(3)).name));
						break;
				}
				break;

			case spv::OpStore:
			case spv::OpAtomicStore:
			case spv::OpImageWrite:
			case spv::OpCopyMemory:
			case spv::OpMemoryBarrier:
				// Don't need to do anything during analysis pass
				break;

			case spv::OpControlBarrier:
				modes.ContainsControlBarriers = true;
				break;

			case spv::OpExtension:
			{
				auto ext = insn.string(1);
				// Part of core SPIR-V 1.3. Vulkan 1.1 implementations must also accept the pre-1.3
				// extension per Appendix A, `Vulkan Environment for SPIR-V`.
				if(!strcmp(ext, "SPV_KHR_storage_buffer_storage_class")) break;
				if(!strcmp(ext, "SPV_KHR_shader_draw_parameters")) break;
				if(!strcmp(ext, "SPV_KHR_16bit_storage")) break;
				if(!strcmp(ext, "SPV_KHR_variable_pointers")) break;
				if(!strcmp(ext, "SPV_KHR_device_group")) break;
				if(!strcmp(ext, "SPV_KHR_multiview")) break;
				if(!strcmp(ext, "SPV_EXT_shader_stencil_export")) break;
				UNSUPPORTED("SPIR-V Extension: %s", ext);
				break;
			}

			default:
				UNSUPPORTED("%s", OpcodeName(opcode).c_str());
		}
	}

	ASSERT_MSG(entryPoint != 0, "Entry point '%s' not found", entryPointName);
	for(auto &it : functions)
	{
		it.second.AssignBlockFields();
	}

	dbgCreateFile();
}

SpirvShader::~SpirvShader()
{
	dbgTerm();
}

void SpirvShader::DeclareType(InsnIterator insn)
{
	Type::ID resultId = insn.word(1);

	auto &type = types[resultId];
	type.definition = insn;
	type.sizeInComponents = ComputeTypeSize(insn);

	// A structure is a builtin block if it has a builtin
	// member. All members of such a structure are builtins.
	switch(insn.opcode())
	{
		case spv::OpTypeStruct:
		{
			auto d = memberDecorations.find(resultId);
			if(d != memberDecorations.end())
			{
				for(auto &m : d->second)
				{
					if(m.HasBuiltIn)
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

SpirvShader::Object &SpirvShader::CreateConstant(InsnIterator insn)
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

	ASSERT(objectTy.opcode() == spv::OpTypePointer);
	auto pointeeTy = getType(objectTy.element);

	auto &builtinInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputBuiltins : outputBuiltins;
	auto &userDefinedInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputs : outputs;

	ASSERT(object.opcode() == spv::OpVariable);
	Object::ID resultId = object.definition.word(2);

	if(objectTy.isBuiltInBlock)
	{
		// walk the builtin block, registering each of its members separately.
		auto m = memberDecorations.find(objectTy.element);
		ASSERT(m != memberDecorations.end());  // otherwise we wouldn't have marked the type chain
		auto &structType = pointeeTy.definition;
		auto offset = 0u;
		auto word = 2u;
		for(auto &member : m->second)
		{
			auto &memberType = getType(structType.word(word));

			if(member.HasBuiltIn)
			{
				builtinInterface[member.BuiltIn] = { resultId, offset, memberType.sizeInComponents };
			}

			offset += memberType.sizeInComponents;
			++word;
		}
		return;
	}

	auto d = decorations.find(resultId);
	if(d != decorations.end() && d->second.HasBuiltIn)
	{
		builtinInterface[d->second.BuiltIn] = { resultId, 0, pointeeTy.sizeInComponents };
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
	switch(mode)
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
			UNREACHABLE("Execution mode: %d", int(mode));
	}
}

uint32_t SpirvShader::ComputeTypeSize(InsnIterator insn)
{
	// Types are always built from the bottom up (with the exception of forward ptrs, which
	// don't appear in Vulkan shaders. Therefore, we can always assume our component parts have
	// already been described (and so their sizes determined)
	switch(insn.opcode())
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
			auto arraySize = GetConstScalarInt(insn.word(3));
			return getType(insn.word(2)).sizeInComponents * arraySize;
		}

		case spv::OpTypeStruct:
		{
			uint32_t size = 0;
			for(uint32_t i = 2u; i < insn.wordCount(); i++)
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
			UNREACHABLE("%s", OpcodeName(insn.opcode()).c_str());
			return 0;
	}
}

int SpirvShader::VisitInterfaceInner(Type::ID id, Decorations d, const InterfaceVisitor &f) const
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
	switch(obj.opcode())
	{
		case spv::OpTypePointer:
			return VisitInterfaceInner(obj.definition.word(3), d, f);
		case spv::OpTypeMatrix:
			for(auto i = 0u; i < obj.definition.word(3); i++, d.Location++)
			{
				// consumes same components of N consecutive locations
				VisitInterfaceInner(obj.definition.word(2), d, f);
			}
			return d.Location;
		case spv::OpTypeVector:
			for(auto i = 0u; i < obj.definition.word(3); i++, d.Component++)
			{
				// consumes N consecutive components in the same location
				VisitInterfaceInner(obj.definition.word(2), d, f);
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
			for(auto i = 0u; i < obj.definition.wordCount() - 2; i++)
			{
				ApplyDecorationsForIdMember(&d, id, i);
				d.Location = VisitInterfaceInner(obj.definition.word(i + 2), d, f);
				d.Component = 0;  // Implicit locations always have component=0
			}
			return d.Location;
		}
		case spv::OpTypeArray:
		{
			auto arraySize = GetConstScalarInt(obj.definition.word(3));
			for(auto i = 0u; i < arraySize; i++)
			{
				d.Location = VisitInterfaceInner(obj.definition.word(2), d, f);
			}
			return d.Location;
		}
		default:
			// Intentionally partial; most opcodes do not participate in type hierarchies
			return 0;
	}
}

void SpirvShader::VisitInterface(Object::ID id, const InterfaceVisitor &f) const
{
	// Walk a variable definition and call f for each component in it.
	Decorations d{};
	ApplyDecorationsForId(&d, id);

	auto def = getObject(id).definition;
	ASSERT(def.opcode() == spv::OpVariable);
	VisitInterfaceInner(def.word(1), d, f);
}

void SpirvShader::ApplyDecorationsForAccessChain(Decorations *d, DescriptorDecorations *dd, Object::ID baseId, uint32_t numIndexes, uint32_t const *indexIds) const
{
	ApplyDecorationsForId(d, baseId);
	auto &baseObject = getObject(baseId);
	ApplyDecorationsForId(d, baseObject.type);
	auto typeId = getType(baseObject.type).element;

	for(auto i = 0u; i < numIndexes; i++)
	{
		ApplyDecorationsForId(d, typeId);
		auto &type = getType(typeId);
		switch(type.opcode())
		{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstScalarInt(indexIds[i]);
				ApplyDecorationsForIdMember(d, typeId, memberIndex);
				typeId = type.definition.word(2u + memberIndex);
				break;
			}
			case spv::OpTypeArray:
			case spv::OpTypeRuntimeArray:
				if(dd->InputAttachmentIndex >= 0)
				{
					dd->InputAttachmentIndex += GetConstScalarInt(indexIds[i]);
				}
				typeId = type.element;
				break;
			case spv::OpTypeVector:
				typeId = type.element;
				break;
			case spv::OpTypeMatrix:
				typeId = type.element;
				d->InsideMatrix = true;
				break;
			default:
				UNREACHABLE("%s", OpcodeName(type.definition.opcode()).c_str());
		}
	}
}

SIMD::Pointer SpirvShader::WalkExplicitLayoutAccessChain(Object::ID baseId, uint32_t numIndexes, uint32_t const *indexIds, EmitState const *state) const
{
	// Produce a offset into external memory in sizeof(float) units

	auto &baseObject = getObject(baseId);
	Type::ID typeId = getType(baseObject.type).element;
	Decorations d = {};
	ApplyDecorationsForId(&d, baseObject.type);

	uint32_t arrayIndex = 0;
	if(baseObject.kind == Object::Kind::DescriptorSet)
	{
		auto type = getType(typeId).definition.opcode();
		if(type == spv::OpTypeArray || type == spv::OpTypeRuntimeArray)
		{
			ASSERT(getObject(indexIds[0]).kind == Object::Kind::Constant);
			arrayIndex = GetConstScalarInt(indexIds[0]);

			numIndexes--;
			indexIds++;
			typeId = getType(typeId).element;
		}
	}

	auto ptr = GetPointerToData(baseId, arrayIndex, state);

	int constantOffset = 0;

	for(auto i = 0u; i < numIndexes; i++)
	{
		auto &type = getType(typeId);
		ApplyDecorationsForId(&d, typeId);

		switch(type.definition.opcode())
		{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstScalarInt(indexIds[i]);
				ApplyDecorationsForIdMember(&d, typeId, memberIndex);
				ASSERT(d.HasOffset);
				constantOffset += d.Offset;
				typeId = type.definition.word(2u + memberIndex);
				break;
			}
			case spv::OpTypeArray:
			case spv::OpTypeRuntimeArray:
			{
				// TODO: b/127950082: Check bounds.
				ASSERT(d.HasArrayStride);
				auto &obj = getObject(indexIds[i]);
				if(obj.kind == Object::Kind::Constant)
				{
					constantOffset += d.ArrayStride * GetConstScalarInt(indexIds[i]);
				}
				else
				{
					ptr += SIMD::Int(d.ArrayStride) * state->getIntermediate(indexIds[i]).Int(0);
				}
				typeId = type.element;
				break;
			}
			case spv::OpTypeMatrix:
			{
				// TODO: b/127950082: Check bounds.
				ASSERT(d.HasMatrixStride);
				d.InsideMatrix = true;
				auto columnStride = (d.HasRowMajor && d.RowMajor) ? static_cast<int32_t>(sizeof(float)) : d.MatrixStride;
				auto &obj = getObject(indexIds[i]);
				if(obj.kind == Object::Kind::Constant)
				{
					constantOffset += columnStride * GetConstScalarInt(indexIds[i]);
				}
				else
				{
					ptr += SIMD::Int(columnStride) * state->getIntermediate(indexIds[i]).Int(0);
				}
				typeId = type.element;
				break;
			}
			case spv::OpTypeVector:
			{
				auto elemStride = (d.InsideMatrix && d.HasRowMajor && d.RowMajor) ? d.MatrixStride : static_cast<int32_t>(sizeof(float));
				auto &obj = getObject(indexIds[i]);
				if(obj.kind == Object::Kind::Constant)
				{
					constantOffset += elemStride * GetConstScalarInt(indexIds[i]);
				}
				else
				{
					ptr += SIMD::Int(elemStride) * state->getIntermediate(indexIds[i]).Int(0);
				}
				typeId = type.element;
				break;
			}
			default:
				UNREACHABLE("%s", OpcodeName(type.definition.opcode()).c_str());
		}
	}

	ptr += constantOffset;
	return ptr;
}

SIMD::Pointer SpirvShader::WalkAccessChain(Object::ID baseId, uint32_t numIndexes, uint32_t const *indexIds, EmitState const *state) const
{
	// TODO: avoid doing per-lane work in some cases if we can?
	auto routine = state->routine;
	auto &baseObject = getObject(baseId);
	Type::ID typeId = getType(baseObject.type).element;

	auto ptr = state->getPointer(baseId);

	int constantOffset = 0;

	for(auto i = 0u; i < numIndexes; i++)
	{
		auto &type = getType(typeId);
		switch(type.opcode())
		{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstScalarInt(indexIds[i]);
				int offsetIntoStruct = 0;
				for(auto j = 0; j < memberIndex; j++)
				{
					auto memberType = type.definition.word(2u + j);
					offsetIntoStruct += getType(memberType).sizeInComponents * sizeof(float);
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
				if(getType(baseObject.type).storageClass == spv::StorageClassUniformConstant)
				{
					// indexing into an array of descriptors.
					auto &obj = getObject(indexIds[i]);
					if(obj.kind != Object::Kind::Constant)
					{
						UNSUPPORTED("SPIR-V SampledImageArrayDynamicIndexing Capability");
					}

					auto d = descriptorDecorations.at(baseId);
					ASSERT(d.DescriptorSet >= 0);
					ASSERT(d.Binding >= 0);
					auto setLayout = routine->pipelineLayout->getDescriptorSetLayout(d.DescriptorSet);
					auto stride = static_cast<uint32_t>(setLayout->getBindingStride(d.Binding));
					ptr.base += stride * GetConstScalarInt(indexIds[i]);
				}
				else
				{
					auto stride = getType(type.element).sizeInComponents * static_cast<uint32_t>(sizeof(float));
					auto &obj = getObject(indexIds[i]);
					if(obj.kind == Object::Kind::Constant)
					{
						ptr += stride * GetConstScalarInt(indexIds[i]);
					}
					else
					{
						ptr += SIMD::Int(stride) * state->getIntermediate(indexIds[i]).Int(0);
					}
				}
				typeId = type.element;
				break;
			}

			default:
				UNREACHABLE("%s", OpcodeName(type.opcode()).c_str());
		}
	}

	if(constantOffset != 0)
	{
		ptr += constantOffset;
	}
	return ptr;
}

uint32_t SpirvShader::WalkLiteralAccessChain(Type::ID typeId, uint32_t numIndexes, uint32_t const *indexes) const
{
	uint32_t componentOffset = 0;

	for(auto i = 0u; i < numIndexes; i++)
	{
		auto &type = getType(typeId);
		switch(type.opcode())
		{
			case spv::OpTypeStruct:
			{
				int memberIndex = indexes[i];
				int offsetIntoStruct = 0;
				for(auto j = 0; j < memberIndex; j++)
				{
					auto memberType = type.definition.word(2u + j);
					offsetIntoStruct += getType(memberType).sizeInComponents;
				}
				componentOffset += offsetIntoStruct;
				typeId = type.definition.word(2u + memberIndex);
				break;
			}

			case spv::OpTypeVector:
			case spv::OpTypeMatrix:
			case spv::OpTypeArray:
			{
				auto elementType = type.definition.word(2);
				auto stride = getType(elementType).sizeInComponents;
				componentOffset += stride * indexes[i];
				typeId = elementType;
				break;
			}

			default:
				UNREACHABLE("%s", OpcodeName(type.opcode()).c_str());
		}
	}

	return componentOffset;
}

void SpirvShader::Decorations::Apply(spv::Decoration decoration, uint32_t arg)
{
	switch(decoration)
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
		case spv::DecorationRelaxedPrecision:
			RelaxedPrecision = true;
			break;
		case spv::DecorationRowMajor:
			HasRowMajor = true;
			RowMajor = true;
			break;
		case spv::DecorationColMajor:
			HasRowMajor = true;
			RowMajor = false;
		default:
			// Intentionally partial, there are many decorations we just don't care about.
			break;
	}
}

void SpirvShader::Decorations::Apply(const sw::SpirvShader::Decorations &src)
{
	// Apply a decoration group to this set of decorations
	if(src.HasBuiltIn)
	{
		HasBuiltIn = true;
		BuiltIn = src.BuiltIn;
	}

	if(src.HasLocation)
	{
		HasLocation = true;
		Location = src.Location;
	}

	if(src.HasComponent)
	{
		HasComponent = true;
		Component = src.Component;
	}

	if(src.HasOffset)
	{
		HasOffset = true;
		Offset = src.Offset;
	}

	if(src.HasArrayStride)
	{
		HasArrayStride = true;
		ArrayStride = src.ArrayStride;
	}

	if(src.HasMatrixStride)
	{
		HasMatrixStride = true;
		MatrixStride = src.MatrixStride;
	}

	if(src.HasRowMajor)
	{
		HasRowMajor = true;
		RowMajor = src.RowMajor;
	}

	Flat |= src.Flat;
	NoPerspective |= src.NoPerspective;
	Centroid |= src.Centroid;
	Block |= src.Block;
	BufferBlock |= src.BufferBlock;
	RelaxedPrecision |= src.RelaxedPrecision;
	InsideMatrix |= src.InsideMatrix;
}

void SpirvShader::DescriptorDecorations::Apply(const sw::SpirvShader::DescriptorDecorations &src)
{
	if(src.DescriptorSet >= 0)
	{
		DescriptorSet = src.DescriptorSet;
	}

	if(src.Binding >= 0)
	{
		Binding = src.Binding;
	}

	if(src.InputAttachmentIndex >= 0)
	{
		InputAttachmentIndex = src.InputAttachmentIndex;
	}
}

void SpirvShader::ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const
{
	auto it = decorations.find(id);
	if(it != decorations.end())
		d->Apply(it->second);
}

void SpirvShader::ApplyDecorationsForIdMember(Decorations *d, Type::ID id, uint32_t member) const
{
	auto it = memberDecorations.find(id);
	if(it != memberDecorations.end() && member < it->second.size())
	{
		d->Apply(it->second[member]);
	}
}

void SpirvShader::DefineResult(const InsnIterator &insn)
{
	Type::ID typeId = insn.word(1);
	Object::ID resultId = insn.word(2);
	auto &object = defs[resultId];
	object.type = typeId;

	switch(getType(typeId).opcode())
	{
		case spv::OpTypePointer:
		case spv::OpTypeImage:
		case spv::OpTypeSampledImage:
		case spv::OpTypeSampler:
			object.kind = Object::Kind::Pointer;
			break;

		default:
			object.kind = Object::Kind::Intermediate;
	}

	object.definition = insn;
	dbgDeclareResult(insn, resultId);
}

OutOfBoundsBehavior SpirvShader::EmitState::getOutOfBoundsBehavior(spv::StorageClass storageClass) const
{
	switch(storageClass)
	{
		case spv::StorageClassUniform:
		case spv::StorageClassStorageBuffer:
			// Buffer resource access. robustBufferAccess feature applies.
			return robustBufferAccess ? OutOfBoundsBehavior::RobustBufferAccess
			                          : OutOfBoundsBehavior::UndefinedBehavior;

		case spv::StorageClassImage:
			return OutOfBoundsBehavior::UndefinedValue;  // "The value returned by a read of an invalid texel is undefined"

		case spv::StorageClassInput:
			if(executionModel == spv::ExecutionModelVertex)
			{
				// Vertex attributes follow robustBufferAccess rules.
				return robustBufferAccess ? OutOfBoundsBehavior::RobustBufferAccess
				                          : OutOfBoundsBehavior::UndefinedBehavior;
			}
			// Fall through to default case.
		default:
			// TODO(b/137183137): Optimize if the pointer resulted from OpInBoundsAccessChain.
			// TODO(b/131224163): Optimize cases statically known to be within bounds.
			return OutOfBoundsBehavior::UndefinedValue;
	}

	return OutOfBoundsBehavior::Nullify;
}

// emit-time

void SpirvShader::emitProlog(SpirvRoutine *routine) const
{
	for(auto insn : *this)
	{
		switch(insn.opcode())
		{
			case spv::OpVariable:
			{
				Type::ID resultPointerTypeId = insn.word(1);
				auto resultPointerType = getType(resultPointerTypeId);
				auto pointeeType = getType(resultPointerType.element);

				if(pointeeType.sizeInComponents > 0)  // TODO: what to do about zero-slot objects?
				{
					Object::ID resultId = insn.word(2);
					routine->createVariable(resultId, pointeeType.sizeInComponents);
				}
				break;
			}
			case spv::OpPhi:
			{
				auto type = getType(insn.word(1));
				Object::ID resultId = insn.word(2);
				routine->phis.emplace(resultId, SpirvRoutine::Variable(type.sizeInComponents));
				break;
			}

			case spv::OpImageDrefGather:
			case spv::OpImageFetch:
			case spv::OpImageGather:
			case spv::OpImageQueryLod:
			case spv::OpImageSampleDrefExplicitLod:
			case spv::OpImageSampleDrefImplicitLod:
			case spv::OpImageSampleExplicitLod:
			case spv::OpImageSampleImplicitLod:
			case spv::OpImageSampleProjDrefExplicitLod:
			case spv::OpImageSampleProjDrefImplicitLod:
			case spv::OpImageSampleProjExplicitLod:
			case spv::OpImageSampleProjImplicitLod:
			{
				Object::ID resultId = insn.word(2);
				routine->samplerCache.emplace(resultId, SpirvRoutine::SamplerCache{});
				break;
			}

			default:
				// Nothing else produces interface variables, so can all be safely ignored.
				break;
		}
	}
}

void SpirvShader::emit(SpirvRoutine *routine, RValue<SIMD::Int> const &activeLaneMask, RValue<SIMD::Int> const &storesAndAtomicsMask, const vk::DescriptorSet::Bindings &descriptorSets) const
{
	EmitState state(routine, entryPoint, activeLaneMask, storesAndAtomicsMask, descriptorSets, robustBufferAccess, executionModel);

	dbgBeginEmit(&state);
	defer(dbgEndEmit(&state));

	// Emit everything up to the first label
	// TODO: Separate out dispatch of block from non-block instructions?
	for(auto insn : *this)
	{
		if(insn.opcode() == spv::OpLabel)
		{
			break;
		}
		EmitInstruction(insn, &state);
	}

	// Emit all the blocks starting from entryPoint.
	EmitBlocks(getFunction(entryPoint).entry, &state);
}

void SpirvShader::EmitInstructions(InsnIterator begin, InsnIterator end, EmitState *state) const
{
	for(auto insn = begin; insn != end; insn++)
	{
		auto res = EmitInstruction(insn, state);
		switch(res)
		{
			case EmitResult::Continue:
				continue;
			case EmitResult::Terminator:
				break;
			default:
				UNREACHABLE("Unexpected EmitResult %d", int(res));
				break;
		}
	}
}

SpirvShader::EmitResult SpirvShader::EmitInstruction(InsnIterator insn, EmitState *state) const
{
	dbgBeginEmitInstruction(insn, state);
	defer(dbgEndEmitInstruction(insn, state));

	auto opcode = insn.opcode();

	switch(opcode)
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
		case spv::OpTypeImage:
		case spv::OpTypeSampledImage:
		case spv::OpTypeSampler:
		case spv::OpExecutionMode:
		case spv::OpMemoryModel:
		case spv::OpFunction:
		case spv::OpFunctionEnd:
		case spv::OpConstant:
		case spv::OpConstantNull:
		case spv::OpConstantTrue:
		case spv::OpConstantFalse:
		case spv::OpConstantComposite:
		case spv::OpSpecConstant:
		case spv::OpSpecConstantTrue:
		case spv::OpSpecConstantFalse:
		case spv::OpSpecConstantComposite:
		case spv::OpSpecConstantOp:
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
		case spv::OpNoLine:
		case spv::OpModuleProcessed:
		case spv::OpString:
			// Nothing to do at emit time. These are either fully handled at analysis time,
			// or don't require any work at all.
			return EmitResult::Continue;

		case spv::OpLine:
			return EmitLine(insn, state);

		case spv::OpLabel:
			return EmitResult::Continue;

		case spv::OpVariable:
			return EmitVariable(insn, state);

		case spv::OpLoad:
		case spv::OpAtomicLoad:
			return EmitLoad(insn, state);

		case spv::OpStore:
		case spv::OpAtomicStore:
			return EmitStore(insn, state);

		case spv::OpAtomicIAdd:
		case spv::OpAtomicISub:
		case spv::OpAtomicSMin:
		case spv::OpAtomicSMax:
		case spv::OpAtomicUMin:
		case spv::OpAtomicUMax:
		case spv::OpAtomicAnd:
		case spv::OpAtomicOr:
		case spv::OpAtomicXor:
		case spv::OpAtomicIIncrement:
		case spv::OpAtomicIDecrement:
		case spv::OpAtomicExchange:
			return EmitAtomicOp(insn, state);

		case spv::OpAtomicCompareExchange:
			return EmitAtomicCompareExchange(insn, state);

		case spv::OpAccessChain:
		case spv::OpInBoundsAccessChain:
			return EmitAccessChain(insn, state);

		case spv::OpCompositeConstruct:
			return EmitCompositeConstruct(insn, state);

		case spv::OpCompositeInsert:
			return EmitCompositeInsert(insn, state);

		case spv::OpCompositeExtract:
			return EmitCompositeExtract(insn, state);

		case spv::OpVectorShuffle:
			return EmitVectorShuffle(insn, state);

		case spv::OpVectorExtractDynamic:
			return EmitVectorExtractDynamic(insn, state);

		case spv::OpVectorInsertDynamic:
			return EmitVectorInsertDynamic(insn, state);

		case spv::OpVectorTimesScalar:
		case spv::OpMatrixTimesScalar:
			return EmitVectorTimesScalar(insn, state);

		case spv::OpMatrixTimesVector:
			return EmitMatrixTimesVector(insn, state);

		case spv::OpVectorTimesMatrix:
			return EmitVectorTimesMatrix(insn, state);

		case spv::OpMatrixTimesMatrix:
			return EmitMatrixTimesMatrix(insn, state);

		case spv::OpOuterProduct:
			return EmitOuterProduct(insn, state);

		case spv::OpTranspose:
			return EmitTranspose(insn, state);

		case spv::OpNot:
		case spv::OpBitFieldInsert:
		case spv::OpBitFieldSExtract:
		case spv::OpBitFieldUExtract:
		case spv::OpBitReverse:
		case spv::OpBitCount:
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
		case spv::OpDPdx:
		case spv::OpDPdxCoarse:
		case spv::OpDPdy:
		case spv::OpDPdyCoarse:
		case spv::OpFwidth:
		case spv::OpFwidthCoarse:
		case spv::OpDPdxFine:
		case spv::OpDPdyFine:
		case spv::OpFwidthFine:
		case spv::OpQuantizeToF16:
			return EmitUnaryOp(insn, state);

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
		case spv::OpIAddCarry:
		case spv::OpISubBorrow:
			return EmitBinaryOp(insn, state);

		case spv::OpDot:
			return EmitDot(insn, state);

		case spv::OpSelect:
			return EmitSelect(insn, state);

		case spv::OpExtInst:
			return EmitExtendedInstruction(insn, state);

		case spv::OpAny:
			return EmitAny(insn, state);

		case spv::OpAll:
			return EmitAll(insn, state);

		case spv::OpBranch:
			return EmitBranch(insn, state);

		case spv::OpPhi:
			return EmitPhi(insn, state);

		case spv::OpSelectionMerge:
		case spv::OpLoopMerge:
			return EmitResult::Continue;

		case spv::OpBranchConditional:
			return EmitBranchConditional(insn, state);

		case spv::OpSwitch:
			return EmitSwitch(insn, state);

		case spv::OpUnreachable:
			return EmitUnreachable(insn, state);

		case spv::OpReturn:
			return EmitReturn(insn, state);

		case spv::OpFunctionCall:
			return EmitFunctionCall(insn, state);

		case spv::OpKill:
			return EmitKill(insn, state);

		case spv::OpImageSampleImplicitLod:
			return EmitImageSampleImplicitLod(None, insn, state);

		case spv::OpImageSampleExplicitLod:
			return EmitImageSampleExplicitLod(None, insn, state);

		case spv::OpImageSampleDrefImplicitLod:
			return EmitImageSampleImplicitLod(Dref, insn, state);

		case spv::OpImageSampleDrefExplicitLod:
			return EmitImageSampleExplicitLod(Dref, insn, state);

		case spv::OpImageSampleProjImplicitLod:
			return EmitImageSampleImplicitLod(Proj, insn, state);

		case spv::OpImageSampleProjExplicitLod:
			return EmitImageSampleExplicitLod(Proj, insn, state);

		case spv::OpImageSampleProjDrefImplicitLod:
			return EmitImageSampleImplicitLod(ProjDref, insn, state);

		case spv::OpImageSampleProjDrefExplicitLod:
			return EmitImageSampleExplicitLod(ProjDref, insn, state);

		case spv::OpImageGather:
			return EmitImageGather(None, insn, state);

		case spv::OpImageDrefGather:
			return EmitImageGather(Dref, insn, state);

		case spv::OpImageFetch:
			return EmitImageFetch(insn, state);

		case spv::OpImageQuerySizeLod:
			return EmitImageQuerySizeLod(insn, state);

		case spv::OpImageQuerySize:
			return EmitImageQuerySize(insn, state);

		case spv::OpImageQueryLod:
			return EmitImageQueryLod(insn, state);

		case spv::OpImageQueryLevels:
			return EmitImageQueryLevels(insn, state);

		case spv::OpImageQuerySamples:
			return EmitImageQuerySamples(insn, state);

		case spv::OpImageRead:
			return EmitImageRead(insn, state);

		case spv::OpImageWrite:
			return EmitImageWrite(insn, state);

		case spv::OpImageTexelPointer:
			return EmitImageTexelPointer(insn, state);

		case spv::OpSampledImage:
		case spv::OpImage:
			return EmitSampledImageCombineOrSplit(insn, state);

		case spv::OpCopyObject:
			return EmitCopyObject(insn, state);

		case spv::OpCopyMemory:
			return EmitCopyMemory(insn, state);

		case spv::OpControlBarrier:
			return EmitControlBarrier(insn, state);

		case spv::OpMemoryBarrier:
			return EmitMemoryBarrier(insn, state);

		case spv::OpGroupNonUniformElect:
		case spv::OpGroupNonUniformAll:
		case spv::OpGroupNonUniformAny:
		case spv::OpGroupNonUniformAllEqual:
		case spv::OpGroupNonUniformBroadcast:
		case spv::OpGroupNonUniformBroadcastFirst:
		case spv::OpGroupNonUniformBallot:
		case spv::OpGroupNonUniformInverseBallot:
		case spv::OpGroupNonUniformBallotBitExtract:
		case spv::OpGroupNonUniformBallotBitCount:
		case spv::OpGroupNonUniformBallotFindLSB:
		case spv::OpGroupNonUniformBallotFindMSB:
		case spv::OpGroupNonUniformShuffle:
		case spv::OpGroupNonUniformShuffleXor:
		case spv::OpGroupNonUniformShuffleUp:
		case spv::OpGroupNonUniformShuffleDown:
		case spv::OpGroupNonUniformIAdd:
		case spv::OpGroupNonUniformFAdd:
		case spv::OpGroupNonUniformIMul:
		case spv::OpGroupNonUniformFMul:
		case spv::OpGroupNonUniformSMin:
		case spv::OpGroupNonUniformUMin:
		case spv::OpGroupNonUniformFMin:
		case spv::OpGroupNonUniformSMax:
		case spv::OpGroupNonUniformUMax:
		case spv::OpGroupNonUniformFMax:
		case spv::OpGroupNonUniformBitwiseAnd:
		case spv::OpGroupNonUniformBitwiseOr:
		case spv::OpGroupNonUniformBitwiseXor:
		case spv::OpGroupNonUniformLogicalAnd:
		case spv::OpGroupNonUniformLogicalOr:
		case spv::OpGroupNonUniformLogicalXor:
			return EmitGroupNonUniform(insn, state);

		case spv::OpArrayLength:
			return EmitArrayLength(insn, state);

		default:
			UNREACHABLE("%s", OpcodeName(opcode).c_str());
			break;
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitAccessChain(InsnIterator insn, EmitState *state) const
{
	Type::ID typeId = insn.word(1);
	Object::ID resultId = insn.word(2);
	Object::ID baseId = insn.word(3);
	uint32_t numIndexes = insn.wordCount() - 4;
	const uint32_t *indexes = insn.wordPointer(4);
	auto &type = getType(typeId);
	ASSERT(type.sizeInComponents == 1);
	ASSERT(getObject(resultId).kind == Object::Kind::Pointer);

	if(type.storageClass == spv::StorageClassPushConstant ||
	   type.storageClass == spv::StorageClassUniform ||
	   type.storageClass == spv::StorageClassStorageBuffer)
	{
		auto ptr = WalkExplicitLayoutAccessChain(baseId, numIndexes, indexes, state);
		state->createPointer(resultId, ptr);
	}
	else
	{
		auto ptr = WalkAccessChain(baseId, numIndexes, indexes, state);
		state->createPointer(resultId, ptr);
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitCompositeConstruct(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto offset = 0u;

	for(auto i = 0u; i < insn.wordCount() - 3; i++)
	{
		Object::ID srcObjectId = insn.word(3u + i);
		auto &srcObject = getObject(srcObjectId);
		auto &srcObjectTy = getType(srcObject.type);
		GenericValue srcObjectAccess(this, state, srcObjectId);

		for(auto j = 0u; j < srcObjectTy.sizeInComponents; j++)
		{
			dst.move(offset++, srcObjectAccess.Float(j));
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitCompositeInsert(InsnIterator insn, EmitState *state) const
{
	Type::ID resultTypeId = insn.word(1);
	auto &type = getType(resultTypeId);
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &newPartObject = getObject(insn.word(3));
	auto &newPartObjectTy = getType(newPartObject.type);
	auto firstNewComponent = WalkLiteralAccessChain(resultTypeId, insn.wordCount() - 5, insn.wordPointer(5));

	GenericValue srcObjectAccess(this, state, insn.word(4));
	GenericValue newPartObjectAccess(this, state, insn.word(3));

	// old components before
	for(auto i = 0u; i < firstNewComponent; i++)
	{
		dst.move(i, srcObjectAccess.Float(i));
	}
	// new part
	for(auto i = 0u; i < newPartObjectTy.sizeInComponents; i++)
	{
		dst.move(firstNewComponent + i, newPartObjectAccess.Float(i));
	}
	// old components after
	for(auto i = firstNewComponent + newPartObjectTy.sizeInComponents; i < type.sizeInComponents; i++)
	{
		dst.move(i, srcObjectAccess.Float(i));
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitCompositeExtract(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &compositeObject = getObject(insn.word(3));
	Type::ID compositeTypeId = compositeObject.definition.word(1);
	auto firstComponent = WalkLiteralAccessChain(compositeTypeId, insn.wordCount() - 4, insn.wordPointer(4));

	GenericValue compositeObjectAccess(this, state, insn.word(3));
	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		dst.move(i, compositeObjectAccess.Float(firstComponent + i));
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitVectorShuffle(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);

	// Note: number of components in result type, first half type, and second
	// half type are all independent.
	auto &firstHalfType = getType(getObject(insn.word(3)).type);

	GenericValue firstHalfAccess(this, state, insn.word(3));
	GenericValue secondHalfAccess(this, state, insn.word(4));

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		auto selector = insn.word(5 + i);
		if(selector == static_cast<uint32_t>(-1))
		{
			// Undefined value. Until we decide to do real undef values, zero is as good
			// a value as any
			dst.move(i, RValue<SIMD::Float>(0.0f));
		}
		else if(selector < firstHalfType.sizeInComponents)
		{
			dst.move(i, firstHalfAccess.Float(selector));
		}
		else
		{
			dst.move(i, secondHalfAccess.Float(selector - firstHalfType.sizeInComponents));
		}
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitVectorExtractDynamic(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &srcType = getType(getObject(insn.word(3)).type);

	GenericValue src(this, state, insn.word(3));
	GenericValue index(this, state, insn.word(4));

	SIMD::UInt v = SIMD::UInt(0);

	for(auto i = 0u; i < srcType.sizeInComponents; i++)
	{
		v |= CmpEQ(index.UInt(0), SIMD::UInt(i)) & src.UInt(i);
	}

	dst.move(0, v);
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitVectorInsertDynamic(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);

	GenericValue src(this, state, insn.word(3));
	GenericValue component(this, state, insn.word(4));
	GenericValue index(this, state, insn.word(5));

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		SIMD::UInt mask = CmpEQ(SIMD::UInt(i), index.UInt(0));
		dst.move(i, (src.UInt(i) & ~mask) | (component.UInt(0) & mask));
	}
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitSelect(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto cond = GenericValue(this, state, insn.word(3));
	auto condIsScalar = (getType(cond.type).sizeInComponents == 1);
	auto lhs = GenericValue(this, state, insn.word(4));
	auto rhs = GenericValue(this, state, insn.word(5));

	for(auto i = 0u; i < type.sizeInComponents; i++)
	{
		auto sel = cond.Int(condIsScalar ? 0 : i);
		dst.move(i, (sel & lhs.Int(i)) | (~sel & rhs.Int(i)));  // TODO: IfThenElse()
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitAny(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	ASSERT(type.sizeInComponents == 1);
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &srcType = getType(getObject(insn.word(3)).type);
	auto src = GenericValue(this, state, insn.word(3));

	SIMD::UInt result = src.UInt(0);

	for(auto i = 1u; i < srcType.sizeInComponents; i++)
	{
		result |= src.UInt(i);
	}

	dst.move(0, result);
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitAll(InsnIterator insn, EmitState *state) const
{
	auto &type = getType(insn.word(1));
	ASSERT(type.sizeInComponents == 1);
	auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
	auto &srcType = getType(getObject(insn.word(3)).type);
	auto src = GenericValue(this, state, insn.word(3));

	SIMD::UInt result = src.UInt(0);

	for(auto i = 1u; i < srcType.sizeInComponents; i++)
	{
		result &= src.UInt(i);
	}

	dst.move(0, result);
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitAtomicOp(InsnIterator insn, EmitState *state) const
{
	auto &resultType = getType(Type::ID(insn.word(1)));
	Object::ID resultId = insn.word(2);
	Object::ID semanticsId = insn.word(5);
	auto memorySemantics = static_cast<spv::MemorySemanticsMask>(getObject(semanticsId).constantValue[0]);
	auto memoryOrder = MemoryOrder(memorySemantics);
	// Where no value is provided (increment/decrement) use an implicit value of 1.
	auto value = (insn.wordCount() == 7) ? GenericValue(this, state, insn.word(6)).UInt(0) : RValue<SIMD::UInt>(1);
	auto &dst = state->createIntermediate(resultId, resultType.sizeInComponents);
	auto ptr = state->getPointer(insn.word(3));
	auto ptrOffsets = ptr.offsets();

	SIMD::UInt x(0);
	auto mask = state->activeLaneMask() & state->storesAndAtomicsMask();
	for(int j = 0; j < SIMD::Width; j++)
	{
		If(Extract(mask, j) != 0)
		{
			auto offset = Extract(ptrOffsets, j);
			auto laneValue = Extract(value, j);
			UInt v;
			switch(insn.opcode())
			{
				case spv::OpAtomicIAdd:
				case spv::OpAtomicIIncrement:
					v = AddAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicISub:
				case spv::OpAtomicIDecrement:
					v = SubAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicAnd:
					v = AndAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicOr:
					v = OrAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicXor:
					v = XorAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicSMin:
					v = As<UInt>(MinAtomic(Pointer<Int>(&ptr.base[offset]), As<Int>(laneValue), memoryOrder));
					break;
				case spv::OpAtomicSMax:
					v = As<UInt>(MaxAtomic(Pointer<Int>(&ptr.base[offset]), As<Int>(laneValue), memoryOrder));
					break;
				case spv::OpAtomicUMin:
					v = MinAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicUMax:
					v = MaxAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				case spv::OpAtomicExchange:
					v = ExchangeAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, memoryOrder);
					break;
				default:
					UNREACHABLE("%s", OpcodeName(insn.opcode()).c_str());
					break;
			}
			x = Insert(x, v, j);
		}
	}

	dst.move(0, x);
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitAtomicCompareExchange(InsnIterator insn, EmitState *state) const
{
	// Separate from EmitAtomicOp due to different instruction encoding
	auto &resultType = getType(Type::ID(insn.word(1)));
	Object::ID resultId = insn.word(2);

	auto memorySemanticsEqual = static_cast<spv::MemorySemanticsMask>(getObject(insn.word(5)).constantValue[0]);
	auto memoryOrderEqual = MemoryOrder(memorySemanticsEqual);
	auto memorySemanticsUnequal = static_cast<spv::MemorySemanticsMask>(getObject(insn.word(6)).constantValue[0]);
	auto memoryOrderUnequal = MemoryOrder(memorySemanticsUnequal);

	auto value = GenericValue(this, state, insn.word(7));
	auto comparator = GenericValue(this, state, insn.word(8));
	auto &dst = state->createIntermediate(resultId, resultType.sizeInComponents);
	auto ptr = state->getPointer(insn.word(3));
	auto ptrOffsets = ptr.offsets();

	SIMD::UInt x(0);
	auto mask = state->activeLaneMask() & state->storesAndAtomicsMask();
	for(int j = 0; j < SIMD::Width; j++)
	{
		If(Extract(mask, j) != 0)
		{
			auto offset = Extract(ptrOffsets, j);
			auto laneValue = Extract(value.UInt(0), j);
			auto laneComparator = Extract(comparator.UInt(0), j);
			UInt v = CompareExchangeAtomic(Pointer<UInt>(&ptr.base[offset]), laneValue, laneComparator, memoryOrderEqual, memoryOrderUnequal);
			x = Insert(x, v, j);
		}
	}

	dst.move(0, x);
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitCopyObject(InsnIterator insn, EmitState *state) const
{
	auto ty = getType(insn.word(1));
	auto &dst = state->createIntermediate(insn.word(2), ty.sizeInComponents);
	auto src = GenericValue(this, state, insn.word(3));
	for(uint32_t i = 0; i < ty.sizeInComponents; i++)
	{
		dst.move(i, src.Int(i));
	}
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitArrayLength(InsnIterator insn, EmitState *state) const
{
	auto resultTyId = Type::ID(insn.word(1));
	auto resultId = Object::ID(insn.word(2));
	auto structPtrId = Object::ID(insn.word(3));
	auto arrayFieldIdx = insn.word(4);

	auto &resultType = getType(resultTyId);
	ASSERT(resultType.sizeInComponents == 1);
	ASSERT(resultType.definition.opcode() == spv::OpTypeInt);

	auto &structPtrTy = getType(getObject(structPtrId).type);
	auto &structTy = getType(structPtrTy.element);
	auto arrayId = Type::ID(structTy.definition.word(2 + arrayFieldIdx));

	auto &result = state->createIntermediate(resultId, 1);
	auto structBase = GetPointerToData(structPtrId, 0, state);

	Decorations structDecorations = {};
	ApplyDecorationsForIdMember(&structDecorations, structPtrTy.element, arrayFieldIdx);
	ASSERT(structDecorations.HasOffset);

	auto arrayBase = structBase + structDecorations.Offset;
	auto arraySizeInBytes = SIMD::Int(arrayBase.limit()) - arrayBase.offsets();

	Decorations arrayDecorations = {};
	ApplyDecorationsForId(&arrayDecorations, arrayId);
	ASSERT(arrayDecorations.HasArrayStride);
	auto arrayLength = arraySizeInBytes / SIMD::Int(arrayDecorations.ArrayStride);

	result.move(0, SIMD::Int(arrayLength));

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitExtendedInstruction(InsnIterator insn, EmitState *state) const
{
	auto ext = getExtension(insn.word(3));
	switch(ext.name)
	{
		case Extension::GLSLstd450:
			return EmitExtGLSLstd450(insn, state);
		case Extension::OpenCLDebugInfo100:
			return EmitOpenCLDebugInfo100(insn, state);
		default:
			UNREACHABLE("Unknown Extension::Name<%d>", int(ext.name));
	}
	return EmitResult::Continue;
}

uint32_t SpirvShader::GetConstScalarInt(Object::ID id) const
{
	auto &scopeObj = getObject(id);
	ASSERT(scopeObj.kind == Object::Kind::Constant);
	ASSERT(getType(scopeObj.type).sizeInComponents == 1);
	return scopeObj.constantValue[0];
}

void SpirvShader::emitEpilog(SpirvRoutine *routine) const
{
	for(auto insn : *this)
	{
		switch(insn.opcode())
		{
			case spv::OpVariable:
			{
				Object::ID resultId = insn.word(2);
				auto &object = getObject(resultId);
				auto &objectTy = getType(object.type);
				if(object.kind == Object::Kind::InterfaceVariable && objectTy.storageClass == spv::StorageClassOutput)
				{
					auto &dst = routine->getVariable(resultId);
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

	// Clear phis that are no longer used. This serves two purposes:
	// (1) The phi rr::Variables are destructed, preventing pointless
	//     materialization.
	// (2) Frees memory that will never be used again.
	routine->phis.clear();
}

VkShaderStageFlagBits SpirvShader::executionModelToStage(spv::ExecutionModel model)
{
	switch(model)
	{
		case spv::ExecutionModelVertex: return VK_SHADER_STAGE_VERTEX_BIT;
		// case spv::ExecutionModelTessellationControl:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		// case spv::ExecutionModelTessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		// case spv::ExecutionModelGeometry:               return VK_SHADER_STAGE_GEOMETRY_BIT;
		case spv::ExecutionModelFragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case spv::ExecutionModelGLCompute: return VK_SHADER_STAGE_COMPUTE_BIT;
		// case spv::ExecutionModelKernel:                 return VkShaderStageFlagBits(0); // Not supported by vulkan.
		// case spv::ExecutionModelTaskNV:                 return VK_SHADER_STAGE_TASK_BIT_NV;
		// case spv::ExecutionModelMeshNV:                 return VK_SHADER_STAGE_MESH_BIT_NV;
		// case spv::ExecutionModelRayGenerationNV:        return VK_SHADER_STAGE_RAYGEN_BIT_NV;
		// case spv::ExecutionModelIntersectionNV:         return VK_SHADER_STAGE_INTERSECTION_BIT_NV;
		// case spv::ExecutionModelAnyHitNV:               return VK_SHADER_STAGE_ANY_HIT_BIT_NV;
		// case spv::ExecutionModelClosestHitNV:           return VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
		// case spv::ExecutionModelMissNV:                 return VK_SHADER_STAGE_MISS_BIT_NV;
		// case spv::ExecutionModelCallableNV:             return VK_SHADER_STAGE_CALLABLE_BIT_NV;
		default:
			UNSUPPORTED("ExecutionModel: %d", int(model));
			return VkShaderStageFlagBits(0);
	}
}

SpirvShader::GenericValue::GenericValue(SpirvShader const *shader, EmitState const *state, SpirvShader::Object::ID objId)
    : obj(shader->getObject(objId))
    , intermediate(obj.kind == SpirvShader::Object::Kind::Intermediate ? &state->getIntermediate(objId) : nullptr)
    , type(obj.type)
{}

SpirvRoutine::SpirvRoutine(vk::PipelineLayout const *pipelineLayout)
    : pipelineLayout(pipelineLayout)
{
}

void SpirvRoutine::setImmutableInputBuiltins(SpirvShader const *shader)
{
	setInputBuiltin(shader, spv::BuiltInSubgroupLocalInvocationId, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(0, 1, 2, 3));
	});

	setInputBuiltin(shader, spv::BuiltInSubgroupEqMask, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 4);
		value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(1, 2, 4, 8));
		value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});

	setInputBuiltin(shader, spv::BuiltInSubgroupGeMask, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 4);
		value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(15, 14, 12, 8));
		value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});

	setInputBuiltin(shader, spv::BuiltInSubgroupGtMask, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 4);
		value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(14, 12, 8, 0));
		value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});

	setInputBuiltin(shader, spv::BuiltInSubgroupLeMask, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 4);
		value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(1, 3, 7, 15));
		value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});

	setInputBuiltin(shader, spv::BuiltInSubgroupLtMask, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 4);
		value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(0, 1, 3, 7));
		value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});

	setInputBuiltin(shader, spv::BuiltInDeviceIndex, [&](const SpirvShader::BuiltinMapping &builtin, Array<SIMD::Float> &value) {
		ASSERT(builtin.SizeInComponents == 1);
		// Only a single physical device is supported.
		value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
	});
}

}  // namespace sw
