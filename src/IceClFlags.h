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

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class ClFlagsExtra;

class ClFlags {
  ClFlags(const ClFlags &) = delete;
  ClFlags &operator=(const ClFlags &) = delete;

public:
  ClFlags() { resetClFlags(*this); }

  static void parseFlags(int argc, char *argv[]);
  static void resetClFlags(ClFlags &OutFlags);
  static void getParsedClFlags(ClFlags &OutFlags);
  static void getParsedClFlagsExtra(ClFlagsExtra &OutFlagsExtra);

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

  bool getEnableBlockProfile() const { return EnableBlockProfile; }
  void setEnableBlockProfile(bool NewValue) { EnableBlockProfile = NewValue; }

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

  bool shouldDoNopInsertion() const { return RandomNopInsertion; }
  void setShouldDoNopInsertion(bool NewValue) { RandomNopInsertion = NewValue; }

  bool shouldRandomizeRegAlloc() const { return RandomRegAlloc; }
  void setShouldRandomizeRegAlloc(bool NewValue) { RandomRegAlloc = NewValue; }

  bool getSkipUnimplemented() const { return SkipUnimplemented; }
  void setSkipUnimplemented(bool NewValue) { SkipUnimplemented = NewValue; }

  bool getSubzeroTimingEnabled() const { return SubzeroTimingEnabled; }
  void setSubzeroTimingEnabled(bool NewValue) {
    SubzeroTimingEnabled = NewValue;
  }

  bool getTimeEachFunction() const { return ALLOW_DUMP && TimeEachFunction; }
  void setTimeEachFunction(bool NewValue) { TimeEachFunction = NewValue; }

  bool getUseSandboxing() const { return UseSandboxing; }
  void setUseSandboxing(bool NewValue) { UseSandboxing = NewValue; }

  // Enum and integer accessors.
  OptLevel getOptLevel() const { return Opt; }
  void setOptLevel(OptLevel NewValue) { Opt = NewValue; }

  FileType getOutFileType() const { return OutFileType; }
  void setOutFileType(FileType NewValue) { OutFileType = NewValue; }

  int getMaxNopsPerInstruction() const { return RandomMaxNopsPerInstruction; }
  void setMaxNopsPerInstruction(int NewValue) {
    RandomMaxNopsPerInstruction = NewValue;
  }

  int getNopProbabilityAsPercentage() const {
    return RandomNopProbabilityAsPercentage;
  }
  void setNopProbabilityAsPercentage(int NewValue) {
    RandomNopProbabilityAsPercentage = NewValue;
  }

  TargetArch getTargetArch() const { return TArch; }
  void setTargetArch(TargetArch NewValue) { TArch = NewValue; }

  TargetInstructionSet getTargetInstructionSet() const { return TInstrSet; }
  void setTargetInstructionSet(TargetInstructionSet NewValue) {
    TInstrSet = NewValue;
  }

  VerboseMask getVerbose() const {
    return ALLOW_DUMP ? VMask : (VerboseMask)IceV_None;
  }
  void setVerbose(VerboseMask NewValue) { VMask = NewValue; }

  void
  setRandomizeAndPoolImmediatesOption(RandomizeAndPoolImmediatesEnum Option) {
    RandomizeAndPoolImmediatesOption = Option;
  }

  RandomizeAndPoolImmediatesEnum getRandomizeAndPoolImmediatesOption() const {
    return RandomizeAndPoolImmediatesOption;
  }

  void setRandomizeAndPoolImmediatesThreshold(uint32_t Threshold) {
    RandomizeAndPoolImmediatesThreshold = Threshold;
  }
  uint32_t getRandomizeAndPoolImmediatesThreshold() const {
    return RandomizeAndPoolImmediatesThreshold;
  }

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

  const IceString &getTestPrefix() const { return TestPrefix; }
  void setTestPrefix(const IceString &NewValue) { TestPrefix = NewValue; }

  const IceString &getTimingFocusOn() const { return TimingFocusOn; }
  void setTimingFocusOn(const IceString &NewValue) { TimingFocusOn = NewValue; }

  const IceString &getTranslateOnly() const { return TranslateOnly; }
  void setTranslateOnly(const IceString &NewValue) { TranslateOnly = NewValue; }

  const IceString &getVerboseFocusOn() const { return VerboseFocusOn; }
  void setVerboseFocusOn(const IceString &NewValue) {
    VerboseFocusOn = NewValue;
  }

  // size_t and 64-bit accessors.

  size_t getNumTranslationThreads() const { return NumTranslationThreads; }
  bool isSequential() const { return NumTranslationThreads == 0; }
  void setNumTranslationThreads(size_t NewValue) {
    NumTranslationThreads = NewValue;
  }

  uint64_t getRandomSeed() const { return RandomSeed; }
  void setRandomSeed(size_t NewValue) { RandomSeed = NewValue; }

private:
  bool AllowErrorRecovery;
  bool AllowUninitializedGlobals;
  bool DataSections;
  bool DecorateAsm;
  bool DisableInternal;
  bool DisableIRGeneration;
  bool DisableTranslation;
  bool DumpStats;
  bool EnableBlockProfile;
  bool FunctionSections;
  bool GenerateUnitTestMessages;
  bool PhiEdgeSplit;
  bool RandomNopInsertion;
  bool RandomRegAlloc;
  bool SkipUnimplemented;
  bool SubzeroTimingEnabled;
  bool TimeEachFunction;
  bool UseSandboxing;

  OptLevel Opt;
  FileType OutFileType;
  int RandomMaxNopsPerInstruction;
  int RandomNopProbabilityAsPercentage;
  TargetArch TArch;
  TargetInstructionSet TInstrSet;
  VerboseMask VMask;

  IceString DefaultFunctionPrefix;
  IceString DefaultGlobalPrefix;
  IceString TestPrefix;
  IceString TimingFocusOn;
  IceString TranslateOnly;
  IceString VerboseFocusOn;

  // Immediates Randomization and Pooling options
  RandomizeAndPoolImmediatesEnum RandomizeAndPoolImmediatesOption;
  uint32_t RandomizeAndPoolImmediatesThreshold;

  size_t NumTranslationThreads; // 0 means completely sequential
  uint64_t RandomSeed;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGS_H
