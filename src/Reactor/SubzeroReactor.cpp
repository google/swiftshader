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

#include "Debug.hpp"
#include "EmulatedReactor.hpp"
#include "Print.hpp"
#include "Reactor.hpp"
#include "ReactorDebugInfo.hpp"

#include "ExecutableMemory.hpp"
#include "Optimizer.hpp"

#include "src/IceCfg.h"
#include "src/IceCfgNode.h"
#include "src/IceELFObjectWriter.h"
#include "src/IceELFStreamer.h"
#include "src/IceGlobalContext.h"
#include "src/IceGlobalInits.h"
#include "src/IceTypes.h"

#include "llvm/Support/Compiler.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#include "marl/event.h"

#if __has_feature(memory_sanitizer)
#	include <sanitizer/msan_interface.h>
#endif

#if defined(_WIN32)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif  // !WIN32_LEAN_AND_MEAN
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif  // !NOMINMAX
#	include <Windows.h>
#endif

#include <array>
#include <iostream>
#include <limits>
#include <mutex>

// Subzero utility functions
// These functions only accept and return Subzero (Ice) types, and do not access any globals.
namespace {
namespace sz {

Ice::Cfg *createFunction(Ice::GlobalContext *context, Ice::Type returnType, const std::vector<Ice::Type> &paramTypes)
{
	uint32_t sequenceNumber = 0;
	auto function = Ice::Cfg::create(context, sequenceNumber).release();

	Ice::CfgLocalAllocatorScope allocScope{ function };

	for(auto type : paramTypes)
	{
		Ice::Variable *arg = function->makeVariable(type);
		function->addArg(arg);
	}

	Ice::CfgNode *node = function->makeNode();
	function->setEntryNode(node);

	return function;
}

Ice::Type getPointerType(Ice::Type elementType)
{
	if(sizeof(void *) == 8)
	{
		return Ice::IceType_i64;
	}
	else
	{
		return Ice::IceType_i32;
	}
}

Ice::Variable *allocateStackVariable(Ice::Cfg *function, Ice::Type type, int arraySize = 0)
{
	int typeSize = Ice::typeWidthInBytes(type);
	int totalSize = typeSize * (arraySize ? arraySize : 1);

	auto bytes = Ice::ConstantInteger32::create(function->getContext(), Ice::IceType_i32, totalSize);
	auto address = function->makeVariable(getPointerType(type));
	auto alloca = Ice::InstAlloca::create(function, address, bytes, typeSize);
	function->getEntryNode()->getInsts().push_front(alloca);

	return address;
}

Ice::Constant *getConstantPointer(Ice::GlobalContext *context, void const *ptr)
{
	if(sizeof(void *) == 8)
	{
		return context->getConstantInt64(reinterpret_cast<intptr_t>(ptr));
	}
	else
	{
		return context->getConstantInt32(reinterpret_cast<intptr_t>(ptr));
	}
}

// TODO(amaiorano): remove this prototype once these are moved to separate header/cpp
Ice::Variable *createTruncate(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Operand *from, Ice::Type toType);

// Wrapper for calls on C functions with Ice types
Ice::Variable *Call(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Type retTy, Ice::Operand *callTarget, const std::vector<Ice::Operand *> &iceArgs, bool isVariadic)
{
	Ice::Variable *ret = nullptr;

	// Subzero doesn't support boolean return values. Replace with an i32 temporarily,
	// then truncate result to bool.
	// TODO(b/151158858): Add support to Subzero's InstCall for bool-returning functions
	const bool returningBool = (retTy == Ice::IceType_i1);
	if(returningBool)
	{
		ret = function->makeVariable(Ice::IceType_i32);
	}
	else if(retTy != Ice::IceType_void)
	{
		ret = function->makeVariable(retTy);
	}

	auto call = Ice::InstCall::create(function, iceArgs.size(), ret, callTarget, false, false, isVariadic);
	for(auto arg : iceArgs)
	{
		call->addArg(arg);
	}

	basicBlock->appendInst(call);

	if(returningBool)
	{
		// Truncate result to bool so that if any (lsb) bits were set, result will be true
		ret = createTruncate(function, basicBlock, ret, Ice::IceType_i1);
	}

	return ret;
}

Ice::Variable *Call(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Type retTy, void const *fptr, const std::vector<Ice::Operand *> &iceArgs, bool isVariadic)
{
	Ice::Operand *callTarget = getConstantPointer(function->getContext(), fptr);
	return Call(function, basicBlock, retTy, callTarget, iceArgs, isVariadic);
}

// Wrapper for calls on C functions with Ice types
template<typename Return, typename... CArgs, typename... RArgs>
Ice::Variable *Call(Ice::Cfg *function, Ice::CfgNode *basicBlock, Return(fptr)(CArgs...), RArgs &&... args)
{
	static_assert(sizeof...(CArgs) == sizeof...(RArgs), "Expected number of args don't match");

	Ice::Type retTy = T(rr::CToReactorT<Return>::getType());
	std::vector<Ice::Operand *> iceArgs{ std::forward<RArgs>(args)... };
	return Call(function, basicBlock, retTy, reinterpret_cast<void const *>(fptr), iceArgs, false);
}

// Returns a non-const variable copy of const v
Ice::Variable *createUnconstCast(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Constant *v)
{
	Ice::Variable *result = function->makeVariable(v->getType());
	Ice::InstCast *cast = Ice::InstCast::create(function, Ice::InstCast::Bitcast, result, v);
	basicBlock->appendInst(cast);
	return result;
}

Ice::Variable *createTruncate(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Operand *from, Ice::Type toType)
{
	Ice::Variable *to = function->makeVariable(toType);
	Ice::InstCast *cast = Ice::InstCast::create(function, Ice::InstCast::Trunc, to, from);
	basicBlock->appendInst(cast);
	return to;
}

Ice::Variable *createLoad(Ice::Cfg *function, Ice::CfgNode *basicBlock, Ice::Operand *ptr, Ice::Type type, unsigned int align)
{
	// TODO(b/148272103): InstLoad assumes that a constant ptr is an offset, rather than an
	// absolute address. We circumvent this by casting to a non-const variable, and loading
	// from that.
	if(auto *cptr = llvm::dyn_cast<Ice::Constant>(ptr))
	{
		ptr = sz::createUnconstCast(function, basicBlock, cptr);
	}

	Ice::Variable *result = function->makeVariable(type);
	auto load = Ice::InstLoad::create(function, result, ptr, align);
	basicBlock->appendInst(load);

	return result;
}

}  // namespace sz
}  // namespace

namespace rr {
class ELFMemoryStreamer;
class CoroutineGenerator;
}  // namespace rr

namespace {

// Default configuration settings. Must be accessed under mutex lock.
std::mutex defaultConfigLock;
rr::Config &defaultConfig()
{
	// This uses a static in a function to avoid the cost of a global static
	// initializer. See http://neugierig.org/software/chromium/notes/2011/08/static-initializers.html
	static rr::Config config = rr::Config::Edit()
	                               .apply({});
	return config;
}

Ice::GlobalContext *context = nullptr;
Ice::Cfg *function = nullptr;
Ice::CfgNode *entryBlock = nullptr;
Ice::CfgNode *basicBlockTop = nullptr;
Ice::CfgNode *basicBlock = nullptr;
Ice::CfgLocalAllocatorScope *allocator = nullptr;
rr::ELFMemoryStreamer *routine = nullptr;

std::mutex codegenMutex;

Ice::ELFFileStreamer *elfFile = nullptr;
Ice::Fdstream *out = nullptr;

// Coroutine globals
rr::Type *coroYieldType = nullptr;
std::shared_ptr<rr::CoroutineGenerator> coroGen;
marl::Scheduler &getOrCreateScheduler()
{
	static auto scheduler = [] {
		auto s = std::make_unique<marl::Scheduler>();
		s->setWorkerThreadCount(8);
		return s;
	}();

	return *scheduler;
}
}  // Anonymous namespace

namespace {

#if !defined(__i386__) && defined(_M_IX86)
#	define __i386__ 1
#endif

#if !defined(__x86_64__) && (defined(_M_AMD64) || defined(_M_X64))
#	define __x86_64__ 1
#endif

Ice::OptLevel toIce(rr::Optimization::Level level)
{
	switch(level)
	{
		// Note that Opt_0 and Opt_1 are not implemented by Subzero
		case rr::Optimization::Level::None: return Ice::Opt_m1;
		case rr::Optimization::Level::Less: return Ice::Opt_m1;
		case rr::Optimization::Level::Default: return Ice::Opt_2;
		case rr::Optimization::Level::Aggressive: return Ice::Opt_2;
		default: UNREACHABLE("Unknown Optimization Level %d", int(level));
	}
	return Ice::Opt_2;
}

Ice::Intrinsics::MemoryOrder stdToIceMemoryOrder(std::memory_order memoryOrder)
{
	switch(memoryOrder)
	{
		case std::memory_order_relaxed: return Ice::Intrinsics::MemoryOrderRelaxed;
		case std::memory_order_consume: return Ice::Intrinsics::MemoryOrderConsume;
		case std::memory_order_acquire: return Ice::Intrinsics::MemoryOrderAcquire;
		case std::memory_order_release: return Ice::Intrinsics::MemoryOrderRelease;
		case std::memory_order_acq_rel: return Ice::Intrinsics::MemoryOrderAcquireRelease;
		case std::memory_order_seq_cst: return Ice::Intrinsics::MemoryOrderSequentiallyConsistent;
	}
	return Ice::Intrinsics::MemoryOrderInvalid;
}

class CPUID
{
public:
	const static bool ARM;
	const static bool SSE4_1;

private:
	static void cpuid(int registers[4], int info)
	{
#if defined(__i386__) || defined(__x86_64__)
#	if defined(_WIN32)
		__cpuid(registers, info);
#	else
		__asm volatile("cpuid"
		               : "=a"(registers[0]), "=b"(registers[1]), "=c"(registers[2]), "=d"(registers[3])
		               : "a"(info));
#	endif
#else
		registers[0] = 0;
		registers[1] = 0;
		registers[2] = 0;
		registers[3] = 0;
#endif
	}

	static bool detectARM()
	{
#if defined(__arm__) || defined(__aarch64__)
		return true;
#elif defined(__i386__) || defined(__x86_64__)
		return false;
#elif defined(__mips__)
		return false;
#else
#	error "Unknown architecture"
#endif
	}

	static bool detectSSE4_1()
	{
#if defined(__i386__) || defined(__x86_64__)
		int registers[4];
		cpuid(registers, 1);
		return (registers[2] & 0x00080000) != 0;
#else
		return false;
#endif
	}
};

const bool CPUID::ARM = CPUID::detectARM();
const bool CPUID::SSE4_1 = CPUID::detectSSE4_1();
const bool emulateIntrinsics = false;
const bool emulateMismatchedBitCast = CPUID::ARM;

constexpr bool subzeroDumpEnabled = false;
constexpr bool subzeroEmitTextAsm = false;

#if !ALLOW_DUMP
static_assert(!subzeroDumpEnabled, "Compile Subzero with ALLOW_DUMP=1 for subzeroDumpEnabled");
static_assert(!subzeroEmitTextAsm, "Compile Subzero with ALLOW_DUMP=1 for subzeroEmitTextAsm");
#endif

}  // anonymous namespace

namespace rr {

std::string BackendName()
{
	return "Subzero";
}

const Capabilities Caps = {
	true,  // CoroutinesSupported
};

enum EmulatedType
{
	EmulatedShift = 16,
	EmulatedV2 = 2 << EmulatedShift,
	EmulatedV4 = 4 << EmulatedShift,
	EmulatedV8 = 8 << EmulatedShift,
	EmulatedBits = EmulatedV2 | EmulatedV4 | EmulatedV8,

