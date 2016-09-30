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

#include "Reactor.hpp"
#include "Routine.hpp"

#include "src/IceTypes.h"
#include "src/IceCfg.h"
#include "src/IceELFStreamer.h"
#include "src/IceGlobalContext.h"
#include "src/IceCfgNode.h"
#include "src/IceELFObjectWriter.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <mutex>
#include <limits>
#include <iostream>
#include <cassert>

namespace
{
	Ice::GlobalContext *context = nullptr;
	Ice::Cfg *function = nullptr;
	Ice::CfgNode *basicBlock = nullptr;
	Ice::CfgLocalAllocatorScope *allocator = nullptr;
	sw::Routine *routine = nullptr;

	std::mutex codegenMutex;

	Ice::ELFFileStreamer *elfFile = nullptr;
	Ice::Fdstream *out = nullptr;
}

namespace sw
{
	class Value : public Ice::Variable {};
	class Constant : public Ice::Constant {};
	class BasicBlock : public Ice::CfgNode {};

	Ice::Type T(Type *t)
	{
		return (Ice::Type)reinterpret_cast<std::intptr_t>(t);
	}

	Type *T(Ice::Type t)
	{
		return reinterpret_cast<Type*>(t);
	}

	Value *V(Ice::Variable *v)
	{
		return reinterpret_cast<Value*>(v);
	}

	Constant *C(Ice::Constant *c)
	{
		return reinterpret_cast<Constant*>(c);
	}

	BasicBlock *B(Ice::CfgNode *b)
	{
		return reinterpret_cast<BasicBlock*>(b);
	}

	Optimization optimization[10] = {InstructionCombining, Disabled};

	void *loadImage(uint8_t *const elfImage)
	{
		using ElfHeader = std::conditional<sizeof(void*) == 8, Elf64_Ehdr, Elf32_Ehdr>::type;
		ElfHeader *elfHeader = (ElfHeader*)elfImage;

		if(!elfHeader->checkMagic())
		{
			return nullptr;
		}

		using SectionHeader = std::conditional<sizeof(void*) == 8, Elf64_Shdr, Elf32_Shdr>::type;
		SectionHeader *sectionHeader = (SectionHeader*)(elfImage + elfHeader->e_shoff);
		void *entry = nullptr;

		for(int i = 0; i < elfHeader->e_shnum; i++)
		{
			if(sectionHeader[i].sh_type == SHT_PROGBITS && sectionHeader[i].sh_flags & SHF_EXECINSTR)
			{
				entry = elfImage + sectionHeader[i].sh_offset;
			}
		}

		return entry;
	}

	template<typename T>
	struct ExecutableAllocator
	{
		ExecutableAllocator() {};
		template<class U> ExecutableAllocator(const ExecutableAllocator<U> &other) {};

		using value_type = T;
		using size_type = std::size_t;

		T *allocate(size_type n)
		{
			return (T*)VirtualAlloc(NULL, sizeof(T) * n, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		}

		void deallocate(T *p, size_type n)
		{
			VirtualFree(p, 0, MEM_RELEASE);
		}
	};

	class ELFMemoryStreamer : public Ice::ELFStreamer, public Routine
	{
		ELFMemoryStreamer(const ELFMemoryStreamer &) = delete;
		ELFMemoryStreamer &operator=(const ELFMemoryStreamer &) = delete;

	public:
		ELFMemoryStreamer() : Routine()
		{
			position = 0;
			buffer.reserve(0x1000);
		}

		virtual ~ELFMemoryStreamer()
		{
			if(buffer.size() != 0)
			{
				DWORD exeProtection;
				VirtualProtect(&buffer[0], buffer.size(), oldProtection, &exeProtection);
			}
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
			else assert(false && "UNIMPLEMENTED");
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

		const void *getEntry() override
		{
			VirtualProtect(&buffer[0], buffer.size(), PAGE_EXECUTE_READ, &oldProtection);
			position = std::numeric_limits<std::size_t>::max();  // Can't write more data after this

			return loadImage(&buffer[0]);
		}

	private:
		std::vector<uint8_t, ExecutableAllocator<uint8_t>> buffer;
		std::size_t position;
		DWORD oldProtection;
	};

