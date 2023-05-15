// Copyright (c) 2017-2022 Valve Corporation
// Copyright (c) 2017-2022 LunarG Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Bindless Check Instrumentation Tests.

#include <string>
#include <vector>

#include "test/opt/pass_fixture.h"
#include "test/opt/pass_utils.h"

namespace spvtools {
namespace opt {
namespace {

using InstBindlessTest = PassTest<::testing::Test>;

static const std::string kOutputDecorations = R"(
; CHECK: OpDecorate [[output_buffer_type:%inst_bindless_OutputBuffer]] Block
; CHECK: OpMemberDecorate [[output_buffer_type]] 0 Offset 0
; CHECK: OpMemberDecorate [[output_buffer_type]] 1 Offset 4
; CHECK: OpDecorate [[output_buffer_var:%\w+]] DescriptorSet 7
; CHECK: OpDecorate [[output_buffer_var]] Binding 0
)";

static const std::string kOutputGlobals = R"(
; CHECK: [[output_buffer_type]] = OpTypeStruct %uint %uint %_runtimearr_uint
; CHECK: [[output_ptr_type:%\w+]] = OpTypePointer StorageBuffer [[output_buffer_type]]
; CHECK: [[output_buffer_var]] = OpVariable [[output_ptr_type]] StorageBuffer
)";

static const std::string kStreamWrite6Begin = R"(
; CHECK: %inst_bindless_stream_write_6 = OpFunction %void None {{%\w+}}
; CHECK: [[param_1:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_2:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_3:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_4:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_5:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_6:%\w+]] = OpFunctionParameter %uint
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_1
; CHECK: {{%\w+}} = OpAtomicIAdd %uint {{%\w+}} %uint_4 %uint_0 %uint_12
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_12
; CHECK: {{%\w+}} = OpArrayLength %uint [[output_buffer_var]] 2
; CHECK: {{%\w+}} = OpULessThanEqual %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_12
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_1
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_23
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_2
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_1]]
)";

static const std::string kStreamWrite6End = R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_7
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_2]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_8
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_3]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_9
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_4]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_10
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_5]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_11
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_6]]
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturn
; CHECK: OpFunctionEnd
)";

// clang-format off
static const std::string kStreamWrite6Frag = kStreamWrite6Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpLoad %v4float %gl_FragCoord
; CHECK: {{%\w+}} = OpBitcast %v4uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite6End;

static const std::string kStreamWrite6Tese = kStreamWrite6Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_2
; CHECK: {{%\w+}} = OpLoad %uint %gl_PrimitiveID
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %v3float %gl_TessCoord
; CHECK: {{%\w+}} = OpBitcast %v3uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_6
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite6End;

static const std::string kStreamWrite6Vert = kStreamWrite6Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpLoad %uint %gl_VertexIndex
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %uint %gl_InstanceIndex
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite6End;

static const std::string kStreamWrite6Compute = kStreamWrite6Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpLoad %v3uint %gl_GlobalInvocationID
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 2
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_6
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite6End;

static const std::string kStreamWrite6Ray = kStreamWrite6Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %v3uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 2
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_6
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite6End;
// clang-format on

static const std::string kStreamWrite7Begin = R"(
; CHECK: %inst_bindless_stream_write_7 = OpFunction %void None {{%\w+}}
; CHECK: [[param_1:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_2:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_3:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_4:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_5:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_6:%\w+]] = OpFunctionParameter %uint
; CHECK: [[param_7:%\w+]] = OpFunctionParameter %uint
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_1
; CHECK: {{%\w+}} = OpAtomicIAdd %uint {{%\w+}} %uint_4 %uint_0 %uint_13
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_13
; CHECK: {{%\w+}} = OpArrayLength %uint [[output_buffer_var]] 2
; CHECK: {{%\w+}} = OpULessThanEqual %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_13
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_1
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_23
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_2
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_1]]
)";

static const std::string kStreamWrite7End = R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_7
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_2]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_8
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_3]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_9
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_4]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_10
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_5]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_11
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_6]]
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_12
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} [[param_7]]
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturn
; CHECK: OpFunctionEnd
)";

// clang-format off
static const std::string kStreamWrite7Frag = kStreamWrite7Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpLoad %v4float %gl_FragCoord
; CHECK: {{%\w+}} = OpBitcast %v4uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite7End;

static const std::string kStreamWrite7Vert = kStreamWrite7Begin + R"(
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpLoad %uint %gl_VertexIndex
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %uint %gl_InstanceIndex
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_5
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer_uint [[output_buffer_var]] %uint_2 {{%\w+}}
; CHECK: OpStore {{%\w+}} {{%\w+}}
)" + kStreamWrite7End;
// clang-format on

static const std::string kInputDecorations = R"(
; CHECK: OpDecorate [[desc_set_struct:%inst_bindless_DescriptorSetData]] Block
; CHECK: OpMemberDecorate [[desc_set_struct]] 0 Offset 0
; CHECK: OpMemberDecorate [[desc_set_struct]] 1 Offset 4
; CHECK: OpDecorate [[input_buffer_type:%inst_bindless_InputBuffer]] Block
; CHECK: OpMemberDecorate [[input_buffer_type]] 0 Offset 0
; CHECK: OpDecorate [[input_buffer_var:%\w+]] DescriptorSet 7
; CHECK: OpDecorate [[input_buffer_var]] Binding 1
)";

static const std::string kInputGlobals = R"(
; CHECK: [[desc_set_struct]] = OpTypeStruct %uint %_runtimearr_uint
; CHECK: [[desc_set_ptr:%\w+]] = OpTypePointer PhysicalStorageBuffer [[desc_set_struct]]
; CHECK: [[desc_set_ptr_array:%\w+]] = OpTypeArray [[desc_set_ptr]] %uint_32
; CHECK: [[input_buffer_type]] = OpTypeStruct [[desc_set_ptr_array]]
; CHECK: [[input_ptr_type:%\w+]] = OpTypePointer StorageBuffer [[input_buffer_type]]
; CHECK: [[input_buffer_var]] = OpVariable [[input_ptr_type]] StorageBuffer
)";

