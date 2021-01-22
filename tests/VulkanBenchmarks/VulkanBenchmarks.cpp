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

#include "benchmark/benchmark.h"

#include "Buffer.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "Swapchain.hpp"
#include "Util.hpp"
#include "VulkanHeaders.hpp"
#include "Window.hpp"

#include <cassert>
#include <vector>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

class VulkanBenchmark
{
public:
	VulkanBenchmark()
	{
	}

	virtual ~VulkanBenchmark()
	{
		device.waitIdle();
		device.destroy(nullptr);
		instance.destroy(nullptr);
	}

	// Call once after construction so that virtual functions may be called during init
	void initialize()
	{
		// TODO(b/158231104): Other platforms
#if defined(_WIN32)
		dl = std::make_unique<vk::DynamicLoader>("./vk_swiftshader.dll");
#elif defined(__linux__)
		dl = std::make_unique<vk::DynamicLoader>("./libvk_swiftshader.so");
#else
#	error Unimplemented platform
#endif
		assert(dl->success());

		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		instance = vk::createInstance({}, nullptr);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

		std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
		assert(!physicalDevices.empty());
		physicalDevice = physicalDevices[0];

		const float defaultQueuePriority = 0.0f;
		vk::DeviceQueueCreateInfo queueCreatInfo;
		queueCreatInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreatInfo.queueCount = 1;
		queueCreatInfo.pQueuePriorities = &defaultQueuePriority;

		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreatInfo;

		device = physicalDevice.createDevice(deviceCreateInfo, nullptr);

		queue = device.getQueue(queueFamilyIndex, 0);
	}

private:
	std::unique_ptr<vk::DynamicLoader> dl;

protected:
	const uint32_t queueFamilyIndex = 0;

	vk::Instance instance;  // Owning handle
	vk::PhysicalDevice physicalDevice;
	vk::Device device;  // Owning handle
	vk::Queue queue;
};

class ClearImageBenchmark : public VulkanBenchmark
{
public:
	void initialize(vk::Format clearFormat, vk::ImageAspectFlagBits clearAspect)
	{
		VulkanBenchmark::initialize();

		vk::ImageCreateInfo imageInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.format = clearFormat;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eGeneral;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
		imageInfo.samples = vk::SampleCountFlagBits::e4;
		imageInfo.extent = vk::Extent3D(1024, 1024, 1);
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;

		image = device.createImage(imageInfo);

		vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image);

		vk::MemoryAllocateInfo allocateInfo;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = 0;

		memory = device.allocateMemory(allocateInfo);

		device.bindImageMemory(image, memory, 0);

		vk::CommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		commandPool = device.createCommandPool(commandPoolCreateInfo);

		vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.commandBufferCount = 1;

		commandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

		vk::CommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.flags = {};

		commandBuffer.begin(commandBufferBeginInfo);

		vk::ImageSubresourceRange range;
		range.aspectMask = clearAspect;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		if(clearAspect == vk::ImageAspectFlagBits::eColor)
		{
			vk::ClearColorValue clearColorValue;
			clearColorValue.float32[0] = 0.0f;
			clearColorValue.float32[1] = 1.0f;
			clearColorValue.float32[2] = 0.0f;
			clearColorValue.float32[3] = 1.0f;

			commandBuffer.clearColorImage(image, vk::ImageLayout::eGeneral, &clearColorValue, 1, &range);
		}
		else if(clearAspect == vk::ImageAspectFlagBits::eDepth)
		{
			vk::ClearDepthStencilValue clearDepthStencilValue;
			clearDepthStencilValue.depth = 1.0f;
			clearDepthStencilValue.stencil = 0xFF;

			commandBuffer.clearDepthStencilImage(image, vk::ImageLayout::eGeneral, &clearDepthStencilValue, 1, &range);
		}
		else
			assert(false);

		commandBuffer.end();
	}

	~ClearImageBenchmark()
	{
		device.freeCommandBuffers(commandPool, 1, &commandBuffer);
		device.destroyCommandPool(commandPool, nullptr);
		device.freeMemory(memory, nullptr);
		device.destroyImage(image, nullptr);
	}

	void clear()
	{
		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		queue.submit(1, &submitInfo, nullptr);
		queue.waitIdle();
	}