	Type_v2i32 = Ice::IceType_v4i32 | EmulatedV2,
	Type_v4i16 = Ice::IceType_v8i16 | EmulatedV4,
	Type_v2i16 = Ice::IceType_v8i16 | EmulatedV2,
	Type_v8i8 = Ice::IceType_v16i8 | EmulatedV8,
	Type_v4i8 = Ice::IceType_v16i8 | EmulatedV4,
	Type_v2f32 = Ice::IceType_v4f32 | EmulatedV2,
};

class Value : public Ice::Operand
{};
class SwitchCases : public Ice::InstSwitch
{};
class BasicBlock : public Ice::CfgNode
{};

Ice::Type T(Type *t)
{
	static_assert(static_cast<unsigned int>(Ice::IceType_NUM) < static_cast<unsigned int>(EmulatedBits), "Ice::Type overlaps with our emulated types!");
	return (Ice::Type)(reinterpret_cast<std::intptr_t>(t) & ~EmulatedBits);
}

Type *T(Ice::Type t)
{
	return reinterpret_cast<Type *>(t);
}

Type *T(EmulatedType t)
{
	return reinterpret_cast<Type *>(t);
}

std::vector<Ice::Type> T(const std::vector<Type *> &types)
{
	std::vector<Ice::Type> result;
	result.reserve(types.size());
	for(auto &t : types)
	{
		result.push_back(T(t));
	}
	return result;
}

Value *V(Ice::Operand *v)
{
	return reinterpret_cast<Value *>(v);
}

Ice::Operand *V(Value *v)
{
	return reinterpret_cast<Ice::Operand *>(v);
}

std::vector<Ice::Operand *> V(const std::vector<Value *> &values)
{
	std::vector<Ice::Operand *> result;
	result.reserve(values.size());
	for(auto &v : values)
	{
		result.push_back(V(v));
	}
	return result;
}

BasicBlock *B(Ice::CfgNode *b)
{
	return reinterpret_cast<BasicBlock *>(b);
}

static size_t typeSize(Type *type)
{
	if(reinterpret_cast<std::intptr_t>(type) & EmulatedBits)
	{
		switch(reinterpret_cast<std::intptr_t>(type))
		{
			case Type_v2i32: return 8;
			case Type_v4i16: return 8;
			case Type_v2i16: return 4;
			case Type_v8i8: return 8;
			case Type_v4i8: return 4;
			case Type_v2f32: return 8;
			default: ASSERT(false);
		}
	}

	return Ice::typeWidthInBytes(T(type));
}

static void finalizeFunction()
{
	// Create a return if none was added
	if(::basicBlock->getInsts().empty() || ::basicBlock->getInsts().back().getKind() != Ice::Inst::Ret)
	{
		Nucleus::createRetVoid();
	}

	// Connect the entry block to the top of the initial basic block
	auto br = Ice::InstBr::create(::function, ::basicBlockTop);
	::entryBlock->appendInst(br);
}

using ElfHeader = std::conditional<sizeof(void *) == 8, Elf64_Ehdr, Elf32_Ehdr>::type;
using SectionHeader = std::conditional<sizeof(void *) == 8, Elf64_Shdr, Elf32_Shdr>::type;

inline const SectionHeader *sectionHeader(const ElfHeader *elfHeader)
{
	return reinterpret_cast<const SectionHeader *>((intptr_t)elfHeader + elfHeader->e_shoff);
}

inline const SectionHeader *elfSection(const ElfHeader *elfHeader, int index)
{
	return &sectionHeader(elfHeader)[index];
}

static void *relocateSymbol(const ElfHeader *elfHeader, const Elf32_Rel &relocation, const SectionHeader &relocationTable)
{
	const SectionHeader *target = elfSection(elfHeader, relocationTable.sh_info);

	uint32_t index = relocation.getSymbol();
	int table = relocationTable.sh_link;
	void *symbolValue = nullptr;

	if(index != SHN_UNDEF)
	{
		if(table == SHN_UNDEF) return nullptr;
		const SectionHeader *symbolTable = elfSection(elfHeader, table);

		uint32_t symtab_entries = symbolTable->sh_size / symbolTable->sh_entsize;
		if(index >= symtab_entries)
		{
			ASSERT(index < symtab_entries && "Symbol Index out of range");
			return nullptr;
		}

		intptr_t symbolAddress = (intptr_t)elfHeader + symbolTable->sh_offset;
		Elf32_Sym &symbol = ((Elf32_Sym *)symbolAddress)[index];
		uint16_t section = symbol.st_shndx;

		if(section != SHN_UNDEF && section < SHN_LORESERVE)
		{
			const SectionHeader *target = elfSection(elfHeader, symbol.st_shndx);
			symbolValue = reinterpret_cast<void *>((intptr_t)elfHeader + symbol.st_value + target->sh_offset);
		}
		else
		{
			return nullptr;
		}
	}

	intptr_t address = (intptr_t)elfHeader + target->sh_offset;
	unaligned_ptr<int32_t> patchSite = (int32_t *)(address + relocation.r_offset);

	if(CPUID::ARM)
	{
		switch(relocation.getType())
		{
			case R_ARM_NONE:
				// No relocation
				break;
			case R_ARM_MOVW_ABS_NC:
			{
				uint32_t thumb = 0;  // Calls to Thumb code not supported.
				uint32_t lo = (uint32_t)(intptr_t)symbolValue | thumb;
				*patchSite = (*patchSite & 0xFFF0F000) | ((lo & 0xF000) << 4) | (lo & 0x0FFF);
			}
			break;
			case R_ARM_MOVT_ABS:
			{
				uint32_t hi = (uint32_t)(intptr_t)(symbolValue) >> 16;
				*patchSite = (*patchSite & 0xFFF0F000) | ((hi & 0xF000) << 4) | (hi & 0x0FFF);
			}
			break;
			default:
				ASSERT(false && "Unsupported relocation type");
				return nullptr;
		}
	}
	else
	{
		switch(relocation.getType())
		{
			case R_386_NONE:
				// No relocation
				break;
			case R_386_32:
				*patchSite = (int32_t)((intptr_t)symbolValue + *patchSite);
				break;
			case R_386_PC32:
				*patchSite = (int32_t)((intptr_t)symbolValue + *patchSite - (intptr_t)patchSite);
				break;
			default:
				ASSERT(false && "Unsupported relocation type");
				return nullptr;
		}
	}

	return symbolValue;
}

static void *relocateSymbol(const ElfHeader *elfHeader, const Elf64_Rela &relocation, const SectionHeader &relocationTable)
{
	const SectionHeader *target = elfSection(elfHeader, relocationTable.sh_info);

	uint32_t index = relocation.getSymbol();
	int table = relocationTable.sh_link;
	void *symbolValue = nullptr;

	if(index != SHN_UNDEF)
	{
		if(table == SHN_UNDEF) return nullptr;
		const SectionHeader *symbolTable = elfSection(elfHeader, table);

		uint32_t symtab_entries = symbolTable->sh_size / symbolTable->sh_entsize;
		if(index >= symtab_entries)
		{
			ASSERT(index < symtab_entries && "Symbol Index out of range");
			return nullptr;
		}

		intptr_t symbolAddress = (intptr_t)elfHeader + symbolTable->sh_offset;
		Elf64_Sym &symbol = ((Elf64_Sym *)symbolAddress)[index];
		uint16_t section = symbol.st_shndx;

		if(section != SHN_UNDEF && section < SHN_LORESERVE)
		{
			const SectionHeader *target = elfSection(elfHeader, symbol.st_shndx);
			symbolValue = reinterpret_cast<void *>((intptr_t)elfHeader + symbol.st_value + target->sh_offset);
		}
		else
		{
			return nullptr;
		}
	}

	intptr_t address = (intptr_t)elfHeader + target->sh_offset;
	unaligned_ptr<int32_t> patchSite32 = (int32_t *)(address + relocation.r_offset);
	unaligned_ptr<int64_t> patchSite64 = (int64_t *)(address + relocation.r_offset);

	switch(relocation.getType())
	{
		case R_X86_64_NONE:
			// No relocation
			break;
		case R_X86_64_64:
			*patchSite64 = (int64_t)((intptr_t)symbolValue + *patchSite64 + relocation.r_addend);
			break;
		case R_X86_64_PC32:
			*patchSite32 = (int32_t)((intptr_t)symbolValue + *patchSite32 - (intptr_t)patchSite32 + relocation.r_addend);
			break;
		case R_X86_64_32S:
			*patchSite32 = (int32_t)((intptr_t)symbolValue + *patchSite32 + relocation.r_addend);
			break;
		default:
			ASSERT(false && "Unsupported relocation type");
			return nullptr;
	}

	return symbolValue;
}

struct EntryPoint
{
	const void *entry;
	size_t codeSize = 0;
};

std::vector<EntryPoint> loadImage(uint8_t *const elfImage, const std::vector<const char *> &functionNames)
{
	ASSERT(functionNames.size() > 0);
	std::vector<EntryPoint> entryPoints(functionNames.size());

	ElfHeader *elfHeader = (ElfHeader *)elfImage;

	// TODO: assert?
	if(!elfHeader->checkMagic())
	{
		return {};
	}

	// Expect ELF bitness to match platform
	ASSERT(sizeof(void *) == 8 ? elfHeader->getFileClass() == ELFCLASS64 : elfHeader->getFileClass() == ELFCLASS32);
#if defined(__i386__)
	ASSERT(sizeof(void *) == 4 && elfHeader->e_machine == EM_386);
#elif defined(__x86_64__)
	ASSERT(sizeof(void *) == 8 && elfHeader->e_machine == EM_X86_64);
#elif defined(__arm__)
	ASSERT(sizeof(void *) == 4 && elfHeader->e_machine == EM_ARM);
#elif defined(__aarch64__)
	ASSERT(sizeof(void *) == 8 && elfHeader->e_machine == EM_AARCH64);
#elif defined(__mips__)
	ASSERT(sizeof(void *) == 4 && elfHeader->e_machine == EM_MIPS);
#else
#	error "Unsupported platform"
#endif

	SectionHeader *sectionHeader = (SectionHeader *)(elfImage + elfHeader->e_shoff);

	for(int i = 0; i < elfHeader->e_shnum; i++)
	{
		if(sectionHeader[i].sh_type == SHT_PROGBITS)
		{
			if(sectionHeader[i].sh_flags & SHF_EXECINSTR)
			{
				auto findSectionNameEntryIndex = [&]() -> size_t {
					auto sectionNameOffset = sectionHeader[elfHeader->e_shstrndx].sh_offset + sectionHeader[i].sh_name;
					const char *sectionName = reinterpret_cast<const char *>(elfImage + sectionNameOffset);

					for(size_t j = 0; j < functionNames.size(); ++j)
					{
						if(strstr(sectionName, functionNames[j]) != nullptr)
						{
							return j;
						}
					}

					UNREACHABLE("Failed to find executable section that matches input function names");
					return static_cast<size_t>(-1);
				};

				size_t index = findSectionNameEntryIndex();
				entryPoints[index].entry = elfImage + sectionHeader[i].sh_offset;
				entryPoints[index].codeSize = sectionHeader[i].sh_size;
			}
		}
		else if(sectionHeader[i].sh_type == SHT_REL)
		{
			ASSERT(sizeof(void *) == 4 && "UNIMPLEMENTED");  // Only expected/implemented for 32-bit code

			for(Elf32_Word index = 0; index < sectionHeader[i].sh_size / sectionHeader[i].sh_entsize; index++)
			{
				const Elf32_Rel &relocation = ((const Elf32_Rel *)(elfImage + sectionHeader[i].sh_offset))[index];
				relocateSymbol(elfHeader, relocation, sectionHeader[i]);
			}
		}
		else if(sectionHeader[i].sh_type == SHT_RELA)
		{
			ASSERT(sizeof(void *) == 8 && "UNIMPLEMENTED");  // Only expected/implemented for 64-bit code

			for(Elf32_Word index = 0; index < sectionHeader[i].sh_size / sectionHeader[i].sh_entsize; index++)
			{
				const Elf64_Rela &relocation = ((const Elf64_Rela *)(elfImage + sectionHeader[i].sh_offset))[index];
				relocateSymbol(elfHeader, relocation, sectionHeader[i]);
			}
		}
	}

	return entryPoints;
}

template<typename T>
struct ExecutableAllocator
{
	ExecutableAllocator() {}
	template<class U>
	ExecutableAllocator(const ExecutableAllocator<U> &other)
	{}

	using value_type = T;
	using size_type = std::size_t;

	T *allocate(size_type n)
	{
		return (T *)allocateMemoryPages(
		    sizeof(T) * n, PERMISSION_READ | PERMISSION_WRITE, true);
	}

	void deallocate(T *p, size_type n)
	{
		deallocateMemoryPages(p, sizeof(T) * n);
	}
};

class ELFMemoryStreamer : public Ice::ELFStreamer, public Routine
{
	ELFMemoryStreamer(const ELFMemoryStreamer &) = delete;
	ELFMemoryStreamer &operator=(const ELFMemoryStreamer &) = delete;

public:
	ELFMemoryStreamer()
	    : Routine()
	{
		position = 0;
		buffer.reserve(0x1000);
	}

	~ELFMemoryStreamer() override
	{
	}

	void write8(uint8_t Value) override
	{
		if(position == (uint64_t)buffer.size())
		{
			buffer.push_back(Value);
			position++;
		}
		else if(position < (uint64_t)buffer.size())
		{
			buffer[position] = Value;
			position++;
		}
		else
			ASSERT(false && "UNIMPLEMENTED");
	}

	void writeBytes(llvm::StringRef Bytes) override
	{
		std::size_t oldSize = buffer.size();
		buffer.resize(oldSize + Bytes.size());
		memcpy(&buffer[oldSize], Bytes.begin(), Bytes.size());
		position += Bytes.size();
	}

	uint64_t tell() const override { return position; }

	void seek(uint64_t Off) override { position = Off; }

	std::vector<EntryPoint> loadImageAndGetEntryPoints(const std::vector<const char *> &functionNames)
	{
		auto entryPoints = loadImage(&buffer[0], functionNames);

#if defined(_WIN32)
		FlushInstructionCache(GetCurrentProcess(), NULL, 0);
#else
		for(auto &entryPoint : entryPoints)
		{
			__builtin___clear_cache((char *)entryPoint.entry, (char *)entryPoint.entry + entryPoint.codeSize);
		}
#endif

		return entryPoints;
	}

	void finalize()
	{
		position = std::numeric_limits<std::size_t>::max();  // Can't stream more data after this

		protectMemoryPages(&buffer[0], buffer.size(), PERMISSION_READ | PERMISSION_EXECUTE);
	}

	void setEntry(int index, const void *func)
	{
		ASSERT(func);
		funcs[index] = func;
	}

	const void *getEntry(int index) const override
	{
		ASSERT(funcs[index]);
		return funcs[index];
	}