static const std::string kReadBindingLength = R"(
; CHECK: %inst_bindless_read_binding_length = OpFunction %uint None {{%\w+}}
; CHECK: [[bl_desc_set_idx:%\w+]] = OpFunctionParameter %uint
; CHECK: [[bl_binding_idx:%\w+]] = OpFunctionParameter %uint
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpUGreaterThanEqual %bool [[bl_desc_set_idx]] %uint_32
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer__ptr_PhysicalStorageBuffer_inst_bindless_DescriptorSetData %inst_bindless_input_buffer %uint_0 [[bl_desc_set_idx]]
; CHECK: {{%\w+}} = OpLoad %_ptr_PhysicalStorageBuffer_inst_bindless_DescriptorSetData {{%\w+}}
; CHECK: {{%\w+}} = OpBitcast %v2uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpIEqual %bool {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpIEqual %bool {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpLogicalAnd %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_PhysicalStorageBuffer_uint {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpLoad %uint {{%\w+}} Aligned 8
; CHECK: {{%\w+}} = OpUGreaterThanEqual %bool [[bl_binding_idx]] {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_PhysicalStorageBuffer_uint {{%\w+}} %uint_1 [[bl_binding_idx]]
; CHECK: {{%\w+}} = OpLoad %uint {{%\w+}} Aligned 4
; CHECK: OpReturnValue {{%\w+}}
; CHECK: OpFunctionEnd
)";

static const std::string kReadDescInit = R"(
; CHECK: %inst_bindless_read_desc_init = OpFunction %uint None {{%\w+}}
; CHECK: [[di_desc_set_idx:%\w+]] = OpFunctionParameter %uint
; CHECK: [[di_binding_idx:%\w+]] = OpFunctionParameter %uint
; CHECK: [[di_desc_idx:%\w+]] = OpFunctionParameter %uint
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpUGreaterThanEqual %bool [[di_desc_set_idx]] %uint_32
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_StorageBuffer__ptr_PhysicalStorageBuffer_inst_bindless_DescriptorSetData %inst_bindless_input_buffer %uint_0 [[di_desc_set_idx]]
; CHECK: {{%\w+}} = OpLoad %_ptr_PhysicalStorageBuffer_inst_bindless_DescriptorSetData {{%\w+}}
; CHECK: {{%\w+}} = OpBitcast %v2uint {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 0
; CHECK: {{%\w+}} = OpIEqual %bool {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpCompositeExtract %uint {{%\w+}} 1
; CHECK: {{%\w+}} = OpIEqual %bool {{%\w+}} %uint_0
; CHECK: {{%\w+}} = OpLogicalAnd %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpAccessChain %_ptr_PhysicalStorageBuffer_uint {{%\w+}} %uint_0
; CHECK: [[di_num_bindings:%\w+]] = OpLoad %uint {{%\w+}} Aligned 8
; CHECK: {{%\w+}} = OpUGreaterThanEqual %bool [[di_binding_idx]] [[di_num_bindings]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpReturnValue %uint_0
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpAccessChain %_ptr_PhysicalStorageBuffer_uint {{%\w+}} %uint_1 {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %uint {{%\w+}} Aligned 4
; CHECK: {{%\w+}} = OpAccessChain %_ptr_PhysicalStorageBuffer_uint {{%\w+}} %uint_1 {{%\w+}}
; CHECK: {{%\w+}} = OpLoad %uint {{%\w+}} Aligned 4
; CHECK: OpReturnValue {{%\w+}}
; CHECK: OpFunctionEnd
)";

TEST_F(InstBindlessTest, NoInstrumentConstIndexInbounds) {
  // Texture2D g_tColor[128];
  //
  // SamplerState g_sAniso;
  //
  // struct PS_INPUT
  // {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT
  // {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i)
  // {
  //   PS_OUTPUT ps_output;
  //
  //   ps_output.vColor = g_tColor[ 37 ].Sample(g_sAniso, i.vTextureCoords.xy);
  //   return ps_output;
  // }

  const std::string before =
      R"(OpCapability Shader
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 3
OpDecorate %g_tColor Binding 5
OpDecorate %g_sAniso DescriptorSet 3
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
%void = OpTypeVoid
%8 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%int_37 = OpConstant %int 37
%15 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%_arr_15_uint_128 = OpTypeArray %15 %uint_128
%_ptr_UniformConstant__arr_15_uint_128 = OpTypePointer UniformConstant %_arr_15_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_15_uint_128 UniformConstant
%_ptr_UniformConstant_15 = OpTypePointer UniformConstant %15
%21 = OpTypeSampler
%_ptr_UniformConstant_21 = OpTypePointer UniformConstant %21
%g_sAniso = OpVariable %_ptr_UniformConstant_21 UniformConstant
%23 = OpTypeSampledImage %15
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
%MainPs = OpFunction %void None %8
%26 = OpLabel
%27 = OpLoad %v2float %i_vTextureCoords
%28 = OpAccessChain %_ptr_UniformConstant_15 %g_tColor %int_37
%29 = OpLoad %15 %28
%30 = OpLoad %21 %g_sAniso
%31 = OpSampledImage %23 %29 %30
%32 = OpImageSampleImplicitLod %v4float %31 %27
OpStore %_entryPointOutput_vColor %32
OpReturn
OpFunctionEnd
)";

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndCheck<InstBindlessCheckPass>(
      before, before, true, true, 7u, 23u, false, false, false, false, false);
}

TEST_F(InstBindlessTest, NoInstrumentNonBindless) {
  // This test verifies that the pass will correctly not instrument vanilla
  // texture sample.
  //
  // Texture2D g_tColor;
  //
  // SamplerState g_sAniso;
  //
  // struct PS_INPUT
  // {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT
  // {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i)
  // {
  //   PS_OUTPUT ps_output;
  //   ps_output.vColor =
  //       g_tColor.Sample(g_sAniso, i.vTextureCoords.xy);
  //   return ps_output;
  // }

  const std::string whole_file =
      R"(OpCapability Shader
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 6
OpDecorate %g_tColor Binding 4
OpDecorate %g_sAniso DescriptorSet 6
OpDecorate %g_sAniso Binding 4
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
%void = OpTypeVoid
%8 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%12 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_12 = OpTypePointer UniformConstant %12
%g_tColor = OpVariable %_ptr_UniformConstant_12 UniformConstant
%14 = OpTypeSampler
%_ptr_UniformConstant_14 = OpTypePointer UniformConstant %14
%g_sAniso = OpVariable %_ptr_UniformConstant_14 UniformConstant
%16 = OpTypeSampledImage %12
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
%MainPs = OpFunction %void None %8
%19 = OpLabel
%20 = OpLoad %v2float %i_vTextureCoords
%21 = OpLoad %12 %g_tColor
%22 = OpLoad %14 %g_sAniso
%23 = OpSampledImage %16 %21 %22
%24 = OpImageSampleImplicitLod %v4float %23 %20
OpStore %_entryPointOutput_vColor %24
OpReturn
OpFunctionEnd
)";

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndCheck<InstBindlessCheckPass>(whole_file, whole_file, true,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, Simple) {
  // Texture2D g_tColor[128];
  //
  // layout(push_constant) cbuffer PerViewConstantBuffer_t
  // {
  //   uint g_nDataIdx;
  // };
  //
  // SamplerState g_sAniso;
  //
  // struct PS_INPUT
  // {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT
  // {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i)
  // {
  //   PS_OUTPUT ps_output;
  //   ps_output.vColor =
  //       g_tColor[ g_nDataIdx ].Sample(g_sAniso, i.vTextureCoords.xy);
  //   return ps_output;
  // }

  const std::string entry = R"(
OpCapability Shader
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
)";

  // clang-format off
  const std::string names_annots = R"(
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 3
OpDecorate %g_tColor Binding 0
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %g_sAniso DescriptorSet 0
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
)";

  const std::string consts_types_vars = R"(
%void = OpTypeVoid
%10 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%16 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%_arr_16_uint_128 = OpTypeArray %16 %uint_128
%_ptr_UniformConstant__arr_16_uint_128 = OpTypePointer UniformConstant %_arr_16_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_16_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_16 = OpTypePointer UniformConstant %16
%24 = OpTypeSampler
%_ptr_UniformConstant_24 = OpTypePointer UniformConstant %24
%g_sAniso = OpVariable %_ptr_UniformConstant_24 UniformConstant
%26 = OpTypeSampledImage %16
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
; CHECK: %bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %10
%29 = OpLabel
%30 = OpLoad %v2float %i_vTextureCoords
%31 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%32 = OpLoad %uint %31
%33 = OpAccessChain %_ptr_UniformConstant_16 %g_tColor %32
%34 = OpLoad %16 %33
%35 = OpLoad %24 %g_sAniso
%36 = OpSampledImage %26 %34 %35
%37 = OpImageSampleImplicitLod %v4float %36 %30
OpStore %_entryPointOutput_vColor %37
; CHECK-NOT: %37 = OpImageSampleImplicitLod %v4float %36 %30
; CHECK-NOT: OpStore %_entryPointOutput_vColor %37
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} %uint_128
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %16 %33
; CHECK: {{%\w+}} = OpSampledImage %26 {{%\w+}} %35
; CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_56 %uint_0 %uint_3 %uint_0 %32 %uint_128
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpStore %_entryPointOutput_vColor [[phi_result]]
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass, uint32_t, uint32_t, bool, bool>(
      entry + names_annots + consts_types_vars + main_func + output_func, true,
      7u, 23u, false, false, false, false, false);
}

TEST_F(InstBindlessTest, InstrumentMultipleInstructions) {
  // Texture2D g_tColor[128];
  //
  // layout(push_constant) cbuffer PerViewConstantBuffer_t
  // {
  //   uint g_nDataIdx;
  //   uint g_nDataIdx2;
  // };
  //
  // SamplerState g_sAniso;
  //
  // struct PS_INPUT
  // {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT
  // {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i)
  // {
  //   PS_OUTPUT ps_output;
  //
  //   float t  = g_tColor[g_nDataIdx ].Sample(g_sAniso, i.vTextureCoords.xy);
  //   float t2 = g_tColor[g_nDataIdx2].Sample(g_sAniso, i.vTextureCoords.xy);
  //   ps_output.vColor = t + t2;
  //   return ps_output;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 3
OpDecorate %g_tColor Binding 4
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpMemberDecorate %PerViewConstantBuffer_t 1 Offset 4
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %g_sAniso DescriptorSet 3
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%10 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%int_1 = OpConstant %int 1
%17 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%_arr_17_uint_128 = OpTypeArray %17 %uint_128
%_ptr_UniformConstant__arr_17_uint_128 = OpTypePointer UniformConstant %_arr_17_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_17_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_17 = OpTypePointer UniformConstant %17
%25 = OpTypeSampler
%_ptr_UniformConstant_25 = OpTypePointer UniformConstant %25
%g_sAniso = OpVariable %_ptr_UniformConstant_25 UniformConstant
%27 = OpTypeSampledImage %17
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
; CHECK: %bool = OpTypeBool
; CHECK: {{%\w+}} = OpTypeFunction %void %uint %uint %uint %uint
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func =
      R"(%MainPs = OpFunction %void None %10
%30 = OpLabel
%31 = OpLoad %v2float %i_vTextureCoords
%32 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%33 = OpLoad %uint %32
%34 = OpAccessChain %_ptr_UniformConstant_17 %g_tColor %33
%35 = OpLoad %17 %34
%36 = OpLoad %25 %g_sAniso
%37 = OpSampledImage %27 %35 %36
%38 = OpImageSampleImplicitLod %v4float %37 %31
; CHECK-NOT: %38 = OpImageSampleImplicitLod %v4float %37 %31
; CHECK: %48 = OpULessThan %bool %33 %uint_128
; CHECK: OpSelectionMerge %49 None
; CHECK: OpBranchConditional %48 %50 %51
; CHECK: %50 = OpLabel
; CHECK: %52 = OpLoad %17 %34
; CHECK: %53 = OpSampledImage %27 %52 %36
; CHECK: %54 = OpImageSampleImplicitLod %v4float %53 %31
; CHECK: OpBranch %49
; CHECK: %51 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_58 %uint_0 %uint_3 %uint_4 %33 %uint_128
; CHECK: OpBranch %49
; CHECK: %49 = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float %54 %50 [[null_v4float]] %51
%39 = OpAccessChain %_ptr_PushConstant_uint %_ %int_1
%40 = OpLoad %uint %39
%41 = OpAccessChain %_ptr_UniformConstant_17 %g_tColor %40
%42 = OpLoad %17 %41
%43 = OpSampledImage %27 %42 %36
%44 = OpImageSampleImplicitLod %v4float %43 %31
%45 = OpFAdd %v4float %38 %44
; CHECK-NOT: %44 = OpImageSampleImplicitLod %v4float %43 %31
; CHECK-NOT: %45 = OpFAdd %v4float %38 %44
; CHECK: {{%\w+}} = OpULessThan %bool %40 %uint_128
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %17 %41
; CHECK: {{%\w+}} = OpSampledImage %27 {{%\w+}} %36
; CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %31
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_64 %uint_0 %uint_3 %uint_4 %40 %uint_128
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %45 = OpFAdd %v4float {{%\w+}} {{%\w+}}
OpStore %_entryPointOutput_vColor %45
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstrumentOpImage) {
  // This test verifies that the pass will correctly instrument shader
  // using OpImage. This test was created by editing the SPIR-V
  // from the Simple test.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability StorageImageReadWithoutFormat
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 3
OpDecorate %g_tColor Binding 9
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%v2int = OpTypeVector %int 2
%int_0 = OpConstant %int 0
%20 = OpTypeImage %float 2D 0 0 0 0 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%39 = OpTypeSampledImage %20
%_arr_39_uint_128 = OpTypeArray %39 %uint_128
%_ptr_UniformConstant__arr_39_uint_128 = OpTypePointer UniformConstant %_arr_39_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_39_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_39 = OpTypePointer UniformConstant %39
%_ptr_Input_v2int = OpTypePointer Input %v2int
%i_vTextureCoords = OpVariable %_ptr_Input_v2int Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
; CHECK: uint_0 = OpConstant %uint 0
; CHECK: bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %3
%5 = OpLabel
%53 = OpLoad %v2int %i_vTextureCoords
%63 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%64 = OpLoad %uint %63
%65 = OpAccessChain %_ptr_UniformConstant_39 %g_tColor %64
%66 = OpLoad %39 %65
%75 = OpImage %20 %66
%71 = OpImageRead %v4float %75 %53
OpStore %_entryPointOutput_vColor %71
; CHECK-NOT: %71 = OpImageRead %v4float %75 %53
; CHECK-NOT: OpStore %_entryPointOutput_vColor %71
; CHECK: %78 = OpULessThan %bool %64 %uint_128
; CHECK: OpSelectionMerge %79 None
; CHECK: OpBranchConditional %78 %80 %81
; CHECK: %80 = OpLabel
; CHECK: %82 = OpLoad %39 %65
; CHECK: %83 = OpImage %20 %82
; CHECK: %84 = OpImageRead %v4float %83 %53
; CHECK: OpBranch %79
; CHECK: %81 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_3 %uint_9 %64 %uint_128
; CHECK: OpBranch %79
; CHECK: %79 = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float %84 %80 [[null_v4float]] %81
; CHECK: OpStore %_entryPointOutput_vColor {{%\w+}}
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstrumentSampledImage) {
  // This test verifies that the pass will correctly instrument shader
  // using sampled image. This test was created by editing the SPIR-V
  // from the Simple test.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 4
OpDecorate %g_tColor Binding 11
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%20 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%39 = OpTypeSampledImage %20
%_arr_39_uint_128 = OpTypeArray %39 %uint_128
%_ptr_UniformConstant__arr_39_uint_128 = OpTypePointer UniformConstant %_arr_39_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_39_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_39 = OpTypePointer UniformConstant %39
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
; CHECK: uint_0 = OpConstant %uint 0
; CHECK: bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %3
%5 = OpLabel
%53 = OpLoad %v2float %i_vTextureCoords
%63 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%64 = OpLoad %uint %63
%65 = OpAccessChain %_ptr_UniformConstant_39 %g_tColor %64
%66 = OpLoad %39 %65
%71 = OpImageSampleImplicitLod %v4float %66 %53
OpStore %_entryPointOutput_vColor %71
; CHECK-NOT: %71 = OpImageSampleImplicitLod %v4float %66 %53
; CHECK-NOT: OpStore %_entryPointOutput_vColor %71
; CHECK: %74 = OpULessThan %bool %64 %uint_128
; CHECK: OpSelectionMerge %75 None
; CHECK: OpBranchConditional %74 %76 %77
; CHECK: %76 = OpLabel
; CHECK: %78 = OpLoad %39 %65
; CHECK: %79 = OpImageSampleImplicitLod %v4float %78 %53
; CHECK: OpBranch %75
; CHECK: %77 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_49 %uint_0 %uint_4 %uint_11 %64 %uint_128
; CHECK: OpBranch %75
; CHECK: %75 = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float %79 %76 [[null_v4float]] %77
; CHECK: OpStore %_entryPointOutput_vColor {{%\w+}}
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstrumentImageWrite) {
  // This test verifies that the pass will correctly instrument shader
  // doing bindless image write. This test was created by editing the SPIR-V
  // from the Simple test.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability StorageImageWriteWithoutFormat
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 30
OpDecorate %g_tColor Binding 2
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%v2int = OpTypeVector %int 2
%int_0 = OpConstant %int 0
%20 = OpTypeImage %float 2D 0 0 0 0 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%80 = OpConstantNull %v4float
%_arr_20_uint_128 = OpTypeArray %20 %uint_128
%_ptr_UniformConstant__arr_20_uint_128 = OpTypePointer UniformConstant %_arr_20_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_20_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_20 = OpTypePointer UniformConstant %20
%_ptr_Input_v2int = OpTypePointer Input %v2int
%i_vTextureCoords = OpVariable %_ptr_Input_v2int Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %3
%5 = OpLabel
%53 = OpLoad %v2int %i_vTextureCoords
%63 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%64 = OpLoad %uint %63
%65 = OpAccessChain %_ptr_UniformConstant_20 %g_tColor %64
%66 = OpLoad %20 %65
OpImageWrite %66 %53 %80
OpStore %_entryPointOutput_vColor %80
; CHECK-NOT: OpImageWrite %66 %53 %80
; CHECK-NOT: OpStore %_entryPointOutput_vColor %80
; CHECK: %35 = OpULessThan %bool %30 %uint_128
; CHECK: OpSelectionMerge %36 None
; CHECK: OpBranchConditional %35 %37 %38
; CHECK: %37 = OpLabel
; CHECK: %39 = OpLoad %16 %31
; CHECK: OpImageWrite %39 %28 %19
; CHECK: OpBranch %36
; CHECK: %38 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_30 %uint_2 %30 %uint_128
; CHECK: OpBranch %36
; CHECK: %36 = OpLabel
; CHECK: OpStore %_entryPointOutput_vColor %19
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstrumentVertexSimple) {
  // This test verifies that the pass will correctly instrument shader
  // doing bindless image write. This test was created by editing the SPIR-V
  // from the Simple test.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability Sampled1D
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Vertex %main "main" %_ %coords2D
OpSource GLSL 450
OpName %main "main"
OpName %lod "lod"
OpName %coords1D "coords1D"
OpName %gl_PerVertex "gl_PerVertex"
OpMemberName %gl_PerVertex 0 "gl_Position"
OpMemberName %gl_PerVertex 1 "gl_PointSize"
OpMemberName %gl_PerVertex 2 "gl_ClipDistance"
OpMemberName %gl_PerVertex 3 "gl_CullDistance"
OpName %_ ""
OpName %texSampler1D "texSampler1D"
OpName %foo "foo"
OpMemberName %foo 0 "g_idx"
OpName %__0 ""
OpName %coords2D "coords2D"
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_VertexIndex BuiltIn VertexIndex
; CHECK: OpDecorate %gl_InstanceIndex BuiltIn InstanceIndex
OpMemberDecorate %gl_PerVertex 0 BuiltIn Position
OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize
OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance
OpMemberDecorate %gl_PerVertex 3 BuiltIn CullDistance
OpDecorate %gl_PerVertex Block
OpDecorate %texSampler1D DescriptorSet 2
OpDecorate %texSampler1D Binding 13
OpMemberDecorate %foo 0 Offset 0
OpDecorate %foo Block
OpDecorate %__0 DescriptorSet 7
OpDecorate %__0 Binding 5
OpDecorate %coords2D Location 0
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Function_float = OpTypePointer Function %float
%float_3 = OpConstant %float 3
%float_1_78900003 = OpConstant %float 1.78900003
%v4float = OpTypeVector %float 4
%uint = OpTypeInt 32 0
%uint_1 = OpConstant %uint 1
%_arr_float_uint_1 = OpTypeArray %float %uint_1
%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex
%_ = OpVariable %_ptr_Output_gl_PerVertex Output
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%21 = OpTypeImage %float 1D 0 0 0 1 Unknown
%22 = OpTypeSampledImage %21
%uint_128 = OpConstant %uint 128
%_arr_22_uint_128 = OpTypeArray %22 %uint_128
%_ptr_UniformConstant__arr_22_uint_128 = OpTypePointer UniformConstant %_arr_22_uint_128
%texSampler1D = OpVariable %_ptr_UniformConstant__arr_22_uint_128 UniformConstant
%foo = OpTypeStruct %int
%_ptr_Uniform_foo = OpTypePointer Uniform %foo
%__0 = OpVariable %_ptr_Uniform_foo Uniform
%_ptr_Uniform_int = OpTypePointer Uniform %int
%_ptr_UniformConstant_22 = OpTypePointer UniformConstant %22
%_ptr_Output_v4float = OpTypePointer Output %v4float
%v2float = OpTypeVector %float 2
%_ptr_Input_v2float = OpTypePointer Input %v2float
%coords2D = OpVariable %_ptr_Input_v2float Input
; CHECK: %bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_uint = OpTypePointer Input %uint
; CHECK: %gl_VertexIndex = OpVariable %_ptr_Input_uint Input
; CHECK: %gl_InstanceIndex = OpVariable %_ptr_Input_uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%lod = OpVariable %_ptr_Function_float Function
%coords1D = OpVariable %_ptr_Function_float Function
OpStore %lod %float_3
OpStore %coords1D %float_1_78900003
%31 = OpAccessChain %_ptr_Uniform_int %__0 %int_0
%32 = OpLoad %int %31
%34 = OpAccessChain %_ptr_UniformConstant_22 %texSampler1D %32
%35 = OpLoad %22 %34
%36 = OpLoad %float %coords1D
%37 = OpLoad %float %lod
%38 = OpImageSampleExplicitLod %v4float %35 %36 Lod %37
%40 = OpAccessChain %_ptr_Output_v4float %_ %int_0
OpStore %40 %38
; CHECK-NOT: %38 = OpImageSampleExplicitLod %v4float %35 %36 Lod %37
; CHECK-NOT: %40 = OpAccessChain %_ptr_Output_v4float %_ %int_0
; CHECK-NOT: OpStore %40 %38
; CHECK: %46 = OpULessThan %bool %37 %uint_128
; CHECK: OpSelectionMerge %47 None
; CHECK: OpBranchConditional %46 %48 %49
; CHECK: %48 = OpLabel
; CHECK: %50 = OpLoad %25 %38
; CHECK: %51 = OpImageSampleExplicitLod %v4float %50 %40 Lod %41
; CHECK: OpBranch %47
; CHECK: %49 = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %37
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_74 %uint_0 %uint_2 %uint_13 {{%\w+}} %uint_128
; CHECK: OpBranch %47
; CHECK: %47 = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float %51 %48 [[null_v4float]] %49
; CHECK: %43 = OpAccessChain %_ptr_Output_v4float %_ %int_0
; CHECK: OpStore %43 {{%\w+}}
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Vert;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstrumentTeseSimple) {
  // This test verifies that the pass will correctly instrument tessellation
  // evaluation shader doing bindless buffer load.
  //
  // clang-format off
  //
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(std140, set = 9, binding = 1) uniform ufoo { uint index; } uniform_index_buffer;
  //
  // layout(set = 9, binding = 2) buffer bfoo { vec4 val; } adds[11];
  //
  // layout(triangles, equal_spacing, cw) in;
  //
  // void main() {
  //   gl_Position = adds[uniform_index_buffer.index].val;
  // }
  //

  const std::string defs = R"(
OpCapability Tessellation
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint TessellationEvaluation %main "main" %_
; CHECK: OpEntryPoint TessellationEvaluation %main "main" %_ %gl_PrimitiveID %gl_TessCoord
OpExecutionMode %main Triangles
OpExecutionMode %main SpacingEqual
OpExecutionMode %main VertexOrderCw
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %gl_PerVertex "gl_PerVertex"
OpMemberName %gl_PerVertex 0 "gl_Position"
OpMemberName %gl_PerVertex 1 "gl_PointSize"
OpMemberName %gl_PerVertex 2 "gl_ClipDistance"
OpMemberName %gl_PerVertex 3 "gl_CullDistance"
OpName %_ ""
OpName %bfoo "bfoo"
OpMemberName %bfoo 0 "val"
OpName %adds "adds"
OpName %ufoo "ufoo"
OpMemberName %ufoo 0 "index"
OpName %uniform_index_buffer "uniform_index_buffer"
OpMemberDecorate %gl_PerVertex 0 BuiltIn Position
OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize
OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance
OpMemberDecorate %gl_PerVertex 3 BuiltIn CullDistance
OpDecorate %gl_PerVertex Block
OpMemberDecorate %bfoo 0 Offset 0
OpDecorate %bfoo Block
OpDecorate %adds DescriptorSet 9
OpDecorate %adds Binding 1
OpMemberDecorate %ufoo 0 Offset 0
OpDecorate %ufoo Block
OpDecorate %uniform_index_buffer DescriptorSet 9
OpDecorate %uniform_index_buffer Binding 2
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_PrimitiveID BuiltIn PrimitiveId
; CHECK: OpDecorate %gl_TessCoord BuiltIn TessCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v4float = OpTypeVector %float 4
%uint = OpTypeInt 32 0
%uint_1 = OpConstant %uint 1
%_arr_float_uint_1 = OpTypeArray %float %uint_1
%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex
%_ = OpVariable %_ptr_Output_gl_PerVertex Output
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%bfoo = OpTypeStruct %v4float
%uint_11 = OpConstant %uint 11
%_arr_bfoo_uint_11 = OpTypeArray %bfoo %uint_11
%_ptr_StorageBuffer__arr_bfoo_uint_11 = OpTypePointer StorageBuffer %_arr_bfoo_uint_11
%adds = OpVariable %_ptr_StorageBuffer__arr_bfoo_uint_11 StorageBuffer
%ufoo = OpTypeStruct %uint
%_ptr_Uniform_ufoo = OpTypePointer Uniform %ufoo
%uniform_index_buffer = OpVariable %_ptr_Uniform_ufoo Uniform
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_StorageBuffer_v4float = OpTypePointer StorageBuffer %v4float
%_ptr_Output_v4float = OpTypePointer Output %v4float
; CHECK: %bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_uint = OpTypePointer Input %uint
; CHECK: %gl_PrimitiveID = OpVariable %_ptr_Input_uint Input
; CHECK: %v3float = OpTypeVector %float 3
; CHECK: %_ptr_Input_v3float = OpTypePointer Input %v3float
; CHECK: %gl_TessCoord = OpVariable %_ptr_Input_v3float Input
; CHECK: %v3uint = OpTypeVector %uint 3
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%25 = OpAccessChain %_ptr_Uniform_uint %uniform_index_buffer %int_0
%26 = OpLoad %uint %25
%28 = OpAccessChain %_ptr_StorageBuffer_v4float %adds %26 %int_0
%29 = OpLoad %v4float %28
; CHECK-NOT: %29 = OpLoad %v4float %28
; CHECK: %34 = OpULessThan %bool %28 %uint_11
; CHECK: OpSelectionMerge %35 None
; CHECK: OpBranchConditional %34 %36 %37
; CHECK: %36 = OpLabel
; CHECK: %38 = OpLoad %v4float %29
; CHECK: OpBranch %35
; CHECK: %37 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_63 %uint_0 %uint_9 %uint_1 %28 %uint_11
; CHECK: OpBranch %35
; CHECK: %35 = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float %38 %36 [[null_v4float]] %37
%31 = OpAccessChain %_ptr_Output_v4float %_ %int_0
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: OpStore %31 {{%\w+}}
OpReturn
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Tese;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + output_func,
                                               true, 7u, 23u, false, false,
                                               false, false, false);
}

TEST_F(InstBindlessTest, MultipleDebugFunctions) {
  // Same source as Simple, but compiled -g and not optimized, especially not
  // inlined. The OpSource has had the source extracted for the sake of brevity.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%2 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
%1 = OpString "foo5.frag"
OpSource HLSL 500 %1
OpName %MainPs "MainPs"
OpName %PS_INPUT "PS_INPUT"
OpMemberName %PS_INPUT 0 "vTextureCoords"
OpName %PS_OUTPUT "PS_OUTPUT"
OpMemberName %PS_OUTPUT 0 "vColor"
OpName %_MainPs_struct_PS_INPUT_vf21_ "@MainPs(struct-PS_INPUT-vf21;"
OpName %i "i"
OpName %ps_output "ps_output"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %g_sAniso "g_sAniso"
OpName %i_0 "i"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpName %param "param"
OpDecorate %g_tColor DescriptorSet 1
OpDecorate %g_tColor Binding 2
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %g_sAniso DescriptorSet 1
OpDecorate %g_sAniso Binding 3
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%4 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%PS_INPUT = OpTypeStruct %v2float
%_ptr_Function_PS_INPUT = OpTypePointer Function %PS_INPUT
%v4float = OpTypeVector %float 4
%PS_OUTPUT = OpTypeStruct %v4float
%13 = OpTypeFunction %PS_OUTPUT %_ptr_Function_PS_INPUT
%_ptr_Function_PS_OUTPUT = OpTypePointer Function %PS_OUTPUT
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%21 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%_arr_21_uint_128 = OpTypeArray %21 %uint_128
%_ptr_UniformConstant__arr_21_uint_128 = OpTypePointer UniformConstant %_arr_21_uint_128
%g_tColor = OpVariable %_ptr_UniformConstant__arr_21_uint_128 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_21 = OpTypePointer UniformConstant %21
%36 = OpTypeSampler
%_ptr_UniformConstant_36 = OpTypePointer UniformConstant %36
%g_sAniso = OpVariable %_ptr_UniformConstant_36 UniformConstant
%40 = OpTypeSampledImage %21
%_ptr_Function_v2float = OpTypePointer Function %v2float
%_ptr_Function_v4float = OpTypePointer Function %v4float
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
; CHECK: %bool = OpTypeBool
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
; CHECK: %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string func1 = R"(
%MainPs = OpFunction %void None %4
%6 = OpLabel
%i_0 = OpVariable %_ptr_Function_PS_INPUT Function
%param = OpVariable %_ptr_Function_PS_INPUT Function
OpLine %1 21 0
%54 = OpLoad %v2float %i_vTextureCoords
%55 = OpAccessChain %_ptr_Function_v2float %i_0 %int_0
OpStore %55 %54
%59 = OpLoad %PS_INPUT %i_0
OpStore %param %59
%60 = OpFunctionCall %PS_OUTPUT %_MainPs_struct_PS_INPUT_vf21_ %param
%61 = OpCompositeExtract %v4float %60 0
OpStore %_entryPointOutput_vColor %61
OpReturn
OpFunctionEnd
)";

  const std::string func2 = R"(
%_MainPs_struct_PS_INPUT_vf21_ = OpFunction %PS_OUTPUT None %13
%i = OpFunctionParameter %_ptr_Function_PS_INPUT
%16 = OpLabel
%ps_output = OpVariable %_ptr_Function_PS_OUTPUT Function
OpLine %1 24 0
%31 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%32 = OpLoad %uint %31
%34 = OpAccessChain %_ptr_UniformConstant_21 %g_tColor %32
%35 = OpLoad %21 %34
%39 = OpLoad %36 %g_sAniso
%41 = OpSampledImage %40 %35 %39
%43 = OpAccessChain %_ptr_Function_v2float %i %int_0
%44 = OpLoad %v2float %43
%45 = OpImageSampleImplicitLod %v4float %41 %44
; CHECK-NOT: %45 = OpImageSampleImplicitLod %v4float %41 %44
; CHECK: OpNoLine
; CHECK: %62 = OpULessThan %bool %50 %uint_128
; CHECK: OpSelectionMerge %63 None
; CHECK: OpBranchConditional %62 %64 %65
; CHECK: %64 = OpLabel
; CHECK: %66 = OpLoad %27 %51
; CHECK: %67 = OpSampledImage %37 %66 %53
; CHECK: OpLine %5 24 0
; CHECK: %68 = OpImageSampleImplicitLod %v4float %67 %56
; CHECK: OpNoLine
; CHECK: OpBranch %63
; CHECK: %65 = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_109 %uint_0 %uint_1 %uint_2 %50 %uint_128
; CHECK: OpBranch %63
; CHECK: %63 = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %v4float %68 %64 [[null_v4float]] %65
; CHECK: OpLine %5 24 0
%47 = OpAccessChain %_ptr_Function_v4float %ps_output %int_0
OpStore %47 %45
; CHECK-NOT: OpStore %47 %45
; CHECK: [[store_loc:%\w+]] = OpAccessChain %_ptr_Function_v4float %ps_output %int_0
; CHECK: OpStore [[store_loc]] [[phi_result]]
OpLine %1 25 0
%48 = OpLoad %PS_OUTPUT %ps_output
OpReturnValue %48
OpFunctionEnd
)";

  const std::string output_func = kStreamWrite6Frag;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(
      defs + func1 + func2 + output_func, true, 7u, 23u, false, false, false,
      false, false);
}

