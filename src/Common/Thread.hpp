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

#ifndef sw_Thread_hpp
#define sw_Thread_hpp

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <intrin.h>
#else
	#include <pthread.h>
	#include <sched.h>
	#include <unistd.h>
	#define TLS_OUT_OF_INDEXES (~0)
#endif

namespace sw
{
	class Event;

	class Thread
	{
	public:
		Thread(void (*threadFunction)(void *parameters), void *parameters);

		~Thread();

		void join();

		static void yield();
		static void sleep(int milliseconds);

		#if defined(_WIN32)
			typedef DWORD LocalStorageKey;
		#else
			typedef pthread_key_t LocalStorageKey;
		#endif

		static LocalStorageKey allocateLocalStorageKey();
		static void freeLocalStorageKey(LocalStorageKey key);
		static void setLocalStorage(LocalStorageKey key, void *value);
		static void *getLocalStorage(LocalStorageKey key);

	private:
		struct Entry
		{
			void (*const threadFunction)(void *parameters);
			void *threadParameters;
			Event *init;
		};

		#if defined(_WIN32)
			static unsigned long __stdcall startFunction(void *parameters);
			HANDLE handle;
		#else
			static void *startFunction(void *parameters);
			pthread_t handle;
		#endif
	};

	class Event
	{
		friend class Thread;

	public:
		Event();

		~Event();

		void signal();
		void wait();

	private:
		#if defined(_WIN32)
			HANDLE handle;
		#else
			pthread_cond_t handle;
			pthread_mutex_t mutex;
			volatile bool signaled;
		#endif
	};

	#if PERF_PROFILE
	int64_t atomicExchange(int64_t volatile *target, int64_t value);
	#endif

	int atomicExchange(int volatile *target, int value);
	int atomicIncrement(int volatile *value);
	int atomicDecrement(int volatile *value);
	int atomicAdd(int volatile *target, int value);
	void nop();
}

namespace sw
{
	inline void Thread::yield()
	{
		#if defined(_WIN32)
			Sleep(0);
		#elif defined(__APPLE__)
			pthread_yield_np();
		#else
			sched_yield();
		#endif
	}

	inline void Thread::sleep(int milliseconds)
	{
		#if defined(_WIN32)
			Sleep(milliseconds);
		#else
			usleep(1000 * milliseconds);
		#endif
	}

	inline Thread::LocalStorageKey Thread::allocateLocalStorageKey()
	{
		#if defined(_WIN32)
			return TlsAlloc();
		#else
			LocalStorageKey key;
			pthread_key_create(&key, 0);
			return key;
		#endif
	}

	inline void Thread::freeLocalStorageKey(LocalStorageKey key)
	{
		#if defined(_WIN32)
			TlsFree(key);
		#else
			pthread_key_delete(key);
		#endif
	}

	inline void Thread::setLocalStorage(LocalStorageKey key, void *value)
	{
		#if defined(_WIN32)
			TlsSetValue(key, value);
		#else
			pthread_setspecific(key, value);
		#endif
	}

	inline void *Thread::getLocalStorage(LocalStorageKey key)
	{
		#if defined(_WIN32)
			return TlsGetValue(key);
		#else
			return pthread_getspecific(key);
		#endif
	}

	inline void Event::signal()
	{
		#if defined(_WIN32)
			SetEvent(handle);
		#else
			pthread_mutex_lock(&mutex);
			signaled = true;
			pthread_cond_signal(&handle);
			pthread_mutex_unlock(&mutex);
		#endif
	}

	inline void Event::wait()
	{
		#if defined(_WIN32)
			WaitForSingleObject(handle, INFINITE);
		#else
			pthread_mutex_lock(&mutex);
			while(!signaled) pthread_cond_wait(&handle, &mutex);
			signaled = false;
			pthread_mutex_unlock(&mutex);
		#endif
	}

	#if PERF_PROFILE
	inline int64_t atomicExchange(volatile int64_t *target, int64_t value)
	{
		#if defined(_WIN32)
			return InterlockedExchange64(target, value);
		#else
			int ret;
			__asm__ __volatile__("lock; xchg8 %0,(%1)" : "=r" (ret) :"r" (target), "0" (value) : "memory" );
			return ret;
		#endif
	}
	#endif

	inline int atomicExchange(volatile int *target, int value)
	{
		#if defined(_WIN32)
			return InterlockedExchange((volatile long*)target, (long)value);
		#else
			int ret;
			__asm__ __volatile__("lock; xchgl %0,(%1)" : "=r" (ret) :"r" (target), "0" (value) : "memory" );
			return ret;
		#endif
	}

	inline int atomicIncrement(volatile int *value)
	{
		#if defined(_WIN32)
			return InterlockedIncrement((volatile long*)value);
		#else
			return __sync_add_and_fetch(value, 1);
		#endif
	}

	inline int atomicDecrement(volatile int *value)
	{
		#if defined(_WIN32)
			return InterlockedDecrement((volatile long*)value);
		#else
			return __sync_sub_and_fetch(value, 1);
		#endif
	}

	inline int atomicAdd(volatile int* target, int value)
	{
		#if defined(_MSC_VER)
			return InterlockedExchangeAdd((volatile long*)target, value) + value;
		#else
			return __sync_add_and_fetch(target, value);
		#endif
	}

	inline void nop()
	{
		#if defined(_WIN32)
			__nop();
		#else
			__asm__ __volatile__ ("nop");
		#endif
	}
}

#endif   // sw_Thread_hpp
