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
#include "ShaderCore.hpp"
#include "SpirvID.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <type_traits>
#include <memory>
#include <spirv/unified1/spirv.hpp>
#include <Device/Config.hpp>

namespace sw
{
	// Incrementally constructed complex bundle of rvalues
	// Effectively a restricted vector, supporting only:
	// - allocation to a (runtime-known) fixed size
	// - in-place construction of elements
	// - const operator[]
	class Intermediate
	{
	public:
		using Scalar = RValue<Float4>;

		Intermediate(uint32_t size) : contents(new ContentsType[size]), size(size) {}

		~Intermediate()
		{
			for (auto i = 0u; i < size; i++)
				reinterpret_cast<Scalar *>(&contents[i])->~Scalar();
			delete [] contents;
		}

		void emplace(uint32_t n, Scalar&& value)
		{
			assert(n < size);
			new (&contents[n]) Scalar(value);
		}

		Scalar const & operator[](uint32_t n) const
		{
			assert(n < size);
			return *reinterpret_cast<Scalar const *>(&contents[n]);
		}

		// No copy/move construction or assignment
		Intermediate(Intermediate const &) = delete;
		Intermediate(Intermediate &&) = delete;
		Intermediate & operator=(Intermediate const &) = delete;
		Intermediate & operator=(Intermediate &&) = delete;

	private:
		using ContentsType = std::aligned_storage<sizeof(Scalar), alignof(Scalar)>::type;

		ContentsType *contents;
		uint32_t size;
	};

	class SpirvRoutine;

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
			{
				return *iter >> spv::WordCountShift;
			}

			uint32_t word(uint32_t n) const
			{
				ASSERT(n < wordCount());
				return iter[n];
			}

			uint32_t const * wordPointer(uint32_t n) const
			{
				ASSERT(n < wordCount());
				return &iter[n];
			}

			bool operator!=(InsnIterator const &other) const
			{
				return iter != other.iter;
			}