TEST_F(InstBindlessTest, RuntimeArray) {
  // This test verifies that the pass will correctly instrument shader
  // with runtime descriptor array. This test was created by editing the
  // SPIR-V from the Simple test.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability RuntimeDescriptorArray
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
OpName %_ ""
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 1
OpDecorate %g_tColor Binding 2
OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
OpDecorate %PerViewConstantBuffer_t Block
OpDecorate %g_sAniso DescriptorSet 1
OpDecorate %g_sAniso Binding 3
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%20 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_1 = OpConstant %uint 1
%_rarr_20 = OpTypeRuntimeArray %20
%_ptr_UniformConstant__arr_20 = OpTypePointer UniformConstant %_rarr_20
%g_tColor = OpVariable %_ptr_UniformConstant__arr_20 UniformConstant
%PerViewConstantBuffer_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
%_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%_ptr_UniformConstant_20 = OpTypePointer UniformConstant %20
%35 = OpTypeSampler
%_ptr_UniformConstant_35 = OpTypePointer UniformConstant %35
%g_sAniso = OpVariable %_ptr_UniformConstant_35 UniformConstant
%39 = OpTypeSampledImage %20
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %3
%5 = OpLabel
%53 = OpLoad %v2float %i_vTextureCoords
%63 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
%64 = OpLoad %uint %63
%65 = OpAccessChain %_ptr_UniformConstant_20 %g_tColor %64
%66 = OpLoad %20 %65
%67 = OpLoad %35 %g_sAniso
%68 = OpSampledImage %39 %66 %67
%71 = OpImageSampleImplicitLod %v4float %68 %53
OpStore %_entryPointOutput_vColor %71
; CHECK-NOT: %71 = OpImageSampleImplicitLod %v4float %68 %53
; CHECK-NOT: OpStore %_entryPointOutput_vColor %71
; CHECK: [[length_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_1 %uint_2
; CHECK: {{%\w+}} = OpULessThan %bool %32 [[length_result]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %16 %33
; CHECK: {{%\w+}} = OpSampledImage %26 {{%\w+}} %35
; CHECK: [[state_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %32
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 [[state_result]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %16 %33
; CHECK: {{%\w+}} = OpSampledImage %26 {{%\w+}} %35
; CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_59 %uint_1 %uint_1 %uint_2 %32 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_59 %uint_0 %uint_1 %uint_2 %32 {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %v4float [[phi_result_1]] {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpStore %_entryPointOutput_vColor [[phi_result_2]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstrumentInitCheckOnScalarDescriptor) {
  // This test verifies that the pass will correctly instrument vanilla
  // texture sample on a scalar descriptor with an initialization check if the
  // input_init_enable argument is set to true. This can happen when the
  // descriptor indexing extension is enabled in the API but the SPIR-V
  // does not have the extension enabled because it does not contain a
  // runtime array. This is the same shader as NoInstrumentNonBindless.

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor
; CHECK: OpEntryPoint Fragment %MainPs "MainPs" %i_vTextureCoords %_entryPointOutput_vColor %gl_FragCoord
OpExecutionMode %MainPs OriginUpperLeft
OpSource HLSL 500
OpName %MainPs "MainPs"
OpName %g_tColor "g_tColor"
OpName %g_sAniso "g_sAniso"
OpName %i_vTextureCoords "i.vTextureCoords"
OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
OpDecorate %g_tColor DescriptorSet 1
OpDecorate %g_tColor Binding 2
OpDecorate %g_sAniso DescriptorSet 1
OpDecorate %g_sAniso Binding 2
OpDecorate %i_vTextureCoords Location 0
OpDecorate %_entryPointOutput_vColor Location 0
; check: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; check: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%8 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%v4float = OpTypeVector %float 4
%12 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_12 = OpTypePointer UniformConstant %12
%g_tColor = OpVariable %_ptr_UniformConstant_12 UniformConstant
%14 = OpTypeSampler
%_ptr_UniformConstant_14 = OpTypePointer UniformConstant %14
%g_sAniso = OpVariable %_ptr_UniformConstant_14 UniformConstant
%16 = OpTypeSampledImage %12
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)";
  // clang-format on

  const std::string main_func = R"(
%MainPs = OpFunction %void None %8
%19 = OpLabel
%20 = OpLoad %v2float %i_vTextureCoords
%21 = OpLoad %12 %g_tColor
%22 = OpLoad %14 %g_sAniso
%23 = OpSampledImage %16 %21 %22
%24 = OpImageSampleImplicitLod %v4float %23 %20
OpStore %_entryPointOutput_vColor %24
; CHECK-NOT: %24 = OpImageSampleImplicitLod %v4float %23 %20
; CHECK-NOT: OpStore %_entryPointOutput_vColor %24
; CHECK: [[state_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 [[state_result]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %12 %g_tColor
; CHECK: {{%\w+}} = OpSampledImage %16 {{%\w+}} %22
; CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_39 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpStore %_entryPointOutput_vColor [[phi_result]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs = kReadDescInit + kStreamWrite6Frag;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, SPV14AddToEntryPoint) {
  const std::string text = R"(
; CHECK: OpEntryPoint Fragment {{%\w+}} "foo" {{%\w+}} {{%\w+}} {{%\w+}} [[v1:%\w+]] [[v2:%\w+]]
; CHECK: OpDecorate [[v1]] DescriptorSet 7
; CHECK: OpDecorate [[v2]] DescriptorSet 7
; CHECK: [[v1]] = OpVariable {{%\w+}} StorageBuffer
; CHECK: [[v2]] = OpVariable {{%\w+}} StorageBuffer
OpCapability Shader
OpExtension "SPV_EXT_descriptor_indexing"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %foo "foo" %gid %image_var %sampler_var
OpExecutionMode %foo OriginUpperLeft
OpDecorate %image_var DescriptorSet 4
OpDecorate %image_var Binding 1
OpDecorate %sampler_var DescriptorSet 4
OpDecorate %sampler_var Binding 2
OpDecorate %gid DescriptorSet 0
OpDecorate %gid Binding 2
OpDecorate %struct Block
OpMemberDecorate %struct 0 Offset 0
%void = OpTypeVoid
%int = OpTypeInt 32 0
%int_0 = OpConstant %int 0
%v3int = OpTypeVector %int 3
%float = OpTypeFloat 32
%v3float = OpTypeVector %float 3
%v4float = OpTypeVector %float 4
%struct = OpTypeStruct %v3int
%ptr_ssbo_struct = OpTypePointer StorageBuffer %struct
%ptr_ssbo_v3int = OpTypePointer StorageBuffer %v3int
%gid = OpVariable %ptr_ssbo_struct StorageBuffer
%image = OpTypeImage %float 3D 0 0 0 1 Unknown
%ptr_uc_image = OpTypePointer UniformConstant %image
%sampler = OpTypeSampler
%ptr_uc_sampler = OpTypePointer UniformConstant %sampler
%image_var = OpVariable %ptr_uc_image UniformConstant
%sampler_var = OpVariable %ptr_uc_sampler UniformConstant
%sampled = OpTypeSampledImage %image
%void_fn = OpTypeFunction %void
%foo = OpFunction %void None %void_fn
%entry = OpLabel
%ld_image = OpLoad %image %image_var
%ld_sampler = OpLoad %sampler %sampler_var
%gep = OpAccessChain %ptr_ssbo_v3int %gid %int_0
%ld_gid = OpLoad %v3int %gep
%convert = OpConvertUToF %v3float %ld_gid
%sampled_image = OpSampledImage %sampled %ld_image %ld_sampler
%sample = OpImageSampleImplicitLod %v4float %sampled_image %convert
OpReturn
OpFunctionEnd
)";

  SetTargetEnv(SPV_ENV_VULKAN_1_1_SPIRV_1_4);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, true, true,
                                               false, false, false);
}

TEST_F(InstBindlessTest, SPV14AddToEntryPoints) {
  const std::string text = R"(
; CHECK: OpEntryPoint Fragment {{%\w+}} "foo" {{%\w+}} {{%\w+}} {{%\w+}} [[v1:%\w+]] [[v2:%\w+]]
; CHECK: OpEntryPoint Fragment {{%\w+}} "bar" {{%\w+}} {{%\w+}} {{%\w+}} [[v1:%\w+]] [[v2:%\w+]]
; CHECK: OpDecorate [[v1]] DescriptorSet 7
; CHECK: OpDecorate [[v2]] DescriptorSet 7
; CHECK: [[v1]] = OpVariable {{%\w+}} StorageBuffer
; CHECK: [[v2]] = OpVariable {{%\w+}} StorageBuffer
OpCapability Shader
OpExtension "SPV_EXT_descriptor_indexing"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %foo "foo" %gid %image_var %sampler_var
OpEntryPoint Fragment %foo "bar" %gid %image_var %sampler_var
OpExecutionMode %foo OriginUpperLeft
OpDecorate %image_var DescriptorSet 3
OpDecorate %image_var Binding 2
OpDecorate %sampler_var DescriptorSet 3
OpDecorate %sampler_var Binding 3
OpDecorate %gid DescriptorSet 3
OpDecorate %gid Binding 4
OpDecorate %struct Block
OpMemberDecorate %struct 0 Offset 0
%void = OpTypeVoid
%int = OpTypeInt 32 0
%int_0 = OpConstant %int 0
%v3int = OpTypeVector %int 3
%float = OpTypeFloat 32
%v3float = OpTypeVector %float 3
%v4float = OpTypeVector %float 4
%struct = OpTypeStruct %v3int
%ptr_ssbo_struct = OpTypePointer StorageBuffer %struct
%ptr_ssbo_v3int = OpTypePointer StorageBuffer %v3int
%gid = OpVariable %ptr_ssbo_struct StorageBuffer
%image = OpTypeImage %float 3D 0 0 0 1 Unknown
%ptr_uc_image = OpTypePointer UniformConstant %image
%sampler = OpTypeSampler
%ptr_uc_sampler = OpTypePointer UniformConstant %sampler
%image_var = OpVariable %ptr_uc_image UniformConstant
%sampler_var = OpVariable %ptr_uc_sampler UniformConstant
%sampled = OpTypeSampledImage %image
%void_fn = OpTypeFunction %void
%foo = OpFunction %void None %void_fn
%entry = OpLabel
%ld_image = OpLoad %image %image_var
%ld_sampler = OpLoad %sampler %sampler_var
%gep = OpAccessChain %ptr_ssbo_v3int %gid %int_0
%ld_gid = OpLoad %v3int %gep
%convert = OpConvertUToF %v3float %ld_gid
%sampled_image = OpSampledImage %sampled %ld_image %ld_sampler
%sample = OpImageSampleImplicitLod %v4float %sampled_image %convert
OpReturn
OpFunctionEnd
)";

  SetTargetEnv(SPV_ENV_VULKAN_1_1_SPIRV_1_4);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, true, true,
                                               false, false, false);
}

TEST_F(InstBindlessTest, InstBoundsAndInitLoadUnsizedUBOArray) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location=0) in nonuniformEXT flat int nu_ii;
  // layout(location=0) out float b;
  //
  // layout(set = 6, binding=3)  uniform uname { float a; }  uniformBuffer[];
  //
  // void main()
  // {
  //     b = uniformBuffer[nu_ii].a;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability ShaderNonUniform
OpCapability RuntimeDescriptorArray
OpCapability UniformBufferArrayNonUniformIndexing
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %b %nu_ii
; CHECK: OpEntryPoint Fragment %main "main" %b %nu_ii %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %b "b"
OpName %uname "uname"
OpMemberName %uname 0 "a"
OpName %uniformBuffer "uniformBuffer"
OpName %nu_ii "nu_ii"
OpDecorate %b Location 0
OpMemberDecorate %uname 0 Offset 0
OpDecorate %uname Block
OpDecorate %uniformBuffer DescriptorSet 6
OpDecorate %uniformBuffer Binding 3
OpDecorate %nu_ii Flat
OpDecorate %nu_ii Location 0
OpDecorate %nu_ii NonUniform
OpDecorate %16 NonUniform
OpDecorate %20 NonUniform
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + R"(
; CHECK: OpDecorate {{%\w+}} NonUniform
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
; CHECK: OpDecorate {{%\w+}} NonUniform
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%b = OpVariable %_ptr_Output_float Output
%uname = OpTypeStruct %float
%_runtimearr_uname = OpTypeRuntimeArray %uname
%_ptr_Uniform__runtimearr_uname = OpTypePointer Uniform %_runtimearr_uname
%uniformBuffer = OpVariable %_ptr_Uniform__runtimearr_uname Uniform
%int = OpTypeInt 32 1
%_ptr_Input_int = OpTypePointer Input %int
%nu_ii = OpVariable %_ptr_Input_int Input
%int_0 = OpConstant %int 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %v4float = OpTypeVector %float 4
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_float:%\w+]] = OpConstantNull %float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%16 = OpLoad %int %nu_ii
%19 = OpAccessChain %_ptr_Uniform_float %uniformBuffer %16 %int_0
%20 = OpLoad %float %19
OpStore %b %20
; CHECK-NOT: %20 = OpLoad %float %19
; CHECK-NOT: OpStore %b %20
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_6 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool %7 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_6 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %float %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_1 %uint_6 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %float {{%\w+}} {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[bitcast_result:%\w+]] = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_0 %uint_6 %uint_3 [[bitcast_result]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %float [[phi_result_1]] {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpStore %b [[phi_result_2]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstBoundsAndInitLoadUnsizedSSBOArrayDeprecated) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location=0) in nonuniformEXT flat int nu_ii;
  // layout(location=0) out float b;
  //
  // layout(set = 7, binding=3)  buffer bname { float b; }  storageBuffer[];
  //
  // void main()
  // {
  //     b = storageBuffer[nu_ii].b;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability ShaderNonUniform
OpCapability RuntimeDescriptorArray
OpCapability StorageBufferArrayNonUniformIndexing
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %b %nu_ii
; CHECK: OpEntryPoint Fragment %main "main" %b %nu_ii %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %b "b"
OpName %bname "bname"
OpMemberName %bname 0 "a"
OpName %storageBuffer "storageBuffer"
OpName %nu_ii "nu_ii"
OpDecorate %b Location 0
OpMemberDecorate %bname 0 Offset 0
OpDecorate %bname Block
OpDecorate %storageBuffer DescriptorSet 7
OpDecorate %storageBuffer Binding 3
OpDecorate %nu_ii Flat
OpDecorate %nu_ii Location 0
OpDecorate %nu_ii NonUniform
OpDecorate %16 NonUniform
OpDecorate %20 NonUniform
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%b = OpVariable %_ptr_Output_float Output
%bname = OpTypeStruct %float
%_runtimearr_bname = OpTypeRuntimeArray %bname
%_ptr_StorageBuffer__runtimearr_bname = OpTypePointer StorageBuffer %_runtimearr_bname
%storageBuffer = OpVariable %_ptr_StorageBuffer__runtimearr_bname StorageBuffer
%int = OpTypeInt 32 1
%_ptr_Input_int = OpTypePointer Input %int
%nu_ii = OpVariable %_ptr_Input_int Input
%int_0 = OpConstant %int 0
%_ptr_StorageBuffer_float = OpTypePointer StorageBuffer %float
; CHECK: %uint = OpTypeInt 32 0
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %v4float = OpTypeVector %float 4
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_float:%\w+]] = OpConstantNull %float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%16 = OpLoad %int %nu_ii
%19 = OpAccessChain %_ptr_StorageBuffer_float %storageBuffer %16 %int_0
%20 = OpLoad %float %19
OpStore %b %20
; CHECK-NOT: %20 = OpLoad %float %19
; CHECK-NOT: OpStore %b %20
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_7 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool %7 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[bitcast_result_1:%\w+]] = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_7 %uint_3 [[bitcast_result_1]]
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %float %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_1 %uint_7 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %float {{%\w+}} {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[bitcast_result_2:%\w+]] = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_0 %uint_7 %uint_3 [[bitcast_result_2]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %float [[phi_result_1]] {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpStore %b [[phi_result_2]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstBoundsAndInitLoadUnsizedSSBOArray) {
  // Same as Deprecated but declaring as StorageBuffer Block

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability ShaderNonUniform
OpCapability RuntimeDescriptorArray
OpCapability StorageBufferArrayNonUniformIndexing
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %b %nu_ii
; CHECK: OpEntryPoint Fragment %main "main" %b %nu_ii %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %b "b"
OpName %bname "bname"
OpMemberName %bname 0 "a"
OpName %storageBuffer "storageBuffer"
OpName %nu_ii "nu_ii"
OpDecorate %b Location 0
OpMemberDecorate %bname 0 Offset 0
OpDecorate %bname Block
OpDecorate %storageBuffer DescriptorSet 0
OpDecorate %storageBuffer Binding 3
OpDecorate %nu_ii Flat
OpDecorate %nu_ii Location 0
OpDecorate %nu_ii NonUniform
OpDecorate %16 NonUniform
OpDecorate %20 NonUniform
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + R"(
; CHECK: OpDecorate {{%\w+}} NonUniform
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
; CHECK: OpDecorate {{%\w+}} NonUniform
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%b = OpVariable %_ptr_Output_float Output
%bname = OpTypeStruct %float
%_runtimearr_bname = OpTypeRuntimeArray %bname
%_ptr_StorageBuffer__runtimearr_bname = OpTypePointer StorageBuffer %_runtimearr_bname
%storageBuffer = OpVariable %_ptr_StorageBuffer__runtimearr_bname StorageBuffer
%int = OpTypeInt 32 1
%_ptr_Input_int = OpTypePointer Input %int
%nu_ii = OpVariable %_ptr_Input_int Input
%int_0 = OpConstant %int 0
%_ptr_StorageBuffer_float = OpTypePointer StorageBuffer %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %v4float = OpTypeVector %float 4
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_float:%\w+]] = OpConstantNull %float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%16 = OpLoad %int %nu_ii
%19 = OpAccessChain %_ptr_StorageBuffer_float %storageBuffer %16 %int_0
%20 = OpLoad %float %19
OpStore %b %20
; CHECK-NOT: %20 = OpLoad %float %19
; CHECK-NOT: OpStore %b %20
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_0 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool %7 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %float %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_1 %uint_0 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %float {{%\w+}} {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[bitcast_result:%\w+]] = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_0 %uint_0 %uint_3 [[bitcast_result]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %float [[phi_result_1]] {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpStore %b [[phi_result_2]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstInitLoadUBOScalar) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location=0) out float b;
  // layout(set=7, binding=3)  uniform uname { float a; }  uniformBuffer;
  //
  // void main()
  // {
  //     b = uniformBuffer.a;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %b
; CHECK: OpEntryPoint Fragment %main "main" %b %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %b "b"
OpName %uname "uname"
OpMemberName %uname 0 "a"
OpName %uniformBuffer "uniformBuffer"
OpDecorate %b Location 0
OpMemberDecorate %uname 0 Offset 0
OpDecorate %uname Block
OpDecorate %uniformBuffer DescriptorSet 7
OpDecorate %uniformBuffer Binding 3
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%b = OpVariable %_ptr_Output_float Output
%uname = OpTypeStruct %float
%_ptr_Uniform_uname = OpTypePointer Uniform %uname
%uniformBuffer = OpVariable %_ptr_Uniform_uname Uniform
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
; CHECK: %int = OpTypeInt 32 1
; CHECK: %_ptr_Uniform_float = OpTypePointer Uniform %float
; CHECK: %uint = OpTypeInt 32 0
; CHECK: %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %v4float = OpTypeVector %float 4
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_float:%\w+]] = OpConstantNull %float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%15 = OpAccessChain %_ptr_Uniform_float %uniformBuffer %int_0
%16 = OpLoad %float %15
OpStore %b %16
; CHECK-NOT: %16 = OpLoad %float %15
; CHECK-NOT: OpStore %b %16
; CHECK: [[check_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_7 %uint_3 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 [[check_result]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %float %15
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_32 %uint_1 %uint_7 %uint_3 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %float {{%\w+}} {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpStore %b [[phi_result]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs = kReadDescInit + kStreamWrite6Frag;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstBoundsInitStoreUnsizedSSBOArray) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location=0) in nonuniformEXT flat int nu_ii;
  // layout(location=1) in float b;
  //
  // layout(set=5, binding=4)  buffer bname { float b; }  storageBuffer[];
  //
  // void main()
  // {
  //     storageBuffer[nu_ii].b = b;
  // }

  // clang-format off
  const std::string defs = R"(OpCapability Shader
OpCapability ShaderNonUniform
OpCapability RuntimeDescriptorArray
OpCapability StorageBufferArrayNonUniformIndexing
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %nu_ii %b
; CHECK: OpEntryPoint Fragment %main "main" %nu_ii %b %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %bname "bname"
OpMemberName %bname 0 "b"
OpName %storageBuffer "storageBuffer"
OpName %nu_ii "nu_ii"
OpName %b "b"
OpMemberDecorate %bname 0 Offset 0
OpDecorate %bname BufferBlock
OpDecorate %storageBuffer DescriptorSet 5
OpDecorate %storageBuffer Binding 4
OpDecorate %nu_ii Flat
OpDecorate %nu_ii Location 0
OpDecorate %nu_ii NonUniform
OpDecorate %14 NonUniform
OpDecorate %b Location 1
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%bname = OpTypeStruct %float
%_runtimearr_bname = OpTypeRuntimeArray %bname
%_ptr_Uniform__runtimearr_bname = OpTypePointer Uniform %_runtimearr_bname
%storageBuffer = OpVariable %_ptr_Uniform__runtimearr_bname Uniform
%int = OpTypeInt 32 1
%_ptr_Input_int = OpTypePointer Input %int
%nu_ii = OpVariable %_ptr_Input_int Input
%int_0 = OpConstant %int 0
%_ptr_Input_float = OpTypePointer Input %float
%b = OpVariable %_ptr_Input_float Input
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%14 = OpLoad %int %nu_ii
%18 = OpLoad %float %b
%20 = OpAccessChain %_ptr_Uniform_float %storageBuffer %14 %int_0
OpStore %20 %18
; CHECK-NOT: OpStore %20 %18
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_5 %uint_4
; CHECK: {{%\w+}} = OpULessThan %bool %7 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_5 %uint_4 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %20 %19
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_1 %uint_5 %uint_4 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_45 %uint_0 %uint_5 %uint_4 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstBoundsInitLoadSizedUBOArray) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location=0) in nonuniformEXT flat int nu_ii;
  // layout(location=0) out float b;
  //
  // layout(set=1, binding=3)  uniform uname { float a; }  uniformBuffer[128];
  //
  // void main()
  // {
  //     b = uniformBuffer[nu_ii].a;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability ShaderNonUniform
OpCapability UniformBufferArrayNonUniformIndexing
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %b %nu_ii
; CHECK: OpEntryPoint Fragment %main "main" %b %nu_ii %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %b "b"
OpName %uname "uname"
OpMemberName %uname 0 "a"
OpName %uniformBuffer "uniformBuffer"
OpName %nu_ii "nu_ii"
OpDecorate %b Location 0
OpMemberDecorate %uname 0 Offset 0
OpDecorate %uname Block
OpDecorate %uniformBuffer DescriptorSet 1
OpDecorate %uniformBuffer Binding 3
OpDecorate %nu_ii Flat
OpDecorate %nu_ii Location 0
OpDecorate %nu_ii NonUniform
OpDecorate %18 NonUniform
OpDecorate %22 NonUniform
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
)" + kInputDecorations + R"(
; CHECK: OpDecorate [[load_result:%\w+]] NonUniform
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%b = OpVariable %_ptr_Output_float Output
%uname = OpTypeStruct %float
%uint = OpTypeInt 32 0
%uint_128 = OpConstant %uint 128
%_arr_uname_uint_128 = OpTypeArray %uname %uint_128
%_ptr_Uniform__arr_uname_uint_128 = OpTypePointer Uniform %_arr_uname_uint_128
%uniformBuffer = OpVariable %_ptr_Uniform__arr_uname_uint_128 Uniform
%int = OpTypeInt 32 1
%_ptr_Input_int = OpTypePointer Input %int
%nu_ii = OpVariable %_ptr_Input_int Input
%int_0 = OpConstant %int 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kOutputGlobals + R"(
; CHECK: %v4float = OpTypeVector %float 4
; CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
; CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
; CHECK: %v4uint = OpTypeVector %uint 4
; CHECK: [[null_float:%\w+]] = OpConstantNull %float
)" + kInputGlobals;
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%18 = OpLoad %int %nu_ii
%21 = OpAccessChain %_ptr_Uniform_float %uniformBuffer %18 %int_0
%22 = OpLoad %float %21
OpStore %b %22
; CHECK-NOT: %22 = OpLoad %float %21
; CHECK-NOT: OpStore %b %22
; CHECK: {{%\w+}} = OpULessThan %bool %7 %uint_128
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[load_result]] = OpLoad %float %22
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_46 %uint_1 %uint_1 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %float {{%\w+}} {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpBitcast %uint %7
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_46 %uint_0 %uint_1 %uint_3 {{%\w+}} %uint_128
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %float [[phi_result_1]] {{%\w+}} [[null_float]] {{%\w+}}
; CHECK: OpStore %b [[phi_result_2]]
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs = kStreamWrite6Frag + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsComputeShaderInitLoadVariableSizedSampledImagesArray) {
  // #version 450
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout (local_size_x = 1, local_size_y = 1) in;
  //
  // layout(set = 2, binding = 0, std140) buffer Input {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 2, binding = 1, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability Shader
OpCapability RuntimeDescriptorArray
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint GLCompute %main "main"
; CHECK: OpEntryPoint GLCompute %main "main" %gl_GlobalInvocationID
OpExecutionMode %main LocalSize 1 1 1
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %Input "Input"
OpMemberName %Input 0 "index"
OpMemberName %Input 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %Input 0 Offset 0
OpMemberDecorate %Input 1 Offset 4
OpDecorate %Input BufferBlock
OpDecorate %sbo DescriptorSet 2
OpDecorate %sbo Binding 0
OpDecorate %images DescriptorSet 2
OpDecorate %images Binding 1
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_GlobalInvocationID BuiltIn GlobalInvocationId
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%Input = OpTypeStruct %uint %float
%_ptr_Uniform_Input = OpTypePointer Uniform %Input
%sbo = OpVariable %_ptr_Uniform_Input Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: %v3uint = OpTypeVector %uint 3
; CHECK: %_ptr_Input_v3uint = OpTypePointer Input %v3uint
; CHECK: %gl_GlobalInvocationID = OpVariable %_ptr_Input_v3uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_0 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_47 %uint_1 %uint_2 %uint_0 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_1:%\w+]] = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images {{%\w+}}
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_2 %uint_1
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_1 [[phi_result_1]]
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_50 %uint_1 %uint_2 %uint_1 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result_2:%\w+]] = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_50 %uint_0 %uint_2 %uint_1 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float [[phi_result_2]] {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: {{%\w+}} = OpCompositeExtract %float {{%\w+}} 0
; CHECK: {{%\w+}} = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: [[desc_state_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_0 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 [[desc_state_result]]
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_53 %uint_1 %uint_2 %uint_0 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Compute + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsRayGenerationInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 3, binding = 1, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 3, binding = 5, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint RayGenerationNV %main "main"
; CHECK: OpEntryPoint RayGenerationNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 3
OpDecorate %sbo Binding 1
OpDecorate %images DescriptorSet 3
OpDecorate %images Binding 5
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_3 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_3 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images {{%\w+}}
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_3 %uint_5
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_3 %uint_5 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_3 %uint_5 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_3 %uint_5 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_3 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_3 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsIntersectionInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 5, binding = 1, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 5, binding = 3, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint IntersectionNV %main "main"
; CHECK: OpEntryPoint IntersectionNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 5
OpDecorate %sbo Binding 1
OpDecorate %images DescriptorSet 5
OpDecorate %images Binding 3
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[launch_id]] = OpVariable %_ptr_Input_v3uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_5 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_5 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images {{%\w+}}
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_5 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_5 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_5 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_5 %uint_3 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_5 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_5 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsAnyHitInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 2, binding = 1, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 2, binding = 3, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint AnyHitNV %main "main"
; CHECK: OpEntryPoint AnyHitNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 2
OpDecorate %sbo Binding 1
OpDecorate %images DescriptorSet 2
OpDecorate %images Binding 3
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[launch_id]] = OpVariable %_ptr_Input_v3uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_2 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images {{%\w+}}
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_2 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_2 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_2 %uint_3 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_2 %uint_1 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_2 %uint_1 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsClosestHitInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 1, binding = 2, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 1, binding = 3, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint ClosestHitNV %main "main"
; CHECK: OpEntryPoint ClosestHitNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 1
OpDecorate %sbo Binding 2
OpDecorate %images DescriptorSet 1
OpDecorate %images Binding 3
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[launch_id]] = OpVariable %_ptr_Input_v3uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images [[phi_result]]
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_1 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_1 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_1 %uint_3 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_1 %uint_2
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsMissInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 1, binding = 2, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 1, binding = 3, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint MissNV %main "main"
; CHECK: OpEntryPoint MissNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 1
OpDecorate %sbo Binding 2
OpDecorate %images DescriptorSet 1
OpDecorate %images Binding 3
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[launch_id]] = OpVariable %_ptr_Input_v3uint Input
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: [[phi_result:%\w+]] = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images [[phi_result]]
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_1 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_1 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_1 %uint_3 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest,
       InstBoundsCallableInitLoadVariableSizedSampledImagesArray) {
  // #version 460
  // #extension GL_EXT_nonuniform_qualifier : require
  // #extension GL_NV_ray_tracing : require
  //
  // layout(set = 1, binding = 2, std140) buffer StorageBuffer {
  //   uint index;
  //   float red;
  // } sbo;
  //
  // layout(set = 1, binding = 3, rgba32f) readonly uniform image2D images[];
  //
  // void main()
  // {
  //    sbo.red = imageLoad(images[sbo.index], ivec2(0, 0)).r;
  // }

  // clang-format off
  const std::string defs = R"(
OpCapability RuntimeDescriptorArray
OpCapability RayTracingNV
OpExtension "SPV_EXT_descriptor_indexing"
OpExtension "SPV_NV_ray_tracing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint CallableNV %main "main"
; CHECK: OpEntryPoint CallableNV %main "main" [[launch_id:%\w+]]
OpSource GLSL 460
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpSourceExtension "GL_NV_ray_tracing"
OpName %main "main"
OpName %StorageBuffer "StorageBuffer"
OpMemberName %StorageBuffer 0 "index"
OpMemberName %StorageBuffer 1 "red"
OpName %sbo "sbo"
OpName %images "images"
OpMemberDecorate %StorageBuffer 0 Offset 0
OpMemberDecorate %StorageBuffer 1 Offset 4
OpDecorate %StorageBuffer BufferBlock
OpDecorate %sbo DescriptorSet 1
OpDecorate %sbo Binding 2
OpDecorate %images DescriptorSet 1
OpDecorate %images Binding 3
OpDecorate %images NonWritable
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
; CHECK: OpDecorate [[launch_id]] BuiltIn LaunchIdNV
%void = OpTypeVoid
%3 = OpTypeFunction %void
%uint = OpTypeInt 32 0
%float = OpTypeFloat 32
%StorageBuffer = OpTypeStruct %uint %float
%_ptr_Uniform_StorageBuffer = OpTypePointer Uniform %StorageBuffer
%sbo = OpVariable %_ptr_Uniform_StorageBuffer Uniform
%int = OpTypeInt 32 1
%int_1 = OpConstant %int 1
%13 = OpTypeImage %float 2D 0 0 0 2 Rgba32f
%_runtimearr_13 = OpTypeRuntimeArray %13
%_ptr_UniformConstant__runtimearr_13 = OpTypePointer UniformConstant %_runtimearr_13
%images = OpVariable %_ptr_UniformConstant__runtimearr_13 UniformConstant
%int_0 = OpConstant %int 0
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%v2int = OpTypeVector %int 2
%25 = OpConstantComposite %v2int %int_0 %int_0
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%_ptr_Uniform_float = OpTypePointer Uniform %float
)" + kInputGlobals + kOutputGlobals + R"(
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
; CHECK: [[null_uint:%\w+]] = OpConstantNull %uint
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%19 = OpAccessChain %_ptr_Uniform_uint %sbo %int_0
%20 = OpLoad %uint %19
%22 = OpAccessChain %_ptr_UniformConstant_13 %images %20
%23 = OpLoad %13 %22
%27 = OpImageRead %v4float %23 %25
%29 = OpCompositeExtract %float %27 0
%31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
OpStore %31 %29
; CHECK-NOT: OpStore %31 %29
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %uint %25
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_48 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0 
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %uint {{%\w+}} {{%\w+}} [[null_uint]] {{%\w+}}
; CHECK: %27 = OpAccessChain %_ptr_UniformConstant_13 %images {{%\w+}}
; CHECK: %28 = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_binding_length %uint_1 %uint_3
; CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_3 {{%\w+}}
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %27
; CHECK: {{%\w+}} = OpImageRead %v4float {{%\w+}} %20
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_1 %uint_1 %uint_3 {{%\w+}} %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_51 %uint_0 %uint_1 %uint_3 {{%\w+}} {{%\w+}}
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %30 = OpCompositeExtract %float {{%\w+}} 0
; CHECK: %31 = OpAccessChain %_ptr_Uniform_float %sbo %int_1
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: OpStore %31 %30
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_54 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs =
      kReadBindingLength + kStreamWrite6Ray + kReadDescInit;

  // SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, InstBoundsInitSameBlockOpReplication) {
  // Test that same block ops like OpSampledImage are replicated properly
  // where needed.
  //
  // clang-format off
  //
  // #version 450 core
  // #extension GL_EXT_nonuniform_qualifier : enable
  //
  // layout(location = 0) in vec2 inTexcoord;
  // layout(location = 0) out vec4 outColor;
  //
  // layout(set = 1, binding = 0) uniform Uniforms {
  //   vec2 var0;
  // } uniforms;
  //
  // layout(set = 1, binding = 1) uniform sampler uniformSampler;
  // layout(set = 1, binding = 2) uniform texture2D uniformTex;
  // layout(set = 1, binding = 3) uniform texture2D uniformTexArr[8];
  //
  // void main() {
  //   int index = 0;
  //   float x = texture(sampler2D(uniformTexArr[nonuniformEXT(index)], uniformSampler), inTexcoord.xy).x;
  //   float y = texture(sampler2D(uniformTex, uniformSampler), inTexcoord.xy * uniforms.var0.xy).x;
  //   outColor = vec4(x, y, 0.0, 0.0);
  // }
  //

  const std::string defs = R"(
OpCapability Shader
OpCapability ShaderNonUniformEXT
OpCapability SampledImageArrayNonUniformIndexingEXT
OpExtension "SPV_EXT_descriptor_indexing"
; CHECK: OpExtension "SPV_KHR_storage_buffer_storage_class"
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" %inTexcoord %outColor
; CHECK: OpEntryPoint Fragment %main "main" %inTexcoord %outColor %gl_FragCoord
OpExecutionMode %main OriginUpperLeft
OpSource GLSL 450
OpSourceExtension "GL_EXT_nonuniform_qualifier"
OpName %main "main"
OpName %index "index"
OpName %x "x"
OpName %uniformTexArr "uniformTexArr"
OpName %uniformSampler "uniformSampler"
OpName %inTexcoord "inTexcoord"
OpName %y "y"
OpName %uniformTex "uniformTex"
OpName %Uniforms "Uniforms"
OpMemberName %Uniforms 0 "var0"
OpName %uniforms "uniforms"
OpName %outColor "outColor"
OpDecorate %uniformTexArr DescriptorSet 1
OpDecorate %uniformTexArr Binding 3
OpDecorate %19 NonUniformEXT
OpDecorate %22 NonUniformEXT
OpDecorate %uniformSampler DescriptorSet 1
OpDecorate %uniformSampler Binding 1
OpDecorate %inTexcoord Location 0
OpDecorate %uniformTex DescriptorSet 1
OpDecorate %uniformTex Binding 2
OpMemberDecorate %Uniforms 0 Offset 0
OpDecorate %Uniforms Block
OpDecorate %uniforms DescriptorSet 1
OpDecorate %uniforms Binding 0
OpDecorate %outColor Location 0
; CHECK: OpDecorate %63 NonUniform
; CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
; CHECK: OpDecorate %gl_FragCoord BuiltIn FragCoord
)" + kInputDecorations + R"(
; CHECK: OpDecorate [[desc_state_result:%\w+]] NonUniform
%void = OpTypeVoid
%3 = OpTypeFunction %void
%int = OpTypeInt 32 1
%_ptr_Function_int = OpTypePointer Function %int
%int_0 = OpConstant %int 0
%float = OpTypeFloat 32
%_ptr_Function_float = OpTypePointer Function %float
%13 = OpTypeImage %float 2D 0 0 0 1 Unknown
%uint = OpTypeInt 32 0
%uint_8 = OpConstant %uint 8
%_arr_13_uint_8 = OpTypeArray %13 %uint_8
%_ptr_UniformConstant__arr_13_uint_8 = OpTypePointer UniformConstant %_arr_13_uint_8
%uniformTexArr = OpVariable %_ptr_UniformConstant__arr_13_uint_8 UniformConstant
%_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
%23 = OpTypeSampler
%_ptr_UniformConstant_23 = OpTypePointer UniformConstant %23
%uniformSampler = OpVariable %_ptr_UniformConstant_23 UniformConstant
%27 = OpTypeSampledImage %13
%v2float = OpTypeVector %float 2
%_ptr_Input_v2float = OpTypePointer Input %v2float
%inTexcoord = OpVariable %_ptr_Input_v2float Input
%v4float = OpTypeVector %float 4
%uint_0 = OpConstant %uint 0
%uniformTex = OpVariable %_ptr_UniformConstant_13 UniformConstant
%Uniforms = OpTypeStruct %v2float
%_ptr_Uniform_Uniforms = OpTypePointer Uniform %Uniforms
%uniforms = OpVariable %_ptr_Uniform_Uniforms Uniform
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
%_ptr_Output_v4float = OpTypePointer Output %v4float
%outColor = OpVariable %_ptr_Output_v4float Output
%float_0 = OpConstant %float 0
)" + kOutputGlobals + R"(
; CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
)" + kInputGlobals + R"(
; CHECK: [[null_v2float:%\w+]] = OpConstantNull %v2float
)";
  // clang-format on

  const std::string main_func = R"(
