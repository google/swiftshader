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
#include "VkEvent.hpp"
#include "VkFramebuffer.hpp"
#include "VkImage.hpp"
#include "VkImageView.hpp"
#include "VkPipeline.hpp"
#include "VkPipelineLayout.hpp"
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
	BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
	                uint32_t clearValueCount, const VkClearValue* pClearValues) :
		renderPass(Cast(renderPass)), framebuffer(Cast(framebuffer)), renderArea(renderArea),
		clearValueCount(clearValueCount)
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
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.renderPass = renderPass;
		executionState.renderPassFramebuffer = framebuffer;
		renderPass->begin();
		framebuffer->clear(executionState.renderPass, clearValueCount, clearValues, renderArea);
	}

private:
	RenderPass* renderPass;
	Framebuffer* framebuffer;
	VkRect2D renderArea;
	uint32_t clearValueCount;
	VkClearValue* clearValues;
};

class NextSubpass : public CommandBuffer::Command
{
public:
	NextSubpass()
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		bool hasResolveAttachments = (executionState.renderPass->getCurrentSubpass().pResolveAttachments != nullptr);
		if(hasResolveAttachments)
		{
			// FIXME(sugoi): remove the following lines and resolve in Renderer::finishRendering()
			//               for a Draw command or after the last command of the current subpass
			//               which modifies pixels.
			executionState.renderer->synchronize();
			executionState.renderPassFramebuffer->resolve(executionState.renderPass);
		}

		executionState.renderPass->nextSubpass();
	}

private:
};

class EndRenderPass : public CommandBuffer::Command
{
public:
	EndRenderPass()
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		// Execute (implicit or explicit) VkSubpassDependency to VK_SUBPASS_EXTERNAL
		// This is somewhat heavier than the actual ordering required.
		executionState.renderer->synchronize();

		// FIXME(sugoi): remove the following line and resolve in Renderer::finishRendering()
		//               for a Draw command or after the last command of the current subpass
		//               which modifies pixels.
		executionState.renderPassFramebuffer->resolve(executionState.renderPass);

		executionState.renderPass->end();
		executionState.renderPass = nullptr;
		executionState.renderPassFramebuffer = nullptr;
	}

private:
};

class ExecuteCommands : public CommandBuffer::Command
{
public:
	ExecuteCommands(const VkCommandBuffer& commandBuffer) : commandBuffer(commandBuffer)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(commandBuffer)->submitSecondary(executionState);
	}

private:
	const VkCommandBuffer commandBuffer;
};

class PipelineBind : public CommandBuffer::Command
{
public:
	PipelineBind(VkPipelineBindPoint pPipelineBindPoint, VkPipeline pPipeline) :
		pipelineBindPoint(pPipelineBindPoint), pipeline(pPipeline)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.pipelines[pipelineBindPoint] = Cast(pipeline);
	}

private:
	VkPipelineBindPoint pipelineBindPoint;
	VkPipeline pipeline;
};

class Dispatch : public CommandBuffer::Command
{
public:
	Dispatch(uint32_t pGroupCountX, uint32_t pGroupCountY, uint32_t pGroupCountZ) :
			groupCountX(pGroupCountX), groupCountY(pGroupCountY), groupCountZ(pGroupCountZ)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		ComputePipeline* pipeline = static_cast<ComputePipeline*>(
			executionState.pipelines[VK_PIPELINE_BIND_POINT_COMPUTE]);
		pipeline->run(groupCountX, groupCountY, groupCountZ,
			MAX_BOUND_DESCRIPTOR_SETS,
			executionState.boundDescriptorSets[VK_PIPELINE_BIND_POINT_COMPUTE],
			executionState.pushConstants);
	}

private:
	uint32_t groupCountX;
	uint32_t groupCountY;
	uint32_t groupCountZ;
};

struct VertexBufferBind : public CommandBuffer::Command
{
	VertexBufferBind(uint32_t pBinding, const VkBuffer pBuffer, const VkDeviceSize pOffset) :
		binding(pBinding), buffer(pBuffer), offset(pOffset)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.vertexInputBindings[binding] = { buffer, offset };
	}

	uint32_t binding;
	const VkBuffer buffer;
	const VkDeviceSize offset;
};

struct IndexBufferBind : public CommandBuffer::Command
{
	IndexBufferBind(const VkBuffer buffer, const VkDeviceSize offset, const VkIndexType indexType) :
		buffer(buffer), offset(offset), indexType(indexType)
	{

	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.indexBufferBinding = {buffer, offset};
		executionState.indexType = indexType;
	}

