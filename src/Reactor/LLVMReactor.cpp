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

#include "Reactor.hpp"

#include "x86.hpp"
#include "CPUID.hpp"
#include "Thread.hpp"
#include "ExecutableMemory.hpp"
#include "MutexLock.hpp"

#undef min
#undef max

#if REACTOR_LLVM_VERSION < 7
	#include "llvm/Analysis/LoopPass.h"
	#include "llvm/Constants.h"
	#include "llvm/Function.h"
	#include "llvm/GlobalVariable.h"
	#include "llvm/Intrinsics.h"
	#include "llvm/LLVMContext.h"
	#include "llvm/Module.h"
	#include "llvm/PassManager.h"
	#include "llvm/Support/IRBuilder.h"
	#include "llvm/Support/TargetSelect.h"
	#include "llvm/Target/TargetData.h"
	#include "llvm/Target/TargetOptions.h"
	#include "llvm/Transforms/Scalar.h"
	#include "../lib/ExecutionEngine/JIT/JIT.h"

	#include "LLVMRoutine.hpp"
	#include "LLVMRoutineManager.hpp"

	#define ARGS(...) __VA_ARGS__
#else
	#include "llvm/Analysis/LoopPass.h"
	#include "llvm/ExecutionEngine/ExecutionEngine.h"
	#include "llvm/ExecutionEngine/JITSymbol.h"
	#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
	#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
	#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
	#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
	#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
	#include "llvm/ExecutionEngine/SectionMemoryManager.h"
	#include "llvm/IR/Constants.h"
	#include "llvm/IR/DataLayout.h"
	#include "llvm/IR/Function.h"
	#include "llvm/IR/GlobalVariable.h"
	#include "llvm/IR/IRBuilder.h"
	#include "llvm/IR/Intrinsics.h"
	#include "llvm/IR/LLVMContext.h"
	#include "llvm/IR/LegacyPassManager.h"
	#include "llvm/IR/Mangler.h"
	#include "llvm/IR/Module.h"
	#include "llvm/Support/Error.h"
	#include "llvm/Support/TargetSelect.h"
	#include "llvm/Target/TargetOptions.h"
	#include "llvm/Transforms/InstCombine/InstCombine.h"
	#include "llvm/Transforms/Scalar.h"
	#include "llvm/Transforms/Scalar/GVN.h"

	#include "LLVMRoutine.hpp"

	#define ARGS(...) {__VA_ARGS__}
	#define CreateCall2 CreateCall
	#define CreateCall3 CreateCall

	#include <unordered_map>
#endif

#include <fstream>
#include <numeric>
#include <thread>

#if defined(__i386__) || defined(__x86_64__)
#include <xmmintrin.h>
#endif

#include <math.h>

#if defined(__x86_64__) && defined(_WIN32)
extern "C" void X86CompilationCallback()
{
	assert(false);   // UNIMPLEMENTED
}
#endif

#if REACTOR_LLVM_VERSION < 7
namespace llvm
{
	extern bool JITEmitDebugInfo;
}
#endif

namespace rr
{
	class LLVMReactorJIT;
}

namespace
{
	rr::LLVMReactorJIT *reactorJIT = nullptr;
	llvm::IRBuilder<> *builder = nullptr;
	llvm::LLVMContext *context = nullptr;
	llvm::Module *module = nullptr;
	llvm::Function *function = nullptr;

	rr::MutexLock codegenMutex;

#ifdef ENABLE_RR_PRINT
	std::string replace(std::string str, const std::string& substr, const std::string& replacement)
	{
		size_t pos = 0;
		while((pos = str.find(substr, pos)) != std::string::npos) {
			str.replace(pos, substr.length(), replacement);
			pos += replacement.length();
		}
		return str;
	}
#endif // ENABLE_RR_PRINT

#if REACTOR_LLVM_VERSION >= 7
	llvm::Value *lowerPAVG(llvm::Value *x, llvm::Value *y)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());

		llvm::VectorType *extTy =
			llvm::VectorType::getExtendedElementVectorType(ty);
		x = ::builder->CreateZExt(x, extTy);
		y = ::builder->CreateZExt(y, extTy);

		// (x + y + 1) >> 1
		llvm::Constant *one = llvm::ConstantInt::get(extTy, 1);
		llvm::Value *res = ::builder->CreateAdd(x, y);
		res = ::builder->CreateAdd(res, one);
		res = ::builder->CreateLShr(res, one);
		return ::builder->CreateTrunc(res, ty);
	}

	llvm::Value *lowerPMINMAX(llvm::Value *x, llvm::Value *y,
	                          llvm::ICmpInst::Predicate pred)
	{
		return ::builder->CreateSelect(::builder->CreateICmp(pred, x, y), x, y);
	}

	llvm::Value *lowerPCMP(llvm::ICmpInst::Predicate pred, llvm::Value *x,
	                       llvm::Value *y, llvm::Type *dstTy)
	{
		return ::builder->CreateSExt(::builder->CreateICmp(pred, x, y), dstTy, "");
	}

#if defined(__i386__) || defined(__x86_64__)
	llvm::Value *lowerPMOV(llvm::Value *op, llvm::Type *dstType, bool sext)
	{
		llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(op->getType());
		llvm::VectorType *dstTy = llvm::cast<llvm::VectorType>(dstType);

		llvm::Value *undef = llvm::UndefValue::get(srcTy);
		llvm::SmallVector<uint32_t, 16> mask(dstTy->getNumElements());
		std::iota(mask.begin(), mask.end(), 0);
		llvm::Value *v = ::builder->CreateShuffleVector(op, undef, mask);

		return sext ? ::builder->CreateSExt(v, dstTy)
		            : ::builder->CreateZExt(v, dstTy);
	}

	llvm::Value *lowerPABS(llvm::Value *v)
	{
		llvm::Value *zero = llvm::Constant::getNullValue(v->getType());
		llvm::Value *cmp = ::builder->CreateICmp(llvm::ICmpInst::ICMP_SGT, v, zero);
		llvm::Value *neg = ::builder->CreateNeg(v);
		return ::builder->CreateSelect(cmp, v, neg);
	}
#endif  // defined(__i386__) || defined(__x86_64__)