%main = OpFunction %void None %3
%5 = OpLabel
%index = OpVariable %_ptr_Function_int Function
%x = OpVariable %_ptr_Function_float Function
%y = OpVariable %_ptr_Function_float Function
OpStore %index %int_0
%19 = OpLoad %int %index
%21 = OpAccessChain %_ptr_UniformConstant_13 %uniformTexArr %19
%22 = OpLoad %13 %21
%26 = OpLoad %23 %uniformSampler
%28 = OpSampledImage %27 %22 %26
%32 = OpLoad %v2float %inTexcoord
%34 = OpImageSampleImplicitLod %v4float %28 %32
%36 = OpCompositeExtract %float %34 0
OpStore %x %36
%39 = OpLoad %13 %uniformTex
%40 = OpLoad %23 %uniformSampler
%41 = OpSampledImage %27 %39 %40
%42 = OpLoad %v2float %inTexcoord
%47 = OpAccessChain %_ptr_Uniform_v2float %uniforms %int_0
%48 = OpLoad %v2float %47
%49 = OpFMul %v2float %42 %48
%50 = OpImageSampleImplicitLod %v4float %41 %49
%51 = OpCompositeExtract %float %50 0
; CHECK-NOT: %51 = OpCompositeExtract %float %50 0
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_0 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %v2float %47
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_87 %uint_1 %uint_1 %uint_0 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
; CHECK: %49 = OpFMul %v2float %42 {{%\w+}}
; CHECK: {{%\w+}} = OpSampledImage %27 %39 %40
; CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 %uint_0
; CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
; CHECK: OpSelectionMerge {{%\w+}} None
; CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpLoad %13 %uniformTex
; CHECK: {{%\w+}} = OpSampledImage %27 {{%\w+}} %40
; CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %49
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_89 %uint_1 %uint_1 %uint_2 %uint_0 %uint_0
; CHECK: OpBranch {{%\w+}}
; CHECK: {{%\w+}} = OpLabel
; CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
; CHECK: %51 = OpCompositeExtract %float {{%\w+}} 0
OpStore %y %51
%54 = OpLoad %float %x
%55 = OpLoad %float %y
%57 = OpCompositeConstruct %v4float %54 %55 %float_0 %float_0
OpStore %outColor %57
OpReturn
OpFunctionEnd
)";

  const std::string new_funcs = kStreamWrite6Frag + kReadDescInit;

  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(defs + main_func + new_funcs,
                                               true, 7u, 23u, true, true, false,
                                               false, false);
}