	const void *addConstantData(const void *data, size_t size, size_t alignment = 1)
	{
		// TODO(b/148086935): Replace with a buffer allocator.
		size_t space = size + alignment;
		auto buf = std::unique_ptr<uint8_t[]>(new uint8_t[space]);
		void *ptr = buf.get();
		void *alignedPtr = std::align(alignment, size, ptr, space);
		ASSERT(alignedPtr);
		memcpy(alignedPtr, data, size);
		constantData.emplace_back(std::move(buf));
		return alignedPtr;
	}

private:
	std::array<const void *, Nucleus::CoroutineEntryCount> funcs = {};
	std::vector<uint8_t, ExecutableAllocator<uint8_t>> buffer;
	std::size_t position;
	std::vector<std::unique_ptr<uint8_t[]>> constantData;
};

#ifdef ENABLE_RR_PRINT
void VPrintf(const std::vector<Value *> &vals)
{
	sz::Call(::function, ::basicBlock, Ice::IceType_i32, reinterpret_cast<const void *>(::printf), V(vals), true);
}
#endif  // ENABLE_RR_PRINT

Nucleus::Nucleus()
{
	::codegenMutex.lock();  // Reactor is currently not thread safe

	Ice::ClFlags &Flags = Ice::ClFlags::Flags;
	Ice::ClFlags::getParsedClFlags(Flags);

#if defined(__arm__)
	Flags.setTargetArch(Ice::Target_ARM32);
	Flags.setTargetInstructionSet(Ice::ARM32InstructionSet_HWDivArm);
#elif defined(__mips__)
	Flags.setTargetArch(Ice::Target_MIPS32);
	Flags.setTargetInstructionSet(Ice::BaseInstructionSet);
#else  // x86
	Flags.setTargetArch(sizeof(void *) == 8 ? Ice::Target_X8664 : Ice::Target_X8632);
	Flags.setTargetInstructionSet(CPUID::SSE4_1 ? Ice::X86InstructionSet_SSE4_1 : Ice::X86InstructionSet_SSE2);
#endif
	Flags.setOutFileType(Ice::FT_Elf);
	Flags.setOptLevel(toIce(getDefaultConfig().getOptimization().getLevel()));
	Flags.setApplicationBinaryInterface(Ice::ABI_Platform);
	Flags.setVerbose(subzeroDumpEnabled ? Ice::IceV_Most : Ice::IceV_None);
	Flags.setDisableHybridAssembly(true);

	// Emit functions into separate sections in the ELF so we can find them by name
	Flags.setFunctionSections(true);

	static llvm::raw_os_ostream cout(std::cout);
	static llvm::raw_os_ostream cerr(std::cerr);

	if(subzeroEmitTextAsm)
	{
		// Decorate text asm with liveness info
		Flags.setDecorateAsm(true);
	}

	if(false)  // Write out to a file
	{
		std::error_code errorCode;
		::out = new Ice::Fdstream("out.o", errorCode, llvm::sys::fs::F_None);
		::elfFile = new Ice::ELFFileStreamer(*out);
		::context = new Ice::GlobalContext(&cout, &cout, &cerr, elfFile);
	}
	else
	{
		ELFMemoryStreamer *elfMemory = new ELFMemoryStreamer();
		::context = new Ice::GlobalContext(&cout, &cout, &cerr, elfMemory);
		::routine = elfMemory;
	}
}

Nucleus::~Nucleus()
{
	delete ::routine;
	::routine = nullptr;

	delete ::allocator;
	::allocator = nullptr;

	delete ::function;
	::function = nullptr;

	delete ::context;
	::context = nullptr;

	delete ::elfFile;
	::elfFile = nullptr;

	delete ::out;
	::out = nullptr;

	::entryBlock = nullptr;
	::basicBlock = nullptr;
	::basicBlockTop = nullptr;

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

// This function lowers and produces executable binary code in memory for the input functions,
// and returns a Routine with the entry points to these functions.
template<size_t Count>
static std::shared_ptr<Routine> acquireRoutine(Ice::Cfg *const (&functions)[Count], const char *const (&names)[Count], const Config::Edit &cfgEdit)
{
	// This logic is modeled after the IceCompiler, as well as GlobalContext::translateFunctions
	// and GlobalContext::emitItems.

	if(subzeroDumpEnabled)
	{
		// Output dump strings immediately, rather than once buffer is full. Useful for debugging.
		::context->getStrDump().SetUnbuffered();
	}

	::context->emitFileHeader();

	// Translate

	for(size_t i = 0; i < Count; ++i)
	{
		Ice::Cfg *currFunc = functions[i];

		// Install function allocator in TLS for Cfg-specific container allocators
		Ice::CfgLocalAllocatorScope allocScope(currFunc);

		currFunc->setFunctionName(Ice::GlobalString::createWithString(::context, names[i]));

		rr::optimize(currFunc);

		currFunc->computeInOutEdges();
		ASSERT_MSG(!currFunc->hasError(), "%s", currFunc->getError().c_str());

		currFunc->translate();
		ASSERT_MSG(!currFunc->hasError(), "%s", currFunc->getError().c_str());

		currFunc->getAssembler<>()->setInternal(currFunc->getInternal());

		if(subzeroEmitTextAsm)
		{
			currFunc->emit();
		}

		currFunc->emitIAS();
	}

	// Emit items

	::context->lowerGlobals("");

	auto objectWriter = ::context->getObjectWriter();

	for(size_t i = 0; i < Count; ++i)
	{
		Ice::Cfg *currFunc = functions[i];

		// Accumulate globals from functions to emit into the "last" section at the end
		auto globals = currFunc->getGlobalInits();
		if(globals && !globals->empty())
		{
			::context->getGlobals()->merge(globals.get());
		}

		auto assembler = currFunc->releaseAssembler();
		assembler->alignFunction();
		objectWriter->writeFunctionCode(currFunc->getFunctionName(), currFunc->getInternal(), assembler.get());
	}

	::context->lowerGlobals("last");
	::context->lowerConstants();
	::context->lowerJumpTables();

	objectWriter->setUndefinedSyms(::context->getConstantExternSyms());
	::context->emitTargetRODataSections();
	objectWriter->writeNonUserSections();

	// Done compiling functions, get entry pointers to each of them
	auto entryPoints = ::routine->loadImageAndGetEntryPoints({ names, names + Count });
	ASSERT(entryPoints.size() == Count);
	for(size_t i = 0; i < entryPoints.size(); ++i)
	{
		::routine->setEntry(i, entryPoints[i].entry);
	}

	::routine->finalize();

	Routine *handoffRoutine = ::routine;
	::routine = nullptr;

	return std::shared_ptr<Routine>(handoffRoutine);
}

std::shared_ptr<Routine> Nucleus::acquireRoutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
{
	finalizeFunction();
	return rr::acquireRoutine({ ::function }, { name }, cfgEdit);
}

Value *Nucleus::allocateStackVariable(Type *t, int arraySize)
{
	Ice::Type type = T(t);
	int typeSize = Ice::typeWidthInBytes(type);
	int totalSize = typeSize * (arraySize ? arraySize : 1);

	auto bytes = Ice::ConstantInteger32::create(::context, Ice::IceType_i32, totalSize);
	auto address = ::function->makeVariable(T(getPointerType(t)));
	auto alloca = Ice::InstAlloca::create(::function, address, bytes, typeSize);
	::function->getEntryNode()->getInsts().push_front(alloca);

	return V(address);
}

BasicBlock *Nucleus::createBasicBlock()
{
	return B(::function->makeNode());
}

BasicBlock *Nucleus::getInsertBlock()
{
	return B(::basicBlock);
}

void Nucleus::setInsertBlock(BasicBlock *basicBlock)
{
	//	ASSERT(::basicBlock->getInsts().back().getTerminatorEdges().size() >= 0 && "Previous basic block must have a terminator");

	Variable::materializeAll();

	::basicBlock = basicBlock;
}

void Nucleus::createFunction(Type *returnType, const std::vector<Type *> &paramTypes)
{
	ASSERT(::function == nullptr);
	ASSERT(::allocator == nullptr);
	ASSERT(::entryBlock == nullptr);
	ASSERT(::basicBlock == nullptr);
	ASSERT(::basicBlockTop == nullptr);

	::function = sz::createFunction(::context, T(returnType), T(paramTypes));

	// NOTE: The scoped allocator sets the TLS allocator to the one in the function. This global one
	// becomes invalid if another one is created; for example, when creating await and destroy functions
	// for coroutines, in which case, we must make sure to create a new scoped allocator for ::function again.
	// TODO: Get rid of this as a global, and create scoped allocs in every Nucleus function instead.
	::allocator = new Ice::CfgLocalAllocatorScope(::function);

	::entryBlock = ::function->getEntryNode();
	::basicBlock = ::function->makeNode();
	::basicBlockTop = ::basicBlock;
}

Value *Nucleus::getArgument(unsigned int index)
{
	return V(::function->getArgs()[index]);
}

void Nucleus::createRetVoid()
{
	RR_DEBUG_INFO_UPDATE_LOC();

	// Code generated after this point is unreachable, so any variables
	// being read can safely return an undefined value. We have to avoid
	// materializing variables after the terminator ret instruction.
	Variable::killUnmaterialized();

	Ice::InstRet *ret = Ice::InstRet::create(::function);
	::basicBlock->appendInst(ret);
}

void Nucleus::createRet(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();

	// Code generated after this point is unreachable, so any variables
	// being read can safely return an undefined value. We have to avoid
	// materializing variables after the terminator ret instruction.
	Variable::killUnmaterialized();

	Ice::InstRet *ret = Ice::InstRet::create(::function, v);
	::basicBlock->appendInst(ret);
}

void Nucleus::createBr(BasicBlock *dest)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	auto br = Ice::InstBr::create(::function, dest);
	::basicBlock->appendInst(br);
}

void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	auto br = Ice::InstBr::create(::function, cond, ifTrue, ifFalse);
	::basicBlock->appendInst(br);
}

static bool isCommutative(Ice::InstArithmetic::OpKind op)
{
	switch(op)
	{
		case Ice::InstArithmetic::Add:
		case Ice::InstArithmetic::Fadd:
		case Ice::InstArithmetic::Mul:
		case Ice::InstArithmetic::Fmul:
		case Ice::InstArithmetic::And:
		case Ice::InstArithmetic::Or:
		case Ice::InstArithmetic::Xor:
			return true;
		default:
			return false;
	}
}

static Value *createArithmetic(Ice::InstArithmetic::OpKind op, Value *lhs, Value *rhs)
{
	ASSERT(lhs->getType() == rhs->getType() || llvm::isa<Ice::Constant>(rhs));

	bool swapOperands = llvm::isa<Ice::Constant>(lhs) && isCommutative(op);

	Ice::Variable *result = ::function->makeVariable(lhs->getType());
	Ice::InstArithmetic *arithmetic = Ice::InstArithmetic::create(::function, op, result, swapOperands ? rhs : lhs, swapOperands ? lhs : rhs);
	::basicBlock->appendInst(arithmetic);

	return V(result);
}

Value *Nucleus::createAdd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Add, lhs, rhs);
}

Value *Nucleus::createSub(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Sub, lhs, rhs);
}

Value *Nucleus::createMul(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Mul, lhs, rhs);
}

Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Udiv, lhs, rhs);
}

Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Sdiv, lhs, rhs);
}

Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Fadd, lhs, rhs);
}

Value *Nucleus::createFSub(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Fsub, lhs, rhs);
}

Value *Nucleus::createFMul(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Fmul, lhs, rhs);
}

Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Fdiv, lhs, rhs);
}

Value *Nucleus::createURem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Urem, lhs, rhs);
}

Value *Nucleus::createSRem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Srem, lhs, rhs);
}

Value *Nucleus::createFRem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// TODO(b/148139679) Fix Subzero generating invalid code for FRem on vector types
	// createArithmetic(Ice::InstArithmetic::Frem, lhs, rhs);
	UNIMPLEMENTED("b/148139679 Nucleus::createFRem");
	return nullptr;
}

RValue<Float4> operator%(RValue<Float4> lhs, RValue<Float4> rhs)
{
	return emulated::FRem(lhs, rhs);
}

Value *Nucleus::createShl(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Shl, lhs, rhs);
}

Value *Nucleus::createLShr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Lshr, lhs, rhs);
}

Value *Nucleus::createAShr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Ashr, lhs, rhs);
}

Value *Nucleus::createAnd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::And, lhs, rhs);
}

Value *Nucleus::createOr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Or, lhs, rhs);
}

Value *Nucleus::createXor(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createArithmetic(Ice::InstArithmetic::Xor, lhs, rhs);
}

Value *Nucleus::createNeg(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createSub(createNullValue(T(v->getType())), v);
}

Value *Nucleus::createFNeg(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	double c[4] = { -0.0, -0.0, -0.0, -0.0 };
	Value *negativeZero = Ice::isVectorType(v->getType()) ? createConstantVector(c, T(v->getType())) : V(::context->getConstantFloat(-0.0f));

	return createFSub(negativeZero, v);
}

Value *Nucleus::createNot(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(Ice::isScalarIntegerType(v->getType()))
	{
		return createXor(v, V(::context->getConstantInt(v->getType(), -1)));
	}
	else  // Vector
	{
		int64_t c[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
		return createXor(v, createConstantVector(c, T(v->getType())));
	}
}

static void validateAtomicAndMemoryOrderArgs(bool atomic, std::memory_order memoryOrder)
{
#if defined(__i386__) || defined(__x86_64__)
	// We're good, atomics and strictest memory order (except seq_cst) are guaranteed.
	// Note that sequential memory ordering could be guaranteed by using x86's LOCK prefix.
	// Note also that relaxed memory order could be implemented using MOVNTPS and friends.
#else
	if(atomic)
	{
		UNIMPLEMENTED("b/150475088 Atomic load/store not implemented for current platform");
	}
	if(memoryOrder != std::memory_order_relaxed)
	{
		UNIMPLEMENTED("b/150475088 Memory order other than memory_order_relaxed not implemented for current platform");
	}
#endif

	// Vulkan doesn't allow sequential memory order
	ASSERT(memoryOrder != std::memory_order_seq_cst);
}

Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int align, bool atomic, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	validateAtomicAndMemoryOrderArgs(atomic, memoryOrder);

	int valueType = (int)reinterpret_cast<intptr_t>(type);
	Ice::Variable *result = nullptr;

	if((valueType & EmulatedBits) && (align != 0))  // Narrow vector not stored on stack.
	{
		if(emulateIntrinsics)
		{
			if(typeSize(type) == 4)
			{
				auto pointer = RValue<Pointer<Byte>>(ptr);
				Int x = *Pointer<Int>(pointer);

				Int4 vector;
				vector = Insert(vector, x, 0);

				result = ::function->makeVariable(T(type));
				auto bitcast = Ice::InstCast::create(::function, Ice::InstCast::Bitcast, result, vector.loadValue());
				::basicBlock->appendInst(bitcast);
			}
			else if(typeSize(type) == 8)
			{
				ASSERT_MSG(!atomic, "Emulated 64-bit loads are not atomic");
				auto pointer = RValue<Pointer<Byte>>(ptr);
				Int x = *Pointer<Int>(pointer);
				Int y = *Pointer<Int>(pointer + 4);

				Int4 vector;
				vector = Insert(vector, x, 0);
				vector = Insert(vector, y, 1);

				result = ::function->makeVariable(T(type));
				auto bitcast = Ice::InstCast::create(::function, Ice::InstCast::Bitcast, result, vector.loadValue());
				::basicBlock->appendInst(bitcast);
			}
			else
				UNREACHABLE("typeSize(type): %d", int(typeSize(type)));
		}
		else
		{
			const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::LoadSubVector, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
			auto target = ::context->getConstantUndef(Ice::IceType_i32);
			result = ::function->makeVariable(T(type));
			auto load = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
			load->addArg(ptr);
			load->addArg(::context->getConstantInt32(typeSize(type)));
			::basicBlock->appendInst(load);
		}
	}
	else
	{
		result = sz::createLoad(::function, ::basicBlock, V(ptr), T(type), align);
	}

	ASSERT(result);
	return V(result);
}

Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int align, bool atomic, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	validateAtomicAndMemoryOrderArgs(atomic, memoryOrder);

#if __has_feature(memory_sanitizer)
	// Mark all (non-stack) memory writes as initialized by calling __msan_unpoison
	if(align != 0)
	{
		auto call = Ice::InstCall::create(::function, 2, nullptr, ::context->getConstantInt64(reinterpret_cast<intptr_t>(__msan_unpoison)), false);
		call->addArg(ptr);
		call->addArg(::context->getConstantInt64(typeSize(type)));
		::basicBlock->appendInst(call);
	}
#endif

	int valueType = (int)reinterpret_cast<intptr_t>(type);

	if((valueType & EmulatedBits) && (align != 0))  // Narrow vector not stored on stack.
	{
		if(emulateIntrinsics)
		{
			if(typeSize(type) == 4)
			{
				Ice::Variable *vector = ::function->makeVariable(Ice::IceType_v4i32);
				auto bitcast = Ice::InstCast::create(::function, Ice::InstCast::Bitcast, vector, value);
				::basicBlock->appendInst(bitcast);

				RValue<Int4> v(V(vector));

				auto pointer = RValue<Pointer<Byte>>(ptr);
				Int x = Extract(v, 0);
				*Pointer<Int>(pointer) = x;
			}
			else if(typeSize(type) == 8)
			{
				ASSERT_MSG(!atomic, "Emulated 64-bit stores are not atomic");
				Ice::Variable *vector = ::function->makeVariable(Ice::IceType_v4i32);
				auto bitcast = Ice::InstCast::create(::function, Ice::InstCast::Bitcast, vector, value);
				::basicBlock->appendInst(bitcast);

				RValue<Int4> v(V(vector));

				auto pointer = RValue<Pointer<Byte>>(ptr);
				Int x = Extract(v, 0);
				*Pointer<Int>(pointer) = x;
				Int y = Extract(v, 1);
				*Pointer<Int>(pointer + 4) = y;
			}
			else
				UNREACHABLE("typeSize(type): %d", int(typeSize(type)));
		}
		else
		{
			const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::StoreSubVector, Ice::Intrinsics::SideEffects_T, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_T };
			auto target = ::context->getConstantUndef(Ice::IceType_i32);
			auto store = Ice::InstIntrinsicCall::create(::function, 3, nullptr, target, intrinsic);
			store->addArg(value);
			store->addArg(ptr);
			store->addArg(::context->getConstantInt32(typeSize(type)));
			::basicBlock->appendInst(store);
		}
	}
	else
	{
		ASSERT(value->getType() == T(type));

		auto store = Ice::InstStore::create(::function, V(value), V(ptr), align);
		::basicBlock->appendInst(store);
	}

	return value;
}

Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index, bool unsignedIndex)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(index->getType() == Ice::IceType_i32);

	if(auto *constant = llvm::dyn_cast<Ice::ConstantInteger32>(index))
	{
		int32_t offset = constant->getValue() * (int)typeSize(type);

		if(offset == 0)
		{
			return ptr;
		}

		return createAdd(ptr, createConstantInt(offset));
	}

	if(!Ice::isByteSizedType(T(type)))
	{
		index = createMul(index, createConstantInt((int)typeSize(type)));
	}

	if(sizeof(void *) == 8)
	{
		if(unsignedIndex)
		{
			index = createZExt(index, T(Ice::IceType_i64));
		}
		else
		{
			index = createSExt(index, T(Ice::IceType_i64));
		}
	}

	return createAdd(ptr, index);
}