private:
	vk::Image image;                  // Owning handle
	vk::DeviceMemory memory;          // Owning handle
	vk::CommandPool commandPool;      // Owning handle
	vk::CommandBuffer commandBuffer;  // Owning handle
};

static void ClearImage(benchmark::State &state, vk::Format clearFormat, vk::ImageAspectFlagBits clearAspect)
{
	ClearImageBenchmark benchmark;
	benchmark.initialize(clearFormat, clearAspect);

	// Execute once to have the Reactor routine generated.
	benchmark.clear();

	for(auto _ : state)
	{
		benchmark.clear();
	}
}

enum class Multisample
{
	False,
	True
};

class DrawBenchmark : public VulkanBenchmark
{
public:
	DrawBenchmark(Multisample multisample)
	    : multisample(multisample == Multisample::True)
	{
	}

	void initialize()
	{
		VulkanBenchmark::initialize();

		window.reset(new Window(instance, windowSize));
		swapchain.reset(new Swapchain(physicalDevice, device, *window));

		renderPass = createRenderPass(swapchain->colorFormat);
		createFramebuffers(renderPass);

		prepareVertices();

		pipeline = createGraphicsPipeline(renderPass);

		createSynchronizationPrimitives();

		createCommandBuffers(renderPass);
	}

	~DrawBenchmark()
	{
		device.freeCommandBuffers(commandPool, commandBuffers);

		device.destroyDescriptorPool(descriptorPool);
		for(auto &sampler : samplers)
		{
			device.destroySampler(sampler, nullptr);
		}
		images.clear();
		device.destroyCommandPool(commandPool, nullptr);

		for(auto &fence : waitFences)
		{
			device.destroyFence(fence, nullptr);
		}

		device.destroySemaphore(renderCompleteSemaphore, nullptr);
		device.destroySemaphore(presentCompleteSemaphore, nullptr);

		device.destroyPipeline(pipeline);
		device.destroyPipelineLayout(pipelineLayout, nullptr);
		device.destroyDescriptorSetLayout(descriptorSetLayout);

		device.freeMemory(vertices.memory, nullptr);
		device.destroyBuffer(vertices.buffer, nullptr);

		for(auto &framebuffer : framebuffers)
		{
			framebuffer.reset();
		}

		device.destroyRenderPass(renderPass, nullptr);

		swapchain.reset();
		window.reset();
	}

	void renderFrame()
	{
		swapchain->acquireNextImage(presentCompleteSemaphore, currentFrameBuffer);

		device.waitForFences(1, &waitFences[currentFrameBuffer], VK_TRUE, UINT64_MAX);
		device.resetFences(1, &waitFences[currentFrameBuffer]);

		vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submitInfo;
		submitInfo.pWaitDstStageMask = &waitStageMask;
		submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderCompleteSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrameBuffer];
		submitInfo.commandBufferCount = 1;

		queue.submit(1, &submitInfo, waitFences[currentFrameBuffer]);

		swapchain->queuePresent(queue, currentFrameBuffer, renderCompleteSemaphore);
	}

	void show()
	{
		window->show();
	}

