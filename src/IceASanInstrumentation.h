//===- subzero/src/IceASanInstrumentation.h - AddressSanitizer --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the AddressSanitizer instrumentation class.
///
/// This class is responsible for inserting redzones around global and stack
/// variables, inserting code responsible for poisoning those redzones, and
/// performing any other instrumentation necessary to implement
/// AddressSanitizer.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASANINSTRUMENTATION_H
#define SUBZERO_SRC_ICEASANINSTRUMENTATION_H

#include "IceGlobalInits.h"
#include "IceInstrumentation.h"

#include <condition_variable>

namespace Ice {

class ASanInstrumentation : public Instrumentation {
  ASanInstrumentation() = delete;
  ASanInstrumentation(const ASanInstrumentation &) = delete;
  ASanInstrumentation &operator=(const ASanInstrumentation &) = delete;

public:
  ASanInstrumentation(GlobalContext *Ctx)
      : Instrumentation(Ctx), RzNum(0),
        GlobalsLock(GlobalsMutex, std::defer_lock) {
    ICE_TLS_INIT_FIELD(LocalDtors);
  }
  void instrumentGlobals(VariableDeclarationList &Globals) override;

private:
  std::string nextRzName();
  void instrumentFuncStart(LoweringContext &Context) override;
  void instrumentCall(LoweringContext &Context, InstCall *Instr) override;
  void instrumentRet(LoweringContext &Context, InstRet *Instr) override;
  void instrumentLoad(LoweringContext &Context, InstLoad *Instr) override;
  void instrumentStore(LoweringContext &Context, InstStore *Instr) override;
  void instrumentAccess(LoweringContext &Context, Operand *Op, SizeT Size,
                        Constant *AccessFunc);
  void instrumentStart(Cfg *Func) override;
  void finishFunc(Cfg *Func) override;
  ICE_TLS_DECLARE_FIELD(std::vector<InstCall *> *, LocalDtors);
  std::atomic<uint32_t> RzNum;
  bool DidProcessGlobals = false;
  SizeT RzGlobalsNum = 0;
  std::mutex GlobalsMutex;
  std::unique_lock<std::mutex> GlobalsLock;
  std::condition_variable GlobalsDoneCV;
};
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASANINSTRUMENTATION_H
