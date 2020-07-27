// Copyright (c) 2020 Google LLC
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

#include "source/fuzz/replayer.h"

#include "source/fuzz/instruction_descriptor.h"
#include "source/fuzz/transformation_split_block.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(ReplayerTest, PartialReplay) {
  const std::string kTestShader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %8 "g"
               OpName %11 "x"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Private %6
          %8 = OpVariable %7 Private
          %9 = OpConstant %6 10
         %10 = OpTypePointer Function %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %11 = OpVariable %10 Function
               OpStore %8 %9
         %12 = OpLoad %6 %8
               OpStore %11 %12
         %13 = OpLoad %6 %8
               OpStore %11 %13
         %14 = OpLoad %6 %8
               OpStore %11 %14
         %15 = OpLoad %6 %8
               OpStore %11 %15
         %16 = OpLoad %6 %8
               OpStore %11 %16
         %17 = OpLoad %6 %8
               OpStore %11 %17
         %18 = OpLoad %6 %8
               OpStore %11 %18
         %19 = OpLoad %6 %8
               OpStore %11 %19
         %20 = OpLoad %6 %8
               OpStore %11 %20
         %21 = OpLoad %6 %8
               OpStore %11 %21
         %22 = OpLoad %6 %8
               OpStore %11 %22
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  spvtools::ValidatorOptions validator_options;

  std::vector<uint32_t> binary_in;
  SpirvTools t(env);
  t.SetMessageConsumer(kSilentConsumer);
  ASSERT_TRUE(t.Assemble(kTestShader, &binary_in, kFuzzAssembleOption));
  ASSERT_TRUE(t.Validate(binary_in));

  protobufs::TransformationSequence transformations;
  for (uint32_t id = 12; id <= 22; id++) {
    *transformations.add_transformation() =
        TransformationSplitBlock(MakeInstructionDescriptor(id, SpvOpLoad, 0),
                                 id + 100)
            .ToMessage();
  }

  {
    // Full replay
    protobufs::TransformationSequence transformations_out;
    protobufs::FactSequence empty_facts;
    std::vector<uint32_t> binary_out;
    Replayer replayer(env, true, validator_options);
    replayer.SetMessageConsumer(kSilentConsumer);
    auto replayer_result_status =
        replayer.Run(binary_in, empty_facts, transformations, 11, &binary_out,
                     &transformations_out);
    // Replay should succeed.
    ASSERT_EQ(Replayer::ReplayerResultStatus::kComplete,
              replayer_result_status);
    // All transformations should be applied.
    ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
        transformations, transformations_out));

    const std::string kFullySplitShader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %8 "g"
               OpName %11 "x"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Private %6
          %8 = OpVariable %7 Private
          %9 = OpConstant %6 10
         %10 = OpTypePointer Function %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %11 = OpVariable %10 Function
               OpStore %8 %9
               OpBranch %112
        %112 = OpLabel
         %12 = OpLoad %6 %8
               OpStore %11 %12
               OpBranch %113
        %113 = OpLabel
         %13 = OpLoad %6 %8
               OpStore %11 %13
               OpBranch %114
        %114 = OpLabel
         %14 = OpLoad %6 %8
               OpStore %11 %14
               OpBranch %115
        %115 = OpLabel
         %15 = OpLoad %6 %8
               OpStore %11 %15
               OpBranch %116
        %116 = OpLabel
         %16 = OpLoad %6 %8
               OpStore %11 %16
               OpBranch %117
        %117 = OpLabel
         %17 = OpLoad %6 %8
               OpStore %11 %17
               OpBranch %118
        %118 = OpLabel
         %18 = OpLoad %6 %8
               OpStore %11 %18
               OpBranch %119
        %119 = OpLabel
         %19 = OpLoad %6 %8
               OpStore %11 %19
               OpBranch %120
        %120 = OpLabel
         %20 = OpLoad %6 %8
               OpStore %11 %20
               OpBranch %121
        %121 = OpLabel
         %21 = OpLoad %6 %8
               OpStore %11 %21
               OpBranch %122
        %122 = OpLabel
         %22 = OpLoad %6 %8
               OpStore %11 %22
               OpReturn
               OpFunctionEnd
    )";
    ASSERT_TRUE(IsEqual(env, kFullySplitShader, binary_out));
  }

  {
    // Half replay
    protobufs::TransformationSequence transformations_out;
    protobufs::FactSequence empty_facts;
    std::vector<uint32_t> binary_out;
    Replayer replayer(env, true, validator_options);
    replayer.SetMessageConsumer(kSilentConsumer);
    auto replayer_result_status =
        replayer.Run(binary_in, empty_facts, transformations, 5, &binary_out,
                     &transformations_out);
    // Replay should succeed.
    ASSERT_EQ(Replayer::ReplayerResultStatus::kComplete,
              replayer_result_status);
    // The first 5 transformations should be applied
    ASSERT_EQ(5, transformations_out.transformation_size());
    for (uint32_t i = 0; i < 5; i++) {
      ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
          transformations.transformation(i),
          transformations_out.transformation(i)));
    }

    const std::string kHalfSplitShader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %8 "g"
               OpName %11 "x"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Private %6
          %8 = OpVariable %7 Private
          %9 = OpConstant %6 10
         %10 = OpTypePointer Function %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %11 = OpVariable %10 Function
               OpStore %8 %9
               OpBranch %112
        %112 = OpLabel
         %12 = OpLoad %6 %8
               OpStore %11 %12
               OpBranch %113
        %113 = OpLabel
         %13 = OpLoad %6 %8
               OpStore %11 %13
               OpBranch %114
        %114 = OpLabel
         %14 = OpLoad %6 %8
               OpStore %11 %14
               OpBranch %115
        %115 = OpLabel
         %15 = OpLoad %6 %8
               OpStore %11 %15
               OpBranch %116
        %116 = OpLabel
         %16 = OpLoad %6 %8
               OpStore %11 %16
         %17 = OpLoad %6 %8
               OpStore %11 %17
         %18 = OpLoad %6 %8
               OpStore %11 %18
         %19 = OpLoad %6 %8
               OpStore %11 %19
         %20 = OpLoad %6 %8
               OpStore %11 %20
         %21 = OpLoad %6 %8
               OpStore %11 %21
         %22 = OpLoad %6 %8
               OpStore %11 %22
               OpReturn
               OpFunctionEnd
    )";
    ASSERT_TRUE(IsEqual(env, kHalfSplitShader, binary_out));
  }

  {
    // Empty replay
    protobufs::TransformationSequence transformations_out;
    protobufs::FactSequence empty_facts;
    std::vector<uint32_t> binary_out;
    Replayer replayer(env, true, validator_options);
    replayer.SetMessageConsumer(kSilentConsumer);
    auto replayer_result_status =
        replayer.Run(binary_in, empty_facts, transformations, 0, &binary_out,
                     &transformations_out);
    // Replay should succeed.
    ASSERT_EQ(Replayer::ReplayerResultStatus::kComplete,
              replayer_result_status);
    // No transformations should be applied
    ASSERT_EQ(0, transformations_out.transformation_size());
    ASSERT_TRUE(IsEqual(env, kTestShader, binary_out));
  }

  {
    // Invalid replay: too many transformations
    protobufs::TransformationSequence transformations_out;
    protobufs::FactSequence empty_facts;
    std::vector<uint32_t> binary_out;
    // The number of transformations requested to be applied exceeds the number
    // of transformations
    Replayer replayer(env, true, validator_options);
    replayer.SetMessageConsumer(kSilentConsumer);
    auto replayer_result_status =
        replayer.Run(binary_in, empty_facts, transformations, 12, &binary_out,
                     &transformations_out);

    // Replay should not succeed.
    ASSERT_EQ(Replayer::ReplayerResultStatus::kTooManyTransformationsRequested,
              replayer_result_status);
    // No transformations should be applied
    ASSERT_EQ(0, transformations_out.transformation_size());
    // The output binary should be empty
    ASSERT_TRUE(binary_out.empty());
  }
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
