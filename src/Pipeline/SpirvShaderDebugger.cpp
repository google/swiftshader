// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "SpirvShader.hpp"

// If enabled, each instruction will be printed before processing.
#define PRINT_EACH_PROCESSED_INSTRUCTION 0
// If enabled, each instruction will be printed before executing.
#define PRINT_EACH_EXECUTED_INSTRUCTION 0

#ifdef ENABLE_VK_DEBUGGER

#	include "Vulkan/Debug/Context.hpp"
#	include "Vulkan/Debug/File.hpp"
#	include "Vulkan/Debug/Thread.hpp"
#	include "Vulkan/Debug/Variable.hpp"

#	include "spirv-tools/ext/OpenCLDebugInfo100.h"
#	include "spirv-tools/libspirv.h"

#	include <algorithm>

namespace {

// ArgTy<F>::type resolves to the single argument type of the function F.
template<typename F>
struct ArgTy
{
	using type = typename ArgTy<decltype(&F::operator())>::type;
};

template<typename R, typename C, typename Arg>
struct ArgTy<R (C::*)(Arg) const>
{
	using type = typename std::decay<Arg>::type;
};

template<typename T>
using ArgTyT = typename ArgTy<T>::type;

}  // anonymous namespace

namespace spvtools {

// Function implemented in third_party/SPIRV-Tools/source/disassemble.cpp
// but with no public header.
// This is a C++ function, so the name is mangled, and signature changes will
// result in a linker error instead of runtime signature mismatches.
extern std::string spvInstructionBinaryToText(const spv_target_env env,
                                              const uint32_t *inst_binary,
                                              const size_t inst_word_count,
                                              const uint32_t *binary,
                                              const size_t word_count,
                                              const uint32_t options);

}  // namespace spvtools

namespace {

const char *laneNames[] = { "Lane 0", "Lane 1", "Lane 2", "Lane 3" };
static_assert(sizeof(laneNames) / sizeof(laneNames[0]) == sw::SIMD::Width,
              "laneNames must have SIMD::Width entries");

template<typename T>
std::string tostring(const T &s)
{
	return std::to_string(s);
}
std::string tostring(char *s)
{
	return s;
}
std::string tostring(const char *s)
{
	return s;
}
std::string tostring(sw::SpirvShader::Object::ID id)
{
	return "%" + std::to_string(id.value());
}

////////////////////////////////////////////////////////////////////////////////
// OpenCL.Debug.100 data structures
////////////////////////////////////////////////////////////////////////////////
namespace debug {

struct Member;

struct Object
{
	enum class Kind
	{
		Object,
		Declare,
		Expression,
		Function,
		InlinedAt,
		LocalVariable,
		Member,
		Operation,
		Source,
		SourceScope,
		Value,

		// Scopes
		CompilationUnit,
		LexicalBlock,

		// Types
		BasicType,
		VectorType,
		FunctionType,
		CompositeType,
	};

	using ID = sw::SpirvID<Object>;
	static constexpr auto KIND = Kind::Object;
	inline Object(Kind kind)
	    : kind(kind)
	{}
	const Kind kind;

	// kindof() returns true iff kind is of this type, or any type deriving from
	// this type.
	static constexpr bool kindof(Object::Kind kind) { return true; }
};

// cstr() returns the c-string name of the given Object::Kind.
constexpr const char *cstr(Object::Kind k)
{
	switch(k)
	{
		case Object::Kind::Object: return "Object";
		case Object::Kind::Declare: return "Declare";
		case Object::Kind::Expression: return "Expression";
		case Object::Kind::Function: return "Function";
		case Object::Kind::InlinedAt: return "InlinedAt";
		case Object::Kind::LocalVariable: return "LocalVariable";
		case Object::Kind::Member: return "Member";
		case Object::Kind::Operation: return "Operation";
		case Object::Kind::Source: return "Source";
		case Object::Kind::SourceScope: return "SourceScope";
		case Object::Kind::Value: return "Value";
		case Object::Kind::CompilationUnit: return "CompilationUnit";
		case Object::Kind::LexicalBlock: return "LexicalBlock";
		case Object::Kind::BasicType: return "BasicType";
		case Object::Kind::VectorType: return "VectorType";
		case Object::Kind::FunctionType: return "FunctionType";
		case Object::Kind::CompositeType: return "CompositeType";
	}
	return "<unknown>";
}

template<typename TYPE_, typename BASE, Object::Kind KIND_>
struct ObjectImpl : public BASE
{
	using ID = sw::SpirvID<TYPE_>;
	static constexpr auto KIND = KIND_;

	ObjectImpl()
	    : BASE(KIND)
	{}
	static_assert(BASE::kindof(KIND), "BASE::kindof() returned false");

	// kindof() returns true iff kind is of this type, or any type deriving from
	// this type.
	static constexpr bool kindof(Object::Kind kind) { return kind == KIND; }
};

template<typename TO, typename FROM>
TO *cast(FROM *obj)
{
	if(obj == nullptr) { return nullptr; }  // None
	return (TO::kindof(obj->kind)) ? static_cast<TO *>(obj) : nullptr;
}

template<typename TO, typename FROM>
const TO *cast(const FROM *obj)
{
	if(obj == nullptr) { return nullptr; }  // None
	return (TO::kindof(obj->kind)) ? static_cast<const TO *>(obj) : nullptr;
}

struct Scope : public Object
{
	// Root represents the root stack frame scope.
	static const Scope Root;

	using ID = sw::SpirvID<Scope>;
	inline Scope(Kind kind)
	    : Object(kind)
	{}

	// kindof() returns true iff kind is of this type, or any type deriving from
	// this type.
	static constexpr bool kindof(Kind kind)
	{
		return kind == Kind::CompilationUnit ||
		       kind == Kind::Function ||
		       kind == Kind::LexicalBlock;
	}

	struct Source *source = nullptr;
	Scope *parent = nullptr;
};

struct Type : public Object
{
	using ID = sw::SpirvID<Type>;
	inline Type(Kind kind)
	    : Object(kind)
	{}

	// kindof() returns true iff kind is of this type, or any type deriving from
	// this type.
	static constexpr bool kindof(Kind kind)
	{
		return kind == Kind::BasicType ||
		       kind == Kind::VectorType ||
		       kind == Kind::FunctionType ||
		       kind == Kind::CompositeType;
	}
};

struct CompilationUnit : ObjectImpl<CompilationUnit, Scope, Object::Kind::CompilationUnit>
{
};

struct Source : ObjectImpl<Source, Object, Object::Kind::Source>
{
	spv::SourceLanguage language;
	uint32_t version = 0;
	std::string file;
	std::string source;

