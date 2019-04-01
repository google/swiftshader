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
#include "Common/Thread.hpp"

#include <chrono>
#include <cstring>
#include <new>

namespace vk
{
	QueryPool::QueryPool(const VkQueryPoolCreateInfo* pCreateInfo, void* mem) :
		pool(reinterpret_cast<Query*>(mem)), type(pCreateInfo->queryType),
		count(pCreateInfo->queryCount)
	{
		// According to the Vulkan spec, section 34.1. Features:
		// "pipelineStatisticsQuery specifies whether the pipeline statistics
		//  queries are supported. If this feature is not enabled, queries of
		//  type VK_QUERY_TYPE_PIPELINE_STATISTICS cannot be created, and
		//  none of the VkQueryPipelineStatisticFlagBits bits can be set in the
		//  pipelineStatistics member of the VkQueryPoolCreateInfo structure."
		if(type == VK_QUERY_TYPE_PIPELINE_STATISTICS)
		{
			UNIMPLEMENTED("pCreateInfo->queryType");
		}

		// Construct all queries
		for(uint32_t i = 0; i < count; i++)
		{
			new (&pool[i]) Query();
		}
	}

	void QueryPool::destroy(const VkAllocationCallbacks* pAllocator)
	{
		vk::deallocate(pool, pAllocator);
	}

	size_t QueryPool::ComputeRequiredAllocationSize(const VkQueryPoolCreateInfo* pCreateInfo)
	{
		return sizeof(Query) * pCreateInfo->queryCount;
	}

	VkResult QueryPool::getResults(uint32_t firstQuery, uint32_t queryCount, size_t dataSize,
	                               void* pData, VkDeviceSize stride, VkQueryResultFlags flags) const
	{
		// dataSize must be large enough to contain the result of each query
		ASSERT(static_cast<size_t>(stride * queryCount) <= dataSize);

		// The sum of firstQuery and queryCount must be less than or equal to the number of queries
		ASSERT((firstQuery + queryCount) <= count);
		
		VkResult result = VK_SUCCESS;
		uint8_t* data = static_cast<uint8_t*>(pData);
		for(uint32_t i = firstQuery; i < (firstQuery + queryCount); i++, data += stride)
		{
			// If VK_QUERY_RESULT_WAIT_BIT and VK_QUERY_RESULT_PARTIAL_BIT are both not set
			// then no result values are written to pData for queries that are in the
			// unavailable state at the time of the call, and vkGetQueryPoolResults returns
			// VK_NOT_READY. However, availability state is still written to pData for those
			// queries if VK_QUERY_RESULT_WITH_AVAILABILITY_BIT is set.
			auto &query = pool[i];
			std::unique_lock<std::mutex> mutexLock(query.mutex);
			if(flags & VK_QUERY_RESULT_WAIT_BIT) // Must wait for query to finish
			{
				query.condition.wait(mutexLock, [&query] { return query.state != Query::ACTIVE; });
			}

			bool writeResult = true;
			if(pool[i].state == Query::ACTIVE)
			{
				result = VK_NOT_READY;
				writeResult = (flags & VK_QUERY_RESULT_PARTIAL_BIT); // Allow writing partial results
			}

			if(flags & VK_QUERY_RESULT_64_BIT)
			{
				uint64_t* result64 = reinterpret_cast<uint64_t*>(data);
				if(writeResult)
				{
					result64[0] = pool[i].data;
				}
				if(flags & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) // Output query availablity
				{
					result64[1] = pool[i].state;
				}
			}
			else
			{
				uint32_t* result32 = reinterpret_cast<uint32_t*>(data);
				if(writeResult)
				{
					result32[0] = static_cast<uint32_t>(pool[i].data);
				}
				if(flags & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) // Output query availablity
				{
					result32[1] = pool[i].state;
				}
			}
		}

		return result;
	}

	void QueryPool::begin(uint32_t query, VkQueryControlFlags flags)
	{
		ASSERT(query < count);

		if(flags != 0)
		{
			UNIMPLEMENTED("flags");
		}

		ASSERT(pool[query].state == Query::UNAVAILABLE);
		pool[query].state = Query::ACTIVE;
		pool[query].data = 0;
		pool[query].reference = 1;
		pool[query].type = type;
	}

	void QueryPool::end(uint32_t query)
	{
		ASSERT(query < count);

		#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
		{
			std::unique_lock<std::mutex> mutexLock(pool[query].mutex);
			ASSERT(pool[query].state == Query::ACTIVE);
		}
		#endif

		int ref = --pool[query].reference;
		if(ref == 0)
		{
			std::unique_lock<std::mutex> mutexLock(pool[query].mutex);
			pool[query].state = Query::FINISHED;
		}
	}

	void QueryPool::reset(uint32_t firstQuery, uint32_t queryCount)
	{
		// The sum of firstQuery and queryCount must be less than or equal to the number of queries
		ASSERT((firstQuery + queryCount) <= count);

		for(uint32_t i = firstQuery; i < (firstQuery + queryCount); i++)
		{
			std::unique_lock<std::mutex> mutexLock(pool[i].mutex);

			ASSERT(pool[i].state != Query::ACTIVE);

			pool[i].state = Query::UNAVAILABLE;
			pool[i].data = 0;
		}
	}

	void QueryPool::writeTimestamp(uint32_t query)
	{
		ASSERT(query < count);
		ASSERT(type == VK_QUERY_TYPE_TIMESTAMP);

		std::unique_lock<std::mutex> mutexLock(pool[query].mutex);
		pool[query].data = std::chrono::time_point_cast<std::chrono::nanoseconds>(
			std::chrono::system_clock::now()).time_since_epoch().count();
	}
} // namespace vk