private:
	void createSynchronizationPrimitives()
	{
		vk::SemaphoreCreateInfo semaphoreCreateInfo;
		presentCompleteSemaphore = device.createSemaphore(semaphoreCreateInfo);
		renderCompleteSemaphore = device.createSemaphore(semaphoreCreateInfo);

		vk::FenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		waitFences.resize(swapchain->imageCount());
		for(auto &fence : waitFences)
		{
			fence = device.createFence(fenceCreateInfo);
		}
	}

	void createCommandBuffers(vk::RenderPass renderPass)
	{
		vk::CommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
		commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		commandPool = device.createCommandPool(commandPoolCreateInfo);

		std::vector<vk::DescriptorSet> descriptorSets;
		if(descriptorSetLayout)
		{
			std::array<vk::DescriptorPoolSize, 1> poolSizes = {};
			poolSizes[0].type = vk::DescriptorType::eCombinedImageSampler;
			poolSizes[0].descriptorCount = 1;

			vk::DescriptorPoolCreateInfo poolInfo;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = 1;

			descriptorPool = device.createDescriptorPool(poolInfo);

			std::vector<vk::DescriptorSetLayout> layouts(1, descriptorSetLayout);
			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = layouts.data();

			descriptorSets = device.allocateDescriptorSets(allocInfo);

			doUpdateDescriptorSet(commandPool, descriptorSets[0]);
		}

		vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(swapchain->imageCount());
		commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;

		commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);

		for(size_t i = 0; i < commandBuffers.size(); i++)
		{
			vk::CommandBufferBeginInfo commandBufferBeginInfo;
			commandBuffers[i].begin(commandBufferBeginInfo);

			vk::ClearValue clearValues[1];
			clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{ 0.5f, 0.5f, 0.5f, 1.0f });

			vk::RenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo.framebuffer = framebuffers[i]->getFramebuffer();
			renderPassBeginInfo.renderPass = renderPass;
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent = windowSize;
			renderPassBeginInfo.clearValueCount = ARRAY_SIZE(clearValues);
			renderPassBeginInfo.pClearValues = clearValues;
			commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

			// Set dynamic state
			vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(windowSize.width), static_cast<float>(windowSize.height), 0.0f, 1.0f);
			commandBuffers[i].setViewport(0, 1, &viewport);

			vk::Rect2D scissor(vk::Offset2D(0, 0), windowSize);
			commandBuffers[i].setScissor(0, 1, &scissor);

			if(!descriptorSets.empty())
			{
				commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr);
			}

			// Draw
			commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
			VULKAN_HPP_NAMESPACE::DeviceSize offset = 0;
			commandBuffers[i].bindVertexBuffers(0, 1, &vertices.buffer, &offset);
			commandBuffers[i].draw(vertices.numVertices, 1, 0, 0);

			commandBuffers[i].endRenderPass();
			commandBuffers[i].end();
		}
	}

	void prepareVertices()
	{
		doCreateVertexBuffers();
	}

	void createFramebuffers(vk::RenderPass renderPass)
	{
		framebuffers.resize(swapchain->imageCount());

		for(size_t i = 0; i < framebuffers.size(); i++)
		{
			framebuffers[i].reset(new Framebuffer(device, swapchain->getImageView(i), swapchain->colorFormat, renderPass, swapchain->getExtent(), multisample));
		}
	}

	vk::RenderPass createRenderPass(vk::Format colorFormat)
	{
		std::vector<vk::AttachmentDescription> attachments(multisample ? 2 : 1);

		if(multisample)
		{
			// Color attachment
			attachments[0].format = colorFormat;
			attachments[0].samples = vk::SampleCountFlagBits::e4;
			attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
			attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
			attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachments[0].initialLayout = vk::ImageLayout::eUndefined;
			attachments[0].finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

			// Resolve attachment
			attachments[1].format = colorFormat;
			attachments[1].samples = vk::SampleCountFlagBits::e1;
			attachments[1].loadOp = vk::AttachmentLoadOp::eDontCare;
			attachments[1].storeOp = vk::AttachmentStoreOp::eStore;
			attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachments[1].initialLayout = vk::ImageLayout::eUndefined;
			attachments[1].finalLayout = vk::ImageLayout::ePresentSrcKHR;
		}
		else
		{
			attachments[0].format = colorFormat;
			attachments[0].samples = vk::SampleCountFlagBits::e1;
			attachments[0].loadOp = vk::AttachmentLoadOp::eDontCare;
			attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
			attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachments[0].initialLayout = vk::ImageLayout::eUndefined;
			attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
		}

		vk::AttachmentReference attachment0;
		attachment0.attachment = 0;
		attachment0.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentReference attachment1;
		attachment1.attachment = 1;
		attachment1.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpassDescription;
		subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pResolveAttachments = multisample ? &attachment1 : nullptr;
		subpassDescription.pColorAttachments = &attachment0;

		std::array<vk::SubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
		dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		dependencies[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
		dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
		dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
		dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		return device.createRenderPass(renderPassInfo);
	}

	vk::Pipeline createGraphicsPipeline(vk::RenderPass renderPass)
	{
		auto setLayoutBindings = doCreateDescriptorSetLayouts();

		std::vector<vk::DescriptorSetLayout> setLayouts;
		if(!setLayoutBindings.empty())
		{
			vk::DescriptorSetLayoutCreateInfo layoutInfo;
			layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
			layoutInfo.pBindings = setLayoutBindings.data();
			descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

			setLayouts.push_back(descriptorSetLayout);
		}

		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
		pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

		vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout;
		pipelineCreateInfo.renderPass = renderPass;

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
		inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;

		vk::PipelineRasterizationStateCreateInfo rasterizationState;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = vk::PolygonMode::eFill;
		rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
		rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		vk::PipelineColorBlendAttachmentState blendAttachmentState;
		blendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		blendAttachmentState.blendEnable = VK_FALSE;
		vk::PipelineColorBlendStateCreateInfo colorBlendState;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &blendAttachmentState;

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		std::vector<vk::DynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(vk::DynamicState::eViewport);
		dynamicStateEnables.push_back(vk::DynamicState::eScissor);
		vk::PipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

		vk::PipelineDepthStencilStateCreateInfo depthStencilState;
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		depthStencilState.depthCompareOp = vk::CompareOp::eLessOrEqual;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.back.failOp = vk::StencilOp::eKeep;
		depthStencilState.back.passOp = vk::StencilOp::eKeep;
		depthStencilState.back.compareOp = vk::CompareOp::eAlways;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = depthStencilState.back;

		vk::PipelineMultisampleStateCreateInfo multisampleState;
		multisampleState.rasterizationSamples = multisample ? vk::SampleCountFlagBits::e4 : vk::SampleCountFlagBits::e1;
		multisampleState.pSampleMask = nullptr;

		vk::ShaderModule vertexModule = doCreateVertexShader();
		vk::ShaderModule fragmentModule = doCreateFragmentShader();

		assert(vertexModule);    // TODO: if nullptr, use a default
		assert(fragmentModule);  // TODO: if nullptr, use a default

		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

		shaderStages[0].module = vertexModule;
		shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
		shaderStages[0].pName = "main";

		shaderStages[1].module = fragmentModule;
		shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
		shaderStages[1].pName = "main";

		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.pVertexInputState = &vertices.inputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.renderPass = renderPass;
		pipelineCreateInfo.pDynamicState = &dynamicState;

		auto pipeline = device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;

		device.destroyShaderModule(fragmentModule);
		device.destroyShaderModule(vertexModule);

		return pipeline;
	}

protected:
	/////////////////////////
	// Hooks
	/////////////////////////

	// Called from prepareVertices.
	// Child type may call addVertexBuffer() from this function.
	virtual void doCreateVertexBuffers() {}

	// Called from createGraphicsPipeline.
	// Child type may return vector of DescriptorSetLayoutBindings for which a DescriptorSetLayout
	// will be created and stored in this->descriptorSetLayout.
	virtual std::vector<vk::DescriptorSetLayoutBinding> doCreateDescriptorSetLayouts()
	{
		return {};
	}

	// Called from createGraphicsPipeline.
	// Child type may call createShaderModule() and return the result.
	virtual vk::ShaderModule doCreateVertexShader()
	{
		return nullptr;
	}

	// Called from createGraphicsPipeline.
	// Child type may call createShaderModule() and return the result.
	virtual vk::ShaderModule doCreateFragmentShader()
	{
		return nullptr;
	}

	// Called from createCommandBuffers.
	// Child type may create resources (addImage, addSampler, etc.), and make sure to
	// call device.updateDescriptorSets.
	virtual void doUpdateDescriptorSet(vk::CommandPool &commandPool, vk::DescriptorSet &descriptorSet)
	{
	}

	/////////////////////////
	// Resource Management
	/////////////////////////

	// Call from doCreateFragmentShader()
	vk::ShaderModule createShaderModule(const char *glslSource, EShLanguage glslLanguage)
	{
		auto spirv = Util::compileGLSLtoSPIRV(glslSource, glslLanguage);

		vk::ShaderModuleCreateInfo moduleCreateInfo;
		moduleCreateInfo.codeSize = spirv.size() * sizeof(uint32_t);
		moduleCreateInfo.pCode = (uint32_t *)spirv.data();

		return device.createShaderModule(moduleCreateInfo);
	}

	// Call from doCreateVertexBuffers()
	template<typename VertexType>
	void addVertexBuffer(VertexType *vertexBufferData, size_t vertexBufferDataSize, std::vector<vk::VertexInputAttributeDescription> inputAttributes)
	{
		assert(!vertices.buffer);  // For now, only support adding once

		vk::BufferCreateInfo vertexBufferInfo;
		vertexBufferInfo.size = vertexBufferDataSize;
		vertexBufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
		vertices.buffer = device.createBuffer(vertexBufferInfo);

		vk::MemoryAllocateInfo memoryAllocateInfo;
		vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(vertices.buffer);
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = Util::getMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vertices.memory = device.allocateMemory(memoryAllocateInfo);

		void *data = device.mapMemory(vertices.memory, 0, VK_WHOLE_SIZE);
		memcpy(data, vertexBufferData, vertexBufferDataSize);
		device.unmapMemory(vertices.memory);
		device.bindBufferMemory(vertices.buffer, vertices.memory, 0);

		vertices.inputBinding.binding = 0;
		vertices.inputBinding.stride = sizeof(VertexType);
		vertices.inputBinding.inputRate = vk::VertexInputRate::eVertex;

		vertices.inputAttributes = std::move(inputAttributes);

		vertices.inputState.vertexBindingDescriptionCount = 1;
		vertices.inputState.pVertexBindingDescriptions = &vertices.inputBinding;
		vertices.inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertices.inputAttributes.size());
		vertices.inputState.pVertexAttributeDescriptions = vertices.inputAttributes.data();

		// Note that we assume data is tightly packed
		vertices.numVertices = static_cast<uint32_t>(vertexBufferDataSize / sizeof(VertexType));
	}

	template<typename T>
	struct Resource
	{
		size_t id;
		T &obj;
	};

	template<typename... Args>
	Resource<Image> addImage(Args &&... args)
	{
		images.emplace_back(std::make_unique<Image>(std::forward<Args>(args)...));
		return { images.size() - 1, *images.back() };
	}

	Image &getImageById(size_t id)
	{
		return *images[id].get();
	}

	Resource<vk::Sampler> addSampler(const vk::SamplerCreateInfo &samplerCreateInfo)
	{
		auto sampler = device.createSampler(samplerCreateInfo);
		samplers.push_back(sampler);
		return { samplers.size() - 1, sampler };
	}

	vk::Sampler &getSamplerById(size_t id)
	{
		return samplers[id];
	}

