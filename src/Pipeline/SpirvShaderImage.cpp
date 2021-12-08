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

#include "System/Types.hpp"

#include "Vulkan/VkDescriptorSetLayout.hpp"
#include "Vulkan/VkPipelineLayout.hpp"

#include <spirv/unified1/spirv.hpp>

namespace {

vk::Format SpirvFormatToVulkanFormat(spv::ImageFormat format)
{
	switch(format)
	{
	case spv::ImageFormatUnknown: return VK_FORMAT_UNDEFINED;
	case spv::ImageFormatRgba32f: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case spv::ImageFormatRgba16f: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case spv::ImageFormatR32f: return VK_FORMAT_R32_SFLOAT;
	case spv::ImageFormatRgba8: return VK_FORMAT_R8G8B8A8_UNORM;
	case spv::ImageFormatRgba8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
	case spv::ImageFormatRg32f: return VK_FORMAT_R32G32_SFLOAT;
	case spv::ImageFormatRg16f: return VK_FORMAT_R16G16_SFLOAT;
	case spv::ImageFormatR11fG11fB10f: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case spv::ImageFormatR16f: return VK_FORMAT_R16_SFLOAT;
	case spv::ImageFormatRgba16: return VK_FORMAT_R16G16B16A16_UNORM;
	case spv::ImageFormatRgb10A2: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case spv::ImageFormatRg16: return VK_FORMAT_R16G16_UNORM;
	case spv::ImageFormatRg8: return VK_FORMAT_R8G8_UNORM;
	case spv::ImageFormatR16: return VK_FORMAT_R16_UNORM;
	case spv::ImageFormatR8: return VK_FORMAT_R8_UNORM;
	case spv::ImageFormatRgba16Snorm: return VK_FORMAT_R16G16B16A16_SNORM;
	case spv::ImageFormatRg16Snorm: return VK_FORMAT_R16G16_SNORM;
	case spv::ImageFormatRg8Snorm: return VK_FORMAT_R8G8_SNORM;
	case spv::ImageFormatR16Snorm: return VK_FORMAT_R16_SNORM;
	case spv::ImageFormatR8Snorm: return VK_FORMAT_R8_SNORM;
	case spv::ImageFormatRgba32i: return VK_FORMAT_R32G32B32A32_SINT;
	case spv::ImageFormatRgba16i: return VK_FORMAT_R16G16B16A16_SINT;
	case spv::ImageFormatRgba8i: return VK_FORMAT_R8G8B8A8_SINT;
	case spv::ImageFormatR32i: return VK_FORMAT_R32_SINT;
	case spv::ImageFormatRg32i: return VK_FORMAT_R32G32_SINT;
	case spv::ImageFormatRg16i: return VK_FORMAT_R16G16_SINT;
	case spv::ImageFormatRg8i: return VK_FORMAT_R8G8_SINT;
	case spv::ImageFormatR16i: return VK_FORMAT_R16_SINT;
	case spv::ImageFormatR8i: return VK_FORMAT_R8_SINT;
	case spv::ImageFormatRgba32ui: return VK_FORMAT_R32G32B32A32_UINT;
	case spv::ImageFormatRgba16ui: return VK_FORMAT_R16G16B16A16_UINT;
	case spv::ImageFormatRgba8ui: return VK_FORMAT_R8G8B8A8_UINT;
	case spv::ImageFormatR32ui: return VK_FORMAT_R32_UINT;
	case spv::ImageFormatRgb10a2ui: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
	case spv::ImageFormatRg32ui: return VK_FORMAT_R32G32_UINT;
	case spv::ImageFormatRg16ui: return VK_FORMAT_R16G16_UINT;
	case spv::ImageFormatRg8ui: return VK_FORMAT_R8G8_UINT;
	case spv::ImageFormatR16ui: return VK_FORMAT_R16_UINT;
	case spv::ImageFormatR8ui: return VK_FORMAT_R8_UINT;

	default:
		UNSUPPORTED("SPIR-V ImageFormat %u", format);
		return VK_FORMAT_UNDEFINED;
	}
}

sw::SIMD::Float sRGBtoLinear(sw::SIMD::Float c)
{
	sw::SIMD::Float lc = c * sw::SIMD::Float(1.0f / 12.92f);
	sw::SIMD::Float ec = sw::power((c + sw::SIMD::Float(0.055f)) * sw::SIMD::Float(1.0f / 1.055f), sw::SIMD::Float(2.4f));

	sw::SIMD::Int linear = CmpLT(c, sw::SIMD::Float(0.04045f));

	return rr::As<sw::SIMD::Float>((linear & rr::As<sw::SIMD::Int>(lc)) | (~linear & rr::As<sw::SIMD::Int>(ec)));  // TODO: IfThenElse()
}

}  // anonymous namespace

