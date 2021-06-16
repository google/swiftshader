// Copyright 2021 The SwiftShader Authors. All Rights Reserved.
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

#include "Pragma.hpp"
#include "PragmaInternals.hpp"

#include "Debug.hpp"

namespace rr {

static thread_local bool memorySanitizerInstrumentation = false;

void Pragma(PragmaBooleanOption option, bool enable)
{
	switch(option)
	{
	case MemorySanitizerInstrumentation:
		memorySanitizerInstrumentation = enable;
		break;
	default:
		UNSUPPORTED("Unknown pragma %d", int(option));
	}
}

bool getPragmaState(PragmaBooleanOption option)
{
	switch(option)
	{
	case MemorySanitizerInstrumentation:
		return memorySanitizerInstrumentation;
	default:
		UNSUPPORTED("Unknown pragma %d", int(option));
		return false;
	}
}

}  // namespace rr