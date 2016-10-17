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

#include "Nucleus.hpp"

#include "llvm/Support/IRBuilder.h"
#include "llvm/Function.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Module.h"
#include "llvm/LLVMContext.h"
#include "llvm/Constants.h"
#include "llvm/Intrinsics.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "../lib/ExecutionEngine/JIT/JIT.h"

#include "LLVMRoutine.hpp"
#include "LLVMRoutineManager.hpp"
#include "x86.hpp"
#include "CPUID.hpp"
#include "Thread.hpp"
#include "Memory.hpp"
#include "MutexLock.hpp"

#include <xmmintrin.h>
#include <fstream>

#if defined(__x86_64__) && defined(_WIN32)
extern "C" void X86CompilationCallback()
{
	assert(false);   // UNIMPLEMENTED
}
#endif

extern "C"
{
	bool (*CodeAnalystInitialize)() = 0;
	void (*CodeAnalystCompleteJITLog)() = 0;
	bool (*CodeAnalystLogJITCode)(const void *jitCodeStartAddr, unsigned int jitCodeSize, const wchar_t *functionName) = 0;
}

namespace llvm
{
	extern bool JITEmitDebugInfo;
}

namespace
{
	sw::LLVMRoutineManager *routineManager = nullptr;
	llvm::ExecutionEngine *executionEngine = nullptr;
	llvm::IRBuilder<> *builder = nullptr;
	llvm::LLVMContext *context = nullptr;
	llvm::Module *module = nullptr;
	llvm::Function *function = nullptr;

	sw::BackoffLock codegenMutex;
}

namespace sw
{
	using namespace llvm;

	Optimization optimization[10] = {InstructionCombining, Disabled};

	class Type : public llvm::Type {};
	class Value : public llvm::Value {};
	class Constant : public llvm::Constant {};
	class BasicBlock : public llvm::BasicBlock {};

	inline Type *T(llvm::Type *t)
	{
		return reinterpret_cast<Type*>(t);
	}

	inline Value *V(llvm::Value *t)
	{
		return reinterpret_cast<Value*>(t);
	}

	inline std::vector<llvm::Type*> &T(std::vector<Type*> &t)
	{
		return reinterpret_cast<std::vector<llvm::Type*>&>(t);
	}

	inline Constant *C(llvm::Constant *c)
	{
		return reinterpret_cast<Constant*>(c);
	}

	inline BasicBlock *B(llvm::BasicBlock *t)
	{
		return reinterpret_cast<BasicBlock*>(t);
	}

	Nucleus::Nucleus()
	{
		::codegenMutex.lock();   // Reactor and LLVM are currently not thread safe

		InitializeNativeTarget();
		JITEmitDebugInfo = false;

		if(!::context)
		{
			::context = new LLVMContext();
		}

		::module = new Module("", *::context);
		::routineManager = new LLVMRoutineManager();

		#if defined(__x86_64__)
			const char *architecture = "x86-64";
		#else
			const char *architecture = "x86";
		#endif

		SmallVector<std::string, 1> MAttrs;
		MAttrs.push_back(CPUID::supportsMMX()    ? "+mmx"   : "-mmx");
		MAttrs.push_back(CPUID::supportsCMOV()   ? "+cmov"  : "-cmov");
		MAttrs.push_back(CPUID::supportsSSE()    ? "+sse"   : "-sse");
		MAttrs.push_back(CPUID::supportsSSE2()   ? "+sse2"  : "-sse2");
		MAttrs.push_back(CPUID::supportsSSE3()   ? "+sse3"  : "-sse3");
		MAttrs.push_back(CPUID::supportsSSSE3()  ? "+ssse3" : "-ssse3");
		MAttrs.push_back(CPUID::supportsSSE4_1() ? "+sse41" : "-sse41");

		std::string error;
		TargetMachine *targetMachine = EngineBuilder::selectTarget(::module, architecture, "", MAttrs, Reloc::Default, CodeModel::JITDefault, &error);
		::executionEngine = JIT::createJIT(::module, 0, ::routineManager, CodeGenOpt::Aggressive, true, targetMachine);

		if(!::builder)
		{
			::builder = new IRBuilder<>(*::context);

			#if defined(_WIN32)
				HMODULE CodeAnalyst = LoadLibrary("CAJitNtfyLib.dll");
				if(CodeAnalyst)
				{
					CodeAnalystInitialize = (bool(*)())GetProcAddress(CodeAnalyst, "CAJIT_Initialize");
					CodeAnalystCompleteJITLog = (void(*)())GetProcAddress(CodeAnalyst, "CAJIT_CompleteJITLog");
					CodeAnalystLogJITCode = (bool(*)(const void*, unsigned int, const wchar_t*))GetProcAddress(CodeAnalyst, "CAJIT_LogJITCode");

					CodeAnalystInitialize();
				}
			#endif
		}
	}

	Nucleus::~Nucleus()
	{
		delete ::executionEngine;
		::executionEngine = nullptr;

		::routineManager = nullptr;
		::function = nullptr;
		::module = nullptr;

		::codegenMutex.unlock();
	}

	Routine *Nucleus::acquireRoutine(const wchar_t *name, bool runOptimizations)
	{
		if(::builder->GetInsertBlock()->empty() || !::builder->GetInsertBlock()->back().isTerminator())
		{
			llvm::Type *type = ::function->getReturnType();

			if(type->isVoidTy())
			{
				createRetVoid();
			}
			else
			{
				createRet(V(UndefValue::get(type)));
			}
		}

		if(false)
		{
			std::string error;
			raw_fd_ostream file("llvm-dump-unopt.txt", error);
			::module->print(file, 0);
		}

		if(runOptimizations)
		{
			optimize();
		}

		if(false)
		{
			std::string error;
			raw_fd_ostream file("llvm-dump-opt.txt", error);
			::module->print(file, 0);
		}

		void *entry = ::executionEngine->getPointerToFunction(::function);
		LLVMRoutine *routine = ::routineManager->acquireRoutine(entry);

		if(CodeAnalystLogJITCode)
		{
			CodeAnalystLogJITCode(routine->getEntry(), routine->getCodeSize(), name);
		}

		return routine;
	}

	void Nucleus::optimize()
	{
		static PassManager *passManager = nullptr;

		if(!passManager)
		{
			passManager = new PassManager();

			UnsafeFPMath = true;
		//	NoInfsFPMath = true;
		//	NoNaNsFPMath = true;

			passManager->add(new TargetData(*::executionEngine->getTargetData()));
			passManager->add(createScalarReplAggregatesPass());

			for(int pass = 0; pass < 10 && optimization[pass] != Disabled; pass++)
			{
				switch(optimization[pass])
				{
				case Disabled:                                                                 break;
				case CFGSimplification:    passManager->add(createCFGSimplificationPass());    break;
				case LICM:                 passManager->add(createLICMPass());                 break;
				case AggressiveDCE:        passManager->add(createAggressiveDCEPass());        break;
				case GVN:                  passManager->add(createGVNPass());                  break;
				case InstructionCombining: passManager->add(createInstructionCombiningPass()); break;
				case Reassociate:          passManager->add(createReassociatePass());          break;
				case DeadStoreElimination: passManager->add(createDeadStoreEliminationPass()); break;
				case SCCP:                 passManager->add(createSCCPPass());                 break;
				case ScalarReplAggregates: passManager->add(createScalarReplAggregatesPass()); break;
				default:
					assert(false);
				}
			}
		}

		passManager->run(*::module);
	}

	Value *Nucleus::allocateStackVariable(Type *type, int arraySize)
	{
		// Need to allocate it in the entry block for mem2reg to work
		llvm::BasicBlock &entryBlock = ::function->getEntryBlock();

		Instruction *declaration;

		if(arraySize)
		{
			declaration = new AllocaInst(type, Nucleus::createConstantInt(arraySize));
		}
		else
		{
			declaration = new AllocaInst(type, (Value*)0);
		}

		entryBlock.getInstList().push_front(declaration);

		return V(declaration);
	}

	BasicBlock *Nucleus::createBasicBlock()
	{
		return B(BasicBlock::Create(*::context, "", ::function));
	}

	BasicBlock *Nucleus::getInsertBlock()
	{
		return B(::builder->GetInsertBlock());
	}

	void Nucleus::setInsertBlock(BasicBlock *basicBlock)
	{
	//	assert(::builder->GetInsertBlock()->back().isTerminator());
		return ::builder->SetInsertPoint(basicBlock);
	}

	BasicBlock *Nucleus::getPredecessor(BasicBlock *basicBlock)
	{
		return B(*pred_begin(basicBlock));
	}

	void Nucleus::createFunction(Type *ReturnType, std::vector<Type*> &Params)
	{
		llvm::FunctionType *functionType = llvm::FunctionType::get(ReturnType, T(Params), false);
		::function = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "", ::module);
		::function->setCallingConv(llvm::CallingConv::C);

