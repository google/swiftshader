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

#include "./Debug/Context.hpp"
#include "./Debug/File.hpp"
#include "./Debug/Thread.hpp"

#include "marl/defer.h"

#include <cstring>

class vk::CommandBuffer::Command
{
public:
	// FIXME (b/119421344): change the commandBuffer argument to a CommandBuffer state
	virtual void play(vk::CommandBuffer::ExecutionState &executionState) = 0;
	virtual std::string description() = 0;
	virtual ~Command() {}
};

namespace {

class CmdBeginRenderPass : public vk::CommandBuffer::Command
{
public:
	CmdBeginRenderPass(vk::RenderPass *renderPass, vk::Framebuffer *framebuffer, VkRect2D renderArea,
	                   uint32_t clearValueCount, const VkClearValue *pClearValues)
	    : renderPass(renderPass)
	    , framebuffer(framebuffer)
	    , renderArea(renderArea)
	    , clearValueCount(clearValueCount)
	{
		// FIXME (b/119409619): use an allocator here so we can control all memory allocations
		clearValues = new VkClearValue[clearValueCount];
		memcpy(clearValues, pClearValues, clearValueCount * sizeof(VkClearValue));
	}

	~CmdBeginRenderPass() override
	{
		delete[] clearValues;
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.renderPass = renderPass;
		executionState.renderPassFramebuffer = framebuffer;
		executionState.subpassIndex = 0;
		framebuffer->clear(executionState.renderPass, clearValueCount, clearValues, renderArea);
	}

	std::string description() override { return "vkCmdBeginRenderPass()"; }

private:
	vk::RenderPass *renderPass;
	vk::Framebuffer *framebuffer;
	VkRect2D renderArea;
	uint32_t clearValueCount;
	VkClearValue *clearValues;
};

class CmdNextSubpass : public vk::CommandBuffer::Command
{
public:
	void play(vk::CommandBuffer::ExecutionState &executionState) override
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

	std::string description() override { return "vkCmdNextSubpass()"; }
};

class CmdEndRenderPass : public vk::CommandBuffer::Command
{
public:
	void play(vk::CommandBuffer::ExecutionState &executionState) override
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

	std::string description() override { return "vkCmdEndRenderPass()"; }
};

class CmdExecuteCommands : public vk::CommandBuffer::Command
{
public:
	CmdExecuteCommands(const vk::CommandBuffer *commandBuffer)
	    : commandBuffer(commandBuffer)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		commandBuffer->submitSecondary(executionState);
	}

	std::string description() override { return "vkCmdExecuteCommands()"; }

private:
	const vk::CommandBuffer *commandBuffer;
};

class CmdPipelineBind : public vk::CommandBuffer::Command
{
public:
	CmdPipelineBind(VkPipelineBindPoint pipelineBindPoint, vk::Pipeline *pipeline)
	    : pipelineBindPoint(pipelineBindPoint)
	    , pipeline(pipeline)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.pipelineState[pipelineBindPoint].pipeline = pipeline;
	}

	std::string description() override { return "vkCmdPipelineBind()"; }

private:
	VkPipelineBindPoint pipelineBindPoint;
	vk::Pipeline *pipeline;
};

class CmdDispatch : public vk::CommandBuffer::Command
{
public:
	CmdDispatch(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	    : baseGroupX(baseGroupX)
	    , baseGroupY(baseGroupY)
	    , baseGroupZ(baseGroupZ)
	    , groupCountX(groupCountX)
	    , groupCountY(groupCountY)
	    , groupCountZ(groupCountZ)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_COMPUTE];

		vk::ComputePipeline *pipeline = static_cast<vk::ComputePipeline *>(pipelineState.pipeline);
		pipeline->run(baseGroupX, baseGroupY, baseGroupZ,
		              groupCountX, groupCountY, groupCountZ,
		              pipelineState.descriptorSets,
		              pipelineState.descriptorDynamicOffsets,
		              executionState.pushConstants);
	}

	std::string description() override { return "vkCmdDispatch()"; }

private:
	uint32_t baseGroupX;
	uint32_t baseGroupY;
	uint32_t baseGroupZ;
	uint32_t groupCountX;
	uint32_t groupCountY;
	uint32_t groupCountZ;
};

class CmdDispatchIndirect : public vk::CommandBuffer::Command
{
public:
	CmdDispatchIndirect(vk::Buffer *buffer, VkDeviceSize offset)
	    : buffer(buffer)
	    , offset(offset)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		auto cmd = reinterpret_cast<VkDispatchIndirectCommand const *>(buffer->getOffsetPointer(offset));

		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_COMPUTE];

		auto pipeline = static_cast<vk::ComputePipeline *>(pipelineState.pipeline);
		pipeline->run(0, 0, 0, cmd->x, cmd->y, cmd->z,
		              pipelineState.descriptorSets,
		              pipelineState.descriptorDynamicOffsets,
		              executionState.pushConstants);
	}

	std::string description() override { return "vkCmdDispatchIndirect()"; }

private:
	const vk::Buffer *buffer;
	VkDeviceSize offset;
};

class CmdVertexBufferBind : public vk::CommandBuffer::Command
{
public:
	CmdVertexBufferBind(uint32_t binding, vk::Buffer *buffer, const VkDeviceSize offset)
	    : binding(binding)
	    , buffer(buffer)
	    , offset(offset)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.vertexInputBindings[binding] = { buffer, offset };
	}

	std::string description() override { return "vkCmdVertexBufferBind()"; }

private:
	uint32_t binding;
	vk::Buffer *buffer;
	const VkDeviceSize offset;
};

class CmdIndexBufferBind : public vk::CommandBuffer::Command
{
public:
	CmdIndexBufferBind(vk::Buffer *buffer, const VkDeviceSize offset, const VkIndexType indexType)
	    : buffer(buffer)
	    , offset(offset)
	    , indexType(indexType)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.indexBufferBinding = { buffer, offset };
		executionState.indexType = indexType;
	}

	std::string description() override { return "vkCmdIndexBufferBind()"; }