private:
	const vk::Extent2D windowSize = { 1280, 720 };
	const bool multisample;

	std::unique_ptr<Window> window;
	std::unique_ptr<Swapchain> swapchain;

	vk::RenderPass renderPass;  // Owning handle
	std::vector<std::unique_ptr<Framebuffer>> framebuffers;
	uint32_t currentFrameBuffer = 0;

	struct VertexBuffer
	{
		vk::Buffer buffer;        // Owning handle
		vk::DeviceMemory memory;  // Owning handle

		vk::VertexInputBindingDescription inputBinding;
		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
		vk::PipelineVertexInputStateCreateInfo inputState;

		uint32_t numVertices;
	} vertices;

	vk::DescriptorSetLayout descriptorSetLayout;  // Owning handle
	vk::PipelineLayout pipelineLayout;            // Owning handle
	vk::Pipeline pipeline;                        // Owning handle

	vk::Semaphore presentCompleteSemaphore;  // Owning handle
	vk::Semaphore renderCompleteSemaphore;   // Owning handle
	std::vector<vk::Fence> waitFences;       // Owning handles

	vk::CommandPool commandPool;        // Owning handle
	vk::DescriptorPool descriptorPool;  // Owning handle

	// Resources
	std::vector<std::unique_ptr<Image>> images;
	std::vector<vk::Sampler> samplers;  // Owning handles

	std::vector<vk::CommandBuffer> commandBuffers;  // Owning handles
};

