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

namespace vk
{

class PipelineCache : public Object<PipelineCache, VkPipelineCache>
{
public:
	PipelineCache(const VkPipelineCacheCreateInfo* pCreateInfo, void* mem);
	~PipelineCache() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkPipelineCacheCreateInfo* pCreateInfo);

	VkResult getData(size_t* pDataSize, void* pData);
	VkResult merge(uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches);

private:
	struct CacheHeader
	{
		uint32_t headerLength;
		uint32_t headerVersion;
		uint32_t vendorID;
		uint32_t deviceID;
		uint8_t  pipelineCacheUUID[VK_UUID_SIZE];
	};

	size_t dataSize = 0;
	uint8_t* data   = nullptr;
};

static inline PipelineCache* Cast(VkPipelineCache object)
{
	return reinterpret_cast<PipelineCache*>(object);
}

} // namespace vk

#endif // VK_PIPELINE_CACHE_HPP_
