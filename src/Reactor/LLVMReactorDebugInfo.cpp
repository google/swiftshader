// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "LLVMReactorDebugInfo.hpp"

#ifdef ENABLE_RR_DEBUG_INFO

#	include "LLVMReactor.hpp"
#	include "Reactor.hpp"
#	include "Print.hpp"

#	include "boost/stacktrace.hpp"

// TODO(b/143539525): Eliminate when warning has been fixed.
#	ifdef _MSC_VER
__pragma(warning(push))
    __pragma(warning(disable : 4146))  // unary minus operator applied to unsigned type, result still unsigned
#	endif

#	include "llvm/Demangle/Demangle.h"
#	include "llvm/ExecutionEngine/JITEventListener.h"
#	include "llvm/IR/DIBuilder.h"
#	include "llvm/IR/IRBuilder.h"
#	include "llvm/IR/Intrinsics.h"

#	ifdef _MSC_VER
    __pragma(warning(pop))
#	endif

#	include <cctype>
#	include <fstream>
#	include <mutex>
#	include <regex>
#	include <sstream>
#	include <string>

#	if 0
#		define LOG(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#	else
#		define LOG(msg, ...)
#	endif

        namespace
{

	std::pair<llvm::StringRef, llvm::StringRef> splitPath(const char *path)
	{
		return llvm::StringRef(path).rsplit('/');
	}

	// Note: createGDBRegistrationListener() returns a pointer to a singleton.
	// Nothing is actually created.
	auto jitEventListener = llvm::JITEventListener::createGDBRegistrationListener();  // guarded by jitEventListenerMutex
	std::mutex jitEventListenerMutex;

}  // namespace

namespace rr {

DebugInfo::DebugInfo(
    llvm::IRBuilder<> *builder,
    llvm::LLVMContext *context,
    llvm::Module *module,
    llvm::Function *function)
    : builder(builder)
    , context(context)
    , module(module)
    , function(function)
{
	using namespace ::llvm;

	auto location = getCallerLocation();

	auto fileAndDir = splitPath(location.function.file.c_str());
	diBuilder.reset(new llvm::DIBuilder(*module));
	diCU = diBuilder->createCompileUnit(
	    llvm::dwarf::DW_LANG_C,
	    diBuilder->createFile(fileAndDir.first, fileAndDir.second),
	    "Reactor",
	    0, "", 0);

	registerBasicTypes();

	SmallVector<Metadata *, 8> EltTys;
	auto funcTy = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(EltTys));

	auto file = getOrCreateFile(location.function.file.c_str());
	auto sp = diBuilder->createFunction(
	    file,                    // scope
	    "ReactorFunction",       // function name
	    "ReactorFunction",       // linkage
	    file,                    // file
	    location.line,           // line
	    funcTy,                  // type
	    false,                   // internal linkage
	    true,                    // definition
	    location.line,           // scope line
	    DINode::FlagPrototyped,  // flags
	    false                    // is optimized
	);
	diSubprogram = sp;
	function->setSubprogram(sp);
	diRootLocation = DILocation::get(*context, location.line, 0, sp);
	builder->SetCurrentDebugLocation(diRootLocation);
}

DebugInfo::~DebugInfo() = default;

void DebugInfo::Finalize()
{
	while(diScope.size() > 0)
	{
		emitPending(diScope.back(), builder);
		diScope.pop_back();
	}
	diBuilder->finalize();
}

void DebugInfo::EmitLocation()
{
	auto const &backtrace = getCallerBacktrace();
	syncScope(backtrace);
	builder->SetCurrentDebugLocation(getLocation(backtrace, backtrace.size() - 1));

#	ifdef ENABLE_RR_EMIT_PRINT_LOCATION
	emitPrintLocation(backtrace);
#	endif  // ENABLE_RR_EMIT_PRINT_LOCATION
}

void DebugInfo::Flush()
{
	emitPending(diScope.back(), builder);
}

