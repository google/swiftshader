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

#ifndef sw_SpirvShader_hpp
#define sw_SpirvShader_hpp

#include "System/Types.hpp"
#include "Vulkan/VkDebug.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <spirv/unified1/spirv.hpp>

namespace sw
{
	class SpirvShader
	{
	public:
		using InsnStore = std::vector<uint32_t>;
		InsnStore insns;

		/* Pseudo-iterator over SPIRV instructions, designed to support range-based-for. */
		class InsnIterator
		{
			InsnStore::const_iterator iter;

		public:
			spv::Op opcode() const
			{
				return static_cast<spv::Op>(*iter & spv::OpCodeMask);
			}

			uint32_t wordCount() const
			{ return *iter >> spv::WordCountShift; }

			uint32_t word(uint32_t n) const
			{
				ASSERT(n < wordCount());
				return iter[n];
			}

			bool operator!=(InsnIterator const &other) const
			{
				return iter != other.iter;
			}

			InsnIterator operator*() const
			{ return *this; }

			InsnIterator &operator++()
			{
				iter += wordCount();
				return *this;
			}

			InsnIterator const operator++(int)
			{
				InsnIterator ret{*this};
				iter += wordCount();
				return ret;
			}

			InsnIterator(InsnIterator const &other) = default;

			InsnIterator() = default;

			explicit InsnIterator(InsnStore::const_iterator iter) : iter{iter}
			{}
		};

		/* range-based-for interface */
		InsnIterator begin() const
		{ return InsnIterator{insns.cbegin() + 5}; }

		InsnIterator end() const
		{ return InsnIterator{insns.cend()}; }

		class Object
		{
		public:
			InsnIterator definition;
			spv::StorageClass storageClass;

			enum class Kind
			{
				Unknown,        /* for paranoia -- if we get left with an object in this state, the module was broken */
				Type,
				Variable,
				Value,
			} kind = Kind::Unknown;
		};

		int getSerialID() const
		{ return serialID; }

		explicit SpirvShader(InsnStore const &insns);

		struct Modes
		{
			bool EarlyFragmentTests : 1;
			bool DepthReplacing : 1;
			bool DepthGreater : 1;
			bool DepthLess : 1;
			bool DepthUnchanged : 1;

			// Compute workgroup dimensions
			int LocalSizeX, LocalSizeY, LocalSizeZ;
		};

		Modes const &getModes() const
		{ return modes; }

	private:
		const int serialID;
		static volatile int serialCounter;
		Modes modes;
		std::unordered_map<uint32_t, Object> defs;

		void ProcessExecutionMode(InsnIterator it);
	};
}

#endif  // sw_SpirvShader_hpp