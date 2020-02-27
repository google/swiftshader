// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "VkPipelineCache.hpp"
#include <cstring>

namespace vk {

PipelineCache::SpirvShaderKey::SpecializationInfo::SpecializationInfo(const VkSpecializationInfo *specializationInfo)
{
	if(specializationInfo)
	{
		auto ptr = reinterpret_cast<VkSpecializationInfo *>(
		    allocate(sizeof(VkSpecializationInfo), REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY));

		info = std::shared_ptr<VkSpecializationInfo>(ptr, Deleter());

		info->mapEntryCount = specializationInfo->mapEntryCount;
		if(specializationInfo->mapEntryCount > 0)
		{
			size_t entriesSize = specializationInfo->mapEntryCount * sizeof(VkSpecializationMapEntry);
			VkSpecializationMapEntry *mapEntries = reinterpret_cast<VkSpecializationMapEntry *>(
			    allocate(entriesSize, REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY));
			memcpy(mapEntries, specializationInfo->pMapEntries, entriesSize);
			info->pMapEntries = mapEntries;
		}

		info->dataSize = specializationInfo->dataSize;
		if(specializationInfo->dataSize > 0)
		{
			void *data = allocate(specializationInfo->dataSize, REQUIRED_MEMORY_ALIGNMENT, DEVICE_MEMORY);
			memcpy(data, specializationInfo->pData, specializationInfo->dataSize);
			info->pData = data;
		}
		else
		{
			info->pData = nullptr;
		}
	}
}

void PipelineCache::SpirvShaderKey::SpecializationInfo::Deleter::operator()(VkSpecializationInfo *info) const
{
	if(info)
	{
		deallocate(const_cast<VkSpecializationMapEntry *>(info->pMapEntries), DEVICE_MEMORY);
		deallocate(const_cast<void *>(info->pData), DEVICE_MEMORY);
		deallocate(info, DEVICE_MEMORY);
	}
}

bool PipelineCache::SpirvShaderKey::SpecializationInfo::operator<(const SpecializationInfo &specializationInfo) const
{
	// Check that either both or neither keys have specialization info.
	if((info.get() == nullptr) != (specializationInfo.info.get() == nullptr))
	{
		return info.get() == nullptr;
	}

	if(!info)
	{
		ASSERT(!specializationInfo.info);
		return false;
	}

	if(info->mapEntryCount != specializationInfo.info->mapEntryCount)
	{
		return info->mapEntryCount < specializationInfo.info->mapEntryCount;
	}

	if(info->dataSize != specializationInfo.info->dataSize)
	{
		return info->dataSize < specializationInfo.info->dataSize;
	}

	if(info->mapEntryCount > 0)
	{
		int cmp = memcmp(info->pMapEntries, specializationInfo.info->pMapEntries, info->mapEntryCount * sizeof(VkSpecializationMapEntry));
		if(cmp != 0)
		{
			return cmp < 0;
		}
	}

	if(info->dataSize > 0)
	{
		int cmp = memcmp(info->pData, specializationInfo.info->pData, info->dataSize);
		if(cmp != 0)
		{
			return cmp < 0;
		}
	}

	return false;
}

PipelineCache::SpirvShaderKey::SpirvShaderKey(const VkShaderStageFlagBits pipelineStage,
                                              const std::string &entryPointName,
                                              const std::vector<uint32_t> &insns,
                                              const vk::RenderPass *renderPass,
                                              const uint32_t subpassIndex,
                                              const VkSpecializationInfo *specializationInfo)
    : pipelineStage(pipelineStage)
    , entryPointName(entryPointName)
    , insns(insns)
    , renderPass(renderPass)
    , subpassIndex(subpassIndex)
    , specializationInfo(specializationInfo)
{
}

bool PipelineCache::SpirvShaderKey::operator<(const SpirvShaderKey &other) const
{
	if(pipelineStage != other.pipelineStage)
	{
		return pipelineStage < other.pipelineStage;
	}

	if(renderPass != other.renderPass)
	{
		return renderPass < other.renderPass;
	}

	if(subpassIndex != other.subpassIndex)
	{
		return subpassIndex < other.subpassIndex;
	}

	if(insns.size() != other.insns.size())
	{
		return insns.size() < other.insns.size();
	}

	if(entryPointName.size() != other.entryPointName.size())
	{
		return entryPointName.size() < other.entryPointName.size();
	}

	int cmp = memcmp(entryPointName.c_str(), other.entryPointName.c_str(), entryPointName.size());
	if(cmp != 0)
	{
		return cmp < 0;
	}

	cmp = memcmp(insns.data(), other.insns.data(), insns.size() * sizeof(uint32_t));
	if(cmp != 0)
	{
		return cmp < 0;
	}

	return (specializationInfo < other.specializationInfo);
}

PipelineCache::PipelineCache(const VkPipelineCacheCreateInfo *pCreateInfo, void *mem)
    : dataSize(ComputeRequiredAllocationSize(pCreateInfo))
    , data(reinterpret_cast<uint8_t *>(mem))
{
	CacheHeader *header = reinterpret_cast<CacheHeader *>(mem);
	header->headerLength = sizeof(CacheHeader);
	header->headerVersion = VK_PIPELINE_CACHE_HEADER_VERSION_ONE;
	header->vendorID = VENDOR_ID;
	header->deviceID = DEVICE_ID;
	memcpy(header->pipelineCacheUUID, SWIFTSHADER_UUID, VK_UUID_SIZE);

	if(pCreateInfo->pInitialData && (pCreateInfo->initialDataSize > 0))
	{
		memcpy(data + sizeof(CacheHeader), pCreateInfo->pInitialData, pCreateInfo->initialDataSize);
	}
}

PipelineCache::~PipelineCache()
{
	spirvShaders.clear();
	computePrograms.clear();
}

void PipelineCache::destroy(const VkAllocationCallbacks *pAllocator)
{
	vk::deallocate(data, pAllocator);
}

size_t PipelineCache::ComputeRequiredAllocationSize(const VkPipelineCacheCreateInfo *pCreateInfo)
{
	return pCreateInfo->initialDataSize + sizeof(CacheHeader);
}

VkResult PipelineCache::getData(size_t *pDataSize, void *pData)
{
	if(!pData)
	{
		*pDataSize = dataSize;
		return VK_SUCCESS;
	}

	if(*pDataSize != dataSize)
	{
		*pDataSize = 0;
		return VK_INCOMPLETE;
	}

	if(*pDataSize > 0)
	{
		memcpy(pData, data, *pDataSize);
	}

	return VK_SUCCESS;
}

VkResult PipelineCache::merge(uint32_t srcCacheCount, const VkPipelineCache *pSrcCaches)
{
	for(uint32_t i = 0; i < srcCacheCount; i++)
	{
		PipelineCache *srcCache = Cast(pSrcCaches[i]);

		{
			std::unique_lock<std::mutex> lock(spirvShadersMutex);
			spirvShaders.insert(srcCache->spirvShaders.begin(), srcCache->spirvShaders.end());
		}

		{
			std::unique_lock<std::mutex> lock(computeProgramsMutex);
			computePrograms.insert(srcCache->computePrograms.begin(), srcCache->computePrograms.end());
		}
	}

	return VK_SUCCESS;
}

const std::shared_ptr<sw::SpirvShader> *PipelineCache::operator[](const PipelineCache::SpirvShaderKey &key) const
{
	auto it = spirvShaders.find(key);
	return (it != spirvShaders.end()) ? &(it->second) : nullptr;
}

void PipelineCache::insert(const PipelineCache::SpirvShaderKey &key, const std::shared_ptr<sw::SpirvShader> &shader)
{
	spirvShaders[key] = shader;
}

const std::shared_ptr<sw::ComputeProgram> *PipelineCache::operator[](const PipelineCache::ComputeProgramKey &key) const
{
	auto it = computePrograms.find(key);
	return (it != computePrograms.end()) ? &(it->second) : nullptr;
}

void PipelineCache::insert(const PipelineCache::ComputeProgramKey &key, const std::shared_ptr<sw::ComputeProgram> &computeProgram)
{
	computePrograms[key] = computeProgram;
}

}  // namespace vk
