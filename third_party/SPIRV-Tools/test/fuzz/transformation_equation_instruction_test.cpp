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

#include "source/fuzz/transformation_equation_instruction.h"
#include "source/fuzz/instruction_descriptor.h"
#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationEquationInstructionTest, SignedNegate) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpConstant %6 24
         %40 = OpTypeBool
         %41 = OpConstantTrue %40
         %20 = OpUndef %6
         %12 = OpFunction %2 None %3
         %13 = OpLabel
         %30 = OpCopyObject %6 %7
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  protobufs::InstructionDescriptor return_instruction =
      MakeInstructionDescriptor(13, SpvOpReturn, 0);

  // Bad: id already in use.
  ASSERT_FALSE(TransformationEquationInstruction(7, SpvOpSNegate, {7},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: identified instruction does not exist.
  ASSERT_FALSE(
      TransformationEquationInstruction(
          14, SpvOpSNegate, {7}, MakeInstructionDescriptor(13, SpvOpLoad, 0))
          .IsApplicable(context.get(), fact_manager));

  // Bad: id 100 does not exist
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpSNegate, {100},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: id 20 is an OpUndef
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpSNegate, {20},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: id 30 is not available right before its definition
  ASSERT_FALSE(TransformationEquationInstruction(
                   14, SpvOpSNegate, {30},
                   MakeInstructionDescriptor(30, SpvOpCopyObject, 0))
                   .IsApplicable(context.get(), fact_manager));

  // Bad: too many arguments to OpSNegate.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpSNegate, {7, 7},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: 40 is a type id.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpSNegate, {40},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: wrong type of argument to OpSNegate.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpSNegate, {41},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  auto transformation1 = TransformationEquationInstruction(
      14, SpvOpSNegate, {7}, return_instruction);
  ASSERT_TRUE(transformation1.IsApplicable(context.get(), fact_manager));
  transformation1.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation2 = TransformationEquationInstruction(
      15, SpvOpSNegate, {14}, return_instruction);
  ASSERT_TRUE(transformation2.IsApplicable(context.get(), fact_manager));
  transformation2.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(15, {}), MakeDataDescriptor(7, {}), context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
          %7 = OpConstant %6 24
         %40 = OpTypeBool
         %41 = OpConstantTrue %40
         %20 = OpUndef %6
         %12 = OpFunction %2 None %3
         %13 = OpLabel
         %30 = OpCopyObject %6 %7
         %14 = OpSNegate %6 %7
         %15 = OpSNegate %6 %14
               OpReturn
               OpFunctionEnd
  )";

  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationEquationInstructionTest, LogicalNot) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 5
         %12 = OpFunction %2 None %3
         %13 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  protobufs::InstructionDescriptor return_instruction =
      MakeInstructionDescriptor(13, SpvOpReturn, 0);

  // Bad: too few arguments to OpLogicalNot.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpLogicalNot, {},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: 6 is a type id.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpLogicalNot, {6},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  // Bad: wrong type of argument to OpLogicalNot.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpLogicalNot, {21},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  auto transformation1 = TransformationEquationInstruction(
      14, SpvOpLogicalNot, {7}, return_instruction);
  ASSERT_TRUE(transformation1.IsApplicable(context.get(), fact_manager));
  transformation1.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation2 = TransformationEquationInstruction(
      15, SpvOpLogicalNot, {14}, return_instruction);
  ASSERT_TRUE(transformation2.IsApplicable(context.get(), fact_manager));
  transformation2.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(15, {}), MakeDataDescriptor(7, {}), context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeBool
          %7 = OpConstantTrue %6
         %20 = OpTypeInt 32 0
         %21 = OpConstant %20 5
         %12 = OpFunction %2 None %3
         %13 = OpLabel
         %14 = OpLogicalNot %6 %7
         %15 = OpLogicalNot %6 %14
               OpReturn
               OpFunctionEnd
  )";

  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationEquationInstructionTest, AddSubNegate1) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
         %30 = OpTypeVector %6 3
         %15 = OpConstant %6 24
         %16 = OpConstant %6 37
         %31 = OpConstantComposite %30 %15 %16 %15
         %33 = OpTypeBool
         %32 = OpConstantTrue %33
         %12 = OpFunction %2 None %3
         %13 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  protobufs::InstructionDescriptor return_instruction =
      MakeInstructionDescriptor(13, SpvOpReturn, 0);

  // Bad: too many arguments to OpIAdd.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpIAdd, {15, 16, 16},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));
  // Bad: boolean argument to OpIAdd.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpIAdd, {15, 32},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));
  // Bad: type as argument to OpIAdd.
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpIAdd, {33, 16},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));
  // Bad: arguments of mismatched widths
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpIAdd, {15, 31},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));
  // Bad: arguments of mismatched widths
  ASSERT_FALSE(TransformationEquationInstruction(14, SpvOpIAdd, {31, 15},
                                                 return_instruction)
                   .IsApplicable(context.get(), fact_manager));

  auto transformation1 = TransformationEquationInstruction(
      14, SpvOpIAdd, {15, 16}, return_instruction);
  ASSERT_TRUE(transformation1.IsApplicable(context.get(), fact_manager));
  transformation1.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation2 = TransformationEquationInstruction(
      19, SpvOpISub, {14, 16}, return_instruction);
  ASSERT_TRUE(transformation2.IsApplicable(context.get(), fact_manager));
  transformation2.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(15, {}), MakeDataDescriptor(19, {}), context.get()));

  auto transformation3 = TransformationEquationInstruction(
      20, SpvOpISub, {14, 15}, return_instruction);
  ASSERT_TRUE(transformation3.IsApplicable(context.get(), fact_manager));
  transformation3.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(20, {}), MakeDataDescriptor(16, {}), context.get()));

  auto transformation4 = TransformationEquationInstruction(
      22, SpvOpISub, {16, 14}, return_instruction);
  ASSERT_TRUE(transformation4.IsApplicable(context.get(), fact_manager));
  transformation4.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation5 = TransformationEquationInstruction(
      24, SpvOpSNegate, {22}, return_instruction);
  ASSERT_TRUE(transformation5.IsApplicable(context.get(), fact_manager));
  transformation5.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(24, {}), MakeDataDescriptor(15, {}), context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
         %30 = OpTypeVector %6 3
         %15 = OpConstant %6 24
         %16 = OpConstant %6 37
         %31 = OpConstantComposite %30 %15 %16 %15
         %33 = OpTypeBool
         %32 = OpConstantTrue %33
         %12 = OpFunction %2 None %3
         %13 = OpLabel
         %14 = OpIAdd %6 %15 %16
         %19 = OpISub %6 %14 %16 ; ==> synonymous(%19, %15)
         %20 = OpISub %6 %14 %15 ; ==> synonymous(%20, %16)
         %22 = OpISub %6 %16 %14
         %24 = OpSNegate %6 %22 ; ==> synonymous(%24, %15)
               OpReturn
               OpFunctionEnd
  )";

  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

