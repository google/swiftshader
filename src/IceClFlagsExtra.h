//===- subzero/src/IceClFlagsExtra.h - Extra Cl Flags -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares command line flags primarily used for non-minimal builds.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECLFLAGSEXTRA_H
#define SUBZERO_SRC_ICECLFLAGSEXTRA_H

#include "llvm/IRReader/IRReader.h"

#include "IceDefs.h"

namespace Ice {

class ClFlagsExtra {
  ClFlagsExtra(const ClFlagsExtra &) = delete;
  ClFlagsExtra &operator=(const ClFlagsExtra &) = delete;

public:
  ClFlagsExtra()
      : AlwaysExitSuccess(false), BuildOnRead(false), GenerateBuildAtts(false),
        LLVMVerboseErrors(false), InputFileFormat(llvm::LLVMFormat) {}

  bool getAlwaysExitSuccess() const { return AlwaysExitSuccess; }
  void setAlwaysExitSuccess(bool NewValue) { AlwaysExitSuccess = NewValue; }

  bool getBuildOnRead() const { return BuildOnRead; }
  void setBuildOnRead(bool NewValue) { BuildOnRead = NewValue; }

  bool getGenerateBuildAtts() const { return GenerateBuildAtts; }
  void setGenerateBuildAtts(bool NewValue) { GenerateBuildAtts = NewValue; }

  bool getLLVMVerboseErrors() const { return LLVMVerboseErrors; }
  void setLLVMVerboseErrors(bool NewValue) { LLVMVerboseErrors = NewValue; }

  llvm::NaClFileFormat getInputFileFormat() const { return InputFileFormat; }
  void setInputFileFormat(llvm::NaClFileFormat NewValue) {
    InputFileFormat = NewValue;
  }

  const IceString &getAppName() const { return AppName; }
  void setAppName(const IceString &NewValue) { AppName = NewValue; }

  const IceString &getIRFilename() const { return IRFilename; }
  void setIRFilename(const IceString &NewValue) { IRFilename = NewValue; }

  const IceString &getLogFilename() const { return LogFilename; }
  void setLogFilename(const IceString &NewValue) { LogFilename = NewValue; }

  const IceString &getOutputFilename() const { return OutputFilename; }
  void setOutputFilename(const IceString &NewValue) {
    OutputFilename = NewValue;
  }

private:
  bool AlwaysExitSuccess;
  bool BuildOnRead;
  bool GenerateBuildAtts;
  bool LLVMVerboseErrors;

  llvm::NaClFileFormat InputFileFormat;

  IceString AppName;
  IceString IRFilename;
  IceString LogFilename;
  IceString OutputFilename;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECLFLAGSEXTRA_H