		::builder->SetInsertPoint(BasicBlock::Create(*::context, "", ::function));
	}

	Value *Nucleus::getArgument(unsigned int index)
	{
		llvm::Function::arg_iterator args = ::function->arg_begin();

		while(index)
		{
			args++;
			index--;
		}

		return V(&*args);
	}

	void Nucleus::createRetVoid()
	{
		x86::emms();

		::builder->CreateRetVoid();
	}

	void Nucleus::createRet(Value *v)
	{
		x86::emms();

		::builder->CreateRet(v);
	}

	void Nucleus::createBr(BasicBlock *dest)
	{
		::builder->CreateBr(dest);
	}

	void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
	{
		::builder->CreateCondBr(cond, ifTrue, ifFalse);
	}

	Value *Nucleus::createAdd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAdd(lhs, rhs));
	}

	Value *Nucleus::createSub(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSub(lhs, rhs));
	}

	Value *Nucleus::createMul(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateMul(lhs, rhs));
	}

	Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateUDiv(lhs, rhs));
	}

	Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSDiv(lhs, rhs));
	}

	Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFAdd(lhs, rhs));
	}

	Value *Nucleus::createFSub(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFSub(lhs, rhs));
	}

	Value *Nucleus::createFMul(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFMul(lhs, rhs));
	}

	Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFDiv(lhs, rhs));
	}

	Value *Nucleus::createURem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateURem(lhs, rhs));
	}

	Value *Nucleus::createSRem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSRem(lhs, rhs));
	}

	Value *Nucleus::createFRem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFRem(lhs, rhs));
	}

	Value *Nucleus::createShl(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateShl(lhs, rhs));
	}

	Value *Nucleus::createLShr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateLShr(lhs, rhs));
	}

	Value *Nucleus::createAShr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAShr(lhs, rhs));
	}

	Value *Nucleus::createAnd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAnd(lhs, rhs));
	}

	Value *Nucleus::createOr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateOr(lhs, rhs));
	}

	Value *Nucleus::createXor(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateXor(lhs, rhs));
	}

	Value *Nucleus::createAssign(Constant *constant)
	{
		return V(constant);
	}

	Value *Nucleus::createNeg(Value *v)
	{
		return V(::builder->CreateNeg(v));
	}

	Value *Nucleus::createFNeg(Value *v)
	{
		return V(::builder->CreateFNeg(v));
	}

	Value *Nucleus::createNot(Value *v)
	{
		return V(::builder->CreateNot(v));
	}

	Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		assert(ptr->getType()->getContainedType(0) == type);
		return V(::builder->Insert(new LoadInst(ptr, "", isVolatile, align)));
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		assert(ptr->getType()->getContainedType(0) == type);
		::builder->Insert(new StoreInst(value, ptr, isVolatile, align));
		return value;
	}

	Constant *Nucleus::createStore(Constant *constant, Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		assert(ptr->getType()->getContainedType(0) == type);
		::builder->Insert(new StoreInst(constant, ptr, isVolatile, align));
		return constant;
	}

	Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index)
	{
		assert(ptr->getType()->getContainedType(0) == type);
		return V(::builder->CreateGEP(ptr, index));
	}

	Value *Nucleus::createAtomicAdd(Value *ptr, Value *value)
	{
		return V(::builder->CreateAtomicRMW(AtomicRMWInst::Add, ptr, value, SequentiallyConsistent));
	}

	Value *Nucleus::createTrunc(Value *v, Type *destType)
	{
		return V(::builder->CreateTrunc(v, destType));
	}

	Value *Nucleus::createZExt(Value *v, Type *destType)
	{
		return V(::builder->CreateZExt(v, destType));
	}

	Value *Nucleus::createSExt(Value *v, Type *destType)
	{
		return V(::builder->CreateSExt(v, destType));
	}

	Value *Nucleus::createFPToSI(Value *v, Type *destType)
	{
		return V(::builder->CreateFPToSI(v, destType));
	}

	Value *Nucleus::createUIToFP(Value *v, Type *destType)
	{
		return V(::builder->CreateUIToFP(v, destType));
	}

	Value *Nucleus::createSIToFP(Value *v, Type *destType)
	{
		return V(::builder->CreateSIToFP(v, destType));
	}

	Value *Nucleus::createFPTrunc(Value *v, Type *destType)
	{
		return V(::builder->CreateFPTrunc(v, destType));
	}

	Value *Nucleus::createFPExt(Value *v, Type *destType)
	{
		return V(::builder->CreateFPExt(v, destType));
	}

	Value *Nucleus::createBitCast(Value *v, Type *destType)
	{
		return V(::builder->CreateBitCast(v, destType));
	}

	Value *Nucleus::createIntCast(Value *v, Type *destType, bool isSigned)
	{
		return V(::builder->CreateIntCast(v, destType, isSigned));
	}

	Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpEQ(lhs, rhs));
	}

	Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpNE(lhs, rhs));
	}

	Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpUGT(lhs, rhs));
	}

	Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpUGE(lhs, rhs));
	}

	Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpULT(lhs, rhs));
	}

	Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpULE(lhs, rhs));
	}

	Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSGT(lhs, rhs));
	}

	Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSGE(lhs, rhs));
	}

	Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSLT(lhs, rhs));
	}

	Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSLE(lhs, rhs));
	}

	Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOEQ(lhs, rhs));
	}

	Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOGT(lhs, rhs));
	}

	Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOGE(lhs, rhs));
	}

	Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOLT(lhs, rhs));
	}

	Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOLE(lhs, rhs));
	}

	Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpONE(lhs, rhs));
	}

	Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpORD(lhs, rhs));
	}

	Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUNO(lhs, rhs));
	}

	Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUEQ(lhs, rhs));
	}

	Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUGT(lhs, rhs));
	}

	Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUGE(lhs, rhs));
	}

	Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpULT(lhs, rhs));
	}

	Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpULE(lhs, rhs));
	}

	Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpULE(lhs, rhs));
	}

	Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
	{
		assert(vector->getType()->getContainedType(0) == type);
		return V(::builder->CreateExtractElement(vector, createConstantInt(index)));
	}

	Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
	{
		return V(::builder->CreateInsertElement(vector, element, createConstantInt(index)));
	}

	Value *Nucleus::createShuffleVector(Value *V1, Value *V2, const int *select)
	{
		int size = llvm::cast<llvm::VectorType>(V1->getType())->getNumElements();
		const int maxSize = 16;
		llvm::Constant *swizzle[maxSize];
		assert(size <= maxSize);

		for(int i = 0; i < size; i++)
		{
			swizzle[i] = Nucleus::createConstantInt(select[i]);
		}

		llvm::Value *shuffle = llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(swizzle, size));

		return V(::builder->CreateShuffleVector(V1, V2, shuffle));
	}

	Value *Nucleus::createSelect(Value *C, Value *ifTrue, Value *ifFalse)
	{
		return V(::builder->CreateSelect(C, ifTrue, ifFalse));
	}

	Value *Nucleus::createSwitch(Value *v, BasicBlock *Dest, unsigned NumCases)
	{
		return V(::builder->CreateSwitch(v, Dest, NumCases));
	}

	void Nucleus::addSwitchCase(Value *Switch, int Case, BasicBlock *Branch)
	{
		reinterpret_cast<SwitchInst*>(Switch)->addCase(llvm::ConstantInt::get(Type::getInt32Ty(*::context), Case, true), Branch);
	}

	void Nucleus::createUnreachable()
	{
		::builder->CreateUnreachable();
	}

	static Value *createSwizzle4(Value *val, unsigned char select)
	{
		int swizzle[4] =
		{
			(select >> 0) & 0x03,
			(select >> 2) & 0x03,
			(select >> 4) & 0x03,
			(select >> 6) & 0x03,
		};

		return Nucleus::createShuffleVector(val, val, swizzle);
	}

	static Value *createMask4(Value *lhs, Value *rhs, unsigned char select)
	{
		bool mask[4] = {false, false, false, false};

		mask[(select >> 0) & 0x03] = true;
		mask[(select >> 2) & 0x03] = true;
		mask[(select >> 4) & 0x03] = true;
		mask[(select >> 6) & 0x03] = true;

		int swizzle[4] =
		{
			mask[0] ? 4 : 0,
			mask[1] ? 5 : 1,
			mask[2] ? 6 : 2,
			mask[3] ? 7 : 3,
		};

		Value *shuffle = Nucleus::createShuffleVector(lhs, rhs, swizzle);

		return shuffle;
	}

	Constant *Nucleus::createConstantPointer(const void *address, Type *Ty, bool isConstant, unsigned int Align)
	{
		const GlobalValue *existingGlobal = ::executionEngine->getGlobalValueAtAddress(const_cast<void*>(address));   // FIXME: Const

		if(existingGlobal)
		{
			return (Constant*)existingGlobal;
		}

		llvm::GlobalValue *global = new llvm::GlobalVariable(*::module, Ty, isConstant, llvm::GlobalValue::ExternalLinkage, 0, "");

		global->setAlignment(Align);

		::executionEngine->addGlobalMapping(global, const_cast<void*>(address));

		return C(global);
	}

	Type *Nucleus::getPointerType(Type *ElementType)
	{
		return T(llvm::PointerType::get(ElementType, 0));
	}

	Constant *Nucleus::createNullValue(Type *Ty)
	{
		return C(llvm::Constant::getNullValue(Ty));
	}

	Constant *Nucleus::createConstantInt(int64_t i)
	{
		return C(llvm::ConstantInt::get(Type::getInt64Ty(*::context), i, true));
	}

	Constant *Nucleus::createConstantInt(int i)
	{
		return C(llvm::ConstantInt::get(Type::getInt32Ty(*::context), i, true));
	}

	Constant *Nucleus::createConstantInt(unsigned int i)
	{
		return C(llvm::ConstantInt::get(Type::getInt32Ty(*::context), i, false));
	}

	Constant *Nucleus::createConstantBool(bool b)
	{
		return C(llvm::ConstantInt::get(Type::getInt1Ty(*::context), b));
	}

	Constant *Nucleus::createConstantByte(signed char i)
	{
		return C(llvm::ConstantInt::get(Type::getInt8Ty(*::context), i, true));
	}

	Constant *Nucleus::createConstantByte(unsigned char i)
	{
		return C(llvm::ConstantInt::get(Type::getInt8Ty(*::context), i, false));
	}

	Constant *Nucleus::createConstantShort(short i)
	{
		return C(llvm::ConstantInt::get(Type::getInt16Ty(*::context), i, true));
	}

	Constant *Nucleus::createConstantShort(unsigned short i)
	{
		return C(llvm::ConstantInt::get(Type::getInt16Ty(*::context), i, false));
	}

	Constant *Nucleus::createConstantFloat(float x)
	{
		return C(ConstantFP::get(Float::getType(), x));
	}

	Constant *Nucleus::createNullPointer(Type *Ty)
	{
		return C(llvm::ConstantPointerNull::get(llvm::PointerType::get(Ty, 0)));
	}

	Constant *Nucleus::createConstantVector(Constant *const *Vals, unsigned NumVals)
	{
		return C(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(reinterpret_cast<llvm::Constant *const*>(Vals), NumVals)));
	}

	Type *Void::getType()
	{
		return T(llvm::Type::getVoidTy(*::context));
	}

	class MMX : public Variable<MMX>
	{
	public:
		static Type *getType();
	};

	Type *MMX::getType()
	{
		return T(llvm::Type::getX86_MMXTy(*::context));
	}

	Bool::Bool(Argument<Bool> argument)
	{
		storeValue(argument.value);
	}

	Bool::Bool()
	{
	}

	Bool::Bool(bool x)
	{
		storeValue(Nucleus::createConstantBool(x));
	}

	Bool::Bool(RValue<Bool> rhs)
	{
		storeValue(rhs.value);
	}

	Bool::Bool(const Bool &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Bool::Bool(const Reference<Bool> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Bool> Bool::operator=(RValue<Bool> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Bool> Bool::operator=(const Bool &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Bool>(value);
	}

	RValue<Bool> Bool::operator=(const Reference<Bool> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Bool>(value);
	}

	RValue<Bool> operator!(RValue<Bool> val)
	{
		return RValue<Bool>(Nucleus::createNot(val.value));
	}

	RValue<Bool> operator&&(RValue<Bool> lhs, RValue<Bool> rhs)
	{
		return RValue<Bool>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Bool> operator||(RValue<Bool> lhs, RValue<Bool> rhs)
	{
		return RValue<Bool>(Nucleus::createOr(lhs.value, rhs.value));
	}

	Type *Bool::getType()
	{
		return T(llvm::Type::getInt1Ty(*::context));
	}

	Byte::Byte(Argument<Byte> argument)
	{
		storeValue(argument.value);
	}

	Byte::Byte(RValue<Int> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, Byte::getType());

		storeValue(integer);
	}

	Byte::Byte(RValue<UInt> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, Byte::getType());

		storeValue(integer);
	}

	Byte::Byte(RValue<UShort> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, Byte::getType());

		storeValue(integer);
	}

	Byte::Byte()
	{
	}

	Byte::Byte(int x)
	{
		storeValue(Nucleus::createConstantByte((unsigned char)x));
	}

	Byte::Byte(unsigned char x)
	{
		storeValue(Nucleus::createConstantByte(x));
	}

	Byte::Byte(RValue<Byte> rhs)
	{
		storeValue(rhs.value);
	}

	Byte::Byte(const Byte &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Byte::Byte(const Reference<Byte> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Byte> Byte::operator=(RValue<Byte> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Byte> Byte::operator=(const Byte &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte>(value);
	}

	RValue<Byte> Byte::operator=(const Reference<Byte> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte>(value);
	}

	RValue<Byte> operator+(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Byte> operator-(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Byte> operator*(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Byte> operator/(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<Byte> operator%(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<Byte> operator&(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Byte> operator|(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Byte> operator^(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Byte> operator<<(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Byte> operator>>(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Byte>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<Byte> operator+=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Byte> operator-=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Byte> operator*=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Byte> operator/=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Byte> operator%=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Byte> operator&=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Byte> operator|=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Byte> operator^=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Byte> operator<<=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Byte> operator>>=(const Byte &lhs, RValue<Byte> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Byte> operator+(RValue<Byte> val)
	{
		return val;
	}

	RValue<Byte> operator-(RValue<Byte> val)
	{
		return RValue<Byte>(Nucleus::createNeg(val.value));
	}

	RValue<Byte> operator~(RValue<Byte> val)
	{
		return RValue<Byte>(Nucleus::createNot(val.value));
	}

	RValue<Byte> operator++(const Byte &val, int)   // Post-increment
	{
		RValue<Byte> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantByte((unsigned char)1)));
		val.storeValue(inc);

		return res;
	}

	const Byte &operator++(const Byte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantByte((unsigned char)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Byte> operator--(const Byte &val, int)   // Post-decrement
	{
		RValue<Byte> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantByte((unsigned char)1)));
		val.storeValue(inc);

		return res;
	}

	const Byte &operator--(const Byte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantByte((unsigned char)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Bool> operator<(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<Byte> lhs, RValue<Byte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Type *Byte::getType()
	{
		return T(llvm::Type::getInt8Ty(*::context));
	}

	SByte::SByte(Argument<SByte> argument)
	{
		storeValue(argument.value);
	}

	SByte::SByte(RValue<Int> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, SByte::getType());

		storeValue(integer);
	}

	SByte::SByte(RValue<Short> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, SByte::getType());

		storeValue(integer);
	}

	SByte::SByte()
	{
	}

	SByte::SByte(signed char x)
	{
		storeValue(Nucleus::createConstantByte(x));
	}

	SByte::SByte(RValue<SByte> rhs)
	{
		storeValue(rhs.value);
	}

	SByte::SByte(const SByte &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	SByte::SByte(const Reference<SByte> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<SByte> SByte::operator=(RValue<SByte> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<SByte> SByte::operator=(const SByte &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<SByte>(value);
	}

	RValue<SByte> SByte::operator=(const Reference<SByte> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<SByte>(value);
	}

	RValue<SByte> operator+(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<SByte> operator-(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<SByte> operator*(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<SByte> operator/(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<SByte> operator%(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<SByte> operator&(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<SByte> operator|(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<SByte> operator^(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<SByte> operator<<(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<SByte> operator>>(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<SByte>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<SByte> operator+=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<SByte> operator-=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<SByte> operator*=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<SByte> operator/=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<SByte> operator%=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<SByte> operator&=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<SByte> operator|=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<SByte> operator^=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<SByte> operator<<=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<SByte> operator>>=(const SByte &lhs, RValue<SByte> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<SByte> operator+(RValue<SByte> val)
	{
		return val;
	}

	RValue<SByte> operator-(RValue<SByte> val)
	{
		return RValue<SByte>(Nucleus::createNeg(val.value));
	}

	RValue<SByte> operator~(RValue<SByte> val)
	{
		return RValue<SByte>(Nucleus::createNot(val.value));
	}

	RValue<SByte> operator++(const SByte &val, int)   // Post-increment
	{
		RValue<SByte> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantByte((signed char)1)));
		val.storeValue(inc);

		return res;
	}

	const SByte &operator++(const SByte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantByte((signed char)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<SByte> operator--(const SByte &val, int)   // Post-decrement
	{
		RValue<SByte> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantByte((signed char)1)));
		val.storeValue(inc);

		return res;
	}

	const SByte &operator--(const SByte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantByte((signed char)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Bool> operator<(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<SByte> lhs, RValue<SByte> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Type *SByte::getType()
	{
		return T(llvm::Type::getInt8Ty(*::context));
	}

	Short::Short(Argument<Short> argument)
	{
		storeValue(argument.value);
	}

	Short::Short(RValue<Int> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, Short::getType());

		storeValue(integer);
	}

	Short::Short()
	{
	}

	Short::Short(short x)
	{
		storeValue(Nucleus::createConstantShort(x));
	}

	Short::Short(RValue<Short> rhs)
	{
		storeValue(rhs.value);
	}

	Short::Short(const Short &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Short::Short(const Reference<Short> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Short> Short::operator=(RValue<Short> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Short> Short::operator=(const Short &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short>(value);
	}

	RValue<Short> Short::operator=(const Reference<Short> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short>(value);
	}

	RValue<Short> operator+(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Short> operator-(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Short> operator*(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Short> operator/(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Short> operator%(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Short> operator&(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Short> operator|(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Short> operator^(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Short> operator<<(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Short> operator>>(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Short>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<Short> operator+=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Short> operator-=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Short> operator*=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Short> operator/=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Short> operator%=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Short> operator&=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Short> operator|=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Short> operator^=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Short> operator<<=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Short> operator>>=(const Short &lhs, RValue<Short> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Short> operator+(RValue<Short> val)
	{
		return val;
	}

	RValue<Short> operator-(RValue<Short> val)
	{
		return RValue<Short>(Nucleus::createNeg(val.value));
	}

	RValue<Short> operator~(RValue<Short> val)
	{
		return RValue<Short>(Nucleus::createNot(val.value));
	}

	RValue<Short> operator++(const Short &val, int)   // Post-increment
	{
		RValue<Short> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantShort((short)1)));
		val.storeValue(inc);

		return res;
	}

	const Short &operator++(const Short &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantShort((short)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Short> operator--(const Short &val, int)   // Post-decrement
	{
		RValue<Short> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantShort((short)1)));
		val.storeValue(inc);

		return res;
	}

	const Short &operator--(const Short &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantShort((short)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Bool> operator<(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<Short> lhs, RValue<Short> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Type *Short::getType()
	{
		return T(llvm::Type::getInt16Ty(*::context));
	}

	UShort::UShort(Argument<UShort> argument)
	{
		storeValue(argument.value);
	}

	UShort::UShort(RValue<UInt> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, UShort::getType());

		storeValue(integer);
	}

	UShort::UShort(RValue<Int> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, UShort::getType());

		storeValue(integer);
	}

	UShort::UShort()
	{
	}

	UShort::UShort(unsigned short x)
	{
		storeValue(Nucleus::createConstantShort(x));
	}

	UShort::UShort(RValue<UShort> rhs)
	{
		storeValue(rhs.value);
	}

	UShort::UShort(const UShort &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UShort::UShort(const Reference<UShort> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<UShort> UShort::operator=(RValue<UShort> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UShort> UShort::operator=(const UShort &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort>(value);
	}

	RValue<UShort> UShort::operator=(const Reference<UShort> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort>(value);
	}

	RValue<UShort> operator+(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UShort> operator-(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UShort> operator*(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UShort> operator/(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UShort> operator%(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UShort> operator&(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UShort> operator|(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UShort> operator^(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UShort> operator<<(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<UShort> operator>>(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<UShort>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<UShort> operator+=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UShort> operator-=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UShort> operator*=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UShort> operator/=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UShort> operator%=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UShort> operator&=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UShort> operator|=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UShort> operator^=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UShort> operator<<=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UShort> operator>>=(const UShort &lhs, RValue<UShort> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UShort> operator+(RValue<UShort> val)
	{
		return val;
	}

	RValue<UShort> operator-(RValue<UShort> val)
	{
		return RValue<UShort>(Nucleus::createNeg(val.value));
	}

	RValue<UShort> operator~(RValue<UShort> val)
	{
		return RValue<UShort>(Nucleus::createNot(val.value));
	}

	RValue<UShort> operator++(const UShort &val, int)   // Post-increment
	{
		RValue<UShort> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantShort((unsigned short)1)));
		val.storeValue(inc);

		return res;
	}

	const UShort &operator++(const UShort &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantShort((unsigned short)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<UShort> operator--(const UShort &val, int)   // Post-decrement
	{
		RValue<UShort> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantShort((unsigned short)1)));
		val.storeValue(inc);

		return res;
	}

	const UShort &operator--(const UShort &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantShort((unsigned short)1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Bool> operator<(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<UShort> lhs, RValue<UShort> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Type *UShort::getType()
	{
		return T(llvm::Type::getInt16Ty(*::context));
	}

	Type *Byte4::getType()
	{
		#if 0
			return T(VectorType::get(Byte::getType(), 4));
		#else
			return UInt::getType();   // FIXME: LLVM doesn't manipulate it as one 32-bit block
		#endif
	}

	Type *SByte4::getType()
	{
		#if 0
			return T(VectorType::get(SByte::getType(), 4));
		#else
			return Int::getType();   // FIXME: LLVM doesn't manipulate it as one 32-bit block
		#endif
	}

	Byte8::Byte8()
	{
	//	xyzw.parent = this;
	}

	Byte8::Byte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte(x0);
		constantVector[1] = Nucleus::createConstantByte(x1);
		constantVector[2] = Nucleus::createConstantByte(x2);
		constantVector[3] = Nucleus::createConstantByte(x3);
		constantVector[4] = Nucleus::createConstantByte(x4);
		constantVector[5] = Nucleus::createConstantByte(x5);
		constantVector[6] = Nucleus::createConstantByte(x6);
		constantVector[7] = Nucleus::createConstantByte(x7);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 8));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	Byte8::Byte8(int64_t x)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte((unsigned char)(x >>  0));
		constantVector[1] = Nucleus::createConstantByte((unsigned char)(x >>  8));
		constantVector[2] = Nucleus::createConstantByte((unsigned char)(x >> 16));
		constantVector[3] = Nucleus::createConstantByte((unsigned char)(x >> 24));
		constantVector[4] = Nucleus::createConstantByte((unsigned char)(x >> 32));
		constantVector[5] = Nucleus::createConstantByte((unsigned char)(x >> 40));
		constantVector[6] = Nucleus::createConstantByte((unsigned char)(x >> 48));
		constantVector[7] = Nucleus::createConstantByte((unsigned char)(x >> 56));
		Value *vector = V(Nucleus::createConstantVector(constantVector, 8));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	Byte8::Byte8(RValue<Byte8> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Byte8::Byte8(const Byte8 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Byte8::Byte8(const Reference<Byte8> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Byte8> Byte8::operator=(RValue<Byte8> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Byte8> Byte8::operator=(const Byte8 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte8>(value);
	}

	RValue<Byte8> Byte8::operator=(const Reference<Byte8> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte8>(value);
	}

	RValue<Byte8> operator+(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::paddb(lhs, rhs);
		}
		else
		{
			return RValue<Byte8>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<Byte8> operator-(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::psubb(lhs, rhs);
		}
		else
		{
			return RValue<Byte8>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

//	RValue<Byte8> operator*(RValue<Byte8> lhs, RValue<Byte8> rhs)
//	{
//		return RValue<Byte8>(Nucleus::createMul(lhs.value, rhs.value));
//	}

//	RValue<Byte8> operator/(RValue<Byte8> lhs, RValue<Byte8> rhs)
//	{
//		return RValue<Byte8>(Nucleus::createUDiv(lhs.value, rhs.value));
//	}

//	RValue<Byte8> operator%(RValue<Byte8> lhs, RValue<Byte8> rhs)
//	{
//		return RValue<Byte8>(Nucleus::createURem(lhs.value, rhs.value));
//	}

	RValue<Byte8> operator&(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Byte8>(x86::pand(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Byte8>(Nucleus::createAnd(lhs.value, rhs.value));
		}
	}

	RValue<Byte8> operator|(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Byte8>(x86::por(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Byte8>(Nucleus::createOr(lhs.value, rhs.value));
		}
	}

	RValue<Byte8> operator^(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Byte8>(x86::pxor(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Byte8>(Nucleus::createXor(lhs.value, rhs.value));
		}
	}

//	RValue<Byte8> operator<<(RValue<Byte8> lhs, unsigned char rhs)
//	{
//		return RValue<Byte8>(Nucleus::createShl(lhs.value, rhs.value));
//	}

//	RValue<Byte8> operator>>(RValue<Byte8> lhs, unsigned char rhs)
//	{
//		return RValue<Byte8>(Nucleus::createLShr(lhs.value, rhs.value));
//	}

	RValue<Byte8> operator+=(const Byte8 &lhs, RValue<Byte8> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Byte8> operator-=(const Byte8 &lhs, RValue<Byte8> rhs)
	{
		return lhs = lhs - rhs;
	}

//	RValue<Byte8> operator*=(const Byte8 &lhs, RValue<Byte8> rhs)
//	{
//		return lhs = lhs * rhs;
//	}

//	RValue<Byte8> operator/=(const Byte8 &lhs, RValue<Byte8> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<Byte8> operator%=(const Byte8 &lhs, RValue<Byte8> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<Byte8> operator&=(const Byte8 &lhs, RValue<Byte8> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Byte8> operator|=(const Byte8 &lhs, RValue<Byte8> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Byte8> operator^=(const Byte8 &lhs, RValue<Byte8> rhs)
	{
		return lhs = lhs ^ rhs;
	}

//	RValue<Byte8> operator<<=(const Byte8 &lhs, RValue<Byte8> rhs)
//	{
//		return lhs = lhs << rhs;
//	}

//	RValue<Byte8> operator>>=(const Byte8 &lhs, RValue<Byte8> rhs)
//	{
//		return lhs = lhs >> rhs;
//	}

//	RValue<Byte8> operator+(RValue<Byte8> val)
//	{
//		return val;
//	}

//	RValue<Byte8> operator-(RValue<Byte8> val)
//	{
//		return RValue<Byte8>(Nucleus::createNeg(val.value));
//	}

	RValue<Byte8> operator~(RValue<Byte8> val)
	{
		if(CPUID::supportsMMX2())
		{
			return val ^ Byte8(0xFFFFFFFFFFFFFFFF);
		}
		else
		{
			return RValue<Byte8>(Nucleus::createNot(val.value));
		}
	}

	RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y)
	{
		return x86::paddusb(x, y);
	}

	RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
	{
		return x86::psubusb(x, y);
	}

	RValue<Short4> Unpack(RValue<Byte4> x)
	{
		Value *int2 = Nucleus::createInsertElement(V(UndefValue::get(VectorType::get(Int::getType(), 2))), x.value, 0);
		Value *byte8 = Nucleus::createBitCast(int2, Byte8::getType());

		return UnpackLow(RValue<Byte8>(byte8), RValue<Byte8>(byte8));
	}

	RValue<Short4> UnpackLow(RValue<Byte8> x, RValue<Byte8> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpcklbw(x, y);
		}
		else
		{
			int shuffle[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Short4>(Nucleus::createBitCast(packed, Short4::getType()));
		}
	}

	RValue<Short4> UnpackHigh(RValue<Byte8> x, RValue<Byte8> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpckhbw(x, y);
		}
		else
		{
			int shuffle[8] = {4, 12, 5, 13, 6, 14, 7, 15};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Short4>(Nucleus::createBitCast(packed, Short4::getType()));
		}
	}

	RValue<Int> SignMask(RValue<Byte8> x)
	{
		return x86::pmovmskb(x);
	}

//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y)
//	{
//		return x86::pcmpgtb(x, y);   // FIXME: Signedness
//	}

	RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y)
	{
		return x86::pcmpeqb(x, y);
	}

	Type *Byte8::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(Byte::getType(), 8));
		}
	}

	SByte8::SByte8()
	{
	//	xyzw.parent = this;
	}

	SByte8::SByte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte(x0);
		constantVector[1] = Nucleus::createConstantByte(x1);
		constantVector[2] = Nucleus::createConstantByte(x2);
		constantVector[3] = Nucleus::createConstantByte(x3);
		constantVector[4] = Nucleus::createConstantByte(x4);
		constantVector[5] = Nucleus::createConstantByte(x5);
		constantVector[6] = Nucleus::createConstantByte(x6);
		constantVector[7] = Nucleus::createConstantByte(x7);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 8));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	SByte8::SByte8(int64_t x)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte((unsigned char)(x >>  0));
		constantVector[1] = Nucleus::createConstantByte((unsigned char)(x >>  8));
		constantVector[2] = Nucleus::createConstantByte((unsigned char)(x >> 16));
		constantVector[3] = Nucleus::createConstantByte((unsigned char)(x >> 24));
		constantVector[4] = Nucleus::createConstantByte((unsigned char)(x >> 32));
		constantVector[5] = Nucleus::createConstantByte((unsigned char)(x >> 40));
		constantVector[6] = Nucleus::createConstantByte((unsigned char)(x >> 48));
		constantVector[7] = Nucleus::createConstantByte((unsigned char)(x >> 56));
		Value *vector = V(Nucleus::createConstantVector(constantVector, 8));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	SByte8::SByte8(RValue<SByte8> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	SByte8::SByte8(const SByte8 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	SByte8::SByte8(const Reference<SByte8> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<SByte8> SByte8::operator=(RValue<SByte8> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<SByte8> SByte8::operator=(const SByte8 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<SByte8>(value);
	}

	RValue<SByte8> SByte8::operator=(const Reference<SByte8> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<SByte8>(value);
	}

	RValue<SByte8> operator+(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<SByte8>(x86::paddb(As<Byte8>(lhs), As<Byte8>(rhs)));
		}
		else
		{
			return RValue<SByte8>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<SByte8> operator-(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<SByte8>(x86::psubb(As<Byte8>(lhs), As<Byte8>(rhs)));
		}
		else
		{
			return RValue<SByte8>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

//	RValue<SByte8> operator*(RValue<SByte8> lhs, RValue<SByte8> rhs)
//	{
//		return RValue<SByte8>(Nucleus::createMul(lhs.value, rhs.value));
//	}

//	RValue<SByte8> operator/(RValue<SByte8> lhs, RValue<SByte8> rhs)
//	{
//		return RValue<SByte8>(Nucleus::createSDiv(lhs.value, rhs.value));
//	}

//	RValue<SByte8> operator%(RValue<SByte8> lhs, RValue<SByte8> rhs)
//	{
//		return RValue<SByte8>(Nucleus::createSRem(lhs.value, rhs.value));
//	}

	RValue<SByte8> operator&(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		return RValue<SByte8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<SByte8> operator|(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		return RValue<SByte8>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<SByte8> operator^(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		return RValue<SByte8>(Nucleus::createXor(lhs.value, rhs.value));
	}

//	RValue<SByte8> operator<<(RValue<SByte8> lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createShl(lhs.value, rhs.value));
//	}

//	RValue<SByte8> operator>>(RValue<SByte8> lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createAShr(lhs.value, rhs.value));
//	}

	RValue<SByte8> operator+=(const SByte8 &lhs, RValue<SByte8> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<SByte8> operator-=(const SByte8 &lhs, RValue<SByte8> rhs)
	{
		return lhs = lhs - rhs;
	}

//	RValue<SByte8> operator*=(const SByte8 &lhs, RValue<SByte8> rhs)
//	{
//		return lhs = lhs * rhs;
//	}

//	RValue<SByte8> operator/=(const SByte8 &lhs, RValue<SByte8> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<SByte8> operator%=(const SByte8 &lhs, RValue<SByte8> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<SByte8> operator&=(const SByte8 &lhs, RValue<SByte8> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<SByte8> operator|=(const SByte8 &lhs, RValue<SByte8> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<SByte8> operator^=(const SByte8 &lhs, RValue<SByte8> rhs)
	{
		return lhs = lhs ^ rhs;
	}

//	RValue<SByte8> operator<<=(const SByte8 &lhs, RValue<SByte8> rhs)
//	{
//		return lhs = lhs << rhs;
//	}

//	RValue<SByte8> operator>>=(const SByte8 &lhs, RValue<SByte8> rhs)
//	{
//		return lhs = lhs >> rhs;
//	}

//	RValue<SByte8> operator+(RValue<SByte8> val)
//	{
//		return val;
//	}

//	RValue<SByte8> operator-(RValue<SByte8> val)
//	{
//		return RValue<SByte8>(Nucleus::createNeg(val.value));
//	}

	RValue<SByte8> operator~(RValue<SByte8> val)
	{
		if(CPUID::supportsMMX2())
		{
			return val ^ SByte8(0xFFFFFFFFFFFFFFFF);
		}
		else
		{
			return RValue<SByte8>(Nucleus::createNot(val.value));
		}
	}

	RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y)
	{
		return x86::paddsb(x, y);
	}

	RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
	{
		return x86::psubsb(x, y);
	}

	RValue<Short4> UnpackLow(RValue<SByte8> x, RValue<SByte8> y)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Short4>(x86::punpcklbw(As<Byte8>(x), As<Byte8>(y)));
		}
		else
		{
			int shuffle[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Short4>(Nucleus::createBitCast(packed, Short4::getType()));
		}
	}

	RValue<Short4> UnpackHigh(RValue<SByte8> x, RValue<SByte8> y)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Short4>(x86::punpckhbw(As<Byte8>(x), As<Byte8>(y)));
		}
		else
		{
			int shuffle[8] = {4, 12, 5, 13, 6, 14, 7, 15};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Short4>(Nucleus::createBitCast(packed, Short4::getType()));
		}
	}

	RValue<Int> SignMask(RValue<SByte8> x)
	{
		return x86::pmovmskb(As<Byte8>(x));
	}

	RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
	{
		return x86::pcmpgtb(x, y);
	}

	RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
	{
		return x86::pcmpeqb(As<Byte8>(x), As<Byte8>(y));
	}

	Type *SByte8::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(SByte::getType(), 8));
		}
	}

	Byte16::Byte16(RValue<Byte16> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Byte16::Byte16(const Byte16 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Byte16::Byte16(const Reference<Byte16> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Byte16> Byte16::operator=(RValue<Byte16> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Byte16> Byte16::operator=(const Byte16 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte16>(value);
	}

	RValue<Byte16> Byte16::operator=(const Reference<Byte16> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Byte16>(value);
	}

	Type *Byte16::getType()
	{
		return T(VectorType::get(Byte::getType(), 16));
	}

	Type *SByte16::getType()
	{
		return T( VectorType::get(SByte::getType(), 16));
	}

	Short4::Short4(RValue<Int> cast)
	{
		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
		Value *swizzle = Swizzle(RValue<Short4>(extend), 0x00).value;

		storeValue(swizzle);
	}

	Short4::Short4(RValue<Int4> cast)
	{
		Value *short8 = Nucleus::createBitCast(cast.value, Short8::getType());

		#if 0   // FIXME: Check codegen (pshuflw phshufhw pshufd)
			Constant *pack[8];
			pack[0] = Nucleus::createConstantInt(0);
			pack[1] = Nucleus::createConstantInt(2);
			pack[2] = Nucleus::createConstantInt(4);
			pack[3] = Nucleus::createConstantInt(6);

			Value *short4 = Nucleus::createShuffleVector(short8, short8, Nucleus::createConstantVector(pack, 4));
		#else
			Value *packed;

			// FIXME: Use Swizzle<Short8>
			if(!CPUID::supportsSSSE3())
			{
				int pshuflw[8] = {0, 2, 0, 2, 4, 5, 6, 7};
				int pshufhw[8] = {0, 1, 2, 3, 4, 6, 4, 6};

				Value *shuffle1 = Nucleus::createShuffleVector(short8, short8, pshuflw);
				Value *shuffle2 = Nucleus::createShuffleVector(shuffle1, shuffle1, pshufhw);
				Value *int4 = Nucleus::createBitCast(shuffle2, Int4::getType());
				packed = createSwizzle4(int4, 0x88);
			}
			else
			{
				int pshufb[16] = {0, 1, 4, 5, 8, 9, 12, 13, 0, 1, 4, 5, 8, 9, 12, 13};
				Value *byte16 = Nucleus::createBitCast(cast.value, Byte16::getType());
				packed = Nucleus::createShuffleVector(byte16, byte16, pshufb);
			}

			#if 0   // FIXME: No optimal instruction selection
				Value *qword2 = Nucleus::createBitCast(packed, Long2::getType());
				Value *element = Nucleus::createExtractElement(qword2, 0);
				Value *short4 = Nucleus::createBitCast(element, Short4::getType());
			#else   // FIXME: Requires SSE
				Value *int2 = RValue<Int2>(Int2(RValue<Int4>(packed))).value;
				Value *short4 = Nucleus::createBitCast(int2, Short4::getType());
			#endif
		#endif

		storeValue(short4);
	}

//	Short4::Short4(RValue<Float> cast)
//	{
//	}

	Short4::Short4(RValue<Float4> cast)
	{
		Int4 v4i32 = Int4(cast);
		v4i32 = As<Int4>(x86::packssdw(v4i32, v4i32));

		storeValue(As<Short4>(Int2(v4i32)).value);
	}

	Short4::Short4()
	{
	//	xyzw.parent = this;
	}

	Short4::Short4(short xyzw)
	{
		//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(xyzw);
		constantVector[1] = Nucleus::createConstantShort(xyzw);
		constantVector[2] = Nucleus::createConstantShort(xyzw);
		constantVector[3] = Nucleus::createConstantShort(xyzw);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 4));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	Short4::Short4(short x, short y, short z, short w)
	{
	//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(x);
		constantVector[1] = Nucleus::createConstantShort(y);
		constantVector[2] = Nucleus::createConstantShort(z);
		constantVector[3] = Nucleus::createConstantShort(w);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 4));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	Short4::Short4(RValue<Short4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Short4::Short4(const Short4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Short4::Short4(const Reference<Short4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Short4::Short4(RValue<UShort4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Short4::Short4(const UShort4 &rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.loadValue());
	}

	Short4::Short4(const Reference<UShort4> &rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.loadValue());
	}

	RValue<Short4> Short4::operator=(RValue<Short4> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Short4> Short4::operator=(const Short4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short4>(value);
	}

	RValue<Short4> Short4::operator=(const Reference<Short4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short4>(value);
	}

	RValue<Short4> Short4::operator=(RValue<UShort4> rhs) const
	{
		storeValue(rhs.value);

		return RValue<Short4>(rhs);
	}

	RValue<Short4> Short4::operator=(const UShort4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short4>(value);
	}

	RValue<Short4> Short4::operator=(const Reference<UShort4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Short4>(value);
	}

	RValue<Short4> operator+(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::paddw(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<Short4> operator-(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::psubw(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

	RValue<Short4> operator*(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::pmullw(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createMul(lhs.value, rhs.value));
		}
	}

//	RValue<Short4> operator/(RValue<Short4> lhs, RValue<Short4> rhs)
//	{
//		return RValue<Short4>(Nucleus::createSDiv(lhs.value, rhs.value));
//	}

//	RValue<Short4> operator%(RValue<Short4> lhs, RValue<Short4> rhs)
//	{
//		return RValue<Short4>(Nucleus::createSRem(lhs.value, rhs.value));
//	}

	RValue<Short4> operator&(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::pand(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createAnd(lhs.value, rhs.value));
		}
	}

	RValue<Short4> operator|(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::por(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createOr(lhs.value, rhs.value));
		}
	}

	RValue<Short4> operator^(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::pxor(lhs, rhs);
		}
		else
		{
			return RValue<Short4>(Nucleus::createXor(lhs.value, rhs.value));
		}
	}

	RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psraw(lhs, rhs);
	}

	RValue<Short4> operator<<(RValue<Short4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psraw(lhs, rhs);
	}

	RValue<Short4> operator+=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Short4> operator-=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Short4> operator*=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs * rhs;
	}

//	RValue<Short4> operator/=(const Short4 &lhs, RValue<Short4> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<Short4> operator%=(const Short4 &lhs, RValue<Short4> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<Short4> operator&=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Short4> operator|=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Short4> operator^=(const Short4 &lhs, RValue<Short4> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Short4> operator<<=(const Short4 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Short4> operator>>=(const Short4 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Short4> operator<<=(const Short4 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Short4> operator>>=(const Short4 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs >> rhs;
	}

//	RValue<Short4> operator+(RValue<Short4> val)
//	{
//		return val;
//	}

	RValue<Short4> operator-(RValue<Short4> val)
	{
		if(CPUID::supportsMMX2())
		{
			return Short4(0, 0, 0, 0) - val;
		}
		else
		{
			return RValue<Short4>(Nucleus::createNeg(val.value));
		}
	}

	RValue<Short4> operator~(RValue<Short4> val)
	{
		if(CPUID::supportsMMX2())
		{
			return val ^ Short4(0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu);
		}
		else
		{
			return RValue<Short4>(Nucleus::createNot(val.value));
		}
	}

	RValue<Short4> RoundShort4(RValue<Float4> cast)
	{
		RValue<Int4> v4i32 = x86::cvtps2dq(cast);
		RValue<Short8> v8i16 = x86::packssdw(v4i32, v4i32);

		return As<Short4>(Int2(As<Int4>(v8i16)));
	}

	RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pmaxsw(x, y);
	}

	RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pminsw(x, y);
	}

	RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::paddsw(x, y);
	}

	RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::psubsw(x, y);
	}

	RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pmulhw(x, y);
	}

	RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pmaddwd(x, y);
	}

	RValue<SByte8> Pack(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::packsswb(x, y);
	}

	RValue<Int2> UnpackLow(RValue<Short4> x, RValue<Short4> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpcklwd(x, y);
		}
		else
		{
			int shuffle[4] = {0, 4, 1, 5};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Int2>(Nucleus::createBitCast(packed, Int2::getType()));
		}
	}

	RValue<Int2> UnpackHigh(RValue<Short4> x, RValue<Short4> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpckhwd(x, y);
		}
		else
		{
			int shuffle[4] = {2, 6, 3, 7};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Int2>(Nucleus::createBitCast(packed, Int2::getType()));
		}
	}

	RValue<Short4> Swizzle(RValue<Short4> x, unsigned char select)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::pshufw(x, select);
		}
		else
		{
			return RValue<Short4>(createSwizzle4(x.value, select));
		}
	}

	RValue<Short4> Insert(RValue<Short4> val, RValue<Short> element, int i)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::pinsrw(val, Int(element), i);
		}
		else
		{
			return RValue<Short4>(Nucleus::createInsertElement(val.value, element.value, i));
		}
	}

	RValue<Short> Extract(RValue<Short4> val, int i)
	{
		if(CPUID::supportsMMX2())
		{
			return Short(x86::pextrw(val, i));
		}
		else
		{
			return RValue<Short>(Nucleus::createExtractElement(val.value, Short::getType(), i));
		}
	}

	RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pcmpgtw(x, y);
	}

	RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
	{
		return x86::pcmpeqw(x, y);
	}

	Type *Short4::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(Short::getType(), 4));
		}
	}

	UShort4::UShort4(RValue<Int4> cast)
	{
		*this = Short4(cast);
	}

	UShort4::UShort4(RValue<Float4> cast, bool saturate)
	{
		Float4 sat;

		if(saturate)
		{
			if(CPUID::supportsSSE4_1())
			{
				sat = Min(cast, Float4(0xFFFF));   // packusdw takes care of 0x0000 saturation
			}
			else
			{
				sat = Max(Min(cast, Float4(0xFFFF)), Float4(0x0000));
			}
		}
		else
		{
			sat = cast;
		}

		Int4 int4(sat);

		if(!saturate || !CPUID::supportsSSE4_1())
		{
			*this = Short4(Int4(int4));
		}
		else
		{
			*this = As<Short4>(Int2(As<Int4>(x86::packusdw(As<UInt4>(int4), As<UInt4>(int4)))));
		}
	}

	UShort4::UShort4()
	{
	//	xyzw.parent = this;
	}

	UShort4::UShort4(unsigned short xyzw)
	{
		//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(xyzw);
		constantVector[1] = Nucleus::createConstantShort(xyzw);
		constantVector[2] = Nucleus::createConstantShort(xyzw);
		constantVector[3] = Nucleus::createConstantShort(xyzw);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 4));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	UShort4::UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
	{
	//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(x);
		constantVector[1] = Nucleus::createConstantShort(y);
		constantVector[2] = Nucleus::createConstantShort(z);
		constantVector[3] = Nucleus::createConstantShort(w);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 4));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	UShort4::UShort4(RValue<UShort4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	UShort4::UShort4(const UShort4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UShort4::UShort4(const Reference<UShort4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UShort4::UShort4(RValue<Short4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	UShort4::UShort4(const Short4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UShort4::UShort4(const Reference<Short4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<UShort4> UShort4::operator=(RValue<UShort4> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UShort4> UShort4::operator=(const UShort4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> UShort4::operator=(const Reference<UShort4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> UShort4::operator=(RValue<Short4> rhs) const
	{
		storeValue(rhs.value);

		return RValue<UShort4>(rhs);
	}

	RValue<UShort4> UShort4::operator=(const Short4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> UShort4::operator=(const Reference<Short4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> operator+(RValue<UShort4> lhs, RValue<UShort4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UShort4>(x86::paddw(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UShort4>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<UShort4> operator-(RValue<UShort4> lhs, RValue<UShort4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UShort4>(x86::psubw(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UShort4>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

	RValue<UShort4> operator*(RValue<UShort4> lhs, RValue<UShort4> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UShort4>(x86::pmullw(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UShort4>(Nucleus::createMul(lhs.value, rhs.value));
		}
	}

	RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrlw(lhs, rhs);
	}

	RValue<UShort4> operator<<(RValue<UShort4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrlw(lhs, rhs);
	}

	RValue<UShort4> operator<<=(const UShort4 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UShort4> operator>>=(const UShort4 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UShort4> operator<<=(const UShort4 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UShort4> operator>>=(const UShort4 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UShort4> operator~(RValue<UShort4> val)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UShort4>(As<Short4>(val) ^ Short4(0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu));
		}
		else
		{
			return RValue<UShort4>(Nucleus::createNot(val.value));
		}
	}

	RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y)
	{
		return RValue<UShort4>(Max(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
	}

	RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y)
	{
		return RValue<UShort4>(Min(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
	}

	RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		return x86::paddusw(x, y);
	}

	RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		return x86::psubusw(x, y);
	}

	RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
	{
		return x86::pmulhuw(x, y);
	}

	RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
	{
		return x86::pavgw(x, y);
	}

	RValue<Byte8> Pack(RValue<UShort4> x, RValue<UShort4> y)
	{
		return x86::packuswb(x, y);
	}

	Type *UShort4::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(UShort::getType(), 4));
		}
	}

	Short8::Short8(short c0, short c1, short c2, short c3, short c4, short c5, short c6, short c7)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantShort(c0);
		constantVector[1] = Nucleus::createConstantShort(c1);
		constantVector[2] = Nucleus::createConstantShort(c2);
		constantVector[3] = Nucleus::createConstantShort(c3);
		constantVector[4] = Nucleus::createConstantShort(c4);
		constantVector[5] = Nucleus::createConstantShort(c5);
		constantVector[6] = Nucleus::createConstantShort(c6);
		constantVector[7] = Nucleus::createConstantShort(c7);

		storeValue(Nucleus::createConstantVector(constantVector, 8));
	}

	Short8::Short8(RValue<Short8> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Short8::Short8(const Reference<Short8> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Short8::Short8(RValue<Short4> lo, RValue<Short4> hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = V(UndefValue::get(Long2::getType()));
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *short8 = Nucleus::createBitCast(long2, Short8::getType());

		storeValue(short8);
	}

	RValue<Short8> operator+(RValue<Short8> lhs, RValue<Short8> rhs)
	{
		return RValue<Short8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Short8> operator&(RValue<Short8> lhs, RValue<Short8> rhs)
	{
		return RValue<Short8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Short8> operator<<(RValue<Short8> lhs, unsigned char rhs)
	{
		return x86::psllw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
	{
		return x86::psraw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
	{
		return x86::pmaddwd(x, y);   // FIXME: Fallback required
	}

	RValue<Int4> Abs(RValue<Int4> x)
	{
		if(CPUID::supportsSSSE3())
		{
			return x86::pabsd(x);
		}
		else
		{
			Int4 mask = (x >> 31);
			return (mask ^ x) - mask;
		}
	}

	RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
	{
		return x86::pmulhw(x, y);   // FIXME: Fallback required
	}

	Type *Short8::getType()
	{
		return T(VectorType::get(Short::getType(), 8));
	}

	UShort8::UShort8(unsigned short c0, unsigned short c1, unsigned short c2, unsigned short c3, unsigned short c4, unsigned short c5, unsigned short c6, unsigned short c7)
	{
	//	xyzw.parent = this;

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantShort(c0);
		constantVector[1] = Nucleus::createConstantShort(c1);
		constantVector[2] = Nucleus::createConstantShort(c2);
		constantVector[3] = Nucleus::createConstantShort(c3);
		constantVector[4] = Nucleus::createConstantShort(c4);
		constantVector[5] = Nucleus::createConstantShort(c5);
		constantVector[6] = Nucleus::createConstantShort(c6);
		constantVector[7] = Nucleus::createConstantShort(c7);

		storeValue(Nucleus::createConstantVector(constantVector, 8));
	}

	UShort8::UShort8(RValue<UShort8> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	UShort8::UShort8(const Reference<UShort8> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UShort8::UShort8(RValue<UShort4> lo, RValue<UShort4> hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = V(UndefValue::get(Long2::getType()));
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *short8 = Nucleus::createBitCast(long2, Short8::getType());

		storeValue(short8);
	}

	RValue<UShort8> UShort8::operator=(RValue<UShort8> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UShort8> UShort8::operator=(const UShort8 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort8>(value);
	}

	RValue<UShort8> UShort8::operator=(const Reference<UShort8> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UShort8>(value);
	}

	RValue<UShort8> operator&(RValue<UShort8> lhs, RValue<UShort8> rhs)
	{
		return RValue<UShort8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UShort8> operator<<(RValue<UShort8> lhs, unsigned char rhs)
	{
		return As<UShort8>(x86::psllw(As<Short8>(lhs), rhs));   // FIXME: Fallback required
	}

	RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
	{
		return x86::psrlw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<UShort8> operator+(RValue<UShort8> lhs, RValue<UShort8> rhs)
	{
		return RValue<UShort8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UShort8> operator*(RValue<UShort8> lhs, RValue<UShort8> rhs)
	{
		return RValue<UShort8>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UShort8> operator+=(const UShort8 &lhs, RValue<UShort8> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UShort8> operator~(RValue<UShort8> val)
	{
		return RValue<UShort8>(Nucleus::createNot(val.value));
	}

	RValue<UShort8> Swizzle(RValue<UShort8> x, char select0, char select1, char select2, char select3, char select4, char select5, char select6, char select7)
	{
		int pshufb[16] =
		{
			select0 + 0,
			select0 + 1,
			select1 + 0,
			select1 + 1,
			select2 + 0,
			select2 + 1,
			select3 + 0,
			select3 + 1,
			select4 + 0,
			select4 + 1,
			select5 + 0,
			select5 + 1,
			select6 + 0,
			select6 + 1,
			select7 + 0,
			select7 + 1,
		};

		Value *byte16 = Nucleus::createBitCast(x.value, Byte16::getType());
		Value *shuffle = Nucleus::createShuffleVector(byte16, byte16, pshufb);
		Value *short8 = Nucleus::createBitCast(shuffle, UShort8::getType());

		return RValue<UShort8>(short8);
	}

	RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)
	{
		return x86::pmulhuw(x, y);   // FIXME: Fallback required
	}

	Type *UShort8::getType()
	{
		return T(VectorType::get(UShort::getType(), 8));
	}

	Int::Int(Argument<Int> argument)
	{
		storeValue(argument.value);
	}

	Int::Int(RValue<Byte> cast)
	{
		Value *integer = Nucleus::createZExt(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int(RValue<SByte> cast)
	{
		Value *integer = Nucleus::createSExt(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int(RValue<Short> cast)
	{
		Value *integer = Nucleus::createSExt(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int(RValue<UShort> cast)
	{
		Value *integer = Nucleus::createZExt(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int(RValue<Int2> cast)
	{
		*this = Extract(cast, 0);
	}

	Int::Int(RValue<Long> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int(RValue<Float> cast)
	{
		Value *integer = Nucleus::createFPToSI(cast.value, Int::getType());

		storeValue(integer);
	}

	Int::Int()
	{
	}

	Int::Int(int x)
	{
		storeValue(Nucleus::createConstantInt(x));
	}

	Int::Int(RValue<Int> rhs)
	{
		storeValue(rhs.value);
	}

	Int::Int(RValue<UInt> rhs)
	{
		storeValue(rhs.value);
	}

	Int::Int(const Int &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int::Int(const Reference<Int> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int::Int(const UInt &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int::Int(const Reference<UInt> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Int> Int::operator=(int rhs) const
	{
		return RValue<Int>(storeValue(Nucleus::createConstantInt(rhs)));
	}

	RValue<Int> Int::operator=(RValue<Int> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Int> Int::operator=(RValue<UInt> rhs) const
	{
		storeValue(rhs.value);

		return RValue<Int>(rhs);
	}

	RValue<Int> Int::operator=(const Int &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int>(value);
	}

	RValue<Int> Int::operator=(const Reference<Int> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int>(value);
	}

	RValue<Int> Int::operator=(const UInt &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int>(value);
	}

	RValue<Int> Int::operator=(const Reference<UInt> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int>(value);
	}

	RValue<Int> operator+(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Int> operator-(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Int> operator*(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Int> operator/(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Int> operator%(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Int> operator&(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Int> operator|(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Int> operator^(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Int> operator<<(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Int> operator>>(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Int>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<Int> operator+=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int> operator-=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Int> operator*=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Int> operator/=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Int> operator%=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Int> operator&=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int> operator|=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int> operator^=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Int> operator<<=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int> operator>>=(const Int &lhs, RValue<Int> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Int> operator+(RValue<Int> val)
	{
		return val;
	}

	RValue<Int> operator-(RValue<Int> val)
	{
		return RValue<Int>(Nucleus::createNeg(val.value));
	}

	RValue<Int> operator~(RValue<Int> val)
	{
		return RValue<Int>(Nucleus::createNot(val.value));
	}

	RValue<Int> operator++(const Int &val, int)   // Post-increment
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return res;
	}

	const Int &operator++(const Int &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> operator--(const Int &val, int)   // Post-decrement
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return res;
	}

	const Int &operator--(const Int &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return val;
	}

	RValue<Bool> operator<(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<Int> lhs, RValue<Int> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	RValue<Int> Max(RValue<Int> x, RValue<Int> y)
	{
		return IfThenElse(x > y, x, y);
	}

	RValue<Int> Min(RValue<Int> x, RValue<Int> y)
	{
		return IfThenElse(x < y, x, y);
	}

	RValue<Int> Clamp(RValue<Int> x, RValue<Int> min, RValue<Int> max)
	{
		return Min(Max(x, min), max);
	}

	RValue<Int> RoundInt(RValue<Float> cast)
	{
		return x86::cvtss2si(cast);

	//	return IfThenElse(val > 0.0f, Int(val + 0.5f), Int(val - 0.5f));
	}

	Type *Int::getType()
	{
		return T(llvm::Type::getInt32Ty(*::context));
	}

	Long::Long(RValue<Int> cast)
	{
		Value *integer = Nucleus::createSExt(cast.value, Long::getType());

		storeValue(integer);
	}

	Long::Long(RValue<UInt> cast)
	{
		Value *integer = Nucleus::createZExt(cast.value, Long::getType());

		storeValue(integer);
	}

	Long::Long()
	{
	}

	Long::Long(RValue<Long> rhs)
	{
		storeValue(rhs.value);
	}

	RValue<Long> Long::operator=(int64_t rhs) const
	{
		return RValue<Long>(storeValue(Nucleus::createConstantInt(rhs)));
	}

	RValue<Long> Long::operator=(RValue<Long> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Long> Long::operator=(const Long &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Long>(value);
	}

	RValue<Long> Long::operator=(const Reference<Long> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Long>(value);
	}

	RValue<Long> operator+(RValue<Long> lhs, RValue<Long> rhs)
	{
		return RValue<Long>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Long> operator-(RValue<Long> lhs, RValue<Long> rhs)
	{
		return RValue<Long>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Long> operator+=(const Long &lhs, RValue<Long> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Long> operator-=(const Long &lhs, RValue<Long> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Long> AddAtomic(RValue<Pointer<Long> > x, RValue<Long> y)
	{
		return RValue<Long>(Nucleus::createAtomicAdd(x.value, y.value));
	}

	Type *Long::getType()
	{
		return T(llvm::Type::getInt64Ty(*::context));
	}

	Long1::Long1(const RValue<UInt> cast)
	{
		Value *undefCast = Nucleus::createInsertElement(V(UndefValue::get(VectorType::get(Int::getType(), 2))), cast.value, 0);
		Value *zeroCast = Nucleus::createInsertElement(undefCast, V(Nucleus::createConstantInt(0)), 1);

		storeValue(Nucleus::createBitCast(zeroCast, Long1::getType()));
	}

	Long1::Long1(RValue<Long1> rhs)
	{
		storeValue(rhs.value);
	}

	Type *Long1::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(Long::getType(), 1));
		}
	}

	RValue<Long2> UnpackHigh(RValue<Long2> x, RValue<Long2> y)
	{
		int shuffle[2] = {1, 3};
		Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

		return RValue<Long2>(packed);
	}

	Type *Long2::getType()
	{
		return T(VectorType::get(Long::getType(), 2));
	}

	UInt::UInt(Argument<UInt> argument)
	{
		storeValue(argument.value);
	}

	UInt::UInt(RValue<UShort> cast)
	{
		Value *integer = Nucleus::createZExt(cast.value, UInt::getType());

		storeValue(integer);
	}

	UInt::UInt(RValue<Long> cast)
	{
		Value *integer = Nucleus::createTrunc(cast.value, UInt::getType());

		storeValue(integer);
	}

	UInt::UInt(RValue<Float> cast)
	{
		// Note: createFPToUI is broken, must perform conversion using createFPtoSI
		// Value *integer = Nucleus::createFPToUI(cast.value, UInt::getType());

		// Smallest positive value representable in UInt, but not in Int
		const unsigned int ustart = 0x80000000u;
		const float ustartf = float(ustart);

		// If the value is negative, store 0, otherwise store the result of the conversion
		storeValue((~(As<Int>(cast) >> 31) &
		// Check if the value can be represented as an Int
			IfThenElse(cast >= ustartf,
		// If the value is too large, subtract ustart and re-add it after conversion.
				As<Int>(As<UInt>(Int(cast - Float(ustartf))) + UInt(ustart)),
		// Otherwise, just convert normally
				Int(cast))).value);
	}

	UInt::UInt()
	{
	}

	UInt::UInt(int x)
	{
		storeValue(Nucleus::createConstantInt(x));
	}

	UInt::UInt(unsigned int x)
	{
		storeValue(Nucleus::createConstantInt(x));
	}

	UInt::UInt(RValue<UInt> rhs)
	{
		storeValue(rhs.value);
	}

	UInt::UInt(RValue<Int> rhs)
	{
		storeValue(rhs.value);
	}

	UInt::UInt(const UInt &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt::UInt(const Reference<UInt> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt::UInt(const Int &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt::UInt(const Reference<Int> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<UInt> UInt::operator=(unsigned int rhs) const
	{
		return RValue<UInt>(storeValue(Nucleus::createConstantInt(rhs)));
	}

	RValue<UInt> UInt::operator=(RValue<UInt> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UInt> UInt::operator=(RValue<Int> rhs) const
	{
		storeValue(rhs.value);

		return RValue<UInt>(rhs);
	}

	RValue<UInt> UInt::operator=(const UInt &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt>(value);
	}

	RValue<UInt> UInt::operator=(const Reference<UInt> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt>(value);
	}

	RValue<UInt> UInt::operator=(const Int &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt>(value);
	}

	RValue<UInt> UInt::operator=(const Reference<Int> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt>(value);
	}

	RValue<UInt> operator+(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UInt> operator-(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UInt> operator*(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UInt> operator/(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UInt> operator%(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UInt> operator&(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UInt> operator|(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UInt> operator^(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UInt> operator<<(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<UInt> operator>>(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<UInt>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<UInt> operator+=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt> operator-=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UInt> operator*=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UInt> operator/=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UInt> operator%=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UInt> operator&=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt> operator|=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt> operator^=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UInt> operator<<=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt> operator>>=(const UInt &lhs, RValue<UInt> rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UInt> operator+(RValue<UInt> val)
	{
		return val;
	}

	RValue<UInt> operator-(RValue<UInt> val)
	{
		return RValue<UInt>(Nucleus::createNeg(val.value));
	}

	RValue<UInt> operator~(RValue<UInt> val)
	{
		return RValue<UInt>(Nucleus::createNot(val.value));
	}

	RValue<UInt> operator++(const UInt &val, int)   // Post-increment
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createAdd(res.value, V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator++(const UInt &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return val;
	}

	RValue<UInt> operator--(const UInt &val, int)   // Post-decrement
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createSub(res.value, V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator--(const UInt &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), V(Nucleus::createConstantInt(1)));
		val.storeValue(inc);

		return val;
	}

	RValue<UInt> Max(RValue<UInt> x, RValue<UInt> y)
	{
		return IfThenElse(x > y, x, y);
	}

	RValue<UInt> Min(RValue<UInt> x, RValue<UInt> y)
	{
		return IfThenElse(x < y, x, y);
	}

	RValue<UInt> Clamp(RValue<UInt> x, RValue<UInt> min, RValue<UInt> max)
	{
		return Min(Max(x, min), max);
	}

	RValue<Bool> operator<(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<UInt> lhs, RValue<UInt> rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

//	RValue<UInt> RoundUInt(RValue<Float> cast)
//	{
//		return x86::cvtss2si(val);   // FIXME: Unsigned
//
//	//	return IfThenElse(val > 0.0f, Int(val + 0.5f), Int(val - 0.5f));
//	}

	Type *UInt::getType()
	{
		return T(llvm::Type::getInt32Ty(*::context));
	}

//	Int2::Int2(RValue<Int> cast)
//	{
//		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
//		Value *vector = Nucleus::createBitCast(extend, Int2::getType());
//
//		int shuffle[2] = {0, 0};
//		Value *replicate = Nucleus::createShuffleVector(vector, vector, shuffle);
//
//		storeValue(replicate);
//	}

	Int2::Int2(RValue<Int4> cast)
	{
		Value *long2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *element = Nucleus::createExtractElement(long2, Long::getType(), 0);
		Value *int2 = Nucleus::createBitCast(element, Int2::getType());

		storeValue(int2);
	}

	Int2::Int2()
	{
	//	xy.parent = this;
	}

	Int2::Int2(int x, int y)
	{
	//	xy.parent = this;

		Constant *constantVector[2];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 2));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	Int2::Int2(RValue<Int2> rhs)
	{
	//	xy.parent = this;

		storeValue(rhs.value);
	}

	Int2::Int2(const Int2 &rhs)
	{
	//	xy.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int2::Int2(const Reference<Int2> &rhs)
	{
	//	xy.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int2::Int2(RValue<Int> lo, RValue<Int> hi)
	{
		if(CPUID::supportsMMX2())
		{
			// movd mm0, lo
			// movd mm1, hi
			// punpckldq mm0, mm1
			storeValue(As<Int2>(UnpackLow(As<Int2>(Long1(RValue<UInt>(lo))), As<Int2>(Long1(RValue<UInt>(hi))))).value);
		}
		else
		{
			int shuffle[2] = {0, 1};
			Value *packed = Nucleus::createShuffleVector(Nucleus::createBitCast(lo.value, T(VectorType::get(Int::getType(), 1))), Nucleus::createBitCast(hi.value, T(VectorType::get(Int::getType(), 1))), shuffle);

			storeValue(Nucleus::createBitCast(packed, Int2::getType()));
		}
	}

	RValue<Int2> Int2::operator=(RValue<Int2> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Int2> Int2::operator=(const Int2 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int2>(value);
	}

	RValue<Int2> Int2::operator=(const Reference<Int2> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int2>(value);
	}

	RValue<Int2> operator+(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::paddd(lhs, rhs);
		}
		else
		{
			return RValue<Int2>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<Int2> operator-(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::psubd(lhs, rhs);
		}
		else
		{
			return RValue<Int2>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

//	RValue<Int2> operator*(RValue<Int2> lhs, RValue<Int2> rhs)
//	{
//		return RValue<Int2>(Nucleus::createMul(lhs.value, rhs.value));
//	}

//	RValue<Int2> operator/(RValue<Int2> lhs, RValue<Int2> rhs)
//	{
//		return RValue<Int2>(Nucleus::createSDiv(lhs.value, rhs.value));
//	}

//	RValue<Int2> operator%(RValue<Int2> lhs, RValue<Int2> rhs)
//	{
//		return RValue<Int2>(Nucleus::createSRem(lhs.value, rhs.value));
//	}

	RValue<Int2> operator&(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Int2>(x86::pand(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Int2>(Nucleus::createAnd(lhs.value, rhs.value));
		}
	}

	RValue<Int2> operator|(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Int2>(x86::por(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Int2>(Nucleus::createOr(lhs.value, rhs.value));
		}
	}

	RValue<Int2> operator^(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<Int2>(x86::pxor(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<Int2>(Nucleus::createXor(lhs.value, rhs.value));
		}
	}

	RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs)
	{
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
	{
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
	}

	RValue<Int2> operator<<(RValue<Int2> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
	}

	RValue<Int2> operator+=(const Int2 &lhs, RValue<Int2> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int2> operator-=(const Int2 &lhs, RValue<Int2> rhs)
	{
		return lhs = lhs - rhs;
	}

//	RValue<Int2> operator*=(const Int2 &lhs, RValue<Int2> rhs)
//	{
//		return lhs = lhs * rhs;
//	}

//	RValue<Int2> operator/=(const Int2 &lhs, RValue<Int2> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<Int2> operator%=(const Int2 &lhs, RValue<Int2> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<Int2> operator&=(const Int2 &lhs, RValue<Int2> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int2> operator|=(const Int2 &lhs, RValue<Int2> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int2> operator^=(const Int2 &lhs, RValue<Int2> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Int2> operator<<=(const Int2 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int2> operator>>=(const Int2 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Int2> operator<<=(const Int2 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int2> operator>>=(const Int2 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs >> rhs;
	}

//	RValue<Int2> operator+(RValue<Int2> val)
//	{
//		return val;
//	}

//	RValue<Int2> operator-(RValue<Int2> val)
//	{
//		return RValue<Int2>(Nucleus::createNeg(val.value));
//	}

	RValue<Int2> operator~(RValue<Int2> val)
	{
		if(CPUID::supportsMMX2())
		{
			return val ^ Int2(0xFFFFFFFF, 0xFFFFFFFF);
		}
		else
		{
			return RValue<Int2>(Nucleus::createNot(val.value));
		}
	}

	RValue<Long1> UnpackLow(RValue<Int2> x, RValue<Int2> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpckldq(x, y);
		}
		else
		{
			int shuffle[2] = {0, 2};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Long1>(Nucleus::createBitCast(packed, Long1::getType()));
		}
	}

	RValue<Long1> UnpackHigh(RValue<Int2> x, RValue<Int2> y)
	{
		if(CPUID::supportsMMX2())
		{
			return x86::punpckhdq(x, y);
		}
		else
		{
			int shuffle[2] = {1, 3};
			Value *packed = Nucleus::createShuffleVector(x.value, y.value, shuffle);

			return RValue<Long1>(Nucleus::createBitCast(packed, Long1::getType()));
		}
	}

	RValue<Int> Extract(RValue<Int2> val, int i)
	{
		if(false)   // FIXME: LLVM does not generate optimal code
		{
			return RValue<Int>(Nucleus::createExtractElement(val.value, Int::getType(), i));
		}
		else
		{
			if(i == 0)
			{
				return RValue<Int>(Nucleus::createExtractElement(Nucleus::createBitCast(val.value, T(VectorType::get(Int::getType(), 2))), Int::getType(), 0));
			}
			else
			{
				Int2 val2 = As<Int2>(UnpackHigh(val, val));

				return Extract(val2, 0);
			}
		}
	}

	RValue<Int2> Insert(RValue<Int2> val, RValue<Int> element, int i)
	{
		return RValue<Int2>(Nucleus::createBitCast(Nucleus::createInsertElement(Nucleus::createBitCast(val.value, T(VectorType::get(Int::getType(), 2))), element.value, i), Int2::getType()));
	}

	Type *Int2::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(Int::getType(), 2));
		}
	}

	UInt2::UInt2()
	{
	//	xy.parent = this;
	}

	UInt2::UInt2(unsigned int x, unsigned int y)
	{
	//	xy.parent = this;

		Constant *constantVector[2];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		Value *vector = V(Nucleus::createConstantVector(constantVector, 2));

		storeValue(Nucleus::createBitCast(vector, getType()));
	}

	UInt2::UInt2(RValue<UInt2> rhs)
	{
	//	xy.parent = this;

		storeValue(rhs.value);
	}

	UInt2::UInt2(const UInt2 &rhs)
	{
	//	xy.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt2::UInt2(const Reference<UInt2> &rhs)
	{
	//	xy.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<UInt2> UInt2::operator=(RValue<UInt2> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UInt2> UInt2::operator=(const UInt2 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt2>(value);
	}

	RValue<UInt2> UInt2::operator=(const Reference<UInt2> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt2>(value);
	}

	RValue<UInt2> operator+(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UInt2>(x86::paddd(As<Int2>(lhs), As<Int2>(rhs)));
		}
		else
		{
			return RValue<UInt2>(Nucleus::createAdd(lhs.value, rhs.value));
		}
	}

	RValue<UInt2> operator-(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UInt2>(x86::psubd(As<Int2>(lhs), As<Int2>(rhs)));
		}
		else
		{
			return RValue<UInt2>(Nucleus::createSub(lhs.value, rhs.value));
		}
	}

//	RValue<UInt2> operator*(RValue<UInt2> lhs, RValue<UInt2> rhs)
//	{
//		return RValue<UInt2>(Nucleus::createMul(lhs.value, rhs.value));
//	}

//	RValue<UInt2> operator/(RValue<UInt2> lhs, RValue<UInt2> rhs)
//	{
//		return RValue<UInt2>(Nucleus::createUDiv(lhs.value, rhs.value));
//	}

//	RValue<UInt2> operator%(RValue<UInt2> lhs, RValue<UInt2> rhs)
//	{
//		return RValue<UInt2>(Nucleus::createURem(lhs.value, rhs.value));
//	}

	RValue<UInt2> operator&(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UInt2>(x86::pand(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UInt2>(Nucleus::createAnd(lhs.value, rhs.value));
		}
	}

	RValue<UInt2> operator|(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UInt2>(x86::por(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UInt2>(Nucleus::createOr(lhs.value, rhs.value));
		}
	}

	RValue<UInt2> operator^(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		if(CPUID::supportsMMX2())
		{
			return As<UInt2>(x86::pxor(As<Short4>(lhs), As<Short4>(rhs)));
		}
		else
		{
			return RValue<UInt2>(Nucleus::createXor(lhs.value, rhs.value));
		}
	}

	RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs)
	{
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
	{
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
	}

	RValue<UInt2> operator<<(RValue<UInt2> lhs, RValue<Long1> rhs)
	{
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, RValue<Long1> rhs)
	{
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
	}

	RValue<UInt2> operator+=(const UInt2 &lhs, RValue<UInt2> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt2> operator-=(const UInt2 &lhs, RValue<UInt2> rhs)
	{
		return lhs = lhs - rhs;
	}

//	RValue<UInt2> operator*=(const UInt2 &lhs, RValue<UInt2> rhs)
//	{
//		return lhs = lhs * rhs;
//	}

//	RValue<UInt2> operator/=(const UInt2 &lhs, RValue<UInt2> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<UInt2> operator%=(const UInt2 &lhs, RValue<UInt2> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<UInt2> operator&=(const UInt2 &lhs, RValue<UInt2> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt2> operator|=(const UInt2 &lhs, RValue<UInt2> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt2> operator^=(const UInt2 &lhs, RValue<UInt2> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UInt2> operator<<=(const UInt2 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt2> operator>>=(const UInt2 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UInt2> operator<<=(const UInt2 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt2> operator>>=(const UInt2 &lhs, RValue<Long1> rhs)
	{
		return lhs = lhs >> rhs;
	}

//	RValue<UInt2> operator+(RValue<UInt2> val)
//	{
//		return val;
//	}

//	RValue<UInt2> operator-(RValue<UInt2> val)
//	{
//		return RValue<UInt2>(Nucleus::createNeg(val.value));
//	}

	RValue<UInt2> operator~(RValue<UInt2> val)
	{
		if(CPUID::supportsMMX2())
		{
			return val ^ UInt2(0xFFFFFFFF, 0xFFFFFFFF);
		}
		else
		{
			return RValue<UInt2>(Nucleus::createNot(val.value));
		}
	}

	Type *UInt2::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return T(VectorType::get(UInt::getType(), 2));
		}
	}

	Int4::Int4(RValue<Byte4> cast)
	{
		Value *x = Nucleus::createBitCast(cast.value, Int::getType());
		Value *a = Nucleus::createInsertElement(V(UndefValue::get(Int4::getType())), x, 0);

		Value *e;

		if (CPUID::supportsSSE4_1())
		{
			e = x86::pmovzxbd(RValue<Int4>(a)).value;
		}
		else
		{
			int swizzle[16] = {0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23};
			Value *b = Nucleus::createBitCast(a, Byte16::getType());
			Value *c = Nucleus::createShuffleVector(b, V(Nucleus::createNullValue(Byte16::getType())), swizzle);

			int swizzle2[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *d = Nucleus::createBitCast(c, Short8::getType());
			e = Nucleus::createShuffleVector(d, V(Nucleus::createNullValue(Short8::getType())), swizzle2);
		}

		Value *f = Nucleus::createBitCast(e, Int4::getType());
		storeValue(f);
	}

	Int4::Int4(RValue<SByte4> cast)
	{
		Value *x = Nucleus::createBitCast(cast.value, Int::getType());
		Value *a = Nucleus::createInsertElement(V(UndefValue::get(Int4::getType())), x, 0);

		Value *g;

		if (CPUID::supportsSSE4_1())
		{
			g = x86::pmovsxbd(RValue<Int4>(a)).value;
		}
		else
		{
			int	swizzle[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
			Value *b = Nucleus::createBitCast(a, Byte16::getType());
			Value *c = Nucleus::createShuffleVector(b, b, swizzle);

			int swizzle2[8] = {0, 0, 1, 1, 2, 2, 3, 3};
			Value *d = Nucleus::createBitCast(c, Short8::getType());
			Value *e = Nucleus::createShuffleVector(d, d, swizzle2);

			Value *f = Nucleus::createBitCast(e, Int4::getType());
			//	g = Nucleus::createAShr(f, Nucleus::createConstantInt(24));
			g = x86::psrad(RValue<Int4>(f), 24).value;
		}

		storeValue(g);
	}

	Int4::Int4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *xyzw = Nucleus::createFPToSI(cast.value, Int4::getType());

		storeValue(xyzw);
	}

	Int4::Int4(RValue<Short4> cast)
	{
		Value *long2 = V(UndefValue::get(Long2::getType()));
		Value *element = Nucleus::createBitCast(cast.value, Long::getType());
		long2 = Nucleus::createInsertElement(long2, element, 0);
		RValue<Int4> vector = RValue<Int4>(Nucleus::createBitCast(long2, Int4::getType()));

		if(CPUID::supportsSSE4_1())
		{
			storeValue(x86::pmovsxwd(vector).value);
		}
		else
		{
			Value *b = Nucleus::createBitCast(vector.value, Short8::getType());

			int swizzle[8] = {0, 0, 1, 1, 2, 2, 3, 3};
			Value *c = Nucleus::createShuffleVector(b, b, swizzle);
			Value *d = Nucleus::createBitCast(c, Int4::getType());
			storeValue(d);

			// Each Short is packed into each Int in the (Short | Short) format.
			// Shifting by 16 will retrieve the original Short value.
			// Shitfing an Int will propagate the sign bit, which will work
			// for both positive and negative values of a Short.
			*this >>= 16;
		}
	}

	Int4::Int4(RValue<UShort4> cast)
	{
		Value *long2 = V(UndefValue::get(Long2::getType()));
		Value *element = Nucleus::createBitCast(cast.value, Long::getType());
		long2 = Nucleus::createInsertElement(long2, element, 0);
		RValue<Int4> vector = RValue<Int4>(Nucleus::createBitCast(long2, Int4::getType()));

		if(CPUID::supportsSSE4_1())
		{
			storeValue(x86::pmovzxwd(RValue<Int4>(vector)).value);
		}
		else
		{
			Value *b = Nucleus::createBitCast(vector.value, Short8::getType());

			int swizzle[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *c = Nucleus::createShuffleVector(b, V(Nucleus::createNullValue(Short8::getType())), swizzle);
			Value *d = Nucleus::createBitCast(c, Int4::getType());
			storeValue(d);
		}
	}

	Int4::Int4()
	{
	//	xyzw.parent = this;
	}

	Int4::Int4(int xyzw)
	{
		constant(xyzw, xyzw, xyzw, xyzw);
	}

	Int4::Int4(int x, int yzw)
	{
		constant(x, yzw, yzw, yzw);
	}

	Int4::Int4(int x, int y, int zw)
	{
		constant(x, y, zw, zw);
	}

	Int4::Int4(int x, int y, int z, int w)
	{
		constant(x, y, z, w);
	}

	void Int4::constant(int x, int y, int z, int w)
	{
	//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		constantVector[2] = Nucleus::createConstantInt(z);
		constantVector[3] = Nucleus::createConstantInt(w);

		storeValue(Nucleus::createConstantVector(constantVector, 4));
	}

	Int4::Int4(RValue<Int4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Int4::Int4(const Int4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int4::Int4(const Reference<Int4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int4::Int4(RValue<UInt4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	Int4::Int4(const UInt4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int4::Int4(const Reference<UInt4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Int4::Int4(RValue<Int2> lo, RValue<Int2> hi)
	{
	//	xyzw.parent = this;

		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = V(UndefValue::get(Long2::getType()));
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *int4 = Nucleus::createBitCast(long2, Int4::getType());

		storeValue(int4);
	}

	Int4::Int4(RValue<Int> rhs)
	{
	//	xyzw.parent = this;

		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	Int4::Int4(const Int &rhs)
	{
	//	xyzw.parent = this;

		*this = RValue<Int>(rhs.loadValue());
	}

	Int4::Int4(const Reference<Int> &rhs)
	{
	//	xyzw.parent = this;

		*this = RValue<Int>(rhs.loadValue());
	}

	RValue<Int4> Int4::operator=(RValue<Int4> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Int4> Int4::operator=(const Int4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int4>(value);
	}

	RValue<Int4> Int4::operator=(const Reference<Int4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Int4>(value);
	}

	RValue<Int4> operator+(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Int4> operator-(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Int4> operator*(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Int4> operator/(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Int4> operator%(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Int4> operator&(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Int4> operator|(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Int4> operator^(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs)
	{
		return x86::pslld(lhs, rhs);
	}

	RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
	{
		return x86::psrad(lhs, rhs);
	}

	RValue<Int4> operator<<(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Int4> operator>>(RValue<Int4> lhs, RValue<Int4> rhs)
	{
		return RValue<Int4>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<Int4> operator+=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int4> operator-=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Int4> operator*=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs * rhs;
	}

//	RValue<Int4> operator/=(const Int4 &lhs, RValue<Int4> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<Int4> operator%=(const Int4 &lhs, RValue<Int4> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<Int4> operator&=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int4> operator|=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int4> operator^=(const Int4 &lhs, RValue<Int4> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Int4> operator<<=(const Int4 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int4> operator>>=(const Int4 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Int4> operator+(RValue<Int4> val)
	{
		return val;
	}

	RValue<Int4> operator-(RValue<Int4> val)
	{
		return RValue<Int4>(Nucleus::createNeg(val.value));
	}

	RValue<Int4> operator~(RValue<Int4> val)
	{
		return RValue<Int4>(Nucleus::createNot(val.value));
	}

	RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
	}

	RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLE(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGT(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
	}

	RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGE(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
	}

	RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::pmaxsd(x, y);
		}
		else
		{
			RValue<Int4> greater = CmpNLE(x, y);
			return x & greater | y & ~greater;
		}
	}

	RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::pminsd(x, y);
		}
		else
		{
			RValue<Int4> less = CmpLT(x, y);
			return x & less | y & ~less;
		}
	}

	RValue<Int4> RoundInt(RValue<Float4> cast)
	{
		return x86::cvtps2dq(cast);
	}

	RValue<Short8> Pack(RValue<Int4> x, RValue<Int4> y)
	{
		return x86::packssdw(x, y);
	}

	RValue<Int> Extract(RValue<Int4> x, int i)
	{
		return RValue<Int>(Nucleus::createExtractElement(x.value, Int::getType(), i));
	}

	RValue<Int4> Insert(RValue<Int4> x, RValue<Int> element, int i)
	{
		return RValue<Int4>(Nucleus::createInsertElement(x.value, element.value, i));
	}

	RValue<Int> SignMask(RValue<Int4> x)
	{
		return x86::movmskps(As<Float4>(x));
	}

	RValue<Int4> Swizzle(RValue<Int4> x, unsigned char select)
	{
		return RValue<Int4>(createSwizzle4(x.value, select));
	}

	Type *Int4::getType()
	{
		return T(VectorType::get(Int::getType(), 4));
	}

	UInt4::UInt4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		// Note: createFPToUI is broken, must perform conversion using createFPtoSI
		// Value *xyzw = Nucleus::createFPToUI(cast.value, UInt4::getType());

		// Smallest positive value representable in UInt, but not in Int
		const unsigned int ustart = 0x80000000u;
		const float ustartf = float(ustart);

		// Check if the value can be represented as an Int
		Int4 uiValue = CmpNLT(cast, Float4(ustartf));
		// If the value is too large, subtract ustart and re-add it after conversion.
		uiValue = (uiValue & As<Int4>(As<UInt4>(Int4(cast - Float4(ustartf))) + UInt4(ustart))) |
		// Otherwise, just convert normally
		          (~uiValue & Int4(cast));
		// If the value is negative, store 0, otherwise store the result of the conversion
		storeValue((~(As<Int4>(cast) >> 31) & uiValue).value);
	}

	UInt4::UInt4()
	{
	//	xyzw.parent = this;
	}

	UInt4::UInt4(int xyzw)
	{
		constant(xyzw, xyzw, xyzw, xyzw);
	}

	UInt4::UInt4(int x, int yzw)
	{
		constant(x, yzw, yzw, yzw);
	}

	UInt4::UInt4(int x, int y, int zw)
	{
		constant(x, y, zw, zw);
	}

	UInt4::UInt4(int x, int y, int z, int w)
	{
		constant(x, y, z, w);
	}

	void UInt4::constant(int x, int y, int z, int w)
	{
	//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		constantVector[2] = Nucleus::createConstantInt(z);
		constantVector[3] = Nucleus::createConstantInt(w);

		storeValue(Nucleus::createConstantVector(constantVector, 4));
	}

	UInt4::UInt4(RValue<UInt4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	UInt4::UInt4(const UInt4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt4::UInt4(const Reference<UInt4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt4::UInt4(RValue<Int4> rhs)
	{
	//	xyzw.parent = this;

		storeValue(rhs.value);
	}

	UInt4::UInt4(const Int4 &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt4::UInt4(const Reference<Int4> &rhs)
	{
	//	xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	UInt4::UInt4(RValue<UInt2> lo, RValue<UInt2> hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = V(UndefValue::get(Long2::getType()));
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *uint4 = Nucleus::createBitCast(long2, Int4::getType());

		storeValue(uint4);
	}

	RValue<UInt4> UInt4::operator=(RValue<UInt4> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<UInt4> UInt4::operator=(const UInt4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt4>(value);
	}

	RValue<UInt4> UInt4::operator=(const Reference<UInt4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<UInt4>(value);
	}

	RValue<UInt4> operator+(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UInt4> operator-(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UInt4> operator*(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UInt4> operator/(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UInt4> operator%(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UInt4> operator&(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UInt4> operator|(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UInt4> operator^(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs)
	{
		return As<UInt4>(x86::pslld(As<Int4>(lhs), rhs));
	}

	RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
	{
		return x86::psrld(lhs, rhs);
	}

	RValue<UInt4> operator<<(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<UInt4> operator>>(RValue<UInt4> lhs, RValue<UInt4> rhs)
	{
		return RValue<UInt4>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<UInt4> operator+=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt4> operator-=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UInt4> operator*=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs * rhs;
	}

//	RValue<UInt4> operator/=(const UInt4 &lhs, RValue<UInt4> rhs)
//	{
//		return lhs = lhs / rhs;
//	}

//	RValue<UInt4> operator%=(const UInt4 &lhs, RValue<UInt4> rhs)
//	{
//		return lhs = lhs % rhs;
//	}

	RValue<UInt4> operator&=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt4> operator|=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt4> operator^=(const UInt4 &lhs, RValue<UInt4> rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UInt4> operator<<=(const UInt4 &lhs, unsigned char rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt4> operator>>=(const UInt4 &lhs, unsigned char rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UInt4> operator+(RValue<UInt4> val)
	{
		return val;
	}

	RValue<UInt4> operator-(RValue<UInt4> val)
	{
		return RValue<UInt4>(Nucleus::createNeg(val.value));
	}

	RValue<UInt4> operator~(RValue<UInt4> val)
	{
		return RValue<UInt4>(Nucleus::createNot(val.value));
	}

	RValue<UInt4> CmpEQ(RValue<UInt4> x, RValue<UInt4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType())) ^ UInt4(0xFFFFFFFF);
	}

	RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULT(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULE(x.value, y.value), Int4::getType()));
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGT(x.value, y.value), Int4::getType())) ^ UInt4(0xFFFFFFFF);
	}

	RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGE(x.value, y.value), Int4::getType()));
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULT(x.value, y.value), Int4::getType())) ^ UInt4(0xFFFFFFFF);
	}

	RValue<UInt4> CmpNLE(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGT(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::pmaxud(x, y);
		}
		else
		{
			RValue<UInt4> greater = CmpNLE(x, y);
			return x & greater | y & ~greater;
		}
	}

	RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::pminud(x, y);
		}
		else
		{
			RValue<UInt4> less = CmpLT(x, y);
			return x & less | y & ~less;
		}
	}

	RValue<UShort8> Pack(RValue<UInt4> x, RValue<UInt4> y)
	{
		return x86::packusdw(x, y);   // FIXME: Fallback required
	}

	Type *UInt4::getType()
	{
		return T(VectorType::get(UInt::getType(), 4));
	}

	Float::Float(RValue<Int> cast)
	{
		Value *integer = Nucleus::createSIToFP(cast.value, Float::getType());

		storeValue(integer);
	}

	Float::Float()
	{

	}

	Float::Float(float x)
	{
		storeValue(Nucleus::createConstantFloat(x));
	}

	Float::Float(RValue<Float> rhs)
	{
		storeValue(rhs.value);
	}

	Float::Float(const Float &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Float::Float(const Reference<Float> &rhs)
	{
		Value *value = rhs.loadValue();
		storeValue(value);
	}

	RValue<Float> Float::operator=(RValue<Float> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Float> Float::operator=(const Float &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Float>(value);
	}

	RValue<Float> Float::operator=(const Reference<Float> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Float>(value);
	}

	RValue<Float> operator+(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Float>(Nucleus::createFAdd(lhs.value, rhs.value));
	}

	RValue<Float> operator-(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Float>(Nucleus::createFSub(lhs.value, rhs.value));
	}

	RValue<Float> operator*(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Float>(Nucleus::createFMul(lhs.value, rhs.value));
	}

	RValue<Float> operator/(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Float>(Nucleus::createFDiv(lhs.value, rhs.value));
	}

	RValue<Float> operator+=(const Float &lhs, RValue<Float> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Float> operator-=(const Float &lhs, RValue<Float> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Float> operator*=(const Float &lhs, RValue<Float> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Float> operator/=(const Float &lhs, RValue<Float> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Float> operator+(RValue<Float> val)
	{
		return val;
	}

	RValue<Float> operator-(RValue<Float> val)
	{
		return RValue<Float>(Nucleus::createFNeg(val.value));
	}

	RValue<Bool> operator<(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpONE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(RValue<Float> lhs, RValue<Float> rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOEQ(lhs.value, rhs.value));
	}

	RValue<Float> Abs(RValue<Float> x)
	{
		return IfThenElse(x > 0.0f, x, -x);
	}

	RValue<Float> Max(RValue<Float> x, RValue<Float> y)
	{
		return IfThenElse(x > y, x, y);
	}

	RValue<Float> Min(RValue<Float> x, RValue<Float> y)
	{
		return IfThenElse(x < y, x, y);
	}

	RValue<Float> Rcp_pp(RValue<Float> x, bool exactAtPow2)
	{
		if(exactAtPow2)
		{
			// rcpss uses a piecewise-linear approximation which minimizes the relative error
			// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
			return x86::rcpss(x) * Float(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
		}
		else
		{
			return x86::rcpss(x);
		}
	}

	RValue<Float> RcpSqrt_pp(RValue<Float> x)
	{
		return x86::rsqrtss(x);
	}

	RValue<Float> Sqrt(RValue<Float> x)
	{
		return x86::sqrtss(x);
	}

	RValue<Float> Round(RValue<Float> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundss(x, 0);
		}
		else
		{
			return Float4(Round(Float4(x))).x;
		}
	}

	RValue<Float> Trunc(RValue<Float> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundss(x, 3);
		}
		else
		{
			return Float(Int(x));   // Rounded toward zero
		}
	}

	RValue<Float> Frac(RValue<Float> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x - x86::floorss(x);
		}
		else
		{
			return Float4(Frac(Float4(x))).x;
		}
	}

	RValue<Float> Floor(RValue<Float> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::floorss(x);
		}
		else
		{
			return Float4(Floor(Float4(x))).x;
		}
	}

	RValue<Float> Ceil(RValue<Float> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::ceilss(x);
		}
		else
		{
			return Float4(Ceil(Float4(x))).x;
		}
	}

	Type *Float::getType()
	{
		return T(llvm::Type::getFloatTy(*::context));
	}

	Float2::Float2(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *int64x2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *int64 = Nucleus::createExtractElement(int64x2, Long::getType(), 0);
		Value *float2 = Nucleus::createBitCast(int64, Float2::getType());

		storeValue(float2);
	}

	Type *Float2::getType()
	{
		return T(VectorType::get(Float::getType(), 2));
	}

	Float4::Float4(RValue<Byte4> cast)
	{
		xyzw.parent = this;

		#if 0
			Value *xyzw = Nucleus::createUIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = loadValue();

			Value *i8x = Nucleus::createExtractElement(cast.value, 0);
			Value *f32x = Nucleus::createUIToFP(i8x, Float::getType());
			Value *x = Nucleus::createInsertElement(vector, f32x, 0);

			Value *i8y = Nucleus::createExtractElement(cast.value, V(Nucleus::createConstantInt(1)));
			Value *f32y = Nucleus::createUIToFP(i8y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, V(Nucleus::createConstantInt(1)));

			Value *i8z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createUIToFP(i8z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i8w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createUIToFP(i8w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *a = Int4(cast).loadValue();
			Value *xyzw = Nucleus::createSIToFP(a, Float4::getType());
		#endif

		storeValue(xyzw);
	}

	Float4::Float4(RValue<SByte4> cast)
	{
		xyzw.parent = this;

		#if 0
			Value *xyzw = Nucleus::createSIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = loadValue();

			Value *i8x = Nucleus::createExtractElement(cast.value, 0);
			Value *f32x = Nucleus::createSIToFP(i8x, Float::getType());
			Value *x = Nucleus::createInsertElement(vector, f32x, 0);

			Value *i8y = Nucleus::createExtractElement(cast.value, V(Nucleus::createConstantInt(1)));
			Value *f32y = Nucleus::createSIToFP(i8y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, V(Nucleus::createConstantInt(1)));

			Value *i8z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createSIToFP(i8z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i8w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createSIToFP(i8w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *a = Int4(cast).loadValue();
			Value *xyzw = Nucleus::createSIToFP(a, Float4::getType());
		#endif

		storeValue(xyzw);
	}

	Float4::Float4(RValue<Short4> cast)
	{
		xyzw.parent = this;

		Int4 c(cast);
		storeValue(Nucleus::createSIToFP(RValue<Int4>(c).value, Float4::getType()));
	}

	Float4::Float4(RValue<UShort4> cast)
	{
		xyzw.parent = this;

		Int4 c(cast);
		storeValue(Nucleus::createSIToFP(RValue<Int4>(c).value, Float4::getType()));
	}

	Float4::Float4(RValue<Int4> cast)
	{
		xyzw.parent = this;

		Value *xyzw = Nucleus::createSIToFP(cast.value, Float4::getType());

		storeValue(xyzw);
	}

	Float4::Float4(RValue<UInt4> cast)
	{
		xyzw.parent = this;

		Value *xyzw = Nucleus::createUIToFP(cast.value, Float4::getType());

		storeValue(xyzw);
	}

	Float4::Float4()
	{
		xyzw.parent = this;
	}

	Float4::Float4(float xyzw)
	{
		constant(xyzw, xyzw, xyzw, xyzw);
	}

	Float4::Float4(float x, float yzw)
	{
		constant(x, yzw, yzw, yzw);
	}

	Float4::Float4(float x, float y, float zw)
	{
		constant(x, y, zw, zw);
	}

	Float4::Float4(float x, float y, float z, float w)
	{
		constant(x, y, z, w);
	}

	void Float4::constant(float x, float y, float z, float w)
	{
		xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantFloat(x);
		constantVector[1] = Nucleus::createConstantFloat(y);
		constantVector[2] = Nucleus::createConstantFloat(z);
		constantVector[3] = Nucleus::createConstantFloat(w);

		storeValue(Nucleus::createConstantVector(constantVector, 4));
	}

	Float4::Float4(RValue<Float4> rhs)
	{
		xyzw.parent = this;

		storeValue(rhs.value);
	}

	Float4::Float4(const Float4 &rhs)
	{
		xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Float4::Float4(const Reference<Float4> &rhs)
	{
		xyzw.parent = this;

		Value *value = rhs.loadValue();
		storeValue(value);
	}

	Float4::Float4(RValue<Float> rhs)
	{
		xyzw.parent = this;

		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	Float4::Float4(const Float &rhs)
	{
		xyzw.parent = this;

		*this = RValue<Float>(rhs.loadValue());
	}

	Float4::Float4(const Reference<Float> &rhs)
	{
		xyzw.parent = this;

		*this = RValue<Float>(rhs.loadValue());
	}

	RValue<Float4> Float4::operator=(float x) const
	{
		return *this = Float4(x, x, x, x);
	}

	RValue<Float4> Float4::operator=(RValue<Float4> rhs) const
	{
		storeValue(rhs.value);

		return rhs;
	}

	RValue<Float4> Float4::operator=(const Float4 &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Float4>(value);
	}

	RValue<Float4> Float4::operator=(const Reference<Float4> &rhs) const
	{
		Value *value = rhs.loadValue();
		storeValue(value);

		return RValue<Float4>(value);
	}

	RValue<Float4> Float4::operator=(RValue<Float> rhs) const
	{
		return *this = Float4(rhs);
	}

	RValue<Float4> Float4::operator=(const Float &rhs) const
	{
		return *this = Float4(rhs);
	}

	RValue<Float4> Float4::operator=(const Reference<Float> &rhs) const
	{
		return *this = Float4(rhs);
	}

	RValue<Float4> operator+(RValue<Float4> lhs, RValue<Float4> rhs)
	{
		return RValue<Float4>(Nucleus::createFAdd(lhs.value, rhs.value));
	}

	RValue<Float4> operator-(RValue<Float4> lhs, RValue<Float4> rhs)
	{
		return RValue<Float4>(Nucleus::createFSub(lhs.value, rhs.value));
	}

	RValue<Float4> operator*(RValue<Float4> lhs, RValue<Float4> rhs)
	{
		return RValue<Float4>(Nucleus::createFMul(lhs.value, rhs.value));
	}

	RValue<Float4> operator/(RValue<Float4> lhs, RValue<Float4> rhs)
	{
		return RValue<Float4>(Nucleus::createFDiv(lhs.value, rhs.value));
	}

	RValue<Float4> operator%(RValue<Float4> lhs, RValue<Float4> rhs)
	{
		return RValue<Float4>(Nucleus::createFRem(lhs.value, rhs.value));
	}

	RValue<Float4> operator+=(const Float4 &lhs, RValue<Float4> rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Float4> operator-=(const Float4 &lhs, RValue<Float4> rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Float4> operator*=(const Float4 &lhs, RValue<Float4> rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Float4> operator/=(const Float4 &lhs, RValue<Float4> rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Float4> operator%=(const Float4 &lhs, RValue<Float4> rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Float4> operator+(RValue<Float4> val)
	{
		return val;
	}

	RValue<Float4> operator-(RValue<Float4> val)
	{
		return RValue<Float4>(Nucleus::createFNeg(val.value));
	}

	RValue<Float4> Abs(RValue<Float4> x)
	{
		Value *vector = Nucleus::createBitCast(x.value, Int4::getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantInt(0x7FFFFFFF);
		constantVector[1] = Nucleus::createConstantInt(0x7FFFFFFF);
		constantVector[2] = Nucleus::createConstantInt(0x7FFFFFFF);
		constantVector[3] = Nucleus::createConstantInt(0x7FFFFFFF);

		Value *result = Nucleus::createAnd(vector, V(Nucleus::createConstantVector(constantVector, 4)));

		return RValue<Float4>(Nucleus::createBitCast(result, Float4::getType()));
	}

	RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
	{
		return x86::maxps(x, y);
	}

	RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
	{
		return x86::minps(x, y);
	}

	RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
	{
		if(exactAtPow2)
		{
			// rcpps uses a piecewise-linear approximation which minimizes the relative error
			// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
			return x86::rcpps(x) * Float4(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
		}
		else
		{
			return x86::rcpps(x);
		}
	}

	RValue<Float4> RcpSqrt_pp(RValue<Float4> x)
	{
		return x86::rsqrtps(x);
	}

	RValue<Float4> Sqrt(RValue<Float4> x)
	{
		return x86::sqrtps(x);
	}

	RValue<Float4> Insert(const Float4 &val, RValue<Float> element, int i)
	{
		Value *value = val.loadValue();
		Value *insert = Nucleus::createInsertElement(value, element.value, i);

		val = RValue<Float4>(insert);

		return val;
	}

	RValue<Float> Extract(RValue<Float4> x, int i)
	{
		return RValue<Float>(Nucleus::createExtractElement(x.value, Float::getType(), i));
	}

	RValue<Float4> Swizzle(RValue<Float4> x, unsigned char select)
	{
		return RValue<Float4>(createSwizzle4(x.value, select));
	}

	RValue<Float4> ShuffleLowHigh(RValue<Float4> x, RValue<Float4> y, unsigned char imm)
	{
		int shuffle[4] =
		{
			((imm >> 0) & 0x03) + 0,
			((imm >> 2) & 0x03) + 0,
			((imm >> 4) & 0x03) + 4,
			((imm >> 6) & 0x03) + 4,
		};

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, shuffle));
	}

	RValue<Float4> UnpackLow(RValue<Float4> x, RValue<Float4> y)
	{
		int shuffle[4] = {0, 4, 1, 5};
		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, shuffle));
	}

	RValue<Float4> UnpackHigh(RValue<Float4> x, RValue<Float4> y)
	{
		int shuffle[4] = {2, 6, 3, 7};
		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, shuffle));
	}

	RValue<Float4> Mask(Float4 &lhs, RValue<Float4> rhs, unsigned char select)
	{
		Value *vector = lhs.loadValue();
		Value *shuffle = createMask4(vector, rhs.value, select);
		lhs.storeValue(shuffle);

		return RValue<Float4>(shuffle);
	}

	RValue<Int> SignMask(RValue<Float4> x)
	{
		return x86::movmskps(x);
	}

	RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpeqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpneqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpONE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpnltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y)
	{
	//	return As<Int4>(x86::cmpnleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGT(x.value, y.value), Int4::getType()));
	}

	RValue<Float4> Round(RValue<Float4> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundps(x, 0);
		}
		else
		{
			return Float4(RoundInt(x));
		}
	}

	RValue<Float4> Trunc(RValue<Float4> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundps(x, 3);
		}
		else
		{
			return Float4(Int4(x));   // Rounded toward zero
		}
	}

	RValue<Float4> Frac(RValue<Float4> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x - x86::floorps(x);
		}
		else
		{
			Float4 frc = x - Float4(Int4(x));   // Signed fractional part

			return frc + As<Float4>(As<Int4>(CmpNLE(Float4(0.0f), frc)) & As<Int4>(Float4(1, 1, 1, 1)));
		}
	}

	RValue<Float4> Floor(RValue<Float4> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::floorps(x);
		}
		else
		{
			return x - Frac(x);
		}
	}

	RValue<Float4> Ceil(RValue<Float4> x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::ceilps(x);
		}
		else
		{
			return -Floor(-x);
		}
	}

	Type *Float4::getType()
	{
		return T(VectorType::get(Float::getType(), 4));
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, int offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, Byte::getType(), V(Nucleus::createConstantInt(offset))));
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<Int> offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, Byte::getType(), offset.value));
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<UInt> offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, Byte::getType(), offset.value));
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, int offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, RValue<Int> offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, RValue<UInt> offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, int offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<Int> offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<UInt> offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, int offset)
	{
		return lhs = lhs - offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, RValue<Int> offset)
	{
		return lhs = lhs - offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, RValue<UInt> offset)
	{
		return lhs = lhs - offset;
	}

	void Return()
	{
		Nucleus::createRetVoid();
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
		Nucleus::createUnreachable();
	}

	void Return(bool ret)
	{
		Nucleus::createRet(V(Nucleus::createConstantBool(ret)));
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
		Nucleus::createUnreachable();
	}

	void Return(const Int &ret)
	{
		Nucleus::createRet(ret.loadValue());
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
		Nucleus::createUnreachable();
	}

	BasicBlock *beginLoop()
	{
		BasicBlock *loopBB = Nucleus::createBasicBlock();

		Nucleus::createBr(loopBB);
		Nucleus::setInsertBlock(loopBB);

		return loopBB;
	}

	bool branch(RValue<Bool> cmp, BasicBlock *bodyBB, BasicBlock *endBB)
	{
		Nucleus::createCondBr(cmp.value, bodyBB, endBB);
		Nucleus::setInsertBlock(bodyBB);

		return true;
	}

	bool elseBlock(BasicBlock *falseBB)
	{
		falseBB->back().eraseFromParent();
		Nucleus::setInsertBlock(falseBB);

		return true;
	}

	RValue<Long> Ticks()
	{
		llvm::Function *rdtsc = Intrinsic::getDeclaration(::module, Intrinsic::readcyclecounter);

		return RValue<Long>(V(::builder->CreateCall(rdtsc)));
	}
}

namespace sw
{
	namespace x86
	{
		RValue<Int> cvtss2si(RValue<Float> val)
		{
			llvm::Function *cvtss2si = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_cvtss2si);

			Float4 vector;
			vector.x = val;

			return RValue<Int>(V(::builder->CreateCall(cvtss2si, RValue<Float4>(vector).value)));
		}

		RValue<Int2> cvtps2pi(RValue<Float4> val)
		{
			llvm::Function *cvtps2pi = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_cvtps2pi);

			return RValue<Int2>(V(::builder->CreateCall(cvtps2pi, val.value)));
		}

		RValue<Int2> cvttps2pi(RValue<Float4> val)
		{
			llvm::Function *cvttps2pi = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_cvttps2pi);

			return RValue<Int2>(V(::builder->CreateCall(cvttps2pi, val.value)));
		}

		RValue<Int4> cvtps2dq(RValue<Float4> val)
		{
			if(CPUID::supportsSSE2())
			{
				llvm::Function *cvtps2dq = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_cvtps2dq);

				return RValue<Int4>(V(::builder->CreateCall(cvtps2dq, val.value)));
			}
			else
			{
				Int2 lo = x86::cvtps2pi(val);
				Int2 hi = x86::cvtps2pi(Swizzle(val, 0xEE));

				return Int4(lo, hi);
			}
		}

		RValue<Float> rcpss(RValue<Float> val)
		{
			llvm::Function *rcpss = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_rcp_ss);

			Value *vector = Nucleus::createInsertElement(V(UndefValue::get(Float4::getType())), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(rcpss, vector)), Float::getType(), 0));
		}

		RValue<Float> sqrtss(RValue<Float> val)
		{
			llvm::Function *sqrtss = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_sqrt_ss);

			Value *vector = Nucleus::createInsertElement(V(UndefValue::get(Float4::getType())), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(sqrtss, vector)), Float::getType(), 0));
		}

		RValue<Float> rsqrtss(RValue<Float> val)
		{
			llvm::Function *rsqrtss = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_rsqrt_ss);

			Value *vector = Nucleus::createInsertElement(V(UndefValue::get(Float4::getType())), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(rsqrtss, vector)), Float::getType(), 0));
		}

		RValue<Float4> rcpps(RValue<Float4> val)
		{
			llvm::Function *rcpps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_rcp_ps);

			return RValue<Float4>(V(::builder->CreateCall(rcpps, val.value)));
		}

		RValue<Float4> sqrtps(RValue<Float4> val)
		{
			llvm::Function *sqrtps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_sqrt_ps);

			return RValue<Float4>(V(::builder->CreateCall(sqrtps, val.value)));
		}

		RValue<Float4> rsqrtps(RValue<Float4> val)
		{
			llvm::Function *rsqrtps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_rsqrt_ps);

			return RValue<Float4>(V(::builder->CreateCall(rsqrtps, val.value)));
		}

		RValue<Float4> maxps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *maxps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_max_ps);

			return RValue<Float4>(V(::builder->CreateCall2(maxps, x.value, y.value)));
		}

		RValue<Float4> minps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *minps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_min_ps);

			return RValue<Float4>(V(::builder->CreateCall2(minps, x.value, y.value)));
		}

		RValue<Float> roundss(RValue<Float> val, unsigned char imm)
		{
			llvm::Function *roundss = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_round_ss);

			Value *undef = V(UndefValue::get(Float4::getType()));
			Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall3(roundss, undef, vector, V(Nucleus::createConstantInt(imm)))), Float::getType(), 0));
		}

		RValue<Float> floorss(RValue<Float> val)
		{
			return roundss(val, 1);
		}

		RValue<Float> ceilss(RValue<Float> val)
		{
			return roundss(val, 2);
		}

		RValue<Float4> roundps(RValue<Float4> val, unsigned char imm)
		{
			llvm::Function *roundps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_round_ps);

			return RValue<Float4>(V(::builder->CreateCall2(roundps, val.value, V(Nucleus::createConstantInt(imm)))));
		}

		RValue<Float4> floorps(RValue<Float4> val)
		{
			return roundps(val, 1);
		}

		RValue<Float4> ceilps(RValue<Float4> val)
		{
			return roundps(val, 2);
		}

		RValue<Float4> cmpps(RValue<Float4> x, RValue<Float4> y, unsigned char imm)
		{
			llvm::Function *cmpps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_cmp_ps);

			return RValue<Float4>(V(::builder->CreateCall3(cmpps, x.value, y.value, V(Nucleus::createConstantByte(imm)))));
		}

		RValue<Float4> cmpeqps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 0);
		}

		RValue<Float4> cmpltps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 1);
		}

		RValue<Float4> cmpleps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 2);
		}

		RValue<Float4> cmpunordps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 3);
		}

		RValue<Float4> cmpneqps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 4);
		}

		RValue<Float4> cmpnltps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 5);
		}

		RValue<Float4> cmpnleps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 6);
		}

		RValue<Float4> cmpordps(RValue<Float4> x, RValue<Float4> y)
		{
			return cmpps(x, y, 7);
		}

		RValue<Float> cmpss(RValue<Float> x, RValue<Float> y, unsigned char imm)
		{
			llvm::Function *cmpss = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_cmp_ss);

			Value *vector1 = Nucleus::createInsertElement(V(UndefValue::get(Float4::getType())), x.value, 0);
			Value *vector2 = Nucleus::createInsertElement(V(UndefValue::get(Float4::getType())), y.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall3(cmpss, vector1, vector2, V(Nucleus::createConstantByte(imm)))), Float::getType(), 0));
		}

		RValue<Float> cmpeqss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 0);
		}

		RValue<Float> cmpltss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 1);
		}

		RValue<Float> cmpless(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 2);
		}

		RValue<Float> cmpunordss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 3);
		}

		RValue<Float> cmpneqss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 4);
		}

		RValue<Float> cmpnltss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 5);
		}

		RValue<Float> cmpnless(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 6);
		}

		RValue<Float> cmpordss(RValue<Float> x, RValue<Float> y)
		{
			return cmpss(x, y, 7);
		}

		RValue<Int4> pabsd(RValue<Int4> x)
		{
			llvm::Function *pabsd = Intrinsic::getDeclaration(::module, Intrinsic::x86_ssse3_pabs_d_128);

			return RValue<Int4>(V(::builder->CreateCall(pabsd, x.value)));
		}

		RValue<Short4> paddsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *paddsw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_padds_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(paddsw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> psubsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *psubsw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psubs_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psubsw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UShort4> paddusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *paddusw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_paddus_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(paddusw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UShort4> psubusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *psubusw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psubus_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(psubusw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<SByte8> paddsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			llvm::Function *paddsb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_padds_b);

			return As<SByte8>(RValue<MMX>(V(::builder->CreateCall2(paddsb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<SByte8> psubsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			llvm::Function *psubsb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psubs_b);

			return As<SByte8>(RValue<MMX>(V(::builder->CreateCall2(psubsb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> paddusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *paddusb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_paddus_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(paddusb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> psubusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *psubusb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psubus_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(psubusb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> paddw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *paddw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_padd_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(paddw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> psubw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *psubw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psub_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psubw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pmullw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmullw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmull_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pmullw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pand(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pand = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pand);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pand, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> por(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *por = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_por);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(por, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pxor(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pxor = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pxor);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pxor, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pshufw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *pshufw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_pshuf_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pshufw, As<MMX>(x).value, V(Nucleus::createConstantByte(y))))));
		}

		RValue<Int2> punpcklwd(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *punpcklwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpcklwd);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(punpcklwd, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> punpckhwd(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *punpckhwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpckhwd);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(punpckhwd, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pinsrw(RValue<Short4> x, RValue<Int> y, unsigned int i)
		{
			llvm::Function *pinsrw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pinsr_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall3(pinsrw, As<MMX>(x).value, y.value, V(Nucleus::createConstantInt(i))))));
		}

		RValue<Int> pextrw(RValue<Short4> x, unsigned int i)
		{
			llvm::Function *pextrw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pextr_w);

			return RValue<Int>(V(::builder->CreateCall2(pextrw, As<MMX>(x).value, V(Nucleus::createConstantInt(i)))));
		}

		RValue<Long1> punpckldq(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *punpckldq = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpckldq);

			return As<Long1>(RValue<MMX>(V(::builder->CreateCall2(punpckldq, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Long1> punpckhdq(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *punpckhdq = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpckhdq);

			return As<Long1>(RValue<MMX>(V(::builder->CreateCall2(punpckhdq, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> punpcklbw(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *punpcklbw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpcklbw);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(punpcklbw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> punpckhbw(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *punpckhbw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_punpckhbw);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(punpckhbw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> paddb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *paddb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_padd_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(paddb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> psubb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *psubb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psub_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(psubb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> paddd(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *paddd = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_padd_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(paddd, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> psubd(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *psubd = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psub_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(psubd, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UShort4> pavgw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *pavgw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pavg_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(pavgw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pmaxsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmaxsw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmaxs_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pmaxsw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pminsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pminsw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmins_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pminsw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pcmpgtw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pcmpgtw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pcmpgt_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pcmpgtw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> pcmpeqw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pcmpeqw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pcmpeq_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pcmpeqw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> pcmpgtb(RValue<SByte8> x, RValue<SByte8> y)
		{
			llvm::Function *pcmpgtb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pcmpgt_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(pcmpgtb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> pcmpeqb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *pcmpeqb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pcmpeq_b);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(pcmpeqb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> packssdw(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *packssdw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_packssdw);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(packssdw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short8> packssdw(RValue<Int4> x, RValue<Int4> y)
		{
			if(CPUID::supportsSSE2())
			{
				llvm::Function *packssdw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_packssdw_128);

				return RValue<Short8>(V(::builder->CreateCall2(packssdw, x.value, y.value)));
			}
			else
			{
				Int2 loX = Int2(x);
				Int2 hiX = Int2(Swizzle(x, 0xEE));

				Int2 loY = Int2(y);
				Int2 hiY = Int2(Swizzle(y, 0xEE));

				Short4 lo = x86::packssdw(loX, hiX);
				Short4 hi = x86::packssdw(loY, hiY);

				return Short8(lo, hi);
			}
		}

		RValue<SByte8> packsswb(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *packsswb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_packsswb);

			return As<SByte8>(RValue<MMX>(V(::builder->CreateCall2(packsswb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Byte8> packuswb(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *packuswb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_packuswb);

			return As<Byte8>(RValue<MMX>(V(::builder->CreateCall2(packuswb, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UShort8> packusdw(RValue<UInt4> x, RValue<UInt4> y)
		{
			if(CPUID::supportsSSE4_1())
			{
				llvm::Function *packusdw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_packusdw);

				return RValue<UShort8>(V(::builder->CreateCall2(packusdw, x.value, y.value)));
			}
			else
			{
				// FIXME: Not an exact replacement!
				return As<UShort8>(packssdw(As<Int4>(x - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000)), As<Int4>(y - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000))) + Short8(0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u));
			}
		}

		RValue<UShort4> psrlw(RValue<UShort4> x, unsigned char y)
		{
			llvm::Function *psrlw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrli_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(psrlw, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<UShort8> psrlw(RValue<UShort8> x, unsigned char y)
		{
			llvm::Function *psrlw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_psrli_w);

			return RValue<UShort8>(V(::builder->CreateCall2(psrlw, x.value, V(Nucleus::createConstantInt(y)))));
		}

		RValue<Short4> psraw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psraw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrai_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psraw, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psraw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psraw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_psrai_w);

			return RValue<Short8>(V(::builder->CreateCall2(psraw, x.value, V(Nucleus::createConstantInt(y)))));
		}

		RValue<Short4> psllw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psllw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pslli_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psllw, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psllw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psllw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_pslli_w);

			return RValue<Short8>(V(::builder->CreateCall2(psllw, x.value, V(Nucleus::createConstantInt(y)))));
		}

		RValue<Int2> pslld(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *pslld = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pslli_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(pslld, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> pslld(RValue<Int4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				llvm::Function *pslld = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_pslli_d);

				return RValue<Int4>(V(::builder->CreateCall2(pslld, x.value, V(Nucleus::createConstantInt(y)))));
			}
			else
			{
				Int2 lo = Int2(x);
				Int2 hi = Int2(Swizzle(x, 0xEE));

				lo = x86::pslld(lo, y);
				hi = x86::pslld(hi, y);

				return Int4(lo, hi);
			}
		}

		RValue<Int2> psrad(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *psrad = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrai_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(psrad, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> psrad(RValue<Int4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				llvm::Function *psrad = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_psrai_d);

				return RValue<Int4>(V(::builder->CreateCall2(psrad, x.value, V(Nucleus::createConstantInt(y)))));
			}
			else
			{
				Int2 lo = Int2(x);
				Int2 hi = Int2(Swizzle(x, 0xEE));

				lo = x86::psrad(lo, y);
				hi = x86::psrad(hi, y);

				return Int4(lo, hi);
			}
		}

		RValue<UInt2> psrld(RValue<UInt2> x, unsigned char y)
		{
			llvm::Function *psrld = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrli_d);

			return As<UInt2>(RValue<MMX>(V(::builder->CreateCall2(psrld, As<MMX>(x).value, V(Nucleus::createConstantInt(y))))));
		}

		RValue<UInt4> psrld(RValue<UInt4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				llvm::Function *psrld = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_psrli_d);

				return RValue<UInt4>(V(::builder->CreateCall2(psrld, x.value, V(Nucleus::createConstantInt(y)))));
			}
			else
			{
				UInt2 lo = As<UInt2>(Int2(As<Int4>(x)));
				UInt2 hi = As<UInt2>(Int2(Swizzle(As<Int4>(x), 0xEE)));

				lo = x86::psrld(lo, y);
				hi = x86::psrld(hi, y);

				return UInt4(lo, hi);
			}
		}

		RValue<UShort4> psrlw(RValue<UShort4> x, RValue<Long1> y)
		{
			llvm::Function *psrlw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrl_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(psrlw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> psraw(RValue<Short4> x, RValue<Long1> y)
		{
			llvm::Function *psraw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psra_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psraw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short4> psllw(RValue<Short4> x, RValue<Long1> y)
		{
			llvm::Function *psllw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psll_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(psllw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> pslld(RValue<Int2> x, RValue<Long1> y)
		{
			llvm::Function *pslld = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psll_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(pslld, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UInt2> psrld(RValue<UInt2> x, RValue<Long1> y)
		{
			llvm::Function *psrld = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psrl_d);

			return As<UInt2>(RValue<MMX>(V(::builder->CreateCall2(psrld, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> psrad(RValue<Int2> x, RValue<Long1> y)
		{
			llvm::Function *psrld = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_psra_d);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(psrld, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int4> pmaxsd(RValue<Int4> x, RValue<Int4> y)
		{
			llvm::Function *pmaxsd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmaxsd);

			return RValue<Int4>(V(::builder->CreateCall2(pmaxsd, x.value, y.value)));
		}

		RValue<Int4> pminsd(RValue<Int4> x, RValue<Int4> y)
		{
			llvm::Function *pminsd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pminsd);

			return RValue<Int4>(V(::builder->CreateCall2(pminsd, x.value, y.value)));
		}

		RValue<UInt4> pmaxud(RValue<UInt4> x, RValue<UInt4> y)
		{
			llvm::Function *pmaxud = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmaxud);

			return RValue<UInt4>(V(::builder->CreateCall2(pmaxud, x.value, y.value)));
		}

		RValue<UInt4> pminud(RValue<UInt4> x, RValue<UInt4> y)
		{
			llvm::Function *pminud = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pminud);

			return RValue<UInt4>(V(::builder->CreateCall2(pminud, x.value, y.value)));
		}

		RValue<Short4> pmulhw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmulhw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmulh_w);

			return As<Short4>(RValue<MMX>(V(::builder->CreateCall2(pmulhw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<UShort4> pmulhuw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmulhu_w);

			return As<UShort4>(RValue<MMX>(V(::builder->CreateCall2(pmulhuw, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Int2> pmaddwd(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmadd_wd);

			return As<Int2>(RValue<MMX>(V(::builder->CreateCall2(pmaddwd, As<MMX>(x).value, As<MMX>(y).value))));
		}

		RValue<Short8> pmulhw(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmulhw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_pmulh_w);

			return RValue<Short8>(V(::builder->CreateCall2(pmulhw, x.value, y.value)));
		}

		RValue<UShort8> pmulhuw(RValue<UShort8> x, RValue<UShort8> y)
		{
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_pmulhu_w);

			return RValue<UShort8>(V(::builder->CreateCall2(pmulhuw, x.value, y.value)));
		}

		RValue<Int4> pmaddwd(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse2_pmadd_wd);

			return RValue<Int4>(V(::builder->CreateCall2(pmaddwd, x.value, y.value)));
		}

		RValue<Int> movmskps(RValue<Float4> x)
		{
			llvm::Function *movmskps = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse_movmsk_ps);

			return RValue<Int>(V(::builder->CreateCall(movmskps, x.value)));
		}

		RValue<Int> pmovmskb(RValue<Byte8> x)
		{
			llvm::Function *pmovmskb = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_pmovmskb);

			return RValue<Int>(V(::builder->CreateCall(pmovmskb, As<MMX>(x).value)));
		}

		//RValue<Int2> movd(RValue<Pointer<Int>> x)
		//{
		//	Value *element = Nucleus::createLoad(x.value);

		////	Value *int2 = UndefValue::get(Int2::getType());
		////	int2 = Nucleus::createInsertElement(int2, element, ConstantInt::get(Int::getType(), 0));

		//	Value *int2 = Nucleus::createBitCast(Nucleus::createZExt(element, Long::getType()), Int2::getType());

		//	return RValue<Int2>(int2);
		//}

		//RValue<Int2> movdq2q(RValue<Int4> x)
		//{
		//	Value *long2 = Nucleus::createBitCast(x.value, Long2::getType());
		//	Value *element = Nucleus::createExtractElement(long2, ConstantInt::get(Int::getType(), 0));

		//	return RValue<Int2>(Nucleus::createBitCast(element, Int2::getType()));
		//}

		RValue<Int4> pmovzxbd(RValue<Int4> x)
		{
			llvm::Function *pmovzxbd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmovzxbd);

			return RValue<Int4>(V(::builder->CreateCall(pmovzxbd, Nucleus::createBitCast(x.value, Byte16::getType()))));
		}

		RValue<Int4> pmovsxbd(RValue<Int4> x)
		{
			llvm::Function *pmovsxbd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmovsxbd);

			return RValue<Int4>(V(::builder->CreateCall(pmovsxbd, Nucleus::createBitCast(x.value, SByte16::getType()))));
		}

		RValue<Int4> pmovzxwd(RValue<Int4> x)
		{
			llvm::Function *pmovzxwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmovzxwd);

			return RValue<Int4>(V(::builder->CreateCall(pmovzxwd, Nucleus::createBitCast(x.value, UShort8::getType()))));
		}

		RValue<Int4> pmovsxwd(RValue<Int4> x)
		{
			llvm::Function *pmovsxwd = Intrinsic::getDeclaration(::module, Intrinsic::x86_sse41_pmovsxwd);

			return RValue<Int4>(V(::builder->CreateCall(pmovsxwd, Nucleus::createBitCast(x.value, Short8::getType()))));
		}

		void emms()
		{
			llvm::Function *emms = Intrinsic::getDeclaration(::module, Intrinsic::x86_mmx_emms);

			V(::builder->CreateCall(emms));
		}
	}
}
