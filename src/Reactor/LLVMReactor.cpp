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
#include "Debug.hpp"
#include "LLVMReactor.hpp"
#include "LLVMReactorDebugInfo.hpp"

#include "x86.hpp"
#include "CPUID.hpp"
#include "Thread.hpp"
#include "ExecutableMemory.hpp"
#include "MutexLock.hpp"

#undef min
#undef max

#if defined(__clang__)
// LLVM has occurances of the extra-semi warning in its headers, which will be
// treated as an error in SwiftShader targets.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#endif  // defined(__clang__)

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable : 4146)) // unary minus operator applied to unsigned type, result still unsigned
#endif

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
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Coroutines.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)

#ifdef _MSC_VER
__pragma(warning(pop))
#endif

#define ARGS(...) {__VA_ARGS__}
#define CreateCall2 CreateCall
#define CreateCall3 CreateCall

#include <unordered_map>

#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>

#if defined(__i386__) || defined(__x86_64__)
#include <xmmintrin.h>
#endif

#include <math.h>

#if defined(__x86_64__) && defined(_WIN32)
	extern "C" void X86CompilationCallback()
	{
		UNIMPLEMENTED("X86CompilationCallback");
	}
#endif

#if defined(_WIN64)
	extern "C" void __chkstk();
#elif defined(_WIN32)
	extern "C" void _chkstk();
#endif

namespace rr
{
	void* resolveExternalSymbol(const char*);
}

namespace
{
	// Default configuration settings. Must be accessed under mutex lock.
	std::mutex defaultConfigLock;
	rr::Config &defaultConfig()
	{
		// This uses a static in a function to avoid the cost of a global static
		// initializer. See http://neugierig.org/software/chromium/notes/2011/08/static-initializers.html
		static rr::Config config = rr::Config::Edit()
			.set(rr::Optimization::Level::Default)
			.add(rr::Optimization::Pass::ScalarReplAggregates)
			.add(rr::Optimization::Pass::InstructionCombining)
			.apply({});
		return config;
	}

	// Cache provides a simple, thread-safe key-value store.
	template <typename KEY, typename VALUE>
	class Cache
	{
	public:
		Cache() = default;
		Cache(const Cache& other);
		VALUE getOrCreate(KEY key, std::function<VALUE()> create);
	private:
		mutable std::mutex mutex; // mutable required for copy constructor.
		std::unordered_map<KEY, VALUE> map;
	};

	template <typename KEY, typename VALUE>
	Cache<KEY, VALUE>::Cache(const Cache& other)
	{
		std::unique_lock<std::mutex> lock(other.mutex);
		map = other.map;
	}

	template <typename KEY, typename VALUE>
	VALUE Cache<KEY, VALUE>::getOrCreate(KEY key, std::function<VALUE()> create)
	{
		std::unique_lock<std::mutex> lock(mutex);
		auto it = map.find(key);
		if (it != map.end())
		{
			return it->second;
		}
		auto value = create();
		map.emplace(key, value);
		return value;
	}

	// JITGlobals is a singleton that holds all the immutable machine specific
	// information for the host device.
	class JITGlobals
	{
	public:
		using TargetMachineSPtr = std::shared_ptr<llvm::TargetMachine>;

		static JITGlobals * get();

		const std::string mcpu;
		const std::vector<std::string> mattrs;
		const char* const march;
		const llvm::TargetOptions targetOptions;
		const llvm::DataLayout dataLayout;

		TargetMachineSPtr getTargetMachine(rr::Optimization::Level optlevel);

	private:
		static JITGlobals create();
		static llvm::CodeGenOpt::Level toLLVM(rr::Optimization::Level level);
		JITGlobals(const char *mcpu,
		           const std::vector<std::string> &mattrs,
		           const char *march,
		           const llvm::TargetOptions &targetOptions,
		           const llvm::DataLayout &dataLayout);
		JITGlobals(const JITGlobals&) = default;

		// The cache key here is actually a rr::Optimization::Level. We use int
		// as 'enum class' types do not provide builtin hash functions until
		// C++14. See: https://stackoverflow.com/a/29618545.
		Cache<int, TargetMachineSPtr> targetMachines;
	};

	JITGlobals * JITGlobals::get()
	{
		static JITGlobals instance = create();
		return &instance;
	}

	JITGlobals::TargetMachineSPtr JITGlobals::getTargetMachine(rr::Optimization::Level optlevel)
	{
		return targetMachines.getOrCreate(static_cast<int>(optlevel), [&]() {
			return TargetMachineSPtr(llvm::EngineBuilder()
#ifdef ENABLE_RR_DEBUG_INFO
				.setOptLevel(toLLVM(rr::Optimization::Level::None))
#else
				.setOptLevel(toLLVM(optlevel))
#endif // ENABLE_RR_DEBUG_INFO
				.setMCPU(mcpu)
				.setMArch(march)
				.setMAttrs(mattrs)
				.setTargetOptions(targetOptions)
				.selectTarget());
		});
	}

	JITGlobals JITGlobals::create()
	{
		struct LLVMInitializer
		{
			LLVMInitializer()
			{
				llvm::InitializeNativeTarget();
				llvm::InitializeNativeTargetAsmPrinter();
				llvm::InitializeNativeTargetAsmParser();
			}
		};
		static LLVMInitializer initializeLLVM;

		auto mcpu = llvm::sys::getHostCPUName();

		llvm::StringMap<bool> features;
		bool ok = llvm::sys::getHostCPUFeatures(features);

#if defined(__i386__) || defined(__x86_64__) || \
(defined(__linux__) && (defined(__arm__) || defined(__aarch64__)))
		ASSERT_MSG(ok, "llvm::sys::getHostCPUFeatures returned false");
#else
		(void) ok; // getHostCPUFeatures always returns false on other platforms
#endif

		std::vector<std::string> mattrs;
		for (auto &feature : features)
		{
			if (feature.second) { mattrs.push_back(feature.first()); }
		}

		const char* march = nullptr;
#if defined(__x86_64__)
		march = "x86-64";
#elif defined(__i386__)
		march = "x86";
#elif defined(__aarch64__)
		march = "arm64";
#elif defined(__arm__)
		march = "arm";
#elif defined(__mips__)
#if defined(__mips64)
		march = "mips64el";
#else
		march = "mipsel";
#endif
#elif defined(__powerpc64__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		march = "ppc64le";
#else
		#error "unknown architecture"
#endif

		llvm::TargetOptions targetOptions;
		targetOptions.UnsafeFPMath = false;

		auto targetMachine = std::unique_ptr<llvm::TargetMachine>(
			llvm::EngineBuilder()
				.setOptLevel(llvm::CodeGenOpt::None)
				.setMCPU(mcpu)
				.setMArch(march)
				.setMAttrs(mattrs)
				.setTargetOptions(targetOptions)
				.selectTarget());

		auto dataLayout = targetMachine->createDataLayout();

		return JITGlobals(mcpu.data(), mattrs, march, targetOptions, dataLayout);
	}

	llvm::CodeGenOpt::Level JITGlobals::toLLVM(rr::Optimization::Level level)
	{
		switch (level)
		{
			case rr::Optimization::Level::None:       return ::llvm::CodeGenOpt::None;
			case rr::Optimization::Level::Less:       return ::llvm::CodeGenOpt::Less;
			case rr::Optimization::Level::Default:    return ::llvm::CodeGenOpt::Default;
			case rr::Optimization::Level::Aggressive: return ::llvm::CodeGenOpt::Aggressive;
			default: UNREACHABLE("Unknown Optimization Level %d", int(level));
		}
		return ::llvm::CodeGenOpt::Default;
	}

	JITGlobals::JITGlobals(const char* mcpu,
	                       const std::vector<std::string> &mattrs,
	                       const char* march,
	                       const llvm::TargetOptions &targetOptions,
	                       const llvm::DataLayout &dataLayout) :
			mcpu(mcpu),
			mattrs(mattrs),
			march(march),
			targetOptions(targetOptions),
			dataLayout(dataLayout)
	{
	}

	// JITRoutine is a rr::Routine that holds a LLVM JIT session, compiler and
	// object layer as each routine may require different target machine
	// settings and no Reactor routine directly links against another.
	class JITRoutine : public rr::Routine
	{
#if LLVM_VERSION_MAJOR >= 8
		using ObjLayer = llvm::orc::LegacyRTDyldObjectLinkingLayer;
		using CompileLayer = llvm::orc::LegacyIRCompileLayer<ObjLayer, llvm::orc::SimpleCompiler>;
#else
		using ObjLayer = llvm::orc::RTDyldObjectLinkingLayer;
		using CompileLayer = llvm::orc::IRCompileLayer<ObjLayer, llvm::orc::SimpleCompiler>;
#endif

	public:
		JITRoutine(
				std::unique_ptr<llvm::Module> module,
				llvm::Function **funcs,
				size_t count,
				const rr::Config &config) :
			resolver(createLegacyLookupResolver(
				session,
				[&](const std::string &name) {
					void *func = rr::resolveExternalSymbol(name.c_str());
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
			targetMachine(JITGlobals::get()->getTargetMachine(config.getOptimization().getLevel())),
			compileLayer(objLayer, llvm::orc::SimpleCompiler(*targetMachine)),
			objLayer(
				session,
				[this](llvm::orc::VModuleKey) {
					return ObjLayer::Resources{std::make_shared<llvm::SectionMemoryManager>(), resolver};
				},
				ObjLayer::NotifyLoadedFtor(),
				[](llvm::orc::VModuleKey, const llvm::object::ObjectFile &Obj, const llvm::RuntimeDyld::LoadedObjectInfo &L) {
#ifdef ENABLE_RR_DEBUG_INFO
					rr::DebugInfo::NotifyObjectEmitted(Obj, L);
#endif // ENABLE_RR_DEBUG_INFO
				},
				[](llvm::orc::VModuleKey, const llvm::object::ObjectFile &Obj) {
#ifdef ENABLE_RR_DEBUG_INFO
					rr::DebugInfo::NotifyFreeingObject(Obj);
#endif // ENABLE_RR_DEBUG_INFO
				}
			),
			addresses(count)
		{
			std::vector<std::string> mangledNames(count);
			for (size_t i = 0; i < count; i++)
			{
				auto func = funcs[i];
				static size_t numEmittedFunctions = 0;
				std::string name = "f" + llvm::Twine(numEmittedFunctions++).str();
				func->setName(name);
				func->setLinkage(llvm::GlobalValue::ExternalLinkage);
				func->setDoesNotThrow();

				llvm::raw_string_ostream mangledNameStream(mangledNames[i]);
				llvm::Mangler::getNameWithPrefix(mangledNameStream, name, JITGlobals::get()->dataLayout);
			}

			auto moduleKey = session.allocateVModule();

			// Once the module is passed to the compileLayer, the
			// llvm::Functions are freed. Make sure funcs are not referenced
			// after this point.
			funcs = nullptr;

			llvm::cantFail(compileLayer.addModule(moduleKey, std::move(module)));

			// Resolve the function addresses.
			for (size_t i = 0; i < count; i++)
			{
				auto symbol = compileLayer.findSymbolIn(moduleKey, mangledNames[i], false);
				if(auto address = symbol.getAddress())
				{
					addresses[i] = reinterpret_cast<void *>(static_cast<intptr_t>(address.get()));
				}
			}
		}

		const void *getEntry(int index) override
		{
			return addresses[index];
		}

	private:
		std::shared_ptr<llvm::orc::SymbolResolver> resolver;
		std::shared_ptr<llvm::TargetMachine> targetMachine;
		llvm::orc::ExecutionSession session;
		CompileLayer compileLayer;
		ObjLayer objLayer;
		std::vector<const void *> addresses;
	};

	// JITBuilder holds all the LLVM state for building routines.
	class JITBuilder
	{
	public:
		JITBuilder(const rr::Config &config) :
			config(config),
			module(new llvm::Module("", context)),
			builder(new llvm::IRBuilder<>(context))
		{
			module->setDataLayout(JITGlobals::get()->dataLayout);
		}

		void optimize(const rr::Config &cfg)
		{

#ifdef ENABLE_RR_DEBUG_INFO
			if (debugInfo != nullptr)
			{
				return; // Don't optimize if we're generating debug info.
			}
#endif // ENABLE_RR_DEBUG_INFO

			std::unique_ptr<llvm::legacy::PassManager> passManager(
				new llvm::legacy::PassManager());

			for(auto pass : cfg.getOptimization().getPasses())
			{
				switch(pass)
				{
				case rr::Optimization::Pass::Disabled:                                                                       break;
				case rr::Optimization::Pass::CFGSimplification:    passManager->add(llvm::createCFGSimplificationPass());    break;
				case rr::Optimization::Pass::LICM:                 passManager->add(llvm::createLICMPass());                 break;
				case rr::Optimization::Pass::AggressiveDCE:        passManager->add(llvm::createAggressiveDCEPass());        break;
				case rr::Optimization::Pass::GVN:                  passManager->add(llvm::createGVNPass());                  break;
				case rr::Optimization::Pass::InstructionCombining: passManager->add(llvm::createInstructionCombiningPass()); break;
				case rr::Optimization::Pass::Reassociate:          passManager->add(llvm::createReassociatePass());          break;
				case rr::Optimization::Pass::DeadStoreElimination: passManager->add(llvm::createDeadStoreEliminationPass()); break;
				case rr::Optimization::Pass::SCCP:                 passManager->add(llvm::createSCCPPass());                 break;
				case rr::Optimization::Pass::ScalarReplAggregates: passManager->add(llvm::createSROAPass());                 break;
				case rr::Optimization::Pass::EarlyCSEPass:         passManager->add(llvm::createEarlyCSEPass());             break;
				default:
					UNREACHABLE("pass: %d", int(pass));
				}
			}

			passManager->run(*module);
		}

		std::shared_ptr<rr::Routine> acquireRoutine(llvm::Function **funcs, size_t count, const rr::Config &cfg)
		{
			ASSERT(module);
			return std::make_shared<JITRoutine>(std::move(module), funcs, count, cfg);
		}

		const rr::Config config;
		llvm::LLVMContext context;
		std::unique_ptr<llvm::Module> module;
		std::unique_ptr<llvm::IRBuilder<>> builder;
		llvm::Function *function = nullptr;

		struct CoroutineState
		{
			llvm::Function *await = nullptr;
			llvm::Function *destroy = nullptr;
			llvm::Value *handle = nullptr;
			llvm::Value *id = nullptr;
			llvm::Value *promise = nullptr;
			llvm::Type *yieldType = nullptr;
			llvm::BasicBlock *entryBlock = nullptr;
			llvm::BasicBlock *suspendBlock = nullptr;
			llvm::BasicBlock *endBlock = nullptr;
			llvm::BasicBlock *destroyBlock = nullptr;
		};
		CoroutineState coroutine;

#ifdef ENABLE_RR_DEBUG_INFO
		std::unique_ptr<rr::DebugInfo> debugInfo;
#endif
	};

	std::unique_ptr<JITBuilder> jit;
	std::mutex codegenMutex;

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

	template <typename T>
	T alignUp(T val, T alignment)
	{
		return alignment * ((val + alignment - 1) / alignment);
	}

	void* alignedAlloc(size_t size, size_t alignment)
	{
		ASSERT(alignment < 256);
		auto allocation = new uint8_t[size + sizeof(uint8_t) + alignment];
		auto aligned = allocation;
		aligned += sizeof(uint8_t); // Make space for the base-address offset.
		aligned = reinterpret_cast<uint8_t*>(alignUp(reinterpret_cast<uintptr_t>(aligned), alignment)); // align
		auto offset = static_cast<uint8_t>(aligned - allocation);
		aligned[-1] = offset;
		return aligned;
	}

	void alignedFree(void* ptr)
	{
		auto aligned = reinterpret_cast<uint8_t*>(ptr);
		auto offset = aligned[-1];
		auto allocation = aligned - offset;
		delete[] allocation;
	}

	llvm::Value *lowerPAVG(llvm::Value *x, llvm::Value *y)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());

		llvm::VectorType *extTy =
			llvm::VectorType::getExtendedElementVectorType(ty);
		x = jit->builder->CreateZExt(x, extTy);
		y = jit->builder->CreateZExt(y, extTy);

		// (x + y + 1) >> 1
		llvm::Constant *one = llvm::ConstantInt::get(extTy, 1);
		llvm::Value *res = jit->builder->CreateAdd(x, y);
		res = jit->builder->CreateAdd(res, one);
		res = jit->builder->CreateLShr(res, one);
		return jit->builder->CreateTrunc(res, ty);
	}