TEST_F(InstBindlessTest, MultipleUniformNonAggregateRefsNoDescInit) {
  // Check that uniform refs do not go out-of-bounds. All checks use same input
  // buffer read function call result at top of function for uniform buffer
  // length. Because descriptor indexing is not being checked, we can avoid one
  // buffer load.
  //
  // Texture2D g_tColor;
  // SamplerState g_sAniso;
  //
  // layout(push_constant) cbuffer PerViewPushConst_t { bool g_B; };
  //
  // cbuffer PerViewConstantBuffer_t {
  //   float2 g_TexOff0;
  //   float2 g_TexOff1;
  // };
  //
  // struct PS_INPUT {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i) {
  //   PS_OUTPUT ps_output;
  //   float2 off;
  //   float2 vtc;
  //   if (g_B)
  //     off = g_TexOff0;
  //   else
  //     off = g_TexOff1;
  //   vtc = i.vTextureCoords.xy + off;
  //   ps_output.vColor = g_tColor.Sample(g_sAniso, vtc);
  //   return ps_output;
  // }

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
;CHECK:        OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor
;CHECK:        OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_FragCoord
               OpExecutionMode %MainPs OriginUpperLeft
               OpSource HLSL 500
               OpName %MainPs "MainPs"
               OpName %PerViewPushConst_t "PerViewPushConst_t"
               OpMemberName %PerViewPushConst_t 0 "g_B"
               OpName %_ ""
               OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
               OpMemberName %PerViewConstantBuffer_t 0 "g_TexOff0"
               OpMemberName %PerViewConstantBuffer_t 1 "g_TexOff1"
               OpName %__0 ""
               OpName %g_tColor "g_tColor"
               OpName %g_sAniso "g_sAniso"
               OpName %i_vTextureCoords "i.vTextureCoords"
               OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
               OpMemberDecorate %PerViewPushConst_t 0 Offset 0
               OpDecorate %PerViewPushConst_t Block
               OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
               OpMemberDecorate %PerViewConstantBuffer_t 1 Offset 8
               OpDecorate %PerViewConstantBuffer_t Block
               OpDecorate %__0 DescriptorSet 0
               OpDecorate %__0 Binding 1
               OpDecorate %g_tColor DescriptorSet 0
               OpDecorate %g_tColor Binding 0
               OpDecorate %g_sAniso DescriptorSet 0
               OpDecorate %g_sAniso Binding 2
               OpDecorate %i_vTextureCoords Location 0
               OpDecorate %_entryPointOutput_vColor Location 0
 ;CHECK:       OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
 ;CHECK:       OpDecorate %gl_FragCoord BuiltIn FragCoord
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
    %v4float = OpTypeVector %float 4
       %uint = OpTypeInt 32 0
%PerViewPushConst_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewPushConst_t = OpTypePointer PushConstant %PerViewPushConst_t
          %_ = OpVariable %_ptr_PushConstant_PerViewPushConst_t PushConstant
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
       %bool = OpTypeBool
     %uint_0 = OpConstant %uint 0
%PerViewConstantBuffer_t = OpTypeStruct %v2float %v2float
%_ptr_Uniform_PerViewConstantBuffer_t = OpTypePointer Uniform %PerViewConstantBuffer_t
        %__0 = OpVariable %_ptr_Uniform_PerViewConstantBuffer_t Uniform
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
      %int_1 = OpConstant %int 1
         %49 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_49 = OpTypePointer UniformConstant %49
   %g_tColor = OpVariable %_ptr_UniformConstant_49 UniformConstant
         %53 = OpTypeSampler
%_ptr_UniformConstant_53 = OpTypePointer UniformConstant %53
   %g_sAniso = OpVariable %_ptr_UniformConstant_53 UniformConstant
         %57 = OpTypeSampledImage %49
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
 )" + kInputGlobals + kOutputGlobals + R"(
 ;CHECK:%_ptr_Input_v4float = OpTypePointer Input %v4float
 ;CHECK:%gl_FragCoord = OpVariable %_ptr_Input_v4float Input
 ;CHECK:     %v4uint = OpTypeVector %uint 4
 ;CHECK:        [[null_v2float:%\w+]] = OpConstantNull %v2float
     %MainPs = OpFunction %void None %3
          %5 = OpLabel
 ;CHECK: [[desc_state_result:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_1 %uint_0
 ;CHECK:        OpBranch %117
 ;CHECK: %117 = OpLabel
 ;CHECK:        OpBranch %116
 ;CHECK: %116 = OpLabel
         %69 = OpLoad %v2float %i_vTextureCoords
         %82 = OpAccessChain %_ptr_PushConstant_uint %_ %int_0
         %83 = OpLoad %uint %82
         %84 = OpINotEqual %bool %83 %uint_0
               OpSelectionMerge %91 None
               OpBranchConditional %84 %85 %88
         %85 = OpLabel
         %86 = OpAccessChain %_ptr_Uniform_v2float %__0 %int_0
         %87 = OpLoad %v2float %86
 ;CHECK-NOT:     %87 = OpLoad %v2float %86
 ;CHECK: %119 = OpIAdd %uint %uint_0 %uint_7
 ;CHECK: {{%\w+}} = OpULessThan %bool %119 [[desc_state_result]]
 ;CHECK: OpSelectionMerge {{%\w+}} None
 ;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpLoad %v2float %86
 ;CHECK: OpBranch {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_71 %uint_4 %uint_0 %uint_1 %uint_0 %119 {{%\w+}}
 ;CHECK: OpBranch {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
               OpBranch %91
         %88 = OpLabel
         %89 = OpAccessChain %_ptr_Uniform_v2float %__0 %int_1
         %90 = OpLoad %v2float %89
 ;CHECK-NOT:     %90 = OpLoad %v2float %89
 ;CHECK: {{%\w+}} = OpIAdd %uint %uint_8 %uint_7
 ;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
 ;CHECK: OpSelectionMerge {{%\w+}} None
 ;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpLoad %v2float %89
 ;CHECK: OpBranch {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_75 %uint_4 %uint_0 %uint_1 %uint_0 {{%\w+}} {{%\w+}}
 ;CHECK: OpBranch {{%\w+}}
 ;CHECK: {{%\w+}} = OpLabel
 ;CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
               OpBranch %91
         %91 = OpLabel
        %115 = OpPhi %v2float %87 %85 %90 %88
 ;CHECK-NOT:       %115 = OpPhi %v2float %87 %85 %90 %88
 ;CHECK: %115 = OpPhi %v2float {{%\w+}} {{%\w+}} {{%\w+}} {{%\w+}}
         %95 = OpFAdd %v2float %69 %115
         %96 = OpLoad %49 %g_tColor
         %97 = OpLoad %53 %g_sAniso
         %98 = OpSampledImage %57 %96 %97
        %100 = OpImageSampleImplicitLod %v4float %98 %95
               OpStore %_entryPointOutput_vColor %100
               OpReturn
               OpFunctionEnd
)" + kReadDescInit + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, UniformArrayRefNoDescInit) {
  // Check that uniform array ref does not go out-of-bounds.
  //
  // Texture2D g_tColor;
  // SamplerState g_sAniso;
  //
  // layout(push_constant) cbuffer PerViewPushConst_t { uint g_c; };
  //
  // struct PerBatchEnvMapConstantBuffer_t {
  //   float4x3 g_matEnvMapWorldToLocal;
  //   float4 g_vEnvironmentMapBoxMins;
  //   float2 g_TexOff;
  // };
  //
  // cbuffer _BindlessFastEnvMapCB_PS_t {
  //   PerBatchEnvMapConstantBuffer_t g_envMapConstants[128];
  // };
  //
  // struct PS_INPUT {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i) {
  //   PS_OUTPUT ps_output;
  //   float2 off;
  //   float2 vtc;
  //   off = g_envMapConstants[g_c].g_TexOff;
  //   vtc = i.vTextureCoords.xy + off;
  //   ps_output.vColor = g_tColor.Sample(g_sAniso, vtc);
  //   return ps_output;
  // }

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor
               OpExecutionMode %MainPs OriginUpperLeft
               OpSource HLSL 500
               OpName %MainPs "MainPs"
               OpName %PerBatchEnvMapConstantBuffer_t "PerBatchEnvMapConstantBuffer_t"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 0 "g_matEnvMapWorldToLocal"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 1 "g_vEnvironmentMapBoxMins"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 2 "g_TexOff"
               OpName %_BindlessFastEnvMapCB_PS_t "_BindlessFastEnvMapCB_PS_t"
               OpMemberName %_BindlessFastEnvMapCB_PS_t 0 "g_envMapConstants"
               OpName %_ ""
               OpName %PerViewPushConst_t "PerViewPushConst_t"
               OpMemberName %PerViewPushConst_t 0 "g_c"
               OpName %__0 ""
               OpName %g_tColor "g_tColor"
               OpName %g_sAniso "g_sAniso"
               OpName %i_vTextureCoords "i.vTextureCoords"
               OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 RowMajor
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 Offset 0
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 MatrixStride 16
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 1 Offset 48
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 2 Offset 64
               OpDecorate %_arr_PerBatchEnvMapConstantBuffer_t_uint_128 ArrayStride 80
               OpMemberDecorate %_BindlessFastEnvMapCB_PS_t 0 Offset 0
               OpDecorate %_BindlessFastEnvMapCB_PS_t Block
               OpDecorate %_ DescriptorSet 0
               OpDecorate %_ Binding 2
               OpMemberDecorate %PerViewPushConst_t 0 Offset 0
               OpDecorate %PerViewPushConst_t Block
               OpDecorate %g_tColor DescriptorSet 0
               OpDecorate %g_tColor Binding 0
               OpDecorate %g_sAniso DescriptorSet 0
               OpDecorate %g_sAniso Binding 1
               OpDecorate %i_vTextureCoords Location 0
               OpDecorate %_entryPointOutput_vColor Location 0
;CHECK:               OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
;CHECK:               OpDecorate %gl_FragCoord BuiltIn FragCoord
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
    %v4float = OpTypeVector %float 4
    %v3float = OpTypeVector %float 3
%mat4v3float = OpTypeMatrix %v3float 4
%PerBatchEnvMapConstantBuffer_t = OpTypeStruct %mat4v3float %v4float %v2float
       %uint = OpTypeInt 32 0
   %uint_128 = OpConstant %uint 128
%_arr_PerBatchEnvMapConstantBuffer_t_uint_128 = OpTypeArray %PerBatchEnvMapConstantBuffer_t %uint_128
%_BindlessFastEnvMapCB_PS_t = OpTypeStruct %_arr_PerBatchEnvMapConstantBuffer_t_uint_128
%_ptr_Uniform__BindlessFastEnvMapCB_PS_t = OpTypePointer Uniform %_BindlessFastEnvMapCB_PS_t
          %_ = OpVariable %_ptr_Uniform__BindlessFastEnvMapCB_PS_t Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
%PerViewPushConst_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewPushConst_t = OpTypePointer PushConstant %PerViewPushConst_t
        %__0 = OpVariable %_ptr_PushConstant_PerViewPushConst_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
      %int_2 = OpConstant %int 2
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
         %46 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_46 = OpTypePointer UniformConstant %46
   %g_tColor = OpVariable %_ptr_UniformConstant_46 UniformConstant
         %50 = OpTypeSampler
%_ptr_UniformConstant_50 = OpTypePointer UniformConstant %50
   %g_sAniso = OpVariable %_ptr_UniformConstant_50 UniformConstant
         %54 = OpTypeSampledImage %46
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK:%_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:%gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK:     %v4uint = OpTypeVector %uint 4
;CHECK:        [[null_v2float:%\w+]] = OpConstantNull %v2float
     %MainPs = OpFunction %void None %3
          %5 = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_2 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
         %66 = OpLoad %v2float %i_vTextureCoords
         %79 = OpAccessChain %_ptr_PushConstant_uint %__0 %int_0
         %80 = OpLoad %uint %79
         %81 = OpAccessChain %_ptr_Uniform_v2float %_ %int_0 %80 %int_2
         %82 = OpLoad %v2float %81
;CHECK-NOT:     %82 = OpLoad %v2float %81
;CHECK: {{%\w+}} = OpIMul %uint %uint_80 %80
;CHECK: {{%\w+}} = OpIAdd %uint %uint_0 {{%\w+}}
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_64
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_7
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpLoad %v2float %81
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_78 %uint_4 %uint_0 %uint_2 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
         %86 = OpFAdd %v2float %66 %82
;CHECK-NOT:         %86 = OpFAdd %v2float %66 %82
;CHECK:             %86 = OpFAdd %v2float %66 {{%\w+}}
         %87 = OpLoad %46 %g_tColor
         %88 = OpLoad %50 %g_sAniso
         %89 = OpSampledImage %54 %87 %88
         %91 = OpImageSampleImplicitLod %v4float %89 %86
               OpStore %_entryPointOutput_vColor %91
               OpReturn
               OpFunctionEnd
)" + kReadDescInit + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, UniformArrayRefWithDescInit) {
  // The buffer-oob and desc-init checks should use the same debug
  // output buffer write function.
  //
  // Same source as UniformArrayRefNoDescInit

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor
;CHECK:        OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_FragCoord
               OpExecutionMode %MainPs OriginUpperLeft
               OpSource HLSL 500
               OpName %MainPs "MainPs"
               OpName %PerBatchEnvMapConstantBuffer_t "PerBatchEnvMapConstantBuffer_t"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 0 "g_matEnvMapWorldToLocal"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 1 "g_vEnvironmentMapBoxMins"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 2 "g_TexOff"
               OpName %_BindlessFastEnvMapCB_PS_t "_BindlessFastEnvMapCB_PS_t"
               OpMemberName %_BindlessFastEnvMapCB_PS_t 0 "g_envMapConstants"
               OpName %_ ""
               OpName %PerViewPushConst_t "PerViewPushConst_t"
               OpMemberName %PerViewPushConst_t 0 "g_c"
               OpName %__0 ""
               OpName %g_tColor "g_tColor"
               OpName %g_sAniso "g_sAniso"
               OpName %i_vTextureCoords "i.vTextureCoords"
               OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 RowMajor
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 Offset 0
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 MatrixStride 16
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 1 Offset 48
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 2 Offset 64
               OpDecorate %_arr_PerBatchEnvMapConstantBuffer_t_uint_128 ArrayStride 80
               OpMemberDecorate %_BindlessFastEnvMapCB_PS_t 0 Offset 0
               OpDecorate %_BindlessFastEnvMapCB_PS_t Block
               OpDecorate %_ DescriptorSet 0
               OpDecorate %_ Binding 2
               OpMemberDecorate %PerViewPushConst_t 0 Offset 0
               OpDecorate %PerViewPushConst_t Block
               OpDecorate %g_tColor DescriptorSet 0
               OpDecorate %g_tColor Binding 0
               OpDecorate %g_sAniso DescriptorSet 0
               OpDecorate %g_sAniso Binding 1
               OpDecorate %i_vTextureCoords Location 0
               OpDecorate %_entryPointOutput_vColor Location 0
;CHECK:               OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
;CHECK:               OpDecorate %gl_FragCoord BuiltIn FragCoord
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
    %v4float = OpTypeVector %float 4
    %v3float = OpTypeVector %float 3
%mat4v3float = OpTypeMatrix %v3float 4
%PerBatchEnvMapConstantBuffer_t = OpTypeStruct %mat4v3float %v4float %v2float
       %uint = OpTypeInt 32 0
   %uint_128 = OpConstant %uint 128
%_arr_PerBatchEnvMapConstantBuffer_t_uint_128 = OpTypeArray %PerBatchEnvMapConstantBuffer_t %uint_128
%_BindlessFastEnvMapCB_PS_t = OpTypeStruct %_arr_PerBatchEnvMapConstantBuffer_t_uint_128
%_ptr_Uniform__BindlessFastEnvMapCB_PS_t = OpTypePointer Uniform %_BindlessFastEnvMapCB_PS_t
          %_ = OpVariable %_ptr_Uniform__BindlessFastEnvMapCB_PS_t Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
%PerViewPushConst_t = OpTypeStruct %uint
%_ptr_PushConstant_PerViewPushConst_t = OpTypePointer PushConstant %PerViewPushConst_t
        %__0 = OpVariable %_ptr_PushConstant_PerViewPushConst_t PushConstant
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
      %int_2 = OpConstant %int 2
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
         %46 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_46 = OpTypePointer UniformConstant %46
   %g_tColor = OpVariable %_ptr_UniformConstant_46 UniformConstant
         %50 = OpTypeSampler
%_ptr_UniformConstant_50 = OpTypePointer UniformConstant %50
   %g_sAniso = OpVariable %_ptr_UniformConstant_50 UniformConstant
         %54 = OpTypeSampledImage %46
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK: %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK: %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK: %v4uint = OpTypeVector %uint 4
;CHECK: [[null_v2float:%\w+]] = OpConstantNull %v2float
;CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
     %MainPs = OpFunction %void None %3
          %5 = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_2 %uint_0
;CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_0 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
         %66 = OpLoad %v2float %i_vTextureCoords
         %79 = OpAccessChain %_ptr_PushConstant_uint %__0 %int_0
         %80 = OpLoad %uint %79
         %81 = OpAccessChain %_ptr_Uniform_v2float %_ %int_0 %80 %int_2
         %82 = OpLoad %v2float %81
         %86 = OpFAdd %v2float %66 %82
;CHECK-NOT: %82 = OpLoad %v2float %81
;CHECK-NOT: %86 = OpFAdd %v2float %66 %82
;CHECK: {{%\w+}} = OpIMul %uint %uint_80 %80
;CHECK: {{%\w+}} = OpIAdd %uint %uint_0 {{%\w+}}
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_64
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_7
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpLoad %v2float %81
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_78 %uint_4 %uint_0 %uint_2 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
;CHECK: %86 = OpFAdd %v2float %66 {{%\w+}}
         %87 = OpLoad %46 %g_tColor
         %88 = OpLoad %50 %g_sAniso
         %89 = OpSampledImage %54 %87 %88
         %91 = OpImageSampleImplicitLod %v4float %89 %86
               OpStore %_entryPointOutput_vColor %91
;CHECK-NOT: %91 = OpImageSampleImplicitLod %v4float %89 %86
;CHECK-NOT:       OpStore %_entryPointOutput_vColor %91
;CHECK: {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpLoad %46 %g_tColor
;CHECK: {{%\w+}} = OpSampledImage %54 {{%\w+}} %88
;CHECK: {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %86
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_83 %uint_1 %uint_0 %uint_0 %uint_0 %uint_0 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
;CHECK: OpStore %_entryPointOutput_vColor {{%\w+}}
               OpReturn
               OpFunctionEnd
)" + kReadDescInit + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, true, true,
                                               true, false, true);
}

TEST_F(InstBindlessTest, Descriptor16BitIdxRef) {
  // Check that descriptor indexed with 16bit index is inbounds and
  // initialized
  //
  // Use Simple source with min16uint g_nDataIdx

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
               OpCapability Int16
               OpCapability StoragePushConstant16
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %MainPs "MainPs" %g_tColor %_ %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor
;CHECK:        OpEntryPoint Fragment %MainPs "MainPs" %g_tColor %_ %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor %inst_bindless_output_buffer %gl_FragCoord %inst_bindless_input_buffer
               OpExecutionMode %MainPs OriginUpperLeft
               OpSource HLSL 500
               OpName %MainPs "MainPs"
               OpName %g_tColor "g_tColor"
               OpName %PerViewConstantBuffer_t "PerViewConstantBuffer_t"
               OpMemberName %PerViewConstantBuffer_t 0 "g_nDataIdx"
               OpName %_ ""
               OpName %g_sAniso "g_sAniso"
               OpName %i_vTextureCoords "i.vTextureCoords"
               OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
               OpDecorate %g_tColor DescriptorSet 1
               OpDecorate %g_tColor Binding 2
               OpMemberDecorate %PerViewConstantBuffer_t 0 Offset 0
               OpDecorate %PerViewConstantBuffer_t Block
               OpDecorate %g_sAniso DescriptorSet 1
               OpDecorate %g_sAniso Binding 2
               OpDecorate %i_vTextureCoords Location 0
               OpDecorate %_entryPointOutput_vColor Location 0
;CHECK:               OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:               OpDecorate %gl_FragCoord BuiltIn FragCoord
)" + kInputDecorations + R"(
       %void = OpTypeVoid
         %10 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
    %v4float = OpTypeVector %float 4
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
         %16 = OpTypeImage %float 2D 0 0 0 1 Unknown
       %uint = OpTypeInt 32 0
   %uint_128 = OpConstant %uint 128
%_arr_16_uint_128 = OpTypeArray %16 %uint_128
%_ptr_UniformConstant__arr_16_uint_128 = OpTypePointer UniformConstant %_arr_16_uint_128
   %g_tColor = OpVariable %_ptr_UniformConstant__arr_16_uint_128 UniformConstant
     %ushort = OpTypeInt 16 0
%PerViewConstantBuffer_t = OpTypeStruct %ushort
%_ptr_PushConstant_PerViewConstantBuffer_t = OpTypePointer PushConstant %PerViewConstantBuffer_t
          %_ = OpVariable %_ptr_PushConstant_PerViewConstantBuffer_t PushConstant
%_ptr_PushConstant_ushort = OpTypePointer PushConstant %ushort
%_ptr_UniformConstant_16 = OpTypePointer UniformConstant %16
         %25 = OpTypeSampler
%_ptr_UniformConstant_25 = OpTypePointer UniformConstant %25
   %g_sAniso = OpVariable %_ptr_UniformConstant_25 UniformConstant
         %27 = OpTypeSampledImage %16
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kOutputGlobals + R"(
;CHECK:%_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:%gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK:        [[null_v4float:%\w+]] = OpConstantNull %v4float
)" + kInputGlobals + R"(
     %MainPs = OpFunction %void None %10
         %30 = OpLabel
;CHECK:               OpBranch {{%\w+}}
;CHECK:        {{%\w+}} = OpLabel
;CHECK:               OpBranch %39
;CHECK:         %39 = OpLabel
         %31 = OpLoad %v2float %i_vTextureCoords
         %32 = OpAccessChain %_ptr_PushConstant_ushort %_ %int_0
         %33 = OpLoad %ushort %32
         %34 = OpAccessChain %_ptr_UniformConstant_16 %g_tColor %33
         %35 = OpLoad %16 %34
         %36 = OpLoad %25 %g_sAniso
         %37 = OpSampledImage %27 %35 %36
         %38 = OpImageSampleImplicitLod %v4float %37 %31
               OpStore %_entryPointOutput_vColor %38
;CHECK-NOT:         %38 = OpImageSampleImplicitLod %v4float %37 %31
;CHECK-NOT:               OpStore %_entryPointOutput_vColor %38
;CHECK:         %41 = OpUConvert %uint %33
;CHECK:         {{%\w+}} = OpULessThan %bool %41 %uint_128
;CHECK:               OpSelectionMerge {{%\w+}} None
;CHECK:               OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK:        {{%\w+}} = OpLabel
;CHECK:        {{%\w+}} = OpLoad %16 %34
;CHECK:        {{%\w+}} = OpSampledImage %27 {{%\w+}} %36
;CHECK:        {{%\w+}} = OpUConvert %uint %33
;CHECK:        {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_1 %uint_2 {{%\w+}}
;CHECK:        {{%\w+}} = OpULessThan %bool %uint_0 {{%\w+}}
;CHECK:               OpSelectionMerge {{%\w+}} None
;CHECK:               OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK:        {{%\w+}} = OpLabel
;CHECK:        {{%\w+}} = OpLoad %16 %34
;CHECK:        {{%\w+}} = OpSampledImage %27 {{%\w+}} %36
;CHECK:        {{%\w+}} = OpImageSampleImplicitLod %v4float {{%\w+}} %31
;CHECK:               OpBranch {{%\w+}}
;CHECK:        {{%\w+}} = OpLabel
;CHECK:        {{%\w+}} = OpUConvert %uint %33
;CHECK:        {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_60 %uint_1 %uint_1 %uint_2 {{%\w+}} %uint_0
;CHECK:               OpBranch {{%\w+}}
;CHECK:        {{%\w+}} = OpLabel
;CHECK:        [[phi_result_1:%\w+]] = OpPhi %v4float {{%\w+}} {{%\w+}} [[null_v4float]] {{%\w+}}
;CHECK:               OpBranch %44
;CHECK:         %46 = OpLabel
;CHECK:        {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_6 %uint_60 %uint_0 %uint_1 %uint_2 %41 %uint_128
;CHECK:               OpBranch %44
;CHECK:         %44 = OpLabel
;CHECK:        [[phi_result_2:%\w+]] = OpPhi %v4float [[phi_result_1]] {{%\w+}} [[null_v4float]] %46
;CHECK:               OpStore %_entryPointOutput_vColor [[phi_result_2]]
               OpReturn
               OpFunctionEnd
)" + kStreamWrite6Frag + kReadDescInit;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, true, true,
                                               false, false, true);
}

TEST_F(InstBindlessTest, UniformArray16bitIdxRef) {
  // Check that uniform array ref with 16bit index does not go out-of-bounds.
  //
  // Texture2D g_tColor;
  // SamplerState g_sAniso;
  //
  // layout(push_constant) cbuffer PerViewPushConst_t { min16uint g_c; };
  //
  // struct PerBatchEnvMapConstantBuffer_t {
  //   float4x3 g_matEnvMapWorldToLocal;
  //   float4 g_vEnvironmentMapBoxMins;
  //   float2 g_TexOff;
  // };
  //
  // cbuffer _BindlessFastEnvMapCB_PS_t {
  //   PerBatchEnvMapConstantBuffer_t g_envMapConstants[128];
  // };
  //
  // struct PS_INPUT {
  //   float2 vTextureCoords : TEXCOORD2;
  // };
  //
  // struct PS_OUTPUT {
  //   float4 vColor : SV_Target0;
  // };
  //
  // PS_OUTPUT MainPs(PS_INPUT i) {
  //   PS_OUTPUT ps_output;
  //   float2 off;
  //   float2 vtc;
  //   off = g_envMapConstants[g_c].g_TexOff;
  //   vtc = i.vTextureCoords.xy + off;
  //   ps_output.vColor = g_tColor.Sample(g_sAniso, vtc);
  //   return ps_output;
  // }

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
               OpCapability Int16
               OpCapability StoragePushConstant16
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor
;CHECK:        OpEntryPoint Fragment %MainPs "MainPs" %_ %__0 %g_tColor %g_sAniso %i_vTextureCoords %_entryPointOutput_vColor %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_FragCoord
               OpExecutionMode %MainPs OriginUpperLeft
               OpSource HLSL 500
               OpName %MainPs "MainPs"
               OpName %PerBatchEnvMapConstantBuffer_t "PerBatchEnvMapConstantBuffer_t"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 0 "g_matEnvMapWorldToLocal"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 1 "g_vEnvironmentMapBoxMins"
               OpMemberName %PerBatchEnvMapConstantBuffer_t 2 "g_TexOff"
               OpName %_BindlessFastEnvMapCB_PS_t "_BindlessFastEnvMapCB_PS_t"
               OpMemberName %_BindlessFastEnvMapCB_PS_t 0 "g_envMapConstants"
               OpName %_ ""
               OpName %PerViewPushConst_t "PerViewPushConst_t"
               OpMemberName %PerViewPushConst_t 0 "g_c"
               OpName %__0 ""
               OpName %g_tColor "g_tColor"
               OpName %g_sAniso "g_sAniso"
               OpName %i_vTextureCoords "i.vTextureCoords"
               OpName %_entryPointOutput_vColor "@entryPointOutput.vColor"
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 RowMajor
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 Offset 0
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 0 MatrixStride 16
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 1 Offset 48
               OpMemberDecorate %PerBatchEnvMapConstantBuffer_t 2 Offset 64
               OpDecorate %_arr_PerBatchEnvMapConstantBuffer_t_uint_128 ArrayStride 80
               OpMemberDecorate %_BindlessFastEnvMapCB_PS_t 0 Offset 0
               OpDecorate %_BindlessFastEnvMapCB_PS_t Block
               OpDecorate %_ DescriptorSet 0
               OpDecorate %_ Binding 0
               OpMemberDecorate %PerViewPushConst_t 0 Offset 0
               OpDecorate %PerViewPushConst_t Block
               OpDecorate %g_tColor DescriptorSet 0
               OpDecorate %g_tColor Binding 0
               OpDecorate %g_sAniso DescriptorSet 0
               OpDecorate %g_sAniso Binding 0
               OpDecorate %i_vTextureCoords Location 0
               OpDecorate %_entryPointOutput_vColor Location 0
;CHECK:               OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + kOutputDecorations + R"(
;CHECK:               OpDecorate %gl_FragCoord BuiltIn FragCoord
       %void = OpTypeVoid
         %14 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
    %v4float = OpTypeVector %float 4
    %v3float = OpTypeVector %float 3
%mat4v3float = OpTypeMatrix %v3float 4
%PerBatchEnvMapConstantBuffer_t = OpTypeStruct %mat4v3float %v4float %v2float
       %uint = OpTypeInt 32 0
   %uint_128 = OpConstant %uint 128
%_arr_PerBatchEnvMapConstantBuffer_t_uint_128 = OpTypeArray %PerBatchEnvMapConstantBuffer_t %uint_128
%_BindlessFastEnvMapCB_PS_t = OpTypeStruct %_arr_PerBatchEnvMapConstantBuffer_t_uint_128
%_ptr_Uniform__BindlessFastEnvMapCB_PS_t = OpTypePointer Uniform %_BindlessFastEnvMapCB_PS_t
          %_ = OpVariable %_ptr_Uniform__BindlessFastEnvMapCB_PS_t Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
     %ushort = OpTypeInt 16 0
%PerViewPushConst_t = OpTypeStruct %ushort
%_ptr_PushConstant_PerViewPushConst_t = OpTypePointer PushConstant %PerViewPushConst_t
        %__0 = OpVariable %_ptr_PushConstant_PerViewPushConst_t PushConstant
%_ptr_PushConstant_ushort = OpTypePointer PushConstant %ushort
      %int_2 = OpConstant %int 2
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
         %30 = OpTypeImage %float 2D 0 0 0 1 Unknown
%_ptr_UniformConstant_30 = OpTypePointer UniformConstant %30
   %g_tColor = OpVariable %_ptr_UniformConstant_30 UniformConstant
         %32 = OpTypeSampler
%_ptr_UniformConstant_32 = OpTypePointer UniformConstant %32
   %g_sAniso = OpVariable %_ptr_UniformConstant_32 UniformConstant
         %34 = OpTypeSampledImage %30
%_ptr_Input_v2float = OpTypePointer Input %v2float
%i_vTextureCoords = OpVariable %_ptr_Input_v2float Input
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_entryPointOutput_vColor = OpVariable %_ptr_Output_v4float Output
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK:%_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:%gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK:        [[null_v2float:%\w+]] = OpConstantNull %v2float
     %MainPs = OpFunction %void None %14
         %37 = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_0 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
         %38 = OpLoad %v2float %i_vTextureCoords
         %39 = OpAccessChain %_ptr_PushConstant_ushort %__0 %int_0
         %40 = OpLoad %ushort %39
         %41 = OpAccessChain %_ptr_Uniform_v2float %_ %int_0 %40 %int_2
         %42 = OpLoad %v2float %41
         %43 = OpFAdd %v2float %38 %42
;CHECK-NOT:     %42 = OpLoad %v2float %41
;CHECK-NOT:     %43 = OpFAdd %v2float %38 %42
;CHECK: {{%\w+}} = OpUConvert %uint %40
;CHECK: {{%\w+}} = OpIMul %uint %uint_80 {{%\w+}}
;CHECK: {{%\w+}} = OpIAdd %uint %uint_0 {{%\w+}}
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_64
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_7
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpLoad %v2float %41
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_81 %uint_4 %uint_0 %uint_0 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpPhi %v2float {{%\w+}} {{%\w+}} [[null_v2float]] {{%\w+}}
;CHECK: %43 = OpFAdd %v2float %38 {{%\w+}}
         %44 = OpLoad %30 %g_tColor
         %45 = OpLoad %32 %g_sAniso
         %46 = OpSampledImage %34 %44 %45
         %47 = OpImageSampleImplicitLod %v4float %46 %43
               OpStore %_entryPointOutput_vColor %47
               OpReturn
               OpFunctionEnd
               )" + kReadDescInit + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, UniformMatrixRefRowMajor) {
  // The buffer-oob row major matrix check
  //
  // #version 450
  // #extension GL_EXT_scalar_block_layout : enable
  //
  // layout(location = 0) in highp vec4 a_position;
  // layout(location = 0) out mediump float v_vtxResult;
  //
  // layout(set = 0, binding = 0, std430, row_major) uniform Block
  // {
  //    lowp mat4x2 var;
  // };
  //
  // void main (void)
  // {
  //    v_vtxResult = var[2][1];
  // }

  // clang-format off
  std::string text = R"(
               OpCapability Shader
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position
;CHECK:        OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_VertexIndex %gl_InstanceIndex
               OpSource GLSL 450
               OpSourceExtension "GL_EXT_scalar_block_layout"
               OpName %main "main"
               OpName %v_vtxResult "v_vtxResult"
               OpName %Block "Block"
               OpMemberName %Block 0 "var"
               OpName %_ ""
               OpName %a_position "a_position"
               OpDecorate %v_vtxResult RelaxedPrecision
               OpDecorate %v_vtxResult Location 0
               OpMemberDecorate %Block 0 RowMajor
               OpMemberDecorate %Block 0 RelaxedPrecision
               OpMemberDecorate %Block 0 Offset 0
               OpMemberDecorate %Block 0 MatrixStride 16
               OpDecorate %Block Block
               OpDecorate %_ DescriptorSet 0
               OpDecorate %_ Binding 0
               OpDecorate %21 RelaxedPrecision
;CHECK-NOT:           OpDecorate %21 RelaxedPrecision
;CHECK: OpDecorate %v_vtxResult RelaxedPrecision
;CHECK: OpDecorate [[phi_result:%\w+]] RelaxedPrecision
               OpDecorate %a_position Location 0
;CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + R"(
;CHECK: OpDecorate [[load_result:%\w+]] RelaxedPrecision
)" + kOutputDecorations + R"(
;CHECK: OpDecorate %gl_VertexIndex BuiltIn VertexIndex
;CHECK: OpDecorate %gl_InstanceIndex BuiltIn InstanceIndex
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%v_vtxResult = OpVariable %_ptr_Output_float Output
    %v2float = OpTypeVector %float 2
%mat4v2float = OpTypeMatrix %v2float 4
      %Block = OpTypeStruct %mat4v2float
%_ptr_Uniform_Block = OpTypePointer Uniform %Block
          %_ = OpVariable %_ptr_Uniform_Block Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
      %int_2 = OpConstant %int 2
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_ptr_Uniform_float = OpTypePointer Uniform %float
    %v4float = OpTypeVector %float 4
%_ptr_Input_v4float = OpTypePointer Input %v4float
 %a_position = OpVariable %_ptr_Input_v4float Input
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK:%_ptr_Input_uint = OpTypePointer Input %uint
;CHECK:%gl_VertexIndex = OpVariable %_ptr_Input_uint Input
;CHECK:%gl_InstanceIndex = OpVariable %_ptr_Input_uint Input
;CHECK: [[null_float:%\w+]] = OpConstantNull %float
       %main = OpFunction %void None %3
          %5 = OpLabel
;CHECK: [[desc_state:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_0 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
         %20 = OpAccessChain %_ptr_Uniform_float %_ %int_0 %int_2 %uint_1
         %21 = OpLoad %float %20
;CHECK-NOT:     %21 = OpLoad %float %20
;CHECK: {{%\w+}} = OpIMul %uint %uint_4 %int_2
;CHECK: {{%\w+}} = OpIAdd %uint %uint_0 {{%\w+}}
;CHECK: {{%\w+}} = OpIMul %uint %uint_16 %uint_1
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpIAdd %uint {{%\w+}} %uint_3
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} [[desc_state]]
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[load_result]] = OpLoad %float %20
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_45 %uint_4 %uint_0 %uint_0 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[phi_result]] = OpPhi %float [[load_result]] {{%\w+}} [[null_float]] {{%\w+}}
               OpStore %v_vtxResult %21
;CHECK-NOT:    OpStore %v_vtxResult %21
;CHECK:  OpStore %v_vtxResult [[phi_result]]
               OpReturn
               OpFunctionEnd
               )" + kReadDescInit + kStreamWrite7Vert;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, UniformMatrixRefColumnMajor) {
  // The buffer-oob column major matrix check
  //
  // #version 450
  // #extension GL_EXT_scalar_block_layout : enable
  //
  // layout(location = 0) in highp vec4 a_position;
  // layout(location = 0) out mediump float v_vtxResult;
  //
  // layout(set = 0, binding = 0, std430, column_major) uniform Block
  // {
  //    lowp mat4x2 var;
  // };
  //
  // void main (void)
  // {
  //    v_vtxResult = var[2][1];
  // }

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position
;CHECK:        OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_VertexIndex %gl_InstanceIndex
               OpSource GLSL 450
               OpSourceExtension "GL_EXT_scalar_block_layout"
               OpName %main "main"
               OpName %v_vtxResult "v_vtxResult"
               OpName %Block "Block"
               OpMemberName %Block 0 "var"
               OpName %_ ""
               OpName %a_position "a_position"
               OpDecorate %v_vtxResult RelaxedPrecision
               OpDecorate %v_vtxResult Location 0
               OpMemberDecorate %Block 0 ColMajor
               OpMemberDecorate %Block 0 RelaxedPrecision
               OpMemberDecorate %Block 0 Offset 0
               OpMemberDecorate %Block 0 MatrixStride 8
               OpDecorate %Block Block
               OpDecorate %_ DescriptorSet 0
               OpDecorate %_ Binding 0
               OpDecorate %21 RelaxedPrecision
;CHECK-NOT:    OpDecorate %21 RelaxedPrecision
;CHECK: OpDecorate %v_vtxResult RelaxedPrecision
;CHECK: OpDecorate [[phi_result:%\w+]] RelaxedPrecision
               OpDecorate %a_position Location 0
;CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + R"(
;CHECK: OpDecorate [[load_result:%\w+]] RelaxedPrecision
)" + kOutputDecorations + R"(
;CHECK: OpDecorate %gl_VertexIndex BuiltIn VertexIndex
;CHECK: OpDecorate %gl_InstanceIndex BuiltIn InstanceIndex
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
%_ptr_Output_float = OpTypePointer Output %float
%v_vtxResult = OpVariable %_ptr_Output_float Output
    %v2float = OpTypeVector %float 2
%mat4v2float = OpTypeMatrix %v2float 4
      %Block = OpTypeStruct %mat4v2float
%_ptr_Uniform_Block = OpTypePointer Uniform %Block
          %_ = OpVariable %_ptr_Uniform_Block Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
      %int_2 = OpConstant %int 2
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_ptr_Uniform_float = OpTypePointer Uniform %float
    %v4float = OpTypeVector %float 4
%_ptr_Input_v4float = OpTypePointer Input %v4float
 %a_position = OpVariable %_ptr_Input_v4float Input
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK:%_ptr_Input_uint = OpTypePointer Input %uint
;CHECK:%gl_VertexIndex = OpVariable %_ptr_Input_uint Input
;CHECK:%gl_InstanceIndex = OpVariable %_ptr_Input_uint Input
;CHECK: [[null_float:%\w+]] = OpConstantNull %float
%main = OpFunction %void None %3
          %5 = OpLabel
;CHECK: [[desc_state:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_0 %uint_0
;CHECK: OpBranch %26
;CHECK: %26 = OpLabel
;CHECK: OpBranch %25
;CHECK: %25 = OpLabel
         %20 = OpAccessChain %_ptr_Uniform_float %_ %int_0 %int_2 %uint_1
         %21 = OpLoad %float %20
;CHECK-NOT:     %21 = OpLoad %float %20
;CHECK: %29 = OpIMul %uint %uint_8 %int_2
;CHECK: %30 = OpIAdd %uint %uint_0 %29
;CHECK: %32 = OpIMul %uint %uint_4 %uint_1
;CHECK: %33 = OpIAdd %uint %30 %32
;CHECK: %35 = OpIAdd %uint %33 %uint_3
;CHECK: {{%\w+}} = OpULessThan %bool %35 [[desc_state]]
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK:[[load_result]] = OpLoad %float %20
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_45 %uint_4 %uint_0 %uint_0 %uint_0 %35 {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[phi_result]] = OpPhi %float [[load_result]] {{%\w+}} [[null_float]] {{%\w+}}
               OpStore %v_vtxResult %21
;CHECK-NOT:           OpStore %v_vtxResult %21
;CHECK: OpStore %v_vtxResult [[phi_result]]
               OpReturn
               OpFunctionEnd
               )" + kReadDescInit + kStreamWrite7Vert;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  ValidatorOptions()->uniform_buffer_standard_layout = true;
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, UniformMatrixVecRefRowMajor) {
  // The buffer-oob row major matrix vector ref check
  //
  // #version 450
  // #extension GL_EXT_scalar_block_layout : enable
  //
  // layout(location = 0) in highp vec4 a_position;
  // layout(location = 0) out highp vec2 v_vtxResult;
  //
  // layout(set = 3, binding = 7, std430, row_major) uniform Block
  // {
  //    lowp mat2 var[3][4];
  // };
  //
  // void main (void)
  // {
  //    v_vtxResult = var[2][3][1];
  // }

  // clang-format off
  const std::string text = R"(
               OpCapability Shader
;CHECK:               OpExtension "SPV_KHR_storage_buffer_storage_class"
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position
;CHECK:        OpEntryPoint Vertex %main "main" %v_vtxResult %_ %a_position %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_VertexIndex %gl_InstanceIndex
               OpSource GLSL 450
               OpSourceExtension "GL_EXT_scalar_block_layout"
               OpName %main "main"
               OpName %v_vtxResult "v_vtxResult"
               OpName %Block "Block"
               OpMemberName %Block 0 "var"
               OpName %_ ""
               OpName %a_position "a_position"
               OpDecorate %v_vtxResult Location 0
               OpDecorate %_arr_mat2v2float_uint_4 ArrayStride 32
               OpDecorate %_arr__arr_mat2v2float_uint_4_uint_3 ArrayStride 128
               OpMemberDecorate %Block 0 RowMajor
               OpMemberDecorate %Block 0 RelaxedPrecision
               OpMemberDecorate %Block 0 Offset 0
               OpMemberDecorate %Block 0 MatrixStride 16
               OpDecorate %Block Block
               OpDecorate %_ DescriptorSet 3
               OpDecorate %_ Binding 7
               OpDecorate %26 RelaxedPrecision
;CHECK-NOT:               OpDecorate %26 RelaxedPrecision
;CHECK: OpDecorate [[phi_result:%\w+]] RelaxedPrecision
               OpDecorate %a_position Location 0
;CHECK: OpDecorate %_runtimearr_uint ArrayStride 4
)" + kInputDecorations + R"(
;CHECK: OpDecorate [[load_result:%\w+]] RelaxedPrecision
)" + kOutputDecorations + R"(
;CHECK: OpDecorate %gl_VertexIndex BuiltIn VertexIndex
;CHECK: OpDecorate %gl_InstanceIndex BuiltIn InstanceIndex
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v2float = OpTypeVector %float 2
%_ptr_Output_v2float = OpTypePointer Output %v2float
%v_vtxResult = OpVariable %_ptr_Output_v2float Output
%mat2v2float = OpTypeMatrix %v2float 2
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_mat2v2float_uint_4 = OpTypeArray %mat2v2float %uint_4
     %uint_3 = OpConstant %uint 3
%_arr__arr_mat2v2float_uint_4_uint_3 = OpTypeArray %_arr_mat2v2float_uint_4 %uint_3
      %Block = OpTypeStruct %_arr__arr_mat2v2float_uint_4_uint_3
%_ptr_Uniform_Block = OpTypePointer Uniform %Block
          %_ = OpVariable %_ptr_Uniform_Block Uniform
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
      %int_2 = OpConstant %int 2
      %int_3 = OpConstant %int 3
      %int_1 = OpConstant %int 1
%_ptr_Uniform_v2float = OpTypePointer Uniform %v2float
    %v4float = OpTypeVector %float 4
%_ptr_Input_v4float = OpTypePointer Input %v4float
 %a_position = OpVariable %_ptr_Input_v4float Input
)" + kInputGlobals + kOutputGlobals + R"(
;CHECK:%_ptr_Input_uint = OpTypePointer Input %uint
;CHECK:%gl_VertexIndex = OpVariable %_ptr_Input_uint Input
;CHECK:%gl_InstanceIndex = OpVariable %_ptr_Input_uint Input
;CHECK: [[null_v2float:%\w+]] = OpConstantNull %v2float
       %main = OpFunction %void None %3
          %5 = OpLabel
;CHECK: [[desc_state:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_3 %uint_7 %uint_0
;CHECK: OpBranch %31
;CHECK: %31 = OpLabel
;CHECK: OpBranch %30
;CHECK: %30 = OpLabel
         %25 = OpAccessChain %_ptr_Uniform_v2float %_ %int_0 %int_2 %int_3 %int_1
         %26 = OpLoad %v2float %25
               OpStore %v_vtxResult %26
;CHECK-NOT: %26 = OpLoad %v2float %25
;CHECK-NOT:    OpStore %v_vtxResult %26
;CHECK: %34 = OpIMul %uint %uint_128 %int_2
;CHECK: %35 = OpIAdd %uint %uint_0 %34
;CHECK: %37 = OpIMul %uint %uint_32 %int_3
;CHECK: %38 = OpIAdd %uint %35 %37
;CHECK: %40 = OpIMul %uint %uint_4 %int_1
;CHECK: %41 = OpIAdd %uint %38 %40
;CHECK: %43 = OpIAdd %uint %41 %uint_19
;CHECK: {{%\w+}} = OpULessThan %bool %43 [[desc_state]]
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[load_result]] = OpLoad %v2float %25
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_51 %uint_4 %uint_3 %uint_7 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[phi_result]] = OpPhi %v2float [[load_result]] {{%\w+}} [[null_v2float]] {{%\w+}}
;CHECK: OpStore %v_vtxResult [[phi_result]]
               OpReturn
               OpFunctionEnd
               )" + kReadDescInit + kStreamWrite7Vert;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, false, true);
}

TEST_F(InstBindlessTest, ImageBufferOOBRead) {
  // Texel buffer (imagebuffer) oob check for ImageRead
  //
  // #version 450
  // layout(set=3, binding=7, r32f) uniform readonly imageBuffer s;
  // layout(location=11) out vec4 x;
  // layout(location=13) in flat int ii;
  //
  // void main(){
  //    x = imageLoad(s, ii);
  // }

  // clang-format off
  const std::string text = R"(
                          OpCapability Shader
                          OpCapability ImageBuffer
;CHECK:                   OpCapability ImageQuery
;CHECK:                   OpExtension "SPV_KHR_storage_buffer_storage_class"
                     %1 = OpExtInstImport "GLSL.std.450"
                          OpMemoryModel Logical GLSL450
                          OpEntryPoint Fragment %main "main" %x %s %ii
                          OpExecutionMode %main OriginUpperLeft
                          OpSource GLSL 450
                          OpName %main "main"
                          OpName %x "x"
                          OpName %s "s"
                          OpName %ii "ii"
                          OpDecorate %x Location 11
                          OpDecorate %s DescriptorSet 3
                          OpDecorate %s Binding 7
                          OpDecorate %s NonWritable
                          OpDecorate %ii Flat
                          OpDecorate %ii Location 13
;CHECK:                   OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:                   OpDecorate %gl_FragCoord BuiltIn FragCoord
                  %void = OpTypeVoid
                     %3 = OpTypeFunction %void
                 %float = OpTypeFloat 32
               %v4float = OpTypeVector %float 4
           %_ptr_Output_v4float = OpTypePointer Output %v4float
                     %x = OpVariable %_ptr_Output_v4float Output
                    %10 = OpTypeImage %float Buffer 0 0 0 2 R32f
           %_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
                     %s = OpVariable %_ptr_UniformConstant_10 UniformConstant
                   %int = OpTypeInt 32 1
           %_ptr_Input_int = OpTypePointer Input %int
                    %ii = OpVariable %_ptr_Input_int Input
;CHECK:           %uint = OpTypeInt 32 0
;CHECK:           %bool = OpTypeBool
;CHECK:    %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
;CHECK:    %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
;CHECK:    %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:    %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
                  %main = OpFunction %void None %3
                     %5 = OpLabel
;CHECK:                   OpBranch %21
;CHECK:             %21 = OpLabel
;CHECK:                   OpBranch %20
;CHECK:             %20 = OpLabel
;CHECK:                   OpBranch %19
;CHECK:             %19 = OpLabel
                    %13 = OpLoad %10 %s
                    %17 = OpLoad %int %ii
                    %18 = OpImageRead %v4float %13 %17
                          OpStore %x %18
;CHECK-NOT:         %18 = OpImageRead %v4float %13 %17
;CHECK-NOT:               OpStore %x %18
;CHECK:             %23 = OpBitcast %uint %17
;CHECK:             %25 = OpImageQuerySize %uint %13
;CHECK:             %27 = OpULessThan %bool %23 %25
;CHECK:                   OpSelectionMerge %29 None
;CHECK:                   OpBranchConditional %27 %30 %31
;CHECK:             %30 = OpLabel
;CHECK:             %32 = OpLoad %10 %s
;CHECK:             %33 = OpImageRead %v4float %32 %17
;CHECK:                   OpBranch %29
;CHECK:             %31 = OpLabel
;CHECK:             {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_33 %uint_7 %uint_3 %uint_7 %uint_0 %23 %25
;CHECK:                   OpBranch %29
;CHECK:             %29 = OpLabel
;CHECK:             [[phi_result:%\w+]] = OpPhi %v4float %33 %30 [[null_v4float]] %31
;CHECK:                   OpStore %x [[phi_result]]
                          OpReturn
                          OpFunctionEnd
                          )" + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, true, true);
}

TEST_F(InstBindlessTest, ImageBufferOOBWrite) {
  // Texel buffer (imagebuffer) oob check for ImageWrite
  //
  // #version 450
  // layout(set=3, binding=7, r32f) uniform readonly imageBuffer s;
  // layout(location=11) out vec4 x;
  // layout(location=13) in flat int ii;
  //
  // void main(){
  //    imageStore(s, ii, x);
  // }

  // clang-format off
  const std::string text = R"(
                          OpCapability Shader
                          OpCapability ImageBuffer
;CHECK:                   OpCapability ImageQuery
;CHECK:                   OpExtension "SPV_KHR_storage_buffer_storage_class"
                     %1 = OpExtInstImport "GLSL.std.450"
                          OpMemoryModel Logical GLSL450
                          OpEntryPoint Fragment %main "main" %s %ii %x
;CHECK:                   OpEntryPoint Fragment %main "main" %s %ii %x %inst_bindless_output_buffer %gl_FragCoord
                          OpExecutionMode %main OriginUpperLeft
                          OpSource GLSL 450
                          OpName %main "main"
                          OpName %s "s"
                          OpName %ii "ii"
                          OpName %x "x"
                          OpDecorate %s DescriptorSet 3
                          OpDecorate %s Binding 7
                          OpDecorate %s NonReadable
                          OpDecorate %ii Flat
                          OpDecorate %ii Location 13
                          OpDecorate %x Location 11
;CHECK:                   OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:                   OpDecorate %gl_FragCoord BuiltIn FragCoord
                  %void = OpTypeVoid
                     %3 = OpTypeFunction %void
                 %float = OpTypeFloat 32
                     %7 = OpTypeImage %float Buffer 0 0 0 2 R32f
           %_ptr_UniformConstant_7 = OpTypePointer UniformConstant %7
                     %s = OpVariable %_ptr_UniformConstant_7 UniformConstant
                   %int = OpTypeInt 32 1
           %_ptr_Input_int = OpTypePointer Input %int
                    %ii = OpVariable %_ptr_Input_int Input
               %v4float = OpTypeVector %float 4
           %_ptr_Output_v4float = OpTypePointer Output %v4float
                     %x = OpVariable %_ptr_Output_v4float Output
;CHECK:           %uint = OpTypeInt 32 0
;CHECK:           %bool = OpTypeBool
;CHECK:        {{%\w+}} = OpTypeFunction %void %uint %uint %uint %uint %uint
;CHECK:    %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
;CHECK:    %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
;CHECK:    %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:    %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK:         %v4uint = OpTypeVector %uint 4
                  %main = OpFunction %void None %3
                     %5 = OpLabel
;CHECK:                   OpBranch %21
;CHECK:             %21 = OpLabel
;CHECK:                   OpBranch %20
;CHECK:             %20 = OpLabel
;CHECK:                   OpBranch %19
;CHECK:             %19 = OpLabel
                    %10 = OpLoad %7 %s
                    %14 = OpLoad %int %ii
                    %18 = OpLoad %v4float %x
                          OpImageWrite %10 %14 %18
;CHECK-NOT:               OpImageWrite %10 %14 %18
;CHECK:             %23 = OpBitcast %uint %14
;CHECK:             %25 = OpImageQuerySize %uint %10
;CHECK:             %27 = OpULessThan %bool %23 %25
;CHECK:                   OpSelectionMerge %29 None
;CHECK:                   OpBranchConditional %27 %30 %31
;CHECK:             %30 = OpLabel
;CHECK:             %32 = OpLoad %7 %s
;CHECK:                   OpImageWrite %32 %14 %18
;CHECK:                   OpBranch %29
;CHECK:             %31 = OpLabel
;CHECK:             {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_34 %uint_7 %uint_3 %uint_7 %uint_0 %23 %25
;CHECK:                   OpBranch %29
;CHECK:             %29 = OpLabel
                          OpReturn
                          OpFunctionEnd
                          )" + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, true, true);
}

TEST_F(InstBindlessTest, TextureBufferOOBFetch) {
  // Texel buffer (texturebuffer) oob check for ImageFetch
  //
  // #version 450
  // layout(set=3, binding=7) uniform textureBuffer s;
  // layout(location=11) out vec4 x;
  // layout(location=13) in flat int ii;
  //
  // void main(){
  //    x = texelFetch(s, ii);
  // }

  // clang-format off
  const std::string text = R"(
                          OpCapability Shader
                          OpCapability SampledBuffer
;CHECK:                   OpCapability ImageQuery
;CHECK:                   OpExtension "SPV_KHR_storage_buffer_storage_class"
                     %1 = OpExtInstImport "GLSL.std.450"
                          OpMemoryModel Logical GLSL450
                          OpEntryPoint Fragment %main "main" %x %s %ii
;CHECK:                   OpEntryPoint Fragment %main "main" %x %s %ii %inst_bindless_output_buffer %gl_FragCoord
                          OpExecutionMode %main OriginUpperLeft
                          OpSource GLSL 450
                          OpName %main "main"
                          OpName %x "x"
                          OpName %s "s"
                          OpName %ii "ii"
                          OpDecorate %x Location 11
                          OpDecorate %s DescriptorSet 3
                          OpDecorate %s Binding 7
                          OpDecorate %ii Flat
                          OpDecorate %ii Location 13
;CHECK:                   OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:                   OpDecorate %gl_FragCoord BuiltIn FragCoord
                  %void = OpTypeVoid
                     %3 = OpTypeFunction %void
                 %float = OpTypeFloat 32
               %v4float = OpTypeVector %float 4
           %_ptr_Output_v4float = OpTypePointer Output %v4float
                     %x = OpVariable %_ptr_Output_v4float Output
                    %10 = OpTypeImage %float Buffer 0 0 0 1 Unknown
           %_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
                     %s = OpVariable %_ptr_UniformConstant_10 UniformConstant
                   %int = OpTypeInt 32 1
           %_ptr_Input_int = OpTypePointer Input %int
                    %ii = OpVariable %_ptr_Input_int Input
;CHECK:           %uint = OpTypeInt 32 0
;CHECK:           %bool = OpTypeBool
;CHECK:    %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
;CHECK:    %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
;CHECK:    %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:    %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
                  %main = OpFunction %void None %3
                     %5 = OpLabel
;CHECK:                   OpBranch %21
;CHECK:             %21 = OpLabel
;CHECK:                   OpBranch %20
;CHECK:             %20 = OpLabel
;CHECK:                   OpBranch %19
;CHECK:             %19 = OpLabel
                    %13 = OpLoad %10 %s
                    %17 = OpLoad %int %ii
                    %18 = OpImageFetch %v4float %13 %17
                          OpStore %x %18
;CHECK-NOT:         %18 = OpImageFetch %v4float %13 %17
;CHECK-NOT:               OpStore %x %18
;CHECK:             %23 = OpBitcast %uint %17
;CHECK:             %25 = OpImageQuerySize %uint %13
;CHECK:             %27 = OpULessThan %bool %23 %25
;CHECK:                   OpSelectionMerge %29 None
;CHECK:                   OpBranchConditional %27 %30 %31
;CHECK:             %30 = OpLabel
;CHECK:             %32 = OpLoad %10 %s
;CHECK:             %33 = OpImageFetch %v4float %32 %17
;CHECK:                   OpBranch %29
;CHECK:             %31 = OpLabel
;CHECK:             {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_32 %uint_6 %uint_3 %uint_7 %uint_0 %23 %25
;CHECK:                   OpBranch %29
;CHECK:             %29 = OpLabel
;CHECK:             [[phi_result:%\w+]] = OpPhi %v4float %33 %30 [[null_v4float]] %31
;CHECK:                   OpStore %x [[phi_result]]
                          OpReturn
                          OpFunctionEnd
                          )" + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, true, true);
}

TEST_F(InstBindlessTest, SamplerBufferOOBFetch) {
  // Texel buffer (samplerbuffer) oob check for ImageFetch
  //
  // #version 450
  // layout(set=3, binding=7) uniform samplerBuffer s;
  // layout(location=11) out vec4 x;
  // layout(location=13) in flat int ii;
  //
  // void main(){
  //    x = texelFetch(s, ii);
  // }

  // clang-format off
  const std::string text = R"(
                          OpCapability Shader
                          OpCapability SampledBuffer
;CHECK:                   OpCapability ImageQuery
;CHECK:                   OpExtension "SPV_KHR_storage_buffer_storage_class"
                     %1 = OpExtInstImport "GLSL.std.450"
                          OpMemoryModel Logical GLSL450
                          OpEntryPoint Fragment %main "main" %x %s %ii
;CHECK:                   OpEntryPoint Fragment %main "main" %x %s %ii %inst_bindless_output_buffer %gl_FragCoord
                          OpExecutionMode %main OriginUpperLeft
                          OpSource GLSL 450
                          OpName %main "main"
                          OpName %x "x"
                          OpName %s "s"
                          OpName %ii "ii"
                          OpDecorate %x Location 11
                          OpDecorate %s DescriptorSet 3
                          OpDecorate %s Binding 7
                          OpDecorate %ii Flat
                          OpDecorate %ii Location 13
;CHECK:                   OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:                   OpDecorate %gl_FragCoord BuiltIn FragCoord
                  %void = OpTypeVoid
                     %3 = OpTypeFunction %void
                 %float = OpTypeFloat 32
               %v4float = OpTypeVector %float 4
           %_ptr_Output_v4float = OpTypePointer Output %v4float
                     %x = OpVariable %_ptr_Output_v4float Output
                    %10 = OpTypeImage %float Buffer 0 0 0 1 Unknown
                    %11 = OpTypeSampledImage %10
           %_ptr_UniformConstant_11 = OpTypePointer UniformConstant %11
                     %s = OpVariable %_ptr_UniformConstant_11 UniformConstant
                   %int = OpTypeInt 32 1
           %_ptr_Input_int = OpTypePointer Input %int
                    %ii = OpVariable %_ptr_Input_int Input
;CHECK:           %uint = OpTypeInt 32 0
;CHECK:           %bool = OpTypeBool
;CHECK:    %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
;CHECK:    %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
;CHECK:    %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:    %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK: [[null_v4float:%\w+]] = OpConstantNull %v4float
                  %main = OpFunction %void None %3
                     %5 = OpLabel
;CHECK:                   OpBranch %23
;CHECK:             %23 = OpLabel
;CHECK:                   OpBranch %22
;CHECK:             %22 = OpLabel
;CHECK:                   OpBranch %21
;CHECK:             %21 = OpLabel
                    %14 = OpLoad %11 %s
                    %18 = OpLoad %int %ii
                    %19 = OpImage %10 %14
                    %20 = OpImageFetch %v4float %19 %18
                          OpStore %x %20
;CHECK-NOT:         %20 = OpImageFetch %v4float %19 %18
;CHECK-NOT:               OpStore %x %20
;CHECK:             %25 = OpBitcast %uint %18
;CHECK:             %27 = OpImageQuerySize %uint %19
;CHECK:             %29 = OpULessThan %bool %25 %27
;CHECK:                   OpSelectionMerge %31 None
;CHECK:                   OpBranchConditional %29 %32 %33
;CHECK:             %32 = OpLabel
;CHECK:             %34 = OpLoad %11 %s
;CHECK:             %35 = OpImage %10 %34
;CHECK:             %36 = OpImageFetch %v4float %35 %18
;CHECK:                   OpBranch %31
;CHECK:             %33 = OpLabel
;CHECK:             {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_34 %uint_6 %uint_3 %uint_7 %uint_0 %25 %27
;CHECK:                   OpBranch %31
;CHECK:             %31 = OpLabel
;CHECK:             [[phi_result:%\w+]] = OpPhi %v4float %36 %32 [[null_v4float]] %33
;CHECK:                   OpStore %x [[phi_result]]
                          OpReturn
                          OpFunctionEnd
                          )" + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, true, true);
}

TEST_F(InstBindlessTest, SamplerBufferConstructorOOBFetch) {
  // Texel buffer (samplerbuffer constructor) oob check for ImageFetch
  //
  // #version 450
  // layout(set=3, binding=7) uniform textureBuffer tBuf;
  // layout(set=3, binding=8) uniform sampler s;
  // layout(location=11) out vec4 x;
  // layout(location=13) in flat int ii;
  //
  // void main(){
  //    x = texelFetch(samplerBuffer(tBuf, s), ii);
  // }

  // clang-format off
  const std::string text = R"(
                          OpCapability Shader
                          OpCapability SampledBuffer
;CHECK:                   OpCapability ImageQuery
;CHECK:                   OpExtension "SPV_KHR_storage_buffer_storage_class"
                     %1 = OpExtInstImport "GLSL.std.450"
                          OpMemoryModel Logical GLSL450
                          OpEntryPoint Fragment %main "main" %x %tBuf %s %ii
;CHECK:                   OpEntryPoint Fragment %main "main" %x %tBuf %s %ii %inst_bindless_output_buffer %gl_FragCoord
                          OpExecutionMode %main OriginUpperLeft
                          OpSource GLSL 450
                          OpName %main "main"
                          OpName %x "x"
                          OpName %tBuf "tBuf"
                          OpName %s "s"
                          OpName %ii "ii"
                          OpDecorate %x Location 11
                          OpDecorate %tBuf DescriptorSet 3
                          OpDecorate %tBuf Binding 7
                          OpDecorate %s DescriptorSet 3
                          OpDecorate %s Binding 8
                          OpDecorate %ii Flat
                          OpDecorate %ii Location 13
;CHECK:                   OpDecorate %_runtimearr_uint ArrayStride 4
)" + kOutputDecorations + R"(
;CHECK:                   OpDecorate %gl_FragCoord BuiltIn FragCoord
                  %void = OpTypeVoid
                     %3 = OpTypeFunction %void
                 %float = OpTypeFloat 32
               %v4float = OpTypeVector %float 4
           %_ptr_Output_v4float = OpTypePointer Output %v4float
                     %x = OpVariable %_ptr_Output_v4float Output
                    %10 = OpTypeImage %float Buffer 0 0 0 1 Unknown
           %_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
                  %tBuf = OpVariable %_ptr_UniformConstant_10 UniformConstant
                    %14 = OpTypeSampler
           %_ptr_UniformConstant_14 = OpTypePointer UniformConstant %14
                     %s = OpVariable %_ptr_UniformConstant_14 UniformConstant
                    %18 = OpTypeSampledImage %10
                   %int = OpTypeInt 32 1
           %_ptr_Input_int = OpTypePointer Input %int
                    %ii = OpVariable %_ptr_Input_int Input
;CHECK:           %uint = OpTypeInt 32 0
;CHECK:           %bool = OpTypeBool
;CHECK:    %_runtimearr_uint = OpTypeRuntimeArray %uint
)" + kOutputGlobals + R"(
;CHECK:    %_ptr_StorageBuffer_uint = OpTypePointer StorageBuffer %uint
;CHECK:    %_ptr_Input_v4float = OpTypePointer Input %v4float
;CHECK:    %gl_FragCoord = OpVariable %_ptr_Input_v4float Input
;CHECK:            [[null_v4float:%\w+]] = OpConstantNull %v4float
                  %main = OpFunction %void None %3
                     %5 = OpLabel
;CHECK:                   OpBranch %28
;CHECK:             %28 = OpLabel
;CHECK:                   OpBranch %27
;CHECK:             %27 = OpLabel
;CHECK:                   OpBranch %26
;CHECK:             %26 = OpLabel
                    %13 = OpLoad %10 %tBuf
                    %17 = OpLoad %14 %s
                    %19 = OpSampledImage %18 %13 %17
                    %23 = OpLoad %int %ii
                    %24 = OpImage %10 %19
                    %25 = OpImageFetch %v4float %24 %23
                          OpStore %x %25
;CHECK-NOT:         %25 = OpImageFetch %v4float %24 %23
;CHECK-NOT:               OpStore %x %25
;CHECK:             %30 = OpBitcast %uint %23
;CHECK:             %32 = OpImageQuerySize %uint %24
;CHECK:             %34 = OpULessThan %bool %30 %32
;CHECK:                   OpSelectionMerge %36 None
;CHECK:                   OpBranchConditional %34 %37 %38
;CHECK:             %37 = OpLabel
;CHECK:             %39 = OpLoad %10 %tBuf
;CHECK:             %40 = OpSampledImage %18 %39 %17
;CHECK:             %41 = OpImage %10 %40
;CHECK:             %42 = OpImageFetch %v4float %41 %23
;CHECK:                   OpBranch %36
;CHECK:             %38 = OpLabel
;CHECK:            {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_42 %uint_6 %uint_3 %uint_7 %uint_0 %30 %32
;CHECK:                   OpBranch %36
;CHECK:             %36 = OpLabel
;CHECK:            [[phi_result:%\w+]] = OpPhi %v4float %42 %37 [[null_v4float]] %38
;CHECK:                   OpStore %x [[phi_result]]
                          OpReturn
                          OpFunctionEnd
                          )" + kStreamWrite7Frag;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false,
                                               false, true, true, true);
}

