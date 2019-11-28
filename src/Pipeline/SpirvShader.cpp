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

#include "SamplerCore.hpp"
#include "System/Math.hpp"
#include "Vulkan/VkBuffer.hpp"
#include "Vulkan/VkBufferView.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkDescriptorSet.hpp"
#include "Vulkan/VkPipelineLayout.hpp"
#include "Vulkan/VkDescriptorSetLayout.hpp"
#include "Vulkan/VkRenderPass.hpp"
#include "Device/Config.hpp"

#include <spirv/unified1/spirv.hpp>
#include <spirv/unified1/GLSL.std.450.h>

#include <queue>

namespace
{
	VkFormat SpirvFormatToVulkanFormat(spv::ImageFormat format)
	{
		switch (format)
		{
		case spv::ImageFormatRgba32f: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case spv::ImageFormatRgba32i: return VK_FORMAT_R32G32B32A32_SINT;
		case spv::ImageFormatRgba32ui: return VK_FORMAT_R32G32B32A32_UINT;
		case spv::ImageFormatR32f: return VK_FORMAT_R32_SFLOAT;
		case spv::ImageFormatR32i: return VK_FORMAT_R32_SINT;
		case spv::ImageFormatR32ui: return VK_FORMAT_R32_UINT;
		case spv::ImageFormatRgba8: return VK_FORMAT_R8G8B8A8_UNORM;
		case spv::ImageFormatRgba8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
		case spv::ImageFormatRgba8i: return VK_FORMAT_R8G8B8A8_SINT;
		case spv::ImageFormatRgba8ui: return VK_FORMAT_R8G8B8A8_UINT;
		case spv::ImageFormatRgba16f: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case spv::ImageFormatRgba16i: return VK_FORMAT_R16G16B16A16_SINT;
		case spv::ImageFormatRgba16ui: return VK_FORMAT_R16G16B16A16_UINT;
		case spv::ImageFormatRg32f: return VK_FORMAT_R32G32_SFLOAT;
		case spv::ImageFormatRg32i: return VK_FORMAT_R32G32_SINT;
		case spv::ImageFormatRg32ui: return VK_FORMAT_R32G32_UINT;

		default:
			UNIMPLEMENTED("SPIR-V ImageFormat %u", format);
			return VK_FORMAT_UNDEFINED;
		}
	}

	sw::SIMD::Float sRGBtoLinear(sw::SIMD::Float c)
	{
		sw::SIMD::Float lc = c * sw::SIMD::Float(1.0f / 12.92f);
		sw::SIMD::Float ec = sw::power((c + sw::SIMD::Float(0.055f)) * sw::SIMD::Float(1.0f / 1.055f), sw::SIMD::Float(2.4f));

		sw::SIMD::Int linear = CmpLT(c, sw::SIMD::Float(0.04045f));

		return rr::As<sw::SIMD::Float>((linear & rr::As<sw::SIMD::Int>(lc)) | (~linear & rr::As<sw::SIMD::Int>(ec)));   // TODO: IfThenElse()
	}

} // anonymous namespace

namespace sw
{