	std::shared_ptr<vk::dbg::File> dbgFile;
};

struct BasicType : ObjectImpl<BasicType, Type, Object::Kind::BasicType>
{
	std::string name;
	uint32_t size = 0;  // in bits.
	OpenCLDebugInfo100DebugBaseTypeAttributeEncoding encoding = OpenCLDebugInfo100Unspecified;
};

struct VectorType : ObjectImpl<VectorType, Type, Object::Kind::VectorType>
{
	Type *base = nullptr;
	uint32_t components = 0;
};

struct FunctionType : ObjectImpl<FunctionType, Type, Object::Kind::FunctionType>
{
	uint32_t flags = 0;  // OR'd from OpenCLDebugInfo100DebugInfoFlags
	Type *returnTy = nullptr;
	std::vector<Type *> paramTys;
};

struct CompositeType : ObjectImpl<CompositeType, Type, Object::Kind::CompositeType>
{
	std::string name;
	OpenCLDebugInfo100DebugCompositeType tag = OpenCLDebugInfo100Class;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	Object *parent = nullptr;
	std::string linkage;
	uint32_t size = 0;   // in bits.
	uint32_t flags = 0;  // OR'd from OpenCLDebugInfo100DebugInfoFlags
	std::vector<Member *> members;
};

struct Member : ObjectImpl<Member, Object, Object::Kind::Member>
{
	std::string name;
	Type *type = nullptr;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	CompositeType *parent = nullptr;
	uint32_t offset = 0;  // in bits
	uint32_t size = 0;    // in bits
	uint32_t flags = 0;   // OR'd from OpenCLDebugInfo100DebugInfoFlags
};

struct Function : ObjectImpl<Function, Scope, Object::Kind::Function>
{
	std::string name;
	FunctionType *type = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	std::string linkage;
	uint32_t flags = 0;  // OR'd from OpenCLDebugInfo100DebugInfoFlags
	uint32_t scopeLine = 0;
	sw::SpirvShader::Function::ID function;
};

struct LexicalBlock : ObjectImpl<LexicalBlock, Scope, Object::Kind::LexicalBlock>
{
	uint32_t line = 0;
	uint32_t column = 0;
	std::string name;
};

struct InlinedAt : ObjectImpl<InlinedAt, Object, Object::Kind::InlinedAt>
{
	uint32_t line = 0;
	Scope *scope = nullptr;
	InlinedAt *inlined = nullptr;
};

struct SourceScope : ObjectImpl<SourceScope, Object, Object::Kind::SourceScope>
{
	Scope *scope = nullptr;
	InlinedAt *inlinedAt = nullptr;
};

struct LocalVariable : ObjectImpl<LocalVariable, Object, Object::Kind::LocalVariable>
{
	static constexpr uint32_t NoArg = ~uint32_t(0);

	std::string name;
	Type *type = nullptr;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	Scope *parent = nullptr;
	uint32_t arg = NoArg;
};

struct Operation : ObjectImpl<Operation, Object, Object::Kind::Operation>
{
	uint32_t opcode = 0;
	std::vector<uint32_t> operands;
};

struct Expression : ObjectImpl<Expression, Object, Object::Kind::Expression>
{
	std::vector<Operation *> operations;
};

struct Declare : ObjectImpl<Declare, Object, Object::Kind::Declare>
{
	LocalVariable *local = nullptr;
	sw::SpirvShader::Object::ID variable;
	Expression *expression = nullptr;
};

struct Value : ObjectImpl<Value, Object, Object::Kind::Value>
{
	LocalVariable *local = nullptr;
	sw::SpirvShader::Object::ID variable;
	Expression *expression = nullptr;
	std::vector<uint32_t> indexes;
};

const Scope Scope::Root = CompilationUnit{};

// find<T>() searches the nested scopes, returning for the first scope that is
// castable to type T. If no scope can be found of type T, then nullptr is
// returned.
template<typename T>
T *find(Scope *scope)
{
	if(auto out = cast<T>(scope)) { return out; }
	return scope->parent ? find<T>(scope->parent) : nullptr;
}

bool hasDebuggerScope(debug::Scope *spirvScope)
{
	return debug::cast<debug::Function>(spirvScope) != nullptr ||
	       debug::cast<debug::LexicalBlock>(spirvScope) != nullptr;
}

}  // namespace debug
}  // anonymous namespace

namespace rr {

////////////////////////////////////////////////////////////////////////////////
// rr::CToReactor<T> specializations.
////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct CToReactor<sw::SpirvID<T>>
{
	using type = rr::Int;
	static rr::Int cast(sw::SpirvID<T> id) { return rr::Int(id.value()); }
};

template<typename T>
struct CToReactor<vk::dbg::ID<T>>
{
	using type = rr::Int;
	static rr::Int cast(vk::dbg::ID<T> id) { return rr::Int(id.value()); }
};

}  // namespace rr

namespace sw {

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader::Impl::Debugger
//
// Private struct holding debugger information for the SpirvShader.
////////////////////////////////////////////////////////////////////////////////
struct SpirvShader::Impl::Debugger
{
	class Group;
	class State;

	enum class Pass
	{
		Define,
		Emit
	};

	void process(const SpirvShader *shader, const InsnIterator &insn, EmitState *state, Pass pass);

	void setLocation(EmitState *state, const std::shared_ptr<vk::dbg::File> &, int line, int column);
	void setLocation(EmitState *state, const std::string &path, int line, int column);

	// exposeVariable exposes the variable with the given ID to the debugger
	// using the specified key.
	template<typename Key>
	void exposeVariable(
	    const SpirvShader *shader,
	    const Key &key,
	    const debug::Scope *scope,
	    const debug::Type *type,
	    Object::ID id,
	    EmitState *state) const;

	// exposeVariable exposes the variable with the given ID to the
	// debugger under the specified group, for the specified SIMD lane.
	template<typename Key>
	void exposeVariable(
	    const SpirvShader *shader,
	    const Group &group,
	    int lane,
	    const Key &key,
	    const debug::Type *type,
	    Object::ID id,
	    EmitState *state,
	    int wordOffset = 0) const;

	std::shared_ptr<vk::dbg::Context> ctx;
	std::shared_ptr<vk::dbg::File> spirvFile;
	std::unordered_map<const void *, int> spirvLineMappings;  // instruction pointer to line
	std::unordered_map<const void *, Object::ID> results;     // instruction pointer to result ID

private:
	// add() registers the debug object with the given id.
	template<typename ID, typename T>
	void add(ID id, T *);

	// addNone() registers given id as a None value or type.
	void addNone(debug::Object::ID id);

	// get() returns the debug object with the given id.
	// The object must exist and be of type (or derive from type) T.
	// A returned nullptr represents a None value or type.
	template<typename T>
	T *get(SpirvID<T> id) const;

	// use get() and add() to access this
	std::unordered_map<debug::Object::ID, std::unique_ptr<debug::Object>> objects;

