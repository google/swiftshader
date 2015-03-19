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
  ClFlags(const ClFlags &) = delete;
  ClFlags &operator=(const ClFlags &) = delete;

public:
  ClFlags()
      : // bool fields.
        AllowErrorRecovery(false),
        AllowUninitializedGlobals(false), DataSections(false),
        DecorateAsm(false), DisableInternal(false), DisableIRGeneration(false),
        DisableTranslation(false), DumpStats(false), FunctionSections(false),
        GenerateUnitTestMessages(false), PhiEdgeSplit(false),
        SubzeroTimingEnabled(false), TimeEachFunction(false),
        UseSandboxing(false),
        // FileType field
        OutFileType(FT_Iasm),
        // IceString fields.
        DefaultFunctionPrefix(""), DefaultGlobalPrefix(""), TimingFocusOn(""),
        TranslateOnly(""), VerboseFocusOn(""),
        // size_t fields.
        NumTranslationThreads(0) {}

  // bool accessors.

  bool getAllowErrorRecovery() const { return AllowErrorRecovery; }
  void setAllowErrorRecovery(bool NewValue) { AllowErrorRecovery = NewValue; }

  bool getAllowUninitializedGlobals() const {
    return AllowUninitializedGlobals;
  }
  void setAllowUninitializedGlobals(bool NewValue) {
    AllowUninitializedGlobals = NewValue;
  }

  bool getDataSections() const { return DataSections; }
  void setDataSections(bool NewValue) { DataSections = NewValue; }

  bool getDecorateAsm() const { return DecorateAsm; }
  void setDecorateAsm(bool NewValue) { DecorateAsm = NewValue; }

  bool getDisableInternal() const { return DisableInternal; }
  void setDisableInternal(bool NewValue) { DisableInternal = NewValue; }

  bool getDisableIRGeneration() const {
    return ALLOW_DISABLE_IR_GEN && DisableIRGeneration;
  }
  void setDisableIRGeneration(bool NewValue) { DisableIRGeneration = NewValue; }

  bool getDisableTranslation() const { return DisableTranslation; }
  void setDisableTranslation(bool NewValue) { DisableTranslation = NewValue; }

  bool getDumpStats() const { return ALLOW_DUMP && DumpStats; }
  void setDumpStats(bool NewValue) { DumpStats = NewValue; }

  bool getFunctionSections() const { return FunctionSections; }
  void setFunctionSections(bool NewValue) { FunctionSections = NewValue; }

  bool getGenerateUnitTestMessages() const {
    // Note: If dump routines have been turned off, the error messages
    // will not be readable. Hence, turn off.
    return !ALLOW_DUMP || GenerateUnitTestMessages;
  }
  void setGenerateUnitTestMessages(bool NewValue) {
    GenerateUnitTestMessages = NewValue;
  }

  bool getPhiEdgeSplit() const { return PhiEdgeSplit; }
  void setPhiEdgeSplit(bool NewValue) { PhiEdgeSplit = NewValue; }

  bool getSubzeroTimingEnabled() const { return SubzeroTimingEnabled; }
  void setSubzeroTimingEnabled(bool NewValue) {
    SubzeroTimingEnabled = NewValue;
  }

  bool getTimeEachFunction() const { return ALLOW_DUMP && TimeEachFunction; }
  void setTimeEachFunction(bool NewValue) { TimeEachFunction = NewValue; }

  bool getUseSandboxing() const { return UseSandboxing; }
  void setUseSandboxing(bool NewValue) { UseSandboxing = NewValue; }

  // FileType accessor.
  FileType getOutFileType() const { return OutFileType; }
  void setOutFileType(FileType NewValue) { OutFileType = NewValue; }

  // IceString accessors.

  const IceString &getDefaultFunctionPrefix() const {
    return DefaultFunctionPrefix;
  }
  void setDefaultFunctionPrefix(const IceString &NewValue) {
    DefaultFunctionPrefix = NewValue;
  }

  const IceString &getDefaultGlobalPrefix() const {
    return DefaultGlobalPrefix;
  }
  void setDefaultGlobalPrefix(const IceString &NewValue) {
    DefaultGlobalPrefix = NewValue;
  }

  const IceString &getTimingFocusOn() const { return TimingFocusOn; }
  void setTimingFocusOn(const IceString &NewValue) { TimingFocusOn = NewValue; }

  const IceString &getTranslateOnly() const { return TranslateOnly; }
  void setTranslateOnly(const IceString &NewValue) { TranslateOnly = NewValue; }

  const IceString &getVerboseFocusOn() const { return VerboseFocusOn; }
  void setVerboseFocusOn(const IceString &NewValue) {
    VerboseFocusOn = NewValue;
  }

  // size_t accessors.

  size_t getNumTranslationThreads() const { return NumTranslationThreads; }
  bool isSequential() const { return NumTranslationThreads == 0; }
  void setNumTranslationThreads(size_t NewValue) {
    NumTranslationThreads = NewValue;
  }

private:
  bool AllowErrorRecovery;
  bool AllowUninitializedGlobals;
  bool DataSections;
  bool DecorateAsm;
  bool DisableInternal;
  bool DisableIRGeneration;
  bool DisableTranslation;
  bool DumpStats;
  bool FunctionSections;
  bool GenerateUnitTestMessages;
  bool PhiEdgeSplit;
  bool SubzeroTimingEnabled;
  bool TimeEachFunction;
  bool UseSandboxing;

  FileType OutFileType;

  IceString DefaultFunctionPrefix;
  IceString DefaultGlobalPrefix;
  IceString TimingFocusOn;
  IceString TranslateOnly;
  IceString VerboseFocusOn;

  size_t NumTranslationThreads; // 0 means completely sequential
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGS_H