private:
	vk::Buffer *buffer;
	const VkDeviceSize offset;
	const VkIndexType indexType;
};

class CmdSetViewport : public vk::CommandBuffer::Command
{
public:
	CmdSetViewport(const VkViewport &viewport, uint32_t viewportID)
	    : viewport(viewport)
	    , viewportID(viewportID)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.dynamicState.viewport = viewport;
	}

	std::string description() override { return "vkCmdSetViewport()"; }

private:
	const VkViewport viewport;
	uint32_t viewportID;
};

class CmdSetScissor : public vk::CommandBuffer::Command
{
public:
	CmdSetScissor(const VkRect2D &scissor, uint32_t scissorID)
	    : scissor(scissor)
	    , scissorID(scissorID)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.dynamicState.scissor = scissor;
	}

	std::string description() override { return "vkCmdSetScissor()"; }

private:
	const VkRect2D scissor;
	uint32_t scissorID;
};

class CmdSetDepthBias : public vk::CommandBuffer::Command
{
public:
	CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
	    : depthBiasConstantFactor(depthBiasConstantFactor)
	    , depthBiasClamp(depthBiasClamp)
	    , depthBiasSlopeFactor(depthBiasSlopeFactor)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.dynamicState.depthBiasConstantFactor = depthBiasConstantFactor;
		executionState.dynamicState.depthBiasClamp = depthBiasClamp;
		executionState.dynamicState.depthBiasSlopeFactor = depthBiasSlopeFactor;
	}

	std::string description() override { return "vkCmdSetDepthBias()"; }

private:
	float depthBiasConstantFactor;
	float depthBiasClamp;
	float depthBiasSlopeFactor;
};

class CmdSetBlendConstants : public vk::CommandBuffer::Command
{
public:
	CmdSetBlendConstants(const float blendConstants[4])
	{
		memcpy(this->blendConstants, blendConstants, sizeof(this->blendConstants));
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		memcpy(&(executionState.dynamicState.blendConstants[0]), blendConstants, sizeof(blendConstants));
	}

	std::string description() override { return "vkCmdSetBlendConstants()"; }

private:
	float blendConstants[4];
};

class CmdSetDepthBounds : public vk::CommandBuffer::Command
{
public:
	CmdSetDepthBounds(float minDepthBounds, float maxDepthBounds)
	    : minDepthBounds(minDepthBounds)
	    , maxDepthBounds(maxDepthBounds)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.dynamicState.minDepthBounds = minDepthBounds;
		executionState.dynamicState.maxDepthBounds = maxDepthBounds;
	}

	std::string description() override { return "vkCmdSetDepthBounds()"; }

private:
	float minDepthBounds;
	float maxDepthBounds;
};

class CmdSetStencilCompareMask : public vk::CommandBuffer::Command
{
public:
	CmdSetStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
	    : faceMask(faceMask)
	    , compareMask(compareMask)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
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

	std::string description() override { return "vkCmdSetStencilCompareMask()"; }

private:
	VkStencilFaceFlags faceMask;
	uint32_t compareMask;
};

class CmdSetStencilWriteMask : public vk::CommandBuffer::Command
{
public:
	CmdSetStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
	    : faceMask(faceMask)
	    , writeMask(writeMask)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
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

	std::string description() override { return "vkCmdSetStencilWriteMask()"; }

private:
	VkStencilFaceFlags faceMask;
	uint32_t writeMask;
};

class CmdSetStencilReference : public vk::CommandBuffer::Command
{
public:
	CmdSetStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
	    : faceMask(faceMask)
	    , reference(reference)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
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

	std::string description() override { return "vkCmdSetStencilReference()"; }

private:
	VkStencilFaceFlags faceMask;
	uint32_t reference;
};

class CmdDrawBase : public vk::CommandBuffer::Command
{
public:
	int bytesPerIndex(vk::CommandBuffer::ExecutionState const &executionState)
	{
		return executionState.indexType == VK_INDEX_TYPE_UINT16 ? 2 : 4;
	}

	template<typename T>
	void processPrimitiveRestart(T *indexBuffer,
	                             uint32_t count,
	                             vk::GraphicsPipeline *pipeline,
	                             std::vector<std::pair<uint32_t, void *>> &indexBuffers)
	{
		static const T RestartIndex = static_cast<T>(-1);
		T *indexBufferStart = indexBuffer;
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

	void draw(vk::CommandBuffer::ExecutionState &executionState, bool indexed,
	          uint32_t count, uint32_t instanceCount, uint32_t first, int32_t vertexOffset, uint32_t firstInstance)
	{
		auto const &pipelineState = executionState.pipelineState[VK_PIPELINE_BIND_POINT_GRAPHICS];

		auto *pipeline = static_cast<vk::GraphicsPipeline *>(pipelineState.pipeline);

		sw::Context context = pipeline->getContext();

		executionState.bindVertexInputs(context, firstInstance);

		context.descriptorSets = pipelineState.descriptorSets;
		context.descriptorDynamicOffsets = pipelineState.descriptorDynamicOffsets;

		// Apply either pipeline state or dynamic state
		executionState.renderer->setScissor(pipeline->hasDynamicState(VK_DYNAMIC_STATE_SCISSOR) ? executionState.dynamicState.scissor : pipeline->getScissor());
		executionState.renderer->setViewport(pipeline->hasDynamicState(VK_DYNAMIC_STATE_VIEWPORT) ? executionState.dynamicState.viewport : pipeline->getViewport());
		executionState.renderer->setBlendConstant(pipeline->hasDynamicState(VK_DYNAMIC_STATE_BLEND_CONSTANTS) ? executionState.dynamicState.blendConstants : pipeline->getBlendConstants());

		if(pipeline->hasDynamicState(VK_DYNAMIC_STATE_DEPTH_BIAS))
		{
			// If the depth bias clamping feature is not enabled, depthBiasClamp must be 0.0
			ASSERT(executionState.dynamicState.depthBiasClamp == 0.0f);

			context.depthBias = executionState.dynamicState.depthBiasConstantFactor;
			context.slopeDepthBias = executionState.dynamicState.depthBiasSlopeFactor;
		}

		if(pipeline->hasDynamicState(VK_DYNAMIC_STATE_DEPTH_BOUNDS) && context.depthBoundsTestEnable)
		{
			// Unless the VK_EXT_depth_range_unrestricted extension is enabled, minDepthBounds and maxDepthBounds must be between 0.0 and 1.0, inclusive
			ASSERT(executionState.dynamicState.minDepthBounds >= 0.0f &&
			       executionState.dynamicState.minDepthBounds <= 1.0f);
			ASSERT(executionState.dynamicState.maxDepthBounds >= 0.0f &&
			       executionState.dynamicState.maxDepthBounds <= 1.0f);

			UNSUPPORTED("VkPhysicalDeviceFeatures::depthBounds");
		}

		if(pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK) && context.stencilEnable)
		{
			context.frontStencil.compareMask = executionState.dynamicState.compareMask[0];
			context.backStencil.compareMask = executionState.dynamicState.compareMask[1];
		}

		if(pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK) && context.stencilEnable)
		{
			context.frontStencil.writeMask = executionState.dynamicState.writeMask[0];
			context.backStencil.writeMask = executionState.dynamicState.writeMask[1];
		}

