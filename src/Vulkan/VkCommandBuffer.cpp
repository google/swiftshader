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
#include "VkFence.hpp"
#include "VkFramebuffer.hpp"
#include "VkImage.hpp"
#include "VkImageView.hpp"
#include "VkPipeline.hpp"
#include "VkPipelineLayout.hpp"
#include "VkQueryPool.hpp"
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
	BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, VkRect2D renderArea,
	                uint32_t clearValueCount, const VkClearValue* pClearValues) :
		renderPass(renderPass), framebuffer(framebuffer), renderArea(renderArea),
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
		executionState.subpassIndex = 0;
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
		bool hasResolveAttachments = (executionState.renderPass->getSubpass(executionState.subpassIndex).pResolveAttachments != nullptr);
		if(hasResolveAttachments)
		{
			// FIXME(sugoi): remove the following lines and resolve in Renderer::finishRendering()
			//               for a Draw command or after the last command of the current subpass
			//               which modifies pixels.
			executionState.renderer->synchronize();
			executionState.renderPassFramebuffer->resolve(executionState.renderPass, executionState.subpassIndex);
		}

		++executionState.subpassIndex;
	}
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
		executionState.renderPassFramebuffer->resolve(executionState.renderPass, executionState.subpassIndex);
		executionState.renderPass = nullptr;
		executionState.renderPassFramebuffer = nullptr;
	}
};

class ExecuteCommands : public CommandBuffer::Command
{
public:
	ExecuteCommands(const CommandBuffer* commandBuffer) : commandBuffer(commandBuffer)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		commandBuffer->submitSecondary(executionState);
	}

private:
	const CommandBuffer* commandBuffer;
};

class PipelineBind : public CommandBuffer::Command
{
public:
	PipelineBind(VkPipelineBindPoint pipelineBindPoint, Pipeline* pipeline) :
		pipelineBindPoint(pipelineBindPoint), pipeline(pipeline)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.pipelineState[pipelineBindPoint].pipeline = pipeline;
	}

private:
	VkPipelineBindPoint pipelineBindPoint;
	Pipeline* pipeline;
};

class Dispatch : public CommandBuffer::Command
{
public:
	Dispatch(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) :
			baseGroupX(baseGroupX), baseGroupY(baseGroupY), baseGroupZ(baseGroupZ),
			groupCountX(groupCountX), groupCountY(groupCountY), groupCountZ(groupCountZ)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_COMPUTE];

		ComputePipeline* pipeline = static_cast<ComputePipeline*>(pipelineState.pipeline);
		pipeline->run(baseGroupX, baseGroupY, baseGroupZ,
			groupCountX, groupCountY, groupCountZ,
			pipelineState.descriptorSets,
			pipelineState.descriptorDynamicOffsets,
			executionState.pushConstants);
	}

private:
	uint32_t baseGroupX;
	uint32_t baseGroupY;
	uint32_t baseGroupZ;
	uint32_t groupCountX;
	uint32_t groupCountY;
	uint32_t groupCountZ;
};

class DispatchIndirect : public CommandBuffer::Command
{
public:
	DispatchIndirect(Buffer* buffer, VkDeviceSize offset) :
			buffer(buffer), offset(offset)
	{
	}

protected:
	void play(CommandBuffer::ExecutionState& executionState) override
	{
		auto cmd = reinterpret_cast<VkDispatchIndirectCommand const *>(buffer->getOffsetPointer(offset));

		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_COMPUTE];

		ComputePipeline* pipeline = static_cast<ComputePipeline*>(pipelineState.pipeline);
		pipeline->run(0, 0, 0, cmd->x, cmd->y, cmd->z,
			pipelineState.descriptorSets,
			pipelineState.descriptorDynamicOffsets,
			executionState.pushConstants);
	}

private:
	const Buffer* buffer;
	VkDeviceSize offset;
};

struct VertexBufferBind : public CommandBuffer::Command
{
	VertexBufferBind(uint32_t binding, Buffer* buffer, const VkDeviceSize offset) :
		binding(binding), buffer(buffer), offset(offset)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.vertexInputBindings[binding] = { buffer, offset };
	}

private:
	uint32_t binding;
	Buffer* buffer;
	const VkDeviceSize offset;
};

