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

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "SPIRV/GlslangToSpv.h"
#include "StandAlone/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cassert>
#include <vector>

class VulkanBenchmark
{
public:
	VulkanBenchmark()
	{
		// TODO(b/158231104): Other platforms
		dl = std::make_unique<vk::DynamicLoader>("./vk_swiftshader.dll");
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

	virtual ~VulkanBenchmark()
	{
		device.waitIdle();
		device.destroy();
		instance.destroy();
	}

protected:
	uint32_t getMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = vk::PhysicalDevice::GetMemoryProperties();
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if((typeBits & 1) == 1)
			{
				if((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}
			typeBits >>= 1;
		}

		assert(false);
		return -1;
	}

	const uint32_t queueFamilyIndex = 0;

	vk::Instance instance;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue queue;

private:
	std::unique_ptr<vk::DynamicLoader> dl;
};

class ClearImageBenchmark : public VulkanBenchmark
{
public:
	ClearImageBenchmark(vk::Format clearFormat, vk::ImageAspectFlagBits clearAspect)
	{
		vk::ImageCreateInfo imageInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.format = clearFormat;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eGeneral;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
		imageInfo.samples = vk::SampleCountFlagBits::e4;
		imageInfo.extent = { 1024, 1024, 1 };
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
		device.freeCommandBuffers(commandPool, { commandBuffer });
		device.destroyCommandPool(commandPool);
		device.freeMemory(memory);
		device.destroyImage(image);
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
	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;

	vk::Image image;
	vk::DeviceMemory memory;
};

static void ClearImage(benchmark::State &state, vk::Format clearFormat, vk::ImageAspectFlagBits clearAspect)
{
	ClearImageBenchmark benchmark(clearFormat, clearAspect);

	// Execute once to have the Reactor routine generated.
	benchmark.clear();

	for(auto _ : state)
	{
		benchmark.clear();
	}
}
BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_R8G8B8A8_UNORM, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_R32_SFLOAT, vk::Format::eR32Sfloat, vk::ImageAspectFlagBits::eColor)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(ClearImage, VK_FORMAT_D32_SFLOAT, vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth)->Unit(benchmark::kMillisecond);

class Window
{
public:
	Window(vk::Instance instance, vk::Extent2D windowSize)
	{
		moduleInstance = GetModuleHandle(NULL);

		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = DefWindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = moduleInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = "Window";
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		RegisterClassEx(&windowClass);

		DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		DWORD extendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = (long)windowSize.width;
		windowRect.bottom = (long)windowSize.height;

		AdjustWindowRectEx(&windowRect, style, FALSE, extendedStyle);
		uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
		uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;

		window = CreateWindowEx(extendedStyle, "Window", "Hello",
		                        style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		                        x, y,
		                        windowRect.right - windowRect.left,
		                        windowRect.bottom - windowRect.top,
		                        NULL, NULL, moduleInstance, NULL);

		SetForegroundWindow(window);
		SetFocus(window);

		// Create the Vulkan surface
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.hinstance = moduleInstance;
		surfaceCreateInfo.hwnd = window;
		surface = instance.createWin32SurfaceKHR(surfaceCreateInfo);
		assert(surface);
	}

	~Window()
	{
		instance.destroySurfaceKHR(surface);

		DestroyWindow(window);
		UnregisterClass("Window", moduleInstance);
	}

	vk::SurfaceKHR getSurface()
	{
		return surface;
	}

	void show()
	{
		ShowWindow(window, SW_SHOW);
	}

private:
	HWND window;
	HINSTANCE moduleInstance;
	WNDCLASSEX windowClass;

