//===- subzero/src/IceClFlagsExtra.h - Extra Cl Flags -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines class Ice::ClFlagsExtra
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECLFLAGSEXTRA_H
#define SUBZERO_SRC_ICECLFLAGSEXTRA_H

#include "IceDefs.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wredundant-move"
#endif // __clang__

#include "llvm/IRReader/IRReader.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

namespace Ice {

///  Declares command line flags primarily used for non-minimal builds.
class ClFlagsExtra {
  ClFlagsExtra(const ClFlagsExtra &) = delete;
  ClFlagsExtra &operator=(const ClFlagsExtra &) = delete;

public:
  ClFlagsExtra() = default;

  /// Get the value of ClFlagsExtra::AlwaysExitSuccess
  bool getAlwaysExitSuccess() const { return AlwaysExitSuccess; }
  /// Set ClFlagsExtra::AlwaysExitSuccess to a new value
  void setAlwaysExitSuccess(bool NewValue) { AlwaysExitSuccess = NewValue; }

  /// Get the value of ClFlagsExtra::BuildOnRead
  bool getBuildOnRead() const { return BuildOnRead; }
  /// Set ClFlagsExtra::BuildOnRead to a new value
  void setBuildOnRead(bool NewValue) { BuildOnRead = NewValue; }

  /// Get the value of ClFlagsExtra::GenerateBuildAtts
  bool getGenerateBuildAtts() const { return GenerateBuildAtts; }
  /// Set ClFlagsExtra::GenerateBuildAtts to a new value
  void setGenerateBuildAtts(bool NewValue) { GenerateBuildAtts = NewValue; }

  /// Get the value of ClFlagsExtra::LLVMVerboseErrors
  bool getLLVMVerboseErrors() const { return LLVMVerboseErrors; }
  /// Set ClFlagsExtra::LLVMVerboseErrors to a new value
  void setLLVMVerboseErrors(bool NewValue) { LLVMVerboseErrors = NewValue; }

  /// Get the value of ClFlagsExtra::BitcodeAsText
  bool getBitcodeAsText() const { return BitcodeAsText; }
  /// Set ClFlagsExtra::BitcodeAsText to a new value
  void setBitcodeAsText(bool NewValue) { BitcodeAsText = NewValue; }

  /// Get the value of ClFlagsExtra::InputFileFormat
  llvm::NaClFileFormat getInputFileFormat() const { return InputFileFormat; }
  /// Set ClFlagsExtra::InputFileFormat to a new value
  void setInputFileFormat(llvm::NaClFileFormat NewValue) {
    InputFileFormat = NewValue;
  }

  /// Get the value of ClFlagsExtra::AppName
  const IceString &getAppName() const { return AppName; }
  /// Set ClFlagsExtra::AppName to a new value
  void setAppName(const IceString &NewValue) { AppName = NewValue; }

  /// Get the value of ClFlagsExtra::IRFilename
  const IceString &getIRFilename() const { return IRFilename; }
  /// Set ClFlagsExtra::IRFilename to a new value
  void setIRFilename(const IceString &NewValue) { IRFilename = NewValue; }

  /// Get the value of ClFlagsExtra::LogFilename
  const IceString &getLogFilename() const { return LogFilename; }
  /// Set ClFlagsExtra::LogFilename to a new value
  void setLogFilename(const IceString &NewValue) { LogFilename = NewValue; }

  /// Get the value of ClFlagsExtra::OutputFilename
  const IceString &getOutputFilename() const { return OutputFilename; }
  /// Set ClFlagsExtra::OutputFilename to a new value
  void setOutputFilename(const IceString &NewValue) {
    OutputFilename = NewValue;
  }

private:
  /// see anonymous_namespace{IceClFlags.cpp}::AlwaysExitSuccess
  bool AlwaysExitSuccess = false;
  /// see anonymous_namespace{IceClFlags.cpp}::BitcodeAsText
  bool BitcodeAsText = false;
  /// see anonymous_namespace{IceClFlags.cpp}::BuildOnRead
  bool BuildOnRead = false;
  /// see anonymous_namespace{IceClFlags.cpp}::GenerateBuildAtts
  bool GenerateBuildAtts = false;
  /// see anonymous_namespace{IceClFlags.cpp}::LLVMVerboseErrors
  bool LLVMVerboseErrors = false;

  /// see anonymous_namespace{IceClFlags.cpp}::InputFileFormat
  llvm::NaClFileFormat InputFileFormat = llvm::LLVMFormat;

  /// see anonymous_namespace{IceClFlags.cpp}::AppName
  IceString AppName = "";
  /// see anonymous_namespace{IceClFlags.cpp}::IRFilename
  IceString IRFilename = "";
  /// see anonymous_namespace{IceClFlags.cpp}::LogFilename
  IceString LogFilename = "";
  /// see anonymous_namespace{IceClFlags.cpp}::OutputFilename
  IceString OutputFilename = "";
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGSEXTRA_H
