//===- subzero/src/IceBrowserCompileServer.cpp - Browser compile server ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the browser-based compile server.
///
//===----------------------------------------------------------------------===//

// Can only compile this with the NaCl compiler (needs irt.h, and the
// unsandboxed LLVM build using the trusted compiler does not have irt.h).
#include "IceBrowserCompileServer.h"

#if PNACL_BROWSER_TRANSLATOR

// Headers which are not properly part of the SDK are included by their path in
// the NaCl tree.
#ifdef __pnacl__
#include "native_client/src/untrusted/nacl/pnacl.h"
#endif // __pnacl__

#include "llvm/Support/QueueStreamer.h"

#include <cstring>
#include <irt.h>
#include <irt_dev.h>
#include <pthread.h>
#include <thread>

namespace Ice {

// Create C wrappers around callback handlers for the IRT interface.
namespace {

BrowserCompileServer *gCompileServer;
struct nacl_irt_private_pnacl_translator_compile gIRTFuncs;

void getIRTInterfaces() {
  size_t QueryResult =
      nacl_interface_query(NACL_IRT_PRIVATE_PNACL_TRANSLATOR_COMPILE_v0_1,
                           &gIRTFuncs, sizeof(gIRTFuncs));
  if (QueryResult != sizeof(gIRTFuncs))
    llvm::report_fatal_error("Failed to get translator compile IRT interface");
}

char *onInitCallback(uint32_t NumThreads, int *ObjFileFDs,
                     size_t ObjFileFDCount, char **CLArgs, size_t CLArgsLen) {
  if (ObjFileFDCount < 1) {
    std::string Buffer;
    llvm::raw_string_ostream StrBuf(Buffer);
    StrBuf << "Invalid number of FDs for onInitCallback " << ObjFileFDCount
           << "\n";
    return strdup(StrBuf.str().c_str());
  }
  int ObjFileFD = ObjFileFDs[0];
  if (ObjFileFD < 0) {
    std::string Buffer;
    llvm::raw_string_ostream StrBuf(Buffer);
    StrBuf << "Invalid FD given for onInitCallback " << ObjFileFD << "\n";
    return strdup(StrBuf.str().c_str());
  }
  // CLArgs is almost an "argv", but is missing the argv[0] program name.
  std::vector<char *> Argv;
  char ProgramName[] = "pnacl-sz.nexe";
  Argv.reserve(CLArgsLen + 1);
  Argv.push_back(ProgramName);
  for (size_t i = 0; i < CLArgsLen; ++i) {
    Argv.push_back(CLArgs[i]);
  }
  // NOTE: strings pointed to by argv are owned by the caller, but we parse
  // here before returning and don't store them.
  gCompileServer->getParsedFlags(NumThreads, Argv.size(), Argv.data());
  gCompileServer->startCompileThread(ObjFileFD);
  return nullptr;
}

int onDataCallback(const void *Data, size_t NumBytes) {
  return gCompileServer->pushInputBytes(Data, NumBytes) ? 1 : 0;
}

char *onEndCallback() {
  gCompileServer->endInputStream();
  gCompileServer->waitForCompileThread();
  // TODO(jvoung): Also return UMA data.
  if (gCompileServer->getErrorCode().value()) {
    const std::string Error = gCompileServer->getErrorStream().getContents();
    return strdup(Error.empty() ? "Some error occurred" : Error.c_str());
  }
  return nullptr;
}

struct nacl_irt_pnacl_compile_funcs SubzeroCallbacks {
  &onInitCallback, &onDataCallback, &onEndCallback
};

std::unique_ptr<llvm::raw_fd_ostream> getOutputStream(int FD) {
  if (FD <= 0)
    llvm::report_fatal_error("Invalid output FD");
  constexpr bool CloseOnDtor = true;
  constexpr bool Unbuffered = false;
  return std::unique_ptr<llvm::raw_fd_ostream>(
      new llvm::raw_fd_ostream(FD, CloseOnDtor, Unbuffered));
}

void fatalErrorHandler(void *UserData, const std::string &Reason,
                       bool GenCrashDialog) {
  (void)GenCrashDialog;
  BrowserCompileServer *Server =
      reinterpret_cast<BrowserCompileServer *>(UserData);
  Server->setFatalError(Reason);
  // Only kill the current thread instead of the whole process. We need the
  // server thread to remain alive in order to respond with the error message.
  // We could also try to pthread_kill all other worker threads, but
  // pthread_kill / raising signals is not supported by NaCl. We'll have to
  // assume that the worker/emitter threads will be well behaved after a fatal
  // error in other threads, and either get stuck waiting on input from a
  // previous stage, or also call report_fatal_error.
  pthread_exit(0);
}

/// Adapted from pnacl-llc's AddDefaultCPU() in srpc_main.cpp.
TargetArch getTargetArch() {
#if defined(__pnacl__)
  switch (__builtin_nacl_target_arch()) {
  case PnaclTargetArchitectureX86_32:
  case PnaclTargetArchitectureX86_32_NonSFI:
    return Target_X8632;
  case PnaclTargetArchitectureX86_64:
    return Target_X8664;
  case PnaclTargetArchitectureARM_32:
  case PnaclTargetArchitectureARM_32_NonSFI:
    return Target_ARM32;
  case PnaclTargetArchitectureMips_32:
    return Target_MIPS32;
  default:
    llvm::report_fatal_error("no target architecture match.");
  }
#elif defined(__i386__)
  return Target_X8632;
#elif defined(__x86_64__)
  return Target_X8664;
#elif defined(__arm__)
  return Target_ARM32;
#else
// TODO(stichnot): Add mips.
#error "Unknown architecture"
#endif
}

} // end of anonymous namespace

BrowserCompileServer::~BrowserCompileServer() = default;

void BrowserCompileServer::run() {
  gCompileServer = this;
  getIRTInterfaces();
  gIRTFuncs.serve_translate_request(&SubzeroCallbacks);
}

void BrowserCompileServer::getParsedFlags(uint32_t NumThreads, int argc,
                                          char **argv) {
  ClFlags::parseFlags(argc, argv);
  ClFlags::getParsedClFlags(*Flags);
  ClFlags::getParsedClFlagsExtra(*ExtraFlags);
  // Set some defaults which aren't specified via the argv string.
  Flags->setNumTranslationThreads(NumThreads);
  Flags->setUseSandboxing(true);
  Flags->setOutFileType(FT_Elf);
  Flags->setTargetArch(getTargetArch());
  // Make the prefixes short to reduce the chance that string construction will
  // have to resort to malloc().  Note that this won't change anything if the
  // pexe was finalize with "--no-strip-syms".  TODO(stichnot): This will be
  // unnecessary when we stop directly constructing strings that are used for
  // names and lookup keys.
  Flags->setDefaultFunctionPrefix("F");
  Flags->setDefaultGlobalPrefix("G");
  ExtraFlags->setBuildOnRead(true);
  ExtraFlags->setInputFileFormat(llvm::PNaClFormat);
}

bool BrowserCompileServer::pushInputBytes(const void *Data, size_t NumBytes) {
  // If there was an earlier error, do not attempt to push bytes to the
  // QueueStreamer. Otherwise the thread could become blocked.
  if (HadError.load())
    return true;
  return InputStream->PutBytes(
             const_cast<unsigned char *>(
                 reinterpret_cast<const unsigned char *>(Data)),
             NumBytes) != NumBytes;
}

void BrowserCompileServer::setFatalError(const IceString &Reason) {
  HadError.store(true);
  Ctx->getStrError() << Reason;
  // Make sure that the QueueStreamer is not stuck by signaling an early end.
  InputStream->SetDone();
}

ErrorCode &BrowserCompileServer::getErrorCode() {
  if (HadError.load()) {
    // HadError means report_fatal_error is called. Make sure that the
    // LastError is not EC_None. We don't know the type of error so just pick
    // some error category.
    LastError.assign(EC_Translation);
  }
  return LastError;
}

void BrowserCompileServer::endInputStream() { InputStream->SetDone(); }

void BrowserCompileServer::startCompileThread(int ObjFD) {
  InputStream = new llvm::QueueStreamer();
  LogStream = getOutputStream(STDOUT_FILENO);
  LogStream->SetUnbuffered();
  EmitStream = getOutputStream(ObjFD);
  EmitStream->SetBufferSize(1 << 14);
  std::unique_ptr<StringStream> ErrStrm(new StringStream());
  ErrorStream = std::move(ErrStrm);
  ELFStream.reset(new ELFStreamer(*EmitStream.get()));
  Ctx.reset(new GlobalContext(LogStream.get(), EmitStream.get(),
                              &ErrorStream->getStream(), ELFStream.get()));
  CompileThread = std::thread([this]() {
    llvm::install_fatal_error_handler(fatalErrorHandler, this);
    Ctx->initParserThread();
    this->getCompiler().run(*ExtraFlags, *Ctx.get(),
                            // Retain original reference, but the compiler
                            // (LLVM's MemoryObject) wants to handle deletion.
                            std::unique_ptr<llvm::DataStreamer>(InputStream));
  });
}

} // end of namespace Ice

#else // !PNACL_BROWSER_TRANSLATOR

#include "llvm/Support/ErrorHandling.h"

namespace Ice {

BrowserCompileServer::~BrowserCompileServer() {}

void BrowserCompileServer::run() {
  llvm::report_fatal_error("no browser hookups");
}

ErrorCode &BrowserCompileServer::getErrorCode() {
  llvm::report_fatal_error("no browser hookups");
}

} // end of namespace Ice

#endif // PNACL_BROWSER_TRANSLATOR
