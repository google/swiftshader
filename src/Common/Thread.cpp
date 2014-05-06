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

#include "Thread.hpp"

namespace sw
{
	Thread::Thread(void (*threadFunction)(void *parameters), void *parameters)
	{
		Event init;
		Entry entry = {threadFunction, parameters, &init};
		
		#if defined(_WIN32)
			handle = CreateThread(0, 1024 * 1024, startFunction, &entry, 0, 0);
		#elif defined(__APPLE__)
			pthread_create(&handle, 0, startFunction, &entry);
		#else
			#error Unimplemented platform
		#endif
		
		init.wait();
	}

	Thread::~Thread()
	{		
		#if defined(_WIN32)
			CloseHandle(handle);
		#elif defined(__APPLE__)
			pthread_cancel(handle);
		#else
			#error Unimplemented platform
		#endif
	}

	void Thread::join()
	{
		#if defined(_WIN32)
			WaitForSingleObject(handle, INFINITE);
		#elif defined(__APPLE__)
			pthread_join(handle, 0);
		#else
			#error Unimplemented platform
		#endif
	}

	#if defined(_WIN32)
		unsigned long __stdcall Thread::startFunction(void *parameters)
		{
			Entry entry = *(Entry*)parameters;
			entry.init->signal();
			entry.threadFunction(entry.threadParameters);
			return 0;
		}
	#elif defined(__APPLE__)
		void *Thread::startFunction(void *parameters)
		{
			Entry entry = *(Entry*)parameters;
			entry.init->signal();
			entry.threadFunction(entry.threadParameters);
			return 0;
		}
	#else
		#error Unimplemented platform
	#endif

	Event::Event()
	{
		#if defined(_WIN32)
			handle = CreateEvent(0, false, false, 0);
		#elif defined(__APPLE__)
			pthread_cond_init(&handle, 0);
			pthread_mutex_init(&mutex, 0);
			signaled = false;
		#else
			#error Unimplemented platform
		#endif
	}

	Event::~Event()
	{
		#if defined(_WIN32)
			CloseHandle(handle);
		#elif defined(__APPLE__)
			pthread_cond_destroy(&handle);
			pthread_mutex_destroy(&mutex);
		#else
			#error Unimplemented platform
		#endif
	}
}
