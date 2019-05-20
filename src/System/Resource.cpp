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

#include "Resource.hpp"

#include "Memory.hpp"
#include "Debug.hpp"

namespace sw
{
	Resource::Resource(size_t bytes) : size(bytes)
	{
		buffer = allocate(bytes);
	}

	Resource::~Resource()
	{
		deallocate(buffer);
	}

	void *Resource::lock(Accessor claimer)
	{
		std::unique_lock<std::mutex> lock(mutex);
		return acquire(lock, claimer);
	}

	void *Resource::lock(Accessor relinquisher, Accessor claimer)
	{
		std::unique_lock<std::mutex> lock(mutex);

		// Release
		if (count > 0 && accessor == relinquisher)
		{
			release(lock);
		}

		// Acquire
		acquire(lock, claimer);

		return buffer;
	}

	void Resource::unlock()
	{
		std::unique_lock<std::mutex> lock(mutex);
		release(lock);
	}

	void *Resource::acquire(std::unique_lock<std::mutex> &lock, Accessor claimer)
	{
		while (count > 0 && accessor != claimer)
		{
			blocked++;
			released.wait(lock, [&] { return count == 0 || accessor == claimer; });
			blocked--;
		}

		accessor = claimer;
		count++;
		return buffer;
	}

	void Resource::release(std::unique_lock<std::mutex> &lock)
	{
		ASSERT(count > 0);

		count--;

		if(count == 0)
		{
			if(orphaned)
			{
				lock.unlock();
				delete this;
				return;
			}
			released.notify_one();
		}
	}

	void Resource::destruct()
	{
		std::unique_lock<std::mutex> lock(mutex);
		if(count == 0 && blocked == 0)
		{
			lock.unlock();
			delete this;
			return;
		}
		orphaned = true;
	}

	const void *Resource::data() const
	{
		return buffer;
	}
}