#if !defined(__i386__) && !defined(__x86_64__)
	llvm::Value *lowerPFMINMAX(llvm::Value *x, llvm::Value *y,
	                           llvm::FCmpInst::Predicate pred)
	{
		return ::builder->CreateSelect(::builder->CreateFCmp(pred, x, y), x, y);
	}

	llvm::Value *lowerRound(llvm::Value *x)
	{
		llvm::Function *nearbyint = llvm::Intrinsic::getDeclaration(
			::module, llvm::Intrinsic::nearbyint, {x->getType()});
		return ::builder->CreateCall(nearbyint, ARGS(x));
	}

	llvm::Value *lowerRoundInt(llvm::Value *x, llvm::Type *ty)
	{
		return ::builder->CreateFPToSI(lowerRound(x), ty);
	}

	llvm::Value *lowerFloor(llvm::Value *x)
	{
		llvm::Function *floor = llvm::Intrinsic::getDeclaration(
			::module, llvm::Intrinsic::floor, {x->getType()});
		return ::builder->CreateCall(floor, ARGS(x));
	}

	llvm::Value *lowerTrunc(llvm::Value *x)
	{
		llvm::Function *trunc = llvm::Intrinsic::getDeclaration(
			::module, llvm::Intrinsic::trunc, {x->getType()});
		return ::builder->CreateCall(trunc, ARGS(x));
	}

	// Packed add/sub saturatation
	llvm::Value *lowerPSAT(llvm::Value *x, llvm::Value *y, bool isAdd, bool isSigned)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

		unsigned numBits = ty->getScalarSizeInBits();

		llvm::Value *max, *min, *extX, *extY;
		if (isSigned)
		{
			max = llvm::ConstantInt::get(extTy, (1LL << (numBits - 1)) - 1, true);
			min = llvm::ConstantInt::get(extTy, (-1LL << (numBits - 1)), true);
			extX = ::builder->CreateSExt(x, extTy);
			extY = ::builder->CreateSExt(y, extTy);
		}
		else
		{
			assert(numBits <= 64);
			uint64_t maxVal = (numBits == 64) ? ~0ULL : (1ULL << numBits) - 1;
			max = llvm::ConstantInt::get(extTy, maxVal, false);
			min = llvm::ConstantInt::get(extTy, 0, false);
			extX = ::builder->CreateZExt(x, extTy);
			extY = ::builder->CreateZExt(y, extTy);
		}

		llvm::Value *res = isAdd ? ::builder->CreateAdd(extX, extY)
		                         : ::builder->CreateSub(extX, extY);

		res = lowerPMINMAX(res, min, llvm::ICmpInst::ICMP_SGT);
		res = lowerPMINMAX(res, max, llvm::ICmpInst::ICMP_SLT);

		return ::builder->CreateTrunc(res, ty);
	}

	llvm::Value *lowerPUADDSAT(llvm::Value *x, llvm::Value *y)
	{
		return lowerPSAT(x, y, true, false);
	}

	llvm::Value *lowerPSADDSAT(llvm::Value *x, llvm::Value *y)
	{
		return lowerPSAT(x, y, true, true);
	}

	llvm::Value *lowerPUSUBSAT(llvm::Value *x, llvm::Value *y)
	{
		return lowerPSAT(x, y, false, false);
	}

	llvm::Value *lowerPSSUBSAT(llvm::Value *x, llvm::Value *y)
	{
		return lowerPSAT(x, y, false, true);
	}

	llvm::Value *lowerSQRT(llvm::Value *x)
	{
		llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(
			::module, llvm::Intrinsic::sqrt, {x->getType()});
		return ::builder->CreateCall(sqrt, ARGS(x));
	}

	llvm::Value *lowerRCP(llvm::Value *x)
	{
		llvm::Type *ty = x->getType();
		llvm::Constant *one;
		if (llvm::VectorType *vectorTy = llvm::dyn_cast<llvm::VectorType>(ty))
		{
			one = llvm::ConstantVector::getSplat(
				vectorTy->getNumElements(),
				llvm::ConstantFP::get(vectorTy->getElementType(), 1));
		}
		else
		{
			one = llvm::ConstantFP::get(ty, 1);
		}
		return ::builder->CreateFDiv(one, x);
	}

	llvm::Value *lowerRSQRT(llvm::Value *x)
	{
		return lowerRCP(lowerSQRT(x));
	}

	llvm::Value *lowerVectorShl(llvm::Value *x, uint64_t scalarY)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Value *y = llvm::ConstantVector::getSplat(
			ty->getNumElements(),
			llvm::ConstantInt::get(ty->getElementType(), scalarY));
		return ::builder->CreateShl(x, y);
	}

	llvm::Value *lowerVectorAShr(llvm::Value *x, uint64_t scalarY)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Value *y = llvm::ConstantVector::getSplat(
			ty->getNumElements(),
			llvm::ConstantInt::get(ty->getElementType(), scalarY));
		return ::builder->CreateAShr(x, y);
	}

	llvm::Value *lowerVectorLShr(llvm::Value *x, uint64_t scalarY)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Value *y = llvm::ConstantVector::getSplat(
			ty->getNumElements(),
			llvm::ConstantInt::get(ty->getElementType(), scalarY));
		return ::builder->CreateLShr(x, y);
	}

	llvm::Value *lowerMulAdd(llvm::Value *x, llvm::Value *y)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

		llvm::Value *extX = ::builder->CreateSExt(x, extTy);
		llvm::Value *extY = ::builder->CreateSExt(y, extTy);
		llvm::Value *mult = ::builder->CreateMul(extX, extY);

		llvm::Value *undef = llvm::UndefValue::get(extTy);

		llvm::SmallVector<uint32_t, 16> evenIdx;
		llvm::SmallVector<uint32_t, 16> oddIdx;
		for (uint64_t i = 0, n = ty->getNumElements(); i < n; i += 2)
		{
			evenIdx.push_back(i);
			oddIdx.push_back(i + 1);
		}

		llvm::Value *lhs = ::builder->CreateShuffleVector(mult, undef, evenIdx);
		llvm::Value *rhs = ::builder->CreateShuffleVector(mult, undef, oddIdx);
		return ::builder->CreateAdd(lhs, rhs);
	}

	llvm::Value *lowerPack(llvm::Value *x, llvm::Value *y, bool isSigned)
	{
		llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *dstTy = llvm::VectorType::getTruncatedElementVectorType(srcTy);

		llvm::IntegerType *dstElemTy =
			llvm::cast<llvm::IntegerType>(dstTy->getElementType());

		uint64_t truncNumBits = dstElemTy->getIntegerBitWidth();
		assert(truncNumBits < 64 && "shift 64 must be handled separately");
		llvm::Constant *max, *min;
		if (isSigned)
		{
			max = llvm::ConstantInt::get(srcTy, (1LL << (truncNumBits - 1)) - 1, true);
			min = llvm::ConstantInt::get(srcTy, (-1LL << (truncNumBits - 1)), true);
		}
		else
		{
			max = llvm::ConstantInt::get(srcTy, (1ULL << truncNumBits) - 1, false);
			min = llvm::ConstantInt::get(srcTy, 0, false);
		}

		x = lowerPMINMAX(x, min, llvm::ICmpInst::ICMP_SGT);
		x = lowerPMINMAX(x, max, llvm::ICmpInst::ICMP_SLT);
		y = lowerPMINMAX(y, min, llvm::ICmpInst::ICMP_SGT);
		y = lowerPMINMAX(y, max, llvm::ICmpInst::ICMP_SLT);

		x = ::builder->CreateTrunc(x, dstTy);
		y = ::builder->CreateTrunc(y, dstTy);

		llvm::SmallVector<uint32_t, 16> index(srcTy->getNumElements() * 2);
		std::iota(index.begin(), index.end(), 0);

		return ::builder->CreateShuffleVector(x, y, index);
	}

	llvm::Value *lowerSignMask(llvm::Value *x, llvm::Type *retTy)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Constant *zero = llvm::ConstantInt::get(ty, 0);
		llvm::Value *cmp = ::builder->CreateICmpSLT(x, zero);

		llvm::Value *ret = ::builder->CreateZExt(
			::builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
		for (uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
		{
			llvm::Value *elem = ::builder->CreateZExt(
				::builder->CreateExtractElement(cmp, i), retTy);
			ret = ::builder->CreateOr(ret, ::builder->CreateShl(elem, i));
		}
		return ret;
	}

	llvm::Value *lowerFPSignMask(llvm::Value *x, llvm::Type *retTy)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Constant *zero = llvm::ConstantFP::get(ty, 0);
		llvm::Value *cmp = ::builder->CreateFCmpULT(x, zero);

		llvm::Value *ret = ::builder->CreateZExt(
			::builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
		for (uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
		{
			llvm::Value *elem = ::builder->CreateZExt(
				::builder->CreateExtractElement(cmp, i), retTy);
			ret = ::builder->CreateOr(ret, ::builder->CreateShl(elem, i));
		}
		return ret;
	}
#endif  // !defined(__i386__) && !defined(__x86_64__)
#endif  // REACTOR_LLVM_VERSION >= 7

	llvm::Value *lowerMulHigh(llvm::Value *x, llvm::Value *y, bool sext)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

		llvm::Value *extX, *extY;
		if (sext)
		{
			extX = ::builder->CreateSExt(x, extTy);
			extY = ::builder->CreateSExt(y, extTy);
		}
		else
		{
			extX = ::builder->CreateZExt(x, extTy);
			extY = ::builder->CreateZExt(y, extTy);
		}

		llvm::Value *mult = ::builder->CreateMul(extX, extY);

		llvm::IntegerType *intTy = llvm::cast<llvm::IntegerType>(ty->getElementType());
		llvm::Value *mulh = ::builder->CreateAShr(mult, intTy->getBitWidth());
		return ::builder->CreateTrunc(mulh, ty);
	}
}

namespace rr
{
#if REACTOR_LLVM_VERSION < 7
	class LLVMReactorJIT
	{
	private:
		std::string arch;
		llvm::SmallVector<std::string, 16> mattrs;
		llvm::ExecutionEngine *executionEngine;
		LLVMRoutineManager *routineManager;

	public:
		LLVMReactorJIT(const std::string &arch_,
		               const llvm::SmallVectorImpl<std::string> &mattrs_) :
			arch(arch_),
			mattrs(mattrs_.begin(), mattrs_.end()),
			executionEngine(nullptr),
			routineManager(nullptr)
		{
		}

		void startSession()
		{
			std::string error;

			::module = new llvm::Module("", *::context);

			routineManager = new LLVMRoutineManager();

			llvm::TargetMachine *targetMachine =
				llvm::EngineBuilder::selectTarget(
					::module, arch, "", mattrs, llvm::Reloc::Default,
					llvm::CodeModel::JITDefault, &error);

			executionEngine = llvm::JIT::createJIT(
				::module, &error, routineManager, llvm::CodeGenOpt::Aggressive,
				true, targetMachine);
		}

		void endSession()
		{
			delete executionEngine;
			executionEngine = nullptr;
			routineManager = nullptr;

			::function = nullptr;
			::module = nullptr;
		}

		LLVMRoutine *acquireRoutine(llvm::Function *func)
		{
			void *entry = executionEngine->getPointerToFunction(::function);
			return routineManager->acquireRoutine(entry);
		}

		void optimize(llvm::Module *module)
		{
			static llvm::PassManager *passManager = nullptr;

			if(!passManager)
			{
				passManager = new llvm::PassManager();

				passManager->add(new llvm::TargetData(*executionEngine->getTargetData()));
				passManager->add(llvm::createScalarReplAggregatesPass());

				for(int pass = 0; pass < 10 && optimization[pass] != Disabled; pass++)
				{
					switch(optimization[pass])
					{
					case Disabled:                                                                       break;
					case CFGSimplification:    passManager->add(llvm::createCFGSimplificationPass());    break;
					case LICM:                 passManager->add(llvm::createLICMPass());                 break;
					case AggressiveDCE:        passManager->add(llvm::createAggressiveDCEPass());        break;
					case GVN:                  passManager->add(llvm::createGVNPass());                  break;
					case InstructionCombining: passManager->add(llvm::createInstructionCombiningPass()); break;
					case Reassociate:          passManager->add(llvm::createReassociatePass());          break;
					case DeadStoreElimination: passManager->add(llvm::createDeadStoreEliminationPass()); break;
					case SCCP:                 passManager->add(llvm::createSCCPPass());                 break;
					case ScalarReplAggregates: passManager->add(llvm::createScalarReplAggregatesPass()); break;
					default:
						assert(false);
					}
				}
			}

			passManager->run(*::module);
		}
	};
#else
	class ExternalFunctionSymbolResolver
	{
	private:
		using FunctionMap = std::unordered_map<std::string, void *>;
		FunctionMap func_;

	public:
		ExternalFunctionSymbolResolver()
		{
			func_.emplace("floorf", reinterpret_cast<void*>(floorf));
			func_.emplace("nearbyintf", reinterpret_cast<void*>(nearbyintf));
			func_.emplace("truncf", reinterpret_cast<void*>(truncf));
			func_.emplace("printf", reinterpret_cast<void*>(printf));
			func_.emplace("puts", reinterpret_cast<void*>(puts));
			func_.emplace("fmodf", reinterpret_cast<void*>(fmodf));
			func_.emplace("sinf", reinterpret_cast<void*>(sinf));
		}

		void *findSymbol(const std::string &name) const
		{
			// Trim off any underscores from the start of the symbol. LLVM likes
			// to append these on macOS.
			const char* trimmed = name.c_str();
			while (trimmed[0] == '_') { trimmed++; }

			FunctionMap::const_iterator it = func_.find(trimmed);
			assert(it != func_.end()); // Missing functions will likely make the module fail in exciting non-obvious ways.
			return it->second;
		}
	};

	class LLVMReactorJIT
	{
	private:
		using ObjLayer = llvm::orc::RTDyldObjectLinkingLayer;
		using CompileLayer = llvm::orc::IRCompileLayer<ObjLayer, llvm::orc::SimpleCompiler>;

		llvm::orc::ExecutionSession session;
		ExternalFunctionSymbolResolver externalSymbolResolver;
		std::shared_ptr<llvm::orc::SymbolResolver> resolver;
		std::unique_ptr<llvm::TargetMachine> targetMachine;
		const llvm::DataLayout dataLayout;
		ObjLayer objLayer;
		CompileLayer compileLayer;
		size_t emittedFunctionsNum;

