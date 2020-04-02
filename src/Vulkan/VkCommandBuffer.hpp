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
#include "VkDescriptorSet.hpp"
#include "VkObject.hpp"
#include "Device/Context.hpp"

#include <memory>
#include <vector>

namespace sw {

class Context;
class Renderer;
class TaskEvents;

}  // namespace sw

namespace vk {

namespace dbg {
class File;
}  // namespace dbg

class Device;
class Buffer;
class Event;
class Framebuffer;
class Image;
class Pipeline;
class PipelineLayout;
class QueryPool;
class RenderPass;

class CommandBuffer
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_OBJECT; }

	CommandBuffer(Device *device, VkCommandBufferLevel pLevel);

	static inline CommandBuffer *Cast(VkCommandBuffer object)
	{
		return reinterpret_cast<CommandBuffer *>(object);
	}

	void destroy(const VkAllocationCallbacks *pAllocator);

	VkResult begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo *pInheritanceInfo);
	VkResult end();
	VkResult reset(VkCommandPoolResetFlags flags);

	void beginRenderPass(RenderPass *renderPass, Framebuffer *framebuffer, VkRect2D renderArea,
	                     uint32_t clearValueCount, const VkClearValue *pClearValues, VkSubpassContents contents);
	void nextSubpass(VkSubpassContents contents);
	void endRenderPass();
	void executeCommands(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers);

	void setDeviceMask(uint32_t deviceMask);
	void dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
	                  uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	void pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
	                     uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
	                     uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
	                     uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers);
	void bindPipeline(VkPipelineBindPoint pipelineBindPoint, Pipeline *pipeline);
	void bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
	                       const VkBuffer *pBuffers, const VkDeviceSize *pOffsets);

	void beginQuery(QueryPool *queryPool, uint32_t query, VkQueryControlFlags flags);
	void endQuery(QueryPool *queryPool, uint32_t query);
	void resetQueryPool(QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount);
	void writeTimestamp(VkPipelineStageFlagBits pipelineStage, QueryPool *queryPool, uint32_t query);
	void copyQueryPoolResults(const QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount,
	                          Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags);
	void pushConstants(PipelineLayout *layout, VkShaderStageFlags stageFlags,
	                   uint32_t offset, uint32_t size, const void *pValues);

	void setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports);
	void setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors);
	void setLineWidth(float lineWidth);
	void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
	void setBlendConstants(const float blendConstants[4]);
	void setDepthBounds(float minDepthBounds, float maxDepthBounds);
	void setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask);
	void setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask);
	void setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference);
	void bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, const PipelineLayout *layout,
	                        uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets,
	                        uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets);
	void bindIndexBuffer(Buffer *buffer, VkDeviceSize offset, VkIndexType indexType);
	void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
	void dispatchIndirect(Buffer *buffer, VkDeviceSize offset);
	void copyBuffer(const Buffer *srcBuffer, Buffer *dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions);
	void copyImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
	               uint32_t regionCount, const VkImageCopy *pRegions);
	void blitImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
	               uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter);
	void copyBufferToImage(Buffer *srcBuffer, Image *dstImage, VkImageLayout dstImageLayout,
	                       uint32_t regionCount, const VkBufferImageCopy *pRegions);
	void copyImageToBuffer(Image *srcImage, VkImageLayout srcImageLayout, Buffer *dstBuffer,
	                       uint32_t regionCount, const VkBufferImageCopy *pRegions);
	void updateBuffer(Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void *pData);
	void fillBuffer(Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data);
	void clearColorImage(Image *image, VkImageLayout imageLayout, const VkClearColorValue *pColor,
	                     uint32_t rangeCount, const VkImageSubresourceRange *pRanges);
	void clearDepthStencilImage(Image *image, VkImageLayout imageLayout, const VkClearDepthStencilValue *pDepthStencil,
	                            uint32_t rangeCount, const VkImageSubresourceRange *pRanges);
	void clearAttachments(uint32_t attachmentCount, const VkClearAttachment *pAttachments,
	                      uint32_t rectCount, const VkClearRect *pRects);
	void resolveImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
	                  uint32_t regionCount, const VkImageResolve *pRegions);
	void setEvent(Event *event, VkPipelineStageFlags stageMask);
	void resetEvent(Event *event, VkPipelineStageFlags stageMask);
	void waitEvents(uint32_t eventCount, const VkEvent *pEvents, VkPipelineStageFlags srcStageMask,
	                VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
	                uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
	                uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers);

	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void drawIndirect(Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
	void drawIndexedIndirect(Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);

	// TODO(sugoi): Move ExecutionState out of CommandBuffer (possibly into Device)
	struct ExecutionState
	{
		struct PipelineState
		{
			Pipeline *pipeline = nullptr;
			vk::DescriptorSet::Bindings descriptorSets = {};
			vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets = {};
		};

		sw::Renderer *renderer = nullptr;
		sw::TaskEvents *events = nullptr;
		RenderPass *renderPass = nullptr;
		Framebuffer *renderPassFramebuffer = nullptr;
		std::array<PipelineState, VK_PIPELINE_BIND_POINT_RANGE_SIZE> pipelineState;

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

			uint32_t compareMask[2] = { 0 };
			uint32_t writeMask[2] = { 0 };
			uint32_t reference[2] = { 0 };
		};
		DynamicState dynamicState;

		sw::PushConstantStorage pushConstants;

		struct VertexInputBinding
		{
			Buffer *buffer;
			VkDeviceSize offset;
		};
		VertexInputBinding vertexInputBindings[MAX_VERTEX_INPUT_BINDINGS] = {};
		VertexInputBinding indexBufferBinding;
		VkIndexType indexType;

		uint32_t subpassIndex = 0;

		void bindAttachments(sw::Context &context);
		void bindVertexInputs(sw::Context &context, int firstInstance);
	};

	void submit(CommandBuffer::ExecutionState &executionState);
	void submitSecondary(CommandBuffer::ExecutionState &executionState) const;

	class Command;

private:
	void resetState();
	template<typename T, typename... Args>
	void addCommand(Args &&... args);

	enum State
	{
		INITIAL,
		RECORDING,
		EXECUTABLE,
		PENDING,
		INVALID
	};

	Device *const device;
	State state = INITIAL;
	VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	std::vector<std::unique_ptr<Command>> *commands;

#ifdef ENABLE_VK_DEBUGGER
	std::shared_ptr<vk::dbg::File> debuggerFile;
#endif  // ENABLE_VK_DEBUGGER
};

using DispatchableCommandBuffer = DispatchableObject<CommandBuffer, VkCommandBuffer>;

static inline CommandBuffer *Cast(VkCommandBuffer object)
{
	return DispatchableCommandBuffer::Cast(object);
}

}  // namespace vk

#endif  // VK_COMMAND_BUFFER_HPP_
