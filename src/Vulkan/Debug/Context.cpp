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

#include "Context.hpp"

#include "EventListener.hpp"
#include "File.hpp"
#include "Thread.hpp"
#include "Variable.hpp"
#include "WeakMap.hpp"

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace {

class Broadcaster : public vk::dbg::EventListener
{
public:
	using Thread = vk::dbg::Thread;

	// EventListener
	void onThreadStarted(Thread::ID) override;
	void onThreadStepped(Thread::ID) override;
	void onLineBreakpointHit(Thread::ID) override;
	void onFunctionBreakpointHit(Thread::ID) override;

	void add(EventListener *);
	void remove(EventListener *);

private:
	template<typename F>
	inline void foreach(F &&);

	template<typename F>
	inline void modify(F &&);

	using ListenerSet = std::unordered_set<EventListener *>;
	std::recursive_mutex mutex;
	std::shared_ptr<ListenerSet> listeners = std::make_shared<ListenerSet>();
	int listenersInUse = 0;
};

void Broadcaster::onThreadStarted(Thread::ID id)
{
	foreach([&](EventListener *l) { l->onThreadStarted(id); });
}

void Broadcaster::onThreadStepped(Thread::ID id)
{
	foreach([&](EventListener *l) { l->onThreadStepped(id); });
}

void Broadcaster::onLineBreakpointHit(Thread::ID id)
{
	foreach([&](EventListener *l) { l->onLineBreakpointHit(id); });
}

void Broadcaster::onFunctionBreakpointHit(Thread::ID id)
{
	foreach([&](EventListener *l) { l->onFunctionBreakpointHit(id); });
}

void Broadcaster::add(EventListener *l)
{
	modify([&]() { listeners->emplace(l); });
}

void Broadcaster::remove(EventListener *l)
{
	modify([&]() { listeners->erase(l); });
}

template<typename F>
void Broadcaster::foreach(F &&f)
{
	std::unique_lock<std::recursive_mutex> lock(mutex);
	++listenersInUse;
	auto copy = listeners;
	for(auto l : *copy) { f(l); }
	--listenersInUse;
}

template<typename F>
void Broadcaster::modify(F &&f)
{
	std::unique_lock<std::recursive_mutex> lock(mutex);
	if(listenersInUse > 0)
	{
		// The listeners map is current being iterated over.
		// Make a copy before making the edit.
		listeners = std::make_shared<ListenerSet>(*listeners);
	}
	f();
}

}  // namespace

namespace vk {
namespace dbg {

////////////////////////////////////////////////////////////////////////////////
// Context::Impl
////////////////////////////////////////////////////////////////////////////////
class Context::Impl : public Context
{
public:
	// Context compliance
	Lock lock() override;
	void addListener(EventListener *) override;
	void removeListener(EventListener *) override;
	EventListener *broadcast() override;

	void addFile(const std::shared_ptr<File> &file);

	Broadcaster broadcaster;