TEST_F(InstBindlessTest, DeviceBufferAddressOOB) {
  // #version 450
  // #extension GL_EXT_buffer_reference : enable
  //  layout(buffer_reference, buffer_reference_align = 16) buffer bufStruct;
  // layout(set = 0, binding = 0) uniform ufoo {
  //     bufStruct data;
  //     int nWrites;
  // } u_info;
  // layout(buffer_reference, std140) buffer bufStruct {
  //     int a[4];
  // };
  // void main() {
  //     for (int i=0; i < u_info.nWrites; ++i) {
  //         u_info.data.a[i] = 0xdeadca71;
  //     }
  // }

  // clang-format off
  const std::string text = R"(
OpCapability Shader
OpCapability PhysicalStorageBufferAddresses
;CHECK: OpCapability Int64
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel PhysicalStorageBuffer64 GLSL450
OpEntryPoint Vertex %main "main" %u_info
;CHECK: OpEntryPoint Vertex %main "main" %u_info %inst_bindless_input_buffer %inst_bindless_output_buffer %gl_VertexIndex %gl_InstanceIndex
OpSource GLSL 450
OpSourceExtension "GL_EXT_buffer_reference"
OpName %main "main"
OpName %i "i"
OpName %ufoo "ufoo"
OpMemberName %ufoo 0 "data"
OpMemberName %ufoo 1 "nWrites"
OpName %bufStruct "bufStruct"
OpMemberName %bufStruct 0 "a"
OpName %u_info "u_info"
OpMemberDecorate %ufoo 0 Offset 0
OpMemberDecorate %ufoo 1 Offset 8
OpDecorate %ufoo Block
OpDecorate %_arr_int_uint_4 ArrayStride 16
OpMemberDecorate %bufStruct 0 Offset 0
OpDecorate %bufStruct Block
OpDecorate %u_info DescriptorSet 0
OpDecorate %u_info Binding 0)"
    + kInputDecorations + kOutputDecorations +
