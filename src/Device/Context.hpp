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
#include "Sampler.hpp"
#include "Stream.hpp"
#include "Point.hpp"
#include "Vertex.hpp"
#include "System/Types.hpp"

#include <Vulkan/VkConfig.h>

namespace vk
{
	class DescriptorSet;
	class ImageView;
	class PipelineLayout;
}

namespace sw
{
	struct Sampler;
	class PixelShader;
	class VertexShader;
	class SpirvShader;
	struct Triangle;
	struct Primitive;
	struct Vertex;
	class Resource;

	enum In   // Default input stream semantic
	{
		Position = 0,
		BlendWeight = 1,
		BlendIndices = 2,
		Normal = 3,
		PointSize = 4,
		Color0 = 5,
		Color1 = 6,
		TexCoord0 = 7,
		TexCoord1 = 8,
		TexCoord2 = 9,
		TexCoord3 = 10,
		TexCoord4 = 11,
		TexCoord5 = 12,
		TexCoord6 = 13,
		TexCoord7 = 14,
		PositionT = 15
	};

	enum CullMode ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		CULL_NONE,
		CULL_CLOCKWISE,
		CULL_COUNTERCLOCKWISE,

		CULL_LAST = CULL_COUNTERCLOCKWISE
	};

	enum TransparencyAntialiasing ENUM_UNDERLYING_TYPE_UNSIGNED_INT
	{
		TRANSPARENCY_NONE,
		TRANSPARENCY_ALPHA_TO_COVERAGE,

		TRANSPARENCY_LAST = TRANSPARENCY_ALPHA_TO_COVERAGE
	};

	struct PushConstantStorage
	{
		unsigned char data[vk::MAX_PUSH_CONSTANT_SIZE];
	};

	class Context
	{
	public:
		Context();

		~Context();

		void *operator new(size_t bytes);
		void operator delete(void *pointer, size_t bytes);

		void init();

		bool isDrawPoint() const;
		bool isDrawLine() const;
		bool isDrawTriangle() const;

		bool setDepthBufferEnable(bool depthBufferEnable);

		bool setAlphaBlendEnable(bool alphaBlendEnable);
		bool setColorWriteMask(int index, int colorWriteMask);
		bool setWriteSRGB(bool sRGB);

		bool depthWriteActive() const;
		bool alphaTestActive() const;
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
		bool twoSidedStencil;
		VkStencilOpState frontStencil;
		VkStencilOpState backStencil;

		// Pixel processor states
		VkCullModeFlags cullMode;
		bool frontFacingCCW;

		float depthBias;
		float slopeDepthBias;

		VkFormat renderTargetInternalFormat(int index) const;
		bool colorWriteActive() const;
		int colorWriteActive(int index) const;
		bool colorUsed() const;

		vk::DescriptorSet::Bindings descriptorSets = {};
		vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
		Stream input[MAX_VERTEX_INPUTS];
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