		if(pipeline->hasDynamicState(VK_DYNAMIC_STATE_STENCIL_REFERENCE) && context.stencilEnable)
		{
			context.frontStencil.reference = executionState.dynamicState.reference[0];
			context.backStencil.reference = executionState.dynamicState.reference[1];
		}

		executionState.bindAttachments(context);

		context.occlusionEnabled = executionState.renderer->hasOcclusionQuery();

		std::vector<std::pair<uint32_t, void *>> indexBuffers;
		if(indexed)
		{
			void *indexBuffer = executionState.indexBufferBinding.buffer->getOffsetPointer(
			    executionState.indexBufferBinding.offset + first * bytesPerIndex(executionState));
			if(pipeline->hasPrimitiveRestartEnable())
			{
				switch(executionState.indexType)
				{
					case VK_INDEX_TYPE_UINT16:
						processPrimitiveRestart(static_cast<uint16_t *>(indexBuffer), count, pipeline, indexBuffers);
						break;
					case VK_INDEX_TYPE_UINT32:
						processPrimitiveRestart(static_cast<uint32_t *>(indexBuffer), count, pipeline, indexBuffers);
						break;
					default:
						UNSUPPORTED("VkIndexType %d", int(executionState.indexType));
				}
			}
			else
			{
				indexBuffers.push_back({ pipeline->computePrimitiveCount(count), indexBuffer });
			}
		}
		else
		{
			indexBuffers.push_back({ pipeline->computePrimitiveCount(count), nullptr });
		}

		for(uint32_t instance = firstInstance; instance != firstInstance + instanceCount; instance++)
		{
			// FIXME: reconsider instances/views nesting.
			auto viewMask = executionState.renderPass->getViewMask(executionState.subpassIndex);
			while(viewMask)
			{
				int viewID = sw::log2i(viewMask);
				viewMask &= ~(1 << viewID);

				for(auto indexBuffer : indexBuffers)
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

class CmdDraw : public CmdDrawBase
{
public:
	CmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	    : vertexCount(vertexCount)
	    , instanceCount(instanceCount)
	    , firstVertex(firstVertex)
	    , firstInstance(firstInstance)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		draw(executionState, false, vertexCount, instanceCount, 0, firstVertex, firstInstance);
	}

	std::string description() override { return "vkCmdDraw()"; }

private:
	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstVertex;
	uint32_t firstInstance;
};

class CmdDrawIndexed : public CmdDrawBase
{
public:
	CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	    : indexCount(indexCount)
	    , instanceCount(instanceCount)
	    , firstIndex(firstIndex)
	    , vertexOffset(vertexOffset)
	    , firstInstance(firstInstance)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		draw(executionState, true, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	std::string description() override { return "vkCmdDrawIndexed()"; }

private:
	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t vertexOffset;
	uint32_t firstInstance;
};

class CmdDrawIndirect : public CmdDrawBase
{
public:
	CmdDrawIndirect(vk::Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
	    : buffer(buffer)
	    , offset(offset)
	    , drawCount(drawCount)
	    , stride(stride)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		for(auto drawId = 0u; drawId < drawCount; drawId++)
		{
			auto cmd = reinterpret_cast<VkDrawIndirectCommand const *>(buffer->getOffsetPointer(offset + drawId * stride));
			draw(executionState, false, cmd->vertexCount, cmd->instanceCount, 0, cmd->firstVertex, cmd->firstInstance);
		}
	}

	std::string description() override { return "vkCmdDrawIndirect()"; }

private:
	const vk::Buffer *buffer;
	VkDeviceSize offset;
	uint32_t drawCount;
	uint32_t stride;
};

class CmdDrawIndexedIndirect : public CmdDrawBase
{
public:
	CmdDrawIndexedIndirect(vk::Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
	    : buffer(buffer)
	    , offset(offset)
	    , drawCount(drawCount)
	    , stride(stride)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		for(auto drawId = 0u; drawId < drawCount; drawId++)
		{
			auto cmd = reinterpret_cast<VkDrawIndexedIndirectCommand const *>(buffer->getOffsetPointer(offset + drawId * stride));
			draw(executionState, true, cmd->indexCount, cmd->instanceCount, cmd->firstIndex, cmd->vertexOffset, cmd->firstInstance);
		}
	}

	std::string description() override { return "vkCmdDrawIndexedIndirect()"; }

private:
	const vk::Buffer *buffer;
	VkDeviceSize offset;
	uint32_t drawCount;
	uint32_t stride;
};

class CmdImageToImageCopy : public vk::CommandBuffer::Command
{
public:
	CmdImageToImageCopy(const vk::Image *srcImage, vk::Image *dstImage, const VkImageCopy &region)
	    : srcImage(srcImage)
	    , dstImage(dstImage)
	    , region(region)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		srcImage->copyTo(dstImage, region);
	}

	std::string description() override { return "vkCmdImageToImageCopy()"; }

private:
	const vk::Image *srcImage;
	vk::Image *dstImage;
	const VkImageCopy region;
};

class CmdBufferToBufferCopy : public vk::CommandBuffer::Command
{
public:
	CmdBufferToBufferCopy(const vk::Buffer *srcBuffer, vk::Buffer *dstBuffer, const VkBufferCopy &region)
	    : srcBuffer(srcBuffer)
	    , dstBuffer(dstBuffer)
	    , region(region)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		srcBuffer->copyTo(dstBuffer, region);
	}

	std::string description() override { return "vkCmdBufferToBufferCopy()"; }

private:
	const vk::Buffer *srcBuffer;
	vk::Buffer *dstBuffer;
	const VkBufferCopy region;
};

class CmdImageToBufferCopy : public vk::CommandBuffer::Command
{
public:
	CmdImageToBufferCopy(vk::Image *srcImage, vk::Buffer *dstBuffer, const VkBufferImageCopy &region)
	    : srcImage(srcImage)
	    , dstBuffer(dstBuffer)
	    , region(region)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		srcImage->copyTo(dstBuffer, region);
	}

