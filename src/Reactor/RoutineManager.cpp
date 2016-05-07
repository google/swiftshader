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

#include "RoutineManager.hpp"

#include "Routine.hpp"
#include "llvm/Function.h"
#include "../Common/Memory.hpp"
#include "../Common/Thread.hpp"
#include "../Common/Debug.hpp"

namespace sw
{
	using namespace llvm;

	volatile int RoutineManager::averageInstructionSize = 4;

	RoutineManager::RoutineManager()
	{
		routine = 0;
	}

	RoutineManager::~RoutineManager()
	{
		delete routine;
	}

	void RoutineManager::AllocateGOT()
	{
		UNIMPLEMENTED();
	}

	uint8_t *RoutineManager::allocateStub(const GlobalValue *function, unsigned stubSize, unsigned alignment)
	{
		UNIMPLEMENTED();
		return 0;
	}

	uint8_t *RoutineManager::startFunctionBody(const llvm::Function *function, uintptr_t &actualSize)
	{
		if(actualSize == 0)   // Estimate size
		{
			int instructionCount = 0;
			for(llvm::Function::const_iterator basicBlock = function->begin(); basicBlock != function->end(); basicBlock++)
			{
				instructionCount += basicBlock->size();
			}

			actualSize = instructionCount * averageInstructionSize;
		}
		else   // Estimate was too low
		{
			sw::atomicIncrement(&averageInstructionSize);
		}

		// Round up to the next page size
		size_t pageSize = memoryPageSize();
		actualSize = (actualSize + pageSize - 1) & ~(pageSize - 1);

		delete routine;
		routine = new Routine(actualSize);

		return (uint8_t*)routine->getBuffer();
	}

	void RoutineManager::endFunctionBody(const llvm::Function *function, uint8_t *functionStart, uint8_t *functionEnd)
	{
		routine->setFunctionSize(functionEnd - functionStart);
	}

	uint8_t *RoutineManager::startExceptionTable(const llvm::Function* F, uintptr_t &ActualSize)
	{
		UNIMPLEMENTED();
		return 0;
	}

	void RoutineManager::endExceptionTable(const llvm::Function *F, uint8_t *TableStart, uint8_t *TableEnd, uint8_t* FrameRegister)
	{
		UNIMPLEMENTED();
	}

	uint8_t *RoutineManager::getGOTBase() const
	{
		ASSERT(!HasGOT);
		return 0;
	}

	uint8_t *RoutineManager::allocateSpace(intptr_t Size, unsigned Alignment)
	{
		UNIMPLEMENTED();
		return 0;
	}

	uint8_t *RoutineManager::allocateGlobal(uintptr_t Size, unsigned Alignment)
	{
		UNIMPLEMENTED();
		return 0;
	}

	void RoutineManager::deallocateFunctionBody(void *Body)
	{
		delete routine;
		routine = 0;
	}

	void RoutineManager::deallocateExceptionTable(void *ET)
	{
		if(ET)
		{
			UNIMPLEMENTED();
		}
	}

	void RoutineManager::setMemoryWritable()
	{
	}

	void RoutineManager::setMemoryExecutable()
	{
		markExecutable(routine->buffer, routine->bufferSize);
	}

	void RoutineManager::setPoisonMemory(bool poison)
	{
		UNIMPLEMENTED();
	}

	Routine *RoutineManager::acquireRoutine(void *entry)
	{
		routine->entry = entry;

		Routine *result = routine;
		routine = 0;

		return result;
	}
}
