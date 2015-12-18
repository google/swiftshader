//===- subzero/src/IceClFlags.h - Cl Flags for translation ------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares Ice::ClFlags which implements command line processing.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECLFLAGS_H
#define SUBZERO_SRC_ICECLFLAGS_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class ClFlagsExtra;

/// Define variables which configure translation and related support functions.
class ClFlags {
  ClFlags(const ClFlags &) = delete;
  ClFlags &operator=(const ClFlags &) = delete;

public:
  /// User defined constructor.
  ClFlags() { resetClFlags(*this); }

  /// \brief Parse commmand line options for Subzero.
  ///
  /// This is done use cl::ParseCommandLineOptions() and the static variables of
  /// type cl::opt defined in IceClFlags.cpp
  static void parseFlags(int argc, char *argv[]);
  /// Reset all configuration options to their nominal values.
  static void resetClFlags(ClFlags &OutFlags);
  /// \brief Retrieve the configuration option state
  ///
  /// This is defined by static variables
  /// anonymous_namespace{IceClFlags.cpp}::AllowErrorRecovery,
  /// anonymous_namespace{IceClFlags.cpp}::AllowIacaMarks,
  /// ...
  static void getParsedClFlags(ClFlags &OutFlags);
  /// Retrieve the extra configuration options state.
  static void getParsedClFlagsExtra(ClFlagsExtra &OutFlagsExtra);

  // bool accessors.

  /// Get the value of ClFlags::AllowErrorRecovery
  bool getAllowErrorRecovery() const { return AllowErrorRecovery; }
  /// Set ClFlags::AllowErrorRecovery to a new value
  void setAllowErrorRecovery(bool NewValue) { AllowErrorRecovery = NewValue; }

  /// Get the value of ClFlags::AllowExternDefinedSymbols
  bool getAllowExternDefinedSymbols() const {
    return AllowExternDefinedSymbols;
  }
  /// Set ClFlags::AllowExternDefinedSymbols to a new value
  void setAllowExternDefinedSymbols(bool NewValue) {
    AllowExternDefinedSymbols = NewValue;
  }

  /// Get the value of ClFlags::AllowIacaMarks
  bool getAllowIacaMarks() const { return AllowIacaMarks; }
  /// Set ClFlags::AllowIacaMarks to a new value
  void setAllowIacaMarks(bool NewValue) { AllowIacaMarks = NewValue; }

  /// Get the value of ClFlags::AllowUninitializedGlobals
  bool getAllowUninitializedGlobals() const {
    return AllowUninitializedGlobals;
  }
  /// Set ClFlags::AllowUninitializedGlobals to a new value
  void setAllowUninitializedGlobals(bool NewValue) {
    AllowUninitializedGlobals = NewValue;
  }

  /// Get the value of ClFlags::DataSections
  bool getDataSections() const { return DataSections; }
  /// Set ClFlags::DataSections to a new value
  void setDataSections(bool NewValue) { DataSections = NewValue; }

  /// Get the value of ClFlags::DecorateAsm
  bool getDecorateAsm() const { return DecorateAsm; }
  /// Set ClFlags::DecorateAsm to a new value
  void setDecorateAsm(bool NewValue) { DecorateAsm = NewValue; }

  /// Get the value of ClFlags::DisableHybridAssembly
  bool getDisableHybridAssembly() const { return DisableHybridAssembly; }
  /// Set ClFlags::DisableHybridAssembly to a new value
  void setDisableHybridAssembly(bool NewValue) {
    DisableHybridAssembly = NewValue;
  }

  /// Get the value of ClFlags::DisableInternal
  bool getDisableInternal() const { return DisableInternal; }
  /// Set ClFlags::DisableInternal to a new value
  void setDisableInternal(bool NewValue) { DisableInternal = NewValue; }

  /// Get the value of ClFlags::DisableTranslation
  bool getDisableTranslation() const { return DisableTranslation; }
  /// Set ClFlags::DisableTranslation to a new value
  void setDisableTranslation(bool NewValue) { DisableTranslation = NewValue; }

  /// Get the value of ClFlags::DumpStats
  bool getDumpStats() const { return BuildDefs::dump() && DumpStats; }
  /// Set ClFlags::DumpStats to a new value
  void setDumpStats(bool NewValue) { DumpStats = NewValue; }

  /// Get the value of ClFlags::EnableBlockProfile
  bool getEnableBlockProfile() const { return EnableBlockProfile; }
  /// Set ClFlags::EnableBlockProfile to a new value
  void setEnableBlockProfile(bool NewValue) { EnableBlockProfile = NewValue; }