TEST(TransformationEquationInstructionTest, AddSubNegate2) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
         %15 = OpConstant %6 24
         %16 = OpConstant %6 37
         %12 = OpFunction %2 None %3
         %13 = OpLabel
               OpReturn
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;

  protobufs::InstructionDescriptor return_instruction =
      MakeInstructionDescriptor(13, SpvOpReturn, 0);

  auto transformation1 = TransformationEquationInstruction(
      14, SpvOpISub, {15, 16}, return_instruction);
  ASSERT_TRUE(transformation1.IsApplicable(context.get(), fact_manager));
  transformation1.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation2 = TransformationEquationInstruction(
      17, SpvOpIAdd, {14, 16}, return_instruction);
  ASSERT_TRUE(transformation2.IsApplicable(context.get(), fact_manager));
  transformation2.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(17, {}), MakeDataDescriptor(15, {}), context.get()));

  auto transformation3 = TransformationEquationInstruction(
      18, SpvOpIAdd, {16, 14}, return_instruction);
  ASSERT_TRUE(transformation3.IsApplicable(context.get(), fact_manager));
  transformation3.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(17, {}), MakeDataDescriptor(18, {}), context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(18, {}), MakeDataDescriptor(15, {}), context.get()));

  auto transformation4 = TransformationEquationInstruction(
      19, SpvOpISub, {14, 15}, return_instruction);
  ASSERT_TRUE(transformation4.IsApplicable(context.get(), fact_manager));
  transformation4.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation5 = TransformationEquationInstruction(
      20, SpvOpSNegate, {19}, return_instruction);
  ASSERT_TRUE(transformation5.IsApplicable(context.get(), fact_manager));
  transformation5.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(20, {}), MakeDataDescriptor(16, {}), context.get()));

  auto transformation6 = TransformationEquationInstruction(
      21, SpvOpISub, {14, 19}, return_instruction);
  ASSERT_TRUE(transformation6.IsApplicable(context.get(), fact_manager));
  transformation6.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(21, {}), MakeDataDescriptor(15, {}), context.get()));

  auto transformation7 = TransformationEquationInstruction(
      22, SpvOpISub, {14, 18}, return_instruction);
  ASSERT_TRUE(transformation7.IsApplicable(context.get(), fact_manager));
  transformation7.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));

  auto transformation8 = TransformationEquationInstruction(
      23, SpvOpSNegate, {22}, return_instruction);
  ASSERT_TRUE(transformation8.IsApplicable(context.get(), fact_manager));
  transformation8.Apply(context.get(), &fact_manager);
  ASSERT_TRUE(IsValid(env, context.get()));
  ASSERT_TRUE(fact_manager.IsSynonymous(
      MakeDataDescriptor(23, {}), MakeDataDescriptor(16, {}), context.get()));

  std::string after_transformation = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %12 "main"
               OpExecutionMode %12 OriginUpperLeft
               OpSource ESSL 310
          %2 = OpTypeVoid
          %3 = OpTypeFunction %2
          %6 = OpTypeInt 32 1
         %15 = OpConstant %6 24
         %16 = OpConstant %6 37
         %12 = OpFunction %2 None %3
         %13 = OpLabel
         %14 = OpISub %6 %15 %16
         %17 = OpIAdd %6 %14 %16 ; ==> synonymous(%17, %15)
         %18 = OpIAdd %6 %16 %14 ; ==> synonymous(%17, %18, %15)
         %19 = OpISub %6 %14 %15
         %20 = OpSNegate %6 %19 ; ==> synonymous(%20, %16)
         %21 = OpISub %6 %14 %19 ; ==> synonymous(%21, %15)
         %22 = OpISub %6 %14 %18
         %23 = OpSNegate %6 %22 ; ==> synonymous(%23, %16)
               OpReturn
               OpFunctionEnd
  )";

  ASSERT_TRUE(IsEqual(env, after_transformation, context.get()));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