	std::string description() override { return "vkCmdImageToBufferCopy()"; }

private:
	vk::Image *srcImage;
	vk::Buffer *dstBuffer;
	const VkBufferImageCopy region;
};

class CmdBufferToImageCopy : public vk::CommandBuffer::Command
{
public:
	CmdBufferToImageCopy(vk::Buffer *srcBuffer, vk::Image *dstImage, const VkBufferImageCopy &region)
	    : srcBuffer(srcBuffer)
	    , dstImage(dstImage)
	    , region(region)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		dstImage->copyFrom(srcBuffer, region);
	}

	std::string description() override { return "vkCmdBufferToImageCopy()"; }

private:
	vk::Buffer *srcBuffer;
	vk::Image *dstImage;
	const VkBufferImageCopy region;
};

class CmdFillBuffer : public vk::CommandBuffer::Command
{
public:
	CmdFillBuffer(vk::Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
	    : dstBuffer(dstBuffer)
	    , dstOffset(dstOffset)
	    , size(size)
	    , data(data)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		dstBuffer->fill(dstOffset, size, data);
	}

	std::string description() override { return "vkCmdFillBuffer()"; }

private:
	vk::Buffer *dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize size;
	uint32_t data;
};

class CmdUpdateBuffer : public vk::CommandBuffer::Command
{
public:
	CmdUpdateBuffer(vk::Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint8_t *pData)
	    : dstBuffer(dstBuffer)
	    , dstOffset(dstOffset)
	    , data(pData, &pData[dataSize])
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		dstBuffer->update(dstOffset, data.size(), data.data());
	}

	std::string description() override { return "vkCmdUpdateBuffer()"; }

private:
	vk::Buffer *dstBuffer;
	VkDeviceSize dstOffset;
	std::vector<uint8_t> data;  // FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
};

class CmdClearColorImage : public vk::CommandBuffer::Command
{
public:
	CmdClearColorImage(vk::Image *image, const VkClearColorValue &color, const VkImageSubresourceRange &range)
	    : image(image)
	    , color(color)
	    , range(range)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		image->clear(color, range);
	}

	std::string description() override { return "vkCmdClearColorImage()"; }

private:
	vk::Image *image;
	const VkClearColorValue color;
	const VkImageSubresourceRange range;
};

class CmdClearDepthStencilImage : public vk::CommandBuffer::Command
{
public:
	CmdClearDepthStencilImage(vk::Image *image, const VkClearDepthStencilValue &depthStencil, const VkImageSubresourceRange &range)
	    : image(image)
	    , depthStencil(depthStencil)
	    , range(range)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		image->clear(depthStencil, range);
	}

	std::string description() override { return "vkCmdClearDepthStencilImage()"; }

private:
	vk::Image *image;
	const VkClearDepthStencilValue depthStencil;
	const VkImageSubresourceRange range;
};

class CmdClearAttachment : public vk::CommandBuffer::Command
{
public:
	CmdClearAttachment(const VkClearAttachment &attachment, const VkClearRect &rect)
	    : attachment(attachment)
	    , rect(rect)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		// attachment clears are drawing operations, and so have rasterization-order guarantees.
		// however, we don't do the clear through the rasterizer, so need to ensure prior drawing
		// has completed first.
		executionState.renderer->synchronize();
		executionState.renderPassFramebuffer->clearAttachment(executionState.renderPass, executionState.subpassIndex, attachment, rect);
	}

	std::string description() override { return "vkCmdClearAttachment()"; }

private:
	const VkClearAttachment attachment;
	const VkClearRect rect;
};

class CmdBlitImage : public vk::CommandBuffer::Command
{
public:
	CmdBlitImage(const vk::Image *srcImage, vk::Image *dstImage, const VkImageBlit &region, VkFilter filter)
	    : srcImage(srcImage)
	    , dstImage(dstImage)
	    , region(region)
	    , filter(filter)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		srcImage->blit(dstImage, region, filter);
	}

	std::string description() override { return "vkCmdBlitImage()"; }

private:
	const vk::Image *srcImage;
	vk::Image *dstImage;
	VkImageBlit region;
	VkFilter filter;
};