  /// Get the value of ClFlags::ForceMemIntrinOpt
  bool getForceMemIntrinOpt() const { return ForceMemIntrinOpt; }
  /// Set ClFlags::ForceMemIntrinOpt to a new value
  void setForceMemIntrinOpt(bool NewValue) { ForceMemIntrinOpt = NewValue; }

  /// Get the value of ClFlags::FunctionSections
  bool getFunctionSections() const { return FunctionSections; }
  /// Set ClFlags::FunctionSections to a new value
  void setFunctionSections(bool NewValue) { FunctionSections = NewValue; }

  /// \brief Get the value of ClFlags::GenerateUnitTestMessages
  ///
  /// Note: If dump routines have been turned off, the error messages
  /// will not be readable. Hence, turn off.
  bool getGenerateUnitTestMessages() const {
    return !BuildDefs::dump() || GenerateUnitTestMessages;
  }
  /// Set ClFlags::GenerateUnitTestMessages to a new value
  void setGenerateUnitTestMessages(bool NewValue) {
    GenerateUnitTestMessages = NewValue;
  }

  /// Get the value of ClFlags::MockBoundsCheck
  bool getMockBoundsCheck() const { return MockBoundsCheck; }
  /// Set ClFlags::MockBoundsCheck to a new value
  void setMockBoundsCheck(bool NewValue) { MockBoundsCheck = NewValue; }

  /// Get the value of ClFlags::PhiEdgeSplit
  bool getPhiEdgeSplit() const { return PhiEdgeSplit; }
  /// Set ClFlags::PhiEdgeSplit to a new value
  void setPhiEdgeSplit(bool NewValue) { PhiEdgeSplit = NewValue; }

  /// Get the value of ClFlags::RandomNopInsertion
  bool shouldDoNopInsertion() const { return RandomNopInsertion; }
  /// Set ClFlags::RandomNopInsertion to a new value
  void setShouldDoNopInsertion(bool NewValue) { RandomNopInsertion = NewValue; }

  /// Get the value of ClFlags::RandomRegAlloc
  bool shouldRandomizeRegAlloc() const { return RandomRegAlloc; }
  /// Set ClFlags::RandomRegAlloc to a new value
  void setShouldRandomizeRegAlloc(bool NewValue) { RandomRegAlloc = NewValue; }

  /// Get the value of ClFlags::RepeatRegAlloc
  bool shouldRepeatRegAlloc() const { return RepeatRegAlloc; }
  /// Set ClFlags::RepeatRegAlloc to a new value
  void setShouldRepeatRegAlloc(bool NewValue) { RepeatRegAlloc = NewValue; }

  /// Get the value of ClFlags::SkipUnimplemented
  bool getSkipUnimplemented() const { return SkipUnimplemented; }
  /// Set ClFlags::SkipUnimplemented to a new value
  void setSkipUnimplemented(bool NewValue) { SkipUnimplemented = NewValue; }

  /// Get the value of ClFlags::SubzeroTimingEnabled
  bool getSubzeroTimingEnabled() const { return SubzeroTimingEnabled; }
  /// Set ClFlags::SubzeroTimingEnableds to a new value
  void setSubzeroTimingEnabled(bool NewValue) {
    SubzeroTimingEnabled = NewValue;
  }

  /// Get the value of ClFlags::TimeEachFunction
  bool getTimeEachFunction() const {
    return BuildDefs::dump() && TimeEachFunction;
  }
  /// Set ClFlags::TimeEachFunction to a new value
  void setTimeEachFunction(bool NewValue) { TimeEachFunction = NewValue; }

  /// Get the value of ClFlags::UseSandboxing
  bool getUseSandboxing() const { return UseSandboxing; }
  /// Set ClFlags::UseSandboxing to a new value
  void setUseSandboxing(bool NewValue) { UseSandboxing = NewValue; }

  // Enum and integer accessors.
  /// Get the value of ClFlags::Opt
  OptLevel getOptLevel() const { return Opt; }
  /// Set ClFlags::Opt to a new value
  void setOptLevel(OptLevel NewValue) { Opt = NewValue; }

  /// Get the value of ClFlags::OutFileType
  FileType getOutFileType() const { return OutFileType; }
  /// Set ClFlags::OutFileType to a new value
  void setOutFileType(FileType NewValue) { OutFileType = NewValue; }