	SpirvShader::SpirvShader(
			uint32_t codeSerialID,
			VkShaderStageFlagBits pipelineStage,
			const char *entryPointName,
			InsnStore const &insns,
			const vk::RenderPass *renderPass,
			uint32_t subpassIndex,
			bool robustBufferAccess)
				: insns{insns}, inputs{MAX_INTERFACE_COMPONENTS},
				  outputs{MAX_INTERFACE_COMPONENTS},
				  codeSerialID(codeSerialID),
				  robustBufferAccess(robustBufferAccess)
	{
		ASSERT(insns.size() > 0);

		if (renderPass)
		{
			// capture formats of any input attachments present
			auto subpass = renderPass->getSubpass(subpassIndex);
			inputAttachmentFormats.reserve(subpass.inputAttachmentCount);
			for (auto i = 0u; i < subpass.inputAttachmentCount; i++)
			{
				auto attachmentIndex = subpass.pInputAttachments[i].attachment;
				inputAttachmentFormats.push_back(attachmentIndex != VK_ATTACHMENT_UNUSED
												 ? renderPass->getAttachment(attachmentIndex).format : VK_FORMAT_UNDEFINED);
			}
		}

		// Simplifying assumptions (to be satisfied by earlier transformations)
		// - The only input/output OpVariables present are those used by the entrypoint

		Function::ID currentFunction;
		Block::ID currentBlock;
		InsnIterator blockStart;

		for (auto insn : *this)
		{
			spv::Op opcode = insn.opcode();

			switch (opcode)
			{
			case spv::OpEntryPoint:
			{
				executionModel = spv::ExecutionModel(insn.word(1));
				auto id = Function::ID(insn.word(2));
				auto name = insn.string(3);
				auto stage = executionModelToStage(executionModel);
				if (stage == pipelineStage && strcmp(name, entryPointName) == 0)
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

				if (decoration == spv::DecorationCentroid)
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
				if (memberIndex >= d.size())
					d.resize(memberIndex + 1);    // on demand; exact size would require another pass...

				d[memberIndex].Apply(decoration, value);

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
				uint32_t group = insn.word(1);
				auto const &groupDecorations = decorations[group];
				auto const &descriptorGroupDecorations = descriptorDecorations[group];
				for (auto i = 2u; i < insn.wordCount(); i++)
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
				ASSERT(currentFunction.value() != 0);

				auto blockEnd = insn; blockEnd++;
				functions[currentFunction].blocks[currentBlock] = Block(blockStart, blockEnd);
				currentBlock = Block::ID(0);

				if (opcode == spv::OpKill)
				{
					modes.ContainsKill = true;
				}
				break;
			}

			case spv::OpLoopMerge:
			case spv::OpSelectionMerge:
				break; // Nothing to do in analysis pass.

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

				switch (storageClass)
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
					break; // Correctly handled.

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
					UNIMPLEMENTED("StorageClass %d not yet implemented", (int)storageClass);
					break;

				case spv::StorageClassCrossWorkgroup:
					UNSUPPORTED("SPIR-V OpenCL Execution Model (StorageClassCrossWorkgroup)");
					break;

				case spv::StorageClassGeneric:
					UNSUPPORTED("SPIR-V GenericPointer Capability (StorageClassGeneric)");
					break;

				default:
					UNREACHABLE("Unexpected StorageClass %d", storageClass); // See Appendix A of the Vulkan spec.
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
				CreateConstant(insn).constantValue[0] = 0;    // Represent Boolean false as zero.
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
				for (auto i = 0u; i < objectTy.sizeInComponents; i++)
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
				for (auto i = 0u; i < insn.wordCount() - 3; i++)
				{
					auto &constituent = getObject(insn.word(i + 3));
					auto &constituentTy = getType(constituent.type);
					for (auto j = 0u; j < constituentTy.sizeInComponents; j++)
					{
						object.constantValue[offset++] = constituent.constantValue[j];
					}
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
			case spv::OpSpecConstantOp:
				EvalSpecConstantOp(insn);
				break;

			case spv::OpCapability:
			{
				auto capability = static_cast<spv::Capability>(insn.word(1));
				switch (capability)
				{
				case spv::CapabilityMatrix: capabilities.Matrix = true; break;
				case spv::CapabilityShader: capabilities.Shader = true; break;
				case spv::CapabilityInputAttachment: capabilities.InputAttachment = true; break;
				case spv::CapabilitySampled1D: capabilities.Sampled1D = true; break;
				case spv::CapabilityImage1D: capabilities.Image1D = true; break;
				case spv::CapabilitySampledBuffer: capabilities.SampledBuffer = true; break;
				case spv::CapabilityImageBuffer: capabilities.ImageBuffer = true; break;
				case spv::CapabilityImageQuery: capabilities.ImageQuery = true; break;
				case spv::CapabilityDerivativeControl: capabilities.DerivativeControl = true; break;
				case spv::CapabilityGroupNonUniform: capabilities.GroupNonUniform = true; break;
				case spv::CapabilityMultiView: capabilities.MultiView = true; break;
				case spv::CapabilityDeviceGroup: capabilities.DeviceGroup = true; break;
				case spv::CapabilityGroupNonUniformVote: capabilities.GroupNonUniformVote = true; break;
				case spv::CapabilityGroupNonUniformBallot: capabilities.GroupNonUniformBallot = true; break;
				case spv::CapabilityGroupNonUniformShuffle: capabilities.GroupNonUniformShuffle = true; break;
				case spv::CapabilityGroupNonUniformShuffleRelative: capabilities.GroupNonUniformShuffleRelative = true; break;
				case spv::CapabilityStorageImageExtendedFormats: capabilities.StorageImageExtendedFormats = true; break;
				default:
					UNSUPPORTED("Unsupported capability %u", insn.word(1));
				}
				break; // Various capabilities will be declared, but none affect our code generation at this point.
			}

			case spv::OpMemoryModel:
				break; // Memory model does not affect our code generation until we decide to do Vulkan Memory Model support.

			case spv::OpFunction:
			{
				auto functionId = Function::ID(insn.word(2));
				ASSERT_MSG(currentFunction == 0, "Functions %d and %d overlap", currentFunction.value(), functionId.value());
				currentFunction = functionId;
				auto &function = functions[functionId];
				function.result = Type::ID(insn.word(1));
				function.type = Type::ID(insn.word(4));
				// Scan forward to find the function's label.
				for (auto it = insn; it != end() && function.entry == 0; it++)
				{
					switch (it.opcode())
					{
					case spv::OpFunction:
					case spv::OpFunctionParameter:
						break;
					case spv::OpLabel:
						function.entry = Block::ID(it.word(1));
						break;
					default:
						WARN("Unexpected opcode '%s' following OpFunction", OpcodeName(it.opcode()).c_str());
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
				// We will only support the GLSL 450 extended instruction set, so no point in tracking the ID we assign it.
				// Valid shaders will not attempt to import any other instruction sets.
				auto ext = insn.string(2);
				if (0 != strcmp("GLSL.std.450", ext))
				{
					UNSUPPORTED("SPIR-V Extension: %s", ext);
				}
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
			case spv::OpString:
				// No semantic impact
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

					if (opcode == spv::OpAccessChain || opcode == spv::OpInBoundsAccessChain)
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
			case spv::OpExtInst:
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
			case spv::OpCopyObject:
			case spv::OpArrayLength:
				// Instructions that yield an intermediate value or divergent pointer
				DefineResult(insn);
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
				if (!strcmp(ext, "SPV_KHR_storage_buffer_storage_class")) break;
				if (!strcmp(ext, "SPV_KHR_shader_draw_parameters")) break;
				if (!strcmp(ext, "SPV_KHR_16bit_storage")) break;
				if (!strcmp(ext, "SPV_KHR_variable_pointers")) break;
				if (!strcmp(ext, "SPV_KHR_device_group")) break;
				if (!strcmp(ext, "SPV_KHR_multiview")) break;
				UNSUPPORTED("SPIR-V Extension: %s", ext);
				break;
			}

			default:
				UNIMPLEMENTED("%s", OpcodeName(opcode).c_str());
			}
		}

		ASSERT_MSG(entryPoint != 0, "Entry point '%s' not found", entryPointName);
		for (auto &it : functions)
		{
			it.second.AssignBlockFields();
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

		ASSERT(objectTy.opcode() == spv::OpTypePointer);
		auto pointeeTy = getType(objectTy.element);

		auto &builtinInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputBuiltins : outputBuiltins;
		auto &userDefinedInterface = (objectTy.storageClass == spv::StorageClassInput) ? inputs : outputs;

		ASSERT(object.opcode() == spv::OpVariable);
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
			UNREACHABLE("Execution mode: %d", int(mode));
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
			auto arraySize = GetConstScalarInt(insn.word(3));
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
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Location++)
			{
				// consumes same components of N consecutive locations
				VisitInterfaceInner(obj.definition.word(2), d, f);
			}
			return d.Location;
		case spv::OpTypeVector:
			for (auto i = 0u; i < obj.definition.word(3); i++, d.Component++)
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
			for (auto i = 0u; i < obj.definition.wordCount() - 2; i++)
			{
				ApplyDecorationsForIdMember(&d, id, i);
				d.Location = VisitInterfaceInner(obj.definition.word(i + 2), d, f);
				d.Component = 0;    // Implicit locations always have component=0
			}
			return d.Location;
		}
		case spv::OpTypeArray:
		{
			auto arraySize = GetConstScalarInt(obj.definition.word(3));
			for (auto i = 0u; i < arraySize; i++)
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

		for (auto i = 0u; i < numIndexes; i++)
		{
			ApplyDecorationsForId(d, typeId);
			auto & type = getType(typeId);
			switch (type.opcode())
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
				if (dd->InputAttachmentIndex >= 0)
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
		if (baseObject.kind == Object::Kind::DescriptorSet)
		{
			auto type = getType(typeId).definition.opcode();
			if (type == spv::OpTypeArray || type == spv::OpTypeRuntimeArray)
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

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			ApplyDecorationsForId(&d, typeId);

			switch (type.definition.opcode())
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
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
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
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
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
				auto & obj = getObject(indexIds[i]);
				if (obj.kind == Object::Kind::Constant)
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

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			switch(type.opcode())
			{
			case spv::OpTypeStruct:
			{
				int memberIndex = GetConstScalarInt(indexIds[i]);
				int offsetIntoStruct = 0;
				for (auto j = 0; j < memberIndex; j++) {
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
				if (getType(baseObject.type).storageClass == spv::StorageClassUniformConstant)
				{
					// indexing into an array of descriptors.
					auto &obj = getObject(indexIds[i]);
					if (obj.kind != Object::Kind::Constant)
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
					auto & obj = getObject(indexIds[i]);
					if (obj.kind == Object::Kind::Constant)
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

		if (constantOffset != 0)
		{
			ptr += constantOffset;
		}
		return ptr;
	}

	uint32_t SpirvShader::WalkLiteralAccessChain(Type::ID typeId, uint32_t numIndexes, uint32_t const *indexes) const
	{
		uint32_t componentOffset = 0;

		for (auto i = 0u; i < numIndexes; i++)
		{
			auto & type = getType(typeId);
			switch(type.opcode())
			{
			case spv::OpTypeStruct:
			{
				int memberIndex = indexes[i];
				int offsetIntoStruct = 0;
				for (auto j = 0; j < memberIndex; j++) {
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

		if (src.HasRowMajor)
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

		if (src.InputAttachmentIndex >= 0)
		{
			InputAttachmentIndex = src.InputAttachmentIndex;
		}
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

	void SpirvShader::DefineResult(const InsnIterator &insn)
	{
		Type::ID typeId = insn.word(1);
		Object::ID resultId = insn.word(2);
		auto &object = defs[resultId];
		object.type = typeId;

		switch (getType(typeId).opcode())
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
		for (auto insn : *this)
		{
			switch (insn.opcode())
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

		// Emit everything up to the first label
		// TODO: Separate out dispatch of block from non-block instructions?
		for (auto insn : *this)
		{
			if (insn.opcode() == spv::OpLabel)
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
		for (auto insn = begin; insn != end; insn++)
		{
			auto res = EmitInstruction(insn, state);
			switch (res)
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
		auto opcode = insn.opcode();

		switch (opcode)
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
		case spv::OpLine:
		case spv::OpNoLine:
		case spv::OpModuleProcessed:
		case spv::OpString:
			// Nothing to do at emit time. These are either fully handled at analysis time,
			// or don't require any work at all.
			return EmitResult::Continue;

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

		for (auto i = 0u; i < insn.wordCount() - 3; i++)
		{
			Object::ID srcObjectId = insn.word(3u + i);
			auto & srcObject = getObject(srcObjectId);
			auto & srcObjectTy = getType(srcObject.type);
			GenericValue srcObjectAccess(this, state, srcObjectId);

			for (auto j = 0u; j < srcObjectTy.sizeInComponents; j++)
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
		for (auto i = 0u; i < firstNewComponent; i++)
		{
			dst.move(i, srcObjectAccess.Float(i));
		}
		// new part
		for (auto i = 0u; i < newPartObjectTy.sizeInComponents; i++)
		{
			dst.move(firstNewComponent + i, newPartObjectAccess.Float(i));
		}
		// old components after
		for (auto i = firstNewComponent + newPartObjectTy.sizeInComponents; i < type.sizeInComponents; i++)
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
		for (auto i = 0u; i < type.sizeInComponents; i++)
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

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto selector = insn.word(5 + i);
			if (selector == static_cast<uint32_t>(-1))
			{
				// Undefined value. Until we decide to do real undef values, zero is as good
				// a value as any
				dst.move(i, RValue<SIMD::Float>(0.0f));
			}
			else if (selector < firstHalfType.sizeInComponents)
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

		for (auto i = 0u; i < srcType.sizeInComponents; i++)
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

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::UInt mask = CmpEQ(SIMD::UInt(i), index.UInt(0));
			dst.move(i, (src.UInt(i) & ~mask) | (component.UInt(0) & mask));
		}
		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitVectorTimesScalar(InsnIterator insn, EmitState *state) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
		auto lhs = GenericValue(this, state, insn.word(3));
		auto rhs = GenericValue(this, state, insn.word(4));

		for (auto i = 0u; i < type.sizeInComponents; i++)
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

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::Float v = lhs.Float(i) * rhs.Float(0);
			for (auto j = 1u; j < rhsType.sizeInComponents; j++)
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

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			SIMD::Float v = lhs.Float(0) * rhs.Float(i * lhsType.sizeInComponents);
			for (auto j = 1u; j < lhsType.sizeInComponents; j++)
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

		for (auto row = 0u; row < numRows; row++)
		{
			for (auto col = 0u; col < numColumns; col++)
			{
				SIMD::Float v = SIMD::Float(0);
				for (auto i = 0u; i < numAdds; i++)
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

		for (auto col = 0u; col < numCols; col++)
		{
			for (auto row = 0u; row < numRows; row++)
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

		for (auto col = 0u; col < numCols; col++)
		{
			for (auto row = 0u; row < numRows; row++)
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

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			switch (insn.opcode())
			{
			case spv::OpNot:
			case spv::OpLogicalNot:		// logical not == bitwise not due to all-bits boolean representation
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
				if (insn.opcode() == spv::OpBitFieldSExtract)
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
				dst.move(i, SIMD::Float(Abs(Extract(src.Float(i), 1) - Extract(src.Float(i), 0))
							+ Abs(Extract(src.Float(i), 2) - Extract(src.Float(i), 0))));
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
				auto isInf  = CmpGT(abs, SIMD::Float(65504.0f));
				auto isNaN  = IsNan(abs);
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

		for (auto i = 0u; i < lhsType.sizeInComponents; i++)
		{
			switch (insn.opcode())
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
				b = b | CmpEQ(b, SIMD::Int(0)); // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1))); // prevent integer overflow
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
				b = b | CmpEQ(b, SIMD::Int(0)); // prevent divide-by-zero
				a = a | (CmpEQ(a, SIMD::Int(0x80000000)) & CmpEQ(b, SIMD::Int(-1))); // prevent integer overflow
				dst.move(i, a % b);
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

	SpirvShader::EmitResult SpirvShader::EmitSelect(InsnIterator insn, EmitState *state) const
	{
		auto &type = getType(insn.word(1));
		auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
		auto cond = GenericValue(this, state, insn.word(3));
		auto condIsScalar = (getType(cond.type).sizeInComponents == 1);
		auto lhs = GenericValue(this, state, insn.word(4));
		auto rhs = GenericValue(this, state, insn.word(5));

		for (auto i = 0u; i < type.sizeInComponents; i++)
		{
			auto sel = cond.Int(condIsScalar ? 0 : i);
			dst.move(i, (sel & lhs.Int(i)) | (~sel & rhs.Int(i)));   // TODO: IfThenElse()
		}

		return EmitResult::Continue;
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

	SIMD::UInt SpirvShader::FloatToHalfBits(SIMD::UInt floatBits, bool storeInUpperBits) const
	{
		static const uint32_t mask_sign = 0x80000000u;
		static const uint32_t mask_round = ~0xfffu;
		static const uint32_t c_f32infty = 255 << 23;
		static const uint32_t c_magic = 15 << 23;
		static const uint32_t c_nanbit = 0x200;
		static const uint32_t c_infty_as_fp16 = 0x7c00;
		static const uint32_t c_clamp = (31 << 23) - 0x1000;

		SIMD::UInt justsign = SIMD::UInt(mask_sign) & floatBits;
		SIMD::UInt absf = floatBits ^ justsign;
		SIMD::UInt b_isnormal = CmpNLE(SIMD::UInt(c_f32infty), absf);

		// Note: this version doesn't round to the nearest even in case of a tie as defined by IEEE 754-2008, it rounds to +inf
		//       instead of nearest even, since that's fine for GLSL ES 3.0's needs (see section 2.1.1 Floating-Point Computation)
		SIMD::UInt joined = ((((As<SIMD::UInt>(Min(As<SIMD::Float>(absf & SIMD::UInt(mask_round)) * As<SIMD::Float>(SIMD::UInt(c_magic)),
										 As<SIMD::Float>(SIMD::UInt(c_clamp))))) - SIMD::UInt(mask_round)) >> 13) & b_isnormal) |
					   ((b_isnormal ^ SIMD::UInt(0xFFFFFFFF)) & ((CmpNLE(absf, SIMD::UInt(c_f32infty)) & SIMD::UInt(c_nanbit)) |
															SIMD::UInt(c_infty_as_fp16)));

		return storeInUpperBits ? ((joined << 16) | justsign) : joined | (justsign >> 16);
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

	SpirvShader::EmitResult SpirvShader::EmitAny(InsnIterator insn, EmitState *state) const
	{
		auto &type = getType(insn.word(1));
		ASSERT(type.sizeInComponents == 1);
		auto &dst = state->createIntermediate(insn.word(2), type.sizeInComponents);
		auto &srcType = getType(getObject(insn.word(3)).type);
		auto src = GenericValue(this, state, insn.word(3));

		SIMD::UInt result = src.UInt(0);

		for (auto i = 1u; i < srcType.sizeInComponents; i++)
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

		for (auto i = 1u; i < srcType.sizeInComponents; i++)
		{
			result &= src.UInt(i);
		}

		dst.move(0, result);
		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageSampleImplicitLod(Variant variant, InsnIterator insn, EmitState *state) const
	{
		return EmitImageSample({variant, Implicit}, insn, state);
	}

	SpirvShader::EmitResult SpirvShader::EmitImageGather(Variant variant, InsnIterator insn, EmitState *state) const
	{
		ImageInstruction instruction = {variant, Gather};
		instruction.gatherComponent = !instruction.isDref() ? getObject(insn.word(5)).constantValue[0] : 0;

		return EmitImageSample(instruction, insn, state);
	}

	SpirvShader::EmitResult SpirvShader::EmitImageSampleExplicitLod(Variant variant, InsnIterator insn, EmitState *state) const
	{
		auto isDref = (variant == Dref) || (variant == ProjDref);
		uint32_t imageOperands = static_cast<spv::ImageOperandsMask>(insn.word(isDref ? 6 : 5));
		imageOperands &= ~spv::ImageOperandsConstOffsetMask;  // Dealt with later.

		if((imageOperands & spv::ImageOperandsLodMask) == imageOperands)
		{
			return EmitImageSample({variant, Lod}, insn, state);
		}
		else if((imageOperands & spv::ImageOperandsGradMask) == imageOperands)
		{
			return EmitImageSample({variant, Grad}, insn, state);
		}
		else UNIMPLEMENTED("Image Operands %x", imageOperands);
		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageFetch(InsnIterator insn, EmitState *state) const
	{
		return EmitImageSample({None, Fetch}, insn, state);
	}

	SpirvShader::EmitResult SpirvShader::EmitImageSample(ImageInstruction instruction, InsnIterator insn, EmitState *state) const
	{
		Type::ID resultTypeId = insn.word(1);
		Object::ID resultId = insn.word(2);
		Object::ID sampledImageId = insn.word(3);  // For OpImageFetch this is just an Image, not a SampledImage.
		Object::ID coordinateId = insn.word(4);
		auto &resultType = getType(resultTypeId);

		auto &result = state->createIntermediate(resultId, resultType.sizeInComponents);
		auto imageDescriptor = state->getPointer(sampledImageId).base; // vk::SampledImageDescriptor*

		// If using a separate sampler, look through the OpSampledImage instruction to find the sampler descriptor
		auto &sampledImage = getObject(sampledImageId);
		auto samplerDescriptor = (sampledImage.opcode() == spv::OpSampledImage) ?
				state->getPointer(sampledImage.definition.word(4)).base : imageDescriptor;

		auto coordinate = GenericValue(this, state, coordinateId);
		auto &coordinateType = getType(coordinate.type);

		Pointer<Byte> sampler = samplerDescriptor + OFFSET(vk::SampledImageDescriptor, sampler); // vk::Sampler*
		Pointer<Byte> texture = imageDescriptor + OFFSET(vk::SampledImageDescriptor, texture);  // sw::Texture*

		// Above we assumed that if the SampledImage operand is not the result of an OpSampledImage,
		// it must be a combined image sampler loaded straight from the descriptor set. For OpImageFetch
		// it's just an Image operand, so there's no sampler descriptor data.
		if(getType(sampledImage.type).opcode() != spv::OpTypeSampledImage)
		{
			sampler = Pointer<Byte>(nullptr);
		}

		uint32_t imageOperands = spv::ImageOperandsMaskNone;
		bool lodOrBias = false;
		Object::ID lodOrBiasId = 0;
		bool grad = false;
		Object::ID gradDxId = 0;
		Object::ID gradDyId = 0;
		bool constOffset = false;
		Object::ID offsetId = 0;
		bool sample = false;
		Object::ID sampleId = 0;

		uint32_t operand = (instruction.isDref() || instruction.samplerMethod == Gather) ? 6 : 5;

		if(insn.wordCount() > operand)
		{
			imageOperands = static_cast<spv::ImageOperandsMask>(insn.word(operand++));

			if(imageOperands & spv::ImageOperandsBiasMask)
			{
				lodOrBias = true;
				lodOrBiasId = insn.word(operand);
				operand++;
				imageOperands &= ~spv::ImageOperandsBiasMask;

				ASSERT(instruction.samplerMethod == Implicit);
				instruction.samplerMethod = Bias;
			}

			if(imageOperands & spv::ImageOperandsLodMask)
			{
				lodOrBias = true;
				lodOrBiasId = insn.word(operand);
				operand++;
				imageOperands &= ~spv::ImageOperandsLodMask;
			}

			if(imageOperands & spv::ImageOperandsGradMask)
			{
				ASSERT(!lodOrBias);  // SPIR-V 1.3: "It is invalid to set both the Lod and Grad bits." Bias is for ImplicitLod, Grad for ExplicitLod.
				grad = true;
				gradDxId = insn.word(operand + 0);
				gradDyId = insn.word(operand + 1);
				operand += 2;
				imageOperands &= ~spv::ImageOperandsGradMask;
			}

			if(imageOperands & spv::ImageOperandsConstOffsetMask)
			{
				constOffset = true;
				offsetId = insn.word(operand);
				operand++;
				imageOperands &= ~spv::ImageOperandsConstOffsetMask;
			}

			if(imageOperands & spv::ImageOperandsSampleMask)
			{
				sample = true;
				sampleId = insn.word(operand);
				imageOperands &= ~spv::ImageOperandsSampleMask;

				ASSERT(instruction.samplerMethod == Fetch);
				instruction.sample = true;
			}

			if(imageOperands != 0)
			{
				UNSUPPORTED("Image operand %x", imageOperands);
			}
		}

		Array<SIMD::Float> in(16);  // Maximum 16 input parameter components.

		uint32_t coordinates = coordinateType.sizeInComponents - instruction.isProj();
		instruction.coordinates = coordinates;

		uint32_t i = 0;
		for( ; i < coordinates; i++)
		{
			if(instruction.isProj())
			{
				in[i] = coordinate.Float(i) / coordinate.Float(coordinates);  // TODO(b/129523279): Optimize using reciprocal.
			}
			else
			{
				in[i] = coordinate.Float(i);
			}
		}

		if(instruction.isDref())
		{
			auto drefValue = GenericValue(this, state, insn.word(5));

			if(instruction.isProj())
			{
				in[i] = drefValue.Float(0) / coordinate.Float(coordinates);  // TODO(b/129523279): Optimize using reciprocal.
			}
			else
			{
				in[i] = drefValue.Float(0);
			}

			i++;
		}

		if(lodOrBias)
		{
			auto lodValue = GenericValue(this, state, lodOrBiasId);
			in[i] = lodValue.Float(0);
			i++;
		}
		else if(grad)
		{
			auto dxValue = GenericValue(this, state, gradDxId);
			auto dyValue = GenericValue(this, state, gradDyId);
			auto &dxyType = getType(dxValue.type);
			ASSERT(dxyType.sizeInComponents == getType(dyValue.type).sizeInComponents);

			instruction.grad = dxyType.sizeInComponents;

			for(uint32_t j = 0; j < dxyType.sizeInComponents; j++, i++)
			{
				in[i] = dxValue.Float(j);
			}

			for(uint32_t j = 0; j < dxyType.sizeInComponents; j++, i++)
			{
				in[i] = dyValue.Float(j);
			}
		}
		else if (instruction.samplerMethod == Fetch)
		{
			// The instruction didn't provide a lod operand, but the sampler's Fetch
			// function requires one to be present. If no lod is supplied, the default
			// is zero.
			in[i] = As<SIMD::Float>(SIMD::Int(0));
			i++;
		}

		if(constOffset)
		{
			auto offsetValue = GenericValue(this, state, offsetId);
			auto &offsetType = getType(offsetValue.type);

			instruction.offset = offsetType.sizeInComponents;

			for(uint32_t j = 0; j < offsetType.sizeInComponents; j++, i++)
			{
				in[i] = As<SIMD::Float>(offsetValue.Int(j));  // Integer values, but transfered as float.
			}
		}

		if(sample)
		{
			auto sampleValue = GenericValue(this, state, sampleId);
			in[i] = As<SIMD::Float>(sampleValue.Int(0));
		}

		auto cacheIt = state->routine->samplerCache.find(resultId);
		ASSERT(cacheIt != state->routine->samplerCache.end());
		auto &cache = cacheIt->second;
		auto cacheHit = cache.imageDescriptor == imageDescriptor && cache.sampler == sampler;

		If(!cacheHit)
		{
			cache.function = Call(getImageSampler, instruction.parameters, imageDescriptor, sampler);
			cache.imageDescriptor = imageDescriptor;
			cache.sampler = sampler;
		}

		Array<SIMD::Float> out(4);
		Call<ImageSampler>(cache.function, texture, sampler, &in[0], &out[0], state->routine->constants);

		for (auto i = 0u; i < resultType.sizeInComponents; i++) { result.move(i, out[i]); }

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageQuerySizeLod(InsnIterator insn, EmitState *state) const
	{
		auto &resultTy = getType(Type::ID(insn.word(1)));
		auto resultId = Object::ID(insn.word(2));
		auto imageId = Object::ID(insn.word(3));
		auto lodId = Object::ID(insn.word(4));

		auto &dst = state->createIntermediate(resultId, resultTy.sizeInComponents);
		GetImageDimensions(state, resultTy, imageId, lodId, dst);

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageQuerySize(InsnIterator insn, EmitState *state) const
	{
		auto &resultTy = getType(Type::ID(insn.word(1)));
		auto resultId = Object::ID(insn.word(2));
		auto imageId = Object::ID(insn.word(3));
		auto lodId = Object::ID(0);

		auto &dst = state->createIntermediate(resultId, resultTy.sizeInComponents);
		GetImageDimensions(state, resultTy, imageId, lodId, dst);

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageQueryLod(InsnIterator insn, EmitState *state) const
	{
		return EmitImageSample({None, Query}, insn, state);
	}

	void SpirvShader::GetImageDimensions(EmitState const *state, Type const &resultTy, Object::ID imageId, Object::ID lodId, Intermediate &dst) const
	{
		auto routine = state->routine;
		auto &image = getObject(imageId);
		auto &imageType = getType(image.type);

		ASSERT(imageType.definition.opcode() == spv::OpTypeImage);
		bool isArrayed = imageType.definition.word(5) != 0;
		bool isCubeMap = imageType.definition.word(3) == spv::DimCube;

		const DescriptorDecorations &d = descriptorDecorations.at(imageId);
		auto setLayout = routine->pipelineLayout->getDescriptorSetLayout(d.DescriptorSet);
		auto &bindingLayout = setLayout->getBindingLayout(d.Binding);

		Pointer<Byte> descriptor = state->getPointer(imageId).base;

		Pointer<Int> extent;
		Int arrayLayers;

		switch (bindingLayout.descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		{
			extent = descriptor + OFFSET(vk::StorageImageDescriptor, extent); // int[3]*
			arrayLayers = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, arrayLayers)); // uint32_t
			break;
		}
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		{
			extent = descriptor + OFFSET(vk::SampledImageDescriptor, extent); // int[3]*
			arrayLayers = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, arrayLayers)); // uint32_t
			break;
		}
		default:
			UNREACHABLE("Image descriptorType: %d", int(bindingLayout.descriptorType));
		}

		auto dimensions = resultTy.sizeInComponents - (isArrayed ? 1 : 0);
		std::vector<Int> out;
		if (lodId != 0)
		{
			auto lodVal = GenericValue(this, state, lodId);
			ASSERT(getType(lodVal.type).sizeInComponents == 1);
			auto lod = lodVal.Int(0);
			auto one = SIMD::Int(1);
			for (uint32_t i = 0; i < dimensions; i++)
			{
				dst.move(i, Max(SIMD::Int(extent[i]) >> lod, one));
			}
		}
		else
		{
			for (uint32_t i = 0; i < dimensions; i++)
			{
				dst.move(i, SIMD::Int(extent[i]));
			}
		}

		if (isArrayed)
		{
			auto numElements = isCubeMap ? (arrayLayers / 6) : RValue<Int>(arrayLayers);
			dst.move(dimensions, SIMD::Int(numElements));
		}
	}

	SpirvShader::EmitResult SpirvShader::EmitImageQueryLevels(InsnIterator insn, EmitState *state) const
	{
		auto &resultTy = getType(Type::ID(insn.word(1)));
		ASSERT(resultTy.sizeInComponents == 1);
		auto resultId = Object::ID(insn.word(2));
		auto imageId = Object::ID(insn.word(3));

		const DescriptorDecorations &d = descriptorDecorations.at(imageId);
		auto setLayout = state->routine->pipelineLayout->getDescriptorSetLayout(d.DescriptorSet);
		auto &bindingLayout = setLayout->getBindingLayout(d.Binding);

		Pointer<Byte> descriptor = state->getPointer(imageId).base;
		Int mipLevels = 0;
		switch (bindingLayout.descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			mipLevels = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, mipLevels)); // uint32_t
			break;
		default:
			UNREACHABLE("Image descriptorType: %d", int(bindingLayout.descriptorType));
		}

		auto &dst = state->createIntermediate(resultId, 1);
		dst.move(0, SIMD::Int(mipLevels));

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageQuerySamples(InsnIterator insn, EmitState *state) const
	{
		auto &resultTy = getType(Type::ID(insn.word(1)));
		ASSERT(resultTy.sizeInComponents == 1);
		auto resultId = Object::ID(insn.word(2));
		auto imageId = Object::ID(insn.word(3));
		auto imageTy = getType(getObject(imageId).type);
		ASSERT(imageTy.definition.opcode() == spv::OpTypeImage);
		ASSERT(imageTy.definition.word(3) == spv::Dim2D);
		ASSERT(imageTy.definition.word(6 /* MS */) == 1);

		const DescriptorDecorations &d = descriptorDecorations.at(imageId);
		auto setLayout = state->routine->pipelineLayout->getDescriptorSetLayout(d.DescriptorSet);
		auto &bindingLayout = setLayout->getBindingLayout(d.Binding);

		Pointer<Byte> descriptor = state->getPointer(imageId).base;
		Int sampleCount = 0;
		switch (bindingLayout.descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			sampleCount = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, sampleCount)); // uint32_t
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			sampleCount = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, sampleCount)); // uint32_t
			break;
		default:
			UNREACHABLE("Image descriptorType: %d", int(bindingLayout.descriptorType));
		}

		auto &dst = state->createIntermediate(resultId, 1);
		dst.move(0, SIMD::Int(sampleCount));

		return EmitResult::Continue;
	}

	SIMD::Pointer SpirvShader::GetTexelAddress(EmitState const *state, SIMD::Pointer ptr, GenericValue const & coordinate, Type const & imageType, Pointer<Byte> descriptor, int texelSize, Object::ID sampleId, bool useStencilAspect) const
	{
		auto routine = state->routine;
		bool isArrayed = imageType.definition.word(5) != 0;
		auto dim = static_cast<spv::Dim>(imageType.definition.word(3));
		int dims = getType(coordinate.type).sizeInComponents - (isArrayed ? 1 : 0);

		SIMD::Int u = coordinate.Int(0);
		SIMD::Int v = SIMD::Int(0);

		if (getType(coordinate.type).sizeInComponents > 1)
		{
			v = coordinate.Int(1);
		}

		if (dim == spv::DimSubpassData)
		{
			u += routine->windowSpacePosition[0];
			v += routine->windowSpacePosition[1];
		}

		auto rowPitch = SIMD::Int(*Pointer<Int>(descriptor + (useStencilAspect
															  ? OFFSET(vk::StorageImageDescriptor, stencilRowPitchBytes)
															  : OFFSET(vk::StorageImageDescriptor, rowPitchBytes))));
		auto slicePitch = SIMD::Int(
				*Pointer<Int>(descriptor + (useStencilAspect
											? OFFSET(vk::StorageImageDescriptor, stencilSlicePitchBytes)
											: OFFSET(vk::StorageImageDescriptor, slicePitchBytes))));
		auto samplePitch = SIMD::Int(
				*Pointer<Int>(descriptor + (useStencilAspect
											? OFFSET(vk::StorageImageDescriptor, stencilSamplePitchBytes)
											: OFFSET(vk::StorageImageDescriptor, samplePitchBytes))));

		ptr += u * SIMD::Int(texelSize);
		if (dims > 1)
		{
			ptr += v * rowPitch;
		}
		if (dims > 2)
		{
			ptr += coordinate.Int(2) * slicePitch;
		}
		if (isArrayed)
		{
			ptr += coordinate.Int(dims) * slicePitch;
		}

		if (dim == spv::DimSubpassData)
		{
			// Multiview input attachment access is to the layer corresponding to the current view
			ptr += SIMD::Int(routine->viewID) * slicePitch;
		}

		if (sampleId.value())
		{
			GenericValue sample(this, state, sampleId);
			ptr += sample.Int(0) * samplePitch;
		}

		return ptr;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageRead(InsnIterator insn, EmitState *state) const
	{
		auto &resultType = getType(Type::ID(insn.word(1)));
		auto imageId = Object::ID(insn.word(3));
		auto &image = getObject(imageId);
		auto &imageType = getType(image.type);
		Object::ID resultId = insn.word(2);

		Object::ID sampleId = 0;

		if (insn.wordCount() > 5)
		{
			int operand = 6;
			auto imageOperands = insn.word(5);
			if (imageOperands & spv::ImageOperandsSampleMask)
			{
				sampleId = insn.word(operand++);
				imageOperands &= ~spv::ImageOperandsSampleMask;
			}

			// Should be no remaining image operands.
			ASSERT(!imageOperands);
		}

		ASSERT(imageType.definition.opcode() == spv::OpTypeImage);
		auto dim = static_cast<spv::Dim>(imageType.definition.word(3));

		auto coordinate = GenericValue(this, state, insn.word(4));
		const DescriptorDecorations &d = descriptorDecorations.at(imageId);

		// For subpass data, format in the instruction is spv::ImageFormatUnknown. Get it from
		// the renderpass data instead. In all other cases, we can use the format in the instruction.
		auto vkFormat = (dim == spv::DimSubpassData)
						? inputAttachmentFormats[d.InputAttachmentIndex]
						: SpirvFormatToVulkanFormat(static_cast<spv::ImageFormat>(imageType.definition.word(8)));

		// Depth+Stencil image attachments select aspect based on the Sampled Type of the
		// OpTypeImage. If float, then we want the depth aspect. If int, we want the stencil aspect.
		auto useStencilAspect = (vkFormat == VK_FORMAT_D32_SFLOAT_S8_UINT &&
				getType(imageType.definition.word(2)).opcode() == spv::OpTypeInt);

		if (useStencilAspect)
		{
			vkFormat = VK_FORMAT_S8_UINT;
		}

		auto pointer = state->getPointer(imageId);
		Pointer<Byte> binding = pointer.base;
		Pointer<Byte> imageBase = *Pointer<Pointer<Byte>>(binding + (useStencilAspect
				? OFFSET(vk::StorageImageDescriptor, stencilPtr)
				: OFFSET(vk::StorageImageDescriptor, ptr)));

		auto imageSizeInBytes = *Pointer<Int>(binding + OFFSET(vk::StorageImageDescriptor, sizeInBytes));

		auto &dst = state->createIntermediate(resultId, resultType.sizeInComponents);

		auto texelSize = vk::Format(vkFormat).bytes();
		auto basePtr = SIMD::Pointer(imageBase, imageSizeInBytes);
		auto texelPtr = GetTexelAddress(state, basePtr, coordinate, imageType, binding, texelSize, sampleId, useStencilAspect);

		// "The value returned by a read of an invalid texel is undefined,
		//  unless that read operation is from a buffer resource and the robustBufferAccess feature is enabled."
		// TODO: Don't always assume a buffer resource.
		auto robustness = OutOfBoundsBehavior::RobustBufferAccess;

		SIMD::Int packed[4];
		// Round up texel size: for formats smaller than 32 bits per texel, we will emit a bunch
		// of (overlapping) 32b loads here, and each lane will pick out what it needs from the low bits.
		// TODO: specialize for small formats?
		for (auto i = 0; i < (texelSize + 3)/4; i++)
		{
			packed[i] = texelPtr.Load<SIMD::Int>(robustness, state->activeLaneMask(), false, std::memory_order_relaxed, std::min(texelSize, 4));
			texelPtr += sizeof(float);
		}

		// Format support requirements here come from two sources:
		// - Minimum required set of formats for loads from storage images
		// - Any format supported as a color or depth/stencil attachment, for input attachments
		switch(vkFormat)
		{
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_UINT:
			dst.move(0, packed[0]);
			dst.move(1, packed[1]);
			dst.move(2, packed[2]);
			dst.move(3, packed[3]);
			break;
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_UINT:
			dst.move(0, packed[0]);
			// Fill remaining channels with 0,0,1 (of the correct type)
			dst.move(1, SIMD::Int(0));
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			dst.move(0, packed[0]);
			// Fill remaining channels with 0,0,1 (of the correct type)
			dst.move(1, SIMD::Float(0));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_D16_UNORM:
			dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xffff)) * SIMD::Float(1.0f / 65535.0f));
			dst.move(1, SIMD::Float(0));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_R16G16B16A16_SINT:
			dst.move(0, (packed[0] << 16) >> 16);
			dst.move(1, (packed[0]) >> 16);
			dst.move(2, (packed[1] << 16) >> 16);
			dst.move(3, (packed[1]) >> 16);
			break;
		case VK_FORMAT_R16G16B16A16_UINT:
			dst.move(0, packed[0] & SIMD::Int(0xffff));
			dst.move(1, (packed[0] >> 16) & SIMD::Int(0xffff));
			dst.move(2, packed[1] & SIMD::Int(0xffff));
			dst.move(3, (packed[1] >> 16) & SIMD::Int(0xffff));
			break;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			dst.move(0, halfToFloatBits(As<SIMD::UInt>(packed[0]) & SIMD::UInt(0x0000FFFF)));
			dst.move(1, halfToFloatBits((As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFFFF0000)) >> 16));
			dst.move(2, halfToFloatBits(As<SIMD::UInt>(packed[1]) & SIMD::UInt(0x0000FFFF)));
			dst.move(3, halfToFloatBits((As<SIMD::UInt>(packed[1]) & SIMD::UInt(0xFFFF0000)) >> 16));
			break;
		case VK_FORMAT_R8G8B8A8_SNORM:
			dst.move(0, Min(Max(SIMD::Float(((packed[0]<<24) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(1, Min(Max(SIMD::Float(((packed[0]<<16) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(2, Min(Max(SIMD::Float(((packed[0]<<8) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			dst.move(3, Min(Max(SIMD::Float(((packed[0]) & SIMD::Int(0xFF000000))) * SIMD::Float(1.0f / float(0x7f000000)), SIMD::Float(-1.0f)), SIMD::Float(1.0f)));
			break;
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
			dst.move(0, SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(1, SIMD::Float(((packed[0]>>8) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(2, SIMD::Float(((packed[0]>>16) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(3, SIMD::Float(((packed[0]>>24) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			break;
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
			dst.move(0, ::sRGBtoLinear(SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(1, ::sRGBtoLinear(SIMD::Float(((packed[0]>>8) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(2, ::sRGBtoLinear(SIMD::Float(((packed[0]>>16) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(3, SIMD::Float(((packed[0]>>24) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			break;
		case VK_FORMAT_B8G8R8A8_UNORM:
			dst.move(0, SIMD::Float(((packed[0]>>16) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(1, SIMD::Float(((packed[0]>>8) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(2, SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(3, SIMD::Float(((packed[0]>>24) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			break;
		case VK_FORMAT_B8G8R8A8_SRGB:
			dst.move(0, ::sRGBtoLinear(SIMD::Float(((packed[0]>>16) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(1, ::sRGBtoLinear(SIMD::Float(((packed[0]>>8) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(2, ::sRGBtoLinear(SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f)));
			dst.move(3, SIMD::Float(((packed[0]>>24) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			break;
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
			dst.move(0, (As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF)));
			dst.move(1, ((As<SIMD::UInt>(packed[0])>>8) & SIMD::UInt(0xFF)));
			dst.move(2, ((As<SIMD::UInt>(packed[0])>>16) & SIMD::UInt(0xFF)));
			dst.move(3, ((As<SIMD::UInt>(packed[0])>>24) & SIMD::UInt(0xFF)));
			break;
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
			dst.move(0, (packed[0] << 24) >> 24);
			dst.move(1, (packed[0] << 16) >> 24);
			dst.move(2, (packed[0] << 8) >> 24);
			dst.move(3, (packed[0]) >> 24);
			break;
		case VK_FORMAT_R8_UNORM:
			dst.move(0, SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(1, SIMD::Float(0));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_S8_UINT:
			dst.move(0, (As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF)));
			dst.move(1, SIMD::UInt(0));
			dst.move(2, SIMD::UInt(0));
			dst.move(3, SIMD::UInt(1));
			break;
		case VK_FORMAT_R8_SINT:
			dst.move(0, (packed[0] << 24) >> 24);
			dst.move(1, SIMD::Int(0));
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R8G8_UNORM:
			dst.move(0, SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(1, SIMD::Float(((packed[0]>>8) & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 255.f));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_R8G8_UINT:
			dst.move(0, (As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF)));
			dst.move(1, ((As<SIMD::UInt>(packed[0])>>8) & SIMD::UInt(0xFF)));
			dst.move(2, SIMD::UInt(0));
			dst.move(3, SIMD::UInt(1));
			break;
		case VK_FORMAT_R8G8_SINT:
			dst.move(0, (packed[0] << 24) >> 24);
			dst.move(1, (packed[0] << 16) >> 24);
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R16_SFLOAT:
			dst.move(0, halfToFloatBits(As<SIMD::UInt>(packed[0]) & SIMD::UInt(0x0000FFFF)));
			dst.move(1, SIMD::Float(0));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_R16_UINT:
			dst.move(0, packed[0] & SIMD::Int(0xffff));
			dst.move(1, SIMD::UInt(0));
			dst.move(2, SIMD::UInt(0));
			dst.move(3, SIMD::UInt(1));
			break;
		case VK_FORMAT_R16_SINT:
			dst.move(0, (packed[0] << 16) >> 16);
			dst.move(1, SIMD::Int(0));
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R16G16_SFLOAT:
			dst.move(0, halfToFloatBits(As<SIMD::UInt>(packed[0]) & SIMD::UInt(0x0000FFFF)));
			dst.move(1, halfToFloatBits((As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFFFF0000)) >> 16));
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_R16G16_UINT:
			dst.move(0, packed[0] & SIMD::Int(0xffff));
			dst.move(1, (packed[0] >> 16) & SIMD::Int(0xffff));
			dst.move(2, SIMD::UInt(0));
			dst.move(3, SIMD::UInt(1));
			break;
		case VK_FORMAT_R16G16_SINT:
			dst.move(0, (packed[0] << 16) >> 16);
			dst.move(1, (packed[0]) >> 16);
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_UINT:
			dst.move(0, packed[0]);
			dst.move(1, packed[1]);
			dst.move(2, SIMD::Int(0));
			dst.move(3, SIMD::Int(1));
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			dst.move(0, packed[0]);
			dst.move(1, packed[1]);
			dst.move(2, SIMD::Float(0));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
			dst.move(0, (packed[0]) & SIMD::Int(0x3FF));
			dst.move(1, (packed[0] >> 10) & SIMD::Int(0x3FF));
			dst.move(2, (packed[0] >> 20) & SIMD::Int(0x3FF));
			dst.move(3, (packed[0] >> 30) & SIMD::Int(0x3));
			break;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
			dst.move(0, SIMD::Float((packed[0]) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
			dst.move(1, SIMD::Float((packed[0] >> 10) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
			dst.move(2, SIMD::Float((packed[0] >> 20) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
			dst.move(3, SIMD::Float((packed[0] >> 30) & SIMD::Int(0x3)) * SIMD::Float(1.0f / 0x3));
			break;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
			dst.move(0, SIMD::Float((packed[0] >> 11) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
			dst.move(1, SIMD::Float((packed[0] >> 5) & SIMD::Int(0x3F)) * SIMD::Float(1.0f / 0x3F));
			dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
			dst.move(3, SIMD::Float(1));
			break;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
			dst.move(0, SIMD::Float((packed[0] >> 10) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
			dst.move(1, SIMD::Float((packed[0] >> 5) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
			dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
			dst.move(3, SIMD::Float((packed[0] >> 15) & SIMD::Int(0x1)));
			break;
		default:
			UNIMPLEMENTED("VkFormat %d", int(vkFormat));
			break;
		}

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageWrite(InsnIterator insn, EmitState *state) const
	{
		auto imageId = Object::ID(insn.word(1));
		auto &image = getObject(imageId);
		auto &imageType = getType(image.type);

		ASSERT(imageType.definition.opcode() == spv::OpTypeImage);

		// TODO(b/131171141): Not handling any image operands yet.
		ASSERT(insn.wordCount() == 4);

		auto coordinate = GenericValue(this, state, insn.word(2));
		auto texel = GenericValue(this, state, insn.word(3));

		Pointer<Byte> binding = state->getPointer(imageId).base;
		Pointer<Byte> imageBase = *Pointer<Pointer<Byte>>(binding + OFFSET(vk::StorageImageDescriptor, ptr));
		auto imageSizeInBytes = *Pointer<Int>(binding + OFFSET(vk::StorageImageDescriptor, sizeInBytes));

		SIMD::Int packed[4];
		auto numPackedElements = 0u;
		int texelSize = 0;
		auto format = static_cast<spv::ImageFormat>(imageType.definition.word(8));
		switch (format)
		{
		case spv::ImageFormatRgba32f:
		case spv::ImageFormatRgba32i:
		case spv::ImageFormatRgba32ui:
			texelSize = 16;
			packed[0] = texel.Int(0);
			packed[1] = texel.Int(1);
			packed[2] = texel.Int(2);
			packed[3] = texel.Int(3);
			numPackedElements = 4;
			break;
		case spv::ImageFormatR32f:
		case spv::ImageFormatR32i:
		case spv::ImageFormatR32ui:
			texelSize = 4;
			packed[0] = texel.Int(0);
			numPackedElements = 1;
			break;
		case spv::ImageFormatRgba8:
			texelSize = 4;
			packed[0] = (SIMD::UInt(Round(Min(Max(texel.Float(0), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) |
				((SIMD::UInt(Round(Min(Max(texel.Float(1), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 8) |
				((SIMD::UInt(Round(Min(Max(texel.Float(2), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 16) |
				((SIMD::UInt(Round(Min(Max(texel.Float(3), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 24);
			numPackedElements = 1;
			break;
		case spv::ImageFormatRgba8Snorm:
			texelSize = 4;
			packed[0] = (SIMD::Int(Round(Min(Max(texel.Float(0), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
						 SIMD::Int(0xFF)) |
						((SIMD::Int(Round(Min(Max(texel.Float(1), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
						  SIMD::Int(0xFF)) << 8) |
						((SIMD::Int(Round(Min(Max(texel.Float(2), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
						  SIMD::Int(0xFF)) << 16) |
						((SIMD::Int(Round(Min(Max(texel.Float(3), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
						  SIMD::Int(0xFF)) << 24);
			numPackedElements = 1;
			break;
		case spv::ImageFormatRgba8i:
		case spv::ImageFormatRgba8ui:
			texelSize = 4;
			packed[0] = (SIMD::UInt(texel.UInt(0) & SIMD::UInt(0xff))) |
						(SIMD::UInt(texel.UInt(1) & SIMD::UInt(0xff)) << 8) |
						(SIMD::UInt(texel.UInt(2) & SIMD::UInt(0xff)) << 16) |
						(SIMD::UInt(texel.UInt(3) & SIMD::UInt(0xff)) << 24);
			numPackedElements = 1;
			break;
		case spv::ImageFormatRgba16f:
			texelSize = 8;
			packed[0] = FloatToHalfBits(texel.UInt(0), false) | FloatToHalfBits(texel.UInt(1), true);
			packed[1] = FloatToHalfBits(texel.UInt(2), false) | FloatToHalfBits(texel.UInt(3), true);
			numPackedElements = 2;
			break;
		case spv::ImageFormatRgba16i:
		case spv::ImageFormatRgba16ui:
			texelSize = 8;
			packed[0] = SIMD::UInt(texel.UInt(0) & SIMD::UInt(0xffff)) | (SIMD::UInt(texel.UInt(1) & SIMD::UInt(0xffff)) << 16);
			packed[1] = SIMD::UInt(texel.UInt(2) & SIMD::UInt(0xffff)) | (SIMD::UInt(texel.UInt(3) & SIMD::UInt(0xffff)) << 16);
			numPackedElements = 2;
			break;
		case spv::ImageFormatRg32f:
		case spv::ImageFormatRg32i:
		case spv::ImageFormatRg32ui:
			texelSize = 8;
			packed[0] = texel.Int(0);
			packed[1] = texel.Int(1);
			numPackedElements = 2;
			break;

		case spv::ImageFormatRg16f:
		case spv::ImageFormatR11fG11fB10f:
		case spv::ImageFormatR16f:
		case spv::ImageFormatRgba16:
		case spv::ImageFormatRgb10A2:
		case spv::ImageFormatRg16:
		case spv::ImageFormatRg8:
		case spv::ImageFormatR16:
		case spv::ImageFormatR8:
		case spv::ImageFormatRgba16Snorm:
		case spv::ImageFormatRg16Snorm:
		case spv::ImageFormatRg8Snorm:
		case spv::ImageFormatR16Snorm:
		case spv::ImageFormatR8Snorm:
		case spv::ImageFormatRg16i:
		case spv::ImageFormatRg8i:
		case spv::ImageFormatR16i:
		case spv::ImageFormatR8i:
		case spv::ImageFormatRgb10a2ui:
		case spv::ImageFormatRg16ui:
		case spv::ImageFormatRg8ui:
		case spv::ImageFormatR16ui:
		case spv::ImageFormatR8ui:
			UNIMPLEMENTED("spv::ImageFormat %d", int(format));
			break;

		default:
			UNREACHABLE("spv::ImageFormat %d", int(format));
			break;
		}

		auto basePtr = SIMD::Pointer(imageBase, imageSizeInBytes);
		auto texelPtr = GetTexelAddress(state, basePtr, coordinate, imageType, binding, texelSize, 0, false);

		// SPIR-V 1.4: "If the coordinates are outside the image, the memory location that is accessed is undefined."
		auto robustness = OutOfBoundsBehavior::UndefinedValue;

		for (auto i = 0u; i < numPackedElements; i++)
		{
			texelPtr.Store(packed[i], robustness, state->activeLaneMask());
			texelPtr += sizeof(float);
		}

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitImageTexelPointer(InsnIterator insn, EmitState *state) const
	{
		auto &resultType = getType(Type::ID(insn.word(1)));
		auto imageId = Object::ID(insn.word(3));
		auto &image = getObject(imageId);
		// Note: OpImageTexelPointer is unusual in that the image is passed by pointer.
		// Look through to get the actual image type.
		auto &imageType = getType(getType(image.type).element);
		Object::ID resultId = insn.word(2);

		ASSERT(imageType.opcode() == spv::OpTypeImage);
		ASSERT(resultType.storageClass == spv::StorageClassImage);
		ASSERT(getType(resultType.element).opcode() == spv::OpTypeInt);

		auto coordinate = GenericValue(this, state, insn.word(4));

		Pointer<Byte> binding = state->getPointer(imageId).base;
		Pointer<Byte> imageBase = *Pointer<Pointer<Byte>>(binding + OFFSET(vk::StorageImageDescriptor, ptr));
		auto imageSizeInBytes = *Pointer<Int>(binding + OFFSET(vk::StorageImageDescriptor, sizeInBytes));

		auto basePtr = SIMD::Pointer(imageBase, imageSizeInBytes);
		auto ptr = GetTexelAddress(state, basePtr, coordinate, imageType, binding, sizeof(uint32_t), 0, false);

		state->createPointer(resultId, ptr);

		return EmitResult::Continue;
	}

	SpirvShader::EmitResult SpirvShader::EmitSampledImageCombineOrSplit(InsnIterator insn, EmitState *state) const
	{
		// Propagate the image pointer in both cases.
		// Consumers of OpSampledImage will look through to find the sampler pointer.

		Object::ID resultId = insn.word(2);
		Object::ID imageId = insn.word(3);

		state->createPointer(resultId, state->getPointer(imageId));

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
		for (int j = 0; j < SIMD::Width; j++)
		{
			If(Extract(mask, j) != 0)
			{
				auto offset = Extract(ptrOffsets, j);
				auto laneValue = Extract(value, j);
				UInt v;
				switch (insn.opcode())
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
		for (int j = 0; j < SIMD::Width; j++)
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
		for (uint32_t i = 0; i < ty.sizeInComponents; i++)
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
		auto &arrayTy = getType(structTy.definition.word(2 + arrayFieldIdx));
		ASSERT(arrayTy.definition.opcode() == spv::OpTypeRuntimeArray);
		auto &arrayElTy = getType(arrayTy.element);

		auto &result = state->createIntermediate(resultId, 1);
		auto structBase = GetPointerToData(structPtrId, 0, state);

		Decorations d = {};
		ApplyDecorationsForIdMember(&d, structPtrTy.element, arrayFieldIdx);
		ASSERT(d.HasOffset);

		auto arrayBase = structBase + d.Offset;
		auto arraySizeInBytes = SIMD::Int(arrayBase.limit()) - arrayBase.offsets();
		auto arrayLength = arraySizeInBytes / SIMD::Int(arrayElTy.sizeInComponents * sizeof(float));

		result.move(0, SIMD::Int(arrayLength));

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
		switch (model)
		{
		case spv::ExecutionModelVertex:                 return VK_SHADER_STAGE_VERTEX_BIT;
		// case spv::ExecutionModelTessellationControl:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		// case spv::ExecutionModelTessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		// case spv::ExecutionModelGeometry:               return VK_SHADER_STAGE_GEOMETRY_BIT;
		case spv::ExecutionModelFragment:               return VK_SHADER_STAGE_FRAGMENT_BIT;
		case spv::ExecutionModelGLCompute:              return VK_SHADER_STAGE_COMPUTE_BIT;
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

	SpirvShader::GenericValue::GenericValue(SpirvShader const *shader, EmitState const *state, SpirvShader::Object::ID objId) :
			obj(shader->getObject(objId)),
			intermediate(obj.kind == SpirvShader::Object::Kind::Intermediate ? &state->getIntermediate(objId) : nullptr),
			type(obj.type) {}

	SpirvRoutine::SpirvRoutine(vk::PipelineLayout const *pipelineLayout) :
		pipelineLayout(pipelineLayout)
	{
	}

	void SpirvRoutine::setImmutableInputBuiltins(SpirvShader const *shader)
	{
		setInputBuiltin(shader, spv::BuiltInSubgroupLocalInvocationId, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 1);
			value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(0, 1, 2, 3));
		});

		setInputBuiltin(shader, spv::BuiltInSubgroupEqMask, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 4);
			value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(1, 2, 4, 8));
			value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});

		setInputBuiltin(shader, spv::BuiltInSubgroupGeMask, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 4);
			value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(15, 14, 12, 8));
			value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});

		setInputBuiltin(shader, spv::BuiltInSubgroupGtMask, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 4);
			value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(14, 12, 8, 0));
			value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});

		setInputBuiltin(shader, spv::BuiltInSubgroupLeMask, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 4);
			value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(1, 3, 7, 15));
			value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});

		setInputBuiltin(shader, spv::BuiltInSubgroupLtMask, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 4);
			value[builtin.FirstComponent + 0] = As<SIMD::Float>(SIMD::Int(0, 1, 3, 7));
			value[builtin.FirstComponent + 1] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 2] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
			value[builtin.FirstComponent + 3] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});

		setInputBuiltin(shader, spv::BuiltInDeviceIndex, [&](const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
		{
			ASSERT(builtin.SizeInComponents == 1);
			// Only a single physical device is supported.
			value[builtin.FirstComponent] = As<SIMD::Float>(SIMD::Int(0, 0, 0, 0));
		});
	}
}
