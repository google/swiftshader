// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef sw_ComputeProgram_hpp
#define sw_ComputeProgram_hpp

#include "SpirvShader.hpp"

#include "Reactor/Reactor.hpp"
#include "Device/Context.hpp"
#include "Vulkan/VkDescriptorSet.hpp"

#include <functional>

namespace vk
{
	class PipelineLayout;
} // namespace vk

namespace sw
{

	using namespace rr;

	class DescriptorSetsLayout;
	struct Constants;

	// ComputeProgram builds a SPIR-V compute shader.
	class ComputeProgram : public Function<Void(
			Pointer<Byte> data,
			Int workgroupX,
			Int workgroupY,
			Int workgroupZ,
			Int firstSubgroup,
			Int subgroupCount)>
	{
	public:
		ComputeProgram(SpirvShader const *spirvShader, vk::PipelineLayout const *pipelineLayout, const vk::DescriptorSet::Bindings &descriptorSets);

		virtual ~ComputeProgram();

		// generate builds the shader program.
		void generate();

		// run executes the compute shader routine for all workgroups.
		// TODO(bclayton): This probably does not belong here. Consider moving.
		static void run(
			Routine *routine, SpirvShader const *shader,
			vk::DescriptorSet::Bindings const &descriptorSetBindings,
			vk::DescriptorSet::DynamicOffsets const &descriptorDynamicOffsets,
			PushConstantStorage const &pushConstants,
			uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	protected:
		void emit();

		void setWorkgroupBuiltins(Int workgroupID[3]);
		void setSubgroupBuiltins(Int workgroupID[3], SIMD::Int localInvocationIndex, Int subgroupIndex);
		void setInputBuiltin(spv::BuiltIn id, std::function<void(const SpirvShader::BuiltinMapping& builtin, Array<SIMD::Float>& value)> cb);

		Pointer<Byte> data; // argument 0

		struct Data
		{
			vk::DescriptorSet::Bindings descriptorSets;
			vk::DescriptorSet::DynamicOffsets descriptorDynamicOffsets;
			uint4 numWorkgroups; // [x, y, z, 0]
			uint4 workgroupSize; // [x, y, z, 0]
			uint32_t invocationsPerSubgroup; // SPIR-V: "SubgroupSize"
			uint32_t subgroupsPerWorkgroup; // SPIR-V: "NumSubgroups"
			uint32_t invocationsPerWorkgroup; // Total number of invocations per workgroup.
			PushConstantStorage pushConstants;
			const Constants *constants;
			uint8_t* workgroupMemory;
		};

		SpirvRoutine routine;
		SpirvShader const * const shader;
		vk::PipelineLayout const * const pipelineLayout;
		const vk::DescriptorSet::Bindings &descriptorSets;
	};

} // namespace sw

#endif   // sw_ComputeProgram_hpp
