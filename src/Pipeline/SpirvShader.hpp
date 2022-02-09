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
#include "SpirvBinary.hpp"
#include "SpirvID.hpp"
#include "SpirvProfiler.hpp"
#include "Device/Config.hpp"
#include "Device/Sampler.hpp"
#include "System/Debug.hpp"
#include "System/Math.hpp"
#include "System/Types.hpp"
#include "Vulkan/VkConfig.hpp"
#include "Vulkan/VkDescriptorSet.hpp"

#define SPV_ENABLE_UTILITY_CODE
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

class Device;
class PipelineLayout;
class ImageView;
class Sampler;
class RenderPass;
struct SampledImageDescriptor;
struct SamplerState;

namespace dbg {
class Context;
}  // namespace dbg

}  // namespace vk

namespace sw {

// Forward declarations.
class SpirvRoutine;

// Incrementally constructed complex bundle of rvalues
// Effectively a restricted vector, supporting only:
// - allocation to a (runtime-known) fixed component count
// - in-place construction of elements
// - const operator[]
class Intermediate
{
public:
	Intermediate(uint32_t componentCount)
	    : componentCount(componentCount)
	    , scalar(new rr::Value *[componentCount])
	{
		for(auto i = 0u; i < componentCount; i++) { scalar[i] = nullptr; }
	}

	~Intermediate()
	{
		delete[] scalar;
	}

	// TypeHint is used as a hint for rr::PrintValue::Ty<sw::Intermediate> to
	// decide the format used to print the intermediate data.
	enum class TypeHint
	{
		Float,
		Int,
		UInt
	};

	void move(uint32_t i, RValue<SIMD::Float> &&scalar) { emplace(i, scalar.value(), TypeHint::Float); }
	void move(uint32_t i, RValue<SIMD::Int> &&scalar) { emplace(i, scalar.value(), TypeHint::Int); }
	void move(uint32_t i, RValue<SIMD::UInt> &&scalar) { emplace(i, scalar.value(), TypeHint::UInt); }

	void move(uint32_t i, const RValue<SIMD::Float> &scalar) { emplace(i, scalar.value(), TypeHint::Float); }
	void move(uint32_t i, const RValue<SIMD::Int> &scalar) { emplace(i, scalar.value(), TypeHint::Int); }
	void move(uint32_t i, const RValue<SIMD::UInt> &scalar) { emplace(i, scalar.value(), TypeHint::UInt); }

	// Value retrieval functions.
	RValue<SIMD::Float> Float(uint32_t i) const
	{
		ASSERT(i < componentCount);
		ASSERT(scalar[i] != nullptr);
		return As<SIMD::Float>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::Float>(scalar)
	}

	RValue<SIMD::Int> Int(uint32_t i) const
	{
		ASSERT(i < componentCount);
		ASSERT(scalar[i] != nullptr);
		return As<SIMD::Int>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::Int>(scalar)
	}

	RValue<SIMD::UInt> UInt(uint32_t i) const
	{
		ASSERT(i < componentCount);
		ASSERT(scalar[i] != nullptr);
		return As<SIMD::UInt>(scalar[i]);  // TODO(b/128539387): RValue<SIMD::UInt>(scalar)
	}

	// No copy/move construction or assignment
	Intermediate(Intermediate const &) = delete;
	Intermediate(Intermediate &&) = delete;
	Intermediate &operator=(Intermediate const &) = delete;
	Intermediate &operator=(Intermediate &&) = delete;

	const uint32_t componentCount;

private:
	void emplace(uint32_t i, rr::Value *value, TypeHint type)
	{
		ASSERT(i < componentCount);
		ASSERT(scalar[i] == nullptr);
		scalar[i] = value;
		RR_PRINT_ONLY(typeHint = type;)
	}

	rr::Value **const scalar;

#ifdef ENABLE_RR_PRINT
	friend struct rr::PrintValue::Ty<sw::Intermediate>;
	TypeHint typeHint = TypeHint::Float;
#endif  // ENABLE_RR_PRINT
};

class SpirvShader
{
public:
	SpirvBinary insns;

	using ImageSampler = void(void *texture, void *uvsIn, void *texelOut, void *constants);

	enum class YieldResult
	{
		ControlBarrier,
	};

	class Type;
	class Object;

	// Pseudo-iterator over SPIRV instructions, designed to support range-based-for.
	class InsnIterator
	{
	public:
		InsnIterator() = default;
		InsnIterator(InsnIterator const &other) = default;
		InsnIterator &operator=(const InsnIterator &other) = default;