	public:
		LLVMReactorJIT(const char *arch, const llvm::SmallVectorImpl<std::string>& mattrs,
					   const llvm::TargetOptions &targetOpts):
			resolver(createLegacyLookupResolver(
				session,
				[this](const std::string &name) {
					void *func = externalSymbolResolver.findSymbol(name);
					if (func != nullptr)
					{
						return llvm::JITSymbol(
							reinterpret_cast<uintptr_t>(func), llvm::JITSymbolFlags::Absolute);
					}

					return objLayer.findSymbol(name, true);
				},
				[](llvm::Error err) {
					if (err)
					{
						// TODO: Log the symbol resolution errors.
						return;
					}
				})),
			targetMachine(llvm::EngineBuilder()
				.setMArch(arch)
				.setMAttrs(mattrs)
				.setTargetOptions(targetOpts)
				.selectTarget()),
			dataLayout(targetMachine->createDataLayout()),
			objLayer(
				session,
				[this](llvm::orc::VModuleKey) {
					return ObjLayer::Resources{
						std::make_shared<llvm::SectionMemoryManager>(),
						resolver};
				}),
			compileLayer(objLayer, llvm::orc::SimpleCompiler(*targetMachine)),
			emittedFunctionsNum(0)
		{
		}

		void startSession()
		{
			::module = new llvm::Module("", *::context);
		}

		void endSession()
		{
			::function = nullptr;
			::module = nullptr;
		}

		LLVMRoutine *acquireRoutine(llvm::Function *func)
		{
			std::string name = "f" + llvm::Twine(emittedFunctionsNum++).str();
			func->setName(name);
			func->setLinkage(llvm::GlobalValue::ExternalLinkage);
			func->setDoesNotThrow();

			std::unique_ptr<llvm::Module> mod(::module);
			::module = nullptr;
			mod->setDataLayout(dataLayout);

			auto moduleKey = session.allocateVModule();
			llvm::cantFail(compileLayer.addModule(moduleKey, std::move(mod)));

			std::string mangledName;
			{
				llvm::raw_string_ostream mangledNameStream(mangledName);
				llvm::Mangler::getNameWithPrefix(mangledNameStream, name, dataLayout);
			}

			llvm::JITSymbol symbol = compileLayer.findSymbolIn(moduleKey, mangledName, false);

			llvm::Expected<llvm::JITTargetAddress> expectAddr = symbol.getAddress();
			if(!expectAddr)
			{
				return nullptr;
			}

			void *addr = reinterpret_cast<void *>(static_cast<intptr_t>(expectAddr.get()));
			return new LLVMRoutine(addr, releaseRoutineCallback, this, moduleKey);
		}

		void optimize(llvm::Module *module)
		{
			std::unique_ptr<llvm::legacy::PassManager> passManager(
				new llvm::legacy::PassManager());

			passManager->add(llvm::createSROAPass());

			for(int pass = 0; pass < 10 && optimization[pass] != Disabled; pass++)
			{
				switch(optimization[pass])
				{
				case Disabled:                                                                       break;
				case CFGSimplification:    passManager->add(llvm::createCFGSimplificationPass());    break;
				case LICM:                 passManager->add(llvm::createLICMPass());                 break;
				case AggressiveDCE:        passManager->add(llvm::createAggressiveDCEPass());        break;
				case GVN:                  passManager->add(llvm::createGVNPass());                  break;
				case InstructionCombining: passManager->add(llvm::createInstructionCombiningPass()); break;
				case Reassociate:          passManager->add(llvm::createReassociatePass());          break;
				case DeadStoreElimination: passManager->add(llvm::createDeadStoreEliminationPass()); break;
				case SCCP:                 passManager->add(llvm::createSCCPPass());                 break;
				case ScalarReplAggregates: passManager->add(llvm::createSROAPass());                 break;
				default:
				                           assert(false);
				}
			}

			passManager->run(*::module);
		}

	private:
		void releaseRoutineModule(llvm::orc::VModuleKey moduleKey)
		{
			llvm::cantFail(compileLayer.removeModule(moduleKey));
		}

		static void releaseRoutineCallback(LLVMReactorJIT *jit, uint64_t moduleKey)
		{
			jit->releaseRoutineModule(moduleKey);
		}
	};
#endif

	Optimization optimization[10] = {InstructionCombining, Disabled};

	// The abstract Type* types are implemented as LLVM types, except that
	// 64-bit vectors are emulated using 128-bit ones to avoid use of MMX in x86
	// and VFP in ARM, and eliminate the overhead of converting them to explicit
	// 128-bit ones. LLVM types are pointers, so we can represent emulated types
	// as abstract pointers with small enum values.
	enum InternalType : uintptr_t
	{
		// Emulated types:
		Type_v2i32,
		Type_v4i16,
		Type_v2i16,
		Type_v8i8,
		Type_v4i8,
		Type_v2f32,
		EmulatedTypeCount,
		// Returned by asInternalType() to indicate that the abstract Type*
		// should be interpreted as LLVM type pointer:
		Type_LLVM
	};

	inline InternalType asInternalType(Type *type)
	{
		InternalType t = static_cast<InternalType>(reinterpret_cast<uintptr_t>(type));
		return (t < EmulatedTypeCount) ? t : Type_LLVM;
	}

	llvm::Type *T(Type *t)
	{
		// Use 128-bit vectors to implement logically shorter ones.
		switch(asInternalType(t))
		{
		case Type_v2i32: return T(Int4::getType());
		case Type_v4i16: return T(Short8::getType());
		case Type_v2i16: return T(Short8::getType());
		case Type_v8i8:  return T(Byte16::getType());
		case Type_v4i8:  return T(Byte16::getType());
		case Type_v2f32: return T(Float4::getType());
		case Type_LLVM:  return reinterpret_cast<llvm::Type*>(t);
		default: assert(false); return nullptr;
		}
	}

	inline Type *T(llvm::Type *t)
	{
		return reinterpret_cast<Type*>(t);
	}

	Type *T(InternalType t)
	{
		return reinterpret_cast<Type*>(t);
	}

	inline llvm::Value *V(Value *t)
	{
		return reinterpret_cast<llvm::Value*>(t);
	}

	inline Value *V(llvm::Value *t)
	{
		return reinterpret_cast<Value*>(t);
	}

	inline std::vector<llvm::Type*> &T(std::vector<Type*> &t)
	{
		return reinterpret_cast<std::vector<llvm::Type*>&>(t);
	}

	inline llvm::BasicBlock *B(BasicBlock *t)
	{
		return reinterpret_cast<llvm::BasicBlock*>(t);
	}

	inline BasicBlock *B(llvm::BasicBlock *t)
	{
		return reinterpret_cast<BasicBlock*>(t);
	}

	static size_t typeSize(Type *type)
	{
		switch(asInternalType(type))
		{
		case Type_v2i32: return 8;
		case Type_v4i16: return 8;
		case Type_v2i16: return 4;
		case Type_v8i8:  return 8;
		case Type_v4i8:  return 4;
		case Type_v2f32: return 8;
		case Type_LLVM:
			{
				llvm::Type *t = T(type);

				if(t->isPointerTy())
				{
					return sizeof(void*);
				}

				// At this point we should only have LLVM 'primitive' types.
				unsigned int bits = t->getPrimitiveSizeInBits();
				assert(bits != 0);

				// TODO(capn): Booleans are 1 bit integers in LLVM's SSA type system,
				// but are typically stored as one byte. The DataLayout structure should
				// be used here and many other places if this assumption fails.
				return (bits + 7) / 8;
			}
			break;
		default:
			assert(false);
			return 0;
		}
	}

	static unsigned int elementCount(Type *type)
	{
		switch(asInternalType(type))
		{
		case Type_v2i32: return 2;
		case Type_v4i16: return 4;
		case Type_v2i16: return 2;
		case Type_v8i8:  return 8;
		case Type_v4i8:  return 4;
		case Type_v2f32: return 2;
		case Type_LLVM:  return llvm::cast<llvm::VectorType>(T(type))->getNumElements();
		default: assert(false); return 0;
		}
	}

	static llvm::AtomicOrdering atomicOrdering(bool atomic, std::memory_order memoryOrder)
	{
		#if REACTOR_LLVM_VERSION < 7
			return llvm::AtomicOrdering::NotAtomic;
		#endif

		if(!atomic)
		{
			return llvm::AtomicOrdering::NotAtomic;
		}

		switch(memoryOrder)
		{
		case std::memory_order_relaxed: return llvm::AtomicOrdering::Monotonic;  // https://llvm.org/docs/Atomics.html#monotonic
		case std::memory_order_consume: return llvm::AtomicOrdering::Acquire;    // https://llvm.org/docs/Atomics.html#acquire: "It should also be used for C++11/C11 memory_order_consume."
		case std::memory_order_acquire: return llvm::AtomicOrdering::Acquire;
		case std::memory_order_release: return llvm::AtomicOrdering::Release;
		case std::memory_order_acq_rel: return llvm::AtomicOrdering::AcquireRelease;
		case std::memory_order_seq_cst: return llvm::AtomicOrdering::SequentiallyConsistent;
		default: assert(false);         return llvm::AtomicOrdering::AcquireRelease;
		}
	}

	Nucleus::Nucleus()
	{
		::codegenMutex.lock();   // Reactor and LLVM are currently not thread safe

		llvm::InitializeNativeTarget();

#if REACTOR_LLVM_VERSION >= 7
		llvm::InitializeNativeTargetAsmPrinter();
		llvm::InitializeNativeTargetAsmParser();
#endif

		if(!::context)
		{
			::context = new llvm::LLVMContext();
		}

		#if defined(__x86_64__)
			static const char arch[] = "x86-64";
		#elif defined(__i386__)
			static const char arch[] = "x86";
		#elif defined(__aarch64__)
			static const char arch[] = "arm64";
		#elif defined(__arm__)
			static const char arch[] = "arm";
		#elif defined(__mips__)
			#if defined(__mips64)
			    static const char arch[] = "mips64el";
			#else
			    static const char arch[] = "mipsel";
			#endif
		#else
		#error "unknown architecture"
		#endif

		llvm::SmallVector<std::string, 1> mattrs;
#if defined(__i386__) || defined(__x86_64__)
		mattrs.push_back(CPUID::supportsMMX()    ? "+mmx"    : "-mmx");
		mattrs.push_back(CPUID::supportsCMOV()   ? "+cmov"   : "-cmov");
		mattrs.push_back(CPUID::supportsSSE()    ? "+sse"    : "-sse");
		mattrs.push_back(CPUID::supportsSSE2()   ? "+sse2"   : "-sse2");
		mattrs.push_back(CPUID::supportsSSE3()   ? "+sse3"   : "-sse3");
		mattrs.push_back(CPUID::supportsSSSE3()  ? "+ssse3"  : "-ssse3");
#if REACTOR_LLVM_VERSION < 7
		mattrs.push_back(CPUID::supportsSSE4_1() ? "+sse41"  : "-sse41");
#else
		mattrs.push_back(CPUID::supportsSSE4_1() ? "+sse4.1" : "-sse4.1");
#endif
#elif defined(__arm__)
#if __ARM_ARCH >= 8
		mattrs.push_back("+armv8-a");
#else
		// armv7-a requires compiler-rt routines; otherwise, compiled kernel
		// might fail to link.
#endif
#endif

#if REACTOR_LLVM_VERSION < 7
		llvm::JITEmitDebugInfo = false;
		llvm::UnsafeFPMath = true;
		// llvm::NoInfsFPMath = true;
		// llvm::NoNaNsFPMath = true;
#else
		llvm::TargetOptions targetOpts;
		targetOpts.UnsafeFPMath = false;
		// targetOpts.NoInfsFPMath = true;
		// targetOpts.NoNaNsFPMath = true;
#endif

		if(!::reactorJIT)
		{
#if REACTOR_LLVM_VERSION < 7
			::reactorJIT = new LLVMReactorJIT(arch, mattrs);
#else
			::reactorJIT = new LLVMReactorJIT(arch, mattrs, targetOpts);
#endif
		}

		::reactorJIT->startSession();

		if(!::builder)
		{
			::builder = new llvm::IRBuilder<>(*::context);
		}
	}