R"(%void = OpTypeVoid
%3 = OpTypeFunction %void
%int = OpTypeInt 32 1
%_ptr_Function_int = OpTypePointer Function %int
%int_0 = OpConstant %int 0
OpTypeForwardPointer %_ptr_PhysicalStorageBuffer_bufStruct PhysicalStorageBuffer
%ufoo = OpTypeStruct %_ptr_PhysicalStorageBuffer_bufStruct %int
%uint = OpTypeInt 32 0
%uint_4 = OpConstant %uint 4
%_arr_int_uint_4 = OpTypeArray %int %uint_4
%bufStruct = OpTypeStruct %_arr_int_uint_4
%_ptr_PhysicalStorageBuffer_bufStruct = OpTypePointer PhysicalStorageBuffer %bufStruct
%_ptr_Uniform_ufoo = OpTypePointer Uniform %ufoo
%u_info = OpVariable %_ptr_Uniform_ufoo Uniform
%int_1 = OpConstant %int 1
%_ptr_Uniform_int = OpTypePointer Uniform %int
%bool = OpTypeBool
%_ptr_Uniform__ptr_PhysicalStorageBuffer_bufStruct = OpTypePointer Uniform %_ptr_PhysicalStorageBuffer_bufStruct
%int_n559035791 = OpConstant %int -559035791
%_ptr_PhysicalStorageBuffer_int = OpTypePointer PhysicalStorageBuffer %int)"
    + kInputGlobals + kOutputGlobals +
