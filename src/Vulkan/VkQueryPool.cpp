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

#include "VkQueryPool.hpp"

namespace vk
{
	QueryPool::QueryPool(const VkQueryPoolCreateInfo* pCreateInfo, void* mem) :
		queryCount(pCreateInfo->queryCount)
	{
		// According to the Vulkan spec, section 34.1. Features:
		// "pipelineStatisticsQuery specifies whether the pipeline statistics
		//  queries are supported. If this feature is not enabled, queries of
		//  type VK_QUERY_TYPE_PIPELINE_STATISTICS cannot be created, and
		//  none of the VkQueryPipelineStatisticFlagBits bits can be set in the
		//  pipelineStatistics member of the VkQueryPoolCreateInfo structure."
		if(pCreateInfo->queryType == VK_QUERY_TYPE_PIPELINE_STATISTICS)
		{
			UNIMPLEMENTED("pCreateInfo->queryType");
		}
	}

	size_t QueryPool::ComputeRequiredAllocationSize(const VkQueryPoolCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void QueryPool::getResults(uint32_t pFirstQuery, uint32_t pQueryCount, size_t pDataSize,
	                           void* pData, VkDeviceSize pStride, VkQueryResultFlags pFlags) const
	{
		// dataSize must be large enough to contain the result of each query
		ASSERT(static_cast<size_t>(pStride * pQueryCount) <= pDataSize);

		// The sum of firstQuery and queryCount must be less than or equal to the number of queries
		ASSERT((pFirstQuery + pQueryCount) <= queryCount);

		char* data = static_cast<char*>(pData);
		for(uint32_t i = 0; i < pQueryCount; i++, data += pStride)
		{
			UNIMPLEMENTED("queries");
		}
	}
} // namespace vk