void DebugInfo::syncScope(Backtrace const &backtrace)
{
	auto shrink = [this](size_t newsize) {
		while(diScope.size() > newsize)
		{
			auto &scope = diScope.back();
			LOG("- STACK(%d): di: %p, location: %s:%d",
			    int(diScope.size() - 1), scope.di,
			    scope.location.function.file.c_str(),
			    int(scope.location.line));
			emitPending(scope, builder);
			diScope.pop_back();
		}
	};

	if(backtrace.size() < diScope.size())
	{
		shrink(backtrace.size());
	}

	for(size_t i = 0; i < diScope.size(); i++)
	{
		auto &scope = diScope[i];
		auto const &oldLocation = scope.location;
		auto const &newLocation = backtrace[i];

		if(oldLocation.function != newLocation.function)
		{
			LOG("  STACK(%d): Changed function %s -> %s", int(i),
			    oldLocation.function.name.c_str(), newLocation.function.name.c_str());
			shrink(i);
			break;
		}

		if(oldLocation.line > newLocation.line)
		{
			// Create a new di block to shadow all the variables in the loop.
			auto file = getOrCreateFile(newLocation.function.file.c_str());
			auto di = diBuilder->createLexicalBlock(scope.di, file, newLocation.line, 0);
			LOG("  STACK(%d): Jumped backwards %d -> %d. di: %p -> %p", int(i),
			    oldLocation.line, newLocation.line, scope.di, di);
			emitPending(scope, builder);
			scope = { newLocation, di };
			shrink(i + 1);
			break;
		}

		scope.location = newLocation;
	}

	while(backtrace.size() > diScope.size())
	{
		auto i = diScope.size();
		auto location = backtrace[i];
		auto file = getOrCreateFile(location.function.file.c_str());
		auto funcTy = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray({}));

		char buf[1024];
		size_t size = sizeof(buf);
		int status = 0;
		llvm::itaniumDemangle(location.function.name.c_str(), buf, &size, &status);
		auto name = "jit!" + (status == 0 ? std::string(buf) : location.function.name);

		auto func = diBuilder->createFunction(
		    file,                          // scope
		    name,                          // function name
		    "",                            // linkage
		    file,                          // file
		    location.line,                 // line
		    funcTy,                        // type
		    false,                         // internal linkage
		    true,                          // definition
		    location.line,                 // scope line
		    llvm::DINode::FlagPrototyped,  // flags
		    false                          // is optimized
		);
		diScope.push_back({ location, func });
		LOG("+ STACK(%d): di: %p, location: %s:%d", int(i), di,
		    location.function.file.c_str(), int(location.line));
	}
}

llvm::DILocation *DebugInfo::getLocation(const Backtrace &backtrace, size_t i)
{
	if(backtrace.size() == 0) { return nullptr; }
	assert(backtrace.size() == diScope.size());
	return llvm::DILocation::get(
	    *context,
	    backtrace[i].line,
	    0,
	    diScope[i].di,
	    i > 0 ? getLocation(backtrace, i - 1) : diRootLocation);
}

void DebugInfo::EmitVariable(Value *variable)
{
	auto const &backtrace = getCallerBacktrace();
	syncScope(backtrace);

	for(int i = backtrace.size() - 1; i >= 0; i--)
	{
		auto const &location = backtrace[i];
		auto tokens = getOrParseFileTokens(location.function.file.c_str());
		auto tokIt = tokens->find(location.line);
		if(tokIt == tokens->end())
		{
			break;
		}
		auto token = tokIt->second;
		auto name = token.identifier;
		if(token.kind == Token::Return)
		{
			// This is a:
			//
			//   return <expr>;
			//
			// Emit this expression as two variables -
			// Once as a synthetic 'return_value' variable at this scope.
			// Again by bubbling the expression value up the callstack as
			// Return Value Optimizations (RVOs) are likely to carry across
			// the value to a local without calling a constructor in
			// statements like:
			//
			//   auto val = foo();
			//
			name = "return_value";
		}

		auto &scope = diScope[i];
		if(scope.pending.location != location)
		{
			emitPending(scope, builder);
		}

		auto value = V(variable);
		auto block = builder->GetInsertBlock();

		auto insertAfter = block->size() > 0 ? &block->back() : nullptr;
		while(insertAfter != nullptr && insertAfter->isTerminator())
		{
			insertAfter = insertAfter->getPrevNode();
		}

		scope.pending = Pending{};
		scope.pending.name = name;
		scope.pending.location = location;
		scope.pending.diLocation = getLocation(backtrace, i);
		scope.pending.value = value;
		scope.pending.block = block;
		scope.pending.insertAfter = insertAfter;
		scope.pending.scope = scope.di;

		if(token.kind == Token::Return)
		{
			// Insert a noop instruction so the debugger can inspect the
			// return value before the function scope closes.
			scope.pending.addNopOnNextLine = true;
		}
		else
		{
			break;
		}
	}
}

