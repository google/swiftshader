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

// If enabled, each instruction will be printed before defining.
#define PRINT_EACH_DEFINED_DBG_INSTRUCTION 0
// If enabled, each instruction will be printed before emitting.
#define PRINT_EACH_EMITTED_INSTRUCTION 0
// If enabled, each instruction will be printed before executing.
#define PRINT_EACH_EXECUTED_INSTRUCTION 0
// If enabled, debugger variables will contain debug information (addresses,
// byte offset, etc).
#define DEBUG_ANNOTATE_VARIABLE_KEYS 0

#ifdef ENABLE_VK_DEBUGGER

#	include "Vulkan/Debug/Context.hpp"
#	include "Vulkan/Debug/File.hpp"
#	include "Vulkan/Debug/Thread.hpp"
#	include "Vulkan/Debug/Variable.hpp"

#	include "spirv/unified1/OpenCLDebugInfo100.h"
#	include "spirv-tools/libspirv.h"

#	include <algorithm>
#	include <queue>

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

template<typename T>
T take(std::queue<T> &queue)
{
	auto v = queue.front();
	queue.pop();
	return v;
}

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
template<typename T>
std::string tostring(T *s)
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%p", s);
	return buf;
}
std::string tostring(sw::SpirvShader::Object::ID id)
{
	return "%" + tostring(id.value());
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
		GlobalVariable,
		LocalVariable,
		Member,
		Operation,
		Source,
		SourceScope,
		Value,
		TemplateParameter,

		// Scopes
		CompilationUnit,
		LexicalBlock,

		// Types
		BasicType,
		ArrayType,
		VectorType,
		FunctionType,
		CompositeType,
		TemplateType,
	};

	using ID = sw::SpirvID<Object>;
	static constexpr auto KIND = Kind::Object;
	inline Object(Kind kind)
	    : kind(kind)
	{
		(void)KIND;  // Used in debug builds. Avoid unused variable warnings in NDEBUG builds.
	}
	const Kind kind;

	// kindof() returns true iff kind is of this type, or any type deriving from
	// this type.
	static constexpr bool kindof(Object::Kind kind) { return true; }

	virtual ~Object() = default;
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
		case Object::Kind::GlobalVariable: return "GlobalVariable";
		case Object::Kind::LocalVariable: return "LocalVariable";
		case Object::Kind::Member: return "Member";
		case Object::Kind::Operation: return "Operation";
		case Object::Kind::Source: return "Source";
		case Object::Kind::SourceScope: return "SourceScope";
		case Object::Kind::Value: return "Value";
		case Object::Kind::TemplateParameter: return "TemplateParameter";
		case Object::Kind::CompilationUnit: return "CompilationUnit";
		case Object::Kind::LexicalBlock: return "LexicalBlock";
		case Object::Kind::BasicType: return "BasicType";
		case Object::Kind::ArrayType: return "ArrayType";
		case Object::Kind::VectorType: return "VectorType";
		case Object::Kind::FunctionType: return "FunctionType";
		case Object::Kind::CompositeType: return "CompositeType";
		case Object::Kind::TemplateType: return "TemplateType";
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
	// Global represents the global scope.
	static const Scope Global;

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
		       kind == Kind::ArrayType ||
		       kind == Kind::VectorType ||
		       kind == Kind::FunctionType ||
		       kind == Kind::CompositeType ||
		       kind == Kind::TemplateType;
	}

	std::pair<const Type *, uint32_t> index(std::queue<uint32_t> &&indices) const
	{
		if(indices.size() == 0)
		{
			return { this, 0 };
		}
		return indexMember(std::move(indices));
	}

	// sizeInBytes() returns the number of bytes of the given debug type.
	virtual uint32_t sizeInBytes() const = 0;

	// value() returns a shared pointer to a vk::dbg::Value that views the data
	// at ptr of this type.
	virtual std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const = 0;

protected:
	// indexMember() returns the nested inner element debug type and byte offset
	// from the base of this type, using the list of indices.
	virtual std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&) const = 0;
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

	uint32_t sizeInBytes() const override { return size / 8; }

	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&) const override
	{
		DABORT("indexMember() called on BasicType %s", name.c_str());
		return {};
	}

	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override
	{
		switch(encoding)
		{
			case OpenCLDebugInfo100Address:
				// return vk::dbg::make_reference(*static_cast<void **>(ptr));
				UNIMPLEMENTED("b/148401179 OpenCLDebugInfo100 OpenCLDebugInfo100Address BasicType");
				return nullptr;
			case OpenCLDebugInfo100Boolean:
				return vk::dbg::make_reference(*static_cast<bool *>(ptr));
			case OpenCLDebugInfo100Float:
				return vk::dbg::make_reference(*static_cast<float *>(ptr));
			case OpenCLDebugInfo100Signed:
				return vk::dbg::make_reference(*static_cast<int32_t *>(ptr));
			case OpenCLDebugInfo100SignedChar:
				return vk::dbg::make_reference(*static_cast<int8_t *>(ptr));
			case OpenCLDebugInfo100Unsigned:
				return vk::dbg::make_reference(*static_cast<uint32_t *>(ptr));
			case OpenCLDebugInfo100UnsignedChar:
				return vk::dbg::make_reference(*static_cast<uint8_t *>(ptr));
			default:
				UNIMPLEMENTED("b/148401179 OpenCLDebugInfo100 encoding %d", int(encoding));
				return nullptr;
		}
	}
};