	Nucleus::~Nucleus()
	{
		::reactorJIT->endSession();

		::codegenMutex.unlock();
	}

	Routine *Nucleus::acquireRoutine(const char *name, bool runOptimizations)
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
				createRet(V(llvm::UndefValue::get(type)));
			}
		}

		if(false)
		{
			#if REACTOR_LLVM_VERSION < 7
				std::string error;
				llvm::raw_fd_ostream file((std::string(name) + "-llvm-dump-unopt.txt").c_str(), error);
			#else
				std::error_code error;
				llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-unopt.txt", error);
			#endif

			::module->print(file, 0);
		}

		if(runOptimizations)
		{
			optimize();
		}

		if(false)
		{
			#if REACTOR_LLVM_VERSION < 7
				std::string error;
				llvm::raw_fd_ostream file((std::string(name) + "-llvm-dump-opt.txt").c_str(), error);
			#else
				std::error_code error;
				llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-opt.txt", error);
			#endif

			::module->print(file, 0);
		}

		LLVMRoutine *routine = ::reactorJIT->acquireRoutine(::function);

		return routine;
	}

	void Nucleus::optimize()
	{
		::reactorJIT->optimize(::module);
	}

	Value *Nucleus::allocateStackVariable(Type *type, int arraySize)
	{
		// Need to allocate it in the entry block for mem2reg to work
		llvm::BasicBlock &entryBlock = ::function->getEntryBlock();

		llvm::Instruction *declaration;

		if(arraySize)
		{
#if REACTOR_LLVM_VERSION < 7
			declaration = new llvm::AllocaInst(T(type), V(Nucleus::createConstantInt(arraySize)));
#else
			declaration = new llvm::AllocaInst(T(type), 0, V(Nucleus::createConstantInt(arraySize)));
#endif
		}
		else
		{
#if REACTOR_LLVM_VERSION < 7
			declaration = new llvm::AllocaInst(T(type), (llvm::Value*)nullptr);
#else
			declaration = new llvm::AllocaInst(T(type), 0, (llvm::Value*)nullptr);
#endif
		}

		entryBlock.getInstList().push_front(declaration);

		return V(declaration);
	}

	BasicBlock *Nucleus::createBasicBlock()
	{
		return B(llvm::BasicBlock::Create(*::context, "", ::function));
	}

	BasicBlock *Nucleus::getInsertBlock()
	{
		return B(::builder->GetInsertBlock());
	}

	void Nucleus::setInsertBlock(BasicBlock *basicBlock)
	{
	//	assert(::builder->GetInsertBlock()->back().isTerminator());

		Variable::materializeAll();

		::builder->SetInsertPoint(B(basicBlock));
	}

	void Nucleus::createFunction(Type *ReturnType, std::vector<Type*> &Params)
	{
		llvm::FunctionType *functionType = llvm::FunctionType::get(T(ReturnType), T(Params), false);
		::function = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, "", ::module);
		::function->setCallingConv(llvm::CallingConv::C);

		#if defined(_WIN32) && REACTOR_LLVM_VERSION >= 7
			// FIXME(capn):
			// On Windows, stack memory is committed in increments of 4 kB pages, with the last page
			// having a trap which allows the OS to grow the stack. For functions with a stack frame
			// larger than 4 kB this can cause an issue when a variable is accessed beyond the guard
			// page. Therefore the compiler emits a call to __chkstk in the function prolog to probe
			// the stack and ensure all pages have been committed. This is currently broken in LLVM
			// JIT, but we can prevent emitting the stack probe call:
			::function->addFnAttr("stack-probe-size", "1048576");
		#endif

		::builder->SetInsertPoint(llvm::BasicBlock::Create(*::context, "", ::function));
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
		// Code generated after this point is unreachable, so any variables
		// being read can safely return an undefined value. We have to avoid
		// materializing variables after the terminator ret instruction.
		Variable::killUnmaterialized();

		::builder->CreateRetVoid();
	}

	void Nucleus::createRet(Value *v)
	{
		// Code generated after this point is unreachable, so any variables
		// being read can safely return an undefined value. We have to avoid
		// materializing variables after the terminator ret instruction.
		Variable::killUnmaterialized();

		::builder->CreateRet(V(v));
	}

	void Nucleus::createBr(BasicBlock *dest)
	{
		Variable::materializeAll();

		::builder->CreateBr(B(dest));
	}

	void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
	{
		Variable::materializeAll();

		::builder->CreateCondBr(V(cond), B(ifTrue), B(ifFalse));
	}

	Value *Nucleus::createAdd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAdd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSub(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSub(V(lhs), V(rhs)));
	}

	Value *Nucleus::createMul(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateMul(V(lhs), V(rhs)));
	}

	Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateUDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFAdd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFSub(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFSub(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFMul(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFMul(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createURem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateURem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSRem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateSRem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFRem(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFRem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createShl(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateShl(V(lhs), V(rhs)));
	}

	Value *Nucleus::createLShr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateLShr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createAShr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAShr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createAnd(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateAnd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createOr(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateOr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createXor(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateXor(V(lhs), V(rhs)));
	}

	Value *Nucleus::createNeg(Value *v)
	{
		return V(::builder->CreateNeg(V(v)));
	}

	Value *Nucleus::createFNeg(Value *v)
	{
		return V(::builder->CreateFNeg(V(v)));
	}

	Value *Nucleus::createNot(Value *v)
	{
		return V(::builder->CreateNot(V(v)));
	}

	Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
	{
		switch(asInternalType(type))
		{
		case Type_v2i32:
		case Type_v4i16:
		case Type_v8i8:
		case Type_v2f32:
			return createBitCast(
				createInsertElement(
					V(llvm::UndefValue::get(llvm::VectorType::get(T(Long::getType()), 2))),
					createLoad(createBitCast(ptr, Pointer<Long>::getType()), Long::getType(), isVolatile, alignment, atomic, memoryOrder),
					0),
				type);
		case Type_v2i16:
		case Type_v4i8:
			if(alignment != 0)   // Not a local variable (all vectors are 128-bit).
			{
				Value *u = V(llvm::UndefValue::get(llvm::VectorType::get(T(Long::getType()), 2)));
				Value *i = createLoad(createBitCast(ptr, Pointer<Int>::getType()), Int::getType(), isVolatile, alignment, atomic, memoryOrder);
				i = createZExt(i, Long::getType());
				Value *v = createInsertElement(u, i, 0);
				return createBitCast(v, type);
			}
			// Fallthrough to non-emulated case.
		case Type_LLVM:
			{
				assert(V(ptr)->getType()->getContainedType(0) == T(type));
				auto load = new llvm::LoadInst(V(ptr), "", isVolatile, alignment);
				load->setAtomic(atomicOrdering(atomic, memoryOrder));

				return V(::builder->Insert(load));
			}
		default:
			assert(false); return nullptr;
		}
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
	{
		switch(asInternalType(type))
		{
		case Type_v2i32:
		case Type_v4i16:
		case Type_v8i8:
		case Type_v2f32:
			createStore(
				createExtractElement(
					createBitCast(value, T(llvm::VectorType::get(T(Long::getType()), 2))), Long::getType(), 0),
				createBitCast(ptr, Pointer<Long>::getType()),
				Long::getType(), isVolatile, alignment, atomic, memoryOrder);
			return value;
		case Type_v2i16:
		case Type_v4i8:
			if(alignment != 0)   // Not a local variable (all vectors are 128-bit).
			{
				createStore(
					createExtractElement(createBitCast(value, Int4::getType()), Int::getType(), 0),
					createBitCast(ptr, Pointer<Int>::getType()),
					Int::getType(), isVolatile, alignment, atomic, memoryOrder);
				return value;
			}
			// Fallthrough to non-emulated case.
		case Type_LLVM:
			{
				assert(V(ptr)->getType()->getContainedType(0) == T(type));
				auto store = ::builder->Insert(new llvm::StoreInst(V(value), V(ptr), isVolatile, alignment));
				store->setAtomic(atomicOrdering(atomic, memoryOrder));

				return value;
			}
		default:
			assert(false); return nullptr;
		}
	}

	Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index, bool unsignedIndex)
	{
		assert(V(ptr)->getType()->getContainedType(0) == T(type));

		if(sizeof(void*) == 8)
		{
			// LLVM manual: "When indexing into an array, pointer or vector,
			// integers of any width are allowed, and they are not required to
			// be constant. These integers are treated as signed values where
			// relevant."
			//
			// Thus if we want indexes to be treated as unsigned we have to
			// zero-extend them ourselves.
			//
			// Note that this is not because we want to address anywhere near
			// 4 GB of data. Instead this is important for performance because
			// x86 supports automatic zero-extending of 32-bit registers to
			// 64-bit. Thus when indexing into an array using a uint32 is
			// actually faster than an int32.
			index = unsignedIndex ?
				createZExt(index, Long::getType()) :
				createSExt(index, Long::getType());
		}

		// For non-emulated types we can rely on LLVM's GEP to calculate the
		// effective address correctly.
		if(asInternalType(type) == Type_LLVM)
		{
			return V(::builder->CreateGEP(V(ptr), V(index)));
		}

		// For emulated types we have to multiply the index by the intended
		// type size ourselves to obain the byte offset.
		index = (sizeof(void*) == 8) ?
			createMul(index, createConstantLong((int64_t)typeSize(type))) :
			createMul(index, createConstantInt((int)typeSize(type)));

		// Cast to a byte pointer, apply the byte offset, and cast back to the
		// original pointer type.
		return createBitCast(
			V(::builder->CreateGEP(V(createBitCast(ptr, T(llvm::PointerType::get(T(Byte::getType()), 0)))), V(index))),
			T(llvm::PointerType::get(T(type), 0)));
	}

	Value *Nucleus::createAtomicAdd(Value *ptr, Value *value)
	{
		return V(::builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, V(ptr), V(value), llvm::AtomicOrdering::SequentiallyConsistent));
	}

	Value *Nucleus::createTrunc(Value *v, Type *destType)
	{
		return V(::builder->CreateTrunc(V(v), T(destType)));
	}

	Value *Nucleus::createZExt(Value *v, Type *destType)
	{
		return V(::builder->CreateZExt(V(v), T(destType)));
	}

	Value *Nucleus::createSExt(Value *v, Type *destType)
	{
		return V(::builder->CreateSExt(V(v), T(destType)));
	}

	Value *Nucleus::createFPToSI(Value *v, Type *destType)
	{
		return V(::builder->CreateFPToSI(V(v), T(destType)));
	}

	Value *Nucleus::createSIToFP(Value *v, Type *destType)
	{
		return V(::builder->CreateSIToFP(V(v), T(destType)));
	}

	Value *Nucleus::createFPTrunc(Value *v, Type *destType)
	{
		return V(::builder->CreateFPTrunc(V(v), T(destType)));
	}

	Value *Nucleus::createFPExt(Value *v, Type *destType)
	{
		return V(::builder->CreateFPExt(V(v), T(destType)));
	}

	Value *Nucleus::createBitCast(Value *v, Type *destType)
	{
		// Bitcasts must be between types of the same logical size. But with emulated narrow vectors we need
		// support for casting between scalars and wide vectors. Emulate them by writing to the stack and
		// reading back as the destination type.
		if(!V(v)->getType()->isVectorTy() && T(destType)->isVectorTy())
		{
			Value *readAddress = allocateStackVariable(destType);
			Value *writeAddress = createBitCast(readAddress, T(llvm::PointerType::get(V(v)->getType(), 0)));
			createStore(v, writeAddress, T(V(v)->getType()));
			return createLoad(readAddress, destType);
		}
		else if(V(v)->getType()->isVectorTy() && !T(destType)->isVectorTy())
		{
			Value *writeAddress = allocateStackVariable(T(V(v)->getType()));
			createStore(v, writeAddress, T(V(v)->getType()));
			Value *readAddress = createBitCast(writeAddress, T(llvm::PointerType::get(T(destType), 0)));
			return createLoad(readAddress, destType);
		}

		return V(::builder->CreateBitCast(V(v), T(destType)));
	}

	Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpNE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpUGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpUGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpULT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpULE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSLT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateICmpSLE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOLT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpOLE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpONE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpORD(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUNO(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpULT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpULE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
	{
		return V(::builder->CreateFCmpUNE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
	{
		assert(V(vector)->getType()->getContainedType(0) == T(type));
		return V(::builder->CreateExtractElement(V(vector), V(createConstantInt(index))));
	}

	Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
	{
		return V(::builder->CreateInsertElement(V(vector), V(element), V(createConstantInt(index))));
	}

	Value *Nucleus::createShuffleVector(Value *v1, Value *v2, const int *select)
	{
		int size = llvm::cast<llvm::VectorType>(V(v1)->getType())->getNumElements();
		const int maxSize = 16;
		llvm::Constant *swizzle[maxSize];
		assert(size <= maxSize);

		for(int i = 0; i < size; i++)
		{
			swizzle[i] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*::context), select[i]);
		}

		llvm::Value *shuffle = llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(swizzle, size));

		return V(::builder->CreateShuffleVector(V(v1), V(v2), shuffle));
	}

	Value *Nucleus::createSelect(Value *c, Value *ifTrue, Value *ifFalse)
	{
		return V(::builder->CreateSelect(V(c), V(ifTrue), V(ifFalse)));
	}

	SwitchCases *Nucleus::createSwitch(Value *control, BasicBlock *defaultBranch, unsigned numCases)
	{
		return reinterpret_cast<SwitchCases*>(::builder->CreateSwitch(V(control), B(defaultBranch), numCases));
	}

	void Nucleus::addSwitchCase(SwitchCases *switchCases, int label, BasicBlock *branch)
	{
		llvm::SwitchInst *sw = reinterpret_cast<llvm::SwitchInst *>(switchCases);
		sw->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*::context), label, true), B(branch));
	}

	void Nucleus::createUnreachable()
	{
		::builder->CreateUnreachable();
	}

	Type *Nucleus::getPointerType(Type *ElementType)
	{
		return T(llvm::PointerType::get(T(ElementType), 0));
	}

	Value *Nucleus::createNullValue(Type *Ty)
	{
		return V(llvm::Constant::getNullValue(T(Ty)));
	}

	Value *Nucleus::createConstantLong(int64_t i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*::context), i, true));
	}

	Value *Nucleus::createConstantInt(int i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*::context), i, true));
	}

	Value *Nucleus::createConstantInt(unsigned int i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*::context), i, false));
	}

	Value *Nucleus::createConstantBool(bool b)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt1Ty(*::context), b));
	}

	Value *Nucleus::createConstantByte(signed char i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*::context), i, true));
	}

	Value *Nucleus::createConstantByte(unsigned char i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*::context), i, false));
	}

	Value *Nucleus::createConstantShort(short i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(*::context), i, true));
	}

	Value *Nucleus::createConstantShort(unsigned short i)
	{
		return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(*::context), i, false));
	}

	Value *Nucleus::createConstantFloat(float x)
	{
		return V(llvm::ConstantFP::get(T(Float::getType()), x));
	}

	Value *Nucleus::createNullPointer(Type *Ty)
	{
		return V(llvm::ConstantPointerNull::get(llvm::PointerType::get(T(Ty), 0)));
	}

	Value *Nucleus::createConstantVector(const int64_t *constants, Type *type)
	{
		assert(llvm::isa<llvm::VectorType>(T(type)));
		const int numConstants = elementCount(type);                                       // Number of provided constants for the (emulated) type.
		const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();   // Number of elements of the underlying vector type.
		assert(numElements <= 16 && numConstants <= numElements);
		llvm::Constant *constantVector[16];

		for(int i = 0; i < numElements; i++)
		{
			constantVector[i] = llvm::ConstantInt::get(T(type)->getContainedType(0), constants[i % numConstants]);
		}

		return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(constantVector, numElements)));
	}

	Value *Nucleus::createConstantVector(const double *constants, Type *type)
	{
		assert(llvm::isa<llvm::VectorType>(T(type)));
		const int numConstants = elementCount(type);                                       // Number of provided constants for the (emulated) type.
		const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();   // Number of elements of the underlying vector type.
		assert(numElements <= 8 && numConstants <= numElements);
		llvm::Constant *constantVector[8];

		for(int i = 0; i < numElements; i++)
		{
			constantVector[i] = llvm::ConstantFP::get(T(type)->getContainedType(0), constants[i % numConstants]);
		}

		return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(constantVector, numElements)));
	}

	Type *Void::getType()
	{
		return T(llvm::Type::getVoidTy(*::context));
	}

	Type *Bool::getType()
	{
		return T(llvm::Type::getInt1Ty(*::context));
	}

	Type *Byte::getType()
	{
		return T(llvm::Type::getInt8Ty(*::context));
	}

	Type *SByte::getType()
	{
		return T(llvm::Type::getInt8Ty(*::context));
	}

	Type *Short::getType()
	{
		return T(llvm::Type::getInt16Ty(*::context));
	}

	Type *UShort::getType()
	{
		return T(llvm::Type::getInt16Ty(*::context));
	}

	Type *Byte4::getType()
	{
		return T(Type_v4i8);
	}

	Type *SByte4::getType()
	{
		return T(Type_v4i8);
	}

	RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddusb(x, y);
