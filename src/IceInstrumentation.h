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
  virtual void instrumentGlobals() {};
  void instrumentFunc(Cfg *Func);

private:
  void instrumentInst(LoweringContext &Context);
  virtual void instrumentFuncStart(LoweringContext &Context) {
    (void) Context;
  }
  virtual void instrumentAlloca(LoweringContext &Context,
                                const class InstAlloca *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentArithmetic(LoweringContext &Context,
                                    const class InstArithmetic *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentBr(LoweringContext &Context,
                            const class InstBr *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentCall(LoweringContext &Context,
                              const class InstCall *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentCast(LoweringContext &Context,
                              const class InstCast *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentExtractElement(LoweringContext &Context,
                                        const class InstExtractElement *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentFcmp(LoweringContext &Context,
                              const class InstFcmp *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentIcmp(LoweringContext &Context,
                              const class InstIcmp *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentInsertElement(LoweringContext &Context,
                                       const class InstInsertElement *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentIntrinsicCall(LoweringContext &Context,
                                       const class InstIntrinsicCall *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentLoad(LoweringContext &Context,
                              const class InstLoad *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentPhi(LoweringContext &Context,
                             const class InstPhi *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentRet(LoweringContext &Context,
                             const class InstRet *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentSelect(LoweringContext &Context,
                                const class InstSelect *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentStore(LoweringContext &Context,
                               const class InstStore *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentSwitch(LoweringContext &Context,
                                const class InstSwitch *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentUnreachable(LoweringContext &Context,
                                     const class InstUnreachable *Instr) {
    (void) Context;
    (void) Instr;
  }
  virtual void instrumentLocalVars(Cfg *Func) {
    (void) Func;
  }

protected:
  GlobalContext *Ctx;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTRUMENTATION_H
