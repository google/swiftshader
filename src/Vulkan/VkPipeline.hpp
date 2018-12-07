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

namespace sw { class SpirvShader; }

namespace vk
{

class Pipeline
{
public:
	operator VkPipeline()
	{
		return reinterpret_cast<VkPipeline>(this);
	}

	void destroy(const VkAllocationCallbacks* pAllocator)
	{
		destroyPipeline(pAllocator);
	}

	virtual void destroyPipeline(const VkAllocationCallbacks* pAllocator) = 0;
#ifndef NDEBUG
	virtual VkPipelineBindPoint bindPoint() const = 0;
#endif
};

class GraphicsPipeline : public Pipeline, public ObjectBase<GraphicsPipeline, VkPipeline>
{
public:
	GraphicsPipeline(const VkGraphicsPipelineCreateInfo* pCreateInfo, void* mem);
	~GraphicsPipeline() = delete;
	void destroyPipeline(const VkAllocationCallbacks* pAllocator) override;

#ifndef NDEBUG
	VkPipelineBindPoint bindPoint() const override
	{
		return VK_PIPELINE_BIND_POINT_GRAPHICS;
	}
#endif

	static size_t ComputeRequiredAllocationSize(const VkGraphicsPipelineCreateInfo* pCreateInfo);

	void compileShaders(const VkAllocationCallbacks* pAllocator, const VkGraphicsPipelineCreateInfo* pCreateInfo);

	uint32_t computePrimitiveCount(uint32_t vertexCount) const;
	const sw::Context& getContext() const;
	const sw::Rect& getScissor() const;
	const VkViewport& getViewport() const;
	const sw::Color<float>& getBlendConstants() const;

private:
	sw::SpirvShader *vertexShader = nullptr;
	sw::SpirvShader *fragmentShader = nullptr;

	rr::Routine* vertexRoutine;
	rr::Routine* fragmentRoutine;

	sw::Context context;
	sw::Rect scissor;
	VkViewport viewport;
	sw::Color<float> blendConstants;
};

class ComputePipeline : public Pipeline, public ObjectBase<ComputePipeline, VkPipeline>
{
public:
	ComputePipeline(const VkComputePipelineCreateInfo* pCreateInfo, void* mem);
	~ComputePipeline() = delete;
	void destroyPipeline(const VkAllocationCallbacks* pAllocator) override;

#ifndef NDEBUG
	VkPipelineBindPoint bindPoint() const override
	{
		return VK_PIPELINE_BIND_POINT_COMPUTE;
	}
#endif

	static size_t ComputeRequiredAllocationSize(const VkComputePipelineCreateInfo* pCreateInfo);
};

static inline Pipeline* Cast(VkPipeline object)
{
	return reinterpret_cast<Pipeline*>(object);
}

} // namespace vk

#endif // VK_PIPELINE_HPP_