struct IndexBufferBind : public CommandBuffer::Command
{
	IndexBufferBind(Buffer* buffer, const VkDeviceSize offset, const VkIndexType indexType) :
		buffer(buffer), offset(offset), indexType(indexType)
	{

	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.indexBufferBinding = { buffer, offset };
		executionState.indexType = indexType;
	}

private:
	Buffer* buffer;
	const VkDeviceSize offset;
	const VkIndexType indexType;
};

struct SetViewport : public CommandBuffer::Command
{
	SetViewport(const VkViewport& viewport, uint32_t viewportID) :
		viewport(viewport), viewportID(viewportID)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.dynamicState.viewport = viewport;
	}

private:
	const VkViewport viewport;
	uint32_t viewportID;
};

struct SetScissor : public CommandBuffer::Command
{
	SetScissor(const VkRect2D& scissor, uint32_t scissorID) :
		scissor(scissor), scissorID(scissorID)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.dynamicState.scissor = scissor;
	}

private:
	const VkRect2D scissor;
	uint32_t scissorID;
};

struct SetDepthBias : public CommandBuffer::Command
{
	SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) :
		depthBiasConstantFactor(depthBiasConstantFactor), depthBiasClamp(depthBiasClamp), depthBiasSlopeFactor(depthBiasSlopeFactor)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.dynamicState.depthBiasConstantFactor = depthBiasConstantFactor;
		executionState.dynamicState.depthBiasClamp = depthBiasClamp;
		executionState.dynamicState.depthBiasSlopeFactor = depthBiasSlopeFactor;
	}

private:
	float depthBiasConstantFactor;
	float depthBiasClamp;
	float depthBiasSlopeFactor;
};

struct SetBlendConstants : public CommandBuffer::Command
{
	SetBlendConstants(const float blendConstants[4])
	{
		memcpy(this->blendConstants, blendConstants, sizeof(this->blendConstants));
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		memcpy(&(executionState.dynamicState.blendConstants[0]), blendConstants, sizeof(blendConstants));
	}

private:
	float blendConstants[4];
};

struct SetDepthBounds : public CommandBuffer::Command
{
	SetDepthBounds(float minDepthBounds, float maxDepthBounds) :
		minDepthBounds(minDepthBounds), maxDepthBounds(maxDepthBounds)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.dynamicState.minDepthBounds = minDepthBounds;
		executionState.dynamicState.maxDepthBounds = maxDepthBounds;
	}

private:
	float minDepthBounds;
	float maxDepthBounds;
};
struct SetStencilCompareMask : public CommandBuffer::Command
{
	SetStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask) :
		faceMask(faceMask), compareMask(compareMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			executionState.dynamicState.compareMask[0] = compareMask;
		}
		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			executionState.dynamicState.compareMask[1] = compareMask;
		}
	}

private:
	VkStencilFaceFlags faceMask;
	uint32_t compareMask;
};

struct SetStencilWriteMask : public CommandBuffer::Command
{
	SetStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask) :
		faceMask(faceMask), writeMask(writeMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			executionState.dynamicState.writeMask[0] = writeMask;
		}
		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			executionState.dynamicState.writeMask[1] = writeMask;
		}
	}

private:
	VkStencilFaceFlags faceMask;
	uint32_t writeMask;
};

struct SetStencilReference : public CommandBuffer::Command
{
	SetStencilReference(VkStencilFaceFlags faceMask, uint32_t reference) :
		faceMask(faceMask), reference(reference)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		if(faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			executionState.dynamicState.reference[0] = reference;
		}
		if(faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			executionState.dynamicState.reference[1] = reference;
		}
	}

private:
	VkStencilFaceFlags faceMask;
	uint32_t reference;
};

void CommandBuffer::ExecutionState::bindVertexInputs(sw::Context& context, int firstInstance)
{
	for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; i++)
	{
		auto &attrib = context.input[i];
		if (attrib.count)
		{
			const auto &vertexInput = vertexInputBindings[attrib.binding];
			VkDeviceSize offset = attrib.offset + vertexInput.offset +
			                      attrib.instanceStride * firstInstance;
			attrib.buffer = vertexInput.buffer ? vertexInput.buffer->getOffsetPointer(offset) : nullptr;

			VkDeviceSize size = vertexInput.buffer ? vertexInput.buffer->getSize() : 0;
			attrib.robustnessSize = (size > offset) ? size - offset : 0;
		}
	}
}