#else
		return As<Byte8>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubusb(x, y);
#else
		return As<Byte8>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Int> SignMask(RValue<Byte8> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmovmskb(x);
#else
		return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
	}

//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y)
//	{
//#if defined(__i386__) || defined(__x86_64__)
//		return x86::pcmpgtb(x, y);   // FIXME: Signedness
//#else
//		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
//#endif
//	}

	RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpeqb(x, y);
#else
		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
	}

	Type *Byte8::getType()
	{
		return T(Type_v8i8);
	}

	RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddsb(x, y);
#else
		return As<SByte8>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubsb(x, y);
#else
		return As<SByte8>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Int> SignMask(RValue<SByte8> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmovmskb(As<Byte8>(x));
#else
		return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
	}

	RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpgtb(x, y);
#else
		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
	}

	RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpeqb(As<Byte8>(x), As<Byte8>(y));
#else
		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
	}

	Type *SByte8::getType()
	{
		return T(Type_v8i8);
	}

	Type *Byte16::getType()
	{
		return T(llvm::VectorType::get(T(Byte::getType()), 16));
	}

	Type *SByte16::getType()
	{
		return T(llvm::VectorType::get(T(SByte::getType()), 16));
	}

	Type *Short2::getType()
	{
		return T(Type_v2i16);
	}

	Type *UShort2::getType()
	{
		return T(Type_v2i16);
	}

	Short4::Short4(RValue<Int4> cast)
	{
		int select[8] = {0, 2, 4, 6, 0, 2, 4, 6};
		Value *short8 = Nucleus::createBitCast(cast.value, Short8::getType());

		Value *packed = Nucleus::createShuffleVector(short8, short8, select);
		Value *short4 = As<Short4>(Int2(As<Int4>(packed))).value;

		storeValue(short4);
	}

//	Short4::Short4(RValue<Float> cast)
//	{
//	}

	Short4::Short4(RValue<Float4> cast)
	{
		Int4 v4i32 = Int4(cast);
#if defined(__i386__) || defined(__x86_64__)
		v4i32 = As<Int4>(x86::packssdw(v4i32, v4i32));
#else
		Value *v = v4i32.loadValue();
		v4i32 = As<Int4>(V(lowerPack(V(v), V(v), true)));
#endif

		storeValue(As<Short4>(Int2(v4i32)).value);
	}

	RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
#else
		return As<Short4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psraw(lhs, rhs);
#else
		return As<Short4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaxsw(x, y);
