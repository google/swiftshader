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

#ifndef VK_PIPELINE_CACHE_HPP_
#define VK_PIPELINE_CACHE_HPP_

#include "VkObject.hpp"
#include "VkSpecializationInfo.hpp"

#include "marl/mutex.h"
#include "marl/tsa.h"

#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sw {

class ComputeProgram;
class SpirvShader;

}  // namespace sw

namespace vk {

class PipelineLayout;
class RenderPass;

class PipelineCache : public Object<PipelineCache, VkPipelineCache>
{
public:
	PipelineCache(const VkPipelineCacheCreateInfo *pCreateInfo, void *mem);
	virtual ~PipelineCache();
	void destroy(const VkAllocationCallbacks *pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkPipelineCacheCreateInfo *pCreateInfo);

	VkResult getData(size_t *pDataSize, void *pData);
	VkResult merge(uint32_t srcCacheCount, const VkPipelineCache *pSrcCaches);

	struct SpirvShaderKey
	{
		SpirvShaderKey(const VkShaderStageFlagBits pipelineStage,
		               const std::string &entryPointName,
		               const std::vector<uint32_t> &insns,
		               const vk::RenderPass *renderPass,
		               const uint32_t subpassIndex,
		               const vk::SpecializationInfo &specializationInfo);

		bool operator<(const SpirvShaderKey &other) const;

		const VkShaderStageFlagBits &getPipelineStage() const { return pipelineStage; }
		const std::string &getEntryPointName() const { return entryPointName; }
		const std::vector<uint32_t> &getInsns() const { return insns; }
		const vk::RenderPass *getRenderPass() const { return renderPass; }
		uint32_t getSubpassIndex() const { return subpassIndex; }
		const VkSpecializationInfo *getSpecializationInfo() const { return specializationInfo.get(); }

	private:
		const VkShaderStageFlagBits pipelineStage;
		const std::string entryPointName;
		const std::vector<uint32_t> insns;
		const vk::RenderPass *renderPass;
		const uint32_t subpassIndex;
		const vk::SpecializationInfo specializationInfo;
	};

	// getOrCreateShader() queries the cache for a shader with the given key.
	// If one is found, it is returned, otherwise create() is called, the
	// returned shader is added to the cache, and it is returned.
	// Function must be a function of the signature:
	//     std::shared_ptr<sw::SpirvShader>()
	template<typename Function>
	inline std::shared_ptr<sw::SpirvShader> getOrCreateShader(const PipelineCache::SpirvShaderKey &key, Function &&create);

	struct ComputeProgramKey
	{
		ComputeProgramKey(const sw::SpirvShader *shader, const vk::PipelineLayout *layout)
		    : shader(shader)
		    , layout(layout)
		{}

		bool operator<(const ComputeProgramKey &other) const
		{
			return std::tie(shader, layout) < std::tie(other.shader, other.layout);
		}

		const sw::SpirvShader *getShader() const { return shader; }
		const vk::PipelineLayout *getLayout() const { return layout; }

	private:
		const sw::SpirvShader *shader;
		const vk::PipelineLayout *layout;
	};

	// getOrCreateComputeProgram() queries the cache for a compute program with
	// the given key.
	// If one is found, it is returned, otherwise create() is called, the
	// returned program is added to the cache, and it is returned.
	// Function must be a function of the signature:
	//     std::shared_ptr<sw::ComputeProgram>()
	template<typename Function>
	inline std::shared_ptr<sw::ComputeProgram> getOrCreateComputeProgram(const PipelineCache::ComputeProgramKey &key, Function &&create);

private:
	struct CacheHeader
	{
		uint32_t headerLength;
		uint32_t headerVersion;
		uint32_t vendorID;
		uint32_t deviceID;
		uint8_t pipelineCacheUUID[VK_UUID_SIZE];
	};

	size_t dataSize = 0;
	uint8_t *data = nullptr;

	marl::mutex spirvShadersMutex;
	std::map<SpirvShaderKey, std::shared_ptr<sw::SpirvShader>> spirvShaders GUARDED_BY(spirvShadersMutex);

	marl::mutex computeProgramsMutex;
	std::map<ComputeProgramKey, std::shared_ptr<sw::ComputeProgram>> computePrograms GUARDED_BY(computeProgramsMutex);
};

static inline PipelineCache *Cast(VkPipelineCache object)
{
	return PipelineCache::Cast(object);
}

template<typename Function>
std::shared_ptr<sw::ComputeProgram> PipelineCache::getOrCreateComputeProgram(const PipelineCache::ComputeProgramKey &key, Function &&create)
{
	marl::lock lock(computeProgramsMutex);

	auto it = computePrograms.find(key);
	if(it != computePrograms.end()) { return it->second; }

	auto created = create();
	computePrograms.emplace(key, created);
	return created;
}

template<typename Function>
std::shared_ptr<sw::SpirvShader> PipelineCache::getOrCreateShader(const PipelineCache::SpirvShaderKey &key, Function &&create)
{
	marl::lock lock(spirvShadersMutex);

	auto it = spirvShaders.find(key);
	if(it != spirvShaders.end()) { return it->second; }

	auto created = create();
	spirvShaders.emplace(key, created);
	return created;
}

}  // namespace vk

#endif  // VK_PIPELINE_CACHE_HPP_
