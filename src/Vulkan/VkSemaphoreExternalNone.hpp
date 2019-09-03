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

#ifndef VK_SEMAPHORE_EXTERNAL_NONE_H_
#define VK_SEMAPHORE_EXTERNAL_NONE_H_

namespace vk
{

// Empty external sempahore implementation.
class Semaphore::External {
public:
	// The type of external semaphore handle types supported by this implementation.
	static const VkExternalSemaphoreHandleTypeFlags kExternalSemaphoreHandleType = 0;

	void init() {}

	void wait() {}

	bool tryWait() { return true; }

	void signal() {}

private:
	int dummy;
};

}  // namespace vk

#endif  // VK_SEMAPHORE_EXTERNAL_NONE_H_
