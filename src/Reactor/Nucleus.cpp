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

#include "Nucleus.hpp"

#include "llvm/Support/IRBuilder.h"
#include "llvm/Function.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Module.h"
#include "llvm/LLVMContext.h"
#include "llvm/Constants.h"
#include "llvm/Intrinsics.h"
#include "llvm/Passmanager.h"
#include "llvm/Instructions.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CFG.h"
#include "../lib/ExecutionEngine/JIT/JIT.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/CallingConv.h"

#include "MemoryManager.hpp"
#include "x86.hpp"
#include "CPUID.hpp"
#include "Thread.hpp"
#include "Memory.hpp"

#include <fstream>

extern "C" void LLVMInitializeX86Target();
extern "C" void LLVMInitializeX86TargetInfo();

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

	MemoryManager *Nucleus::memoryManager = 0;
	ExecutionEngine *Nucleus::executionEngine = 0;
	Builder *Nucleus::builder = 0;
	LLVMContext *Nucleus::context = 0;
	Module *Nucleus::module = 0;
	llvm::Function *Nucleus::function = 0;

	class Builder : public IRBuilder<>
	{
	};

	Routine::Routine(int bufferSize) : bufferSize(bufferSize), dynamic(true)
	{
		void *memory = allocateExecutable(bufferSize);

		buffer = memory;
		entry = memory;
		functionSize = bufferSize;   // Updated by MemoryManager::endFunctionBody

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

	Nucleus::Nucleus()
	{
		LLVMInitializeX86Target();
		LLVMInitializeX86TargetInfo();
		JITEmitDebugInfo = false;

		if(!context)
		{
			context = new LLVMContext();
		}

		module = new Module("", *context);
		memoryManager = new MemoryManager();
		
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

		executionEngine = JIT::createJIT(module, 0, memoryManager, CodeGenOpt::Aggressive, true, CodeModel::Default, architecture, "", MAttrs);

		if(!builder)
		{
			builder = static_cast<Builder*>(new IRBuilder<>(*context));

			HMODULE CodeAnalyst = LoadLibrary("CAJitNtfyLib.dll");
			if(CodeAnalyst)
			{
				CodeAnalystInitialize = (bool(*)())GetProcAddress(CodeAnalyst, "CAJIT_Initialize");
				CodeAnalystCompleteJITLog = (void(*)())GetProcAddress(CodeAnalyst, "CAJIT_CompleteJITLog");
				CodeAnalystLogJITCode = (bool(*)(const void*, unsigned int, const wchar_t*))GetProcAddress(CodeAnalyst, "CAJIT_LogJITCode");
			
				CodeAnalystInitialize();
			}
		}
	}

	Nucleus::~Nucleus()
	{
		delete executionEngine;
		executionEngine = 0;

		memoryManager = 0;
		function = 0;
		module = 0;
	}

	Routine *Nucleus::acquireRoutine(const wchar_t *name, bool runOptimizations)
	{
		#if !(defined(_M_AMD64) || defined(_M_X64))
			x86::emms();
		#endif

		Nucleus::createRetVoid();

		if(false)
		{
			module->print(raw_fd_ostream("llvm-dump-unopt.txt", std::string()), 0);
		}

		if(runOptimizations)
		{
			optimize();
		}

		if(false)
		{
			module->print(raw_fd_ostream("llvm-dump-opt.txt", std::string()), 0);
		}

		void *entry = executionEngine->getPointerToFunction(function);

		Routine *routine = memoryManager->acquireRoutine();
		routine->entry = entry;
		markExecutable(routine->buffer, routine->bufferSize);

		if(CodeAnalystLogJITCode)
		{
			CodeAnalystLogJITCode(routine->entry, routine->getCodeSize(), name);
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

		builder->SetInsertPoint(BasicBlock::Create(*context, function));
	}

	Module *Nucleus::getModule()
	{
		return module;
	}

	Builder *Nucleus::getBuilder()
	{
		return builder;
	}

	llvm::Function *Nucleus::getFunction()
	{
		return function;
	}

	llvm::LLVMContext *Nucleus::getContext()
	{
		return context;
	}

	Value *Nucleus::allocateStackVariable(const Type *type, int arraySize)
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
		return BasicBlock::Create(*context, Nucleus::getFunction());
	}

	BasicBlock *Nucleus::getInsertBlock()
	{
		return builder->GetInsertBlock();
	}

	void Nucleus::setInsertBlock(BasicBlock *basicBlock)
	{
		return builder->SetInsertPoint(basicBlock);
	}

	BasicBlock *Nucleus::getPredecessor(BasicBlock *basicBlock)
	{
		return *pred_begin(basicBlock);
	}

	llvm::Function *Nucleus::createFunction(const llvm::Type *ReturnType, const std::vector<const llvm::Type*> &Params)
	{
		llvm::FunctionType *functionType = llvm::FunctionType::get(ReturnType, Params, false);
		llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "", Nucleus::getModule());
		function->setCallingConv(llvm::CallingConv::C);

		return function;
	}

	llvm::Argument *Nucleus::getArgument(llvm::Function *function, unsigned int index)
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
		return builder->CreateRetVoid();
	}

	Value *Nucleus::createRet(Value *V)
	{
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
		return builder->Insert(new LoadInst(ptr, isVolatile, align));
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, bool isVolatile, unsigned int align)
	{
		return builder->Insert(new StoreInst(value, ptr, isVolatile, align));
	}

	Value *Nucleus::createGEP(Value *ptr, Value *index)
	{
		return builder->CreateGEP(ptr, index);
	}

	Value *Nucleus::createTrunc(Value *V, const Type *destType)
	{
		return builder->CreateTrunc(V, destType);
	}

	Value *Nucleus::createZExt(Value *V, const Type *destType)
	{
		return builder->CreateZExt(V, destType);
	}

	Value *Nucleus::createSExt(Value *V, const Type *destType)
	{
		return builder->CreateSExt(V, destType);
	}

	Value *Nucleus::createFPToUI(Value *V, const Type *destType)
	{
		return builder->CreateFPToUI(V, destType);
	}

	Value *Nucleus::createFPToSI(Value *V, const Type *destType)
	{
		return builder->CreateFPToSI(V, destType);
	}

	Value *Nucleus::createUIToFP(Value *V, const Type *destType)
	{
		return builder->CreateUIToFP(V, destType);
	}

	Value *Nucleus::createSIToFP(Value *V, const Type *destType)
	{
		return builder->CreateSIToFP(V, destType);
	}

	Value *Nucleus::createFPTrunc(Value *V, const Type *destType)
	{
		return builder->CreateFPTrunc(V, destType);
	}

	Value *Nucleus::createFPExt(Value *V, const Type *destType)
	{
		return builder->CreateFPExt(V, destType);
	}

	Value *Nucleus::createPtrToInt(Value *V, const Type *destType)
	{
		return builder->CreatePtrToInt(V, destType);
	}

	Value *Nucleus::createIntToPtr(Value *V, const Type *destType)
	{
		return builder->CreateIntToPtr(V, destType);
	}

	Value *Nucleus::createBitCast(Value *V, const Type *destType)
	{
		return builder->CreateBitCast(V, destType);
	}

	Value *Nucleus::createIntCast(Value *V, const Type *destType, bool isSigned)
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

	llvm::GlobalValue *Nucleus::createGlobalValue(const llvm::Type *Ty, bool isConstant, unsigned int Align)
	{
		llvm::GlobalValue *global = new llvm::GlobalVariable(Ty, isConstant, llvm::GlobalValue::ExternalLinkage, 0, "", false);
		global->setAlignment(Align);

		return global;
	}

	llvm::Type *Nucleus::getPointerType(const llvm::Type *ElementType)
	{
		return llvm::PointerType::get(ElementType, 0);
	}

	llvm::Constant *Nucleus::createNullValue(const llvm::Type *Ty)
	{
		return llvm::Constant::getNullValue(Ty);
	}

	llvm::ConstantInt *Nucleus::createConstantLong(int64_t i)
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

	llvm::Value *Nucleus::createNullPointer(const llvm::Type *Ty)
	{
		return llvm::ConstantPointerNull::get(llvm::PointerType::get(Ty, 0));
	}

	llvm::Value *Nucleus::createConstantVector(Constant* const* Vals, unsigned NumVals)
	{
		return llvm::ConstantVector::get(Vals, NumVals);
	}

	const Type *Void::getType()
	{
		return Type::getVoidTy(*Nucleus::getContext());
	}

	Bool::Bool(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	Bool::Bool()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Bool::Bool(bool x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantBool(x), address);
	}

	Bool::Bool(const RValue<Bool> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Bool::Bool(const Bool &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Bool> Bool::operator=(const RValue<Bool> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Bool> Bool::operator=(const Bool &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Bool>(value);
	}

	RValue<Pointer<Bool>> Bool::operator&()
	{
		return RValue<Pointer<Bool>>(address);
	}

	RValue<Bool> operator!(const RValue<Bool> &val)
	{
		return RValue<Bool>(Nucleus::createNot(val.value));
	}

	RValue<Bool> operator&&(const RValue<Bool> &lhs, const RValue<Bool> &rhs)
	{
		return RValue<Bool>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Bool> operator||(const RValue<Bool> &lhs, const RValue<Bool> &rhs)
	{
		return RValue<Bool>(Nucleus::createOr(lhs.value, rhs.value));
	}

	Bool *Bool::getThis()
	{
		return this;
	}

	const Type *Bool::getType()
	{
		return Type::getInt1Ty(*Nucleus::getContext());
	}

	Byte::Byte(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	Byte::Byte(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createTrunc(cast.value, Byte::getType());

		Nucleus::createStore(integer, address);
	}

	Byte::Byte()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Byte::Byte(int x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantByte((unsigned char)x), address);
	}

	Byte::Byte(unsigned char x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantByte(x), address);
	}

	Byte::Byte(const RValue<Byte> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Byte::Byte(const Byte &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Byte> Byte::operator=(const RValue<Byte> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Byte> Byte::operator=(const Byte &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Byte>(value);
	}

	RValue<Pointer<Byte>> Byte::operator&()
	{
		return RValue<Pointer<Byte>>(address);
	}

	RValue<Byte> operator+(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Byte> operator-(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Byte> operator*(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Byte> operator/(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<Byte> operator%(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<Byte> operator&(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Byte> operator|(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Byte> operator^(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Byte> operator<<(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Byte> operator>>(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Byte>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<Byte> operator+=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Byte> operator-=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Byte> operator*=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Byte> operator/=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Byte> operator%=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Byte> operator&=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Byte> operator|=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Byte> operator^=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Byte> operator<<=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Byte> operator>>=(const Byte &lhs, const RValue<Byte> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Byte> operator+(const RValue<Byte> &val)
	{
		return val;
	}

	RValue<Byte> operator-(const RValue<Byte> &val)
	{
		return RValue<Byte>(Nucleus::createNeg(val.value));
	}

	RValue<Byte> operator~(const RValue<Byte> &val)
	{
		return RValue<Byte>(Nucleus::createNot(val.value));
	}

	RValue<Byte> operator++(const Byte &val, int)   // Post-increment
	{
		RValue<Byte> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantByte((unsigned char)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Byte &operator++(const Byte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantByte((unsigned char)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Byte> operator--(const Byte &val, int)   // Post-decrement
	{
		RValue<Byte> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantByte((unsigned char)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Byte &operator--(const Byte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantByte((unsigned char)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<Byte> &lhs, const RValue<Byte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Byte *Byte::getThis()
	{
		return this;
	}

	const Type *Byte::getType()
	{
		return Type::getInt8Ty(*Nucleus::getContext());
	}

	SByte::SByte(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	SByte::SByte()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	SByte::SByte(signed char x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantByte(x), address);
	}

	SByte::SByte(const RValue<SByte> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	SByte::SByte(const SByte &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<SByte> SByte::operator=(const RValue<SByte> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<SByte> SByte::operator=(const SByte &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<SByte>(value);
	}

	RValue<Pointer<SByte>> SByte::operator&()
	{
		return RValue<Pointer<SByte>>(address);
	}

	RValue<SByte> operator+(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<SByte> operator-(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<SByte> operator*(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<SByte> operator/(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<SByte> operator%(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<SByte> operator&(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<SByte> operator|(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<SByte> operator^(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<SByte> operator<<(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<SByte> operator>>(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<SByte>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<SByte> operator+=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<SByte> operator-=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<SByte> operator*=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<SByte> operator/=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<SByte> operator%=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<SByte> operator&=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<SByte> operator|=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<SByte> operator^=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<SByte> operator<<=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<SByte> operator>>=(const SByte &lhs, const RValue<SByte> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<SByte> operator+(const RValue<SByte> &val)
	{
		return val;
	}

	RValue<SByte> operator-(const RValue<SByte> &val)
	{
		return RValue<SByte>(Nucleus::createNeg(val.value));
	}

	RValue<SByte> operator~(const RValue<SByte> &val)
	{
		return RValue<SByte>(Nucleus::createNot(val.value));
	}

	RValue<SByte> operator++(const SByte &val, int)   // Post-increment
	{
		RValue<SByte> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantByte((signed char)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const SByte &operator++(const SByte &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantByte((signed char)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<SByte> operator--(const SByte &val, int)   // Post-decrement
	{
		RValue<SByte> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantByte((signed char)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const SByte &operator--(const SByte &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantByte((signed char)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<SByte> &lhs, const RValue<SByte> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	SByte *SByte::getThis()
	{
		return this;
	}

	const Type *SByte::getType()
	{
		return Type::getInt8Ty(*Nucleus::getContext());
	}

	Short::Short(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	Short::Short(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createTrunc(cast.value, Short::getType());

		Nucleus::createStore(integer, address);
	}

	Short::Short()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Short::Short(short x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantShort(x), address);
	}

	Short::Short(const RValue<Short> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Short::Short(const Short &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Short> Short::operator=(const RValue<Short> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Short> Short::operator=(const Short &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Short>(value);
	}

	RValue<Pointer<Short>> Short::operator&()
	{
		return RValue<Pointer<Short>>(address);
	}

	RValue<Short> operator+(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Short> operator-(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Short> operator*(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Short> operator/(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Short> operator%(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Short> operator&(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Short> operator|(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Short> operator^(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Short> operator<<(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Short> operator>>(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Short>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<Short> operator+=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Short> operator-=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Short> operator*=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Short> operator/=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Short> operator%=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Short> operator&=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Short> operator|=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Short> operator^=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Short> operator<<=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Short> operator>>=(const Short &lhs, const RValue<Short> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Short> operator+(const RValue<Short> &val)
	{
		return val;
	}

	RValue<Short> operator-(const RValue<Short> &val)
	{
		return RValue<Short>(Nucleus::createNeg(val.value));
	}

	RValue<Short> operator~(const RValue<Short> &val)
	{
		return RValue<Short>(Nucleus::createNot(val.value));
	}

	RValue<Short> operator++(const Short &val, int)   // Post-increment
	{
		RValue<Short> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantShort((short)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Short &operator++(const Short &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantShort((short)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Short> operator--(const Short &val, int)   // Post-decrement
	{
		RValue<Short> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantShort((short)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Short &operator--(const Short &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantShort((short)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<Short> &lhs, const RValue<Short> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	Short *Short::getThis()
	{
		return this;
	}

	const Type *Short::getType()
	{
		return Type::getInt16Ty(*Nucleus::getContext());
	}

	UShort::UShort(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	UShort::UShort()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	UShort::UShort(unsigned short x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantShort(x), address);
	}

	UShort::UShort(const RValue<UShort> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UShort::UShort(const UShort &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<UShort> UShort::operator=(const RValue<UShort> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UShort> UShort::operator=(const UShort &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UShort>(value);
	}

	RValue<Pointer<UShort>> UShort::operator&()
	{
		return RValue<Pointer<UShort>>(address);
	}

	RValue<UShort> operator+(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UShort> operator-(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UShort> operator*(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UShort> operator/(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UShort> operator%(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UShort> operator&(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UShort> operator|(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UShort> operator^(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UShort> operator<<(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<UShort> operator>>(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<UShort>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<UShort> operator+=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UShort> operator-=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UShort> operator*=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UShort> operator/=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UShort> operator%=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UShort> operator&=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UShort> operator|=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UShort> operator^=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UShort> operator<<=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UShort> operator>>=(const UShort &lhs, const RValue<UShort> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UShort> operator+(const RValue<UShort> &val)
	{
		return val;
	}

	RValue<UShort> operator-(const RValue<UShort> &val)
	{
		return RValue<UShort>(Nucleus::createNeg(val.value));
	}

	RValue<UShort> operator~(const RValue<UShort> &val)
	{
		return RValue<UShort>(Nucleus::createNot(val.value));
	}

	RValue<UShort> operator++(const UShort &val, int)   // Post-increment
	{
		RValue<UShort> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantShort((unsigned short)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const UShort &operator++(const UShort &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantShort((unsigned short)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<UShort> operator--(const UShort &val, int)   // Post-decrement
	{
		RValue<UShort> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantShort((unsigned short)1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const UShort &operator--(const UShort &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantShort((unsigned short)1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<UShort> &lhs, const RValue<UShort> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	UShort *UShort::getThis()
	{
		return this;
	}

	const Type *UShort::getType()
	{
		return Type::getInt16Ty(*Nucleus::getContext());
	}

	Byte4 *Byte4::getThis()
	{
		return this;
	}

	const Type *Byte4::getType()
	{
		#if 0
			return VectorType::get(Byte::getType(), 4);
		#else
			return UInt::getType();   // FIXME: LLVM doesn't manipulate it as one 32-bit block
		#endif
	}

	SByte4 *SByte4::getThis()
	{
		return this;
	}

	const Type *SByte4::getType()
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
		address = Nucleus::allocateStackVariable(getType());
	}

	Byte8::Byte8(byte x0, byte x1, byte x2, byte x3, byte x4, byte x5, byte x6, byte x7)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte(x0);
		constantVector[1] = Nucleus::createConstantByte(x1);
		constantVector[2] = Nucleus::createConstantByte(x2);
		constantVector[3] = Nucleus::createConstantByte(x3);
		constantVector[4] = Nucleus::createConstantByte(x4);
		constantVector[5] = Nucleus::createConstantByte(x5);
		constantVector[6] = Nucleus::createConstantByte(x6);
		constantVector[7] = Nucleus::createConstantByte(x7);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	Byte8::Byte8(int64_t x)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte((unsigned char)(x >>  0));
		constantVector[1] = Nucleus::createConstantByte((unsigned char)(x >>  8));
		constantVector[2] = Nucleus::createConstantByte((unsigned char)(x >> 16));
		constantVector[3] = Nucleus::createConstantByte((unsigned char)(x >> 24));
		constantVector[4] = Nucleus::createConstantByte((unsigned char)(x >> 32));
		constantVector[5] = Nucleus::createConstantByte((unsigned char)(x >> 40));
		constantVector[6] = Nucleus::createConstantByte((unsigned char)(x >> 48));
		constantVector[7] = Nucleus::createConstantByte((unsigned char)(x >> 56));

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	Byte8::Byte8(const RValue<Byte8> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Byte8::Byte8(const Byte8 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Byte8> Byte8::operator=(const RValue<Byte8> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Byte8> Byte8::operator=(const Byte8 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Byte8>(value);
	}

	RValue<Byte8> operator+(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Byte8> operator-(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Byte8> operator*(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Byte8> operator/(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<Byte8> operator%(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<Byte8> operator&(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Byte8> operator|(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Byte8> operator^(const RValue<Byte8> &lhs, const RValue<Byte8> &rhs)
	{
		return RValue<Byte8>(Nucleus::createXor(lhs.value, rhs.value));
	}

//	RValue<Byte8> operator<<(const RValue<Byte8> &lhs, unsigned char rhs)
//	{
//		return RValue<Byte8>(Nucleus::createShl(lhs.value, rhs.value));
//	}

//	RValue<Byte8> operator>>(const RValue<Byte8> &lhs, unsigned char rhs)
//	{
//		return RValue<Byte8>(Nucleus::createLShr(lhs.value, rhs.value));
//	}

	RValue<Byte8> operator+=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Byte8> operator-=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Byte8> operator*=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Byte8> operator/=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Byte8> operator%=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Byte8> operator&=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Byte8> operator|=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Byte8> operator^=(const Byte8 &lhs, const RValue<Byte8> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

//	RValue<Byte8> operator<<=(const Byte8 &lhs, const RValue<Byte8> &rhs)
//	{
//		return lhs = lhs << rhs;
//	}

//	RValue<Byte8> operator>>=(const Byte8 &lhs, const RValue<Byte8> &rhs)
//	{
//		return lhs = lhs >> rhs;
//	}

	RValue<Byte8> operator+(const RValue<Byte8> &val)
	{
		return val;
	}

	RValue<Byte8> operator-(const RValue<Byte8> &val)
	{
		return RValue<Byte8>(Nucleus::createNeg(val.value));
	}

	RValue<Byte8> operator~(const RValue<Byte8> &val)
	{
		return RValue<Byte8>(Nucleus::createNot(val.value));
	}

	RValue<Byte8> AddSat(const RValue<Byte8> &x, const RValue<Byte8> &y)
	{
		return x86::paddusb(x, y);
	}
	
	RValue<Byte8> SubSat(const RValue<Byte8> &x, const RValue<Byte8> &y)
	{
		return x86::psubusb(x, y);
	}

	RValue<Short4> UnpackLow(const RValue<Byte8> &x, const RValue<Byte8> &y)
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
	
	RValue<Short4> UnpackHigh(const RValue<Byte8> &x, const RValue<Byte8> &y)
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

	RValue<Int> SignMask(const RValue<Byte8> &x)
	{
		return x86::pmovmskb(x);
	}

//	RValue<Byte8> CmpGT(const RValue<Byte8> &x, const RValue<Byte8> &y)
//	{
//		return x86::pcmpgtb(x, y);   // FIXME: Signedness
//	}
	
	RValue<Byte8> CmpEQ(const RValue<Byte8> &x, const RValue<Byte8> &y)
	{
		return x86::pcmpeqb(x, y);
	}

	Byte8 *Byte8::getThis()
	{
		return this;
	}

	const Type *Byte8::getType()
	{
		return VectorType::get(Byte::getType(), 8);
	}

	SByte8::SByte8()
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());
	}

	SByte8::SByte8(byte x0, byte x1, byte x2, byte x3, byte x4, byte x5, byte x6, byte x7)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte(x0);
		constantVector[1] = Nucleus::createConstantByte(x1);
		constantVector[2] = Nucleus::createConstantByte(x2);
		constantVector[3] = Nucleus::createConstantByte(x3);
		constantVector[4] = Nucleus::createConstantByte(x4);
		constantVector[5] = Nucleus::createConstantByte(x5);
		constantVector[6] = Nucleus::createConstantByte(x6);
		constantVector[7] = Nucleus::createConstantByte(x7);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	SByte8::SByte8(int64_t x)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantByte((unsigned char)(x >>  0));
		constantVector[1] = Nucleus::createConstantByte((unsigned char)(x >>  8));
		constantVector[2] = Nucleus::createConstantByte((unsigned char)(x >> 16));
		constantVector[3] = Nucleus::createConstantByte((unsigned char)(x >> 24));
		constantVector[4] = Nucleus::createConstantByte((unsigned char)(x >> 32));
		constantVector[5] = Nucleus::createConstantByte((unsigned char)(x >> 40));
		constantVector[6] = Nucleus::createConstantByte((unsigned char)(x >> 48));
		constantVector[7] = Nucleus::createConstantByte((unsigned char)(x >> 56));

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	SByte8::SByte8(const RValue<SByte8> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	SByte8::SByte8(const SByte8 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<SByte8> SByte8::operator=(const RValue<SByte8> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<SByte8> SByte8::operator=(const SByte8 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<SByte8>(value);
	}

	RValue<SByte8> operator+(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<SByte8> operator-(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<SByte8> operator*(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<SByte8> operator/(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<SByte8> operator%(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<SByte8> operator&(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<SByte8> operator|(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<SByte8> operator^(const RValue<SByte8> &lhs, const RValue<SByte8> &rhs)
	{
		return RValue<SByte8>(Nucleus::createXor(lhs.value, rhs.value));
	}

//	RValue<SByte8> operator<<(const RValue<SByte8> &lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createShl(lhs.value, rhs.value));
//	}

//	RValue<SByte8> operator>>(const RValue<SByte8> &lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createAShr(lhs.value, rhs.value));
//	}

	RValue<SByte8> operator+=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<SByte8> operator-=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<SByte8> operator*=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<SByte8> operator/=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<SByte8> operator%=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<SByte8> operator&=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<SByte8> operator|=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<SByte8> operator^=(const SByte8 &lhs, const RValue<SByte8> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

//	RValue<SByte8> operator<<=(const SByte8 &lhs, const RValue<SByte8> &rhs)
//	{
//		return lhs = lhs << rhs;
//	}

//	RValue<SByte8> operator>>=(const SByte8 &lhs, const RValue<SByte8> &rhs)
//	{
//		return lhs = lhs >> rhs;
//	}

	RValue<SByte8> operator+(const RValue<SByte8> &val)
	{
		return val;
	}

	RValue<SByte8> operator-(const RValue<SByte8> &val)
	{
		return RValue<SByte8>(Nucleus::createNeg(val.value));
	}

	RValue<SByte8> operator~(const RValue<SByte8> &val)
	{
		return RValue<SByte8>(Nucleus::createNot(val.value));
	}

	RValue<SByte8> AddSat(const RValue<SByte8> &x, const RValue<SByte8> &y)
	{
		return x86::paddsb(x, y);
	}
	
	RValue<SByte8> SubSat(const RValue<SByte8> &x, const RValue<SByte8> &y)
	{
		return x86::psubsb(x, y);
	}

	RValue<Short4> UnpackLow(const RValue<SByte8> &x, const RValue<SByte8> &y)
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
	
	RValue<Short4> UnpackHigh(const RValue<SByte8> &x, const RValue<SByte8> &y)
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

	RValue<Int> SignMask(const RValue<SByte8> &x)
	{
		return x86::pmovmskb(As<Byte8>(x));
	}

	RValue<Byte8> CmpGT(const RValue<SByte8> &x, const RValue<SByte8> &y)
	{
		return x86::pcmpgtb(x, y);
	}
	
	RValue<Byte8> CmpEQ(const RValue<SByte8> &x, const RValue<SByte8> &y)
	{
		return x86::pcmpeqb(As<Byte8>(x), As<Byte8>(y));
	}

	SByte8 *SByte8::getThis()
	{
		return this;
	}

	const Type *SByte8::getType()
	{
		return VectorType::get(SByte::getType(), 8);
	}

	Byte16::Byte16(const RValue<Byte16> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Byte16::Byte16(const Byte16 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Byte16> Byte16::operator=(const RValue<Byte16> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Byte16> Byte16::operator=(const Byte16 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Byte16>(value);
	}

	Byte16 *Byte16::getThis()
	{
		return this;
	}

	const Type *Byte16::getType()
	{
		return VectorType::get(Byte::getType(), 16);
	}

	SByte16 *SByte16::getThis()
	{
		return this;
	}

	const Type *SByte16::getType()
	{
		return VectorType::get(SByte::getType(), 16);
	}

	Short4::Short4(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
		Value *vector = Nucleus::createBitCast(extend, Short4::getType());
		Value *swizzle = Nucleus::createSwizzle(vector, 0x00);
		
		Nucleus::createStore(swizzle, address);
	}

	Short4::Short4(const RValue<Int4> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

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

		Nucleus::createStore(short4, address);
	}

//	Short4::Short4(const RValue<Float> &cast)
//	{
//	}

	Short4::Short4(const RValue<Float4> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Int4 v4i32 = Int4(cast);
		v4i32 = As<Int4>(x86::packssdw(v4i32, v4i32));
		
		Nucleus::createStore(As<Short4>(Int2(v4i32)).value, address);
	}

	Short4::Short4()
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());
	}

	Short4::Short4(short x, short y, short z, short w)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(x);
		constantVector[1] = Nucleus::createConstantShort(y);
		constantVector[2] = Nucleus::createConstantShort(z);
		constantVector[3] = Nucleus::createConstantShort(w);
		Nucleus::createConstantVector(constantVector, 4);
		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 4), address);
	}

	Short4::Short4(const RValue<Short4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Short4::Short4(const Short4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	Short4::Short4(const RValue<UShort4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Short4::Short4(const UShort4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Short4> Short4::operator=(const RValue<Short4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Short4> Short4::operator=(const Short4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Short4>(value);
	}

	RValue<Short4> Short4::operator=(const RValue<UShort4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Short4> Short4::operator=(const UShort4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Short4>(value);
	}

	RValue<Short4> operator+(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Short4> operator-(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Short4> operator*(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Short4> operator/(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Short4> operator%(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Short4> operator&(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Short4> operator|(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Short4> operator^(const RValue<Short4> &lhs, const RValue<Short4> &rhs)
	{
		return RValue<Short4>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Short4> operator<<(const RValue<Short4> &lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
	}

	RValue<Short4> operator>>(const RValue<Short4> &lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psraw(lhs, rhs);
	}

	RValue<Short4> operator<<(const RValue<Short4> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
	}

	RValue<Short4> operator>>(const RValue<Short4> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psraw(lhs, rhs);
	}

	RValue<Short4> operator+=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Short4> operator-=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Short4> operator*=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Short4> operator/=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Short4> operator%=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Short4> operator&=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Short4> operator|=(const Short4 &lhs, const RValue<Short4> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Short4> operator^=(const Short4 &lhs, const RValue<Short4> &rhs)
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

	RValue<Short4> operator<<=(const Short4 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Short4> operator>>=(const Short4 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Short4> operator+(const RValue<Short4> &val)
	{
		return val;
	}

	RValue<Short4> operator-(const RValue<Short4> &val)
	{
		return RValue<Short4>(Nucleus::createNeg(val.value));
	}

	RValue<Short4> operator~(const RValue<Short4> &val)
	{
		return RValue<Short4>(Nucleus::createNot(val.value));
	}

	RValue<Short4> RoundShort4(const RValue<Float4> &cast)
	{
		RValue<Int4> v4i32 = x86::cvtps2dq(cast);
		v4i32 = As<Int4>(x86::packssdw(v4i32, v4i32));
		
		return As<Short4>(Int2(v4i32));
	}

	RValue<Short4> Max(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pmaxsw(x, y);
	}

	RValue<Short4> Min(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pminsw(x, y);
	}

	RValue<Short4> AddSat(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::paddsw(x, y);
	}

	RValue<Short4> SubSat(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::psubsw(x, y);
	}

	RValue<Short4> MulHigh(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pmulhw(x, y);
	}

	RValue<Int2> MulAdd(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pmaddwd(x, y);
	}

	RValue<SByte8> Pack(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::packsswb(x, y);
	}

	RValue<Int2> UnpackLow(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(0);
		shuffle[1] = Nucleus::createConstantInt(4);
		shuffle[2] = Nucleus::createConstantInt(1);
		shuffle[3] = Nucleus::createConstantInt(5);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4));

		return RValue<Int2>(Nucleus::createBitCast(packed, Int2::getType()));
	}

	RValue<Int2> UnpackHigh(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(2);
		shuffle[1] = Nucleus::createConstantInt(6);
		shuffle[2] = Nucleus::createConstantInt(3);
		shuffle[3] = Nucleus::createConstantInt(7);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4));

		return RValue<Int2>(Nucleus::createBitCast(packed, Int2::getType()));
	}

	RValue<Short4> Swizzle(const RValue<Short4> &x, unsigned char select)
	{
		return RValue<Short4>(Nucleus::createSwizzle(x.value, select));
	}

	RValue<Short4> Insert(const RValue<Short4> &val, const RValue<Short> &element, int i)
	{
		return RValue<Short4>(Nucleus::createInsertElement(val.value, element.value, i));
	}

	RValue<Short> Extract(const RValue<Short4> &val, int i)
	{
		return RValue<Short>(Nucleus::createExtractElement(val.value, i));
	}

	RValue<Short4> CmpGT(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pcmpgtw(x, y);
	}

	RValue<Short4> CmpEQ(const RValue<Short4> &x, const RValue<Short4> &y)
	{
		return x86::pcmpeqw(x, y);
	}

	Short4 *Short4::getThis()
	{
		return this;
	}

	const Type *Short4::getType()
	{
		return VectorType::get(Short::getType(), 4);
	}

	UShort4::UShort4(const RValue<Int4> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		*this = Short4(cast);
	}

	UShort4::UShort4(const RValue<Float4> &cast, bool saturate)
	{
		address = Nucleus::allocateStackVariable(getType());

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
		address = Nucleus::allocateStackVariable(getType());
	}

	UShort4::UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantShort(x);
		constantVector[1] = Nucleus::createConstantShort(y);
		constantVector[2] = Nucleus::createConstantShort(z);
		constantVector[3] = Nucleus::createConstantShort(w);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 4), address);
	}

	UShort4::UShort4(const RValue<UShort4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UShort4::UShort4(const UShort4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	UShort4::UShort4(const RValue<Short4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UShort4::UShort4(const Short4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<UShort4> UShort4::operator=(const RValue<UShort4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UShort4> UShort4::operator=(const UShort4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> UShort4::operator=(const RValue<Short4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UShort4> UShort4::operator=(const Short4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UShort4>(value);
	}

	RValue<UShort4> operator+(const RValue<UShort4> &lhs, const RValue<UShort4> &rhs)
	{
		return RValue<UShort4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UShort4> operator-(const RValue<UShort4> &lhs, const RValue<UShort4> &rhs)
	{
		return RValue<UShort4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UShort4> operator*(const RValue<UShort4> &lhs, const RValue<UShort4> &rhs)
	{
		return RValue<UShort4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UShort4> operator<<(const RValue<UShort4> &lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
	}

	RValue<UShort4> operator>>(const RValue<UShort4> &lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrlw(lhs, rhs);
	}

	RValue<UShort4> operator<<(const RValue<UShort4> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
	}

	RValue<UShort4> operator>>(const RValue<UShort4> &lhs, const RValue<Long1> &rhs)
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

	RValue<UShort4> operator<<=(const UShort4 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UShort4> operator>>=(const UShort4 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UShort4> operator~(const RValue<UShort4> &val)
	{
		return RValue<UShort4>(Nucleus::createNot(val.value));
	}

	RValue<UShort4> Max(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return Max(As<Short4>(x) - Short4(0x8000, 0x8000, 0x8000, 0x8000), As<Short4>(y) - Short4(0x8000, 0x8000, 0x8000, 0x8000)) + Short4(0x8000, 0x8000, 0x8000, 0x8000);;
	}

	RValue<UShort4> Min(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return Min(As<Short4>(x) - Short4(0x8000, 0x8000, 0x8000, 0x8000), As<Short4>(y) - Short4(0x8000, 0x8000, 0x8000, 0x8000)) + Short4(0x8000, 0x8000, 0x8000, 0x8000);;
	}

	RValue<UShort4> AddSat(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return x86::paddusw(x, y);
	}

	RValue<UShort4> SubSat(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return x86::psubusw(x, y);
	}

	RValue<UShort4> MulHigh(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return x86::pmulhuw(x, y);
	}

	RValue<UShort4> Average(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return x86::pavgw(x, y);
	}

	RValue<Byte8> Pack(const RValue<UShort4> &x, const RValue<UShort4> &y)
	{
		return x86::packuswb(x, y);
	}

	UShort4 *UShort4::getThis()
	{
		return this;
	}

	const Type *UShort4::getType()
	{
		return VectorType::get(UShort::getType(), 4);
	}

	Short8::Short8(short c0, short c1, short c2, short c3, short c4, short c5, short c6, short c7)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantShort(c0);
		constantVector[1] = Nucleus::createConstantShort(c1);
		constantVector[2] = Nucleus::createConstantShort(c2);
		constantVector[3] = Nucleus::createConstantShort(c3);
		constantVector[4] = Nucleus::createConstantShort(c4);
		constantVector[5] = Nucleus::createConstantShort(c5);
		constantVector[6] = Nucleus::createConstantShort(c6);
		constantVector[7] = Nucleus::createConstantShort(c7);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	Short8::Short8(const RValue<Short8> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	RValue<Short8> operator+(const RValue<Short8> &lhs, const RValue<Short8> &rhs)
	{
		return RValue<Short8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Short8> operator&(const RValue<Short8> &lhs, const RValue<Short8> &rhs)
	{
		return RValue<Short8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Short8> operator<<(const RValue<Short8> &lhs, unsigned char rhs)
	{
		return x86::psllw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<Short8> operator>>(const RValue<Short8> &lhs, unsigned char rhs)
	{
		return x86::psraw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<Short8> Concatenate(const RValue<Short4> &lo, const RValue<Short4> &hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *short8 = Nucleus::createBitCast(long2, Short8::getType());

		return RValue<Short8>(short8);
	}

	RValue<Int4> MulAdd(const RValue<Short8> &x, const RValue<Short8> &y)
	{
		return x86::pmaddwd(x, y);   // FIXME: Fallback required
	}

	RValue<Short8> MulHigh(const RValue<Short8> &x, const RValue<Short8> &y)
	{
		return x86::pmulhw(x, y);   // FIXME: Fallback required
	}

	Short8 *Short8::getThis()
	{
		return this;
	}

	const Type *Short8::getType()
	{
		return VectorType::get(Short::getType(), 8);
	}

	UShort8::UShort8(unsigned short c0, unsigned short c1, unsigned short c2, unsigned short c3, unsigned short c4, unsigned short c5, unsigned short c6, unsigned short c7)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[8];
		constantVector[0] = Nucleus::createConstantShort(c0);
		constantVector[1] = Nucleus::createConstantShort(c1);
		constantVector[2] = Nucleus::createConstantShort(c2);
		constantVector[3] = Nucleus::createConstantShort(c3);
		constantVector[4] = Nucleus::createConstantShort(c4);
		constantVector[5] = Nucleus::createConstantShort(c5);
		constantVector[6] = Nucleus::createConstantShort(c6);
		constantVector[7] = Nucleus::createConstantShort(c7);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 8), address);
	}

	UShort8::UShort8(const RValue<UShort8> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	RValue<UShort8> UShort8::operator=(const RValue<UShort8> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UShort8> UShort8::operator=(const UShort8 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UShort8>(value);
	}

	RValue<UShort8> operator&(const RValue<UShort8> &lhs, const RValue<UShort8> &rhs)
	{
		return RValue<UShort8>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UShort8> operator<<(const RValue<UShort8> &lhs, unsigned char rhs)
	{
		return As<UShort8>(x86::psllw(As<Short8>(lhs), rhs));   // FIXME: Fallback required
	}

	RValue<UShort8> operator>>(const RValue<UShort8> &lhs, unsigned char rhs)
	{
		return x86::psrlw(lhs, rhs);   // FIXME: Fallback required
	}

	RValue<UShort8> operator+(const RValue<UShort8> &lhs, const RValue<UShort8> &rhs)
	{
		return RValue<UShort8>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UShort8> operator*(const RValue<UShort8> &lhs, const RValue<UShort8> &rhs)
	{
		return RValue<UShort8>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UShort8> operator+=(const UShort8 &lhs, const RValue<UShort8> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UShort8> operator~(const RValue<UShort8> &val)
	{
		return RValue<UShort8>(Nucleus::createNot(val.value));
	}

	RValue<UShort8> Swizzle(const RValue<UShort8> &x, char select0, char select1, char select2, char select3, char select4, char select5, char select6, char select7)
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

	RValue<UShort8> Concatenate(const RValue<UShort4> &lo, const RValue<UShort4> &hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *short8 = Nucleus::createBitCast(long2, Short8::getType());

		return RValue<UShort8>(short8);
	}

	RValue<UShort8> MulHigh(const RValue<UShort8> &x, const RValue<UShort8> &y)
	{
		return x86::pmulhuw(x, y);   // FIXME: Fallback required
	}

	// FIXME: Implement as Shuffle(x, y, Select(i0, ..., i16)) and Shuffle(x, y, SELECT_PACK_REPEAT(element))
//	RValue<UShort8> PackRepeat(const RValue<Byte16> &x, const RValue<Byte16> &y, int element)
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

	UShort8 *UShort8::getThis()
	{
		return this;
	}

	const Type *UShort8::getType()
	{
		return VectorType::get(UShort::getType(), 8);
	}

	Int::Int(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	Int::Int(const RValue<Byte> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createZExt(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int(const RValue<SByte> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createSExt(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int(const RValue<Short> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createSExt(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int(const RValue<UShort> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createZExt(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int(const RValue<Int2> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		*this = Extract(cast, 0);
	}

	Int::Int(const RValue<Long> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createTrunc(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int(const RValue<Float> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createFPToSI(cast.value, Int::getType());

		Nucleus::createStore(integer, address);
	}

	Int::Int()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Int::Int(int x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantInt(x), address);
	}

	Int::Int(const RValue<Int> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Int::Int(const RValue<UInt> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Int::Int(const Int &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	Int::Int(const UInt &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Int> Int::operator=(int rhs) const
	{
		return RValue<Int>(Nucleus::createStore(Nucleus::createConstantInt(rhs), address));
	}

	RValue<Int> Int::operator=(const RValue<Int> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Int> Int::operator=(const RValue<UInt> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Int> Int::operator=(const Int &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Int>(value);
	}

	RValue<Int> Int::operator=(const UInt &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Int>(value);
	}

	RValue<Pointer<Int>> Int::operator&()
	{
		return RValue<Pointer<Int>>(address);
	}

	RValue<Int> operator+(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Int> operator-(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Int> operator*(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Int> operator/(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Int> operator%(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Int> operator&(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Int> operator|(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Int> operator^(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Int> operator<<(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<Int> operator>>(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Int>(Nucleus::createAShr(lhs.value, rhs.value));
	}

	RValue<Int> operator+=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int> operator-=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Int> operator*=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Int> operator/=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Int> operator%=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Int> operator&=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int> operator|=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int> operator^=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<Int> operator<<=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int> operator>>=(const Int &lhs, const RValue<Int> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Int> operator+(const RValue<Int> &val)
	{
		return val;
	}

	RValue<Int> operator-(const RValue<Int> &val)
	{
		return RValue<Int>(Nucleus::createNeg(val.value));
	}

	RValue<Int> operator~(const RValue<Int> &val)
	{
		return RValue<Int>(Nucleus::createNot(val.value));
	}

	RValue<Int> operator++(const Int &val, int)   // Post-increment
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Int &operator++(const Int &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Int> operator--(const Int &val, int)   // Post-decrement
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const Int &operator--(const Int &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpSGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<Int> &lhs, const RValue<Int> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

	RValue<Int> RoundInt(const RValue<Float> &cast)
	{
		return x86::cvtss2si(cast);

	//	return IfThenElse(val > Float(0), Int(val + Float(0.5f)), Int(val - Float(0.5f)));
	}

	Int *Int::getThis()
	{
		return this;
	}

	const Type *Int::getType()
	{
		return Type::getInt32Ty(*Nucleus::getContext());
	}

	Long::Long(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createSExt(cast.value, Long::getType());

		Nucleus::createStore(integer, address);
	}

	Long::Long(const RValue<UInt> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createZExt(cast.value, Long::getType());

		Nucleus::createStore(integer, address);
	}

	Long::Long()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Long::Long(const RValue<Long> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	RValue<Long> Long::operator=(int64_t rhs) const
	{
		return RValue<Long>(Nucleus::createStore(Nucleus::createConstantLong(rhs), address));
	}

	RValue<Long> Long::operator=(const RValue<Long> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Long> Long::operator=(const Long &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Long>(value);
	}

	RValue<Long> operator+(const RValue<Long> &lhs, const RValue<Long> &rhs)
	{
		return RValue<Long>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Long> operator-(const RValue<Long> &lhs, const RValue<Long> &rhs)
	{
		return RValue<Long>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Long> operator+=(const Long &lhs, const RValue<Long> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Long> operator-=(const Long &lhs, const RValue<Long> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Long> AddAtomic(const RValue<Pointer<Long>> &x, const RValue<Long> &y)
	{
		Module *module = Nucleus::getModule();
		const llvm::Type *type = Long::getType();
		llvm::Function *atomic = Intrinsic::getDeclaration(module, Intrinsic::atomic_load_add, &type, 1);

		return RValue<Long>(Nucleus::createCall(atomic, x.value, y.value));
	}

	Long *Long::getThis()
	{
		return this;
	}

	const Type *Long::getType()
	{
		return Type::getInt64Ty(*Nucleus::getContext());
	}

	Long1::Long1(const Reference<UInt> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *uint = Nucleus::createLoad(cast.address);
		Value *int64 = Nucleus::createZExt(uint, Long::getType());
		Value *long1 = Nucleus::createBitCast(int64, Long1::getType());
		
		Nucleus::createStore(long1, address);
	}

	Long1::Long1(const RValue<Long1> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Long1 *Long1::getThis()
	{
		return this;
	}

	const Type *Long1::getType()
	{
		return VectorType::get(Long::getType(), 1);
	}

	RValue<Long2> UnpackHigh(const RValue<Long2> &x, const RValue<Long2> &y)
	{
		Constant *shuffle[2];
		shuffle[0] = Nucleus::createConstantInt(1);
		shuffle[1] = Nucleus::createConstantInt(3);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

		return RValue<Long2>(packed);
	}

	Long2 *Long2::getThis()
	{
		return this;
	}

	const Type *Long2::getType()
	{
		return VectorType::get(Long::getType(), 2);
	}

	UInt::UInt(Argument *argument)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(argument, address);
	}

	UInt::UInt(const RValue<UShort> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createZExt(cast.value, UInt::getType());

		Nucleus::createStore(integer, address);
	}

	UInt::UInt(const RValue<Long> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createTrunc(cast.value, UInt::getType());

		Nucleus::createStore(integer, address);
	}

	UInt::UInt(const RValue<Float> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createFPToSI(cast.value, UInt::getType());

		Nucleus::createStore(integer, address);
	}

	UInt::UInt()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	UInt::UInt(int x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantInt(x), address);
	}

	UInt::UInt(unsigned int x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantInt(x), address);
	}

	UInt::UInt(const RValue<UInt> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UInt::UInt(const RValue<Int> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UInt::UInt(const UInt &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	UInt::UInt(const Int &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<UInt> UInt::operator=(unsigned int rhs) const
	{
		return RValue<UInt>(Nucleus::createStore(Nucleus::createConstantInt(rhs), address));
	}

	RValue<UInt> UInt::operator=(const RValue<UInt> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UInt> UInt::operator=(const RValue<Int> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UInt> UInt::operator=(const UInt &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UInt>(value);
	}

	RValue<UInt> UInt::operator=(const Int &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UInt>(value);
	}

	RValue<Pointer<UInt>> UInt::operator&()
	{
		return RValue<Pointer<UInt>>(address);
	}

	RValue<UInt> operator+(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UInt> operator-(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UInt> operator*(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UInt> operator/(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UInt> operator%(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UInt> operator&(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UInt> operator|(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UInt> operator^(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UInt> operator<<(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createShl(lhs.value, rhs.value));
	}

	RValue<UInt> operator>>(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<UInt>(Nucleus::createLShr(lhs.value, rhs.value));
	}

	RValue<UInt> operator+=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt> operator-=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UInt> operator*=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UInt> operator/=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UInt> operator%=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UInt> operator&=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt> operator|=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt> operator^=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs ^ rhs;
	}

	RValue<UInt> operator<<=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt> operator>>=(const UInt &lhs, const RValue<UInt> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UInt> operator+(const RValue<UInt> &val)
	{
		return val;
	}

	RValue<UInt> operator-(const RValue<UInt> &val)
	{
		return RValue<UInt>(Nucleus::createNeg(val.value));
	}

	RValue<UInt> operator~(const RValue<UInt> &val)
	{
		return RValue<UInt>(Nucleus::createNot(val.value));
	}

	RValue<UInt> operator++(const UInt &val, int)   // Post-increment
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const UInt &operator++(const UInt &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(Nucleus::createLoad(val.address), Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<UInt> operator--(const UInt &val, int)   // Post-decrement
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return res;
	}

	const UInt &operator--(const UInt &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(Nucleus::createLoad(val.address), Nucleus::createConstantInt(1));
		Nucleus::createStore(inc, val.address);

		return val;
	}

	RValue<Bool> operator<(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpULE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpUGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpNE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<UInt> &lhs, const RValue<UInt> &rhs)
	{
		return RValue<Bool>(Nucleus::createICmpEQ(lhs.value, rhs.value));
	}

//	RValue<UInt> RoundUInt(const RValue<Float> &cast)
//	{
//		return x86::cvtss2si(val);   // FIXME: Unsigned
//
//	//	return IfThenElse(val > Float(0), Int(val + Float(0.5f)), Int(val - Float(0.5f)));
//	}

	UInt *UInt::getThis()
	{
		return this;
	}

	const Type *UInt::getType()
	{
		return Type::getInt32Ty(*Nucleus::getContext());
	}

	Int2::Int2(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
		Value *vector = Nucleus::createBitCast(extend, Int2::getType());
		
		Constant *shuffle[2];
		shuffle[0] = Nucleus::createConstantInt(0);
		shuffle[1] = Nucleus::createConstantInt(0);

		Value *replicate = Nucleus::createShuffleVector(vector, UndefValue::get(Int2::getType()), Nucleus::createConstantVector(shuffle, 2));

		Nucleus::createStore(replicate, address);
	}

	Int2::Int2(const RValue<Int4> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *long2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *element = Nucleus::createExtractElement(long2, 0);
		Value *int2 = Nucleus::createBitCast(element, Int2::getType());

		Nucleus::createStore(int2, address);
	}

	Int2::Int2()
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());
	}

	Int2::Int2(int x, int y)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[2];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 2), address);
	}

	Int2::Int2(const RValue<Int2> &rhs)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Int2::Int2(const Int2 &rhs)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Int2> Int2::operator=(const RValue<Int2> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Int2> Int2::operator=(const Int2 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Int2>(value);
	}

	RValue<Int2> operator+(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Int2> operator-(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Int2> operator*(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Int2> operator/(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Int2> operator%(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Int2> operator&(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Int2> operator|(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Int2> operator^(const RValue<Int2> &lhs, const RValue<Int2> &rhs)
	{
		return RValue<Int2>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Int2> operator<<(const RValue<Int2> &lhs, unsigned char rhs)
	{
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
	}

	RValue<Int2> operator>>(const RValue<Int2> &lhs, unsigned char rhs)
	{
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
	}

	RValue<Int2> operator<<(const RValue<Int2> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
	}

	RValue<Int2> operator>>(const RValue<Int2> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
	}

	RValue<Int2> operator+=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int2> operator-=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Int2> operator*=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Int2> operator/=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Int2> operator%=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Int2> operator&=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int2> operator|=(const Int2 &lhs, const RValue<Int2> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int2> operator^=(const Int2 &lhs, const RValue<Int2> &rhs)
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

	RValue<Int2> operator<<=(const Int2 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<Int2> operator>>=(const Int2 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<Int2> operator+(const RValue<Int2> &val)
	{
		return val;
	}

	RValue<Int2> operator-(const RValue<Int2> &val)
	{
		return RValue<Int2>(Nucleus::createNeg(val.value));
	}

	RValue<Int2> operator~(const RValue<Int2> &val)
	{
		return RValue<Int2>(Nucleus::createNot(val.value));
	}

	RValue<Long1> UnpackLow(const RValue<Int2> &x, const RValue<Int2> &y)
	{
		Constant *shuffle[2];
		shuffle[0] = Nucleus::createConstantInt(0);
		shuffle[1] = Nucleus::createConstantInt(2);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

		return RValue<Long1>(Nucleus::createBitCast(packed, Long1::getType()));
	}
	
	RValue<Long1> UnpackHigh(const RValue<Int2> &x, const RValue<Int2> &y)
	{
		Constant *shuffle[2];
		shuffle[0] = Nucleus::createConstantInt(1);
		shuffle[1] = Nucleus::createConstantInt(3);

		Value *packed = Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 2));

		return RValue<Long1>(Nucleus::createBitCast(packed, Long1::getType()));
	}

	RValue<Int> Extract(const RValue<Int2> &val, int i)
	{
		if(false)   // FIXME: LLVM does not generate optimal code
		{
			return RValue<Int>(Nucleus::createExtractElement(val.value, i));
		}
		else
		{
			if(i == 0)
			{
				return RValue<Int>(Nucleus::createExtractElement(val.value, 0));
			}
			else
			{
				Int2 val2 = As<Int2>(UnpackHigh(val, val));

				return Extract(val2, 0);
			}
		}
	}

	// FIXME: Crashes LLVM
//	RValue<Int2> Insert(const RValue<Int2> &val, const RValue<Int> &element, int i)
//	{
//		return RValue<Int2>(Nucleus::createInsertElement(val.value, element.value, Nucleus::createConstantInt(i)));
//	}

	Int2 *Int2::getThis()
	{
		return this;
	}

	const Type *Int2::getType()
	{
		return VectorType::get(Int::getType(), 2);
	}

	UInt2::UInt2()
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());
	}

	UInt2::UInt2(unsigned int x, unsigned int y)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[2];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 2), address);
	}

	UInt2::UInt2(const RValue<UInt2> &rhs)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UInt2::UInt2(const UInt2 &rhs)
	{
	//	xy.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<UInt2> UInt2::operator=(const RValue<UInt2> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UInt2> UInt2::operator=(const UInt2 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UInt2>(value);
	}

	RValue<UInt2> operator+(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UInt2> operator-(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UInt2> operator*(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UInt2> operator/(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UInt2> operator%(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UInt2> operator&(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UInt2> operator|(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UInt2> operator^(const RValue<UInt2> &lhs, const RValue<UInt2> &rhs)
	{
		return RValue<UInt2>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UInt2> operator<<(const RValue<UInt2> &lhs, unsigned char rhs)
	{
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
	}

	RValue<UInt2> operator>>(const RValue<UInt2> &lhs, unsigned char rhs)
	{
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
	}

	RValue<UInt2> operator<<(const RValue<UInt2> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
	}

	RValue<UInt2> operator>>(const RValue<UInt2> &lhs, const RValue<Long1> &rhs)
	{
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
	}

	RValue<UInt2> operator+=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt2> operator-=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UInt2> operator*=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UInt2> operator/=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UInt2> operator%=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UInt2> operator&=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt2> operator|=(const UInt2 &lhs, const RValue<UInt2> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt2> operator^=(const UInt2 &lhs, const RValue<UInt2> &rhs)
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

	RValue<UInt2> operator<<=(const UInt2 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs << rhs;
	}

	RValue<UInt2> operator>>=(const UInt2 &lhs, const RValue<Long1> &rhs)
	{
		return lhs = lhs >> rhs;
	}

	RValue<UInt2> operator+(const RValue<UInt2> &val)
	{
		return val;
	}

	RValue<UInt2> operator-(const RValue<UInt2> &val)
	{
		return RValue<UInt2>(Nucleus::createNeg(val.value));
	}

	RValue<UInt2> operator~(const RValue<UInt2> &val)
	{
		return RValue<UInt2>(Nucleus::createNot(val.value));
	}

	UInt2 *UInt2::getThis()
	{
		return this;
	}

	const Type *UInt2::getType()
	{
		return Int2::getType();
	}

	Int4::Int4(const RValue<Float4> &cast)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *xyzw = Nucleus::createFPToSI(cast.value, Int4::getType());
	//	Value *xyzw = x86::cvttps2dq(cast).value;

		Nucleus::createStore(xyzw, address);
	}

	Int4::Int4()
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());
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
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		constantVector[2] = Nucleus::createConstantInt(z);
		constantVector[3] = Nucleus::createConstantInt(w);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 4), address);
	}

	Int4::Int4(const RValue<Int4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Int4::Int4(const Int4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Int4> Int4::operator=(const RValue<Int4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Int4> Int4::operator=(const Int4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Int4>(value);
	}

	RValue<Int4> operator+(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<Int4> operator-(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<Int4> operator*(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<Int4> operator/(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createSDiv(lhs.value, rhs.value));
	}

	RValue<Int4> operator%(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createSRem(lhs.value, rhs.value));
	}

	RValue<Int4> operator&(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<Int4> operator|(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<Int4> operator^(const RValue<Int4> &lhs, const RValue<Int4> &rhs)
	{
		return RValue<Int4>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<Int4> operator<<(const RValue<Int4> &lhs, unsigned char rhs)
	{
	//	return RValue<Int4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
	}

	RValue<Int4> operator>>(const RValue<Int4> &lhs, unsigned char rhs)
	{
	//	return RValue<Int4>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
	}

	RValue<Int4> operator+=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Int4> operator-=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Int4> operator*=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Int4> operator/=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Int4> operator%=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Int4> operator&=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<Int4> operator|=(const Int4 &lhs, const RValue<Int4> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<Int4> operator^=(const Int4 &lhs, const RValue<Int4> &rhs)
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

	RValue<Int4> operator+(const RValue<Int4> &val)
	{
		return val;
	}

	RValue<Int4> operator-(const RValue<Int4> &val)
	{
		return RValue<Int4>(Nucleus::createNeg(val.value));
	}

	RValue<Int4> operator~(const RValue<Int4> &val)
	{
		return RValue<Int4>(Nucleus::createNot(val.value));
	}

	RValue<Int4> RoundInt(const RValue<Float4> &cast)
	{
		return x86::cvtps2dq(cast);
	}

	RValue<Short8> Pack(const RValue<Int4> &x, const RValue<Int4> &y)
	{
		return x86::packssdw(x, y);
	}

	RValue<Int4> Concatenate(const RValue<Int2> &lo, const RValue<Int2> &hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *int4 = Nucleus::createBitCast(long2, Int4::getType());

		return RValue<Int4>(int4);
	}

	RValue<Int> Extract(const RValue<Int4> &x, int i)
	{
		return RValue<Int>(Nucleus::createExtractElement(x.value, i));
	}

	RValue<Int4> Insert(const RValue<Int4> &x, const RValue<Int> &element, int i)
	{
		return RValue<Int4>(Nucleus::createInsertElement(x.value, element.value, i));
	}

	RValue<Int> SignMask(const RValue<Int4> &x)
	{
		return x86::movmskps(As<Float4>(x));
	}

	RValue<Int4> Swizzle(const RValue<Int4> &x, unsigned char select)
	{
		return RValue<Int4>(Nucleus::createSwizzle(x.value, select));
	}

	Int4 *Int4::getThis()
	{
		return this;
	}

	const Type *Int4::getType()
	{
		return VectorType::get(Int::getType(), 4);
	}

	UInt4::UInt4(const RValue<Float4> &cast)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *xyzw = Nucleus::createFPToUI(cast.value, UInt4::getType());

		Nucleus::createStore(xyzw, address);
	}

	UInt4::UInt4()
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());
	}

	UInt4::UInt4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantInt(x);
		constantVector[1] = Nucleus::createConstantInt(y);
		constantVector[2] = Nucleus::createConstantInt(z);
		constantVector[3] = Nucleus::createConstantInt(w);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 4), address);
	}

	UInt4::UInt4(const RValue<UInt4> &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	UInt4::UInt4(const UInt4 &rhs)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<UInt4> UInt4::operator=(const RValue<UInt4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<UInt4> UInt4::operator=(const UInt4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<UInt4>(value);
	}

	RValue<UInt4> operator+(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createAdd(lhs.value, rhs.value));
	}

	RValue<UInt4> operator-(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createSub(lhs.value, rhs.value));
	}

	RValue<UInt4> operator*(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createMul(lhs.value, rhs.value));
	}

	RValue<UInt4> operator/(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createUDiv(lhs.value, rhs.value));
	}

	RValue<UInt4> operator%(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createURem(lhs.value, rhs.value));
	}

	RValue<UInt4> operator&(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createAnd(lhs.value, rhs.value));
	}

	RValue<UInt4> operator|(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createOr(lhs.value, rhs.value));
	}

	RValue<UInt4> operator^(const RValue<UInt4> &lhs, const RValue<UInt4> &rhs)
	{
		return RValue<UInt4>(Nucleus::createXor(lhs.value, rhs.value));
	}

	RValue<UInt4> operator<<(const RValue<UInt4> &lhs, unsigned char rhs)
	{
	//	return RValue<UInt4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt4>(x86::pslld(As<Int4>(lhs), rhs));
	}

	RValue<UInt4> operator>>(const RValue<UInt4> &lhs, unsigned char rhs)
	{
	//	return RValue<UInt4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
	}

	RValue<UInt4> operator+=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<UInt4> operator-=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<UInt4> operator*=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<UInt4> operator/=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<UInt4> operator%=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<UInt4> operator&=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs & rhs;
	}

	RValue<UInt4> operator|=(const UInt4 &lhs, const RValue<UInt4> &rhs)
	{
		return lhs = lhs | rhs;
	}

	RValue<UInt4> operator^=(const UInt4 &lhs, const RValue<UInt4> &rhs)
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

	RValue<UInt4> operator+(const RValue<UInt4> &val)
	{
		return val;
	}

	RValue<UInt4> operator-(const RValue<UInt4> &val)
	{
		return RValue<UInt4>(Nucleus::createNeg(val.value));
	}

	RValue<UInt4> operator~(const RValue<UInt4> &val)
	{
		return RValue<UInt4>(Nucleus::createNot(val.value));
	}

	RValue<UShort8> Pack(const RValue<UInt4> &x, const RValue<UInt4> &y)
	{
		return x86::packusdw(x, y);   // FIXME: Fallback required
	}

	RValue<UInt4> Concatenate(const RValue<UInt2> &lo, const RValue<UInt2> &hi)
	{
		Value *loLong = Nucleus::createBitCast(lo.value, Long::getType());
		Value *hiLong = Nucleus::createBitCast(hi.value, Long::getType());

		Value *long2 = UndefValue::get(Long2::getType());
		long2 = Nucleus::createInsertElement(long2, loLong, 0);
		long2 = Nucleus::createInsertElement(long2, hiLong, 1);
		Value *uint4 = Nucleus::createBitCast(long2, Int4::getType());

		return RValue<UInt4>(uint4);
	}

	UInt4 *UInt4::getThis()
	{
		return this;
	}

	const Type *UInt4::getType()
	{
		return VectorType::get(UInt::getType(), 4);
	}

	Float::Float(const RValue<Int> &cast)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *integer = Nucleus::createSIToFP(cast.value, Float::getType());

		Nucleus::createStore(integer, address);
	}

	Float::Float()
	{
		address = Nucleus::allocateStackVariable(getType());
	}

	Float::Float(float x)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(Nucleus::createConstantFloat(x), address);
	}

	Float::Float(const RValue<Float> &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Float::Float(const Float &rhs)
	{
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	RValue<Float> Float::operator=(const RValue<Float> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Float> Float::operator=(const Float &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Float>(value);
	}

	RValue<Pointer<Float>> Float::operator&()
	{
		return RValue<Pointer<Float>>(address);
	}

	RValue<Float> operator+(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Float>(Nucleus::createFAdd(lhs.value, rhs.value));
	}

	RValue<Float> operator-(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Float>(Nucleus::createFSub(lhs.value, rhs.value));
	}

	RValue<Float> operator*(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Float>(Nucleus::createFMul(lhs.value, rhs.value));
	}

	RValue<Float> operator/(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Float>(Nucleus::createFDiv(lhs.value, rhs.value));
	}

	RValue<Float> operator+=(const Float &lhs, const RValue<Float> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Float> operator-=(const Float &lhs, const RValue<Float> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Float> operator*=(const Float &lhs, const RValue<Float> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Float> operator/=(const Float &lhs, const RValue<Float> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Float> operator+(const RValue<Float> &val)
	{
		return val;
	}

	RValue<Float> operator-(const RValue<Float> &val)
	{
		return RValue<Float>(Nucleus::createFNeg(val.value));
	}

	RValue<Bool> operator<(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOLT(lhs.value, rhs.value));
	}

	RValue<Bool> operator<=(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOLE(lhs.value, rhs.value));
	}

	RValue<Bool> operator>(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOGT(lhs.value, rhs.value));
	}

	RValue<Bool> operator>=(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOGE(lhs.value, rhs.value));
	}

	RValue<Bool> operator!=(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpONE(lhs.value, rhs.value));
	}

	RValue<Bool> operator==(const RValue<Float> &lhs, const RValue<Float> &rhs)
	{
		return RValue<Bool>(Nucleus::createFCmpOEQ(lhs.value, rhs.value));
	}

	RValue<Float> Abs(const RValue<Float> &x)
	{
		return IfThenElse(x > Float(0), x, -x);
	}

	RValue<Float> Max(const RValue<Float> &x, const RValue<Float> &y)
	{
		return IfThenElse(x > y, x, y);
	}

	RValue<Float> Min(const RValue<Float> &x, const RValue<Float> &y)
	{
		return IfThenElse(x < y, x, y);
	}

	RValue<Float> Rcp_pp(const RValue<Float> &x)
	{
		return x86::rcpss(x);
	}
	
	RValue<Float> RcpSqrt_pp(const RValue<Float> &x)
	{
		return x86::rsqrtss(x);
	}

	RValue<Float> Sqrt(const RValue<Float> &x)
	{
		return x86::sqrtss(x);
	}

	RValue<Float> Fraction(const RValue<Float> &x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x - x86::floorss(x);
		}
		else
		{
			return Float4(Fraction(Float4(x))).x;
		}
	}

	RValue<Float> Floor(const RValue<Float> &x)
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

	Float *Float::getThis()
	{
		return this;
	}

	const Type *Float::getType()
	{
		return Type::getFloatTy(*Nucleus::getContext());
	}

	Float2::Float2(const RValue<Float4> &cast)
	{
	//	xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *int64x2 = Nucleus::createBitCast(cast.value, Long2::getType());
		Value *int64 = Nucleus::createExtractElement(int64x2, 0);
		Value *float2 = Nucleus::createBitCast(int64, Float2::getType());

		Nucleus::createStore(float2, address);
	}

	Float2 *Float2::getThis()
	{
		return this;
	}

	const Type *Float2::getType()
	{
		return VectorType::get(Float::getType(), 2);
	}

	Float4::Float4(const RValue<Byte4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		#if 0
			Value *xyzw = Nucleus::createUIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = Nucleus::createLoad(address);

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
		//	Value *g = Nucleus::createSIToFP(f, Float4::getType());
			Value *g = x86::cvtdq2ps(RValue<Int4>(f)).value;
			Value *xyzw = g;
		#endif
		
		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4(const RValue<SByte4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		#if 0
			Value *xyzw = Nucleus::createSIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = Nucleus::createLoad(address);

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

		//	Value *h = Nucleus::createSIToFP(g, Float4::getType());
			Value *h = x86::cvtdq2ps(RValue<Int4>(g)).value;
			Value *xyzw = h;
		#endif
		
		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4(const RValue<Short4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		#if 0
			Value *xyzw = Nucleus::createSIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = Nucleus::createLoad(address);

			Value *i16x = Nucleus::createExtractElement(cast.value, 0);
			Value *f32x = Nucleus::createSIToFP(i16x, Float::getType());
			Value *x = Nucleus::createInsertElement(vector, f32x, 0);

			Value *i16y = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(1));
			Value *f32y = Nucleus::createSIToFP(i16y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, Nucleus::createConstantInt(1));

			Value *i16z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createSIToFP(i16z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i16w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createSIToFP(i16w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *long2 = UndefValue::get(Long2::getType());
			Value *element = Nucleus::createBitCast(cast.value, Long::getType());
			long2 = Nucleus::createInsertElement(long2, element, 0);
			RValue<Int4> vector = RValue<Int4>(Nucleus::createBitCast(long2, Int4::getType()));

			Value *xyzw;

			if(CPUID::supportsSSE4_1())
			{
				Value *c = x86::pmovsxwd(vector).value;

				// xyzw = Nucleus::createSIToFP(d, Float4::getType());
				xyzw = x86::cvtdq2ps(RValue<Int4>(c)).value;
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

				// Value *e = Nucleus::createSIToFP(d, Float4::getType());
				Value *e = x86::cvtdq2ps(RValue<Int4>(d)).value;
				
				Constant *constantVector[4];
				constantVector[0] = Nucleus::createConstantFloat(1.0f / (1 << 16));
				constantVector[1] = Nucleus::createConstantFloat(1.0f / (1 << 16));
				constantVector[2] = Nucleus::createConstantFloat(1.0f / (1 << 16));
				constantVector[3] = Nucleus::createConstantFloat(1.0f / (1 << 16));

				xyzw = Nucleus::createFMul(e, Nucleus::createConstantVector(constantVector, 4));
			}
		#endif
		
		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4(const RValue<UShort4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		#if 0
			Value *xyzw = Nucleus::createUIToFP(cast.value, Float4::getType());   // FIXME: Crashes
		#elif 0
			Value *vector = Nucleus::createLoad(address);

			Value *i16x = Nucleus::createExtractElement(cast.value, 0);
			Value *f32x = Nucleus::createUIToFP(i16x, Float::getType());
			Value *x = Nucleus::createInsertElement(vector, f32x, 0);

			Value *i16y = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(1));
			Value *f32y = Nucleus::createUIToFP(i16y, Float::getType());
			Value *xy = Nucleus::createInsertElement(x, f32y, Nucleus::createConstantInt(1));

			Value *i16z = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(2));
			Value *f32z = Nucleus::createUIToFP(i16z, Float::getType());
			Value *xyz = Nucleus::createInsertElement(xy, f32z, Nucleus::createConstantInt(2));

			Value *i16w = Nucleus::createExtractElement(cast.value, Nucleus::createConstantInt(3));
			Value *f32w = Nucleus::createUIToFP(i16w, Float::getType());
			Value *xyzw = Nucleus::createInsertElement(xyz, f32w, Nucleus::createConstantInt(3));
		#else
			Value *long2 = UndefValue::get(Long2::getType());
			Value *element = Nucleus::createBitCast(cast.value, Long::getType());
			long2 = Nucleus::createInsertElement(long2, element, 0);
			RValue<Int4> vector = RValue<Int4>(Nucleus::createBitCast(long2, Int4::getType()));

			Value *c;
				
			if(CPUID::supportsSSE4_1())
			{
				c = x86::pmovzxwd(RValue<Int4>(vector)).value;
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

				c = Nucleus::createShuffleVector(b, Nucleus::createNullValue(Short8::getType()), Nucleus::createConstantVector(swizzle, 8));
			}

			Value *d = Nucleus::createBitCast(c, Int4::getType());
		//	Value *e = Nucleus::createSIToFP(d, Float4::getType());
			Value *e = x86::cvtdq2ps(RValue<Int4>(d)).value;
			Value *xyzw = e;
		#endif
		
		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4(const RValue<Int4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *xyzw = Nucleus::createSIToFP(cast.value, Float4::getType());
	//	Value *xyzw = x86::cvtdq2ps(cast).value;

		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4(const RValue<UInt4> &cast)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *xyzw = Nucleus::createUIToFP(cast.value, Float4::getType());

		Nucleus::createStore(xyzw, address);
	}

	Float4::Float4()
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());
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
		address = Nucleus::allocateStackVariable(getType());

		Constant *constantVector[4];
		constantVector[0] = Nucleus::createConstantFloat(x);
		constantVector[1] = Nucleus::createConstantFloat(y);
		constantVector[2] = Nucleus::createConstantFloat(z);
		constantVector[3] = Nucleus::createConstantFloat(w);

		Nucleus::createStore(Nucleus::createConstantVector(constantVector, 4), address);
	}

	Float4::Float4(const RValue<Float4> &rhs)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Nucleus::createStore(rhs.value, address);
	}

	Float4::Float4(const Float4 &rhs)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);
	}

	Float4::Float4(const RValue<Float> &rhs)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *vector = Nucleus::createLoad(address);
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		Constant *swizzle[4];
		swizzle[0] = Nucleus::createConstantInt(0);
		swizzle[1] = Nucleus::createConstantInt(0);
		swizzle[2] = Nucleus::createConstantInt(0);
		swizzle[3] = Nucleus::createConstantInt(0);

		Value *replicate = Nucleus::createShuffleVector(insert, UndefValue::get(Float4::getType()), Nucleus::createConstantVector(swizzle, 4));

		Nucleus::createStore(replicate, address);
	}

	Float4::Float4(const Float &rhs)
	{
		xyzw.parent = this;
		address = Nucleus::allocateStackVariable(getType());

		Value *vector = Nucleus::createLoad(address);
		Value *element = Nucleus::createLoad(rhs.address);
		Value *insert = Nucleus::createInsertElement(vector, element, 0);

		Constant *swizzle[4];
		swizzle[0] = Nucleus::createConstantInt(0);
		swizzle[1] = Nucleus::createConstantInt(0);
		swizzle[2] = Nucleus::createConstantInt(0);
		swizzle[3] = Nucleus::createConstantInt(0);

		Value *replicate = Nucleus::createShuffleVector(insert, UndefValue::get(Float4::getType()), Nucleus::createConstantVector(swizzle, 4));

		Nucleus::createStore(replicate, address);
	}

	RValue<Float4> Float4::operator=(float x) const
	{
		return *this = Float4(x, x, x, x);
	}

	RValue<Float4> Float4::operator=(const RValue<Float4> &rhs) const
	{
		Nucleus::createStore(rhs.value, address);

		return rhs;
	}

	RValue<Float4> Float4::operator=(const Float4 &rhs) const
	{
		Value *value = Nucleus::createLoad(rhs.address);
		Nucleus::createStore(value, address);

		return RValue<Float4>(value);
	}

	RValue<Float4> Float4::operator=(const RValue<Float> &rhs) const
	{
		return *this = Float4(rhs);
	}

	RValue<Float4> Float4::operator=(const Float &rhs) const
	{
		return *this = Float4(rhs);
	}

	RValue<Pointer<Float4>> Float4::operator&()
	{
		return RValue<Pointer<Float4>>(address);
	}

	RValue<Float4> operator+(const RValue<Float4> &lhs, const RValue<Float4> &rhs)
	{
		return RValue<Float4>(Nucleus::createFAdd(lhs.value, rhs.value));
	}

	RValue<Float4> operator-(const RValue<Float4> &lhs, const RValue<Float4> &rhs)
	{
		return RValue<Float4>(Nucleus::createFSub(lhs.value, rhs.value));
	}

	RValue<Float4> operator*(const RValue<Float4> &lhs, const RValue<Float4> &rhs)
	{
		return RValue<Float4>(Nucleus::createFMul(lhs.value, rhs.value));
	}

	RValue<Float4> operator/(const RValue<Float4> &lhs, const RValue<Float4> &rhs)
	{
		return RValue<Float4>(Nucleus::createFDiv(lhs.value, rhs.value));
	}

	RValue<Float4> operator%(const RValue<Float4> &lhs, const RValue<Float4> &rhs)
	{
		return RValue<Float4>(Nucleus::createFRem(lhs.value, rhs.value));
	}

	RValue<Float4> operator+=(const Float4 &lhs, const RValue<Float4> &rhs)
	{
		return lhs = lhs + rhs;
	}

	RValue<Float4> operator-=(const Float4 &lhs, const RValue<Float4> &rhs)
	{
		return lhs = lhs - rhs;
	}

	RValue<Float4> operator*=(const Float4 &lhs, const RValue<Float4> &rhs)
	{
		return lhs = lhs * rhs;
	}

	RValue<Float4> operator/=(const Float4 &lhs, const RValue<Float4> &rhs)
	{
		return lhs = lhs / rhs;
	}

	RValue<Float4> operator%=(const Float4 &lhs, const RValue<Float4> &rhs)
	{
		return lhs = lhs % rhs;
	}

	RValue<Float4> operator+(const RValue<Float4> &val)
	{
		return val;
	}

	RValue<Float4> operator-(const RValue<Float4> &val)
	{
		return RValue<Float4>(Nucleus::createFNeg(val.value));
	}

	RValue<Float4> Abs(const RValue<Float4> &x)
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

	RValue<Float4> Max(const RValue<Float4> &x, const RValue<Float4> &y)
	{
		return x86::maxps(x, y);
	}

	RValue<Float4> Min(const RValue<Float4> &x, const RValue<Float4> &y)
	{
		return x86::minps(x, y);
	}

	RValue<Float4> Rcp_pp(const RValue<Float4> &x)
	{
		return x86::rcpps(x);
	}
	
	RValue<Float4> RcpSqrt_pp(const RValue<Float4> &x)
	{
		return x86::rsqrtps(x);
	}

	RValue<Float4> Sqrt(const RValue<Float4> &x)
	{
		return x86::sqrtps(x);
	}

	RValue<Float4> Insert(const Float4 &val, const RValue<Float> &element, int i)
	{
		llvm::Value *value = Nucleus::createLoad(val.address);
		llvm::Value *insert = Nucleus::createInsertElement(value, element.value, i);

		val = RValue<Float4>(insert);

		return val;
	}

	RValue<Float> Extract(const RValue<Float4> &x, int i)
	{
		return RValue<Float>(Nucleus::createExtractElement(x.value, i));
	}

	RValue<Float4> Swizzle(const RValue<Float4> &x, unsigned char select)
	{
		return RValue<Float4>(Nucleus::createSwizzle(x.value, select));
	}

	RValue<Float4> ShuffleLowHigh(const RValue<Float4> &x, const RValue<Float4> &y, unsigned char imm)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(((imm >> 0) & 0x03) + 0);
		shuffle[1] = Nucleus::createConstantInt(((imm >> 2) & 0x03) + 0);
		shuffle[2] = Nucleus::createConstantInt(((imm >> 4) & 0x03) + 4);
		shuffle[3] = Nucleus::createConstantInt(((imm >> 6) & 0x03) + 4);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}

	RValue<Float4> UnpackLow(const RValue<Float4> &x, const RValue<Float4> &y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(0);
		shuffle[1] = Nucleus::createConstantInt(4);
		shuffle[2] = Nucleus::createConstantInt(1);
		shuffle[3] = Nucleus::createConstantInt(5);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}

	RValue<Float4> UnpackHigh(const RValue<Float4> &x, const RValue<Float4> &y)
	{
		Constant *shuffle[4];
		shuffle[0] = Nucleus::createConstantInt(2);
		shuffle[1] = Nucleus::createConstantInt(6);
		shuffle[2] = Nucleus::createConstantInt(3);
		shuffle[3] = Nucleus::createConstantInt(7);

		return RValue<Float4>(Nucleus::createShuffleVector(x.value, y.value, Nucleus::createConstantVector(shuffle, 4)));
	}
	
	RValue<Float4> Mask(Float4 &lhs, const RValue<Float4> &rhs, unsigned char select)
	{
		Value *vector = Nucleus::createLoad(lhs.address);
		Value *shuffle = Nucleus::createMask(vector, rhs.value, select);
		Nucleus::createStore(shuffle, lhs.address);

		return RValue<Float4>(shuffle);
	}

	RValue<Int> SignMask(const RValue<Float4> &x)
	{
		return x86::movmskps(x);
	}

	RValue<Int4> CmpEQ(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpeqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpneqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpONE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpnltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLE(const RValue<Float4> &x, const RValue<Float4> &y)
	{
	//	return As<Int4>(x86::cmpnleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGT(x.value, y.value), Int4::getType()));
	}

	RValue<Float4> Fraction(const RValue<Float4> &x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x - x86::floorps(x);
		}
		else
		{
			Float4 frc = x - Float4(Int4(x));   // Signed fraction

			return frc + As<Float4>(As<Int4>(CmpNLE(Float4(0, 0, 0, 0), frc)) & As<Int4>(Float4(1, 1, 1, 1)));
		}
	}

	RValue<Float4> Floor(const RValue<Float4> &x)
	{
		if(CPUID::supportsSSE4_1())
		{
			return x86::floorps(x);
		}
		else
		{
			Float4 trunc = Float4(Int4(x));   // Rounded toward zero

			return trunc + As<Float4>(As<Int4>(CmpNLE(Float4(0, 0, 0, 0), trunc)) & As<Int4>(Float4(1, 1, 1, 1)));
		}
	}

	Float4 *Float4::getThis()
	{
		return this;
	}

	const Type *Float4::getType()
	{
		return VectorType::get(Float::getType(), 4);
	}

	RValue<Pointer<Byte>> operator+(const RValue<Pointer<Byte>> &lhs, int offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, Nucleus::createConstantInt(offset)));
	}

	RValue<Pointer<Byte>> operator+(const RValue<Pointer<Byte>> &lhs, const RValue<Int> &offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, offset.value));
	}

	RValue<Pointer<Byte>> operator+(const RValue<Pointer<Byte>> &lhs, const RValue<UInt> &offset)
	{
		return RValue<Pointer<Byte>>(Nucleus::createGEP(lhs.value, offset.value));
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, int offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, const RValue<Int> &offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, const RValue<UInt> &offset)
	{
		return lhs = lhs + offset;
	}

	RValue<Pointer<Byte>> operator-(const RValue<Pointer<Byte>> &lhs, int offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-(const RValue<Pointer<Byte>> &lhs, const RValue<Int> &offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-(const RValue<Pointer<Byte>> &lhs, const RValue<UInt> &offset)
	{
		return lhs + -offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, int offset)
	{
		return lhs = lhs - offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, const RValue<Int> &offset)
	{
		return lhs = lhs - offset;
	}

	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, const RValue<UInt> &offset)
	{
		return lhs = lhs - offset;
	}

	void Return()
	{
		#if !(defined(_M_AMD64) || defined(_M_X64))
			x86::emms();
		#endif

		Nucleus::createRetVoid();
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
	}

	void Return(const Int &ret)
	{
		#if !(defined(_M_AMD64) || defined(_M_X64))
			x86::emms();
		#endif

		Nucleus::createRet(Nucleus::createLoad(ret.address));
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
	}

	BasicBlock *beginLoop()
	{
		BasicBlock *loopBB = Nucleus::createBasicBlock();

		Nucleus::createBr(loopBB);
		Nucleus::getBuilder()->SetInsertPoint(loopBB);

		return loopBB;
	}

	bool branch(const RValue<Bool> &cmp, BasicBlock *bodyBB, BasicBlock *endBB)
	{
		Nucleus::createCondBr(cmp.value, bodyBB, endBB);
		Nucleus::getBuilder()->SetInsertPoint(bodyBB);
		
		return true;
	}

	bool elseBlock(BasicBlock *falseBB)
	{
		falseBB->back().eraseFromParent();
		Nucleus::getBuilder()->SetInsertPoint(falseBB);

		return true;
	}

	RValue<Long> Ticks()
	{
		Module *module = Nucleus::getModule();
		llvm::Function *rdtsc = Intrinsic::getDeclaration(module, Intrinsic::readcyclecounter);

		return RValue<Long>(Nucleus::createCall(rdtsc));
	}

	void Emms()
	{
		x86::emms();
	}
}

namespace sw
{
	namespace x86
	{
		RValue<Int> cvtss2si(const RValue<Float> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvtss2si = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvtss2si);
			
			Float4 vector;
			vector.x = val;

			return RValue<Int>(Nucleus::createCall(cvtss2si, RValue<Float4>(vector).value));
		}

		RValue<Int2> cvtps2pi(const RValue<Float4> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvtps2pi = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvtps2pi);

			return RValue<Int2>(Nucleus::createCall(cvtps2pi, val.value));
		}

		RValue<Int2> cvttps2pi(const RValue<Float4> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvttps2pi = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvttps2pi);

			return RValue<Int2>(Nucleus::createCall(cvttps2pi, val.value));
		}

		RValue<Int4> cvtps2dq(const RValue<Float4> &val)
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
				
				return Concatenate(lo, hi);
			}
		}

		RValue<Int4> cvttps2dq(const RValue<Float4> &val)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *cvttps2dq = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_cvttps2dq);

				return RValue<Int4>(Nucleus::createCall(cvttps2dq, val.value));
			}
			else
			{
				Int2 lo = x86::cvttps2pi(val);
				Int2 hi = x86::cvttps2pi(Swizzle(val, 0xEE));
				
				return Concatenate(lo, hi);
			}
		}

		RValue<Float4> cvtpi2ps(const RValue<Float4> &x, const RValue<Int2> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cvtpi2ps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cvtpi2ps);

			return RValue<Float4>(Nucleus::createCall(cvtpi2ps, x.value, y.value));
		}

		RValue<Float4> cvtdq2ps(const RValue<Int4> &val)
		{
			if(CPUID::supportsSSE2())
			{
				Module *module = Nucleus::getModule();
				llvm::Function *cvtdq2ps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_cvtdq2ps);

				return RValue<Float4>(Nucleus::createCall(cvtdq2ps, val.value));
			}
			else
			{
				Int2 lo = Int2(val);
				Int2 hi = Int2(Swizzle(val, 0xEE));

				Float4 scratch1;
				Float4 scratch2;

				return Float4(Float4(x86::cvtpi2ps(scratch1, lo)).xy, Float4(x86::cvtpi2ps(scratch2, hi)).xy);
			}
		}

		RValue<Float> rcpss(const RValue<Float> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rcpss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rcp_ss);

			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);
			
			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(rcpss, vector), 0));
		}

		RValue<Float> sqrtss(const RValue<Float> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *sqrtss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_sqrt_ss);

			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);
			
			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(sqrtss, vector), 0));
		}

		RValue<Float> rsqrtss(const RValue<Float> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rsqrtss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rsqrt_ss);
			
			Value *vector = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(rsqrtss, vector), 0));
		}

		RValue<Float4> rcpps(const RValue<Float4> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rcpps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rcp_ps);
			
			return RValue<Float4>(Nucleus::createCall(rcpps, val.value));
		}

		RValue<Float4> sqrtps(const RValue<Float4> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *sqrtps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_sqrt_ps);
			
			return RValue<Float4>(Nucleus::createCall(sqrtps, val.value));
		}

		RValue<Float4> rsqrtps(const RValue<Float4> &val)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *rsqrtps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_rsqrt_ps);
			
			return RValue<Float4>(Nucleus::createCall(rsqrtps, val.value));
		}

		RValue<Float4> maxps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *maxps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_max_ps);

			return RValue<Float4>(Nucleus::createCall(maxps, x.value, y.value));
		}

		RValue<Float4> minps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *minps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_min_ps);

			return RValue<Float4>(Nucleus::createCall(minps, x.value, y.value));
		}

		RValue<Float> roundss(const RValue<Float> &val, unsigned char imm)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *roundss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_round_ss);

			Value *undef = UndefValue::get(Float4::getType());
			Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(roundss, undef, vector, Nucleus::createConstantInt(imm)), 0));
		}

		RValue<Float> floorss(const RValue<Float> &val)
		{
			return roundss(val, 1);
		}

		RValue<Float> ceilss(const RValue<Float> &val)
		{
			return roundss(val, 2);
		}

		RValue<Float4> roundps(const RValue<Float4> &val, unsigned char imm)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *roundps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_round_ps);

			return RValue<Float4>(Nucleus::createCall(roundps, val.value, Nucleus::createConstantInt(imm)));
		}

		RValue<Float4> floorps(const RValue<Float4> &val)
		{
			return roundps(val, 1);
		}

		RValue<Float4> ceilps(const RValue<Float4> &val)
		{
			return roundps(val, 2);
		}

		RValue<Float4> cmpps(const RValue<Float4> &x, const RValue<Float4> &y, unsigned char imm)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cmpps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cmp_ps);

			return RValue<Float4>(Nucleus::createCall(cmpps, x.value, y.value, Nucleus::createConstantByte(imm)));
		}

		RValue<Float4> cmpeqps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 0);
		}

		RValue<Float4> cmpltps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 1);
		}

		RValue<Float4> cmpleps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 2);
		}

		RValue<Float4> cmpunordps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 3);
		}

		RValue<Float4> cmpneqps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 4);
		}

		RValue<Float4> cmpnltps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 5);
		}

		RValue<Float4> cmpnleps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 6);
		}

		RValue<Float4> cmpordps(const RValue<Float4> &x, const RValue<Float4> &y)
		{
			return cmpps(x, y, 7);
		}

		RValue<Float> cmpss(const RValue<Float> &x, const RValue<Float> &y, unsigned char imm)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *cmpss = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_cmp_ss);

			Value *vector1 = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), x.value, 0);
			Value *vector2 = Nucleus::createInsertElement(UndefValue::get(Float4::getType()), y.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(Nucleus::createCall(cmpss, vector1, vector2, Nucleus::createConstantByte(imm)), 0));
		}

		RValue<Float> cmpeqss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 0);
		}

		RValue<Float> cmpltss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 1);
		}

		RValue<Float> cmpless(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 2);
		}

		RValue<Float> cmpunordss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 3);
		}

		RValue<Float> cmpneqss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 4);
		}

		RValue<Float> cmpnltss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 5);
		}

		RValue<Float> cmpnless(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 6);
		}

		RValue<Float> cmpordss(const RValue<Float> &x, const RValue<Float> &y)
		{
			return cmpss(x, y, 7);
		}

		RValue<Int4> pabsd(const RValue<Int4> &x, const RValue<Int4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pabsd = Intrinsic::getDeclaration(module, Intrinsic::x86_ssse3_pabs_d_128);

			return RValue<Int4>(Nucleus::createCall(pabsd, x.value, y.value));
		}

		RValue<Short4> paddsw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padds_w);

			return RValue<Short4>(Nucleus::createCall(paddsw, x.value, y.value));
		}
		
		RValue<Short4> psubsw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubs_w);

			return RValue<Short4>(Nucleus::createCall(psubsw, x.value, y.value));
		}

		RValue<UShort4> paddusw(const RValue<UShort4> &x, const RValue<UShort4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddusw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_paddus_w);

			return RValue<UShort4>(Nucleus::createCall(paddusw, x.value, y.value));
		}
		
		RValue<UShort4> psubusw(const RValue<UShort4> &x, const RValue<UShort4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubusw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubus_w);

			return RValue<UShort4>(Nucleus::createCall(psubusw, x.value, y.value));
		}

		RValue<SByte8> paddsb(const RValue<SByte8> &x, const RValue<SByte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddsb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_padds_b);

			return RValue<SByte8>(Nucleus::createCall(paddsb, x.value, y.value));
		}
		
		RValue<SByte8> psubsb(const RValue<SByte8> &x, const RValue<SByte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubsb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubs_b);

			return RValue<SByte8>(Nucleus::createCall(psubsb, x.value, y.value));
		}
		
		RValue<Byte8> paddusb(const RValue<Byte8> &x, const RValue<Byte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *paddusb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_paddus_b);

			return RValue<Byte8>(Nucleus::createCall(paddusb, x.value, y.value));
		}
		
		RValue<Byte8> psubusb(const RValue<Byte8> &x, const RValue<Byte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psubusb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psubus_b);

			return RValue<Byte8>(Nucleus::createCall(psubusb, x.value, y.value));
		}

		RValue<UShort4> pavgw(const RValue<UShort4> &x, const RValue<UShort4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pavgw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pavg_w);

			return RValue<UShort4>(Nucleus::createCall(pavgw, x.value, y.value));
		}

		RValue<Short4> pmaxsw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaxsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmaxs_w);

			return RValue<Short4>(Nucleus::createCall(pmaxsw, x.value, y.value));
		}

		RValue<Short4> pminsw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pminsw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmins_w);

			return RValue<Short4>(Nucleus::createCall(pminsw, x.value, y.value));
		}

		RValue<Short4> pcmpgtw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpgtw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpgt_w);

			return RValue<Short4>(Nucleus::createCall(pcmpgtw, x.value, y.value));
		}

		RValue<Short4> pcmpeqw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpeqw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpeq_w);

			return RValue<Short4>(Nucleus::createCall(pcmpeqw, x.value, y.value));
		}

		RValue<Byte8> pcmpgtb(const RValue<SByte8> &x, const RValue<SByte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpgtb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpgt_b);

			return RValue<Byte8>(Nucleus::createCall(pcmpgtb, x.value, y.value));
		}

		RValue<Byte8> pcmpeqb(const RValue<Byte8> &x, const RValue<Byte8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pcmpeqb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pcmpeq_b);

			return RValue<Byte8>(Nucleus::createCall(pcmpeqb, x.value, y.value));
		}

		RValue<Short4> packssdw(const RValue<Int2> &x, const RValue<Int2> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *packssdw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packssdw);

			return RValue<Short4>(Nucleus::createCall(packssdw, x.value, y.value));
		}

		RValue<Short8> packssdw(const RValue<Int4> &x, const RValue<Int4> &y)
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
				
				return Concatenate(lo, hi);
			}
		}

		RValue<SByte8> packsswb(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *packsswb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packsswb);

			return RValue<SByte8>(Nucleus::createCall(packsswb, x.value, y.value));
		}

		RValue<Byte8> packuswb(const RValue<UShort4> &x, const RValue<UShort4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *packuswb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_packuswb);

			return RValue<Byte8>(Nucleus::createCall(packuswb, x.value, y.value));
		}

		RValue<UShort8> packusdw(const RValue<UInt4> &x, const RValue<UInt4> &y)
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
				return As<UShort8>(packssdw(As<Int4>(x - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000)), As<Int4>(y - UInt4(0x00008000, 0x00008000, 0x00008000, 0x00008000))) + Short8(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000));
			}
		}

		RValue<UShort4> psrlw(const RValue<UShort4> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrli_w);

			return RValue<UShort4>(Nucleus::createCall(psrlw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<UShort8> psrlw(const RValue<UShort8> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrli_w);

			return RValue<UShort8>(Nucleus::createCall(psrlw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short4> psraw(const RValue<Short4> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrai_w);

			return RValue<Short4>(Nucleus::createCall(psraw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short8> psraw(const RValue<Short8> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_psrai_w);

			return RValue<Short8>(Nucleus::createCall(psraw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short4> psllw(const RValue<Short4> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pslli_w);

			return RValue<Short4>(Nucleus::createCall(psllw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Short8> psllw(const RValue<Short8> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pslli_w);

			return RValue<Short8>(Nucleus::createCall(psllw, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Int2> pslld(const RValue<Int2> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pslld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pslli_d);

			return RValue<Int2>(Nucleus::createCall(pslld, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Int4> pslld(const RValue<Int4> &x, unsigned char y)
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
				
				return Concatenate(lo, hi);
			}
		}

		RValue<Int2> psrad(const RValue<Int2> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrad = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrai_d);

			return RValue<Int2>(Nucleus::createCall(psrad, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<Int4> psrad(const RValue<Int4> &x, unsigned char y)
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
				
				return Concatenate(lo, hi);
			}
		}

		RValue<UInt2> psrld(const RValue<UInt2> &x, unsigned char y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrli_d);

			return RValue<UInt2>(Nucleus::createCall(psrld, x.value, Nucleus::createConstantInt(y)));
		}

		RValue<UInt4> psrld(const RValue<UInt4> &x, unsigned char y)
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
				
				return Concatenate(lo, hi);
			}
		}

		RValue<UShort4> psrlw(const RValue<UShort4> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrlw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrl_w);

			return RValue<UShort4>(Nucleus::createCall(psrlw, x.value, y.value));
		}

		RValue<Short4> psraw(const RValue<Short4> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psraw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psra_w);

			return RValue<Short4>(Nucleus::createCall(psraw, x.value, y.value));
		}

		RValue<Short4> psllw(const RValue<Short4> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psllw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psll_w);

			return RValue<Short4>(Nucleus::createCall(psllw, x.value, y.value));
		}

		RValue<Int2> pslld(const RValue<Int2> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pslld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psll_d);

			return RValue<Int2>(Nucleus::createCall(pslld, x.value, y.value));
		}

		RValue<UInt2> psrld(const RValue<UInt2> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psrl_d);

			return RValue<UInt2>(Nucleus::createCall(psrld, x.value, y.value));
		}

		RValue<Int2> psrad(const RValue<Int2> &x, const RValue<Long1> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *psrld = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_psra_d);

			return RValue<Int2>(Nucleus::createCall(psrld, x.value, y.value));
		}

		RValue<Short4> pmulhw(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmulh_w);

			return RValue<Short4>(Nucleus::createCall(pmulhw, x.value, y.value));
		}

		RValue<UShort4> pmulhuw(const RValue<UShort4> &x, const RValue<UShort4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmulhu_w);

			return RValue<UShort4>(Nucleus::createCall(pmulhuw, x.value, y.value));
		}

		RValue<Int2> pmaddwd(const RValue<Short4> &x, const RValue<Short4> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmadd_wd);

			return RValue<Int2>(Nucleus::createCall(pmaddwd, x.value, y.value));
		}

		RValue<Short8> pmulhw(const RValue<Short8> &x, const RValue<Short8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmulh_w);

			return RValue<Short8>(Nucleus::createCall(pmulhw, x.value, y.value));
		}

		RValue<UShort8> pmulhuw(const RValue<UShort8> &x, const RValue<UShort8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmulhuw = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmulhu_w);

			return RValue<UShort8>(Nucleus::createCall(pmulhuw, x.value, y.value));
		}

		RValue<Int4> pmaddwd(const RValue<Short8> &x, const RValue<Short8> &y)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmaddwd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse2_pmadd_wd);

			return RValue<Int4>(Nucleus::createCall(pmaddwd, x.value, y.value));
		}

		RValue<Int> movmskps(const RValue<Float4> &x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *movmskps = Intrinsic::getDeclaration(module, Intrinsic::x86_sse_movmsk_ps);

			return RValue<Int>(Nucleus::createCall(movmskps, x.value));
		}

		RValue<Int> pmovmskb(const RValue<Byte8> &x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovmskb = Intrinsic::getDeclaration(module, Intrinsic::x86_mmx_pmovmskb);

			return RValue<Int>(Nucleus::createCall(pmovmskb, x.value));
		}

		//RValue<Int2> movd(const RValue<Pointer<Int>> &x)
		//{
		//	Value *element = Nucleus::createLoad(x.value);

		////	Value *int2 = UndefValue::get(Int2::getType());
		////	int2 = Nucleus::createInsertElement(int2, element, ConstantInt::get(Int::getType(), 0));

		//	Value *int2 = Nucleus::createBitCast(Nucleus::createZExt(element, Long::getType()), Int2::getType());

		//	return RValue<Int2>(int2);
		//}

		//RValue<Int2> movdq2q(const RValue<Int4> &x)
		//{
		//	Value *long2 = Nucleus::createBitCast(x.value, Long2::getType());
		//	Value *element = Nucleus::createExtractElement(long2, ConstantInt::get(Int::getType(), 0));

		//	return RValue<Int2>(Nucleus::createBitCast(element, Int2::getType()));
		//}

		RValue<Int4> pmovzxbd(const RValue<Int4> &x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovzxbd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovzxbd);
		
			return RValue<Int4>(Nucleus::createCall(pmovzxbd, Nucleus::createBitCast(x.value, Byte16::getType())));
		}

		RValue<Int4> pmovsxbd(const RValue<Int4> &x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovsxbd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovsxbd);
		
			return RValue<Int4>(Nucleus::createCall(pmovsxbd, Nucleus::createBitCast(x.value, SByte16::getType())));
		}

		RValue<Int4> pmovzxwd(const RValue<Int4> &x)
		{
			Module *module = Nucleus::getModule();
			llvm::Function *pmovzxwd = Intrinsic::getDeclaration(module, Intrinsic::x86_sse41_pmovzxwd);
		
			return RValue<Int4>(Nucleus::createCall(pmovzxwd, Nucleus::createBitCast(x.value, UShort8::getType())));
		}

		RValue<Int4> pmovsxwd(const RValue<Int4> &x)
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
