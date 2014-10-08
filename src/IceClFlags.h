//===- subzero/src/IceClFlags.h - Cl Flags for translation ------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares command line flags controlling translation.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECLFLAGS_H
#define SUBZERO_SRC_ICECLFLAGS_H

#include "IceTypes.def"

namespace Ice {

// TODO(stichnot) Move more command line flags into ClFlags.
class ClFlags {
public:
  ClFlags()
      : DisableInternal(false), SubzeroTimingEnabled(false),
        DisableTranslation(false), FunctionSections(false), DataSections(false),
        UseIntegratedAssembler(false), UseSandboxing(false), DumpStats(false),
        AllowUninitializedGlobals(false), TimeEachFunction(false),
        DefaultGlobalPrefix(""), DefaultFunctionPrefix(""), TimingFocusOn(""),
        VerboseFocusOn("") {}
  bool DisableInternal;
  bool SubzeroTimingEnabled;
  bool DisableTranslation;
  bool FunctionSections;
  bool DataSections;
  bool UseIntegratedAssembler;
  bool UseSandboxing;
  bool DumpStats;
  bool AllowUninitializedGlobals;
  bool TimeEachFunction;
  IceString DefaultGlobalPrefix;
  IceString DefaultFunctionPrefix;
  IceString TimingFocusOn;
  IceString VerboseFocusOn;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGS_H