	// defineOrEmit() when called in Pass::Define, creates and stores a
	// zero-initialized object into the Debugger::objects map using the
	// object identifier held by second instruction operand.
	// When called in Pass::Emit, defineOrEmit() calls the function F with the
	// previously-built object.
	//
	// F must be a function with the signature:
	//   void(OBJECT_TYPE *)
	//
	// The object type is automatically inferred from the function signature.
	template<typename F, typename T = typename std::remove_pointer<ArgTyT<F>>::type>
	void defineOrEmit(InsnIterator insn, Pass pass, F &&emit);

	std::unordered_map<std::string, std::shared_ptr<vk::dbg::File>> files;
};

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader::Impl::Debugger::State
//
// State holds the runtime data structures for the shader debug session.
////////////////////////////////////////////////////////////////////////////////
class SpirvShader::Impl::Debugger::State
{
public:
	static State *create(const Debugger *debugger, const char *name);
	static void destroy(State *);

	State(const Debugger *debugger, const char *stackBase, vk::dbg::Context::Lock &lock);
	~State();

	void enter(vk::dbg::Context::Lock &lock, const char *name);
	void exit();
	void updateActiveLaneMask(int lane, bool enabled);
	void updateLocation(vk::dbg::File::ID file, int line, int column);
	void createScope(const debug::Scope *);
	void setScope(debug::SourceScope *newScope);

	vk::dbg::VariableContainer *hovers(const debug::Scope *);
	vk::dbg::VariableContainer *localsLane(const debug::Scope *, int lane);
	vk::dbg::VariableContainer *builtinsLane(int lane);

	template<typename K>
	vk::dbg::VariableContainer *group(vk::dbg::VariableContainer *vc, K key);

	template<typename K, typename V>
	void putVal(vk::dbg::VariableContainer *vc, K key, V value);

	template<typename K, typename V>
	void putRef(vk::dbg::VariableContainer *vc, K key, V *ptr);

	// Scopes holds pointers to the vk::dbg::Scopes for local variables, hover
	// variables and the locals indexed by SIMD lane.
	struct Scopes
	{
		std::shared_ptr<vk::dbg::Scope> locals;
		std::shared_ptr<vk::dbg::Scope> hovers;
		std::array<std::shared_ptr<vk::dbg::VariableContainer>, sw::SIMD::Width> localsByLane;
	};

	// getScopes() returns the Scopes object for the given debug::Scope.
	const Scopes &getScopes(const debug::Scope *scope);

	const Debugger *debugger;
	const std::shared_ptr<vk::dbg::Thread> thread;
	std::unordered_map<const debug::Scope *, Scopes> scopes;
	Scopes rootScopes;                                                                        // Scopes for the root stack frame.
	std::array<std::shared_ptr<vk::dbg::VariableContainer>, sw::SIMD::Width> builtinsByLane;  // Scopes for builtin varibles (shared by all shader frames).
	debug::SourceScope *srcScope = nullptr;                                                   // Current source scope.
};

SpirvShader::Impl::Debugger::State *SpirvShader::Impl::Debugger::State::create(const Debugger *debugger, const char *name)
{
	auto lock = debugger->ctx->lock();
	return new State(debugger, name, lock);
}

void SpirvShader::Impl::Debugger::State::destroy(State *state)
{
	delete state;
}

SpirvShader::Impl::Debugger::State::State(const Debugger *debugger, const char *stackBase, vk::dbg::Context::Lock &lock)
    : debugger(debugger)
    , thread(lock.currentThread())
{
	enter(lock, stackBase);

	for(int i = 0; i < sw::SIMD::Width; i++)
	{
		builtinsByLane[i] = lock.createVariableContainer();
	}

	thread->update([&](vk::dbg::Frame &frame) {
		rootScopes.locals = frame.locals;
		rootScopes.hovers = frame.hovers;
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			auto locals = lock.createVariableContainer();
			locals->extend(builtinsByLane[i]);
			frame.locals->variables->put(laneNames[i], locals);
			rootScopes.localsByLane[i] = locals;
		}
	});
}

SpirvShader::Impl::Debugger::State::~State()
{
	exit();
}

void SpirvShader::Impl::Debugger::State::enter(vk::dbg::Context::Lock &lock, const char *name)
{
	thread->enter(lock, debugger->spirvFile, name);
}

void SpirvShader::Impl::Debugger::State::exit()
{
	thread->exit();
}

void SpirvShader::Impl::Debugger::State::updateActiveLaneMask(int lane, bool enabled)
{
	rootScopes.localsByLane[lane]->put("enabled", vk::dbg::make_constant(enabled));
}

void SpirvShader::Impl::Debugger::State::updateLocation(vk::dbg::File::ID fileID, int line, int column)
{
	auto file = debugger->ctx->lock().get(fileID);
	thread->update([&](vk::dbg::Frame &frame) {
		frame.location = { file, line, column };
	});
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::hovers(const debug::Scope *scope)
{
	return getScopes(scope).hovers->variables.get();
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::localsLane(const debug::Scope *scope, int i)
{
	return getScopes(scope).localsByLane[i].get();
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::builtinsLane(int i)
{
	return builtinsByLane[i].get();
}

template<typename K>
vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::group(vk::dbg::VariableContainer *vc, K key)
{
	auto out = debugger->ctx->lock().createVariableContainer();
	vc->put(tostring(key), out);
	return out.get();
}

template<typename K, typename V>
void SpirvShader::Impl::Debugger::State::putVal(vk::dbg::VariableContainer *vc, K key, V value)
{
	vc->put(tostring(key), vk::dbg::make_constant(value));
}

template<typename K, typename V>
void SpirvShader::Impl::Debugger::State::putRef(vk::dbg::VariableContainer *vc, K key, V *ptr)
{
	vc->put(tostring(key), vk::dbg::make_reference(*ptr));
}

void SpirvShader::Impl::Debugger::State::createScope(const debug::Scope *spirvScope)
{
	// TODO(b/151338669): We're creating scopes per-shader invocation.
	// This is all really static information, and should only be created
	// once *per program*.

	ASSERT(spirvScope != nullptr);

	auto lock = debugger->ctx->lock();
	Scopes s = {};
	s.locals = lock.createScope(spirvScope->source->dbgFile);
	s.hovers = lock.createScope(spirvScope->source->dbgFile);

	for(int i = 0; i < sw::SIMD::Width; i++)
	{
		auto locals = lock.createVariableContainer();
		s.localsByLane[i] = locals;
		s.locals->variables->put(laneNames[i], locals);
	}

	if(hasDebuggerScope(spirvScope->parent))
	{
		auto parent = getScopes(spirvScope->parent);
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			s.localsByLane[i]->extend(parent.localsByLane[i]);
		}
		s.hovers->variables->extend(parent.hovers->variables);
	}
	else
	{
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			s.localsByLane[i]->extend(builtinsByLane[i]);
		}
	}

	scopes.emplace(spirvScope, std::move(s));
}

void SpirvShader::Impl::Debugger::State::setScope(debug::SourceScope *newSrcScope)
{
	auto oldSrcScope = srcScope;
	if(oldSrcScope == newSrcScope) { return; }
	srcScope = newSrcScope;

	if(hasDebuggerScope(srcScope->scope))
	{
		auto lock = debugger->ctx->lock();
		auto thread = lock.currentThread();

		debug::Function *oldFunction = oldSrcScope ? debug::find<debug::Function>(oldSrcScope->scope) : nullptr;
		debug::Function *newFunction = newSrcScope ? debug::find<debug::Function>(newSrcScope->scope) : nullptr;

		if(oldFunction != newFunction)
		{
			if(oldFunction) { thread->exit(); }
			if(newFunction) { thread->enter(lock, newFunction->source->dbgFile, newFunction->name); }
		}

		auto dbgScope = getScopes(srcScope->scope);
		thread->update([&](vk::dbg::Frame &frame) {
			frame.locals = dbgScope.locals;
			frame.hovers = dbgScope.hovers;
		});
	}
}

const SpirvShader::Impl::Debugger::State::Scopes &SpirvShader::Impl::Debugger::State::getScopes(const debug::Scope *scope)
{
	if(scope == &debug::Scope::Root)
	{
		return rootScopes;
	}

	auto dbgScopeIt = scopes.find(scope);
	ASSERT_MSG(dbgScopeIt != scopes.end(),
	           "createScope() not called for debug::Scope %s %p",
	           cstr(scope->kind), scope);
	return dbgScopeIt->second;
}

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader::Impl::Debugger::Group
//
// This provides a convenient C++ interface for adding debugger values to
// VariableContainers.
////////////////////////////////////////////////////////////////////////////////
class SpirvShader::Impl::Debugger::Group
{
public:
	using Ptr = rr::Pointer<rr::Byte>;

	static Group hovers(Ptr state, const debug::Scope *scope);
	static Group locals(Ptr state, const debug::Scope *scope);
	static Group localsLane(Ptr state, const debug::Scope *scope, int lane);
	static Group builtinsLane(Ptr state, int lane);

	Group(Ptr state, Ptr group);

	template<typename K, typename RK>
	Group group(RK key) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV value) const;

	template<typename K, typename V, typename RK>
	void putRef(RK key, RValue<Pointer<Byte>> ref) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV x, RV y) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV x, RV y, RV z) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV x, RV y, RV z, RV w) const;

	template<typename K, typename V, typename VEC>
	void putVec3(K key, const VEC &v) const;

