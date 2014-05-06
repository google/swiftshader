// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Thread_hpp
#define sw_Thread_hpp

#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <intrin.h>
#elif defined(__APPLE__)
	#include <pthread.h>
#else
	#error Unimplemented platform
#endif

#include "Types.hpp"

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
		#elif defined(__APPLE__)
			static void *startFunction(void *parameters);
			pthread_t handle;
		#else
			#error Unimplemented platform
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
		#elif defined(__APPLE__)
			pthread_cond_t handle;
			pthread_mutex_t mutex;
			volatile bool signaled;
		#else
			#error Unimplemented platform
		#endif
	};

	#if PERF_PROFILE
	int64_t atomicExchange(int64_t volatile *target, int64_t value);
	#endif

	int atomicExchange(int volatile *target, int value);
	int atomicIncrement(int volatile *value);
	int atomicDecrement(int volatile *value);
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
			#error Unimplemented platform
		#endif
	}

	inline void Thread::sleep(int milliseconds)
	{
		#if defined(_WIN32)
			Sleep(milliseconds);
		#elif defined(__APPLE__)
			nap(milliseconds);
		#else
			#error Unimplemented platform
		#endif
	}

	inline void Event::signal()
	{
		#if defined(_WIN32)
			SetEvent(handle);
		#elif defined(__APPLE__)
			pthread_mutex_lock(&mutex);
			signaled = true;
			pthread_cond_signal(&handle);
			pthread_mutex_unlock(&mutex);
		#else
			#error Unimplemented platform
		#endif
	}

	inline void Event::wait()
	{
		#if defined(_WIN32)
			WaitForSingleObject(handle, INFINITE);
		#elif defined(__APPLE__)
			pthread_mutex_lock(&mutex);
			while(!signaled) pthread_cond_wait(&handle, &mutex);
			signaled = false;
			pthread_mutex_unlock(&mutex);
		#else
			#error Unimplemented platform
		#endif
	}

	#if PERF_PROFILE
	inline int64_t atomicExchange(volatile int64_t *target, int64_t value)
	{
		#if defined(_WIN32)
			return InterlockedExchange64(target, value);
		#elif defined(__APPLE__)
			int ret;
			__asm__ __volatile__("lock; xchg8 %0,(%1)" : "=r" (ret) :"r" (target), "0" (value) : "memory" );
			return ret;
		#else
			#error Unimplemented platform
		#endif
	}
	#endif

	inline int atomicExchange(volatile int *target, int value)
	{
		#if defined(_WIN32)
			return InterlockedExchange((volatile long*)target, (long)value);
		#elif defined(__APPLE__)
			int ret;
			__asm__ __volatile__("lock; xchgl %0,(%1)" : "=r" (ret) :"r" (target), "0" (value) : "memory" );
			return ret;
		#else
			#error Unimplemented platform
		#endif
	}

	inline int atomicIncrement(volatile int *value)
	{
		#if defined(_WIN32)
			return InterlockedIncrement((volatile long*)value);
		#elif defined(__APPLE__)
			 int ret;
			__asm__ __volatile__( "lock; xaddl %0,(%1)" : "=r" (ret) : "r" (dest), "0" (1) : "memory" );
			return ret + 1;
		#else
			#error Unimplemented platform
		#endif
	}

	inline int atomicDecrement(volatile int *value)
	{
		#if defined(_WIN32)
			return InterlockedDecrement((volatile long*)value);
		#elif defined(__APPLE__)
			int ret;
			__asm__ __volatile__( "lock; xaddl %0,(%1)" : "=r" (ret) : "r" (dest), "0" (-1) : "memory" );
			return ret - 1;
		#else
			#error Unimplemented platform
		#endif
	}

	inline void nop()
	{
		#if defined(_WIN32)
			__nop();
		#elif defined(__APPLE__)
			__asm__ __volatile__ ("nop");
		#else
			#error Unimplemented platform
		#endif
	}
}

#endif   // sw_Thread_hpp
