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

#include "ShaderCore.hpp"
#include "SpirvID.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkConfig.h"

#include <array>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <type_traits>
#include <memory>
#include <spirv/unified1/spirv.hpp>
#include <Device/Config.hpp>

namespace vk
{
	class PipelineLayout;
} // namespace vk

namespace sw
{
	// Forward declarations.
	class SpirvRoutine;

	// SIMD contains types that represent multiple scalars packed into a single
	// vector data type. Types in the SIMD namespace provide a semantic hint
	// that the data should be treated as a per-execution-lane scalar instead of
	// a typical euclidean-style vector type.
	namespace SIMD
	{
		// Width is the number of per-lane scalars packed into each SIMD vector.
		static constexpr int Width = 4;

		using Float = rr::Float4;
		using Int = rr::Int4;
		using UInt = rr::UInt4;
	}

	// Incrementally constructed complex bundle of rvalues
	// Effectively a restricted vector, supporting only:
	// - allocation to a (runtime-known) fixed size
	// - in-place construction of elements
	// - const operator[]
	class Intermediate
	{
	public:
		using Scalar = RValue<SIMD::Float>;

		Intermediate(uint32_t size) : contents(new ContentsType[size]), size(size) {
#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
			memset(contents, 0, sizeof(ContentsType) * size);
#endif
		}

		~Intermediate()
		{
			for (auto i = 0u; i < size; i++)
				reinterpret_cast<Scalar *>(&contents[i])->~Scalar();
			delete [] contents;
		}

		void emplace(uint32_t n, Scalar&& value)
		{
			ASSERT(n < size);
			ASSERT(reinterpret_cast<Scalar const *>(&contents[n])->value == nullptr);
			new (&contents[n]) Scalar(value);
		}

		void emplace(uint32_t n, const Scalar& value)
		{
			ASSERT(n < size);
			ASSERT(reinterpret_cast<Scalar const *>(&contents[n])->value == nullptr);
			new (&contents[n]) Scalar(value);
		}

		Scalar const & operator[](uint32_t n) const
		{
			ASSERT(n < size);
			auto scalar = reinterpret_cast<Scalar const *>(&contents[n]);
			ASSERT(scalar->value != nullptr);
			return *scalar;
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
			int32_t DescriptorSet;
			int32_t Binding;
			spv::BuiltIn BuiltIn;
			bool HasLocation : 1;
			bool HasComponent : 1;
			bool HasDescriptorSet : 1;
			bool HasBinding : 1;
			bool HasBuiltIn : 1;
			bool Flat : 1;
			bool Centroid : 1;
			bool NoPerspective : 1;
			bool Block : 1;
			bool BufferBlock : 1;

			Decorations()
					: Location{-1}, Component{0}, DescriptorSet{-1}, Binding{-1},
					  BuiltIn{static_cast<spv::BuiltIn>(-1)},
					  HasLocation{false}, HasComponent{false},
					  HasDescriptorSet{false}, HasBinding{false},
					  HasBuiltIn{false}, Flat{false}, Centroid{false},
					  NoPerspective{false}, Block{false}, BufferBlock{false}
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
			ASSERT(it != types.end());
			return it->second;
		}

		Object const &getObject(ObjectID id) const
		{
			auto it = defs.find(id);
			ASSERT(it != defs.end());
			return it->second;
		}

	private:
		const int serialID;
		static volatile int serialCounter;
		Modes modes;
		HandleMap<Type> types;
		HandleMap<Object> defs;

		// DeclareType creates a Type for the given OpTypeX instruction, storing
		// it into the types map. It is called from the analysis pass (constructor).
		void DeclareType(InsnIterator insn);

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

		SIMD::Int WalkAccessChain(ObjectID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
		uint32_t WalkLiteralAccessChain(TypeID id, uint32_t numIndexes, uint32_t const *indexes) const;

		// Emit pass instructions:
		void EmitVariable(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitLoad(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitStore(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitAccessChain(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeConstruct(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeInsert(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeExtract(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitVectorShuffle(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitUnaryOp(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitBinaryOp(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitDot(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitSelect(InsnIterator insn, SpirvRoutine *routine) const;

		// OpcodeName returns the name of the opcode op.
		// If NDEBUG is defined, then OpcodeName will only return the numerical code.
		static std::string OpcodeName(spv::Op op);
	};

	class SpirvRoutine
	{
	public:
		SpirvRoutine(vk::PipelineLayout const *pipelineLayout);

		using Value = Array<SIMD::Float>;

		vk::PipelineLayout const * const pipelineLayout;

		std::unordered_map<SpirvShader::ObjectID, Value> lvalues;

		std::unordered_map<SpirvShader::ObjectID, Intermediate> intermediates;

		Value inputs = Value{MAX_INTERFACE_COMPONENTS};
		Value outputs = Value{MAX_INTERFACE_COMPONENTS};

		std::array<Pointer<Byte>, vk::MAX_BOUND_DESCRIPTOR_SETS> descriptorSets;

		void createLvalue(SpirvShader::ObjectID id, uint32_t size)
		{
			lvalues.emplace(id, Value(size));
		}

		Intermediate& createIntermediate(SpirvShader::ObjectID id, uint32_t size)
		{
			auto it = intermediates.emplace(std::piecewise_construct,
					std::forward_as_tuple(id),
					std::forward_as_tuple(size));
			return it.first->second;
		}

		Value& getValue(SpirvShader::ObjectID id)
		{
			auto it = lvalues.find(id);
			ASSERT(it != lvalues.end());
			return it->second;
		}

		Intermediate const& getIntermediate(SpirvShader::ObjectID id) const
		{
			auto it = intermediates.find(id);
			ASSERT(it != intermediates.end());
			return it->second;
		}
	};

	class GenericValue
	{
		// Generic wrapper over either per-lane intermediate value, or a constant.
		// Constants are transparently widened to per-lane values in operator[].
		// This is appropriate in most cases -- if we're not going to do something
		// significantly different based on whether the value is uniform across lanes.

		SpirvShader::Object const &obj;
		Intermediate const *intermediate;

	public:
		GenericValue(SpirvShader const *shader, SpirvRoutine const *routine, SpirvShader::ObjectID objId) :
				obj(shader->getObject(objId)),
				intermediate(obj.kind == SpirvShader::Object::Kind::Value ? &routine->getIntermediate(objId) : nullptr) {}

		RValue<SIMD::Float> operator[](uint32_t i) const
		{
			if (intermediate)
				return (*intermediate)[i];

			auto constantValue = reinterpret_cast<float *>(obj.constantValue.get());
			return RValue<SIMD::Float>(constantValue[i]);
		}
	};

}

#endif  // sw_SpirvShader_hpp
