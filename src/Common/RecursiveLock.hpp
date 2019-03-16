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

#ifndef sw_RecursiveLock_hpp
#define sw_RecursiveLock_hpp

#include "Thread.hpp"

#if defined(__linux__)
// Use a pthread mutex on Linux. Since many processes may use SwiftShader
// at the same time it's best to just have the scheduler overhead.
#include <pthread.h>

namespace sw
{
	class RecursiveLock
	{
	public:
		RecursiveLock()
		{
			pthread_mutexattr_t Attr;
			pthread_mutexattr_init(&Attr);
			pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mutex, &Attr);
			pthread_mutexattr_destroy(&Attr);
		}

		~RecursiveLock()
		{
			pthread_mutex_destroy(&mutex);
		}

		bool attemptLock()
		{
			return pthread_mutex_trylock(&mutex) == 0;
		}

		void lock()
		{
			pthread_mutex_lock(&mutex);
		}

		void unlock()
		{
			ASSERT(pthread_mutex_unlock(&mutex) == 0);
		}

	private:
		pthread_mutex_t mutex;
	};
}

#else   // !__linux__

#include <mutex>

namespace sw
{
	class RecursiveLock
	{
	public:
		RecursiveLock()
		{
		}

		bool attemptLock()
		{
			return mutex.try_lock();
		}

		void lock()
		{
			mutex.lock();
		}

		void unlock()
		{
			mutex.unlock();
		}
	private:
		std::recursive_mutex mutex;
	};
}

#endif   // !__linux__

class RecursiveLockGuard
{
public:
	explicit RecursiveLockGuard(sw::RecursiveLock &mutex) : mutex(&mutex)
	{
		mutex.lock();
	}

	explicit RecursiveLockGuard(sw::RecursiveLock *mutex) : mutex(mutex)
	{
		if (mutex) mutex->lock();
	}

	~RecursiveLockGuard()
	{
		if (mutex) mutex->unlock();
	}

protected:
	sw::RecursiveLock *mutex;
};

#endif   // sw_RecursiveLock_hpp