static Value *createAtomicRMW(Ice::Intrinsics::AtomicRMWOperation rmwOp, Value *ptr, Value *value, std::memory_order memoryOrder)
{
	Ice::Variable *result = ::function->makeVariable(value->getType());

	const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AtomicRMW, Ice::Intrinsics::SideEffects_T, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_T };
	auto target = ::context->getConstantUndef(Ice::IceType_i32);
	auto inst = Ice::InstIntrinsicCall::create(::function, 0, result, target, intrinsic);
	auto op = ::context->getConstantInt32(rmwOp);
	auto order = ::context->getConstantInt32(stdToIceMemoryOrder(memoryOrder));
	inst->addArg(op);
	inst->addArg(ptr);
	inst->addArg(value);
	inst->addArg(order);
	::basicBlock->appendInst(inst);

	return V(result);
}

Value *Nucleus::createAtomicAdd(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicAdd, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicSub(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicSub, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicAnd(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicAnd, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicOr(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicOr, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicXor(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicXor, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicExchange(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createAtomicRMW(Ice::Intrinsics::AtomicExchange, ptr, value, memoryOrder);
}

Value *Nucleus::createAtomicCompareExchange(Value *ptr, Value *value, Value *compare, std::memory_order memoryOrderEqual, std::memory_order memoryOrderUnequal)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *result = ::function->makeVariable(value->getType());

	const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AtomicCmpxchg, Ice::Intrinsics::SideEffects_T, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_T };
	auto target = ::context->getConstantUndef(Ice::IceType_i32);
	auto inst = Ice::InstIntrinsicCall::create(::function, 0, result, target, intrinsic);
	auto orderEq = ::context->getConstantInt32(stdToIceMemoryOrder(memoryOrderEqual));
	auto orderNeq = ::context->getConstantInt32(stdToIceMemoryOrder(memoryOrderUnequal));
	inst->addArg(ptr);
	inst->addArg(compare);
	inst->addArg(value);
	inst->addArg(orderEq);
	inst->addArg(orderNeq);
	::basicBlock->appendInst(inst);

	return V(result);
}

static Value *createCast(Ice::InstCast::OpKind op, Value *v, Type *destType)
{
	if(v->getType() == T(destType))
	{
		return v;
	}

	Ice::Variable *result = ::function->makeVariable(T(destType));
	Ice::InstCast *cast = Ice::InstCast::create(::function, op, result, v);
	::basicBlock->appendInst(cast);

	return V(result);
}

Value *Nucleus::createTrunc(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Trunc, v, destType);
}

Value *Nucleus::createZExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Zext, v, destType);
}

Value *Nucleus::createSExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Sext, v, destType);
}

Value *Nucleus::createFPToUI(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Fptoui, v, destType);
}

Value *Nucleus::createFPToSI(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Fptosi, v, destType);
}

Value *Nucleus::createSIToFP(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Sitofp, v, destType);
}

Value *Nucleus::createFPTrunc(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Fptrunc, v, destType);
}

Value *Nucleus::createFPExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createCast(Ice::InstCast::Fpext, v, destType);
}

Value *Nucleus::createBitCast(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// Bitcasts must be between types of the same logical size. But with emulated narrow vectors we need
	// support for casting between scalars and wide vectors. For platforms where this is not supported,
	// emulate them by writing to the stack and reading back as the destination type.
	if(emulateMismatchedBitCast)
	{
		if(!Ice::isVectorType(v->getType()) && Ice::isVectorType(T(destType)))
		{
			Value *address = allocateStackVariable(destType);
			createStore(v, address, T(v->getType()));
			return createLoad(address, destType);
		}
		else if(Ice::isVectorType(v->getType()) && !Ice::isVectorType(T(destType)))
		{
			Value *address = allocateStackVariable(T(v->getType()));
			createStore(v, address, T(v->getType()));
			return createLoad(address, destType);
		}
	}

	return createCast(Ice::InstCast::Bitcast, v, destType);
}

static Value *createIntCompare(Ice::InstIcmp::ICond condition, Value *lhs, Value *rhs)
{
	ASSERT(lhs->getType() == rhs->getType());

	auto result = ::function->makeVariable(Ice::isScalarIntegerType(lhs->getType()) ? Ice::IceType_i1 : lhs->getType());
	auto cmp = Ice::InstIcmp::create(::function, condition, result, lhs, rhs);
	::basicBlock->appendInst(cmp);

	return V(result);
}

Value *Nucleus::createPtrEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Eq, lhs, rhs);
}

Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Eq, lhs, rhs);
}

Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Ne, lhs, rhs);
}

Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Ugt, lhs, rhs);
}

Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Uge, lhs, rhs);
}

Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Ult, lhs, rhs);
}

Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Ule, lhs, rhs);
}

Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Sgt, lhs, rhs);
}

Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Sge, lhs, rhs);
}

Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Slt, lhs, rhs);
}

Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createIntCompare(Ice::InstIcmp::Sle, lhs, rhs);
}

static Value *createFloatCompare(Ice::InstFcmp::FCond condition, Value *lhs, Value *rhs)
{
	ASSERT(lhs->getType() == rhs->getType());
	ASSERT(Ice::isScalarFloatingType(lhs->getType()) || lhs->getType() == Ice::IceType_v4f32);

	auto result = ::function->makeVariable(Ice::isScalarFloatingType(lhs->getType()) ? Ice::IceType_i1 : Ice::IceType_v4i32);
	auto cmp = Ice::InstFcmp::create(::function, condition, result, lhs, rhs);
	::basicBlock->appendInst(cmp);

	return V(result);
}

Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Oeq, lhs, rhs);
}

Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ogt, lhs, rhs);
}

Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Oge, lhs, rhs);
}

Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Olt, lhs, rhs);
}

Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ole, lhs, rhs);
}

Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::One, lhs, rhs);
}

Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ord, lhs, rhs);
}

Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Uno, lhs, rhs);
}

Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ueq, lhs, rhs);
}

Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ugt, lhs, rhs);
}

Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Uge, lhs, rhs);
}

Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ult, lhs, rhs);
}

Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Ule, lhs, rhs);
}

Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createFloatCompare(Ice::InstFcmp::Une, lhs, rhs);
}

Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto result = ::function->makeVariable(T(type));
	auto extract = Ice::InstExtractElement::create(::function, result, V(vector), ::context->getConstantInt32(index));
	::basicBlock->appendInst(extract);

	return V(result);
}

Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto result = ::function->makeVariable(vector->getType());
	auto insert = Ice::InstInsertElement::create(::function, result, vector, element, ::context->getConstantInt32(index));
	::basicBlock->appendInst(insert);

	return V(result);
}

Value *Nucleus::createShuffleVector(Value *V1, Value *V2, const int *select)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(V1->getType() == V2->getType());

	int size = Ice::typeNumElements(V1->getType());
	auto result = ::function->makeVariable(V1->getType());
	auto shuffle = Ice::InstShuffleVector::create(::function, result, V1, V2);

	for(int i = 0; i < size; i++)
	{
		shuffle->addIndex(llvm::cast<Ice::ConstantInteger32>(::context->getConstantInt32(select[i])));
	}

	::basicBlock->appendInst(shuffle);

	return V(result);
}

Value *Nucleus::createSelect(Value *C, Value *ifTrue, Value *ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(ifTrue->getType() == ifFalse->getType());

	auto result = ::function->makeVariable(ifTrue->getType());
	auto *select = Ice::InstSelect::create(::function, result, C, ifTrue, ifFalse);
	::basicBlock->appendInst(select);

	return V(result);
}

SwitchCases *Nucleus::createSwitch(Value *control, BasicBlock *defaultBranch, unsigned numCases)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto switchInst = Ice::InstSwitch::create(::function, numCases, control, defaultBranch);
	::basicBlock->appendInst(switchInst);

	return reinterpret_cast<SwitchCases *>(switchInst);
}

void Nucleus::addSwitchCase(SwitchCases *switchCases, int label, BasicBlock *branch)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	switchCases->addBranch(label, label, branch);
}

void Nucleus::createUnreachable()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::InstUnreachable *unreachable = Ice::InstUnreachable::create(::function);
	::basicBlock->appendInst(unreachable);
}

Type *Nucleus::getType(Value *value)
{
	return T(V(value)->getType());
}

Type *Nucleus::getContainedType(Type *vectorType)
{
	Ice::Type vecTy = T(vectorType);
	switch(vecTy)
	{
		case Ice::IceType_v4i1: return T(Ice::IceType_i1);
		case Ice::IceType_v8i1: return T(Ice::IceType_i1);
		case Ice::IceType_v16i1: return T(Ice::IceType_i1);
		case Ice::IceType_v16i8: return T(Ice::IceType_i8);
		case Ice::IceType_v8i16: return T(Ice::IceType_i16);
		case Ice::IceType_v4i32: return T(Ice::IceType_i32);
		case Ice::IceType_v4f32: return T(Ice::IceType_f32);
		default:
			ASSERT_MSG(false, "getContainedType: input type is not a vector type");
			return {};
	}
}

Type *Nucleus::getPointerType(Type *ElementType)
{
	return T(sz::getPointerType(T(ElementType)));
}

static constexpr Ice::Type getNaturalIntType()
{
	constexpr size_t intSize = sizeof(int);
	static_assert(intSize == 4 || intSize == 8, "");
	return intSize == 4 ? Ice::IceType_i32 : Ice::IceType_i64;
}

Type *Nucleus::getPrintfStorageType(Type *valueType)
{
	Ice::Type valueTy = T(valueType);
	switch(valueTy)
	{
		case Ice::IceType_i32:
			return T(getNaturalIntType());

		case Ice::IceType_f32:
			return T(Ice::IceType_f64);

		default:
			UNIMPLEMENTED_NO_BUG("getPrintfStorageType: add more cases as needed");
			return {};
	}
}

Value *Nucleus::createNullValue(Type *Ty)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(Ice::isVectorType(T(Ty)))
	{
		ASSERT(Ice::typeNumElements(T(Ty)) <= 16);
		int64_t c[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		return createConstantVector(c, Ty);
	}
	else
	{
		return V(::context->getConstantZero(T(Ty)));
	}
}

Value *Nucleus::createConstantLong(int64_t i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt64(i));
}

Value *Nucleus::createConstantInt(int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt32(i));
}

Value *Nucleus::createConstantInt(unsigned int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt32(i));
}

Value *Nucleus::createConstantBool(bool b)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt1(b));
}

Value *Nucleus::createConstantByte(signed char i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt8(i));
}

Value *Nucleus::createConstantByte(unsigned char i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt8(i));
}

Value *Nucleus::createConstantShort(short i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt16(i));
}

Value *Nucleus::createConstantShort(unsigned short i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantInt16(i));
}

Value *Nucleus::createConstantFloat(float x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(::context->getConstantFloat(x));
}

Value *Nucleus::createNullPointer(Type *Ty)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return createNullValue(T(sizeof(void *) == 8 ? Ice::IceType_i64 : Ice::IceType_i32));
}

static Ice::Constant *IceConstantData(void const *data, size_t size, size_t alignment = 1)
{
	return sz::getConstantPointer(::context, ::routine->addConstantData(data, size, alignment));
}

Value *Nucleus::createConstantVector(const int64_t *constants, Type *type)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	const int vectorSize = 16;
	ASSERT(Ice::typeWidthInBytes(T(type)) == vectorSize);
	const int alignment = vectorSize;

	const int64_t *i = constants;
	const double *f = reinterpret_cast<const double *>(constants);

	// TODO(b/148082873): Fix global variable constants when generating multiple functions
	Ice::Constant *ptr = nullptr;

	switch((int)reinterpret_cast<intptr_t>(type))
	{
		case Ice::IceType_v4i32:
		case Ice::IceType_v4i1:
		{
			const int initializer[4] = { (int)i[0], (int)i[1], (int)i[2], (int)i[3] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Ice::IceType_v4f32:
		{
			const float initializer[4] = { (float)f[0], (float)f[1], (float)f[2], (float)f[3] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Ice::IceType_v8i16:
		case Ice::IceType_v8i1:
		{
			const short initializer[8] = { (short)i[0], (short)i[1], (short)i[2], (short)i[3], (short)i[4], (short)i[5], (short)i[6], (short)i[7] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Ice::IceType_v16i8:
		case Ice::IceType_v16i1:
		{
			const char initializer[16] = { (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[4], (char)i[5], (char)i[6], (char)i[7], (char)i[8], (char)i[9], (char)i[10], (char)i[11], (char)i[12], (char)i[13], (char)i[14], (char)i[15] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Type_v2i32:
		{
			const int initializer[4] = { (int)i[0], (int)i[1], (int)i[0], (int)i[1] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Type_v2f32:
		{
			const float initializer[4] = { (float)f[0], (float)f[1], (float)f[0], (float)f[1] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Type_v4i16:
		{
			const short initializer[8] = { (short)i[0], (short)i[1], (short)i[2], (short)i[3], (short)i[0], (short)i[1], (short)i[2], (short)i[3] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Type_v8i8:
		{
			const char initializer[16] = { (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[4], (char)i[5], (char)i[6], (char)i[7], (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[4], (char)i[5], (char)i[6], (char)i[7] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		case Type_v4i8:
		{
			const char initializer[16] = { (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[0], (char)i[1], (char)i[2], (char)i[3], (char)i[0], (char)i[1], (char)i[2], (char)i[3] };
			static_assert(sizeof(initializer) == vectorSize, "!");
			ptr = IceConstantData(initializer, vectorSize, alignment);
		}
		break;
		default:
			UNREACHABLE("Unknown constant vector type: %d", (int)reinterpret_cast<intptr_t>(type));
	}

	ASSERT(ptr);

	Ice::Variable *result = sz::createLoad(::function, ::basicBlock, ptr, T(type), alignment);
	return V(result);
}

Value *Nucleus::createConstantVector(const double *constants, Type *type)
{
	return createConstantVector((const int64_t *)constants, type);
}

Value *Nucleus::createConstantString(const char *v)
{
	// NOTE: Do not call RR_DEBUG_INFO_UPDATE_LOC() here to avoid recursion when called from rr::Printv
	return V(IceConstantData(v, strlen(v) + 1));
}

Type *Void::getType()
{
	return T(Ice::IceType_void);
}

Type *Bool::getType()
{
	return T(Ice::IceType_i1);
}

Type *Byte::getType()
{
	return T(Ice::IceType_i8);
}

Type *SByte::getType()
{
	return T(Ice::IceType_i8);
}

Type *Short::getType()
{
	return T(Ice::IceType_i16);
}

Type *UShort::getType()
{
	return T(Ice::IceType_i16);
}

Type *Byte4::getType()
{
	return T(Type_v4i8);
}

Type *SByte4::getType()
{
	return T(Type_v4i8);
}

namespace {
RValue<Byte> SaturateUnsigned(RValue<Short> x)
{
	return Byte(IfThenElse(Int(x) > 0xFF, Int(0xFF), IfThenElse(Int(x) < 0, Int(0), Int(x))));
}

RValue<Byte> Extract(RValue<Byte8> val, int i)
{
	return RValue<Byte>(Nucleus::createExtractElement(val.value, Byte::getType(), i));
}

RValue<Byte8> Insert(RValue<Byte8> val, RValue<Byte> element, int i)
{
	return RValue<Byte8>(Nucleus::createInsertElement(val.value, element.value, i));
}
}  // namespace

RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Byte8 result;
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 0)) + Int(Extract(y, 0)))), 0);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 1)) + Int(Extract(y, 1)))), 1);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 2)) + Int(Extract(y, 2)))), 2);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 3)) + Int(Extract(y, 3)))), 3);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 4)) + Int(Extract(y, 4)))), 4);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 5)) + Int(Extract(y, 5)))), 5);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 6)) + Int(Extract(y, 6)))), 6);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 7)) + Int(Extract(y, 7)))), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AddSaturateUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto paddusb = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		paddusb->addArg(x.value);
		paddusb->addArg(y.value);
		::basicBlock->appendInst(paddusb);

		return RValue<Byte8>(V(result));
	}
}

RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Byte8 result;
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 0)) - Int(Extract(y, 0)))), 0);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 1)) - Int(Extract(y, 1)))), 1);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 2)) - Int(Extract(y, 2)))), 2);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 3)) - Int(Extract(y, 3)))), 3);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 4)) - Int(Extract(y, 4)))), 4);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 5)) - Int(Extract(y, 5)))), 5);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 6)) - Int(Extract(y, 6)))), 6);
		result = Insert(result, SaturateUnsigned(Short(Int(Extract(x, 7)) - Int(Extract(y, 7)))), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SubtractSaturateUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto psubusw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		psubusw->addArg(x.value);
		psubusw->addArg(y.value);
		::basicBlock->appendInst(psubusw);

		return RValue<Byte8>(V(result));
	}
}

RValue<SByte> Extract(RValue<SByte8> val, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<SByte>(Nucleus::createExtractElement(val.value, SByte::getType(), i));
}

RValue<SByte8> Insert(RValue<SByte8> val, RValue<SByte> element, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<SByte8>(Nucleus::createInsertElement(val.value, element.value, i));
}

RValue<SByte8> operator>>(RValue<SByte8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		SByte8 result;
		result = Insert(result, Extract(lhs, 0) >> SByte(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> SByte(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> SByte(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> SByte(rhs), 3);
		result = Insert(result, Extract(lhs, 4) >> SByte(rhs), 4);
		result = Insert(result, Extract(lhs, 5) >> SByte(rhs), 5);
		result = Insert(result, Extract(lhs, 6) >> SByte(rhs), 6);
		result = Insert(result, Extract(lhs, 7) >> SByte(rhs), 7);

		return result;
	}
	else
	{
#if defined(__i386__) || defined(__x86_64__)
		// SSE2 doesn't support byte vector shifts, so shift as shorts and recombine.
		RValue<Short4> hi = (As<Short4>(lhs) >> rhs) & Short4(0xFF00u);
		RValue<Short4> lo = As<Short4>(As<UShort4>((As<Short4>(lhs) << 8) >> rhs) >> 8);

		return As<SByte8>(hi | lo);
#else
		return RValue<SByte8>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
#endif
	}
}

RValue<Int> SignMask(RValue<Byte8> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		Byte8 xx = As<Byte8>(As<SByte8>(x) >> 7) & Byte8(0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80);
		return Int(Extract(xx, 0)) | Int(Extract(xx, 1)) | Int(Extract(xx, 2)) | Int(Extract(xx, 3)) | Int(Extract(xx, 4)) | Int(Extract(xx, 5)) | Int(Extract(xx, 6)) | Int(Extract(xx, 7));
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SignMask, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto movmsk = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		movmsk->addArg(x.value);
		::basicBlock->appendInst(movmsk);

		return RValue<Int>(V(result)) & 0xFF;
	}
}

//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y)
//	{
//		return RValue<Byte8>(createIntCompare(Ice::InstIcmp::Ugt, x.value, y.value));
//	}

RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Byte8>(Nucleus::createICmpEQ(x.value, y.value));
}

Type *Byte8::getType()
{
	return T(Type_v8i8);
}

//	RValue<SByte8> operator<<(RValue<SByte8> lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
//	}

//	RValue<SByte8> operator>>(RValue<SByte8> lhs, unsigned char rhs)
//	{
//		return RValue<SByte8>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
//	}

RValue<SByte> SaturateSigned(RValue<Short> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return SByte(IfThenElse(Int(x) > 0x7F, Int(0x7F), IfThenElse(Int(x) < -0x80, Int(0x80), Int(x))));
}

RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		SByte8 result;
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 0)) + Int(Extract(y, 0)))), 0);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 1)) + Int(Extract(y, 1)))), 1);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 2)) + Int(Extract(y, 2)))), 2);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 3)) + Int(Extract(y, 3)))), 3);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 4)) + Int(Extract(y, 4)))), 4);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 5)) + Int(Extract(y, 5)))), 5);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 6)) + Int(Extract(y, 6)))), 6);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 7)) + Int(Extract(y, 7)))), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AddSaturateSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto paddsb = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		paddsb->addArg(x.value);
		paddsb->addArg(y.value);
		::basicBlock->appendInst(paddsb);

		return RValue<SByte8>(V(result));
	}
}

RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		SByte8 result;
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 0)) - Int(Extract(y, 0)))), 0);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 1)) - Int(Extract(y, 1)))), 1);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 2)) - Int(Extract(y, 2)))), 2);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 3)) - Int(Extract(y, 3)))), 3);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 4)) - Int(Extract(y, 4)))), 4);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 5)) - Int(Extract(y, 5)))), 5);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 6)) - Int(Extract(y, 6)))), 6);
		result = Insert(result, SaturateSigned(Short(Int(Extract(x, 7)) - Int(Extract(y, 7)))), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SubtractSaturateSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto psubsb = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		psubsb->addArg(x.value);
		psubsb->addArg(y.value);
		::basicBlock->appendInst(psubsb);

		return RValue<SByte8>(V(result));
	}
}

RValue<Int> SignMask(RValue<SByte8> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		SByte8 xx = (x >> 7) & SByte8(0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80);
		return Int(Extract(xx, 0)) | Int(Extract(xx, 1)) | Int(Extract(xx, 2)) | Int(Extract(xx, 3)) | Int(Extract(xx, 4)) | Int(Extract(xx, 5)) | Int(Extract(xx, 6)) | Int(Extract(xx, 7));
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SignMask, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto movmsk = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		movmsk->addArg(x.value);
		::basicBlock->appendInst(movmsk);

		return RValue<Int>(V(result)) & 0xFF;
	}
}

RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Byte8>(createIntCompare(Ice::InstIcmp::Sgt, x.value, y.value));
}

RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Byte8>(Nucleus::createICmpEQ(x.value, y.value));
}

Type *SByte8::getType()
{
	return T(Type_v8i8);
}

Type *Byte16::getType()
{
	return T(Ice::IceType_v16i8);
}

Type *SByte16::getType()
{
	return T(Ice::IceType_v16i8);
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
	int select[8] = { 0, 2, 4, 6, 0, 2, 4, 6 };
	Value *short8 = Nucleus::createBitCast(cast.value, Short8::getType());
	Value *packed = Nucleus::createShuffleVector(short8, short8, select);

	Value *int2 = RValue<Int2>(Int2(As<Int4>(packed))).value;
	Value *short4 = Nucleus::createBitCast(int2, Short4::getType());

	storeValue(short4);
}

//	Short4::Short4(RValue<Float> cast)
//	{
//	}

Short4::Short4(RValue<Float4> cast)
{
	// TODO(b/150791192): Generalize and optimize
	auto smin = std::numeric_limits<short>::min();
	auto smax = std::numeric_limits<short>::max();
	*this = Short4(Int4(Max(Min(cast, Float4(smax)), Float4(smin))));
}

RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short4 result;
		result = Insert(result, Extract(lhs, 0) << Short(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << Short(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << Short(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << Short(rhs), 3);

		return result;
	}
	else
	{
		return RValue<Short4>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short4 result;
		result = Insert(result, Extract(lhs, 0) >> Short(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> Short(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> Short(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> Short(rhs), 3);

		return result;
	}
	else
	{
		return RValue<Short4>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v8i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Sle, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<Short4>(V(result));
}

RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v8i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Sgt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<Short4>(V(result));
}

RValue<Short> SaturateSigned(RValue<Int> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Short(IfThenElse(x > 0x7FFF, Int(0x7FFF), IfThenElse(x < -0x8000, Int(0x8000), x)));
}

RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short4 result;
		result = Insert(result, SaturateSigned(Int(Extract(x, 0)) + Int(Extract(y, 0))), 0);
		result = Insert(result, SaturateSigned(Int(Extract(x, 1)) + Int(Extract(y, 1))), 1);
		result = Insert(result, SaturateSigned(Int(Extract(x, 2)) + Int(Extract(y, 2))), 2);
		result = Insert(result, SaturateSigned(Int(Extract(x, 3)) + Int(Extract(y, 3))), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AddSaturateSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto paddsw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		paddsw->addArg(x.value);
		paddsw->addArg(y.value);
		::basicBlock->appendInst(paddsw);

		return RValue<Short4>(V(result));
	}
}

RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short4 result;
		result = Insert(result, SaturateSigned(Int(Extract(x, 0)) - Int(Extract(y, 0))), 0);
		result = Insert(result, SaturateSigned(Int(Extract(x, 1)) - Int(Extract(y, 1))), 1);
		result = Insert(result, SaturateSigned(Int(Extract(x, 2)) - Int(Extract(y, 2))), 2);
		result = Insert(result, SaturateSigned(Int(Extract(x, 3)) - Int(Extract(y, 3))), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SubtractSaturateSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto psubsw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		psubsw->addArg(x.value);
		psubsw->addArg(y.value);
		::basicBlock->appendInst(psubsw);

		return RValue<Short4>(V(result));
	}
}

RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short4 result;
		result = Insert(result, Short((Int(Extract(x, 0)) * Int(Extract(y, 0))) >> 16), 0);
		result = Insert(result, Short((Int(Extract(x, 1)) * Int(Extract(y, 1))) >> 16), 1);
		result = Insert(result, Short((Int(Extract(x, 2)) * Int(Extract(y, 2))) >> 16), 2);
		result = Insert(result, Short((Int(Extract(x, 3)) * Int(Extract(y, 3))) >> 16), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::MultiplyHighSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pmulhw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pmulhw->addArg(x.value);
		pmulhw->addArg(y.value);
		::basicBlock->appendInst(pmulhw);

		return RValue<Short4>(V(result));
	}
}

RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Int2 result;
		result = Insert(result, Int(Extract(x, 0)) * Int(Extract(y, 0)) + Int(Extract(x, 1)) * Int(Extract(y, 1)), 0);
		result = Insert(result, Int(Extract(x, 2)) * Int(Extract(y, 2)) + Int(Extract(x, 3)) * Int(Extract(y, 3)), 1);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::MultiplyAddPairs, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pmaddwd = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pmaddwd->addArg(x.value);
		pmaddwd->addArg(y.value);
		::basicBlock->appendInst(pmaddwd);

		return As<Int2>(V(result));
	}
}

RValue<SByte8> PackSigned(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		SByte8 result;
		result = Insert(result, SaturateSigned(Extract(x, 0)), 0);
		result = Insert(result, SaturateSigned(Extract(x, 1)), 1);
		result = Insert(result, SaturateSigned(Extract(x, 2)), 2);
		result = Insert(result, SaturateSigned(Extract(x, 3)), 3);
		result = Insert(result, SaturateSigned(Extract(y, 0)), 4);
		result = Insert(result, SaturateSigned(Extract(y, 1)), 5);
		result = Insert(result, SaturateSigned(Extract(y, 2)), 6);
		result = Insert(result, SaturateSigned(Extract(y, 3)), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::VectorPackSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pack = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pack->addArg(x.value);
		pack->addArg(y.value);
		::basicBlock->appendInst(pack);

		return As<SByte8>(Swizzle(As<Int4>(V(result)), 0x0202));
	}
}

RValue<Byte8> PackUnsigned(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Byte8 result;
		result = Insert(result, SaturateUnsigned(Extract(x, 0)), 0);
		result = Insert(result, SaturateUnsigned(Extract(x, 1)), 1);
		result = Insert(result, SaturateUnsigned(Extract(x, 2)), 2);
		result = Insert(result, SaturateUnsigned(Extract(x, 3)), 3);
		result = Insert(result, SaturateUnsigned(Extract(y, 0)), 4);
		result = Insert(result, SaturateUnsigned(Extract(y, 1)), 5);
		result = Insert(result, SaturateUnsigned(Extract(y, 2)), 6);
		result = Insert(result, SaturateUnsigned(Extract(y, 3)), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v16i8);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::VectorPackUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pack = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pack->addArg(x.value);
		pack->addArg(y.value);
		::basicBlock->appendInst(pack);

		return As<Byte8>(Swizzle(As<Int4>(V(result)), 0x0202));
	}
}

RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Short4>(createIntCompare(Ice::InstIcmp::Sgt, x.value, y.value));
}

RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Short4>(Nucleus::createICmpEQ(x.value, y.value));
}

Type *Short4::getType()
{
	return T(Type_v4i16);
}

UShort4::UShort4(RValue<Float4> cast, bool saturate)
{
	if(saturate)
	{
		if(CPUID::SSE4_1)
		{
			// x86 produces 0x80000000 on 32-bit integer overflow/underflow.
			// PackUnsigned takes care of 0x0000 saturation.
			Int4 int4(Min(cast, Float4(0xFFFF)));
			*this = As<UShort4>(PackUnsigned(int4, int4));
		}
		else if(CPUID::ARM)
		{
			// ARM saturates the 32-bit integer result on overflow/undeflow.
			Int4 int4(cast);
			*this = As<UShort4>(PackUnsigned(int4, int4));
		}
		else
		{
			*this = Short4(Int4(Max(Min(cast, Float4(0xFFFF)), Float4(0x0000))));
		}
	}
	else
	{
		*this = Short4(Int4(cast));
	}
}

