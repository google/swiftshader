// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

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

#include "Routine.hpp"
#include "RoutineManager.hpp"
#include "x86.hpp"
#include "CPUID.hpp"
#include "Thread.hpp"
#include "Memory.hpp"

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

namespace sw
{
	Optimization optimization[10] = {InstructionCombining, Disabled};

	using namespace llvm;

	RoutineManager *Nucleus::routineManager = 0;
	ExecutionEngine *Nucleus::executionEngine = 0;
	Builder *Nucleus::builder = 0;
	LLVMContext *Nucleus::context = 0;
	Module *Nucleus::module = 0;
	llvm::Function *Nucleus::function = 0;
	BackoffLock Nucleus::codegenMutex;

	class Builder : public IRBuilder<>
	{
	};

	Nucleus::Nucleus()
	{
		codegenMutex.lock();   // Reactor and LLVM are currently not thread safe

		InitializeNativeTarget();
		JITEmitDebugInfo = false;

		if(!context)
		{
			context = new LLVMContext();
		}

		module = new Module("", *context);
		routineManager = new RoutineManager();

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
		TargetMachine *targetMachine = EngineBuilder::selectTarget(module, architecture, "", MAttrs, Reloc::Default, CodeModel::JITDefault, &error);
		executionEngine = JIT::createJIT(module, 0, routineManager, CodeGenOpt::Aggressive, true, targetMachine);

		if(!builder)
		{
			builder = static_cast<Builder*>(new IRBuilder<>(*context));

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
		delete executionEngine;
		executionEngine = 0;

		routineManager = 0;
		function = 0;
		module = 0;

		codegenMutex.unlock();
	}

	Routine *Nucleus::acquireRoutine(const wchar_t *name, bool runOptimizations)
	{
		if(builder->GetInsertBlock()->empty() || !builder->GetInsertBlock()->back().isTerminator())
		{
			Type *type = function->getReturnType();

			if(type->isVoidTy())
			{
				createRetVoid();
			}
			else
			{
				createRet(UndefValue::get(type));
			}
		}

		if(false)
		{
			std::string error;
			raw_fd_ostream file("llvm-dump-unopt.txt", error);
			module->print(file, 0);
		}

		if(runOptimizations)
		{
			optimize();
		}

		if(false)
		{
			std::string error;
			raw_fd_ostream file("llvm-dump-opt.txt", error);
			module->print(file, 0);
		}

		void *entry = executionEngine->getPointerToFunction(function);
		Routine *routine = routineManager->acquireRoutine(entry);

		if(CodeAnalystLogJITCode)
		{
			CodeAnalystLogJITCode(routine->getEntry(), routine->getCodeSize(), name);
		}

		return routine;
	}

	void Nucleus::optimize()
	{
		static PassManager *passManager = 0;

		if(!passManager)
		{
			passManager = new PassManager();

			UnsafeFPMath = true;
		//	NoInfsFPMath = true;
		//	NoNaNsFPMath = true;

			passManager->add(new TargetData(*executionEngine->getTargetData()));
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

		passManager->run(*module);
	}

	void Nucleus::setFunction(llvm::Function *function)
	{
		Nucleus::function = function;

		builder->SetInsertPoint(BasicBlock::Create(*context, "", function));
	}

	Module *Nucleus::getModule()
	{
		return module;
	}

	llvm::Function *Nucleus::getFunction()
	{
		return function;
	}

	llvm::LLVMContext *Nucleus::getContext()
	{
		return context;
	}

	Value *Nucleus::allocateStackVariable(Type *type, int arraySize)
	{
		// Need to allocate it in the entry block for mem2reg to work
		llvm::Function *function = getFunction();
		BasicBlock &entryBlock = function->getEntryBlock();

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

		return declaration;
	}

	BasicBlock *Nucleus::createBasicBlock()
	{
		return BasicBlock::Create(*context, "", Nucleus::getFunction());
	}

	BasicBlock *Nucleus::getInsertBlock()
	{
		return builder->GetInsertBlock();
	}

	void Nucleus::setInsertBlock(BasicBlock *basicBlock)
	{
	//	assert(builder->GetInsertBlock()->back().isTerminator());
		return builder->SetInsertPoint(basicBlock);
	}

	BasicBlock *Nucleus::getPredecessor(BasicBlock *basicBlock)
	{
		return *pred_begin(basicBlock);
	}

	llvm::Function *Nucleus::createFunction(llvm::Type *ReturnType, std::vector<llvm::Type*> &Params)
	{
		llvm::FunctionType *functionType = llvm::FunctionType::get(ReturnType, Params, false);
		llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "", Nucleus::getModule());
		function->setCallingConv(llvm::CallingConv::C);

		return function;
	}

	llvm::Value *Nucleus::getArgument(llvm::Function *function, unsigned int index)
	{
		llvm::Function::arg_iterator args = function->arg_begin();

		while(index)
		{
			args++;
			index--;
		}

		return &*args;
	}

	Value *Nucleus::createRetVoid()
	{
		x86::emms();

		return builder->CreateRetVoid();
	}

	Value *Nucleus::createRet(Value *V)
	{
		x86::emms();

		return builder->CreateRet(V);
	}

	Value *Nucleus::createBr(BasicBlock *dest)
	{
		return builder->CreateBr(dest);
	}

	Value *Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
	{
		return builder->CreateCondBr(cond, ifTrue, ifFalse);
	}

	Value *Nucleus::createAdd(Value *lhs, Value *rhs)
	{
		return builder->CreateAdd(lhs, rhs);
	}

	Value *Nucleus::createSub(Value *lhs, Value *rhs)
	{
		return builder->CreateSub(lhs, rhs);
	}

	Value *Nucleus::createMul(Value *lhs, Value *rhs)
	{
		return builder->CreateMul(lhs, rhs);
	}

	Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
	{
		return builder->CreateUDiv(lhs, rhs);
	}

	Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
	{
		return builder->CreateSDiv(lhs, rhs);
	}

	Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
	{
		return builder->CreateFAdd(lhs, rhs);
	}

	Value *Nucleus::createFSub(Value *lhs, Value *rhs)
	{
		return builder->CreateFSub(lhs, rhs);
	}

	Value *Nucleus::createFMul(Value *lhs, Value *rhs)
	{
		return builder->CreateFMul(lhs, rhs);
	}

	Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
	{
		return builder->CreateFDiv(lhs, rhs);
	}

	Value *Nucleus::createURem(Value *lhs, Value *rhs)
	{
		return builder->CreateURem(lhs, rhs);
	}

	Value *Nucleus::createSRem(Value *lhs, Value *rhs)
	{
		return builder->CreateSRem(lhs, rhs);
	}

	Value *Nucleus::createFRem(Value *lhs, Value *rhs)
	{
		return builder->CreateFRem(lhs, rhs);
	}

	Value *Nucleus::createShl(Value *lhs, Value *rhs)
	{
		return builder->CreateShl(lhs, rhs);
	}

	Value *Nucleus::createLShr(Value *lhs, Value *rhs)
	{
		return builder->CreateLShr(lhs, rhs);
	}

	Value *Nucleus::createAShr(Value *lhs, Value *rhs)
	{
		return builder->CreateAShr(lhs, rhs);
	}

	Value *Nucleus::createAnd(Value *lhs, Value *rhs)
	{
		return builder->CreateAnd(lhs, rhs);
	}

	Value *Nucleus::createOr(Value *lhs, Value *rhs)
	{
		return builder->CreateOr(lhs, rhs);
	}

	Value *Nucleus::createXor(Value *lhs, Value *rhs)
	{
		return builder->CreateXor(lhs, rhs);
	}

	Value *Nucleus::createNeg(Value *V)
	{
		return builder->CreateNeg(V);
	}

	Value *Nucleus::createFNeg(Value *V)
	{
		return builder->CreateFNeg(V);
	}

	Value *Nucleus::createNot(Value *V)
	{
		return builder->CreateNot(V);
	}

	Value *Nucleus::createLoad(Value *ptr, bool isVolatile, unsigned int align)
	{
		return builder->Insert(new LoadInst(ptr, "", isVolatile, align));
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, bool isVolatile, unsigned int align)
	{
		return builder->Insert(new StoreInst(value, ptr, isVolatile, align));
	}

	Value *Nucleus::createGEP(Value *ptr, Value *index)
	{
		return builder->CreateGEP(ptr, index);
	}

	Value *Nucleus::createAtomicAdd(Value *ptr, Value *value)
	{
		return builder->CreateAtomicRMW(AtomicRMWInst::Add, ptr, value, SequentiallyConsistent);
	}

	Value *Nucleus::createTrunc(Value *V, Type *destType)
	{
		return builder->CreateTrunc(V, destType);
	}

	Value *Nucleus::createZExt(Value *V, Type *destType)
	{
		return builder->CreateZExt(V, destType);
	}

	Value *Nucleus::createSExt(Value *V, Type *destType)
	{
		return builder->CreateSExt(V, destType);
	}

	Value *Nucleus::createFPToUI(Value *V, Type *destType)
	{
		return builder->CreateFPToUI(V, destType);
	}

	Value *Nucleus::createFPToSI(Value *V, Type *destType)
	{
		return builder->CreateFPToSI(V, destType);
	}

	Value *Nucleus::createUIToFP(Value *V, Type *destType)
	{
		return builder->CreateUIToFP(V, destType);
	}

	Value *Nucleus::createSIToFP(Value *V, Type *destType)
	{
		return builder->CreateSIToFP(V, destType);
	}

	Value *Nucleus::createFPTrunc(Value *V, Type *destType)
	{
		return builder->CreateFPTrunc(V, destType);
	}

	Value *Nucleus::createFPExt(Value *V, Type *destType)
	{
		return builder->CreateFPExt(V, destType);
	}

	Value *Nucleus::createPtrToInt(Value *V, Type *destType)
	{
		return builder->CreatePtrToInt(V, destType);
	}

	Value *Nucleus::createIntToPtr(Value *V, Type *destType)
	{
		return builder->CreateIntToPtr(V, destType);
	}

	Value *Nucleus::createBitCast(Value *V, Type *destType)
	{
		return builder->CreateBitCast(V, destType);
	}

	Value *Nucleus::createIntCast(Value *V, Type *destType, bool isSigned)
	{
		return builder->CreateIntCast(V, destType, isSigned);
	}

	Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpEQ(lhs, rhs);
	}

	Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpNE(lhs, rhs);
	}

	Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpUGT(lhs, rhs);
	}

	Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpUGE(lhs, rhs);
	}

	Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpULT(lhs, rhs);
	}

	Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpULE(lhs, rhs);
	}

	Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpSGT(lhs, rhs);
	}

	Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpSGE(lhs, rhs);
	}

	Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpSLT(lhs, rhs);
	}

	Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
	{
		return builder->CreateICmpSLE(lhs, rhs);
	}

	Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpOEQ(lhs, rhs);
	}

	Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpOGT(lhs, rhs);
	}

	Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpOGE(lhs, rhs);
	}

	Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpOLT(lhs, rhs);
	}

	Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpOLE(lhs, rhs);
	}

	Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpONE(lhs, rhs);
	}

	Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpORD(lhs, rhs);
	}

	Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpUNO(lhs, rhs);
	}

	Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpUEQ(lhs, rhs);
	}

	Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpUGT(lhs, rhs);
	}

	Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpUGE(lhs, rhs);
	}

	Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpULT(lhs, rhs);
	}

	Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpULE(lhs, rhs);
	}

	Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
	{
		return builder->CreateFCmpULE(lhs, rhs);
	}

	Value *Nucleus::createCall(Value *callee)
	{
		return builder->CreateCall(callee);
	}

	Value *Nucleus::createCall(Value *callee, Value *arg)
	{
		return builder->CreateCall(callee, arg);
	}

	Value *Nucleus::createCall(Value *callee, Value *arg1, Value *arg2)
	{
		return builder->CreateCall2(callee, arg1, arg2);
	}

	Value *Nucleus::createCall(Value *callee, Value *arg1, Value *arg2, Value *arg3)
	{
		return builder->CreateCall3(callee, arg1, arg2, arg3);
	}

	Value *Nucleus::createCall(Value *callee, Value *arg1, Value *arg2, Value *arg3, Value *arg4)
	{
		return builder->CreateCall4(callee, arg1, arg2, arg3, arg4);
	}

	Value *Nucleus::createExtractElement(Value *vector, int index)
	{
		return builder->CreateExtractElement(vector, createConstantInt(index));
	}

	Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
	{
		return builder->CreateInsertElement(vector, element, createConstantInt(index));
	}

	Value *Nucleus::createShuffleVector(Value *V1, Value *V2, Value *mask)
	{
		return builder->CreateShuffleVector(V1, V2, mask);
	}

	Value *Nucleus::createSelect(Value *C, Value *ifTrue, Value *ifFalse)
	{
		return builder->CreateSelect(C, ifTrue, ifFalse);
	}

	Value *Nucleus::createSwitch(llvm::Value *V, llvm::BasicBlock *Dest, unsigned NumCases)
	{
		return builder->CreateSwitch(V, Dest, NumCases);
	}

	void Nucleus::addSwitchCase(llvm::Value *Switch, int Case, llvm::BasicBlock *Branch)
	{
		static_cast<SwitchInst*>(Switch)->addCase(Nucleus::createConstantInt(Case), Branch);
	}

	Value *Nucleus::createUnreachable()
	{
		return builder->CreateUnreachable();
	}

	Value *Nucleus::createSwizzle(Value *val, unsigned char select)
	{
		Constant *swizzle[4];
		swizzle[0] = Nucleus::createConstantInt((select >> 0) & 0x03);
		swizzle[1] = Nucleus::createConstantInt((select >> 2) & 0x03);
		swizzle[2] = Nucleus::createConstantInt((select >> 4) & 0x03);
		swizzle[3] = Nucleus::createConstantInt((select >> 6) & 0x03);

		Value *shuffle = Nucleus::createShuffleVector(val, UndefValue::get(val->getType()), Nucleus::createConstantVector(swizzle, 4));

		return shuffle;
	}

	Value *Nucleus::createMask(Value *lhs, Value *rhs, unsigned char select)
	{
		bool mask[4] = {false, false, false, false};

		mask[(select >> 0) & 0x03] = true;
		mask[(select >> 2) & 0x03] = true;
		mask[(select >> 4) & 0x03] = true;
		mask[(select >> 6) & 0x03] = true;

		Constant *swizzle[4];
		swizzle[0] = Nucleus::createConstantInt(mask[0] ? 4 : 0);
		swizzle[1] = Nucleus::createConstantInt(mask[1] ? 5 : 1);
		swizzle[2] = Nucleus::createConstantInt(mask[2] ? 6 : 2);
		swizzle[3] = Nucleus::createConstantInt(mask[3] ? 7 : 3);

		Value *shuffle = Nucleus::createShuffleVector(lhs, rhs, Nucleus::createConstantVector(swizzle, 4));

		return shuffle;
	}

	const llvm::GlobalValue *Nucleus::getGlobalValueAtAddress(void *Addr)
	{
		return executionEngine->getGlobalValueAtAddress(Addr);
	}

	void Nucleus::addGlobalMapping(const llvm::GlobalValue *GV, void *Addr)
	{
		executionEngine->addGlobalMapping(GV, Addr);
	}

	llvm::GlobalValue *Nucleus::createGlobalValue(llvm::Type *Ty, bool isConstant, unsigned int Align)
	{
		llvm::GlobalValue *global = new llvm::GlobalVariable(*Nucleus::getModule(), Ty, isConstant, llvm::GlobalValue::ExternalLinkage, 0, "");
		global->setAlignment(Align);

		return global;
	}

	llvm::Type *Nucleus::getPointerType(llvm::Type *ElementType)
	{
		return llvm::PointerType::get(ElementType, 0);
	}

	llvm::Constant *Nucleus::createNullValue(llvm::Type *Ty)
	{
		return llvm::Constant::getNullValue(Ty);
	}

	llvm::ConstantInt *Nucleus::createConstantInt(int64_t i)
	{
		return llvm::ConstantInt::get(Type::getInt64Ty(*context), i, true);
	}

	llvm::ConstantInt *Nucleus::createConstantInt(int i)
	{
		return llvm::ConstantInt::get(Type::getInt32Ty(*context), i, true);
	}

	llvm::ConstantInt *Nucleus::createConstantInt(unsigned int i)
	{
		return llvm::ConstantInt::get(Type::getInt32Ty(*context), i, false);
	}

	llvm::ConstantInt *Nucleus::createConstantBool(bool b)
	{
		return llvm::ConstantInt::get(Type::getInt1Ty(*context), b);
	}

	llvm::ConstantInt *Nucleus::createConstantByte(signed char i)
	{
		return llvm::ConstantInt::get(Type::getInt8Ty(*context), i, true);
	}

	llvm::ConstantInt *Nucleus::createConstantByte(unsigned char i)
	{
		return llvm::ConstantInt::get(Type::getInt8Ty(*context), i, false);
	}

	llvm::ConstantInt *Nucleus::createConstantShort(short i)
	{
		return llvm::ConstantInt::get(Type::getInt16Ty(*context), i, true);
	}

	llvm::ConstantInt *Nucleus::createConstantShort(unsigned short i)
	{
		return llvm::ConstantInt::get(Type::getInt16Ty(*context), i, false);
	}

	llvm::Constant *Nucleus::createConstantFloat(float x)
	{
		return ConstantFP::get(Float::getType(), x);
	}

	llvm::Value *Nucleus::createNullPointer(llvm::Type *Ty)
	{
		return llvm::ConstantPointerNull::get(llvm::PointerType::get(Ty, 0));
	}

	llvm::Value *Nucleus::createConstantVector(llvm::Constant *const *Vals, unsigned NumVals)
	{
		return llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(Vals, NumVals));
	}

	Type *Void::getType()
	{
		return Type::getVoidTy(*Nucleus::getContext());
	}

	LValue::LValue(llvm::Type *type, int arraySize)
	{
		address = Nucleus::allocateStackVariable(type, arraySize);
	}

	llvm::Value *LValue::loadValue(unsigned int alignment) const
	{
		return Nucleus::createLoad(address, false, alignment);
	}

	llvm::Value *LValue::storeValue(llvm::Value *value, unsigned int alignment) const
	{
		return Nucleus::createStore(value, address, false, alignment);
	}

	llvm::Value *LValue::getAddress(llvm::Value *index) const
	{
		return Nucleus::createGEP(address, index);
	}

	Type *MMX::getType()
	{
		return Type::getX86_MMXTy(*Nucleus::getContext());
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
		return Type::getInt1Ty(*Nucleus::getContext());
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantByte((unsigned char)1));
		val.storeValue(inc);

		return res;
	}

	const Byte &operator++(const Byte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantByte((unsigned char)1));
		val.storeValue(inc);

		return val;
	}

	RValue<Byte> operator--(const Byte &val, int)   // Post-decrement
	{
		RValue<Byte> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantByte((unsigned char)1));
		val.storeValue(inc);

		return res;
	}

	const Byte &operator--(const Byte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantByte((unsigned char)1));
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
		return Type::getInt8Ty(*Nucleus::getContext());
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantByte((signed char)1));
		val.storeValue(inc);

		return res;
	}

	const SByte &operator++(const SByte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantByte((signed char)1));
		val.storeValue(inc);

		return val;
	}

	RValue<SByte> operator--(const SByte &val, int)   // Post-decrement
	{
		RValue<SByte> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantByte((signed char)1));
		val.storeValue(inc);

		return res;
	}

	const SByte &operator--(const SByte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantByte((signed char)1));
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
		return Type::getInt8Ty(*Nucleus::getContext());
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantShort((short)1));
		val.storeValue(inc);

		return res;
	}

	const Short &operator++(const Short &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantShort((short)1));
		val.storeValue(inc);

		return val;
	}

	RValue<Short> operator--(const Short &val, int)   // Post-decrement
	{
		RValue<Short> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantShort((short)1));
		val.storeValue(inc);

		return res;
	}

	const Short &operator--(const Short &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantShort((short)1));
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
		return Type::getInt16Ty(*Nucleus::getContext());
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantShort((unsigned short)1));
		val.storeValue(inc);

		return res;
	}

	const UShort &operator++(const UShort &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantShort((unsigned short)1));
		val.storeValue(inc);

		return val;
	}

	RValue<UShort> operator--(const UShort &val, int)   // Post-decrement
	{
		RValue<UShort> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantShort((unsigned short)1));
		val.storeValue(inc);

		return res;
	}

	const UShort &operator--(const UShort &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantShort((unsigned short)1));
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
		return Type::getInt16Ty(*Nucleus::getContext());
	}

	Type *Byte4::getType()
	{
		#if 0
			return VectorType::get(Byte::getType(), 4);
		#else
			return UInt::getType();   // FIXME: LLVM doesn't manipulate it as one 32-bit block
		#endif
	}

	Type *SByte4::getType()
	{
		#if 0
			return VectorType::get(SByte::getType(), 4);
		#else
			return Int::getType();   // FIXME: LLVM doesn't manipulate it as one 32-bit block
		#endif
	}

	Byte8::Byte8()
	{
	//	xyzw.parent = this;
	}

	Byte8::Byte8(byte x0, byte x1, byte x2, byte x3, byte x4, byte x5, byte x6, byte x7)
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
		Value *vector = Nucleus::createConstantVector(constantVector, 8);

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
		Value *vector = Nucleus::createConstantVector(constantVector, 8);

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
		Value *int2 = Nucleus::createInsertElement(UndefValue::get(VectorType::get(Int::getType(), 2)), x.value, 0);
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
			Constant *shuffle[8];
			shuffle[0] = Nucleus::createConstantInt(0);
			shuffle[1] = Nucleus::createConstantInt(8);
			shuffle[2] = Nucleus::createConstantInt(1);
			shuffle[3] = Nucleus::createConstantInt(9);
			shuffle[4] = Nucleus::createConstantInt(2);
			shuffle[5] = Nucleus::createConstantInt(10);
			shuffle[6] = Nucleus::createConstantInt(3);
			shuffle[7] = Nucleus::createConstantInt(11);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 8));

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
			Constant *shuffle[8];
			shuffle[0] = Nucleus::createConstantInt(4);
			shuffle[1] = Nucleus::createConstantInt(12);
			shuffle[2] = Nucleus::createConstantInt(5);
			shuffle[3] = Nucleus::createConstantInt(13);
			shuffle[4] = Nucleus::createConstantInt(6);
			shuffle[5] = Nucleus::createConstantInt(14);
			shuffle[6] = Nucleus::createConstantInt(7);
			shuffle[7] = Nucleus::createConstantInt(15);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 8));

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
			return VectorType::get(Byte::getType(), 8);
		}
	}

	SByte8::SByte8()
	{
	//	xyzw.parent = this;
	}

	SByte8::SByte8(byte x0, byte x1, byte x2, byte x3, byte x4, byte x5, byte x6, byte x7)
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
		Value *vector = Nucleus::createConstantVector(constantVector, 8);

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
		Value *vector = Nucleus::createConstantVector(constantVector, 8);

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
			Constant *shuffle[8];
			shuffle[0] = Nucleus::createConstantInt(0);
			shuffle[1] = Nucleus::createConstantInt(8);
			shuffle[2] = Nucleus::createConstantInt(1);
			shuffle[3] = Nucleus::createConstantInt(9);
			shuffle[4] = Nucleus::createConstantInt(2);
			shuffle[5] = Nucleus::createConstantInt(10);
			shuffle[6] = Nucleus::createConstantInt(3);
			shuffle[7] = Nucleus::createConstantInt(11);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 8));

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
			Constant *shuffle[8];
			shuffle[0] = Nucleus::createConstantInt(4);
			shuffle[1] = Nucleus::createConstantInt(12);
			shuffle[2] = Nucleus::createConstantInt(5);
			shuffle[3] = Nucleus::createConstantInt(13);
			shuffle[4] = Nucleus::createConstantInt(6);
			shuffle[5] = Nucleus::createConstantInt(14);
			shuffle[6] = Nucleus::createConstantInt(7);
			shuffle[7] = Nucleus::createConstantInt(15);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 8));

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
			return VectorType::get(SByte::getType(), 8);
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
		return VectorType::get(Byte::getType(), 16);
	}

	Type *SByte16::getType()
	{
		return VectorType::get(SByte::getType(), 16);
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
				Constant *pshuflw[8];
				pshuflw[0] = Nucleus::createConstantInt(0);
				pshuflw[1] = Nucleus::createConstantInt(2);
				pshuflw[2] = Nucleus::createConstantInt(0);
				pshuflw[3] = Nucleus::createConstantInt(2);
				pshuflw[4] = Nucleus::createConstantInt(4);
				pshuflw[5] = Nucleus::createConstantInt(5);
				pshuflw[6] = Nucleus::createConstantInt(6);
				pshuflw[7] = Nucleus::createConstantInt(7);

				Constant *pshufhw[8];
				pshufhw[0] = Nucleus::createConstantInt(0);
				pshufhw[1] = Nucleus::createConstantInt(1);
				pshufhw[2] = Nucleus::createConstantInt(2);
				pshufhw[3] = Nucleus::createConstantInt(3);
				pshufhw[4] = Nucleus::createConstantInt(4);
				pshufhw[5] = Nucleus::createConstantInt(6);
				pshufhw[6] = Nucleus::createConstantInt(4);
				pshufhw[7] = Nucleus::createConstantInt(6);

				Value *shuffle1 = Nucleus::createShuffleVector(short8, UndefValue::get(Short8::getType()), Nucleus::createConstantVector(pshuflw, 8));
				Value *shuffle2 = Nucleus::createShuffleVector(shuffle1, UndefValue::get(Short8::getType()), Nucleus::createConstantVector(pshufhw, 8));
				Value *int4 = Nucleus::createBitCast(shuffle2, Int4::getType());
				packed = Nucleus::createSwizzle(int4, 0x88);
			}
			else
			{
				Constant *pshufb[16];
				pshufb[0] = Nucleus::createConstantInt(0);
				pshufb[1] = Nucleus::createConstantInt(1);
				pshufb[2] = Nucleus::createConstantInt(4);
				pshufb[3] = Nucleus::createConstantInt(5);
				pshufb[4] = Nucleus::createConstantInt(8);
				pshufb[5] = Nucleus::createConstantInt(9);
				pshufb[6] = Nucleus::createConstantInt(12);
				pshufb[7] = Nucleus::createConstantInt(13);
				pshufb[8] = Nucleus::createConstantInt(0);
				pshufb[9] = Nucleus::createConstantInt(1);
				pshufb[10] = Nucleus::createConstantInt(4);
				pshufb[11] = Nucleus::createConstantInt(5);
				pshufb[12] = Nucleus::createConstantInt(8);
				pshufb[13] = Nucleus::createConstantInt(9);
				pshufb[14] = Nucleus::createConstantInt(12);
				pshufb[15] = Nucleus::createConstantInt(13);

				Value *byte16 = Nucleus::createBitCast(cast.value, Byte16::getType());
				packed = Nucleus::createShuffleVector(byte16, UndefValue::get(Byte16::getType()), Nucleus::createConstantVector(pshufb, 16));
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
		Value *vector = Nucleus::createConstantVector(constantVector, 4);

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
		Value *vector = Nucleus::createConstantVector(constantVector, 4);

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
			Constant *shuffle[4];
			shuffle[0] = Nucleus::createConstantInt(0);
			shuffle[1] = Nucleus::createConstantInt(4);
			shuffle[2] = Nucleus::createConstantInt(1);
			shuffle[3] = Nucleus::createConstantInt(5);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4));

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
			Constant *shuffle[4];
			shuffle[0] = Nucleus::createConstantInt(2);
			shuffle[1] = Nucleus::createConstantInt(6);
			shuffle[2] = Nucleus::createConstantInt(3);
			shuffle[3] = Nucleus::createConstantInt(7);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4));

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
			return RValue<Short4>(Nucleus::createSwizzle(x.value, select));
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
			return RValue<Short>(Nucleus::createExtractElement(val.value, i));
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
			return VectorType::get(Short::getType(), 4);
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

	UShort4::UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
	{
	//	xyzw.parent = this;

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(x);
		constantVector[1] = Nucleus::createConstantShort(y);
		constantVector[2] = Nucleus::createConstantShort(z);
		constantVector[3] = Nucleus::createConstantShort(w);
		Value *vector = Nucleus::createConstantVector(constantVector, 4);

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
			return VectorType::get(UShort::getType(), 4);
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

	Short8::Short8(RValue<Short4> lo, RValue<Short4> hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
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
		return VectorType::get(Short::getType(), 8);
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

	UShort8::UShort8(RValue<UShort4> lo, RValue<UShort4> hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
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
		Constant *pshufb[16];
		pshufb[0] = Nucleus::createConstantInt(select0 + 0);
		pshufb[1] = Nucleus::createConstantInt(select0 + 1);
		pshufb[2] = Nucleus::createConstantInt(select1 + 0);
		pshufb[3] = Nucleus::createConstantInt(select1 + 1);
		pshufb[4] = Nucleus::createConstantInt(select2 + 0);
		pshufb[5] = Nucleus::createConstantInt(select2 + 1);
		pshufb[6] = Nucleus::createConstantInt(select3 + 0);
		pshufb[7] = Nucleus::createConstantInt(select3 + 1);
		pshufb[8] = Nucleus::createConstantInt(select4 + 0);
		pshufb[9] = Nucleus::createConstantInt(select4 + 1);
		pshufb[10] = Nucleus::createConstantInt(select5 + 0);
		pshufb[11] = Nucleus::createConstantInt(select5 + 1);
		pshufb[12] = Nucleus::createConstantInt(select6 + 0);
		pshufb[13] = Nucleus::createConstantInt(select6 + 1);
		pshufb[14] = Nucleus::createConstantInt(select7 + 0);
		pshufb[15] = Nucleus::createConstantInt(select7 + 1);

		Value *byte16 = Nucleus::createBitCast(x.value, Byte16::getType());
		Value *shuffle = Nucleus::createShuffleVector(byte16, UndefValue::get(Byte16::getType()), Nucleus::createConstantVector(pshufb, 16));
		Value *short8 = Nucleus::createBitCast(shuffle, UShort8::getType());

		return RValue<UShort8>(short8);
	}

	RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)
	{
		return x86::pmulhuw(x, y);   // FIXME: Fallback required
	}

	// FIXME: Implement as Shuffle(x, y, Select(i0, ..., i16)) and Shuffle(x, y, SELECT_PACK_REPEAT(element))
//	RValue<UShort8> PackRepeat(RValue<Byte16> x, RValue<Byte16> y, int element)
//	{
//		Constant *pshufb[16];
//		pshufb[0] = Nucleus::createConstantInt(element + 0);
//		pshufb[1] = Nucleus::createConstantInt(element + 0);
//		pshufb[2] = Nucleus::createConstantInt(element + 4);
//		pshufb[3] = Nucleus::createConstantInt(element + 4);
//		pshufb[4] = Nucleus::createConstantInt(element + 8);
//		pshufb[5] = Nucleus::createConstantInt(element + 8);
//		pshufb[6] = Nucleus::createConstantInt(element + 12);
//		pshufb[7] = Nucleus::createConstantInt(element + 12);
//		pshufb[8] = Nucleus::createConstantInt(element + 16);
//		pshufb[9] = Nucleus::createConstantInt(element + 16);
//		pshufb[10] = Nucleus::createConstantInt(element + 20);
//		pshufb[11] = Nucleus::createConstantInt(element + 20);
//		pshufb[12] = Nucleus::createConstantInt(element + 24);
//		pshufb[13] = Nucleus::createConstantInt(element + 24);
//		pshufb[14] = Nucleus::createConstantInt(element + 28);
//		pshufb[15] = Nucleus::createConstantInt(element + 28);
//
//		Value *shuffle = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(pshufb, 16));
//		Value *short8 = Nucleus::createBitCast(shuffle, UShort8::getType());
//
//		return RValue<UShort8>(short8);
//	}

	Type *UShort8::getType()
	{
		return VectorType::get(UShort::getType(), 8);
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator++(const Int &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> operator--(const Int &val, int)   // Post-decrement
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator--(const Int &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
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
		return Type::getInt32Ty(*Nucleus::getContext());
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
		return Type::getInt64Ty(*Nucleus::getContext());
	}

	Long1::Long1(const RValue<UInt> cast)
	{
		Value *undefCast = Nucleus::createInsertElement(UndefValue::get(VectorType::get(Int::getType(), 2)), cast.value, 0);
		Value *zeroCast = Nucleus::createInsertElement(undefCast, Nucleus::createConstantInt(0), 1);

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
			return VectorType::get(Long::getType(), 1);
		}
	}

	RValue<Long2> UnpackHigh(RValue<Long2> x, RValue<Long2> y)
	{
		Constant *shuffle[2];
		shuffle[0] = Nucleus::createConstantInt(1);
		shuffle[1] = Nucleus::createConstantInt(3);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

		return RValue<Long2>(packed);
	}

	Type *Long2::getType()
	{
		return VectorType::get(Long::getType(), 2);
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
		Value *integer = Nucleus::createFPToUI(cast.value, UInt::getType());

		storeValue(integer);
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

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator++(const UInt &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<UInt> operator--(const UInt &val, int)   // Post-decrement
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator--(const UInt &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
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
		return Type::getInt32Ty(*Nucleus::getContext());
	}

//	Int2::Int2(RValue<Int> cast)
//	{
//		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
//		Value *vector = Nucleus::createBitCast(extend, Int2::getType());
//
//		Constant *shuffle[2];
//		shuffle[0] = Nucleus::createConstantInt(0);
//		shuffle[1] = Nucleus::createConstantInt(0);
//
//		Value *replicate = Nucleus::createShuffleVector(vector, UndefValue::get(Int2::getType()), Nucleus::createConstantVector(shuffle, 2));
//
//		storeValue(replicate);
//	}

	Int2::Int2(RValue<Int4> cast)
	{
		Value *long2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *element = Nucleus::createExtractElement(long2, 0);
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
		Value *vector = Nucleus::createConstantVector(constantVector, 2);

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
			Constant *shuffle[2];
			shuffle[0] = Nucleus::createConstantInt(0);
			shuffle[1] = Nucleus::createConstantInt(1);

			Value *packed = Nucleus::createShuffleVector(Nucleus::createBitCast(lo.value, VectorType::get(Int::getType(), 1)), Nucleus::createBitCast(hi.value, VectorType::get(Int::getType(), 1)), Nucleus::createConstantVector(shuffle, 2));

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
			Constant *shuffle[2];
			shuffle[0] = Nucleus::createConstantInt(0);
			shuffle[1] = Nucleus::createConstantInt(2);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

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
			Constant *shuffle[2];
			shuffle[0] = Nucleus::createConstantInt(1);
			shuffle[1] = Nucleus::createConstantInt(3);

			Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

			return RValue<Long1>(Nucleus::createBitCast(packed, Long1::getType()));
		}
	}

	RValue<Int> Extract(RValue<Int2> val, int i)
	{
		if(false)   // FIXME: LLVM does not generate optimal code
		{
			return RValue<Int>(Nucleus::createExtractElement(val.value, i));
		}
		else
		{
			if(i == 0)
			{
				return RValue<Int>(Nucleus::createExtractElement(Nucleus::createBitCast(val.value, VectorType::get(Int::getType(), 2)), 0));
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
		return RValue<Int2>(Nucleus::createBitCast(Nucleus::createInsertElement(Nucleus::createBitCast(val.value, VectorType::get(Int::getType(), 2)), element.value, i), Int2::getType()));
	}

	Type *Int2::getType()
	{
		if(CPUID::supportsMMX2())
		{
			return MMX::getType();
		}
		else
		{
			return VectorType::get(Int::getType(), 2);
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
		Value *vector = Nucleus::createConstantVector(constantVector, 2);

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
			return VectorType::get(UInt::getType(), 2);
		}
	}

	Int4::Int4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *xyzw = Nucleus::createFPToSI(cast.value, Int4::getType());

		storeValue(xyzw);
	}

	Int4::Int4(RValue<Short4> cast)
	{
		Value *long2 = UndefValue::get(Long2::getType());
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

			Constant *swizzle[8];
			swizzle[0] = Nucleus::createConstantInt(0);
			swizzle[1] = Nucleus::createConstantInt(0);
			swizzle[2] = Nucleus::createConstantInt(1);
			swizzle[3] = Nucleus::createConstantInt(1);
			swizzle[4] = Nucleus::createConstantInt(2);
			swizzle[5] = Nucleus::createConstantInt(2);
			swizzle[6] = Nucleus::createConstantInt(3);
			swizzle[7] = Nucleus::createConstantInt(3);

			Value *c = Nucleus::createShuffleVector(b, b, Nucleus::createConstantVector(swizzle, 8));
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
		Value *long2 = UndefValue::get(Long2::getType());
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

			Constant *swizzle[8];
			swizzle[0] = Nucleus::createConstantInt(0);
			swizzle[1] = Nucleus::createConstantInt(8);
			swizzle[2] = Nucleus::createConstantInt(1);
			swizzle[3] = Nucleus::createConstantInt(9);
			swizzle[4] = Nucleus::createConstantInt(2);
			swizzle[5] = Nucleus::createConstantInt(10);
			swizzle[6] = Nucleus::createConstantInt(3);
			swizzle[7] = Nucleus::createConstantInt(11);

			Value *c = Nucleus::createShuffleVector(b, Nucleus::createNullValue(Short8::getType()), Nucleus::createConstantVector(swizzle, 8));
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
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *int4 = Nucleus::createBitCast(long2, Int4::getType());

		storeValue(int4);
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
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGE(x.value, y.value), Int4::getType()));
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
		return RValue<Int>(Nucleus::createExtractElement(x.value, i));
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
		return RValue<Int4>(Nucleus::createSwizzle(x.value, select));
	}

	Type *Int4::getType()
	{
		return VectorType::get(Int::getType(), 4);
	}

	UInt4::UInt4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *xyzw = Nucleus::createFPToUI(cast.value, UInt4::getType());

		storeValue(xyzw);
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

		Value *long2 = UndefValue::get(Long2::getType());
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
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULT(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGE(x.value, y.value), Int4::getType()));
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
		return VectorType::get(UInt::getType(), 4);
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
		return Type::getFloatTy(*Nucleus::getContext());
	}

	Float2::Float2(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *int64x2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *int64 = Nucleus::createExtractElement(int64x2, 0);
		Value *float2 = Nucleus::createBitCast(int64, Float2::getType());

		storeValue(float2);
	}

	Type *Float2::getType()
	{
		return VectorType::get(Float::getType(), 2);
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

			Value *i8y = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(1));
			Value *f32y = Nucleus::createUIToFP(i8y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, Nucleus::createConstantInt(1));

			Value *i8z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createUIToFP(i8z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i8w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createUIToFP(i8w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *x = Nucleus::createBitCast(cast.value, Int::getType());
			Value *a = Nucleus::createInsertElement(UndefValue::get(Int4::getType()), x, 0);

			Value *e;

			if(CPUID::supportsSSE4_1())
			{
				e = x86::pmovzxbd(RValue<Int4>(a)).value;
			}
			else
			{
				Constant *swizzle[16];
				swizzle[0] = Nucleus::createConstantInt(0);
				swizzle[1] = Nucleus::createConstantInt(16);
				swizzle[2] = Nucleus::createConstantInt(1);
				swizzle[3] = Nucleus::createConstantInt(17);
				swizzle[4] = Nucleus::createConstantInt(2);
				swizzle[5] = Nucleus::createConstantInt(18);
				swizzle[6] = Nucleus::createConstantInt(3);
				swizzle[7] = Nucleus::createConstantInt(19);
				swizzle[8] = Nucleus::createConstantInt(4);
				swizzle[9] = Nucleus::createConstantInt(20);
				swizzle[10] = Nucleus::createConstantInt(5);
				swizzle[11] = Nucleus::createConstantInt(21);
				swizzle[12] = Nucleus::createConstantInt(6);
				swizzle[13] = Nucleus::createConstantInt(22);
				swizzle[14] = Nucleus::createConstantInt(7);
				swizzle[15] = Nucleus::createConstantInt(23);

				Value *b = Nucleus::createBitCast(a, Byte16::getType());
				Value *c = Nucleus::createShuffleVector(b, Nucleus::createNullValue(Byte16::getType()), Nucleus::createConstantVector(swizzle, 16));

				Constant *swizzle2[8];
				swizzle2[0] = Nucleus::createConstantInt(0);
				swizzle2[1] = Nucleus::createConstantInt(8);
				swizzle2[2] = Nucleus::createConstantInt(1);
				swizzle2[3] = Nucleus::createConstantInt(9);
				swizzle2[4] = Nucleus::createConstantInt(2);
				swizzle2[5] = Nucleus::createConstantInt(10);
				swizzle2[6] = Nucleus::createConstantInt(3);
				swizzle2[7] = Nucleus::createConstantInt(11);

				Value *d = Nucleus::createBitCast(c, Short8::getType());
				e = Nucleus::createShuffleVector(d, Nucleus::createNullValue(Short8::getType()), Nucleus::createConstantVector(swizzle2, 8));
			}

			Value *f = Nucleus::createBitCast(e, Int4::getType());
			Value *g = Nucleus::createSIToFP(f, Float4::getType());
			Value *xyzw = g;
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

			Value *i8y = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(1));
			Value *f32y = Nucleus::createSIToFP(i8y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, Nucleus::createConstantInt(1));

			Value *i8z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createSIToFP(i8z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i8w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createSIToFP(i8w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *x = Nucleus::createBitCast(cast.value, Int::getType());
			Value *a = Nucleus::createInsertElement(UndefValue::get(Int4::getType()), x, 0);

			Value *g;

			if(CPUID::supportsSSE4_1())
			{
				g = x86::pmovsxbd(RValue<Int4>(a)).value;
			}
			else
			{
				Constant *swizzle[16];
				swizzle[0] = Nucleus::createConstantInt(0);
				swizzle[1] = Nucleus::createConstantInt(0);
				swizzle[2] = Nucleus::createConstantInt(1);
				swizzle[3] = Nucleus::createConstantInt(1);
				swizzle[4] = Nucleus::createConstantInt(2);
				swizzle[5] = Nucleus::createConstantInt(2);
				swizzle[6] = Nucleus::createConstantInt(3);
				swizzle[7] = Nucleus::createConstantInt(3);
				swizzle[8] = Nucleus::createConstantInt(4);
				swizzle[9] = Nucleus::createConstantInt(4);
				swizzle[10] = Nucleus::createConstantInt(5);
				swizzle[11] = Nucleus::createConstantInt(5);
				swizzle[12] = Nucleus::createConstantInt(6);
				swizzle[13] = Nucleus::createConstantInt(6);
				swizzle[14] = Nucleus::createConstantInt(7);
				swizzle[15] = Nucleus::createConstantInt(7);

				Value *b = Nucleus::createBitCast(a, Byte16::getType());
				Value *c = Nucleus::createShuffleVector(b, b, Nucleus::createConstantVector(swizzle, 16));

				Constant *swizzle2[8];
				swizzle2[0] = Nucleus::createConstantInt(0);
				swizzle2[1] = Nucleus::createConstantInt(0);
				swizzle2[2] = Nucleus::createConstantInt(1);
				swizzle2[3] = Nucleus::createConstantInt(1);
				swizzle2[4] = Nucleus::createConstantInt(2);
				swizzle2[5] = Nucleus::createConstantInt(2);
				swizzle2[6] = Nucleus::createConstantInt(3);
				swizzle2[7] = Nucleus::createConstantInt(3);

				Value *d = Nucleus::createBitCast(c, Short8::getType());
				Value *e = Nucleus::createShuffleVector(d, d, Nucleus::createConstantVector(swizzle2, 8));

				Value *f = Nucleus::createBitCast(e, Int4::getType());
			//	g = Nucleus::createAShr(f, Nucleus::createConstantInt(24));
				g = x86::psrad(RValue<Int4>(f), 24).value;
			}

			Value *xyzw = Nucleus::createSIToFP(g, Float4::getType());
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

		Constant *swizzle[4];
		swizzle[0] = Nucleus::createConstantInt(0);
		swizzle[1] = Nucleus::createConstantInt(0);
		swizzle[2] = Nucleus::createConstantInt(0);
		swizzle[3] = Nucleus::createConstantInt(0);

		Value *replicate = Nucleus::createShuffleVector(insert, UndefValue::get(Float4::getType()), Nucleus::createConstantVector(swizzle, 4));

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

		Value *result = Nucleus::createAnd(vector, Nucleus::createConstantVector(constantVector, 4));

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
		llvm::Value *value = val.loadValue();
		llvm::Value *insert = Nucleus::createInsertElement(value, element.value, i);

		val = RValue<Float4>(insert);

		return val;
	}

	RValue<Float> Extract(RValue<Float4> x, int i)
	{
		return RValue<Float>(Nucleus::createExtractElement(x.value, i));
	}

	RValue<Float4> Swizzle(RValue<Float4> x, unsigned char select)
	{
		return RValue<Float4>(Nucleus::createSwizzle(x.value, select));
	}

	RValue<Float4> ShuffleLowHigh(RValue<Float4> x, RValue<Float4> y, unsigned char imm)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(((imm >> 0) & 0x03) + 0);
		shuffle[1] = Nucleus::createConstantInt(((imm >> 2) & 0x03) + 0);
		shuffle[2] = Nucleus::createConstantInt(((imm >> 4) & 0x03) + 4);
		shuffle[3] = Nucleus::createConstantInt(((imm >> 6) & 0x03) + 4);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}

	RValue<Float4> UnpackLow(RValue<Float4> x, RValue<Float4> y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(0);
		shuffle[1] = Nucleus::createConstantInt(4);
		shuffle[2] = Nucleus::createConstantInt(1);
		shuffle[3] = Nucleus::createConstantInt(5);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}

	RValue<Float4> UnpackHigh(RValue<Float4> x, RValue<Float4> y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(2);
		shuffle[1] = Nucleus::createConstantInt(6);
		shuffle[2] = Nucleus::createConstantInt(3);
		shuffle[3] = Nucleus::createConstantInt(7);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}

	RValue<Float4> Mask(Float4 &lhs, RValue<Float4> rhs, unsigned char select)
	{
		Value *vector = lhs.loadValue();
		Value *shuffle = Nucleus::createMask(vector, rhs.value, select);
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
		return VectorType::get(Float::getType(), 4);
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, int offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, Nucleus::createConstantInt(offset)));
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<Int> offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, offset.value));
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<UInt> offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, offset.value));
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
		Nucleus::createRet(Nucleus::createConstantBool(ret));
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
		Module *module = Nucleus::getModule();
		llvm::Function *rdtsc = Intrinsic::getDeclaration(module, Intrinsic::readcyclecounter);

		return RValue<Long>(Nucleus::createCall(rdtsc));
	}
}