void CommandBuffer::ExecutionState::bindAttachments(sw::Context& context)
{
	// Binds all the attachments for the current subpass
	// Ideally this would be performed by BeginRenderPass and NextSubpass, but
	// there is too much stomping of the renderer's state by setContext() in
	// draws.

	auto const & subpass = renderPass->getSubpass(subpassIndex);

	for (auto i = 0u; i < subpass.colorAttachmentCount; i++)
	{
		auto attachmentReference = subpass.pColorAttachments[i];
		if (attachmentReference.attachment != VK_ATTACHMENT_UNUSED)
		{
			context.renderTarget[i] = renderPassFramebuffer->getAttachment(attachmentReference.attachment);
		}
	}

	auto attachmentReference = subpass.pDepthStencilAttachment;
	if (attachmentReference && attachmentReference->attachment != VK_ATTACHMENT_UNUSED)
	{
		auto attachment = renderPassFramebuffer->getAttachment(attachmentReference->attachment);
		if (attachment->hasDepthAspect())
		{
			context.depthBuffer = attachment;
		}
		if (attachment->hasStencilAspect())
		{
			context.stencilBuffer = attachment;
		}
	}
}

struct DrawBase : public CommandBuffer::Command
{
	int bytesPerIndex(CommandBuffer::ExecutionState const& executionState)
	{
		return executionState.indexType == VK_INDEX_TYPE_UINT16 ? 2 : 4;
	}

	template<typename T>
	void processPrimitiveRestart(T* indexBuffer,
	                             uint32_t count,
		                         GraphicsPipeline* pipeline,
	                             std::vector<std::pair<uint32_t, void*>>& indexBuffers)
	{
		static const T RestartIndex = static_cast<T>(-1);
		T* indexBufferStart = indexBuffer;
		uint32_t vertexCount = 0;
		for(uint32_t i = 0; i < count; i++)
		{
			if(indexBuffer[i] == RestartIndex)
			{
				// Record previous segment
				if(vertexCount > 0)
				{
					uint32_t primitiveCount = pipeline->computePrimitiveCount(vertexCount);
					if(primitiveCount > 0)
					{
						indexBuffers.push_back({ primitiveCount, indexBufferStart });
					}
				}
				vertexCount = 0;
			}
			else
			{
				if(vertexCount == 0)
				{
					indexBufferStart = indexBuffer + i;
				}
				vertexCount++;
			}
		}

		// Record last segment
		if(vertexCount > 0)
		{
			uint32_t primitiveCount = pipeline->computePrimitiveCount(vertexCount);
			if(primitiveCount > 0)
			{
				indexBuffers.push_back({ primitiveCount, indexBufferStart });
			}
		}
	}

	void draw(CommandBuffer::ExecutionState& executionState, bool indexed,
			uint32_t count, uint32_t instanceCount, uint32_t first, int32_t vertexOffset, uint32_t firstInstance)
	{
		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_GRAPHICS];

		GraphicsPipeline *pipeline = static_cast<GraphicsPipeline *>(pipelineState.pipeline);

		sw::Context context = pipeline->getContext();

		executionState.bindVertexInputs(context, firstInstance);

		context.descriptorSets = pipelineState.descriptorSets;
		context.descriptorDynamicOffsets = pipelineState.descriptorDynamicOffsets;

