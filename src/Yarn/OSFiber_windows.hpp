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

#include <functional>
#include <memory>

#include <Windows.h>

namespace yarn {

class OSFiber
{
public:
	inline ~OSFiber();

    // createFiberFromCurrentThread() returns a fiber created from the current
    // thread.
    static inline OSFiber* createFiberFromCurrentThread();

    // createFiber() returns a new fiber with the given stack size that will
    // call func when switched to. func() must end by switching back to another
    // fiber, and must not return.
    static inline OSFiber* createFiber(size_t stackSize, const std::function<void()>& func);

    // switchTo() immediately switches execution to the given fiber.
    // switchTo() must be called on the currently executing fiber.
    inline void switchTo(OSFiber*);

private:
	static inline void WINAPI run(void* self);
	LPVOID fiber = nullptr;
	bool isFiberFromThread = false;
	std::function<void()> target;
};

OSFiber::~OSFiber()
{
	if (fiber != nullptr)
	{
		if (isFiberFromThread)
		{
			ConvertFiberToThread();
		}
		else
		{
			DeleteFiber(fiber);
		}
	}
}

OSFiber* OSFiber::createFiberFromCurrentThread()
{
	auto out = new OSFiber();
	out->fiber = ConvertThreadToFiber(nullptr);
	out->isFiberFromThread = true;
	return out;
}

OSFiber* OSFiber::createFiber(size_t stackSize, const std::function<void()>& func)
{
	auto out = new OSFiber();
	out->fiber = CreateFiber(stackSize, &OSFiber::run, out);
	out->target = func;
	return out;
}

void OSFiber::switchTo(OSFiber* fiber)
{
	SwitchToFiber(fiber->fiber);
}

void WINAPI OSFiber::run(void* self)
{
	std::function<void()> func;
	std::swap(func, reinterpret_cast<OSFiber*>(self)->target);
	func();
}

}  // namespace yarn
