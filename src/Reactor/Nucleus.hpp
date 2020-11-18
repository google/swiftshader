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

#ifndef rr_Nucleus_hpp
#define rr_Nucleus_hpp

#include <atomic>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifdef None
#	undef None  // TODO(b/127920555)
#endif

static_assert(sizeof(short) == 2, "Reactor's 'Short' type is 16-bit, and requires the C++ 'short' to match that.");
static_assert(sizeof(int) == 4, "Reactor's 'Int' type is 32-bit, and requires the C++ 'int' to match that.");

namespace rr {

class Type;
class Value;
class SwitchCases;
class BasicBlock;
class Routine;

// Optimization holds the optimization settings for code generation.
class Optimization
{
public:
	enum class Level
	{
		None,
		Less,
		Default,
		Aggressive,
	};

	enum class Pass
	{
		Disabled,
		InstructionCombining,
		CFGSimplification,
		LICM,
		AggressiveDCE,
		GVN,
		Reassociate,
		DeadStoreElimination,
		SCCP,
		ScalarReplAggregates,
		EarlyCSEPass,

		Count,
	};

	using Passes = std::vector<Pass>;

	Optimization(Level level = Level::Default, const Passes &passes = {})
	    : level(level)
	    , passes(passes)
	{
#if defined(REACTOR_DEFAULT_OPT_LEVEL)
		{
			this->level = Level::REACTOR_DEFAULT_OPT_LEVEL;
		}
#endif
	}

	Level getLevel() const { return level; }
	const Passes &getPasses() const { return passes; }

private:
	Level level = Level::Default;
	Passes passes;
};

// Config holds the Reactor configuration settings.
class Config
{
public:
	// Edit holds a number of modifications to a config, that can be applied
	// on an existing Config to produce a new Config with the specified
	// changes.
	class Edit
	{
	public:
		static const Edit None;

		Edit &set(Optimization::Level level)
		{
			optLevel = level;
			optLevelChanged = true;
			return *this;
		}
		Edit &add(Optimization::Pass pass)
		{
			optPassEdits.push_back({ ListEdit::Add, pass });
			return *this;
		}
		Edit &remove(Optimization::Pass pass)
		{
			optPassEdits.push_back({ ListEdit::Remove, pass });
			return *this;
		}
		Edit &clearOptimizationPasses()
		{
			optPassEdits.push_back({ ListEdit::Clear, Optimization::Pass::Disabled });
			return *this;
		}

		Config apply(const Config &cfg) const;

	private:
		enum class ListEdit
		{
			Add,
			Remove,
			Clear
		};
		using OptPassesEdit = std::pair<ListEdit, Optimization::Pass>;

		template<typename T>
		void apply(const std::vector<std::pair<ListEdit, T>> &edits, std::vector<T> &list) const;

		Optimization::Level optLevel;
		bool optLevelChanged = false;
		std::vector<OptPassesEdit> optPassEdits;
	};

	Config() = default;
	Config(const Optimization &optimization)
	    : optimization(optimization)
	{}

	const Optimization &getOptimization() const { return optimization; }

private:
	Optimization optimization;
};

class Nucleus
{
public:
	Nucleus();

	virtual ~Nucleus();

	// Default configuration to use when no other configuration is specified.
	// The new configuration will be applied to subsequent reactor calls.
	static void setDefaultConfig(const Config &cfg);
	static void adjustDefaultConfig(const Config::Edit &cfgEdit);
	static Config getDefaultConfig();

	std::shared_ptr<Routine> acquireRoutine(const char *name, const Config::Edit &cfgEdit = Config::Edit::None);

	static Value *allocateStackVariable(Type *type, int arraySize = 0);
	static BasicBlock *createBasicBlock();
	static BasicBlock *getInsertBlock();
	static void setInsertBlock(BasicBlock *basicBlock);

	static void createFunction(Type *returnType, const std::vector<Type *> &paramTypes);
	static Value *getArgument(unsigned int index);

	// Coroutines
	using CoroutineHandle = void *;

