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
#include <queue>

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

			bool operator==(InsnIterator const &other) const
			{
				return iter == other.iter;
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
			std::unique_ptr<uint32_t[]> constantValue = nullptr;

			enum class Kind
			{
				// Invalid default kind.
				// If we get left with an object in this state, the module was
				// broken.
				Unknown,

				// TODO: Better document this kind.
				// A shader interface variable pointer.
				// Pointer with uniform address across all lanes.
				// Pointer held by SpirvRoutine::pointers
				InterfaceVariable,

				// Constant value held by Object::constantValue.
				Constant,

				// Value held by SpirvRoutine::intermediates.
				Intermediate,

				// DivergentPointer formed from a base pointer and per-lane offset.
				// Base pointer held by SpirvRoutine::pointers
				// Per-lane offset held by SpirvRoutine::intermediates.
				DivergentPointer,

				// Pointer with uniform address across all lanes.
				// Pointer held by SpirvRoutine::pointers
				NonDivergentPointer,

				// A pointer to a vk::DescriptorSet*.
				// Pointer held by SpirvRoutine::pointers.
				DescriptorSet,

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
			InsnIterator mergeInstruction; // Structured control flow merge instruction.
			InsnIterator branchInstruction; // Branch instruction.
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
		void emit(SpirvRoutine *routine, RValue<SIMD::Int> const &activeLaneMask) const;
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

		// Walks all reachable the blocks starting from id adding them to
		// reachable.
		void TraverseReachableBlocks(Block::ID id, Block::Set& reachable);

		// Assigns Block::ins from Block::outs for every block.
		void AssignBlockIns();

		// DeclareType creates a Type for the given OpTypeX instruction, storing
		// it into the types map. It is called from the analysis pass (constructor).
		void DeclareType(InsnIterator insn);

		void ProcessExecutionMode(InsnIterator it);

		uint32_t ComputeTypeSize(InsnIterator insn);
		void ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const;
		void ApplyDecorationsForIdMember(Decorations *d, Type::ID id, uint32_t member) const;

		// Returns true if data in the given storage class is word-interleaved
		// by each SIMD vector lane, otherwise data is stored linerally.
		//
		// Each lane addresses a single word, picked by a base pointer and an
		// integer offset.
		//
		// A word is currently 32 bits (single float, int32_t, uint32_t).
		// A lane is a single element of a SIMD vector register.
		//
		// Storage interleaved by lane - (IsStorageInterleavedByLane() == true):
		// ---------------------------------------------------------------------
		//
		// Address = PtrBase + sizeof(Word) * (SIMD::Width * LaneOffset + LaneIndex)
		//
		// Assuming SIMD::Width == 4:
		//
		//                   Lane[0]  |  Lane[1]  |  Lane[2]  |  Lane[3]
		//                 ===========+===========+===========+==========
		//  LaneOffset=0: |  Word[0]  |  Word[1]  |  Word[2]  |  Word[3]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=1: |  Word[4]  |  Word[5]  |  Word[6]  |  Word[7]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=2: |  Word[8]  |  Word[9]  |  Word[a]  |  Word[b]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=3: |  Word[c]  |  Word[d]  |  Word[e]  |  Word[f]
		//
		//
		// Linear storage - (IsStorageInterleavedByLane() == false):
		// ---------------------------------------------------------
		//
		// Address = PtrBase + sizeof(Word) * LaneOffset
		//
		//                   Lane[0]  |  Lane[1]  |  Lane[2]  |  Lane[3]
		//                 ===========+===========+===========+==========
		//  LaneOffset=0: |  Word[0]  |  Word[0]  |  Word[0]  |  Word[0]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=1: |  Word[1]  |  Word[1]  |  Word[1]  |  Word[1]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=2: |  Word[2]  |  Word[2]  |  Word[2]  |  Word[2]
		// ---------------+-----------+-----------+-----------+----------
		//  LaneOffset=3: |  Word[3]  |  Word[3]  |  Word[3]  |  Word[3]
		//
		static bool IsStorageInterleavedByLane(spv::StorageClass storageClass);

		template<typename F>
		int VisitInterfaceInner(Type::ID id, Decorations d, F f) const;

		template<typename F>
		void VisitInterface(Object::ID id, F f) const;

		uint32_t GetConstantInt(Object::ID id) const;
		Object& CreateConstant(InsnIterator it);

		void ProcessInterfaceVariable(Object &object);

		// Returns a base pointer and per-lane offset to the underlying data for
		// the given pointer object. Handles objects of the following kinds:
		//  • DescriptorSet
		//  • DivergentPointer
		//  • InterfaceVariable
		//  • NonDivergentPointer
		// Calling GetPointerToData with objects of any other kind will assert.
		std::pair<Pointer<Byte>, SIMD::Int> GetPointerToData(Object::ID id, int arrayIndex, SpirvRoutine *routine) const;

		std::pair<Pointer<Byte>, SIMD::Int> WalkExplicitLayoutAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
		SIMD::Int WalkAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, SpirvRoutine *routine) const;
		uint32_t WalkLiteralAccessChain(Type::ID id, uint32_t numIndexes, uint32_t const *indexes) const;

		// EmitState holds control-flow state for the emit() pass.
		class EmitState
		{
		public:
			RValue<SIMD::Int> activeLaneMask() const
			{
				ASSERT(activeLaneMaskValue != nullptr);
				return RValue<SIMD::Int>(activeLaneMaskValue);
			}

			void setActiveLaneMask(RValue<SIMD::Int> mask)
			{
				activeLaneMaskValue = mask.value;
			}

			// Add a new active lane mask edge from the current block to out.
			// The edge mask value will be (mask AND activeLaneMaskValue).
			// If multiple active lane masks are added for the same edge, then
			// they will be ORed together.
			void addOutputActiveLaneMaskEdge(Block::ID out, RValue<SIMD::Int> mask);

			// Add a new active lane mask for the edge from -> to.
			// If multiple active lane masks are added for the same edge, then
			// they will be ORed together.
			void addActiveLaneMaskEdge(Block::ID from, Block::ID to, RValue<SIMD::Int> mask);

			SpirvRoutine *routine = nullptr; // The current routine being built.
			rr::Value *activeLaneMaskValue = nullptr; // The current active lane mask.
			Block::ID currentBlock; // The current block being built.
			Block::Set visited; // Blocks already built.
			std::unordered_map<Block::Edge, RValue<SIMD::Int>, Block::Edge::Hash> edgeActiveLaneMasks;
			std::queue<Block::ID> *pending;
		};

		// EmitResult is an enumerator of result values from the Emit functions.
		enum class EmitResult
		{
			Continue, // No termination instructions.
			Terminator, // Reached a termination instruction.
		};

		// existsPath returns true if there's a direct or indirect flow from
		// the 'from' block to the 'to' block that does not pass through
		// notPassingThrough.
		bool existsPath(Block::ID from, Block::ID to, Block::ID notPassingThrough) const;

		// Lookup the active lane mask for the edge from -> to.
		// If from is unreachable, then a mask of all zeros is returned.
		// Asserts if from is reachable and the edge does not exist.
		RValue<SIMD::Int> GetActiveLaneMaskEdge(EmitState *state, Block::ID from, Block::ID to) const;

		// Emit all the unvisited blocks (except for ignore) in BFS order,
		// starting with id.
		void EmitBlocks(Block::ID id, EmitState *state, Block::ID ignore = 0) const;
		void EmitNonLoop(EmitState *state) const;
		void EmitLoop(EmitState *state) const;

		void EmitInstructions(InsnIterator begin, InsnIterator end, EmitState *state) const;
		EmitResult EmitInstruction(InsnIterator insn, EmitState *state) const;

		// Emit pass instructions:
		EmitResult EmitVariable(InsnIterator insn, EmitState *state) const;
		EmitResult EmitLoad(InsnIterator insn, EmitState *state) const;
		EmitResult EmitStore(InsnIterator insn, EmitState *state) const;
		EmitResult EmitAccessChain(InsnIterator insn, EmitState *state) const;
		EmitResult EmitCompositeConstruct(InsnIterator insn, EmitState *state) const;
		EmitResult EmitCompositeInsert(InsnIterator insn, EmitState *state) const;
		EmitResult EmitCompositeExtract(InsnIterator insn, EmitState *state) const;
		EmitResult EmitVectorShuffle(InsnIterator insn, EmitState *state) const;
		EmitResult EmitVectorTimesScalar(InsnIterator insn, EmitState *state) const;
		EmitResult EmitMatrixTimesVector(InsnIterator insn, EmitState *state) const;
		EmitResult EmitVectorTimesMatrix(InsnIterator insn, EmitState *state) const;
		EmitResult EmitMatrixTimesMatrix(InsnIterator insn, EmitState *state) const;
		EmitResult EmitOuterProduct(InsnIterator insn, EmitState *state) const;
		EmitResult EmitTranspose(InsnIterator insn, EmitState *state) const;
		EmitResult EmitVectorExtractDynamic(InsnIterator insn, EmitState *state) const;
		EmitResult EmitVectorInsertDynamic(InsnIterator insn, EmitState *state) const;
		EmitResult EmitUnaryOp(InsnIterator insn, EmitState *state) const;
		EmitResult EmitBinaryOp(InsnIterator insn, EmitState *state) const;
		EmitResult EmitDot(InsnIterator insn, EmitState *state) const;
		EmitResult EmitSelect(InsnIterator insn, EmitState *state) const;
		EmitResult EmitExtendedInstruction(InsnIterator insn, EmitState *state) const;
		EmitResult EmitAny(InsnIterator insn, EmitState *state) const;
		EmitResult EmitAll(InsnIterator insn, EmitState *state) const;
		EmitResult EmitBranch(InsnIterator insn, EmitState *state) const;
		EmitResult EmitBranchConditional(InsnIterator insn, EmitState *state) const;
		EmitResult EmitSwitch(InsnIterator insn, EmitState *state) const;
		EmitResult EmitUnreachable(InsnIterator insn, EmitState *state) const;
		EmitResult EmitReturn(InsnIterator insn, EmitState *state) const;
		EmitResult EmitKill(InsnIterator insn, EmitState *state) const;
		EmitResult EmitPhi(InsnIterator insn, EmitState *state) const;

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

		using Variable = Array<SIMD::Float>;

		vk::PipelineLayout const * const pipelineLayout;

		std::unordered_map<SpirvShader::Object::ID, Variable> variables;

		std::unordered_map<SpirvShader::Object::ID, Intermediate> intermediates;

		std::unordered_map<SpirvShader::Object::ID, Pointer<Byte> > pointers;

		Variable inputs = Variable{MAX_INTERFACE_COMPONENTS};
		Variable outputs = Variable{MAX_INTERFACE_COMPONENTS};

		Pointer<Pointer<Byte>> descriptorSets;
		Pointer<Int> descriptorDynamicOffsets;
		Pointer<Byte> pushConstants;
		Int killMask = Int{0};

		void createVariable(SpirvShader::Object::ID id, uint32_t size)
		{
			bool added = variables.emplace(id, Variable(size)).second;
			ASSERT_MSG(added, "Variable %d created twice", id.value());
		}

		template <typename T>
		void createPointer(SpirvShader::Object::ID id, Pointer<T> ptrBase)
		{
			bool added = pointers.emplace(id, ptrBase).second;
			ASSERT_MSG(added, "Pointer %d created twice", id.value());
		}

		template <typename T>
		void createPointer(SpirvShader::Object::ID id, RValue<Pointer<T>> ptrBase)
		{
			createPointer(id, Pointer<T>(ptrBase));
		}

		template <typename T>
		void createPointer(SpirvShader::Object::ID id, Reference<Pointer<T>> ptrBase)
		{
			createPointer(id, Pointer<T>(ptrBase));
		}

		Intermediate& createIntermediate(SpirvShader::Object::ID id, uint32_t size)
		{
			auto it = intermediates.emplace(std::piecewise_construct,
					std::forward_as_tuple(id),
					std::forward_as_tuple(size));
			ASSERT_MSG(it.second, "Intermediate %d created twice", id.value());
			return it.first->second;
		}

		Variable& getVariable(SpirvShader::Object::ID id)
		{
			auto it = variables.find(id);
			ASSERT_MSG(it != variables.end(), "Unknown variables %d", id.value());
			return it->second;
		}

		Intermediate const& getIntermediate(SpirvShader::Object::ID id) const
		{
			auto it = intermediates.find(id);
			ASSERT_MSG(it != intermediates.end(), "Unknown intermediate %d", id.value());
			return it->second;
		}

		Pointer<Byte>& getPointer(SpirvShader::Object::ID id)
		{
			auto it = pointers.find(id);
			ASSERT_MSG(it != pointers.end(), "Unknown pointer %d", id.value());
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
				intermediate(obj.kind == SpirvShader::Object::Kind::Intermediate ? &routine->getIntermediate(objId) : nullptr),
				type(obj.type) {}

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

		SpirvShader::Type::ID const type;
	};

}

#endif  // sw_SpirvShader_hpp
