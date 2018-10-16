// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

#ifndef VK_EVENT_HPP_
#define VK_EVENT_HPP_

#include "VkObject.hpp"

namespace vk
{

class Event : public Object<Event, VkEvent>
{
public:
	Event(const VkEventCreateInfo* pCreateInfo, void* mem)
	{
	}

	~Event() = delete;

	static size_t ComputeRequiredAllocationSize(const VkEventCreateInfo* pCreateInfo)
	{
		return 0;
	}

	void signal()
	{
		status = VK_EVENT_SET;
	}

	void reset()
	{
		status = VK_EVENT_RESET;
	}

	VkResult getStatus() const
	{
		return status;
	}

private:
	VkResult status = VK_EVENT_RESET;
};

static inline Event* Cast(VkEvent object)
{
	return reinterpret_cast<Event*>(object);
}

} // namespace vk

#endif // VK_EVENT_HPP_