	const VkBuffer buffer;
	const VkDeviceSize offset;
	const VkIndexType indexType;
};

void CommandBuffer::ExecutionState::bindVertexInputs(sw::Context& context, int firstVertex, int firstInstance)
{
	for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; i++)
	{
		auto &attrib = context.input[i];
		if (attrib.count)
		{
			const auto &vertexInput = vertexInputBindings[attrib.binding];
			Buffer *buffer = Cast(vertexInput.buffer);
			attrib.buffer = buffer ? buffer->getOffsetPointer(
					attrib.offset + vertexInput.offset + attrib.vertexStride * firstVertex + attrib.instanceStride * firstInstance) : nullptr;
		}
	}
}

void CommandBuffer::ExecutionState::bindAttachments()
{
	// Binds all the attachments for the current subpass
	// Ideally this would be performed by BeginRenderPass and NextSubpass, but
	// there is too much stomping of the renderer's state by setContext() in
	// draws.

	for (auto i = 0u; i < renderPass->getCurrentSubpass().colorAttachmentCount; i++)
	{
		auto attachmentReference = renderPass->getCurrentSubpass().pColorAttachments[i];
		if (attachmentReference.attachment != VK_ATTACHMENT_UNUSED)
		{
			auto attachment = renderPassFramebuffer->getAttachment(attachmentReference.attachment);
			renderer->setRenderTarget(i, attachment, 0);
		}
	}

	auto attachmentReference = renderPass->getCurrentSubpass().pDepthStencilAttachment;
	if (attachmentReference && attachmentReference->attachment != VK_ATTACHMENT_UNUSED)
	{
		auto attachment = renderPassFramebuffer->getAttachment(attachmentReference->attachment);
		if (attachment->hasDepthAspect())
		{
			renderer->setDepthBuffer(attachment, 0);
		}
		if (attachment->hasStencilAspect())
		{
			renderer->setStencilBuffer(attachment, 0);
		}
	}
}

struct Draw : public CommandBuffer::Command
{
	Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
		: vertexCount(vertexCount), instanceCount(instanceCount), firstVertex(firstVertex), firstInstance(firstInstance)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		GraphicsPipeline* pipeline = static_cast<GraphicsPipeline*>(
			executionState.pipelines[VK_PIPELINE_BIND_POINT_GRAPHICS]);

		sw::Context context = pipeline->getContext();
		executionState.bindVertexInputs(context, firstVertex, firstInstance);

		const auto& boundDescriptorSets = executionState.boundDescriptorSets[VK_PIPELINE_BIND_POINT_GRAPHICS];
		for(int i = 0; i < vk::MAX_BOUND_DESCRIPTOR_SETS; i++)
		{
			context.descriptorSets[i] = reinterpret_cast<vk::DescriptorSet*>(boundDescriptorSets[i]);
		}

		context.pushConstants = executionState.pushConstants;

		executionState.renderer->setContext(context);
		executionState.renderer->setScissor(pipeline->getScissor());
		executionState.renderer->setViewport(pipeline->getViewport());
		executionState.renderer->setBlendConstant(pipeline->getBlendConstants());

		executionState.bindAttachments();

		const uint32_t primitiveCount = pipeline->computePrimitiveCount(vertexCount);
		for(uint32_t instance = firstInstance; instance != firstInstance + instanceCount; instance++)
		{
			executionState.renderer->setInstanceID(instance);
			executionState.renderer->draw(context.drawType, primitiveCount);
			executionState.renderer->advanceInstanceAttributes();
		}
	}

	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstVertex;
	uint32_t firstInstance;
};

struct DrawIndexed : public CommandBuffer::Command
{
	DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
			: indexCount(indexCount), instanceCount(instanceCount), firstIndex(firstIndex), vertexOffset(vertexOffset), firstInstance(firstInstance)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		GraphicsPipeline* pipeline = static_cast<GraphicsPipeline*>(
				executionState.pipelines[VK_PIPELINE_BIND_POINT_GRAPHICS]);

		sw::Context context = pipeline->getContext();

		executionState.bindVertexInputs(context, vertexOffset, firstInstance);

		const auto& boundDescriptorSets = executionState.boundDescriptorSets[VK_PIPELINE_BIND_POINT_GRAPHICS];
		for(int i = 0; i < vk::MAX_BOUND_DESCRIPTOR_SETS; i++)
		{
			context.descriptorSets[i] = reinterpret_cast<vk::DescriptorSet*>(boundDescriptorSets[i]);
		}

		context.pushConstants = executionState.pushConstants;