namespace sw {

SpirvShader::ImageInstruction::ImageInstruction(InsnIterator insn, const SpirvShader &spirv)
    : ImageInstructionSignature(parseVariantAndMethod(insn))
    , position(insn.distanceFrom(spirv.begin()))
{
	if(samplerMethod == Write)
	{
		imageId = insn.word(1);
		coordinateId = insn.word(2);
		texelId = insn.word(3);
	}
	else
	{
		resultTypeId = insn.resultTypeId();  // word(1)
		resultId = insn.resultId();          // word(2)

		if(samplerMethod == Fetch || samplerMethod == Read || samplerMethod == TexelPointer)  // Samplerless
		{
			imageId = insn.word(3);
		}
		else
		{
			Object::ID sampledImageId = insn.word(3);
			const Object &sampledImage = spirv.getObject(sampledImageId);

			if(sampledImage.opcode() == spv::OpSampledImage)
			{
				imageId = sampledImage.definition.word(3);
				samplerId = sampledImage.definition.word(4);
			}
			else  // Combined image/sampler
			{
				imageId = sampledImageId;
				samplerId = sampledImageId;
			}
		}

		coordinateId = insn.word(4);
	}

	// `imageId` can represent either a Sampled Image, a samplerless Image, or a pointer to an Image.
	// To get to the OpTypeImage operands, traverse the OpTypeSampledImage or OpTypePointer.
	const Type &imageObjectType = spirv.getObjectType(imageId);
	const Type &imageReferenceType = (imageObjectType.opcode() == spv::OpTypeSampledImage)
	                                     ? spirv.getType(imageObjectType.definition.word(2))
	                                     : imageObjectType;
	const Type &imageType = ((imageReferenceType.opcode() == spv::OpTypePointer)
	                             ? spirv.getType(imageReferenceType.element)
	                             : imageReferenceType);

	ASSERT(imageType.opcode() == spv::OpTypeImage);
	dim = imageType.definition.word(3);
	arrayed = imageType.definition.word(5);
	imageFormat = imageType.definition.word(8);

	const Object &coordinateObject = spirv.getObject(coordinateId);
	const Type &coordinateType = spirv.getType(coordinateObject);
	coordinates = coordinateType.componentCount - (isProj() ? 1 : 0);

	if(samplerMethod == TexelPointer)
	{
		sampleId = insn.word(5);
		sample = !spirv.getObject(sampleId).isConstantZero();
	}

	if(isDref())
	{
		drefId = insn.word(5);
	}

	if(samplerMethod == Gather)
	{
		gatherComponent = !isDref() ? spirv.getObject(insn.word(5)).constantValue[0] : 0;
	}

	uint32_t operandsIndex = getImageOperandsIndex(insn);
	uint32_t imageOperands = (operandsIndex != 0) ? insn.word(operandsIndex) : 0;  // The mask which indicates which operands are provided.

	operandsIndex += 1;  // Advance to the first actual operand <id> location.

	if(imageOperands & spv::ImageOperandsBiasMask)
	{
		ASSERT(samplerMethod == Bias);
		lodOrBiasId = insn.word(operandsIndex);
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsBiasMask;
	}

	if(imageOperands & spv::ImageOperandsLodMask)
	{
		ASSERT(samplerMethod == Lod || samplerMethod == Fetch);
		lodOrBiasId = insn.word(operandsIndex);
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsLodMask;
	}

	if(imageOperands & spv::ImageOperandsGradMask)
	{
		ASSERT(samplerMethod == Grad);
		gradDxId = insn.word(operandsIndex + 0);
		gradDyId = insn.word(operandsIndex + 1);
		operandsIndex += 2;
		imageOperands &= ~spv::ImageOperandsGradMask;

		grad = spirv.getObjectType(gradDxId).componentCount;
	}

	if(imageOperands & spv::ImageOperandsConstOffsetMask)
	{
		offsetId = insn.word(operandsIndex);
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsConstOffsetMask;

		offset = spirv.getObjectType(offsetId).componentCount;
	}

	if(imageOperands & spv::ImageOperandsSampleMask)
	{
		ASSERT(samplerMethod == Fetch || samplerMethod == Read || samplerMethod == Write);
		sampleId = insn.word(operandsIndex);
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsSampleMask;

		sample = !spirv.getObject(sampleId).isConstantZero();
	}

	// TODO(b/174475384)
	if(imageOperands & spv::ImageOperandsZeroExtendMask)
	{
		ASSERT(samplerMethod == Read || samplerMethod == Write);
		imageOperands &= ~spv::ImageOperandsZeroExtendMask;
	}
	else if(imageOperands & spv::ImageOperandsSignExtendMask)
	{
		ASSERT(samplerMethod == Read || samplerMethod == Write);
		imageOperands &= ~spv::ImageOperandsSignExtendMask;
	}

	[[maybe_unused]] spv::Scope scope = spv::ScopeCrossDevice;  // "Whilst the CrossDevice scope is defined in SPIR-V, it is disallowed in Vulkan."

	if(imageOperands & spv::ImageOperandsMakeTexelAvailableMask)
	{
		scope = static_cast<spv::Scope>(insn.word(operandsIndex));
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsMakeTexelAvailableMask;
	}

	if(imageOperands & spv::ImageOperandsMakeTexelVisibleMask)
	{
		scope = static_cast<spv::Scope>(insn.word(operandsIndex));
		operandsIndex += 1;
		imageOperands &= ~spv::ImageOperandsMakeTexelVisibleMask;
	}

	if(imageOperands & spv::ImageOperandsNonPrivateTexelMask)
	{
		imageOperands &= ~spv::ImageOperandsNonPrivateTexelMask;
	}

	if(imageOperands & spv::ImageOperandsVolatileTexelMask)
	{
		UNIMPLEMENTED("b/176819536");
		imageOperands &= ~spv::ImageOperandsVolatileTexelMask;
	}

	// There should be no remaining image operands.
	if(imageOperands != 0)
	{
		UNSUPPORTED("Image operands 0x%08X", imageOperands);
	}
}

SpirvShader::ImageInstructionSignature SpirvShader::ImageInstruction::parseVariantAndMethod(InsnIterator insn)
{
	uint32_t imageOperands = getImageOperandsMask(insn);
	bool bias = imageOperands & spv::ImageOperandsBiasMask;
	bool grad = imageOperands & spv::ImageOperandsGradMask;

	switch(insn.opcode())
	{
	case spv::OpImageSampleImplicitLod: return { None, bias ? Bias : Implicit };
	case spv::OpImageSampleExplicitLod: return { None, grad ? Grad : Lod };
	case spv::OpImageSampleDrefImplicitLod: return { Dref, bias ? Bias : Implicit };
	case spv::OpImageSampleDrefExplicitLod: return { Dref, grad ? Grad : Lod };
	case spv::OpImageSampleProjImplicitLod: return { Proj, bias ? Bias : Implicit };
	case spv::OpImageSampleProjExplicitLod: return { Proj, grad ? Grad : Lod };
	case spv::OpImageSampleProjDrefImplicitLod: return { ProjDref, bias ? Bias : Implicit };
	case spv::OpImageSampleProjDrefExplicitLod: return { ProjDref, grad ? Grad : Lod };
	case spv::OpImageGather: return { None, Gather };
	case spv::OpImageDrefGather: return { Dref, Gather };
	case spv::OpImageFetch: return { None, Fetch };
	case spv::OpImageQueryLod: return { None, Query };
	case spv::OpImageRead: return { None, Read };
	case spv::OpImageWrite: return { None, Write };
	case spv::OpImageTexelPointer: return { None, TexelPointer };

	default:
		ASSERT(false);
		return { None, Implicit };
	}
}

// Returns the instruction word index at which the Image Operands mask is located, or 0 if not present.
uint32_t SpirvShader::ImageInstruction::getImageOperandsIndex(InsnIterator insn)
{
	switch(insn.opcode())
	{
	case spv::OpImageSampleImplicitLod:
	case spv::OpImageSampleProjImplicitLod:
		return insn.wordCount() > 5 ? 5 : 0;  // Optional
	case spv::OpImageSampleExplicitLod:
	case spv::OpImageSampleProjExplicitLod:
		return 5;  // "Either Lod or Grad image operands must be present."
	case spv::OpImageSampleDrefImplicitLod:
	case spv::OpImageSampleProjDrefImplicitLod:
		return insn.wordCount() > 6 ? 6 : 0;  // Optional
	case spv::OpImageSampleDrefExplicitLod:
	case spv::OpImageSampleProjDrefExplicitLod:
		return 6;  // "Either Lod or Grad image operands must be present."
	case spv::OpImageGather:
	case spv::OpImageDrefGather:
		return insn.wordCount() > 6 ? 6 : 0;  // Optional
	case spv::OpImageFetch:
		return insn.wordCount() > 5 ? 5 : 0;  // Optional
	case spv::OpImageQueryLod:
		ASSERT(insn.wordCount() == 5);
		return 0;  // No image operands.
	case spv::OpImageRead:
		return insn.wordCount() > 5 ? 5 : 0;  // Optional
	case spv::OpImageWrite:
		return insn.wordCount() > 4 ? 4 : 0;  // Optional
	case spv::OpImageTexelPointer:
		ASSERT(insn.wordCount() == 6);
		return 0;  // No image operands.

	default:
		ASSERT(false);
		return 0;
	}
}

uint32_t SpirvShader::ImageInstruction::getImageOperandsMask(InsnIterator insn)
{
	uint32_t operandsIndex = getImageOperandsIndex(insn);
	return (operandsIndex != 0) ? insn.word(operandsIndex) : 0;
}

SpirvShader::EmitResult SpirvShader::EmitImageSample(const ImageInstruction &instruction, EmitState *state) const
{
	auto &resultType = getType(instruction.resultTypeId);
	auto &result = state->createIntermediate(instruction.resultId, resultType.componentCount);
	Array<SIMD::Float> out(4);

	// TODO(b/153380916): When we're in a code path that is always executed,
	// i.e. post-dominators of the entry block, we don't have to dynamically
	// check whether any lanes are active, and can elide the jump.
	If(AnyTrue(state->activeLaneMask()))
	{
		EmitImageSampleUnconditional(out, instruction, state);
	}

	for(auto i = 0u; i < resultType.componentCount; i++) { result.move(i, out[i]); }

	return EmitResult::Continue;
}

void SpirvShader::EmitImageSampleUnconditional(Array<SIMD::Float> &out, const ImageInstruction &instruction, EmitState *state) const
{
	Pointer<Byte> imageDescriptor = state->getPointer(instruction.imageId).base;  // vk::SampledImageDescriptor*

	Pointer<Byte> samplerFunction = lookupSamplerFunction(imageDescriptor, instruction, state);

	callSamplerFunction(samplerFunction, out, imageDescriptor, instruction, state);
}

Pointer<Byte> SpirvShader::lookupSamplerFunction(Pointer<Byte> imageDescriptor, const ImageInstruction &instruction, EmitState *state) const
{
	Int samplerId = 0;

	if(instruction.samplerId != 0)
	{
		Pointer<Byte> samplerDescriptor = state->getPointer(instruction.samplerId).base;  // vk::SampledImageDescriptor*

		samplerId = *Pointer<rr::Int>(samplerDescriptor + OFFSET(vk::SampledImageDescriptor, samplerId));  // vk::Sampler::id
	}

	auto &cache = state->routine->samplerCache.at(instruction.position);
	Bool cacheHit = (cache.imageDescriptor == imageDescriptor) && (cache.samplerId == samplerId);  // TODO(b/205566405): Skip sampler ID check for samplerless instructions.

	If(!cacheHit)
	{
		rr::Int imageViewId = *Pointer<rr::Int>(imageDescriptor + OFFSET(vk::ImageDescriptor, imageViewId));
		cache.function = Call(getImageSampler, state->routine->device, instruction.signature, samplerId, imageViewId);
		cache.imageDescriptor = imageDescriptor;
		cache.samplerId = samplerId;
	}

	return cache.function;
}

void SpirvShader::callSamplerFunction(Pointer<Byte> samplerFunction, Array<SIMD::Float> &out, Pointer<Byte> imageDescriptor, const ImageInstruction &instruction, EmitState *state) const
{
	Array<SIMD::Float> in(16);  // Maximum 16 input parameter components.

	auto coordinate = Operand(this, state, instruction.coordinateId);

	uint32_t i = 0;
	for(; i < instruction.coordinates; i++)
	{
		if(instruction.isProj())
		{
			in[i] = coordinate.Float(i) / coordinate.Float(instruction.coordinates);  // TODO(b/129523279): Optimize using reciprocal.
		}
		else
		{
			in[i] = coordinate.Float(i);
		}
	}

	if(instruction.isDref())
	{
		auto drefValue = Operand(this, state, instruction.drefId);

		if(instruction.isProj())
		{
			in[i] = drefValue.Float(0) / coordinate.Float(instruction.coordinates);  // TODO(b/129523279): Optimize using reciprocal.
		}
		else
		{
			in[i] = drefValue.Float(0);
		}

		i++;
	}

	if(instruction.lodOrBiasId != 0)
	{
		auto lodValue = Operand(this, state, instruction.lodOrBiasId);
		in[i] = lodValue.Float(0);
		i++;
	}
	else if(instruction.gradDxId != 0)
	{
		auto dxValue = Operand(this, state, instruction.gradDxId);
		auto dyValue = Operand(this, state, instruction.gradDyId);
		ASSERT(dxValue.componentCount == dxValue.componentCount);

		for(uint32_t j = 0; j < dxValue.componentCount; j++, i++)
		{
			in[i] = dxValue.Float(j);
		}

		for(uint32_t j = 0; j < dxValue.componentCount; j++, i++)
		{
			in[i] = dyValue.Float(j);
		}
	}
	else if(instruction.samplerMethod == Fetch)
	{
		// The instruction didn't provide a lod operand, but the sampler's Fetch
		// function requires one to be present. If no lod is supplied, the default
		// is zero.
		in[i] = As<SIMD::Float>(SIMD::Int(0));
		i++;
	}

	if(instruction.offsetId != 0)
	{
		auto offsetValue = Operand(this, state, instruction.offsetId);

		for(uint32_t j = 0; j < offsetValue.componentCount; j++, i++)
		{
			in[i] = As<SIMD::Float>(offsetValue.Int(j));  // Integer values, but transfered as float.
		}
	}

	if(instruction.sample)
	{
		auto sampleValue = Operand(this, state, instruction.sampleId);
		in[i] = As<SIMD::Float>(sampleValue.Int(0));
	}

	Pointer<Byte> texture = imageDescriptor + OFFSET(vk::SampledImageDescriptor, texture);  // sw::Texture*

	Call<ImageSampler>(samplerFunction, texture, &in, &out, state->routine->constants);
}

SpirvShader::EmitResult SpirvShader::EmitImageQuerySizeLod(InsnIterator insn, EmitState *state) const
{
	auto &resultTy = getType(insn.resultTypeId());
	auto imageId = Object::ID(insn.word(3));
	auto lodId = Object::ID(insn.word(4));

	auto &dst = state->createIntermediate(insn.resultId(), resultTy.componentCount);
	GetImageDimensions(state, resultTy, imageId, lodId, dst);

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitImageQuerySize(InsnIterator insn, EmitState *state) const
{
	auto &resultTy = getType(insn.resultTypeId());
	auto imageId = Object::ID(insn.word(3));
	auto lodId = Object::ID(0);

	auto &dst = state->createIntermediate(insn.resultId(), resultTy.componentCount);
	GetImageDimensions(state, resultTy, imageId, lodId, dst);

	return EmitResult::Continue;
}

void SpirvShader::GetImageDimensions(EmitState const *state, Type const &resultTy, Object::ID imageId, Object::ID lodId, Intermediate &dst) const
{
	auto routine = state->routine;
	auto &image = getObject(imageId);
	auto &imageType = getType(image);

	ASSERT(imageType.definition.opcode() == spv::OpTypeImage);
	bool isArrayed = imageType.definition.word(5) != 0;
	uint32_t dimensions = resultTy.componentCount - (isArrayed ? 1 : 0);

	const DescriptorDecorations &d = descriptorDecorations.at(imageId);
	auto descriptorType = routine->pipelineLayout->getDescriptorType(d.DescriptorSet, d.Binding);

	Pointer<Byte> descriptor = state->getPointer(imageId).base;

	Int width;
	Int height;
	Int depth;

	switch(descriptorType)
	{
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		width = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, width));
		height = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, height));
		depth = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, depth));
		break;
	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		width = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, width));
		height = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, height));
		depth = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, depth));
		break;
	default:
		UNREACHABLE("Image descriptorType: %d", int(descriptorType));
	}

	if(lodId != 0)
	{
		auto lodVal = Operand(this, state, lodId);
		ASSERT(lodVal.componentCount == 1);
		auto lod = lodVal.Int(0);
		auto one = SIMD::Int(1);

		if(dimensions >= 1) dst.move(0, Max(SIMD::Int(width) >> lod, one));
		if(dimensions >= 2) dst.move(1, Max(SIMD::Int(height) >> lod, one));
		if(dimensions >= 3) dst.move(2, Max(SIMD::Int(depth) >> lod, one));
	}
	else
	{

		if(dimensions >= 1) dst.move(0, SIMD::Int(width));
		if(dimensions >= 2) dst.move(1, SIMD::Int(height));
		if(dimensions >= 3) dst.move(2, SIMD::Int(depth));
	}

	if(isArrayed)
	{
		dst.move(dimensions, SIMD::Int(depth));
	}
}

