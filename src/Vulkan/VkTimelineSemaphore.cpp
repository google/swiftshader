// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#include "VkTimelineSemaphore.hpp"
#include "VkSemaphore.hpp"

#include "marl/blockingcall.h"
#include "marl/conditionvariable.h"

#include <vector>

namespace vk {

TimelineSemaphore::TimelineSemaphore(const VkSemaphoreCreateInfo *pCreateInfo, void *mem, const VkAllocationCallbacks *pAllocator)
    : Semaphore(VK_SEMAPHORE_TYPE_TIMELINE)
{
	SemaphoreCreateInfo info(pCreateInfo);
	ASSERT(info.semaphoreType == VK_SEMAPHORE_TYPE_TIMELINE);
	type = info.semaphoreType;
	shared = marl::Allocator::Default->make_shared<TimelineSemaphore::Shared>(marl::Allocator::Default, info.initialPayload);
}

TimelineSemaphore::TimelineSemaphore()
    : Semaphore(VK_SEMAPHORE_TYPE_TIMELINE)
{
	type = VK_SEMAPHORE_TYPE_TIMELINE;
	shared = marl::Allocator::Default->make_shared<TimelineSemaphore::Shared>(marl::Allocator::Default, 0);
}

size_t TimelineSemaphore::ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo *pCreateInfo)
{
	return 0;
}

void TimelineSemaphore::destroy(const VkAllocationCallbacks *pAllocator)
{
}

void TimelineSemaphore::signal(uint64_t value)
{
	return shared->signal(value);
}

void TimelineSemaphore::Shared::signal(uint64_t value)
{
	marl::lock lock(mutex);
	if(counter < value)
	{
		counter = value;
		cv.notify_all();
		for(auto dep : deps)
		{
			dep->signal(id, counter);
		}
	}
}

void TimelineSemaphore::wait(uint64_t value)
{
	shared->wait(value);
}

void TimelineSemaphore::Shared::wait(uint64_t value)
{
	marl::lock lock(mutex);
	cv.wait(lock, [&]() { return counter >= value; });
}

uint64_t TimelineSemaphore::getCounterValue()
{
	return shared->getCounterValue();
}

uint64_t TimelineSemaphore::Shared::getCounterValue()
{
	marl::lock lock(mutex);
	return counter;
}

std::atomic<int> TimelineSemaphore::Shared::nextId;

TimelineSemaphore::Shared::Shared(marl::Allocator *allocator, uint64_t initialState)
    : cv(allocator)
    , counter(initialState)
    , id(nextId++)
{
}

void TimelineSemaphore::Shared::signal(int parentId, uint64_t value)
{
	marl::lock lock(mutex);
	auto it = waitMap.find(parentId);
	// Either we aren't waiting for a signal, or parentId is not something we're waiting for
	// Reject any signals that we aren't waiting on
	if(counter == 0 && it != waitMap.end() && value == it->second)
	{
		// Stop waiting on all parents once we find a signal
		waitMap.clear();
		counter = 1;
		cv.notify_all();
		for(auto dep : deps)
		{
			dep->signal(id, counter);
		}
	}
}

void TimelineSemaphore::addDependent(TimelineSemaphore &other, uint64_t waitValue)
{
	shared->addDependent(other);
	other.addDependency(shared->id, waitValue);
}

void TimelineSemaphore::Shared::addDependent(TimelineSemaphore &other)
{
	marl::lock lock(mutex);
	deps.push_back(other.shared);
}

void TimelineSemaphore::addDependency(int id, uint64_t waitValue)
{
	shared->addDependency(id, waitValue);
}

void TimelineSemaphore::Shared::addDependency(int id, uint64_t waitValue)
{
	marl::lock lock(mutex);
	auto mapPos = waitMap.find(id);
	ASSERT(mapPos == waitMap.end());

	waitMap.insert(mapPos, std::make_pair(id, waitValue));
}

}  // namespace vk
