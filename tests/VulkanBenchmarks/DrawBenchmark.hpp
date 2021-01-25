// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#ifndef DRAW_BENCHMARK_HPP_
#define DRAW_BENCHMARK_HPP_

#include "Framebuffer.hpp"
#include "Image.hpp"
#include "Swapchain.hpp"
#include "Util.hpp"
#include "VulkanBenchmark.hpp"
#include "Window.hpp"

enum class Multisample
{
	False,
	True
};

class DrawBenchmark : public VulkanBenchmark
{
public:
	DrawBenchmark(Multisample multisample);
	~DrawBenchmark();

	void initialize();
	void renderFrame();
	void show();

private:
	void createSynchronizationPrimitives();
	void createCommandBuffers(vk::RenderPass renderPass);
	void prepareVertices();
	void createFramebuffers(vk::RenderPass renderPass);
	vk::RenderPass createRenderPass(vk::Format colorFormat);
	vk::Pipeline createGraphicsPipeline(vk::RenderPass renderPass);
	void addVertexBuffer(void *vertexBufferData, size_t vertexBufferDataSize, size_t vertexSize, std::vector<vk::VertexInputAttributeDescription> inputAttributes);

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
	vk::ShaderModule createShaderModule(const char *glslSource, EShLanguage glslLanguage);

	// Call from doCreateVertexBuffers()
	template<typename VertexType>
	void addVertexBuffer(VertexType *vertexBufferData, size_t vertexBufferDataSize, std::vector<vk::VertexInputAttributeDescription> inputAttributes)
	{
		addVertexBuffer(vertexBufferData, vertexBufferDataSize, sizeof(VertexType), std::move(inputAttributes));
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

#endif  // DRAW_BENCHMARK_HPP_
