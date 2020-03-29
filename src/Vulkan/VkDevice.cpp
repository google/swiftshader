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

#include "VkDevice.hpp"

#include "VkConfig.h"
#include "VkDescriptorSetLayout.hpp"
#include "VkFence.hpp"
#include "VkQueue.hpp"
#include "Debug/Context.hpp"
#include "Debug/Server.hpp"
#include "Device/Blitter.hpp"
#include "System/Debug.hpp"

#include <chrono>
#include <climits>
#include <new>  // Must #include this to use "placement new"

namespace {

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now()
{
	return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
}

}  // anonymous namespace

namespace vk {

rr::Routine *Device::SamplingRoutineCache::querySnapshot(const vk::Device::SamplingRoutineCache::Key &key) const
{
	return cache.querySnapshot(key).get();
}

void Device::SamplingRoutineCache::updateSnapshot()
{
	std::lock_guard<std::mutex> lock(mutex);

	cache.updateSnapshot();
}

Device::SamplerIndexer::~SamplerIndexer()
{
	ASSERT(map.empty());
}

uint32_t Device::SamplerIndexer::index(const SamplerState &samplerState)
{
	std::lock_guard<std::mutex> lock(mutex);

	auto it = map.find(samplerState);

	if(it != map.end())
	{
		it->second.count++;
		return it->second.id;
	}

	nextID++;

	map.emplace(samplerState, Identifier{ nextID, 1 });

	return nextID;
}

void Device::SamplerIndexer::remove(const SamplerState &samplerState)
{
	std::lock_guard<std::mutex> lock(mutex);

	auto it = map.find(samplerState);
	ASSERT(it != map.end());

	auto count = --it->second.count;
	if(count == 0)
	{
		map.erase(it);
	}
}

Device::Device(const VkDeviceCreateInfo *pCreateInfo, void *mem, PhysicalDevice *physicalDevice, const VkPhysicalDeviceFeatures *enabledFeatures, const std::shared_ptr<marl::Scheduler> &scheduler)
    : physicalDevice(physicalDevice)
    , queues(reinterpret_cast<Queue *>(mem))
    , enabledExtensionCount(pCreateInfo->enabledExtensionCount)
    , enabledFeatures(enabledFeatures ? *enabledFeatures : VkPhysicalDeviceFeatures{})
    ,  // "Setting pEnabledFeatures to NULL and not including a VkPhysicalDeviceFeatures2 in the pNext member of VkDeviceCreateInfo is equivalent to setting all members of the structure to VK_FALSE."
    scheduler(scheduler)
{
	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo &queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];
		queueCount += queueCreateInfo.queueCount;
	}

	uint32_t queueID = 0;
	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		const VkDeviceQueueCreateInfo &queueCreateInfo = pCreateInfo->pQueueCreateInfos[i];

		for(uint32_t j = 0; j < queueCreateInfo.queueCount; j++, queueID++)
		{
			new(&queues[queueID]) Queue(this, scheduler.get());
		}
	}

	extensions = reinterpret_cast<ExtensionName *>(static_cast<uint8_t *>(mem) + (sizeof(Queue) * queueCount));
	for(uint32_t i = 0; i < enabledExtensionCount; i++)
	{
		strncpy(extensions[i], pCreateInfo->ppEnabledExtensionNames[i], VK_MAX_EXTENSION_NAME_SIZE);
	}

	if(pCreateInfo->enabledLayerCount)
	{
		// "The ppEnabledLayerNames and enabledLayerCount members of VkDeviceCreateInfo are deprecated and their values must be ignored by implementations."
		UNSUPPORTED("enabledLayerCount");
	}

	// FIXME (b/119409619): use an allocator here so we can control all memory allocations
	blitter.reset(new sw::Blitter());
	samplingRoutineCache.reset(new SamplingRoutineCache());
	samplerIndexer.reset(new SamplerIndexer());

#ifdef ENABLE_VK_DEBUGGER
	static auto port = getenv("VK_DEBUGGER_PORT");
	if(port)
	{
		// Construct the debugger context and server - this may block for a
		// debugger connection, allowing breakpoints to be set before they're
		// executed.
		debugger.context = vk::dbg::Context::create();
		debugger.server = vk::dbg::Server::create(debugger.context, atoi(port));
	}
#endif  // ENABLE_VK_DEBUGGER
}

void Device::destroy(const VkAllocationCallbacks *pAllocator)
{
	for(uint32_t i = 0; i < queueCount; i++)
	{
		queues[i].~Queue();
	}

	vk::deallocate(queues, pAllocator);
}

