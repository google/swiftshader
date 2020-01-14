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

#ifdef ENABLE_VK_DEBUGGER

#	include "Vulkan/Debug/Context.hpp"
#	include "Vulkan/Debug/File.hpp"
#	include "Vulkan/Debug/Thread.hpp"
#	include "Vulkan/Debug/Variable.hpp"

#	include "spirv-tools/libspirv.h"

#	include <algorithm>

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
std::string tostring(const char *s)
{
	return s;
}
std::string tostring(sw::SpirvShader::Object::ID id)
{
	return "%" + std::to_string(id.value());
}

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

	void setPosition(EmitState *state, const std::string &path, uint32_t line, uint32_t column);

	// exposeVariable exposes the variable with the given ID to the debugger
	// using the specified key.
	template<typename Key>
	void exposeVariable(
	    const SpirvShader *shader,
	    const Key &key,
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
	    Object::ID id,
	    EmitState *state,
	    int wordOffset = 0) const;

	std::shared_ptr<vk::dbg::Context> ctx;
	std::shared_ptr<vk::dbg::File> spirvFile;
	std::unordered_map<const void *, int> spirvLineMappings;  // instruction pointer to line
	std::unordered_map<const void *, Object::ID> results;     // instruction pointer to result ID

private:
	std::unordered_map<std::string, vk::dbg::File::ID> fileIDs;
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
	void update(vk::dbg::File::ID file, int line, int column);

	vk::dbg::VariableContainer *locals();
	vk::dbg::VariableContainer *hovers();
	vk::dbg::VariableContainer *localsLane(int lane);

	template<typename K>
	vk::dbg::VariableContainer *group(vk::dbg::VariableContainer *vc, K key);

	template<typename K, typename V>
	void putVal(vk::dbg::VariableContainer *vc, K key, V value);

	struct Scopes
	{
		std::shared_ptr<vk::dbg::Scope> locals;
		std::shared_ptr<vk::dbg::Scope> hovers;
		std::array<std::shared_ptr<vk::dbg::VariableContainer>, sw::SIMD::Width> localsByLane;
	};

	const Debugger *debugger;
	const std::shared_ptr<vk::dbg::Thread> thread;
	Scopes threadScopes;
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
	thread->update([&](vk::dbg::Frame &frame) {
		threadScopes.locals = frame.locals;
		threadScopes.hovers = frame.hovers;
		for(int i = 0; i < sw::SIMD::Width; i++)
		{
			threadScopes.localsByLane[i] = lock.createVariableContainer();
			frame.locals->variables->put(laneNames[i], threadScopes.localsByLane[i]);
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
	threadScopes.localsByLane[lane]->put("enabled", vk::dbg::make_constant(enabled));
}

void SpirvShader::Impl::Debugger::State::update(vk::dbg::File::ID fileID, int line, int column)
{
	auto file = debugger->ctx->lock().get(fileID);
	thread->update([&](vk::dbg::Frame &frame) {
		frame.location = { file, line, column };
	});
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::locals()
{
	return threadScopes.locals->variables.get();
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::hovers()
{
	return threadScopes.hovers->variables.get();
}

vk::dbg::VariableContainer *SpirvShader::Impl::Debugger::State::localsLane(int i)
{
	return threadScopes.localsByLane[i].get();
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

	static Group hovers(Ptr state);
	static Group locals(Ptr state);
	static Group localsLane(Ptr state, int lane);

	Group(Ptr state, Ptr group);

	template<typename K, typename RK>
	Group group(RK key) const;

	template<typename K, typename V, typename RK, typename RV>
	void put(RK key, RV value) const;

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
SpirvShader::Impl::Debugger::Group::hovers(Ptr state)
{
	return Group(state, rr::Call(&State::hovers, state));
}

SpirvShader::Impl::Debugger::Group
SpirvShader::Impl::Debugger::Group::locals(Ptr state)
{
	return Group(state, rr::Call(&State::locals, state));
}

SpirvShader::Impl::Debugger::Group
SpirvShader::Impl::Debugger::Group::localsLane(Ptr state, int lane)
{
	return Group(state, rr::Call(&State::localsLane, state, lane));
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
void SpirvShader::Impl::Debugger::setPosition(EmitState *state, const std::string &path, uint32_t line, uint32_t column)
{
	auto it = fileIDs.find(path);
	if(it != fileIDs.end())
	{
		rr::Call(&State::update, state->routine->dbgState, it->second, line, column);
	}
}

template<typename Key>
void SpirvShader::Impl::Debugger::exposeVariable(
    const SpirvShader *shader,
    const Key &key,
    Object::ID id,
    EmitState *state) const
{
	auto dbgState = state->routine->dbgState;
	auto hover = Group::hovers(dbgState).group<Key>(key);
	for(int lane = 0; lane < SIMD::Width; lane++)
	{
		exposeVariable(shader, Group::localsLane(dbgState, lane), lane, key, id, state);
		exposeVariable(shader, hover, lane, laneNames[lane], id, state);
	}
}

template<typename Key>
void SpirvShader::Impl::Debugger::exposeVariable(
    const SpirvShader *shader,
    const Group &group,
    int l,
    const Key &key,
    Object::ID id,
    EmitState *state,
    int wordOffset /* = 0 */) const
{
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

	auto locals = Group::locals(dbgState);
	locals.put<const char *, int>("subgroupSize", routine->invocationsPerSubgroup);

	switch(executionModel)
	{
		case spv::ExecutionModelGLCompute:
			locals.putVec3<const char *, int>("numWorkgroups", routine->numWorkgroups);
			locals.putVec3<const char *, int>("workgroupID", routine->workgroupID);
			locals.putVec3<const char *, int>("workgroupSize", routine->workgroupSize);
			locals.put<const char *, int>("numSubgroups", routine->subgroupsPerWorkgroup);
			locals.put<const char *, int>("subgroupIndex", routine->subgroupIndex);

			for(int i = 0; i < SIMD::Width; i++)
			{
				auto lane = Group::localsLane(dbgState, i);
				lane.put<const char *, int>("globalInvocationId",
				                            rr::Extract(routine->globalInvocationID[0], i),
				                            rr::Extract(routine->globalInvocationID[1], i),
				                            rr::Extract(routine->globalInvocationID[2], i));
				lane.put<const char *, int>("localInvocationId",
				                            rr::Extract(routine->localInvocationID[0], i),
				                            rr::Extract(routine->localInvocationID[1], i),
				                            rr::Extract(routine->localInvocationID[2], i));
				lane.put<const char *, int>("localInvocationIndex", rr::Extract(routine->localInvocationIndex, i));
			}
			break;

		case spv::ExecutionModelFragment:
			locals.put<const char *, int>("viewIndex", routine->viewID);
			for(int i = 0; i < SIMD::Width; i++)
			{
				auto lane = Group::localsLane(dbgState, i);
				lane.put<const char *, float>("fragCoord",
				                              rr::Extract(routine->fragCoord[0], i),
				                              rr::Extract(routine->fragCoord[1], i),
				                              rr::Extract(routine->fragCoord[2], i),
				                              rr::Extract(routine->fragCoord[3], i));
				lane.put<const char *, float>("pointCoord",
				                              rr::Extract(routine->pointCoord[0], i),
				                              rr::Extract(routine->pointCoord[1], i));
				lane.put<const char *, int>("windowSpacePosition",
				                            rr::Extract(routine->windowSpacePosition[0], i),
				                            rr::Extract(routine->windowSpacePosition[1], i));
				lane.put<const char *, int>("helperInvocation", rr::Extract(routine->helperInvocation, i));
			}
			break;

		default:
			break;
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
	auto instruction = spvtools::spvInstructionBinaryToText(
	    SPV_ENV_VULKAN_1_1,
	    insn.wordPointer(0),
	    insn.wordCount(),
	    insns.data(),
	    insns.size(),
	    SPV_BINARY_TO_TEXT_OPTION_NO_HEADER);
	printf("%s\n", instruction.c_str());
#	endif  // PRINT_EACH_PROCESSED_INSTRUCTION

	auto dbg = impl.debugger;
	if(!dbg) { return; }

	auto line = dbg->spirvLineMappings.at(insn.wordPointer(0));
	auto column = 0;
	rr::Call(&Impl::Debugger::State::update, state->routine->dbgState, dbg->spirvFile->id, line, column);
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

	dbg->exposeVariable(this, id, id, state);
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
		dbg->setPosition(state, path, line, column);
	}
	return EmitResult::Continue;
}

void SpirvShader::DefineOpenCLDebugInfo100(const InsnIterator &insn)
{
}

SpirvShader::EmitResult SpirvShader::EmitOpenCLDebugInfo100(InsnIterator insn, EmitState *state) const
{
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
