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

#include "Context.hpp"

#include "Vulkan/VkBuffer.hpp"
#include "Vulkan/VkDevice.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Vulkan/VkRenderPass.hpp"
#include "Vulkan/VkStringify.hpp"

namespace {

uint32_t ComputePrimitiveCount(VkPrimitiveTopology topology, uint32_t vertexCount)
{
	switch(topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return vertexCount;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
		return vertexCount / 2;
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return std::max<uint32_t>(vertexCount, 1) - 1;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
		return vertexCount / 3;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
		return std::max<uint32_t>(vertexCount, 2) - 2;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return std::max<uint32_t>(vertexCount, 2) - 2;
	default:
		UNSUPPORTED("VkPrimitiveTopology %d", int(topology));
	}

	return 0;
}

template<typename T>
void ProcessPrimitiveRestart(T *indexBuffer,
                             VkPrimitiveTopology topology,
                             uint32_t count,
                             std::vector<std::pair<uint32_t, void *>> *indexBuffers)
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
				uint32_t primitiveCount = ComputePrimitiveCount(topology, vertexCount);
				if(primitiveCount > 0)
				{
					indexBuffers->push_back({ primitiveCount, indexBufferStart });
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
		uint32_t primitiveCount = ComputePrimitiveCount(topology, vertexCount);
		if(primitiveCount > 0)
		{
			indexBuffers->push_back({ primitiveCount, indexBufferStart });
		}
	}
}

}  // namespace