class TriangleSolidColorBenchmark : public DrawBenchmark
{
public:
	TriangleSolidColorBenchmark(Multisample multisample)
	    : DrawBenchmark(multisample)
	{}

protected:
	void doCreateVertexBuffers() override
	{
		struct Vertex
		{
			float position[3];
		};

		Vertex vertexBufferData[] = {
			{ { 1.0f, 1.0f, 0.5f } },
			{ { -1.0f, 1.0f, 0.5f } },
			{ { 0.0f, -1.0f, 0.5f } }
		};

		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
		inputAttributes.push_back(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)));

		addVertexBuffer(vertexBufferData, sizeof(vertexBufferData), std::move(inputAttributes));
	}

	vk::ShaderModule doCreateVertexShader() override
	{
		const char *vertexShader = R"(#version 310 es
			layout(location = 0) in vec3 inPos;

			void main()
			{
				gl_Position = vec4(inPos.xyz, 1.0);
			})";

		return createShaderModule(vertexShader, EShLanguage::EShLangVertex);
	}

	vk::ShaderModule doCreateFragmentShader() override
	{
		const char *fragmentShader = R"(#version 310 es
			precision highp float;

			layout(location = 0) out vec4 outColor;

			void main()
			{
				outColor = vec4(1.0, 1.0, 1.0, 1.0);
			})";

		return createShaderModule(fragmentShader, EShLanguage::EShLangFragment);
	}
};

