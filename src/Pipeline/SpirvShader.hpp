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
#include "Device/Config.hpp"

#include <spirv/unified1/spirv.hpp>

#include <array>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <type_traits>
#include <memory>

namespace vk
{
	class PipelineLayout;
} // namespace vk

namespace sw
{
	// Forward declarations.
	class SpirvRoutine;
	class GenericValue;

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
		Intermediate(uint32_t size) : scalar(new rr::Value*[size]), size(size) {
			memset(scalar, 0, sizeof(rr::Value*) * size);
		}

		~Intermediate()
		{
			delete[] scalar;
		}

		void move(uint32_t i, RValue<SIMD::Float> &&scalar) { emplace(i, scalar.value); }
		void move(uint32_t i, RValue<SIMD::Int> &&scalar)   { emplace(i, scalar.value); }
		void move(uint32_t i, RValue<SIMD::UInt> &&scalar)  { emplace(i, scalar.value); }

		void move(uint32_t i, const RValue<SIMD::Float> &scalar) { emplace(i, scalar.value); }
		void move(uint32_t i, const RValue<SIMD::Int> &scalar)   { emplace(i, scalar.value); }
		void move(uint32_t i, const RValue<SIMD::UInt> &scalar)  { emplace(i, scalar.value); }

		// Value retrieval functions.
		RValue<SIMD::Float> Float(uint32_t i) const
		{
			ASSERT(i < size);
			ASSERT(scalar[i] != nullptr);
			return As<SIMD::Float>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::Float>(scalar)
		}

		RValue<SIMD::Int> Int(uint32_t i) const
		{
			ASSERT(i < size);
			ASSERT(scalar[i] != nullptr);
			return As<SIMD::Int>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::Int>(scalar)
		}

		RValue<SIMD::UInt> UInt(uint32_t i) const
		{
			ASSERT(i < size);
			ASSERT(scalar[i] != nullptr);
			return As<SIMD::UInt>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::UInt>(scalar)
		}

		// No copy/move construction or assignment
		Intermediate(Intermediate const &) = delete;
		Intermediate(Intermediate &&) = delete;
		Intermediate & operator=(Intermediate const &) = delete;
		Intermediate & operator=(Intermediate &&) = delete;

	private:
		void emplace(uint32_t i, rr::Value *value)
		{
			ASSERT(i < size);
			ASSERT(scalar[i] == nullptr);
			scalar[i] = value;
		}

		rr::Value **const scalar;
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

		class Type
		{
		public:
			using ID = SpirvID<Type>;

			spv::Op opcode() const { return definition.opcode(); }

			InsnIterator definition;
			spv::StorageClass storageClass = static_cast<spv::StorageClass>(-1);
			uint32_t sizeInComponents = 0;
			bool isBuiltInBlock = false;

			// Inner element type for pointers, arrays, vectors and matrices.
			ID element;
		};

		class Object
		{
		public:
			using ID = SpirvID<Object>;

			spv::Op opcode() const { return definition.opcode(); }

			InsnIterator definition;
			Type::ID type;
			ID pointerBase;
			std::unique_ptr<uint32_t[]> constantValue = nullptr;

			enum class Kind
			{
				Unknown,        /* for paranoia -- if we get left with an object in this state, the module was broken */
				Variable,          // TODO: Document
				InterfaceVariable, // TODO: Document
				Constant,          // Values held by Object::constantValue
				Value,             // Values held by SpirvRoutine::intermediates
				PhysicalPointer,   // Pointer held by SpirvRoutine::physicalPointers
			} kind = Kind::Unknown;
		};

		// Block is an interval of SPIR-V instructions, starting with the
		// opening OpLabel, and ending with a termination instruction.
		class Block
		{
		public:
			using ID = SpirvID<Block>;
			using Set = std::unordered_set<ID>;

			// Edge represents the graph edge between two blocks.
			struct Edge
			{
				ID from;
				ID to;

				bool operator == (const Edge& other) const { return from == other.from && to == other.to; }

				struct Hash
				{
					std::size_t operator()(const Edge& edge) const noexcept
					{
						return std::hash<uint32_t>()(edge.from.value() * 31 + edge.to.value());
					}
				};
			};

			Block() = default;
			Block(const Block& other) = default;
			explicit Block(InsnIterator begin, InsnIterator end);

			/* range-based-for interface */
			inline InsnIterator begin() const { return begin_; }
			inline InsnIterator end() const { return end_; }

			enum Kind
			{
				Simple, // OpBranch or other simple terminator.
				StructuredBranchConditional, // OpSelectionMerge + OpBranchConditional
				UnstructuredBranchConditional, // OpBranchConditional
				StructuredSwitch, // OpSelectionMerge + OpSwitch
				UnstructuredSwitch, // OpSwitch
				Loop, // OpLoopMerge + [OpBranchConditional | OpBranch]
			};

			Kind kind;
			InsnIterator mergeInstruction; // Merge instruction.
			InsnIterator branchInstruction; //
			ID mergeBlock; // Structured flow merge block.
			ID continueTarget; // Loop continue block.
			Set ins; // Blocks that branch into this block.
			Set outs; // Blocks that this block branches to.