struct ArrayType : ObjectImpl<ArrayType, Type, Object::Kind::ArrayType>
{
	Type *base = nullptr;
	std::vector<uint32_t> dimensions;

	// build() loops over each element of the multi-dimensional array, calling
	// enter() for building each new dimension group, and element() for each
	// inner-most dimension element.
	//
	// enter must be a function of the signature:
	//   std::shared_ptr<vk::dbg::VariableContainer>
	//       (std::shared_ptr<vk::dbg::VariableContainer>& parent, uint32_t idx)
	// where:
	//   parent is the outer dimension group
	//   idx is the index of the next deepest dimension.
	//
	// element must be a function of the signature:
	//   void(std::shared_ptr<vk::dbg::VariableContainer> &parent,
	//        uint32_t idx, uint32_t offset)
	// where:
	//   parent is the penultimate deepest dimension group
	//   idx is the index of the element in parent group
	//   offset is the 'flattened array' index for the element.
	template<typename GROUP, typename ENTER_FUNC, typename ELEMENT_FUNC>
	void build(const GROUP &group, ENTER_FUNC &&enter, ELEMENT_FUNC &&element) const
	{
		if(dimensions.size() == 0) { return; }

		struct Dimension
		{
			uint32_t idx = 0;
			GROUP group;
			bool built = false;
		};

		std::vector<Dimension> dims;
		dims.resize(dimensions.size());

		uint32_t offset = 0;

		int dimIdx = 0;
		const int n = static_cast<int>(dimensions.size()) - 1;
		while(dimIdx >= 0)
		{
			// (Re)build groups to inner dimensions.
			for(; dimIdx <= n; dimIdx++)
			{
				if(!dims[dimIdx].built)
				{
					dims[dimIdx].group = (dimIdx == 0)
					                         ? group
					                         : enter(dims[dimIdx - 1].group, dims[dimIdx - 1].idx);
					dims[dimIdx].built = true;
				}
			}

			// Emit each of the inner-most dimension elements.
			for(dims[n].idx = 0; dims[n].idx < dimensions[n]; dims[n].idx++)
			{
				ASSERT(dims[n].built);
				element(dims[n].group, dims[n].idx, offset++);
			}

			dimIdx = n;
			while(dims[dimIdx].idx == dimensions[dimIdx])
			{
				dims[dimIdx] = {};  // Clear the the current dimension
				dimIdx--;           // Step up a dimension
				if(dimIdx < 0) { break; }
				dims[dimIdx].idx++;  // Increment the next dimension index
			}
		}
	}

	uint32_t sizeInBytes() const override
	{
		auto numBytes = base->sizeInBytes();
		for(auto dim : dimensions)
		{
			numBytes *= dim;
		}
		return numBytes;
	}

	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&indices) const override
	{
		std::vector<uint32_t> arrIndices(dimensions.size());
		for(size_t i = 0; i < dimensions.size(); i++)
		{
			arrIndices[i] = take(indices);
		}

		auto out = base->index(std::move(indices));
		auto stride = base->sizeInBytes();
		for(int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--)
		{
			out.second += arrIndices[i] * stride;
			stride *= dimensions[i];
		}
		return out;
	}

	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override
	{
		auto vc = std::make_shared<vk::dbg::VariableContainer>();
		build(
		    vc,
		    [&](std::shared_ptr<vk::dbg::VariableContainer> &parent, uint32_t idx) {
			    auto child = std::make_shared<vk::dbg::VariableContainer>();
			    parent->put(tostring(idx), child);
			    return child;
		    },
		    [&](std::shared_ptr<vk::dbg::VariableContainer> &parent, uint32_t idx, uint32_t offset) {
			    offset = offset * base->sizeInBytes() * (interleaved ? sw::SIMD::Width : 1);
			    auto addr = static_cast<uint8_t *>(ptr) + offset;
			    auto child = base->value(addr, interleaved);
			    auto key = tostring(idx);
#	if DEBUG_ANNOTATE_VARIABLE_KEYS
			    key += " (" + tostring(addr) + " +" + tostring(offset) + ", idx: " + tostring(idx) + ")" + (interleaved ? "I" : "F");
#	endif
			    parent->put(key, child);
		    });
		return vc;
	}
};

struct VectorType : ObjectImpl<VectorType, Type, Object::Kind::VectorType>
{
	Type *base = nullptr;
	uint32_t components = 0;

	uint32_t sizeInBytes() const override
	{
		return base->sizeInBytes() * components;
	}

	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&indices) const override
	{
		auto idx = take(indices);
		auto out = base->index(std::move(indices));
		out.second += base->sizeInBytes() * idx;
		return out;
	}

	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override
	{
		const auto elSize = base->sizeInBytes();
		auto vc = std::make_shared<vk::dbg::VariableContainer>();
		for(uint32_t i = 0; i < components; i++)
		{
			auto offset = elSize * i * (interleaved ? sw::SIMD::Width : 1);
			auto elPtr = static_cast<uint8_t *>(ptr) + offset;
			auto elKey = (components > 4) ? tostring(i) : &"x\0y\0z\0w\0"[i * 2];
#	if DEBUG_ANNOTATE_VARIABLE_KEYS
			elKey += " (" + tostring(elPtr) + " +" + tostring(offset) + ")" + (interleaved ? "I" : "F");
#	endif
			vc->put(elKey, base->value(elPtr, interleaved));
		}
		return vc;
	}
};

