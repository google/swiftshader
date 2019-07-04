// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef sw_Context_hpp
#define sw_Context_hpp

#include "Vulkan/VkConfig.h"
#include "Vulkan/VkDescriptorSet.hpp"
#include "Config.hpp"
#include "Stream.hpp"
#include "System/Types.hpp"

namespace vk
{
	class ImageView;
	class PipelineLayout;
}

namespace sw
{
	class SpirvShader;

	struct PushConstantStorage
	{
		unsigned char data[vk::MAX_PUSH_CONSTANT_SIZE];
	};

	class Context
	{
	public:
		Context();

		void init();

		bool isDrawPoint() const;
		bool isDrawLine() const;
		bool isDrawTriangle() const;

		bool depthWriteActive() const;
		bool depthBufferActive() const;
		bool stencilActive() const;

		bool allTargetsColorClamp() const;
		bool alphaBlendActive() const;
		VkBlendFactor sourceBlendFactor() const;
		VkBlendFactor destBlendFactor() const;
		VkBlendOp blendOperation() const;

		VkBlendFactor sourceBlendFactorAlpha() const;
		VkBlendFactor destBlendFactorAlpha() const;
		VkBlendOp blendOperationAlpha() const;

		VkPrimitiveTopology topology;

		bool stencilEnable;
		VkStencilOpState frontStencil;
		VkStencilOpState backStencil;

		// Pixel processor states
		VkCullModeFlags cullMode;
		VkFrontFace frontFace;

		float depthBias;
		float slopeDepthBias;

		VkFormat renderTargetInternalFormat(int index) const;
		bool colorWriteActive() const;
		int colorWriteActive(int index) const;
		bool colorUsed() const;

		vk::DescriptorSet::Bindings descriptorSets = {};
		vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
		Stream input[MAX_INTERFACE_COMPONENTS / 4];
		void *indexBuffer;

		vk::ImageView *renderTarget[RENDERTARGETS];
		vk::ImageView *depthBuffer;
		vk::ImageView *stencilBuffer;

		vk::PipelineLayout const *pipelineLayout;

		// Shaders
		const SpirvShader *pixelShader;
		const SpirvShader *vertexShader;

		// Instancing
		int instanceID;

		bool occlusionEnabled;

		// Pixel processor states
		bool rasterizerDiscard;
		bool depthBoundsTestEnable;
		bool depthBufferEnable;
		VkCompareOp depthCompareMode;
		bool depthWriteEnable;

		bool alphaBlendEnable;
		VkBlendFactor sourceBlendFactorState;
		VkBlendFactor destBlendFactorState;
		VkBlendOp blendOperationState;

		VkBlendFactor sourceBlendFactorStateAlpha;
		VkBlendFactor destBlendFactorStateAlpha;
		VkBlendOp blendOperationStateAlpha;

		float lineWidth;

		int colorWriteMask[RENDERTARGETS];   // RGBA
		unsigned int sampleMask;
		unsigned int multiSampleMask;
		int sampleCount;
		bool alphaToCoverage;

		PushConstantStorage pushConstants;
	};
}

#endif   // sw_Context_hpp
