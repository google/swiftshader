// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#ifndef vk_Context_hpp
#define vk_Context_hpp

#include "Config.hpp"
#include "Memset.hpp"
#include "Stream.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkDescriptorSet.hpp"
#include "Vulkan/VkFormat.hpp"

#include <vector>

namespace vk {

class Buffer;
class Device;
class ImageView;
class PipelineLayout;

struct VertexInputBinding
{
	Buffer *buffer = nullptr;
	VkDeviceSize offset = 0;
	VkDeviceSize size = 0;
	VkDeviceSize stride = 0;
};

struct IndexBuffer
{
	inline VkIndexType getIndexType() const { return indexType; }
	void setIndexBufferBinding(const VertexInputBinding &indexBufferBinding, VkIndexType type);
	void getIndexBuffers(VkPrimitiveTopology topology, uint32_t count, uint32_t first, bool indexed, bool hasPrimitiveRestartEnable, std::vector<std::pair<uint32_t, void *>> *indexBuffers) const;

private:
	int bytesPerIndex() const;

	VertexInputBinding binding;
	VkIndexType indexType;
};

struct Attachments
{
	ImageView *colorBuffer[sw::MAX_COLOR_BUFFERS] = {};
	ImageView *depthBuffer = nullptr;
	ImageView *stencilBuffer = nullptr;

	VkFormat colorFormat(int index) const;
	VkFormat depthFormat() const;
};

struct Inputs
{
	Inputs(const VkPipelineVertexInputStateCreateInfo *vertexInputState);

	void updateDescriptorSets(const DescriptorSet::Array &dso,
	                          const DescriptorSet::Bindings &ds,
	                          const DescriptorSet::DynamicOffsets &ddo);
	inline const DescriptorSet::Array &getDescriptorSetObjects() const { return descriptorSetObjects; }
	inline const DescriptorSet::Bindings &getDescriptorSets() const { return descriptorSets; }
	inline const DescriptorSet::DynamicOffsets &getDescriptorDynamicOffsets() const { return descriptorDynamicOffsets; }
	inline const sw::Stream &getStream(uint32_t i) const { return stream[i]; }

	void bindVertexInputs(int firstInstance);
	void setVertexInputBinding(const VertexInputBinding vertexInputBindings[]);
	void advanceInstanceAttributes();
	VkDeviceSize getVertexStride(uint32_t i, bool dynamicVertexStride) const;

private:
	VertexInputBinding vertexInputBindings[MAX_VERTEX_INPUT_BINDINGS] = {};
	DescriptorSet::Array descriptorSetObjects = {};
	DescriptorSet::Bindings descriptorSets = {};
	DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
	sw::Stream stream[sw::MAX_INTERFACE_COMPONENTS / 4];
};

struct BlendState : sw::Memset<BlendState>
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

struct DynamicState
{
	VkViewport viewport;
	VkRect2D scissor;
	sw::float4 blendConstants;
	float depthBiasConstantFactor = 0.0f;
	float depthBiasClamp = 0.0f;
	float depthBiasSlopeFactor = 0.0f;
	float minDepthBounds = 0.0f;
	float maxDepthBounds = 0.0f;

	VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
	VkBool32 depthBoundsTestEnable = VK_FALSE;
	VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER;
	VkBool32 depthTestEnable = VK_FALSE;
	VkBool32 depthWriteEnable = VK_FALSE;
	VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	uint32_t scissorCount = 0;
	VkRect2D scissors[vk::MAX_VIEWPORTS] = {};
	VkStencilFaceFlags faceMask = (VkStencilFaceFlags)0;
	VkStencilOpState frontStencil = {};
	VkStencilOpState backStencil = {};
	VkBool32 stencilTestEnable = VK_FALSE;
	uint32_t viewportCount = 0;
	VkRect2D viewports[vk::MAX_VIEWPORTS] = {};
	VkBool32 rasterizerDiscardEnable = VK_FALSE;
	VkBool32 depthBiasEnable = VK_FALSE;
	VkBool32 primitiveRestartEnable = VK_FALSE;
};

struct GraphicsState
{
	GraphicsState(const Device *device, const VkGraphicsPipelineCreateInfo *pCreateInfo, const PipelineLayout *layout, bool robustBufferAccess);

	const GraphicsState combineStates(const DynamicState &dynamicState) const;

	inline const PipelineLayout *getPipelineLayout() const { return pipelineLayout; }
	inline bool getRobustBufferAccess() const { return robustBufferAccess; }
	inline VkPrimitiveTopology getTopology() const { return topology; }

	inline VkProvokingVertexModeEXT getProvokingVertexMode() const { return provokingVertexMode; }

	inline VkStencilOpState getFrontStencil() const { return frontStencil; }
	inline VkStencilOpState getBackStencil() const { return backStencil; }

	// Pixel processor states
	inline VkCullModeFlags getCullMode() const { return cullMode; }
	inline VkFrontFace getFrontFace() const { return frontFace; }
	inline VkPolygonMode getPolygonMode() const { return polygonMode; }
	inline VkLineRasterizationModeEXT getLineRasterizationMode() const { return lineRasterizationMode; }

	inline float getConstantDepthBias() const { return depthBiasEnable ? constantDepthBias : 0; }
	inline float getSlopeDepthBias() const { return depthBiasEnable ? slopeDepthBias : 0; }
	inline float getDepthBiasClamp() const { return depthBiasEnable ? depthBiasClamp : 0; }
	inline float getMinDepthBounds() const { return minDepthBounds; }
	inline float getMaxDepthBounds() const { return maxDepthBounds; }
	inline bool hasDepthRangeUnrestricted() const { return depthRangeUnrestricted; }
	inline bool getDepthClampEnable() const { return depthClampEnable; }
	inline bool getDepthClipEnable() const { return depthClipEnable; }

