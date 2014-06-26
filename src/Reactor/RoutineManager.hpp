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

#ifndef sw_RoutineManager_hpp
#define sw_RoutineManager_hpp

#include "llvm/GlobalValue.h"
#include "llvm/ExecutionEngine/JITMemoryManager.h"

namespace sw
{
	class Routine;

	class RoutineManager : public llvm::JITMemoryManager
	{
	public:
		RoutineManager();

		virtual ~RoutineManager();

		virtual void AllocateGOT();

		virtual uint8_t *allocateStub(const llvm::GlobalValue *function, unsigned stubSize, unsigned alignment);
		virtual uint8_t *startFunctionBody(const llvm::Function *function, uintptr_t &actualSize);
		virtual void endFunctionBody(const llvm::Function *function, uint8_t *functionStart, uint8_t *functionEnd);
		virtual uint8_t *startExceptionTable(const llvm::Function *function, uintptr_t &ActualSize);
		virtual void endExceptionTable(const llvm::Function *function, uint8_t *tableStart, uint8_t *tableEnd, uint8_t *frameRegister);
		virtual uint8_t *getGOTBase() const;
		virtual uint8_t *allocateSpace(intptr_t Size, unsigned Alignment);
		virtual uint8_t *allocateGlobal(uintptr_t Size, unsigned int Alignment);
		virtual void deallocateFunctionBody(void *Body);
		virtual void deallocateExceptionTable(void *ET);
		virtual void setMemoryWritable();
		virtual void setMemoryExecutable();
		virtual void setPoisonMemory(bool poison);

		Routine *acquireRoutine();

	private:
		Routine *routine;

		static volatile int averageInstructionSize;
	};
}

#endif   // sw_RoutineManager_hpp
