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

#ifndef sw_Thread_hpp
#define sw_Thread_hpp

#include <atomic>

namespace sw
{
	class AtomicInt
	{
	public:
		AtomicInt() : ai() {}
		AtomicInt(int i) : ai(i) {}

		inline operator int() const { return ai.load(std::memory_order_acquire); }
		inline void operator=(const AtomicInt& i) { ai.store(i.ai.load(std::memory_order_acquire), std::memory_order_release); }
		inline void operator=(int i) { ai.store(i, std::memory_order_release); }
		inline void operator--() { ai.fetch_sub(1, std::memory_order_acq_rel); }
		inline void operator++() { ai.fetch_add(1, std::memory_order_acq_rel); }
		inline int operator--(int) { return ai.fetch_sub(1, std::memory_order_acq_rel) - 1; }
		inline int operator++(int) { return ai.fetch_add(1, std::memory_order_acq_rel) + 1; }
		inline void operator-=(int i) { ai.fetch_sub(i, std::memory_order_acq_rel); }
		inline void operator+=(int i) { ai.fetch_add(i, std::memory_order_acq_rel); }
	private:
		std::atomic<int> ai;
	};
}

#endif   // sw_Thread_hpp