#else
		return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
#endif
	}

	RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pminsw(x, y);
#else
		return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
#endif
	}

	RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddsw(x, y);
#else
		return As<Short4>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubsw(x, y);
#else
		return As<Short4>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhw(x, y);
#else
		return As<Short4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
#endif
	}

	RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaddwd(x, y);
#else
		return As<Int2>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
	}

	RValue<SByte8> PackSigned(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		auto result = x86::packsswb(x, y);
#else
		auto result = V(lowerPack(V(x.value), V(y.value), true));
#endif
		return As<SByte8>(Swizzle(As<Int4>(result), 0x88));
	}

	RValue<Byte8> PackUnsigned(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		auto result = x86::packuswb(x, y);
#else
		auto result = V(lowerPack(V(x.value), V(y.value), false));
#endif
		return As<Byte8>(Swizzle(As<Int4>(result), 0x88));
	}

	RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpgtw(x, y);
#else
		return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
#endif
	}

	RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpeqw(x, y);
#else
		return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Short4::getType()))));
#endif
	}

	Type *Short4::getType()
	{
		return T(Type_v4i16);
	}

	UShort4::UShort4(RValue<Float4> cast, bool saturate)
	{
		if(saturate)
		{
#if defined(__i386__) || defined(__x86_64__)
			if(CPUID::supportsSSE4_1())
			{
				Int4 int4(Min(cast, Float4(0xFFFF)));   // packusdw takes care of 0x0000 saturation
				*this = As<Short4>(PackUnsigned(int4, int4));
			}
			else
#endif
			{
				*this = Short4(Int4(Max(Min(cast, Float4(0xFFFF)), Float4(0x0000))));
			}
		}
		else
		{
			*this = Short4(Int4(cast));
		}
	}

	RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
#else
		return As<UShort4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrlw(lhs, rhs);
#else
		return As<UShort4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
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
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddusw(x, y);
#else
		return As<UShort4>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubusw(x, y);
#else
		return As<UShort4>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhuw(x, y);
#else
		return As<UShort4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
#endif
	}

	RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pavgw(x, y);
#else
		return As<UShort4>(V(lowerPAVG(V(x.value), V(y.value))));
#endif
	}

	Type *UShort4::getType()
	{
		return T(Type_v4i16);
	}

	RValue<Short8> operator<<(RValue<Short8> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psllw(lhs, rhs);
#else
		return As<Short8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psraw(lhs, rhs);
#else
		return As<Short8>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaddwd(x, y);
#else
		return As<Int4>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
	}

	RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhw(x, y);
#else
		return As<Short8>(V(lowerMulHigh(V(x.value), V(y.value), true)));
#endif
	}

	Type *Short8::getType()
	{
		return T(llvm::VectorType::get(T(Short::getType()), 8));
	}

	RValue<UShort8> operator<<(RValue<UShort8> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return As<UShort8>(x86::psllw(As<Short8>(lhs), rhs));
#else
		return As<UShort8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrlw(lhs, rhs);   // FIXME: Fallback required
#else
		return As<UShort8>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
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
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhuw(x, y);
#else
		return As<UShort8>(V(lowerMulHigh(V(x.value), V(y.value), false)));
#endif
	}

	Type *UShort8::getType()
	{
		return T(llvm::VectorType::get(T(UShort::getType()), 8));
	}

	RValue<Int> operator++(Int &val, int)   // Post-increment
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator++(Int &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> operator--(Int &val, int)   // Post-decrement
	{
		RValue<Int> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator--(Int &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> RoundInt(RValue<Float> cast)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::cvtss2si(cast);
#else
		return RValue<Int>(V(lowerRoundInt(V(cast.value), T(Int::getType()))));
#endif
	}

	Type *Int::getType()
	{
		return T(llvm::Type::getInt32Ty(*::context));
	}

	Type *Long::getType()
	{
		return T(llvm::Type::getInt64Ty(*::context));
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

	RValue<UInt> operator++(UInt &val, int)   // Post-increment
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator++(UInt &val)   // Pre-increment
	{
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<UInt> operator--(UInt &val, int)   // Post-decrement
	{
		RValue<UInt> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator--(UInt &val)   // Pre-decrement
	{
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

//	RValue<UInt> RoundUInt(RValue<Float> cast)
//	{
//#if defined(__i386__) || defined(__x86_64__)
//		return x86::cvtss2si(val);   // FIXME: Unsigned
//#else
//		return IfThenElse(cast > 0.0f, Int(cast + 0.5f), Int(cast - 0.5f));
//#endif
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

	RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
#else
		return As<Int2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

		return x86::psrad(lhs, rhs);
#else
		return As<Int2>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	Type *Int2::getType()
	{
		return T(Type_v2i32);
	}

	RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
#else
		return As<UInt2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrld(lhs, rhs);
#else
		return As<UInt2>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
	}

	Type *UInt2::getType()
	{
		return T(Type_v2i32);
	}

	Int4::Int4(RValue<Byte4> cast) : XYZW(this)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			*this = x86::pmovzxbd(As<Byte16>(cast));
		}
		else
#endif
		{
			int swizzle[16] = {0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23};
			Value *a = Nucleus::createBitCast(cast.value, Byte16::getType());
			Value *b = Nucleus::createShuffleVector(a, Nucleus::createNullValue(Byte16::getType()), swizzle);

			int swizzle2[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *c = Nucleus::createBitCast(b, Short8::getType());
			Value *d = Nucleus::createShuffleVector(c, Nucleus::createNullValue(Short8::getType()), swizzle2);

			*this = As<Int4>(d);
		}
	}

	Int4::Int4(RValue<SByte4> cast) : XYZW(this)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			*this = x86::pmovsxbd(As<SByte16>(cast));
		}
		else
#endif
		{
			int swizzle[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
			Value *a = Nucleus::createBitCast(cast.value, Byte16::getType());
			Value *b = Nucleus::createShuffleVector(a, a, swizzle);

			int swizzle2[8] = {0, 0, 1, 1, 2, 2, 3, 3};
			Value *c = Nucleus::createBitCast(b, Short8::getType());
			Value *d = Nucleus::createShuffleVector(c, c, swizzle2);

			*this = As<Int4>(d) >> 24;
		}
	}

	Int4::Int4(RValue<Short4> cast) : XYZW(this)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			*this = x86::pmovsxwd(As<Short8>(cast));
		}
		else
#endif
		{
			int swizzle[8] = {0, 0, 1, 1, 2, 2, 3, 3};
			Value *c = Nucleus::createShuffleVector(cast.value, cast.value, swizzle);
			*this = As<Int4>(c) >> 16;
		}
	}

	Int4::Int4(RValue<UShort4> cast) : XYZW(this)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			*this = x86::pmovzxwd(As<UShort8>(cast));
		}
		else
#endif
		{
			int swizzle[8] = {0, 8, 1, 9, 2, 10, 3, 11};
			Value *c = Nucleus::createShuffleVector(cast.value, Short8(0, 0, 0, 0, 0, 0, 0, 0).loadValue(), swizzle);
			*this = As<Int4>(c);
		}
	}

	Int4::Int4(RValue<Int> rhs) : XYZW(this)
	{
		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::pslld(lhs, rhs);
#else
		return As<Int4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrad(lhs, rhs);
#else
		return As<Int4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
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
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGE(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
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
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
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
		// FIXME: An LLVM bug causes SExt(ICmpCC()) to produce 0 or 1 instead of 0 or ~0
		//        Restore the following line when LLVM is updated to a version where this issue is fixed.
		// return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGT(x.value, y.value), Int4::getType()));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLE(x.value, y.value), Int4::getType())) ^ Int4(0xFFFFFFFF);
	}

	RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::pmaxsd(x, y);
		}
		else
#endif
		{
			RValue<Int4> greater = CmpNLE(x, y);
			return (x & greater) | (y & ~greater);
		}
	}

	RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::pminsd(x, y);
		}
		else
#endif
		{
			RValue<Int4> less = CmpLT(x, y);
			return (x & less) | (y & ~less);
		}
	}

	RValue<Int4> RoundInt(RValue<Float4> cast)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::cvtps2dq(cast);
#else
		return As<Int4>(V(lowerRoundInt(V(cast.value), T(Int4::getType()))));
#endif
	}

	RValue<Int4> MulHigh(RValue<Int4> x, RValue<Int4> y)
	{
		// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
		return As<Int4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
	}

	RValue<UInt4> MulHigh(RValue<UInt4> x, RValue<UInt4> y)
	{
		// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
		return As<UInt4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
	}

	RValue<Short8> PackSigned(RValue<Int4> x, RValue<Int4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::packssdw(x, y);
#else
		return As<Short8>(V(lowerPack(V(x.value), V(y.value), true)));
#endif
	}

	RValue<UShort8> PackUnsigned(RValue<Int4> x, RValue<Int4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::packusdw(x, y);
#else
		return As<UShort8>(V(lowerPack(V(x.value), V(y.value), false)));
#endif
	}

	RValue<Int> SignMask(RValue<Int4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::movmskps(As<Float4>(x));
#else
		return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
	}

	Type *Int4::getType()
	{
		return T(llvm::VectorType::get(T(Int::getType()), 4));
	}

	UInt4::UInt4(RValue<Float4> cast) : XYZW(this)
	{
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

	RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return As<UInt4>(x86::pslld(As<Int4>(lhs), rhs));
#else
		return As<UInt4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrld(lhs, rhs);
#else
		return As<UInt4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
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
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::pmaxud(x, y);
		}
		else
#endif
		{
			RValue<UInt4> greater = CmpNLE(x, y);
			return (x & greater) | (y & ~greater);
		}
	}

	RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::pminud(x, y);
		}
		else
#endif
		{
			RValue<UInt4> less = CmpLT(x, y);
			return (x & less) | (y & ~less);
		}
	}

	Type *UInt4::getType()
	{
		return T(llvm::VectorType::get(T(UInt::getType()), 4));
	}

	Type *Half::getType()
	{
		return T(llvm::Type::getInt16Ty(*::context));
	}

	RValue<Float> Rcp_pp(RValue<Float> x, bool exactAtPow2)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(exactAtPow2)
		{
			// rcpss uses a piecewise-linear approximation which minimizes the relative error
			// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
			return x86::rcpss(x) * Float(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
		}
		return x86::rcpss(x);
#else
		return As<Float>(V(lowerRCP(V(x.value))));
#endif
	}

	RValue<Float> RcpSqrt_pp(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::rsqrtss(x);
#else
		return As<Float>(V(lowerRSQRT(V(x.value))));
#endif
	}

	RValue<Float> Sqrt(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::sqrtss(x);
#else
		return As<Float>(V(lowerSQRT(V(x.value))));
#endif
	}

	RValue<Float> Round(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundss(x, 0);
		}
		else
		{
			return Float4(Round(Float4(x))).x;
		}
