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

#include "MemoryManager.hpp"

#include "Nucleus.hpp"

namespace sw
{
	using namespace llvm;

	MemoryManager::MemoryManager()
	{
		routine = 0;
	}

	MemoryManager::~MemoryManager()
	{
		delete routine;
	}

	void MemoryManager::AllocateGOT()
	{
		// FIXME: ASSERT(false);
	}

	unsigned char *MemoryManager::allocateStub(const GlobalValue *function, unsigned stubSize, unsigned alignment)
	{
		// FIXME: ASSERT(false);

		return 0;
	}

	unsigned char *MemoryManager::startFunctionBody(const llvm::Function *function, uintptr_t &actualSize)
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

	void MemoryManager::endFunctionBody(const llvm::Function *function, unsigned char *functionStart, unsigned char *functionEnd)
	{
		routine->setFunctionSize(functionEnd - functionStart);
	}

	unsigned char *MemoryManager::startExceptionTable(const llvm::Function* F, uintptr_t &ActualSize)
	{
		// FIXME: ASSERT(false);

		return 0;
	}

	void MemoryManager::endExceptionTable(const llvm::Function *F, unsigned char *TableStart, unsigned char *TableEnd, unsigned char* FrameRegister) 
	{
		// FIXME: ASSERT(false);
	}
    
	unsigned char *MemoryManager::getGOTBase() const
	{
		return 0;
	}

	unsigned char *MemoryManager::allocateSpace(intptr_t Size, unsigned Alignment)
	{
		return 0;
	}

	unsigned char *MemoryManager::allocateGlobal(uintptr_t Size, unsigned Alignment)
	{
		return 0;
	}

	void MemoryManager::deallocateFunctionBody(void *Body)
	{
	}

	void MemoryManager::deallocateExceptionTable(void *ET)
	{
	}

	void MemoryManager::setMemoryWritable()
	{
	}

	void MemoryManager::setMemoryExecutable()
	{
	}

	void MemoryManager::setPoisonMemory(bool poison)
	{
	}

	Routine *MemoryManager::acquireRoutine()
	{
		Routine *result = routine;

		routine = 0;

		return result;
	}

	void MemoryManager::SetDlsymTable(void *pointer)
	{
	}

	void *MemoryManager::getDlsymTable() const
	{
		return 0;
	}
}