			InsnIterator operator*() const
			{
				return *this;
			}

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
			{
			}
		};

		/* range-based-for interface */
		InsnIterator begin() const
		{
			return InsnIterator{insns.cbegin() + 5};
		}

		InsnIterator end() const
		{
			return InsnIterator{insns.cend()};
		}

		class Type;
		using TypeID = SpirvID<Type>;

		class Type
		{
		public:
			InsnIterator definition;
			spv::StorageClass storageClass = static_cast<spv::StorageClass>(-1);
			uint32_t sizeInComponents = 0;
			bool isBuiltInBlock = false;

			// Inner element type for pointers, arrays, vectors and matrices.
			TypeID element;
		};

		class Object;
		using ObjectID = SpirvID<Object>;

		class Object
		{
		public:
			InsnIterator definition;
			TypeID type;
			ObjectID pointerBase;
			std::unique_ptr<uint32_t[]> constantValue = nullptr;

			enum class Kind
			{
				Unknown,        /* for paranoia -- if we get left with an object in this state, the module was broken */
				Variable,
				InterfaceVariable,
				Constant,
				Value,
			} kind = Kind::Unknown;
		};

		struct TypeOrObject {}; // Dummy struct to represent a Type or Object.

		// TypeOrObjectID is an identifier that represents a Type or an Object,
		// and supports implicit casting to and from TypeID or ObjectID.
		class TypeOrObjectID : public SpirvID<TypeOrObject>
		{
		public:
			using Hash = std::hash<SpirvID<TypeOrObject>>;

			inline TypeOrObjectID(uint32_t id) : SpirvID(id) {}
			inline TypeOrObjectID(TypeID id) : SpirvID(id.value()) {}
			inline TypeOrObjectID(ObjectID id) : SpirvID(id.value()) {}
			inline operator TypeID() const { return TypeID(value()); }
			inline operator ObjectID() const { return ObjectID(value()); }
		};

		int getSerialID() const
		{
			return serialID;
		}

		explicit SpirvShader(InsnStore const &insns);

		struct Modes
		{
			bool EarlyFragmentTests : 1;
			bool DepthReplacing : 1;
			bool DepthGreater : 1;
			bool DepthLess : 1;
			bool DepthUnchanged : 1;
			bool ContainsKill : 1;
			bool NeedsCentroid : 1;

			// Compute workgroup dimensions
			int LocalSizeX, LocalSizeY, LocalSizeZ;
		};

		Modes const &getModes() const
		{
			return modes;
		}

		enum AttribType : unsigned char
		{
			ATTRIBTYPE_FLOAT,
			ATTRIBTYPE_INT,
			ATTRIBTYPE_UINT,
			ATTRIBTYPE_UNUSED,

			ATTRIBTYPE_LAST = ATTRIBTYPE_UINT
		};

		bool hasBuiltinInput(spv::BuiltIn b) const
		{
			return inputBuiltins.find(b) != inputBuiltins.end();
		}

		struct Decorations
		{
			int32_t Location;
			int32_t Component;
			spv::BuiltIn BuiltIn;
			bool HasLocation : 1;
			bool HasComponent : 1;
			bool HasBuiltIn : 1;
			bool Flat : 1;
			bool Centroid : 1;
			bool NoPerspective : 1;
			bool Block : 1;
			bool BufferBlock : 1;

			Decorations()
					: Location{-1}, Component{0}, BuiltIn{}, HasLocation{false}, HasComponent{false}, HasBuiltIn{false},
					  Flat{false},
					  Centroid{false}, NoPerspective{false}, Block{false},
					  BufferBlock{false}
			{
			}

			Decorations(Decorations const &) = default;

			void Apply(Decorations const &src);

			void Apply(spv::Decoration decoration, uint32_t arg);
		};

		std::unordered_map<TypeOrObjectID, Decorations, TypeOrObjectID::Hash> decorations;
		std::unordered_map<TypeID, std::vector<Decorations>> memberDecorations;

		struct InterfaceComponent
		{
			AttribType Type;
			bool Flat : 1;
			bool Centroid : 1;
			bool NoPerspective : 1;

			InterfaceComponent()
					: Type{ATTRIBTYPE_UNUSED}, Flat{false}, Centroid{false}, NoPerspective{false}
			{
			}
		};

		struct BuiltinMapping
		{
			ObjectID Id;
			uint32_t FirstComponent;
			uint32_t SizeInComponents;
		};

		std::vector<InterfaceComponent> inputs;
		std::vector<InterfaceComponent> outputs;

		void emitProlog(SpirvRoutine *routine) const;
		void emit(SpirvRoutine *routine) const;
		void emitEpilog(SpirvRoutine *routine) const;

		using BuiltInHash = std::hash<std::underlying_type<spv::BuiltIn>::type>;
		std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> inputBuiltins;
		std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> outputBuiltins;

		Type const &getType(TypeID id) const
		{
			auto it = types.find(id);
			assert(it != types.end());
			return it->second;
		}

		Object const &getObject(ObjectID id) const
		{
			auto it = defs.find(id);
			assert(it != defs.end());
			return it->second;
		}

	private:
		const int serialID;
		static volatile int serialCounter;
		Modes modes;
		HandleMap<Type> types;
		HandleMap<Object> defs;

		void ProcessExecutionMode(InsnIterator it);

		uint32_t ComputeTypeSize(InsnIterator insn);
		void ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const;
		void ApplyDecorationsForIdMember(Decorations *d, TypeID id, uint32_t member) const;

		template<typename F>
		int VisitInterfaceInner(TypeID id, Decorations d, F f) const;

		template<typename F>
		void VisitInterface(ObjectID id, F f) const;

		uint32_t GetConstantInt(ObjectID id) const;
		Object& CreateConstant(InsnIterator it);

		void ProcessInterfaceVariable(Object &object);

		Int4 WalkAccessChain(ObjectID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
	};

	class SpirvRoutine
	{
	public:
		using Value = Array<Float4>;
		std::unordered_map<SpirvShader::ObjectID, Value> lvalues;

		std::unordered_map<SpirvShader::ObjectID, Intermediate> intermediates;

		Value inputs = Value{MAX_INTERFACE_COMPONENTS};
		Value outputs = Value{MAX_INTERFACE_COMPONENTS};

		void createLvalue(SpirvShader::ObjectID id, uint32_t size)
		{
			lvalues.emplace(id, Value(size));
		}

		void createIntermediate(SpirvShader::ObjectID id, uint32_t size)
		{
			intermediates.emplace(std::piecewise_construct,
					std::forward_as_tuple(id),
					std::forward_as_tuple(size));
		}

		Value& getValue(SpirvShader::ObjectID id)
		{
			auto it = lvalues.find(id);
			assert(it != lvalues.end());
			return it->second;
		}

		Intermediate& getIntermediate(SpirvShader::ObjectID id)
		{
			auto it = intermediates.find(id);
			assert(it != intermediates.end());
			return it->second;
		}
	};

}

#endif  // sw_SpirvShader_hpp