		explicit InsnIterator(SpirvBinary::const_iterator iter)
		    : iter{ iter }
		{
		}

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
			return &iter[n];
		}

		const char *string(uint32_t n) const
		{
			return reinterpret_cast<const char *>(wordPointer(n));
		}

		// Returns the number of whole-words that a string literal starting at
		// word n consumes. If the end of the intruction is reached before the
		// null-terminator is found, then the function DABORT()s and 0 is
		// returned.
		uint32_t stringSizeInWords(uint32_t n) const
		{
			uint32_t c = wordCount();
			for(uint32_t i = n; n < c; i++)
			{
				auto *u32 = wordPointer(i);
				auto *u8 = reinterpret_cast<const uint8_t *>(u32);
				// SPIR-V spec 2.2.1. Instructions:
				// A string is interpreted as a nul-terminated stream of
				// characters. The character set is Unicode in the UTF-8
				// encoding scheme. The UTF-8 octets (8-bit bytes) are packed
				// four per word, following the little-endian convention (i.e.,
				// the first octet is in the lowest-order 8 bits of the word).
				// The final word contains the string’s nul-termination
				// character (0), and all contents past the end of the string in
				// the final word are padded with 0.
				if(u8[3] == 0)
				{
					return 1 + i - n;
				}
			}
			DABORT("SPIR-V string literal was not null-terminated");
			return 0;
		}

		bool hasResultAndType() const
		{
			bool hasResult = false, hasResultType = false;
			spv::HasResultAndType(opcode(), &hasResult, &hasResultType);

			return hasResultType;
		}

		SpirvID<Type> resultTypeId() const
		{
			ASSERT(hasResultAndType());
			return word(1);
		}

		SpirvID<Object> resultId() const
		{
			ASSERT(hasResultAndType());
			return word(2);
		}

		uint32_t distanceFrom(const InsnIterator &other) const
		{
			return static_cast<uint32_t>(iter - other.iter);
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

	private:
		SpirvBinary::const_iterator iter;
	};

	// Range-based-for interface
	InsnIterator begin() const
	{
		// Skip over the header words
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
		uint32_t componentCount = 0;
		bool isBuiltInBlock = false;

		// Inner element type for pointers, arrays, vectors and matrices.
		ID element;
	};

	class Object
	{
	public:
		using ID = SpirvID<Object>;

		spv::Op opcode() const { return definition.opcode(); }
		Type::ID typeId() const { return definition.resultTypeId(); }
		Object::ID id() const { return definition.resultId(); }

		bool isConstantZero() const;

		InsnIterator definition;
		std::vector<uint32_t> constantValue;

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
		Block &operator=(const Block &other) = default;
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
	{};

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
	enum Variant : uint32_t
	{
		None,  // No Dref or Proj. Also used by OpImageFetch and OpImageQueryLod.
		Dref,
		Proj,
		ProjDref,
		VARIANT_LAST = ProjDref
	};

	// Compact representation of image instruction state that is passed to the
	// trampoline function for retrieving/generating the corresponding sampling routine.
	struct ImageInstructionSignature
	{
		ImageInstructionSignature(Variant variant, SamplerMethod samplerMethod)
		{
			this->variant = variant;
			this->samplerMethod = samplerMethod;
		}

		// Unmarshal from raw 32-bit data
		explicit ImageInstructionSignature(uint32_t signature)
		    : signature(signature)
		{}

		SamplerFunction getSamplerFunction() const
		{
			return { samplerMethod, offset != 0, sample != 0 };
		}

		bool isDref() const
		{
			return (variant == Dref) || (variant == ProjDref);
		}

		bool isProj() const
		{
			return (variant == Proj) || (variant == ProjDref);
		}

		bool hasLod() const
		{
			return samplerMethod == Lod || samplerMethod == Fetch;  // We always pass a Lod operand for Fetch operations.
		}

		bool hasGrad() const
		{
			return samplerMethod == Grad;
		}

		union
		{
			struct
			{
				Variant variant : BITS(VARIANT_LAST);
				SamplerMethod samplerMethod : BITS(SAMPLER_METHOD_LAST);
				uint32_t gatherComponent : 2;
				uint32_t dim : BITS(spv::DimSubpassData);  // spv::Dim
				uint32_t arrayed : 1;
				uint32_t imageFormat : BITS(spv::ImageFormatR64i);  // spv::ImageFormat