	template<typename... ARGS>
	using CoroutineBegin = CoroutineHandle(ARGS...);
	using CoroutineAwait = bool(CoroutineHandle, void *yieldValue);
	using CoroutineDestroy = void(CoroutineHandle);

	enum CoroutineEntries
	{
		CoroutineEntryBegin = 0,
		CoroutineEntryAwait,
		CoroutineEntryDestroy,
		CoroutineEntryCount
	};

	// Begins the generation of the three coroutine functions: CoroutineBegin, CoroutineAwait, and CoroutineDestroy,
	// which will be returned by Routine::getEntry() with arg CoroutineEntryBegin, CoroutineEntryAwait, and CoroutineEntryDestroy
	// respectively. Called by Coroutine constructor.
	// Params are used to generate the params to CoroutineBegin, while ReturnType is used as the YieldType for the coroutine,
	// returned via CoroutineAwait..
	static void createCoroutine(Type *returnType, const std::vector<Type *> &params);
	// Generates code to store the passed in value, and to suspend execution of the coroutine, such that the next call to
	// CoroutineAwait can set the output yieldValue and resume execution of the coroutine.
	static void yield(Value *val);
	// Called to finalize coroutine creation. After this call, Routine::getEntry can be called to retrieve the entry point to any
	// of the three coroutine functions. Called by Coroutine::finalize.
	std::shared_ptr<Routine> acquireCoroutine(const char *name, const Config::Edit &cfg = Config::Edit::None);
	// Called by Coroutine::operator() to execute CoroutineEntryBegin wrapped up in func. This is needed in case
	// the call must be run on a separate thread of execution (e.g. on a fiber).
	static CoroutineHandle invokeCoroutineBegin(Routine &routine, std::function<CoroutineHandle()> func);

	// Terminators
	static void createRetVoid();
	static void createRet(Value *V);
	static void createBr(BasicBlock *dest);
	static void createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse);

	// Binary operators
	static Value *createAdd(Value *lhs, Value *rhs);
	static Value *createSub(Value *lhs, Value *rhs);
	static Value *createMul(Value *lhs, Value *rhs);
	static Value *createUDiv(Value *lhs, Value *rhs);
	static Value *createSDiv(Value *lhs, Value *rhs);
	static Value *createFAdd(Value *lhs, Value *rhs);
	static Value *createFSub(Value *lhs, Value *rhs);
	static Value *createFMul(Value *lhs, Value *rhs);
	static Value *createFDiv(Value *lhs, Value *rhs);
	static Value *createURem(Value *lhs, Value *rhs);
	static Value *createSRem(Value *lhs, Value *rhs);
	static Value *createFRem(Value *lhs, Value *rhs);
	static Value *createShl(Value *lhs, Value *rhs);
	static Value *createLShr(Value *lhs, Value *rhs);
	static Value *createAShr(Value *lhs, Value *rhs);
	static Value *createAnd(Value *lhs, Value *rhs);
	static Value *createOr(Value *lhs, Value *rhs);
	static Value *createXor(Value *lhs, Value *rhs);

	// Unary operators
	static Value *createNeg(Value *V);
	static Value *createFNeg(Value *V);
	static Value *createNot(Value *V);