#else
		return RValue<Float>(V(lowerRound(V(x.value))));
#endif
	}

	RValue<Float> Trunc(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundss(x, 3);
		}
		else
		{
			return Float(Int(x));   // Rounded toward zero
		}
#else
		return RValue<Float>(V(lowerTrunc(V(x.value))));
#endif
	}

	RValue<Float> Frac(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x - x86::floorss(x);
		}
		else
		{
			return Float4(Frac(Float4(x))).x;
		}
#else
		// x - floor(x) can be 1.0 for very small negative x.
		// Clamp against the value just below 1.0.
		return Min(x - Floor(x), As<Float>(Int(0x3F7FFFFF)));
#endif
	}

	RValue<Float> Floor(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::floorss(x);
		}
		else
		{
			return Float4(Floor(Float4(x))).x;
		}
#else
		return RValue<Float>(V(lowerFloor(V(x.value))));
#endif
	}

	RValue<Float> Ceil(RValue<Float> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::ceilss(x);
		}
		else
#endif
		{
			return Float4(Ceil(Float4(x))).x;
		}
	}

	Type *Float::getType()
	{
		return T(llvm::Type::getFloatTy(*::context));
	}

	Type *Float2::getType()
	{
		return T(Type_v2f32);
	}

	Float4::Float4(RValue<Float> rhs) : XYZW(this)
	{
		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::maxps(x, y);
#else
		return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OGT)));
#endif
	}

	RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::minps(x, y);
#else
		return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OLT)));
#endif
	}

	RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(exactAtPow2)
		{
			// rcpps uses a piecewise-linear approximation which minimizes the relative error
			// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
			return x86::rcpps(x) * Float4(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
		}
		return x86::rcpps(x);
#else
		return As<Float4>(V(lowerRCP(V(x.value))));
#endif
	}

	RValue<Float4> RcpSqrt_pp(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::rsqrtps(x);
#else
		return As<Float4>(V(lowerRSQRT(V(x.value))));
#endif
	}

	RValue<Float4> Sqrt(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::sqrtps(x);
#else
		return As<Float4>(V(lowerSQRT(V(x.value))));
#endif
	}

	RValue<Int> SignMask(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		return x86::movmskps(x);
#else
		return As<Int>(V(lowerFPSignMask(V(x.value), T(Int::getType()))));
#endif
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

	RValue<Int4> CmpUEQ(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpULT(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpULE(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNEQ(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUNE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNLT(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNLE(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGT(x.value, y.value), Int4::getType()));
	}

	RValue<Float4> Round(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundps(x, 0);
		}
		else
		{
			return Float4(RoundInt(x));
		}
#else
		return RValue<Float4>(V(lowerRound(V(x.value))));
#endif
	}

	RValue<Float4> Trunc(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::roundps(x, 3);
		}
		else
		{
			return Float4(Int4(x));
		}
#else
		return RValue<Float4>(V(lowerTrunc(V(x.value))));
#endif
	}

	RValue<Float4> Frac(RValue<Float4> x)
	{
		Float4 frc;

#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			frc = x - Floor(x);
		}
		else
		{
			frc = x - Float4(Int4(x));   // Signed fractional part.

			frc += As<Float4>(As<Int4>(CmpNLE(Float4(0.0f), frc)) & As<Int4>(Float4(1.0f)));   // Add 1.0 if negative.
		}
#else
		frc = x - Floor(x);
#endif

		// x - floor(x) can be 1.0 for very small negative x.
		// Clamp against the value just below 1.0.
		return Min(frc, As<Float4>(Int4(0x3F7FFFFF)));
	}

	RValue<Float4> Floor(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::floorps(x);
		}
		else
		{
			return x - Frac(x);
		}
#else
		return RValue<Float4>(V(lowerFloor(V(x.value))));
#endif
	}

	RValue<Float4> Ceil(RValue<Float4> x)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			return x86::ceilps(x);
		}
		else
#endif
		{
			return -Floor(-x);
		}
	}

	RValue<Float4> Sin(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::sin, { V(v.value)->getType() } );
		return RValue<Float4>(V(::builder->CreateCall(func, V(v.value))));
	}

	Type *Float4::getType()
	{
		return T(llvm::VectorType::get(T(Float::getType()), 4));
	}

	RValue<Long> Ticks()
	{
		llvm::Function *rdtsc = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::readcyclecounter);

		return RValue<Long>(V(::builder->CreateCall(rdtsc)));
	}
}

namespace rr
{
#if defined(__i386__) || defined(__x86_64__)
	namespace x86
	{
		RValue<Int> cvtss2si(RValue<Float> val)
		{
			llvm::Function *cvtss2si = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_cvtss2si);

			Float4 vector;
			vector.x = val;

			return RValue<Int>(V(::builder->CreateCall(cvtss2si, ARGS(V(RValue<Float4>(vector).value)))));
		}

		RValue<Int4> cvtps2dq(RValue<Float4> val)
		{
			llvm::Function *cvtps2dq = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_cvtps2dq);

			return RValue<Int4>(V(::builder->CreateCall(cvtps2dq, ARGS(V(val.value)))));
		}

		RValue<Float> rcpss(RValue<Float> val)
		{
			llvm::Function *rcpss = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_rcp_ss);

			Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(rcpss, ARGS(V(vector)))), Float::getType(), 0));
		}

		RValue<Float> sqrtss(RValue<Float> val)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *sqrtss = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_sqrt_ss);
			Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(sqrtss, ARGS(V(vector)))), Float::getType(), 0));
#else
			llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::sqrt, {V(val.value)->getType()});
			return RValue<Float>(V(::builder->CreateCall(sqrt, ARGS(V(val.value)))));
#endif
		}

		RValue<Float> rsqrtss(RValue<Float> val)
		{
			llvm::Function *rsqrtss = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_rsqrt_ss);

			Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall(rsqrtss, ARGS(V(vector)))), Float::getType(), 0));
		}

		RValue<Float4> rcpps(RValue<Float4> val)
		{
			llvm::Function *rcpps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_rcp_ps);

			return RValue<Float4>(V(::builder->CreateCall(rcpps, ARGS(V(val.value)))));
		}

		RValue<Float4> sqrtps(RValue<Float4> val)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *sqrtps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_sqrt_ps);
#else
			llvm::Function *sqrtps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::sqrt, {V(val.value)->getType()});
#endif

			return RValue<Float4>(V(::builder->CreateCall(sqrtps, ARGS(V(val.value)))));
		}

		RValue<Float4> rsqrtps(RValue<Float4> val)
		{
			llvm::Function *rsqrtps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_rsqrt_ps);

			return RValue<Float4>(V(::builder->CreateCall(rsqrtps, ARGS(V(val.value)))));
		}

		RValue<Float4> maxps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *maxps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_max_ps);

			return RValue<Float4>(V(::builder->CreateCall2(maxps, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Float4> minps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *minps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_min_ps);

			return RValue<Float4>(V(::builder->CreateCall2(minps, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Float> roundss(RValue<Float> val, unsigned char imm)
		{
			llvm::Function *roundss = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_round_ss);

			Value *undef = V(llvm::UndefValue::get(T(Float4::getType())));
			Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(::builder->CreateCall3(roundss, ARGS(V(undef), V(vector), V(Nucleus::createConstantInt(imm))))), Float::getType(), 0));
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
			llvm::Function *roundps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_round_ps);

			return RValue<Float4>(V(::builder->CreateCall2(roundps, ARGS(V(val.value), V(Nucleus::createConstantInt(imm))))));
		}

		RValue<Float4> floorps(RValue<Float4> val)
		{
			return roundps(val, 1);
		}

		RValue<Float4> ceilps(RValue<Float4> val)
		{
			return roundps(val, 2);
		}

		RValue<Int4> pabsd(RValue<Int4> x)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pabsd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_ssse3_pabs_d_128);

			return RValue<Int4>(V(::builder->CreateCall(pabsd, ARGS(V(x.value)))));
#else
			return RValue<Int4>(V(lowerPABS(V(x.value))));