R"(%main = OpFunction %void None %3
%5 = OpLabel
%i = OpVariable %_ptr_Function_int Function
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[desc_state:%\w+]] = OpFunctionCall %uint %inst_bindless_read_desc_init %uint_0 %uint_0 %uint_0
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
OpStore %i %int_0
OpBranch %10
%10 = OpLabel
OpLoopMerge %12 %13 None
OpBranch %14
%14 = OpLabel
%15 = OpLoad %int %i
%26 = OpAccessChain %_ptr_Uniform_int %u_info %int_1
%27 = OpLoad %int %26
%29 = OpSLessThan %bool %15 %27
;CHECK-NOT: %27 = OpLoad %int %26
;CHECK-NOT: %29 = OpSLessThan %bool %15 %27
;CHECK: {{%\w+}} = OpIAdd %uint %uint_8 %uint_3
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[load_result:%\w+]] = OpLoad %int %26
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_54 %uint_4 %uint_0 %uint_0 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[phi_result:%\w+]] = OpPhi %int [[load_result]] {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpSLessThan %bool %15 [[phi_result]]
OpBranchConditional %29 %11 %12
%11 = OpLabel
%31 = OpAccessChain %_ptr_Uniform__ptr_PhysicalStorageBuffer_bufStruct %u_info %int_0
%32 = OpLoad %_ptr_PhysicalStorageBuffer_bufStruct %31
;CHECK-NOT: %32 = OpLoad %_ptr_PhysicalStorageBuffer_bufStruct %31
;CHECK: {{%\w+}} = OpIAdd %uint %uint_0 %uint_7
;CHECK: {{%\w+}} = OpULessThan %bool {{%\w+}} {{%\w+}}
;CHECK: OpSelectionMerge {{%\w+}} None
;CHECK: OpBranchConditional {{%\w+}} {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[load_result_2:%\w+]] = OpLoad %_ptr_PhysicalStorageBuffer_bufStruct %31
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: {{%\w+}} = OpFunctionCall %void %inst_bindless_stream_write_7 %uint_59 %uint_4 %uint_0 %uint_0 %uint_0 {{%\w+}} {{%\w+}}
;CHECK: {{%\w+}} = OpConvertUToPtr %_ptr_PhysicalStorageBuffer_bufStruct {{%\w+}}
;CHECK: OpBranch {{%\w+}}
;CHECK: {{%\w+}} = OpLabel
;CHECK: [[phi_result_2:%\w+]] = OpPhi %_ptr_PhysicalStorageBuffer_bufStruct [[load_result_2]] {{%\w+}} {{%\w+}} {{%\w+}}
%33 = OpLoad %int %i
%36 = OpAccessChain %_ptr_PhysicalStorageBuffer_int %32 %int_0 %33
;CHECK-NOT: %36 = OpAccessChain %_ptr_PhysicalStorageBuffer_int %32 %int_0 %33
;CHECK: %36 = OpAccessChain %_ptr_PhysicalStorageBuffer_int [[phi_result_2]] %int_0 %33
OpStore %36 %int_n559035791 Aligned 16
OpBranch %13
%13 = OpLabel
%37 = OpLoad %int %i
%38 = OpIAdd %int %37 %int_1
OpStore %i %38
OpBranch %10
%12 = OpLabel
OpReturn
OpFunctionEnd)"
    + kReadDescInit + kStreamWrite7Vert;
  // clang-format on

  SetTargetEnv(SPV_ENV_VULKAN_1_2);
  SetAssembleOptions(SPV_TEXT_TO_BINARY_OPTION_PRESERVE_NUMERIC_IDS);
  SinglePassRunAndMatch<InstBindlessCheckPass>(text, true, 7u, 23u, false, true,
                                               true, true, true);
}

// TODO(greg-lunarg): Add tests to verify handling of these cases:
//
//   Compute shader
//   Geometry shader
//   Tessellation control shader
//   Tessellation eval shader
//   OpImage
//   SampledImage variable

}  // namespace
}  // namespace opt
}  // namespace spvtools