class CmdResolveImage : public vk::CommandBuffer::Command
{
public:
	CmdResolveImage(const vk::Image *srcImage, vk::Image *dstImage, const VkImageResolve &region)
	    : srcImage(srcImage)
	    , dstImage(dstImage)
	    , region(region)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		srcImage->resolve(dstImage, region);
	}

	std::string description() override { return "vkCmdBlitImage()"; }

private:
	const vk::Image *srcImage;
	vk::Image *dstImage;
	VkImageResolve region;
};

class CmdPipelineBarrier : public vk::CommandBuffer::Command
{
public:
	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		// This is a very simple implementation that simply calls sw::Renderer::synchronize(),
		// since the driver is free to move the source stage towards the bottom of the pipe
		// and the target stage towards the top, so a full pipeline sync is spec compliant.
		executionState.renderer->synchronize();

		// Right now all buffers are read-only in drawcalls but a similar mechanism will be required once we support SSBOs.

		// Also note that this would be a good moment to update cube map borders or decompress compressed textures, if necessary.
	}

	std::string description() override { return "vkCmdPipelineBarrier()"; }
};

class CmdSignalEvent : public vk::CommandBuffer::Command
{
public:
	CmdSignalEvent(vk::Event *ev, VkPipelineStageFlags stageMask)
	    : ev(ev)
	    , stageMask(stageMask)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.renderer->synchronize();
		ev->signal();
	}

	std::string description() override { return "vkCmdSignalEvent()"; }

private:
	vk::Event *ev;
	VkPipelineStageFlags stageMask;  // FIXME(b/117835459) : We currently ignore the flags and signal the event at the last stage
};

class CmdResetEvent : public vk::CommandBuffer::Command
{
public:
	CmdResetEvent(vk::Event *ev, VkPipelineStageFlags stageMask)
	    : ev(ev)
	    , stageMask(stageMask)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		ev->reset();
	}

	std::string description() override { return "vkCmdResetEvent()"; }

private:
	vk::Event *ev;
	VkPipelineStageFlags stageMask;  // FIXME(b/117835459) : We currently ignore the flags and reset the event at the last stage
};

class CmdWaitEvent : public vk::CommandBuffer::Command
{
public:
	CmdWaitEvent(vk::Event *ev)
	    : ev(ev)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.renderer->synchronize();
		ev->wait();
	}

	std::string description() override { return "vkCmdWaitEvent()"; }

private:
	vk::Event *ev;
};

class CmdBindDescriptorSet : public vk::CommandBuffer::Command
{
public:
	CmdBindDescriptorSet(VkPipelineBindPoint pipelineBindPoint, const vk::PipelineLayout *pipelineLayout, uint32_t set, vk::DescriptorSet *descriptorSet,
	                     uint32_t dynamicOffsetCount, uint32_t const *dynamicOffsets)
	    : pipelineBindPoint(pipelineBindPoint)
	    , pipelineLayout(pipelineLayout)
	    , set(set)
	    , descriptorSet(descriptorSet)
	    , dynamicOffsetCount(dynamicOffsetCount)
	{
		for(uint32_t i = 0; i < dynamicOffsetCount; i++)
		{
			this->dynamicOffsets[i] = dynamicOffsets[i];
		}
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		ASSERT_OR_RETURN((pipelineBindPoint < VK_PIPELINE_BIND_POINT_RANGE_SIZE) && (set < vk::MAX_BOUND_DESCRIPTOR_SETS));
		auto &pipelineState = executionState.pipelineState[pipelineBindPoint];
		auto dynamicOffsetBase = pipelineLayout->getDynamicOffsetBase(set);
		ASSERT_OR_RETURN(dynamicOffsetBase + dynamicOffsetCount <= vk::MAX_DESCRIPTOR_SET_COMBINED_BUFFERS_DYNAMIC);

		pipelineState.descriptorSets[set] = descriptorSet;
		for(uint32_t i = 0; i < dynamicOffsetCount; i++)
		{
			pipelineState.descriptorDynamicOffsets[dynamicOffsetBase + i] = dynamicOffsets[i];
		}
	}

	std::string description() override { return "vkCmdBindDescriptorSet()"; }

private:
	VkPipelineBindPoint pipelineBindPoint;
	const vk::PipelineLayout *pipelineLayout;
	uint32_t set;
	vk::DescriptorSet *descriptorSet;
	uint32_t dynamicOffsetCount;
	vk::DescriptorSet::DynamicOffsets dynamicOffsets;
};

class CmdSetPushConstants : public vk::CommandBuffer::Command
{
public:
	CmdSetPushConstants(uint32_t offset, uint32_t size, void const *pValues)
	    : offset(offset)
	    , size(size)
	{
		ASSERT(offset < vk::MAX_PUSH_CONSTANT_SIZE);
		ASSERT(offset + size <= vk::MAX_PUSH_CONSTANT_SIZE);

		memcpy(data, pValues, size);
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		memcpy(&executionState.pushConstants.data[offset], data, size);
	}

	std::string description() override { return "vkCmdSetPushConstants()"; }

private:
	uint32_t offset;
	uint32_t size;
	unsigned char data[vk::MAX_PUSH_CONSTANT_SIZE];
};

class CmdBeginQuery : public vk::CommandBuffer::Command
{
public:
	CmdBeginQuery(vk::QueryPool *queryPool, uint32_t query, VkQueryControlFlags flags)
	    : queryPool(queryPool)
	    , query(query)
	    , flags(flags)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		queryPool->begin(query, flags);
		executionState.renderer->addQuery(queryPool->getQuery(query));
	}

	std::string description() override { return "vkCmdBeginQuery()"; }

private:
	vk::QueryPool *queryPool;
	uint32_t query;
	VkQueryControlFlags flags;
};

class CmdEndQuery : public vk::CommandBuffer::Command
{
public:
	CmdEndQuery(vk::QueryPool *queryPool, uint32_t query)
	    : queryPool(queryPool)
	    , query(query)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		executionState.renderer->removeQuery(queryPool->getQuery(query));
		queryPool->end(query);
	}

	std::string description() override { return "vkCmdEndQuery()"; }