struct FunctionType : ObjectImpl<FunctionType, Type, Object::Kind::FunctionType>
{
	uint32_t flags = 0;  // OR'd from OpenCLDebugInfo100DebugInfoFlags
	Type *returnTy = nullptr;
	std::vector<Type *> paramTys;

	uint32_t sizeInBytes() const override { return 0; }
	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&indices) const override
	{
		DABORT("indexMember() called on FunctionType");
		return {};
	}
	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override { return nullptr; }
};

struct Member : ObjectImpl<Member, Object, Object::Kind::Member>
{
	std::string name;
	Type *type = nullptr;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	struct CompositeType *parent = nullptr;
	uint32_t offset = 0;  // in bits
	uint32_t size = 0;    // in bits
	uint32_t flags = 0;   // OR'd from OpenCLDebugInfo100DebugInfoFlags
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

	uint32_t sizeInBytes() const override { return size / 8; }

	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&indices) const override
	{
		auto idx = take(indices);
		auto member = members[idx];
		auto out = member->type->index(std::move(indices));
		out.second += member->offset / 8;
		return out;
	}

	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override
	{
		auto vc = std::make_shared<vk::dbg::VariableContainer>();
		for(auto &member : members)
		{
			auto offset = (member->offset / 8) * (interleaved ? sw::SIMD::Width : 1);
			auto elPtr = static_cast<uint8_t *>(ptr) + offset;
			auto elKey = member->name;
#	if DEBUG_ANNOTATE_VARIABLE_KEYS
			// elKey += " (" + tostring(elPtr) + " +" + tostring(offset) + ")" + (interleaved ? "I" : "F");
#	endif
			vc->put(elKey, member->type->value(elPtr, interleaved));
		}
		return vc;
	}
};

struct TemplateParameter : ObjectImpl<TemplateParameter, Object, Object::Kind::TemplateParameter>
{
	std::string name;
	Type *type = nullptr;
	uint32_t value = 0;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
};

struct TemplateType : ObjectImpl<TemplateType, Type, Object::Kind::TemplateType>
{
	Type *target = nullptr;  // Class, struct or function.
	std::vector<TemplateParameter *> parameters;

	uint32_t sizeInBytes() const override { return target->sizeInBytes(); }
	std::pair<const Type *, uint32_t> indexMember(std::queue<uint32_t> &&indices) const override
	{
		return target->index(std::move(indices));
	}
	std::shared_ptr<vk::dbg::Value> value(void *ptr, bool interleaved) const override
	{
		return target->value(ptr, interleaved);
	}
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

struct GlobalVariable : ObjectImpl<GlobalVariable, Object, Object::Kind::GlobalVariable>
{
	std::string name;
	Type *type = nullptr;
	Source *source = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;
	Scope *parent = nullptr;
	std::string linkage;
	sw::SpirvShader::Object::ID variable;
	uint32_t flags = 0;  // OR'd from OpenCLDebugInfo100DebugInfoFlags
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
	sw::SpirvShader::Object::ID value;
	Expression *expression = nullptr;
	std::vector<uint32_t> indexes;
};

const Scope Scope::Global = CompilationUnit{};

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
// There is an instance of this class per shader program.
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

	void setNextSetLocationIsStep();
	void setLocation(EmitState *state, const std::shared_ptr<vk::dbg::File> &, int line, int column);
	void setLocation(EmitState *state, const std::string &path, int line, int column);

	// foreachLane() calls f for each debugger group representing the SIMD
	// lanes of execution.
	// FUNC is a function with the signature:
	//   (int lane, const Group &group, auto &key)
	template<typename Key, typename Func>
	void foreachLane(const Key &key, const debug::Scope *scope, EmitState *state, Func &&f) const;

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

	// Shadow memory is used to construct a contiguous memory block for local
	// variables that may be formed from multiple SSA values.
	struct Shadow
	{
		// Offset in the shadow memory allocation for the given local variable.
		std::unordered_map<debug::LocalVariable *, uint32_t> offsets;

		// Total size of the shadow memory in bytes.
		uint32_t size;
	} shadow;

private:
	// add() registers the debug object with the given id.
	template<typename ID>
	void add(ID id, std::unique_ptr<debug::Object> &&);

	// addNone() registers given id as a None value or type.
	void addNone(debug::Object::ID id);

	// isNone() returns true if the given id was registered as none with
	// addNone().
	bool isNone(debug::Object::ID id) const;

	// get() returns the debug object with the given id.
	// The object must exist and be of type (or derive from type) T.
	// A returned nullptr represents a None value or type.
	template<typename T>
	T *get(SpirvID<T> id) const;

	// getOrNull() returns the debug object with the given id if
	// the object exists and is of type (or derive from type) T.
	// Otherwise, returns nullptr.
	template<typename T>
	T *getOrNull(SpirvID<T> id) const;

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
	bool nextSetLocationIsStep = true;
	int lastSetLocationLine = 0;
};

