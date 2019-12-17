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

#ifndef rr_Routine_hpp
#define rr_Routine_hpp

#include <memory>

namespace rr {

class Routine
{
public:
	Routine() = default;
	virtual ~Routine() = default;

	virtual const void *getEntry(int index = 0) const = 0;
};

// RoutineT is a type-safe wrapper around a Routine and its callable entry, returned by FunctionT
template<typename FunctionType>
class RoutineT;

template<typename Return, typename... Arguments>
class RoutineT<Return(Arguments...)>
{
public:
	RoutineT() = default;

	explicit RoutineT(const std::shared_ptr<Routine> &routine)
	    : routine(routine)
	{
		if(routine)
		{
			callable = reinterpret_cast<CallableType>(const_cast<void *>(routine->getEntry(0)));
		}
	}

	operator bool() const
	{
		return callable != nullptr;
	}

	template<typename... Args>
	Return operator()(Args &&... args) const
	{
		return callable(std::forward<Args>(args)...);
	}

	const void *getEntry() const
	{
		return reinterpret_cast<void *>(callable);
	}

private:
	std::shared_ptr<Routine> routine;
	using CallableType = Return (*)(Arguments...);
	CallableType callable = nullptr;
};

}  // namespace rr

#endif  // rr_Routine_hpp