  /// Get the value of ClFlags::RandomMaxNopsPerInstruction
  int getMaxNopsPerInstruction() const { return RandomMaxNopsPerInstruction; }
  /// Set ClFlags::RandomMaxNopsPerInstruction to a new value
  void setMaxNopsPerInstruction(int NewValue) {
    RandomMaxNopsPerInstruction = NewValue;
  }

  /// Get the value of ClFlags::RandomNopProbabilityAsPercentage
  int getNopProbabilityAsPercentage() const {
    return RandomNopProbabilityAsPercentage;
  }
  /// Set ClFlags::RandomNopProbabilityAsPercentage to a new value
  void setNopProbabilityAsPercentage(int NewValue) {
    RandomNopProbabilityAsPercentage = NewValue;
  }

  /// Get the value of ClFlags::TArch
  TargetArch getTargetArch() const { return TArch; }
  /// Set ClFlags::TArch to a new value
  void setTargetArch(TargetArch NewValue) { TArch = NewValue; }

  /// Get the value of ClFlags::TInstrSet
  TargetInstructionSet getTargetInstructionSet() const { return TInstrSet; }
  /// Set ClFlags::TInstrSet to a new value
  void setTargetInstructionSet(TargetInstructionSet NewValue) {
    TInstrSet = NewValue;
  }

  /// \brief Get the value of ClFlags::TestStackExtra
  ///
  /// Always 0 if BuildDefs::minimal()
  uint32_t getTestStackExtra() const {
    return BuildDefs::minimal() ? 0 : TestStackExtra;
  }
  /// \brief Set ClFlags::TestStackExtra to a new value
  ///
  /// Always 0 if BuildDefs::minimal()
  void setTestStackExtra(uint32_t NewValue) {
    if (BuildDefs::minimal())
      return;
    TestStackExtra = NewValue;
  }

  /// \brief Get the value of ClFlags::VMask
  ///
  /// None if BuildDefs::dump()
  VerboseMask getVerbose() const {
    return BuildDefs::dump() ? VMask : (VerboseMask)IceV_None;
  }
  /// \brief Set ClFlags::VMask to a new value
  ///
  /// None if BuildDefs::dump()
  void setVerbose(VerboseMask NewValue) { VMask = NewValue; }

  /// Set ClFlags::RandomizeAndPoolImmediatesOption to a new value
  void
  setRandomizeAndPoolImmediatesOption(RandomizeAndPoolImmediatesEnum Option) {
    RandomizeAndPoolImmediatesOption = Option;
  }
  /// Get the value of ClFlags::RandomizeAndPoolImmediatesOption
  RandomizeAndPoolImmediatesEnum getRandomizeAndPoolImmediatesOption() const {
    return RandomizeAndPoolImmediatesOption;
  }

  /// Set ClFlags::RandomizeAndPoolImmediatesThreshold to a new value
  void setRandomizeAndPoolImmediatesThreshold(uint32_t Threshold) {
    RandomizeAndPoolImmediatesThreshold = Threshold;
  }
  /// Get the value of ClFlags::RandomizeAndPoolImmediatesThreshold
  uint32_t getRandomizeAndPoolImmediatesThreshold() const {
    return RandomizeAndPoolImmediatesThreshold;
  }

  /// Get the value of ClFlags::ReorderBasicBlocks
  bool shouldReorderBasicBlocks() const { return ReorderBasicBlocks; }
  /// Set ClFlags::ReorderBasicBlocks to a new value
  void setShouldReorderBasicBlocks(bool NewValue) {
    ReorderBasicBlocks = NewValue;
  }

  /// Set ClFlags::ReorderFunctions to a new value
  void setShouldReorderFunctions(bool Option) { ReorderFunctions = Option; }
  /// Get the value of ClFlags::ReorderFunctions
  bool shouldReorderFunctions() const { return ReorderFunctions; }

  /// Set ClFlags::ReorderFunctionsWindowSize to a new value
  void setReorderFunctionsWindowSize(uint32_t Size) {
    ReorderFunctionsWindowSize = Size;
  }
  /// Get the value of ClFlags::ReorderFunctionsWindowSize
  uint32_t getReorderFunctionsWindowSize() const {
    return ReorderFunctionsWindowSize;
  }

  /// Set ClFlags::ReorderGlobalVariables to a new value
  void setShouldReorderGlobalVariables(bool Option) {
    ReorderGlobalVariables = Option;
  }
  /// Get the value of ClFlags::ReorderGlobalVariables
  bool shouldReorderGlobalVariables() const { return ReorderGlobalVariables; }

