// Copyright (c) 2022 The Khronos Group Inc.
// Copyright (c) 2022 LunarG Inc.
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

#include "source/opt/eliminate_dead_input_components_pass.h"

#include <set>
#include <vector>

#include "source/opt/instruction.h"
#include "source/opt/ir_builder.h"
#include "source/opt/ir_context.h"
#include "source/util/bit_vector.h"

namespace spvtools {
namespace opt {
namespace {
constexpr uint32_t kAccessChainBaseInIdx = 0;
constexpr uint32_t kAccessChainIndex0InIdx = 1;
constexpr uint32_t kConstantValueInIdx = 0;
}  // namespace

Pass::Status EliminateDeadInputComponentsPass::Process() {
  // Process non-vertex only if explicitly allowed.
  auto stage = context()->GetStage();
  if (stage != spv::ExecutionModel::Vertex && vertex_shader_only_)
    return Status::SuccessWithoutChange;
  // Current functionality assumes shader capability.
  if (!context()->get_feature_mgr()->HasCapability(spv::Capability::Shader))
    return Status::SuccessWithoutChange;
  analysis::DefUseManager* def_use_mgr = context()->get_def_use_mgr();
  analysis::TypeManager* type_mgr = context()->get_type_mgr();
  bool modified = false;
  std::vector<Instruction*> vars_to_move;
  for (auto& var : context()->types_values()) {
    if (var.opcode() != spv::Op::OpVariable) {
      continue;
    }
    analysis::Type* var_type = type_mgr->GetType(var.type_id());
    analysis::Pointer* ptr_type = var_type->AsPointer();
    if (ptr_type == nullptr) {
      continue;
    }
    if (output_instead_) {
      if (ptr_type->storage_class() != spv::StorageClass::Output) {
        continue;
      }
    } else {
      if (ptr_type->storage_class() != spv::StorageClass::Input) {
        continue;
      }
    }
    const analysis::Array* arr_type = ptr_type->pointee_type()->AsArray();
    if (arr_type != nullptr) {
      // Only process array if input of vertex shader, or output of
      // fragment shader. Otherwise, if one shader has a runtime index and the
      // other does not, interface incompatibility can occur.
      if (!((ptr_type->storage_class() == spv::StorageClass::Input &&
             stage == spv::ExecutionModel::Vertex) ||
            (ptr_type->storage_class() == spv::StorageClass::Output &&
             stage == spv::ExecutionModel::Fragment)))
        continue;
      unsigned arr_len_id = arr_type->LengthId();
      Instruction* arr_len_inst = def_use_mgr->GetDef(arr_len_id);
      if (arr_len_inst->opcode() != spv::Op::OpConstant) {
        continue;
      }
      // SPIR-V requires array size is >= 1, so this works for signed or
      // unsigned size.
      unsigned original_max =
          arr_len_inst->GetSingleWordInOperand(kConstantValueInIdx) - 1;
      unsigned max_idx = FindMaxIndex(var, original_max);
      if (max_idx != original_max) {
        ChangeArrayLength(var, max_idx + 1);
        vars_to_move.push_back(&var);
        modified = true;
      }
      continue;
    }
    const analysis::Struct* struct_type = ptr_type->pointee_type()->AsStruct();
    if (struct_type == nullptr) continue;
    const auto elt_types = struct_type->element_types();
    unsigned original_max = static_cast<unsigned>(elt_types.size()) - 1;
    unsigned max_idx = FindMaxIndex(var, original_max);
    if (max_idx != original_max) {
      ChangeStructLength(var, max_idx + 1);
      vars_to_move.push_back(&var);
      modified = true;
    }
  }

  // Move changed vars after their new type instruction to preserve backward
  // referencing.
  for (auto var : vars_to_move) {
    auto type_id = var->type_id();
    auto type_inst = def_use_mgr->GetDef(type_id);
    var->RemoveFromList();
    var->InsertAfter(type_inst);
  }

  return modified ? Status::SuccessWithChange : Status::SuccessWithoutChange;
}

unsigned EliminateDeadInputComponentsPass::FindMaxIndex(Instruction& var,
                                                        unsigned original_max) {
  unsigned max = 0;
  bool seen_non_const_ac = false;
  assert(var.opcode() == spv::Op::OpVariable && "must be variable");
  context()->get_def_use_mgr()->WhileEachUser(
      var.result_id(), [&max, &seen_non_const_ac, var, this](Instruction* use) {
        auto use_opcode = use->opcode();
        if (use_opcode == spv::Op::OpLoad || use_opcode == spv::Op::OpStore ||
            use_opcode == spv::Op::OpCopyMemory ||
            use_opcode == spv::Op::OpCopyMemorySized ||
            use_opcode == spv::Op::OpCopyObject) {
          seen_non_const_ac = true;
          return false;
        }
        if (use->opcode() != spv::Op::OpAccessChain &&
            use->opcode() != spv::Op::OpInBoundsAccessChain) {
          return true;
        }
        // OpAccessChain with no indices currently not optimized
        if (use->NumInOperands() == 1) {
          seen_non_const_ac = true;
          return false;
        }
        unsigned base_id = use->GetSingleWordInOperand(kAccessChainBaseInIdx);
        USE_ASSERT(base_id == var.result_id() && "unexpected base");
        unsigned idx_id = use->GetSingleWordInOperand(kAccessChainIndex0InIdx);
        Instruction* idx_inst = context()->get_def_use_mgr()->GetDef(idx_id);
        if (idx_inst->opcode() != spv::Op::OpConstant) {
          seen_non_const_ac = true;
          return false;
        }
        unsigned value = idx_inst->GetSingleWordInOperand(kConstantValueInIdx);
        if (value > max) max = value;
        return true;
      });
  return seen_non_const_ac ? original_max : max;
}

void EliminateDeadInputComponentsPass::ChangeArrayLength(Instruction& arr_var,
                                                         unsigned length) {
  analysis::TypeManager* type_mgr = context()->get_type_mgr();
  analysis::ConstantManager* const_mgr = context()->get_constant_mgr();
  analysis::DefUseManager* def_use_mgr = context()->get_def_use_mgr();
  analysis::Pointer* ptr_type =
      type_mgr->GetType(arr_var.type_id())->AsPointer();
  const analysis::Array* arr_ty = ptr_type->pointee_type()->AsArray();
  assert(arr_ty && "expecting array type");
  uint32_t length_id = const_mgr->GetUIntConst(length);
  analysis::Array new_arr_ty(arr_ty->element_type(),
                             arr_ty->GetConstantLengthInfo(length_id, length));
  analysis::Type* reg_new_arr_ty = type_mgr->GetRegisteredType(&new_arr_ty);
  analysis::Pointer new_ptr_ty(reg_new_arr_ty, ptr_type->storage_class());
  analysis::Type* reg_new_ptr_ty = type_mgr->GetRegisteredType(&new_ptr_ty);
  uint32_t new_ptr_ty_id = type_mgr->GetTypeInstruction(reg_new_ptr_ty);
  arr_var.SetResultType(new_ptr_ty_id);
  def_use_mgr->AnalyzeInstUse(&arr_var);
}

void EliminateDeadInputComponentsPass::ChangeStructLength(
    Instruction& struct_var, unsigned length) {
  analysis::TypeManager* type_mgr = context()->get_type_mgr();
  analysis::Pointer* ptr_type =
      type_mgr->GetType(struct_var.type_id())->AsPointer();
  const analysis::Struct* struct_ty = ptr_type->pointee_type()->AsStruct();
  assert(struct_ty && "expecting struct type");
  const auto orig_elt_types = struct_ty->element_types();
  std::vector<const analysis::Type*> new_elt_types;
  for (unsigned u = 0; u < length; ++u)
    new_elt_types.push_back(orig_elt_types[u]);
  analysis::Struct new_struct_ty(new_elt_types);
  uint32_t old_struct_ty_id = type_mgr->GetTypeInstruction(struct_ty);
  std::vector<Instruction*> decorations =
      context()->get_decoration_mgr()->GetDecorationsFor(old_struct_ty_id,
                                                         true);
  for (auto dec : decorations) {
    if (dec->opcode() == spv::Op::OpMemberDecorate) {
      uint32_t midx = dec->GetSingleWordInOperand(1);
      if (midx >= length) continue;
    }
    type_mgr->AttachDecoration(*dec, &new_struct_ty);
  }
  analysis::Type* reg_new_struct_ty =
      type_mgr->GetRegisteredType(&new_struct_ty);
  analysis::Pointer new_ptr_ty(reg_new_struct_ty, ptr_type->storage_class());
  analysis::Type* reg_new_ptr_ty = type_mgr->GetRegisteredType(&new_ptr_ty);
  uint32_t new_ptr_ty_id = type_mgr->GetTypeInstruction(reg_new_ptr_ty);
  struct_var.SetResultType(new_ptr_ty_id);
  analysis::DefUseManager* def_use_mgr = context()->get_def_use_mgr();
  def_use_mgr->AnalyzeInstUse(&struct_var);
}

}  // namespace opt
}  // namespace spvtools