#endif
		}

		RValue<Short4> paddsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *paddsw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_padds_w);

			return As<Short4>(V(::builder->CreateCall2(paddsw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Short4> psubsw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *psubsw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psubs_w);

			return As<Short4>(V(::builder->CreateCall2(psubsw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort4> paddusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *paddusw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_paddus_w);

			return As<UShort4>(V(::builder->CreateCall2(paddusw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort4> psubusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *psubusw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psubus_w);

			return As<UShort4>(V(::builder->CreateCall2(psubusw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<SByte8> paddsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			llvm::Function *paddsb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_padds_b);

			return As<SByte8>(V(::builder->CreateCall2(paddsb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<SByte8> psubsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			llvm::Function *psubsb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psubs_b);

			return As<SByte8>(V(::builder->CreateCall2(psubsb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Byte8> paddusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *paddusb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_paddus_b);

			return As<Byte8>(V(::builder->CreateCall2(paddusb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Byte8> psubusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			llvm::Function *psubusb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psubus_b);

			return As<Byte8>(V(::builder->CreateCall2(psubusb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort4> pavgw(RValue<UShort4> x, RValue<UShort4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pavgw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pavg_w);

			return As<UShort4>(V(::builder->CreateCall2(pavgw, ARGS(V(x.value), V(y.value)))));
#else
			return As<UShort4>(V(lowerPAVG(V(x.value), V(y.value))));
#endif
		}

		RValue<Short4> pmaxsw(RValue<Short4> x, RValue<Short4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmaxsw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmaxs_w);

			return As<Short4>(V(::builder->CreateCall2(pmaxsw, ARGS(V(x.value), V(y.value)))));
#else
			return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
#endif
		}

		RValue<Short4> pminsw(RValue<Short4> x, RValue<Short4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pminsw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmins_w);

			return As<Short4>(V(::builder->CreateCall2(pminsw, ARGS(V(x.value), V(y.value)))));
#else
			return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
#endif
		}

		RValue<Short4> pcmpgtw(RValue<Short4> x, RValue<Short4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pcmpgtw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pcmpgt_w);

			return As<Short4>(V(::builder->CreateCall2(pcmpgtw, ARGS(V(x.value), V(y.value)))));
#else
			return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
#endif
		}

		RValue<Short4> pcmpeqw(RValue<Short4> x, RValue<Short4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pcmpeqw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pcmpeq_w);

			return As<Short4>(V(::builder->CreateCall2(pcmpeqw, ARGS(V(x.value), V(y.value)))));
#else
			return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Short4::getType()))));
#endif
		}

		RValue<Byte8> pcmpgtb(RValue<SByte8> x, RValue<SByte8> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pcmpgtb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pcmpgt_b);

			return As<Byte8>(V(::builder->CreateCall2(pcmpgtb, ARGS(V(x.value), V(y.value)))));
#else
			return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
		}

		RValue<Byte8> pcmpeqb(RValue<Byte8> x, RValue<Byte8> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pcmpeqb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pcmpeq_b);

			return As<Byte8>(V(::builder->CreateCall2(pcmpeqb, ARGS(V(x.value), V(y.value)))));
#else
			return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
		}

		RValue<Short4> packssdw(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_packssdw_128);

			return As<Short4>(V(::builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Short8> packssdw(RValue<Int4> x, RValue<Int4> y)
		{
			llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_packssdw_128);

			return RValue<Short8>(V(::builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<SByte8> packsswb(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *packsswb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_packsswb_128);

			return As<SByte8>(V(::builder->CreateCall2(packsswb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Byte8> packuswb(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *packuswb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_packuswb_128);

			return As<Byte8>(V(::builder->CreateCall2(packuswb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort8> packusdw(RValue<Int4> x, RValue<Int4> y)
		{
			if(CPUID::supportsSSE4_1())
			{
				llvm::Function *packusdw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_packusdw);

				return RValue<UShort8>(V(::builder->CreateCall2(packusdw, ARGS(V(x.value), V(y.value)))));
			}
			else
			{
				RValue<Int4> bx = (x & ~(x >> 31)) - Int4(0x8000);
				RValue<Int4> by = (y & ~(y >> 31)) - Int4(0x8000);

				return As<UShort8>(packssdw(bx, by) + Short8(0x8000u));
			}
		}

		RValue<UShort4> psrlw(RValue<UShort4> x, unsigned char y)
		{
			llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrli_w);

			return As<UShort4>(V(::builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UShort8> psrlw(RValue<UShort8> x, unsigned char y)
		{
			llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrli_w);

			return RValue<UShort8>(V(::builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short4> psraw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psraw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrai_w);

			return As<Short4>(V(::builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psraw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psraw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrai_w);

			return RValue<Short8>(V(::builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short4> psllw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psllw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pslli_w);

			return As<Short4>(V(::builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psllw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psllw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pslli_w);

			return RValue<Short8>(V(::builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int2> pslld(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *pslld = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pslli_d);

			return As<Int2>(V(::builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> pslld(RValue<Int4> x, unsigned char y)
		{
			llvm::Function *pslld = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pslli_d);

			return RValue<Int4>(V(::builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int2> psrad(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *psrad = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrai_d);

			return As<Int2>(V(::builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> psrad(RValue<Int4> x, unsigned char y)
		{
			llvm::Function *psrad = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrai_d);

			return RValue<Int4>(V(::builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UInt2> psrld(RValue<UInt2> x, unsigned char y)
		{
			llvm::Function *psrld = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrli_d);

			return As<UInt2>(V(::builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UInt4> psrld(RValue<UInt4> x, unsigned char y)
		{
			llvm::Function *psrld = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_psrli_d);

			return RValue<UInt4>(V(::builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> pmaxsd(RValue<Int4> x, RValue<Int4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmaxsd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmaxsd);

			return RValue<Int4>(V(::builder->CreateCall2(pmaxsd, ARGS(V(x.value), V(y.value)))));
#else
			return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
#endif
		}

		RValue<Int4> pminsd(RValue<Int4> x, RValue<Int4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pminsd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pminsd);

			return RValue<Int4>(V(::builder->CreateCall2(pminsd, ARGS(V(x.value), V(y.value)))));
#else
			return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
#endif
		}

		RValue<UInt4> pmaxud(RValue<UInt4> x, RValue<UInt4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmaxud = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmaxud);

			return RValue<UInt4>(V(::builder->CreateCall2(pmaxud, ARGS(V(x.value), V(y.value)))));
#else
			return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_UGT)));
#endif
		}

		RValue<UInt4> pminud(RValue<UInt4> x, RValue<UInt4> y)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pminud = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pminud);

			return RValue<UInt4>(V(::builder->CreateCall2(pminud, ARGS(V(x.value), V(y.value)))));
#else
			return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_ULT)));
#endif
		}

		RValue<Short4> pmulhw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmulh_w);

			return As<Short4>(V(::builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort4> pmulhuw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmulhu_w);

			return As<UShort4>(V(::builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int2> pmaddwd(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmadd_wd);

			return As<Int2>(V(::builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Short8> pmulhw(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmulh_w);

			return RValue<Short8>(V(::builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort8> pmulhuw(RValue<UShort8> x, RValue<UShort8> y)
		{
			llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmulhu_w);

			return RValue<UShort8>(V(::builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int4> pmaddwd(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmadd_wd);

			return RValue<Int4>(V(::builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int> movmskps(RValue<Float4> x)
		{
			llvm::Function *movmskps = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse_movmsk_ps);

			return RValue<Int>(V(::builder->CreateCall(movmskps, ARGS(V(x.value)))));
		}

		RValue<Int> pmovmskb(RValue<Byte8> x)
		{
			llvm::Function *pmovmskb = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse2_pmovmskb_128);

			return RValue<Int>(V(::builder->CreateCall(pmovmskb, ARGS(V(x.value))))) & 0xFF;
		}

		RValue<Int4> pmovzxbd(RValue<Byte16> x)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmovzxbd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmovzxbd);

			return RValue<Int4>(V(::builder->CreateCall(pmovzxbd, ARGS(V(x.value)))));
#else
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
#endif
		}

		RValue<Int4> pmovsxbd(RValue<SByte16> x)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmovsxbd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmovsxbd);

			return RValue<Int4>(V(::builder->CreateCall(pmovsxbd, ARGS(V(x.value)))));
#else
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
#endif
		}

		RValue<Int4> pmovzxwd(RValue<UShort8> x)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmovzxwd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmovzxwd);

			return RValue<Int4>(V(::builder->CreateCall(pmovzxwd, ARGS(V(x.value)))));
#else
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
#endif
		}

		RValue<Int4> pmovsxwd(RValue<Short8> x)
		{
#if REACTOR_LLVM_VERSION < 7
			llvm::Function *pmovsxwd = llvm::Intrinsic::getDeclaration(::module, llvm::Intrinsic::x86_sse41_pmovsxwd);

			return RValue<Int4>(V(::builder->CreateCall(pmovsxwd, ARGS(V(x.value)))));
#else
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
#endif
		}
	}
#endif  // defined(__i386__) || defined(__x86_64__)

#ifdef ENABLE_RR_PRINT
	// extractAll returns a vector containing the extracted n scalar value of
	// the vector vec.
	static std::vector<Value*> extractAll(Value* vec, int n)
	{
		std::vector<Value*> elements;
		elements.reserve(n);
		for (int i = 0; i < n; i++)
		{
			auto el = V(::builder->CreateExtractElement(V(vec), i));
			elements.push_back(el);
		}
		return elements;
	}

	// toDouble returns all the float values in vals extended to doubles.
	static std::vector<Value*> toDouble(const std::vector<Value*>& vals)
	{
		auto doubleTy = ::llvm::Type::getDoubleTy(*::context);
		std::vector<Value*> elements;
		elements.reserve(vals.size());
		for (auto v : vals)
		{
			elements.push_back(V(::builder->CreateFPExt(V(v), doubleTy)));
		}
		return elements;
	}

	std::vector<Value*> PrintValue::Ty<Byte4>::val(const RValue<Byte4>& v) { return extractAll(v.value, 4); }
	std::vector<Value*> PrintValue::Ty<Int4>::val(const RValue<Int4>& v) { return extractAll(v.value, 4); }
	std::vector<Value*> PrintValue::Ty<UInt4>::val(const RValue<UInt4>& v) { return extractAll(v.value, 4); }
	std::vector<Value*> PrintValue::Ty<Short4>::val(const RValue<Short4>& v) { return extractAll(v.value, 4); }
	std::vector<Value*> PrintValue::Ty<UShort4>::val(const RValue<UShort4>& v) { return extractAll(v.value, 4); }
	std::vector<Value*> PrintValue::Ty<Float>::val(const RValue<Float>& v) { return toDouble({v.value}); }
	std::vector<Value*> PrintValue::Ty<Float4>::val(const RValue<Float4>& v) { return toDouble(extractAll(v.value, 4)); }

	void Printv(const char* function, const char* file, int line, const char* fmt, std::initializer_list<PrintValue> args)
	{
		// LLVM types used below.
		auto i32Ty = ::llvm::Type::getInt32Ty(*::context);
		auto intTy = ::llvm::Type::getInt64Ty(*::context); // TODO: Natural int width.
		auto i8PtrTy = ::llvm::Type::getInt8PtrTy(*::context);
		auto funcTy = ::llvm::FunctionType::get(i32Ty, {i8PtrTy}, true);

		auto func = ::module->getOrInsertFunction("printf", funcTy);

		// Build the printf format message string.
		std::string str;
		if (file != nullptr) { str += (line > 0) ? "%s:%d " : "%s "; }
		if (function != nullptr) { str += "%s "; }
		str += fmt;

		// Perform subsitution on all '{n}' bracketed indices in the format
		// message.
		int i = 0;
		for (const PrintValue& arg : args)
		{
			str = replace(str, "{" + std::to_string(i++) + "}", arg.format);
		}

		::llvm::SmallVector<::llvm::Value*, 8> vals;

		// The format message is always the first argument.
		vals.push_back(::builder->CreateGlobalStringPtr(str));

		// Add optional file, line and function info if provided.
		if (file != nullptr)
		{
			vals.push_back(::builder->CreateGlobalStringPtr(file));
			if (line > 0)
			{
				vals.push_back(::llvm::ConstantInt::get(intTy, line));
			}
		}
		if (function != nullptr)
		{
			vals.push_back(::builder->CreateGlobalStringPtr(function));
		}

		// Add all format arguments.
		for (const PrintValue& arg : args)
		{
			for (auto val : arg.values)
			{
				vals.push_back(V(val));
			}
		}

		::builder->CreateCall(func, vals);
	}
#endif // ENABLE_RR_PRINT

}