	llvm::Value *lowerPMINMAX(llvm::Value *x, llvm::Value *y,
	                          llvm::ICmpInst::Predicate pred)
	{
		return jit->builder->CreateSelect(jit->builder->CreateICmp(pred, x, y), x, y);
	}

	llvm::Value *lowerPCMP(llvm::ICmpInst::Predicate pred, llvm::Value *x,
	                       llvm::Value *y, llvm::Type *dstTy)
	{
		return jit->builder->CreateSExt(jit->builder->CreateICmp(pred, x, y), dstTy, "");
	}

#if defined(__i386__) || defined(__x86_64__)
	llvm::Value *lowerPMOV(llvm::Value *op, llvm::Type *dstType, bool sext)
	{
		llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(op->getType());
		llvm::VectorType *dstTy = llvm::cast<llvm::VectorType>(dstType);

		llvm::Value *undef = llvm::UndefValue::get(srcTy);
		llvm::SmallVector<uint32_t, 16> mask(dstTy->getNumElements());
		std::iota(mask.begin(), mask.end(), 0);
		llvm::Value *v = jit->builder->CreateShuffleVector(op, undef, mask);

		return sext ? jit->builder->CreateSExt(v, dstTy)
		            : jit->builder->CreateZExt(v, dstTy);
	}

	llvm::Value *lowerPABS(llvm::Value *v)
	{
		llvm::Value *zero = llvm::Constant::getNullValue(v->getType());
		llvm::Value *cmp = jit->builder->CreateICmp(llvm::ICmpInst::ICMP_SGT, v, zero);
		llvm::Value *neg = jit->builder->CreateNeg(v);
		return jit->builder->CreateSelect(cmp, v, neg);
	}
#endif  // defined(__i386__) || defined(__x86_64__)

#if !defined(__i386__) && !defined(__x86_64__)
	llvm::Value *lowerPFMINMAX(llvm::Value *x, llvm::Value *y,
	                           llvm::FCmpInst::Predicate pred)
	{
		return jit->builder->CreateSelect(jit->builder->CreateFCmp(pred, x, y), x, y);
	}

	llvm::Value *lowerRound(llvm::Value *x)
	{
		llvm::Function *nearbyint = llvm::Intrinsic::getDeclaration(
			jit->module.get(), llvm::Intrinsic::nearbyint, {x->getType()});
		return jit->builder->CreateCall(nearbyint, ARGS(x));
	}

	llvm::Value *lowerRoundInt(llvm::Value *x, llvm::Type *ty)
	{
		return jit->builder->CreateFPToSI(lowerRound(x), ty);
	}

	llvm::Value *lowerFloor(llvm::Value *x)
	{
		llvm::Function *floor = llvm::Intrinsic::getDeclaration(
			jit->module.get(), llvm::Intrinsic::floor, {x->getType()});
		return jit->builder->CreateCall(floor, ARGS(x));
	}

	llvm::Value *lowerTrunc(llvm::Value *x)
	{
		llvm::Function *trunc = llvm::Intrinsic::getDeclaration(
			jit->module.get(), llvm::Intrinsic::trunc, {x->getType()});
		return jit->builder->CreateCall(trunc, ARGS(x));
	}

	// Packed add/sub with saturation
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
			extX = jit->builder->CreateSExt(x, extTy);
			extY = jit->builder->CreateSExt(y, extTy);
		}
		else
		{
			ASSERT_MSG(numBits <= 64, "numBits: %d", int(numBits));
			uint64_t maxVal = (numBits == 64) ? ~0ULL : (1ULL << numBits) - 1;
			max = llvm::ConstantInt::get(extTy, maxVal, false);
			min = llvm::ConstantInt::get(extTy, 0, false);
			extX = jit->builder->CreateZExt(x, extTy);
			extY = jit->builder->CreateZExt(y, extTy);
		}

		llvm::Value *res = isAdd ? jit->builder->CreateAdd(extX, extY)
		                         : jit->builder->CreateSub(extX, extY);

		res = lowerPMINMAX(res, min, llvm::ICmpInst::ICMP_SGT);
		res = lowerPMINMAX(res, max, llvm::ICmpInst::ICMP_SLT);

		return jit->builder->CreateTrunc(res, ty);
	}

	llvm::Value *lowerSQRT(llvm::Value *x)
	{
		llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(
			jit->module.get(), llvm::Intrinsic::sqrt, {x->getType()});
		return jit->builder->CreateCall(sqrt, ARGS(x));
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
		return jit->builder->CreateFDiv(one, x);
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
		return jit->builder->CreateShl(x, y);
	}

	llvm::Value *lowerVectorAShr(llvm::Value *x, uint64_t scalarY)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Value *y = llvm::ConstantVector::getSplat(
			ty->getNumElements(),
			llvm::ConstantInt::get(ty->getElementType(), scalarY));
		return jit->builder->CreateAShr(x, y);
	}

	llvm::Value *lowerVectorLShr(llvm::Value *x, uint64_t scalarY)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Value *y = llvm::ConstantVector::getSplat(
			ty->getNumElements(),
			llvm::ConstantInt::get(ty->getElementType(), scalarY));
		return jit->builder->CreateLShr(x, y);
	}

	llvm::Value *lowerMulAdd(llvm::Value *x, llvm::Value *y)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

		llvm::Value *extX = jit->builder->CreateSExt(x, extTy);
		llvm::Value *extY = jit->builder->CreateSExt(y, extTy);
		llvm::Value *mult = jit->builder->CreateMul(extX, extY);

		llvm::Value *undef = llvm::UndefValue::get(extTy);

		llvm::SmallVector<uint32_t, 16> evenIdx;
		llvm::SmallVector<uint32_t, 16> oddIdx;
		for (uint64_t i = 0, n = ty->getNumElements(); i < n; i += 2)
		{
			evenIdx.push_back(i);
			oddIdx.push_back(i + 1);
		}

		llvm::Value *lhs = jit->builder->CreateShuffleVector(mult, undef, evenIdx);
		llvm::Value *rhs = jit->builder->CreateShuffleVector(mult, undef, oddIdx);
		return jit->builder->CreateAdd(lhs, rhs);
	}

	llvm::Value *lowerPack(llvm::Value *x, llvm::Value *y, bool isSigned)
	{
		llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *dstTy = llvm::VectorType::getTruncatedElementVectorType(srcTy);

		llvm::IntegerType *dstElemTy =
			llvm::cast<llvm::IntegerType>(dstTy->getElementType());

		uint64_t truncNumBits = dstElemTy->getIntegerBitWidth();
		ASSERT_MSG(truncNumBits < 64, "shift 64 must be handled separately. truncNumBits: %d", int(truncNumBits));
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

		x = jit->builder->CreateTrunc(x, dstTy);
		y = jit->builder->CreateTrunc(y, dstTy);

		llvm::SmallVector<uint32_t, 16> index(srcTy->getNumElements() * 2);
		std::iota(index.begin(), index.end(), 0);

		return jit->builder->CreateShuffleVector(x, y, index);
	}

	llvm::Value *lowerSignMask(llvm::Value *x, llvm::Type *retTy)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Constant *zero = llvm::ConstantInt::get(ty, 0);
		llvm::Value *cmp = jit->builder->CreateICmpSLT(x, zero);

		llvm::Value *ret = jit->builder->CreateZExt(
			jit->builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
		for (uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
		{
			llvm::Value *elem = jit->builder->CreateZExt(
				jit->builder->CreateExtractElement(cmp, i), retTy);
			ret = jit->builder->CreateOr(ret, jit->builder->CreateShl(elem, i));
		}
		return ret;
	}

	llvm::Value *lowerFPSignMask(llvm::Value *x, llvm::Type *retTy)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::Constant *zero = llvm::ConstantFP::get(ty, 0);
		llvm::Value *cmp = jit->builder->CreateFCmpULT(x, zero);

		llvm::Value *ret = jit->builder->CreateZExt(
			jit->builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
		for (uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
		{
			llvm::Value *elem = jit->builder->CreateZExt(
				jit->builder->CreateExtractElement(cmp, i), retTy);
			ret = jit->builder->CreateOr(ret, jit->builder->CreateShl(elem, i));
		}
		return ret;
	}
#endif  // !defined(__i386__) && !defined(__x86_64__)

#if (LLVM_VERSION_MAJOR >= 8) || (!defined(__i386__) && !defined(__x86_64__))
	llvm::Value *lowerPUADDSAT(llvm::Value *x, llvm::Value *y)
	{
		#if LLVM_VERSION_MAJOR >= 8
			return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::uadd_sat, x, y);
		#else
			return lowerPSAT(x, y, true, false);
		#endif
	}

	llvm::Value *lowerPSADDSAT(llvm::Value *x, llvm::Value *y)
	{
		#if LLVM_VERSION_MAJOR >= 8
			return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::sadd_sat, x, y);
		#else
			return lowerPSAT(x, y, true, true);
		#endif
	}

	llvm::Value *lowerPUSUBSAT(llvm::Value *x, llvm::Value *y)
	{
		#if LLVM_VERSION_MAJOR >= 8
			return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::usub_sat, x, y);
		#else
			return lowerPSAT(x, y, false, false);
		#endif
	}

	llvm::Value *lowerPSSUBSAT(llvm::Value *x, llvm::Value *y)
	{
		#if LLVM_VERSION_MAJOR >= 8
			return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::ssub_sat, x, y);
		#else
			return lowerPSAT(x, y, false, true);
		#endif
	}
#endif  // (LLVM_VERSION_MAJOR >= 8) || (!defined(__i386__) && !defined(__x86_64__))

	llvm::Value *lowerMulHigh(llvm::Value *x, llvm::Value *y, bool sext)
	{
		llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
		llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

		llvm::Value *extX, *extY;
		if (sext)
		{
			extX = jit->builder->CreateSExt(x, extTy);
			extY = jit->builder->CreateSExt(y, extTy);
		}
		else
		{
			extX = jit->builder->CreateZExt(x, extTy);
			extY = jit->builder->CreateZExt(y, extTy);
		}

		llvm::Value *mult = jit->builder->CreateMul(extX, extY);

		llvm::IntegerType *intTy = llvm::cast<llvm::IntegerType>(ty->getElementType());
		llvm::Value *mulh = jit->builder->CreateAShr(mult, intTy->getBitWidth());
		return jit->builder->CreateTrunc(mulh, ty);
	}
}

