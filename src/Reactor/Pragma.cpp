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

// The CLANG_NO_SANITIZE_MEMORY macro suppresses MemorySanitizer checks for
// use-of-uninitialized-data. It is used to decorate functions with known
// false positives.
#ifdef __clang__
#	define CLANG_NO_SANITIZE_MEMORY __attribute__((no_sanitize_memory))
#else
#	define CLANG_NO_SANITIZE_MEMORY
#endif

namespace {

struct PragmaState
{
	bool memorySanitizerInstrumentation;
};

// The initialization of static thread-local data is not observed by MemorySanitizer
// when inside a shared library, leading to false-positive use-of-uninitialized-data
// errors: https://github.com/google/sanitizers/issues/1409
// We work around this by assigning an initial value to it ourselves on first use.
// Note that since the flag to check whether this initialization has already been
// done is itself a static thread-local, we must suppress the MemorySanitizer check
// with a function attribute.
CLANG_NO_SANITIZE_MEMORY PragmaState &getPragmaState()
{
	static thread_local bool initialized = false;
	static thread_local PragmaState state;

	if(!initialized)
	{
		state = {};

		initialized = true;
	}

	return state;
}

}  // namespace

namespace rr {

void Pragma(PragmaBooleanOption option, bool enable)
{
	PragmaState &state = ::getPragmaState();

	switch(option)
	{
	case MemorySanitizerInstrumentation:
		state.memorySanitizerInstrumentation = enable;
		break;
	default:
		UNSUPPORTED("Unknown pragma %d", int(option));
	}
}

bool getPragmaState(PragmaBooleanOption option)
{
	PragmaState &state = ::getPragmaState();

	switch(option)
	{
	case MemorySanitizerInstrumentation:
		return state.memorySanitizerInstrumentation;
	default:
		UNSUPPORTED("Unknown pragma %d", int(option));
		return false;
	}
}

}  // namespace rr