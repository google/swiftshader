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

namespace Ice {

class ASanInstrumentation : public Instrumentation {
  ASanInstrumentation() = delete;
  ASanInstrumentation(const ASanInstrumentation &) = delete;
  ASanInstrumentation &operator=(const ASanInstrumentation &) = delete;

public:
  ASanInstrumentation(GlobalContext *Ctx) : Instrumentation(Ctx) {}
  void instrumentGlobals(VariableDeclarationList &Globals) override;

private:
  std::string nextRzName();
  VariableDeclaration *createRz(VariableDeclarationList *List,
                                VariableDeclaration *RzArray,
                                SizeT &RzArraySize,
                                VariableDeclaration *Global);
  void instrumentStart(Cfg *Func) override;
  bool DidInsertRedZones = false;
  uint32_t RzNum = 0;
};
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASANINSTRUMENTATION_H