  /// Set ClFlags::ReorderPooledConstants to a new value
  void setShouldReorderPooledConstants(bool Option) {
    ReorderPooledConstants = Option;
  }
  /// Get the value of ClFlags::ReorderPooledConstants
  bool shouldReorderPooledConstants() const { return ReorderPooledConstants; }

  // IceString accessors.

  /// Get the value of ClFlags::DefaultFunctionPrefix
  const IceString &getDefaultFunctionPrefix() const {
    return DefaultFunctionPrefix;
  }
  /// Set ClFlags::DefaultFunctionPrefix to a new value
  void setDefaultFunctionPrefix(const IceString &NewValue) {
    DefaultFunctionPrefix = NewValue;
  }

  /// Get the value of ClFlags::DefaultGlobalPrefix
  const IceString &getDefaultGlobalPrefix() const {
    return DefaultGlobalPrefix;
  }
  /// Set ClFlags::DefaultGlobalPrefix to a new value
  void setDefaultGlobalPrefix(const IceString &NewValue) {
    DefaultGlobalPrefix = NewValue;
  }

  /// Get the value of ClFlags::TestPrefix
  const IceString &getTestPrefix() const { return TestPrefix; }
  /// Set ClFlags::TestPrefix to a new value
  void setTestPrefix(const IceString &NewValue) { TestPrefix = NewValue; }

  /// Get the value of ClFlags::TimingFocusOn
  const IceString &getTimingFocusOn() const { return TimingFocusOn; }
  /// Set ClFlags::TimingFocusOn to a new value
  void setTimingFocusOn(const IceString &NewValue) { TimingFocusOn = NewValue; }

  /// Get the value of ClFlags::TranslateOnly
  const IceString &getTranslateOnly() const { return TranslateOnly; }
  /// Set ClFlags::TranslateOnly to a new value
  void setTranslateOnly(const IceString &NewValue) { TranslateOnly = NewValue; }

  /// Get the value of ClFlags::VerboseFocusOn
  const IceString &getVerboseFocusOn() const { return VerboseFocusOn; }
  /// Set ClFlags::VerboseFocusOns to a new value
  void setVerboseFocusOn(const IceString &NewValue) {
    VerboseFocusOn = NewValue;
  }

  // size_t and 64-bit accessors.

  /// Get the value of ClFlags::NumTranslationThreads
  size_t getNumTranslationThreads() const { return NumTranslationThreads; }
  bool isSequential() const { return NumTranslationThreads == 0; }
  /// Set ClFlags::NumTranslationThreads to a new value
  void setNumTranslationThreads(size_t NewValue) {
    NumTranslationThreads = NewValue;
  }