namespace rr
{
	const Capabilities Caps =
	{
		true, // CoroutinesSupported
	};

	static std::memory_order atomicOrdering(llvm::AtomicOrdering memoryOrder)
	{
		switch(memoryOrder)
		{
		case llvm::AtomicOrdering::Monotonic: return std::memory_order_relaxed;  // https://llvm.org/docs/Atomics.html#monotonic
		case llvm::AtomicOrdering::Acquire: return std::memory_order_acquire;
		case llvm::AtomicOrdering::Release: return std::memory_order_release;
		case llvm::AtomicOrdering::AcquireRelease: return std::memory_order_acq_rel;
		case llvm::AtomicOrdering::SequentiallyConsistent: return std::memory_order_seq_cst;
		default:
			UNREACHABLE("memoryOrder: %d", int(memoryOrder));
			return std::memory_order_acq_rel;
		}
	}

	static llvm::AtomicOrdering atomicOrdering(bool atomic, std::memory_order memoryOrder)
	{
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
		default:
			UNREACHABLE("memoryOrder: %d", int(memoryOrder));
			return llvm::AtomicOrdering::AcquireRelease;
		}
	}

	template <typename T>
	static void atomicLoad(void *ptr, void *ret, llvm::AtomicOrdering ordering)
	{
		*reinterpret_cast<T*>(ret) = std::atomic_load_explicit<T>(reinterpret_cast<std::atomic<T>*>(ptr), atomicOrdering(ordering));
	}

	template <typename T>
	static void atomicStore(void *ptr, void *val, llvm::AtomicOrdering ordering)
	{
		std::atomic_store_explicit<T>(reinterpret_cast<std::atomic<T>*>(ptr), *reinterpret_cast<T*>(val), atomicOrdering(ordering));
	}

#ifdef __ANDROID__
	template<typename F>
	static uint32_t sync_fetch_and_op(uint32_t volatile *ptr, uint32_t val, F f)
	{
		// Build an arbitrary op out of looped CAS
		for (;;)
		{
			uint32_t expected = *ptr;
			uint32_t desired = f(expected, val);

			if (expected == __sync_val_compare_and_swap_4(ptr, expected, desired))
				return expected;
		}
	}