	const vk::Instance instance;
	vk::SurfaceKHR surface;
};

class Swapchain
{
public:
	Swapchain(vk::PhysicalDevice physicalDevice, vk::Device device, Window *window)
	    : device(device)
	{
		vk::SurfaceKHR surface = window->getSurface();

		// Create the swapchain
		vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		vk::SwapchainCreateInfoKHR swapchainCreateInfo;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = 2;  // double-buffered
		swapchainCreateInfo.imageFormat = colorFormat;
		swapchainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
		swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
		swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
		swapchainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		swapchainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		swapchain = device.createSwapchainKHR(swapchainCreateInfo);

		// Obtain the images and create views for them
		images = device.getSwapchainImagesKHR(swapchain);

		imageViews.resize(images.size());
		for(size_t i = 0; i < imageViews.size(); i++)
		{
			vk::ImageViewCreateInfo colorAttachmentView;
			colorAttachmentView.image = images[i];
			colorAttachmentView.viewType = vk::ImageViewType::e2D;
			colorAttachmentView.format = colorFormat;
			colorAttachmentView.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;

			imageViews[i] = device.createImageView(colorAttachmentView);
		}
	}

	~Swapchain()
	{
		for(auto &imageView : imageViews)
		{
			device.destroyImageView(imageView);
		}

		device.destroySwapchainKHR(swapchain);
	}

	void acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t &imageIndex)
	{
		auto result = device.acquireNextImageKHR(swapchain, UINT64_MAX, presentCompleteSemaphore, vk::Fence());
		imageIndex = result.value;
	}

	void queuePresent(vk::Queue queue, uint32_t imageIndex, vk::Semaphore waitSemaphore)
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		queue.presentKHR(presentInfo);
	}

	size_t imageCount() const
	{
		return images.size();
	}

	vk::ImageView getImageView(size_t i) const
	{
		return imageViews[i];
	}

	const vk::Format colorFormat = vk::Format::eB8G8R8A8Unorm;

private:
	const vk::Device device;

	vk::SwapchainKHR swapchain;

	std::vector<vk::Image> images;  // Weak pointers. Presentable images owned by swapchain object.
	std::vector<vk::ImageView> imageViews;
};

struct Image
{
	vk::Image image;
	vk::DeviceMemory imageMemory;
	vk::ImageView imageView;
};

class Framebuffer
{
public:
	Framebuffer(vk::Device device, vk::ImageView attachment, vk::Format colorFormat, vk::RenderPass renderPass, uint32_t width, uint32_t height, bool multisample)
	    : device(device)
	{
		std::vector<vk::ImageView> attachments(multisample ? 2 : 1);

		if(multisample)
		{
			// Create multisample images
			vk::ImageCreateInfo imageInfo;
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.format = colorFormat;
			imageInfo.tiling = vk::ImageTiling::eOptimal;
			imageInfo.initialLayout = vk::ImageLayout::eGeneral;
			imageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment;
			imageInfo.samples = vk::SampleCountFlagBits::e4;
			imageInfo.extent = { width, height, 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;

			multisampleImage.image = device.createImage(imageInfo);

			vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(multisampleImage.image);

			vk::MemoryAllocateInfo allocateInfo;
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = 0;  //getMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

			multisampleImage.imageMemory = device.allocateMemory(allocateInfo);

			device.bindImageMemory(multisampleImage.image, multisampleImage.imageMemory, 0);

			vk::ImageViewCreateInfo colorAttachmentView;
			colorAttachmentView.image = multisampleImage.image;
			colorAttachmentView.viewType = vk::ImageViewType::e2D;
			colorAttachmentView.format = colorFormat;
			colorAttachmentView.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;

			multisampleImage.imageView = device.createImageView(colorAttachmentView);

			// We'll be rendering to attachment location 0
			attachments[0] = multisampleImage.imageView;
			attachments[1] = attachment;  // Resolve attachment
		}
		else
		{
			attachments[0] = attachment;
		}

		vk::FramebufferCreateInfo framebufferCreateInfo;

		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		framebuffer = device.createFramebuffer(framebufferCreateInfo);
	}

	~Framebuffer()
	{
		device.destroyFramebuffer(framebuffer);

		device.destroyImage(multisampleImage.image);
		device.destroyImageView(multisampleImage.imageView);
		device.freeMemory(multisampleImage.imageMemory);
	}

	vk::Framebuffer getFramebuffer()
	{
		return framebuffer;
	}

private:
	vk::Device device;

	vk::Framebuffer framebuffer;

	Image multisampleImage;
};

static std::vector<uint32_t> compileGLSLtoSPIRV(const char *glslSource, EShLanguage glslLanguage)
{
	// glslang requires one-time initialization.
	const struct GlslangProcessInitialiser
	{
		GlslangProcessInitialiser() { glslang::InitializeProcess(); }
		~GlslangProcessInitialiser() { glslang::FinalizeProcess(); }
	} glslangInitialiser;

	std::unique_ptr<glslang::TShader> glslangShader = std::make_unique<glslang::TShader>(glslLanguage);

	glslangShader->setStrings(&glslSource, 1);
	glslangShader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	glslangShader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

	const int defaultVersion = 100;
	EShMessages messages = static_cast<EShMessages>(EShMessages::EShMsgDefault | EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules);
	bool parseResult = glslangShader->parse(&glslang::DefaultTBuiltInResource, defaultVersion, false, messages);

	if(!parseResult)
	{
		std::string debugLog = glslangShader->getInfoDebugLog();
		std::string infoLog = glslangShader->getInfoLog();
		assert(false);
	}

	glslang::TIntermediate *intermediateRepresentation = glslangShader->getIntermediate();
	assert(intermediateRepresentation);

	std::vector<uint32_t> spirv;
	glslang::SpvOptions options;
	glslang::GlslangToSpv(*intermediateRepresentation, spirv, &options);
	assert(spirv.size() != 0);

	return spirv;
}

class TriangleBenchmark : public VulkanBenchmark
{
public:
	TriangleBenchmark(bool multisample)
	    : multisample(multisample)
	{
		window = new Window(instance, windowSize);
		swapchain = new Swapchain(physicalDevice, device, window);

		renderPass = createRenderPass(swapchain->colorFormat);
		createFramebuffers(renderPass);

		prepareVertices();

		pipeline = createGraphicsPipeline(renderPass);

		createSynchronizationPrimitives();

		createCommandBuffers(renderPass);
	}