		context.indexBuffer = Cast(executionState.indexBufferBinding.buffer)->getOffsetPointer(
				executionState.indexBufferBinding.offset + firstIndex * (executionState.indexType == VK_INDEX_TYPE_UINT16 ? 2 : 4));

		executionState.renderer->setContext(context);
		executionState.renderer->setScissor(pipeline->getScissor());
		executionState.renderer->setViewport(pipeline->getViewport());
		executionState.renderer->setBlendConstant(pipeline->getBlendConstants());

		executionState.bindAttachments();

		auto drawType = executionState.indexType == VK_INDEX_TYPE_UINT16
				? (context.drawType | sw::DRAW_INDEXED16) : (context.drawType | sw::DRAW_INDEXED32);

		const uint32_t primitiveCount = pipeline->computePrimitiveCount(indexCount);
		for(uint32_t instance = firstInstance; instance != firstInstance + instanceCount; instance++)
		{
			executionState.renderer->setInstanceID(instance);
			executionState.renderer->draw(static_cast<sw::DrawType>(drawType), primitiveCount);
			executionState.renderer->advanceInstanceAttributes();
		}
	}

	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t vertexOffset;
	uint32_t firstInstance;
};

struct ImageToImageCopy : public CommandBuffer::Command
{
	ImageToImageCopy(VkImage pSrcImage, VkImage pDstImage, const VkImageCopy& pRegion) :
		srcImage(pSrcImage), dstImage(pDstImage), region(pRegion)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
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

	void play(CommandBuffer::ExecutionState& executionState) override
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

	void play(CommandBuffer::ExecutionState& executionState) override
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

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(dstImage)->copyFrom(srcBuffer, region);
	}

private:
	VkBuffer srcBuffer;
	VkImage dstImage;
	const VkBufferImageCopy region;
};

struct FillBuffer : public CommandBuffer::Command
{
	FillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) :
		dstBuffer(dstBuffer), dstOffset(dstOffset), size(size), data(data)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(dstBuffer)->fill(dstOffset, size, data);
	}

private:
	VkBuffer dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize size;
	uint32_t data;
};

struct UpdateBuffer : public CommandBuffer::Command
{
	UpdateBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) :
		dstBuffer(dstBuffer), dstOffset(dstOffset), dataSize(dataSize), pData(pData)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(dstBuffer)->update(dstOffset, dataSize, pData);
	}

private:
	VkBuffer dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize dataSize;
	const void* pData;
};

struct ClearColorImage : public CommandBuffer::Command
{
	ClearColorImage(VkImage image, const VkClearColorValue& color, const VkImageSubresourceRange& range) :
		image(image), color(color), range(range)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
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

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(image)->clear(depthStencil, range);
	}

private:
	VkImage image;
	const VkClearDepthStencilValue depthStencil;
	const VkImageSubresourceRange range;
};

struct ClearAttachment : public CommandBuffer::Command
{
	ClearAttachment(const VkClearAttachment& attachment, const VkClearRect& rect) :
		attachment(attachment), rect(rect)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.renderPassFramebuffer->clear(executionState.renderPass, attachment, rect);
	}

private:
	const VkClearAttachment attachment;
	const VkClearRect rect;
};

struct BlitImage : public CommandBuffer::Command
{
	BlitImage(VkImage srcImage, VkImage dstImage, const VkImageBlit& region, VkFilter filter) :
		srcImage(srcImage), dstImage(dstImage), region(region), filter(filter)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(srcImage)->blit(dstImage, region, filter);
	}

private:
	VkImage srcImage;
	VkImage dstImage;
	VkImageBlit region;
	VkFilter filter;
};

struct PipelineBarrier : public CommandBuffer::Command
{
	PipelineBarrier()
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		// This is a very simple implementation that simply calls sw::Renderer::synchronize(),
		// since the driver is free to move the source stage towards the bottom of the pipe
		// and the target stage towards the top, so a full pipeline sync is spec compliant.
		executionState.renderer->synchronize();

		// Right now all buffers are read-only in drawcalls but a similar mechanism will be required once we support SSBOs.

		// Also note that this would be a good moment to update cube map borders or decompress compressed textures, if necessary.
	}

private:
};

struct SignalEvent : public CommandBuffer::Command
{
	SignalEvent(VkEvent ev, VkPipelineStageFlags stageMask) : ev(ev), stageMask(stageMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		if(Cast(ev)->signal())
		{
			// Was waiting for signal on this event, sync now
			executionState.renderer->synchronize();
		}
	}

private:
	VkEvent ev;
	VkPipelineStageFlags stageMask; // FIXME(b/117835459) : We currently ignore the flags and signal the event at the last stage
};

