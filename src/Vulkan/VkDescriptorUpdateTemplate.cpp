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

#include "VkDescriptorUpdateTemplate.hpp"
#include "VkDescriptorSetLayout.hpp"
#include <cstring>

namespace vk
{
	DescriptorUpdateTemplate::DescriptorUpdateTemplate(const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, void* mem) :
		descriptorUpdateEntryCount(pCreateInfo->descriptorUpdateEntryCount),
		descriptorUpdateEntries(reinterpret_cast<VkDescriptorUpdateTemplateEntry*>(mem)),
		descriptorSetLayout(Cast(pCreateInfo->descriptorSetLayout))
	{
		for(uint32_t i = 0; i < descriptorUpdateEntryCount; i++)
		{
			descriptorUpdateEntries[i] = pCreateInfo->pDescriptorUpdateEntries[i];
		}
	}

	size_t DescriptorUpdateTemplate::ComputeRequiredAllocationSize(const VkDescriptorUpdateTemplateCreateInfo* info)
	{
		return info->descriptorUpdateEntryCount * sizeof(VkDescriptorUpdateTemplateEntry);
	}

	void DescriptorUpdateTemplate::updateDescriptorSet(VkDescriptorSet vkDescriptorSet, const void* pData)
	{
		DescriptorSet* descriptorSet = vk::Cast(vkDescriptorSet);

		for(uint32_t i = 0; i < descriptorUpdateEntryCount; i++)
		{
			for(uint32_t j = 0; j < descriptorUpdateEntries[i].descriptorCount; j++)
			{
				const char *memToRead = (const char *)pData + descriptorUpdateEntries[i].offset + j * descriptorUpdateEntries[i].stride;
				size_t typeSize = 0;
				uint8_t* memToWrite = descriptorSetLayout->getOffsetPointer(
					descriptorSet, descriptorUpdateEntries[i].dstBinding, descriptorUpdateEntries[i].dstArrayElement, 1, &typeSize);
				memcpy(memToWrite, memToRead, typeSize);
			}
		}
	}
}