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

class ClFlags {
public:
  ClFlags()
      : DisableInternal(false), SubzeroTimingEnabled(false),
        DisableTranslation(false), FunctionSections(false), DataSections(false),
        UseELFWriter(false), UseIntegratedAssembler(false),
        UseSandboxing(false), PhiEdgeSplit(false), DecorateAsm(false),
        DumpStats(false), AllowUninitializedGlobals(false),
        TimeEachFunction(false), DisableIRGeneration(false),
        DefaultGlobalPrefix(""), DefaultFunctionPrefix(""), TimingFocusOn(""),
        VerboseFocusOn(""), TranslateOnly("") {}
  bool DisableInternal;
  bool SubzeroTimingEnabled;
  bool DisableTranslation;
  bool FunctionSections;
  bool DataSections;
  bool UseELFWriter;
  bool UseIntegratedAssembler;
  bool UseSandboxing;
  bool PhiEdgeSplit;
  bool DecorateAsm;
  bool DumpStats;
  bool AllowUninitializedGlobals;
  bool TimeEachFunction;
  bool DisableIRGeneration;
  IceString DefaultGlobalPrefix;
  IceString DefaultFunctionPrefix;
  IceString TimingFocusOn;
  IceString VerboseFocusOn;
  IceString TranslateOnly;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGS_H
