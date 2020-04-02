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

#include "SamplerCore.hpp"
#include "ShaderCore.hpp"
#include "SpirvID.hpp"
#include "Device/Config.hpp"
#include "Device/Sampler.hpp"
#include "System/Debug.hpp"
#include "System/Math.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkConfig.h"
#include "Vulkan/VkDescriptorSet.hpp"

#include <spirv/unified1/spirv.hpp>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#undef Yield  // b/127920555

namespace vk {

class PipelineLayout;
class ImageView;
class Sampler;
class RenderPass;
struct SampledImageDescriptor;

namespace dbg {
class Context;
}  // namespace dbg

}  // namespace vk

namespace sw {

// Forward declarations.
class SpirvRoutine;

// Incrementally constructed complex bundle of rvalues
// Effectively a restricted vector, supporting only:
// - allocation to a (runtime-known) fixed size
// - in-place construction of elements
// - const operator[]
class Intermediate
{
public:
	Intermediate(uint32_t size)
	    : scalar(new rr::Value *[size])
	    , size(size)
	{
		memset(scalar, 0, sizeof(rr::Value *) * size);
	}

	~Intermediate()
	{
		delete[] scalar;
	}

	void move(uint32_t i, RValue<SIMD::Float> &&scalar) { emplace(i, scalar.value); }
	void move(uint32_t i, RValue<SIMD::Int> &&scalar) { emplace(i, scalar.value); }
	void move(uint32_t i, RValue<SIMD::UInt> &&scalar) { emplace(i, scalar.value); }

	void move(uint32_t i, const RValue<SIMD::Float> &scalar) { emplace(i, scalar.value); }
	void move(uint32_t i, const RValue<SIMD::Int> &scalar) { emplace(i, scalar.value); }
	void move(uint32_t i, const RValue<SIMD::UInt> &scalar) { emplace(i, scalar.value); }

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
	Intermediate &operator=(Intermediate const &) = delete;
	Intermediate &operator=(Intermediate &&) = delete;

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

	using ImageSampler = void(void *texture, void *uvsIn, void *texelOut, void *constants);

	enum class YieldResult
	{
		ControlBarrier,
	};

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

		uint32_t const *wordPointer(uint32_t n) const
		{
			ASSERT(n < wordCount());
			return &iter[n];
		}

		const char *string(uint32_t n) const
		{
			return reinterpret_cast<const char *>(wordPointer(n));
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
			InsnIterator ret{ *this };
			iter += wordCount();
			return ret;
		}

		InsnIterator(InsnIterator const &other) = default;

		InsnIterator() = default;

		explicit InsnIterator(InsnStore::const_iterator iter)
		    : iter{ iter }
		{
		}
	};

	/* range-based-for interface */
	InsnIterator begin() const
	{
		return InsnIterator{ insns.cbegin() + 5 };
	}

	InsnIterator end() const
	{
		return InsnIterator{ insns.cend() };
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

			// Pointer held by SpirvRoutine::pointers
			Pointer,

			// A pointer to a vk::DescriptorSet*.
			// Pointer held by SpirvRoutine::pointers.
			DescriptorSet,
		};

		Kind kind = Kind::Unknown;
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

			bool operator==(const Edge &other) const { return from == other.from && to == other.to; }

