//===- subzero/src/main.cpp - Entry point for bitcode translation ---------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the entry point for translating PNaCl bitcode into native
/// code.
///
//===----------------------------------------------------------------------===//

#include "IceBrowserCompileServer.h"
#include "IceCompiler.h"
#include "IceCompileServer.h"

int main(int argc, char **argv) {
  // Start file server and "wait" for compile request.
  Ice::Compiler Comp;
// Can only compile the BrowserCompileServer w/ the NaCl compiler.
#if PNACL_BROWSER_TRANSLATOR
  // There are no real commandline arguments in the browser case. They are
  // supplied via IPC.
  assert(argc == 1);
  (void)argc;
  (void)argv;
  Ice::BrowserCompileServer Server(Comp);
  Server.run();
  return Server.getErrorCode().value();
#else  // !PNACL_BROWSER_TRANSLATOR
  Ice::CLCompileServer Server(Comp, argc, argv);
  Server.run();
  return Server.getErrorCode().value();
#endif // !PNACL_BROWSER_TRANSLATOR
}