RValue<UShort> Extract(RValue<UShort4> val, int i)
{
	return RValue<UShort>(Nucleus::createExtractElement(val.value, UShort::getType(), i));
}

RValue<UShort4> Insert(RValue<UShort4> val, RValue<UShort> element, int i)
{
	return RValue<UShort4>(Nucleus::createInsertElement(val.value, element.value, i));
}

RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)

	{
		UShort4 result;
		result = Insert(result, Extract(lhs, 0) << UShort(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << UShort(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << UShort(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << UShort(rhs), 3);

		return result;
	}
	else
	{
		return RValue<UShort4>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort4 result;
		result = Insert(result, Extract(lhs, 0) >> UShort(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> UShort(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> UShort(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> UShort(rhs), 3);

		return result;
	}
	else
	{
		return RValue<UShort4>(Nucleus::createLShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v8i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Ule, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<UShort4>(V(result));
}

RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y)
{
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v8i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Ugt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<UShort4>(V(result));
}

RValue<UShort> SaturateUnsigned(RValue<Int> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return UShort(IfThenElse(x > 0xFFFF, Int(0xFFFF), IfThenElse(x < 0, Int(0), x)));
}

RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort4 result;
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 0)) + Int(Extract(y, 0))), 0);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 1)) + Int(Extract(y, 1))), 1);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 2)) + Int(Extract(y, 2))), 2);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 3)) + Int(Extract(y, 3))), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AddSaturateUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto paddusw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		paddusw->addArg(x.value);
		paddusw->addArg(y.value);
		::basicBlock->appendInst(paddusw);

		return RValue<UShort4>(V(result));
	}
}

RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort4 result;
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 0)) - Int(Extract(y, 0))), 0);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 1)) - Int(Extract(y, 1))), 1);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 2)) - Int(Extract(y, 2))), 2);
		result = Insert(result, SaturateUnsigned(Int(Extract(x, 3)) - Int(Extract(y, 3))), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SubtractSaturateUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto psubusw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		psubusw->addArg(x.value);
		psubusw->addArg(y.value);
		::basicBlock->appendInst(psubusw);

		return RValue<UShort4>(V(result));
	}
}

RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort4 result;
		result = Insert(result, UShort((UInt(Extract(x, 0)) * UInt(Extract(y, 0))) >> 16), 0);
		result = Insert(result, UShort((UInt(Extract(x, 1)) * UInt(Extract(y, 1))) >> 16), 1);
		result = Insert(result, UShort((UInt(Extract(x, 2)) * UInt(Extract(y, 2))) >> 16), 2);
		result = Insert(result, UShort((UInt(Extract(x, 3)) * UInt(Extract(y, 3))) >> 16), 3);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::MultiplyHighUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pmulhuw = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pmulhuw->addArg(x.value);
		pmulhuw->addArg(y.value);
		::basicBlock->appendInst(pmulhuw);

		return RValue<UShort4>(V(result));
	}
}

RValue<Int4> MulHigh(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.

	// Scalarized implementation.
	Int4 result;
	result = Insert(result, Int((Long(Extract(x, 0)) * Long(Extract(y, 0))) >> Long(Int(32))), 0);
	result = Insert(result, Int((Long(Extract(x, 1)) * Long(Extract(y, 1))) >> Long(Int(32))), 1);
	result = Insert(result, Int((Long(Extract(x, 2)) * Long(Extract(y, 2))) >> Long(Int(32))), 2);
	result = Insert(result, Int((Long(Extract(x, 3)) * Long(Extract(y, 3))) >> Long(Int(32))), 3);

	return result;
}

RValue<UInt4> MulHigh(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.

	if(false)  // Partial product based implementation.
	{
		auto xh = x >> 16;
		auto yh = y >> 16;
		auto xl = x & UInt4(0x0000FFFF);
		auto yl = y & UInt4(0x0000FFFF);
		auto xlyh = xl * yh;
		auto xhyl = xh * yl;
		auto xlyhh = xlyh >> 16;
		auto xhylh = xhyl >> 16;
		auto xlyhl = xlyh & UInt4(0x0000FFFF);
		auto xhyll = xhyl & UInt4(0x0000FFFF);
		auto xlylh = (xl * yl) >> 16;
		auto oflow = (xlyhl + xhyll + xlylh) >> 16;

		return (xh * yh) + (xlyhh + xhylh) + oflow;
	}

	// Scalarized implementation.
	Int4 result;
	result = Insert(result, Int((Long(UInt(Extract(As<Int4>(x), 0))) * Long(UInt(Extract(As<Int4>(y), 0)))) >> Long(Int(32))), 0);
	result = Insert(result, Int((Long(UInt(Extract(As<Int4>(x), 1))) * Long(UInt(Extract(As<Int4>(y), 1)))) >> Long(Int(32))), 1);
	result = Insert(result, Int((Long(UInt(Extract(As<Int4>(x), 2))) * Long(UInt(Extract(As<Int4>(y), 2)))) >> Long(Int(32))), 2);
	result = Insert(result, Int((Long(UInt(Extract(As<Int4>(x), 3))) * Long(UInt(Extract(As<Int4>(y), 3)))) >> Long(Int(32))), 3);

	return As<UInt4>(result);
}

RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)");
	return UShort4(0);
}

Type *UShort4::getType()
{
	return T(Type_v4i16);
}

RValue<Short> Extract(RValue<Short8> val, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Short>(Nucleus::createExtractElement(val.value, Short::getType(), i));
}

RValue<Short8> Insert(RValue<Short8> val, RValue<Short> element, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Short8>(Nucleus::createInsertElement(val.value, element.value, i));
}

RValue<Short8> operator<<(RValue<Short8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short8 result;
		result = Insert(result, Extract(lhs, 0) << Short(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << Short(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << Short(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << Short(rhs), 3);
		result = Insert(result, Extract(lhs, 4) << Short(rhs), 4);
		result = Insert(result, Extract(lhs, 5) << Short(rhs), 5);
		result = Insert(result, Extract(lhs, 6) << Short(rhs), 6);
		result = Insert(result, Extract(lhs, 7) << Short(rhs), 7);

		return result;
	}
	else
	{
		return RValue<Short8>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short8 result;
		result = Insert(result, Extract(lhs, 0) >> Short(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> Short(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> Short(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> Short(rhs), 3);
		result = Insert(result, Extract(lhs, 4) >> Short(rhs), 4);
		result = Insert(result, Extract(lhs, 5) >> Short(rhs), 5);
		result = Insert(result, Extract(lhs, 6) >> Short(rhs), 6);
		result = Insert(result, Extract(lhs, 7) >> Short(rhs), 7);

		return result;
	}
	else
	{
		return RValue<Short8>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)");
	return Int4(0);
}

RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)");
	return Short8(0);
}

Type *Short8::getType()
{
	return T(Ice::IceType_v8i16);
}

RValue<UShort> Extract(RValue<UShort8> val, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UShort>(Nucleus::createExtractElement(val.value, UShort::getType(), i));
}

RValue<UShort8> Insert(RValue<UShort8> val, RValue<UShort> element, int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UShort8>(Nucleus::createInsertElement(val.value, element.value, i));
}

RValue<UShort8> operator<<(RValue<UShort8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort8 result;
		result = Insert(result, Extract(lhs, 0) << UShort(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << UShort(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << UShort(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << UShort(rhs), 3);
		result = Insert(result, Extract(lhs, 4) << UShort(rhs), 4);
		result = Insert(result, Extract(lhs, 5) << UShort(rhs), 5);
		result = Insert(result, Extract(lhs, 6) << UShort(rhs), 6);
		result = Insert(result, Extract(lhs, 7) << UShort(rhs), 7);

		return result;
	}
	else
	{
		return RValue<UShort8>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UShort8 result;
		result = Insert(result, Extract(lhs, 0) >> UShort(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> UShort(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> UShort(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> UShort(rhs), 3);
		result = Insert(result, Extract(lhs, 4) >> UShort(rhs), 4);
		result = Insert(result, Extract(lhs, 5) >> UShort(rhs), 5);
		result = Insert(result, Extract(lhs, 6) >> UShort(rhs), 6);
		result = Insert(result, Extract(lhs, 7) >> UShort(rhs), 7);

		return result;
	}
	else
	{
		return RValue<UShort8>(Nucleus::createLShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)");
	return UShort8(0);
}

Type *UShort8::getType()
{
	return T(Ice::IceType_v8i16);
}

RValue<Int> operator++(Int &val, int)  // Post-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<Int> res = val;
	val += 1;
	return res;
}

const Int &operator++(Int &val)  // Pre-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	val += 1;
	return val;
}

RValue<Int> operator--(Int &val, int)  // Post-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<Int> res = val;
	val -= 1;
	return res;
}

const Int &operator--(Int &val)  // Pre-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	val -= 1;
	return val;
}

RValue<Int> RoundInt(RValue<Float> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		// Push the fractional part off the mantissa. Accurate up to +/-2^22.
		return Int((cast + Float(0x00C00000)) - Float(0x00C00000));
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Nearbyint, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto nearbyint = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		nearbyint->addArg(cast.value);
		::basicBlock->appendInst(nearbyint);

		return RValue<Int>(V(result));
	}
}

Type *Int::getType()
{
	return T(Ice::IceType_i32);
}

Type *Long::getType()
{
	return T(Ice::IceType_i64);
}

UInt::UInt(RValue<Float> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
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
	                       Int(cast)))
	               .value);
}

RValue<UInt> operator++(UInt &val, int)  // Post-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<UInt> res = val;
	val += 1;
	return res;
}

const UInt &operator++(UInt &val)  // Pre-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	val += 1;
	return val;
}

RValue<UInt> operator--(UInt &val, int)  // Post-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<UInt> res = val;
	val -= 1;
	return res;
}

const UInt &operator--(UInt &val)  // Pre-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	val -= 1;
	return val;
}

//	RValue<UInt> RoundUInt(RValue<Float> cast)
//	{
//		ASSERT(false && "UNIMPLEMENTED"); return RValue<UInt>(V(nullptr));
//	}

Type *UInt::getType()
{
	return T(Ice::IceType_i32);
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

RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Int2 result;
		result = Insert(result, Extract(lhs, 0) << Int(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << Int(rhs), 1);

		return result;
	}
	else
	{
		return RValue<Int2>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Int2 result;
		result = Insert(result, Extract(lhs, 0) >> Int(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> Int(rhs), 1);

		return result;
	}
	else
	{
		return RValue<Int2>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

Type *Int2::getType()
{
	return T(Type_v2i32);
}

RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UInt2 result;
		result = Insert(result, Extract(lhs, 0) << UInt(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << UInt(rhs), 1);

		return result;
	}
	else
	{
		return RValue<UInt2>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UInt2 result;
		result = Insert(result, Extract(lhs, 0) >> UInt(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> UInt(rhs), 1);

		return result;
	}
	else
	{
		return RValue<UInt2>(Nucleus::createLShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

Type *UInt2::getType()
{
	return T(Type_v2i32);
}

Int4::Int4(RValue<Byte4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *x = Nucleus::createBitCast(cast.value, Int::getType());
	Value *a = Nucleus::createInsertElement(loadValue(), x, 0);

	Value *e;
	int swizzle[16] = { 0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23 };
	Value *b = Nucleus::createBitCast(a, Byte16::getType());
	Value *c = Nucleus::createShuffleVector(b, Nucleus::createNullValue(Byte16::getType()), swizzle);

	int swizzle2[8] = { 0, 8, 1, 9, 2, 10, 3, 11 };
	Value *d = Nucleus::createBitCast(c, Short8::getType());
	e = Nucleus::createShuffleVector(d, Nucleus::createNullValue(Short8::getType()), swizzle2);

	Value *f = Nucleus::createBitCast(e, Int4::getType());
	storeValue(f);
}

Int4::Int4(RValue<SByte4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *x = Nucleus::createBitCast(cast.value, Int::getType());
	Value *a = Nucleus::createInsertElement(loadValue(), x, 0);

	int swizzle[16] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };
	Value *b = Nucleus::createBitCast(a, Byte16::getType());
	Value *c = Nucleus::createShuffleVector(b, b, swizzle);

	int swizzle2[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };
	Value *d = Nucleus::createBitCast(c, Short8::getType());
	Value *e = Nucleus::createShuffleVector(d, d, swizzle2);

	*this = As<Int4>(e) >> 24;
}

Int4::Int4(RValue<Short4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	int swizzle[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };
	Value *c = Nucleus::createShuffleVector(cast.value, cast.value, swizzle);

	*this = As<Int4>(c) >> 16;
}

Int4::Int4(RValue<UShort4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	int swizzle[8] = { 0, 8, 1, 9, 2, 10, 3, 11 };
	Value *c = Nucleus::createShuffleVector(cast.value, Short8(0, 0, 0, 0, 0, 0, 0, 0).loadValue(), swizzle);
	Value *d = Nucleus::createBitCast(c, Int4::getType());
	storeValue(d);
}

Int4::Int4(RValue<Int> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = Nucleus::createBitCast(rhs.value, Int4::getType());

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(vector, vector, swizzle);

	storeValue(replicate);
}

RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Int4 result;
		result = Insert(result, Extract(lhs, 0) << Int(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << Int(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << Int(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << Int(rhs), 3);

		return result;
	}
	else
	{
		return RValue<Int4>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Int4 result;
		result = Insert(result, Extract(lhs, 0) >> Int(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> Int(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> Int(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> Int(rhs), 3);

		return result;
	}
	else
	{
		return RValue<Int4>(Nucleus::createAShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpEQ(x.value, y.value));
}

RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpSLT(x.value, y.value));
}

RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpSLE(x.value, y.value));
}

RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpNE(x.value, y.value));
}

RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpSGE(x.value, y.value));
}

RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createICmpSGT(x.value, y.value));
}

RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Sle, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4i32);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<Int4>(V(result));
}

RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Sgt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4i32);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<Int4>(V(result));
}

RValue<Int4> RoundInt(RValue<Float4> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		// Push the fractional part off the mantissa. Accurate up to +/-2^22.
		return Int4((cast + Float4(0x00C00000)) - Float4(0x00C00000));
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Nearbyint, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto nearbyint = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		nearbyint->addArg(cast.value);
		::basicBlock->appendInst(nearbyint);

		return RValue<Int4>(V(result));
	}
}

RValue<Short8> PackSigned(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		Short8 result;
		result = Insert(result, SaturateSigned(Extract(x, 0)), 0);
		result = Insert(result, SaturateSigned(Extract(x, 1)), 1);
		result = Insert(result, SaturateSigned(Extract(x, 2)), 2);
		result = Insert(result, SaturateSigned(Extract(x, 3)), 3);
		result = Insert(result, SaturateSigned(Extract(y, 0)), 4);
		result = Insert(result, SaturateSigned(Extract(y, 1)), 5);
		result = Insert(result, SaturateSigned(Extract(y, 2)), 6);
		result = Insert(result, SaturateSigned(Extract(y, 3)), 7);

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::VectorPackSigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pack = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pack->addArg(x.value);
		pack->addArg(y.value);
		::basicBlock->appendInst(pack);

		return RValue<Short8>(V(result));
	}
}

