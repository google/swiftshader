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

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <cassert>
#include <vector>

class VulkanBenchmark : public benchmark::Fixture
{
public:
	void SetUp(::benchmark::State &state) override
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

		const float defaultQueuePriority = 0.0f;
		vk::DeviceQueueCreateInfo queueCreatInfo;
		queueCreatInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreatInfo.queueCount = 1;
		queueCreatInfo.pQueuePriorities = &defaultQueuePriority;

		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreatInfo;

		device = physicalDevices[0].createDevice(deviceCreateInfo, nullptr);

		queue = device.getQueue(queueFamilyIndex, 0);
	}

	void TearDown(const ::benchmark::State &state) override
	{
		device.waitIdle();
		device.destroy();
		instance.destroy();
	}

protected:
	const uint32_t queueFamilyIndex = 0;

	vk::Instance instance;
	vk::Device device;
	vk::Queue queue;

private:
	std::unique_ptr<vk::DynamicLoader> dl;
};

struct ClearBenchmarkVariant
{
	vk::Format format;
	const char *formatName;
	vk::ImageAspectFlags aspect;
};

const std::vector<ClearBenchmarkVariant> clearBenchmarkVariants = {
	{ vk::Format::eR8G8B8A8Unorm, "VK_FORMAT_R8G8B8A8_UNORM", vk::ImageAspectFlagBits::eColor },
	{ vk::Format::eR32Sfloat, "VK_FORMAT_R32_SFLOAT", vk::ImageAspectFlagBits::eColor },
	{ vk::Format::eD32Sfloat, "VK_FORMAT_D32_SFLOAT", vk::ImageAspectFlagBits::eDepth },
};

class ClearImageBenchmark : public VulkanBenchmark
{
public:
	void SetUp(::benchmark::State &state) override
	{
		VulkanBenchmark::SetUp(state);

		benchmark = clearBenchmarkVariants[state.range(0)];
		state.counters[benchmark.formatName] = 1;  // Small hack to display the format's name.
	}

	void TearDown(const ::benchmark::State &state) override
	{
		VulkanBenchmark::TearDown(state);
	}

protected:
	ClearBenchmarkVariant benchmark;
};

BENCHMARK_DEFINE_F(ClearImageBenchmark, Clear)
(benchmark::State &state)
{
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.format = benchmark.format;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = vk::ImageLayout::eGeneral;
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
	imageInfo.samples = vk::SampleCountFlagBits::e4;
	imageInfo.extent = { 1024, 1024, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;

	vk::Image image = device.createImage(imageInfo);

	vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocateInfo;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = 0;

	vk::DeviceMemory memory = device.allocateMemory(allocateInfo);

	device.bindImageMemory(image, memory, 0);

	vk::CommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

	vk::CommandPool commandPool = device.createCommandPool(commandPoolCreateInfo);

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = {};

	commandBuffer.begin(commandBufferBeginInfo);

	vk::ImageSubresourceRange range;
	range.aspectMask = benchmark.aspect;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.baseArrayLayer = 0;
	range.layerCount = 1;

	if(benchmark.aspect == vk::ImageAspectFlagBits::eColor)
	{
		vk::ClearColorValue clearColorValue;
		clearColorValue.float32[0] = 0.0f;
		clearColorValue.float32[1] = 1.0f;
		clearColorValue.float32[2] = 0.0f;
		clearColorValue.float32[3] = 1.0f;

		commandBuffer.clearColorImage(image, vk::ImageLayout::eGeneral, &clearColorValue, 1, &range);
	}
	else if(benchmark.aspect == vk::ImageAspectFlagBits::eDepth)
	{
		vk::ClearDepthStencilValue clearDepthStencilValue;
		clearDepthStencilValue.depth = 1.0f;
		clearDepthStencilValue.stencil = 0xFF;

		commandBuffer.clearDepthStencilImage(image, vk::ImageLayout::eGeneral, &clearDepthStencilValue, 1, &range);
	}
	else
		assert(false);

	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// Execute once to have the Reactor routine generated.
	queue.submit(1, &submitInfo, nullptr);
	queue.waitIdle();

	for(auto _ : state)
	{
		queue.submit(1, &submitInfo, nullptr);
		queue.waitIdle();
	}

	device.freeCommandBuffers(commandPool, { commandBuffer });
	device.destroyCommandPool(commandPool);
	device.freeMemory(memory);
	device.destroyImage(image);
}
BENCHMARK_REGISTER_F(ClearImageBenchmark, Clear)
    ->Unit(benchmark::kMillisecond)
    ->DenseRange(0, clearBenchmarkVariants.size() - 1, 1);