	// Pixel processor states
	inline bool hasRasterizerDiscard() const { return rasterizerDiscard; }
	inline VkCompareOp getDepthCompareMode() const { return depthCompareMode; }

	inline float getLineWidth() const { return lineWidth; }

	inline unsigned int getMultiSampleMask() const { return multiSampleMask; }
	inline int getSampleCount() const { return sampleCount; }
	inline bool hasSampleShadingEnabled() const { return sampleShadingEnable; }
	inline float getMinSampleShading() const { return minSampleShading; }
	inline bool hasAlphaToCoverage() const { return alphaToCoverage; }

	inline bool hasPrimitiveRestartEnable() const { return primitiveRestartEnable; }
	inline const VkRect2D &getScissor() const { return scissor; }
	inline const VkViewport &getViewport() const { return viewport; }
	inline const sw::float4 &getBlendConstants() const { return blendConstants; }

	bool isDrawPoint(bool polygonModeAware) const;
	bool isDrawLine(bool polygonModeAware) const;
	bool isDrawTriangle(bool polygonModeAware) const;

	BlendState getBlendState(int index, const Attachments &attachments, bool fragmentContainsKill) const;

	int colorWriteActive(int index, const Attachments &attachments) const;
	bool depthWriteActive(const Attachments &attachments) const;
	bool depthTestActive(const Attachments &attachments) const;
	bool stencilActive(const Attachments &attachments) const;
	bool depthBoundsTestActive(const Attachments &attachments) const;

	inline bool hasDynamicVertexStride() const { return dynamicStateFlags.dynamicVertexInputBindingStride; }
	inline bool hasDynamicTopology() const { return dynamicStateFlags.dynamicPrimitiveTopology; }

private:
	struct DynamicStateFlags
	{
		bool dynamicViewport : 1;
		bool dynamicScissor : 1;
		bool dynamicLineWidth : 1;
		bool dynamicDepthBias : 1;
		bool dynamicBlendConstants : 1;
		bool dynamicDepthBounds : 1;
		bool dynamicStencilCompareMask : 1;
		bool dynamicStencilWriteMask : 1;
		bool dynamicStencilReference : 1;
		bool dynamicCullMode : 1;
		bool dynamicFrontFace : 1;
		bool dynamicPrimitiveTopology : 1;
		bool dynamicViewportWithCount : 1;
		bool dynamicScissorWithCount : 1;
		bool dynamicVertexInputBindingStride : 1;
		bool dynamicDepthTestEnable : 1;
		bool dynamicDepthWriteEnable : 1;
		bool dynamicDepthCompareOp : 1;
		bool dynamicDepthBoundsTestEnable : 1;
		bool dynamicStencilTestEnable : 1;
		bool dynamicStencilOp : 1;
		bool dynamicRasterizerDiscardEnable : 1;
		bool dynamicDepthBiasEnable : 1;
		bool dynamicPrimitiveRestartEnable : 1;
	};

	static DynamicStateFlags ParseDynamicStateFlags(const VkPipelineDynamicStateCreateInfo *dynamicStateCreateInfo);

	VkBlendFactor blendFactor(VkBlendOp blendOperation, VkBlendFactor blendFactor) const;
	VkBlendOp blendOperation(VkBlendOp blendOperation, VkBlendFactor sourceBlendFactor, VkBlendFactor destBlendFactor, vk::Format format) const;

	bool alphaBlendActive(int index, const Attachments &attachments, bool fragmentContainsKill) const;
	bool colorWriteActive(const Attachments &attachments) const;

	const PipelineLayout *pipelineLayout = nullptr;
	const bool robustBufferAccess = false;
	const DynamicStateFlags dynamicStateFlags = {};
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

	VkProvokingVertexModeEXT provokingVertexMode = VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT;

	bool stencilEnable = false;
	VkStencilOpState frontStencil = {};
	VkStencilOpState backStencil = {};

	// Pixel processor states
	VkCullModeFlags cullMode = 0;
	VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
	VkLineRasterizationModeEXT lineRasterizationMode = VK_LINE_RASTERIZATION_MODE_DEFAULT_EXT;

	bool depthBiasEnable = false;
	float constantDepthBias = 0.0f;
	float slopeDepthBias = 0.0f;
	float depthBiasClamp = 0.0f;
	float minDepthBounds = 0.0f;
	float maxDepthBounds = 0.0f;
	bool depthRangeUnrestricted = false;

	// Pixel processor states
	bool rasterizerDiscard = false;
	bool depthBoundsTestEnable = false;
	bool depthTestEnable = false;
	VkCompareOp depthCompareMode = VK_COMPARE_OP_NEVER;
	bool depthWriteEnable = false;
	bool depthClampEnable = false;
	bool depthClipEnable = false;

	float lineWidth = 0.0f;

	int colorWriteMask[sw::MAX_COLOR_BUFFERS] = {};  // RGBA
	unsigned int multiSampleMask = 0;
	int sampleCount = 0;
	bool alphaToCoverage = false;

	bool sampleShadingEnable = false;
	float minSampleShading = 0.0f;

	bool primitiveRestartEnable = false;
	VkRect2D scissor = {};
	VkViewport viewport = {};
	sw::float4 blendConstants = {};

	BlendState blendState[sw::MAX_COLOR_BUFFERS] = {};
};

}  // namespace vk

#endif  // vk_Context_hpp