RValue<UShort8> PackUnsigned(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || !(CPUID::SSE4_1 || CPUID::ARM))
	{
		RValue<Int4> sx = As<Int4>(x);
		RValue<Int4> bx = (sx & ~(sx >> 31)) - Int4(0x8000);

		RValue<Int4> sy = As<Int4>(y);
		RValue<Int4> by = (sy & ~(sy >> 31)) - Int4(0x8000);

		return As<UShort8>(PackSigned(bx, by) + Short8(0x8000u));
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v8i16);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::VectorPackUnsigned, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto pack = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		pack->addArg(x.value);
		pack->addArg(y.value);
		::basicBlock->appendInst(pack);

		return RValue<UShort8>(V(result));
	}
}

RValue<Int> SignMask(RValue<Int4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		Int4 xx = (x >> 31) & Int4(0x00000001, 0x00000002, 0x00000004, 0x00000008);
		return Extract(xx, 0) | Extract(xx, 1) | Extract(xx, 2) | Extract(xx, 3);
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SignMask, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto movmsk = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		movmsk->addArg(x.value);
		::basicBlock->appendInst(movmsk);

		return RValue<Int>(V(result));
	}
}

Type *Int4::getType()
{
	return T(Ice::IceType_v4i32);
}

UInt4::UInt4(RValue<Float4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
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

UInt4::UInt4(RValue<UInt> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = Nucleus::createBitCast(rhs.value, UInt4::getType());

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(vector, vector, swizzle);

	storeValue(replicate);
}

RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UInt4 result;
		result = Insert(result, Extract(lhs, 0) << UInt(rhs), 0);
		result = Insert(result, Extract(lhs, 1) << UInt(rhs), 1);
		result = Insert(result, Extract(lhs, 2) << UInt(rhs), 2);
		result = Insert(result, Extract(lhs, 3) << UInt(rhs), 3);

		return result;
	}
	else
	{
		return RValue<UInt4>(Nucleus::createShl(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UInt4 result;
		result = Insert(result, Extract(lhs, 0) >> UInt(rhs), 0);
		result = Insert(result, Extract(lhs, 1) >> UInt(rhs), 1);
		result = Insert(result, Extract(lhs, 2) >> UInt(rhs), 2);
		result = Insert(result, Extract(lhs, 3) >> UInt(rhs), 3);

		return result;
	}
	else
	{
		return RValue<UInt4>(Nucleus::createLShr(lhs.value, V(::context->getConstantInt32(rhs))));
	}
}

RValue<UInt4> CmpEQ(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpEQ(x.value, y.value));
}

RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpULT(x.value, y.value));
}

RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpULE(x.value, y.value));
}

RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpNE(x.value, y.value));
}

RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpUGE(x.value, y.value));
}

RValue<UInt4> CmpNLE(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createICmpUGT(x.value, y.value));
}

RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Ule, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4i32);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<UInt4>(V(result));
}

RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Ugt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4i32);
	auto select = Ice::InstSelect::create(::function, result, condition, y.value, x.value);
	::basicBlock->appendInst(select);

	return RValue<UInt4>(V(result));
}

Type *UInt4::getType()
{
	return T(Ice::IceType_v4i32);
}

Type *Half::getType()
{
	return T(Ice::IceType_i16);
}

RValue<Float> Rcp_pp(RValue<Float> x, bool exactAtPow2)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return 1.0f / x;
}

RValue<Float> RcpSqrt_pp(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Rcp_pp(Sqrt(x));
}

RValue<Float> Sqrt(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *result = ::function->makeVariable(Ice::IceType_f32);
	const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Sqrt, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
	auto target = ::context->getConstantUndef(Ice::IceType_i32);
	auto sqrt = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
	sqrt->addArg(x.value);
	::basicBlock->appendInst(sqrt);

	return RValue<Float>(V(result));
}

RValue<Float> Round(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(Round(Float4(x))).x;
}

RValue<Float> Trunc(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(Trunc(Float4(x))).x;
}

RValue<Float> Frac(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(Frac(Float4(x))).x;
}

RValue<Float> Floor(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(Floor(Float4(x))).x;
}

RValue<Float> Ceil(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(Ceil(Float4(x))).x;
}

Type *Float::getType()
{
	return T(Ice::IceType_f32);
}

Type *Float2::getType()
{
	return T(Type_v2f32);
}

Float4::Float4(RValue<Float> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = Nucleus::createBitCast(rhs.value, Float4::getType());

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(vector, vector, swizzle);

	storeValue(replicate);
}

RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstFcmp::create(::function, Ice::InstFcmp::Ogt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
	auto select = Ice::InstSelect::create(::function, result, condition, x.value, y.value);
	::basicBlock->appendInst(select);

	return RValue<Float4>(V(result));
}

RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Ice::Variable *condition = ::function->makeVariable(Ice::IceType_v4i1);
	auto cmp = Ice::InstFcmp::create(::function, Ice::InstFcmp::Olt, condition, x.value, y.value);
	::basicBlock->appendInst(cmp);

	Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
	auto select = Ice::InstSelect::create(::function, result, condition, x.value, y.value);
	::basicBlock->appendInst(select);

	return RValue<Float4>(V(result));
}

RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Float4(1.0f) / x;
}

RValue<Float4> RcpSqrt_pp(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Rcp_pp(Sqrt(x));
}

RValue<Float4> Sqrt(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		Float4 result;
		result.x = Sqrt(Float(Float4(x).x));
		result.y = Sqrt(Float(Float4(x).y));
		result.z = Sqrt(Float(Float4(x).z));
		result.w = Sqrt(Float(Float4(x).w));

		return result;
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Sqrt, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto sqrt = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		sqrt->addArg(x.value);
		::basicBlock->appendInst(sqrt);

		return RValue<Float4>(V(result));
	}
}

RValue<Int> SignMask(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		Int4 xx = (As<Int4>(x) >> 31) & Int4(0x00000001, 0x00000002, 0x00000004, 0x00000008);
		return Extract(xx, 0) | Extract(xx, 1) | Extract(xx, 2) | Extract(xx, 3);
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::SignMask, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto movmsk = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		movmsk->addArg(x.value);
		::basicBlock->appendInst(movmsk);

		return RValue<Int>(V(result));
	}
}

RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpOEQ(x.value, y.value));
}

RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpOLT(x.value, y.value));
}

RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpOLE(x.value, y.value));
}

RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpONE(x.value, y.value));
}

RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpOGE(x.value, y.value));
}

RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpOGT(x.value, y.value));
}

RValue<Int4> CmpUEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpUEQ(x.value, y.value));
}

RValue<Int4> CmpULT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpULT(x.value, y.value));
}

RValue<Int4> CmpULE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpULE(x.value, y.value));
}

RValue<Int4> CmpUNEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpUNE(x.value, y.value));
}

RValue<Int4> CmpUNLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpUGE(x.value, y.value));
}

RValue<Int4> CmpUNLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createFCmpUGT(x.value, y.value));
}

RValue<Float4> Round(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics || CPUID::ARM)
	{
		// Push the fractional part off the mantissa. Accurate up to +/-2^22.
		return (x + Float4(0x00C00000)) - Float4(0x00C00000);
	}
	else if(CPUID::SSE4_1)
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Round, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto round = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		round->addArg(x.value);
		round->addArg(::context->getConstantInt32(0));
		::basicBlock->appendInst(round);

		return RValue<Float4>(V(result));
	}
	else
	{
		return Float4(RoundInt(x));
	}
}

RValue<Float4> Trunc(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(CPUID::SSE4_1)
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Round, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto round = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		round->addArg(x.value);
		round->addArg(::context->getConstantInt32(3));
		::basicBlock->appendInst(round);

		return RValue<Float4>(V(result));
	}
	else
	{
		return Float4(Int4(x));
	}
}

RValue<Float4> Frac(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Float4 frc;

	if(CPUID::SSE4_1)
	{
		frc = x - Floor(x);
	}
	else
	{
		frc = x - Float4(Int4(x));  // Signed fractional part.

		frc += As<Float4>(As<Int4>(CmpNLE(Float4(0.0f), frc)) & As<Int4>(Float4(1, 1, 1, 1)));  // Add 1.0 if negative.
	}

	// x - floor(x) can be 1.0 for very small negative x.
	// Clamp against the value just below 1.0.
	return Min(frc, As<Float4>(Int4(0x3F7FFFFF)));
}

RValue<Float4> Floor(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(CPUID::SSE4_1)
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Round, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto round = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		round->addArg(x.value);
		round->addArg(::context->getConstantInt32(1));
		::basicBlock->appendInst(round);

		return RValue<Float4>(V(result));
	}
	else
	{
		return x - Frac(x);
	}
}

RValue<Float4> Ceil(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(CPUID::SSE4_1)
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_v4f32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Round, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto round = Ice::InstIntrinsicCall::create(::function, 2, result, target, intrinsic);
		round->addArg(x.value);
		round->addArg(::context->getConstantInt32(2));
		::basicBlock->appendInst(round);

		return RValue<Float4>(V(result));
	}
	else
	{
		return -Floor(-x);
	}
}

Type *Float4::getType()
{
	return T(Ice::IceType_v4f32);
}

RValue<Long> Ticks()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("RValue<Long> Ticks()");
	return Long(Int(0));
}

RValue<Pointer<Byte>> ConstantPointer(void const *ptr)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Pointer<Byte>>{ V(sz::getConstantPointer(::context, ptr)) };
}

RValue<Pointer<Byte>> ConstantData(void const *data, size_t size)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Pointer<Byte>>{ V(IceConstantData(data, size)) };
}

Value *Call(RValue<Pointer<Byte>> fptr, Type *retTy, std::initializer_list<Value *> args, std::initializer_list<Type *> argTys)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(sz::Call(::function, ::basicBlock, T(retTy), V(fptr.value), V(args), false));
}

void Breakpoint()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Trap, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
	auto target = ::context->getConstantUndef(Ice::IceType_i32);
	auto trap = Ice::InstIntrinsicCall::create(::function, 0, nullptr, target, intrinsic);
	::basicBlock->appendInst(trap);
}

void Nucleus::createFence(std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::AtomicFence, Ice::Intrinsics::SideEffects_T, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
	auto target = ::context->getConstantUndef(Ice::IceType_i32);
	auto inst = Ice::InstIntrinsicCall::create(::function, 0, nullptr, target, intrinsic);
	auto order = ::context->getConstantInt32(stdToIceMemoryOrder(memoryOrder));
	inst->addArg(order);
	::basicBlock->appendInst(inst);
}

Value *Nucleus::createMaskedLoad(Value *ptr, Type *elTy, Value *mask, unsigned int alignment, bool zeroMaskedLanes)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("Subzero createMaskedLoad()");
	return nullptr;
}
void Nucleus::createMaskedStore(Value *ptr, Value *val, Value *mask, unsigned int alignment)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	UNIMPLEMENTED_NO_BUG("Subzero createMaskedStore()");
}

RValue<Float4> Gather(RValue<Pointer<Float>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Gather(base, offsets, mask, alignment, zeroMaskedLanes);
}

RValue<Int4> Gather(RValue<Pointer<Int>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Gather(base, offsets, mask, alignment, zeroMaskedLanes);
}

void Scatter(RValue<Pointer<Float>> base, RValue<Float4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Scatter(base, val, offsets, mask, alignment);
}

void Scatter(RValue<Pointer<Int>> base, RValue<Int4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Scatter(base, val, offsets, mask, alignment);
}

RValue<Float> Exp2(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Exp2(x);
}

RValue<Float> Log2(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Log2(x);
}

RValue<Float4> Sin(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Sin(x);
}

RValue<Float4> Cos(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Cos(x);
}

RValue<Float4> Tan(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Tan(x);
}

RValue<Float4> Asin(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Asin(x);
}

RValue<Float4> Acos(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Acos(x);
}

RValue<Float4> Atan(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Atan(x);
}

RValue<Float4> Sinh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Sinh(x);
}

RValue<Float4> Cosh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Cosh(x);
}

RValue<Float4> Tanh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Tanh(x);
}

RValue<Float4> Asinh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Asinh(x);
}

RValue<Float4> Acosh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Acosh(x);
}

RValue<Float4> Atanh(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Atanh(x);
}

RValue<Float4> Atan2(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Atan2(x, y);
}

RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Pow(x, y);
}

RValue<Float4> Exp(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Exp(x);
}

RValue<Float4> Log(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Log(x);
}

RValue<Float4> Exp2(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Exp2(x);
}

RValue<Float4> Log2(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Log2(x);
}

RValue<UInt> Ctlz(RValue<UInt> x, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UNIMPLEMENTED_NO_BUG("Subzero Ctlz()");
		return UInt(0);
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Ctlz, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto ctlz = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		ctlz->addArg(x.value);
		::basicBlock->appendInst(ctlz);

		return RValue<UInt>(V(result));
	}
}

RValue<UInt4> Ctlz(RValue<UInt4> x, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UNIMPLEMENTED_NO_BUG("Subzero Ctlz()");
		return UInt4(0);
	}
	else
	{
		// TODO: implement vectorized version in Subzero
		UInt4 result;
		result = Insert(result, Ctlz(Extract(x, 0), isZeroUndef), 0);
		result = Insert(result, Ctlz(Extract(x, 1), isZeroUndef), 1);
		result = Insert(result, Ctlz(Extract(x, 2), isZeroUndef), 2);
		result = Insert(result, Ctlz(Extract(x, 3), isZeroUndef), 3);
		return result;
	}
}

RValue<UInt> Cttz(RValue<UInt> x, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UNIMPLEMENTED_NO_BUG("Subzero Cttz()");
		return UInt(0);
	}
	else
	{
		Ice::Variable *result = ::function->makeVariable(Ice::IceType_i32);
		const Ice::Intrinsics::IntrinsicInfo intrinsic = { Ice::Intrinsics::Cttz, Ice::Intrinsics::SideEffects_F, Ice::Intrinsics::ReturnsTwice_F, Ice::Intrinsics::MemoryWrite_F };
		auto target = ::context->getConstantUndef(Ice::IceType_i32);
		auto ctlz = Ice::InstIntrinsicCall::create(::function, 1, result, target, intrinsic);
		ctlz->addArg(x.value);
		::basicBlock->appendInst(ctlz);

		return RValue<UInt>(V(result));
	}
}