size_t Device::ComputeRequiredAllocationSize(const VkDeviceCreateInfo *pCreateInfo)
{
	uint32_t queueCount = 0;
	for(uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++)
	{
		queueCount += pCreateInfo->pQueueCreateInfos[i].queueCount;
	}

	return (sizeof(Queue) * queueCount) + (pCreateInfo->enabledExtensionCount * sizeof(ExtensionName));
}

bool Device::hasExtension(const char *extensionName) const
{
	for(uint32_t i = 0; i < enabledExtensionCount; i++)
	{
		if(strncmp(extensions[i], extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0)
		{
			return true;
		}
	}
	return false;
}

VkQueue Device::getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
{
	ASSERT(queueFamilyIndex == 0);

	return queues[queueIndex];
}

VkResult Device::waitForFences(uint32_t fenceCount, const VkFence *pFences, VkBool32 waitAll, uint64_t timeout)
{
	using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
	const time_point start = now();
	const uint64_t max_timeout = (LLONG_MAX - start.time_since_epoch().count());
	bool infiniteTimeout = (timeout > max_timeout);
	const time_point end_ns = start + std::chrono::nanoseconds(std::min(max_timeout, timeout));

	if(waitAll != VK_FALSE)  // All fences must be signaled
	{
		for(uint32_t i = 0; i < fenceCount; i++)
		{
			if(timeout == 0)
			{
				if(Cast(pFences[i])->getStatus() != VK_SUCCESS)  // At least one fence is not signaled
				{
					return VK_TIMEOUT;
				}
			}
			else if(infiniteTimeout)
			{
				if(Cast(pFences[i])->wait() != VK_SUCCESS)  // At least one fence is not signaled
				{
					return VK_TIMEOUT;
				}
			}
			else
			{
				if(Cast(pFences[i])->wait(end_ns) != VK_SUCCESS)  // At least one fence is not signaled
				{
					return VK_TIMEOUT;
				}
			}
		}

		return VK_SUCCESS;
	}
	else  // At least one fence must be signaled
	{
		marl::containers::vector<marl::Event, 8> events;
		for(uint32_t i = 0; i < fenceCount; i++)
		{
			events.push_back(Cast(pFences[i])->getEvent());
		}

		auto any = marl::Event::any(events.begin(), events.end());

		if(timeout == 0)
		{
			return any.isSignalled() ? VK_SUCCESS : VK_TIMEOUT;
		}
		else if(infiniteTimeout)
		{
			any.wait();
			return VK_SUCCESS;
		}
		else
		{
			return any.wait_until(end_ns) ? VK_SUCCESS : VK_TIMEOUT;
		}
	}
}

VkResult Device::waitIdle()
{
	for(uint32_t i = 0; i < queueCount; i++)
	{
		queues[i].waitIdle();
	}

	return VK_SUCCESS;
}

void Device::getDescriptorSetLayoutSupport(const VkDescriptorSetLayoutCreateInfo *pCreateInfo,
                                           VkDescriptorSetLayoutSupport *pSupport) const
{
	// From Vulkan Spec 13.2.1 Descriptor Set Layout, in description of vkGetDescriptorSetLayoutSupport:
	// "This command does not consider other limits such as maxPerStageDescriptor*, and so a descriptor
	// set layout that is supported according to this command must still satisfy the pipeline layout limits
	// such as maxPerStageDescriptor* in order to be used in a pipeline layout."

	// We have no "strange" limitations to enforce beyond the device limits, so we can safely always claim support.
	pSupport->supported = VK_TRUE;
}

void Device::updateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites,
                                  uint32_t descriptorCopyCount, const VkCopyDescriptorSet *pDescriptorCopies)
{
	for(uint32_t i = 0; i < descriptorWriteCount; i++)
	{
		DescriptorSetLayout::WriteDescriptorSet(this, pDescriptorWrites[i]);
	}

	for(uint32_t i = 0; i < descriptorCopyCount; i++)
	{
		DescriptorSetLayout::CopyDescriptorSet(pDescriptorCopies[i]);
	}
}

void Device::getRequirements(VkMemoryDedicatedRequirements *requirements) const
{
	requirements->prefersDedicatedAllocation = VK_FALSE;
	requirements->requiresDedicatedAllocation = VK_FALSE;
}

Device::SamplingRoutineCache *Device::getSamplingRoutineCache() const
{
	return samplingRoutineCache.get();
}

rr::Routine *Device::querySnapshotCache(const SamplingRoutineCache::Key &key) const
{
	return samplingRoutineCache->querySnapshot(key);
}

void Device::updateSamplingRoutineSnapshotCache()
{
	samplingRoutineCache->updateSnapshot();
}

uint32_t Device::indexSampler(const SamplerState &samplerState)
{
	return samplerIndexer->index(samplerState);
}

void Device::removeSampler(const SamplerState &samplerState)
{
	samplerIndexer->remove(samplerState);
}

}  // namespace vk
