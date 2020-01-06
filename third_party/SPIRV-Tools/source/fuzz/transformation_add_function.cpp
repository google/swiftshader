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

#include "source/fuzz/fuzzer_util.h"
#include "source/fuzz/instruction_message.h"

namespace spvtools {
namespace fuzz {

TransformationAddFunction::TransformationAddFunction(
    const spvtools::fuzz::protobufs::TransformationAddFunction& message)
    : message_(message) {}

TransformationAddFunction::TransformationAddFunction(
    const std::vector<protobufs::Instruction>& instructions) {
  for (auto& instruction : instructions) {
    *message_.add_instruction() = instruction;
  }
}

bool TransformationAddFunction::IsApplicable(
    opt::IRContext* context,
    const spvtools::fuzz::FactManager& /*unused*/) const {
  // Because checking all the conditions for a function to be valid is a big
  // job that the SPIR-V validator can already do, a "try it and see" approach
  // is taken here.

  // We first clone the current module, so that we can try adding the new
  // function without risking wrecking |context|.
  auto cloned_module = fuzzerutil::CloneIRContext(context);

  // We try to add a function to the cloned module, which may fail if
  // |message_.instruction| is not sufficiently well-formed.
  if (!TryToAddFunction(cloned_module.get())) {
    return false;
  }
  // Having managed to add the new function to the cloned module, we ascertain
  // whether the cloned module is still valid.  If it is, the transformation is
  // applicable.
  return fuzzerutil::IsValid(cloned_module.get());
}

void TransformationAddFunction::Apply(
    opt::IRContext* context, spvtools::fuzz::FactManager* /*unused*/) const {
  auto success = TryToAddFunction(context);
  assert(success && "The function should be successfully added.");
  (void)(success);  // Keep release builds happy (otherwise they may complain
                    // that |success| is not used).
  context->InvalidateAnalysesExceptFor(opt::IRContext::kAnalysisNone);
}

protobufs::Transformation TransformationAddFunction::ToMessage() const {
  protobufs::Transformation result;
  *result.mutable_add_function() = message_;
  return result;
}

bool TransformationAddFunction::TryToAddFunction(
    opt::IRContext* context) const {
  // This function returns false if |message_.instruction| was not well-formed
  // enough to actually create a function and add it to |context|.

  // A function must have at least some instructions.
  if (message_.instruction().empty()) {
    return false;
  }

  // A function must start with OpFunction.
  auto function_begin = message_.instruction(0);
  if (function_begin.opcode() != SpvOpFunction) {
    return false;
  }

  // Make a function, headed by the OpFunction instruction.
  std::unique_ptr<opt::Function> new_function = MakeUnique<opt::Function>(
      InstructionFromMessage(context, function_begin));

  // Keeps track of which instruction protobuf message we are currently
  // considering.
  uint32_t instruction_index = 1;
  const auto num_instructions =
      static_cast<uint32_t>(message_.instruction().size());

  // Iterate through all function parameter instructions, adding parameters to
  // the new function.
  while (instruction_index < num_instructions &&
         message_.instruction(instruction_index).opcode() ==
             SpvOpFunctionParameter) {
    new_function->AddParameter(InstructionFromMessage(
        context, message_.instruction(instruction_index)));
    instruction_index++;
  }

  // After the parameters, there needs to be a label.
  if (instruction_index == num_instructions ||
      message_.instruction(instruction_index).opcode() != SpvOpLabel) {
    return false;
  }

  // Iterate through the instructions block by block until the end of the
  // function is reached.
  while (instruction_index < num_instructions &&
         message_.instruction(instruction_index).opcode() != SpvOpFunctionEnd) {
    // Invariant: we should always be at a label instruction at this point.
    assert(message_.instruction(instruction_index).opcode() == SpvOpLabel);

    // Make a basic block using the label instruction, with the new function
    // as its parent.
    std::unique_ptr<opt::BasicBlock> block =
        MakeUnique<opt::BasicBlock>(InstructionFromMessage(
            context, message_.instruction(instruction_index)));
    block->SetParent(new_function.get());

    // Consider successive instructions until we hit another label or the end
    // of the function, adding each such instruction to the block.
    instruction_index++;
    while (instruction_index < num_instructions &&
           message_.instruction(instruction_index).opcode() !=
               SpvOpFunctionEnd &&
           message_.instruction(instruction_index).opcode() != SpvOpLabel) {
      block->AddInstruction(InstructionFromMessage(
          context, message_.instruction(instruction_index)));
      instruction_index++;
    }
    // Add the block to the new function.
    new_function->AddBasicBlock(std::move(block));
  }
  // Having considered all the blocks, we should be at the last instruction and
  // it needs to be OpFunctionEnd.
  if (instruction_index != num_instructions - 1 ||
      message_.instruction(instruction_index).opcode() != SpvOpFunctionEnd) {
    return false;
  }
  // Set the function's final instruction, add the function to the module and
  // report success.
  new_function->SetFunctionEnd(
      InstructionFromMessage(context, message_.instruction(instruction_index)));
  context->AddFunction(std::move(new_function));
  return true;
}

}  // namespace fuzz
}  // namespace spvtools