private:
	Ptr state;
	Ptr ptr;
};

SpirvShader::Impl::Debugger::Group
SpirvShader::Impl::Debugger::Group::hovers(Ptr state, const debug::Scope *scope)
{
	return Group(state, rr::Call(&State::hovers, state, scope));
}

SpirvShader::Impl::Debugger::Group
SpirvShader::Impl::Debugger::Group::localsLane(Ptr state, const debug::Scope *scope, int lane)
{
	return Group(state, rr::Call(&State::localsLane, state, scope, lane));
}

SpirvShader::Impl::Debugger::Group
SpirvShader::Impl::Debugger::Group::builtinsLane(Ptr state, int lane)
{
	return Group(state, rr::Call(&State::builtinsLane, state, lane));
}

SpirvShader::Impl::Debugger::Group::Group(Ptr state, Ptr group)
    : state(state)
    , ptr(group)
{}

template<typename K, typename RK>
SpirvShader::Impl::Debugger::Group SpirvShader::Impl::Debugger::Group::group(RK key) const
{
	return Group(state, rr::Call(&State::group<K>, state, ptr, key));
}

template<typename K, typename V, typename RK, typename RV>
void SpirvShader::Impl::Debugger::Group::put(RK key, RV value) const
{
	rr::Call(&State::putVal<K, V>, state, ptr, key, value);
}

template<typename K, typename V, typename RK>
void SpirvShader::Impl::Debugger::Group::putRef(RK key, RValue<Pointer<Byte>> ref) const
{
	rr::Call(&State::putRef<K, V>, state, ptr, key, ref);
}

template<typename K, typename V, typename RK, typename RV>
void SpirvShader::Impl::Debugger::Group::put(RK key, RV x, RV y) const
{
	auto vec = group<K>(key);
	vec.template put<const char *, V>("x", x);
	vec.template put<const char *, V>("y", y);
}

template<typename K, typename V, typename RK, typename RV>
void SpirvShader::Impl::Debugger::Group::put(RK key, RV x, RV y, RV z) const
{
	auto vec = group<K>(key);
	vec.template put<const char *, V>("x", x);
	vec.template put<const char *, V>("y", y);
	vec.template put<const char *, V>("z", z);
}

template<typename K, typename V, typename RK, typename RV>
void SpirvShader::Impl::Debugger::Group::put(RK key, RV x, RV y, RV z, RV w) const
{
	auto vec = group<K>(key);
	vec.template put<const char *, V>("x", x);
	vec.template put<const char *, V>("y", y);
	vec.template put<const char *, V>("z", z);
	vec.template put<const char *, V>("w", w);
}

template<typename K, typename V, typename VEC>
void SpirvShader::Impl::Debugger::Group::putVec3(K key, const VEC &v) const
{
	auto vec = group<K>(key);
	vec.template put<const char *, V>("x", Extract(v, 0));
	vec.template put<const char *, V>("y", Extract(v, 1));
	vec.template put<const char *, V>("z", Extract(v, 2));
}

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader::Impl::Debugger methods
////////////////////////////////////////////////////////////////////////////////
template<typename F, typename T>
void SpirvShader::Impl::Debugger::defineOrEmit(InsnIterator insn, Pass pass, F &&emit)
{
	auto id = SpirvID<T>(insn.word(2));
	switch(pass)
	{
		case Pass::Define:
			add(id, new T());
			break;
		case Pass::Emit:
			emit(get<T>(id));
			break;
	}
}

