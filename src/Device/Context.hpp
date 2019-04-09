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
	class Sampler;
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
		bool setSourceBlendFactor(VkBlendFactor sourceBlendFactor);
		bool setDestBlendFactor(VkBlendFactor destBlendFactor);
		bool setBlendOperation(VkBlendOp blendOperation);

		bool setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable);
		bool setSourceBlendFactorAlpha(VkBlendFactor sourceBlendFactorAlpha);
		bool setDestBlendFactorAlpha(VkBlendFactor destBlendFactorAlpha);
		bool setBlendOperationAlpha(VkBlendOp blendOperationAlpha);

		bool setColorWriteMask(int index, int colorWriteMask);
		bool setWriteSRGB(bool sRGB);

		bool depthWriteActive();
		bool alphaTestActive();
		bool depthBufferActive();
		bool stencilActive();

		bool perspectiveActive();

		bool alphaBlendActive();
		VkBlendFactor sourceBlendFactor();
		VkBlendFactor destBlendFactor();
		VkBlendOp blendOperation();

		VkBlendFactor sourceBlendFactorAlpha();
		VkBlendFactor destBlendFactorAlpha();
		VkBlendOp blendOperationAlpha();

		VkLogicOp colorLogicOp();

		VkPrimitiveTopology topology;

		bool stencilEnable;
		bool twoSidedStencil;
		VkStencilOpState frontStencil;
		VkStencilOpState backStencil;

		// Pixel processor states
		VkCullModeFlags cullMode;
		bool frontFacingCCW;
		float alphaReference;

		float depthBias;
		float slopeDepthBias;

		VkFormat renderTargetInternalFormat(int index);
		bool colorWriteActive();
		int colorWriteActive(int index);
		bool colorUsed();

		vk::DescriptorSet::Bindings descriptorSets = {};
		vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
		Stream input[MAX_VERTEX_INPUTS];
		void *indexBuffer;

		vk::ImageView *renderTarget[RENDERTARGETS];
		unsigned int renderTargetLayer[RENDERTARGETS];
		vk::ImageView *depthBuffer;
		unsigned int depthBufferLayer;
		vk::ImageView *stencilBuffer;
		unsigned int stencilBufferLayer;

		vk::PipelineLayout const *pipelineLayout;

		// Shaders
		const SpirvShader *pixelShader;
		const SpirvShader *vertexShader;

		// Instancing
		int instanceID;

		bool occlusionEnabled;

		// Pixel processor states
		bool rasterizerDiscard;
		bool depthBufferEnable;
		VkCompareOp depthCompareMode;
		bool depthWriteEnable;

		bool alphaBlendEnable;
		VkBlendFactor sourceBlendFactorState;
		VkBlendFactor destBlendFactorState;
		VkBlendOp blendOperationState;

		bool separateAlphaBlendEnable;
		VkBlendFactor sourceBlendFactorStateAlpha;
		VkBlendFactor destBlendFactorStateAlpha;
		VkBlendOp blendOperationStateAlpha;

		float lineWidth;

		int colorWriteMask[RENDERTARGETS];   // RGBA
		bool writeSRGB;
		unsigned int sampleMask;
		unsigned int multiSampleMask;
		int sampleCount;

		PushConstantStorage pushConstants;
	};
}

#endif   // sw_Context_hpp
