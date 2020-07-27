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

#include "source/fuzz/transformation_replace_load_store_with_copy_memory.h"
#include "source/fuzz/instruction_descriptor.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationReplaceLoadStoreWithCopyMemoryTest, BasicScenarios) {
  // This is a simple transformation and this test handles the main cases.

  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %8 "a"
               OpName %10 "b"
               OpName %12 "c"
               OpName %14 "d"
               OpName %18 "e"
               OpName %20 "f"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %9 = OpConstant %6 2
         %11 = OpConstant %6 3
         %13 = OpConstant %6 4
         %15 = OpConstant %6 5
         %16 = OpTypeFloat 32
         %17 = OpTypePointer Function %16
         %19 = OpConstant %16 2
         %21 = OpConstant %16 3
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %8 = OpVariable %7 Function
         %10 = OpVariable %7 Function
         %12 = OpVariable %7 Function
         %14 = OpVariable %7 Function
         %18 = OpVariable %17 Function
         %20 = OpVariable %17 Function
               OpStore %8 %9
               OpStore %10 %11
               OpStore %12 %13
               OpStore %14 %15
               OpStore %18 %19
               OpStore %20 %21
         %22 = OpLoad %6 %8
               OpCopyMemory %10 %8
               OpStore %10 %22
         %23 = OpLoad %6 %12
               OpStore %14 %23
         %24 = OpLoad %16 %18
               OpStore %20 %24
               OpReturn
               OpFunctionEnd
    )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);

  FactManager fact_manager;
  spvtools::ValidatorOptions validator_options;
  TransformationContext transformation_context(&fact_manager,
                                               validator_options);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto bad_instruction_descriptor_1 =
      MakeInstructionDescriptor(5, SpvOpVariable, 0);

  auto load_instruction_descriptor_1 =
      MakeInstructionDescriptor(5, SpvOpLoad, 0);
  auto load_instruction_descriptor_2 =
      MakeInstructionDescriptor(5, SpvOpLoad, 1);
  auto load_instruction_descriptor_3 =
      MakeInstructionDescriptor(5, SpvOpLoad, 2);
  auto store_instruction_descriptor_1 =
      MakeInstructionDescriptor(22, SpvOpStore, 0);
  auto store_instruction_descriptor_2 =
      MakeInstructionDescriptor(23, SpvOpStore, 0);
  auto store_instruction_descriptor_3 =
      MakeInstructionDescriptor(24, SpvOpStore, 0);

  // Bad: |load_instruction_descriptor| is incorrect.
  auto transformation_bad_1 = TransformationReplaceLoadStoreWithCopyMemory(
      bad_instruction_descriptor_1, store_instruction_descriptor_1);
  ASSERT_FALSE(
      transformation_bad_1.IsApplicable(context.get(), transformation_context));

  // Bad: |store_instruction_descriptor| is incorrect.
  auto transformation_bad_2 = TransformationReplaceLoadStoreWithCopyMemory(
      load_instruction_descriptor_1, bad_instruction_descriptor_1);
  ASSERT_FALSE(
      transformation_bad_2.IsApplicable(context.get(), transformation_context));

  // Bad: Intermediate values of the OpLoad and the OpStore don't match.
  auto transformation_bad_3 = TransformationReplaceLoadStoreWithCopyMemory(
      load_instruction_descriptor_1, store_instruction_descriptor_2);
  ASSERT_FALSE(
      transformation_bad_3.IsApplicable(context.get(), transformation_context));

  // Bad: There is a interfering OpCopyMemory instruction between the OpLoad and
  // the OpStore.
  auto transformation_bad_4 = TransformationReplaceLoadStoreWithCopyMemory(
      load_instruction_descriptor_1, store_instruction_descriptor_1);
  ASSERT_FALSE(
      transformation_bad_4.IsApplicable(context.get(), transformation_context));

  auto transformation_good_1 = TransformationReplaceLoadStoreWithCopyMemory(
      load_instruction_descriptor_2, store_instruction_descriptor_2);
  ASSERT_TRUE(transformation_good_1.IsApplicable(context.get(),
                                                 transformation_context));
  transformation_good_1.Apply(context.get(), &transformation_context);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation_good_2 = TransformationReplaceLoadStoreWithCopyMemory(
      load_instruction_descriptor_3, store_instruction_descriptor_3);
  ASSERT_TRUE(transformation_good_2.IsApplicable(context.get(),
                                                 transformation_context));
  transformation_good_2.Apply(context.get(), &transformation_context);
  ASSERT_TRUE(IsValid(env, context.get()));

  std::string after_transformations = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
               OpName %8 "a"
               OpName %10 "b"
               OpName %12 "c"
               OpName %14 "d"
               OpName %18 "e"
               OpName %20 "f"
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypePointer Function %6
          %9 = OpConstant %6 2
         %11 = OpConstant %6 3
         %13 = OpConstant %6 4
         %15 = OpConstant %6 5
         %16 = OpTypeFloat 32
         %17 = OpTypePointer Function %16
         %19 = OpConstant %16 2
         %21 = OpConstant %16 3
          %4 = OpFunction %2 None %3
          %5 = OpLabel
          %8 = OpVariable %7 Function
         %10 = OpVariable %7 Function
         %12 = OpVariable %7 Function
         %14 = OpVariable %7 Function
         %18 = OpVariable %17 Function
         %20 = OpVariable %17 Function
               OpStore %8 %9
               OpStore %10 %11
               OpStore %12 %13
               OpStore %14 %15
               OpStore %18 %19
               OpStore %20 %21
         %22 = OpLoad %6 %8
               OpCopyMemory %10 %8
               OpStore %10 %22
         %23 = OpLoad %6 %12
               OpCopyMemory %14 %12
         %24 = OpLoad %16 %18
               OpCopyMemory %20 %18
               OpReturn
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformations, context.get()));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