void SpirvShader::Impl::Debugger::process(const SpirvShader *shader, const InsnIterator &insn, EmitState *state, Pass pass)
{
	auto dbg = shader->impl.debugger;
	auto extInstIndex = insn.word(4);
	switch(extInstIndex)
	{
		case OpenCLDebugInfo100DebugInfoNone:
			if(pass == Pass::Define)
			{
				addNone(debug::Object::ID(insn.word(2)));
			}
			break;
		case OpenCLDebugInfo100DebugCompilationUnit:
			defineOrEmit(insn, pass, [&](debug::CompilationUnit *cu) {
				cu->source = get(debug::Source::ID(insn.word(7)));
			});
			break;
		case OpenCLDebugInfo100DebugTypeBasic:
			defineOrEmit(insn, pass, [&](debug::BasicType *type) {
				type->name = shader->getString(insn.word(5));
				type->size = shader->GetConstScalarInt(insn.word(6));
				type->encoding = static_cast<OpenCLDebugInfo100DebugBaseTypeAttributeEncoding>(insn.word(7));
			});
			break;
		case OpenCLDebugInfo100DebugTypeVector:
			defineOrEmit(insn, pass, [&](debug::VectorType *type) {
				type->base = get(debug::Type::ID(insn.word(5)));
				type->components = insn.word(6);
			});
			break;
		case OpenCLDebugInfo100DebugTypeFunction:
			defineOrEmit(insn, pass, [&](debug::FunctionType *type) {
				type->flags = insn.word(5);
				type->returnTy = get(debug::Type::ID(insn.word(6)));
				for(uint32_t i = 7; i < insn.wordCount(); i++)
				{
					type->paramTys.push_back(get(debug::Type::ID(insn.word(i))));
				}
			});
			break;
		case OpenCLDebugInfo100DebugTypeComposite:
			defineOrEmit(insn, pass, [&](debug::CompositeType *type) {
				type->name = shader->getString(insn.word(5));
				type->tag = static_cast<OpenCLDebugInfo100DebugCompositeType>(insn.word(6));
				type->source = get(debug::Source::ID(insn.word(7)));
				type->line = insn.word(8);
				type->column = insn.word(9);
				type->parent = get(debug::Object::ID(insn.word(10)));
				type->linkage = shader->getString(insn.word(11));
				type->size = shader->GetConstScalarInt(insn.word(12));
				type->flags = insn.word(13);
				for(uint32_t i = 14; i < insn.wordCount(); i++)
				{
					auto obj = get(debug::Object::ID(insn.word(i)));
					if(auto member = debug::cast<debug::Member>(obj))  // Can also be Function or TypeInheritance, which we don't care about.
					{
						type->members.push_back(member);
					}
				}
			});
			break;
		case OpenCLDebugInfo100DebugTypeMember:
			defineOrEmit(insn, pass, [&](debug::Member *member) {
				member->name = shader->getString(insn.word(5));
				member->type = get(debug::Type::ID(insn.word(6)));
				member->source = get(debug::Source::ID(insn.word(7)));
				member->line = insn.word(8);
				member->column = insn.word(9);
				member->parent = get(debug::CompositeType::ID(insn.word(10)));
				member->offset = shader->GetConstScalarInt(insn.word(11));
				member->size = shader->GetConstScalarInt(insn.word(12));
				member->flags = insn.word(13);
			});
			break;
		case OpenCLDebugInfo100DebugFunction:
			defineOrEmit(insn, pass, [&](debug::Function *func) {
				func->name = shader->getString(insn.word(5));
				func->type = get(debug::FunctionType::ID(insn.word(6)));
				func->source = get(debug::Source::ID(insn.word(7)));
				func->line = insn.word(8);
				func->column = insn.word(9);
				func->parent = get(debug::Scope::ID(insn.word(10)));
				func->linkage = shader->getString(insn.word(11));
				func->flags = insn.word(12);
				func->scopeLine = insn.word(13);
				func->function = Function::ID(insn.word(14));
				// declaration: word(13)

				rr::Call(&State::createScope, state->routine->dbgState, func);
			});
			break;
		case OpenCLDebugInfo100DebugLexicalBlock:
			defineOrEmit(insn, pass, [&](debug::LexicalBlock *scope) {
				scope->source = get(debug::Source::ID(insn.word(5)));
				scope->line = insn.word(6);
				scope->column = insn.word(7);
				scope->parent = get(debug::Scope::ID(insn.word(8)));
				if(insn.wordCount() > 9)
				{
					scope->name = shader->getString(insn.word(9));
				}

				rr::Call(&State::createScope, state->routine->dbgState, scope);
			});
			break;
		case OpenCLDebugInfo100DebugScope:
			defineOrEmit(insn, pass, [&](debug::SourceScope *ss) {
				ss->scope = get(debug::Scope::ID(insn.word(5)));
				if(insn.wordCount() > 6)
				{
					ss->inlinedAt = get(debug::InlinedAt::ID(insn.word(6)));
				}

				rr::Call(&State::setScope, state->routine->dbgState, ss);
			});
			break;
		case OpenCLDebugInfo100DebugNoScope:
			break;
		case OpenCLDebugInfo100DebugInlinedAt:
			defineOrEmit(insn, pass, [&](debug::InlinedAt *ia) {
				ia->line = insn.word(5);
				ia->scope = get(debug::Scope::ID(insn.word(6)));
				if(insn.wordCount() > 7)
				{
					ia->inlined = get(debug::InlinedAt::ID(insn.word(7)));
				}
			});
			break;
		case OpenCLDebugInfo100DebugLocalVariable:
			defineOrEmit(insn, pass, [&](debug::LocalVariable *var) {
				var->name = shader->getString(insn.word(5));
				var->type = get(debug::Type::ID(insn.word(6)));
				var->source = get(debug::Source::ID(insn.word(7)));
				var->line = insn.word(8);
				var->column = insn.word(9);
				var->parent = get(debug::Scope::ID(insn.word(10)));
				if(insn.wordCount() > 11)
				{
					var->arg = insn.word(11);
				}
			});
			break;
		case OpenCLDebugInfo100DebugDeclare:
			defineOrEmit(insn, pass, [&](debug::Declare *decl) {
				decl->local = get(debug::LocalVariable::ID(insn.word(5)));
				decl->variable = Object::ID(insn.word(6));
				decl->expression = get(debug::Expression::ID(insn.word(7)));
				exposeVariable(
				    shader,
				    decl->local->name.c_str(),
				    decl->local->parent,
				    decl->local->type,
				    decl->variable,
				    state);
			});
			break;
		case OpenCLDebugInfo100DebugValue:
			defineOrEmit(insn, pass, [&](debug::Value *value) {
				value->local = get(debug::LocalVariable::ID(insn.word(5)));
				value->variable = Object::ID(insn.word(6));
				value->expression = get(debug::Expression::ID(insn.word(7)));
				for(uint32_t i = 8; i < insn.wordCount(); i++)
				{
					value->indexes.push_back(insn.word(i));
				}
			});
			break;
		case OpenCLDebugInfo100DebugExpression:
			defineOrEmit(insn, pass, [&](debug::Expression *expr) {
				for(uint32_t i = 5; i < insn.wordCount(); i++)
				{
					expr->operations.push_back(get(debug::Operation::ID(insn.word(i))));
				}
			});
			break;
		case OpenCLDebugInfo100DebugSource:
			defineOrEmit(insn, pass, [&](debug::Source *source) {
				source->file = shader->getString(insn.word(5));
				if(insn.wordCount() > 6)
				{
					source->source = shader->getString(insn.word(6));
				}

				auto file = dbg->ctx->lock().createVirtualFile(source->file.c_str(), source->source.c_str());
				source->dbgFile = file;
				files.emplace(source->file.c_str(), file);
			});
			break;

		case OpenCLDebugInfo100DebugTypePointer:
		case OpenCLDebugInfo100DebugTypeQualifier:
		case OpenCLDebugInfo100DebugTypeArray:
		case OpenCLDebugInfo100DebugTypedef:
		case OpenCLDebugInfo100DebugTypeEnum:
		case OpenCLDebugInfo100DebugTypeInheritance:
		case OpenCLDebugInfo100DebugTypePtrToMember:
		case OpenCLDebugInfo100DebugTypeTemplate:
		case OpenCLDebugInfo100DebugTypeTemplateParameter:
		case OpenCLDebugInfo100DebugTypeTemplateTemplateParameter:
		case OpenCLDebugInfo100DebugTypeTemplateParameterPack:
		case OpenCLDebugInfo100DebugGlobalVariable:
		case OpenCLDebugInfo100DebugFunctionDeclaration:
		case OpenCLDebugInfo100DebugLexicalBlockDiscriminator:
		case OpenCLDebugInfo100DebugInlinedVariable:
		case OpenCLDebugInfo100DebugOperation:
		case OpenCLDebugInfo100DebugMacroDef:
		case OpenCLDebugInfo100DebugMacroUndef:
		case OpenCLDebugInfo100DebugImportedEntity:
			UNIMPLEMENTED("b/148401179 OpenCLDebugInfo100 instruction %d", int(extInstIndex));
			break;
		default:
			UNSUPPORTED("OpenCLDebugInfo100 instruction %d", int(extInstIndex));
	}
}