private:
	vk::QueryPool *queryPool;
	uint32_t query;
};

class CmdResetQueryPool : public vk::CommandBuffer::Command
{
public:
	CmdResetQueryPool(vk::QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount)
	    : queryPool(queryPool)
	    , firstQuery(firstQuery)
	    , queryCount(queryCount)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		queryPool->reset(firstQuery, queryCount);
	}

	std::string description() override { return "vkCmdResetQueryPool()"; }

private:
	vk::QueryPool *queryPool;
	uint32_t firstQuery;
	uint32_t queryCount;
};

class CmdWriteTimeStamp : public vk::CommandBuffer::Command
{
public:
	CmdWriteTimeStamp(vk::QueryPool *queryPool, uint32_t query, VkPipelineStageFlagBits stage)
	    : queryPool(queryPool)
	    , query(query)
	    , stage(stage)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		if(stage & ~(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT))
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

	std::string description() override { return "vkCmdWriteTimeStamp()"; }

private:
	vk::QueryPool *queryPool;
	uint32_t query;
	VkPipelineStageFlagBits stage;
};

class CmdCopyQueryPoolResults : public vk::CommandBuffer::Command
{
public:
	CmdCopyQueryPoolResults(const vk::QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount,
	                        vk::Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
	    : queryPool(queryPool)
	    , firstQuery(firstQuery)
	    , queryCount(queryCount)
	    , dstBuffer(dstBuffer)
	    , dstOffset(dstOffset)
	    , stride(stride)
	    , flags(flags)
	{
	}

	void play(vk::CommandBuffer::ExecutionState &executionState) override
	{
		queryPool->getResults(firstQuery, queryCount, dstBuffer->getSize() - dstOffset,
		                      dstBuffer->getOffsetPointer(dstOffset), stride, flags);
	}

	std::string description() override { return "vkCmdCopyQueryPoolResults()"; }

private:
	const vk::QueryPool *queryPool;
	uint32_t firstQuery;
	uint32_t queryCount;
	vk::Buffer *dstBuffer;
	VkDeviceSize dstOffset;
	VkDeviceSize stride;
	VkQueryResultFlags flags;
};

}  // anonymous namespace

namespace vk {

CommandBuffer::CommandBuffer(Device *device, VkCommandBufferLevel pLevel)
    : device(device)
    , level(pLevel)
{
	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	commands = new std::vector<std::unique_ptr<Command>>();
}

void CommandBuffer::destroy(const VkAllocationCallbacks *pAllocator)
{
	delete commands;
}

void CommandBuffer::resetState()
{
	// FIXME (b/119409619): replace this vector by an allocator so we can control all memory allocations
	commands->clear();

	state = INITIAL;
}

VkResult CommandBuffer::begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo *pInheritanceInfo)
{
	ASSERT((state != RECORDING) && (state != PENDING));

	// Nothing interesting to do based on flags. We don't have any optimizations
	// to apply for ONE_TIME_SUBMIT or (lack of) SIMULTANEOUS_USE. RENDER_PASS_CONTINUE
	// must also provide a non-null pInheritanceInfo, which we don't implement yet, but is caught below.
	(void)flags;

	// pInheritanceInfo merely contains optimization hints, so we currently ignore it

	// "pInheritanceInfo is a pointer to a VkCommandBufferInheritanceInfo structure, used if commandBuffer is a
	//  secondary command buffer. If this is a primary command buffer, then this value is ignored."
	if(level == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
	{
		if(pInheritanceInfo->queryFlags != 0)
		{
			// "If the inherited queries feature is not enabled, queryFlags must be 0"
			UNSUPPORTED("VkPhysicalDeviceFeatures::inheritedQueries");
		}
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

#ifdef ENABLE_VK_DEBUGGER
	auto debuggerContext = device->getDebuggerContext();
	if(debuggerContext)
	{
		std::string source;
		for(auto &command : *commands)
		{
			source += command->description() + "\n";
		}
		debuggerFile = debuggerContext->lock().createVirtualFile("VkCommandBuffer", source.c_str());
	}
#endif  // ENABLE_VK_DEBUGGER

	return VK_SUCCESS;
}

VkResult CommandBuffer::reset(VkCommandPoolResetFlags flags)
{
	ASSERT(state != PENDING);

	resetState();

	return VK_SUCCESS;
}

template<typename T, typename... Args>
void CommandBuffer::addCommand(Args &&... args)
{
	// FIXME (b/119409619): use an allocator here so we can control all memory allocations
	commands->push_back(std::make_unique<T>(std::forward<Args>(args)...));
}

void CommandBuffer::beginRenderPass(RenderPass *renderPass, Framebuffer *framebuffer, VkRect2D renderArea,
                                    uint32_t clearValueCount, const VkClearValue *clearValues, VkSubpassContents contents)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdBeginRenderPass>(renderPass, framebuffer, renderArea, clearValueCount, clearValues);
}

void CommandBuffer::nextSubpass(VkSubpassContents contents)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdNextSubpass>();
}

void CommandBuffer::endRenderPass()
{
	addCommand<::CmdEndRenderPass>();
}

void CommandBuffer::executeCommands(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < commandBufferCount; ++i)
	{
		addCommand<::CmdExecuteCommands>(vk::Cast(pCommandBuffers[i]));
	}
}

void CommandBuffer::setDeviceMask(uint32_t deviceMask)
{
	// SwiftShader only has one device, so we ignore the device mask
}

void CommandBuffer::dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                                 uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	addCommand<::CmdDispatch>(baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    VkDependencyFlags dependencyFlags,
                                    uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
                                    uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
                                    uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
	addCommand<::CmdPipelineBarrier>();
}

