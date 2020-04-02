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

#ifndef VK_PIPELINE_HPP_
#define VK_PIPELINE_HPP_

#include "VkObject.hpp"
#include "Device/Renderer.hpp"
#include "Vulkan/VkDescriptorSet.hpp"
#include "Vulkan/VkPipelineCache.hpp"
#include <memory>

namespace sw {

class ComputeProgram;
class SpirvShader;

}  // namespace sw

namespace vk {

namespace dbg {
class Context;
}  // namespace dbg

class PipelineCache;
class PipelineLayout;
class ShaderModule;
class Device;

class Pipeline
{
public:
	Pipeline(PipelineLayout const *layout, const Device *device);
	virtual ~Pipeline() = default;

	operator VkPipeline()
	{
		return vk::TtoVkT<Pipeline, VkPipeline>(this);
	}

	static inline Pipeline *Cast(VkPipeline object)
	{
		return vk::VkTtoT<Pipeline, VkPipeline>(object);
	}

	void destroy(const VkAllocationCallbacks *pAllocator)
	{
		destroyPipeline(pAllocator);
	}

	virtual void destroyPipeline(const VkAllocationCallbacks *pAllocator) = 0;
#ifndef NDEBUG
	virtual VkPipelineBindPoint bindPoint() const = 0;
#endif

	PipelineLayout const *getLayout() const
	{
		return layout;
	}

protected:
	PipelineLayout const *layout = nullptr;
	Device const *const device;

	const bool robustBufferAccess = true;
};

class GraphicsPipeline : public Pipeline, public ObjectBase<GraphicsPipeline, VkPipeline>
{
public:
	GraphicsPipeline(const VkGraphicsPipelineCreateInfo *pCreateInfo,
	                 void *mem,
	                 const Device *device);
	virtual ~GraphicsPipeline() = default;

	void destroyPipeline(const VkAllocationCallbacks *pAllocator) override;

#ifndef NDEBUG
	VkPipelineBindPoint bindPoint() const override
	{
		return VK_PIPELINE_BIND_POINT_GRAPHICS;
	}
#endif

	static size_t ComputeRequiredAllocationSize(const VkGraphicsPipelineCreateInfo *pCreateInfo);

	void compileShaders(const VkAllocationCallbacks *pAllocator, const VkGraphicsPipelineCreateInfo *pCreateInfo, PipelineCache *pipelineCache);

	uint32_t computePrimitiveCount(uint32_t vertexCount) const;
	const sw::Context &getContext() const;
	const VkRect2D &getScissor() const;
	const VkViewport &getViewport() const;
	const sw::float4 &getBlendConstants() const;
	bool hasDynamicState(VkDynamicState dynamicState) const;
	bool hasPrimitiveRestartEnable() const { return primitiveRestartEnable; }

private:
	void setShader(const VkShaderStageFlagBits &stage, const std::shared_ptr<sw::SpirvShader> spirvShader);
	const std::shared_ptr<sw::SpirvShader> getShader(const VkShaderStageFlagBits &stage) const;
	std::shared_ptr<sw::SpirvShader> vertexShader;
	std::shared_ptr<sw::SpirvShader> fragmentShader;

	uint32_t dynamicStateFlags = 0;
	bool primitiveRestartEnable = false;
	sw::Context context;
	VkRect2D scissor;
	VkViewport viewport;
	sw::float4 blendConstants;
};

class ComputePipeline : public Pipeline, public ObjectBase<ComputePipeline, VkPipeline>
{
public:
	ComputePipeline(const VkComputePipelineCreateInfo *pCreateInfo, void *mem, const Device *device);
	virtual ~ComputePipeline() = default;

	void destroyPipeline(const VkAllocationCallbacks *pAllocator) override;

#ifndef NDEBUG
	VkPipelineBindPoint bindPoint() const override
	{
		return VK_PIPELINE_BIND_POINT_COMPUTE;
	}
#endif

	static size_t ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo *pCreateInfo);

	void compileShaders(const VkAllocationCallbacks *pAllocator, const VkComputePipelineCreateInfo *pCreateInfo, PipelineCache *pipelineCache);

	void run(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
	         uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ,
	         vk::DescriptorSet::Bindings const &descriptorSets,
	         vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
	         sw::PushConstantStorage const &pushConstants);

protected:
	std::shared_ptr<sw::SpirvShader> shader;
	std::shared_ptr<sw::ComputeProgram> program;
};

static inline Pipeline *Cast(VkPipeline object)
{
	return Pipeline::Cast(object);
}

}  // namespace vk

#endif  // VK_PIPELINE_HPP_
