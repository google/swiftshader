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

#include "source/fuzz/transformation_add_function.h"
#include "source/fuzz/instruction_message.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationAddFunctionTest, BasicTest) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFloat 32
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %8 %7 %9
         %18 = OpConstant %8 0
         %20 = OpConstant %6 0
         %28 = OpTypeBool
         %37 = OpConstant %6 1
         %42 = OpTypePointer Private %8
         %43 = OpVariable %42 Private
         %47 = OpConstant %8 1
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

  TransformationAddFunction transformation1(std::vector<protobufs::Instruction>(
      {MakeInstructionMessage(
           SpvOpFunction, 8, 13,
           {{SPV_OPERAND_TYPE_FUNCTION_CONTROL, {SpvFunctionControlMaskNone}},
            {SPV_OPERAND_TYPE_ID, {10}}}),
       MakeInstructionMessage(SpvOpFunctionParameter, 7, 11, {}),
       MakeInstructionMessage(SpvOpFunctionParameter, 9, 12, {}),
       MakeInstructionMessage(SpvOpLabel, 0, 14, {}),
       MakeInstructionMessage(
           SpvOpVariable, 9, 17,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpVariable, 7, 19,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {17}}, {SPV_OPERAND_TYPE_ID, {18}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {19}}, {SPV_OPERAND_TYPE_ID, {20}}}),
       MakeInstructionMessage(SpvOpBranch, 0, 0, {{SPV_OPERAND_TYPE_ID, {21}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 21, {}),
       MakeInstructionMessage(
           SpvOpLoopMerge, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {23}},
            {SPV_OPERAND_TYPE_ID, {24}},
            {SPV_OPERAND_TYPE_LOOP_CONTROL, {SpvLoopControlMaskNone}}}),
       MakeInstructionMessage(SpvOpBranch, 0, 0, {{SPV_OPERAND_TYPE_ID, {25}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 25, {}),
       MakeInstructionMessage(SpvOpLoad, 6, 26, {{SPV_OPERAND_TYPE_ID, {19}}}),
       MakeInstructionMessage(SpvOpLoad, 6, 27, {{SPV_OPERAND_TYPE_ID, {11}}}),
       MakeInstructionMessage(
           SpvOpSLessThan, 28, 29,
           {{SPV_OPERAND_TYPE_ID, {26}}, {SPV_OPERAND_TYPE_ID, {27}}}),
       MakeInstructionMessage(SpvOpBranchConditional, 0, 0,
                              {{SPV_OPERAND_TYPE_ID, {29}},
                               {SPV_OPERAND_TYPE_ID, {22}},
                               {SPV_OPERAND_TYPE_ID, {23}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 22, {}),
       MakeInstructionMessage(SpvOpLoad, 8, 30, {{SPV_OPERAND_TYPE_ID, {12}}}),
       MakeInstructionMessage(SpvOpLoad, 6, 31, {{SPV_OPERAND_TYPE_ID, {19}}}),
       MakeInstructionMessage(SpvOpConvertSToF, 8, 32,
                              {{SPV_OPERAND_TYPE_ID, {31}}}),
       MakeInstructionMessage(
           SpvOpFMul, 8, 33,
           {{SPV_OPERAND_TYPE_ID, {30}}, {SPV_OPERAND_TYPE_ID, {32}}}),
       MakeInstructionMessage(SpvOpLoad, 8, 34, {{SPV_OPERAND_TYPE_ID, {17}}}),
       MakeInstructionMessage(
           SpvOpFAdd, 8, 35,
           {{SPV_OPERAND_TYPE_ID, {34}}, {SPV_OPERAND_TYPE_ID, {33}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {17}}, {SPV_OPERAND_TYPE_ID, {35}}}),
       MakeInstructionMessage(SpvOpBranch, 0, 0, {{SPV_OPERAND_TYPE_ID, {24}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 24, {}),
       MakeInstructionMessage(SpvOpLoad, 6, 36, {{SPV_OPERAND_TYPE_ID, {19}}}),
       MakeInstructionMessage(
           SpvOpIAdd, 6, 38,
           {{SPV_OPERAND_TYPE_ID, {36}}, {SPV_OPERAND_TYPE_ID, {37}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {19}}, {SPV_OPERAND_TYPE_ID, {38}}}),
       MakeInstructionMessage(SpvOpBranch, 0, 0, {{SPV_OPERAND_TYPE_ID, {21}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 23, {}),
       MakeInstructionMessage(SpvOpLoad, 8, 39, {{SPV_OPERAND_TYPE_ID, {17}}}),
       MakeInstructionMessage(SpvOpReturnValue, 0, 0,
                              {{SPV_OPERAND_TYPE_ID, {39}}}),
       MakeInstructionMessage(SpvOpFunctionEnd, 0, 0, {})}));

  ASSERT_TRUE(transformation1.IsApplicable(context.get(), fact_manager));
  transformation1.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation1 = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFloat 32
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %8 %7 %9
         %18 = OpConstant %8 0
         %20 = OpConstant %6 0
         %28 = OpTypeBool
         %37 = OpConstant %6 1
         %42 = OpTypePointer Private %8
         %43 = OpVariable %42 Private
         %47 = OpConstant %8 1
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpReturn
               OpFunctionEnd
         %13 = OpFunction %8 None %10
         %11 = OpFunctionParameter %7
         %12 = OpFunctionParameter %9
         %14 = OpLabel
         %17 = OpVariable %9 Function
         %19 = OpVariable %7 Function
               OpStore %17 %18
               OpStore %19 %20
               OpBranch %21
         %21 = OpLabel
               OpLoopMerge %23 %24 None
               OpBranch %25
         %25 = OpLabel
         %26 = OpLoad %6 %19
         %27 = OpLoad %6 %11
         %29 = OpSLessThan %28 %26 %27
               OpBranchConditional %29 %22 %23
         %22 = OpLabel
         %30 = OpLoad %8 %12
         %31 = OpLoad %6 %19
         %32 = OpConvertSToF %8 %31
         %33 = OpFMul %8 %30 %32
         %34 = OpLoad %8 %17
         %35 = OpFAdd %8 %34 %33
               OpStore %17 %35
               OpBranch %24
         %24 = OpLabel
         %36 = OpLoad %6 %19
         %38 = OpIAdd %6 %36 %37
               OpStore %19 %38
               OpBranch %21
         %23 = OpLabel
         %39 = OpLoad %8 %17
               OpReturnValue %39
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation1, context.get()));

  TransformationAddFunction transformation2(std::vector<protobufs::Instruction>(
      {MakeInstructionMessage(
           SpvOpFunction, 2, 15,
           {{SPV_OPERAND_TYPE_FUNCTION_CONTROL, {SpvFunctionControlMaskNone}},
            {SPV_OPERAND_TYPE_ID, {3}}}),
       MakeInstructionMessage(SpvOpLabel, 0, 16, {}),
       MakeInstructionMessage(
           SpvOpVariable, 7, 44,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpVariable, 9, 45,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpVariable, 7, 48,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpVariable, 9, 49,
           {{SPV_OPERAND_TYPE_STORAGE_CLASS, {SpvStorageClassFunction}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {44}}, {SPV_OPERAND_TYPE_ID, {20}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {45}}, {SPV_OPERAND_TYPE_ID, {18}}}),
       MakeInstructionMessage(SpvOpFunctionCall, 8, 46,
                              {{SPV_OPERAND_TYPE_ID, {13}},
                               {SPV_OPERAND_TYPE_ID, {44}},
                               {SPV_OPERAND_TYPE_ID, {45}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {48}}, {SPV_OPERAND_TYPE_ID, {37}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {49}}, {SPV_OPERAND_TYPE_ID, {47}}}),
       MakeInstructionMessage(SpvOpFunctionCall, 8, 50,
                              {{SPV_OPERAND_TYPE_ID, {13}},
                               {SPV_OPERAND_TYPE_ID, {48}},
                               {SPV_OPERAND_TYPE_ID, {49}}}),
       MakeInstructionMessage(
           SpvOpFAdd, 8, 51,
           {{SPV_OPERAND_TYPE_ID, {46}}, {SPV_OPERAND_TYPE_ID, {50}}}),
       MakeInstructionMessage(
           SpvOpStore, 0, 0,
           {{SPV_OPERAND_TYPE_ID, {43}}, {SPV_OPERAND_TYPE_ID, {51}}}),
       MakeInstructionMessage(SpvOpReturn, 0, 0, {}),
       MakeInstructionMessage(SpvOpFunctionEnd, 0, 0, {})}));

  ASSERT_TRUE(transformation2.IsApplicable(context.get(), fact_manager));
  transformation2.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformation2 = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFloat 32
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %8 %7 %9
         %18 = OpConstant %8 0
         %20 = OpConstant %6 0
         %28 = OpTypeBool
         %37 = OpConstant %6 1
         %42 = OpTypePointer Private %8
         %43 = OpVariable %42 Private
         %47 = OpConstant %8 1
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpReturn
               OpFunctionEnd
         %13 = OpFunction %8 None %10
         %11 = OpFunctionParameter %7
         %12 = OpFunctionParameter %9
         %14 = OpLabel
         %17 = OpVariable %9 Function
         %19 = OpVariable %7 Function
               OpStore %17 %18
               OpStore %19 %20
               OpBranch %21
         %21 = OpLabel
               OpLoopMerge %23 %24 None
               OpBranch %25
         %25 = OpLabel
         %26 = OpLoad %6 %19
         %27 = OpLoad %6 %11
         %29 = OpSLessThan %28 %26 %27
               OpBranchConditional %29 %22 %23
         %22 = OpLabel
         %30 = OpLoad %8 %12
         %31 = OpLoad %6 %19
         %32 = OpConvertSToF %8 %31
         %33 = OpFMul %8 %30 %32
         %34 = OpLoad %8 %17
         %35 = OpFAdd %8 %34 %33
               OpStore %17 %35
               OpBranch %24
         %24 = OpLabel
         %36 = OpLoad %6 %19
         %38 = OpIAdd %6 %36 %37
               OpStore %19 %38
               OpBranch %21
         %23 = OpLabel
         %39 = OpLoad %8 %17
               OpReturnValue %39
               OpFunctionEnd
         %15 = OpFunction %2 None %3
         %16 = OpLabel
         %44 = OpVariable %7 Function
         %45 = OpVariable %9 Function
         %48 = OpVariable %7 Function
         %49 = OpVariable %9 Function
               OpStore %44 %20
               OpStore %45 %18
         %46 = OpFunctionCall %8 %13 %44 %45
               OpStore %48 %37
               OpStore %49 %47
         %50 = OpFunctionCall %8 %13 %48 %49
         %51 = OpFAdd %8 %46 %50
               OpStore %43 %51
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation2, context.get()));
}

TEST(TransformationAddFunctionTest, InapplicableTransformations) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %8 = OpTypeFloat 32
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %8 %7 %9
         %18 = OpConstant %8 0
         %20 = OpConstant %6 0
         %28 = OpTypeBool
         %37 = OpConstant %6 1
         %42 = OpTypePointer Private %8
         %43 = OpVariable %42 Private
         %47 = OpConstant %8 1
          %4 = OpFunction %2 None %3
          %5 = OpLabel
               OpReturn
               OpFunctionEnd
         %13 = OpFunction %8 None %10
         %11 = OpFunctionParameter %7
         %12 = OpFunctionParameter %9
         %14 = OpLabel
         %17 = OpVariable %9 Function
         %19 = OpVariable %7 Function
               OpStore %17 %18
               OpStore %19 %20
               OpBranch %21
         %21 = OpLabel
               OpLoopMerge %23 %24 None
               OpBranch %25
         %25 = OpLabel
         %26 = OpLoad %6 %19
         %27 = OpLoad %6 %11
         %29 = OpSLessThan %28 %26 %27
               OpBranchConditional %29 %22 %23
         %22 = OpLabel
         %30 = OpLoad %8 %12
         %31 = OpLoad %6 %19
         %32 = OpConvertSToF %8 %31
         %33 = OpFMul %8 %30 %32
         %34 = OpLoad %8 %17
         %35 = OpFAdd %8 %34 %33
               OpStore %17 %35
               OpBranch %24
         %24 = OpLabel
         %36 = OpLoad %6 %19
         %38 = OpIAdd %6 %36 %37
               OpStore %19 %38
               OpBranch %21
         %23 = OpLabel
         %39 = OpLoad %8 %17
               OpReturnValue %39
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  // No instructions
  ASSERT_FALSE(
      TransformationAddFunction(std::vector<protobufs::Instruction>({}))
          .IsApplicable(context.get(), fact_manager));

  // No function begin
  ASSERT_FALSE(
      TransformationAddFunction(
          std::vector<protobufs::Instruction>(
              {MakeInstructionMessage(SpvOpFunctionParameter, 7, 11, {}),
               MakeInstructionMessage(SpvOpFunctionParameter, 9, 12, {}),
               MakeInstructionMessage(SpvOpLabel, 0, 14, {})}))
          .IsApplicable(context.get(), fact_manager));

  // No OpLabel
  ASSERT_FALSE(
      TransformationAddFunction(
          std::vector<protobufs::Instruction>(
              {MakeInstructionMessage(SpvOpFunction, 8, 13,
                                      {{SPV_OPERAND_TYPE_FUNCTION_CONTROL,
                                        {SpvFunctionControlMaskNone}},
                                       {SPV_OPERAND_TYPE_ID, {10}}}),
               MakeInstructionMessage(SpvOpReturnValue, 0, 0,
                                      {{SPV_OPERAND_TYPE_ID, {39}}}),
               MakeInstructionMessage(SpvOpFunctionEnd, 0, 0, {})}))
          .IsApplicable(context.get(), fact_manager));

  // Abrupt end of instructions
  ASSERT_FALSE(TransformationAddFunction(
                   std::vector<protobufs::Instruction>({MakeInstructionMessage(
                       SpvOpFunction, 8, 13,
                       {{SPV_OPERAND_TYPE_FUNCTION_CONTROL,
                         {SpvFunctionControlMaskNone}},
                        {SPV_OPERAND_TYPE_ID, {10}}})}))
                   .IsApplicable(context.get(), fact_manager));

  // No function end
  ASSERT_FALSE(
      TransformationAddFunction(
          std::vector<protobufs::Instruction>(
              {MakeInstructionMessage(SpvOpFunction, 8, 13,
                                      {{SPV_OPERAND_TYPE_FUNCTION_CONTROL,
                                        {SpvFunctionControlMaskNone}},
                                       {SPV_OPERAND_TYPE_ID, {10}}}),
               MakeInstructionMessage(SpvOpLabel, 0, 14, {}),
               MakeInstructionMessage(SpvOpReturnValue, 0, 0,
                                      {{SPV_OPERAND_TYPE_ID, {39}}})}))
          .IsApplicable(context.get(), fact_manager));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