RValue<UInt4> Cttz(RValue<UInt4> x, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(emulateIntrinsics)
	{
		UNIMPLEMENTED_NO_BUG("Subzero Cttz()");
		return UInt4(0);
	}
	else
	{
		// TODO: implement vectorized version in Subzero
		UInt4 result;
		result = Insert(result, Cttz(Extract(x, 0), isZeroUndef), 0);
		result = Insert(result, Cttz(Extract(x, 1), isZeroUndef), 1);
		result = Insert(result, Cttz(Extract(x, 2), isZeroUndef), 2);
		result = Insert(result, Cttz(Extract(x, 3), isZeroUndef), 3);
		return result;
	}
}

RValue<Int> MinAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::MinAtomic(x, y, memoryOrder);
}

RValue<UInt> MinAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::MinAtomic(x, y, memoryOrder);
}

RValue<Int> MaxAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::MaxAtomic(x, y, memoryOrder);
}

RValue<UInt> MaxAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::MaxAtomic(x, y, memoryOrder);
}

void EmitDebugLocation()
{
#ifdef ENABLE_RR_DEBUG_INFO
#	ifdef ENABLE_RR_EMIT_PRINT_LOCATION
	emitPrintLocation(getCallerBacktrace());
#	endif  // ENABLE_RR_EMIT_PRINT_LOCATION
#endif      // ENABLE_RR_DEBUG_INFO
}
void EmitDebugVariable(Value *value) {}
void FlushDebug() {}

namespace {
namespace coro {

// Instance data per generated coroutine
// This is the "handle" type used for Coroutine functions
// Lifetime: from yield to when CoroutineEntryDestroy generated function is called.
struct CoroutineData
{
	bool useInternalScheduler = false;
	bool done = false;        // the coroutine should stop at the next yield()
	bool terminated = false;  // the coroutine has finished.
	bool inRoutine = false;   // is the coroutine currently executing?
	marl::Scheduler::Fiber *mainFiber = nullptr;
	marl::Scheduler::Fiber *routineFiber = nullptr;
	void *promisePtr = nullptr;
};

CoroutineData *createCoroutineData()
{
	return new CoroutineData{};
}

void destroyCoroutineData(CoroutineData *coroData)
{
	delete coroData;
}

// suspend() pauses execution of the coroutine, and resumes execution from the
// caller's call to await().
// Returns true if await() is called again, or false if coroutine_destroy()
// is called.
bool suspend(Nucleus::CoroutineHandle handle)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	ASSERT(marl::Scheduler::Fiber::current() == coroData->routineFiber);
	ASSERT(coroData->inRoutine);
	coroData->inRoutine = false;
	coroData->mainFiber->notify();
	while(!coroData->inRoutine)
	{
		coroData->routineFiber->wait();
	}
	return !coroData->done;
}

// resume() is called by await(), blocking until the coroutine calls yield()
// or the coroutine terminates.
void resume(Nucleus::CoroutineHandle handle)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	ASSERT(marl::Scheduler::Fiber::current() == coroData->mainFiber);
	ASSERT(!coroData->inRoutine);
	coroData->inRoutine = true;
	coroData->routineFiber->notify();
	while(coroData->inRoutine)
	{
		coroData->mainFiber->wait();
	}
}

// stop() is called by coroutine_destroy(), signalling that it's done, then blocks
// until the coroutine ends, and deletes the coroutine data.
void stop(Nucleus::CoroutineHandle handle)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	ASSERT(marl::Scheduler::Fiber::current() == coroData->mainFiber);
	ASSERT(!coroData->inRoutine);
	if(!coroData->terminated)
	{
		coroData->done = true;
		coroData->inRoutine = true;
		coroData->routineFiber->notify();
		while(!coroData->terminated)
		{
			coroData->mainFiber->wait();
		}
	}
	if(coroData->useInternalScheduler)
	{
		::getOrCreateScheduler().unbind();
	}
	coro::destroyCoroutineData(coroData);  // free the coroutine data.
}

namespace detail {
thread_local rr::Nucleus::CoroutineHandle coroHandle{};
}  // namespace detail

void setHandleParam(Nucleus::CoroutineHandle handle)
{
	ASSERT(!detail::coroHandle);
	detail::coroHandle = handle;
}

Nucleus::CoroutineHandle getHandleParam()
{
	ASSERT(detail::coroHandle);
	auto handle = detail::coroHandle;
	detail::coroHandle = {};
	return handle;
}

bool isDone(Nucleus::CoroutineHandle handle)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	return coroData->done;
}

void setPromisePtr(Nucleus::CoroutineHandle handle, void *promisePtr)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	coroData->promisePtr = promisePtr;
}

void *getPromisePtr(Nucleus::CoroutineHandle handle)
{
	auto *coroData = reinterpret_cast<CoroutineData *>(handle);
	return coroData->promisePtr;
}

}  // namespace coro
}  // namespace

// Used to generate coroutines.
// Lifetime: from yield to acquireCoroutine
class CoroutineGenerator
{
public:
	CoroutineGenerator()
	{
	}

	// Inserts instructions at the top of the current function to make it a coroutine.
	void generateCoroutineBegin()
	{
		// Begin building the main coroutine_begin() function.
		// We insert these instructions at the top of the entry node,
		// before existing reactor-generated instructions.

		//    CoroutineHandle coroutine_begin(<Arguments>)
		//    {
		//        this->handle = coro::getHandleParam();
		//
		//        YieldType promise;
		//        coro::setPromisePtr(handle, &promise); // For await
		//
		//        ... <REACTOR CODE> ...
		//

		//        this->handle = coro::getHandleParam();
		this->handle = sz::Call(::function, ::entryBlock, coro::getHandleParam);

		//        YieldType promise;
		//        coro::setPromisePtr(handle, &promise); // For await
		this->promise = sz::allocateStackVariable(::function, T(::coroYieldType));
		sz::Call(::function, ::entryBlock, coro::setPromisePtr, this->handle, this->promise);
	}

	// Adds instructions for Yield() calls at the current location of the main coroutine function.
	void generateYield(Value *val)
	{
		//        ... <REACTOR CODE> ...
		//
		//        promise = val;
		//        if (!coro::suspend(handle)) {
		//            return false; // coroutine has been stopped by the caller.
		//        }
		//
		//        ... <REACTOR CODE> ...

		//        promise = val;
		Nucleus::createStore(val, V(this->promise), ::coroYieldType);

		//        if (!coro::suspend(handle)) {
		auto result = sz::Call(::function, ::basicBlock, coro::suspend, this->handle);
		auto doneBlock = Nucleus::createBasicBlock();
		auto resumeBlock = Nucleus::createBasicBlock();
		Nucleus::createCondBr(V(result), resumeBlock, doneBlock);

		//            return false; // coroutine has been stopped by the caller.
		::basicBlock = doneBlock;
		Nucleus::createRetVoid();  // coroutine return value is ignored.

		//        ... <REACTOR CODE> ...
		::basicBlock = resumeBlock;
	}

	using FunctionUniquePtr = std::unique_ptr<Ice::Cfg>;

	// Generates the await function for the current coroutine.
	// Cannot use Nucleus functions that modify ::function and ::basicBlock.
	static FunctionUniquePtr generateAwaitFunction()
	{
		// bool coroutine_await(CoroutineHandle handle, YieldType* out)
		// {
		//     if (coro::isDone())
		//     {
		//         return false;
		//     }
		//     else // resume
		//     {
		//         YieldType* promise = coro::getPromisePtr(handle);
		//         *out = *promise;
		//         coro::resume(handle);
		//         return true;
		//     }
		// }

		// Subzero doesn't support bool types (IceType_i1) as return type
		const Ice::Type ReturnType = Ice::IceType_i32;
		const Ice::Type YieldPtrType = sz::getPointerType(T(::coroYieldType));
		const Ice::Type HandleType = sz::getPointerType(Ice::IceType_void);

		Ice::Cfg *awaitFunc = sz::createFunction(::context, ReturnType, std::vector<Ice::Type>{ HandleType, YieldPtrType });
		Ice::CfgLocalAllocatorScope scopedAlloc{ awaitFunc };

		Ice::Variable *handle = awaitFunc->getArgs()[0];
		Ice::Variable *outPtr = awaitFunc->getArgs()[1];

		auto doneBlock = awaitFunc->makeNode();
		{
			//         return false;
			Ice::InstRet *ret = Ice::InstRet::create(awaitFunc, ::context->getConstantInt32(0));
			doneBlock->appendInst(ret);
		}

		auto resumeBlock = awaitFunc->makeNode();
		{
			//         YieldType* promise = coro::getPromisePtr(handle);
			Ice::Variable *promise = sz::Call(awaitFunc, resumeBlock, coro::getPromisePtr, handle);

			//         *out = *promise;
			// Load promise value
			Ice::Variable *promiseVal = awaitFunc->makeVariable(T(::coroYieldType));
			auto load = Ice::InstLoad::create(awaitFunc, promiseVal, promise);
			resumeBlock->appendInst(load);
			// Then store it in output param
			auto store = Ice::InstStore::create(awaitFunc, promiseVal, outPtr);
			resumeBlock->appendInst(store);

			//         coro::resume(handle);
			sz::Call(awaitFunc, resumeBlock, coro::resume, handle);

			//         return true;
			Ice::InstRet *ret = Ice::InstRet::create(awaitFunc, ::context->getConstantInt32(1));
			resumeBlock->appendInst(ret);
		}

		//     if (coro::isDone())
		//     {
		//         <doneBlock>
		//     }
		//     else // resume
		//     {
		//         <resumeBlock>
		//     }
		Ice::CfgNode *bb = awaitFunc->getEntryNode();
		Ice::Variable *done = sz::Call(awaitFunc, bb, coro::isDone, handle);
		auto br = Ice::InstBr::create(awaitFunc, done, doneBlock, resumeBlock);
		bb->appendInst(br);

		return FunctionUniquePtr{ awaitFunc };
	}

	// Generates the destroy function for the current coroutine.
	// Cannot use Nucleus functions that modify ::function and ::basicBlock.
	static FunctionUniquePtr generateDestroyFunction()
	{
		// void coroutine_destroy(Nucleus::CoroutineHandle handle)
		// {
		//     coro::stop(handle); // signal and wait for coroutine to stop, and delete coroutine data
		//     return;
		// }

		const Ice::Type ReturnType = Ice::IceType_void;
		const Ice::Type HandleType = sz::getPointerType(Ice::IceType_void);

		Ice::Cfg *destroyFunc = sz::createFunction(::context, ReturnType, std::vector<Ice::Type>{ HandleType });
		Ice::CfgLocalAllocatorScope scopedAlloc{ destroyFunc };

		Ice::Variable *handle = destroyFunc->getArgs()[0];

		auto *bb = destroyFunc->getEntryNode();

		//     coro::stop(handle); // signal and wait for coroutine to stop, and delete coroutine data
		sz::Call(destroyFunc, bb, coro::stop, handle);

		//     return;
		Ice::InstRet *ret = Ice::InstRet::create(destroyFunc);
		bb->appendInst(ret);

		return FunctionUniquePtr{ destroyFunc };
	}

private:
	Ice::Variable *handle{};
	Ice::Variable *promise{};
};

static Nucleus::CoroutineHandle invokeCoroutineBegin(std::function<Nucleus::CoroutineHandle()> beginFunc)
{
	// This doubles up as our coroutine handle
	auto coroData = coro::createCoroutineData();

	coroData->useInternalScheduler = (marl::Scheduler::get() == nullptr);
	if(coroData->useInternalScheduler)
	{
		::getOrCreateScheduler().bind();
	}

	auto run = [=] {
		// Store handle in TLS so that the coroutine can grab it right away, before
		// any fiber switch occurs.
		coro::setHandleParam(coroData);

		ASSERT(!coroData->routineFiber);
		coroData->routineFiber = marl::Scheduler::Fiber::current();

		beginFunc();

		ASSERT(coroData->inRoutine);
		coroData->done = true;        // coroutine is done.
		coroData->terminated = true;  // signal that the coroutine data is ready for freeing.
		coroData->inRoutine = false;
		coroData->mainFiber->notify();
	};

	ASSERT(!coroData->mainFiber);
	coroData->mainFiber = marl::Scheduler::Fiber::current();

	// block until the first yield or coroutine end
	ASSERT(!coroData->inRoutine);
	coroData->inRoutine = true;
	marl::schedule(marl::Task(run, marl::Task::Flags::SameThread));
	while(coroData->inRoutine)
	{
		coroData->mainFiber->wait();
	}

	return coroData;
}

void Nucleus::createCoroutine(Type *yieldType, const std::vector<Type *> &params)
{
	// Start by creating a regular function
	createFunction(yieldType, params);

	// Save in case yield() is called
	ASSERT(::coroYieldType == nullptr);  // Only one coroutine can be generated at once
	::coroYieldType = yieldType;
}

void Nucleus::yield(Value *val)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	// On first yield, we start generating coroutine functions
	if(!::coroGen)
	{
		::coroGen = std::make_shared<CoroutineGenerator>();
		::coroGen->generateCoroutineBegin();
	}

	ASSERT(::coroGen);
	::coroGen->generateYield(val);
}

static bool coroutineEntryAwaitStub(Nucleus::CoroutineHandle, void *yieldValue)
{
	return false;
}

static void coroutineEntryDestroyStub(Nucleus::CoroutineHandle handle)
{
}

std::shared_ptr<Routine> Nucleus::acquireCoroutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
{
	if(::coroGen)
	{
		// Finish generating coroutine functions
		{
			Ice::CfgLocalAllocatorScope scopedAlloc{ ::function };
			finalizeFunction();
		}

		auto awaitFunc = ::coroGen->generateAwaitFunction();
		auto destroyFunc = ::coroGen->generateDestroyFunction();

		// At this point, we no longer need the CoroutineGenerator.
		::coroGen.reset();
		::coroYieldType = nullptr;

		auto routine = rr::acquireRoutine({ ::function, awaitFunc.get(), destroyFunc.get() },
		                                  { name, "await", "destroy" },
		                                  cfgEdit);

		return routine;
	}
	else
	{
		{
			Ice::CfgLocalAllocatorScope scopedAlloc{ ::function };
			finalizeFunction();
		}

		::coroYieldType = nullptr;

		// Not an actual coroutine (no yields), so return stubs for await and destroy
		auto routine = rr::acquireRoutine({ ::function }, { name }, cfgEdit);

		auto routineImpl = std::static_pointer_cast<ELFMemoryStreamer>(routine);
		routineImpl->setEntry(Nucleus::CoroutineEntryAwait, reinterpret_cast<const void *>(&coroutineEntryAwaitStub));
		routineImpl->setEntry(Nucleus::CoroutineEntryDestroy, reinterpret_cast<const void *>(&coroutineEntryDestroyStub));
		return routine;
	}
}

Nucleus::CoroutineHandle Nucleus::invokeCoroutineBegin(Routine &routine, std::function<Nucleus::CoroutineHandle()> func)
{
	const bool isCoroutine = routine.getEntry(Nucleus::CoroutineEntryAwait) != reinterpret_cast<const void *>(&coroutineEntryAwaitStub);

	if(isCoroutine)
	{
		return rr::invokeCoroutineBegin(func);
	}
	else
	{
		// For regular routines, just invoke the begin func directly
		return func();
	}
}

}  // namespace rr