void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, Pipeline *pipeline)
{
	switch(pipelineBindPoint)
	{
		case VK_PIPELINE_BIND_POINT_COMPUTE:
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			addCommand<::CmdPipelineBind>(pipelineBindPoint, pipeline);
			break;
		default:
			UNSUPPORTED("VkPipelineBindPoint %d", int(pipelineBindPoint));
	}
}

void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                                      const VkBuffer *pBuffers, const VkDeviceSize *pOffsets)
{
	for(uint32_t i = 0; i < bindingCount; ++i)
	{
		addCommand<::CmdVertexBufferBind>(i + firstBinding, vk::Cast(pBuffers[i]), pOffsets[i]);
	}
}

void CommandBuffer::beginQuery(QueryPool *queryPool, uint32_t query, VkQueryControlFlags flags)
{
	addCommand<::CmdBeginQuery>(queryPool, query, flags);
}

void CommandBuffer::endQuery(QueryPool *queryPool, uint32_t query)
{
	addCommand<::CmdEndQuery>(queryPool, query);
}

void CommandBuffer::resetQueryPool(QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount)
{
	addCommand<::CmdResetQueryPool>(queryPool, firstQuery, queryCount);
}

void CommandBuffer::writeTimestamp(VkPipelineStageFlagBits pipelineStage, QueryPool *queryPool, uint32_t query)
{
	addCommand<::CmdWriteTimeStamp>(queryPool, query, pipelineStage);
}

void CommandBuffer::copyQueryPoolResults(const QueryPool *queryPool, uint32_t firstQuery, uint32_t queryCount,
                                         Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
	addCommand<::CmdCopyQueryPoolResults>(queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

void CommandBuffer::pushConstants(PipelineLayout *layout, VkShaderStageFlags stageFlags,
                                  uint32_t offset, uint32_t size, const void *pValues)
{
	addCommand<::CmdSetPushConstants>(offset, size, pValues);
}

void CommandBuffer::setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports)
{
	if(firstViewport != 0 || viewportCount > 1)
	{
		UNSUPPORTED("VkPhysicalDeviceFeatures::multiViewport");
	}

	for(uint32_t i = 0; i < viewportCount; i++)
	{
		addCommand<::CmdSetViewport>(pViewports[i], i + firstViewport);
	}
}

void CommandBuffer::setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors)
{
	if(firstScissor != 0 || scissorCount > 1)
	{
		UNSUPPORTED("VkPhysicalDeviceFeatures::multiViewport");
	}

	for(uint32_t i = 0; i < scissorCount; i++)
	{
		addCommand<::CmdSetScissor>(pScissors[i], i + firstScissor);
	}
}

void CommandBuffer::setLineWidth(float lineWidth)
{
	// If the wide lines feature is not enabled, lineWidth must be 1.0
	ASSERT(lineWidth == 1.0f);
}

void CommandBuffer::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
	addCommand<::CmdSetDepthBias>(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void CommandBuffer::setBlendConstants(const float blendConstants[4])
{
	addCommand<::CmdSetBlendConstants>(blendConstants);
}

void CommandBuffer::setDepthBounds(float minDepthBounds, float maxDepthBounds)
{
	addCommand<::CmdSetDepthBounds>(minDepthBounds, maxDepthBounds);
}

void CommandBuffer::setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<::CmdSetStencilCompareMask>(faceMask, compareMask);
}

void CommandBuffer::setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<::CmdSetStencilWriteMask>(faceMask, writeMask);
}

void CommandBuffer::setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
	// faceMask must not be 0
	ASSERT(faceMask != 0);

	addCommand<::CmdSetStencilReference>(faceMask, reference);
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, const PipelineLayout *layout,
                                       uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets,
                                       uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < descriptorSetCount; i++)
	{
		auto descriptorSetIndex = firstSet + i;
		auto setLayout = layout->getDescriptorSetLayout(descriptorSetIndex);

		auto numDynamicDescriptors = setLayout->getDynamicDescriptorCount();
		ASSERT(numDynamicDescriptors == 0 || pDynamicOffsets != nullptr);
		ASSERT(dynamicOffsetCount >= numDynamicDescriptors);

		addCommand<::CmdBindDescriptorSet>(
		    pipelineBindPoint, layout, descriptorSetIndex, vk::Cast(pDescriptorSets[i]),
		    dynamicOffsetCount, pDynamicOffsets);

		pDynamicOffsets += numDynamicDescriptors;
		dynamicOffsetCount -= numDynamicDescriptors;
	}
}

void CommandBuffer::bindIndexBuffer(Buffer *buffer, VkDeviceSize offset, VkIndexType indexType)
{
	addCommand<::CmdIndexBufferBind>(buffer, offset, indexType);
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	addCommand<::CmdDispatch>(0, 0, 0, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::dispatchIndirect(Buffer *buffer, VkDeviceSize offset)
{
	addCommand<::CmdDispatchIndirect>(buffer, offset);
}

void CommandBuffer::copyBuffer(const Buffer *srcBuffer, Buffer *dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdBufferToBufferCopy>(srcBuffer, dstBuffer, pRegions[i]);
	}
}

void CommandBuffer::copyImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
                              uint32_t regionCount, const VkImageCopy *pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdImageToImageCopy>(srcImage, dstImage, pRegions[i]);
	}
}

void CommandBuffer::blitImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
                              uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdBlitImage>(srcImage, dstImage, pRegions[i], filter);
	}
}

void CommandBuffer::copyBufferToImage(Buffer *srcBuffer, Image *dstImage, VkImageLayout dstImageLayout,
                                      uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdBufferToImageCopy>(srcBuffer, dstImage, pRegions[i]);
	}
}

void CommandBuffer::copyImageToBuffer(Image *srcImage, VkImageLayout srcImageLayout, Buffer *dstBuffer,
                                      uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL || srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdImageToBufferCopy>(srcImage, dstBuffer, pRegions[i]);
	}
}

void CommandBuffer::updateBuffer(Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void *pData)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdUpdateBuffer>(dstBuffer, dstOffset, dataSize, reinterpret_cast<const uint8_t *>(pData));
}