		// Apply either pipeline state or dynamic state
		executionState.renderer->setScissor(pipeline->hasDynamicState(VK_DYNAMIC_STATE_SCISSOR) ?
		                                    executionState.dynamicState.scissor : pipeline->getScissor());
		executionState.renderer->setViewport(pipeline->hasDynamicState(VK_DYNAMIC_STATE_VIEWPORT) ?
		                                     executionState.dynamicState.viewport : pipeline->getViewport());
		executionState.renderer->setBlendConstant(pipeline->hasDynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS) ?
		                                          executionState.dynamicState.blendConstants : pipeline->getBlendConstants());

		if (pipeline->hasDynamicState(VK_DYNAMIC_STATE_DEPTH_BIAS))
		{
			// If the depth bias clamping feature is not enabled, depthBiasClamp must be 0.0
			ASSERT(executionState.dynamicState.depthBiasClamp == 0.0f);

			context.depthBias = executionState.dynamicState.depthBiasConstantFactor;
			context.slopeDepthBias = executionState.dynamicState.depthBiasSlopeFactor;
		}
		if (pipeline->hasDynamicState(VK_DYNAMIC_STATE_DEPTH_BOUNDS) && context.depthBoundsTestEnable)
		{
			// Unless the VK_EXT_depth_range_unrestricted extension is enabled minDepthBounds and maxDepthBounds must be between 0.0 and 1.0, inclusive
			ASSERT(executionState.dynamicState.minDepthBounds >= 0.0f &&
				   executionState.dynamicState.minDepthBounds <= 1.0f);
			ASSERT(executionState.dynamicState.maxDepthBounds >= 0.0f &&
				   executionState.dynamicState.maxDepthBounds <= 1.0f);

			UNIMPLEMENTED("depthBoundsTestEnable");
		}
		if (pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK) && context.stencilEnable)
		{
			context.frontStencil.compareMask = executionState.dynamicState.compareMask[0];
			context.backStencil.compareMask = executionState.dynamicState.compareMask[1];
		}
		if (pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK) && context.stencilEnable)
		{
			context.frontStencil.writeMask = executionState.dynamicState.writeMask[0];
			context.backStencil.writeMask = executionState.dynamicState.writeMask[1];
		}
		if (pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_REFERENCE) && context.stencilEnable)
		{
			context.frontStencil.reference = executionState.dynamicState.reference[0];
			context.backStencil.reference = executionState.dynamicState.reference[1];
		}

		executionState.bindAttachments(context);

		context.occlusionEnabled = executionState.renderer->hasOcclusionQuery();

		std::vector<std::pair<uint32_t, void *>> indexBuffers;
		if (indexed)
		{
			void *indexBuffer = executionState.indexBufferBinding.buffer->getOffsetPointer(
					executionState.indexBufferBinding.offset + first * bytesPerIndex(executionState));
			if (pipeline->hasPrimitiveRestartEnable())
			{
				switch (executionState.indexType)
				{
				case VK_INDEX_TYPE_UINT16:
					processPrimitiveRestart(static_cast<uint16_t *>(indexBuffer), count, pipeline, indexBuffers);
					break;
				case VK_INDEX_TYPE_UINT32:
					processPrimitiveRestart(static_cast<uint32_t *>(indexBuffer), count, pipeline, indexBuffers);
					break;
				default:
					UNIMPLEMENTED("executionState.indexType %d", int(executionState.indexType));
				}
			}
			else
			{
				indexBuffers.push_back({pipeline->computePrimitiveCount(count), indexBuffer});
			}
		}
		else
		{
			indexBuffers.push_back({pipeline->computePrimitiveCount(count), nullptr});
		}

		for (uint32_t instance = firstInstance; instance != firstInstance + instanceCount; instance++)
		{
			// FIXME: reconsider instances/views nesting.
			auto viewMask = executionState.renderPass->getViewMask(executionState.subpassIndex);
			while (viewMask)
			{
				int viewID = sw::log2i(viewMask);
				viewMask &= ~(1 << viewID);

				for (auto indexBuffer : indexBuffers)
				{
					executionState.renderer->draw(&context, executionState.indexType, indexBuffer.first, vertexOffset,
												  executionState.events, instance, viewID, indexBuffer.second,
												  executionState.renderPassFramebuffer->getExtent(),
												  executionState.pushConstants);
				}
			}

			executionState.renderer->advanceInstanceAttributes(context.input);
		}
	}
};

struct Draw : public DrawBase
{
	Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
		: vertexCount(vertexCount), instanceCount(instanceCount), firstVertex(firstVertex), firstInstance(firstInstance)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		draw(executionState, false, vertexCount, instanceCount, 0, firstVertex, firstInstance);
	}

private:
	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstVertex;
	uint32_t firstInstance;
};