#endif

	void* resolveExternalSymbol(const char* name)
	{
		struct Atomic
		{
			static void load(size_t size, void *ptr, void *ret, llvm::AtomicOrdering ordering)
			{
				switch (size)
				{
					case 1: atomicLoad<uint8_t>(ptr, ret, ordering); break;
					case 2: atomicLoad<uint16_t>(ptr, ret, ordering); break;
					case 4: atomicLoad<uint32_t>(ptr, ret, ordering); break;
					case 8: atomicLoad<uint64_t>(ptr, ret, ordering); break;
					default:
						UNIMPLEMENTED("Atomic::load(size: %d)", int(size));
				}
			}
			static void store(size_t size, void *ptr, void *ret, llvm::AtomicOrdering ordering)
			{
				switch (size)
				{
					case 1: atomicStore<uint8_t>(ptr, ret, ordering); break;
					case 2: atomicStore<uint16_t>(ptr, ret, ordering); break;
					case 4: atomicStore<uint32_t>(ptr, ret, ordering); break;
					case 8: atomicStore<uint64_t>(ptr, ret, ordering); break;
					default:
						UNIMPLEMENTED("Atomic::store(size: %d)", int(size));
				}
			}
		};

		struct F
		{
			static void nop() {}
			static void neverCalled() { UNREACHABLE("Should never be called"); }

			static void* coroutine_alloc_frame(size_t size) { return alignedAlloc(size, 16); }
			static void coroutine_free_frame(void* ptr) { alignedFree(ptr); }

#ifdef __ANDROID__
			// forwarders since we can't take address of builtins
			static void sync_synchronize() { __sync_synchronize(); }
			static uint32_t sync_fetch_and_add_4(uint32_t *ptr, uint32_t val) { return __sync_fetch_and_add_4(ptr, val); }
			static uint32_t sync_fetch_and_and_4(uint32_t *ptr, uint32_t val) { return __sync_fetch_and_and_4(ptr, val); }
			static uint32_t sync_fetch_and_or_4(uint32_t *ptr, uint32_t val) { return __sync_fetch_and_or_4(ptr, val); }
			static uint32_t sync_fetch_and_xor_4(uint32_t *ptr, uint32_t val) { return __sync_fetch_and_xor_4(ptr, val); }
			static uint32_t sync_fetch_and_sub_4(uint32_t *ptr, uint32_t val) { return __sync_fetch_and_sub_4(ptr, val); }
			static uint32_t sync_lock_test_and_set_4(uint32_t *ptr, uint32_t val) { return __sync_lock_test_and_set_4(ptr, val); }
			static uint32_t sync_val_compare_and_swap_4(uint32_t *ptr, uint32_t expected, uint32_t desired) { return __sync_val_compare_and_swap_4(ptr, expected, desired); }

			static uint32_t sync_fetch_and_max_4(uint32_t *ptr, uint32_t val) { return sync_fetch_and_op(ptr, val, [](int32_t a, int32_t b) { return std::max(a,b);}); }
			static uint32_t sync_fetch_and_min_4(uint32_t *ptr, uint32_t val) { return sync_fetch_and_op(ptr, val, [](int32_t a, int32_t b) { return std::min(a,b);}); }
			static uint32_t sync_fetch_and_umax_4(uint32_t *ptr, uint32_t val) { return sync_fetch_and_op(ptr, val, [](uint32_t a, uint32_t b) { return std::max(a,b);}); }
			static uint32_t sync_fetch_and_umin_4(uint32_t *ptr, uint32_t val) { return sync_fetch_and_op(ptr, val, [](uint32_t a, uint32_t b) { return std::min(a,b);}); }
#endif
		};

		class Resolver
		{
		public:
			using FunctionMap = std::unordered_map<std::string, void *>;

			FunctionMap functions;

			Resolver()
			{
				functions.emplace("nop", reinterpret_cast<void*>(F::nop));
				functions.emplace("floorf", reinterpret_cast<void*>(floorf));
				functions.emplace("nearbyintf", reinterpret_cast<void*>(nearbyintf));
				functions.emplace("truncf", reinterpret_cast<void*>(truncf));
				functions.emplace("printf", reinterpret_cast<void*>(printf));
				functions.emplace("puts", reinterpret_cast<void*>(puts));
				functions.emplace("fmodf", reinterpret_cast<void*>(fmodf));

				functions.emplace("sinf", reinterpret_cast<void*>(sinf));
				functions.emplace("cosf", reinterpret_cast<void*>(cosf));
				functions.emplace("asinf", reinterpret_cast<void*>(asinf));
				functions.emplace("acosf", reinterpret_cast<void*>(acosf));
				functions.emplace("atanf", reinterpret_cast<void*>(atanf));
				functions.emplace("sinhf", reinterpret_cast<void*>(sinhf));
				functions.emplace("coshf", reinterpret_cast<void*>(coshf));
				functions.emplace("tanhf", reinterpret_cast<void*>(tanhf));
				functions.emplace("asinhf", reinterpret_cast<void*>(asinhf));
				functions.emplace("acoshf", reinterpret_cast<void*>(acoshf));
				functions.emplace("atanhf", reinterpret_cast<void*>(atanhf));
				functions.emplace("atan2f", reinterpret_cast<void*>(atan2f));
				functions.emplace("powf", reinterpret_cast<void*>(powf));
				functions.emplace("expf", reinterpret_cast<void*>(expf));
				functions.emplace("logf", reinterpret_cast<void*>(logf));
				functions.emplace("exp2f", reinterpret_cast<void*>(exp2f));
				functions.emplace("log2f", reinterpret_cast<void*>(log2f));

				functions.emplace("sin", reinterpret_cast<void*>(static_cast<double(*)(double)>(sin)));
				functions.emplace("cos", reinterpret_cast<void*>(static_cast<double(*)(double)>(cos)));
				functions.emplace("asin", reinterpret_cast<void*>(static_cast<double(*)(double)>(asin)));
				functions.emplace("acos", reinterpret_cast<void*>(static_cast<double(*)(double)>(acos)));
				functions.emplace("atan", reinterpret_cast<void*>(static_cast<double(*)(double)>(atan)));
				functions.emplace("sinh", reinterpret_cast<void*>(static_cast<double(*)(double)>(sinh)));
				functions.emplace("cosh", reinterpret_cast<void*>(static_cast<double(*)(double)>(cosh)));
				functions.emplace("tanh", reinterpret_cast<void*>(static_cast<double(*)(double)>(tanh)));
				functions.emplace("asinh", reinterpret_cast<void*>(static_cast<double(*)(double)>(asinh)));
				functions.emplace("acosh", reinterpret_cast<void*>(static_cast<double(*)(double)>(acosh)));
				functions.emplace("atanh", reinterpret_cast<void*>(static_cast<double(*)(double)>(atanh)));
				functions.emplace("atan2", reinterpret_cast<void*>(static_cast<double(*)(double,double)>(atan2)));
				functions.emplace("pow", reinterpret_cast<void*>(static_cast<double(*)(double,double)>(pow)));
				functions.emplace("exp", reinterpret_cast<void*>(static_cast<double(*)(double)>(exp)));
				functions.emplace("log", reinterpret_cast<void*>(static_cast<double(*)(double)>(log)));
				functions.emplace("exp2", reinterpret_cast<void*>(static_cast<double(*)(double)>(exp2)));
				functions.emplace("log2", reinterpret_cast<void*>(static_cast<double(*)(double)>(log2)));

				functions.emplace("atomic_load", reinterpret_cast<void*>(Atomic::load));
				functions.emplace("atomic_store", reinterpret_cast<void*>(Atomic::store));

				// FIXME (b/119409619): use an allocator here so we can control all memory allocations
				functions.emplace("coroutine_alloc_frame", reinterpret_cast<void*>(F::coroutine_alloc_frame));
				functions.emplace("coroutine_free_frame", reinterpret_cast<void*>(F::coroutine_free_frame));

#ifdef __APPLE__
				functions.emplace("sincosf_stret", reinterpret_cast<void*>(__sincosf_stret));
#elif defined(__linux__)
				functions.emplace("sincosf", reinterpret_cast<void*>(sincosf));
#elif defined(_WIN64)
				functions.emplace("chkstk", reinterpret_cast<void*>(__chkstk));
#elif defined(_WIN32)
				functions.emplace("chkstk", reinterpret_cast<void*>(_chkstk));
#endif

#ifdef __ANDROID__
				functions.emplace("aeabi_unwind_cpp_pr0", reinterpret_cast<void*>(F::neverCalled));
				functions.emplace("sync_synchronize", reinterpret_cast<void*>(F::sync_synchronize));
				functions.emplace("sync_fetch_and_add_4", reinterpret_cast<void*>(F::sync_fetch_and_add_4));
				functions.emplace("sync_fetch_and_and_4", reinterpret_cast<void*>(F::sync_fetch_and_and_4));
				functions.emplace("sync_fetch_and_or_4", reinterpret_cast<void*>(F::sync_fetch_and_or_4));
				functions.emplace("sync_fetch_and_xor_4", reinterpret_cast<void*>(F::sync_fetch_and_xor_4));
				functions.emplace("sync_fetch_and_sub_4", reinterpret_cast<void*>(F::sync_fetch_and_sub_4));
				functions.emplace("sync_lock_test_and_set_4", reinterpret_cast<void*>(F::sync_lock_test_and_set_4));
				functions.emplace("sync_val_compare_and_swap_4", reinterpret_cast<void*>(F::sync_val_compare_and_swap_4));
				functions.emplace("sync_fetch_and_max_4", reinterpret_cast<void*>(F::sync_fetch_and_max_4));
				functions.emplace("sync_fetch_and_min_4", reinterpret_cast<void*>(F::sync_fetch_and_min_4));
				functions.emplace("sync_fetch_and_umax_4", reinterpret_cast<void*>(F::sync_fetch_and_umax_4));
				functions.emplace("sync_fetch_and_umin_4", reinterpret_cast<void*>(F::sync_fetch_and_umin_4));
	#endif
			}
		};

		static Resolver resolver;

		// Trim off any underscores from the start of the symbol. LLVM likes
		// to append these on macOS.
		const char* trimmed = name;
		while (trimmed[0] == '_') { trimmed++; }

		auto it = resolver.functions.find(trimmed);
		// Missing functions will likely make the module fail in exciting non-obvious ways.
		ASSERT_MSG(it != resolver.functions.end(), "Missing external function: '%s'", name);
		return it->second;
	}

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
		default:
			UNREACHABLE("asInternalType(t): %d", int(asInternalType(t)));
			return nullptr;
		}
	}

	Type *T(InternalType t)
	{
		return reinterpret_cast<Type*>(t);
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
				ASSERT_MSG(bits != 0, "bits: %d", int(bits));

				// TODO(capn): Booleans are 1 bit integers in LLVM's SSA type system,
				// but are typically stored as one byte. The DataLayout structure should
				// be used here and many other places if this assumption fails.
				return (bits + 7) / 8;
			}
			break;
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
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
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return 0;
		}
	}

	static ::llvm::Function* createFunction(const char *name, ::llvm::Type *retTy, const std::vector<::llvm::Type*> &params)
	{
		llvm::FunctionType *functionType = llvm::FunctionType::get(retTy, params, false);
		auto func = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, name, jit->module.get());
		func->setDoesNotThrow();
		func->setCallingConv(llvm::CallingConv::C);
		return func;
	}

	Nucleus::Nucleus()
	{
		::codegenMutex.lock();   // Reactor and LLVM are currently not thread safe

		ASSERT(jit == nullptr);
		jit.reset(new JITBuilder(Nucleus::getDefaultConfig()));
	}

	Nucleus::~Nucleus()
	{
		jit.reset();
		::codegenMutex.unlock();
	}

	void Nucleus::setDefaultConfig(const Config &cfg)
	{
		std::unique_lock<std::mutex> lock(::defaultConfigLock);
		::defaultConfig() = cfg;
	}

	void Nucleus::adjustDefaultConfig(const Config::Edit &cfgEdit)
	{
		std::unique_lock<std::mutex> lock(::defaultConfigLock);
		auto &config = ::defaultConfig();
		config = cfgEdit.apply(config);
	}

	Config Nucleus::getDefaultConfig()
	{
		std::unique_lock<std::mutex> lock(::defaultConfigLock);
		return ::defaultConfig();
	}

	std::shared_ptr<Routine> Nucleus::acquireRoutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
	{
		auto cfg = cfgEdit.apply(jit->config);

		if(jit->builder->GetInsertBlock()->empty() || !jit->builder->GetInsertBlock()->back().isTerminator())
		{
			llvm::Type *type = jit->function->getReturnType();

			if(type->isVoidTy())
			{
				createRetVoid();
			}
			else
			{
				createRet(V(llvm::UndefValue::get(type)));
			}
		}

#ifdef ENABLE_RR_DEBUG_INFO
		if (jit->debugInfo != nullptr)
		{
			jit->debugInfo->Finalize();
		}
#endif // ENABLE_RR_DEBUG_INFO

		if(false)
		{
			std::error_code error;
			llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-unopt.txt", error);
			jit->module->print(file, 0);
		}

#if defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)
		{
			llvm::legacy::PassManager pm;
			pm.add(llvm::createVerifierPass());
			pm.run(*jit->module);
		}
#endif // defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)

		jit->optimize(cfg);

		if(false)
		{
			std::error_code error;
			llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-opt.txt", error);
			jit->module->print(file, 0);
		}

		auto routine = jit->acquireRoutine(&jit->function, 1, cfg);
		jit.reset();

		return routine;
	}

	Value *Nucleus::allocateStackVariable(Type *type, int arraySize)
	{
		// Need to allocate it in the entry block for mem2reg to work
		llvm::BasicBlock &entryBlock = jit->function->getEntryBlock();

		llvm::Instruction *declaration;

		if(arraySize)
		{
			declaration = new llvm::AllocaInst(T(type), 0, V(Nucleus::createConstantInt(arraySize)));
		}
		else
		{
			declaration = new llvm::AllocaInst(T(type), 0, (llvm::Value*)nullptr);
		}

		entryBlock.getInstList().push_front(declaration);

		return V(declaration);
	}

	BasicBlock *Nucleus::createBasicBlock()
	{
		return B(llvm::BasicBlock::Create(jit->context, "", jit->function));
	}

	BasicBlock *Nucleus::getInsertBlock()
	{
		return B(jit->builder->GetInsertBlock());
	}

	void Nucleus::setInsertBlock(BasicBlock *basicBlock)
	{
	//	assert(jit->builder->GetInsertBlock()->back().isTerminator());

		Variable::materializeAll();

		jit->builder->SetInsertPoint(B(basicBlock));
	}

	void Nucleus::createFunction(Type *ReturnType, std::vector<Type*> &Params)
	{
		jit->function = rr::createFunction("", T(ReturnType), T(Params));

#ifdef ENABLE_RR_DEBUG_INFO
		jit->debugInfo = std::unique_ptr<DebugInfo>(new DebugInfo(jit->builder.get(), &jit->context, jit->module.get(), jit->function));
#endif // ENABLE_RR_DEBUG_INFO

		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->function));
	}

	Value *Nucleus::getArgument(unsigned int index)
	{
		llvm::Function::arg_iterator args = jit->function->arg_begin();

		while(index)
		{
			args++;
			index--;
		}

		return V(&*args);
	}

	void Nucleus::createRetVoid()
	{
		RR_DEBUG_INFO_UPDATE_LOC();

		ASSERT_MSG(jit->function->getReturnType() == T(Void::getType()), "Return type mismatch");

		// Code generated after this point is unreachable, so any variables
		// being read can safely return an undefined value. We have to avoid
		// materializing variables after the terminator ret instruction.
		Variable::killUnmaterialized();

		jit->builder->CreateRetVoid();
	}

	void Nucleus::createRet(Value *v)
	{
		RR_DEBUG_INFO_UPDATE_LOC();

		ASSERT_MSG(jit->function->getReturnType() == V(v)->getType(), "Return type mismatch");

		// Code generated after this point is unreachable, so any variables
		// being read can safely return an undefined value. We have to avoid
		// materializing variables after the terminator ret instruction.
		Variable::killUnmaterialized();

		jit->builder->CreateRet(V(v));
	}

	void Nucleus::createBr(BasicBlock *dest)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Variable::materializeAll();

		jit->builder->CreateBr(B(dest));
	}

	void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Variable::materializeAll();
		jit->builder->CreateCondBr(V(cond), B(ifTrue), B(ifFalse));
	}

	Value *Nucleus::createAdd(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAdd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSub(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSub(V(lhs), V(rhs)));
	}

	Value *Nucleus::createMul(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateMul(V(lhs), V(rhs)));
	}

	Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateUDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFAdd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFSub(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFSub(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFMul(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFMul(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFDiv(V(lhs), V(rhs)));
	}

	Value *Nucleus::createURem(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateURem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createSRem(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSRem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFRem(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFRem(V(lhs), V(rhs)));
	}

	Value *Nucleus::createShl(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateShl(V(lhs), V(rhs)));
	}

	Value *Nucleus::createLShr(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateLShr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createAShr(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAShr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createAnd(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAnd(V(lhs), V(rhs)));
	}

	Value *Nucleus::createOr(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateOr(V(lhs), V(rhs)));
	}

	Value *Nucleus::createXor(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateXor(V(lhs), V(rhs)));
	}

	Value *Nucleus::createNeg(Value *v)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateNeg(V(v)));
	}

	Value *Nucleus::createFNeg(Value *v)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFNeg(V(v)));
	}

	Value *Nucleus::createNot(Value *v)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateNot(V(v)));
	}

	Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
				auto elTy = T(type);
				ASSERT(V(ptr)->getType()->getContainedType(0) == elTy);

				if (!atomic)
				{
					return V(jit->builder->CreateAlignedLoad(V(ptr), alignment, isVolatile));
				}
				else if (elTy->isIntegerTy() || elTy->isPointerTy())
				{
					// Integers and pointers can be atomically loaded by setting
					// the ordering constraint on the load instruction.
					auto load = jit->builder->CreateAlignedLoad(V(ptr), alignment, isVolatile);
					load->setAtomic(atomicOrdering(atomic, memoryOrder));
					return V(load);
				}
				else if (elTy->isFloatTy() || elTy->isDoubleTy())
				{
					// LLVM claims to support atomic loads of float types as
					// above, but certain backends cannot deal with this.
					// Load as an integer and bitcast. See b/136037244.
  					auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
					auto elAsIntTy = ::llvm::IntegerType::get(jit->context, size * 8);
					auto ptrCast = jit->builder->CreatePointerCast(V(ptr), elAsIntTy->getPointerTo());
					auto load = jit->builder->CreateAlignedLoad(ptrCast, alignment, isVolatile);
					load->setAtomic(atomicOrdering(atomic, memoryOrder));
					auto loadCast = jit->builder->CreateBitCast(load, elTy);
					return V(loadCast);
				}
				else
				{
					// More exotic types require falling back to the extern:
					// void __atomic_load(size_t size, void *ptr, void *ret, int ordering)
					auto sizetTy = ::llvm::IntegerType::get(jit->context, sizeof(size_t) * 8);
					auto intTy = ::llvm::IntegerType::get(jit->context, sizeof(int) * 8);
					auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
					auto i8PtrTy = i8Ty->getPointerTo();
					auto voidTy = ::llvm::Type::getVoidTy(jit->context);
					auto funcTy = ::llvm::FunctionType::get(voidTy, {sizetTy, i8PtrTy, i8PtrTy, intTy}, false);
					auto func = jit->module->getOrInsertFunction("__atomic_load", funcTy);
  					auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
					auto out = allocateStackVariable(type);
					jit->builder->CreateCall(func, {
						::llvm::ConstantInt::get(sizetTy, size),
						jit->builder->CreatePointerCast(V(ptr), i8PtrTy),
						jit->builder->CreatePointerCast(V(out), i8PtrTy),
						::llvm::ConstantInt::get(intTy, uint64_t(atomicOrdering(true, memoryOrder))),
					 });
					 return V(jit->builder->CreateLoad(V(out)));
				}
			}
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return nullptr;
		}
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
				auto elTy = T(type);
				ASSERT(V(ptr)->getType()->getContainedType(0) == elTy);

				if (!atomic)
				{
					jit->builder->CreateAlignedStore(V(value), V(ptr), alignment, isVolatile);
				}
				else if (elTy->isIntegerTy() || elTy->isPointerTy())
				{
					// Integers and pointers can be atomically stored by setting
					// the ordering constraint on the store instruction.
					auto store = jit->builder->CreateAlignedStore(V(value), V(ptr), alignment, isVolatile);
					store->setAtomic(atomicOrdering(atomic, memoryOrder));
				}
				else if (elTy->isFloatTy() || elTy->isDoubleTy())
				{
					// LLVM claims to support atomic stores of float types as
					// above, but certain backends cannot deal with this.
					// Store as an bitcast integer. See b/136037244.
  					auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
					auto elAsIntTy = ::llvm::IntegerType::get(jit->context, size * 8);
					auto valCast = jit->builder->CreateBitCast(V(value), elAsIntTy);
					auto ptrCast = jit->builder->CreatePointerCast(V(ptr), elAsIntTy->getPointerTo());
					auto store = jit->builder->CreateAlignedStore(valCast, ptrCast, alignment, isVolatile);
					store->setAtomic(atomicOrdering(atomic, memoryOrder));
				}
				else
				{
					// More exotic types require falling back to the extern:
					// void __atomic_store(size_t size, void *ptr, void *val, int ordering)
					auto sizetTy = ::llvm::IntegerType::get(jit->context, sizeof(size_t) * 8);
					auto intTy = ::llvm::IntegerType::get(jit->context, sizeof(int) * 8);
					auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
					auto i8PtrTy = i8Ty->getPointerTo();
					auto voidTy = ::llvm::Type::getVoidTy(jit->context);
					auto funcTy = ::llvm::FunctionType::get(voidTy, {sizetTy, i8PtrTy, i8PtrTy, intTy}, false);
					auto func = jit->module->getOrInsertFunction("__atomic_store", funcTy);
  					auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
					auto copy = allocateStackVariable(type);
					jit->builder->CreateStore(V(value), V(copy));
					jit->builder->CreateCall(func, {
						::llvm::ConstantInt::get(sizetTy, size),
						jit->builder->CreatePointerCast(V(ptr), i8PtrTy),
						jit->builder->CreatePointerCast(V(copy), i8PtrTy),
						::llvm::ConstantInt::get(intTy, uint64_t(atomicOrdering(true, memoryOrder))),
					 });
				}

				return value;
			}
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return nullptr;
		}
	}

	Value *Nucleus::createMaskedLoad(Value *ptr, Type *elTy, Value *mask, unsigned int alignment, bool zeroMaskedLanes)
	{
		ASSERT(V(ptr)->getType()->isPointerTy());
		ASSERT(V(mask)->getType()->isVectorTy());

		auto numEls = V(mask)->getType()->getVectorNumElements();
		auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
		auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
		auto elVecTy = ::llvm::VectorType::get(T(elTy), numEls);
		auto elVecPtrTy = elVecTy->getPointerTo();
		auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false); // vec<int, int, ...> -> vec<bool, bool, ...>
		auto passthrough = zeroMaskedLanes ? ::llvm::Constant::getNullValue(elVecTy) : llvm::UndefValue::get(elVecTy);
		auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
		auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_load, { elVecTy, elVecPtrTy } );
		return V(jit->builder->CreateCall(func, { V(ptr), align, i8Mask, passthrough }));
	}

	void Nucleus::createMaskedStore(Value *ptr, Value *val, Value *mask, unsigned int alignment)
	{
		ASSERT(V(ptr)->getType()->isPointerTy());
		ASSERT(V(val)->getType()->isVectorTy());
		ASSERT(V(mask)->getType()->isVectorTy());

		auto numEls = V(mask)->getType()->getVectorNumElements();
		auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
		auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
		auto elVecTy = V(val)->getType();
		auto elVecPtrTy = elVecTy->getPointerTo();
		auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false); // vec<int, int, ...> -> vec<bool, bool, ...>
		auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
		auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_store, { elVecTy, elVecPtrTy } );
		jit->builder->CreateCall(func, { V(val), V(ptr), align, i8Mask });
	}

	Value *Nucleus::createGather(Value *base, Type *elTy, Value *offsets, Value *mask, unsigned int alignment, bool zeroMaskedLanes)
	{
		ASSERT(V(base)->getType()->isPointerTy());
		ASSERT(V(offsets)->getType()->isVectorTy());
		ASSERT(V(mask)->getType()->isVectorTy());

		auto numEls = V(mask)->getType()->getVectorNumElements();
		auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
		auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
		auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
		auto i8PtrTy = i8Ty->getPointerTo();
		auto elPtrTy = T(elTy)->getPointerTo();
		auto elVecTy = ::llvm::VectorType::get(T(elTy), numEls);
		auto elPtrVecTy = ::llvm::VectorType::get(elPtrTy, numEls);
		auto i8Base = jit->builder->CreatePointerCast(V(base), i8PtrTy);
		auto i8Ptrs = jit->builder->CreateGEP(i8Base, V(offsets));
		auto elPtrs = jit->builder->CreatePointerCast(i8Ptrs, elPtrVecTy);
		auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false); // vec<int, int, ...> -> vec<bool, bool, ...>
		auto passthrough = zeroMaskedLanes ? ::llvm::Constant::getNullValue(elVecTy) : llvm::UndefValue::get(elVecTy);
		auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
		auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_gather, { elVecTy, elPtrVecTy } );
		return V(jit->builder->CreateCall(func, { elPtrs, align, i8Mask, passthrough }));
	}

	void Nucleus::createScatter(Value *base, Value *val, Value *offsets, Value *mask, unsigned int alignment)
	{
		ASSERT(V(base)->getType()->isPointerTy());
		ASSERT(V(val)->getType()->isVectorTy());
		ASSERT(V(offsets)->getType()->isVectorTy());
		ASSERT(V(mask)->getType()->isVectorTy());

		auto numEls = V(mask)->getType()->getVectorNumElements();
		auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
		auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
		auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
		auto i8PtrTy = i8Ty->getPointerTo();
		auto elVecTy = V(val)->getType();
		auto elTy = elVecTy->getVectorElementType();
		auto elPtrTy = elTy->getPointerTo();
		auto elPtrVecTy = ::llvm::VectorType::get(elPtrTy, numEls);
		auto i8Base = jit->builder->CreatePointerCast(V(base), i8PtrTy);
		auto i8Ptrs = jit->builder->CreateGEP(i8Base, V(offsets));
		auto elPtrs = jit->builder->CreatePointerCast(i8Ptrs, elPtrVecTy);
		auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false); // vec<int, int, ...> -> vec<bool, bool, ...>
		auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
		auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_scatter, { elVecTy, elPtrVecTy } );
		jit->builder->CreateCall(func, { V(val), elPtrs, align, i8Mask });
	}

	void Nucleus::createFence(std::memory_order memoryOrder)
	{
		jit->builder->CreateFence(atomicOrdering(true, memoryOrder));
	}

	Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index, bool unsignedIndex)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		ASSERT(V(ptr)->getType()->getContainedType(0) == T(type));
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
			return V(jit->builder->CreateGEP(V(ptr), V(index)));
		}

		// For emulated types we have to multiply the index by the intended
		// type size ourselves to obain the byte offset.
		index = (sizeof(void*) == 8) ?
			createMul(index, createConstantLong((int64_t)typeSize(type))) :
			createMul(index, createConstantInt((int)typeSize(type)));

		// Cast to a byte pointer, apply the byte offset, and cast back to the
		// original pointer type.
		return createBitCast(
			V(jit->builder->CreateGEP(V(createBitCast(ptr, T(llvm::PointerType::get(T(Byte::getType()), 0)))), V(index))),
			T(llvm::PointerType::get(T(type), 0)));
	}

	Value *Nucleus::createAtomicAdd(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicSub(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicAnd(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::And, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicOr(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Or, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicXor(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Xor, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicMin(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Min, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicMax(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Max, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicUMin(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::UMin, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicUMax(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::UMax, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}


	Value *Nucleus::createAtomicExchange(Value *ptr, Value *value, std::memory_order memoryOrder)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Xchg, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
	}

	Value *Nucleus::createAtomicCompareExchange(Value *ptr, Value *value, Value *compare, std::memory_order memoryOrderEqual, std::memory_order memoryOrderUnequal)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		// Note: AtomicCmpXchgInstruction returns a 2-member struct containing {result, success-flag}, not the result directly.
		return V(jit->builder->CreateExtractValue(
				jit->builder->CreateAtomicCmpXchg(V(ptr), V(compare), V(value), atomicOrdering(true, memoryOrderEqual), atomicOrdering(true, memoryOrderUnequal)),
				llvm::ArrayRef<unsigned>(0u)));
	}

	Value *Nucleus::createTrunc(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateTrunc(V(v), T(destType)));
	}

	Value *Nucleus::createZExt(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateZExt(V(v), T(destType)));
	}

	Value *Nucleus::createSExt(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSExt(V(v), T(destType)));
	}

	Value *Nucleus::createFPToSI(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFPToSI(V(v), T(destType)));
	}

	Value *Nucleus::createSIToFP(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSIToFP(V(v), T(destType)));
	}

	Value *Nucleus::createFPTrunc(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFPTrunc(V(v), T(destType)));
	}

	Value *Nucleus::createFPExt(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFPExt(V(v), T(destType)));
	}

	Value *Nucleus::createBitCast(Value *v, Type *destType)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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

		return V(jit->builder->CreateBitCast(V(v), T(destType)));
	}

	Value *Nucleus::createPtrEQ(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpNE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpUGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpUGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpULT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpULE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpSGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpSGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpSLT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateICmpSLE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpOEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpOGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpOGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpOLT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpOLE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpONE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpORD(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpUNO(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpUEQ(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpUGT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpUGE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpULT(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpULE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateFCmpUNE(V(lhs), V(rhs)));
	}

	Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		ASSERT(V(vector)->getType()->getContainedType(0) == T(type));
		return V(jit->builder->CreateExtractElement(V(vector), V(createConstantInt(index))));
	}

	Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateInsertElement(V(vector), V(element), V(createConstantInt(index))));
	}

	Value *Nucleus::createShuffleVector(Value *v1, Value *v2, const int *select)
	{
		RR_DEBUG_INFO_UPDATE_LOC();

		int size = llvm::cast<llvm::VectorType>(V(v1)->getType())->getNumElements();
		const int maxSize = 16;
		llvm::Constant *swizzle[maxSize];
		ASSERT(size <= maxSize);

		for(int i = 0; i < size; i++)
		{
			swizzle[i] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), select[i]);
		}

		llvm::Value *shuffle = llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(swizzle, size));

		return V(jit->builder->CreateShuffleVector(V(v1), V(v2), shuffle));
	}

	Value *Nucleus::createSelect(Value *c, Value *ifTrue, Value *ifFalse)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(jit->builder->CreateSelect(V(c), V(ifTrue), V(ifFalse)));
	}

	SwitchCases *Nucleus::createSwitch(Value *control, BasicBlock *defaultBranch, unsigned numCases)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return reinterpret_cast<SwitchCases*>(jit->builder->CreateSwitch(V(control), B(defaultBranch), numCases));
	}

	void Nucleus::addSwitchCase(SwitchCases *switchCases, int label, BasicBlock *branch)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		llvm::SwitchInst *sw = reinterpret_cast<llvm::SwitchInst *>(switchCases);
		sw->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), label, true), B(branch));
	}

	void Nucleus::createUnreachable()
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		jit->builder->CreateUnreachable();
	}

	Type *Nucleus::getPointerType(Type *ElementType)
	{
		return T(llvm::PointerType::get(T(ElementType), 0));
	}

	Value *Nucleus::createNullValue(Type *Ty)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::Constant::getNullValue(T(Ty)));
	}

	Value *Nucleus::createConstantLong(int64_t i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt64Ty(jit->context), i, true));
	}

	Value *Nucleus::createConstantInt(int i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), i, true));
	}

	Value *Nucleus::createConstantInt(unsigned int i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), i, false));
	}

	Value *Nucleus::createConstantBool(bool b)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt1Ty(jit->context), b));
	}

	Value *Nucleus::createConstantByte(signed char i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(jit->context), i, true));
	}

	Value *Nucleus::createConstantByte(unsigned char i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(jit->context), i, false));
	}

	Value *Nucleus::createConstantShort(short i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(jit->context), i, true));
	}

	Value *Nucleus::createConstantShort(unsigned short i)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(jit->context), i, false));
	}

	Value *Nucleus::createConstantFloat(float x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantFP::get(T(Float::getType()), x));
	}

	Value *Nucleus::createNullPointer(Type *Ty)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return V(llvm::ConstantPointerNull::get(llvm::PointerType::get(T(Ty), 0)));
	}

	Value *Nucleus::createConstantVector(const int64_t *constants, Type *type)
	{
		ASSERT(llvm::isa<llvm::VectorType>(T(type)));
		const int numConstants = elementCount(type);                                       // Number of provided constants for the (emulated) type.
		const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();   // Number of elements of the underlying vector type.
		ASSERT(numElements <= 16 && numConstants <= numElements);
		llvm::Constant *constantVector[16];

		for(int i = 0; i < numElements; i++)
		{
			constantVector[i] = llvm::ConstantInt::get(T(type)->getContainedType(0), constants[i % numConstants]);
		}

		return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(constantVector, numElements)));
	}

	Value *Nucleus::createConstantVector(const double *constants, Type *type)
	{
		ASSERT(llvm::isa<llvm::VectorType>(T(type)));
		const int numConstants = elementCount(type);                                       // Number of provided constants for the (emulated) type.
		const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();   // Number of elements of the underlying vector type.
		ASSERT(numElements <= 8 && numConstants <= numElements);
		llvm::Constant *constantVector[8];

		for(int i = 0; i < numElements; i++)
		{
			constantVector[i] = llvm::ConstantFP::get(T(type)->getContainedType(0), constants[i % numConstants]);
		}

		return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(constantVector, numElements)));
	}

	Type *Void::getType()
	{
		return T(llvm::Type::getVoidTy(jit->context));
	}

	Type *Bool::getType()
	{
		return T(llvm::Type::getInt1Ty(jit->context));
	}

	Type *Byte::getType()
	{
		return T(llvm::Type::getInt8Ty(jit->context));
	}

	Type *SByte::getType()
	{
		return T(llvm::Type::getInt8Ty(jit->context));
	}

	Type *Short::getType()
	{
		return T(llvm::Type::getInt16Ty(jit->context));
	}

	Type *UShort::getType()
	{
		return T(llvm::Type::getInt16Ty(jit->context));
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddusb(x, y);
#else
		return As<Byte8>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubusb(x, y);
#else
		return As<Byte8>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Int> SignMask(RValue<Byte8> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddsb(x, y);
#else
		return As<SByte8>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubsb(x, y);
#else
		return As<SByte8>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Int> SignMask(RValue<SByte8> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmovmskb(As<Byte8>(x));
#else
		return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
	}

	RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpgtb(x, y);
#else
		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
	}

	RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::psllw(lhs, rhs);
#else
		return As<Short4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psraw(lhs, rhs);
#else
		return As<Short4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaxsw(x, y);
#else
		return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
#endif
	}

	RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pminsw(x, y);