struct ResetEvent : public CommandBuffer::Command
{
	ResetEvent(VkEvent ev, VkPipelineStageFlags stageMask) : ev(ev), stageMask(stageMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		Cast(ev)->reset();
	}

private:
	VkEvent ev;
	VkPipelineStageFlags stageMask; // FIXME(b/117835459) : We currently ignore the flags and reset the event at the last stage
};

struct WaitEvent : public CommandBuffer::Command
{
	WaitEvent(VkEvent ev) : ev(ev)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		if(!Cast(ev)->wait())
		{
			// Already signaled, sync now
			executionState.renderer->synchronize();
		}
	}

private:
	VkEvent ev;
};

struct BindDescriptorSet : public CommandBuffer::Command
{
	BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, uint32_t set, const VkDescriptorSet& descriptorSet)
		: pipelineBindPoint(pipelineBindPoint), set(set), descriptorSet(descriptorSet)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		ASSERT((pipelineBindPoint < VK_PIPELINE_BIND_POINT_RANGE_SIZE) && (set < MAX_BOUND_DESCRIPTOR_SETS));
		executionState.boundDescriptorSets[pipelineBindPoint][set] = descriptorSet;
	}

private:
	VkPipelineBindPoint pipelineBindPoint;
	uint32_t set;
	const VkDescriptorSet descriptorSet;
};

struct SetPushConstants : public CommandBuffer::Command
{
	SetPushConstants(uint32_t offset, uint32_t size, void const *pValues)
		: offset(offset), size(size)
	{
		ASSERT(offset < MAX_PUSH_CONSTANT_SIZE);
		ASSERT(offset + size <= MAX_PUSH_CONSTANT_SIZE);

		memcpy(data, pValues, size);
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		memcpy(&executionState.pushConstants.data[offset], data, size);
	}

private:
	uint32_t offset;
	uint32_t size;
	unsigned char data[MAX_PUSH_CONSTANT_SIZE];
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

	// Nothing interesting to do based on flags. We don't have any optimizations
	// to apply for ONE_TIME_SUBMIT or (lack of) SIMULTANEOUS_USE. RENDER_PASS_CONTINUE
	// must also provide a non-null pInheritanceInfo, which we don't implement yet, but is caught below.
	(void) flags;

	// pInheritanceInfo merely contains optimization hints, so we currently ignore it

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

	addCommand<BeginRenderPass>(renderPass, framebuffer, renderArea, clearValueCount, clearValues);
}

void CommandBuffer::nextSubpass(VkSubpassContents contents)
{
	ASSERT(state == RECORDING);

	addCommand<NextSubpass>();
}

void CommandBuffer::endRenderPass()
{
	addCommand<EndRenderPass>();
}

void CommandBuffer::executeCommands(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < commandBufferCount; ++i)
	{
		addCommand<ExecuteCommands>(pCommandBuffers[i]);
	}
}

void CommandBuffer::setDeviceMask(uint32_t deviceMask)
{
	UNIMPLEMENTED("setDeviceMask");
}

void CommandBuffer::dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                                 uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	UNIMPLEMENTED("dispatchBase");
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
	switch(pipelineBindPoint)
	{
		case VK_PIPELINE_BIND_POINT_COMPUTE:
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			addCommand<PipelineBind>(pipelineBindPoint, pipeline);
			break;
		default:
			UNIMPLEMENTED("pipelineBindPoint");
	}
}

void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                                      const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
	for(uint32_t i = 0; i < bindingCount; ++i)
	{
		addCommand<VertexBufferBind>(i + firstBinding, pBuffers[i], pOffsets[i]);
	}
}

void CommandBuffer::beginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
	UNIMPLEMENTED("beginQuery");
}

void CommandBuffer::endQuery(VkQueryPool queryPool, uint32_t query)
{
	UNIMPLEMENTED("endQuery");
}

void CommandBuffer::resetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	UNIMPLEMENTED("resetQueryPool");
}

void CommandBuffer::writeTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
	UNIMPLEMENTED("writeTimestamp");
}

void CommandBuffer::copyQueryPoolResults(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount,
	VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	UNIMPLEMENTED("copyQueryPoolResults");
}

void CommandBuffer::pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags,
	uint32_t offset, uint32_t size, const void* pValues)
{
	addCommand<SetPushConstants>(offset, size, pValues);
}

void CommandBuffer::setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_VIEWPORT dynamic state enabled
	UNIMPLEMENTED("setViewport");
}