struct DrawIndexed : public DrawBase
{
	DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
			: indexCount(indexCount), instanceCount(instanceCount), firstIndex(firstIndex), vertexOffset(vertexOffset), firstInstance(firstInstance)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		draw(executionState, true, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

private:
	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t vertexOffset;
	uint32_t firstInstance;
};

struct DrawIndirect : public DrawBase
{
	DrawIndirect(Buffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
			: buffer(buffer), offset(offset), drawCount(drawCount), stride(stride)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		for (auto drawId = 0u; drawId < drawCount; drawId++)
		{
			auto cmd = reinterpret_cast<VkDrawIndirectCommand const *>(buffer->getOffsetPointer(offset + drawId * stride));
			draw(executionState, false, cmd->vertexCount, cmd->instanceCount, 0, cmd->firstVertex, cmd->firstInstance);
		}
	}

private:
	const Buffer* buffer;
	VkDeviceSize offset;
	uint32_t drawCount;
	uint32_t stride;
};

struct DrawIndexedIndirect : public DrawBase
{
	DrawIndexedIndirect(Buffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
			: buffer(buffer), offset(offset), drawCount(drawCount), stride(stride)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		for (auto drawId = 0u; drawId < drawCount; drawId++)
		{
			auto cmd = reinterpret_cast<VkDrawIndexedIndirectCommand const *>(buffer->getOffsetPointer(offset + drawId * stride));
			draw(executionState, true, cmd->indexCount, cmd->instanceCount, cmd->firstIndex, cmd->vertexOffset, cmd->firstInstance);
		}
	}

private:
	const Buffer* buffer;
	VkDeviceSize offset;
	uint32_t drawCount;
	uint32_t stride;
};

struct ImageToImageCopy : public CommandBuffer::Command
{
	ImageToImageCopy(const Image* srcImage, Image* dstImage, const VkImageCopy& region) :
		srcImage(srcImage), dstImage(dstImage), region(region)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		srcImage->copyTo(dstImage, region);
	}

private:
	const Image* srcImage;
	Image* dstImage;
	const VkImageCopy region;
};

struct BufferToBufferCopy : public CommandBuffer::Command
{
	BufferToBufferCopy(const Buffer* srcBuffer, Buffer* dstBuffer, const VkBufferCopy& region) :
		srcBuffer(srcBuffer), dstBuffer(dstBuffer), region(region)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		srcBuffer->copyTo(dstBuffer, region);
	}

private:
	const Buffer* srcBuffer;
	Buffer* dstBuffer;
	const VkBufferCopy region;
};

struct ImageToBufferCopy : public CommandBuffer::Command
{
	ImageToBufferCopy(Image* srcImage, Buffer* dstBuffer, const VkBufferImageCopy& region) :
		srcImage(srcImage), dstBuffer(dstBuffer), region(region)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		srcImage->copyTo(dstBuffer, region);
	}

private:
	Image* srcImage;
	Buffer* dstBuffer;
	const VkBufferImageCopy region;
};

struct BufferToImageCopy : public CommandBuffer::Command
{
	BufferToImageCopy(Buffer* srcBuffer, Image* dstImage, const VkBufferImageCopy& region) :
		srcBuffer(srcBuffer), dstImage(dstImage), region(region)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		dstImage->copyFrom(srcBuffer, region);
	}

private:
	Buffer* srcBuffer;
	Image* dstImage;
	const VkBufferImageCopy region;
};

struct FillBuffer : public CommandBuffer::Command
{
	FillBuffer(Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) :
		dstBuffer(dstBuffer), dstOffset(dstOffset), size(size), data(data)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		dstBuffer->fill(dstOffset, size, data);
	}

private:
	Buffer* dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize size;
	uint32_t data;
};

struct UpdateBuffer : public CommandBuffer::Command
{
	UpdateBuffer(Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint8_t* pData) :
		dstBuffer(dstBuffer), dstOffset(dstOffset), data(pData, &pData[dataSize])
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		dstBuffer->update(dstOffset, data.size(), data.data());
	}

private:
	Buffer* dstBuffer;
	VkDeviceSize dstOffset;
	std::vector<uint8_t> data; // FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
};

struct ClearColorImage : public CommandBuffer::Command
{
	ClearColorImage(Image* image, const VkClearColorValue& color, const VkImageSubresourceRange& range) :
		image(image), color(color), range(range)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		image->clear(color, range);
	}

private:
	Image* image;
	const VkClearColorValue color;
	const VkImageSubresourceRange range;
};

struct ClearDepthStencilImage : public CommandBuffer::Command
{
	ClearDepthStencilImage(Image* image, const VkClearDepthStencilValue& depthStencil, const VkImageSubresourceRange& range) :
		image(image), depthStencil(depthStencil), range(range)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		image->clear(depthStencil, range);
	}

private:
	Image* image;
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
		// attachment clears are drawing operations, and so have rasterization-order guarantees.
		// however, we don't do the clear through the rasterizer, so need to ensure prior drawing
		// has completed first.
		executionState.renderer->synchronize();
		executionState.renderPassFramebuffer->clearAttachment(executionState.renderPass, executionState.subpassIndex, attachment, rect);
	}

private:
	const VkClearAttachment attachment;
	const VkClearRect rect;
};

struct BlitImage : public CommandBuffer::Command
{
	BlitImage(const Image* srcImage, Image* dstImage, const VkImageBlit& region, VkFilter filter) :
		srcImage(srcImage), dstImage(dstImage), region(region), filter(filter)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		srcImage->blit(dstImage, region, filter);
	}