void SpirvShader::Impl::Debugger::setLocation(EmitState *state, const std::shared_ptr<vk::dbg::File> &file, int line, int column)
{
	rr::Call(&State::updateLocation, state->routine->dbgState, file->id, line, column);
}

void SpirvShader::Impl::Debugger::setLocation(EmitState *state, const std::string &path, int line, int column)
{
	auto it = files.find(path);
	if(it != files.end())
	{
		setLocation(state, it->second, line, column);
	}
}

template<typename ID, typename T>
void SpirvShader::Impl::Debugger::add(ID id, T *obj)
{
	ASSERT_MSG(obj != nullptr, "add() called with nullptr obj");
	bool added = objects.emplace(debug::Object::ID(id.value()), obj).second;
	ASSERT_MSG(added, "Debug object with %d already exists", id.value());
}

void SpirvShader::Impl::Debugger::addNone(debug::Object::ID id)
{
	bool added = objects.emplace(debug::Object::ID(id.value()), nullptr).second;
	ASSERT_MSG(added, "Debug object with %d already exists", id.value());
}

template<typename T>
T *SpirvShader::Impl::Debugger::get(SpirvID<T> id) const
{
	auto it = objects.find(debug::Object::ID(id.value()));
	ASSERT_MSG(it != objects.end(), "Unknown debug object %d", id.value());
	auto ptr = debug::cast<T>(it->second.get());
	ASSERT_MSG(ptr, "Debug object %d is not of the correct type. Got: %s, want: %s",
	           id.value(), cstr(it->second->kind), cstr(T::KIND));
	return ptr;
}

template<typename Key>
void SpirvShader::Impl::Debugger::exposeVariable(
    const SpirvShader *shader,
    const Key &key,
    const debug::Scope *scope,
    const debug::Type *type,
    Object::ID id,
    EmitState *state) const
{
	auto dbgState = state->routine->dbgState;
	auto hover = Group::hovers(dbgState, scope).group<Key>(key);
	for(int lane = 0; lane < SIMD::Width; lane++)
	{
		exposeVariable(shader, Group::localsLane(dbgState, scope, lane), lane, key, type, id, state);
		exposeVariable(shader, hover, lane, laneNames[lane], type, id, state);
	}
}