				// Parameters are passed to the sampling routine in this order:
				uint32_t coordinates : 3;       // 1-4 (does not contain projection component)
				/*	uint32_t dref : 1; */       // Indicated by Variant::ProjDref|Dref
				/*	uint32_t lodOrBias : 1; */  // Indicated by SamplerMethod::Lod|Bias|Fetch
				uint32_t grad : 2;              // 0-3 components (for each of dx / dy)
				uint32_t offset : 2;            // 0-3 components
				uint32_t sample : 1;            // 0-1 scalar integer
			};

			uint32_t signature = 0;
		};
	};

	// This gets stored as a literal in the generated code, so it should be compact.
	static_assert(sizeof(ImageInstructionSignature) == sizeof(uint32_t), "ImageInstructionSignature must be 32-bit");

	struct ImageInstruction : public ImageInstructionSignature
	{
		ImageInstruction(InsnIterator insn, const SpirvShader &spirv);

		const uint32_t position;

		Type::ID resultTypeId = 0;
		Object::ID resultId = 0;
		Object::ID imageId = 0;
		Object::ID samplerId = 0;
		Object::ID coordinateId = 0;
		Object::ID texelId = 0;
		Object::ID drefId = 0;
		Object::ID lodOrBiasId = 0;
		Object::ID gradDxId = 0;
		Object::ID gradDyId = 0;
		Object::ID offsetId = 0;
		Object::ID sampleId = 0;

	private:
		static ImageInstructionSignature parseVariantAndMethod(InsnIterator insn);
		static uint32_t getImageOperandsIndex(InsnIterator insn);
		static uint32_t getImageOperandsMask(InsnIterator insn);
	};

	// This method is for retrieving an ID that uniquely identifies the
	// shader entry point represented by this object.
	uint64_t getIdentifier() const
	{
		return ((uint64_t)entryPoint.value() << 32) | insns.getIdentifier();
	}

	SpirvShader(VkShaderStageFlagBits stage,
	            const char *entryPointName,
	            SpirvBinary const &insns,
	            const vk::RenderPass *renderPass,
	            uint32_t subpassIndex,
	            bool robustBufferAccess,
	            const std::shared_ptr<vk::dbg::Context> &dbgctx,
	            std::shared_ptr<SpirvProfiler> profiler);

	~SpirvShader();

	struct ExecutionModes
	{
		bool EarlyFragmentTests : 1;
		bool DepthReplacing : 1;
		bool DepthGreater : 1;
		bool DepthLess : 1;
		bool DepthUnchanged : 1;

		// Compute workgroup dimensions
		int WorkgroupSizeX = 1;
		int WorkgroupSizeY = 1;
		int WorkgroupSizeZ = 1;
	};

	const ExecutionModes &getExecutionModes() const
	{
		return executionModes;
	}

	struct Analysis
	{
		bool ContainsKill : 1;
		bool ContainsControlBarriers : 1;
		bool NeedsCentroid : 1;
		bool ContainsSampleQualifier : 1;
	};

	const Analysis &getAnalysis() const
	{
		return analysis;
	}

	struct Capabilities
	{
		bool Matrix : 1;
		bool Shader : 1;
		bool StorageImageMultisample : 1;
		bool ClipDistance : 1;
		bool CullDistance : 1;
		bool ImageCubeArray : 1;
		bool SampleRateShading : 1;
		bool InputAttachment : 1;
		bool Sampled1D : 1;
		bool Image1D : 1;
		bool SampledBuffer : 1;
		bool SampledCubeArray : 1;
		bool ImageBuffer : 1;
		bool ImageMSArray : 1;
		bool StorageImageExtendedFormats : 1;
		bool ImageQuery : 1;
		bool DerivativeControl : 1;
		bool InterpolationFunction : 1;
		bool StorageImageWriteWithoutFormat : 1;
		bool GroupNonUniform : 1;
		bool GroupNonUniformVote : 1;
		bool GroupNonUniformBallot : 1;
		bool GroupNonUniformShuffle : 1;
		bool GroupNonUniformShuffleRelative : 1;
		bool GroupNonUniformArithmetic : 1;
		bool DeviceGroup : 1;
		bool MultiView : 1;
		bool StencilExportEXT : 1;
		bool VulkanMemoryModel : 1;
		bool VulkanMemoryModelDeviceScope : 1;
	};