private:
	const Image* srcImage;
	Image* dstImage;
	VkImageBlit region;
	VkFilter filter;
};

struct ResolveImage : public CommandBuffer::Command
{
	ResolveImage(const Image* srcImage, Image* dstImage, const VkImageResolve& region) :
		srcImage(srcImage), dstImage(dstImage), region(region)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		srcImage->resolve(dstImage, region);
	}

private:
	const Image* srcImage;
	Image* dstImage;
	VkImageResolve region;
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
	SignalEvent(Event* ev, VkPipelineStageFlags stageMask) : ev(ev), stageMask(stageMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.renderer->synchronize();
		ev->signal();
	}

private:
	Event* ev;
	VkPipelineStageFlags stageMask; // FIXME(b/117835459) : We currently ignore the flags and signal the event at the last stage
};

struct ResetEvent : public CommandBuffer::Command
{
	ResetEvent(Event* ev, VkPipelineStageFlags stageMask) : ev(ev), stageMask(stageMask)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		ev->reset();
	}

private:
	Event* ev;
	VkPipelineStageFlags stageMask; // FIXME(b/117835459) : We currently ignore the flags and reset the event at the last stage
};

struct WaitEvent : public CommandBuffer::Command
{
	WaitEvent(Event* ev) : ev(ev)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState) override
	{
		executionState.renderer->synchronize();
		ev->wait();
	}

private:
	Event* ev;
};

struct BindDescriptorSet : public CommandBuffer::Command
{
	BindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, const PipelineLayout *pipelineLayout, uint32_t set, DescriptorSet* descriptorSet,
		uint32_t dynamicOffsetCount, uint32_t const *dynamicOffsets)
		: pipelineBindPoint(pipelineBindPoint), pipelineLayout(pipelineLayout), set(set), descriptorSet(descriptorSet),
		  dynamicOffsetCount(dynamicOffsetCount)
	{
		for (uint32_t i = 0; i < dynamicOffsetCount; i++)
		{
			this->dynamicOffsets[i] = dynamicOffsets[i];
		}
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		ASSERT_OR_RETURN((pipelineBindPoint < VK_PIPELINE_BIND_POINT_RANGE_SIZE) && (set < MAX_BOUND_DESCRIPTOR_SETS));
		auto &pipelineState = executionState.pipelineState[pipelineBindPoint];
		auto dynamicOffsetBase = pipelineLayout->getDynamicOffsetBase(set);
		ASSERT_OR_RETURN(dynamicOffsetBase + dynamicOffsetCount <= MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC);

		pipelineState.descriptorSets[set] = descriptorSet;
		for (uint32_t i = 0; i < dynamicOffsetCount; i++)
		{
			pipelineState.descriptorDynamicOffsets[dynamicOffsetBase + i] = dynamicOffsets[i];
		}
	}

private:
	VkPipelineBindPoint pipelineBindPoint;
	const PipelineLayout *pipelineLayout;
	uint32_t set;
	vk::DescriptorSet* descriptorSet;
	uint32_t dynamicOffsetCount;
	DescriptorSet::DynamicOffsets dynamicOffsets;
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

struct BeginQuery : public CommandBuffer::Command
{
	BeginQuery(QueryPool* queryPool, uint32_t query, VkQueryControlFlags flags)
		: queryPool(queryPool), query(query), flags(flags)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		queryPool->begin(query, flags);
		executionState.renderer->addQuery(queryPool->getQuery(query));
	}

private:
	QueryPool* queryPool;
	uint32_t query;
	VkQueryControlFlags flags;
};

struct EndQuery : public CommandBuffer::Command
{
	EndQuery(QueryPool* queryPool, uint32_t query)
		: queryPool(queryPool), query(query)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		executionState.renderer->removeQuery(queryPool->getQuery(query));
		queryPool->end(query);
	}

private:
	QueryPool* queryPool;
	uint32_t query;
};

struct ResetQueryPool : public CommandBuffer::Command
{
	ResetQueryPool(QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount)
		: queryPool(queryPool), firstQuery(firstQuery), queryCount(queryCount)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		queryPool->reset(firstQuery, queryCount);
	}

private:
	QueryPool* queryPool;
	uint32_t firstQuery;
	uint32_t queryCount;
};