////////////////////////////////////////////////////////////////////////////////
// sw::SpirvShader::Impl::Debugger::State
//
// State holds the runtime data structures for the shader debug session.
// There is an instance of this class per shader invocation.
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
	void updateLocation(bool isStep, vk::dbg::File::ID file, int line, int column);
	void createScope(const debug::Scope *);
	void setScope(debug::SourceScope *newScope);

	vk::dbg::VariableContainer *hovers(const debug::Scope *);
	vk::dbg::VariableContainer *localsLane(const debug::Scope *, int lane);

	template<typename K>
	vk::dbg::VariableContainer *group(vk::dbg::VariableContainer *vc, K key);

	template<typename K, typename V>
	void putVal(vk::dbg::VariableContainer *vc, K key, V value);

	template<typename K>
	void putPtr(vk::dbg::VariableContainer *vc, K key, void *ptr, bool interleaved, const debug::Type *type);

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
	std::unique_ptr<uint8_t[]> const shadow;
	std::unordered_map<const debug::Scope *, Scopes> scopes;
	Scopes globals;                          // Scope for globals.
	debug::SourceScope *srcScope = nullptr;  // Current source scope.

	const size_t initialThreadDepth = 0;
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
    , shadow(new uint8_t[debugger->shadow.size])
    , initialThreadDepth(thread->depth())
{
	enter(lock, stackBase);

	thread->update(true, [&](vk::dbg::Frame &frame) {
		globals.locals = frame.locals;
		globals.hovers = frame.hovers;
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			auto locals = std::make_shared<vk::dbg::VariableContainer>();
			frame.locals->variables->put(laneNames[i], locals);
			globals.localsByLane[i] = locals;
		}
	});
}

SpirvShader::Impl::Debugger::State::~State()
{
	for(auto depth = thread->depth(); depth > initialThreadDepth; depth--)
	{
		exit();
	}
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
	globals.localsByLane[lane]->put("enabled", vk::dbg::make_constant(enabled));
}