	const Capabilities &getUsedCapabilities() const
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
	std::vector<vk::Format> inputAttachmentFormats;

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
	void emit(SpirvRoutine *routine, RValue<SIMD::Int> const &activeLaneMask, RValue<SIMD::Int> const &storesAndAtomicsMask, const vk::DescriptorSet::Bindings &descriptorSets, unsigned int multiSampleCount = 0) const;
	void emitEpilog(SpirvRoutine *routine) const;
	void clearPhis(SpirvRoutine *routine) const;

	bool containsImageWrite() const { return imageWriteEmitted; }

	using BuiltInHash = std::hash<std::underlying_type<spv::BuiltIn>::type>;
	std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> inputBuiltins;
	std::unordered_map<spv::BuiltIn, BuiltinMapping, BuiltInHash> outputBuiltins;
	WorkgroupMemory workgroupMemory;

private:
	const bool robustBufferAccess;

	Function::ID entryPoint;
	spv::ExecutionModel executionModel = spv::ExecutionModelMax;  // Invalid prior to OpEntryPoint parsing.
	ExecutionModes executionModes = {};
	Capabilities capabilities = {};
	spv::AddressingModel addressingModel = spv::AddressingModelLogical;
	spv::MemoryModel memoryModel = spv::MemoryModelSimple;
	HandleMap<Extension> extensionsByID;
	std::unordered_set<uint32_t> extensionsImported;

	Analysis analysis = {};
	mutable bool imageWriteEmitted = false;

	HandleMap<Type> types;
	HandleMap<Object> defs;
	HandleMap<Function> functions;
	std::unordered_map<StringID, String> strings;

	std::shared_ptr<SpirvProfiler> profiler;

	bool IsProfilingEnabled() const
	{
		return profiler != nullptr;
	}

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
		          unsigned int multiSampleCount,
		          spv::ExecutionModel executionModel)
		    : routine(routine)
		    , function(function)
		    , activeLaneMaskValue(activeLaneMask.value())
		    , storesAndAtomicsMaskValue(storesAndAtomicsMask.value())
		    , descriptorSets(descriptorSets)
		    , robustBufferAccess(robustBufferAccess)
		    , multiSampleCount(multiSampleCount)
		    , executionModel(executionModel)
		{
			ASSERT(executionModel != spv::ExecutionModelMax);  // Must parse OpEntryPoint before emitting.
		}

		// Returns the mask describing the active lanes as updated by dynamic
		// control flow. Active lanes include helper invocations, used for
		// calculating fragment derivitives, which must not perform memory
		// stores or atomic writes.
		//
		// Use activeStoresAndAtomicsMask() to consider both control flow and
		// lanes which are permitted to perform memory stores and atomic
		// operations
		RValue<SIMD::Int> activeLaneMask() const
		{
			ASSERT(activeLaneMaskValue != nullptr);
			return RValue<SIMD::Int>(activeLaneMaskValue);
		}

		// Returns the immutable lane mask that describes which lanes are
		// permitted to perform memory stores and atomic operations.
		// Note that unlike activeStoresAndAtomicsMask() this mask *does not*
		// consider lanes that have been made inactive due to control flow.
		RValue<SIMD::Int> storesAndAtomicsMask() const
		{
			ASSERT(storesAndAtomicsMaskValue != nullptr);
			return RValue<SIMD::Int>(storesAndAtomicsMaskValue);
		}

		// Returns a lane mask that describes which lanes are permitted to
		// perform memory stores and atomic operations, considering lanes that
		// may have been made inactive due to control flow.
		RValue<SIMD::Int> activeStoresAndAtomicsMask() const
		{
			return activeLaneMask() & storesAndAtomicsMask();
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

		unsigned int getMultiSampleCount() const { return multiSampleCount; }

		Intermediate &createIntermediate(Object::ID id, uint32_t componentCount)
		{
			auto it = intermediates.emplace(std::piecewise_construct,
			                                std::forward_as_tuple(id),
			                                std::forward_as_tuple(componentCount));
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

		const bool robustBufferAccess;  // Emit robustBufferAccess safe code.
		const unsigned int multiSampleCount;
		const spv::ExecutionModel executionModel;
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
	class Operand
	{
	public:
		Operand(const SpirvShader *shader, const EmitState *state, SpirvShader::Object::ID objectId);
		Operand(const Intermediate &value);

		RValue<SIMD::Float> Float(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->Float(i);
			}

			// Constructing a constant SIMD::Float is not guaranteed to preserve the data's exact
			// bit pattern, but SPIR-V provides 32-bit words representing "the bit pattern for the constant".
			// Thus we must first construct an integer constant, and bitcast to float.
			return As<SIMD::Float>(SIMD::UInt(constant[i]));
		}

		RValue<SIMD::Int> Int(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->Int(i);
			}

			return SIMD::Int(constant[i]);
		}

		RValue<SIMD::UInt> UInt(uint32_t i) const
		{
			if(intermediate)
			{
				return intermediate->UInt(i);
			}

			return SIMD::UInt(constant[i]);
		}

	private:
		RR_PRINT_ONLY(friend struct rr::PrintValue::Ty<Operand>;)

		// Delegate constructor
		Operand(const EmitState *state, const Object &object);

		const uint32_t *constant;
		const Intermediate *intermediate;

	public:
		const uint32_t componentCount;
	};

