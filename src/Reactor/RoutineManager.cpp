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

#include "Nucleus.hpp"

namespace sw
{
	using namespace llvm;

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
		// FIXME: ASSERT(false);
	}

	unsigned char *RoutineManager::allocateStub(const GlobalValue *function, unsigned stubSize, unsigned alignment)
	{
		// FIXME: ASSERT(false);

		return 0;
	}

	unsigned char *RoutineManager::startFunctionBody(const llvm::Function *function, uintptr_t &actualSize)
	{
		if(actualSize == 0)
		{
			actualSize = 4096;
		}

		actualSize = (actualSize + 15) & -16;

		delete routine;
		routine = new Routine(actualSize);

		return (unsigned char*)routine->getBuffer();
	}

	void RoutineManager::endFunctionBody(const llvm::Function *function, unsigned char *functionStart, unsigned char *functionEnd)
	{
		routine->setFunctionSize(functionEnd - functionStart);
	}

	unsigned char *RoutineManager::startExceptionTable(const llvm::Function* F, uintptr_t &ActualSize)
	{
		// FIXME: ASSERT(false);

		return 0;
	}

	void RoutineManager::endExceptionTable(const llvm::Function *F, unsigned char *TableStart, unsigned char *TableEnd, unsigned char* FrameRegister) 
	{
		// FIXME: ASSERT(false);
	}
    
	unsigned char *RoutineManager::getGOTBase() const
	{
		return 0;
	}

	unsigned char *RoutineManager::allocateSpace(intptr_t Size, unsigned Alignment)
	{
		return 0;
	}

	unsigned char *RoutineManager::allocateGlobal(uintptr_t Size, unsigned Alignment)
	{
		return 0;
	}

	void RoutineManager::deallocateFunctionBody(void *Body)
	{
	}

	void RoutineManager::deallocateExceptionTable(void *ET)
	{
	}

	void RoutineManager::setMemoryWritable()
	{
	}

	void RoutineManager::setMemoryExecutable()
	{
	}

	void RoutineManager::setPoisonMemory(bool poison)
	{
	}

	Routine *RoutineManager::acquireRoutine()
	{
		Routine *result = routine;

		routine = 0;

		return result;
	}

	void RoutineManager::SetDlsymTable(void *pointer)
	{
	}

	void *RoutineManager::getDlsymTable() const
	{
		return 0;
	}
}