struct WriteTimeStamp : public CommandBuffer::Command
{
	WriteTimeStamp(QueryPool* queryPool, uint32_t query, VkPipelineStageFlagBits stage)
		: queryPool(queryPool), query(query), stage(stage)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		if (stage & ~(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT))
		{
			// The `top of pipe` and `draw indirect` stages are handled in command buffer processing so a timestamp write
			// done in those stages can just be done here without any additional synchronization.
			// Everything else is deferred to the Renderer; we will treat those stages all as if they were
			// `bottom of pipe`.
			//
			// FIXME(chrisforbes): once Marl is integrated, do this in a task so we don't have to stall here.
			executionState.renderer->synchronize();
		}

		queryPool->writeTimestamp(query);
	}

private:
	QueryPool* queryPool;
	uint32_t query;
	VkPipelineStageFlagBits stage;
};

struct CopyQueryPoolResults : public CommandBuffer::Command
{
	CopyQueryPoolResults(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount,
		Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
		: queryPool(queryPool), firstQuery(firstQuery), queryCount(queryCount),
		  dstBuffer(dstBuffer), dstOffset(dstOffset), stride(stride), flags(flags)
	{
	}

	void play(CommandBuffer::ExecutionState& executionState)
	{
		queryPool->getResults(firstQuery, queryCount, dstBuffer->getSize() - dstOffset,
		                      dstBuffer->getOffsetPointer(dstOffset), stride, flags);
	}

private:
	const QueryPool* queryPool;
	uint32_t firstQuery;
	uint32_t queryCount;
	Buffer* dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize stride;
	VkQueryResultFlags flags;
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
	// FIXME (b/119409619): use an allocator here so we can control all memory allocations
	commands->push_back(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
}

void CommandBuffer::beginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, VkRect2D renderArea,
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
		addCommand<ExecuteCommands>(vk::Cast(pCommandBuffers[i]));
	}
}

void CommandBuffer::setDeviceMask(uint32_t deviceMask)
{
	// SwiftShader only has one device, so we ignore the device mask
}

void CommandBuffer::dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                                 uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	addCommand<Dispatch>(baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    VkDependencyFlags dependencyFlags,
                                    uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                    uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                    uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
	addCommand<PipelineBarrier>();
}

void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, Pipeline* pipeline)
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
		addCommand<VertexBufferBind>(i + firstBinding, vk::Cast(pBuffers[i]), pOffsets[i]);
	}
}

void CommandBuffer::beginQuery(QueryPool* queryPool, uint32_t query, VkQueryControlFlags flags)
{
	addCommand<BeginQuery>(queryPool, query, flags);
}

void CommandBuffer::endQuery(QueryPool* queryPool, uint32_t query)
{
	addCommand<EndQuery>(queryPool, query);
}

void CommandBuffer::resetQueryPool(QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	addCommand<ResetQueryPool>(queryPool, firstQuery, queryCount);
}

void CommandBuffer::writeTimestamp(VkPipelineStageFlagBits pipelineStage, QueryPool* queryPool, uint32_t query)
{
	addCommand<WriteTimeStamp>(queryPool, query, pipelineStage);
}

void CommandBuffer::copyQueryPoolResults(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount,
	Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	addCommand<CopyQueryPoolResults>(queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

void CommandBuffer::pushConstants(PipelineLayout* layout, VkShaderStageFlags stageFlags,
	uint32_t offset, uint32_t size, const void* pValues)
{
	addCommand<SetPushConstants>(offset, size, pValues);
}

void CommandBuffer::setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
	if(firstViewport != 0 || viewportCount > 1)
	{
		UNIMPLEMENTED("viewport");
	}

	for(uint32_t i = 0; i < viewportCount; i++)
	{
		addCommand<SetViewport>(pViewports[i], i + firstViewport);
	}
}

void CommandBuffer::setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
	if(firstScissor != 0 || scissorCount > 1)
	{
		UNIMPLEMENTED("scissor");
	}

	for(uint32_t i = 0; i < scissorCount; i++)
	{
		addCommand<SetScissor>(pScissors[i], i + firstScissor);
	}
}

void CommandBuffer::setLineWidth(float lineWidth)
{
	// If the wide lines feature is not enabled, lineWidth must be 1.0
	ASSERT(lineWidth == 1.0f);
}