SpirvShader::EmitResult SpirvShader::EmitImageQueryLevels(InsnIterator insn, EmitState *state) const
{
	auto &resultTy = getType(insn.resultTypeId());
	ASSERT(resultTy.componentCount == 1);
	auto imageId = Object::ID(insn.word(3));

	const DescriptorDecorations &d = descriptorDecorations.at(imageId);
	auto descriptorType = state->routine->pipelineLayout->getDescriptorType(d.DescriptorSet, d.Binding);

	Pointer<Byte> descriptor = state->getPointer(imageId).base;
	Int mipLevels = 0;
	switch(descriptorType)
	{
	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		mipLevels = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, mipLevels));  // uint32_t
		break;
	default:
		UNREACHABLE("Image descriptorType: %d", int(descriptorType));
	}

	auto &dst = state->createIntermediate(insn.resultId(), 1);
	dst.move(0, SIMD::Int(mipLevels));

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitImageQuerySamples(InsnIterator insn, EmitState *state) const
{
	auto &resultTy = getType(insn.resultTypeId());
	ASSERT(resultTy.componentCount == 1);
	auto imageId = Object::ID(insn.word(3));
	auto imageTy = getObjectType(imageId);
	ASSERT(imageTy.definition.opcode() == spv::OpTypeImage);
	ASSERT(imageTy.definition.word(3) == spv::Dim2D);
	ASSERT(imageTy.definition.word(6 /* MS */) == 1);

	const DescriptorDecorations &d = descriptorDecorations.at(imageId);
	auto descriptorType = state->routine->pipelineLayout->getDescriptorType(d.DescriptorSet, d.Binding);

	Pointer<Byte> descriptor = state->getPointer(imageId).base;
	Int sampleCount = 0;
	switch(descriptorType)
	{
	case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		sampleCount = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, sampleCount));  // uint32_t
		break;
	case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
	case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		sampleCount = *Pointer<Int>(descriptor + OFFSET(vk::SampledImageDescriptor, sampleCount));  // uint32_t
		break;
	default:
		UNREACHABLE("Image descriptorType: %d", int(descriptorType));
	}

	auto &dst = state->createIntermediate(insn.resultId(), 1);
	dst.move(0, SIMD::Int(sampleCount));

	return EmitResult::Continue;
}