namespace vk {

int IndexBuffer::bytesPerIndex() const
{
	return indexType == VK_INDEX_TYPE_UINT16 ? 2 : 4;
}

void IndexBuffer::setIndexBufferBinding(const VertexInputBinding &indexBufferBinding, VkIndexType type)
{
	binding = indexBufferBinding;
	indexType = type;
}

void IndexBuffer::getIndexBuffers(VkPrimitiveTopology topology, uint32_t count, uint32_t first, bool indexed, bool hasPrimitiveRestartEnable, std::vector<std::pair<uint32_t, void *>> *indexBuffers) const
{

	if(indexed)
	{
		void *indexBuffer = binding.buffer->getOffsetPointer(binding.offset + first * bytesPerIndex());
		if(hasPrimitiveRestartEnable)
		{
			switch(indexType)
			{
			case VK_INDEX_TYPE_UINT16:
				ProcessPrimitiveRestart(static_cast<uint16_t *>(indexBuffer), topology, count, indexBuffers);
				break;
			case VK_INDEX_TYPE_UINT32:
				ProcessPrimitiveRestart(static_cast<uint32_t *>(indexBuffer), topology, count, indexBuffers);
				break;
			default:
				UNSUPPORTED("VkIndexType %d", int(indexType));
			}
		}
		else
		{
			indexBuffers->push_back({ ComputePrimitiveCount(topology, count), indexBuffer });
		}
	}
	else
	{
		indexBuffers->push_back({ ComputePrimitiveCount(topology, count), nullptr });
	}
}

VkFormat Attachments::colorFormat(int index) const
{
	ASSERT((index >= 0) && (index < sw::MAX_COLOR_BUFFERS));

	if(colorBuffer[index])
	{
		return colorBuffer[index]->getFormat();
	}
	else
	{
		return VK_FORMAT_UNDEFINED;
	}
}

VkFormat Attachments::depthFormat() const
{
	if(depthBuffer)
	{
		return depthBuffer->getFormat();
	}
	else
	{
		return VK_FORMAT_UNDEFINED;
	}
}

Inputs::Inputs(const VkPipelineVertexInputStateCreateInfo *vertexInputState)
{
	if(vertexInputState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("vertexInputState->flags");
	}

	// Temporary in-binding-order representation of buffer strides, to be consumed below
	// when considering attributes. TODO: unfuse buffers from attributes in backend, is old GL model.
	uint32_t vertexStrides[MAX_VERTEX_INPUT_BINDINGS];
	uint32_t instanceStrides[MAX_VERTEX_INPUT_BINDINGS];
	for(uint32_t i = 0; i < vertexInputState->vertexBindingDescriptionCount; i++)
	{
		auto const &desc = vertexInputState->pVertexBindingDescriptions[i];
		vertexStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_VERTEX ? desc.stride : 0;
		instanceStrides[desc.binding] = desc.inputRate == VK_VERTEX_INPUT_RATE_INSTANCE ? desc.stride : 0;
	}

	for(uint32_t i = 0; i < vertexInputState->vertexAttributeDescriptionCount; i++)
	{
		auto const &desc = vertexInputState->pVertexAttributeDescriptions[i];
		sw::Stream &input = stream[desc.location];
		input.format = desc.format;
		input.offset = desc.offset;
		input.binding = desc.binding;
		input.vertexStride = vertexStrides[desc.binding];
		input.instanceStride = instanceStrides[desc.binding];
	}
}

void Inputs::updateDescriptorSets(const DescriptorSet::Array &dso,
                                  const DescriptorSet::Bindings &ds,
                                  const DescriptorSet::DynamicOffsets &ddo)
{
	descriptorSetObjects = dso;
	descriptorSets = ds;
	descriptorDynamicOffsets = ddo;
}

void Inputs::bindVertexInputs(int firstInstance)
{
	for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; i++)
	{
		auto &attrib = stream[i];
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

void Inputs::setVertexInputBinding(const VertexInputBinding bindings[])
{
	for(uint32_t i = 0; i < MAX_VERTEX_INPUT_BINDINGS; ++i)
	{
		vertexInputBindings[i] = bindings[i];
	}
}

// TODO(b/137740918): Optimize instancing to use a single draw call.
void Inputs::advanceInstanceAttributes()
{
	for(uint32_t i = 0; i < vk::MAX_VERTEX_INPUT_BINDINGS; i++)
	{
		auto &attrib = stream[i];
		if((attrib.format != VK_FORMAT_UNDEFINED) && attrib.instanceStride && (attrib.instanceStride < attrib.robustnessSize))
		{
			// Under the casts: attrib.buffer += attrib.instanceStride
			attrib.buffer = (void const *)((uintptr_t)attrib.buffer + attrib.instanceStride);
			attrib.robustnessSize -= attrib.instanceStride;
		}
	}
}

GraphicsState::DynamicStateFlags GraphicsState::ParseDynamicStateFlags(const VkPipelineDynamicStateCreateInfo *dynamicStateCreateInfo)
{
	GraphicsState::DynamicStateFlags dynamicStateFlags = {};

	if(dynamicStateCreateInfo)
	{
		if(dynamicStateCreateInfo->flags != 0)
		{
			// Vulkan 1.3: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("dynamicStateCreateInfo->flags %d", int(dynamicStateCreateInfo->flags));
		}

		for(uint32_t i = 0; i < dynamicStateCreateInfo->dynamicStateCount; i++)
		{
			VkDynamicState dynamicState = dynamicStateCreateInfo->pDynamicStates[i];
			switch(dynamicState)
			{
			case VK_DYNAMIC_STATE_VIEWPORT:
				dynamicStateFlags.dynamicViewport = true;
				break;
			case VK_DYNAMIC_STATE_SCISSOR:
				dynamicStateFlags.dynamicScissor = true;
				break;
			case VK_DYNAMIC_STATE_LINE_WIDTH:
				dynamicStateFlags.dynamicLineWidth = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_BIAS:
				dynamicStateFlags.dynamicDepthBias = true;
				break;
			case VK_DYNAMIC_STATE_BLEND_CONSTANTS:
				dynamicStateFlags.dynamicBlendConstants = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_BOUNDS:
				dynamicStateFlags.dynamicDepthBounds = true;
				break;
			case VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK:
				dynamicStateFlags.dynamicStencilCompareMask = true;
				break;
			case VK_DYNAMIC_STATE_STENCIL_WRITE_MASK:
				dynamicStateFlags.dynamicStencilWriteMask = true;
				break;
			case VK_DYNAMIC_STATE_STENCIL_REFERENCE:
				dynamicStateFlags.dynamicStencilReference = true;
				break;
			case VK_DYNAMIC_STATE_CULL_MODE:
				dynamicStateFlags.dynamicCullMode = true;
				break;
			case VK_DYNAMIC_STATE_FRONT_FACE:
				dynamicStateFlags.dynamicFrontFace = true;
				break;
			case VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY:
				dynamicStateFlags.dynamicPrimitiveTopology = true;
				break;
			case VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT:
				dynamicStateFlags.dynamicViewportWithCount = true;
				break;
			case VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT:
				dynamicStateFlags.dynamicScissorWithCount = true;
				break;
			case VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE:
				dynamicStateFlags.dynamicVertexInputBindingStride = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE:
				dynamicStateFlags.dynamicDepthTestEnable = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE:
				dynamicStateFlags.dynamicDepthWriteEnable = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_COMPARE_OP:
				dynamicStateFlags.dynamicDepthCompareOp = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE:
				dynamicStateFlags.dynamicDepthBoundsTestEnable = true;
				break;
			case VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE:
				dynamicStateFlags.dynamicStencilTestEnable = true;
				break;
			case VK_DYNAMIC_STATE_STENCIL_OP:
				dynamicStateFlags.dynamicStencilOp = true;
				break;
			case VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE:
				dynamicStateFlags.dynamicRasterizerDiscardEnable = true;
				break;
			case VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE:
				dynamicStateFlags.dynamicDepthBiasEnable = true;
				break;
			case VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE:
				dynamicStateFlags.dynamicPrimitiveRestartEnable = true;
				break;
			default:
				UNSUPPORTED("VkDynamicState %d", int(dynamicState));
			}
		}
	}

	return dynamicStateFlags;
}

VkDeviceSize Inputs::getVertexStride(uint32_t i, bool dynamicVertexStride) const
{
	auto &attrib = stream[i];
	if(attrib.format != VK_FORMAT_UNDEFINED)
	{
		if (dynamicVertexStride)
		{
			return vertexInputBindings[attrib.binding].stride;
		}
		else
		{
			return attrib.vertexStride;
		}
	}

	return 0;
}

GraphicsState::GraphicsState(const Device *device, const VkGraphicsPipelineCreateInfo *pCreateInfo,
                             const PipelineLayout *layout, bool robustBufferAccess)
    : pipelineLayout(layout)
    , robustBufferAccess(robustBufferAccess)
    , dynamicStateFlags(ParseDynamicStateFlags(pCreateInfo->pDynamicState))
{
	if((pCreateInfo->flags &
	    ~(VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT |
	      VK_PIPELINE_CREATE_DERIVATIVE_BIT |
	      VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT |
	      VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT_EXT |
	      VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT_EXT)) != 0)
	{
		UNSUPPORTED("pCreateInfo->flags %d", int(pCreateInfo->flags));
	}

	const VkPipelineVertexInputStateCreateInfo *vertexInputState = pCreateInfo->pVertexInputState;

	if(vertexInputState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("vertexInputState->flags");
	}

	const VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState = pCreateInfo->pInputAssemblyState;

	if(inputAssemblyState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->pInputAssemblyState->flags %d", int(pCreateInfo->pInputAssemblyState->flags));
	}

	primitiveRestartEnable = (inputAssemblyState->primitiveRestartEnable != VK_FALSE);
	topology = inputAssemblyState->topology;

	const VkPipelineRasterizationStateCreateInfo *rasterizationState = pCreateInfo->pRasterizationState;

	if(rasterizationState->flags != 0)
	{
		// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
		UNSUPPORTED("pCreateInfo->pRasterizationState->flags %d", int(pCreateInfo->pRasterizationState->flags));
	}

	rasterizerDiscard = (rasterizationState->rasterizerDiscardEnable != VK_FALSE);
	cullMode = rasterizationState->cullMode;
	frontFace = rasterizationState->frontFace;
	polygonMode = rasterizationState->polygonMode;
	constantDepthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasConstantFactor : 0.0f;
	slopeDepthBias = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasSlopeFactor : 0.0f;
	depthBiasClamp = (rasterizationState->depthBiasEnable != VK_FALSE) ? rasterizationState->depthBiasClamp : 0.0f;
	depthRangeUnrestricted = device->hasExtension(VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME);
	depthClampEnable = rasterizationState->depthClampEnable != VK_FALSE;
	depthClipEnable = !depthClampEnable;

	// From the Vulkan spec for vkCmdSetDepthBias:
	//    The bias value O for a polygon is:
	//        O = dbclamp(...)
	//    where dbclamp(x) =
	//        * x                       depthBiasClamp = 0 or NaN
	//        * min(x, depthBiasClamp)  depthBiasClamp > 0
	//        * max(x, depthBiasClamp)  depthBiasClamp < 0
	// So it should be safe to resolve NaNs to 0.0f.
	if(std::isnan(depthBiasClamp))
	{
		depthBiasClamp = 0.0f;
	}

	lineWidth = rasterizationState->lineWidth;

	const VkBaseInStructure *extensionCreateInfo = reinterpret_cast<const VkBaseInStructure *>(rasterizationState->pNext);
	while(extensionCreateInfo)
	{
		switch(extensionCreateInfo->sType)
		{
		case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO_EXT:
			{
				const VkPipelineRasterizationLineStateCreateInfoEXT *lineStateCreateInfo = reinterpret_cast<const VkPipelineRasterizationLineStateCreateInfoEXT *>(extensionCreateInfo);
				lineRasterizationMode = lineStateCreateInfo->lineRasterizationMode;
			}
			break;
		case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_PROVOKING_VERTEX_STATE_CREATE_INFO_EXT:
			{
				const VkPipelineRasterizationProvokingVertexStateCreateInfoEXT *provokingVertexModeCreateInfo =
				    reinterpret_cast<const VkPipelineRasterizationProvokingVertexStateCreateInfoEXT *>(extensionCreateInfo);
				provokingVertexMode = provokingVertexModeCreateInfo->provokingVertexMode;
			}
			break;
		case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT:
			{
				const auto *depthClipInfo = reinterpret_cast<const VkPipelineRasterizationDepthClipStateCreateInfoEXT *>(extensionCreateInfo);
				// Reserved for future use.
				ASSERT(depthClipInfo->flags == 0);
				depthClipEnable = depthClipInfo->depthClipEnable != VK_FALSE;
			}
			break;
		case VK_STRUCTURE_TYPE_APPLICATION_INFO:
			// SwiftShader doesn't interact with application info, but dEQP includes it
			break;
		case VK_STRUCTURE_TYPE_MAX_ENUM:
			// dEQP tests that this value is ignored.
			break;
		default:
			UNSUPPORTED("pCreateInfo->pRasterizationState->pNext sType = %s", vk::Stringify(extensionCreateInfo->sType).c_str());
			break;
		}

		extensionCreateInfo = extensionCreateInfo->pNext;
	}

	// The sample count affects the batch size, so it needs initialization even if rasterization is disabled.
	// TODO(b/147812380): Eliminate the dependency between multisampling and batch size.
	sampleCount = 1;

	// Only access rasterization state if rasterization is not disabled.
	if(rasterizationState->rasterizerDiscardEnable == VK_FALSE)
	{
		const VkPipelineViewportStateCreateInfo *viewportState = pCreateInfo->pViewportState;
		const VkPipelineMultisampleStateCreateInfo *multisampleState = pCreateInfo->pMultisampleState;
		const VkPipelineDepthStencilStateCreateInfo *depthStencilState = pCreateInfo->pDepthStencilState;
		const VkPipelineColorBlendStateCreateInfo *colorBlendState = pCreateInfo->pColorBlendState;

		if(viewportState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pViewportState->flags %d", int(pCreateInfo->pViewportState->flags));
		}

		if((viewportState->viewportCount > 1) ||
		   (viewportState->scissorCount > 1))
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::multiViewport");
		}

		if(!dynamicStateFlags.dynamicScissor)
		{
			scissor = viewportState->pScissors[0];
		}

		if(!dynamicStateFlags.dynamicViewport)
		{
			viewport = viewportState->pViewports[0];
		}

		if(multisampleState->flags != 0)
		{
			// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
			UNSUPPORTED("pCreateInfo->pMultisampleState->flags %d", int(pCreateInfo->pMultisampleState->flags));
		}

		sampleShadingEnable = (multisampleState->sampleShadingEnable != VK_FALSE);
		if(sampleShadingEnable)
		{
			minSampleShading = multisampleState->minSampleShading;
		}

		if(multisampleState->alphaToOneEnable != VK_FALSE)
		{
			UNSUPPORTED("VkPhysicalDeviceFeatures::alphaToOne");
		}

		switch(multisampleState->rasterizationSamples)
		{
		case VK_SAMPLE_COUNT_1_BIT:
			sampleCount = 1;
			break;
		case VK_SAMPLE_COUNT_4_BIT:
			sampleCount = 4;
			break;
		default:
			UNSUPPORTED("Unsupported sample count");
		}

		VkSampleMask sampleMask;
		if(multisampleState->pSampleMask)
		{
			sampleMask = multisampleState->pSampleMask[0];
		}
		else  // "If pSampleMask is NULL, it is treated as if the mask has all bits set to 1."
		{
			sampleMask = ~0;
		}

		alphaToCoverage = (multisampleState->alphaToCoverageEnable != VK_FALSE);
		multiSampleMask = sampleMask & ((unsigned)0xFFFFFFFF >> (32 - sampleCount));

		const vk::RenderPass *renderPass = vk::Cast(pCreateInfo->renderPass);
		const VkSubpassDescription &subpass = renderPass->getSubpass(pCreateInfo->subpass);

		// Ignore pDepthStencilState when "the subpass of the render pass the pipeline is created against does not use a depth/stencil attachment"
		if(subpass.pDepthStencilAttachment && subpass.pDepthStencilAttachment->attachment != VK_ATTACHMENT_UNUSED)
		{
			if(depthStencilState->flags != 0)
			{
				// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
				UNSUPPORTED("pCreateInfo->pDepthStencilState->flags %d", int(pCreateInfo->pDepthStencilState->flags));
			}

			depthBoundsTestEnable = (depthStencilState->depthBoundsTestEnable != VK_FALSE);
			minDepthBounds = depthStencilState->minDepthBounds;
			maxDepthBounds = depthStencilState->maxDepthBounds;

			depthTestEnable = (depthStencilState->depthTestEnable != VK_FALSE);
			depthWriteEnable = (depthStencilState->depthWriteEnable != VK_FALSE);
			depthCompareMode = depthStencilState->depthCompareOp;

			stencilEnable = (depthStencilState->stencilTestEnable != VK_FALSE);
			if(stencilEnable)
			{
				frontStencil = depthStencilState->front;
				backStencil = depthStencilState->back;
			}
		}

		bool colorAttachmentUsed = false;
		for(uint32_t i = 0; i < subpass.colorAttachmentCount; i++)
		{
			if(subpass.pColorAttachments[i].attachment != VK_ATTACHMENT_UNUSED)
			{
				colorAttachmentUsed = true;
				break;
			}
		}

		// Ignore pColorBlendState when "the subpass of the render pass the pipeline is created against does not use any color attachments"
		if(colorAttachmentUsed)
		{
			if(colorBlendState->flags != 0)
			{
				// Vulkan 1.2: "flags is reserved for future use." "flags must be 0"
				UNSUPPORTED("pCreateInfo->pColorBlendState->flags %d", int(pCreateInfo->pColorBlendState->flags));
			}

			if(colorBlendState->logicOpEnable != VK_FALSE)
			{
				UNSUPPORTED("VkPhysicalDeviceFeatures::logicOp");
			}

			if(!dynamicStateFlags.dynamicBlendConstants)
			{
				blendConstants.x = colorBlendState->blendConstants[0];
				blendConstants.y = colorBlendState->blendConstants[1];
				blendConstants.z = colorBlendState->blendConstants[2];
				blendConstants.w = colorBlendState->blendConstants[3];
			}

			const VkBaseInStructure *extensionColorBlendInfo = reinterpret_cast<const VkBaseInStructure *>(colorBlendState->pNext);
			while(extensionColorBlendInfo)
			{
				switch(extensionColorBlendInfo->sType)
				{
				case VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT:
					{
						const VkPipelineColorBlendAdvancedStateCreateInfoEXT *colorBlendAdvancedCreateInfo = reinterpret_cast<const VkPipelineColorBlendAdvancedStateCreateInfoEXT *>(extensionColorBlendInfo);
						ASSERT(colorBlendAdvancedCreateInfo->blendOverlap == VK_BLEND_OVERLAP_UNCORRELATED_EXT);
						ASSERT(colorBlendAdvancedCreateInfo->dstPremultiplied == VK_TRUE);
						ASSERT(colorBlendAdvancedCreateInfo->srcPremultiplied == VK_TRUE);
					}
					break;
				case VK_STRUCTURE_TYPE_MAX_ENUM:
					// dEQP tests that this value is ignored.
					break;
				default:
					UNSUPPORTED("pCreateInfo->colorBlendState->pNext sType = %s", vk::Stringify(extensionColorBlendInfo->sType).c_str());
					break;
				}

				extensionColorBlendInfo = extensionColorBlendInfo->pNext;
			}

			ASSERT(colorBlendState->attachmentCount <= sw::MAX_COLOR_BUFFERS);
			for(auto i = 0u; i < colorBlendState->attachmentCount; i++)
			{
				const VkPipelineColorBlendAttachmentState &attachment = colorBlendState->pAttachments[i];
				colorWriteMask[i] = attachment.colorWriteMask;
				blendState[i] = { (attachment.blendEnable != VK_FALSE),
					              attachment.srcColorBlendFactor, attachment.dstColorBlendFactor, attachment.colorBlendOp,
					              attachment.srcAlphaBlendFactor, attachment.dstAlphaBlendFactor, attachment.alphaBlendOp };
			}
		}
	}
}

bool GraphicsState::isDrawPoint(bool polygonModeAware) const
{
	switch(topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return true;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return false;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_POINT) : false;
	default:
		UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

bool GraphicsState::isDrawLine(bool polygonModeAware) const
{
	switch(topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
		return false;
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return true;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_LINE) : false;
	default:
		UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

bool GraphicsState::isDrawTriangle(bool polygonModeAware) const
{
	switch(topology)
	{
	case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
	case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
		return false;
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
	case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
		return polygonModeAware ? (polygonMode == VK_POLYGON_MODE_FILL) : true;
	default:
		UNSUPPORTED("topology %d", int(topology));
	}
	return false;
}

bool GraphicsState::depthWriteActive(const Attachments &attachments) const
{
	// "Depth writes are always disabled when depthTestEnable is VK_FALSE."
	return depthTestActive(attachments) && depthWriteEnable;
}

bool GraphicsState::depthTestActive(const Attachments &attachments) const
{
	return attachments.depthBuffer && depthTestEnable;
}

bool GraphicsState::stencilActive(const Attachments &attachments) const
{
	return attachments.stencilBuffer && stencilEnable;
}

bool GraphicsState::depthBoundsTestActive(const Attachments &attachments) const
{
	return attachments.depthBuffer && depthBoundsTestEnable;
}

const GraphicsState GraphicsState::combineStates(const DynamicState &dynamicState) const
{
	GraphicsState combinedState = *this;

	// Apply either pipeline state or dynamic state
	if(dynamicStateFlags.dynamicDepthTestEnable)
	{
		combinedState.depthTestEnable = dynamicState.depthTestEnable;
	}

	if(dynamicStateFlags.dynamicDepthWriteEnable)
	{
		combinedState.depthWriteEnable = dynamicState.depthWriteEnable;
	}

	if(dynamicStateFlags.dynamicDepthCompareOp)
	{
		combinedState.depthCompareMode = dynamicState.depthCompareOp;
	}

	if(dynamicStateFlags.dynamicDepthBoundsTestEnable)
	{
		combinedState.depthBoundsTestEnable = dynamicState.depthBoundsTestEnable;
	}

	if(dynamicStateFlags.dynamicStencilTestEnable)
	{
		combinedState.stencilEnable = dynamicState.stencilTestEnable;
	}

	if(dynamicStateFlags.dynamicScissor)
	{
		combinedState.scissor = dynamicState.scissor;
	}

	if(dynamicStateFlags.dynamicViewport)
	{
		combinedState.viewport = dynamicState.viewport;
	}

	if(dynamicStateFlags.dynamicBlendConstants)
	{
		combinedState.blendConstants = dynamicState.blendConstants;
	}

	if(dynamicStateFlags.dynamicDepthBias)
	{
		combinedState.constantDepthBias = dynamicState.depthBiasConstantFactor;
		combinedState.slopeDepthBias = dynamicState.depthBiasSlopeFactor;
		combinedState.depthBiasClamp = dynamicState.depthBiasClamp;
	}

	if(dynamicStateFlags.dynamicDepthBounds && combinedState.depthBoundsTestEnable)
	{
		combinedState.minDepthBounds = dynamicState.minDepthBounds;
		combinedState.maxDepthBounds = dynamicState.maxDepthBounds;
	}

	if(dynamicStateFlags.dynamicStencilCompareMask && combinedState.stencilEnable)
	{
		combinedState.frontStencil.compareMask = dynamicState.frontStencil.compareMask;
		combinedState.backStencil.compareMask = dynamicState.backStencil.compareMask;
	}

	if(dynamicStateFlags.dynamicStencilWriteMask && combinedState.stencilEnable)
	{
		combinedState.frontStencil.writeMask = dynamicState.frontStencil.writeMask;
		combinedState.backStencil.writeMask = dynamicState.backStencil.writeMask;
	}

	if(dynamicStateFlags.dynamicStencilReference && combinedState.stencilEnable)
	{
		combinedState.frontStencil.reference = dynamicState.frontStencil.reference;
		combinedState.backStencil.reference = dynamicState.backStencil.reference;
	}

	if(dynamicStateFlags.dynamicStencilOp && combinedState.stencilEnable)
	{
		if(dynamicState.faceMask & VK_STENCIL_FACE_FRONT_BIT)
		{
			combinedState.frontStencil.compareOp = dynamicState.frontStencil.compareOp;
			combinedState.frontStencil.depthFailOp = dynamicState.frontStencil.depthFailOp;
			combinedState.frontStencil.failOp = dynamicState.frontStencil.failOp;
			combinedState.frontStencil.passOp = dynamicState.frontStencil.passOp;
		}

		if(dynamicState.faceMask & VK_STENCIL_FACE_BACK_BIT)
		{
			combinedState.backStencil.compareOp = dynamicState.backStencil.compareOp;
			combinedState.backStencil.depthFailOp = dynamicState.backStencil.depthFailOp;
			combinedState.backStencil.failOp = dynamicState.backStencil.failOp;
			combinedState.backStencil.passOp = dynamicState.backStencil.passOp;
		}
	}

	if(dynamicStateFlags.dynamicCullMode)
	{
		combinedState.cullMode = dynamicState.cullMode;
	}

	if(dynamicStateFlags.dynamicFrontFace)
	{
		combinedState.frontFace = dynamicState.frontFace;
	}

	if(dynamicStateFlags.dynamicPrimitiveTopology)
	{
		combinedState.topology = dynamicState.primitiveTopology;
	}

	if(dynamicStateFlags.dynamicViewportWithCount && (dynamicState.viewportCount > 0))
	{
		combinedState.viewport.width = static_cast<float>(dynamicState.viewports[0].extent.width);
		combinedState.viewport.height = static_cast<float>(dynamicState.viewports[0].extent.height);
		combinedState.viewport.x = static_cast<float>(dynamicState.viewports[0].offset.x);
		combinedState.viewport.y = static_cast<float>(dynamicState.viewports[0].offset.y);
	}

	if(dynamicStateFlags.dynamicScissorWithCount && (dynamicState.scissorCount > 0))
	{
		combinedState.scissor = dynamicState.scissors[0];
	}

	return combinedState;
}

BlendState GraphicsState::getBlendState(int index, const Attachments &attachments, bool fragmentContainsKill) const
{
	ASSERT((index >= 0) && (index < sw::MAX_COLOR_BUFFERS));
	auto &state = blendState[index];

	BlendState activeBlendState = {};
	activeBlendState.alphaBlendEnable = alphaBlendActive(index, attachments, fragmentContainsKill);

	if(activeBlendState.alphaBlendEnable)
	{
		vk::Format format = attachments.colorBuffer[index]->getFormat(VK_IMAGE_ASPECT_COLOR_BIT);

		activeBlendState.sourceBlendFactor = blendFactor(state.blendOperation, state.sourceBlendFactor);
		activeBlendState.destBlendFactor = blendFactor(state.blendOperation, state.destBlendFactor);
		activeBlendState.blendOperation = blendOperation(state.blendOperation, state.sourceBlendFactor, state.destBlendFactor, format);
		activeBlendState.sourceBlendFactorAlpha = blendFactor(state.blendOperationAlpha, state.sourceBlendFactorAlpha);
		activeBlendState.destBlendFactorAlpha = blendFactor(state.blendOperationAlpha, state.destBlendFactorAlpha);
		activeBlendState.blendOperationAlpha = blendOperation(state.blendOperationAlpha, state.sourceBlendFactorAlpha, state.destBlendFactorAlpha, format);
	}

	return activeBlendState;
}

bool GraphicsState::alphaBlendActive(int index, const Attachments &attachments, bool fragmentContainsKill) const
{
	ASSERT((index >= 0) && (index < sw::MAX_COLOR_BUFFERS));
	auto &state = blendState[index];

	if(!attachments.colorBuffer[index] || !blendState[index].alphaBlendEnable)
	{
		return false;
	}

	if(!(colorWriteActive(attachments) || fragmentContainsKill))
	{
		return false;
	}

	vk::Format format = attachments.colorBuffer[index]->getFormat(VK_IMAGE_ASPECT_COLOR_BIT);
	bool colorBlend = blendOperation(state.blendOperation, state.sourceBlendFactor, state.destBlendFactor, format) != VK_BLEND_OP_SRC_EXT;
	bool alphaBlend = blendOperation(state.blendOperationAlpha, state.sourceBlendFactorAlpha, state.destBlendFactorAlpha, format) != VK_BLEND_OP_SRC_EXT;

	return colorBlend || alphaBlend;
}

VkBlendFactor GraphicsState::blendFactor(VkBlendOp blendOperation, VkBlendFactor blendFactor) const
{
	switch(blendOperation)
	{
	case VK_BLEND_OP_ADD:
	case VK_BLEND_OP_SUBTRACT:
	case VK_BLEND_OP_REVERSE_SUBTRACT:
		return blendFactor;
	case VK_BLEND_OP_MIN:
	case VK_BLEND_OP_MAX:
	case VK_BLEND_OP_MULTIPLY_EXT:
	case VK_BLEND_OP_SCREEN_EXT:
	case VK_BLEND_OP_OVERLAY_EXT:
	case VK_BLEND_OP_DARKEN_EXT:
	case VK_BLEND_OP_LIGHTEN_EXT:
	case VK_BLEND_OP_COLORDODGE_EXT:
	case VK_BLEND_OP_COLORBURN_EXT:
	case VK_BLEND_OP_HARDLIGHT_EXT:
	case VK_BLEND_OP_SOFTLIGHT_EXT:
	case VK_BLEND_OP_DIFFERENCE_EXT:
	case VK_BLEND_OP_EXCLUSION_EXT:
	case VK_BLEND_OP_HSL_HUE_EXT:
	case VK_BLEND_OP_HSL_SATURATION_EXT:
	case VK_BLEND_OP_HSL_COLOR_EXT:
	case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
		return VK_BLEND_FACTOR_ONE;
	default:
		ASSERT(false);
		return blendFactor;
	}
}

VkBlendOp GraphicsState::blendOperation(VkBlendOp blendOperation, VkBlendFactor sourceBlendFactor, VkBlendFactor destBlendFactor, vk::Format format) const
{
	switch(blendOperation)
	{
	case VK_BLEND_OP_ADD:
		if(sourceBlendFactor == VK_BLEND_FACTOR_ZERO)
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_ZERO_EXT;
			}
			else if(destBlendFactor == VK_BLEND_FACTOR_ONE)
			{
				return VK_BLEND_OP_DST_EXT;
			}
		}
		else if(sourceBlendFactor == VK_BLEND_FACTOR_ONE)
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_SRC_EXT;
			}
		}
		break;
	case VK_BLEND_OP_SUBTRACT:
		if(sourceBlendFactor == VK_BLEND_FACTOR_ZERO)
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_ZERO_EXT;
			}
			else if(format.isUnsignedNormalized())
			{
				return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
			}
		}
		else if(sourceBlendFactor == VK_BLEND_FACTOR_ONE)
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_SRC_EXT;
			}
		}
		break;
	case VK_BLEND_OP_REVERSE_SUBTRACT:
		if(sourceBlendFactor == VK_BLEND_FACTOR_ZERO)
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO)
			{
				return VK_BLEND_OP_ZERO_EXT;
			}
			else if(destBlendFactor == VK_BLEND_FACTOR_ONE)
			{
				return VK_BLEND_OP_DST_EXT;
			}
		}
		else
		{
			if(destBlendFactor == VK_BLEND_FACTOR_ZERO && format.isUnsignedNormalized())
			{
				return VK_BLEND_OP_ZERO_EXT;  // Negative, clamped to zero
			}
		}
		break;
	case VK_BLEND_OP_MIN:
		return VK_BLEND_OP_MIN;
	case VK_BLEND_OP_MAX:
		return VK_BLEND_OP_MAX;
	case VK_BLEND_OP_MULTIPLY_EXT:
	case VK_BLEND_OP_SCREEN_EXT:
	case VK_BLEND_OP_OVERLAY_EXT:
	case VK_BLEND_OP_DARKEN_EXT:
	case VK_BLEND_OP_LIGHTEN_EXT:
	case VK_BLEND_OP_COLORDODGE_EXT:
	case VK_BLEND_OP_COLORBURN_EXT:
	case VK_BLEND_OP_HARDLIGHT_EXT:
	case VK_BLEND_OP_SOFTLIGHT_EXT:
	case VK_BLEND_OP_DIFFERENCE_EXT:
	case VK_BLEND_OP_EXCLUSION_EXT:
	case VK_BLEND_OP_HSL_HUE_EXT:
	case VK_BLEND_OP_HSL_SATURATION_EXT:
	case VK_BLEND_OP_HSL_COLOR_EXT:
	case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
		return blendOperation;
	default:
		ASSERT(false);
	}

	return blendOperation;
}

bool GraphicsState::colorWriteActive(const Attachments &attachments) const
{
	for(int i = 0; i < sw::MAX_COLOR_BUFFERS; i++)
	{
		if(colorWriteActive(i, attachments))
		{
			return true;
		}
	}

	return false;
}

int GraphicsState::colorWriteActive(int index, const Attachments &attachments) const
{
	ASSERT((index >= 0) && (index < sw::MAX_COLOR_BUFFERS));
	auto &state = blendState[index];

	if(!attachments.colorBuffer[index] || attachments.colorBuffer[index]->getFormat() == VK_FORMAT_UNDEFINED)
	{
		return 0;
	}

	vk::Format format = attachments.colorBuffer[index]->getFormat(VK_IMAGE_ASPECT_COLOR_BIT);

	if(blendOperation(state.blendOperation, state.sourceBlendFactor, state.destBlendFactor, format) == VK_BLEND_OP_DST_EXT &&
	   blendOperation(state.blendOperationAlpha, state.sourceBlendFactorAlpha, state.destBlendFactorAlpha, format) == VK_BLEND_OP_DST_EXT)
	{
		return 0;
	}

	return colorWriteMask[index];
}

}  // namespace vk