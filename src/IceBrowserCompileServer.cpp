//===- subzero/src/IceBrowserCompileServer.cpp - Browser compile server ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the browser-based compile server.
//
//===----------------------------------------------------------------------===//

// Can only compile this with the NaCl compiler (needs irt.h, and the
// unsandboxed LLVM build using the trusted compiler does not have irt.h).
#if PNACL_BROWSER_TRANSLATOR

#include <cstring>
#include <irt.h>
#include <irt_dev.h>
#include <thread>

#include "llvm/Support/QueueStreamer.h"

#include "IceBrowserCompileServer.h"

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
  // TODO(jvoung): Also return an error string, and UMA data.
  // Set up a report_fatal_error handler to grab that string.
  if (gCompileServer->getErrorCode().value()) {
    return strdup("Some error occurred");
  }
  return nullptr;
}

struct nacl_irt_pnacl_compile_funcs SubzeroCallbacks {
  &onInitCallback, &onDataCallback, &onEndCallback
};

std::unique_ptr<llvm::raw_fd_ostream> getOutputStream(int FD) {
  if (FD <= 0)
    llvm::report_fatal_error("Invalid output FD");
  const bool CloseOnDtor = true;
  const bool Unbuffered = false;
  return std::unique_ptr<llvm::raw_fd_ostream>(
      new llvm::raw_fd_ostream(FD, CloseOnDtor, Unbuffered));
}

} // end of anonymous namespace

BrowserCompileServer::~BrowserCompileServer() {}

void BrowserCompileServer::run() {
  gCompileServer = this;
  // TODO(jvoung): make a useful fatal error handler that can
  // exit all *worker* threads, keep the server thread alive,
  // and be able to handle a server request for the last error string.
  getIRTInterfaces();
  gIRTFuncs.serve_translate_request(&SubzeroCallbacks);
}

void BrowserCompileServer::getParsedFlags(uint32_t NumThreads, int argc,
                                          char **argv) {
  ClFlags::parseFlags(argc, argv);
  ClFlags::getParsedClFlags(Flags);
  ClFlags::getParsedClFlagsExtra(ExtraFlags);
  // Set some defaults which aren't specified via the argv string.
  Flags.setNumTranslationThreads(NumThreads);
  Flags.setUseSandboxing(true);
  Flags.setOutFileType(FT_Elf);
  // TODO(jvoung): allow setting different target arches.
  Flags.setTargetArch(Target_X8632);
  ExtraFlags.setBuildOnRead(true);
  ExtraFlags.setInputFileFormat(llvm::PNaClFormat);
}

bool BrowserCompileServer::pushInputBytes(const void *Data, size_t NumBytes) {
  return InputStream->PutBytes(
             const_cast<unsigned char *>(
                 reinterpret_cast<const unsigned char *>(Data)),
             NumBytes) != NumBytes;
}

void BrowserCompileServer::endInputStream() { InputStream->SetDone(); }

void BrowserCompileServer::startCompileThread(int ObjFD) {
  InputStream = new llvm::QueueStreamer();
  LogStream = getOutputStream(STDOUT_FILENO);
  LogStream->SetUnbuffered();
  EmitStream = getOutputStream(ObjFD);
  EmitStream->SetBufferSize(1 << 14);
  ELFStream.reset(new ELFStreamer(*EmitStream.get()));
  Ctx.reset(new GlobalContext(LogStream.get(), EmitStream.get(),
                              ELFStream.get(), Flags));
  CompileThread = std::thread([this]() {
    Ctx->initParserThread();
    this->getCompiler().run(ExtraFlags, *Ctx.get(),
                            // Retain original reference, but the compiler
                            // (LLVM's MemoryObject) wants to handle deletion.
                            std::unique_ptr<llvm::DataStreamer>(InputStream));
  });
}

} // end of namespace Ice

#endif // PNACL_BROWSER_TRANSLATOR
