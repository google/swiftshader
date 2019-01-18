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
#include "VkBuffer.hpp"
#include "VkFramebuffer.hpp"
#include "VkImage.hpp"
#include "VkPipeline.hpp"
#include "VkRenderPass.hpp"
#include "Device/Renderer.hpp"

#include <cstring>

namespace vk
{

class CommandBuffer::Command
{
public:
	// FIXME (b/119421344): change the commandBuffer argument to a CommandBuffer state
	virtual void play(CommandBuffer::ExecutionState& executionState) = 0;
	virtual ~Command() {}
};

class BeginRenderPass : public CommandBuffer::Command
{
public:
	BeginRenderPass(VkRenderPass pRenderPass, VkFramebuffer pFramebuffer, VkRect2D pRenderArea,
	                uint32_t pClearValueCount, const VkClearValue* pClearValues) :
		renderPass(pRenderPass), framebuffer(pFramebuffer), renderArea(pRenderArea),
		clearValueCount(pClearValueCount)
	{
		// FIXME (b/119409619): use an allocator here so we can control all memory allocations
		clearValues = new VkClearValue[clearValueCount];
		memcpy(clearValues, pClearValues, clearValueCount * sizeof(VkClearValue));
	}

	~BeginRenderPass() override
	{
		delete [] clearValues;
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(renderPass)->begin();
		Cast(framebuffer)->clear(clearValueCount, clearValues, renderArea);
	}

private:
	VkRenderPass renderPass;
	VkFramebuffer framebuffer;
	VkRect2D renderArea;
	uint32_t clearValueCount;
	VkClearValue* clearValues;
};

class EndRenderPass : public CommandBuffer::Command
{
public:
	EndRenderPass()
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(executionState.renderpass)->end();
	}

private:
};

class PipelineBind : public CommandBuffer::Command
{
public:
	PipelineBind(VkPipelineBindPoint pPipelineBindPoint, VkPipeline pPipeline) :
		pipelineBindPoint(pPipelineBindPoint), pipeline(pPipeline)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState)
	{
		executionState.pipelines[pipelineBindPoint] = pipeline;
	}

private:
	VkPipelineBindPoint pipelineBindPoint;
	VkPipeline pipeline;
};

struct VertexBufferBind : public CommandBuffer::Command
{
	VertexBufferBind(uint32_t pBinding, const VkBuffer pBuffer, const VkDeviceSize pOffset) :
		binding(pBinding), buffer(pBuffer), offset(pOffset)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		executionState.vertexInputBindings[binding] = { buffer, offset };
	}

	uint32_t binding;
	const VkBuffer buffer;
	const VkDeviceSize offset;
};

struct Draw : public CommandBuffer::Command
{
	Draw(uint32_t pVertexCount) : vertexCount(pVertexCount)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		GraphicsPipeline* pipeline = static_cast<GraphicsPipeline*>(
			Cast(executionState.pipelines[VK_PIPELINE_BIND_POINT_GRAPHICS]));

		sw::Context context = pipeline->getContext();
		for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; i++)
		{
			const auto& vertexInput = executionState.vertexInputBindings[i];
			Buffer* buffer = Cast(vertexInput.buffer);
			context.input[i].buffer = buffer ? buffer->getOffsetPointer(vertexInput.offset) : nullptr;
		}

		executionState.renderer->setContext(context);
		executionState.renderer->setScissor(pipeline->getScissor());
		executionState.renderer->setViewport(pipeline->getViewport());
		executionState.renderer->setBlendConstant(pipeline->getBlendConstants());

		executionState.renderer->draw(context.drawType, 0, pipeline->computePrimitiveCount(vertexCount));
	}

	uint32_t vertexCount;
};

struct ImageToImageCopy : public CommandBuffer::Command
{
	ImageToImageCopy(VkImage pSrcImage, VkImage pDstImage, const VkImageCopy& pRegion) :
		srcImage(pSrcImage), dstImage(pDstImage), region(pRegion)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(srcImage)->copyTo(dstImage, region);
	}

private:
	VkImage srcImage;
	VkImage dstImage;
	const VkImageCopy region;
};

struct BufferToBufferCopy : public CommandBuffer::Command
{
	BufferToBufferCopy(VkBuffer pSrcBuffer, VkBuffer pDstBuffer, const VkBufferCopy& pRegion) :
		srcBuffer(pSrcBuffer), dstBuffer(pDstBuffer), region(pRegion)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(srcBuffer)->copyTo(Cast(dstBuffer), region);
	}

private:
	VkBuffer srcBuffer;
	VkBuffer dstBuffer;
	const VkBufferCopy region;
};

struct ImageToBufferCopy : public CommandBuffer::Command
{
	ImageToBufferCopy(VkImage pSrcImage, VkBuffer pDstBuffer, const VkBufferImageCopy& pRegion) :
		srcImage(pSrcImage), dstBuffer(pDstBuffer), region(pRegion)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(srcImage)->copyTo(dstBuffer, region);
	}

private:
	VkImage srcImage;
	VkBuffer dstBuffer;
	const VkBufferImageCopy region;
};

struct BufferToImageCopy : public CommandBuffer::Command
{
	BufferToImageCopy(VkBuffer pSrcBuffer, VkImage pDstImage, const VkBufferImageCopy& pRegion) :
		srcBuffer(pSrcBuffer), dstImage(pDstImage), region(pRegion)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(dstImage)->copyFrom(srcBuffer, region);
	}

private:
	VkBuffer srcBuffer;
	VkImage dstImage;
	const VkBufferImageCopy region;
};