			struct Hash
			{
				std::size_t operator()(const Edge &edge) const noexcept
				{
					return std::hash<uint32_t>()(edge.from.value() * 31 + edge.to.value());
				}
			};
		};

		Block() = default;
		Block(const Block &other) = default;
		explicit Block(InsnIterator begin, InsnIterator end);

		/* range-based-for interface */
		inline InsnIterator begin() const { return begin_; }
		inline InsnIterator end() const { return end_; }

		enum Kind
		{
			Simple,                         // OpBranch or other simple terminator.
			StructuredBranchConditional,    // OpSelectionMerge + OpBranchConditional
			UnstructuredBranchConditional,  // OpBranchConditional
			StructuredSwitch,               // OpSelectionMerge + OpSwitch
			UnstructuredSwitch,             // OpSwitch
			Loop,                           // OpLoopMerge + [OpBranchConditional | OpBranch]
		};

		Kind kind = Simple;
		InsnIterator mergeInstruction;   // Structured control flow merge instruction.
		InsnIterator branchInstruction;  // Branch instruction.
		ID mergeBlock;                   // Structured flow merge block.
		ID continueTarget;               // Loop continue block.
		Set ins;                         // Blocks that branch into this block.
		Set outs;                        // Blocks that this block branches to.
		bool isLoopMerge = false;

	private:
		InsnIterator begin_;
		InsnIterator end_;
	};

	class Function
	{
	public:
		using ID = SpirvID<Function>;

		// Walks all reachable the blocks starting from id adding them to
		// reachable.
		void TraverseReachableBlocks(Block::ID id, Block::Set &reachable) const;

		// AssignBlockFields() performs the following for all reachable blocks:
		// * Assigns Block::ins with the identifiers of all blocks that contain
		//   this block in their Block::outs.
		// * Sets Block::isLoopMerge to true if the block is the merge of a
		//   another loop block.
		void AssignBlockFields();

		// ForeachBlockDependency calls f with each dependency of the given
		// block. A dependency is an incoming block that is not a loop-back
		// edge.
		void ForeachBlockDependency(Block::ID blockId, std::function<void(Block::ID)> f) const;

		// ExistsPath returns true if there's a direct or indirect flow from
		// the 'from' block to the 'to' block that does not pass through
		// notPassingThrough.
		bool ExistsPath(Block::ID from, Block::ID to, Block::ID notPassingThrough) const;

		Block const &getBlock(Block::ID id) const
		{
			auto it = blocks.find(id);
			ASSERT_MSG(it != blocks.end(), "Unknown block %d", id.value());
			return it->second;
		}

		Block::ID entry;          // function entry point block.
		HandleMap<Block> blocks;  // blocks belonging to this function.
		Type::ID type;            // type of the function.
		Type::ID result;          // return type.
	};

	using String = std::string;
	using StringID = SpirvID<std::string>;

	class Extension
	{
	public:
		using ID = SpirvID<Extension>;

		enum Name
		{
			Unknown,
			GLSLstd450,
			OpenCLDebugInfo100
		};

		Name name;
	};

	struct TypeOrObject
	{};  // Dummy struct to represent a Type or Object.

	// TypeOrObjectID is an identifier that represents a Type or an Object,
	// and supports implicit casting to and from Type::ID or Object::ID.
	class TypeOrObjectID : public SpirvID<TypeOrObject>
	{
	public:
		using Hash = std::hash<SpirvID<TypeOrObject>>;

		inline TypeOrObjectID(uint32_t id)
		    : SpirvID(id)
		{}
		inline TypeOrObjectID(Type::ID id)
		    : SpirvID(id.value())
		{}
		inline TypeOrObjectID(Object::ID id)
		    : SpirvID(id.value())
		{}
		inline operator Type::ID() const { return Type::ID(value()); }
		inline operator Object::ID() const { return Object::ID(value()); }
	};

	// OpImageSample variants
	enum Variant
	{
		None,  // No Dref or Proj. Also used by OpImageFetch and OpImageQueryLod.
		Dref,
		Proj,
		ProjDref,
		VARIANT_LAST = ProjDref
	};

	// Compact representation of image instruction parameters that is passed to the
	// trampoline function for retrieving/generating the corresponding sampling routine.
	struct ImageInstruction
	{
		ImageInstruction(Variant variant, SamplerMethod samplerMethod)
		    : parameters(0)
		{
			this->variant = variant;
			this->samplerMethod = samplerMethod;
		}

		// Unmarshal from raw 32-bit data
		ImageInstruction(uint32_t parameters)
		    : parameters(parameters)
		{}

		SamplerFunction getSamplerFunction() const
		{
			return { static_cast<SamplerMethod>(samplerMethod), offset != 0, sample != 0 };
		}

		bool isDref() const
		{
			return (variant == Dref) || (variant == ProjDref);
		}

		bool isProj() const
		{
			return (variant == Proj) || (variant == ProjDref);
		}

		union
		{
			struct
			{
				uint32_t variant : BITS(VARIANT_LAST);
				uint32_t samplerMethod : BITS(SAMPLER_METHOD_LAST);
				uint32_t gatherComponent : 2;

				// Parameters are passed to the sampling routine in this order:
				uint32_t coordinates : 3;  // 1-4 (does not contain projection component)
				                           //	uint32_t dref : 1;              // Indicated by Variant::ProjDref|Dref
				                           //	uint32_t lodOrBias : 1;         // Indicated by SamplerMethod::Lod|Bias|Fetch
				uint32_t grad : 2;         // 0-3 components (for each of dx / dy)
				uint32_t offset : 2;       // 0-3 components
				uint32_t sample : 1;       // 0-1 scalar integer
			};

			uint32_t parameters;
		};
	};

	static_assert(sizeof(ImageInstruction) == sizeof(uint32_t), "ImageInstruction must be 32-bit");

	// This method is for retrieving an ID that uniquely identifies the
	// shader entry point represented by this object.
	uint64_t getSerialID() const
	{
		return ((uint64_t)entryPoint.value() << 32) | codeSerialID;
	}

	SpirvShader(uint32_t codeSerialID,
	            VkShaderStageFlagBits stage,
	            const char *entryPointName,
	            InsnStore const &insns,
	            const vk::RenderPass *renderPass,
	            uint32_t subpassIndex,
	            bool robustBufferAccess,
	            const std::shared_ptr<vk::dbg::Context> &dbgctx);

	~SpirvShader();

	struct Modes
	{
		bool EarlyFragmentTests : 1;
		bool DepthReplacing : 1;
		bool DepthGreater : 1;
		bool DepthLess : 1;
		bool DepthUnchanged : 1;
		bool ContainsKill : 1;
		bool ContainsControlBarriers : 1;
		bool NeedsCentroid : 1;

		// Compute workgroup dimensions
		int WorkgroupSizeX = 1, WorkgroupSizeY = 1, WorkgroupSizeZ = 1;
	};

	Modes const &getModes() const
	{
		return modes;
	}

	struct Capabilities
	{
		bool Matrix : 1;
		bool Shader : 1;
		bool ClipDistance : 1;
		bool CullDistance : 1;
		bool InputAttachment : 1;
		bool Sampled1D : 1;
		bool Image1D : 1;
		bool ImageCubeArray : 1;
		bool SampledBuffer : 1;
		bool SampledCubeArray : 1;
		bool ImageBuffer : 1;
		bool StorageImageExtendedFormats : 1;
		bool ImageQuery : 1;
		bool DerivativeControl : 1;
		bool GroupNonUniform : 1;
		bool GroupNonUniformVote : 1;
		bool GroupNonUniformBallot : 1;
		bool GroupNonUniformShuffle : 1;
		bool GroupNonUniformShuffleRelative : 1;
		bool GroupNonUniformArithmetic : 1;
		bool DeviceGroup : 1;
		bool MultiView : 1;
		bool StencilExportEXT : 1;
	};

	Capabilities const &getUsedCapabilities() const
	{
		return capabilities;
	}

	// getNumOutputClipDistances() returns the number of ClipDistances
	// outputted by this shader.
	unsigned int getNumOutputClipDistances() const
	{
		if(getUsedCapabilities().ClipDistance)
		{
			auto it = outputBuiltins.find(spv::BuiltInClipDistance);
			if(it != outputBuiltins.end())
			{
				return it->second.SizeInComponents;
			}
		}
		return 0;
	}

	// getNumOutputCullDistances() returns the number of CullDistances
	// outputted by this shader.
	unsigned int getNumOutputCullDistances() const
	{
		if(getUsedCapabilities().CullDistance)
		{
			auto it = outputBuiltins.find(spv::BuiltInCullDistance);
			if(it != outputBuiltins.end())
			{
				return it->second.SizeInComponents;
			}
		}
		return 0;
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

	bool hasBuiltinOutput(spv::BuiltIn b) const
	{
		return outputBuiltins.find(b) != outputBuiltins.end();
	}

	struct Decorations
	{
		int32_t Location = -1;
		int32_t Component = 0;
		spv::BuiltIn BuiltIn = static_cast<spv::BuiltIn>(-1);
		int32_t Offset = -1;
		int32_t ArrayStride = -1;
		int32_t MatrixStride = 1;

		bool HasLocation : 1;
		bool HasComponent : 1;
		bool HasBuiltIn : 1;
		bool HasOffset : 1;
		bool HasArrayStride : 1;
		bool HasMatrixStride : 1;
		bool HasRowMajor : 1;  // whether RowMajor bit is valid.

		bool Flat : 1;
		bool Centroid : 1;
		bool NoPerspective : 1;
		bool Block : 1;
		bool BufferBlock : 1;
		bool RelaxedPrecision : 1;
		bool RowMajor : 1;      // RowMajor if true; ColMajor if false
		bool InsideMatrix : 1;  // pseudo-decoration for whether we're inside a matrix.

		Decorations()
		    : Location{ -1 }
		    , Component{ 0 }
		    , BuiltIn{ static_cast<spv::BuiltIn>(-1) }
		    , Offset{ -1 }
		    , ArrayStride{ -1 }
		    , MatrixStride{ -1 }
		    , HasLocation{ false }
		    , HasComponent{ false }
		    , HasBuiltIn{ false }
		    , HasOffset{ false }
		    , HasArrayStride{ false }
		    , HasMatrixStride{ false }
		    , HasRowMajor{ false }
		    , Flat{ false }
		    , Centroid{ false }
		    , NoPerspective{ false }
		    , Block{ false }
		    , BufferBlock{ false }
		    , RelaxedPrecision{ false }
		    , RowMajor{ false }
		    , InsideMatrix{ false }
		{
		}

		Decorations(Decorations const &) = default;

		void Apply(Decorations const &src);

		void Apply(spv::Decoration decoration, uint32_t arg);
	};

	std::unordered_map<TypeOrObjectID, Decorations, TypeOrObjectID::Hash> decorations;
	std::unordered_map<Type::ID, std::vector<Decorations>> memberDecorations;

	struct DescriptorDecorations
	{
		int32_t DescriptorSet = -1;
		int32_t Binding = -1;
		int32_t InputAttachmentIndex = -1;

		void Apply(DescriptorDecorations const &src);
	};

	std::unordered_map<Object::ID, DescriptorDecorations> descriptorDecorations;
	std::vector<VkFormat> inputAttachmentFormats;

	struct InterfaceComponent
	{
		AttribType Type;

		union
		{
			struct
			{
				bool Flat : 1;
				bool Centroid : 1;
				bool NoPerspective : 1;
			};

			uint8_t DecorationBits;
		};

		InterfaceComponent()
		    : Type{ ATTRIBTYPE_UNUSED }
		    , DecorationBits{ 0 }
		{
		}
	};

	struct BuiltinMapping
	{
		Object::ID Id;
		uint32_t FirstComponent;
		uint32_t SizeInComponents;
	};

	struct WorkgroupMemory
	{
		// allocates a new variable of size bytes with the given identifier.
		inline void allocate(Object::ID id, uint32_t size)
		{
			uint32_t offset = totalSize;
			auto it = offsets.emplace(id, offset);
			ASSERT_MSG(it.second, "WorkgroupMemory already has an allocation for object %d", int(id.value()));
			totalSize += size;
		}
		// returns the byte offset of the variable with the given identifier.
		inline uint32_t offsetOf(Object::ID id) const
		{
			auto it = offsets.find(id);
			ASSERT_MSG(it != offsets.end(), "WorkgroupMemory has no allocation for object %d", int(id.value()));
			return it->second;
		}
		// returns the total allocated size in bytes.
		inline uint32_t size() const { return totalSize; }

	private:
		uint32_t totalSize = 0;                            // in bytes
		std::unordered_map<Object::ID, uint32_t> offsets;  // in bytes
	};

	std::vector<InterfaceComponent> inputs;
	std::vector<InterfaceComponent> outputs;

	void emitProlog(SpirvRoutine *routine) const;
	void emit(SpirvRoutine *routine, RValue<SIMD::Int> const &activeLaneMask, RValue<SIMD::Int> const &storesAndAtomicsMask, const vk::DescriptorSet::Bindings &descriptorSets) const;
	void emitEpilog(SpirvRoutine *routine) const;

	using BuiltInHash = std::hash<std::underlying_type<spv::BuiltIn>::type>;
	std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> inputBuiltins;
	std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> outputBuiltins;
	WorkgroupMemory workgroupMemory;

private:
	const uint32_t codeSerialID;
	Modes modes = {};
	Capabilities capabilities = {};
	HandleMap<Type> types;
	HandleMap<Object> defs;
	HandleMap<Function> functions;
	std::unordered_map<StringID, String> strings;
	HandleMap<Extension> extensionsByID;
	std::unordered_set<Extension::Name> extensionsImported;
	Function::ID entryPoint;

	const bool robustBufferAccess = true;
	spv::ExecutionModel executionModel = spv::ExecutionModelMax;  // Invalid prior to OpEntryPoint parsing.

	// DeclareType creates a Type for the given OpTypeX instruction, storing
	// it into the types map. It is called from the analysis pass (constructor).
	void DeclareType(InsnIterator insn);

	void ProcessExecutionMode(InsnIterator it);

	uint32_t ComputeTypeSize(InsnIterator insn);
	void ApplyDecorationsForId(Decorations *d, TypeOrObjectID id) const;
	void ApplyDecorationsForIdMember(Decorations *d, Type::ID id, uint32_t member) const;
	void ApplyDecorationsForAccessChain(Decorations *d, DescriptorDecorations *dd, Object::ID baseId, uint32_t numIndexes, uint32_t const *indexIds) const;

	// Creates an Object for the instruction's result in 'defs'.
	void DefineResult(const InsnIterator &insn);

	// Processes the OpenCL.Debug.100 instruction for the initial definition
	// pass of the SPIR-V.
	void DefineOpenCLDebugInfo100(const InsnIterator &insn);

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
	static bool IsExplicitLayout(spv::StorageClass storageClass);

	static sw::SIMD::Pointer InterleaveByLane(sw::SIMD::Pointer p);

	// Output storage buffers and images should not be affected by helper invocations
	static bool StoresInHelperInvocation(spv::StorageClass storageClass);

	using InterfaceVisitor = std::function<void(Decorations const, AttribType)>;

	void VisitInterface(Object::ID id, const InterfaceVisitor &v) const;

	int VisitInterfaceInner(Type::ID id, Decorations d, const InterfaceVisitor &v) const;

	// MemoryElement describes a scalar element within a structure, and is
	// used by the callback function of VisitMemoryObject().
	struct MemoryElement
	{
		uint32_t index;    // index of the scalar element
		uint32_t offset;   // offset (in bytes) from the base of the object
		const Type &type;  // element type
	};

	using MemoryVisitor = std::function<void(const MemoryElement &)>;

	// VisitMemoryObject() walks a type tree in an explicitly laid out
	// storage class, calling the MemoryVisitor for each scalar element
	// within the
	void VisitMemoryObject(Object::ID id, const MemoryVisitor &v) const;

	// VisitMemoryObjectInner() is internally called by VisitMemoryObject()
	void VisitMemoryObjectInner(Type::ID id, Decorations d, uint32_t &index, uint32_t offset, const MemoryVisitor &v) const;

	Object &CreateConstant(InsnIterator it);

	void ProcessInterfaceVariable(Object &object);

	// EmitState holds control-flow state for the emit() pass.
	class EmitState
	{
	public:
		EmitState(SpirvRoutine *routine,
		          Function::ID function,
		          RValue<SIMD::Int> activeLaneMask,
		          RValue<SIMD::Int> storesAndAtomicsMask,
		          const vk::DescriptorSet::Bindings &descriptorSets,
		          bool robustBufferAccess,
		          spv::ExecutionModel executionModel)
		    : routine(routine)
		    , function(function)
		    , activeLaneMaskValue(activeLaneMask.value)
		    , storesAndAtomicsMaskValue(storesAndAtomicsMask.value)
		    , descriptorSets(descriptorSets)
		    , robustBufferAccess(robustBufferAccess)
		    , executionModel(executionModel)
		{
			ASSERT(executionModelToStage(executionModel) != VkShaderStageFlagBits(0));  // Must parse OpEntryPoint before emitting.
		}

		RValue<SIMD::Int> activeLaneMask() const
		{
			ASSERT(activeLaneMaskValue != nullptr);
			return RValue<SIMD::Int>(activeLaneMaskValue);
		}

		RValue<SIMD::Int> storesAndAtomicsMask() const
		{
			ASSERT(storesAndAtomicsMaskValue != nullptr);
			return RValue<SIMD::Int>(storesAndAtomicsMaskValue);
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

		SpirvRoutine *routine = nullptr;                 // The current routine being built.
		Function::ID function;                           // The current function being built.
		Block::ID block;                                 // The current block being built.
		rr::Value *activeLaneMaskValue = nullptr;        // The current active lane mask.
		rr::Value *storesAndAtomicsMaskValue = nullptr;  // The current atomics mask.
		Block::Set visited;                              // Blocks already built.
		std::unordered_map<Block::Edge, RValue<SIMD::Int>, Block::Edge::Hash> edgeActiveLaneMasks;
		std::deque<Block::ID> *pending;

		const vk::DescriptorSet::Bindings &descriptorSets;

		OutOfBoundsBehavior getOutOfBoundsBehavior(spv::StorageClass storageClass) const;

		Intermediate &createIntermediate(Object::ID id, uint32_t size)
		{
			auto it = intermediates.emplace(std::piecewise_construct,
			                                std::forward_as_tuple(id),
			                                std::forward_as_tuple(size));
			ASSERT_MSG(it.second, "Intermediate %d created twice", id.value());
			return it.first->second;
		}

		Intermediate const &getIntermediate(Object::ID id) const
		{
			auto it = intermediates.find(id);
			ASSERT_MSG(it != intermediates.end(), "Unknown intermediate %d", id.value());
			return it->second;
		}

		void createPointer(Object::ID id, SIMD::Pointer ptr)
		{
			bool added = pointers.emplace(id, ptr).second;
			ASSERT_MSG(added, "Pointer %d created twice", id.value());
		}

		SIMD::Pointer const &getPointer(Object::ID id) const
		{
			auto it = pointers.find(id);
			ASSERT_MSG(it != pointers.end(), "Unknown pointer %d", id.value());
			return it->second;
		}

	private:
		std::unordered_map<Object::ID, Intermediate> intermediates;
		std::unordered_map<Object::ID, SIMD::Pointer> pointers;

		const bool robustBufferAccess = true;  // Emit robustBufferAccess safe code.
		const spv::ExecutionModel executionModel = spv::ExecutionModelMax;
	};

	// EmitResult is an enumerator of result values from the Emit functions.
	enum class EmitResult
	{
		Continue,    // No termination instructions.
		Terminator,  // Reached a termination instruction.
	};

	// Generic wrapper over either per-lane intermediate value, or a constant.
	// Constants are transparently widened to per-lane values in operator[].
	// This is appropriate in most cases -- if we're not going to do something
	// significantly different based on whether the value is uniform across lanes.
	class GenericValue
	{
		SpirvShader::Object const &obj;
		Intermediate const *intermediate;

	public:
		GenericValue(SpirvShader const *shader, EmitState const *state, SpirvShader::Object::ID objId);

		RValue<SIMD::Float> Float(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->Float(i);
			}

			// Constructing a constant SIMD::Float is not guaranteed to preserve the data's exact
			// bit pattern, but SPIR-V provides 32-bit words representing "the bit pattern for the constant".
			// Thus we must first construct an integer constant, and bitcast to float.
			ASSERT(obj.kind == SpirvShader::Object::Kind::Constant);
			auto constantValue = reinterpret_cast<uint32_t *>(obj.constantValue.get());
			return As<SIMD::Float>(SIMD::UInt(constantValue[i]));
		}

		RValue<SIMD::Int> Int(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->Int(i);
			}
			ASSERT(obj.kind == SpirvShader::Object::Kind::Constant);
			auto constantValue = reinterpret_cast<int *>(obj.constantValue.get());
			return SIMD::Int(constantValue[i]);
		}

		RValue<SIMD::UInt> UInt(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->UInt(i);
			}
			ASSERT(obj.kind == SpirvShader::Object::Kind::Constant);
			auto constantValue = reinterpret_cast<uint32_t *>(obj.constantValue.get());
			return SIMD::UInt(constantValue[i]);
		}

		SpirvShader::Type::ID const type;
	};

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

	Function const &getFunction(Function::ID id) const
	{
		auto it = functions.find(id);
		ASSERT_MSG(it != functions.end(), "Unknown function %d", id.value());
		return it->second;
	}

	String const &getString(StringID id) const
	{
		auto it = strings.find(id);
		ASSERT_MSG(it != strings.end(), "Unknown string %d", id.value());
		return it->second;
	}

	Extension const &getExtension(Extension::ID id) const
	{
		auto it = extensionsByID.find(id);
		ASSERT_MSG(it != extensionsByID.end(), "Unknown extension %d", id.value());
		return it->second;
	}

	// Returns a SIMD::Pointer to the underlying data for the given pointer
	// object.
	// Handles objects of the following kinds:
	//  • DescriptorSet
	//  • DivergentPointer
	//  • InterfaceVariable
	//  • NonDivergentPointer
	// Calling GetPointerToData with objects of any other kind will assert.
	SIMD::Pointer GetPointerToData(Object::ID id, int arrayIndex, EmitState const *state) const;

	SIMD::Pointer WalkExplicitLayoutAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, EmitState const *state) const;
	SIMD::Pointer WalkAccessChain(Object::ID id, uint32_t numIndexes, uint32_t const *indexIds, EmitState const *state) const;

	// Returns the *component* offset in the literal for the given access chain.
	uint32_t WalkLiteralAccessChain(Type::ID id, uint32_t numIndexes, uint32_t const *indexes) const;

	// Lookup the active lane mask for the edge from -> to.
	// If from is unreachable, then a mask of all zeros is returned.
	// Asserts if from is reachable and the edge does not exist.
	RValue<SIMD::Int> GetActiveLaneMaskEdge(EmitState *state, Block::ID from, Block::ID to) const;

	// Updates the current active lane mask.
	void SetActiveLaneMask(RValue<SIMD::Int> mask, EmitState *state) const;

	// Emit all the unvisited blocks (except for ignore) in DFS order,
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
	EmitResult EmitExtGLSLstd450(InsnIterator insn, EmitState *state) const;
	EmitResult EmitOpenCLDebugInfo100(InsnIterator insn, EmitState *state) const;
	EmitResult EmitLine(InsnIterator insn, EmitState *state) const;
	EmitResult EmitAny(InsnIterator insn, EmitState *state) const;
	EmitResult EmitAll(InsnIterator insn, EmitState *state) const;
	EmitResult EmitBranch(InsnIterator insn, EmitState *state) const;
	EmitResult EmitBranchConditional(InsnIterator insn, EmitState *state) const;
	EmitResult EmitSwitch(InsnIterator insn, EmitState *state) const;
	EmitResult EmitUnreachable(InsnIterator insn, EmitState *state) const;
	EmitResult EmitReturn(InsnIterator insn, EmitState *state) const;
	EmitResult EmitKill(InsnIterator insn, EmitState *state) const;
	EmitResult EmitFunctionCall(InsnIterator insn, EmitState *state) const;
	EmitResult EmitPhi(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageSampleImplicitLod(Variant variant, InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageSampleExplicitLod(Variant variant, InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageGather(Variant variant, InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageFetch(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageSample(ImageInstruction instruction, InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQuerySizeLod(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQuerySize(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQueryLod(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQueryLevels(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQuerySamples(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageRead(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageWrite(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageTexelPointer(InsnIterator insn, EmitState *state) const;
	EmitResult EmitAtomicOp(InsnIterator insn, EmitState *state) const;
	EmitResult EmitAtomicCompareExchange(InsnIterator insn, EmitState *state) const;
	EmitResult EmitSampledImageCombineOrSplit(InsnIterator insn, EmitState *state) const;
	EmitResult EmitCopyObject(InsnIterator insn, EmitState *state) const;
	EmitResult EmitCopyMemory(InsnIterator insn, EmitState *state) const;
	EmitResult EmitControlBarrier(InsnIterator insn, EmitState *state) const;
	EmitResult EmitMemoryBarrier(InsnIterator insn, EmitState *state) const;
	EmitResult EmitGroupNonUniform(InsnIterator insn, EmitState *state) const;
	EmitResult EmitArrayLength(InsnIterator insn, EmitState *state) const;

	void GetImageDimensions(EmitState const *state, Type const &resultTy, Object::ID imageId, Object::ID lodId, Intermediate &dst) const;
	SIMD::Pointer GetTexelAddress(EmitState const *state, SIMD::Pointer base, GenericValue const &coordinate, Type const &imageType, Pointer<Byte> descriptor, int texelSize, Object::ID sampleId, bool useStencilAspect) const;
	uint32_t GetConstScalarInt(Object::ID id) const;
	void EvalSpecConstantOp(InsnIterator insn);
	void EvalSpecConstantUnaryOp(InsnIterator insn);
	void EvalSpecConstantBinaryOp(InsnIterator insn);

	// LoadPhi loads the phi values from the alloca storage and places the
	// load values into the intermediate with the phi's result id.
	void LoadPhi(InsnIterator insn, EmitState *state) const;

	// StorePhi updates the phi's alloca storage value using the incoming
	// values from blocks that are both in the OpPhi instruction and in
	// filter.
	void StorePhi(Block::ID blockID, InsnIterator insn, EmitState *state, std::unordered_set<SpirvShader::Block::ID> const &filter) const;

	// Emits a rr::Fence for the given MemorySemanticsMask.
	void Fence(spv::MemorySemanticsMask semantics) const;

	// Helper for calling rr::Yield with res cast to an rr::Int.
	void Yield(YieldResult res) const;

	// OpcodeName() returns the name of the opcode op.
	// If NDEBUG is defined, then OpcodeName() will only return the numerical code.
	static std::string OpcodeName(spv::Op op);
	static std::memory_order MemoryOrder(spv::MemorySemanticsMask memorySemantics);

	// IsStatement() returns true if the given opcode actually performs
	// work (as opposed to declaring a type, defining a function start / end,
	// etc).
	static bool IsStatement(spv::Op op);

	// Helper as we often need to take dot products as part of doing other things.
	SIMD::Float Dot(unsigned numComponents, GenericValue const &x, GenericValue const &y) const;

	// Splits x into a floating-point significand in the range [0.5, 1.0)
	// and an integral exponent of two, such that:
	//   x = significand * 2^exponent
	// Returns the pair <significand, exponent>
	std::pair<SIMD::Float, SIMD::Int> Frexp(RValue<SIMD::Float> val) const;

	static ImageSampler *getImageSampler(uint32_t instruction, vk::SampledImageDescriptor const *imageDescriptor, const vk::Sampler *sampler);
	static std::shared_ptr<rr::Routine> emitSamplerRoutine(ImageInstruction instruction, const Sampler &samplerState);

	// TODO(b/129523279): Eliminate conversion and use vk::Sampler members directly.
	static sw::FilterType convertFilterMode(const vk::Sampler *sampler);
	static sw::MipmapType convertMipmapMode(const vk::Sampler *sampler);
	static sw::AddressingMode convertAddressingMode(int coordinateIndex, const vk::Sampler *sampler, VkImageViewType imageViewType);

	// Returns 0 when invalid.
	static VkShaderStageFlagBits executionModelToStage(spv::ExecutionModel model);

	// Debugger API functions. When ENABLE_VK_DEBUGGER is not defined, these
	// are all no-ops.

	// dbgInit() initializes the debugger code generation.
	// All other dbgXXX() functions are no-op until this is called.
	void dbgInit(const std::shared_ptr<vk::dbg::Context> &dbgctx);

	// dbgTerm() terminates the debugger code generation.
	void dbgTerm();

	// dbgCreateFile() generates a synthetic file containing the disassembly
	// of the SPIR-V shader. This is the file displayed in the debug
	// session.
	void dbgCreateFile();

	// dbgBeginEmit() sets up the debugging state for the shader.
	void dbgBeginEmit(EmitState *state) const;

	// dbgEndEmit() tears down the debugging state for the shader.
	void dbgEndEmit(EmitState *state) const;

	// dbgBeginEmitInstruction() updates the current debugger location for
	// the given instruction.
	void dbgBeginEmitInstruction(InsnIterator insn, EmitState *state) const;

	// dbgEndEmitInstruction() creates any new debugger variables for the
	// instruction that just completed.
	void dbgEndEmitInstruction(InsnIterator insn, EmitState *state) const;

	// dbgExposeIntermediate() exposes the intermediate with the given ID to
	// the debugger.
	void dbgExposeIntermediate(Object::ID id, EmitState *state) const;

	// dbgUpdateActiveLaneMask() updates the active lane masks to the
	// debugger.
	void dbgUpdateActiveLaneMask(RValue<SIMD::Int> mask, EmitState *state) const;

	// dbgDeclareResult() associates resultId as the result of the given
	// instruction.
	void dbgDeclareResult(const InsnIterator &insn, Object::ID resultId) const;

	// Impl holds forward declaration structs and pointers to state for the
	// private implementations in the corresponding SpirvShaderXXX.cpp files.
	// This allows access to the private members of the SpirvShader, without
	// littering the header with implementation details.
	struct Impl
	{
		struct Debugger;
		struct Group;
		Debugger *debugger = nullptr;
	};
	Impl impl;
};

class SpirvRoutine
{
public:
	SpirvRoutine(vk::PipelineLayout const *pipelineLayout);

	using Variable = Array<SIMD::Float>;

	struct SamplerCache
	{
		Pointer<Byte> imageDescriptor = nullptr;
		Pointer<Byte> sampler;
		Pointer<Byte> function;
	};

	vk::PipelineLayout const *const pipelineLayout;

	std::unordered_map<SpirvShader::Object::ID, Variable> variables;
	std::unordered_map<SpirvShader::Object::ID, SamplerCache> samplerCache;
	Variable inputs = Variable{ MAX_INTERFACE_COMPONENTS };
	Variable outputs = Variable{ MAX_INTERFACE_COMPONENTS };

	Pointer<Byte> workgroupMemory;
	Pointer<Pointer<Byte>> descriptorSets;
	Pointer<Int> descriptorDynamicOffsets;
	Pointer<Byte> pushConstants;
	Pointer<Byte> constants;
	Int killMask = Int{ 0 };

	// Shader invocation state.
	// Not all of these variables are used for every type of shader, and some
	// are only used when debugging. See b/146486064 for more information.
	// Give careful consideration to the runtime performance loss before adding
	// more state here.
	SIMD::Int windowSpacePosition[2];
	Int viewID;  // slice offset into input attachments for multiview, even if the shader doesn't use ViewIndex
	Int instanceID;
	SIMD::Int vertexIndex;
	std::array<SIMD::Float, 4> fragCoord;
	std::array<SIMD::Float, 4> pointCoord;
	SIMD::Int helperInvocation;
	Int4 numWorkgroups;
	Int4 workgroupID;
	Int4 workgroupSize;
	Int subgroupsPerWorkgroup;
	Int invocationsPerSubgroup;
	Int subgroupIndex;
	SIMD::Int localInvocationIndex;
	std::array<SIMD::Int, 3> localInvocationID;
	std::array<SIMD::Int, 3> globalInvocationID;

	Pointer<Byte> dbgState;  // Pointer to a debugger state.

	void createVariable(SpirvShader::Object::ID id, uint32_t size)
	{
		bool added = variables.emplace(id, Variable(size)).second;
		ASSERT_MSG(added, "Variable %d created twice", id.value());
	}

	Variable &getVariable(SpirvShader::Object::ID id)
	{
		auto it = variables.find(id);
		ASSERT_MSG(it != variables.end(), "Unknown variables %d", id.value());
		return it->second;
	}

	// setImmutableInputBuiltins() sets all the immutable input builtins,
	// common for all shader types.
	void setImmutableInputBuiltins(SpirvShader const *shader);

	// setInputBuiltin() calls f() with the builtin and value if the shader
	// uses the input builtin, otherwise the call is a no-op.
	// F is a function with the signature:
	// void(const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)
	template<typename F>
	inline void setInputBuiltin(SpirvShader const *shader, spv::BuiltIn id, F &&f)
	{
		auto it = shader->inputBuiltins.find(id);
		if(it != shader->inputBuiltins.end())
		{
			const auto &builtin = it->second;
			f(builtin, getVariable(builtin.Id));
		}
	}

private:
	// The phis are only accessible to SpirvShader as they are only used and
	// exist between calls to SpirvShader::emitProlog() and
	// SpirvShader::emitEpilog().
	friend class SpirvShader;

	std::unordered_map<SpirvShader::Object::ID, Variable> phis;
};

}  // namespace sw

#endif  // sw_SpirvShader_hpp
