// Copyright (c) 2019 Google LLC
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

#include "source/fuzz/transformation_outline_function.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationOutlineFunctionTest, TrivialOutline) {
  // This tests outlining of a single, empty basic block.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(5, 5, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
        %103 = OpFunctionCall %2 %101
               OpReturn
               OpFunctionEnd
        %101 = OpFunction %2 None %3
        %102 = OpLabel
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineIfRegionStartsWithOpVariable) {
  // This checks that we do not outline the first block of a function if it
  // contains OpVariable.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %7 = OpTypeBool
          %8 = OpTypePointer Function %7
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %6 = OpVariable %8 Function
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(5, 5, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, OutlineInterestingControlFlowNoState) {
  // This tests outlining of some non-trivial control flow, but such that the
  // basic blocks in the control flow do not actually do anything.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpSelectionMerge %9 None
               OpBranchConditional %21 %8 %9
          %8 = OpLabel
               OpBranch %9
          %9 = OpLabel
               OpLoopMerge %12 %11 None
               OpBranch %10
         %10 = OpLabel
               OpBranchConditional %21 %11 %12
         %11 = OpLabel
               OpBranch %9
         %12 = OpLabel
               OpBranch %13
         %13 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 13, /* not relevant */
                                               200, 100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
        %103 = OpFunctionCall %2 %101
               OpReturn
               OpFunctionEnd
        %101 = OpFunction %2 None %3
        %102 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpSelectionMerge %9 None
               OpBranchConditional %21 %8 %9
          %8 = OpLabel
               OpBranch %9
          %9 = OpLabel
               OpLoopMerge %12 %11 None
               OpBranch %10
         %10 = OpLabel
               OpBranchConditional %21 %11 %12
         %11 = OpLabel
               OpBranch %9
         %12 = OpLabel
               OpBranch %13
         %13 = OpLabel
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineCodeThatGeneratesUnusedIds) {
  // This tests outlining of a single basic block that does some computation,
  // but that does not use nor generate ids required outside of the outlined
  // region.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
          %7 = OpCopyObject %20 %21
          %8 = OpCopyObject %20 %21
          %9 = OpIAdd %20 %7 %8
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 6, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
        %103 = OpFunctionCall %2 %101
               OpReturn
               OpFunctionEnd
        %101 = OpFunction %2 None %3
        %102 = OpLabel
          %7 = OpCopyObject %20 %21
          %8 = OpCopyObject %20 %21
          %9 = OpIAdd %20 %7 %8
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineCodeThatGeneratesSingleUsedId) {
  // This tests outlining of a block that generates an id that is used in a
  // later block.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
          %7 = OpCopyObject %20 %21
          %8 = OpCopyObject %20 %21
          %9 = OpIAdd %20 %7 %8
               OpBranch %10
         %10 = OpLabel
         %11 = OpCopyObject %20 %9
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 6, 99, 100, 101, 102, 103,
                                               105, {}, {{9, 104}});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
         %99 = OpTypeStruct %20
        %100 = OpTypeFunction %99
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
        %103 = OpFunctionCall %99 %101
          %9 = OpCompositeExtract %20 %103 0
               OpBranch %10
         %10 = OpLabel
         %11 = OpCopyObject %20 %9
               OpReturn
               OpFunctionEnd
        %101 = OpFunction %99 None %100
        %102 = OpLabel
          %7 = OpCopyObject %20 %21
          %8 = OpCopyObject %20 %21
        %104 = OpIAdd %20 %7 %8
        %105 = OpCompositeConstruct %99 %104
               OpReturnValue %105
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineDiamondThatGeneratesSeveralIds) {
  // This tests outlining of several blocks that generate a number of ids that
  // are used in later blocks.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
         %22 = OpTypeBool
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
          %7 = OpCopyObject %20 %21
          %8 = OpCopyObject %20 %21
          %9 = OpSLessThan %22 %7 %8
               OpSelectionMerge %12 None
               OpBranchConditional %9 %10 %11
         %10 = OpLabel
         %13 = OpIAdd %20 %7 %8
               OpBranch %12
         %11 = OpLabel
         %14 = OpIAdd %20 %7 %7
               OpBranch %12
         %12 = OpLabel
         %15 = OpPhi %20 %13 %10 %14 %11
               OpBranch %80
         %80 = OpLabel
               OpBranch %16
         %16 = OpLabel
         %17 = OpCopyObject %20 %15
         %18 = OpCopyObject %22 %9
         %19 = OpIAdd %20 %7 %8
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      6, 80, 100, 101, 102, 103, 104, 105, {},
      {{15, 106}, {9, 107}, {7, 108}, {8, 109}});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
         %22 = OpTypeBool
          %3 = OpTypeFunction %2
        %100 = OpTypeStruct %20 %20 %22 %20
        %101 = OpTypeFunction %100
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
        %104 = OpFunctionCall %100 %102
          %7 = OpCompositeExtract %20 %104 0
          %8 = OpCompositeExtract %20 %104 1
          %9 = OpCompositeExtract %22 %104 2
         %15 = OpCompositeExtract %20 %104 3
               OpBranch %16
         %16 = OpLabel
         %17 = OpCopyObject %20 %15
         %18 = OpCopyObject %22 %9
         %19 = OpIAdd %20 %7 %8
               OpReturn
               OpFunctionEnd
        %102 = OpFunction %100 None %101
        %103 = OpLabel
        %108 = OpCopyObject %20 %21
        %109 = OpCopyObject %20 %21
        %107 = OpSLessThan %22 %108 %109
               OpSelectionMerge %12 None
               OpBranchConditional %107 %10 %11
         %10 = OpLabel
         %13 = OpIAdd %20 %108 %109
               OpBranch %12
         %11 = OpLabel
         %14 = OpIAdd %20 %108 %108
               OpBranch %12
         %12 = OpLabel
        %106 = OpPhi %20 %13 %10 %14 %11
               OpBranch %80
         %80 = OpLabel
        %105 = OpCompositeConstruct %100 %108 %109 %107 %106
               OpReturnValue %105
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineCodeThatUsesASingleId) {
  // This tests outlining of a block that uses an id defined earlier.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %7 = OpCopyObject %20 %21
               OpBranch %6
          %6 = OpLabel
          %8 = OpCopyObject %20 %7
               OpBranch %10
         %10 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 6, 100, 101, 102, 103, 104,
                                               105, {{7, 106}}, {});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
        %101 = OpTypeFunction %2 %20
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %7 = OpCopyObject %20 %21
               OpBranch %6
          %6 = OpLabel
        %104 = OpFunctionCall %2 %102 %7
               OpBranch %10
         %10 = OpLabel
               OpReturn
               OpFunctionEnd
        %102 = OpFunction %2 None %101
        %106 = OpFunctionParameter %20
        %103 = OpLabel
          %8 = OpCopyObject %20 %106
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineCodeThatUsesAVariable) {
  // This tests outlining of a block that uses a variable.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
         %12 = OpTypePointer Function %20
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %13 = OpVariable %12 Function
               OpBranch %6
          %6 = OpLabel
          %8 = OpLoad %20 %13
               OpBranch %10
         %10 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 6, 100, 101, 102, 103, 104,
                                               105, {{13, 106}}, {});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeInt 32 1
         %21 = OpConstant %20 5
          %3 = OpTypeFunction %2
         %12 = OpTypePointer Function %20
        %101 = OpTypeFunction %2 %12
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %13 = OpVariable %12 Function
               OpBranch %6
          %6 = OpLabel
        %104 = OpFunctionCall %2 %102 %13
               OpBranch %10
         %10 = OpLabel
               OpReturn
               OpFunctionEnd
        %102 = OpFunction %2 None %101
        %106 = OpFunctionParameter %12
        %103 = OpLabel
          %8 = OpLoad %20 %106
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineCodeThatUsesAParameter) {
  // This tests outlining of a block that uses a function parameter.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %10 "foo(i1;"
               OpName %9 "x"
               OpName %18 "param"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFunction %6 %7
         %13 = OpConstant %6 1
         %17 = OpConstant %6 3
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %18 = OpVariable %7 Function
               OpStore %18 %17
         %19 = OpFunctionCall %6 %10 %18
               OpReturn
               OpFunctionEnd
         %10 = OpFunction %6 None %8
          %9 = OpFunctionParameter %7
         %11 = OpLabel
         %12 = OpLoad %6 %9
         %14 = OpIAdd %6 %12 %13
               OpReturnValue %14
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(11, 11, 100, 101, 102, 103, 104,
                                               105, {{9, 106}}, {{14, 107}});
  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %10 "foo(i1;"
               OpName %9 "x"
               OpName %18 "param"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFunction %6 %7
         %13 = OpConstant %6 1
         %17 = OpConstant %6 3
        %100 = OpTypeStruct %6
        %101 = OpTypeFunction %100 %7
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %18 = OpVariable %7 Function
               OpStore %18 %17
         %19 = OpFunctionCall %6 %10 %18
               OpReturn
               OpFunctionEnd
         %10 = OpFunction %6 None %8
          %9 = OpFunctionParameter %7
         %11 = OpLabel
        %104 = OpFunctionCall %100 %102 %9
         %14 = OpCompositeExtract %6 %104 0
               OpReturnValue %14
               OpFunctionEnd
        %102 = OpFunction %100 None %101
        %106 = OpFunctionParameter %7
        %103 = OpLabel
         %12 = OpLoad %6 %106
        %107 = OpIAdd %6 %12 %13
        %105 = OpCompositeConstruct %100 %107
               OpReturnValue %105
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineIfLoopMergeIsOutsideRegion) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %9 = OpTypeBool
         %10 = OpConstantTrue %9
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpLoopMerge %7 %8 None
               OpBranch %8
          %8 = OpLabel
               OpBranchConditional %10 %6 %7
          %7 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 8, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, DoNotOutlineIfRegionInvolvesReturn) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpSelectionMerge %10 None
               OpBranchConditional %21 %8 %9
          %8 = OpLabel
               OpReturn
          %9 = OpLabel
               OpBranch %10
         %10 = OpLabel
               OpBranch %11
         %11 = OpLabel
               OpBranch %12
         %12 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 11, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, DoNotOutlineIfRegionInvolvesKill) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpSelectionMerge %10 None
               OpBranchConditional %21 %8 %9
          %8 = OpLabel
               OpKill
          %9 = OpLabel
               OpBranch %10
         %10 = OpLabel
               OpBranch %11
         %11 = OpLabel
               OpBranch %12
         %12 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 11, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineIfRegionInvolvesUnreachable) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpSelectionMerge %10 None
               OpBranchConditional %21 %8 %9
          %8 = OpLabel
               OpBranch %10
          %9 = OpLabel
               OpUnreachable
         %10 = OpLabel
               OpBranch %11
         %11 = OpLabel
               OpBranch %12
         %12 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 11, /* not relevant */ 200,
                                               100, 101, 102, 103,
                                               /* not relevant */ 201, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineIfSelectionMergeIsOutsideRegion) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %9 = OpTypeBool
         %10 = OpConstantTrue %9
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpSelectionMerge %7 None
               OpBranchConditional %10 %8 %7
          %8 = OpLabel
               OpBranch %7
          %7 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 8, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, DoNotOutlineIfLoopHeadIsOutsideRegion) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %9 = OpTypeBool
         %10 = OpConstantTrue %9
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpLoopMerge %8 %11 None
               OpBranch %7
          %7 = OpLabel
               OpBranchConditional %10 %11 %8
         %11 = OpLabel
               OpBranch %6
          %8 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(7, 8, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineIfLoopContinueIsOutsideRegion) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %9 = OpTypeBool
         %10 = OpConstantTrue %9
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
               OpLoopMerge %7 %8 None
               OpBranch %7
          %8 = OpLabel
               OpBranch %6
          %7 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 7, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineWithLoopCarriedPhiDependence) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %9 = OpTypeBool
         %10 = OpConstantTrue %9
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %6
          %6 = OpLabel
         %12 = OpPhi %9 %10 %5 %13 %8
               OpLoopMerge %7 %8 None
               OpBranch %8
          %8 = OpLabel
         %13 = OpCopyObject %9 %10
               OpBranchConditional %10 %6 %7
          %7 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(6, 7, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineSelectionHeaderNotInRegion) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpSelectionMerge %10 None
               OpBranchConditional %7 %8 %8
          %8 = OpLabel
               OpBranch %9
          %9 = OpLabel
               OpBranch %10
         %10 = OpLabel
               OpBranch %11
         %11 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(8, 11, 100, 101, 102, 103, 104,
                                               105, {}, {});
  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, OutlineRegionEndingWithReturnVoid) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 1
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %22 = OpCopyObject %20 %21
               OpBranch %54
         %54 = OpLabel
               OpBranch %57
         %57 = OpLabel
         %23 = OpCopyObject %20 %22
               OpBranch %58
         %58 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 54,
      /*exit_block*/ 58,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {{22, 206}},
      /*output_id_to_fresh_id*/ {});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 1
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
        %201 = OpTypeFunction %2 %20
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %22 = OpCopyObject %20 %21
               OpBranch %54
         %54 = OpLabel
        %204 = OpFunctionCall %2 %202 %22
               OpReturn
               OpFunctionEnd
        %202 = OpFunction %2 None %201
        %206 = OpFunctionParameter %20
        %203 = OpLabel
               OpBranch %57
         %57 = OpLabel
         %23 = OpCopyObject %20 %206
               OpBranch %58
         %58 = OpLabel
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, OutlineRegionEndingWithReturnValue) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 1
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
         %30 = OpTypeFunction %20
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %6 = OpFunctionCall %20 %100
               OpReturn
               OpFunctionEnd
        %100 = OpFunction %20 None %30
          %8 = OpLabel
         %31 = OpCopyObject %20 %21
               OpBranch %9
          %9 = OpLabel
         %32 = OpCopyObject %20 %31
               OpBranch %10
         %10 = OpLabel
               OpReturnValue %32
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 9,
      /*exit_block*/ 10,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {{31, 206}},
      /*output_id_to_fresh_id*/ {{32, 207}});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 1
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
         %30 = OpTypeFunction %20
        %200 = OpTypeStruct %20
        %201 = OpTypeFunction %200 %20
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %6 = OpFunctionCall %20 %100
               OpReturn
               OpFunctionEnd
        %100 = OpFunction %20 None %30
          %8 = OpLabel
         %31 = OpCopyObject %20 %21
               OpBranch %9
          %9 = OpLabel
        %204 = OpFunctionCall %200 %202 %31
         %32 = OpCompositeExtract %20 %204 0
               OpReturnValue %32
               OpFunctionEnd
        %202 = OpFunction %200 None %201
        %206 = OpFunctionParameter %20
        %203 = OpLabel
        %207 = OpCopyObject %20 %206
               OpBranch %10
         %10 = OpLabel
        %205 = OpCompositeConstruct %200 %207
               OpReturnValue %205
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest,
     OutlineRegionEndingWithConditionalBranch) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %54
         %54 = OpLabel
          %6 = OpCopyObject %20 %21
               OpSelectionMerge %8 None
               OpBranchConditional %6 %7 %8
          %7 = OpLabel
               OpBranch %8
          %8 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 54,
      /*exit_block*/ 54,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {{}},
      /*output_id_to_fresh_id*/ {{6, 206}});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
        %200 = OpTypeStruct %20
        %201 = OpTypeFunction %200
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %54
         %54 = OpLabel
        %204 = OpFunctionCall %200 %202
          %6 = OpCompositeExtract %20 %204 0
               OpSelectionMerge %8 None
               OpBranchConditional %6 %7 %8
          %7 = OpLabel
               OpBranch %8
          %8 = OpLabel
               OpReturn
               OpFunctionEnd
        %202 = OpFunction %200 None %201
        %203 = OpLabel
        %206 = OpCopyObject %20 %21
        %205 = OpCompositeConstruct %200 %206
               OpReturnValue %205
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest,
     OutlineRegionEndingWithConditionalBranch2) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %6 = OpCopyObject %20 %21
               OpBranch %54
         %54 = OpLabel
               OpSelectionMerge %8 None
               OpBranchConditional %6 %7 %8
          %7 = OpLabel
               OpBranch %8
          %8 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 54,
      /*exit_block*/ 54,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {},
      /*output_id_to_fresh_id*/ {});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
         %20 = OpTypeBool
         %21 = OpConstantTrue %20
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %6 = OpCopyObject %20 %21
               OpBranch %54
         %54 = OpLabel
        %204 = OpFunctionCall %2 %202
               OpSelectionMerge %8 None
               OpBranchConditional %6 %7 %8
          %7 = OpLabel
               OpBranch %8
          %8 = OpLabel
               OpReturn
               OpFunctionEnd
        %202 = OpFunction %2 None %3
        %203 = OpLabel
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, DoNotOutlineRegionThatStartsWithOpPhi) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %21
         %21 = OpLabel
         %22 = OpPhi %6 %7 %5
         %23 = OpCopyObject %6 %22
               OpBranch %24
         %24 = OpLabel
         %25 = OpCopyObject %6 %23
         %26 = OpCopyObject %6 %22
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 21,
      /*exit_block*/ 21,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 204,
      /*new_caller_result_id*/ 205,
      /*new_callee_result_id*/ 206,
      /*input_id_to_fresh_id*/ {{22, 207}},
      /*output_id_to_fresh_id*/ {{23, 208}});

  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineRegionThatStartsWithLoopHeader) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %21
         %21 = OpLabel
               OpLoopMerge %22 %23 None
               OpBranch %24
         %24 = OpLabel
               OpBranchConditional %7 %22 %23
         %23 = OpLabel
               OpBranch %21
         %22 = OpLabel
               OpBranch %25
         %25 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 21,
      /*exit_block*/ 24,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 204,
      /*new_caller_result_id*/ 205,
      /*new_callee_result_id*/ 206,
      /*input_id_to_fresh_id*/ {},
      /*output_id_to_fresh_id*/ {});

  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest,
     DoNotOutlineRegionThatEndsWithLoopMerge) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %21
         %21 = OpLabel
               OpLoopMerge %22 %23 None
               OpBranch %24
         %24 = OpLabel
               OpBranchConditional %7 %22 %23
         %23 = OpLabel
               OpBranch %21
         %22 = OpLabel
               OpBranch %25
         %25 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 5,
      /*exit_block*/ 22,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 204,
      /*new_caller_result_id*/ 205,
      /*new_callee_result_id*/ 206,
      /*input_id_to_fresh_id*/ {},
      /*output_id_to_fresh_id*/ {});

  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, DoNotOutlineRegionThatUsesAccessChain) {
  // An access chain result is a pointer, but it cannot be passed as a function
  // parameter, as it is not a memory object.
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeFloat 32
          %7 = OpTypeVector %6 4
          %8 = OpTypePointer Function %7
          %9 = OpTypePointer Function %6
         %18 = OpTypeInt 32 0
         %19 = OpConstant %18 0
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %10 = OpVariable %8 Function
               OpBranch %11
         %11 = OpLabel
         %12 = OpAccessChain %9 %10 %19
               OpBranch %13
         %13 = OpLabel
         %14 = OpLoad %6 %12
               OpBranch %15
         %15 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 13,
      /*exit_block*/ 15,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 204,
      /*new_caller_result_id*/ 205,
      /*new_callee_result_id*/ 206,
      /*input_id_to_fresh_id*/ {{12, 207}},
      /*output_id_to_fresh_id*/ {});

  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, Miscellaneous1) {
  // This tests outlining of some non-trivial code.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main" %85
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %28 "buf"
               OpMemberName %28 0 "u1"
               OpMemberName %28 1 "u2"
               OpName %30 ""
               OpName %85 "color"
               OpMemberDecorate %28 0 Offset 0
               OpMemberDecorate %28 1 Offset 4
               OpDecorate %28 Block
               OpDecorate %30 DescriptorSet 0
               OpDecorate %30 Binding 0
               OpDecorate %85 Location 0
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeFloat 32
          %7 = OpTypeVector %6 4
         %10 = OpConstant %6 1
         %11 = OpConstant %6 2
         %12 = OpConstant %6 3
         %13 = OpConstant %6 4
         %14 = OpConstantComposite %7 %10 %11 %12 %13
         %15 = OpTypeInt 32 1
         %18 = OpConstant %15 0
         %28 = OpTypeStruct %6 %6
         %29 = OpTypePointer Uniform %28
         %30 = OpVariable %29 Uniform
         %31 = OpTypePointer Uniform %6
         %35 = OpTypeBool
         %39 = OpConstant %15 1
         %84 = OpTypePointer Output %7
         %85 = OpVariable %84 Output
        %114 = OpConstant %15 8
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %22
         %22 = OpLabel
        %103 = OpPhi %15 %18 %5 %106 %43
        %102 = OpPhi %7 %14 %5 %107 %43
        %101 = OpPhi %15 %18 %5 %40 %43
         %32 = OpAccessChain %31 %30 %18
         %33 = OpLoad %6 %32
         %34 = OpConvertFToS %15 %33
         %36 = OpSLessThan %35 %101 %34
               OpLoopMerge %24 %43 None
               OpBranchConditional %36 %23 %24
         %23 = OpLabel
         %40 = OpIAdd %15 %101 %39
               OpBranch %150
        %150 = OpLabel
               OpBranch %41
         %41 = OpLabel
        %107 = OpPhi %7 %102 %150 %111 %65
        %106 = OpPhi %15 %103 %150 %110 %65
        %104 = OpPhi %15 %40 %150 %81 %65
         %47 = OpAccessChain %31 %30 %39
         %48 = OpLoad %6 %47
         %49 = OpConvertFToS %15 %48
         %50 = OpSLessThan %35 %104 %49
               OpLoopMerge %1000 %65 None
               OpBranchConditional %50 %42 %1000
         %42 = OpLabel
         %60 = OpIAdd %15 %106 %114
         %63 = OpSGreaterThan %35 %104 %60
               OpBranchConditional %63 %64 %65
         %64 = OpLabel
         %71 = OpCompositeExtract %6 %107 0
         %72 = OpFAdd %6 %71 %11
         %97 = OpCompositeInsert %7 %72 %107 0
         %76 = OpCompositeExtract %6 %107 3
         %77 = OpConvertFToS %15 %76
         %79 = OpIAdd %15 %60 %77
               OpBranch %65
         %65 = OpLabel
        %111 = OpPhi %7 %107 %42 %97 %64
        %110 = OpPhi %15 %60 %42 %79 %64
         %81 = OpIAdd %15 %104 %39
               OpBranch %41
       %1000 = OpLabel
               OpBranch %1001
       %1001 = OpLabel
               OpBranch %43
         %43 = OpLabel
               OpBranch %22
         %24 = OpLabel
         %87 = OpCompositeExtract %6 %102 0
         %91 = OpConvertSToF %6 %103
         %92 = OpCompositeConstruct %7 %87 %11 %91 %10
               OpStore %85 %92
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 150,
      /*exit_block*/ 1001,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {{102, 300}, {103, 301}, {40, 302}},
      /*output_id_to_fresh_id*/ {{106, 400}, {107, 401}});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main" %85
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %28 "buf"
               OpMemberName %28 0 "u1"
               OpMemberName %28 1 "u2"
               OpName %30 ""
               OpName %85 "color"
               OpMemberDecorate %28 0 Offset 0
               OpMemberDecorate %28 1 Offset 4
               OpDecorate %28 Block
               OpDecorate %30 DescriptorSet 0
               OpDecorate %30 Binding 0
               OpDecorate %85 Location 0
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeFloat 32
          %7 = OpTypeVector %6 4
         %10 = OpConstant %6 1
         %11 = OpConstant %6 2
         %12 = OpConstant %6 3
         %13 = OpConstant %6 4
         %14 = OpConstantComposite %7 %10 %11 %12 %13
         %15 = OpTypeInt 32 1
         %18 = OpConstant %15 0
         %28 = OpTypeStruct %6 %6
         %29 = OpTypePointer Uniform %28
         %30 = OpVariable %29 Uniform
         %31 = OpTypePointer Uniform %6
         %35 = OpTypeBool
         %39 = OpConstant %15 1
         %84 = OpTypePointer Output %7
         %85 = OpVariable %84 Output
        %114 = OpConstant %15 8
        %200 = OpTypeStruct %7 %15
        %201 = OpTypeFunction %200 %15 %7 %15
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %22
         %22 = OpLabel
        %103 = OpPhi %15 %18 %5 %106 %43
        %102 = OpPhi %7 %14 %5 %107 %43
        %101 = OpPhi %15 %18 %5 %40 %43
         %32 = OpAccessChain %31 %30 %18
         %33 = OpLoad %6 %32
         %34 = OpConvertFToS %15 %33
         %36 = OpSLessThan %35 %101 %34
               OpLoopMerge %24 %43 None
               OpBranchConditional %36 %23 %24
         %23 = OpLabel
         %40 = OpIAdd %15 %101 %39
               OpBranch %150
        %150 = OpLabel
        %204 = OpFunctionCall %200 %202 %103 %102 %40
        %107 = OpCompositeExtract %7 %204 0
        %106 = OpCompositeExtract %15 %204 1
               OpBranch %43
         %43 = OpLabel
               OpBranch %22
         %24 = OpLabel
         %87 = OpCompositeExtract %6 %102 0
         %91 = OpConvertSToF %6 %103
         %92 = OpCompositeConstruct %7 %87 %11 %91 %10
               OpStore %85 %92
               OpReturn
               OpFunctionEnd
        %202 = OpFunction %200 None %201
        %301 = OpFunctionParameter %15
        %300 = OpFunctionParameter %7
        %302 = OpFunctionParameter %15
        %203 = OpLabel
               OpBranch %41
         %41 = OpLabel
        %401 = OpPhi %7 %300 %203 %111 %65
        %400 = OpPhi %15 %301 %203 %110 %65
        %104 = OpPhi %15 %302 %203 %81 %65
         %47 = OpAccessChain %31 %30 %39
         %48 = OpLoad %6 %47
         %49 = OpConvertFToS %15 %48
         %50 = OpSLessThan %35 %104 %49
               OpLoopMerge %1000 %65 None
               OpBranchConditional %50 %42 %1000
         %42 = OpLabel
         %60 = OpIAdd %15 %400 %114
         %63 = OpSGreaterThan %35 %104 %60
               OpBranchConditional %63 %64 %65
         %64 = OpLabel
         %71 = OpCompositeExtract %6 %401 0
         %72 = OpFAdd %6 %71 %11
         %97 = OpCompositeInsert %7 %72 %401 0
         %76 = OpCompositeExtract %6 %401 3
         %77 = OpConvertFToS %15 %76
         %79 = OpIAdd %15 %60 %77
               OpBranch %65
         %65 = OpLabel
        %111 = OpPhi %7 %401 %42 %97 %64
        %110 = OpPhi %15 %60 %42 %79 %64
         %81 = OpIAdd %15 %104 %39
               OpBranch %41
       %1000 = OpLabel
               OpBranch %1001
       %1001 = OpLabel
        %205 = OpCompositeConstruct %200 %401 %400
               OpReturnValue %205
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationOutlineFunctionTest, Miscellaneous2) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
         %21 = OpTypeBool
        %167 = OpConstantTrue %21
        %168 = OpConstantFalse %21
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpBranch %34
         %34 = OpLabel
               OpLoopMerge %36 %37 None
               OpBranchConditional %168 %37 %38
         %38 = OpLabel
               OpBranchConditional %168 %37 %36
         %37 = OpLabel
               OpBranch %34
         %36 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 38,
      /*exit_block*/ 36,
      /*new_function_struct_return_type_id*/ 200,
      /*new_function_type_id*/ 201,
      /*new_function_id*/ 202,
      /*new_function_region_entry_block*/ 203,
      /*new_caller_result_id*/ 204,
      /*new_callee_result_id*/ 205,
      /*input_id_to_fresh_id*/ {},
      /*output_id_to_fresh_id*/ {});

  ASSERT_FALSE(transformation.IsApplicable(context.get(), fact_manager));
}

