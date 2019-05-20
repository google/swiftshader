// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef sw_Resource_hpp
#define sw_Resource_hpp

#include "MutexLock.hpp"

namespace sw
{
	enum Accessor
	{
		PUBLIC,    // Application/API access
		PRIVATE,   // Renderer access, shared by multiple threads if read-only
		MANAGED,   // Renderer access, shared read/write access if partitioned
		EXCLUSIVE
	};

	// Resource is a form of shared mutex that guards an internally allocated
	// buffer. Resource has an exclusive lock mode (sw::Accessor) and lock
	// count, defaulting to sw::Accessor::PUBLIC and 0, respectively.
	// Resource doesn't treat any of the sw::Accessor enumerator lock modes
	// differently, all semantic meaning comes from the usage of Resource.
	// You can have multiple locks in mode sw::Accessor::EXCLUSIVE.
	class Resource
	{
	public:
		Resource(size_t bytes);

		// destruct() is an asynchronous destructor, that will atomically:
		//   When the resource is unlocked:
		//     * Delete itself.
		//   When the resource is locked:
		//     * Flag itself for deletion when next fully unlocked.
		void destruct();

		// lock() will atomically:
		//   When the resource is unlocked OR the lock mode equals claimer:
		//     * Increment the lock count.
		//     * Return a pointer to the buffer.
		//   When the resource is locked AND the lock mode does not equal claimer:
		//     * Block until all existing locks are released (lock count equals 0).
		//     * Switch lock mode to claimer.
		//     * Increment the lock count.
		//     * Return a pointer to the buffer.
		void *lock(Accessor claimer);

		// lock() will atomically:
		//   When the resource is unlocked OR the lock mode equals claimer:
		//     * Increment the lock count.
		//     * Return a pointer to the buffer.
		//   When the resource is locked AND the lock mode equals relinquisher:
		//     * Release *all* existing locks (regardless of prior count).
		//     * Delete itself and return nullptr if Resource::destruct() had been called while locked.
		//     * Switch lock mode to claimer.
		//     * Increment the lock count to 1.
		//     * Return a pointer to the buffer.
		//   When the resource is locked AND the lock mode does not equal relinquisher:
		//     * Block until all existing locks are released (lock count equals 0)
		//     * Switch lock mode to claimer
		//     * Increment the lock count to 1.
		//     * Return a pointer to the buffer.
		void *lock(Accessor relinquisher, Accessor claimer);

		// unlock() will atomically:
		// * Assert if there are no locks.
		// * Release a single lock.
		// * Delete itself if Resource::destruct() had been called while locked.
		void unlock();

		// unlock() will atomically:
		//   When the resource is locked AND the lock mode equals relinquisher:
		//     * Release *all* existing locks (regardless of prior count).
		//     * Delete itself if Resource::destruct() had been called while locked.
		//   When the resource is not locked OR the lock mode does not equal relinquisher:
		//     * Do nothing.
		void unlock(Accessor relinquisher);

		// data() will return the Resource's buffer pointer regardless of lock
		// state.
		const void *data() const;

		// size is the size in bytes of the Resource's buffer.
		const size_t size;

	private:
		~Resource();   // Always call destruct() instead

		MutexLock criticalSection;
		Event unblock;
		volatile int blocked;

		volatile Accessor accessor;
		volatile int count;
		bool orphaned;

		void *buffer;
	};
}

#endif   // sw_Resource_hpp