	Nucleus::Nucleus()
	{
		::codegenMutex.lock();   // Reactor is currently not thread safe

		Ice::ClFlags::Flags.setTargetArch(sizeof(void*) == 8 ? Ice::Target_X8664 : Ice::Target_X8632);
		Ice::ClFlags::Flags.setOutFileType(Ice::FT_Elf);
		Ice::ClFlags::Flags.setOptLevel(Ice::Opt_2);
		Ice::ClFlags::Flags.setApplicationBinaryInterface(Ice::ABI_Platform);

		std::unique_ptr<Ice::Ostream> cout(new llvm::raw_os_ostream(std::cout));

		if(false)   // Write out to a file
		{
			std::error_code errorCode;
			::out = new Ice::Fdstream("out.o", errorCode, llvm::sys::fs::F_None);
			::elfFile = new Ice::ELFFileStreamer(*out);
			::context = new Ice::GlobalContext(cout.get(), cout.get(), cout.get(), elfFile);
		}
		else
		{
			ELFMemoryStreamer *elfMemory = new ELFMemoryStreamer();
			::context = new Ice::GlobalContext(cout.get(), cout.get(), cout.get(), elfMemory);
			::routine = elfMemory;
		}
	}

	Nucleus::~Nucleus()
	{
		delete ::allocator;
		delete ::function;
		delete ::context;

		delete ::elfFile;
		delete ::out;

		::codegenMutex.unlock();
	}

	Routine *Nucleus::acquireRoutine(const wchar_t *name, bool runOptimizations)
	{
		if(basicBlock->getInsts().empty() || basicBlock->getInsts().back().getKind() != Ice::Inst::Ret)
		{
			createRetVoid();
		}

		std::wstring wideName(name);
		std::string asciiName(wideName.begin(), wideName.end());
		::function->setFunctionName(Ice::GlobalString::createWithString(::context, asciiName));

		::function->translate();

		::context->emitFileHeader();
		::function->emitIAS();
		auto assembler = ::function->releaseAssembler();
		::context->getObjectWriter()->writeFunctionCode(::function->getFunctionName(), false, assembler.get());
		::context->getObjectWriter()->writeNonUserSections();

		return ::routine;
	}

	void Nucleus::optimize()
	{
	}

	Value *Nucleus::allocateStackVariable(Type *t, int arraySize)
	{
		assert(arraySize == 0 && "UNIMPLEMENTED");

		Ice::Type type = T(t);
		int size = Ice::typeWidthInBytes(type);

		auto bytes = Ice::ConstantInteger32::create(::context, type, size);
		auto address = ::function->makeVariable(T(getPointerType(t)));
		auto alloca = Ice::InstAlloca::create(::function, address, bytes, size);
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
		assert(::basicBlock->getInsts().back().getTerminatorEdges().size() >= 0 && "Previous basic block must have a terminator");
		::basicBlock = basicBlock;
	}

	BasicBlock *Nucleus::getPredecessor(BasicBlock *basicBlock)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	void Nucleus::createFunction(Type *ReturnType, std::vector<Type*> &Params)
	{
		uint32_t sequenceNumber = 0;
		::function = Ice::Cfg::create(::context, sequenceNumber).release();
		::allocator = new Ice::CfgLocalAllocatorScope(::function);

		for(Type *type : Params)
		{
			Ice::Variable *arg = ::function->makeVariable(T(type));
			::function->addArg(arg);
		}

		Ice::CfgNode *node = ::function->makeNode();
		::function->setEntryNode(node);
		::basicBlock = node;
	}

	Value *Nucleus::getArgument(unsigned int index)
	{
		return V(::function->getArgs()[index]);
	}