void SpirvShader::Impl::Debugger::State::updateLocation(bool isStep, vk::dbg::File::ID fileID, int line, int column)
{
	auto file = debugger->ctx->lock().get(fileID);
	thread->update(isStep, [&](vk::dbg::Frame &frame) {
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

template<typename K>
vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::group(vk::dbg::VariableContainer *vc, K key)
{
	auto out = std::make_shared<vk::dbg::VariableContainer>();
	vc->put(tostring(key), out);
	return out.get();
}

template<typename K, typename V>
void SpirvShader::Impl::Debugger::State::putVal(vk::dbg::VariableContainer *vc, K key, V value)
{
	vc->put(tostring(key), vk::dbg::make_constant(value));
}

template<typename K>
void SpirvShader::Impl::Debugger::State::putPtr(vk::dbg::VariableContainer *vc, K key, void *ptr, bool interleaved, const debug::Type *type)
{
	vc->put(tostring(key), type->value(ptr, interleaved));
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
		auto locals = std::make_shared<vk::dbg::VariableContainer>();
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
		// Scope has no parent. Ensure the globals are inherited for this stack
		// frame.
		//
		// Note: We're combining globals with locals as DAP doesn't have a
		// 'globals' enum value for Scope::presentationHint.
		// TODO(bclayton): We should probably keep globals separate from locals
		// and combine them at the server interface. That way we can easily
		// provide globals if DAP later supports it as a Scope::presentationHint
		// type.
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			s.localsByLane[i]->extend(globals.localsByLane[i]);
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
		thread->update(true, [&](vk::dbg::Frame &frame) {
			frame.locals = dbgScope.locals;
			frame.hovers = dbgScope.hovers;
		});
	}
}

const SpirvShader::Impl::Debugger::State::Scopes &SpirvShader::Impl::Debugger::State::getScopes(const debug::Scope *scope)
{
	if(scope == &debug::Scope::Global)
	{
		return globals;
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
	static Group globals(Ptr state, int lane);

	inline Group();
	inline Group(Ptr state, Ptr group);

	inline bool isValid() const;

	template<typename K, typename RK>
	Group group(RK key) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV value) const;

	template<typename K, typename RK>
	void putPtr(RK key, RValue<Pointer<Byte>> ptr, bool interleaved, const debug::Type *type) const;

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
	bool valid = false;
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
SpirvShader::Impl::Debugger::Group::globals(Ptr state, int lane)
{
	return localsLane(state, &debug::Scope::Global, lane);
}

SpirvShader::Impl::Debugger::Group::Group() {}

SpirvShader::Impl::Debugger::Group::Group(Ptr state, Ptr group)
    : state(state)
    , ptr(group)
    , valid(true)
{}

bool SpirvShader::Impl::Debugger::Group::isValid() const
{
	return valid;
}

template<typename K, typename RK>
SpirvShader::Impl::Debugger::Group SpirvShader::Impl::Debugger::Group::group(RK key) const
{
	ASSERT(isValid());
	return Group(state, rr::Call(&State::group<K>, state, ptr, key));
}

template<typename K, typename V, typename RK, typename RV>
void SpirvShader::Impl::Debugger::Group::put(RK key, RV value) const
{
	ASSERT(isValid());
	rr::Call(&State::putVal<K, V>, state, ptr, key, value);
}

template<typename K, typename RK>
void SpirvShader::Impl::Debugger::Group::putPtr(RK key, RValue<Pointer<Byte>> addr, bool interleaved, const debug::Type *type) const
{
	ASSERT(isValid());
	rr::Call(&State::putPtr<K>, state, ptr, key, addr, interleaved, type);
}

template<typename K, typename V, typename RK>
void SpirvShader::Impl::Debugger::Group::putRef(RK key, RValue<Pointer<Byte>> ref) const
{
	ASSERT(isValid());
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
			add(id, std::unique_ptr<debug::Object>(new T()));
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
		case OpenCLDebugInfo100DebugTypeArray:
			defineOrEmit(insn, pass, [&](debug::ArrayType *type) {
				type->base = get(debug::Type::ID(insn.word(5)));
				for(uint32_t i = 6; i < insn.wordCount(); i++)
				{
					type->dimensions.emplace_back(shader->GetConstScalarInt(insn.word(i)));
				}
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
				type->returnTy = getOrNull(debug::Type::ID(insn.word(6)));

				// 'Return Type' operand must be a debug type or OpTypeVoid. See
				// https://www.khronos.org/registry/spir-v/specs/unified1/OpenCL.DebugInfo.100.html#DebugTypeFunction
				ASSERT_MSG(type->returnTy != nullptr || shader->getType(insn.word(6)).opcode() == spv::Op::OpTypeVoid, "Invalid return type of DebugTypeFunction: %d", insn.word(6));

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
				type->size = isNone(insn.word(12)) ? 0 : shader->GetConstScalarInt(insn.word(12));
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
		case OpenCLDebugInfo100DebugTypeTemplate:
			defineOrEmit(insn, pass, [&](debug::TemplateType *tpl) {
				tpl->target = get(debug::Type::ID(insn.word(5)));
				for(size_t i = 6, c = insn.wordCount(); i < c; i++)
				{
					tpl->parameters.emplace_back(get(debug::TemplateParameter::ID(insn.word(i))));
				}
			});
			break;
		case OpenCLDebugInfo100DebugTypeTemplateParameter:
			defineOrEmit(insn, pass, [&](debug::TemplateParameter *param) {
				param->name = shader->getString(insn.word(5));
				param->type = get(debug::Type::ID(insn.word(6)));
				param->value = 0;  // TODO: Get value from OpConstant if "a template value parameter".
				param->source = get(debug::Source::ID(insn.word(8)));
				param->line = insn.word(9);
				param->column = insn.word(10);
			});
			break;
		case OpenCLDebugInfo100DebugGlobalVariable:
			defineOrEmit(insn, pass, [&](debug::GlobalVariable *var) {
				var->name = shader->getString(insn.word(5));
				var->type = get(debug::Type::ID(insn.word(6)));
				var->source = get(debug::Source::ID(insn.word(7)));
				var->line = insn.word(8);
				var->column = insn.word(9);
				var->parent = get(debug::Scope::ID(insn.word(10)));
				var->linkage = shader->getString(insn.word(11));
				var->variable = isNone(insn.word(12)) ? 0 : insn.word(12);
				var->flags = insn.word(13);
				// static member declaration: word(14)

				// TODO(b/148401179): Instead of simply hiding variables that have been stripped by optimizations, show them in the debugger as `<optimized-away>`
				if(var->variable != 0)
				{
					exposeVariable(shader, var->name.c_str(), &debug::Scope::Global, var->type, var->variable, state);
				}
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
				value->value = Object::ID(insn.word(6));
				value->expression = get(debug::Expression::ID(insn.word(7)));
				for(uint32_t i = 8; i < insn.wordCount(); i++)
				{
					auto idx = shader->GetConstScalarInt(insn.word(i));
					value->indexes.push_back(idx);
				}

				// DebugValue partially updates a DebugLocalVariable with an SSA
				// value. This is typically used to update a DebugLocalVariable
				// of a composite type, which holds structure member offsets
				// from a base address.
				// To handle these, we allocate shadow memory to hold a copy of
				// the entire variable in contiguous memory and have the
				// DebugLocalVariable point to this memory. Whenever we
				// encounter a DebugValue, we copy the necessary fields to the
				// shadow memory.

				// type of the full DebugLocalVariable.
				auto type = value->local->type;

				// base address of the variable.
				// Start by pointing base to the root of the shadow memory.
				// This will be offset to the variable, then the member within
				// the variable below.
				SIMD::Pointer base(*Pointer<Pointer<Byte>>(state->routine->dbgState + OFFSET(State, shadow)), shadow.size);

				// All variables are considered local, and therefore
				// interleaved.
				base = InterleaveByLane(base);

				// Have we already allocated shadow memory for this variable?
				auto it = shadow.offsets.find(value->local);
				if(it == shadow.offsets.end())
				{
					// No shadow memory has been allocated for this local
					// variable yet.

					// Allocate the memory for the variable.
					auto offset = shadow.size;
					shadow.offsets.emplace(value->local, offset);
					auto size = type->sizeInBytes() * SIMD::Width;
					base += offset;
					shadow.size += size;

					// Expose the variable.
					auto name = value->local->name.c_str();
					auto scope = value->local->parent;
					auto offsets = base.offsets();
					foreachLane(name, scope, state, [&](int lane, const Group &group, auto &key) {
						auto ptr = base.base + Extract(offsets, lane);
						group.putPtr<const char *>(name, ptr, true, value->local->type);
					});
				}
				else
				{
					// Shadow memory already allocated for this variable.
					// Offset base to point to it.
					base += it->second;
				}

				// Find the byte offset on the indexed member of the variable.
				std::queue<uint32_t> indices;
				for(auto idx : value->indexes)
				{
					indices.emplace(idx);
				}
				auto offset = type->index(std::move(indices)).second;

				// Update base to point to the particular member.
				base += offset;

				// Now copy the updated value into shadow memory representation
				// of the variable.
				// TODO(b/148401179): This assumes tight packing of all
				// components, which may not match with the debug structure
				// layout.
				auto &valObject = shader->getObject(value->value);
				auto &valType = shader->getType(valObject);
				for(auto i = 0u; i < valType.componentCount; i++)
				{
					auto val = Operand(shader, state, value->value).Int(i);
					auto dst = base + i * sizeof(uint32_t) * SIMD::Width;
					// Use RobustBufferAccess as the size as described by the
					// debug type may be smaller than the true SSA size.
					dst.Store(val, sw::OutOfBoundsBehavior::RobustBufferAccess, state->activeLaneMask());
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
					auto file = dbg->ctx->lock().createVirtualFile(source->file.c_str(), source->source.c_str());
					source->dbgFile = file;
					files.emplace(source->file.c_str(), file);
				}
				else
				{
					auto file = dbg->ctx->lock().createPhysicalFile(source->file.c_str());
					source->dbgFile = file;
					files.emplace(source->file.c_str(), file);
				}
			});
			break;
		case OpenCLDebugInfo100DebugOperation:
			defineOrEmit(insn, pass, [&](debug::Operation *operation) {
				operation->opcode = insn.word(5);
				for(uint32_t i = 6; i < insn.wordCount(); i++)
				{
					operation->operands.push_back(insn.word(i));
				}
			});
			break;

		case OpenCLDebugInfo100DebugTypePointer:
		case OpenCLDebugInfo100DebugTypeQualifier:
		case OpenCLDebugInfo100DebugTypedef:
		case OpenCLDebugInfo100DebugTypeEnum:
		case OpenCLDebugInfo100DebugTypeInheritance:
		case OpenCLDebugInfo100DebugTypePtrToMember:
		case OpenCLDebugInfo100DebugTypeTemplateTemplateParameter:
		case OpenCLDebugInfo100DebugTypeTemplateParameterPack:
		case OpenCLDebugInfo100DebugFunctionDeclaration:
		case OpenCLDebugInfo100DebugLexicalBlockDiscriminator:
		case OpenCLDebugInfo100DebugInlinedVariable:
		case OpenCLDebugInfo100DebugMacroDef:
		case OpenCLDebugInfo100DebugMacroUndef:
		case OpenCLDebugInfo100DebugImportedEntity:
			UNIMPLEMENTED("b/148401179 OpenCLDebugInfo100 instruction %d", int(extInstIndex));
			break;
		default:
			UNSUPPORTED("OpenCLDebugInfo100 instruction %d", int(extInstIndex));
	}
}

void SpirvShader::Impl::Debugger::setNextSetLocationIsStep()
{
	nextSetLocationIsStep = true;
}

void SpirvShader::Impl::Debugger::setLocation(EmitState *state, const std::shared_ptr<vk::dbg::File> &file, int line, int column)
{
	if(line != lastSetLocationLine)
	{
		// If the line number has changed, then this is always a step.
		nextSetLocationIsStep = true;
		lastSetLocationLine = line;
	}
	rr::Call(&State::updateLocation, state->routine->dbgState, nextSetLocationIsStep, file->id, line, column);
	nextSetLocationIsStep = false;
}

void SpirvShader::Impl::Debugger::setLocation(EmitState *state, const std::string &path, int line, int column)
{
	auto it = files.find(path);
	if(it != files.end())
	{
		setLocation(state, it->second, line, column);
	}
}

template<typename ID>
void SpirvShader::Impl::Debugger::add(ID id, std::unique_ptr<debug::Object> &&obj)
{
	ASSERT_MSG(obj != nullptr, "add() called with nullptr obj");
	bool added = objects.emplace(debug::Object::ID(id.value()), std::move(obj)).second;
	ASSERT_MSG(added, "Debug object with %d already exists", id.value());
}

void SpirvShader::Impl::Debugger::addNone(debug::Object::ID id)
{
	bool added = objects.emplace(debug::Object::ID(id.value()), nullptr).second;
	ASSERT_MSG(added, "Debug object with %d already exists", id.value());
}

bool SpirvShader::Impl::Debugger::isNone(debug::Object::ID id) const
{
	auto it = objects.find(debug::Object::ID(id.value()));
	if(it == objects.end()) { return false; }
	return it->second.get() == nullptr;
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

template<typename T>
T *SpirvShader::Impl::Debugger::getOrNull(SpirvID<T> id) const
{
	auto it = objects.find(debug::Object::ID(id.value()));
	if(it == objects.end()) { return nullptr; }  // Not found.
	auto ptr = debug::cast<T>(it->second.get());
	ASSERT_MSG(ptr, "Debug object %d is not of the correct type. Got: %s, want: %s",
	           id.value(), cstr(it->second->kind), cstr(T::KIND));
	return ptr;
}

template<typename Key, typename Func>
void SpirvShader::Impl::Debugger::foreachLane(
    const Key &key,
    const debug::Scope *scope,
    EmitState *state,
    Func &&f) const
{
	auto dbgState = state->routine->dbgState;
	auto hover = Group::hovers(dbgState, scope).group<Key>(key);
	for(int lane = 0; lane < SIMD::Width; lane++)
	{
		f(lane, Group::localsLane(dbgState, scope, lane), key);
		f(lane, hover, laneNames[lane]);
	}
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
	foreachLane(key, scope, state, [&](int lane, const Group &group, auto &key) {
		exposeVariable(shader, group, lane, laneNames[lane], type, id, state);
	});
}

template<typename Key>
void SpirvShader::Impl::Debugger::exposeVariable(
    const SpirvShader *shader,
    const Group &group,
    int lane,
    const Key &key,
    const debug::Type *type,
    Object::ID id,
    EmitState *state,
    int wordOffset /* = 0 */) const
{
	auto &obj = shader->getObject(id);

	if(type != nullptr)
	{
		switch(obj.kind)
		{
			case Object::Kind::InterfaceVariable:
			case Object::Kind::Pointer:
			case Object::Kind::DescriptorSet:
			{
				ASSERT(wordOffset == 0);                            // TODO.
				auto ptr = shader->GetPointerToData(id, 0, state);  //  + sizeof(uint32_t) * wordOffset;
				auto &ptrTy = shader->getType(obj);
				auto interleaved = IsStorageInterleavedByLane(ptrTy.storageClass);
				if(interleaved)
				{
					ptr = InterleaveByLane(ptr);
				}
				auto addr = &ptr.base[Extract(ptr.offsets(), lane)];
				group.putPtr<Key>(key, addr, interleaved, type);
			}
			break;

			case Object::Kind::Constant:
			case Object::Kind::Intermediate:
			{
				if(auto ty = debug::cast<debug::BasicType>(type))
				{
					auto val = Operand(shader, state, id).Int(wordOffset);
					switch(ty->encoding)
					{
						case OpenCLDebugInfo100Address:
							// TODO: This function takes a SIMD vector, and pointers cannot
							// be held in them.
							break;
						case OpenCLDebugInfo100Boolean:
							group.put<Key, bool>(key, Extract(val, lane) != 0);
							break;
						case OpenCLDebugInfo100Float:
							group.put<Key, float>(key, Extract(As<SIMD::Float>(val), lane));
							break;
						case OpenCLDebugInfo100Signed:
							group.put<Key, int>(key, Extract(val, lane));
							break;
						case OpenCLDebugInfo100SignedChar:
							group.put<Key, int8_t>(key, SByte(Extract(val, lane)));
							break;
						case OpenCLDebugInfo100Unsigned:
							group.put<Key, unsigned int>(key, Extract(val, lane));
							break;
						case OpenCLDebugInfo100UnsignedChar:
							group.put<Key, uint8_t>(key, Byte(Extract(val, lane)));
							break;
						default:
							break;
					}
				}
				else if(auto ty = debug::cast<debug::VectorType>(type))
				{
					auto elWords = 1;  // Currently vector elements must only be basic types, 32-bit wide
					auto elTy = ty->base;
					auto vecGroup = group.group<Key>(key);
					switch(ty->components)
					{
						case 1:
							exposeVariable(shader, vecGroup, lane, "x", elTy, id, state, wordOffset + 0 * elWords);
							break;
						case 2:
							exposeVariable(shader, vecGroup, lane, "x", elTy, id, state, wordOffset + 0 * elWords);
							exposeVariable(shader, vecGroup, lane, "y", elTy, id, state, wordOffset + 1 * elWords);
							break;
						case 3:
							exposeVariable(shader, vecGroup, lane, "x", elTy, id, state, wordOffset + 0 * elWords);
							exposeVariable(shader, vecGroup, lane, "y", elTy, id, state, wordOffset + 1 * elWords);
							exposeVariable(shader, vecGroup, lane, "z", elTy, id, state, wordOffset + 2 * elWords);
							break;
						case 4:
							exposeVariable(shader, vecGroup, lane, "x", elTy, id, state, wordOffset + 0 * elWords);
							exposeVariable(shader, vecGroup, lane, "y", elTy, id, state, wordOffset + 1 * elWords);
							exposeVariable(shader, vecGroup, lane, "z", elTy, id, state, wordOffset + 2 * elWords);
							exposeVariable(shader, vecGroup, lane, "w", elTy, id, state, wordOffset + 3 * elWords);
							break;
						default:
							for(uint32_t i = 0; i < ty->components; i++)
							{
								exposeVariable(shader, vecGroup, lane, tostring(i).c_str(), elTy, id, state, wordOffset + i * elWords);
							}
							break;
					}
				}
				else if(auto ty = debug::cast<debug::CompositeType>(type))
				{
					auto objectGroup = group.group<Key>(key);

					for(auto member : ty->members)
					{
						exposeVariable(shader, objectGroup, lane, member->name.c_str(), member->type, id, state, member->offset / 32);
					}
				}
				else if(auto ty = debug::cast<debug::ArrayType>(type))
				{
					ty->build(
					    group.group<Key>(key),
					    [&](Debugger::Group &parent, uint32_t idx) {
						    return parent.template group<int>(idx);
					    },
					    [&](Debugger::Group &parent, uint32_t idx, uint32_t offset) {
						    exposeVariable(shader, parent, lane, idx, ty->base, id, state, offset);
					    });
				}
				else
				{
					UNIMPLEMENTED("b/145351270: Debug type: %s", cstr(type->kind));
				}
				return;
			}
			break;

			case Object::Kind::Unknown:
				UNIMPLEMENTED("b/145351270: Object kind: %d", (int)obj.kind);
		}
		return;
	}

	// No debug type information. Derive from SPIR-V.
	switch(shader->getType(obj).opcode())
	{
		case spv::OpTypeInt:
		{
			Operand val(shader, state, id);
			group.put<Key, int>(key, Extract(val.Int(0), lane));
		}
		break;
		case spv::OpTypeFloat:
		{
			Operand val(shader, state, id);
			group.put<Key, float>(key, Extract(val.Float(0), lane));
		}
		break;
		case spv::OpTypeVector:
		{
			Operand val(shader, state, id);
			auto count = shader->getType(obj).definition.word(3);
			switch(count)
			{
				case 1:
					group.put<Key, float>(key, Extract(val.Float(0), lane));
					break;
				case 2:
					group.put<Key, float>(key, Extract(val.Float(0), lane), Extract(val.Float(1), lane));
					break;
				case 3:
					group.put<Key, float>(key, Extract(val.Float(0), lane), Extract(val.Float(1), lane), Extract(val.Float(2), lane));
					break;
				case 4:
					group.put<Key, float>(key, Extract(val.Float(0), lane), Extract(val.Float(1), lane), Extract(val.Float(2), lane), Extract(val.Float(3), lane));
					break;
				default:
				{
					auto vec = group.group<Key>(key);
					for(uint32_t i = 0; i < count; i++)
					{
						vec.template put<int, float>(i, Extract(val.Float(i), lane));
					}
				}
				break;
			}
		}
		break;
		case spv::OpTypePointer:
		{
			auto objectTy = shader->getType(shader->getObject(id));
			bool interleavedByLane = IsStorageInterleavedByLane(objectTy.storageClass);
			auto ptr = state->getPointer(id);
			auto ptrGroup = group.group<Key>(key);
			shader->VisitMemoryObject(id, [&](const MemoryElement &el) {
				auto p = ptr + el.offset;
				if(interleavedByLane) { p = InterleaveByLane(p); }  // TODO: Interleave once, then add offset?
				auto simd = p.Load<SIMD::Float>(sw::OutOfBoundsBehavior::Nullify, state->activeLaneMask());
				ptrGroup.template put<int, float>(el.index, Extract(simd, lane));
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
	name += tostring(id++) + ".spvasm";
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
		auto globals = Group::globals(dbgState, i);
		globals.put<const char *, int>("subgroupSize", routine->invocationsPerSubgroup);

		switch(executionModel)
		{
			case spv::ExecutionModelGLCompute:
				globals.putVec3<const char *, int>("numWorkgroups", routine->numWorkgroups);
				globals.putVec3<const char *, int>("workgroupID", routine->workgroupID);
				globals.putVec3<const char *, int>("workgroupSize", routine->workgroupSize);
				globals.put<const char *, int>("numSubgroups", routine->subgroupsPerWorkgroup);
				globals.put<const char *, int>("subgroupIndex", routine->subgroupIndex);

				globals.put<const char *, int>("globalInvocationId",
				                               rr::Extract(routine->globalInvocationID[0], i),
				                               rr::Extract(routine->globalInvocationID[1], i),
				                               rr::Extract(routine->globalInvocationID[2], i));
				globals.put<const char *, int>("localInvocationId",
				                               rr::Extract(routine->localInvocationID[0], i),
				                               rr::Extract(routine->localInvocationID[1], i),
				                               rr::Extract(routine->localInvocationID[2], i));
				globals.put<const char *, int>("localInvocationIndex", rr::Extract(routine->localInvocationIndex, i));
				break;

			case spv::ExecutionModelFragment:
				globals.put<const char *, int>("viewIndex", routine->viewID);
				globals.put<const char *, float>("fragCoord",
				                                 rr::Extract(routine->fragCoord[0], i),
				                                 rr::Extract(routine->fragCoord[1], i),
				                                 rr::Extract(routine->fragCoord[2], i),
				                                 rr::Extract(routine->fragCoord[3], i));
				globals.put<const char *, float>("pointCoord",
				                                 rr::Extract(routine->pointCoord[0], i),
				                                 rr::Extract(routine->pointCoord[1], i));
				globals.put<const char *, int>("windowSpacePosition",
				                               rr::Extract(routine->windowSpacePosition[0], i),
				                               rr::Extract(routine->windowSpacePosition[1], i));
				globals.put<const char *, int>("helperInvocation", rr::Extract(routine->helperInvocation, i));
				break;

			case spv::ExecutionModelVertex:
				globals.put<const char *, int>("viewIndex", routine->viewID);
				globals.put<const char *, int>("instanceIndex", routine->instanceID);
				globals.put<const char *, int>("vertexIndex",
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
#	if PRINT_EACH_EMITTED_INSTRUCTION
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
#	endif  // PRINT_EACH_EMITTED_INSTRUCTION

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
		if(insn.opcode() == spv::OpLabel)
		{
			// Whenever we hit a label, force the next OpLine to be steppable.
			// This handles the case where we have control flow on the same line
			// For example:
			//   while(true) { foo(); }
			// foo() should be repeatedly steppable.
			dbg->setNextSetLocationIsStep();
		}

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

	// Don't display SSA values if rich debug info is available
	if(extensionsImported.count(Extension::OpenCLDebugInfo100) == 0)
	{
		// We're emitting debugger logic for SPIR-V.
		// Does this instruction emit a result that should be exposed to the
		// debugger?
		auto resIt = dbg->results.find(insn.wordPointer(0));
		if(resIt != dbg->results.end())
		{
			auto id = resIt->second;
			dbgExposeIntermediate(id, state);
		}
	}
}

void SpirvShader::dbgExposeIntermediate(Object::ID id, EmitState *state) const
{
	auto dbg = impl.debugger;
	if(!dbg) { return; }

	dbg->exposeVariable(this, id, &debug::Scope::Global, nullptr, id, state);
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
#	if PRINT_EACH_DEFINED_DBG_INSTRUCTION
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
#	endif  // PRINT_EACH_DEFINED_DBG_INSTRUCTION

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
