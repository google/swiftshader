// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

SpecializationInfo::SpecializationInfo(const VkSpecializationInfo *specializationInfo)
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

void SpecializationInfo::Deleter::operator()(VkSpecializationInfo *info) const
{
	if(info)
	{
		deallocate(const_cast<VkSpecializationMapEntry *>(info->pMapEntries), DEVICE_MEMORY);
		deallocate(const_cast<void *>(info->pData), DEVICE_MEMORY);
		deallocate(info, DEVICE_MEMORY);
	}
}

bool SpecializationInfo::operator<(const SpecializationInfo &specializationInfo) const
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

}  // namespace vk