template<typename Key>
void SpirvShader::Impl::Debugger::exposeVariable(
    const SpirvShader *shader,
    const Group &group,
    int l,
    const Key &key,
    const debug::Type *type,
    Object::ID id,
    EmitState *state,
    int wordOffset /* = 0 */) const
{
	if(type != nullptr)
	{
		if(auto ty = debug::cast<debug::BasicType>(type))
		{
			auto &obj = shader->getObject(id);
			SIMD::Int val;
			switch(obj.kind)
			{
				case Object::Kind::InterfaceVariable:
				case Object::Kind::Pointer:
				{
					auto ptr = shader->GetPointerToData(id, 0, state) + sizeof(uint32_t) * wordOffset;
					auto &ptrTy = shader->getType(obj.type);
					if(IsStorageInterleavedByLane(ptrTy.storageClass))
					{
						ptr = InterleaveByLane(ptr);
					}
					auto addr = &ptr.base[Extract(ptr.offsets(), l)];
					switch(ty->encoding)
					{
						case OpenCLDebugInfo100Address:
							// TODO: This function takes a SIMD vector, and pointers cannot
							// be held in them.
							break;
						case OpenCLDebugInfo100Boolean:
							group.putRef<Key, bool>(key, addr);
							break;
						case OpenCLDebugInfo100Float:
							group.putRef<Key, float>(key, addr);
							break;
						case OpenCLDebugInfo100Signed:
							group.putRef<Key, int>(key, addr);
							break;
						case OpenCLDebugInfo100SignedChar:
							group.putRef<Key, int8_t>(key, addr);
							break;
						case OpenCLDebugInfo100Unsigned:
							group.putRef<Key, unsigned int>(key, addr);
							break;
						case OpenCLDebugInfo100UnsignedChar:
							group.putRef<Key, uint8_t>(key, addr);
							break;
						default:
							break;
					}
				}
				break;
				case Object::Kind::Constant:
				case Object::Kind::Intermediate:
				{
					auto val = GenericValue(shader, state, id).Int(wordOffset);

					switch(ty->encoding)
					{
						case OpenCLDebugInfo100Address:
							// TODO: This function takes a SIMD vector, and pointers cannot
							// be held in them.
							break;
						case OpenCLDebugInfo100Boolean:
							group.put<Key, bool>(key, Extract(val, l) != 0);
							break;
						case OpenCLDebugInfo100Float:
							group.put<Key, float>(key, Extract(As<SIMD::Float>(val), l));
							break;
						case OpenCLDebugInfo100Signed:
							group.put<Key, int>(key, Extract(val, l));
							break;
						case OpenCLDebugInfo100SignedChar:
							group.put<Key, int8_t>(key, SByte(Extract(val, l)));
							break;
						case OpenCLDebugInfo100Unsigned:
							group.put<Key, unsigned int>(key, Extract(val, l));
							break;
						case OpenCLDebugInfo100UnsignedChar:
							group.put<Key, uint8_t>(key, Byte(Extract(val, l)));
							break;
						default:
							break;
					}
				}
				break;
				default:
					break;
			}
			return;
		}
		else if(auto ty = debug::cast<debug::VectorType>(type))
		{
			auto elWords = 1;  // Currently vector elements must only be basic types, 32-bit wide
			auto elTy = ty->base;
			auto vecGroup = group.group<Key>(key);
			switch(ty->components)
			{
				case 1:
					exposeVariable(shader, vecGroup, l, "x", elTy, id, state, wordOffset + 0 * elWords);
					break;
				case 2:
					exposeVariable(shader, vecGroup, l, "x", elTy, id, state, wordOffset + 0 * elWords);
					exposeVariable(shader, vecGroup, l, "y", elTy, id, state, wordOffset + 1 * elWords);
					break;
				case 3:
					exposeVariable(shader, vecGroup, l, "x", elTy, id, state, wordOffset + 0 * elWords);
					exposeVariable(shader, vecGroup, l, "y", elTy, id, state, wordOffset + 1 * elWords);
					exposeVariable(shader, vecGroup, l, "z", elTy, id, state, wordOffset + 2 * elWords);
					break;
				case 4:
					exposeVariable(shader, vecGroup, l, "x", elTy, id, state, wordOffset + 0 * elWords);
					exposeVariable(shader, vecGroup, l, "y", elTy, id, state, wordOffset + 1 * elWords);
					exposeVariable(shader, vecGroup, l, "z", elTy, id, state, wordOffset + 2 * elWords);
					exposeVariable(shader, vecGroup, l, "w", elTy, id, state, wordOffset + 3 * elWords);
					break;
				default:
					for(uint32_t i = 0; i < ty->components; i++)
					{
						exposeVariable(shader, vecGroup, l, std::to_string(i).c_str(), elTy, id, state, wordOffset + i * elWords);
					}
					break;
			}
			return;
		}
		else if(auto ty = debug::cast<debug::CompositeType>(type))
		{
			auto objectGroup = group.group<Key>(key);

			for(auto member : ty->members)
			{
				exposeVariable(shader, objectGroup, l, member->name.c_str(), member->type, id, state, member->offset / 32);
			}

			return;
		}
	}

	// No debug type information. Derive from SPIR-V.
	GenericValue val(shader, state, id);
	switch(shader->getType(val.type).opcode())
	{
		case spv::OpTypeInt:
		{
			group.put<Key, int>(key, Extract(val.Int(0), l));
		}
		break;
		case spv::OpTypeFloat:
		{
			group.put<Key, float>(key, Extract(val.Float(0), l));
		}
		break;
		case spv::OpTypeVector:
		{
			auto count = shader->getType(val.type).definition.word(3);
			switch(count)
			{
				case 1:
					group.put<Key, float>(key, Extract(val.Float(0), l));
					break;
				case 2:
					group.put<Key, float>(key, Extract(val.Float(0), l), Extract(val.Float(1), l));
					break;
				case 3:
					group.put<Key, float>(key, Extract(val.Float(0), l), Extract(val.Float(1), l), Extract(val.Float(2), l));
					break;
				case 4:
					group.put<Key, float>(key, Extract(val.Float(0), l), Extract(val.Float(1), l), Extract(val.Float(2), l), Extract(val.Float(3), l));
					break;
				default:
				{
					auto vec = group.group<Key>(key);
					for(uint32_t i = 0; i < count; i++)
					{
						vec.template put<int, float>(i, Extract(val.Float(i), l));
					}
				}
				break;
			}
		}
		break;
		case spv::OpTypePointer:
		{
			auto objectTy = shader->getType(shader->getObject(id).type);
			bool interleavedByLane = IsStorageInterleavedByLane(objectTy.storageClass);
			auto ptr = state->getPointer(id);
			auto ptrGroup = group.group<Key>(key);
			shader->VisitMemoryObject(id, [&](const MemoryElement &el) {
				auto p = ptr + el.offset;
				if(interleavedByLane) { p = InterleaveByLane(p); }  // TODO: Interleave once, then add offset?
				auto simd = p.Load<SIMD::Float>(sw::OutOfBoundsBehavior::Nullify, state->activeLaneMask());
				ptrGroup.template put<int, float>(el.index, Extract(simd, l));
			});
		}
		break;
		default:
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader
////////////////////////////////////////////////////////////////////////////////
void SpirvShader::dbgInit(const std::shared_ptr<vk::dbg::Context> &dbgctx)
{
	impl.debugger = new Impl::Debugger();
	impl.debugger->ctx = dbgctx;
}

void SpirvShader::dbgTerm()
{
	if(impl.debugger)
	{
		delete impl.debugger;
	}
}

void SpirvShader::dbgCreateFile()
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	int currentLine = 1;
	std::string source;
	for(auto insn : *this)
	{
		auto instruction = spvtools::spvInstructionBinaryToText(
		                       SPV_ENV_VULKAN_1_1,
		                       insn.wordPointer(0),
		                       insn.wordCount(),
		                       insns.data(),
		                       insns.size(),
		                       SPV_BINARY_TO_TEXT_OPTION_NO_HEADER) +
		                   "\n";
		dbg->spirvLineMappings[insn.wordPointer(0)] = currentLine;
		currentLine += std::count(instruction.begin(), instruction.end(), '\n');
		source += instruction;
	}
	std::string name;
	switch(executionModel)
	{
		case spv::ExecutionModelVertex: name = "VertexShader"; break;
		case spv::ExecutionModelFragment: name = "FragmentShader"; break;
		case spv::ExecutionModelGLCompute: name = "ComputeShader"; break;
		default: name = "SPIR-V Shader"; break;
	}
	static std::atomic<int> id = { 0 };
	name += std::to_string(id++) + ".spvasm";
	dbg->spirvFile = dbg->ctx->lock().createVirtualFile(name.c_str(), source.c_str());
}

void SpirvShader::dbgBeginEmit(EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	using Group = Impl::Debugger::Group;

	auto routine = state->routine;

	auto type = "SPIR-V";
	switch(executionModel)
	{
		case spv::ExecutionModelVertex: type = "VertexShader"; break;
		case spv::ExecutionModelFragment: type = "FragmentShader"; break;
		case spv::ExecutionModelGLCompute: type = "ComputeShader"; break;
		default: type = "SPIR-V Shader"; break;
	}
	auto dbgState = rr::Call(&Impl::Debugger::State::create, dbg, type);

	routine->dbgState = dbgState;

	SetActiveLaneMask(state->activeLaneMask(), state);

	for(int i = 0; i < SIMD::Width; i++)
	{
		auto builtins = Group::builtinsLane(dbgState, i);
		builtins.put<const char *, int>("subgroupSize", routine->invocationsPerSubgroup);

		switch(executionModel)
		{
			case spv::ExecutionModelGLCompute:
				builtins.putVec3<const char *, int>("numWorkgroups", routine->numWorkgroups);
				builtins.putVec3<const char *, int>("workgroupID", routine->workgroupID);
				builtins.putVec3<const char *, int>("workgroupSize", routine->workgroupSize);
				builtins.put<const char *, int>("numSubgroups", routine->subgroupsPerWorkgroup);
				builtins.put<const char *, int>("subgroupIndex", routine->subgroupIndex);

				builtins.put<const char *, int>("globalInvocationId",
				                                rr::Extract(routine->globalInvocationID[0], i),
				                                rr::Extract(routine->globalInvocationID[1], i),
				                                rr::Extract(routine->globalInvocationID[2], i));
				builtins.put<const char *, int>("localInvocationId",
				                                rr::Extract(routine->localInvocationID[0], i),
				                                rr::Extract(routine->localInvocationID[1], i),
				                                rr::Extract(routine->localInvocationID[2], i));
				builtins.put<const char *, int>("localInvocationIndex", rr::Extract(routine->localInvocationIndex, i));
				break;

			case spv::ExecutionModelFragment:
				builtins.put<const char *, int>("viewIndex", routine->viewID);
				builtins.put<const char *, float>("fragCoord",
				                                  rr::Extract(routine->fragCoord[0], i),
				                                  rr::Extract(routine->fragCoord[1], i),
				                                  rr::Extract(routine->fragCoord[2], i),
				                                  rr::Extract(routine->fragCoord[3], i));
				builtins.put<const char *, float>("pointCoord",
				                                  rr::Extract(routine->pointCoord[0], i),
				                                  rr::Extract(routine->pointCoord[1], i));
				builtins.put<const char *, int>("windowSpacePosition",
				                                rr::Extract(routine->windowSpacePosition[0], i),
				                                rr::Extract(routine->windowSpacePosition[1], i));
				builtins.put<const char *, int>("helperInvocation", rr::Extract(routine->helperInvocation, i));
				break;

			case spv::ExecutionModelVertex:
				builtins.put<const char *, int>("viewIndex", routine->viewID);
				builtins.put<const char *, int>("instanceIndex", routine->instanceID);
				builtins.put<const char *, int>("vertexIndex",
				                                rr::Extract(routine->vertexIndex, i));
				break;

			default:
				break;
		}
	}
}

void SpirvShader::dbgEndEmit(EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	rr::Call(&Impl::Debugger::State::destroy, state->routine->dbgState);
}

void SpirvShader::dbgBeginEmitInstruction(InsnIterator insn, EmitState *state) const
{
#	if PRINT_EACH_PROCESSED_INSTRUCTION
	{
		auto instruction = spvtools::spvInstructionBinaryToText(
		    SPV_ENV_VULKAN_1_1,
		    insn.wordPointer(0),
		    insn.wordCount(),
		    insns.data(),
		    insns.size(),
		    SPV_BINARY_TO_TEXT_OPTION_NO_HEADER);
		printf("%s\n", instruction.c_str());
	}
#	endif  // PRINT_EACH_PROCESSED_INSTRUCTION

#	if PRINT_EACH_EXECUTED_INSTRUCTION
	{
		auto instruction = spvtools::spvInstructionBinaryToText(
		    SPV_ENV_VULKAN_1_1,
		    insn.wordPointer(0),
		    insn.wordCount(),
		    insns.data(),
		    insns.size(),
		    SPV_BINARY_TO_TEXT_OPTION_NO_HEADER);
		rr::Print("{0}\n", instruction);
	}
#	endif  // PRINT_EACH_EXECUTED_INSTRUCTION

	// Only single line step over statement instructions.

	if(auto dbg = impl.debugger)
	{
		if(extensionsImported.count(Extension::OpenCLDebugInfo100) == 0)
		{
			// We're emitting debugger logic for SPIR-V.
			if(IsStatement(insn.opcode()))
			{
				auto line = dbg->spirvLineMappings.at(insn.wordPointer(0));
				auto column = 0;
				dbg->setLocation(state, dbg->spirvFile, line, column);
			}
		}
	}
}

void SpirvShader::dbgEndEmitInstruction(InsnIterator insn, EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	auto resIt = dbg->results.find(insn.wordPointer(0));
	if(resIt != dbg->results.end())
	{
		auto id = resIt->second;
		dbgExposeIntermediate(id, state);
	}
}

void SpirvShader::dbgExposeIntermediate(Object::ID id, EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	dbg->exposeVariable(this, id, &debug::Scope::Root, nullptr, id, state);
}

void SpirvShader::dbgUpdateActiveLaneMask(RValue<SIMD::Int> mask, EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	for(int lane = 0; lane < SIMD::Width; lane++)
	{
		rr::Call(&Impl::Debugger::State::updateActiveLaneMask, state->routine->dbgState, lane, rr::Extract(mask, lane) != 0);
	}
}

void SpirvShader::dbgDeclareResult(const InsnIterator &insn, Object::ID resultId) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	dbg->results.emplace(insn.wordPointer(0), resultId);
}

