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

#ifndef VK_TIMELINE_SEMAPHORE_HPP_
#define VK_TIMELINE_SEMAPHORE_HPP_

#include "VkConfig.hpp"
#include "VkObject.hpp"
#include "VkSemaphore.hpp"

#include "marl/conditionvariable.h"
#include "marl/mutex.h"

#include "System/Synchronization.hpp"

#include <chrono>

namespace vk {

struct Shared;

// Timeline Semaphores track a 64-bit payload instead of a binary payload.
//
// A timeline does not have a "signaled" and "unsignalled" state. Threads instead wait
// for the payload to become a certain value. When a thread signals the timeline, it provides
// a new payload that is greater than the current payload.
//
// There is no way to reset a timeline or to decrease the payload's value. A user must instead
// create a new timeline with a new initial payload if they desire this behavior.
class TimelineSemaphore : public Semaphore, public Object<TimelineSemaphore, VkSemaphore>
{
public:
	TimelineSemaphore(const VkSemaphoreCreateInfo *pCreateInfo, void *mem, const VkAllocationCallbacks *pAllocator);
	TimelineSemaphore();

	static size_t ComputeRequiredAllocationSize(const VkSemaphoreCreateInfo *pCreateInfo);

	// Block until this semaphore is signaled with the specified value;
	void wait(uint64_t value);

	// Wait until a certain amount of time has passed or until the specified value is signaled.
	template<class CLOCK, class DURATION>
	VkResult wait(uint64_t value, const std::chrono::time_point<CLOCK, DURATION> end_ns);

	// Set the payload to the specified value and signal all waiting threads.
	void signal(uint64_t value);

	// Retrieve the current payload. This should not be used to make thread execution decisions
	// as there's no guarantee that the value returned here matches the actual payload's value.
	uint64_t getCounterValue();

	// Dependent timeline semaphores allow an 'any' semaphore to be created that can wait on the
	// state of multiple other timeline semaphores and be signaled like a binary semaphore
	// if any of its parent semaphores are signaled with a certain value.
	//
	// Since a timeline semaphore can be signalled with nearly any value, but threads waiting
	// on a timeline semaphore only unblock when a specific value is signaled, dependents can't
	// naively become signaled whenever their parent semaphores are signaled with a new value.
	// Instead, the dependent semaphore needs to wait for its parent semaphore to be signaled
	// with a specific value as well. This specific value may differ for each parent semaphore.
	//
	// So this function adds other as a dependent semaphore, and tells it to only become unsignaled
	// by this semaphore when this semaphore is signaled with waitValue.
	void addDependent(TimelineSemaphore &other, uint64_t waitValue);
	void addDependency(int id, uint64_t waitValue);

	// Tells this semaphore to become signaled as part of a dependency chain when the parent semaphore
	// with the specified id is signaled with the specified waitValue.
	void addToWaitMap(int parentId, uint64_t waitValue);

	// Clean up any allocated resources
	void destroy(const VkAllocationCallbacks *pAllocator);

private:
	// Track the 64-bit payload. Timeline Semaphores have a shared_ptr<Shared>
	// that they can pass to other Timeline Semaphores to create dependency chains.
	struct Shared
	{
	private:
		// Guards access to all the resources that may be accessed by other threads.
		// No clang Thread Safety Analysis is used on variables guarded by mutex
		// as there is an issue with TSA. Despite instrumenting everything properly,
		// compilation will fail when a lambda function uses a guarded resource.
		marl::mutex mutex;

		static std::atomic<int> nextId;

	public:
		Shared(marl::Allocator *allocator, uint64_t initialState);

		// Block until this semaphore is signaled with the specified value;
		void wait(uint64_t value);
		// Wait until a certain amount of time has passed or until the specified value is signaled.
		template<class CLOCK, class DURATION>
		VkResult wait(uint64_t value, const std::chrono::time_point<CLOCK, DURATION> end_ns);

		// Pass a signal down to a dependent.
		void signal(int parentId, uint64_t value);
		// Set the payload to the specified value and signal all waiting threads.
		void signal(uint64_t value);

		// Retrieve the current payload. This should not be used to make thread execution decisions
		// as there's no guarantee that the value returned here matches the actual payload's value.
		uint64_t getCounterValue();

		// Add the other semaphore's Shared to deps.
		void addDependent(TimelineSemaphore &other);

		// Add {id, waitValue} as a key-value pair to waitMap.
		void addDependency(int id, uint64_t waitValue);

		// Entry point to the marl threading library that handles blocking and unblocking.
		marl::ConditionVariable cv;

		// TODO(b/181683382) -- Add Thread Safety Analysis instrumentation when it can properly
		// analyze lambdas.
		// The 64-bit payload.
		uint64_t counter;

		// A list of this semaphore's dependents.
		marl::containers::vector<std::shared_ptr<Shared>, 1> deps;

		// A map of {parentId: waitValue} pairs that tracks when this semaphore should unblock if it's
		// signaled as a dependent by another semaphore.
		std::map<int, uint64_t> waitMap;

		// An ID that's unique for each instance of Shared
		const int id;
	};

	std::shared_ptr<Shared> shared;
};

template<typename Clock, typename Duration>
VkResult TimelineSemaphore::wait(uint64_t value,
                                 const std::chrono::time_point<Clock, Duration> timeout)
{
	return shared->wait(value, timeout);
}

template<typename Clock, typename Duration>
VkResult TimelineSemaphore::Shared::wait(uint64_t value,
                                         const std::chrono::time_point<Clock, Duration> timeout)
{
	marl::lock lock(mutex);
	if(!cv.wait_until(lock, timeout, [&]() { return counter >= value; }))
	{
		return VK_TIMEOUT;
	}
	return VK_SUCCESS;
}

}  // namespace vk

#endif  // VK_TIMELINE_SEMAPHORE_HPP_