	~TriangleBenchmark()
	{
		device.destroyPipelineLayout(pipelineLayout);
		device.destroyPipelineCache(pipelineCache);

		device.destroyBuffer(vertices.buffer);
		device.freeMemory(vertices.memory);

		device.destroySemaphore(presentCompleteSemaphore);
		device.destroySemaphore(renderCompleteSemaphore);

		for(auto &fence : waitFences)
		{
			device.destroyFence(fence);
		}

		for(auto *framebuffer : framebuffers)
		{
			delete framebuffer;
		}

		device.destroyRenderPass(renderPass);

		device.freeCommandBuffers(commandPool, commandBuffers);
		device.destroyCommandPool(commandPool);

		delete swapchain;
		delete window;
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

protected:
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

		vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.commandBufferCount = swapchain->imageCount();
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
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
			commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

			// Set dynamic state
			vk::Viewport viewport(0.0f, 0.0f, windowSize.width, windowSize.height, 0.0f, 1.0f);
			commandBuffers[i].setViewport(0, 1, &viewport);

			vk::Rect2D scissor(vk::Offset2D(0, 0), windowSize);
			commandBuffers[i].setScissor(0, 1, &scissor);

			// Draw a triangle
			commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
			commandBuffers[i].bindVertexBuffers(0, { vertices.buffer }, { 0 });
			commandBuffers[i].draw(3, 1, 0, 0);

			commandBuffers[i].endRenderPass();
			commandBuffers[i].end();
		}
	}

	void prepareVertices()
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

		vk::BufferCreateInfo vertexBufferInfo;
		vertexBufferInfo.size = sizeof(vertexBufferData);
		vertexBufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
		vertices.buffer = device.createBuffer(vertexBufferInfo);

		vk::MemoryAllocateInfo memoryAllocateInfo;
		vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(vertices.buffer);
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = getMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vk::DeviceMemory vertexBufferMemory = device.allocateMemory(memoryAllocateInfo);

		void *data = device.mapMemory(vertexBufferMemory, 0, VK_WHOLE_SIZE);
		memcpy(data, vertexBufferData, sizeof(vertexBufferData));
		device.unmapMemory(vertexBufferMemory);
		device.bindBufferMemory(vertices.buffer, vertexBufferMemory, 0);

