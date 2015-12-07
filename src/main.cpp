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
#include "IceBuildDefs.h"
#include "IceCompileServer.h"

int main(int argc, char **argv) {
  // Start file server and "wait" for compile request.
  // Can only compile the BrowserCompileServer w/ the NaCl compiler.
  if (Ice::BuildDefs::browser()) {
    // There are no real commandline arguments in the browser case. They are
    // supplied via IPC.
    assert(argc == 1);
    return Ice::BrowserCompileServer().runAndReturnErrorCode();
  }
  return Ice::CLCompileServer(argc, argv).runAndReturnErrorCode();
}