TEST(TransformationOutlineFunctionTest, Miscellaneous3) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %6 "main"
               OpExecutionMode %6 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
         %21 = OpTypeBool
        %167 = OpConstantTrue %21
          %6 = OpFunction %2 None %3
          %7 = OpLabel
               OpBranch %80
         %80 = OpLabel
               OpBranch %14
         %14 = OpLabel
               OpLoopMerge %16 %17 None
               OpBranch %18
         %18 = OpLabel
               OpBranchConditional %167 %15 %16
         %15 = OpLabel
               OpBranch %17
         %16 = OpLabel
               OpBranch %81
         %81 = OpLabel
               OpReturn
         %17 = OpLabel
               OpBranch %14
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_5;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  TransformationOutlineFunction transformation(
      /*entry_block*/ 80,
      /*exit_block*/ 81,
      /*new_function_struct_return_type_id*/ 300,
      /*new_function_type_id*/ 301,
      /*new_function_id*/ 302,
      /*new_function_region_entry_block*/ 304,
      /*new_caller_result_id*/ 305,
      /*new_callee_result_id*/ 306,
      /*input_id_to_fresh_id*/ {},
      /*output_id_to_fresh_id*/ {});

  ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
  transformation.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %6 "main"
               OpExecutionMode %6 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
         %21 = OpTypeBool
        %167 = OpConstantTrue %21
          %6 = OpFunction %2 None %3
          %7 = OpLabel
               OpBranch %80
         %80 = OpLabel
        %305 = OpFunctionCall %2 %302
               OpReturn
               OpFunctionEnd
        %302 = OpFunction %2 None %3
        %304 = OpLabel
               OpBranch %14
         %14 = OpLabel
               OpLoopMerge %16 %17 None
               OpBranch %18
         %18 = OpLabel
               OpBranchConditional %167 %15 %16
         %15 = OpLabel
               OpBranch %17
         %16 = OpLabel
               OpBranch %81
         %81 = OpLabel
               OpReturn
         %17 = OpLabel
               OpBranch %14
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
