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

namespace vk
{

class QueryPool : public Object<QueryPool, VkQueryPool>
{
public:
	QueryPool(const VkQueryPoolCreateInfo* pCreateInfo, void* mem);
	~QueryPool() = delete;

	static size_t ComputeRequiredAllocationSize(const VkQueryPoolCreateInfo* pCreateInfo);

	void getResults(uint32_t pFirstQuery, uint32_t pQueryCount, size_t pDataSize,
		            void* pData, VkDeviceSize pStride, VkQueryResultFlags pFlags) const;

private:
	uint32_t queryCount;
};

static inline QueryPool* Cast(VkQueryPool object)
{
	return reinterpret_cast<QueryPool*>(object);
}

} // namespace vk

#endif // VK_QUERY_POOL_HPP_
