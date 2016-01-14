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

#ifndef sw_MutexLock_hpp
#define sw_MutexLock_hpp

#include "Thread.hpp"

namespace sw
{
	class BackoffLock
	{
	public:
		BackoffLock()
		{
			mutex = 0;
		}

		bool attemptLock()
		{
			if(!isLocked())
			{
				if(atomicExchange(&mutex, 1) == 0)
				{
					return true;
				}
			}

			return false;
		}

		void lock()
		{
			int backoff = 1;

			while(!attemptLock())
			{
				if(backoff <= 64)
				{
					for(int i = 0; i < backoff; i++)
					{
						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();

						nop();
						nop();
						nop();
						nop();
						nop();
					}

					backoff *= 2;
				}
				else
				{
					Thread::yield();

					backoff = 1;
				}
			};
		}

		void unlock()
		{
			mutex = 0;
		}

		bool isLocked()
		{
			return mutex != 0;
		}

	private:
		struct
		{
			// Ensure that the mutex variable is on its own 64-byte cache line to avoid false sharing
			// Padding must be public to avoid compiler warnings
			volatile int padding1[16];
			volatile int mutex;
			volatile int padding2[15];
		};
	};
}

#endif   // sw_MutexLock_hpp
