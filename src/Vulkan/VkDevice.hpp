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

#ifndef VK_DEVICE_HPP_
#define VK_DEVICE_HPP_

#include "VkObject.hpp"
#include "VkSampler.hpp"
#include "Device/LRUCache.hpp"
#include "Reactor/Routine.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace marl {
class Scheduler;
}
namespace sw {
class Blitter;
}

namespace vk {

class PhysicalDevice;
class Queue;

namespace dbg {
class Context;
class Server;
}  // namespace dbg

class Device
{
public:
	static constexpr VkSystemAllocationScope GetAllocationScope() { return VK_SYSTEM_ALLOCATION_SCOPE_DEVICE; }

	Device(const VkDeviceCreateInfo *pCreateInfo, void *mem, PhysicalDevice *physicalDevice, const VkPhysicalDeviceFeatures *enabledFeatures, const std::shared_ptr<marl::Scheduler> &scheduler);
	void destroy(const VkAllocationCallbacks *pAllocator);

	static size_t ComputeRequiredAllocationSize(const VkDeviceCreateInfo *pCreateInfo);

	bool hasExtension(const char *extensionName) const;
	VkQueue getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
	VkResult waitForFences(uint32_t fenceCount, const VkFence *pFences, VkBool32 waitAll, uint64_t timeout);
	VkResult waitIdle();
	void getDescriptorSetLayoutSupport(const VkDescriptorSetLayoutCreateInfo *pCreateInfo,
	                                   VkDescriptorSetLayoutSupport *pSupport) const;
	PhysicalDevice *getPhysicalDevice() const { return physicalDevice; }
	void updateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites,
	                          uint32_t descriptorCopyCount, const VkCopyDescriptorSet *pDescriptorCopies);
	void getRequirements(VkMemoryDedicatedRequirements *requirements) const;
	const VkPhysicalDeviceFeatures &getEnabledFeatures() const { return enabledFeatures; }
	sw::Blitter *getBlitter() const { return blitter.get(); }

	class SamplingRoutineCache
	{
	public:
		SamplingRoutineCache()
		    : cache(1024)
		{}
		~SamplingRoutineCache() {}

		struct Key
		{
			uint32_t instruction;
			uint32_t sampler;
			uint32_t imageView;

			inline bool operator==(const Key &rhs) const;

			struct Hash
			{
				inline std::size_t operator()(const Key &key) const noexcept;
			};
		};

		template<typename Function>
		std::shared_ptr<rr::Routine> getOrCreate(const Key &key, Function createRoutine)
		{
			std::lock_guard<std::mutex> lock(mutex);

			if(auto existingRoutine = cache.query(key))
			{
				return existingRoutine;
			}

			std::shared_ptr<rr::Routine> newRoutine = createRoutine(key);
			cache.add(key, newRoutine);

			return newRoutine;
		}

		rr::Routine *querySnapshot(const Key &key) const;
		void updateSnapshot();

	private:
		sw::LRUSnapshotCache<Key, std::shared_ptr<rr::Routine>, Key::Hash> cache;  // guarded by mutex
		std::mutex mutex;
	};

	SamplingRoutineCache *getSamplingRoutineCache() const;
	rr::Routine *querySnapshotCache(const SamplingRoutineCache::Key &key) const;
	void updateSamplingRoutineSnapshotCache();

	class SamplerIndexer
	{
	public:
		~SamplerIndexer();

		uint32_t index(const SamplerState &samplerState);
		void remove(const SamplerState &samplerState);

	private:
		struct Identifier
		{
			uint32_t id;
			uint32_t count;  // Number of samplers sharing this state identifier.
		};

		std::map<SamplerState, Identifier> map;  // guarded by mutex
		std::mutex mutex;

		uint32_t nextID = 0;
	};

	uint32_t indexSampler(const SamplerState &samplerState);
	void removeSampler(const SamplerState &samplerState);

	std::shared_ptr<vk::dbg::Context> getDebuggerContext() const
	{
#ifdef ENABLE_VK_DEBUGGER
		return debugger.context;
#else
		return nullptr;
#endif  // ENABLE_VK_DEBUGGER
	}

private:
	PhysicalDevice *const physicalDevice = nullptr;
	Queue *const queues = nullptr;
	uint32_t queueCount = 0;
	std::unique_ptr<sw::Blitter> blitter;
	uint32_t enabledExtensionCount = 0;
	typedef char ExtensionName[VK_MAX_EXTENSION_NAME_SIZE];
	ExtensionName *extensions = nullptr;
	const VkPhysicalDeviceFeatures enabledFeatures = {};

	std::shared_ptr<marl::Scheduler> scheduler;
	std::unique_ptr<SamplingRoutineCache> samplingRoutineCache;
	std::unique_ptr<SamplerIndexer> samplerIndexer;

#ifdef ENABLE_VK_DEBUGGER
	struct
	{
		std::shared_ptr<vk::dbg::Context> context;
		std::shared_ptr<vk::dbg::Server> server;
	} debugger;
#endif  // ENABLE_VK_DEBUGGER
};

using DispatchableDevice = DispatchableObject<Device, VkDevice>;

static inline Device *Cast(VkDevice object)
{
	return DispatchableDevice::Cast(object);
}

inline bool vk::Device::SamplingRoutineCache::Key::operator==(const Key &rhs) const
{
	return instruction == rhs.instruction && sampler == rhs.sampler && imageView == rhs.imageView;
}

inline std::size_t vk::Device::SamplingRoutineCache::Key::Hash::operator()(const Key &key) const noexcept
{
	// Combine three 32-bit integers into a 64-bit hash.
	// 2642239 is the largest prime which when cubed is smaller than 2^64.
	uint64_t hash = key.instruction;
	hash = (hash * 2642239) ^ key.sampler;
	hash = (hash * 2642239) ^ key.imageView;
	return static_cast<std::size_t>(hash);  // Truncates to 32-bits on 32-bit platforms.
}

}  // namespace vk

#endif  // VK_DEVICE_HPP_