#else
		return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
#endif
	}

	RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddsw(x, y);
#else
		return As<Short4>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubsw(x, y);
#else
		return As<Short4>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhw(x, y);
#else
		return As<Short4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
#endif
	}

	RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaddwd(x, y);
#else
		return As<Int2>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
	}

	RValue<SByte8> PackSigned(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		auto result = x86::packsswb(x, y);
#else
		auto result = V(lowerPack(V(x.value), V(y.value), true));
#endif
		return As<SByte8>(Swizzle(As<Int4>(result), 0x88));
	}

	RValue<Byte8> PackUnsigned(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		auto result = x86::packuswb(x, y);
#else
		auto result = V(lowerPack(V(x.value), V(y.value), false));
#endif
		return As<Byte8>(Swizzle(As<Int4>(result), 0x88));
	}

	RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pcmpgtw(x, y);
#else
		return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
#endif
	}

	RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
#else
		return As<UShort4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

		return x86::psrlw(lhs, rhs);
#else
		return As<UShort4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UShort4>(Max(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
	}

	RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UShort4>(Min(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
	}

	RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::paddusw(x, y);
#else
		return As<UShort4>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psubusw(x, y);
#else
		return As<UShort4>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
	}

	RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmulhuw(x, y);
#else
		return As<UShort4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
#endif
	}

	RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psllw(lhs, rhs);
#else
		return As<Short8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psraw(lhs, rhs);
#else
		return As<Short8>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pmaddwd(x, y);
#else
		return As<Int4>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
	}

	RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return As<UShort8>(x86::psllw(As<Short8>(lhs), rhs));