SIMD::Pointer SpirvShader::GetTexelAddress(ImageInstructionSignature instruction, Pointer<Byte> descriptor, SIMD::Int coordinate[], SIMD::Int sample, vk::Format imageFormat, OutOfBoundsBehavior outOfBoundsBehavior, const EmitState *state)
{
	bool isArrayed = instruction.arrayed;
	spv::Dim dim = static_cast<spv::Dim>(instruction.dim);
	int dims = instruction.coordinates - (isArrayed ? 1 : 0);

	SIMD::Int u = coordinate[0];
	SIMD::Int v = SIMD::Int(0);

	if(dims > 1)
	{
		v = coordinate[1];
	}

	if(dim == spv::DimSubpassData)
	{
		u += state->routine->windowSpacePosition[0];
		v += state->routine->windowSpacePosition[1];
	}

	const int texelSize = imageFormat.bytes();
	const bool useStencilAspect = (imageFormat == VK_FORMAT_S8_UINT);
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

	SIMD::Int ptrOffset = u * SIMD::Int(texelSize);

	if(dims > 1)
	{
		ptrOffset += v * rowPitch;
	}

	SIMD::Int w = 0;
	if((dims > 2) || isArrayed)
	{
		if(dims > 2)
		{
			w += coordinate[2];
		}

		if(isArrayed)
		{
			w += coordinate[dims];
		}

		ptrOffset += w * slicePitch;
	}

	if(dim == spv::DimSubpassData)
	{
		// Multiview input attachment access is to the layer corresponding to the current view
		ptrOffset += SIMD::Int(state->routine->viewID) * slicePitch;
	}

	if(instruction.sample)
	{
		ptrOffset += sample * samplePitch;
	}

	// If the out-of-bounds behavior is set to nullify, then each coordinate must be tested individually.
	// Other out-of-bounds behaviors work properly by just comparing the offset against the total size.
	if(outOfBoundsBehavior == OutOfBoundsBehavior::Nullify)
	{
		SIMD::UInt width = *Pointer<UInt>(descriptor + OFFSET(vk::StorageImageDescriptor, width));
		SIMD::Int oobMask = As<SIMD::Int>(CmpNLT(As<SIMD::UInt>(u), width));

		if(dims > 1)
		{
			SIMD::UInt height = *Pointer<UInt>(descriptor + OFFSET(vk::StorageImageDescriptor, height));
			oobMask |= As<SIMD::Int>(CmpNLT(As<SIMD::UInt>(v), height));
		}

		if((dims > 2) || isArrayed)
		{
			UInt depth = *Pointer<UInt>(descriptor + OFFSET(vk::StorageImageDescriptor, depth));
			if(dim == spv::DimCube) { depth *= 6; }
			oobMask |= As<SIMD::Int>(CmpNLT(As<SIMD::UInt>(w), SIMD::UInt(depth)));
		}

		if(instruction.sample)
		{
			SIMD::UInt sampleCount = *Pointer<UInt>(descriptor + OFFSET(vk::StorageImageDescriptor, sampleCount));
			oobMask |= As<SIMD::Int>(CmpNLT(As<SIMD::UInt>(sample), sampleCount));
		}

		constexpr int32_t OOB_OFFSET = 0x7FFFFFFF - 16;  // SIMD pointer offsets are signed 32-bit, so this is the largest offset (for 16-byte texels).
		static_assert(OOB_OFFSET >= vk::MAX_MEMORY_ALLOCATION_SIZE, "the largest offset must be guaranteed to be out-of-bounds");

		ptrOffset = (ptrOffset & ~oobMask) | (oobMask & SIMD::Int(OOB_OFFSET));  // oob ? OOB_OFFSET : ptrOffset  // TODO: IfThenElse()
	}

	Pointer<Byte> imageBase = *Pointer<Pointer<Byte>>(descriptor + (useStencilAspect
	                                                                    ? OFFSET(vk::StorageImageDescriptor, stencilPtr)
	                                                                    : OFFSET(vk::StorageImageDescriptor, ptr)));

	Int imageSizeInBytes = *Pointer<Int>(descriptor + OFFSET(vk::StorageImageDescriptor, sizeInBytes));

	return SIMD::Pointer(imageBase, imageSizeInBytes, ptrOffset);
}