	RR_PRINT_ONLY(friend struct rr::PrintValue::Ty<Operand>;)

	Type const &getType(Type::ID id) const
	{
		auto it = types.find(id);
		ASSERT_MSG(it != types.end(), "Unknown type %d", id.value());
		return it->second;
	}

	Type const &getType(const Object &object) const
	{
		return getType(object.typeId());
	}

	Object const &getObject(Object::ID id) const
	{
		auto it = defs.find(id);
		ASSERT_MSG(it != defs.end(), "Unknown object %d", id.value());
		return it->second;
	}

	Type const &getObjectType(Object::ID id) const
	{
		return getType(getObject(id));
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
	//  - DescriptorSet
	//  - Pointer
	//  - InterfaceVariable
	// Calling GetPointerToData with objects of any other kind will assert.
	SIMD::Pointer GetPointerToData(Object::ID id, Int arrayIndex, EmitState const *state) const;

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
	EmitResult EmitImageSample(const ImageInstruction &instruction, EmitState *state) const;
	EmitResult EmitImageQuerySizeLod(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQuerySize(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQueryLevels(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageQuerySamples(InsnIterator insn, EmitState *state) const;
	EmitResult EmitImageRead(const ImageInstruction &instruction, EmitState *state) const;
	EmitResult EmitImageWrite(const ImageInstruction &instruction, EmitState *state) const;
	EmitResult EmitImageTexelPointer(const ImageInstruction &instruction, EmitState *state) const;
	EmitResult EmitAtomicOp(InsnIterator insn, EmitState *state) const;
	EmitResult EmitAtomicCompareExchange(InsnIterator insn, EmitState *state) const;
	EmitResult EmitSampledImageCombineOrSplit(InsnIterator insn, EmitState *state) const;
	EmitResult EmitCopyObject(InsnIterator insn, EmitState *state) const;
	EmitResult EmitCopyMemory(InsnIterator insn, EmitState *state) const;
	EmitResult EmitControlBarrier(InsnIterator insn, EmitState *state) const;
	EmitResult EmitMemoryBarrier(InsnIterator insn, EmitState *state) const;
	EmitResult EmitGroupNonUniform(InsnIterator insn, EmitState *state) const;
	EmitResult EmitArrayLength(InsnIterator insn, EmitState *state) const;

	// Emits code to sample an image, regardless of whether any SIMD lanes are active.
	void EmitImageSampleUnconditional(Array<SIMD::Float> &out, const ImageInstruction &instruction, EmitState *state) const;

	Pointer<Byte> lookupSamplerFunction(Pointer<Byte> imageDescriptor, const ImageInstruction &instruction, EmitState *state) const;
	void callSamplerFunction(Pointer<Byte> samplerFunction, Array<SIMD::Float> &out, Pointer<Byte> imageDescriptor, const ImageInstruction &instruction, EmitState *state) const;

	void GetImageDimensions(EmitState const *state, Type const &resultTy, Object::ID imageId, Object::ID lodId, Intermediate &dst) const;
	static SIMD::Pointer GetTexelAddress(ImageInstructionSignature instruction, Pointer<Byte> descriptor, SIMD::Int coordinate[], SIMD::Int sample, vk::Format imageFormat, OutOfBoundsBehavior outOfBoundsBehavior, const EmitState *state);
	static void WriteImage(ImageInstructionSignature instruction, Pointer<Byte> descriptor, const Pointer<SIMD::Int> &coord, const Pointer<SIMD::Int> &texelAndMask, vk::Format imageFormat);
	uint32_t GetConstScalarInt(Object::ID id) const;
	void EvalSpecConstantOp(InsnIterator insn);
	void EvalSpecConstantUnaryOp(InsnIterator insn);
	void EvalSpecConstantBinaryOp(InsnIterator insn);

	// Fragment input interpolation functions
	uint32_t GetNumInputComponents(int32_t location) const;
	uint32_t GetPackedInterpolant(int32_t location) const;
	enum InterpolationType
	{
		Centroid,
		AtSample,
		AtOffset,
	};
	SIMD::Float Interpolate(SIMD::Pointer const &ptr, int32_t location, Object::ID paramId,
	                        uint32_t component, EmitState *state, InterpolationType type) const;

	// Helper for implementing OpStore, which doesn't take an InsnIterator so it
	// can also store independent operands.
	void Store(Object::ID pointerId, const Operand &value, bool atomic, std::memory_order memoryOrder, EmitState *state) const;

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

	// WriteCFGGraphVizDotFile() writes a graphviz dot file of the shader's
	// control flow to the given file path.
	void WriteCFGGraphVizDotFile(const char *path) const;

	// OpcodeName() returns the name of the opcode op.
	static const char *OpcodeName(spv::Op op);
	static std::memory_order MemoryOrder(spv::MemorySemanticsMask memorySemantics);

	// IsStatement() returns true if the given opcode actually performs
	// work (as opposed to declaring a type, defining a function start / end,
	// etc).
	static bool IsStatement(spv::Op op);

	// HasTypeAndResult() returns true if the given opcode's instruction
	// has a result type ID and result ID, i.e. defines an Object.
	static bool HasTypeAndResult(spv::Op op);

	// Helper as we often need to take dot products as part of doing other things.
	SIMD::Float Dot(unsigned numComponents, Operand const &x, Operand const &y) const;

	// Splits x into a floating-point significand in the range [0.5, 1.0)
	// and an integral exponent of two, such that:
	//   x = significand * 2^exponent
	// Returns the pair <significand, exponent>
	std::pair<SIMD::Float, SIMD::Int> Frexp(RValue<SIMD::Float> val) const;

	static ImageSampler *getImageSampler(const vk::Device *device, uint32_t signature, uint32_t samplerId, uint32_t imageViewId);
	static std::shared_ptr<rr::Routine> emitSamplerRoutine(ImageInstructionSignature instruction, const Sampler &samplerState);
	static std::shared_ptr<rr::Routine> emitWriteRoutine(ImageInstructionSignature instruction, const Sampler &samplerState);

	// TODO(b/129523279): Eliminate conversion and use vk::Sampler members directly.
	static sw::FilterType convertFilterMode(const vk::SamplerState *samplerState, VkImageViewType imageViewType, SamplerMethod samplerMethod);
	static sw::MipmapType convertMipmapMode(const vk::SamplerState *samplerState);
	static sw::AddressingMode convertAddressingMode(int coordinateIndex, const vk::SamplerState *samplerState, VkImageViewType imageViewType);

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

	// Single-entry 'inline' sampler routine cache.
	struct SamplerCache
	{
		Pointer<Byte> imageDescriptor = nullptr;
		Int samplerId;

		Pointer<Byte> function;
	};

	struct InterpolationData
	{
		Pointer<Byte> primitive;
		SIMD::Float x;
		SIMD::Float y;
		SIMD::Float rhw;
		SIMD::Float xCentroid;
		SIMD::Float yCentroid;
		SIMD::Float rhwCentroid;
	};

	vk::PipelineLayout const *const pipelineLayout;

	std::unordered_map<SpirvShader::Object::ID, Variable> variables;
	std::unordered_map<uint32_t, SamplerCache> samplerCache;  // Indexed by the instruction position, in words.
	Variable inputs = Variable{ MAX_INTERFACE_COMPONENTS };
	Variable outputs = Variable{ MAX_INTERFACE_COMPONENTS };
	InterpolationData interpolationData;

	Pointer<Byte> device;
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
	std::array<SIMD::Int, 2> windowSpacePosition;
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

	void createVariable(SpirvShader::Object::ID id, uint32_t componentCount)
	{
		bool added = variables.emplace(id, Variable(componentCount)).second;
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

	static SIMD::Float interpolateAtXY(const SIMD::Float &x, const SIMD::Float &y, const SIMD::Float &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);

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
	// The phis and the profile data are only accessible to SpirvShader
	// as they are only used and exist between calls to
	// SpirvShader::emitProlog() and SpirvShader::emitEpilog().
	friend class SpirvShader;

	std::unordered_map<SpirvShader::Object::ID, Variable> phis;
	std::unique_ptr<SpirvProfileData> profData;
};

}  // namespace sw

#endif  // sw_SpirvShader_hpp
