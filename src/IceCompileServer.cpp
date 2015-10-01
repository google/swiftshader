//===- subzero/src/IceCompileServer.cpp - Compile server ------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the basic commandline-based compile server.
///
//===----------------------------------------------------------------------===//

#include "IceCompileServer.h"

#include "IceClFlags.h"
#include "IceClFlagsExtra.h"
#include "IceELFStreamer.h"
#include "IceGlobalContext.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "llvm/Bitcode/NaCl/NaClBitcodeMungeUtils.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/StreamingMemoryObject.h"
#pragma clang diagnostic pop

#include <fstream>
#include <iostream>
#include <thread>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif
#if defined(_MSC_VER)
# include <io.h>
# include <fcntl.h>
#endif

namespace Ice {

namespace {

// Define a SmallVector backed buffer as a data stream, so that it can hold the
// generated binary version of the textual bitcode in the input file.
class TextDataStreamer : public llvm::DataStreamer {
public:
  TextDataStreamer() = default;
  ~TextDataStreamer() final = default;
  static TextDataStreamer *create(const IceString &Filename, std::string *Err);
  size_t GetBytes(unsigned char *Buf, size_t Len) final;

private:
  llvm::SmallVector<char, 1024> BitcodeBuffer;
  size_t Cursor = 0;
};

TextDataStreamer *TextDataStreamer::create(const IceString &Filename,
                                           std::string *Err) {
  TextDataStreamer *Streamer = new TextDataStreamer();
  llvm::raw_string_ostream ErrStrm(*Err);
  if (std::error_code EC = llvm::readNaClRecordTextAndBuildBitcode(
          Filename, Streamer->BitcodeBuffer, &ErrStrm)) {
    ErrStrm << EC.message();
    ErrStrm.flush();
    delete Streamer;
    return nullptr;
  }
  ErrStrm.flush();
  return Streamer;
}

size_t TextDataStreamer::GetBytes(unsigned char *Buf, size_t Len) {
  if (Cursor >= BitcodeBuffer.size())
    return 0;
  size_t Remaining = BitcodeBuffer.size();
  Len = std::min(Len, Remaining);
  for (size_t i = 0; i < Len; ++i)
    Buf[i] = BitcodeBuffer[Cursor + i];
  Cursor += Len;
  return Len;
}

std::unique_ptr<Ostream> makeStream(const IceString &Filename,
                                    std::error_code &EC) {
  if (Filename == "-") {
    return std::unique_ptr<Ostream>(new llvm::raw_os_ostream(std::cout));
  } else {
    return std::unique_ptr<Ostream>(
        new llvm::raw_fd_ostream(Filename, EC, llvm::sys::fs::F_None));
  }
}

ErrorCodes getReturnValue(const Ice::ClFlagsExtra &Flags, ErrorCodes Val) {
  if (Flags.getAlwaysExitSuccess())
    return EC_None;
  return Val;
}

// Reports fatal error message, and then exits with success status 0.
void reportFatalErrorThenExitSuccess(void *UserData, const std::string &Reason,
                                     bool GenCrashDag) {
  (void)UserData;
  (void)GenCrashDag;

  // Note: This code is (mostly) copied from llvm/lib/Support/ErrorHandling.cpp

  // Blast the result out to stderr.  We don't try hard to make sure this
  // succeeds (e.g. handling EINTR) and we can't use errs() here because
  // raw ostreams can call report_fatal_error.
  llvm::SmallVector<char, 64> Buffer;
  llvm::raw_svector_ostream OS(Buffer);
  OS << "LLVM ERROR: " << Reason << "\n";
  llvm::StringRef MessageStr = OS.str();
  ssize_t written = ::write(2, MessageStr.data(), MessageStr.size());
  (void)written; // If something went wrong, we deliberately just give up.

  // If we reached here, we are failing ungracefully. Run the interrupt handlers
  // to make sure any special cleanups get done, in particular that we remove
  // files registered with RemoveFileOnSignal.
  llvm::sys::RunInterruptHandlers();

  exit(0);
}

} // end of anonymous namespace

void CLCompileServer::run() {
  if (BuildDefs::dump()) {
    llvm::sys::PrintStackTraceOnErrorSignal();
  }
  ClFlags::parseFlags(argc, argv);
  ClFlags Flags;
  ClFlagsExtra ExtraFlags;
  ClFlags::getParsedClFlags(Flags);
  ClFlags::getParsedClFlagsExtra(ExtraFlags);

  // Override report_fatal_error if we want to exit with 0 status.
  if (ExtraFlags.getAlwaysExitSuccess())
    llvm::install_fatal_error_handler(reportFatalErrorThenExitSuccess, this);

  std::error_code EC;
  std::unique_ptr<Ostream> Ls = makeStream(ExtraFlags.getLogFilename(), EC);
  if (EC) {
    llvm::report_fatal_error("Unable to open log file");
  }
  Ls->SetUnbuffered();
  std::unique_ptr<Ostream> Os;
  std::unique_ptr<ELFStreamer> ELFStr;
  switch (Flags.getOutFileType()) {
  case FT_Elf: {
    if (ExtraFlags.getOutputFilename() == "-") {
      *Ls << "Error: writing binary ELF to stdout is unsupported\n";
      return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Args));
    }
    std::unique_ptr<llvm::raw_fd_ostream> FdOs(new llvm::raw_fd_ostream(
        ExtraFlags.getOutputFilename(), EC, llvm::sys::fs::F_None));
    if (EC) {
      *Ls << "Failed to open output file: " << ExtraFlags.getOutputFilename()
          << ":\n" << EC.message() << "\n";
      return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Args));
    }
    ELFStr.reset(new ELFStreamer(*FdOs.get()));
    Os.reset(FdOs.release());
    // NaCl sets st_blksize to 0, and LLVM uses that to pick the default
    // preferred buffer size. Set to something non-zero.
    Os->SetBufferSize(1 << 14);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    Os = makeStream(ExtraFlags.getOutputFilename(), EC);
    if (EC) {
      *Ls << "Failed to open output file: " << ExtraFlags.getOutputFilename()
          << ":\n" << EC.message() << "\n";
      return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Args));
    }
    Os->SetUnbuffered();
  } break;
  }

  if (BuildDefs::minimal() && ExtraFlags.getBitcodeAsText())
    llvm::report_fatal_error("Can't specify 'bitcode-as-text' flag in "
                             "minimal build");

  IceString StrError;
  std::unique_ptr<llvm::DataStreamer> InputStream(
      (!BuildDefs::minimal() && ExtraFlags.getBitcodeAsText())
          ? TextDataStreamer::create(ExtraFlags.getIRFilename(), &StrError)
          : llvm::getDataFileStreamer(ExtraFlags.getIRFilename(), &StrError));
  if (!StrError.empty() || !InputStream) {
    llvm::SMDiagnostic Err(ExtraFlags.getIRFilename(),
                           llvm::SourceMgr::DK_Error, StrError);
    Err.print(ExtraFlags.getAppName().c_str(), *Ls);
    return transferErrorCode(getReturnValue(ExtraFlags, Ice::EC_Bitcode));
  }

  Ctx.reset(
      new GlobalContext(Ls.get(), Os.get(), Ls.get(), ELFStr.get(), Flags));
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