class TriangleInterpolateColorBenchmark : public DrawBenchmark
{
public:
	TriangleInterpolateColorBenchmark(Multisample multisample)
	    : DrawBenchmark(multisample)
	{}

protected:
	void doCreateVertexBuffers() override
	{
		struct Vertex
		{
			float position[3];
			float color[3];
		};

		Vertex vertexBufferData[] = {
			{ { 1.0f, 1.0f, 0.05f }, { 1.0f, 0.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -1.0f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
		};

		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
		inputAttributes.push_back(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)));
		inputAttributes.push_back(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)));

		addVertexBuffer(vertexBufferData, sizeof(vertexBufferData), std::move(inputAttributes));
	}

	vk::ShaderModule doCreateVertexShader() override
	{
		const char *vertexShader = R"(#version 310 es
			layout(location = 0) in vec3 inPos;
			layout(location = 1) in vec3 inColor;

			layout(location = 0) out vec3 outColor;

			void main()
			{
				outColor = inColor;
				gl_Position = vec4(inPos.xyz, 1.0);
			})";

		return createShaderModule(vertexShader, EShLanguage::EShLangVertex);
	}

	vk::ShaderModule doCreateFragmentShader() override
	{
		const char *fragmentShader = R"(#version 310 es
			precision highp float;

			layout(location = 0) in vec3 inColor;

			layout(location = 0) out vec4 outColor;

			void main()
			{
				outColor = vec4(inColor, 1.0);
			})";

		return createShaderModule(fragmentShader, EShLanguage::EShLangFragment);
	}
};