#else
		return As<UShort8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrlw(lhs, rhs);   // FIXME: Fallback required
#else
		return As<UShort8>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
	}

	RValue<UShort8> Swizzle(RValue<UShort8> x, char select0, char select1, char select2, char select3, char select4, char select5, char select6, char select7)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
		RValue<Int> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator++(Int &val)   // Pre-increment
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> operator--(Int &val, int)   // Post-decrement
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		RValue<Int> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const Int &operator--(Int &val)   // Pre-decrement
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<Int> RoundInt(RValue<Float> cast)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::cvtss2si(cast);
#else
		return RValue<Int>(V(lowerRoundInt(V(cast.value), T(Int::getType()))));
#endif
	}

	Type *Int::getType()
	{
		return T(llvm::Type::getInt32Ty(jit->context));
	}

	Type *Long::getType()
	{
		return T(llvm::Type::getInt64Ty(jit->context));
	}

	UInt::UInt(RValue<Float> cast)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
		RValue<UInt> res = val;

		Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator++(UInt &val)   // Pre-increment
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return val;
	}

	RValue<UInt> operator--(UInt &val, int)   // Post-decrement
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		RValue<UInt> res = val;

		Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
		val.storeValue(inc);

		return res;
	}

	const UInt &operator--(UInt &val)   // Pre-decrement
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		return T(llvm::Type::getInt32Ty(jit->context));
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

		return x86::pslld(lhs, rhs);
#else
		return As<Int2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

		return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
#else
		return As<UInt2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::pslld(lhs, rhs);
#else
		return As<Int4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrad(lhs, rhs);
#else
		return As<Int4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
	}

	RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::cvtps2dq(cast);
#else
		return As<Int4>(V(lowerRoundInt(V(cast.value), T(Int4::getType()))));
#endif
	}

	RValue<Int4> MulHigh(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
		return As<Int4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
	}

	RValue<UInt4> MulHigh(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
		return As<UInt4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
	}

	RValue<Short8> PackSigned(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::packssdw(x, y);
#else
		return As<Short8>(V(lowerPack(V(x.value), V(y.value), true)));
#endif
	}

	RValue<UShort8> PackUnsigned(RValue<Int4> x, RValue<Int4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::packusdw(x, y);
#else
		return As<UShort8>(V(lowerPack(V(x.value), V(y.value), false)));
#endif
	}

	RValue<Int> SignMask(RValue<Int4> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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

	UInt4::UInt4(RValue<UInt> rhs) : XYZW(this)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return As<UInt4>(x86::pslld(As<Int4>(lhs), rhs));
#else
		return As<UInt4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
	}

	RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::psrld(lhs, rhs);
#else
		return As<UInt4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
	}

	RValue<UInt4> CmpEQ(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULT(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGE(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> CmpNLE(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGT(x.value, y.value), Int4::getType()));
	}

	RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		return T(llvm::Type::getInt16Ty(jit->context));
	}

	RValue<Float> Rcp_pp(RValue<Float> x, bool exactAtPow2)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::rsqrtss(x);
#else
		return As<Float>(V(lowerRSQRT(V(x.value))));
#endif
	}

	RValue<Float> Sqrt(RValue<Float> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::sqrtss(x);
#else
		return As<Float>(V(lowerSQRT(V(x.value))));
#endif
	}

	RValue<Float> Round(RValue<Float> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		return T(llvm::Type::getFloatTy(jit->context));
	}

	Type *Float2::getType()
	{
		return T(Type_v2f32);
	}

	RValue<Float> Exp2(RValue<Float> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp2, { T(Float::getType()) } );
		return RValue<Float>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float> Log2(RValue<Float> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log2, { T(Float::getType()) } );
		return RValue<Float>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	Float4::Float4(RValue<Float> rhs) : XYZW(this)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		Value *vector = loadValue();
		Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

		int swizzle[4] = {0, 0, 0, 0};
		Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

		storeValue(replicate);
	}

	RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::maxps(x, y);
#else
		return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OGT)));
#endif
	}

	RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::minps(x, y);
#else
		return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OLT)));
#endif
	}

	RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::rsqrtps(x);
#else
		return As<Float4>(V(lowerRSQRT(V(x.value))));
#endif
	}

	RValue<Float4> Sqrt(RValue<Float4> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::sqrtps(x);
#else
		return As<Float4>(V(lowerSQRT(V(x.value))));
#endif
	}

	RValue<Int> SignMask(RValue<Float4> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
		return x86::movmskps(x);
#else
		return As<Int>(V(lowerFPSignMask(V(x.value), T(Int::getType()))));
#endif
	}

	RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpeqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpneqps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpONE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpnltps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpnleps(x, y));
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUEQ(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpULT(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpULE(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNEQ(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUNE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNLT(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpUNLE(RValue<Float4> x, RValue<Float4> y)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGT(x.value, y.value), Int4::getType()));
	}

	RValue<Float4> Round(RValue<Float4> x)
	{
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		RR_DEBUG_INFO_UPDATE_LOC();
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
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sin, { V(v.value)->getType() } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float4> Cos(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cos, { V(v.value)->getType() } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float4> Tan(RValue<Float4> v)
	{
		return Sin(v) / Cos(v);
	}

	static RValue<Float4> TransformFloat4PerElement(RValue<Float4> v, const char* name)
	{
		auto funcTy = ::llvm::FunctionType::get(T(Float::getType()), ::llvm::ArrayRef<llvm::Type*>(T(Float::getType())), false);
		auto func = jit->module->getOrInsertFunction(name, funcTy);
		llvm::Value *out = ::llvm::UndefValue::get(T(Float4::getType()));
		for (uint64_t i = 0; i < 4; i++)
		{
			auto el = jit->builder->CreateCall(func, V(Nucleus::createExtractElement(v.value, Float::getType(), i)));
			out = V(Nucleus::createInsertElement(V(out), V(el), i));
		}
		return RValue<Float4>(V(out));
	}

	RValue<Float4> Asin(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "asinf");
	}

	RValue<Float4> Acos(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "acosf");
	}

	RValue<Float4> Atan(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "atanf");
	}

	RValue<Float4> Sinh(RValue<Float4> v)
	{
		return Float4(0.5f) * (Exp(v) - Exp(-v));
	}

	RValue<Float4> Cosh(RValue<Float4> v)
	{
		return Float4(0.5f) * (Exp(v) + Exp(-v));
	}

	RValue<Float4> Tanh(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "tanhf");
	}

	RValue<Float4> Asinh(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "asinhf");
	}

	RValue<Float4> Acosh(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "acoshf");
	}

	RValue<Float4> Atanh(RValue<Float4> v)
	{
		return TransformFloat4PerElement(v, "atanhf");
	}

	RValue<Float4> Atan2(RValue<Float4> x, RValue<Float4> y)
	{
		::llvm::SmallVector<::llvm::Type*, 2> paramTys;
		paramTys.push_back(T(Float::getType()));
		paramTys.push_back(T(Float::getType()));
		auto funcTy = ::llvm::FunctionType::get(T(Float::getType()), paramTys, false);
		auto func = jit->module->getOrInsertFunction("atan2f", funcTy);
		llvm::Value *out = ::llvm::UndefValue::get(T(Float4::getType()));
		for (uint64_t i = 0; i < 4; i++)
		{
			auto el = jit->builder->CreateCall2(func, ARGS(
					V(Nucleus::createExtractElement(x.value, Float::getType(), i)),
					V(Nucleus::createExtractElement(y.value, Float::getType(), i))
				));
			out = V(Nucleus::createInsertElement(V(out), V(el), i));
		}
		return RValue<Float4>(V(out));
	}

	RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::pow, { T(Float4::getType()) });
		return RValue<Float4>(V(jit->builder->CreateCall2(func, ARGS(V(x.value), V(y.value)))));
	}

	RValue<Float4> Exp(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp, { T(Float4::getType()) } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float4> Log(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log, { T(Float4::getType()) } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float4> Exp2(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp2, { T(Float4::getType()) } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<Float4> Log2(RValue<Float4> v)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log2, { T(Float4::getType()) } );
		return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
	}

	RValue<UInt> Ctlz(RValue<UInt> v, bool isZeroUndef)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::ctlz, { T(UInt::getType()) } );
		return RValue<UInt>(V(jit->builder->CreateCall2(func, ARGS(
			V(v.value),
			isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)
		))));
	}

	RValue<UInt4> Ctlz(RValue<UInt4> v, bool isZeroUndef)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::ctlz, { T(UInt4::getType()) } );
		return RValue<UInt4>(V(jit->builder->CreateCall2(func, ARGS(
			V(v.value),
			isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)
		))));
	}

	RValue<UInt> Cttz(RValue<UInt> v, bool isZeroUndef)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cttz, { T(UInt::getType()) } );
		return RValue<UInt>(V(jit->builder->CreateCall2(func, ARGS(
			V(v.value),
			isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)
		))));
	}

	RValue<UInt4> Cttz(RValue<UInt4> v, bool isZeroUndef)
	{
		auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cttz, { T(UInt4::getType()) } );
		return RValue<UInt4>(V(jit->builder->CreateCall2(func, ARGS(
			V(v.value),
			isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)
		))));
	}

	Type *Float4::getType()
	{
		return T(llvm::VectorType::get(T(Float::getType()), 4));
	}

	RValue<Long> Ticks()
	{
		RR_DEBUG_INFO_UPDATE_LOC();
		llvm::Function *rdtsc = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::readcyclecounter);

		return RValue<Long>(V(jit->builder->CreateCall(rdtsc)));
	}

	RValue<Pointer<Byte>> ConstantPointer(void const * ptr)
	{
		// Note: this should work for 32-bit pointers as well because 'inttoptr'
		// is defined to truncate (and zero extend) if necessary.
		auto ptrAsInt = ::llvm::ConstantInt::get(::llvm::Type::getInt64Ty(jit->context), reinterpret_cast<uintptr_t>(ptr));
		return RValue<Pointer<Byte>>(V(jit->builder->CreateIntToPtr(ptrAsInt, T(Pointer<Byte>::getType()))));
	}

	RValue<Pointer<Byte>> ConstantData(void const * data, size_t size)
	{
		auto str = ::llvm::StringRef(reinterpret_cast<const char*>(data), size);
		auto ptr = jit->builder->CreateGlobalStringPtr(str);
		return RValue<Pointer<Byte>>(V(ptr));
	}

	Value* Call(RValue<Pointer<Byte>> fptr, Type* retTy, std::initializer_list<Value*> args, std::initializer_list<Type*> argTys)
	{
		::llvm::SmallVector<::llvm::Type*, 8> paramTys;
		for (auto ty : argTys) { paramTys.push_back(T(ty)); }
		auto funcTy = ::llvm::FunctionType::get(T(retTy), paramTys, false);

		auto funcPtrTy = funcTy->getPointerTo();
		auto funcPtr = jit->builder->CreatePointerCast(V(fptr.value), funcPtrTy);

		::llvm::SmallVector<::llvm::Value*, 8> arguments;
		for (auto arg : args) { arguments.push_back(V(arg)); }
		return V(jit->builder->CreateCall(funcPtr, arguments));
	}

	void Breakpoint()
	{
		llvm::Function *debugtrap = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::debugtrap);

		jit->builder->CreateCall(debugtrap);
	}
}

namespace rr
{
#if defined(__i386__) || defined(__x86_64__)
	namespace x86
	{
		RValue<Int> cvtss2si(RValue<Float> val)
		{
			llvm::Function *cvtss2si = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_cvtss2si);

			Float4 vector;
			vector.x = val;

			return RValue<Int>(V(jit->builder->CreateCall(cvtss2si, ARGS(V(RValue<Float4>(vector).value)))));
		}

		RValue<Int4> cvtps2dq(RValue<Float4> val)
		{
			llvm::Function *cvtps2dq = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_cvtps2dq);