SpirvShader::EmitResult SpirvShader::EmitLine(InsnIterator insn, EmitState *state) const
{
	if(auto dbg = impl.debugger)
	{
		auto path = getString(insn.word(1));
		auto line = insn.word(2);
		auto column = insn.word(3);
		dbg->setLocation(state, path, line, column);
	}
	return EmitResult::Continue;
}

void SpirvShader::DefineOpenCLDebugInfo100(const InsnIterator &insn)
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	dbg->process(this, insn, nullptr, Impl::Debugger::Pass::Define);
}

SpirvShader::EmitResult SpirvShader::EmitOpenCLDebugInfo100(InsnIterator insn, EmitState *state) const
{
	if(auto dbg = impl.debugger)
	{
		dbg->process(this, insn, state, Impl::Debugger::Pass::Emit);
	}
	return EmitResult::Continue;
}

}  // namespace sw

#else  // ENABLE_VK_DEBUGGER

// Stub implementations of the dbgXXX functions.
namespace sw {

void SpirvShader::dbgInit(const std::shared_ptr<vk::dbg::Context> &dbgctx) {}
void SpirvShader::dbgTerm() {}
void SpirvShader::dbgCreateFile() {}
void SpirvShader::dbgBeginEmit(EmitState *state) const {}
void SpirvShader::dbgEndEmit(EmitState *state) const {}
void SpirvShader::dbgBeginEmitInstruction(InsnIterator insn, EmitState *state) const {}
void SpirvShader::dbgEndEmitInstruction(InsnIterator insn, EmitState *state) const {}
void SpirvShader::dbgExposeIntermediate(Object::ID id, EmitState *state) const {}
void SpirvShader::dbgUpdateActiveLaneMask(RValue<SIMD::Int> mask, EmitState *state) const {}
void SpirvShader::dbgDeclareResult(const InsnIterator &insn, Object::ID resultId) const {}

void SpirvShader::DefineOpenCLDebugInfo100(const InsnIterator &insn) {}

SpirvShader::EmitResult SpirvShader::EmitOpenCLDebugInfo100(InsnIterator insn, EmitState *state) const
{
	return EmitResult::Continue;
}

SpirvShader::EmitResult SpirvShader::EmitLine(InsnIterator insn, EmitState *state) const
{
	return EmitResult::Continue;
}

}  // namespace sw

#endif  // ENABLE_VK_DEBUGGER
