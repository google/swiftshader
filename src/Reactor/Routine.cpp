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

#include "Routine.hpp"

#include "../Common/Memory.hpp"
#include "../Common/Thread.hpp"
#include "../Common/Types.hpp"

namespace sw
{
	Routine::Routine(int bufferSize) : bufferSize(bufferSize), dynamic(true)
	{
		void *memory = allocateExecutable(bufferSize);

		buffer = memory;
		entry = memory;
		functionSize = bufferSize;   // Updated by RoutineManager::endFunctionBody

		bindCount = 0;
	}

	Routine::Routine(void *memory, int bufferSize, int offset) : bufferSize(bufferSize), functionSize(bufferSize), dynamic(false)
	{
		buffer = (unsigned char*)memory - offset;
		entry = memory;

		bindCount = 0;
	}

	Routine::~Routine()
	{
		if(dynamic)
		{
			deallocateExecutable(buffer, bufferSize);
		}
	}

	void Routine::setFunctionSize(int functionSize)
	{
		this->functionSize = functionSize;
	}

	const void *Routine::getBuffer()
	{
		return buffer;
	}

	const void *Routine::getEntry()
	{
		return entry;
	}

	int Routine::getBufferSize()
	{
		return bufferSize;
	}

	int Routine::getFunctionSize()
	{
		return functionSize;
	}

	int Routine::getCodeSize()
	{
		return functionSize - ((uintptr_t)entry - (uintptr_t)buffer);
	}

	bool Routine::isDynamic()
	{
		return dynamic;
	}

	void Routine::bind()
	{
		atomicIncrement(&bindCount);
	}

	void Routine::unbind()
	{
		long count = atomicDecrement(&bindCount);

		if(count == 0)
		{
			delete this;
		}
	}
}
