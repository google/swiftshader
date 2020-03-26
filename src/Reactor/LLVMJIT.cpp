// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "LLVMReactor.hpp"

#include "Debug.hpp"
#include "ExecutableMemory.hpp"
#include "Routine.hpp"

// TODO(b/143539525): Eliminate when warning has been fixed.
#ifdef _MSC_VER
__pragma(warning(push))
    __pragma(warning(disable : 4146))  // unary minus operator applied to unsigned type, result still unsigned
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
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Coroutines.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#ifdef _MSC_VER
    __pragma(warning(pop))
#endif

#include <unordered_map>

#if defined(_WIN64)
        extern "C" void __chkstk();
#elif defined(_WIN32)
extern "C" void _chkstk();
#endif

#if __has_feature(memory_sanitizer)
#	include <sanitizer/msan_interface.h>
#endif

#ifdef __ARM_EABI__
extern "C" signed __aeabi_idivmod();
#endif

namespace {

// Cache provides a simple, thread-safe key-value store.
template<typename KEY, typename VALUE>
class Cache
{
public:
	Cache() = default;
	Cache(const Cache &other);
	VALUE getOrCreate(KEY key, std::function<VALUE()> create);

private:
	mutable std::mutex mutex;  // mutable required for copy constructor.
	std::unordered_map<KEY, VALUE> map;
};

template<typename KEY, typename VALUE>
Cache<KEY, VALUE>::Cache(const Cache &other)
{
	std::unique_lock<std::mutex> lock(other.mutex);
	map = other.map;
}

template<typename KEY, typename VALUE>
VALUE Cache<KEY, VALUE>::getOrCreate(KEY key, std::function<VALUE()> create)
{
	std::unique_lock<std::mutex> lock(mutex);
	auto it = map.find(key);
	if(it != map.end())
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

	static JITGlobals *get();

	const std::string mcpu;
	const std::vector<std::string> mattrs;
	const char *const march;
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
	JITGlobals(const JITGlobals &) = default;

	Cache<rr::Optimization::Level, TargetMachineSPtr> targetMachines;
};

JITGlobals *JITGlobals::get()
{
	static JITGlobals instance = create();
	return &instance;
}

JITGlobals::TargetMachineSPtr JITGlobals::getTargetMachine(rr::Optimization::Level optlevel)
{
#ifdef ENABLE_RR_DEBUG_INFO
	auto llvmOptLevel = toLLVM(rr::Optimization::Level::None);
#else   // ENABLE_RR_DEBUG_INFO
	auto llvmOptLevel = toLLVM(optlevel);
#endif  // ENABLE_RR_DEBUG_INFO

	return targetMachines.getOrCreate(optlevel, [&]() {
		return TargetMachineSPtr(llvm::EngineBuilder()
		                             .setOptLevel(llvmOptLevel)
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
	(void)ok;  // getHostCPUFeatures always returns false on other platforms
#endif

	std::vector<std::string> mattrs;
	for(auto &feature : features)
	{
		if(feature.second) { mattrs.push_back(feature.first().str()); }
	}

	const char *march = nullptr;
#if defined(__x86_64__)
	march = "x86-64";
#elif defined(__i386__)
	march = "x86";
#elif defined(__aarch64__)
	march = "arm64";
#elif defined(__arm__)
	march = "arm";
#elif defined(__mips__)
#	if defined(__mips64)
	march = "mips64el";
#	else
	march = "mipsel";
#	endif
#elif defined(__powerpc64__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	march = "ppc64le";
#else
#	error "unknown architecture"
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
	switch(level)
	{
		case rr::Optimization::Level::None: return ::llvm::CodeGenOpt::None;
		case rr::Optimization::Level::Less: return ::llvm::CodeGenOpt::Less;
		case rr::Optimization::Level::Default: return ::llvm::CodeGenOpt::Default;
		case rr::Optimization::Level::Aggressive: return ::llvm::CodeGenOpt::Aggressive;
		default: UNREACHABLE("Unknown Optimization Level %d", int(level));
	}
	return ::llvm::CodeGenOpt::Default;
}

JITGlobals::JITGlobals(const char *mcpu,
                       const std::vector<std::string> &mattrs,
                       const char *march,
                       const llvm::TargetOptions &targetOptions,
                       const llvm::DataLayout &dataLayout)
    : mcpu(mcpu)
    , mattrs(mattrs)
    , march(march)
    , targetOptions(targetOptions)
    , dataLayout(dataLayout)
{
}

class MemoryMapper final : public llvm::SectionMemoryManager::MemoryMapper
{
public:
	MemoryMapper() {}
	~MemoryMapper() final {}

	llvm::sys::MemoryBlock allocateMappedMemory(
	    llvm::SectionMemoryManager::AllocationPurpose purpose,
	    size_t numBytes, const llvm::sys::MemoryBlock *const nearBlock,
	    unsigned flags, std::error_code &errorCode) final
	{
		errorCode = std::error_code();

		// Round up numBytes to page size.
		size_t pageSize = rr::memoryPageSize();
		numBytes = (numBytes + pageSize - 1) & ~(pageSize - 1);

		bool need_exec =
		    purpose == llvm::SectionMemoryManager::AllocationPurpose::Code;
		void *addr = rr::allocateMemoryPages(
		    numBytes, flagsToPermissions(flags), need_exec);
		if(!addr)
			return llvm::sys::MemoryBlock();
		return llvm::sys::MemoryBlock(addr, numBytes);
	}

	std::error_code protectMappedMemory(const llvm::sys::MemoryBlock &block,
	                                    unsigned flags)
	{
		// Round down base address to align with a page boundary. This matches
		// DefaultMMapper behavior.
		void *addr = block.base();
#if LLVM_VERSION_MAJOR >= 9
		size_t size = block.allocatedSize();
#else
		size_t size = block.size();
#endif
		size_t pageSize = rr::memoryPageSize();
		addr = reinterpret_cast<void *>(
		    reinterpret_cast<uintptr_t>(addr) & ~(pageSize - 1));
		size += reinterpret_cast<uintptr_t>(block.base()) -
		        reinterpret_cast<uintptr_t>(addr);

		rr::protectMemoryPages(addr, size, flagsToPermissions(flags));
		return std::error_code();
	}

	std::error_code releaseMappedMemory(llvm::sys::MemoryBlock &block)
	{
#if LLVM_VERSION_MAJOR >= 9
		size_t size = block.allocatedSize();
#else
		size_t size = block.size();
#endif

		rr::deallocateMemoryPages(block.base(), size);
		return std::error_code();
	}

private:
	int flagsToPermissions(unsigned flags)
	{
		int result = 0;
		if(flags & llvm::sys::Memory::MF_READ)
		{
			result |= rr::PERMISSION_READ;
		}
		if(flags & llvm::sys::Memory::MF_WRITE)
		{
			result |= rr::PERMISSION_WRITE;
		}
		if(flags & llvm::sys::Memory::MF_EXEC)
		{
			result |= rr::PERMISSION_EXECUTE;
		}
		return result;
	}
};

template<typename T>
T alignUp(T val, T alignment)
{
	return alignment * ((val + alignment - 1) / alignment);
}

void *alignedAlloc(size_t size, size_t alignment)
{
	ASSERT(alignment < 256);
	auto allocation = new uint8_t[size + sizeof(uint8_t) + alignment];
	auto aligned = allocation;
	aligned += sizeof(uint8_t);                                                                       // Make space for the base-address offset.
	aligned = reinterpret_cast<uint8_t *>(alignUp(reinterpret_cast<uintptr_t>(aligned), alignment));  // align
	auto offset = static_cast<uint8_t>(aligned - allocation);
	aligned[-1] = offset;
	return aligned;
}

void alignedFree(void *ptr)
{
	auto aligned = reinterpret_cast<uint8_t *>(ptr);
	auto offset = aligned[-1];
	auto allocation = aligned - offset;
	delete[] allocation;
}

template<typename T>
static void atomicLoad(void *ptr, void *ret, llvm::AtomicOrdering ordering)
{
	*reinterpret_cast<T *>(ret) = std::atomic_load_explicit<T>(reinterpret_cast<std::atomic<T> *>(ptr), rr::atomicOrdering(ordering));
}

template<typename T>
static void atomicStore(void *ptr, void *val, llvm::AtomicOrdering ordering)
{
	std::atomic_store_explicit<T>(reinterpret_cast<std::atomic<T> *>(ptr), *reinterpret_cast<T *>(val), rr::atomicOrdering(ordering));
}

#ifdef __ANDROID__
template<typename F>
static uint32_t sync_fetch_and_op(uint32_t volatile *ptr, uint32_t val, F f)
{
	// Build an arbitrary op out of looped CAS
	for(;;)
	{
		uint32_t expected = *ptr;
		uint32_t desired = f(expected, val);

		if(expected == __sync_val_compare_and_swap_4(ptr, expected, desired))
		{
			return expected;
		}
	}
}
#endif

void *resolveExternalSymbol(const char *name)
{
	struct Atomic
	{
		static void load(size_t size, void *ptr, void *ret, llvm::AtomicOrdering ordering)
		{
			switch(size)
			{
				case 1: atomicLoad<uint8_t>(ptr, ret, ordering); break;
				case 2: atomicLoad<uint16_t>(ptr, ret, ordering); break;
				case 4: atomicLoad<uint32_t>(ptr, ret, ordering); break;
				case 8: atomicLoad<uint64_t>(ptr, ret, ordering); break;
				default:
					UNIMPLEMENTED_NO_BUG("Atomic::load(size: %d)", int(size));
			}
		}
		static void store(size_t size, void *ptr, void *ret, llvm::AtomicOrdering ordering)
		{
			switch(size)
			{
				case 1: atomicStore<uint8_t>(ptr, ret, ordering); break;
				case 2: atomicStore<uint16_t>(ptr, ret, ordering); break;
				case 4: atomicStore<uint32_t>(ptr, ret, ordering); break;
				case 8: atomicStore<uint64_t>(ptr, ret, ordering); break;
				default:
					UNIMPLEMENTED_NO_BUG("Atomic::store(size: %d)", int(size));
			}
		}
	};

	struct F
	{
		static void nop() {}
		static void neverCalled() { UNREACHABLE("Should never be called"); }

		static void *coroutine_alloc_frame(size_t size) { return alignedAlloc(size, 16); }
		static void coroutine_free_frame(void *ptr) { alignedFree(ptr); }

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

		static uint32_t sync_fetch_and_max_4(uint32_t *ptr, uint32_t val)
		{
			return sync_fetch_and_op(ptr, val, [](int32_t a, int32_t b) { return std::max(a, b); });
		}
		static uint32_t sync_fetch_and_min_4(uint32_t *ptr, uint32_t val)
		{
			return sync_fetch_and_op(ptr, val, [](int32_t a, int32_t b) { return std::min(a, b); });
		}
		static uint32_t sync_fetch_and_umax_4(uint32_t *ptr, uint32_t val)
		{
			return sync_fetch_and_op(ptr, val, [](uint32_t a, uint32_t b) { return std::max(a, b); });
		}
		static uint32_t sync_fetch_and_umin_4(uint32_t *ptr, uint32_t val)
		{
			return sync_fetch_and_op(ptr, val, [](uint32_t a, uint32_t b) { return std::min(a, b); });
		}
#endif
	};

	class Resolver
	{
	public:
		using FunctionMap = std::unordered_map<std::string, void *>;

		FunctionMap functions;

		Resolver()
		{
			functions.emplace("nop", reinterpret_cast<void *>(F::nop));
			functions.emplace("floorf", reinterpret_cast<void *>(floorf));
			functions.emplace("nearbyintf", reinterpret_cast<void *>(nearbyintf));
			functions.emplace("truncf", reinterpret_cast<void *>(truncf));
			functions.emplace("printf", reinterpret_cast<void *>(printf));
			functions.emplace("puts", reinterpret_cast<void *>(puts));
			functions.emplace("fmodf", reinterpret_cast<void *>(fmodf));

			functions.emplace("sinf", reinterpret_cast<void *>(sinf));
			functions.emplace("cosf", reinterpret_cast<void *>(cosf));
			functions.emplace("asinf", reinterpret_cast<void *>(asinf));
			functions.emplace("acosf", reinterpret_cast<void *>(acosf));
			functions.emplace("atanf", reinterpret_cast<void *>(atanf));
			functions.emplace("sinhf", reinterpret_cast<void *>(sinhf));
			functions.emplace("coshf", reinterpret_cast<void *>(coshf));
			functions.emplace("tanhf", reinterpret_cast<void *>(tanhf));
			functions.emplace("asinhf", reinterpret_cast<void *>(asinhf));
			functions.emplace("acoshf", reinterpret_cast<void *>(acoshf));
			functions.emplace("atanhf", reinterpret_cast<void *>(atanhf));
			functions.emplace("atan2f", reinterpret_cast<void *>(atan2f));
			functions.emplace("powf", reinterpret_cast<void *>(powf));
			functions.emplace("expf", reinterpret_cast<void *>(expf));
			functions.emplace("logf", reinterpret_cast<void *>(logf));
			functions.emplace("exp2f", reinterpret_cast<void *>(exp2f));
			functions.emplace("log2f", reinterpret_cast<void *>(log2f));

			functions.emplace("sin", reinterpret_cast<void *>(static_cast<double (*)(double)>(sin)));
			functions.emplace("cos", reinterpret_cast<void *>(static_cast<double (*)(double)>(cos)));
			functions.emplace("asin", reinterpret_cast<void *>(static_cast<double (*)(double)>(asin)));
			functions.emplace("acos", reinterpret_cast<void *>(static_cast<double (*)(double)>(acos)));
			functions.emplace("atan", reinterpret_cast<void *>(static_cast<double (*)(double)>(atan)));
			functions.emplace("sinh", reinterpret_cast<void *>(static_cast<double (*)(double)>(sinh)));
			functions.emplace("cosh", reinterpret_cast<void *>(static_cast<double (*)(double)>(cosh)));
			functions.emplace("tanh", reinterpret_cast<void *>(static_cast<double (*)(double)>(tanh)));
			functions.emplace("asinh", reinterpret_cast<void *>(static_cast<double (*)(double)>(asinh)));
			functions.emplace("acosh", reinterpret_cast<void *>(static_cast<double (*)(double)>(acosh)));
			functions.emplace("atanh", reinterpret_cast<void *>(static_cast<double (*)(double)>(atanh)));
			functions.emplace("atan2", reinterpret_cast<void *>(static_cast<double (*)(double, double)>(atan2)));
			functions.emplace("pow", reinterpret_cast<void *>(static_cast<double (*)(double, double)>(pow)));
			functions.emplace("exp", reinterpret_cast<void *>(static_cast<double (*)(double)>(exp)));
			functions.emplace("log", reinterpret_cast<void *>(static_cast<double (*)(double)>(log)));
			functions.emplace("exp2", reinterpret_cast<void *>(static_cast<double (*)(double)>(exp2)));
			functions.emplace("log2", reinterpret_cast<void *>(static_cast<double (*)(double)>(log2)));

			functions.emplace("atomic_load", reinterpret_cast<void *>(Atomic::load));
			functions.emplace("atomic_store", reinterpret_cast<void *>(Atomic::store));

			// FIXME(b/119409619): use an allocator here so we can control all memory allocations
			functions.emplace("coroutine_alloc_frame", reinterpret_cast<void *>(F::coroutine_alloc_frame));
			functions.emplace("coroutine_free_frame", reinterpret_cast<void *>(F::coroutine_free_frame));

#ifdef __APPLE__
			functions.emplace("sincosf_stret", reinterpret_cast<void *>(__sincosf_stret));
#elif defined(__linux__)
			functions.emplace("sincosf", reinterpret_cast<void *>(sincosf));
#elif defined(_WIN64)
			functions.emplace("chkstk", reinterpret_cast<void *>(__chkstk));
#elif defined(_WIN32)
			functions.emplace("chkstk", reinterpret_cast<void *>(_chkstk));
#endif

#ifdef __ARM_EABI__
			functions.emplace("aeabi_idivmod", reinterpret_cast<void *>(__aeabi_idivmod));
#endif
#ifdef __ANDROID__
			functions.emplace("aeabi_unwind_cpp_pr0", reinterpret_cast<void *>(F::neverCalled));
			functions.emplace("sync_synchronize", reinterpret_cast<void *>(F::sync_synchronize));
			functions.emplace("sync_fetch_and_add_4", reinterpret_cast<void *>(F::sync_fetch_and_add_4));
			functions.emplace("sync_fetch_and_and_4", reinterpret_cast<void *>(F::sync_fetch_and_and_4));
			functions.emplace("sync_fetch_and_or_4", reinterpret_cast<void *>(F::sync_fetch_and_or_4));
			functions.emplace("sync_fetch_and_xor_4", reinterpret_cast<void *>(F::sync_fetch_and_xor_4));
			functions.emplace("sync_fetch_and_sub_4", reinterpret_cast<void *>(F::sync_fetch_and_sub_4));
			functions.emplace("sync_lock_test_and_set_4", reinterpret_cast<void *>(F::sync_lock_test_and_set_4));
			functions.emplace("sync_val_compare_and_swap_4", reinterpret_cast<void *>(F::sync_val_compare_and_swap_4));
			functions.emplace("sync_fetch_and_max_4", reinterpret_cast<void *>(F::sync_fetch_and_max_4));
			functions.emplace("sync_fetch_and_min_4", reinterpret_cast<void *>(F::sync_fetch_and_min_4));
			functions.emplace("sync_fetch_and_umax_4", reinterpret_cast<void *>(F::sync_fetch_and_umax_4));
			functions.emplace("sync_fetch_and_umin_4", reinterpret_cast<void *>(F::sync_fetch_and_umin_4));
#endif
#if __has_feature(memory_sanitizer)
			functions.emplace("msan_unpoison", reinterpret_cast<void *>(__msan_unpoison));
#endif
		}
	};

	static Resolver resolver;

	// Trim off any underscores from the start of the symbol. LLVM likes
	// to append these on macOS.
	const char *trimmed = name;
	while(trimmed[0] == '_') { trimmed++; }

	auto it = resolver.functions.find(trimmed);
	// Missing functions will likely make the module fail in exciting non-obvious ways.
	ASSERT_MSG(it != resolver.functions.end(), "Missing external function: '%s'", name);
	return it->second;
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
#if defined(__clang__)
// TODO(bclayton): Switch to new JIT
// error: 'LegacyIRCompileLayer' is deprecated: ORCv1 layers (layers with the 'Legacy' prefix) are deprecated.
// Please use the ORCv2 IRCompileLayer instead [-Werror,-Wdeprecated-declarations]
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

	JITRoutine(
	    std::unique_ptr<llvm::Module> module,
	    llvm::Function **funcs,
	    size_t count,
	    const rr::Config &config)
	    : resolver(createLegacyLookupResolver(
	          session,
	          [&](const llvm::StringRef &name) {
		          void *func = resolveExternalSymbol(name.str().c_str());
		          if(func != nullptr)
		          {
			          return llvm::JITSymbol(
			              reinterpret_cast<uintptr_t>(func), llvm::JITSymbolFlags::Absolute);
		          }
		          return objLayer.findSymbol(name, true);
	          },
	          [](llvm::Error err) {
		          if(err)
		          {
			          // TODO: Log the symbol resolution errors.
			          return;
		          }
	          }))
	    , targetMachine(JITGlobals::get()->getTargetMachine(config.getOptimization().getLevel()))
	    , compileLayer(objLayer, llvm::orc::SimpleCompiler(*targetMachine))
	    , objLayer(
	          session,
	          [this](llvm::orc::VModuleKey) {
		          return ObjLayer::Resources{ std::make_shared<llvm::SectionMemoryManager>(&memoryMapper), resolver };
	          },
	          ObjLayer::NotifyLoadedFtor(),
	          [](llvm::orc::VModuleKey, const llvm::object::ObjectFile &Obj, const llvm::RuntimeDyld::LoadedObjectInfo &L) {
#ifdef ENABLE_RR_DEBUG_INFO
		          rr::DebugInfo::NotifyObjectEmitted(Obj, L);
#endif  // ENABLE_RR_DEBUG_INFO
	          },
	          [](llvm::orc::VModuleKey, const llvm::object::ObjectFile &Obj) {
#ifdef ENABLE_RR_DEBUG_INFO
		          rr::DebugInfo::NotifyFreeingObject(Obj);
#endif  // ENABLE_RR_DEBUG_INFO
	          })
	    , addresses(count)
	{

#if defined(__clang__)
#	pragma clang diagnostic pop
#elif defined(__GNUC__)
#	pragma GCC diagnostic pop
#endif

		std::vector<std::string> mangledNames(count);
		for(size_t i = 0; i < count; i++)
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
		for(size_t i = 0; i < count; i++)
		{
			auto symbol = compileLayer.findSymbolIn(moduleKey, mangledNames[i], false);
			if(auto address = symbol.getAddress())
			{
				addresses[i] = reinterpret_cast<void *>(static_cast<intptr_t>(address.get()));
			}
		}
	}

	const void *getEntry(int index) const override
	{
		return addresses[index];
	}

private:
	std::shared_ptr<llvm::orc::SymbolResolver> resolver;
	std::shared_ptr<llvm::TargetMachine> targetMachine;
	llvm::orc::ExecutionSession session;
	CompileLayer compileLayer;
	MemoryMapper memoryMapper;
	ObjLayer objLayer;
	std::vector<const void *> addresses;
};

}  // anonymous namespace

namespace rr {

JITBuilder::JITBuilder(const rr::Config &config)
    : config(config)
    , module(new llvm::Module("", context))
    , builder(new llvm::IRBuilder<>(context))
{
	module->setDataLayout(JITGlobals::get()->dataLayout);
}

void JITBuilder::optimize(const rr::Config &cfg)
{

#ifdef ENABLE_RR_DEBUG_INFO
	if(debugInfo != nullptr)
	{
		return;  // Don't optimize if we're generating debug info.
	}
#endif  // ENABLE_RR_DEBUG_INFO

	std::unique_ptr<llvm::legacy::PassManager> passManager(
	    new llvm::legacy::PassManager());

	for(auto pass : cfg.getOptimization().getPasses())
	{
		switch(pass)
		{
			case rr::Optimization::Pass::Disabled: break;
			case rr::Optimization::Pass::CFGSimplification: passManager->add(llvm::createCFGSimplificationPass()); break;
			case rr::Optimization::Pass::LICM: passManager->add(llvm::createLICMPass()); break;
			case rr::Optimization::Pass::AggressiveDCE: passManager->add(llvm::createAggressiveDCEPass()); break;
			case rr::Optimization::Pass::GVN: passManager->add(llvm::createGVNPass()); break;
			case rr::Optimization::Pass::InstructionCombining: passManager->add(llvm::createInstructionCombiningPass()); break;
			case rr::Optimization::Pass::Reassociate: passManager->add(llvm::createReassociatePass()); break;
			case rr::Optimization::Pass::DeadStoreElimination: passManager->add(llvm::createDeadStoreEliminationPass()); break;
			case rr::Optimization::Pass::SCCP: passManager->add(llvm::createSCCPPass()); break;
			case rr::Optimization::Pass::ScalarReplAggregates: passManager->add(llvm::createSROAPass()); break;
			case rr::Optimization::Pass::EarlyCSEPass: passManager->add(llvm::createEarlyCSEPass()); break;
			default:
				UNREACHABLE("pass: %d", int(pass));
		}
	}

	passManager->run(*module);
}

std::shared_ptr<rr::Routine> JITBuilder::acquireRoutine(llvm::Function **funcs, size_t count, const rr::Config &cfg)
{
	ASSERT(module);
	return std::make_shared<JITRoutine>(std::move(module), funcs, count, cfg);
}

}  // namespace rr