			return RValue<Int4>(V(jit->builder->CreateCall(cvtps2dq, ARGS(V(val.value)))));
		}

		RValue<Float> rcpss(RValue<Float> val)
		{
			llvm::Function *rcpss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rcp_ss);

			Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall(rcpss, ARGS(V(vector)))), Float::getType(), 0));
		}

		RValue<Float> sqrtss(RValue<Float> val)
		{
			llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sqrt, {V(val.value)->getType()});
			return RValue<Float>(V(jit->builder->CreateCall(sqrt, ARGS(V(val.value)))));
		}

		RValue<Float> rsqrtss(RValue<Float> val)
		{
			llvm::Function *rsqrtss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rsqrt_ss);

			Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall(rsqrtss, ARGS(V(vector)))), Float::getType(), 0));
		}

		RValue<Float4> rcpps(RValue<Float4> val)
		{
			llvm::Function *rcpps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rcp_ps);

			return RValue<Float4>(V(jit->builder->CreateCall(rcpps, ARGS(V(val.value)))));
		}

		RValue<Float4> sqrtps(RValue<Float4> val)
		{
			llvm::Function *sqrtps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sqrt, {V(val.value)->getType()});

			return RValue<Float4>(V(jit->builder->CreateCall(sqrtps, ARGS(V(val.value)))));
		}

		RValue<Float4> rsqrtps(RValue<Float4> val)
		{
			llvm::Function *rsqrtps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rsqrt_ps);

			return RValue<Float4>(V(jit->builder->CreateCall(rsqrtps, ARGS(V(val.value)))));
		}

		RValue<Float4> maxps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *maxps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_max_ps);

			return RValue<Float4>(V(jit->builder->CreateCall2(maxps, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Float4> minps(RValue<Float4> x, RValue<Float4> y)
		{
			llvm::Function *minps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_min_ps);

			return RValue<Float4>(V(jit->builder->CreateCall2(minps, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Float> roundss(RValue<Float> val, unsigned char imm)
		{
			llvm::Function *roundss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_round_ss);

			Value *undef = V(llvm::UndefValue::get(T(Float4::getType())));
			Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

			return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall3(roundss, ARGS(V(undef), V(vector), V(Nucleus::createConstantInt(imm))))), Float::getType(), 0));
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
			llvm::Function *roundps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_round_ps);

			return RValue<Float4>(V(jit->builder->CreateCall2(roundps, ARGS(V(val.value), V(Nucleus::createConstantInt(imm))))));
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
			return RValue<Int4>(V(lowerPABS(V(x.value))));
		}

		RValue<Short4> paddsw(RValue<Short4> x, RValue<Short4> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<Short4>(V(lowerPSADDSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *paddsw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_padds_w);

				return As<Short4>(V(jit->builder->CreateCall2(paddsw, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<Short4> psubsw(RValue<Short4> x, RValue<Short4> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<Short4>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *psubsw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubs_w);

				return As<Short4>(V(jit->builder->CreateCall2(psubsw, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<UShort4> paddusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<UShort4>(V(lowerPUADDSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *paddusw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_paddus_w);

				return As<UShort4>(V(jit->builder->CreateCall2(paddusw, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<UShort4> psubusw(RValue<UShort4> x, RValue<UShort4> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<UShort4>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *psubusw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubus_w);

				return As<UShort4>(V(jit->builder->CreateCall2(psubusw, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<SByte8> paddsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<SByte8>(V(lowerPSADDSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *paddsb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_padds_b);

				return As<SByte8>(V(jit->builder->CreateCall2(paddsb, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<SByte8> psubsb(RValue<SByte8> x, RValue<SByte8> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<SByte8>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *psubsb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubs_b);

				return As<SByte8>(V(jit->builder->CreateCall2(psubsb, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<Byte8> paddusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<Byte8>(V(lowerPUADDSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *paddusb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_paddus_b);

				return As<Byte8>(V(jit->builder->CreateCall2(paddusb, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<Byte8> psubusb(RValue<Byte8> x, RValue<Byte8> y)
		{
			#if LLVM_VERSION_MAJOR >= 8
				return As<Byte8>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
			#else
				llvm::Function *psubusb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubus_b);

				return As<Byte8>(V(jit->builder->CreateCall2(psubusb, ARGS(V(x.value), V(y.value)))));
			#endif
		}

		RValue<UShort4> pavgw(RValue<UShort4> x, RValue<UShort4> y)
		{
			return As<UShort4>(V(lowerPAVG(V(x.value), V(y.value))));
		}

		RValue<Short4> pmaxsw(RValue<Short4> x, RValue<Short4> y)
		{
			return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
		}

		RValue<Short4> pminsw(RValue<Short4> x, RValue<Short4> y)
		{
			return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
		}

		RValue<Short4> pcmpgtw(RValue<Short4> x, RValue<Short4> y)
		{
			return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
		}

		RValue<Short4> pcmpeqw(RValue<Short4> x, RValue<Short4> y)
		{
			return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Short4::getType()))));
		}

		RValue<Byte8> pcmpgtb(RValue<SByte8> x, RValue<SByte8> y)
		{
			return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
		}

		RValue<Byte8> pcmpeqb(RValue<Byte8> x, RValue<Byte8> y)
		{
			return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
		}

		RValue<Short4> packssdw(RValue<Int2> x, RValue<Int2> y)
		{
			llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packssdw_128);

			return As<Short4>(V(jit->builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Short8> packssdw(RValue<Int4> x, RValue<Int4> y)
		{
			llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packssdw_128);

			return RValue<Short8>(V(jit->builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<SByte8> packsswb(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *packsswb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packsswb_128);

			return As<SByte8>(V(jit->builder->CreateCall2(packsswb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Byte8> packuswb(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *packuswb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packuswb_128);

			return As<Byte8>(V(jit->builder->CreateCall2(packuswb, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort8> packusdw(RValue<Int4> x, RValue<Int4> y)
		{
			if(CPUID::supportsSSE4_1())
			{
				llvm::Function *packusdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_packusdw);

				return RValue<UShort8>(V(jit->builder->CreateCall2(packusdw, ARGS(V(x.value), V(y.value)))));
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
			llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_w);

			return As<UShort4>(V(jit->builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UShort8> psrlw(RValue<UShort8> x, unsigned char y)
		{
			llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_w);

			return RValue<UShort8>(V(jit->builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short4> psraw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psraw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_w);

			return As<Short4>(V(jit->builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psraw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psraw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_w);

			return RValue<Short8>(V(jit->builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short4> psllw(RValue<Short4> x, unsigned char y)
		{
			llvm::Function *psllw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_w);

			return As<Short4>(V(jit->builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Short8> psllw(RValue<Short8> x, unsigned char y)
		{
			llvm::Function *psllw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_w);

			return RValue<Short8>(V(jit->builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int2> pslld(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *pslld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_d);

			return As<Int2>(V(jit->builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> pslld(RValue<Int4> x, unsigned char y)
		{
			llvm::Function *pslld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_d);

			return RValue<Int4>(V(jit->builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int2> psrad(RValue<Int2> x, unsigned char y)
		{
			llvm::Function *psrad = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_d);

			return As<Int2>(V(jit->builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> psrad(RValue<Int4> x, unsigned char y)
		{
			llvm::Function *psrad = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_d);

			return RValue<Int4>(V(jit->builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UInt2> psrld(RValue<UInt2> x, unsigned char y)
		{
			llvm::Function *psrld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_d);

			return As<UInt2>(V(jit->builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<UInt4> psrld(RValue<UInt4> x, unsigned char y)
		{
			llvm::Function *psrld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_d);

			return RValue<UInt4>(V(jit->builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
		}

		RValue<Int4> pmaxsd(RValue<Int4> x, RValue<Int4> y)
		{
			return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
		}

		RValue<Int4> pminsd(RValue<Int4> x, RValue<Int4> y)
		{
			return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
		}

		RValue<UInt4> pmaxud(RValue<UInt4> x, RValue<UInt4> y)
		{
			return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_UGT)));
		}

		RValue<UInt4> pminud(RValue<UInt4> x, RValue<UInt4> y)
		{
			return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_ULT)));
		}

		RValue<Short4> pmulhw(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulh_w);

			return As<Short4>(V(jit->builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort4> pmulhuw(RValue<UShort4> x, RValue<UShort4> y)
		{
			llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulhu_w);

			return As<UShort4>(V(jit->builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int2> pmaddwd(RValue<Short4> x, RValue<Short4> y)
		{
			llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmadd_wd);

			return As<Int2>(V(jit->builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Short8> pmulhw(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulh_w);

			return RValue<Short8>(V(jit->builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<UShort8> pmulhuw(RValue<UShort8> x, RValue<UShort8> y)
		{
			llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulhu_w);

			return RValue<UShort8>(V(jit->builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int4> pmaddwd(RValue<Short8> x, RValue<Short8> y)
		{
			llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmadd_wd);

			return RValue<Int4>(V(jit->builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
		}

		RValue<Int> movmskps(RValue<Float4> x)
		{
			llvm::Function *movmskps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_movmsk_ps);

			return RValue<Int>(V(jit->builder->CreateCall(movmskps, ARGS(V(x.value)))));
		}

		RValue<Int> pmovmskb(RValue<Byte8> x)
		{
			llvm::Function *pmovmskb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmovmskb_128);

			return RValue<Int>(V(jit->builder->CreateCall(pmovmskb, ARGS(V(x.value))))) & 0xFF;
		}

		RValue<Int4> pmovzxbd(RValue<Byte16> x)
		{
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
		}

		RValue<Int4> pmovsxbd(RValue<SByte16> x)
		{
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
		}

		RValue<Int4> pmovzxwd(RValue<UShort8> x)
		{
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
		}

		RValue<Int4> pmovsxwd(RValue<Short8> x)
		{
			return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
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
			auto el = V(jit->builder->CreateExtractElement(V(vec), i));
			elements.push_back(el);
		}
		return elements;
	}

	// toInt returns all the integer values in vals extended to a native width
	// integer.
	static std::vector<Value*> toInt(const std::vector<Value*>& vals, bool isSigned)
	{
		auto intTy = ::llvm::Type::getIntNTy(jit->context, sizeof(int) * 8); // Natural integer width.
		std::vector<Value*> elements;
		elements.reserve(vals.size());
		for (auto v : vals)
		{
			if (isSigned)
			{
				elements.push_back(V(jit->builder->CreateSExt(V(v), intTy)));
			}
			else
			{
				elements.push_back(V(jit->builder->CreateZExt(V(v), intTy)));
			}
		}
		return elements;
	}

	// toDouble returns all the float values in vals extended to doubles.
	static std::vector<Value*> toDouble(const std::vector<Value*>& vals)
	{
		auto doubleTy = ::llvm::Type::getDoubleTy(jit->context);
		std::vector<Value*> elements;
		elements.reserve(vals.size());
		for (auto v : vals)
		{
			elements.push_back(V(jit->builder->CreateFPExt(V(v), doubleTy)));
		}
		return elements;
	}

	std::vector<Value*> PrintValue::Ty<Byte>::val(const RValue<Byte>& v) { return toInt({v.value}, false); }
	std::vector<Value*> PrintValue::Ty<Byte4>::val(const RValue<Byte4>& v) { return toInt(extractAll(v.value, 4), false); }
	std::vector<Value*> PrintValue::Ty<Int>::val(const RValue<Int>& v) { return toInt({v.value}, true); }
	std::vector<Value*> PrintValue::Ty<Int2>::val(const RValue<Int2>& v) { return toInt(extractAll(v.value, 2), true); }
	std::vector<Value*> PrintValue::Ty<Int4>::val(const RValue<Int4>& v) { return toInt(extractAll(v.value, 4), true); }
	std::vector<Value*> PrintValue::Ty<UInt>::val(const RValue<UInt>& v) { return toInt({v.value}, false); }
	std::vector<Value*> PrintValue::Ty<UInt2>::val(const RValue<UInt2>& v) { return toInt(extractAll(v.value, 2), false); }
	std::vector<Value*> PrintValue::Ty<UInt4>::val(const RValue<UInt4>& v) { return toInt(extractAll(v.value, 4), false); }
	std::vector<Value*> PrintValue::Ty<Short>::val(const RValue<Short>& v) { return toInt({v.value}, true); }
	std::vector<Value*> PrintValue::Ty<Short4>::val(const RValue<Short4>& v) { return toInt(extractAll(v.value, 4), true); }
	std::vector<Value*> PrintValue::Ty<UShort>::val(const RValue<UShort>& v) { return toInt({v.value}, false); }
	std::vector<Value*> PrintValue::Ty<UShort4>::val(const RValue<UShort4>& v) { return toInt(extractAll(v.value, 4), false); }
	std::vector<Value*> PrintValue::Ty<Float>::val(const RValue<Float>& v) { return toDouble({v.value}); }
	std::vector<Value*> PrintValue::Ty<Float4>::val(const RValue<Float4>& v) { return toDouble(extractAll(v.value, 4)); }
	std::vector<Value*> PrintValue::Ty<const char*>::val(const char* v) { return {V(jit->builder->CreateGlobalStringPtr(v))}; }

	void Printv(const char* function, const char* file, int line, const char* fmt, std::initializer_list<PrintValue> args)
	{
		// LLVM types used below.
		auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
		auto intTy = ::llvm::Type::getIntNTy(jit->context, sizeof(int) * 8); // Natural integer width.
		auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
		auto funcTy = ::llvm::FunctionType::get(i32Ty, {i8PtrTy}, true);

		auto func = jit->module->getOrInsertFunction("printf", funcTy);

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
		vals.push_back(jit->builder->CreateGlobalStringPtr(str));

		// Add optional file, line and function info if provided.
		if (file != nullptr)
		{
			vals.push_back(jit->builder->CreateGlobalStringPtr(file));
			if (line > 0)
			{
				vals.push_back(::llvm::ConstantInt::get(intTy, line));
			}
		}
		if (function != nullptr)
		{
			vals.push_back(jit->builder->CreateGlobalStringPtr(function));
		}

		// Add all format arguments.
		for (const PrintValue& arg : args)
		{
			for (auto val : arg.values)
			{
				vals.push_back(V(val));
			}
		}

		jit->builder->CreateCall(func, vals);
	}
#endif // ENABLE_RR_PRINT

	void Nop()
	{
		auto voidTy = ::llvm::Type::getVoidTy(jit->context);
		auto funcTy = ::llvm::FunctionType::get(voidTy, {}, false);
		auto func = jit->module->getOrInsertFunction("nop", funcTy);
		jit->builder->CreateCall(func);
	}

	void EmitDebugLocation()
	{
#ifdef ENABLE_RR_DEBUG_INFO
		if (jit->debugInfo != nullptr)
		{
			jit->debugInfo->EmitLocation();
		}
#endif // ENABLE_RR_DEBUG_INFO
	}

	void EmitDebugVariable(Value* value)
	{
#ifdef ENABLE_RR_DEBUG_INFO
		if (jit->debugInfo != nullptr)
		{
			jit->debugInfo->EmitVariable(value);
		}
#endif // ENABLE_RR_DEBUG_INFO
	}

	void FlushDebug()
	{
#ifdef ENABLE_RR_DEBUG_INFO
		if (jit->debugInfo != nullptr)
		{
			jit->debugInfo->Flush();
		}
#endif // ENABLE_RR_DEBUG_INFO
	}

} // namespace rr

// ------------------------------  Coroutines ------------------------------

namespace {
	// Magic values retuned by llvm.coro.suspend.
	// See: https://llvm.org/docs/Coroutines.html#llvm-coro-suspend-intrinsic
	enum SuspendAction
	{
		SuspendActionSuspend = -1,
		SuspendActionResume = 0,
		SuspendActionDestroy = 1
	};


void promoteFunctionToCoroutine()
{
	ASSERT(jit->coroutine.id == nullptr);

	// Types
	auto voidTy = ::llvm::Type::getVoidTy(jit->context);
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
	auto promiseTy = jit->coroutine.yieldType;
	auto promisePtrTy = promiseTy->getPointerTo();

	// LLVM intrinsics
	auto coro_id = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_id);
	auto coro_size = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_size, {i32Ty});
	auto coro_begin = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_begin);
	auto coro_resume = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_resume);
	auto coro_end = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_end);
	auto coro_free = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_free);
	auto coro_destroy = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_destroy);
	auto coro_promise = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_promise);
	auto coro_done = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_done);
	auto coro_suspend = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_suspend);

	auto allocFrameTy = ::llvm::FunctionType::get(i8PtrTy, {i32Ty}, false);
	auto allocFrame = jit->module->getOrInsertFunction("coroutine_alloc_frame", allocFrameTy);
	auto freeFrameTy = ::llvm::FunctionType::get(voidTy, {i8PtrTy}, false);
	auto freeFrame = jit->module->getOrInsertFunction("coroutine_free_frame", freeFrameTy);

	auto oldInsertionPoint = jit->builder->saveIP();

	// Build the coroutine_await() function:
	//
	//    bool coroutine_await(CoroutineHandle* handle, YieldType* out)
	//    {
	//        if (llvm.coro.done(handle))
	//        {
	//            return false;
	//        }
	//        else
	//        {
	//            *value = (T*)llvm.coro.promise(handle);
	//            llvm.coro.resume(handle);
	//            return true;
	//        }
	//    }
	//
	{
		auto args = jit->coroutine.await->arg_begin();
		auto handle = args++;
		auto outPtr = args++;
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "co_await", jit->coroutine.await));
		auto doneBlock = llvm::BasicBlock::Create(jit->context, "done", jit->coroutine.await);
		auto resumeBlock = llvm::BasicBlock::Create(jit->context, "resume", jit->coroutine.await);

		auto done = jit->builder->CreateCall(coro_done, {handle}, "done");
		jit->builder->CreateCondBr(done, doneBlock, resumeBlock);

		jit->builder->SetInsertPoint(doneBlock);
		jit->builder->CreateRet(::llvm::ConstantInt::getFalse(i1Ty));

		jit->builder->SetInsertPoint(resumeBlock);
		auto promiseAlignment = ::llvm::ConstantInt::get(i32Ty, 4); // TODO: Get correct alignment.
		auto promisePtr = jit->builder->CreateCall(coro_promise, {handle, promiseAlignment, ::llvm::ConstantInt::get(i1Ty, 0)});
		auto promise = jit->builder->CreateLoad(jit->builder->CreatePointerCast(promisePtr, promisePtrTy));
		jit->builder->CreateStore(promise, outPtr);
		jit->builder->CreateCall(coro_resume, {handle});
		jit->builder->CreateRet(::llvm::ConstantInt::getTrue(i1Ty));
	}

	// Build the coroutine_destroy() function:
	//
	//    void coroutine_destroy(CoroutineHandle* handle)
	//    {
	//        llvm.coro.destroy(handle);
	//    }
	//
	{
		auto handle = jit->coroutine.destroy->arg_begin();
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.destroy));
		jit->builder->CreateCall(coro_destroy, {handle});
		jit->builder->CreateRetVoid();
	}

	// Begin building the main coroutine_begin() function.
	//
	//    CoroutineHandle* coroutine_begin(<Arguments>)
	//    {
	//        YieldType promise;
	//        auto id = llvm.coro.id(0, &promise, nullptr, nullptr);
	//        void* frame = coroutine_alloc_frame(llvm.coro.size.i32());
	//        CoroutineHandle *handle = llvm.coro.begin(id, frame);
	//
	//        ... <REACTOR CODE> ...
	//
	//    end:
	//        SuspendAction action = llvm.coro.suspend(none, true /* final */);  // <-- RESUME POINT
	//        switch (action)
	//        {
	//        case SuspendActionResume:
	//            UNREACHABLE(); // Illegal to resume after final suspend.
	//        case SuspendActionDestroy:
	//            goto destroy;
	//        default: // (SuspendActionSuspend)
	//            goto suspend;
	//        }
	//
	//    destroy:
	//        coroutine_free_frame(llvm.coro.free(id, handle));
	//        goto suspend;
	//
	//    suspend:
	//        llvm.coro.end(handle, false);
	//        return handle;
	//    }
	//

#ifdef ENABLE_RR_DEBUG_INFO
	jit->debugInfo = std::unique_ptr<rr::DebugInfo>(new rr::DebugInfo(jit->builder.get(), &jit->context, jit->module.get(), jit->function));
#endif // ENABLE_RR_DEBUG_INFO

	jit->coroutine.suspendBlock = llvm::BasicBlock::Create(jit->context, "suspend", jit->function);
	jit->coroutine.endBlock = llvm::BasicBlock::Create(jit->context, "end", jit->function);
	jit->coroutine.destroyBlock = llvm::BasicBlock::Create(jit->context, "destroy", jit->function);

	jit->builder->SetInsertPoint(jit->coroutine.entryBlock, jit->coroutine.entryBlock->begin());
	jit->coroutine.promise = jit->builder->CreateAlloca(promiseTy, nullptr, "promise");
	jit->coroutine.id = jit->builder->CreateCall(coro_id, {
		::llvm::ConstantInt::get(i32Ty, 0),
		jit->builder->CreatePointerCast(jit->coroutine.promise, i8PtrTy),
		::llvm::ConstantPointerNull::get(i8PtrTy),
		::llvm::ConstantPointerNull::get(i8PtrTy),
	});
	auto size = jit->builder->CreateCall(coro_size, {});
	auto frame = jit->builder->CreateCall(allocFrame, {size});
	jit->coroutine.handle = jit->builder->CreateCall(coro_begin, {jit->coroutine.id, frame});

	// Build the suspend block
	jit->builder->SetInsertPoint(jit->coroutine.suspendBlock);
	jit->builder->CreateCall(coro_end, {jit->coroutine.handle, ::llvm::ConstantInt::get(i1Ty, 0)});
	jit->builder->CreateRet(jit->coroutine.handle);

	// Build the end block
	jit->builder->SetInsertPoint(jit->coroutine.endBlock);
	auto action = jit->builder->CreateCall(coro_suspend, {
		::llvm::ConstantTokenNone::get(jit->context),
		::llvm::ConstantInt::get(i1Ty, 1), // final: true
	});
	auto switch_ = jit->builder->CreateSwitch(action, jit->coroutine.suspendBlock, 3);
	// switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionResume), trapBlock); // TODO: Trap attempting to resume after final suspend
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionDestroy), jit->coroutine.destroyBlock);

	// Build the destroy block
	jit->builder->SetInsertPoint(jit->coroutine.destroyBlock);
	auto memory = jit->builder->CreateCall(coro_free, {jit->coroutine.id, jit->coroutine.handle});
	jit->builder->CreateCall(freeFrame, {memory});
	jit->builder->CreateBr(jit->coroutine.suspendBlock);

	// Switch back to original insert point to continue building the coroutine.
	jit->builder->restoreIP(oldInsertionPoint);
}

} // anonymous namespace

namespace rr {

void Nucleus::createCoroutine(Type *YieldType, std::vector<Type*> &Params)
{
	// Coroutines are initially created as a regular function.
	// Upon the first call to Yield(), the function is promoted to a true
	// coroutine.
	auto voidTy = ::llvm::Type::getVoidTy(jit->context);
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
	auto handleTy = i8PtrTy;
	auto boolTy = i1Ty;
	auto promiseTy = T(YieldType);
	auto promisePtrTy = promiseTy->getPointerTo();

	jit->function = rr::createFunction("coroutine_begin", handleTy, T(Params));
	jit->coroutine.await = rr::createFunction("coroutine_await", boolTy, {handleTy, promisePtrTy});
	jit->coroutine.destroy = rr::createFunction("coroutine_destroy", voidTy, {handleTy});
	jit->coroutine.yieldType = promiseTy;
	jit->coroutine.entryBlock = llvm::BasicBlock::Create(jit->context, "function", jit->function);

	jit->builder->SetInsertPoint(jit->coroutine.entryBlock);
}

void Nucleus::yield(Value* val)
{
	if (jit->coroutine.id == nullptr)
	{
		// First call to yield().
		// Promote the function to a full coroutine.
		promoteFunctionToCoroutine();
		ASSERT(jit->coroutine.id != nullptr);
	}

	//      promise = val;
	//
	//      auto action = llvm.coro.suspend(none, false /* final */); // <-- RESUME POINT
	//      switch (action)
	//      {
	//      case SuspendActionResume:
	//          goto resume;
	//      case SuspendActionDestroy:
	//          goto destroy;
	//      default: // (SuspendActionSuspend)
	//          goto suspend;
	//      }
	//  resume:
	//

	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	// Types
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);

	// Intrinsics
	auto coro_suspend = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_suspend);

	// Create a block to resume execution.
	auto resumeBlock = llvm::BasicBlock::Create(jit->context, "resume", jit->function);

	// Store the promise (yield value)
	jit->builder->CreateStore(V(val), jit->coroutine.promise);
	auto action = jit->builder->CreateCall(coro_suspend, {
		::llvm::ConstantTokenNone::get(jit->context),
		::llvm::ConstantInt::get(i1Ty, 0), // final: true
	});
	auto switch_ = jit->builder->CreateSwitch(action, jit->coroutine.suspendBlock, 3);
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionResume), resumeBlock);
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionDestroy), jit->coroutine.destroyBlock);

	// Continue building in the resume block.
	jit->builder->SetInsertPoint(resumeBlock);
}

std::shared_ptr<Routine> Nucleus::acquireCoroutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
{
	bool isCoroutine = jit->coroutine.id != nullptr;
	if (isCoroutine)
	{
		jit->builder->CreateBr(jit->coroutine.endBlock);
	}
	else
	{
		// Coroutine without a Yield acts as a regular function.
		// The 'coroutine_begin' function returns a nullptr for the coroutine
		// handle.
		jit->builder->CreateRet(llvm::Constant::getNullValue(jit->function->getReturnType()));
		// The 'coroutine_await' function always returns false (coroutine done).
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.await));
		jit->builder->CreateRet(llvm::Constant::getNullValue(jit->coroutine.await->getReturnType()));
		// The 'coroutine_destroy' does nothing, returns void.
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.destroy));
		jit->builder->CreateRetVoid();
	}

#ifdef ENABLE_RR_DEBUG_INFO
	if (jit->debugInfo != nullptr)
	{
		jit->debugInfo->Finalize();
	}
#endif // ENABLE_RR_DEBUG_INFO

	if(false)
	{
		std::error_code error;
		llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-unopt.txt", error);
		jit->module->print(file, 0);
	}

	if (isCoroutine)
	{
		// Run manadory coroutine transforms.
		llvm::legacy::PassManager pm;
		pm.add(llvm::createCoroEarlyPass());
		pm.add(llvm::createCoroSplitPass());
		pm.add(llvm::createCoroElidePass());
		pm.add(llvm::createBarrierNoopPass());
		pm.add(llvm::createCoroCleanupPass());
		pm.run(*jit->module);
	}

#if defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)
	{
		llvm::legacy::PassManager pm;
		pm.add(llvm::createVerifierPass());
		pm.run(*jit->module);
	}
#endif // defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)

	auto cfg = cfgEdit.apply(jit->config);
	jit->optimize(cfg);

	if(false)
	{
		std::error_code error;
		llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-opt.txt", error);
		jit->module->print(file, 0);
	}

	llvm::Function *funcs[Nucleus::CoroutineEntryCount];
	funcs[Nucleus::CoroutineEntryBegin] = jit->function;
	funcs[Nucleus::CoroutineEntryAwait] = jit->coroutine.await;
	funcs[Nucleus::CoroutineEntryDestroy] = jit->coroutine.destroy;
	auto routine = jit->acquireRoutine(funcs, Nucleus::CoroutineEntryCount, cfg);
	jit.reset();

	return routine;
}

} // namespace rr