	// Memory instructions
	static Value *createLoad(Value *ptr, Type *type, bool isVolatile = false, unsigned int alignment = 0, bool atomic = false, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createStore(Value *value, Value *ptr, Type *type, bool isVolatile = false, unsigned int aligment = 0, bool atomic = false, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createGEP(Value *ptr, Type *type, Value *index, bool unsignedIndex);

	// Masked Load / Store instructions
	static Value *createMaskedLoad(Value *base, Type *elementType, Value *mask, unsigned int alignment, bool zeroMaskedLanes);
	static void createMaskedStore(Value *base, Value *value, Value *mask, unsigned int alignment);

	// Barrier instructions
	static void createFence(std::memory_order memoryOrder);

	// Atomic instructions
	static Value *createAtomicAdd(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicSub(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicAnd(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicOr(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicXor(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicMin(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicMax(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicUMin(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicUMax(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicExchange(Value *ptr, Value *value, std::memory_order memoryOrder = std::memory_order_relaxed);
	static Value *createAtomicCompareExchange(Value *ptr, Value *value, Value *compare, std::memory_order memoryOrderEqual, std::memory_order memoryOrderUnequal);

	// Cast/Conversion Operators
	static Value *createTrunc(Value *V, Type *destType);
	static Value *createZExt(Value *V, Type *destType);
	static Value *createSExt(Value *V, Type *destType);
	static Value *createFPToUI(Value *V, Type *destType);
	static Value *createFPToSI(Value *V, Type *destType);
	static Value *createSIToFP(Value *V, Type *destType);
	static Value *createFPTrunc(Value *V, Type *destType);
	static Value *createFPExt(Value *V, Type *destType);
	static Value *createBitCast(Value *V, Type *destType);

	// Compare instructions
	static Value *createPtrEQ(Value *lhs, Value *rhs);
	static Value *createICmpEQ(Value *lhs, Value *rhs);
	static Value *createICmpNE(Value *lhs, Value *rhs);
	static Value *createICmpUGT(Value *lhs, Value *rhs);
	static Value *createICmpUGE(Value *lhs, Value *rhs);
	static Value *createICmpULT(Value *lhs, Value *rhs);
	static Value *createICmpULE(Value *lhs, Value *rhs);
	static Value *createICmpSGT(Value *lhs, Value *rhs);
	static Value *createICmpSGE(Value *lhs, Value *rhs);
	static Value *createICmpSLT(Value *lhs, Value *rhs);
	static Value *createICmpSLE(Value *lhs, Value *rhs);
	static Value *createFCmpOEQ(Value *lhs, Value *rhs);
	static Value *createFCmpOGT(Value *lhs, Value *rhs);
	static Value *createFCmpOGE(Value *lhs, Value *rhs);
	static Value *createFCmpOLT(Value *lhs, Value *rhs);
	static Value *createFCmpOLE(Value *lhs, Value *rhs);
	static Value *createFCmpONE(Value *lhs, Value *rhs);
	static Value *createFCmpORD(Value *lhs, Value *rhs);
	static Value *createFCmpUNO(Value *lhs, Value *rhs);
	static Value *createFCmpUEQ(Value *lhs, Value *rhs);
	static Value *createFCmpUGT(Value *lhs, Value *rhs);
	static Value *createFCmpUGE(Value *lhs, Value *rhs);
	static Value *createFCmpULT(Value *lhs, Value *rhs);
	static Value *createFCmpULE(Value *lhs, Value *rhs);
	static Value *createFCmpUNE(Value *lhs, Value *rhs);

	// Vector instructions
	static Value *createExtractElement(Value *vector, Type *type, int index);
	static Value *createInsertElement(Value *vector, Value *element, int index);
	static Value *createShuffleVector(Value *V1, Value *V2, const int *select);

	// Other instructions
	static Value *createSelect(Value *C, Value *ifTrue, Value *ifFalse);
	static SwitchCases *createSwitch(Value *control, BasicBlock *defaultBranch, unsigned numCases);
	static void addSwitchCase(SwitchCases *switchCases, int label, BasicBlock *branch);
	static void createUnreachable();

	// Constant values
	static Value *createNullValue(Type *type);
	static Value *createConstantLong(int64_t i);
	static Value *createConstantInt(int i);
	static Value *createConstantInt(unsigned int i);
	static Value *createConstantBool(bool b);
	static Value *createConstantByte(signed char i);
	static Value *createConstantByte(unsigned char i);
	static Value *createConstantShort(short i);
	static Value *createConstantShort(unsigned short i);
	static Value *createConstantFloat(float x);
	static Value *createNullPointer(Type *type);
	static Value *createConstantVector(const int64_t *constants, Type *type);
	static Value *createConstantVector(const double *constants, Type *type);
	static Value *createConstantString(const char *v);
	static Value *createConstantString(const std::string &v) { return createConstantString(v.c_str()); }

	static Type *getType(Value *value);
	static Type *getContainedType(Type *vectorType);
	static Type *getPointerType(Type *elementType);
	static Type *getPrintfStorageType(Type *valueType);
};

}  // namespace rr

#endif  // rr_Nucleus_hpp
