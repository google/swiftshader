// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Resource.hpp"

#include "Memory.hpp"

namespace sw
{
	Resource::Resource(size_t bytes) : size(bytes)
	{
		blocked = 0;

		accessor = PUBLIC;
		count = 0;
		orphaned = false;

		buffer = allocateZero(bytes);
	}

	Resource::~Resource()
	{
 		deallocate(buffer);
	}

	void *Resource::lock(Accessor claimer)
	{
		criticalSection.lock();

		while(count != 0 && accessor != claimer)
		{
			blocked++;
			criticalSection.unlock();

			unblock.wait();

			criticalSection.lock();
			blocked--;
		}

		accessor = claimer;
		count++;

		criticalSection.unlock();

		return buffer;
	}

	void *Resource::lock(Accessor relinquisher, Accessor claimer)
	{
		criticalSection.lock();

		// Release
		while(count > 0 && accessor == relinquisher)
		{
			count--;

			if(count == 0)
			{
				if(blocked)
				{
					unblock.signal();
				}
				else if(orphaned)
				{
					criticalSection.unlock();

					delete this;

					return 0;
				}
			}
		}

		// Acquire
		while(count != 0 && accessor != claimer)
		{
			blocked++;
			criticalSection.unlock();

			unblock.wait();

			criticalSection.lock();
			blocked--;
		}

		accessor = claimer;
		count++;

		criticalSection.unlock();

		return buffer;
	}

	void Resource::unlock()
	{
		criticalSection.lock();

		count--;

		if(count == 0)
		{
			if(blocked)
			{
				unblock.signal();
			}
			else if(orphaned)
			{
				criticalSection.unlock();

				delete this;

				return;
			}
		}

		criticalSection.unlock();
	}

	void Resource::unlock(Accessor relinquisher)
	{
		criticalSection.lock();

		while(count > 0 && accessor == relinquisher)
		{
			count--;

			if(count == 0)
			{
				if(blocked)
				{
					unblock.signal();
				}
				else if(orphaned)
				{
					criticalSection.unlock();

					delete this;

					return;
				}
			}
		}

		criticalSection.unlock();
	}

	void Resource::destruct()
	{
		criticalSection.lock();

		if(count == 0 && !blocked)
		{
			criticalSection.unlock();

			delete this;

			return;
		}

		orphaned = true;

		criticalSection.unlock();
	}

	const void *Resource::data() const
	{
		return buffer;
	}
}
