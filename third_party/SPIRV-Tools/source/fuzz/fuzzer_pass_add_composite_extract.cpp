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

#include "source/fuzz/fuzzer_pass_add_composite_extract.h"

#include "source/fuzz/fuzzer_context.h"
#include "source/fuzz/fuzzer_util.h"
#include "source/fuzz/instruction_descriptor.h"
#include "source/fuzz/transformation_composite_extract.h"

namespace spvtools {
namespace fuzz {

FuzzerPassAddCompositeExtract::FuzzerPassAddCompositeExtract(
    opt::IRContext* ir_context, TransformationContext* transformation_context,
    FuzzerContext* fuzzer_context,
    protobufs::TransformationSequence* transformations)
    : FuzzerPass(ir_context, transformation_context, fuzzer_context,
                 transformations) {}

FuzzerPassAddCompositeExtract::~FuzzerPassAddCompositeExtract() = default;

void FuzzerPassAddCompositeExtract::Apply() {
  std::vector<const protobufs::DataDescriptor*> composite_synonyms;
  for (const auto* dd :
       GetTransformationContext()->GetFactManager()->GetAllSynonyms()) {
    // |dd| must describe a component of a composite.
    if (!dd->index().empty()) {
      composite_synonyms.push_back(dd);
    }
  }

  // We don't want to invalidate the module every time we apply this
  // transformation since rebuilding DominatorAnalysis can be expensive, so we
  // collect up the transformations we wish to apply and apply them all later.
  std::vector<TransformationCompositeExtract> transformations;

  ForEachInstructionWithInstructionDescriptor(
      [this, &composite_synonyms, &transformations](
          opt::Function* function, opt::BasicBlock* block,
          opt::BasicBlock::iterator inst_it,
          const protobufs::InstructionDescriptor& instruction_descriptor) {
        if (!fuzzerutil::CanInsertOpcodeBeforeInstruction(SpvOpCompositeExtract,
                                                          inst_it)) {
          return;
        }

        if (!GetFuzzerContext()->ChoosePercentage(
                GetFuzzerContext()->GetChanceOfAddingCompositeExtract())) {
          return;
        }

        auto available_composites = FindAvailableInstructions(
            function, block, inst_it,
            [](opt::IRContext* ir_context, opt::Instruction* inst) {
              return inst->type_id() && inst->result_id() &&
                     fuzzerutil::IsCompositeType(
                         ir_context->get_type_mgr()->GetType(inst->type_id()));
            });

        std::vector<const protobufs::DataDescriptor*> available_synonyms;
        for (const auto* dd : composite_synonyms) {
          if (fuzzerutil::IdIsAvailableBeforeInstruction(
                  GetIRContext(), &*inst_it, dd->object())) {
            available_synonyms.push_back(dd);
          }
        }

        if (available_synonyms.empty() && available_composites.empty()) {
          return;
        }

        uint32_t composite_id = 0;
        std::vector<uint32_t> indices;

        if (available_synonyms.empty() || (!available_composites.empty() &&
                                           GetFuzzerContext()->ChooseEven())) {
          const auto* inst =
              available_composites[GetFuzzerContext()->RandomIndex(
                  available_composites)];
          composite_id = inst->result_id();

          const auto* type =
              GetIRContext()->get_type_mgr()->GetType(inst->type_id());
          assert(type && "Composite instruction has invalid type id");

          do {
            uint32_t number_of_members = 0;

            if (const auto* array_type = type->AsArray()) {
              const auto* type_inst =
                  GetIRContext()->get_def_use_mgr()->GetDef(inst->type_id());
              assert(type_inst && "Type instruction must exist");

              number_of_members =
                  fuzzerutil::GetArraySize(*type_inst, GetIRContext());
              type = array_type->element_type();
            } else if (const auto* vector_type = type->AsVector()) {
              number_of_members = vector_type->element_count();
              type = vector_type->element_type();
            } else if (const auto* matrix_type = type->AsMatrix()) {
              number_of_members = matrix_type->element_count();
              type = matrix_type->element_type();
            } else if (const auto* struct_type = type->AsStruct()) {
              number_of_members =
                  static_cast<uint32_t>(struct_type->element_types().size());
              // The next value of |type| will be assigned when we know the
              // index of the OpTypeStruct's operand.
            } else {
              assert(false && "|inst| is not a composite");
              return;
            }

            if (number_of_members == 0) {
              return;
            }

            indices.push_back(
                GetFuzzerContext()->GetRandomCompositeExtractIndex(
                    number_of_members));

            if (const auto* struct_type = type->AsStruct()) {
              type = struct_type->element_types()[indices.back()];
            }
          } while (fuzzerutil::IsCompositeType(type) &&
                   GetFuzzerContext()->ChoosePercentage(
                       GetFuzzerContext()
                           ->GetChanceOfGoingDeeperToExtractComposite()));
        } else {
          const auto* dd = available_synonyms[GetFuzzerContext()->RandomIndex(
              available_synonyms)];

          composite_id = dd->object();
          indices.assign(dd->index().begin(), dd->index().end());
        }

        assert(composite_id != 0 && !indices.empty() &&
               "Composite object should have been chosen correctly");

        transformations.emplace_back(instruction_descriptor,
                                     GetFuzzerContext()->GetFreshId(),
                                     composite_id, indices);
      });

  for (const auto& transformation : transformations) {
    ApplyTransformation(transformation);
  }
}

}  // namespace fuzz
}  // namespace spvtools