  /// Get the value of ClFlags::RandomSeed
  uint64_t getRandomSeed() const { return RandomSeed; }
  /// Set ClFlags::RandomSeed to a new value
  void setRandomSeed(size_t NewValue) { RandomSeed = NewValue; }

private:
  /// see anonymous_namespace{IceClFlags.cpp}::AllowErrorRecovery
  bool AllowErrorRecovery;
  /// see anonymous_namespace{IceClFlags.cpp}::AllowExternDefinedSymbols
  bool AllowExternDefinedSymbols;
  /// see anonymous_namespace{IceClFlags.cpp}::AllowIacaMarks
  bool AllowIacaMarks;
  /// see anonymous_namespace{IceClFlags.cpp}::AllowUninitializedGlobals
  bool AllowUninitializedGlobals;
  /// see anonymous_namespace{IceClFlags.cpp}::DataSections
  bool DataSections;
  /// see anonymous_namespace{IceClFlags.cpp}::DecorateAsm
  bool DecorateAsm;
  /// see anonymous_namespace{IceClFlags.cpp}::DisableHybridAssembly
  bool DisableHybridAssembly;
  /// see anonymous_namespace{IceClFlags.cpp}::DisableInternal
  bool DisableInternal;
  /// see anonymous_namespace{IceClFlags.cpp}::DisableTranslation
  bool DisableTranslation;
  /// see anonymous_namespace{IceClFlags.cpp}::DumpStats
  bool DumpStats;
  /// see anonymous_namespace{IceClFlags.cpp}::EnableBlockProfile
  bool EnableBlockProfile;
  /// see anonymous_namespace{IceClFlags.cpp}::ForceMemIntrinOpt
  bool ForceMemIntrinOpt;
  /// see anonymous_namespace{IceClFlags.cpp}::FunctionSections
  bool FunctionSections;
  /// Initialized to false; not set by the command line.
  bool GenerateUnitTestMessages;
  /// see anonymous_namespace{IceClFlags.cpp}::MockBoundsCheck
  bool MockBoundsCheck;
  /// see anonymous_namespace{IceClFlags.cpp}::EnablePhiEdgeSplit
  bool PhiEdgeSplit;
  /// see anonymous_namespace{IceClFlags.cpp}::ShouldDoNopInsertion
  bool RandomNopInsertion;
  /// see anonymous_namespace{IceClFlags.cpp}::RandomizeRegisterAllocation
  bool RandomRegAlloc;
  /// see anonymous_namespace{IceClFlags.cpp}::RepeatRegAlloc
  bool RepeatRegAlloc;
  /// see anonymous_namespace{IceClFlags.cpp}::ReorderBasicBlocks
  bool ReorderBasicBlocks;
  /// see anonymous_namespace{IceClFlags.cpp}::ReorderFunctions
  bool ReorderFunctions;
  /// see anonymous_namespace{IceClFlags.cpp}::ReorderGlobalVariables
  bool ReorderGlobalVariables;
  /// see anonymous_namespace{IceClFlags.cpp}::ReorderPooledConstants
  bool ReorderPooledConstants;
  /// see anonymous_namespace{IceClFlags.cpp}::SkipUnimplemented
  bool SkipUnimplemented;
  /// see anonymous_namespace{IceClFlags.cpp}::SubzeroTimingEnabled
  bool SubzeroTimingEnabled;
  /// see anonymous_namespace{IceClFlags.cpp}::TimeEachFunction
  bool TimeEachFunction;
  /// see anonymous_namespace{IceClFlags.cpp}::UseSandboxing
  bool UseSandboxing;
  /// see anonymous_namespace{IceClFlags.cpp}::OLevel
  OptLevel Opt;
  /// see anonymous_namespace{IceClFlags.cpp}::OutFileType
  FileType OutFileType;
  /// see anonymous_namespace{IceClFlags.cpp}::RandomizeAndPoolImmediatesOption
  RandomizeAndPoolImmediatesEnum RandomizeAndPoolImmediatesOption;
  /// see
  /// anonymous_namespace{IceClFlags.cpp}::RandomizeAndPoolImmediatesThreshold
  uint32_t RandomizeAndPoolImmediatesThreshold;
  /// see anonymous_namespace{IceClFlags.cpp}::MaxNopsPerInstruction
  int RandomMaxNopsPerInstruction;
  /// see anonymous_namespace{IceClFlags.cpp}::NopProbabilityAsPercentage
  int RandomNopProbabilityAsPercentage;
  /// see anonymous_namespace{IceClFlags.cpp}::ReorderFunctionsWindowSize
  uint32_t ReorderFunctionsWindowSize;
  /// see anonymous_namespace{IceClFlags.cpp}::TargetArch
  TargetArch TArch;
  /// see anonymous_namespace{IceClFlags.cpp}::TestStackExtra
  uint32_t TestStackExtra;
  /// see anonymous_namespace{IceClFlags.cpp}::TargetInstructionSet
  TargetInstructionSet TInstrSet;
  /// see anonymous_namespace{IceClFlags.cpp}::VerboseList
  VerboseMask VMask;
  /// see anonymous_namespace{IceClFlags.cpp}::DefaultFunctionPrefix
  IceString DefaultFunctionPrefix;
  /// see anonymous_namespace{IceClFlags.cpp}::DefaultGlobalPrefix
  IceString DefaultGlobalPrefix;
  /// see anonymous_namespace{IceClFlags.cpp}::TestPrefix
  IceString TestPrefix;
  /// see anonymous_namespace{IceClFlags.cpp}::TimingFocusOn
  IceString TimingFocusOn;
  /// see anonymous_namespace{IceClFlags.cpp}::TranslateOnly
  IceString TranslateOnly;
  /// see anonymous_namespace{IceClFlags.cpp}::VerboseFocusOn
  IceString VerboseFocusOn;
  /// see anonymous_namespace{IceClFlags.cpp}::NumThreads

  size_t NumTranslationThreads; // 0 means completely sequential
  /// see anonymous_namespace{IceClFlags.cpp}::RandomSeed
  uint64_t RandomSeed;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGS_H