void DebugInfo::emitPending(Scope &scope, IRBuilder *builder)
{
	auto const &pending = scope.pending;
	if(pending.value == nullptr)
	{
		return;
	}

	if(!scope.symbols.emplace(pending.name).second)
	{
		return;
	}

	bool isAlloca = llvm::isa<llvm::AllocaInst>(pending.value);

	LOG("  EMIT(%s): di: %p, location: %s:%d, isAlloca: %s", pending.name.c_str(), scope.di,
	    pending.location.function.file.c_str(), pending.location.line, isAlloca ? "true" : "false");

	auto value = pending.value;

	IRBuilder::InsertPointGuard guard(*builder);
	if(pending.insertAfter != nullptr)
	{
		builder->SetInsertPoint(pending.block, ++pending.insertAfter->getIterator());
	}
	else
	{
		builder->SetInsertPoint(pending.block);
	}
	builder->SetCurrentDebugLocation(pending.diLocation);

	if(!isAlloca)
	{
		// While insertDbgValueIntrinsic should be enough to declare a
		// variable with no storage, variables of RValues can share the same
		// llvm::Value, and only one can be named. Take for example:
		//
		//   Int a = 42;
		//   RValue<Int> b = a;
		//   RValue<Int> c = b;
		//
		// To handle this, always promote named RValues to an alloca.

		llvm::BasicBlock &entryBlock = function->getEntryBlock();
		auto alloca = new llvm::AllocaInst(value->getType(), 0, pending.name);
		entryBlock.getInstList().push_front(alloca);
		builder->CreateStore(value, alloca);
		value = alloca;
	}

	value->setName(pending.name);

	auto diFile = getOrCreateFile(pending.location.function.file.c_str());
	auto diType = getOrCreateType(value->getType()->getPointerElementType());
	auto diVar = diBuilder->createAutoVariable(scope.di, pending.name, diFile, pending.location.line, diType);

	auto di = diBuilder->insertDeclare(value, diVar, diBuilder->createExpression(), pending.diLocation, pending.block);
	if(pending.insertAfter != nullptr) { di->moveAfter(pending.insertAfter); }

	if(pending.addNopOnNextLine)
	{
		builder->SetCurrentDebugLocation(llvm::DILocation::get(
		    *context,
		    pending.diLocation->getLine() + 1,
		    0,
		    pending.diLocation->getScope(),
		    pending.diLocation->getInlinedAt()));
		Nop();
	}

	scope.pending = Pending{};
}

void DebugInfo::NotifyObjectEmitted(const llvm::object::ObjectFile &Obj, const llvm::LoadedObjectInfo &L)
{
	std::unique_lock<std::mutex> lock(jitEventListenerMutex);
	jitEventListener->NotifyObjectEmitted(Obj, static_cast<const llvm::RuntimeDyld::LoadedObjectInfo &>(L));
}

void DebugInfo::NotifyFreeingObject(const llvm::object::ObjectFile &Obj)
{
	std::unique_lock<std::mutex> lock(jitEventListenerMutex);
	jitEventListener->NotifyFreeingObject(Obj);
}

void DebugInfo::registerBasicTypes()
{
	using namespace rr;
	using namespace llvm;

	auto vec4 = diBuilder->getOrCreateArray(diBuilder->getOrCreateSubrange(0, 4));
	auto vec8 = diBuilder->getOrCreateArray(diBuilder->getOrCreateSubrange(0, 8));
	auto vec16 = diBuilder->getOrCreateArray(diBuilder->getOrCreateSubrange(0, 16));

	diTypes.emplace(T(Bool::getType()), diBuilder->createBasicType("Bool", sizeof(bool), dwarf::DW_ATE_boolean));
	diTypes.emplace(T(Byte::getType()), diBuilder->createBasicType("Byte", 8, dwarf::DW_ATE_unsigned_char));
	diTypes.emplace(T(SByte::getType()), diBuilder->createBasicType("SByte", 8, dwarf::DW_ATE_signed_char));
	diTypes.emplace(T(Short::getType()), diBuilder->createBasicType("Short", 16, dwarf::DW_ATE_signed));
	diTypes.emplace(T(UShort::getType()), diBuilder->createBasicType("UShort", 16, dwarf::DW_ATE_unsigned));
	diTypes.emplace(T(Int::getType()), diBuilder->createBasicType("Int", 32, dwarf::DW_ATE_signed));
	diTypes.emplace(T(UInt::getType()), diBuilder->createBasicType("UInt", 32, dwarf::DW_ATE_unsigned));
	diTypes.emplace(T(Long::getType()), diBuilder->createBasicType("Long", 64, dwarf::DW_ATE_signed));
	diTypes.emplace(T(Half::getType()), diBuilder->createBasicType("Half", 16, dwarf::DW_ATE_float));
	diTypes.emplace(T(Float::getType()), diBuilder->createBasicType("Float", 32, dwarf::DW_ATE_float));

	diTypes.emplace(T(Byte4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Byte::getType())], { vec16 }));
	diTypes.emplace(T(SByte4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(SByte::getType())], { vec16 }));
	diTypes.emplace(T(Byte8::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Byte::getType())], { vec16 }));
	diTypes.emplace(T(SByte8::getType()), diBuilder->createVectorType(128, 128, diTypes[T(SByte::getType())], { vec16 }));
	diTypes.emplace(T(Byte16::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Byte::getType())], { vec16 }));
	diTypes.emplace(T(SByte16::getType()), diBuilder->createVectorType(128, 128, diTypes[T(SByte::getType())], { vec16 }));
	diTypes.emplace(T(Short2::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Short::getType())], { vec8 }));
	diTypes.emplace(T(UShort2::getType()), diBuilder->createVectorType(128, 128, diTypes[T(UShort::getType())], { vec8 }));
	diTypes.emplace(T(Short4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Short::getType())], { vec8 }));
	diTypes.emplace(T(UShort4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(UShort::getType())], { vec8 }));
	diTypes.emplace(T(Short8::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Short::getType())], { vec8 }));
	diTypes.emplace(T(UShort8::getType()), diBuilder->createVectorType(128, 128, diTypes[T(UShort::getType())], { vec8 }));
	diTypes.emplace(T(Int2::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Int::getType())], { vec4 }));
	diTypes.emplace(T(UInt2::getType()), diBuilder->createVectorType(128, 128, diTypes[T(UInt::getType())], { vec4 }));
	diTypes.emplace(T(Int4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Int::getType())], { vec4 }));
	diTypes.emplace(T(UInt4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(UInt::getType())], { vec4 }));
	diTypes.emplace(T(Float2::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Float::getType())], { vec4 }));
	diTypes.emplace(T(Float4::getType()), diBuilder->createVectorType(128, 128, diTypes[T(Float::getType())], { vec4 }));
}