struct ClearColorImage : public CommandBuffer::Command
{
	ClearColorImage(VkImage image, const VkClearColorValue& color, const VkImageSubresourceRange& range) :
		image(image), color(color), range(range)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(image)->clear(color, range);
	}

private:
	VkImage image;
	const VkClearColorValue color;
	const VkImageSubresourceRange range;
};

struct ClearDepthStencilImage : public CommandBuffer::Command
{
	ClearDepthStencilImage(VkImage image, const VkClearDepthStencilValue& depthStencil, const VkImageSubresourceRange& range) :
		image(image), depthStencil(depthStencil), range(range)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(image)->clear(depthStencil, range);
	}

private:
	VkImage image;
	const VkClearDepthStencilValue depthStencil;
	const VkImageSubresourceRange range;
};

struct BlitImage : public CommandBuffer::Command
{
	BlitImage(VkImage srcImage, VkImage dstImage, const VkImageBlit& region, VkFilter filter) :
		srcImage(srcImage), dstImage(dstImage), region(region), filter(filter)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		Cast(srcImage)->blit(dstImage, region, filter);
	}

private:
	VkImage srcImage;
	VkImage dstImage;
	const VkImageBlit& region;
	VkFilter filter;
};

struct PipelineBarrier : public CommandBuffer::Command
{
	PipelineBarrier()
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		// This can currently be a noop. The sw::Surface locking/unlocking mechanism used by the renderer already takes care of
		// making sure the read/writes always happen in order. Eventually, if we remove this synchronization mechanism, we can
		// have a very simple implementation that simply calls sw::Renderer::sync(), since the driver is free to move the source
		// stage towards the bottom of the pipe and the target stage towards the top, so a full pipeline sync is spec compliant.

		// Right now all buffers are read-only in drawcalls but a similar mechanism will be required once we support SSBOs.

		// Also note that this would be a good moment to update cube map borders or decompress compressed textures, if necessary.
	}

private:
};

CommandBuffer::CommandBuffer(VkCommandBufferLevel pLevel) : level(pLevel)
{
	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	commands = new std::vector<std::unique_ptr<Command> >();
}

void CommandBuffer::destroy(const VkAllocationCallbacks* pAllocator)
{
	delete commands;
}

void CommandBuffer::resetState()
{
	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	commands->clear();

	state = INITIAL;
}

VkResult CommandBuffer::begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
	ASSERT((state != RECORDING) && (state != PENDING));

	if(!((flags == 0) || (flags == VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) || pInheritanceInfo)
	{
		UNIMPLEMENTED();
	}

	if(state != INITIAL)
	{
		// Implicit reset
		resetState();
	}

	state = RECORDING;

	return VK_SUCCESS;
}

VkResult CommandBuffer::end()
{
	ASSERT(state == RECORDING);

	state = EXECUTABLE;

	return VK_SUCCESS;
}

VkResult CommandBuffer::reset(VkCommandPoolResetFlags flags)
{
	ASSERT(state != PENDING);

	resetState();

	return VK_SUCCESS;
}

template<typename T, typename... Args>
void CommandBuffer::addCommand(Args&&... args)
{
	commands->push_back(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
}

void CommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
                                    uint32_t clearValueCount, const VkClearValue* clearValues, VkSubpassContents contents)
{
	ASSERT(state == RECORDING);

	if(contents != VK_SUBPASS_CONTENTS_INLINE)
	{
		UNIMPLEMENTED();
	}

	addCommand<BeginRenderPass>(renderPass, framebuffer, renderArea, clearValueCount, clearValues);
}

void CommandBuffer::nextSubpass(VkSubpassContents contents)
{
	UNIMPLEMENTED();
}

void CommandBuffer::endRenderPass()
{
	addCommand<EndRenderPass>();
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
	addCommand<PipelineBarrier>();
}

void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
	if(pipelineBindPoint != VK_PIPELINE_BIND_POINT_GRAPHICS)
	{
		UNIMPLEMENTED();
	}

	addCommand<PipelineBind>(pipelineBindPoint, pipeline);
}

void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                                      const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
	for(uint32_t i = firstBinding; i < (firstBinding + bindingCount); ++i)
	{
		addCommand<VertexBufferBind>(i, pBuffers[i], pOffsets[i]);
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
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<BufferToBufferCopy>(srcBuffer, dstBuffer, pRegions[i]);
	}
}

void CommandBuffer::copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageCopy* pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<ImageToImageCopy>(srcImage, dstImage, pRegions[i]);
	}
}

void CommandBuffer::blitImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<BlitImage>(srcImage, dstImage, pRegions[i], filter);
	}
}

void CommandBuffer::copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<BufferToImageCopy>(srcBuffer, dstImage, pRegions[i]);
	}
}

void CommandBuffer::copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<ImageToBufferCopy>(srcImage, dstBuffer, pRegions[i]);
	}
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
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<ClearColorImage>(image, pColor[i], pRanges[i]);
	}
}

void CommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil,
	uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<ClearDepthStencilImage>(image, pDepthStencil[i], pRanges[i]);
	}
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
	if(instanceCount > 1 || firstVertex != 0 || firstInstance != 0)
	{
		UNIMPLEMENTED();
	}

	addCommand<Draw>(vertexCount);
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

void CommandBuffer::submit(CommandBuffer::ExecutionState& executionState)
{
	// Perform recorded work
	state = PENDING;

	for(auto& command : *commands)
	{
		command->play(executionState);
	}

	// After work is completed
	state = EXECUTABLE;
}

} // namespace vk
