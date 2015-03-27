//===- subzero/src/IceCompileServer.cpp - Compile server ------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the basic commandline-based compile server.
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <iostream>
#include <thread>

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/StreamingMemoryObject.h"

#include "IceClFlags.h"
#include "IceClFlagsExtra.h"
#include "IceCompileServer.h"
#include "IceELFStreamer.h"
#include "IceGlobalContext.h"

namespace Ice {

namespace {

std::unique_ptr<Ostream> getStream(const IceString &Filename) {
  std::ofstream Ofs;
  if (Filename != "-") {
    Ofs.open(Filename.c_str(), std::ofstream::out);
    return std::unique_ptr<Ostream>(new llvm::raw_os_ostream(Ofs));
  } else {
    return std::unique_ptr<Ostream>(new llvm::raw_os_ostream(std::cout));
  }
}

ErrorCodes getReturnValue(const Ice::ClFlagsExtra &Flags, ErrorCodes Val) {
  if (Flags.getAlwaysExitSuccess())
    return EC_None;
  return Val;
}

} // end of anonymous namespace

void CLCompileServer::run() {
  ClFlags::parseFlags(argc, argv);
  ClFlags Flags;
  ClFlagsExtra ExtraFlags;
  ClFlags::getParsedClFlags(Flags);
  ClFlags::getParsedClFlagsExtra(ExtraFlags);

  std::unique_ptr<Ostream> Ls = getStream(ExtraFlags.getLogFilename());
  Ls->SetUnbuffered();
  std::unique_ptr<Ostream> Os;
  std::unique_ptr<ELFStreamer> ELFStr;
  switch (Flags.getOutFileType()) {
  case FT_Elf: {
    if (ExtraFlags.getOutputFilename() == "-") {
      *Ls << "Error: writing binary ELF to stdout is unsupported\n";
      return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Args));
    }
    std::error_code EC;
    std::unique_ptr<llvm::raw_fd_ostream> FdOs(new llvm::raw_fd_ostream(
        ExtraFlags.getOutputFilename(), EC, llvm::sys::fs::F_None));
    if (EC) {
      *Ls << "Failed to open output file: " << ExtraFlags.getOutputFilename()
          << ":\n" << EC.message() << "\n";
      return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Args));
    }
    ELFStr.reset(new ELFStreamer(*FdOs.get()));
    Os.reset(FdOs.release());
    // NaCl sets st_blksize to 0, and LLVM uses that to pick the
    // default preferred buffer size. Set to something non-zero.
    Os->SetBufferSize(1 << 14);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    Os = getStream(ExtraFlags.getOutputFilename());
    Os->SetUnbuffered();
  } break;
  }

  IceString StrError;
  std::unique_ptr<llvm::DataStreamer> InputStream(
      llvm::getDataFileStreamer(ExtraFlags.getIRFilename(), &StrError));
  if (!StrError.empty() || !InputStream) {
    llvm::SMDiagnostic Err(ExtraFlags.getIRFilename(),
                           llvm::SourceMgr::DK_Error, StrError);
    Err.print(ExtraFlags.getAppName().c_str(), *Ls);
    return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Bitcode));
  }

  Ctx.reset(new GlobalContext(Ls.get(), Os.get(), ELFStr.get(), Flags));
  if (Ctx->getFlags().getNumTranslationThreads() != 0) {
    std::thread CompileThread([this, &ExtraFlags, &InputStream]() {
      Ctx->initParserThread();
      getCompiler().run(ExtraFlags, *Ctx.get(), std::move(InputStream));
    });
    CompileThread.join();
  } else {
    getCompiler().run(ExtraFlags, *Ctx.get(), std::move(InputStream));
  }
  transferErrorCode(getReturnValue(
      ExtraFlags, static_cast<ErrorCodes>(Ctx->getErrorStatus()->value())));
}

} // end of namespace Ice
