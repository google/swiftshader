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
		actualSize = (actualSize + pageSize - 1) & -pageSize;

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
		UNIMPLEMENTED();
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