	void Nucleus::createRetVoid()
	{
		assert(false && "UNIMPLEMENTED");
	}

	void Nucleus::createRet(Value *v)
	{
		assert(false && "UNIMPLEMENTED");
	}

	void Nucleus::createBr(BasicBlock *dest)
	{
		auto br = Ice::InstBr::create(::function, dest);
		::basicBlock->appendInst(br);
	}

	void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
	{
		auto br = Ice::InstBr::create(::function, cond, ifTrue, ifFalse);
		::basicBlock->appendInst(br);
	}

	static Value *createArithmetic(Ice::InstArithmetic::OpKind op, Value *lhs, Value *rhs)
	{
		assert(lhs->getType() == rhs->getType());

		Ice::Variable *result = ::function->makeVariable(lhs->getType());
		Ice::InstArithmetic *arithmetic = Ice::InstArithmetic::create(::function, op, result, lhs, rhs);
		::basicBlock->appendInst(arithmetic);

		return V(result);
	}

	Value *Nucleus::createAdd(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Add, lhs, rhs);
	}

	Value *Nucleus::createSub(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Sub, lhs, rhs);
	}

	Value *Nucleus::createMul(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Mul, lhs, rhs);
	}

	Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Udiv, lhs, rhs);
	}

	Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Sdiv, lhs, rhs);
	}

	Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Fadd, lhs, rhs);
	}

	Value *Nucleus::createFSub(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Fsub, lhs, rhs);
	}

	Value *Nucleus::createFMul(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Fmul, lhs, rhs);
	}

	Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Fdiv, lhs, rhs);
	}

	Value *Nucleus::createURem(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Urem, lhs, rhs);
	}

	Value *Nucleus::createSRem(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Srem, lhs, rhs);
	}

	Value *Nucleus::createFRem(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Frem, lhs, rhs);
	}

	Value *Nucleus::createShl(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Shl, lhs, rhs);
	}

	Value *Nucleus::createLShr(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Lshr, lhs, rhs);
	}

	Value *Nucleus::createAShr(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Ashr, lhs, rhs);
	}

	Value *Nucleus::createAnd(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::And, lhs, rhs);
	}

	Value *Nucleus::createOr(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Or, lhs, rhs);
	}

	Value *Nucleus::createXor(Value *lhs, Value *rhs)
	{
		return createArithmetic(Ice::InstArithmetic::Xor, lhs, rhs);
	}

	Value *Nucleus::createAssign(Constant *constant)
	{
		Ice::Variable *value = ::function->makeVariable(constant->getType());
		auto assign = Ice::InstAssign::create(::function, value, constant);
		::basicBlock->appendInst(assign);

		return V(value);
	}

	Value *Nucleus::createNeg(Value *v)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFNeg(Value *v)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createNot(Value *v)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		Ice::Variable *value = ::function->makeVariable(T(type));
		auto load = Ice::InstLoad::create(::function, value, ptr, align);
		::basicBlock->appendInst(load);
		return V(value);
	}

	Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		auto store = Ice::InstStore::create(::function, value, ptr, align);
		::basicBlock->appendInst(store);
		return value;
	}

	Constant *Nucleus::createStore(Constant *constant, Value *ptr, Type *type, bool isVolatile, unsigned int align)
	{
		auto store = Ice::InstStore::create(::function, constant, ptr, align);
		::basicBlock->appendInst(store);
		return constant;
	}

	Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index)
	{
		assert(index->getType() == Ice::IceType_i32);

		if(!Ice::isByteSizedType(T(type)))
		{
			index = createMul(index, createAssign(createConstantInt((int)Ice::typeWidthInBytes(T(type)))));
		}

		if(sizeof(void*) == 8)
		{
			index = createSExt(index, T(Ice::IceType_i64));
		}

		return createAdd(ptr, index);
	}

	Value *Nucleus::createAtomicAdd(Value *ptr, Value *value)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	static Value *createCast(Ice::InstCast::OpKind op, Value *v, Type *destType)
	{
		if(T(v->getType()) == destType)
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
		return createCast(Ice::InstCast::Trunc, v, destType);
	}

	Value *Nucleus::createZExt(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Zext, v, destType);
	}

	Value *Nucleus::createSExt(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Sext, v, destType);
	}

	Value *Nucleus::createFPToSI(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Fptosi, v, destType);
	}

	Value *Nucleus::createUIToFP(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Uitofp, v, destType);
	}

	Value *Nucleus::createSIToFP(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Sitofp, v, destType);
	}

	Value *Nucleus::createFPTrunc(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Fptrunc, v, destType);
	}

	Value *Nucleus::createFPExt(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Fpext, v, destType);
	}

	Value *Nucleus::createBitCast(Value *v, Type *destType)
	{
		return createCast(Ice::InstCast::Bitcast, v, destType);
	}

	Value *Nucleus::createIntCast(Value *v, Type *destType, bool isSigned)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
	{
		assert(lhs->getType() == rhs->getType());

		auto result = ::function->makeVariable(Ice::IceType_i1);
		auto cmp = Ice::InstIcmp::create(::function, Ice::InstIcmp::Slt, result, lhs, rhs);
		::basicBlock->appendInst(cmp);

		return V(result);
	}

	Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
	{
		auto result = ::function->makeVariable(T(type));
		auto extract = Ice::InstExtractElement::create(::function, result, vector, ::context->getConstantInt32(index));
		::basicBlock->appendInst(extract);

		return V(result);
	}

	Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
	{
		auto result = ::function->makeVariable(vector->getType());
		auto insert = Ice::InstInsertElement::create(::function, result, vector, element, ::context->getConstantInt32(index));
		::basicBlock->appendInst(insert);

		return V(result);
	}

	Value *Nucleus::createShuffleVector(Value *V1, Value *V2, const int *select)
	{
		assert(V1->getType() == V2->getType());

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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Value *Nucleus::createSwitch(Value *v, BasicBlock *Dest, unsigned NumCases)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	void Nucleus::addSwitchCase(Value *Switch, int Case, BasicBlock *Branch)
	{
		assert(false && "UNIMPLEMENTED"); return;
	}

	void Nucleus::createUnreachable()
	{
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantPointer(const void *address, Type *Ty, bool isConstant, unsigned int Align)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Type *Nucleus::getPointerType(Type *ElementType)
	{
		if(sizeof(void*) == 8)
		{
			return T(Ice::IceType_i64);
		}
		else
		{
			return T(Ice::IceType_i32);
		}
	}

	Constant *Nucleus::createNullValue(Type *Ty)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantInt(int64_t i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantInt(int i)
	{
		return C(::context->getConstantInt32(i));
	}

	Constant *Nucleus::createConstantInt(unsigned int i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantBool(bool b)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantByte(signed char i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantByte(unsigned char i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantShort(short i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantShort(unsigned short i)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantFloat(float x)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createNullPointer(Type *Ty)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Constant *Nucleus::createConstantVector(Constant *const *Vals, unsigned NumVals)
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Type *Void::getType()
	{
		return T(Ice::IceType_void);
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
		assert(false && "UNIMPLEMENTED"); return nullptr;
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

		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const Byte &operator++(const Byte &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED");

		return val;
	}

	RValue<Byte> operator--(const Byte &val, int)   // Post-decrement
	{
		RValue<Byte> res = val;

		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const Byte &operator--(const Byte &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED");

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
		return T(Ice::IceType_i8);
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

		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const SByte &operator++(const SByte &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return val;
	}

	RValue<SByte> operator--(const SByte &val, int)   // Post-decrement
	{
		RValue<SByte> res = val;

		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const SByte &operator--(const SByte &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

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
		assert(false && "UNIMPLEMENTED"); return nullptr;
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

		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const Short &operator++(const Short &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return val;
	}

	RValue<Short> operator--(const Short &val, int)   // Post-decrement
	{
		RValue<Short> res = val;

		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const Short &operator--(const Short &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

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
		assert(false && "UNIMPLEMENTED"); return nullptr;
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

		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const UShort &operator++(const UShort &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return val;
	}

	RValue<UShort> operator--(const UShort &val, int)   // Post-decrement
	{
		RValue<UShort> res = val;

		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

		return res;
	}

	const UShort &operator--(const UShort &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED");
		assert(false && "UNIMPLEMENTED");

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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Type *Byte4::getType()
	{
		#if 0
			return VectorType::get(Byte::getType(), 4);
		#else
			return UInt::getType();   // FIXME
		#endif
	}

	Type *SByte4::getType()
	{
		#if 0
			return VectorType::get(SByte::getType(), 4);
		#else
			return Int::getType();   // FIXME
		#endif
	}

	Byte8::Byte8()
	{
	//	xyzw.parent = this;
	}

	Byte8::Byte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7)
	{
	//	xyzw.parent = this;
	}

	Byte8::Byte8(int64_t x)
	{
	//	xyzw.parent = this;
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
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Byte8> operator-(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Byte8> operator|(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Byte8> operator^(RValue<Byte8> lhs, RValue<Byte8> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
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
		return RValue<Byte8>(Nucleus::createNot(val.value));
	}

	RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Short4> Unpack(RValue<Byte4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> UnpackLow(RValue<Byte8> x, RValue<Byte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> UnpackHigh(RValue<Byte8> x, RValue<Byte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Int> SignMask(RValue<Byte8> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y)
//	{
//		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
//	}

	RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	Type *Byte8::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	SByte8::SByte8()
	{
	//	xyzw.parent = this;
	}

	SByte8::SByte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
	}

	SByte8::SByte8(int64_t x)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<SByte8>(V(nullptr));
	}

	RValue<SByte8> operator-(RValue<SByte8> lhs, RValue<SByte8> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<SByte8>(V(nullptr));
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
		return RValue<SByte8>(Nucleus::createNot(val.value));
	}

	RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<SByte8>(V(nullptr));
	}

	RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<SByte8>(V(nullptr));
	}

	RValue<Short4> UnpackLow(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> UnpackHigh(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Int> SignMask(RValue<SByte8> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	Type *SByte8::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Type *SByte16::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Short4::Short4(RValue<Int> cast)
	{
		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
		Value *swizzle = Swizzle(RValue<Short4>(extend), 0x00).value;

		storeValue(swizzle);
	}

	Short4::Short4(RValue<Int4> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

//	Short4::Short4(RValue<Float> cast)
//	{
//	}

	Short4::Short4(RValue<Float4> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

	Short4::Short4()
	{
	//	xyzw.parent = this;
	}

	Short4::Short4(short xyzw)
	{
		//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
	}

	Short4::Short4(short x, short y, short z, short w)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator-(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator*(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator|(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator^(RValue<Short4> lhs, RValue<Short4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator<<(RValue<Short4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator>>(RValue<Short4> lhs, RValue<Long1> rhs)
	{
	//	return RValue<Short4>(Nucleus::createAShr(lhs.value, rhs.value));

		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> operator~(RValue<Short4> val)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> RoundShort4(RValue<Float4> cast)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<SByte8> Pack(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<SByte8>(V(nullptr));
	}

	RValue<Int2> UnpackLow(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> UnpackHigh(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Short4> Swizzle(RValue<Short4> x, unsigned char select)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> Insert(RValue<Short4> val, RValue<Short> element, int i)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short> Extract(RValue<Short4> val, int i)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short>(V(nullptr));
	}

	RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short4>(V(nullptr));
	}

	Type *Short4::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	UShort4::UShort4(RValue<Int4> cast)
	{
		*this = Short4(cast);
	}

	UShort4::UShort4(RValue<Float4> cast, bool saturate)
	{
		assert(false && "UNIMPLEMENTED");
	}

	UShort4::UShort4()
	{
	//	xyzw.parent = this;
	}

	UShort4::UShort4(unsigned short xyzw)
	{
		//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
	}

	UShort4::UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator-(RValue<UShort4> lhs, RValue<UShort4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator*(RValue<UShort4> lhs, RValue<UShort4> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator<<(RValue<UShort4> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> operator>>(RValue<UShort4> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort4>(V(nullptr));
	}

	RValue<Byte8> Pack(RValue<UShort4> x, RValue<UShort4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Byte8>(V(nullptr));
	}

	Type *UShort4::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Short8::Short8(short c0, short c1, short c2, short c3, short c4, short c5, short c6, short c7)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<Short8>(V(nullptr));
	}

	RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short8>(V(nullptr));
	}

	RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Int4> Abs(RValue<Int4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short8>(V(nullptr));
	}

	Type *Short8::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	UShort8::UShort8(unsigned short c0, unsigned short c1, unsigned short c2, unsigned short c3, unsigned short c4, unsigned short c5, unsigned short c6, unsigned short c7)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
	}

	RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
	}

	RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
	}

	// FIXME: Implement as Shuffle(x, y, Select(i0, ..., i16)) and Shuffle(x, y, SELECT_PACK_REPEAT(element))
//	RValue<UShort8> PackRepeat(RValue<Byte16> x, RValue<Byte16> y, int element)
//	{
//		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
//	}

	Type *UShort8::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
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
		auto oldValue = val.loadValue();
		auto newValue = ::function->makeVariable(Ice::IceType_i32);
		auto inc = Ice::InstArithmetic::create(::function, Ice::InstArithmetic::Add, newValue, oldValue, ::context->getConstantInt32(1));
		::basicBlock->appendInst(inc);
		val.storeValue(V(newValue));

		return RValue<Int>(oldValue);
	}

	const Int &operator++(const Int &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED"); return val;
	}

	RValue<Int> operator--(const Int &val, int)   // Post-decrement
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	const Int &operator--(const Int &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED"); return val;
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	Type *Int::getType()
	{
		return T(Ice::IceType_i32);
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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Long1::Long1(const RValue<UInt> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

	Long1::Long1(RValue<Long1> rhs)
	{
		storeValue(rhs.value);
	}

	Type *Long1::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	RValue<Long2> UnpackHigh(RValue<Long2> x, RValue<Long2> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Long2>(V(nullptr));
	}

	Type *Long2::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
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
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<UInt>(V(nullptr));
	}

	const UInt &operator++(const UInt &val)   // Pre-increment
	{
		assert(false && "UNIMPLEMENTED"); return val;
	}

	RValue<UInt> operator--(const UInt &val, int)   // Post-decrement
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt>(V(nullptr));
	}

	const UInt &operator--(const UInt &val)   // Pre-decrement
	{
		assert(false && "UNIMPLEMENTED"); return val;
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
//		assert(false && "UNIMPLEMENTED"); return RValue<UInt>(V(nullptr));
//	}

	Type *UInt::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
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
		Value *element = Nucleus::createExtractElement(long2, Long2::getType(), 0);
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

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator-(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator|(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator^(RValue<Int2> lhs, RValue<Int2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator<<(RValue<Int2> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Int2> operator>>(RValue<Int2> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	RValue<Long1> UnpackLow(RValue<Int2> x, RValue<Int2> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Long1>(V(nullptr));
	}

	RValue<Long1> UnpackHigh(RValue<Int2> x, RValue<Int2> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Long1>(V(nullptr));
	}

	RValue<Int> Extract(RValue<Int2> val, int i)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	RValue<Int2> Insert(RValue<Int2> val, RValue<Int> element, int i)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int2>(V(nullptr));
	}

	Type *Int2::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	UInt2::UInt2()
	{
	//	xy.parent = this;
	}

	UInt2::UInt2(unsigned int x, unsigned int y)
	{
	//	xy.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator-(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator|(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator^(RValue<UInt2> lhs, RValue<UInt2> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator<<(RValue<UInt2> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
	}

	RValue<UInt2> operator>>(RValue<UInt2> lhs, RValue<Long1> rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt2>(V(nullptr));
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
		return RValue<UInt2>(Nucleus::createNot(val.value));
	}

	Type *UInt2::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Int4::Int4(RValue<Byte4> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

	Int4::Int4(RValue<SByte4> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

	Int4::Int4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		Value *xyzw = Nucleus::createFPToSI(cast.value, Int4::getType());

		storeValue(xyzw);
	}

	Int4::Int4(RValue<Short4> cast)
	{
		assert(false && "UNIMPLEMENTED");
	}

	Int4::Int4(RValue<UShort4> cast)
	{
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED");
	}

	Int4::Int4(RValue<Int> rhs)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Int4> RoundInt(RValue<Float4> cast)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Int4>(V(nullptr));
	}

	RValue<Short8> Pack(RValue<Int4> x, RValue<Int4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Short8>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	RValue<Int4> Swizzle(RValue<Int4> x, unsigned char select)
	{
		return RValue<Int4>(createSwizzle4(x.value, select));
	}

	Type *Int4::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	UInt4::UInt4(RValue<Float4> cast)
	{
	//	xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<UInt4>(V(nullptr));
	}

	RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<UInt4>(V(nullptr));
	}

	RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UInt4>(V(nullptr));
	}

	RValue<UShort8> Pack(RValue<UInt4> x, RValue<UInt4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<UShort8>(V(nullptr));
	}

	Type *UInt4::getType()
	{
		assert(false && "UNIMPLEMENTED"); return nullptr;
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
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> RcpSqrt_pp(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Sqrt(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Round(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Trunc(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Frac(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Floor(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	RValue<Float> Ceil(RValue<Float> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float>(V(nullptr));
	}

	Type *Float::getType()
	{
		return T(Ice::IceType_f32);
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
		assert(false && "UNIMPLEMENTED"); return nullptr;
	}

	Float4::Float4(RValue<Byte4> cast)
	{
		xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
	}

	Float4::Float4(RValue<SByte4> cast)
	{
		xyzw.parent = this;

		assert(false && "UNIMPLEMENTED");
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

		assert(false && "UNIMPLEMENTED");
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
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> RcpSqrt_pp(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Sqrt(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> UnpackLow(RValue<Float4> x, RValue<Float4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> UnpackHigh(RValue<Float4> x, RValue<Float4> y)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
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
		assert(false && "UNIMPLEMENTED"); return RValue<Int>(V(nullptr));
	}

	RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOEQ(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLT(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpONE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGE(x.value, y.value), Int4::getType()));
	}

	RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y)
	{
		return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGT(x.value, y.value), Int4::getType()));
	}

	RValue<Float4> Round(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Trunc(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Frac(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Floor(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	RValue<Float4> Ceil(RValue<Float4> x)
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Float4>(V(nullptr));
	}

	Type *Float4::getType()
	{
		return T(Ice::IceType_v4f32);
	}

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, int offset)
	{
		return lhs + RValue<Int>(Nucleus::createConstantInt(offset));
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
		Ice::Operand *Ret = Ice::ConstantInteger32::create(::context, Ice::IceType_i32, ret ? 1 : 0);
		Ice::InstRet *retu = Ice::InstRet::create(::function, Ret);
		::basicBlock->appendInst(retu);
	}

	void Return(const Int &ret)
	{
		Ice::InstRet *retu = Ice::InstRet::create(::function, ret.loadValue());
		::basicBlock->appendInst(retu);
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
		Nucleus::setInsertBlock(falseBB);

		return true;
	}

	RValue<Long> Ticks()
	{
		assert(false && "UNIMPLEMENTED"); return RValue<Long>(V(nullptr));
	}
}