SpirvShader::EmitResult SpirvShader::EmitImageRead(const ImageInstruction &instruction, EmitState *state) const
{
	auto &resultType = getObjectType(instruction.resultId);
	auto &image = getObject(instruction.imageId);
	auto &imageType = getType(image);

	ASSERT(imageType.definition.opcode() == spv::OpTypeImage);
	auto dim = static_cast<spv::Dim>(instruction.dim);

	auto coordinate = Operand(this, state, instruction.coordinateId);
	const DescriptorDecorations &d = descriptorDecorations.at(instruction.imageId);

	// For subpass data, format in the instruction is spv::ImageFormatUnknown. Get it from
	// the renderpass data instead. In all other cases, we can use the format in the instruction.
	vk::Format imageFormat = (dim == spv::DimSubpassData)
	                             ? inputAttachmentFormats[d.InputAttachmentIndex]
	                             : SpirvFormatToVulkanFormat(static_cast<spv::ImageFormat>(instruction.imageFormat));

	// Depth+Stencil image attachments select aspect based on the Sampled Type of the
	// OpTypeImage. If float, then we want the depth aspect. If int, we want the stencil aspect.
	bool useStencilAspect = (imageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT &&
	                         getType(imageType.definition.word(2)).opcode() == spv::OpTypeInt);

	if(useStencilAspect)
	{
		imageFormat = VK_FORMAT_S8_UINT;
	}

	Pointer<Byte> descriptor = state->getPointer(instruction.imageId).base;  // vk::StorageImageDescriptor*
	auto &dst = state->createIntermediate(instruction.resultId, resultType.componentCount);

	// VK_EXT_image_robustness requires replacing out-of-bounds access with zero.
	// TODO(b/162327166): Only perform bounds checks when VK_EXT_image_robustness is enabled.
	auto robustness = OutOfBoundsBehavior::Nullify;

	SIMD::Int uvwa[4];
	SIMD::Int sample;

	for(uint32_t i = 0; i < instruction.coordinates; i++)
	{
		uvwa[i] = coordinate.Int(i);
	}

	if(instruction.sample)
	{
		sample = Operand(this, state, instruction.sampleId).Int(0);
	}

	auto texelPtr = GetTexelAddress(instruction, descriptor, uvwa, sample, imageFormat, robustness, state);

	const int texelSize = imageFormat.bytes();

	// Gather packed texel data. Texels larger than 4 bytes occupy multiple SIMD::Int elements.
	// TODO(b/160531165): Provide gather abstractions for various element sizes.
	SIMD::Int packed[4];
	if(texelSize == 4 || texelSize == 8 || texelSize == 16)
	{
		for(auto i = 0; i < texelSize / 4; i++)
		{
			packed[i] = texelPtr.Load<SIMD::Int>(robustness, state->activeLaneMask());
			texelPtr += sizeof(float);
		}
	}
	else if(texelSize == 2)
	{
		SIMD::Int offsets = texelPtr.offsets();
		SIMD::Int mask = state->activeLaneMask() & texelPtr.isInBounds(2, robustness);

		for(int i = 0; i < SIMD::Width; i++)
		{
			If(Extract(mask, i) != 0)
			{
				packed[0] = Insert(packed[0], Int(*Pointer<Short>(texelPtr.base + Extract(offsets, i))), i);
			}
		}
	}
	else if(texelSize == 1)
	{
		SIMD::Int offsets = texelPtr.offsets();
		SIMD::Int mask = state->activeLaneMask() & texelPtr.isInBounds(1, robustness);

		for(int i = 0; i < SIMD::Width; i++)
		{
			If(Extract(mask, i) != 0)
			{
				packed[0] = Insert(packed[0], Int(*Pointer<Byte>(texelPtr.base + Extract(offsets, i))), i);
			}
		}
	}
	else
		UNREACHABLE("texelSize: %d", int(texelSize));

	// Format support requirements here come from two sources:
	// - Minimum required set of formats for loads from storage images
	// - Any format supported as a color or depth/stencil attachment, for input attachments
	switch(imageFormat)
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
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_D16_UNORM:
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(1, SIMD::Float((packed[0] >> 16) & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(2, SIMD::Float(packed[1] & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(3, SIMD::Float((packed[1] >> 16) & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		break;
	case VK_FORMAT_R16G16B16A16_SNORM:
		dst.move(0, Max(SIMD::Float((packed[0] << 16) & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(1, Max(SIMD::Float(packed[0] & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(2, Max(SIMD::Float((packed[1] << 16) & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(3, Max(SIMD::Float(packed[1] & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		break;
	case VK_FORMAT_R16G16B16A16_SINT:
		dst.move(0, (packed[0] << 16) >> 16);
		dst.move(1, packed[0] >> 16);
		dst.move(2, (packed[1] << 16) >> 16);
		dst.move(3, packed[1] >> 16);
		break;
	case VK_FORMAT_R16G16B16A16_UINT:
		dst.move(0, packed[0] & SIMD::Int(0xFFFF));
		dst.move(1, (packed[0] >> 16) & SIMD::Int(0xFFFF));
		dst.move(2, packed[1] & SIMD::Int(0xFFFF));
		dst.move(3, (packed[1] >> 16) & SIMD::Int(0xFFFF));
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		dst.move(0, halfToFloatBits(As<SIMD::UInt>(packed[0]) & SIMD::UInt(0x0000FFFF)));
		dst.move(1, halfToFloatBits((As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFFFF0000)) >> 16));
		dst.move(2, halfToFloatBits(As<SIMD::UInt>(packed[1]) & SIMD::UInt(0x0000FFFF)));
		dst.move(3, halfToFloatBits((As<SIMD::UInt>(packed[1]) & SIMD::UInt(0xFFFF0000)) >> 16));
		break;
	case VK_FORMAT_R8G8B8A8_SNORM:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		dst.move(0, Max(SIMD::Float((packed[0] << 24) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(1, Max(SIMD::Float((packed[0] << 16) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(2, Max(SIMD::Float((packed[0] << 8) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(3, Max(SIMD::Float((packed[0]) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(1, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(2, SIMD::Float((packed[0] >> 16) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(3, SIMD::Float((packed[0] >> 24) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		break;
	case VK_FORMAT_R8G8B8A8_SRGB:
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		dst.move(0, ::sRGBtoLinear(SIMD::Float(packed[0] & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(1, ::sRGBtoLinear(SIMD::Float((packed[0] >> 8) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(2, ::sRGBtoLinear(SIMD::Float((packed[0] >> 16) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(3, SIMD::Float((packed[0] >> 24) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		dst.move(0, SIMD::Float((packed[0] >> 16) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(1, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(2, SIMD::Float(packed[0] & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(3, SIMD::Float((packed[0] >> 24) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		break;
	case VK_FORMAT_B8G8R8A8_SRGB:
		dst.move(0, ::sRGBtoLinear(SIMD::Float((packed[0] >> 16) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(1, ::sRGBtoLinear(SIMD::Float((packed[0] >> 8) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(2, ::sRGBtoLinear(SIMD::Float(packed[0] & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF)));
		dst.move(3, SIMD::Float((packed[0] >> 24) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		break;
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		dst.move(0, As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF));
		dst.move(1, (As<SIMD::UInt>(packed[0]) >> 8) & SIMD::UInt(0xFF));
		dst.move(2, (As<SIMD::UInt>(packed[0]) >> 16) & SIMD::UInt(0xFF));
		dst.move(3, (As<SIMD::UInt>(packed[0]) >> 24) & SIMD::UInt(0xFF));
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		dst.move(0, (packed[0] << 24) >> 24);
		dst.move(1, (packed[0] << 16) >> 24);
		dst.move(2, (packed[0] << 8) >> 24);
		dst.move(3, packed[0] >> 24);
		break;
	case VK_FORMAT_R8_UNORM:
		dst.move(0, SIMD::Float((packed[0] & SIMD::Int(0xFF))) * SIMD::Float(1.0f / 0xFF));
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R8_SNORM:
		dst.move(0, Max(SIMD::Float((packed[0] << 24) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R8_UINT:
	case VK_FORMAT_S8_UINT:
		dst.move(0, As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF));
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
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(1, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xFF)) * SIMD::Float(1.0f / 0xFF));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R8G8_SNORM:
		dst.move(0, Max(SIMD::Float((packed[0] << 24) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(1, Max(SIMD::Float((packed[0] << 16) & SIMD::Int(0xFF000000)) * SIMD::Float(1.0f / 0x7F000000), SIMD::Float(-1.0f)));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R8G8_UINT:
		dst.move(0, As<SIMD::UInt>(packed[0]) & SIMD::UInt(0xFF));
		dst.move(1, (As<SIMD::UInt>(packed[0]) >> 8) & SIMD::UInt(0xFF));
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
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16_UNORM:
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16_SNORM:
		dst.move(0, Max(SIMD::Float((packed[0] << 16) & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(1, SIMD::Float(0.0f));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16_UINT:
		dst.move(0, packed[0] & SIMD::Int(0xFFFF));
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
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16G16_UNORM:
		dst.move(0, SIMD::Float(packed[0] & SIMD::Int(0xFFFF)) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(1, SIMD::Float(As<SIMD::UInt>(packed[0]) >> 16) * SIMD::Float(1.0f / 0xFFFF));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16G16_SNORM:
		dst.move(0, Max(SIMD::Float((packed[0] << 16) & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(1, Max(SIMD::Float(packed[0] & SIMD::Int(0xFFFF0000)) * SIMD::Float(1.0f / 0x7FFF0000), SIMD::Float(-1.0f)));
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R16G16_UINT:
		dst.move(0, packed[0] & SIMD::Int(0xFFFF));
		dst.move(1, (packed[0] >> 16) & SIMD::Int(0xFFFF));
		dst.move(2, SIMD::UInt(0));
		dst.move(3, SIMD::UInt(1));
		break;
	case VK_FORMAT_R16G16_SINT:
		dst.move(0, (packed[0] << 16) >> 16);
		dst.move(1, packed[0] >> 16);
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
		dst.move(2, SIMD::Float(0.0f));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		dst.move(0, packed[0] & SIMD::Int(0x3FF));
		dst.move(1, (packed[0] >> 10) & SIMD::Int(0x3FF));
		dst.move(2, (packed[0] >> 20) & SIMD::Int(0x3FF));
		dst.move(3, (packed[0] >> 30) & SIMD::Int(0x3));
		break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		dst.move(2, packed[0] & SIMD::Int(0x3FF));
		dst.move(1, (packed[0] >> 10) & SIMD::Int(0x3FF));
		dst.move(0, (packed[0] >> 20) & SIMD::Int(0x3FF));
		dst.move(3, (packed[0] >> 30) & SIMD::Int(0x3));
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		dst.move(0, SIMD::Float((packed[0]) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(1, SIMD::Float((packed[0] >> 10) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(2, SIMD::Float((packed[0] >> 20) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(3, SIMD::Float((packed[0] >> 30) & SIMD::Int(0x3)) * SIMD::Float(1.0f / 0x3));
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(1, SIMD::Float((packed[0] >> 10) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(0, SIMD::Float((packed[0] >> 20) & SIMD::Int(0x3FF)) * SIMD::Float(1.0f / 0x3FF));
		dst.move(3, SIMD::Float((packed[0] >> 30) & SIMD::Int(0x3)) * SIMD::Float(1.0f / 0x3));
		break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 12) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(1, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(2, SIMD::Float((packed[0] >> 4) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(3, SIMD::Float((packed[0]) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 4) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(1, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(2, SIMD::Float((packed[0] >> 12) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(3, SIMD::Float((packed[0]) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		break;
	case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
		dst.move(0, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(1, SIMD::Float((packed[0] >> 4) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(3, SIMD::Float((packed[0] >> 12) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		break;
	case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
		dst.move(0, SIMD::Float((packed[0]) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(1, SIMD::Float((packed[0] >> 4) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(2, SIMD::Float((packed[0] >> 8) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		dst.move(3, SIMD::Float((packed[0] >> 12) & SIMD::Int(0xF)) * SIMD::Float(1.0f / 0xF));
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 11) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(1, SIMD::Float((packed[0] >> 5) & SIMD::Int(0x3F)) * SIMD::Float(1.0f / 0x3F));
		dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0]) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(1, SIMD::Float((packed[0] >> 5) & SIMD::Int(0x3F)) * SIMD::Float(1.0f / 0x3F));
		dst.move(2, SIMD::Float((packed[0] >> 11) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(3, SIMD::Float(1.0f));
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 11) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(1, SIMD::Float((packed[0] >> 6) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(2, SIMD::Float((packed[0] >> 1) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(3, SIMD::Float((packed[0]) & SIMD::Int(0x1)));
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 1) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(1, SIMD::Float((packed[0] >> 6) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(2, SIMD::Float((packed[0] >> 11) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(3, SIMD::Float((packed[0]) & SIMD::Int(0x1)));
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		dst.move(0, SIMD::Float((packed[0] >> 10) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(1, SIMD::Float((packed[0] >> 5) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(2, SIMD::Float((packed[0]) & SIMD::Int(0x1F)) * SIMD::Float(1.0f / 0x1F));
		dst.move(3, SIMD::Float((packed[0] >> 15) & SIMD::Int(0x1)));
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		dst.move(0, halfToFloatBits((packed[0] << 4) & SIMD::Int(0x7FF0)));
		dst.move(1, halfToFloatBits((packed[0] >> 7) & SIMD::Int(0x7FF0)));
		dst.move(2, halfToFloatBits((packed[0] >> 17) & SIMD::Int(0x7FE0)));
		dst.move(3, SIMD::Float(1.0f));
		break;
	default:
		UNSUPPORTED("VkFormat %d", int(imageFormat));
		break;
	}

	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitImageWrite(const ImageInstruction &instruction, EmitState *state) const
{
	imageWriteEmitted = true;

	auto &image = getObject(instruction.imageId);
	auto &imageType = getType(image);

	ASSERT(imageType.definition.opcode() == spv::OpTypeImage);
	ASSERT(static_cast<spv::Dim>(instruction.dim) != spv::DimSubpassData);  // "Its Dim operand must not be SubpassData."

	auto coordinate = Operand(this, state, instruction.coordinateId);
	auto texel = Operand(this, state, instruction.texelId);

	Array<SIMD::Int> coord(5);  // uvwa & sample

	uint32_t i = 0;
	for(; i < instruction.coordinates; i++)
	{
		coord[i] = coordinate.Int(i);
	}

	if(instruction.sample)
	{
		coord[i] = Operand(this, state, instruction.sampleId).Int(0);
	}

	Array<SIMD::Int> texelAndMask(5);
	texelAndMask[0] = texel.Int(0);
	texelAndMask[1] = texel.Int(1);
	texelAndMask[2] = texel.Int(2);
	texelAndMask[3] = texel.Int(3);
	texelAndMask[4] = state->activeStoresAndAtomicsMask();

	Pointer<Byte> descriptor = state->getPointer(instruction.imageId).base;  // vk::StorageImageDescriptor*

	vk::Format imageFormat = SpirvFormatToVulkanFormat(static_cast<spv::ImageFormat>(instruction.imageFormat));

	if(imageFormat == VK_FORMAT_UNDEFINED)  // spv::ImageFormatUnknown
	{
		Pointer<Byte> samplerFunction = lookupSamplerFunction(descriptor, instruction, state);

		Call<ImageSampler>(samplerFunction, descriptor, &coord, &texelAndMask, state->routine->constants);
	}
	else
	{
		WriteImage(instruction, descriptor, &coord, &texelAndMask, imageFormat);
	}

	return EmitResult::Continue;
}

void SpirvShader::WriteImage(ImageInstructionSignature instruction, Pointer<Byte> descriptor, const Pointer<SIMD::Int> &coord, const Pointer<SIMD::Int> &texelAndMask, vk::Format imageFormat)
{
	SIMD::Int texel[4];
	texel[0] = texelAndMask[0];
	texel[1] = texelAndMask[1];
	texel[2] = texelAndMask[2];
	texel[3] = texelAndMask[3];
	SIMD::Int mask = texelAndMask[4];

	SIMD::Int packed[4];
	switch(imageFormat)
	{
	case VK_FORMAT_R32G32B32A32_SFLOAT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_UINT:
		packed[0] = texel[0];
		packed[1] = texel[1];
		packed[2] = texel[2];
		packed[3] = texel[3];
		break;
	case VK_FORMAT_R32_SFLOAT:
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_UINT:
		packed[0] = texel[0];
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		packed[0] = (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 8) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 16) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 24);
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		packed[0] = (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 8) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 16) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(255.0f)))) << 24);
		break;
	case VK_FORMAT_R8G8B8A8_SNORM:
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		packed[0] = (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
		             SIMD::Int(0xFF)) |
		            ((SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
		              SIMD::Int(0xFF))
		             << 8) |
		            ((SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
		              SIMD::Int(0xFF))
		             << 16) |
		            ((SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(127.0f))) &
		              SIMD::Int(0xFF))
		             << 24);
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		packed[0] = (SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xff))) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[1]) & SIMD::UInt(0xff)) << 8) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[2]) & SIMD::UInt(0xff)) << 16) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[3]) & SIMD::UInt(0xff)) << 24);
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		packed[0] = floatToHalfBits(As<SIMD::UInt>(texel[0]), false) | floatToHalfBits(As<SIMD::UInt>(texel[1]), true);
		packed[1] = floatToHalfBits(As<SIMD::UInt>(texel[2]), false) | floatToHalfBits(As<SIMD::UInt>(texel[3]), true);
		break;
	case VK_FORMAT_R16G16B16A16_SINT:
	case VK_FORMAT_R16G16B16A16_UINT:
		packed[0] = SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xFFFF)) | (SIMD::UInt(As<SIMD::UInt>(texel[1]) & SIMD::UInt(0xFFFF)) << 16);
		packed[1] = SIMD::UInt(As<SIMD::UInt>(texel[2]) & SIMD::UInt(0xFFFF)) | (SIMD::UInt(As<SIMD::UInt>(texel[3]) & SIMD::UInt(0xFFFF)) << 16);
		break;
	case VK_FORMAT_R32G32_SFLOAT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_UINT:
		packed[0] = texel[0];
		packed[1] = texel[1];
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		packed[0] = floatToHalfBits(As<SIMD::UInt>(texel[0]), false) | floatToHalfBits(As<SIMD::UInt>(texel[1]), true);
		break;
	case VK_FORMAT_R16G16_SINT:
	case VK_FORMAT_R16G16_UINT:
		packed[0] = SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xFFFF)) | (SIMD::UInt(As<SIMD::UInt>(texel[1]) & SIMD::UInt(0xFFFF)) << 16);
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		// Truncates instead of rounding. See b/147900455
		packed[0] = ((floatToHalfBits(As<SIMD::UInt>(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f))), false) & SIMD::UInt(0x7FF0)) >> 4) |
		            ((floatToHalfBits(As<SIMD::UInt>(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f))), false) & SIMD::UInt(0x7FF0)) << 7) |
		            ((floatToHalfBits(As<SIMD::UInt>(Max(As<SIMD::Float>(texel[2]), SIMD::Float(0.0f))), false) & SIMD::UInt(0x7FE0)) << 17);
		break;
	case VK_FORMAT_R16_SFLOAT:
		packed[0] = floatToHalfBits(As<SIMD::UInt>(texel[0]), false);
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		packed[0] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) |
		            (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) << 16);
		packed[1] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) |
		            (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) << 16);
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		packed[0] = (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x3FF)))) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x3FF)))) << 10) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x3FF)))) << 20) |
		            ((SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x3)))) << 30);
		break;
	case VK_FORMAT_R16G16_UNORM:
		packed[0] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) |
		            (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF))) << 16);
		break;
	case VK_FORMAT_R8G8_UNORM:
		packed[0] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFF))) |
		            (SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFF))) << 8);
		break;
	case VK_FORMAT_R16_UNORM:
		packed[0] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFFFF)));
		break;
	case VK_FORMAT_R8_UNORM:
		packed[0] = SIMD::UInt(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(0.0f)), SIMD::Float(1.0f)) * SIMD::Float(0xFF)));
		break;
	case VK_FORMAT_R16G16B16A16_SNORM:
		packed[0] = (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) & SIMD::Int(0xFFFF)) |
		            (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) << 16);
		packed[1] = (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[2]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) & SIMD::Int(0xFFFF)) |
		            (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[3]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) << 16);
		break;
	case VK_FORMAT_R16G16_SNORM:
		packed[0] = (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) & SIMD::Int(0xFFFF)) |
		            (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF))) << 16);
		break;
	case VK_FORMAT_R8G8_SNORM:
		packed[0] = (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7F))) & SIMD::Int(0xFF)) |
		            (SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[1]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7F))) << 8);
		break;
	case VK_FORMAT_R16_SNORM:
		packed[0] = SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7FFF)));
		break;
	case VK_FORMAT_R8_SNORM:
		packed[0] = SIMD::Int(Round(Min(Max(As<SIMD::Float>(texel[0]), SIMD::Float(-1.0f)), SIMD::Float(1.0f)) * SIMD::Float(0x7F)));
		break;
	case VK_FORMAT_R8G8_SINT:
	case VK_FORMAT_R8G8_UINT:
		packed[0] = SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xFF)) | (SIMD::UInt(As<SIMD::UInt>(texel[1]) & SIMD::UInt(0xFF)) << 8);
		break;
	case VK_FORMAT_R16_SINT:
	case VK_FORMAT_R16_UINT:
		packed[0] = SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xFFFF));
		break;
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UINT:
		packed[0] = SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0xFF));
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		packed[0] = (SIMD::UInt(As<SIMD::UInt>(texel[0]) & SIMD::UInt(0x3FF))) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[1]) & SIMD::UInt(0x3FF)) << 10) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[2]) & SIMD::UInt(0x3FF)) << 20) |
		            (SIMD::UInt(As<SIMD::UInt>(texel[3]) & SIMD::UInt(0x3)) << 30);
		break;
	default:
		UNSUPPORTED("VkFormat %d", int(imageFormat));
		break;
	}

	// "The integer texel coordinates are validated according to the same rules as for texel input coordinate
	//  validation. If the texel fails integer texel coordinate validation, then the write has no effect."
	// - https://www.khronos.org/registry/vulkan/specs/1.2/html/chap16.html#textures-output-coordinate-validation
	auto robustness = OutOfBoundsBehavior::Nullify;
	// GetTexelAddress() only needs the EmitState* for SubpassData accesses (i.e. input attachments).
	const EmitState *state = nullptr;

	SIMD::Int uvwa[4];
	SIMD::Int sample;

	uint32_t i = 0;
	for(; i < instruction.coordinates; i++)
	{
		uvwa[i] = As<SIMD::Int>(coord[i]);
	}

	if(instruction.sample)
	{
		sample = As<SIMD::Int>(coord[i]);
	}

	auto texelPtr = GetTexelAddress(instruction, descriptor, uvwa, sample, imageFormat, robustness, state);

	const int texelSize = imageFormat.bytes();

	// Scatter packed texel data.
	// TODO(b/160531165): Provide scatter abstractions for various element sizes.
	if(texelSize == 4 || texelSize == 8 || texelSize == 16)
	{
		for(auto i = 0; i < texelSize / 4; i++)
		{
			texelPtr.Store(packed[i], robustness, mask);
			texelPtr += sizeof(float);
		}
	}
	else if(texelSize == 2)
	{
		SIMD::Int offsets = texelPtr.offsets();
		mask = mask & texelPtr.isInBounds(2, robustness);

		for(int i = 0; i < SIMD::Width; i++)
		{
			If(Extract(mask, i) != 0)
			{
				*Pointer<Short>(texelPtr.base + Extract(offsets, i)) = Short(Extract(packed[0], i));
			}
		}
	}
	else if(texelSize == 1)
	{
		SIMD::Int offsets = texelPtr.offsets();
		mask = mask & texelPtr.isInBounds(1, robustness);

		for(int i = 0; i < SIMD::Width; i++)
		{
			If(Extract(mask, i) != 0)
			{
				*Pointer<Byte>(texelPtr.base + Extract(offsets, i)) = Byte(Extract(packed[0], i));
			}
		}
	}
	else
		UNREACHABLE("texelSize: %d", int(texelSize));
}

SpirvShader::EmitResult SpirvShader::EmitImageTexelPointer(const ImageInstruction &instruction, EmitState *state) const
{
	auto coordinate = Operand(this, state, instruction.coordinateId);

	Pointer<Byte> descriptor = state->getPointer(instruction.imageId).base;  // vk::StorageImageDescriptor*

	// VK_EXT_image_robustness requires checking for out-of-bounds accesses.
	// TODO(b/162327166): Only perform bounds checks when VK_EXT_image_robustness is enabled.
	auto robustness = OutOfBoundsBehavior::Nullify;
	vk::Format imageFormat = SpirvFormatToVulkanFormat(static_cast<spv::ImageFormat>(instruction.imageFormat));

	SIMD::Int uvwa[4];

	for(uint32_t i = 0; i < instruction.coordinates; i++)
	{
		uvwa[i] = coordinate.Int(i);
	}

	SIMD::Int sample = Operand(this, state, instruction.sampleId).Int(0);

	auto ptr = GetTexelAddress(instruction, descriptor, uvwa, sample, imageFormat, robustness, state);

	state->createPointer(instruction.resultId, ptr);

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

}  // namespace sw