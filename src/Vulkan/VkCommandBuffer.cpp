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

#include "VkCommandBuffer.hpp"

namespace vk
{

CommandBuffer::CommandBuffer(VkCommandBufferLevel pLevel) : level(pLevel)
{
	pipelines[VK_PIPELINE_BIND_POINT_GRAPHICS] = VK_NULL_HANDLE;
	pipelines[VK_PIPELINE_BIND_POINT_COMPUTE] = VK_NULL_HANDLE;
}

void CommandBuffer::destroy(const VkAllocationCallbacks* pAllocator)
{
}

VkResult CommandBuffer::begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
	state = RECORDING;

	UNIMPLEMENTED();

	return VK_SUCCESS;
}

VkResult CommandBuffer::end()
{
	state = EXECUTABLE;

	UNIMPLEMENTED();

	return VK_SUCCESS;
}

VkResult CommandBuffer::reset(VkCommandPoolResetFlags flags)
{
	ASSERT(state != PENDING);

	UNIMPLEMENTED();

	// FIXME: put command buffer back to the initial state
	state = INITIAL;

	return VK_SUCCESS;
}

void CommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
                                    uint32_t clearValueCount, const VkClearValue* pClearValues, VkSubpassContents contents)
{
	UNIMPLEMENTED();
}

void CommandBuffer::nextSubpass(VkSubpassContents contents)
{
	UNIMPLEMENTED();
}

void CommandBuffer::endRenderPass()
{
	UNIMPLEMENTED();
}

void CommandBuffer::executeCommands(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	UNIMPLEMENTED();
}

void CommandBuffer::setDeviceMask(uint32_t deviceMask)
{
	UNIMPLEMENTED();
}

void CommandBuffer::dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                                 uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	UNIMPLEMENTED();
}

void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    VkDependencyFlags dependencyFlags,
                                    uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                    uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                    uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	UNIMPLEMENTED();
}

void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
	pipelines[pipelineBindPoint] = pipeline;
}

void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                                      const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
	for(uint32_t i = firstBinding; i < (firstBinding + bindingCount); ++i)
	{
		vertexInputBindings[i].buffer = pBuffers[i];
		vertexInputBindings[i].offset = pOffsets[i];
	}
}

void CommandBuffer::beginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
	UNIMPLEMENTED();
}

void CommandBuffer::endQuery(VkQueryPool queryPool, uint32_t query)
{
	UNIMPLEMENTED();
}

void CommandBuffer::resetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	UNIMPLEMENTED();
}

void CommandBuffer::writeTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
	UNIMPLEMENTED();
}

void CommandBuffer::copyQueryPoolResults(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount,
	VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	UNIMPLEMENTED();
}

void CommandBuffer::pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags,
	uint32_t offset, uint32_t size, const void* pValues)
{
	UNIMPLEMENTED();
}

void CommandBuffer::setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_VIEWPORT dynamic state enabled
	UNIMPLEMENTED();
}

void CommandBuffer::setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_SCISSOR dynamic state enabled
	UNIMPLEMENTED();
}

void CommandBuffer::setLineWidth(float lineWidth)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_LINE_WIDTH dynamic state enabled

	// If the wide lines feature is not enabled, lineWidth must be 1.0
	ASSERT(lineWidth == 1.0f);

	UNIMPLEMENTED();
}

void CommandBuffer::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_DEPTH_BIAS dynamic state enabled

	// If the depth bias clamping feature is not enabled, depthBiasClamp must be 0.0
	ASSERT(depthBiasClamp == 0.0f);

	UNIMPLEMENTED();
}

void CommandBuffer::setBlendConstants(const float blendConstants[4])
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_BLEND_CONSTANTS dynamic state enabled

	// blendConstants is an array of four values specifying the R, G, B, and A components
	// of the blend constant color used in blending, depending on the blend factor.

	UNIMPLEMENTED();
}

void CommandBuffer::setDepthBounds(float minDepthBounds, float maxDepthBounds)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_DEPTH_BOUNDS dynamic state enabled

	// Unless the VK_EXT_depth_range_unrestricted extension is enabled minDepthBounds and maxDepthBounds must be between 0.0 and 1.0, inclusive
	ASSERT(minDepthBounds >= 0.0f && minDepthBounds <= 1.0f);
	ASSERT(maxDepthBounds >= 0.0f && maxDepthBounds <= 1.0f);

	UNIMPLEMENTED();
}

void CommandBuffer::setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED();
}

void CommandBuffer::setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_WRITE_MASK dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED();
}

void CommandBuffer::setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_REFERENCE dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED();
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
	uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets,
	uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
	UNIMPLEMENTED();
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
	UNIMPLEMENTED();
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	UNIMPLEMENTED();
}

void CommandBuffer::dispatchIndirect(VkBuffer buffer, VkDeviceSize offset)
{
	UNIMPLEMENTED();
}

void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
	UNIMPLEMENTED();
}

void CommandBuffer::copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageCopy* pRegions)
{
	UNIMPLEMENTED();
}

void CommandBuffer::blitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
	UNIMPLEMENTED();
}

void CommandBuffer::copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	UNIMPLEMENTED();
}

void CommandBuffer::copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	UNIMPLEMENTED();
}

void CommandBuffer::updateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
	UNIMPLEMENTED();
}

void CommandBuffer::fillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	UNIMPLEMENTED();
}

void CommandBuffer::clearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor,
	uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	UNIMPLEMENTED();
}

void CommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil,
	uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	UNIMPLEMENTED();
}

void CommandBuffer::clearAttachments(uint32_t attachmentCount, const VkClearAttachment* pAttachments,
	uint32_t rectCount, const VkClearRect* pRects)
{
	UNIMPLEMENTED();
}

void CommandBuffer::resolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageResolve* pRegions)
{
	UNIMPLEMENTED();
}

void CommandBuffer::setEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	UNIMPLEMENTED();
}

void CommandBuffer::resetEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	UNIMPLEMENTED();
}

void CommandBuffer::waitEvents(uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
	uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	UNIMPLEMENTED();
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	UNIMPLEMENTED();
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	UNIMPLEMENTED();
}

void CommandBuffer::drawIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	UNIMPLEMENTED();
}

void CommandBuffer::drawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	UNIMPLEMENTED();
}

void CommandBuffer::submit()
{
	// Perform recorded work
	state = PENDING;

	UNIMPLEMENTED();

	// After work is completed
	state = EXECUTABLE;
}

} // namespace vk