// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_SEMAPHORE_EXTERNAL_LINUX_H_
#define VK_SEMAPHORE_EXTERNAL_LINUX_H_

#include "System/Linux/MemFd.hpp"
#include "System/Memory.hpp"

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>

// An external semaphore implementation for Linux, that uses memfd-backed
// shared memory regions as the underlying implementation. The region contains
// a single SharedSemaphore instance, which is a reference-counted semaphore
// implementation based on a pthread process-shared mutex + condition variable
// pair.
//
// This implementation works on any Linux system with at least kernel 3.17
// (which should be sufficient for any not-so-recent Android system) and doesn't
// require special libraries installed on the system.
//
// NOTE: This is not interoperable with other Linux ICDs that use Linux kernel
// sync file objects (which correspond to
// VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT) instead.
//

// A process-shared semaphore implementation that can be stored in
// a process-shared memory region. It also includes a reference count to
// ensure it is only destroyed when the last reference to it is dropped.
class SharedSemaphore
{
public:
	SharedSemaphore()
	{
		pthread_mutexattr_t mattr;
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&mutex, &mattr);
		pthread_mutexattr_destroy(&mattr);

		pthread_condattr_t cattr;
		pthread_condattr_init(&cattr);
		pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
		pthread_cond_init(&cond, &cattr);
		pthread_condattr_destroy(&cattr);
	}

	~SharedSemaphore()
	{
		pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mutex);
	}

	// Increment reference count.
	void addRef()
	{
		pthread_mutex_lock(&mutex);
		ref_count++;
		pthread_mutex_unlock(&mutex);
	}

	// Decrement reference count and returns true iff it reaches 0.
	bool deref()
	{
		pthread_mutex_lock(&mutex);
		bool result = (--ref_count == 0);
		pthread_mutex_unlock(&mutex);
		return result;
	}

	void wait()
	{
		pthread_mutex_lock(&mutex);
		while (!signaled)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		// From Vulkan 1.1.119 spec, section 6.4.2:
		// Unlike fences or events, the act of waiting for a semaphore also
		// unsignals that semaphore.
		signaled = false;
		pthread_mutex_unlock(&mutex);
	}

	// Just like wait() but never blocks. Returns true if the semaphore
	// was signaled (and reset by the function), or false otherwise.
	// Used to avoid using a background thread for waiting in the case
	// where the semaphore is already signaled.
	bool tryWait()
	{
		pthread_mutex_lock(&mutex);
		bool result = signaled;
		if (result)
		{
			signaled = false;
		}
		pthread_mutex_unlock(&mutex);
		return result;
	}

	void signal()
	{
		pthread_mutex_lock(&mutex);
		signaled = true;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}

private:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int ref_count = 1;
	bool signaled = false;
};

namespace vk
{

class Semaphore::External {
public:
	// The type of external semaphore handle types supported by this implementation.
	static const VkExternalSemaphoreHandleTypeFlags kExternalSemaphoreHandleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;

	// Default constructor. Note that one should call either init() or
	// importFd() before any call to wait() or signal().
	External() = default;

	~External() { close(); }

	// Initialize instance by creating a new shared memory region.
	void init()
	{
		// Allocate or import the region's file descriptor.
		const size_t size = sw::memoryPageSize();
		// To be exportable, the PosixSemaphore must be stored in a shared
		// memory region.
		static int counter = 0;
		char name[40];
		snprintf(name, sizeof(name), "SwiftShader.Semaphore.%d", ++counter);
		if (!memfd.allocate(name, size))
		{
			ABORT("memfd.allocate() returned %s", strerror(errno));
		}
		mapRegion(size, true);
	}

	// Import an existing semaphore through its file descriptor.
	VkResult importFd(int fd)
	{
		close();
		memfd.importFd(fd);
		mapRegion(sw::memoryPageSize(), false);
		return VK_SUCCESS;
	}

	// Export the current semaphore as a duplicated file descriptor to the same
	// region. This can be consumed by importFd() running in a different
	// process.
	VkResult exportFd(int* pFd) const
	{
		int fd = memfd.exportFd();
		if (fd < 0)
		{
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
		}
		*pFd = fd;
		return VK_SUCCESS;
	}

	void wait()
	{
		semaphore->wait();
	}

	bool tryWait()
	{
		return semaphore->tryWait();
	}

	void signal()
	{
		semaphore->signal();
	}

private:
	// Unmap the semaphore if needed and close its file descriptor.
	void close()
	{
		if (semaphore)
		{
			if (semaphore->deref())
			{
				semaphore->~SharedSemaphore();
			}
			memfd.unmap(semaphore, sw::memoryPageSize());
			memfd.close();
			semaphore = nullptr;
		}
	}

	// Remap the shared region and setup the semaphore or increment its reference count.
	void mapRegion(size_t size, bool needInitialization)
	{
		// Map the region into memory and point the semaphore to it.
		void* addr = memfd.mapReadWrite(0, size);
		if (!addr)
		{
			ABORT("mmap() failed: %s", strerror(errno));
		}
		semaphore = reinterpret_cast<SharedSemaphore *>(addr);
		if (needInitialization)
		{
			new (semaphore) SharedSemaphore();
		}
		else
		{
			semaphore->addRef();
		}
	}

	LinuxMemFd memfd;
	SharedSemaphore* semaphore = nullptr;
};

}  // namespace vk

#endif  // VK_SEMAPHORE_EXTERNAL_LINUX_H_
