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

#ifndef VK_DEBUG_EVENT_LISTENER_HPP_
#define VK_DEBUG_EVENT_LISTENER_HPP_

namespace vk {
namespace dbg {

class Thread;

// EventListener is an interface that is used to listen for thread events.
class EventListener
{
public:
	virtual ~EventListener() = default;

	// onThreadStarted() is called when a new thread begins execution.
	virtual void onThreadStarted(ID<Thread>) {}

	// onThreadStepped() is called when a thread performs a single line /
	// instruction step.
	virtual void onThreadStepped(ID<Thread>) {}

	// onLineBreakpointHit() is called when a thread hits a line breakpoint and
	// pauses execution.
	virtual void onLineBreakpointHit(ID<Thread>) {}

	// onFunctionBreakpointHit() is called when a thread hits a function
	// breakpoint and pauses execution.
	virtual void onFunctionBreakpointHit(ID<Thread>) {}
};

}  // namespace dbg
}  // namespace vk

#endif  // VK_DEBUG_EVENT_LISTENER_HPP_