		private:
			InsnIterator begin_;
			InsnIterator end_;
		};

		struct TypeOrObject {}; // Dummy struct to represent a Type or Object.

		// TypeOrObjectID is an identifier that represents a Type or an Object,
		// and supports implicit casting to and from Type::ID or Object::ID.
		class TypeOrObjectID : public SpirvID<TypeOrObject>
		{
		public:
			using Hash = std::hash<SpirvID<TypeOrObject>>;

			inline TypeOrObjectID(uint32_t id) : SpirvID(id) {}
			inline TypeOrObjectID(Type::ID id) : SpirvID(id.value()) {}
			inline TypeOrObjectID(Object::ID id) : SpirvID(id.value()) {}
			inline operator Type::ID() const { return Type::ID(value()); }
			inline operator Object::ID() const { return Object::ID(value()); }
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
			int WorkgroupSizeX = 1, WorkgroupSizeY = 1, WorkgroupSizeZ = 1;
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
			int32_t Offset;
			int32_t ArrayStride;
			int32_t MatrixStride;
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
			bool HasOffset : 1;
			bool HasArrayStride : 1;
			bool HasMatrixStride : 1;

			Decorations()
					: Location{-1}, Component{0}, DescriptorSet{-1}, Binding{-1},
					  BuiltIn{static_cast<spv::BuiltIn>(-1)},
					  Offset{-1}, ArrayStride{-1}, MatrixStride{-1},
					  HasLocation{false}, HasComponent{false},
					  HasDescriptorSet{false}, HasBinding{false},
					  HasBuiltIn{false}, Flat{false}, Centroid{false},
					  NoPerspective{false}, Block{false}, BufferBlock{false},
					  HasOffset{false}, HasArrayStride{false}, HasMatrixStride{false}
			{
			}

			Decorations(Decorations const &) = default;

			void Apply(Decorations const &src);

			void Apply(spv::Decoration decoration, uint32_t arg);
		};

		std::unordered_map<TypeOrObjectID, Decorations, TypeOrObjectID::Hash> decorations;
		std::unordered_map<Type::ID, std::vector<Decorations>> memberDecorations;

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
			Object::ID Id;
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

		Type const &getType(Type::ID id) const
		{
			auto it = types.find(id);
			ASSERT_MSG(it != types.end(), "Unknown type %d", id.value());
			return it->second;
		}

		Object const &getObject(Object::ID id) const
		{
			auto it = defs.find(id);
			ASSERT_MSG(it != defs.end(), "Unknown object %d", id.value());
			return it->second;
		}

		Block const &getBlock(Block::ID id) const
		{
			auto it = blocks.find(id);
			ASSERT_MSG(it != blocks.end(), "Unknown block %d", id.value());
			return it->second;
		}

	private:
		const int serialID;
		static volatile int serialCounter;
		Modes modes;
		HandleMap<Type> types;
		HandleMap<Object> defs;
		HandleMap<Block> blocks;
		Block::ID mainBlockId; // Block of the entry point function.

		void EmitBlock(SpirvRoutine *routine, Block const &block) const;
		void EmitInstruction(SpirvRoutine *routine, InsnIterator insn) const;

		// DeclareType creates a Type for the given OpTypeX instruction, storing
		// it into the types map. It is called from the analysis pass (constructor).
		void DeclareType(InsnIterator insn);

		void ProcessExecutionMode(InsnIterator it);

		uint32_t ComputeTypeSize(InsnIterator insn);
		void ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const;
		void ApplyDecorationsForIdMember(Decorations *d, Type::ID id, uint32_t member) const;

		// Returns true if data in the given storage class is word-interleaved
		// by each SIMD vector lane, otherwise data is linerally stored.
		//
		// A 'lane' is a component of a SIMD vector register.
		// Given 4 consecutive loads/stores of 4 SIMD vector registers:
		//
		// "StorageInterleavedByLane":
		//
		//  Ptr+0:Reg0.x | Ptr+1:Reg0.y | Ptr+2:Reg0.z | Ptr+3:Reg0.w
		// --------------+--------------+--------------+--------------
		//  Ptr+4:Reg1.x | Ptr+5:Reg1.y | Ptr+6:Reg1.z | Ptr+7:Reg1.w
		// --------------+--------------+--------------+--------------
		//  Ptr+8:Reg2.x | Ptr+9:Reg2.y | Ptr+a:Reg2.z | Ptr+b:Reg2.w
		// --------------+--------------+--------------+--------------
		//  Ptr+c:Reg3.x | Ptr+d:Reg3.y | Ptr+e:Reg3.z | Ptr+f:Reg3.w
		//
		// Not "StorageInterleavedByLane":
		//
		//  Ptr+0:Reg0.x | Ptr+0:Reg0.y | Ptr+0:Reg0.z | Ptr+0:Reg0.w
		// --------------+--------------+--------------+--------------
		//  Ptr+1:Reg1.x | Ptr+1:Reg1.y | Ptr+1:Reg1.z | Ptr+1:Reg1.w
		// --------------+--------------+--------------+--------------
		//  Ptr+2:Reg2.x | Ptr+2:Reg2.y | Ptr+2:Reg2.z | Ptr+2:Reg2.w
		// --------------+--------------+--------------+--------------
		//  Ptr+3:Reg3.x | Ptr+3:Reg3.y | Ptr+3:Reg3.z | Ptr+3:Reg3.w
		//
		static bool IsStorageInterleavedByLane(spv::StorageClass storageClass);