	std::mutex mutex;
	std::vector<EventListener *> eventListeners;
	std::unordered_map<std::thread::id, std::shared_ptr<Thread>> threadsByStdId;
	std::unordered_set<std::string> functionBreakpoints;
	std::unordered_map<std::string, std::vector<int>> pendingBreakpoints;
	WeakMap<Thread::ID, Thread> threads;
	WeakMap<File::ID, File> files;
	WeakMap<Frame::ID, Frame> frames;
	WeakMap<Scope::ID, Scope> scopes;
	WeakMap<VariableContainer::ID, VariableContainer> variableContainers;
	Thread::ID nextThreadID = 1;
	File::ID nextFileID = 1;
	Frame::ID nextFrameID = 1;
	Scope::ID nextScopeID = 1;
	VariableContainer::ID nextVariableContainerID = 1;
};

Context::Lock Context::Impl::lock()
{
	return Lock(this);
}

void Context::Impl::addListener(EventListener *l)
{
	broadcaster.add(l);
}

void Context::Impl::removeListener(EventListener *l)
{
	broadcaster.remove(l);
}

EventListener *Context::Impl::broadcast()
{
	return &broadcaster;
}

void Context::Impl::addFile(const std::shared_ptr<File> &file)
{
	files.add(file->id, file);

	auto it = pendingBreakpoints.find(file->name);
	if(it != pendingBreakpoints.end())
	{
		for(auto line : it->second)
		{
			file->addBreakpoint(line);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Context
////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Context> Context::create()
{
	return std::shared_ptr<Context>(new Context::Impl());
}

////////////////////////////////////////////////////////////////////////////////
// Context::Lock
////////////////////////////////////////////////////////////////////////////////
Context::Lock::Lock(Impl *ctx)
    : ctx(ctx)
{
	ctx->mutex.lock();
}

Context::Lock::Lock(Lock &&o)
    : ctx(o.ctx)
{
	o.ctx = nullptr;
}

Context::Lock::~Lock()
{
	unlock();
}

Context::Lock &Context::Lock::operator=(Lock &&o)
{
	ctx = o.ctx;
	o.ctx = nullptr;
	return *this;
}

void Context::Lock::unlock()
{
	if(ctx)
	{
		ctx->mutex.unlock();
		ctx = nullptr;
	}
}

std::shared_ptr<Thread> Context::Lock::currentThread()
{
	auto threadIt = ctx->threadsByStdId.find(std::this_thread::get_id());
	if(threadIt != ctx->threadsByStdId.end())
	{
		return threadIt->second;
	}
	auto id = ++ctx->nextThreadID;
	char name[256];
	snprintf(name, sizeof(name), "Thread<0x%x>", id.value());

	auto thread = std::make_shared<Thread>(id, ctx);
	ctx->threads.add(id, thread);
	thread->setName(name);
	ctx->threadsByStdId.emplace(std::this_thread::get_id(), thread);

	ctx->broadcast()->onThreadStarted(id);

	return thread;
}

std::shared_ptr<Thread> Context::Lock::get(Thread::ID id)
{
	return ctx->threads.get(id);
}

std::vector<std::shared_ptr<Thread>> Context::Lock::threads()
{
	std::vector<std::shared_ptr<Thread>> out;
	out.reserve(ctx->threads.approx_size());
	for(auto it : ctx->threads)
	{
		out.push_back(it.second);
	}
	return out;
}

std::shared_ptr<File> Context::Lock::createVirtualFile(const std::string &name,
                                                       const std::string &source)
{
	auto file = File::createVirtual(ctx->nextFileID++, name, source);
	ctx->addFile(file);
	return file;
}

std::shared_ptr<File> Context::Lock::createPhysicalFile(const std::string &path)
{
	auto file = File::createPhysical(ctx->nextFileID++, path);
	ctx->addFile(file);
	return file;
}

std::shared_ptr<File> Context::Lock::get(File::ID id)
{
	return ctx->files.get(id);
}

std::vector<std::shared_ptr<File>> Context::Lock::files()
{
	std::vector<std::shared_ptr<File>> out;
	out.reserve(ctx->files.approx_size());
	for(auto it : ctx->files)
	{
		out.push_back(it.second);
	}
	return out;
}

std::shared_ptr<Frame> Context::Lock::createFrame(
    const std::shared_ptr<File> &file, std::string function)
{
	auto frame = std::make_shared<Frame>(ctx->nextFrameID++, std::move(function));
	ctx->frames.add(frame->id, frame);
	frame->arguments = createScope(file);
	frame->locals = createScope(file);
	frame->registers = createScope(file);
	frame->hovers = createScope(file);
	return frame;
}

std::shared_ptr<Frame> Context::Lock::get(Frame::ID id)
{
	return ctx->frames.get(id);
}

std::shared_ptr<Scope> Context::Lock::createScope(
    const std::shared_ptr<File> &file)
{
	auto scope = std::make_shared<Scope>(ctx->nextScopeID++, file, createVariableContainer());
	ctx->scopes.add(scope->id, scope);
	return scope;
}

std::shared_ptr<Scope> Context::Lock::get(Scope::ID id)
{
	return ctx->scopes.get(id);
}

std::shared_ptr<VariableContainer> Context::Lock::createVariableContainer()
{
	auto container = std::make_shared<VariableContainer>(ctx->nextVariableContainerID++);
	ctx->variableContainers.add(container->id, container);
	return container;
}

std::shared_ptr<VariableContainer> Context::Lock::get(VariableContainer::ID id)
{
	return ctx->variableContainers.get(id);
}

void Context::Lock::addFunctionBreakpoint(const std::string &name)
{
	ctx->functionBreakpoints.emplace(name);
}

void Context::Lock::addPendingBreakpoints(const std::string &filename, const std::vector<int> &lines)
{
	ctx->pendingBreakpoints.emplace(filename, lines);
}

bool Context::Lock::isFunctionBreakpoint(const std::string &name)
{
	return ctx->functionBreakpoints.count(name) > 0;
}

}  // namespace dbg
}  // namespace vk