void CommandBuffer::fillBuffer(Buffer *dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdFillBuffer>(dstBuffer, dstOffset, size, data);
}

void CommandBuffer::clearColorImage(Image *image, VkImageLayout imageLayout, const VkClearColorValue *pColor,
                                    uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<::CmdClearColorImage>(image, *pColor, pRanges[i]);
	}
}

void CommandBuffer::clearDepthStencilImage(Image *image, VkImageLayout imageLayout, const VkClearDepthStencilValue *pDepthStencil,
                                           uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < rangeCount; i++)
	{
		addCommand<::CmdClearDepthStencilImage>(image, *pDepthStencil, pRanges[i]);
	}
}

void CommandBuffer::clearAttachments(uint32_t attachmentCount, const VkClearAttachment *pAttachments,
                                     uint32_t rectCount, const VkClearRect *pRects)
{
	ASSERT(state == RECORDING);

	for(uint32_t i = 0; i < attachmentCount; i++)
	{
		for(uint32_t j = 0; j < rectCount; j++)
		{
			addCommand<::CmdClearAttachment>(pAttachments[i], pRects[j]);
		}
	}
}

void CommandBuffer::resolveImage(const Image *srcImage, VkImageLayout srcImageLayout, Image *dstImage, VkImageLayout dstImageLayout,
                                 uint32_t regionCount, const VkImageResolve *pRegions)
{
	ASSERT(state == RECORDING);
	ASSERT(srcImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ||
	       srcImageLayout == VK_IMAGE_LAYOUT_GENERAL);
	ASSERT(dstImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
	       dstImageLayout == VK_IMAGE_LAYOUT_GENERAL);

	for(uint32_t i = 0; i < regionCount; i++)
	{
		addCommand<::CmdResolveImage>(srcImage, dstImage, pRegions[i]);
	}
}

void CommandBuffer::setEvent(Event *event, VkPipelineStageFlags stageMask)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdSignalEvent>(event, stageMask);
}

void CommandBuffer::resetEvent(Event *event, VkPipelineStageFlags stageMask)
{
	ASSERT(state == RECORDING);

	addCommand<::CmdResetEvent>(event, stageMask);
}

void CommandBuffer::waitEvents(uint32_t eventCount, const VkEvent *pEvents, VkPipelineStageFlags srcStageMask,
                               VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
                               uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
                               uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
	ASSERT(state == RECORDING);

	// TODO(b/117835459): Since we always do a full barrier, all memory barrier related arguments are ignored

	// Note: srcStageMask and dstStageMask are currently ignored
	for(uint32_t i = 0; i < eventCount; i++)
	{
		addCommand<::CmdWaitEvent>(vk::Cast(pEvents[i]));
	}
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	addCommand<::CmdDraw>(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	addCommand<::CmdDrawIndexed>(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::drawIndirect(Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	addCommand<::CmdDrawIndirect>(buffer, offset, drawCount, stride);
}

void CommandBuffer::drawIndexedIndirect(Buffer *buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
	addCommand<::CmdDrawIndexedIndirect>(buffer, offset, drawCount, stride);
}

void CommandBuffer::submit(CommandBuffer::ExecutionState &executionState)
{
	// Perform recorded work
	state = PENDING;

#ifdef ENABLE_VK_DEBUGGER
	std::shared_ptr<vk::dbg::Thread> debuggerThread;
	auto debuggerContext = device->getDebuggerContext();
	if(debuggerContext)
	{
		auto lock = debuggerContext->lock();
		debuggerThread = lock.currentThread();
		debuggerThread->setName("vkQueue processor");
		debuggerThread->enter(lock, debuggerFile, "vkCommandBuffer::submit");
		lock.unlock();
	}
	defer(if(debuggerThread) { debuggerThread->exit(); });
	int line = 1;
#endif  // ENABLE_VK_DEBUGGER

	for(auto &command : *commands)
	{
#ifdef ENABLE_VK_DEBUGGER
		if(debuggerThread)
		{
			debuggerThread->update([&](vk::dbg::Frame &frame) {
				frame.location = { debuggerFile, line++, 0 };
			});
		}
#endif  // ENABLE_VK_DEBUGGER

		command->play(executionState);
	}

	// After work is completed
	state = EXECUTABLE;
}

void CommandBuffer::submitSecondary(CommandBuffer::ExecutionState &executionState) const
{
	for(auto &command : *commands)
	{
		command->play(executionState);
	}
}

void CommandBuffer::ExecutionState::bindVertexInputs(sw::Context &context, int firstInstance)
{
	for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; i++)
	{
		auto &attrib = context.input[i];
		if(attrib.format != VK_FORMAT_UNDEFINED)
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

void CommandBuffer::ExecutionState::bindAttachments(sw::Context &context)
{
	// Binds all the attachments for the current subpass
	// Ideally this would be performed by BeginRenderPass and NextSubpass, but
	// there is too much stomping of the renderer's state by setContext() in
	// draws.

	auto const &subpass = renderPass->getSubpass(subpassIndex);

	for(auto i = 0u; i < subpass.colorAttachmentCount; i++)
	{
		auto attachmentReference = subpass.pColorAttachments[i];
		if(attachmentReference.attachment != VK_ATTACHMENT_UNUSED)
		{
			context.renderTarget[i] = renderPassFramebuffer->getAttachment(attachmentReference.attachment);
		}
	}

	auto attachmentReference = subpass.pDepthStencilAttachment;
	if(attachmentReference && attachmentReference->attachment != VK_ATTACHMENT_UNUSED)
	{
		auto attachment = renderPassFramebuffer->getAttachment(attachmentReference->attachment);
		if(attachment->hasDepthAspect())
		{
			context.depthBuffer = attachment;
		}
		if(attachment->hasStencilAspect())
		{
			context.stencilBuffer = attachment;
		}
	}
}

}  // namespace vk
