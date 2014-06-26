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

		~RoutineManager();

		void AllocateGOT();

		unsigned char *allocateStub(const llvm::GlobalValue *function, unsigned stubSize, unsigned alignment);
		unsigned char *startFunctionBody(const llvm::Function *function, uintptr_t &actualSize);
		void endFunctionBody(const llvm::Function *function, unsigned char *functionStart, unsigned char *functionEnd);
		unsigned char *startExceptionTable(const llvm::Function *function, uintptr_t &ActualSize);
		void endExceptionTable(const llvm::Function *function, unsigned char *tableStart, unsigned char *tableEnd, unsigned char *frameRegister);
		unsigned char *getGOTBase() const;
		unsigned char *allocateSpace(intptr_t Size, unsigned Alignment);
		unsigned char *allocateGlobal(uintptr_t Size, unsigned int Alignment);
		void deallocateFunctionBody(void *Body);
		void deallocateExceptionTable(void *ET);
		void setMemoryWritable();
		void setMemoryExecutable();
		void setPoisonMemory(bool poison);
		void SetDlsymTable(void *pointer);
		void *getDlsymTable() const;

		Routine *acquireRoutine();

	private:
		Routine *routine;
	};
}

#endif   // sw_RoutineManager_hpp
