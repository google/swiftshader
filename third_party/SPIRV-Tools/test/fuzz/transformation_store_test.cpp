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

#include "source/fuzz/transformation_store.h"
#include "source/fuzz/instruction_descriptor.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationStoreTest, BasicTest) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main" %92 %52 %53
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpDecorate %92 BuiltIn FragCoord
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypeFloat 32
          %8 = OpTypeStruct %6 %7
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %6 %9
         %14 = OpConstant %6 0
         %15 = OpTypePointer Function %6
         %51 = OpTypePointer Private %6
         %21 = OpConstant %6 2
         %23 = OpConstant %6 1
         %24 = OpConstant %7 1
         %25 = OpTypePointer Function %7
         %50 = OpTypePointer Private %7
         %34 = OpTypeBool
         %35 = OpConstantFalse %34
         %60 = OpConstantNull %50
         %61 = OpUndef %51
         %52 = OpVariable %50 Private
         %53 = OpVariable %51 Private
         %80 = OpConstantComposite %8 %21 %24
         %90 = OpTypeVector %7 4
         %91 = OpTypePointer Input %90
         %92 = OpVariable %91 Input
         %93 = OpConstantComposite %90 %24 %24 %24 %24
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %20 = OpVariable %9 Function
         %27 = OpVariable %9 Function ; irrelevant
         %22 = OpAccessChain %15 %20 %14
         %44 = OpCopyObject %9 %20
         %26 = OpAccessChain %25 %20 %23
         %29 = OpFunctionCall %6 %12 %27
         %30 = OpAccessChain %15 %20 %14
         %45 = OpCopyObject %15 %30
         %81 = OpCopyObject %9 %27 ; irrelevant
         %33 = OpAccessChain %15 %20 %14
               OpSelectionMerge %37 None
               OpBranchConditional %35 %36 %37
         %36 = OpLabel
         %38 = OpAccessChain %15 %20 %14
         %40 = OpAccessChain %15 %20 %14
         %43 = OpAccessChain %15 %20 %14
         %82 = OpCopyObject %9 %27 ; irrelevant
               OpBranch %37
         %37 = OpLabel
               OpReturn
               OpFunctionEnd
         %12 = OpFunction %6 None %10
         %11 = OpFunctionParameter %9 ; irrelevant
         %13 = OpLabel
         %46 = OpCopyObject %9 %11 ; irrelevant
         %16 = OpAccessChain %15 %11 %14 ; irrelevant
         %95 = OpCopyObject %8 %80
               OpReturnValue %21
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_4;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  fact_manager.AddFactValueOfPointeeIsIrrelevant(27);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(11);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(46);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(16);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(52);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(81);
  fact_manager.AddFactValueOfPointeeIsIrrelevant(82);

  fact_manager.AddFactBlockIsDead(36);

  // Variables with pointee types:
  //  52 - ptr_to(7)
  //  53 - ptr_to(6)
  //  20 - ptr_to(8)
  //  27 - ptr_to(8) - irrelevant
  //  92 - ptr_to(90) - read only

  // Access chains with pointee type:
  //  22 - ptr_to(6)
  //  26 - ptr_to(6)
  //  30 - ptr_to(6)
  //  33 - ptr_to(6)
  //  38 - ptr_to(6)
  //  40 - ptr_to(6)
  //  43 - ptr_to(6)
  //  16 - ptr_to(6) - irrelevant

  // Copied object with pointee type:
  //  44 - ptr_to(8)
  //  45 - ptr_to(6)
  //  46 - ptr_to(8) - irrelevant
  //  81 - ptr_to(8) - irrelevant
  //  82 - ptr_to(8) - irrelevant

  // Function parameters with pointee type:
  //  11 - ptr_to(8) - irrelevant

  // Pointers that cannot be used:
  //  60 - null
  //  61 - undefined

  // Bad: attempt to store to 11 from outside its function
  ASSERT_FALSE(TransformationStore(
                   11, 80, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: pointer is not available
  ASSERT_FALSE(TransformationStore(
                   81, 80, MakeInstructionDescriptor(45, SpvOpCopyObject, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: attempt to insert before OpVariable
  ASSERT_FALSE(TransformationStore(
                   52, 24, MakeInstructionDescriptor(27, SpvOpVariable, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: pointer id does not exist
  ASSERT_FALSE(TransformationStore(
                   1000, 24, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: pointer id exists but does not have a type
  ASSERT_FALSE(TransformationStore(
                   5, 24, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: pointer id exists and has a type, but is not a pointer
  ASSERT_FALSE(TransformationStore(
                   24, 24, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: attempt to store to a null pointer
  ASSERT_FALSE(TransformationStore(
                   60, 24, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: attempt to store to an undefined pointer
  ASSERT_FALSE(TransformationStore(
                   61, 21, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: %82 is not available at the program point
  ASSERT_FALSE(
      TransformationStore(82, 80, MakeInstructionDescriptor(37, SpvOpReturn, 0))
          .IsApplicable(context.get(), fact_manager));

  // Bad: value id does not exist
  ASSERT_FALSE(TransformationStore(
                   27, 1000, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: value id exists but does not have a type
  ASSERT_FALSE(TransformationStore(
                   27, 15, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: value id exists but has the wrong type
  ASSERT_FALSE(TransformationStore(
                   27, 14, MakeInstructionDescriptor(38, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: attempt to store to read-only variable
  ASSERT_FALSE(TransformationStore(
                   92, 93, MakeInstructionDescriptor(40, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: value is not available
  ASSERT_FALSE(TransformationStore(
                   27, 95, MakeInstructionDescriptor(40, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: variable being stored to does not have an irrelevant pointee value,
  // and the store is not in a dead block.
  ASSERT_FALSE(TransformationStore(
                   20, 95, MakeInstructionDescriptor(45, SpvOpCopyObject, 0))
                   .IsApplicable(context.get(), fact_manager));

  // The described instruction does not exist.
  ASSERT_FALSE(TransformationStore(
                   27, 80, MakeInstructionDescriptor(1000, SpvOpAccessChain, 0))
                   .IsApplicable(context.get(), fact_manager));

  {
    // Store to irrelevant variable from dead block.
    TransformationStore transformation(
        27, 80, MakeInstructionDescriptor(38, SpvOpAccessChain, 0));
    ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
    transformation.Apply(context.get(), &fact_manager);
    ASSERT_TRUE(IsValid(env, context.get()));
  }

  {
    // Store to irrelevant variable from live block.
    TransformationStore transformation(
        11, 95, MakeInstructionDescriptor(95, SpvOpReturnValue, 0));
    ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
    transformation.Apply(context.get(), &fact_manager);
    ASSERT_TRUE(IsValid(env, context.get()));
  }

  {
    // Store to irrelevant variable from live block.
    TransformationStore transformation(
        46, 80, MakeInstructionDescriptor(95, SpvOpReturnValue, 0));
    ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
    transformation.Apply(context.get(), &fact_manager);
    ASSERT_TRUE(IsValid(env, context.get()));
  }

  {
    // Store to irrelevant variable from live block.
    TransformationStore transformation(
        16, 21, MakeInstructionDescriptor(95, SpvOpReturnValue, 0));
    ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
    transformation.Apply(context.get(), &fact_manager);
    ASSERT_TRUE(IsValid(env, context.get()));
  }

  {
    // Store to non-irrelevant variable from dead block.
    TransformationStore transformation(
        53, 21, MakeInstructionDescriptor(38, SpvOpAccessChain, 0));
    ASSERT_TRUE(transformation.IsApplicable(context.get(), fact_manager));
    transformation.Apply(context.get(), &fact_manager);
    ASSERT_TRUE(IsValid(env, context.get()));
  }

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main" %92 %52 %53
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpDecorate %92 BuiltIn FragCoord
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpTypeFloat 32
          %8 = OpTypeStruct %6 %7
          %9 = OpTypePointer Function %8
         %10 = OpTypeFunction %6 %9
         %14 = OpConstant %6 0
         %15 = OpTypePointer Function %6
         %51 = OpTypePointer Private %6
         %21 = OpConstant %6 2
         %23 = OpConstant %6 1
         %24 = OpConstant %7 1
         %25 = OpTypePointer Function %7
         %50 = OpTypePointer Private %7
         %34 = OpTypeBool
         %35 = OpConstantFalse %34
         %60 = OpConstantNull %50
         %61 = OpUndef %51
         %52 = OpVariable %50 Private
         %53 = OpVariable %51 Private
         %80 = OpConstantComposite %8 %21 %24
         %90 = OpTypeVector %7 4
         %91 = OpTypePointer Input %90
         %92 = OpVariable %91 Input
         %93 = OpConstantComposite %90 %24 %24 %24 %24
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %20 = OpVariable %9 Function
         %27 = OpVariable %9 Function ; irrelevant
         %22 = OpAccessChain %15 %20 %14
         %44 = OpCopyObject %9 %20
         %26 = OpAccessChain %25 %20 %23
         %29 = OpFunctionCall %6 %12 %27
         %30 = OpAccessChain %15 %20 %14
         %45 = OpCopyObject %15 %30
         %81 = OpCopyObject %9 %27 ; irrelevant
         %33 = OpAccessChain %15 %20 %14
               OpSelectionMerge %37 None
               OpBranchConditional %35 %36 %37
         %36 = OpLabel
               OpStore %27 %80
               OpStore %53 %21
         %38 = OpAccessChain %15 %20 %14
         %40 = OpAccessChain %15 %20 %14
         %43 = OpAccessChain %15 %20 %14
         %82 = OpCopyObject %9 %27 ; irrelevant
               OpBranch %37
         %37 = OpLabel
               OpReturn
               OpFunctionEnd
         %12 = OpFunction %6 None %10
         %11 = OpFunctionParameter %9 ; irrelevant
         %13 = OpLabel
         %46 = OpCopyObject %9 %11 ; irrelevant
         %16 = OpAccessChain %15 %11 %14 ; irrelevant
         %95 = OpCopyObject %8 %80
               OpStore %11 %95
               OpStore %46 %80
               OpStore %16 %21
               OpReturnValue %21
               OpFunctionEnd
  )";
  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