		vertices.inputBinding.binding = 0;
		vertices.inputBinding.stride = sizeof(Vertex);
		vertices.inputBinding.inputRate = vk::VertexInputRate::eVertex;

		vertices.inputAttributes.push_back(vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)));
		vertices.inputAttributes.push_back(vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)));

		vertices.inputState.vertexBindingDescriptionCount = 1;
		vertices.inputState.pVertexBindingDescriptions = &vertices.inputBinding;
		vertices.inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertices.inputAttributes.size());
		vertices.inputState.pVertexAttributeDescriptions = vertices.inputAttributes.data();
	}

	void createFramebuffers(vk::RenderPass renderPass)
	{
		framebuffers.resize(swapchain->imageCount());

		for(size_t i = 0; i < framebuffers.size(); i++)
		{
			framebuffers[i] = new Framebuffer(device, swapchain->getImageView(i), swapchain->colorFormat, renderPass, windowSize.width, windowSize.height, multisample);
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

	vk::UniqueShaderModule createShaderModule(const char *glslSource, EShLanguage glslLanguage)
	{
		auto spirv = compileGLSLtoSPIRV(glslSource, glslLanguage);

		vk::ShaderModuleCreateInfo moduleCreateInfo;
		moduleCreateInfo.codeSize = spirv.size() * sizeof(uint32_t);
		moduleCreateInfo.pCode = (uint32_t *)spirv.data();

		return device.createShaderModuleUnique(moduleCreateInfo);
	}

	vk::UniquePipeline createGraphicsPipeline(vk::RenderPass renderPass)
	{
		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
		pPipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayout = device.createPipelineLayout(pPipelineLayoutCreateInfo);

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

		const char *vertexShader = R"(#version 310 es
			layout(location = 0) in vec3 inPos;
			layout(location = 1) in vec3 inColor;
			layout(location = 0) out vec3 outColor;

			void main()
			{
				outColor = inColor;
				gl_Position = vec4(inPos.xyz, 1.0);
			}
		)";

		const char *fragmentShader = R"(#version 310 es
			precision highp float;

			layout(location = 0) in vec3 inColor;
			layout(location = 0) out vec4 outColor;

			void main()
			{
				outColor = vec4(inColor, 1.0);
			}
		)";

		vk::UniqueShaderModule vertexModule = createShaderModule(vertexShader, EShLanguage::EShLangVertex);
		vk::UniqueShaderModule fragmentModule = createShaderModule(fragmentShader, EShLanguage::EShLangFragment);

		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

		shaderStages[0].module = vertexModule.get();
		shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
		shaderStages[0].pName = "main";

		shaderStages[1].module = fragmentModule.get();
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

		return device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
	}

	const vk::Extent2D windowSize = { 1280, 720 };
	const bool multisample;
	Window *window = nullptr;

	Swapchain *swapchain = nullptr;

	vk::RenderPass renderPass;
	std::vector<Framebuffer *> framebuffers;
	uint32_t currentFrameBuffer = 0;

	struct VertexBuffer
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;

		vk::PipelineVertexInputStateCreateInfo inputState;
		vk::VertexInputBindingDescription inputBinding;
		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
	} vertices;

	vk::PipelineLayout pipelineLayout;
	vk::PipelineCache pipelineCache;
	vk::UniquePipeline pipeline;

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::Semaphore presentCompleteSemaphore;
	vk::Semaphore renderCompleteSemaphore;

	std::vector<vk::Fence> waitFences;
};

static void Triangle(benchmark::State &state, bool multisample)
{
	TriangleBenchmark benchmark(multisample);

	if(false) benchmark.show();  // Enable for visual verification.

	// Warmup
	benchmark.renderFrame();

	for(auto _ : state)
	{
		benchmark.renderFrame();
	}
}
BENCHMARK_CAPTURE(Triangle, Hello, false)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(Triangle, Multisample, true)->Unit(benchmark::kMillisecond);
