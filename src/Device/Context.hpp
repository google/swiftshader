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

#include "Config.hpp"
#include "Memset.hpp"
#include "Stream.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkConfig.h"
#include "Vulkan/VkDescriptorSet.hpp"

namespace vk {

class ImageView;
class PipelineLayout;

}  // namespace vk

namespace sw {

class SpirvShader;

struct PushConstantStorage
{
	unsigned char data[vk::MAX_PUSH_CONSTANT_SIZE];
};

struct BlendState : Memset<BlendState>
{
	BlendState()
	    : Memset(this, 0)
	{}

	BlendState(bool alphaBlendEnable,
	           VkBlendFactor sourceBlendFactor,
	           VkBlendFactor destBlendFactor,
	           VkBlendOp blendOperation,
	           VkBlendFactor sourceBlendFactorAlpha,
	           VkBlendFactor destBlendFactorAlpha,
	           VkBlendOp blendOperationAlpha)
	    : Memset(this, 0)
	    , alphaBlendEnable(alphaBlendEnable)
	    , sourceBlendFactor(sourceBlendFactor)
	    , destBlendFactor(destBlendFactor)
	    , blendOperation(blendOperation)
	    , sourceBlendFactorAlpha(sourceBlendFactorAlpha)
	    , destBlendFactorAlpha(destBlendFactorAlpha)
	    , blendOperationAlpha(blendOperationAlpha)
	{}

	bool alphaBlendEnable;
	VkBlendFactor sourceBlendFactor;
	VkBlendFactor destBlendFactor;
	VkBlendOp blendOperation;
	VkBlendFactor sourceBlendFactorAlpha;
	VkBlendFactor destBlendFactorAlpha;
	VkBlendOp blendOperationAlpha;
};

class Context
{
public:
	Context();

	void init();

	bool isDrawPoint(bool polygonModeAware) const;
	bool isDrawLine(bool polygonModeAware) const;
	bool isDrawTriangle(bool polygonModeAware) const;

	bool depthWriteActive() const;
	bool depthBufferActive() const;
	bool stencilActive() const;

	bool allTargetsColorClamp() const;

	void setBlendState(int index, BlendState state);
	BlendState getBlendState(int index) const;

	VkPrimitiveTopology topology;
	VkProvokingVertexModeEXT provokingVertexMode;

	bool stencilEnable;
	VkStencilOpState frontStencil;
	VkStencilOpState backStencil;

	// Pixel processor states
	VkCullModeFlags cullMode;
	VkFrontFace frontFace;
	VkPolygonMode polygonMode;
	VkLineRasterizationModeEXT lineRasterizationMode;

	float depthBias;
	float slopeDepthBias;

	VkFormat renderTargetInternalFormat(int index) const;
	int colorWriteActive(int index) const;

	vk::DescriptorSet::Bindings descriptorSets = {};
	vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
	Stream input[MAX_INTERFACE_COMPONENTS / 4];
	bool robustBufferAccess;

	vk::ImageView *renderTarget[RENDERTARGETS];
	vk::ImageView *depthBuffer;
	vk::ImageView *stencilBuffer;

	vk::PipelineLayout const *pipelineLayout;

	// Shaders
	const SpirvShader *pixelShader;
	const SpirvShader *vertexShader;

	bool occlusionEnabled;

	// Pixel processor states
	bool rasterizerDiscard;
	bool depthBoundsTestEnable;
	bool depthBufferEnable;
	VkCompareOp depthCompareMode;
	bool depthWriteEnable;

	float lineWidth;

	int colorWriteMask[RENDERTARGETS];  // RGBA
	unsigned int sampleMask;
	unsigned int multiSampleMask;
	int sampleCount;
	bool alphaToCoverage;

private:
	bool colorWriteActive() const;
	bool colorUsed() const;

	bool alphaBlendActive(int index) const;
	VkBlendFactor sourceBlendFactor(int index) const;
	VkBlendFactor destBlendFactor(int index) const;
	VkBlendOp blendOperation(int index) const;

	VkBlendFactor sourceBlendFactorAlpha(int index) const;
	VkBlendFactor destBlendFactorAlpha(int index) const;
	VkBlendOp blendOperationAlpha(int index) const;

	BlendState blendState[RENDERTARGETS];
};

}  // namespace sw

#endif  // sw_Context_hpp