Location DebugInfo::getCallerLocation() const
{
	return getCallerBacktrace(1)[0];
}

Backtrace DebugInfo::getCallerBacktrace(size_t limit /* = 0 */) const
{
	return rr::getCallerBacktrace(limit);
}

llvm::DIType *DebugInfo::getOrCreateType(llvm::Type *type)
{
	auto it = diTypes.find(type);
	if(it != diTypes.end()) { return it->second; }

	if(type->isPointerTy())
	{
		auto dbgTy = diBuilder->createPointerType(
		    getOrCreateType(type->getPointerElementType()),
		    sizeof(void *) * 8, alignof(void *) * 8);
		diTypes.emplace(type, dbgTy);
		return dbgTy;
	}
	llvm::errs() << "Unimplemented debug type: " << type << "\n";
	assert(false);
	return nullptr;
}

llvm::DIFile *DebugInfo::getOrCreateFile(const char *path)
{
	auto it = diFiles.find(path);
	if(it != diFiles.end()) { return it->second; }
	auto dirAndName = splitPath(path);
	auto file = diBuilder->createFile(dirAndName.second, dirAndName.first);
	diFiles.emplace(path, file);
	return file;
}

DebugInfo::LineTokens const *DebugInfo::getOrParseFileTokens(const char *path)
{
	static std::regex reLocalDecl(
	    "^"                                              // line start
	    "\\s*"                                           // initial whitespace
	    "(?:For\\s*\\(\\s*)?"                            // optional 'For('
	    "((?:\\w+(?:<[^>]+>)?)(?:::\\w+(?:<[^>]+>)?)*)"  // type (match group 1)
	    "\\s+"                                           // whitespace between type and name
	    "(\\w+)"                                         // identifier (match group 2)
	    "\\s*"                                           // whitespace after identifier
	    "(\\[.*\\])?");                                  // optional array suffix (match group 3)

	auto it = fileTokens.find(path);
	if(it != fileTokens.end())
	{
		return it->second.get();
	}

	auto tokens = std::make_unique<LineTokens>();

	std::ifstream file(path);
	std::string line;
	int lineCount = 0;
	while(std::getline(file, line))
	{
		lineCount++;
		std::smatch match;
		if(std::regex_search(line, match, reLocalDecl) && match.size() > 3)
		{
			bool isArray = match.str(3) != "";
			if(!isArray)  // Cannot deal with C-arrays of values.
			{
				if(match.str(1) == "return")
				{
					(*tokens)[lineCount] = Token{ Token::Return };
				}
				else
				{
					(*tokens)[lineCount] = Token{ Token::Identifier, match.str(2) };
				}
			}
		}
	}

	auto out = tokens.get();
	fileTokens.emplace(path, std::move(tokens));
	return out;
}

}  // namespace rr

#endif  // ENABLE_RR_DEBUG_INFO