void CommandBuffer::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	addCommand<SetDepthBias>(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void CommandBuffer::setBlendConstants(const float blendConstants[4])
{
	addCommand<SetBlendConstants>(blendConstants);
}

void CommandBuffer::setDepthBounds(float minDepthBounds, float maxDepthBounds)
{
	addCommand<SetDepthBounds>(minDepthBounds, maxDepthBounds);
}

void CommandBuffer::setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<SetStencilCompareMask>(faceMask, compareMask);
}

void CommandBuffer::setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<SetStencilWriteMask>(faceMask, writeMask);
}

void CommandBuffer::setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<SetStencilReference>(faceMask, reference);
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, const PipelineLayout* layout,
	uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets,
	uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < descriptorSetCount; i++)
	{
		auto descriptorSetIndex = firstSet + i;
		auto setLayout = layout->getDescriptorSetLayout(descriptorSetIndex);

		auto numDynamicDescriptors = setLayout->getDynamicDescriptorCount();
		ASSERT(numDynamicDescriptors == 0 || pDynamicOffsets != nullptr);
		ASSERT(dynamicOffsetCount >= numDynamicDescriptors);

		addCommand<BindDescriptorSet>(
				pipelineBindPoint, layout, descriptorSetIndex, vk::Cast(pDescriptorSets[i]),
				dynamicOffsetCount, pDynamicOffsets);

		pDynamicOffsets += numDynamicDescriptors;
		dynamicOffsetCount -= numDynamicDescriptors;
	}
}

void CommandBuffer::bindIndexBuffer(Buffer* buffer, VkDeviceSize offset, VkIndexType indexType)
{
	addCommand<IndexBufferBind>(buffer, offset, indexType);
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	addCommand<Dispatch>(0, 0, 0, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::dispatchIndirect(Buffer* buffer, VkDeviceSize offset)
{
	addCommand<DispatchIndirect>(buffer, offset);
}

void CommandBuffer::copyBuffer(const Buffer* srcBuffer, Buffer* dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<BufferToBufferCopy>(srcBuffer, dstBuffer, pRegions[i]);
	}
}

void CommandBuffer::copyImage(const Image* srcImage, VkImageLayout srcImageLayout, Image* dstImage, VkImageLayout dstImageLayout,
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

void CommandBuffer::blitImage(const Image* srcImage, VkImageLayout srcImageLayout, Image* dstImage, VkImageLayout dstImageLayout,
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

void CommandBuffer::copyBufferToImage(Buffer* srcBuffer, Image* dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<BufferToImageCopy>(srcBuffer, dstImage, pRegions[i]);
	}
}

void CommandBuffer::copyImageToBuffer(Image* srcImage, VkImageLayout srcImageLayout, Buffer* dstBuffer,
	uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL || srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<ImageToBufferCopy>(srcImage, dstBuffer, pRegions[i]);
	}
}

void CommandBuffer::updateBuffer(Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
	ASSERT(state == RECORDING);

	addCommand<UpdateBuffer>(dstBuffer, dstOffset, dataSize, reinterpret_cast<const uint8_t*>(pData));
}

void CommandBuffer::fillBuffer(Buffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	ASSERT(state == RECORDING);

	addCommand<FillBuffer>(dstBuffer, dstOffset, size, data);
}

void CommandBuffer::clearColorImage(Image* image, VkImageLayout imageLayout, const VkClearColorValue* pColor,
	uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<ClearColorImage>(image, *pColor, pRanges[i]);
	}
}

void CommandBuffer::clearDepthStencilImage(Image* image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil,
	uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<ClearDepthStencilImage>(image, *pDepthStencil, pRanges[i]);
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

void CommandBuffer::resolveImage(const Image* srcImage, VkImageLayout srcImageLayout, Image* dstImage, VkImageLayout dstImageLayout,
	uint32_t regionCount, const VkImageResolve* pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<ResolveImage>(srcImage, dstImage, pRegions[i]);
	}
}

void CommandBuffer::setEvent(Event* event, VkPipelineStageFlags stageMask)
{
	ASSERT(state == RECORDING);

	addCommand<SignalEvent>(event, stageMask);
}

void CommandBuffer::resetEvent(Event* event, VkPipelineStageFlags stageMask)
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
		addCommand<WaitEvent>(vk::Cast(pEvents[i]));
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

void CommandBuffer::drawIndirect(Buffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	addCommand<DrawIndirect>(buffer, offset, drawCount, stride);
}

void CommandBuffer::drawIndexedIndirect(Buffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	addCommand<DrawIndexedIndirect>(buffer, offset, drawCount, stride);
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
