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

#include "Buffer.hpp"
#include "DrawBenchmark.hpp"
#include "benchmark/benchmark.h"

#include <cassert>
#include <vector>

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

BENCHMARK_CAPTURE(TriangleSolidColor, TriangleSolidColor, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleInterpolateColor, TriangleInterpolateColor, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSampleTexture, TriangleSampleTexture, Multisample::False)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSolidColor, TriangleSolidColor_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleInterpolateColor, TriangleInterpolateColor_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(TriangleSampleTexture, TriangleSampleTexture_Multisample, Multisample::True)->Unit(benchmark::kMillisecond);
