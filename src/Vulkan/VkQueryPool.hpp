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

#ifndef VK_QUERY_POOL_HPP_
#define VK_QUERY_POOL_HPP_

#include "VkObject.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace vk
{

struct Query
{
	enum State
	{
		UNAVAILABLE,
		ACTIVE,
		FINISHED
	};

	std::mutex mutex;
	std::condition_variable condition;
	State state;  // guarded by mutex
	int64_t data; // guarded by mutex
	std::atomic<int> reference;
	VkQueryType type;
};

class QueryPool : public Object<QueryPool, VkQueryPool>
{
public:
	QueryPool(const VkQueryPoolCreateInfo* pCreateInfo, void* mem);
	~QueryPool() = delete;
	void destroy(const VkAllocationCallbacks* pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkQueryPoolCreateInfo* pCreateInfo);

	VkResult getResults(uint32_t firstQuery, uint32_t queryCount, size_t dataSize,
		                void* pData, VkDeviceSize stride, VkQueryResultFlags flags) const;
	void begin(uint32_t query, VkQueryControlFlags flags);
	void end(uint32_t query);
	void reset(uint32_t firstQuery, uint32_t queryCount);
	
	void writeTimestamp(uint32_t query);

	inline Query* getQuery(uint32_t query) const { return &(pool[query]); }

private:
	Query* pool;
	VkQueryType type;
	uint32_t count;
};

static inline QueryPool* Cast(VkQueryPool object)
{
	return reinterpret_cast<QueryPool*>(object);
}

} // namespace vk

#endif // VK_QUERY_POOL_HPP_
