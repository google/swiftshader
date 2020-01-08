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

#ifndef VK_DEBUG_THREAD_HPP_
#define VK_DEBUG_THREAD_HPP_

#include "Context.hpp"
#include "ID.hpp"
#include "Location.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace vk {
namespace dbg {

class File;
class VariableContainer;
class EventListener;

// Scope is a container for variables and is used to provide source data for the
// DAP 'Scope' type:
// https://microsoft.github.io/debug-adapter-protocol/specification#Types_Scope
class Scope
{
public:
	using ID = dbg::ID<Scope>;

	inline Scope(ID id,
	             const std::shared_ptr<File> &file,
	             const std::shared_ptr<VariableContainer> &variables);

	// The unique identifier of the scope.
	const ID id;

	// The file this scope is associated with.
	const std::shared_ptr<File> file;

	// The scope's variables.
	const std::shared_ptr<VariableContainer> variables;
};

Scope::Scope(ID id,
             const std::shared_ptr<File> &file,
             const std::shared_ptr<VariableContainer> &variables)
    : id(id)
    , file(file)
    , variables(variables)
{}

// Frame holds a number of variable scopes for one of a thread's stack frame,
// and is used to provide source data for the DAP 'StackFrame' type:
// https://microsoft.github.io/debug-adapter-protocol/specification#Types_StackFrame
class Frame
{
public:
	using ID = dbg::ID<Frame>;

	inline Frame(ID id, std::string function);

	// The unique identifier of the stack frame.
	const ID id;

	// The name of function for this stack frame.
	const std::string function;

	// The current execution location within the stack frame.
	Location location;

	// The scope for the frame's arguments.
	std::shared_ptr<Scope> arguments;

	// The scope for the frame's locals.
	std::shared_ptr<Scope> locals;

	// The scope for the frame's registers.
	std::shared_ptr<Scope> registers;

	// The scope for variables that should only appear in hover tooltips.
	std::shared_ptr<Scope> hovers;
};

Frame::Frame(ID id, std::string function)
    : id(id)
    , function(std::move(function))
{}

// Thread holds the state for a single thread of execution.
class Thread
{
public:
	using ID = dbg::ID<Thread>;

	// The current execution state.
	enum class State
	{
		Running,   // Thread is running.
		Stepping,  // Thread is currently single line stepping.
		Paused     // Thread is currently paused.
	};

	Thread(ID id, Context *ctx);

	// setName() sets the name of the thread.
	void setName(const std::string &);

	// name() returns the name of the thread.
	std::string name() const;

	// enter() pushes the thread's stack with a new frame created with the given
	// file and function.
	void enter(Context::Lock &lock, const std::shared_ptr<File> &file, const std::string &function);

	// exit() pops the thread's stack frame.
	void exit();

	// frame() returns a copy of the thread's top most stack frame.
	Frame frame() const;

	// stack() returns a copy of the thread's current stack frames.
	std::vector<Frame> stack() const;

	// state() returns the current thread's state.
	State state() const;

	// update() calls f to modify the top most frame of the stack.
	// If the frame's location is changed, update() potentially blocks until the
	// thread is resumed with one of the methods below.
	void update(std::function<void(Frame &)> f);

	// resume() resumes execution of the thread by unblocking a call to
	// update() and setting the thread's state to State::Running.
	void resume();

	// pause() suspends execution of the thread by blocking the next call to
	// update() and setting the thread's state to State::Paused.
	void pause();

	// stepIn() temporarily resumes execution of the thread by unblocking a
	// call to update(), and setting the thread's state to State::Stepping.
	// The next call to update() will suspend execution again.
	void stepIn();

	// stepOver() temporarily resumes execution of the thread by unblocking a
	// call to update(), and setting the thread's state to State::Stepping.
	// The next call to update() within the same stack frame will suspend
	// execution again.
	void stepOver();

	// stepOut() temporarily resumes execution of the thread by unblocking a
	// call to update(), and setting the thread's state to State::Stepping.
	// The next call to update() at the stack frame above the current frame will
	// suspend execution again.
	void stepOut();

	// The unique identifier of the thread.
	const ID id;

private:
	EventListener *const broadcast;

	void onLocationUpdate(std::unique_lock<std::mutex> &lock);

	mutable std::mutex mutex;
	std::string name_;
	std::vector<std::shared_ptr<Frame>> frames;
	std::condition_variable stateCV;
	State state_ = State::Running;
	std::shared_ptr<Frame> pauseAtFrame;
};

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_THREAD_HPP_
