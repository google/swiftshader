//===- subzero/src/IceInstrumentation.h - ICE instrumentation ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the Ice::Instrumentation class.
///
/// Instrumentation is an abstract class used to drive the instrumentation
/// process for tools such as AddressSanitizer and MemorySanitizer. It uses a
/// LoweringContext to enable the insertion of new instructions into a given
/// Cfg. Although Instrumentation is an abstract class, each of its virtual
/// functions has a trivial default implementation to make subclasses more
/// succinct.
///
/// If instrumentation is required by the command line arguments, a single
/// Instrumentation subclass is instantiated and installed in the
/// GlobalContext. If multiple types of instrumentation are requested, a single
/// subclass is still responsible for driving the instrumentation, but it can
/// use other Instrumentation subclasses however it needs to.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTRUMENTATION_H
#define SUBZERO_SRC_ICEINSTRUMENTATION_H

#include "IceDefs.h"

namespace Ice {

class LoweringContext;

class Instrumentation {
  Instrumentation() = delete;
  Instrumentation(const Instrumentation &) = delete;
  Instrumentation &operator=(const Instrumentation &) = delete;

public:
  Instrumentation(GlobalContext *Ctx) : Ctx(Ctx) {}
  virtual void instrumentGlobals(VariableDeclarationList &) {}
  void instrumentFunc(Cfg *Func);

private:
  void instrumentInst(LoweringContext &Context);
  virtual void instrumentFuncStart(LoweringContext &) {}
  virtual void instrumentAlloca(LoweringContext &, const class InstAlloca *) {}
  virtual void instrumentArithmetic(LoweringContext &,
                                    const class InstArithmetic *) {}
  virtual void instrumentBr(LoweringContext &, const class InstBr *) {}
  virtual void instrumentCall(LoweringContext &, const class InstCall *) {}
  virtual void instrumentCast(LoweringContext &, const class InstCast *) {}
  virtual void instrumentExtractElement(LoweringContext &,
                                        const class InstExtractElement *) {}
  virtual void instrumentFcmp(LoweringContext &, const class InstFcmp *) {}
  virtual void instrumentIcmp(LoweringContext &, const class InstIcmp *) {}
  virtual void instrumentInsertElement(LoweringContext &,
                                       const class InstInsertElement *) {}
  virtual void instrumentIntrinsicCall(LoweringContext &,
                                       const class InstIntrinsicCall *) {}
  virtual void instrumentLoad(LoweringContext &, const class InstLoad *) {}
  virtual void instrumentPhi(LoweringContext &, const class InstPhi *) {}
  virtual void instrumentRet(LoweringContext &, const class InstRet *) {}
  virtual void instrumentSelect(LoweringContext &, const class InstSelect *) {}
  virtual void instrumentStore(LoweringContext &, const class InstStore *) {}
  virtual void instrumentSwitch(LoweringContext &, const class InstSwitch *) {}
  virtual void instrumentUnreachable(LoweringContext &,
                                     const class InstUnreachable *) {}
  virtual void instrumentStart(Cfg *) {}
  virtual void instrumentLocalVars(Cfg *) {}

protected:
  GlobalContext *Ctx;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTRUMENTATION_H
