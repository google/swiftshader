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