class TriangleSampleTextureBenchmark : public DrawBenchmark
{
public:
	TriangleSampleTextureBenchmark(Multisample multisample)
	    : DrawBenchmark(multisample)
	{}

protected:
	void doCreateVertexBuffers() override
	{
		struct Vertex
		{
			float position[3];
			float color[3];
			float texCoord[2];
		};

		Vertex vertexBufferData[] = {
			{ { 1.0f, 1.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
			{ { 0.0f, -1.0f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }
		};

		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
		inputAttributes.push_back(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)));
		inputAttributes.push_back(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)));
		inputAttributes.push_back(vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord)));

		addVertexBuffer(vertexBufferData, sizeof(vertexBufferData), std::move(inputAttributes));
	}

	vk::ShaderModule doCreateVertexShader() override
	{
		const char *vertexShader = R"(#version 310 es
			layout(location = 0) in vec3 inPos;
			layout(location = 1) in vec3 inColor;

			layout(location = 0) out vec3 outColor;
			layout(location = 1) out vec2 fragTexCoord;

			void main()
			{
				outColor = inColor;
				gl_Position = vec4(inPos.xyz, 1.0);
				fragTexCoord = inPos.xy;
			})";

		return createShaderModule(vertexShader, EShLanguage::EShLangVertex);
	}

	vk::ShaderModule doCreateFragmentShader() override
	{
		const char *fragmentShader = R"(#version 310 es
			precision highp float;

			layout(location = 0) in vec3 inColor;
			layout(location = 1) in vec2 fragTexCoord;

			layout(location = 0) out vec4 outColor;

			layout(binding = 0) uniform sampler2D texSampler;

			void main()
			{
				outColor = texture(texSampler, fragTexCoord) * vec4(inColor, 1.0);
			})";

		return createShaderModule(fragmentShader, EShLanguage::EShLangFragment);
	}

	std::vector<vk::DescriptorSetLayoutBinding> doCreateDescriptorSetLayouts() override
	{
		vk::DescriptorSetLayoutBinding samplerLayoutBinding;
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		return { samplerLayoutBinding };
	}

	void doUpdateDescriptorSet(vk::CommandPool &commandPool, vk::DescriptorSet &descriptorSet) override
	{
		auto &texture = addImage(device, 16, 16, vk::Format::eR8G8B8A8Unorm).obj;

		// Fill texture with white
		vk::DeviceSize bufferSize = 16 * 16 * 4;
		Buffer buffer(device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc);
		void *data = buffer.mapMemory();
		memset(data, 255, bufferSize);
		buffer.unmapMemory();

		Util::transitionImageLayout(device, commandPool, queue, texture.getImage(), vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		Util::copyBufferToImage(device, commandPool, queue, buffer.getBuffer(), texture.getImage(), 16, 16);
		Util::transitionImageLayout(device, commandPool, queue, texture.getImage(), vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		vk::SamplerCreateInfo samplerInfo;
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		auto sampler = addSampler(samplerInfo);

		vk::DescriptorImageInfo imageInfo;
		imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView = texture.getImageView();
		imageInfo.sampler = sampler.obj;

		std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};

		descriptorWrites[0].dstSet = descriptorSet;
		descriptorWrites[0].dstBinding = 1;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &imageInfo;

		device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
};

template<typename T>
static void RunBenchmark(benchmark::State &state, T &benchmark)
{
	benchmark.initialize();

	if(false) benchmark.show();  // Enable for visual verification.

	// Warmup
	benchmark.renderFrame();

	for(auto _ : state)
	{
		benchmark.renderFrame();
	}
}

static void TriangleSolidColor(benchmark::State &state, Multisample multisample)
{
	TriangleSolidColorBenchmark benchmark(multisample);
	RunBenchmark(state, benchmark);
}

static void TriangleInterpolateColor(benchmark::State &state, Multisample multisample)
{
	TriangleInterpolateColorBenchmark benchmark(multisample);
	RunBenchmark(state, benchmark);
}

static void TriangleSampleTexture(benchmark::State &state, Multisample multisample)
{
	TriangleSampleTextureBenchmark benchmark(multisample);
	RunBenchmark(state, benchmark);
}

BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_R8G8B8A8_UNORM, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_R32_SFLOAT, vk::Format::eR32Sfloat, vk::ImageAspectFlagBits::eColor)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_D32_SFLOAT, vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(TriangleSolidColor, TriangleSolidColor, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleInterpolateColor, TriangleInterpolateColor, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSampleTexture, TriangleSampleTexture, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSolidColor, TriangleSolidColor_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleInterpolateColor, TriangleInterpolateColor_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSampleTexture, TriangleSampleTexture_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
