// Copyright (c) 2020 Vasyl Teliman
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

#include "source/fuzz/transformation_add_parameter.h"

#include "test/fuzz/fuzz_test_util.h"

namespace spvtools {
namespace fuzz {
namespace {

TEST(TransformationAddParameterTest, BasicTest) {
  std::string shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %7 = OpTypeBool
         %11 = OpTypeInt 32 1
          %3 = OpTypeFunction %2
          %6 = OpTypeFunction %7 %7
          %8 = OpConstant %11 23
         %12 = OpConstantTrue %7
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %13 = OpFunctionCall %7 %9 %12
               OpReturn
               OpFunctionEnd
          %9 = OpFunction %7 None %6
         %14 = OpFunctionParameter %7
         %10 = OpLabel
               OpReturnValue %12
               OpFunctionEnd
  )";

  const auto env = SPV_ENV_UNIVERSAL_1_3;
  const auto consumer = nullptr;
  const auto context = BuildModule(env, consumer, shader, kFuzzAssembleOption);
  ASSERT_TRUE(IsValid(env, context.get()));

  FactManager fact_manager;
  spvtools::ValidatorOptions validator_options;
  TransformationContext transformation_context(&fact_manager,
                                               validator_options);

  // Can't modify entry point function.
  ASSERT_FALSE(TransformationAddParameter(4, 15, 12, 16)
                   .IsApplicable(context.get(), transformation_context));

  // There is no function with result id 29.
  ASSERT_FALSE(TransformationAddParameter(29, 15, 8, 16)
                   .IsApplicable(context.get(), transformation_context));

  // Parameter id is not fresh.
  ASSERT_FALSE(TransformationAddParameter(9, 14, 8, 16)
                   .IsApplicable(context.get(), transformation_context));

  // Function type id is not fresh.
  ASSERT_FALSE(TransformationAddParameter(9, 15, 8, 14)
                   .IsApplicable(context.get(), transformation_context));

  // Function type id and parameter type id are equal.
  ASSERT_FALSE(TransformationAddParameter(9, 15, 8, 15)
                   .IsApplicable(context.get(), transformation_context));

  // Parameter's initializer doesn't exist.
  ASSERT_FALSE(TransformationAddParameter(9, 15, 15, 16)
                   .IsApplicable(context.get(), transformation_context));

  // Correct transformation.
  TransformationAddParameter correct(9, 15, 8, 16);
  ASSERT_TRUE(correct.IsApplicable(context.get(), transformation_context));
  correct.Apply(context.get(), &transformation_context);

  // The module remains valid.
  ASSERT_TRUE(IsValid(env, context.get()));

  ASSERT_TRUE(fact_manager.IdIsIrrelevant(15));

  std::string expected_shader = R"(
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %4 "main"
               OpExecutionMode %4 OriginUpperLeft
               OpSource ESSL 310
               OpName %4 "main"
          %2 = OpTypeVoid
          %7 = OpTypeBool
         %11 = OpTypeInt 32 1
          %3 = OpTypeFunction %2
          %8 = OpConstant %11 23
         %12 = OpConstantTrue %7
          %6 = OpTypeFunction %7 %7 %11
          %4 = OpFunction %2 None %3
          %5 = OpLabel
         %13 = OpFunctionCall %7 %9 %12 %8
               OpReturn
               OpFunctionEnd
          %9 = OpFunction %7 None %6
         %14 = OpFunctionParameter %7
         %15 = OpFunctionParameter %11
         %10 = OpLabel
               OpReturnValue %12
               OpFunctionEnd
  )";

  ASSERT_TRUE(IsEqual(env, expected_shader, context.get()));
}

}  // namespace
}  // namespace fuzz
}  // namespace spvtools
