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

#ifndef VK_DEBUG_CONTEXT_HPP_
#define VK_DEBUG_CONTEXT_HPP_

#include "ID.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vk {
namespace dbg {

// Forward declarations.
class Thread;
class File;
class Frame;
class Scope;
class VariableContainer;
class EventListener;

// Context holds the full state of the debugger, including all current files,
// threads, frames and variables. It also holds a list of EventListeners that
// can be broadcast to using the Context::broadcast() interface.
// Context requires locking before accessing any state. The lock is
// non-reentrant and careful use is required to prevent accidentical
// double-locking by the same thread.
class Context
{
	class Impl;

public:
	// Lock is the interface to the Context's state.
	// The lock is automatically released when the Lock is destructed.
	class Lock
	{
	public:
		Lock(Impl *);
		Lock(Lock &&);
		~Lock();

		// move-assignment operator.
		Lock &operator=(Lock &&);

		// unlock() explicitly unlocks before the Lock destructor is called.
		// It is illegal to call any other methods after calling unlock().
		void unlock();

		// currentThread() creates (or returns an existing) a Thread that
		// represents the currently executing thread.
		std::shared_ptr<Thread> currentThread();

		// get() returns the thread with the given ID, or null if the thread
		// does not exist or no longer has any external shared_ptr references.
		std::shared_ptr<Thread> get(ID<Thread>);

		// threads() returns the full list of threads that still have an
		// external shared_ptr reference.
		std::vector<std::shared_ptr<Thread>> threads();

		// createVirtualFile() returns a new file that is not backed by the
		// filesystem.
		// name is the unique name of the file.
		// source is the content of the file.
		std::shared_ptr<File> createVirtualFile(const std::string &name,
		                                        const std::string &source);

		// createPhysicalFile() returns a new file that is backed by the file
		// at path.
		std::shared_ptr<File> createPhysicalFile(const std::string &path);

		// get() returns the file with the given ID, or null if the file
		// does not exist or no longer has any external shared_ptr references.
		std::shared_ptr<File> get(ID<File>);

		// files() returns the full list of files.
		std::vector<std::shared_ptr<File>> files();

		// createFrame() returns a new frame for the given file and function
		// name.
		std::shared_ptr<Frame> createFrame(
		    const std::shared_ptr<File> &file, std::string function);

		// get() returns the frame with the given ID, or null if the frame
		// does not exist or no longer has any external shared_ptr references.
		std::shared_ptr<Frame> get(ID<Frame>);

		// createScope() returns a new scope for the given file.
		std::shared_ptr<Scope> createScope(
		    const std::shared_ptr<File> &file);

		// get() returns the scope with the given ID, or null if the scope
		// does not exist.
		std::shared_ptr<Scope> get(ID<Scope>);

		// createVariableContainer() returns a new variable container.
		std::shared_ptr<VariableContainer> createVariableContainer();

		// get() returns the variable container with the given ID, or null if
		// the variable container does not exist or no longer has any external
		// shared_ptr references.
		std::shared_ptr<VariableContainer> get(ID<VariableContainer>);

		// addFunctionBreakpoint() adds a breakpoint to the start of the
		// function with the given name.
		void addFunctionBreakpoint(const std::string &name);

		// addPendingBreakpoints() adds a number of breakpoints to the file with
		// the given name which has not yet been created with a call to
		// createVirtualFile() or createPhysicalFile().
		void addPendingBreakpoints(const std::string &name, const std::vector<int> &lines);

		// isFunctionBreakpoint() returns true if the function with the given
		// name has a function breakpoint set.
		bool isFunctionBreakpoint(const std::string &name);

	private:
		Lock(const Lock &) = delete;
		Lock &operator=(const Lock &) = delete;
		Impl *ctx;
	};

	// create() creates and returns a new Context.
	static std::shared_ptr<Context> create();

	virtual ~Context() = default;

	// lock() returns a Lock which exclusively locks the context for state
	// access.
	virtual Lock lock() = 0;

	// addListener() registers an EventListener for event notifications.
	virtual void addListener(EventListener *) = 0;

	// removeListener() unregisters an EventListener that was previously
	// registered by a call to addListener().
	virtual void removeListener(EventListener *) = 0;

	// broadcast() returns an EventListener that will broadcast all methods on
	// to all registered EventListeners.
	virtual EventListener *broadcast() = 0;
};

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_CONTEXT_HPP_