void CommandBuffer::setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_SCISSOR dynamic state enabled
	UNIMPLEMENTED("setScissor");
}

void CommandBuffer::setLineWidth(float lineWidth)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_LINE_WIDTH dynamic state enabled

	// If the wide lines feature is not enabled, lineWidth must be 1.0
	ASSERT(lineWidth == 1.0f);

	UNIMPLEMENTED("setLineWidth");
}

void CommandBuffer::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_DEPTH_BIAS dynamic state enabled

	// If the depth bias clamping feature is not enabled, depthBiasClamp must be 0.0
	ASSERT(depthBiasClamp == 0.0f);

	UNIMPLEMENTED("setDepthBias");
}

void CommandBuffer::setBlendConstants(const float blendConstants[4])
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_BLEND_CONSTANTS dynamic state enabled

	// blendConstants is an array of four values specifying the R, G, B, and A components
	// of the blend constant color used in blending, depending on the blend factor.

	UNIMPLEMENTED("setBlendConstants");
}

void CommandBuffer::setDepthBounds(float minDepthBounds, float maxDepthBounds)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_DEPTH_BOUNDS dynamic state enabled

	// Unless the VK_EXT_depth_range_unrestricted extension is enabled minDepthBounds and maxDepthBounds must be between 0.0 and 1.0, inclusive
	ASSERT(minDepthBounds >= 0.0f && minDepthBounds <= 1.0f);
	ASSERT(maxDepthBounds >= 0.0f && maxDepthBounds <= 1.0f);

	UNIMPLEMENTED("setDepthBounds");
}

void CommandBuffer::setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED("setStencilCompareMask");
}

void CommandBuffer::setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_WRITE_MASK dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED("setStencilWriteMask");
}

void CommandBuffer::setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
	// Note: The bound graphics pipeline must have been created with the VK_DYNAMIC_STATE_STENCIL_REFERENCE dynamic state enabled

	// faceMask must not be 0
	ASSERT(faceMask != 0);

	UNIMPLEMENTED("setStencilReference");
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
	uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets,
	uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
	ASSERT(state == RECORDING);

	if(dynamicOffsetCount > 0)
	{
		UNIMPLEMENTED("bindDescriptorSets");
	}

	for(uint32_t i = 0; i < descriptorSetCount; i++)
	{
		addCommand<BindDescriptorSet>(pipelineBindPoint, firstSet + i, pDescriptorSets[i]);
	}
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
	addCommand<IndexBufferBind>(buffer, offset, indexType);
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	addCommand<Dispatch>(groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::dispatchIndirect(VkBuffer buffer, VkDeviceSize offset)
{
	UNIMPLEMENTED("dispatchIndirect");
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
	ASSERT(state == RECORDING);

	addCommand<UpdateBuffer>(dstBuffer, dstOffset, dataSize, pData);
}

void CommandBuffer::fillBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	ASSERT(state == RECORDING);

	addCommand<FillBuffer>(dstBuffer, dstOffset, size, data);
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
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < attachmentCount; i++)
	{
		for(uint32_t j = 0; j < rectCount; j++)
		{
			addCommand<ClearAttachment>(pAttachments[i], pRects[j]);
		}
	}
}

void CommandBuffer::resolveImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageResolve* pRegions)
{
	UNIMPLEMENTED("resolveImage");
}

void CommandBuffer::setEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	ASSERT(state == RECORDING);

	addCommand<SignalEvent>(event, stageMask);
}

void CommandBuffer::resetEvent(VkEvent event, VkPipelineStageFlags stageMask)
{
	ASSERT(state == RECORDING);

	addCommand<ResetEvent>(event, stageMask);
}

void CommandBuffer::waitEvents(uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
	uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	ASSERT(state == RECORDING);

	// TODO(b/117835459): Since we always do a full barrier, all memory barrier related arguments are ignored

	// Note: srcStageMask and dstStageMask are currently ignored
	for(uint32_t i = 0; i < eventCount; i++)
	{
		addCommand<WaitEvent>(pEvents[i]);
	}
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	addCommand<Draw>(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	addCommand<DrawIndexed>(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::drawIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	UNIMPLEMENTED("drawIndirect");
}

void CommandBuffer::drawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	UNIMPLEMENTED("drawIndexedIndirect");
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

void CommandBuffer::submitSecondary(CommandBuffer::ExecutionState& executionState) const
{
	for(auto& command : *commands)
	{
		command->play(executionState);
	}
}

} // namespace vk
