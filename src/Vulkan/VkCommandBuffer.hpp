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

#ifndef VK_COMMAND_BUFFER_HPP_
#define VK_COMMAND_BUFFER_HPP_

#include "VkConfig.h"
#include "VkObject.hpp"
#include "Device/Context.hpp"
#include <memory>
#include <vector>

namespace sw
{
	class Context;
	class Renderer;
}

namespace vk
{

class Framebuffer;
class Pipeline;
class RenderPass;

class CommandBuffer
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_OBJECT; }

	CommandBuffer(VkCommandBufferLevel pLevel);

	void destroy(const VkAllocationCallbacks* pAllocator);

	VkResult begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* pInheritanceInfo);
	VkResult end();
	VkResult reset(VkCommandPoolResetFlags flags);

	void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
	                     uint32_t clearValueCount, const VkClearValue* pClearValues, VkSubpassContents contents);
	void nextSubpass(VkSubpassContents contents);
	void endRenderPass();
	void executeCommands(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers);

	void setDeviceMask(uint32_t deviceMask);
	void dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
	                  uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	void pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
	                     uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
	                     uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	                     uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers);
	void bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);
	void bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
	                       const VkBuffer* pBuffers, const VkDeviceSize* pOffsets);

	void beginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags);
	void endQuery(VkQueryPool queryPool, uint32_t query);
	void resetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount);
	void writeTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query);
	void copyQueryPoolResults(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount,
	                          VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags);
	void pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags,
	                   uint32_t offset, uint32_t size, const void* pValues);

	void setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports);
	void setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors);
	void setLineWidth(float lineWidth);
	void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
	void setBlendConstants(const float blendConstants[4]);
	void setDepthBounds(float minDepthBounds, float maxDepthBounds);
	void setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask);
	void setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask);
	void setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference);
	void bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
		uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets,
		uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets);
	void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
	void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
	void dispatchIndirect(VkBuffer buffer, VkDeviceSize offset);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions);
	void copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
		uint32_t regionCount, const VkImageCopy* pRegions);
	void blitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
		uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter);
	void copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout,
		uint32_t regionCount, const VkBufferImageCopy* pRegions);
	void copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer,
		uint32_t regionCount, const VkBufferImageCopy* pRegions);
	void updateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData);
	void fillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data);
	void clearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor,
		uint32_t rangeCount, const VkImageSubresourceRange* pRanges);
	void clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil,
		uint32_t rangeCount, const VkImageSubresourceRange* pRanges);
	void clearAttachments(uint32_t attachmentCount, const VkClearAttachment* pAttachments,
		uint32_t rectCount, const VkClearRect* pRects);
	void resolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
		uint32_t regionCount, const VkImageResolve* pRegions);
	void setEvent(VkEvent event, VkPipelineStageFlags stageMask);
	void resetEvent(VkEvent event, VkPipelineStageFlags stageMask);
	void waitEvents(uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
		uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
		uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers);

	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void drawIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
	void drawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);

	// TODO(sugoi): Move ExecutionState out of CommandBuffer (possibly into Device)
	struct ExecutionState
	{
		sw::Renderer* renderer = nullptr;
		RenderPass* renderPass = nullptr;
		Framebuffer* renderPassFramebuffer = nullptr;
		Pipeline* pipelines[VK_PIPELINE_BIND_POINT_RANGE_SIZE] = {};
		VkDescriptorSet boundDescriptorSets[VK_PIPELINE_BIND_POINT_RANGE_SIZE][MAX_BOUND_DESCRIPTOR_SETS] = { { VK_NULL_HANDLE } };
		sw::PushConstantStorage pushConstants;

		struct VertexInputBinding
		{
			VkBuffer buffer;
			VkDeviceSize offset;
		};
		VertexInputBinding vertexInputBindings[MAX_VERTEX_INPUT_BINDINGS] = {};
		VertexInputBinding indexBufferBinding;
		VkIndexType indexType;

		void bindAttachments();
		void bindVertexInputs(sw::Context& context, int firstVertex, int firstInstance);
	};

	void submit(CommandBuffer::ExecutionState& executionState);
	void submitSecondary(CommandBuffer::ExecutionState& executionState) const;

	class Command;
private:
	void resetState();
	template<typename T, typename... Args> void addCommand(Args&&... args);

	enum State { INITIAL, RECORDING, EXECUTABLE, PENDING, INVALID };
	State state = INITIAL;
	VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	std::vector<std::unique_ptr<Command>>* commands;
};

using DispatchableCommandBuffer = DispatchableObject<CommandBuffer, VkCommandBuffer>;

static inline CommandBuffer* Cast(VkCommandBuffer object)
{
	return DispatchableCommandBuffer::Cast(object);
}

} // namespace vk

#endif // VK_COMMAND_BUFFER_HPP_
