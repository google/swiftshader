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

#ifndef rr_LLVMRoutine_hpp
#define rr_LLVMRoutine_hpp

#include "Routine.hpp"

#include <cstdint>
#include <stddef.h>
#include <vector>

namespace rr
{
	class LLVMReactorJIT;

	class LLVMRoutine : public Routine
	{
	public:
		LLVMRoutine(void **entries, size_t entry_count,
					void (*callback)(LLVMReactorJIT *, uint64_t),
		            LLVMReactorJIT *jit, uint64_t key)
			: entries(entries, entries+entry_count),
			  dtor(callback), reactorJIT(jit), moduleKey(key)
		{ }

		virtual ~LLVMRoutine();

		const void *getEntry(int index)
		{
			return entries[index];
		}

	private:
		const std::vector<const void *> entries;

		void (*dtor)(LLVMReactorJIT *, uint64_t);
		LLVMReactorJIT *reactorJIT;
		uint64_t moduleKey;
	};
}

#endif   // rr_LLVMRoutine_hpp