		template<typename F>
		int VisitInterfaceInner(Type::ID id, Decorations d, F f) const;

		template<typename F>
		void VisitInterface(Object::ID id, F f) const;

		uint32_t GetConstantInt(Object::ID id) const;
		Object& CreateConstant(InsnIterator it);

		void ProcessInterfaceVariable(Object &object);

		SIMD::Int WalkExplicitLayoutAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
		SIMD::Int WalkAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
		uint32_t WalkLiteralAccessChain(Type::ID id, uint32_t numIndexes, uint32_t const *indexes) const;

		// Emit pass instructions:
		void EmitVariable(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitLoad(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitStore(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitAccessChain(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeConstruct(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeInsert(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitCompositeExtract(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitVectorShuffle(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitVectorTimesScalar(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitVectorExtractDynamic(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitVectorInsertDynamic(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitUnaryOp(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitBinaryOp(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitDot(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitSelect(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitExtendedInstruction(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitAny(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitAll(InsnIterator insn, SpirvRoutine *routine) const;
		void EmitBranch(InsnIterator insn, SpirvRoutine *routine) const;

		// OpcodeName() returns the name of the opcode op.
		// If NDEBUG is defined, then OpcodeName() will only return the numerical code.
		static std::string OpcodeName(spv::Op op);
		static std::memory_order MemoryOrder(spv::MemorySemanticsMask memorySemantics);

		// Helper as we often need to take dot products as part of doing other things.
		SIMD::Float Dot(unsigned numComponents, GenericValue const & x, GenericValue const & y) const;
	};

	class SpirvRoutine
	{
	public:
		SpirvRoutine(vk::PipelineLayout const *pipelineLayout);

		using Value = Array<SIMD::Float>;

		vk::PipelineLayout const * const pipelineLayout;

		std::unordered_map<SpirvShader::Object::ID, Value> lvalues;

		std::unordered_map<SpirvShader::Object::ID, Intermediate> intermediates;

		std::unordered_map<SpirvShader::Object::ID, Pointer<Byte> > physicalPointers;

		Value inputs = Value{MAX_INTERFACE_COMPONENTS};
		Value outputs = Value{MAX_INTERFACE_COMPONENTS};

		SIMD::Int activeLaneMask = SIMD::Int(0xFFFFFFFF);

		std::array<Pointer<Byte>, vk::MAX_BOUND_DESCRIPTOR_SETS> descriptorSets;
		Pointer<Byte> pushConstants;

		void createLvalue(SpirvShader::Object::ID id, uint32_t size)
		{
			lvalues.emplace(id, Value(size));
		}

		Intermediate& createIntermediate(SpirvShader::Object::ID id, uint32_t size)
		{
			auto it = intermediates.emplace(std::piecewise_construct,
					std::forward_as_tuple(id),
					std::forward_as_tuple(size));
			return it.first->second;
		}

		Value& getValue(SpirvShader::Object::ID id)
		{
			auto it = lvalues.find(id);
			ASSERT_MSG(it != lvalues.end(), "Unknown value %d", id.value());
			return it->second;
		}

		Intermediate const& getIntermediate(SpirvShader::Object::ID id) const
		{
			auto it = intermediates.find(id);
			ASSERT_MSG(it != intermediates.end(), "Unknown intermediate %d", id.value());
			return it->second;
		}

		Pointer<Byte>& getPhysicalPointer(SpirvShader::Object::ID id)
		{
			auto it = physicalPointers.find(id);
			ASSERT_MSG(it != physicalPointers.end(), "Unknown physical pointer %d", id.value());
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
		GenericValue(SpirvShader const *shader, SpirvRoutine const *routine, SpirvShader::Object::ID objId) :
				obj(shader->getObject(objId)),
				intermediate(obj.kind == SpirvShader::Object::Kind::Value ? &routine->getIntermediate(objId) : nullptr) {}

		RValue<SIMD::Float> Float(uint32_t i) const
		{
			if (intermediate != nullptr)
			{
				return intermediate->Float(i);
			}
			auto constantValue = reinterpret_cast<float *>(obj.constantValue.get());
			return RValue<SIMD::Float>(constantValue[i]);
		}

		RValue<SIMD::Int> Int(uint32_t i) const
		{
			return As<SIMD::Int>(Float(i));
		}

		RValue<SIMD::UInt> UInt(uint32_t i) const
		{
			return As<SIMD::UInt>(Float(i));
		}
	};

}

#endif  // sw_SpirvShader_hpp