namespace sw
{
	namespace x86
	{
		RValue<Int> cvtss2si(RValue<Float> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvtss2si = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvtss2si);

			Float4 vector;
			vector.x = val;

			return RValue<Int>(Nucleus::createCall(cvtss2si, RValue<Float4>(vector).value));
		}

		RValue<Int2> cvtps2pi(RValue<Float4> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvtps2pi = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvtps2pi);

			return RValue<Int2>(Nucleus::createCall(cvtps2pi, val.value));
		}

		RValue<Int2> cvttps2pi(RValue<Float4> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvttps2pi = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvttps2pi);

			return RValue<Int2>(Nucleus::createCall(cvttps2pi, val.value));
		}

		RValue<Int4> cvtps2dq(RValue<Float4> val)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *cvtps2dq = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_cvtps2dq);

				return RValue<Int4>(Nucleus::createCall(cvtps2dq, val.value));
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
			Module *module = Nucleus::getModule();
			llvm::Function *rcpss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rcp_ss);

			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(rcpss, vector), 0));
		}

		RValue<Float> sqrtss(RValue<Float> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *sqrtss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_sqrt_ss);

			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(sqrtss, vector), 0));
		}

		RValue<Float> rsqrtss(RValue<Float> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rsqrtss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rsqrt_ss);

			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(rsqrtss, vector), 0));
		}

		RValue<Float4> rcpps(RValue<Float4> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rcpps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rcp_ps);

			return RValue<Float4>(Nucleus::createCall(rcpps, val.value));
		}

		RValue<Float4> sqrtps(RValue<Float4> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *sqrtps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_sqrt_ps);

			return RValue<Float4>(Nucleus::createCall(sqrtps, val.value));
		}

		RValue<Float4> rsqrtps(RValue<Float4> val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rsqrtps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rsqrt_ps);

			return RValue<Float4>(Nucleus::createCall(rsqrtps, val.value));
		}

		RValue<Float4> maxps(RValue<Float4> x, RValue<Float4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *maxps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_max_ps);

			return RValue<Float4>(Nucleus::createCall(maxps, x.value, y.value));
		}

		RValue<Float4> minps(RValue<Float4> x, RValue<Float4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *minps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_min_ps);

			return RValue<Float4>(Nucleus::createCall(minps, x.value, y.value));
		}

		RValue<Float> roundss(RValue<Float> val, unsigned char imm)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *roundss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_round_ss);

			Value *undef = UndefValue::get(Float4::getType());
			Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(roundss, undef, vector, Nucleus::createConstantInt(imm)), 0));
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
			Module *module = Nucleus::getModule();
			llvm::Function *roundps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_round_ps);

			return RValue<Float4>(Nucleus::createCall(roundps, val.value, Nucleus::createConstantInt(imm)));
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
			Module *module = Nucleus::getModule();
			llvm::Function *cmpps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cmp_ps);

			return RValue<Float4>(Nucleus::createCall(cmpps, x.value, y.value, Nucleus::createConstantByte(imm)));
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
			Module *module = Nucleus::getModule();
			llvm::Function *cmpss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cmp_ss);

			Value *vector1 = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), x.value, 0);
			Value *vector2 = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), y.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(cmpss, vector1, vector2, Nucleus::createConstantByte(imm)), 0));
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
			Module *module = Nucleus::getModule();
			llvm::Function *pabsd = Intrinsic::getDeclaration(module, Intrinsic::x86_ssse3_pabs_d_128);

			return RValue<Int4>(Nucleus::createCall(pabsd, x.value));
		}

		RValue<Short4> paddsw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padds_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(paddsw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> psubsw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubs_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psubsw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UShort4> paddusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddusw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_paddus_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(paddusw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UShort4> psubusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubusw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubus_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(psubusw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<SByte8> paddsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddsb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padds_b);

			return As<SByte8>(RValue<MMX>(Nucleus::createCall(paddsb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<SByte8> psubsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubsb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubs_b);

			return As<SByte8>(RValue<MMX>(Nucleus::createCall(psubsb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> paddusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddusb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_paddus_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(paddusb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> psubusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubusb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubus_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(psubusb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> paddw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padd_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(paddw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> psubw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psub_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psubw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pmullw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmullw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmull_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pmullw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pand(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pand = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pand);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pand, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> por(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *por = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_por);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(por, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pxor(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pxor = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pxor);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pxor, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pshufw(RValue<Short4> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pshufw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_pshuf_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pshufw, As<MMX>(x).value, Nucleus::createConstantByte(y))));
		}

		RValue<Int2> punpcklwd(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpcklwd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpcklwd);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(punpcklwd, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> punpckhwd(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpckhwd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpckhwd);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(punpckhwd, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pinsrw(RValue<Short4> x, RValue<Int> y, unsigned int i)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pinsrw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pinsr_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pinsrw, As<MMX>(x).value, y.value, Nucleus::createConstantInt(i))));
		}

		RValue<Int> pextrw(RValue<Short4> x, unsigned int i)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pextrw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pextr_w);

			return RValue<Int>(Nucleus::createCall(pextrw, As<MMX>(x).value, Nucleus::createConstantInt(i)));
		}

		RValue<Long1> punpckldq(RValue<Int2> x, RValue<Int2> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpckldq = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpckldq);

			return As<Long1>(RValue<MMX>(Nucleus::createCall(punpckldq, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Long1> punpckhdq(RValue<Int2> x, RValue<Int2> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpckhdq = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpckhdq);

			return As<Long1>(RValue<MMX>(Nucleus::createCall(punpckhdq, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> punpcklbw(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpcklbw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpcklbw);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(punpcklbw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> punpckhbw(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *punpckhbw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_punpckhbw);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(punpckhbw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> paddb(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padd_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(paddb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> psubb(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psub_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(psubb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> paddd(RValue<Int2> x, RValue<Int2> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padd_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(paddd, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> psubd(RValue<Int2> x, RValue<Int2> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psub_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(psubd, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UShort4> pavgw(RValue<UShort4> x, RValue<UShort4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pavgw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pavg_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(pavgw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pmaxsw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaxsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmaxs_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pmaxsw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pminsw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pminsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmins_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pminsw,  As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pcmpgtw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpgtw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpgt_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pcmpgtw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> pcmpeqw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpeqw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpeq_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pcmpeqw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> pcmpgtb(RValue<SByte8> x, RValue<SByte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpgtb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpgt_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(pcmpgtb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> pcmpeqb(RValue<Byte8> x, RValue<Byte8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpeqb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpeq_b);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(pcmpeqb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> packssdw(RValue<Int2> x, RValue<Int2> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *packssdw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packssdw);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(packssdw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short8> packssdw(RValue<Int4> x, RValue<Int4> y)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *packssdw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_packssdw_128);

				return RValue<Short8>(Nucleus::createCall(packssdw, x.value, y.value));
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
			Module *module = Nucleus::getModule();
			llvm::Function *packsswb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packsswb);

			return As<SByte8>(RValue<MMX>(Nucleus::createCall(packsswb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Byte8> packuswb(RValue<UShort4> x, RValue<UShort4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *packuswb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packuswb);

			return As<Byte8>(RValue<MMX>(Nucleus::createCall(packuswb, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UShort8> packusdw(RValue<UInt4> x, RValue<UInt4> y)
		{
			if(CPUID::supportsSSE4_1())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *packusdw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_packusdw);

				return RValue<UShort8>(Nucleus::createCall(packusdw, x.value, y.value));
			}
			else
			{
				// FIXME: Not an exact replacement!
				return As<UShort8>(packssdw(As<Int4>(x - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000)), As<Int4>(y - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000))) + Short8(0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u, 0x8000u));
			}
		}

		RValue<UShort4> psrlw(RValue<UShort4> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrli_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(psrlw, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<UShort8> psrlw(RValue<UShort8> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrli_w);

			return RValue<UShort8>(Nucleus::createCall(psrlw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short4> psraw(RValue<Short4> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrai_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psraw, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<Short8> psraw(RValue<Short8> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrai_w);

			return RValue<Short8>(Nucleus::createCall(psraw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short4> psllw(RValue<Short4> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pslli_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psllw, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<Short8> psllw(RValue<Short8> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pslli_w);

			return RValue<Short8>(Nucleus::createCall(psllw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Int2> pslld(RValue<Int2> x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pslld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pslli_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(pslld, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<Int4> pslld(RValue<Int4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *pslld = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pslli_d);

				return RValue<Int4>(Nucleus::createCall(pslld, x.value, Nucleus::createConstantInt(y)));
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
			Module *module = Nucleus::getModule();
			llvm::Function *psrad = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrai_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(psrad, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<Int4> psrad(RValue<Int4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *psrad = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrai_d);

				return RValue<Int4>(Nucleus::createCall(psrad, x.value, Nucleus::createConstantInt(y)));
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
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrli_d);

			return As<UInt2>(RValue<MMX>(Nucleus::createCall(psrld, As<MMX>(x).value, Nucleus::createConstantInt(y))));
		}

		RValue<UInt4> psrld(RValue<UInt4> x, unsigned char y)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrli_d);

				return RValue<UInt4>(Nucleus::createCall(psrld, x.value, Nucleus::createConstantInt(y)));
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
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrl_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(psrlw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> psraw(RValue<Short4> x, RValue<Long1> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psra_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psraw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short4> psllw(RValue<Short4> x, RValue<Long1> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psll_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(psllw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> pslld(RValue<Int2> x, RValue<Long1> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pslld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psll_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(pslld, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UInt2> psrld(RValue<UInt2> x, RValue<Long1> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrl_d);

			return As<UInt2>(RValue<MMX>(Nucleus::createCall(psrld, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> psrad(RValue<Int2> x, RValue<Long1> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psra_d);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(psrld, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int4> pmaxsd(RValue<Int4> x, RValue<Int4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaxsd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmaxsd);

			return RValue<Int4>(Nucleus::createCall(pmaxsd, x.value, y.value));
		}

		RValue<Int4> pminsd(RValue<Int4> x, RValue<Int4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pminsd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pminsd);

			return RValue<Int4>(Nucleus::createCall(pminsd, x.value, y.value));
		}

		RValue<UInt4> pmaxud(RValue<UInt4> x, RValue<UInt4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaxud = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmaxud);

			return RValue<UInt4>(Nucleus::createCall(pmaxud, x.value, y.value));
		}

		RValue<UInt4> pminud(RValue<UInt4> x, RValue<UInt4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pminud = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pminud);

			return RValue<UInt4>(Nucleus::createCall(pminud, x.value, y.value));
		}

		RValue<Short4> pmulhw(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmulh_w);

			return As<Short4>(RValue<MMX>(Nucleus::createCall(pmulhw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<UShort4> pmulhuw(RValue<UShort4> x, RValue<UShort4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmulhu_w);

			return As<UShort4>(RValue<MMX>(Nucleus::createCall(pmulhuw, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Int2> pmaddwd(RValue<Short4> x, RValue<Short4> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmadd_wd);

			return As<Int2>(RValue<MMX>(Nucleus::createCall(pmaddwd, As<MMX>(x).value, As<MMX>(y).value)));
		}

		RValue<Short8> pmulhw(RValue<Short8> x, RValue<Short8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmulh_w);

			return RValue<Short8>(Nucleus::createCall(pmulhw, x.value, y.value));
		}

		RValue<UShort8> pmulhuw(RValue<UShort8> x, RValue<UShort8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmulhu_w);

			return RValue<UShort8>(Nucleus::createCall(pmulhuw, x.value, y.value));
		}

		RValue<Int4> pmaddwd(RValue<Short8> x, RValue<Short8> y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmadd_wd);

			return RValue<Int4>(Nucleus::createCall(pmaddwd, x.value, y.value));
		}

		RValue<Int> movmskps(RValue<Float4> x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *movmskps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_movmsk_ps);

			return RValue<Int>(Nucleus::createCall(movmskps, x.value));
		}

		RValue<Int> pmovmskb(RValue<Byte8> x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovmskb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmovmskb);

			return RValue<Int>(Nucleus::createCall(pmovmskb, As<MMX>(x).value));
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
			Module *module = Nucleus::getModule();
			llvm::Function *pmovzxbd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovzxbd);

			return RValue<Int4>(Nucleus::createCall(pmovzxbd, Nucleus::createBitCast(x.value, Byte16::getType())));
		}

		RValue<Int4> pmovsxbd(RValue<Int4> x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovsxbd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovsxbd);

			return RValue<Int4>(Nucleus::createCall(pmovsxbd, Nucleus::createBitCast(x.value, SByte16::getType())));
		}

		RValue<Int4> pmovzxwd(RValue<Int4> x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovzxwd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovzxwd);

			return RValue<Int4>(Nucleus::createCall(pmovzxwd, Nucleus::createBitCast(x.value, UShort8::getType())));
		}

		RValue<Int4> pmovsxwd(RValue<Int4> x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovsxwd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovsxwd);

			return RValue<Int4>(Nucleus::createCall(pmovsxwd, Nucleus::createBitCast(x.value, Short8::getType())));
		}

		void emms()
		{
			Module *module = Nucleus::getModule();
			llvm::Function *emms = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_emms);

			Nucleus::createCall(emms);
		}
	}
}
