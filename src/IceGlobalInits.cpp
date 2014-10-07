//===- subzero/src/IceGlobalInits.cpp - Global initializers ---------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the notion of global addresses and
// initializers in Subzero.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include "IceDefs.h"
#include "IceGlobalInits.h"
#include "IceTypes.h"

namespace {
char hexdigit(unsigned X) { return X < 10 ? '0' + X : 'A' + X - 10; }
}

namespace Ice {

GlobalAddress::~GlobalAddress() { llvm::DeleteContainerPointers(Initializers); }

void GlobalAddress::dumpType(Ostream &Stream) const {
  if (Initializers.size() == 1) {
    Initializers.front()->dumpType(Stream);
  } else {
    Stream << "<{ ";
    bool IsFirst = true;
    for (Initializer *Init : Initializers) {
      if (IsFirst) {
        IsFirst = false;
      } else {
        Stream << ", ";
      }
      Init->dumpType(Stream);
    }
    Stream << " }>";
  }
}

void GlobalAddress::dump(Ostream &Stream) const {
  Stream << "@" << getName() << " = internal "
         << (IsConstant ? "constant" : "global") << " ";

  // Add initializer.
  if (Initializers.size() == 1) {
    Initializers.front()->dump(Stream);
  } else {
    dumpType(Stream);
    Stream << " <{ ";
    bool IsFirst = true;
    for (Initializer *Init : Initializers) {
      if (IsFirst) {
        IsFirst = false;
      } else {
        Stream << ", ";
      }
      Init->dump(Stream);
    }
    Stream << " }>";
  }

  // Add alignment.
  if (Alignment > 0)
    Stream << ", align " << Alignment;
  Stream << "\n";
}

void GlobalAddress::Initializer::dumpType(Ostream &Stream) const {
  Stream << "[" << getNumBytes() << " x " << Ice::IceType_i8 << "]";
}

void GlobalAddress::DataInitializer::dump(Ostream &Stream) const {
  dumpType(Stream);
  Stream << " c\"";
  // Code taken from PrintEscapedString() in AsmWriter.cpp.  Keep
  // the strings in the same format as the .ll file for practical
  // diffing.
  for (uint8_t C : Contents) {
    if (isprint(C) && C != '\\' && C != '"')
      Stream << C;
    else
      Stream << '\\' << hexdigit(C >> 4) << hexdigit(C & 0x0F);
  }
  Stream << "\"";
}

void GlobalAddress::ZeroInitializer::dump(Ostream &Stream) const {
  dumpType(Stream);
  Stream << " zeroinitializer";
}

IceString GlobalAddress::RelocInitializer::getName() const {
  switch (Address.getKind()) {
  case FunctionRelocation:
    return Address.getFunction()->getName();
  case GlobalAddressRelocation:
    return Address.getGlobalAddr()->getName();
  default:
    llvm::report_fatal_error("Malformed relocation address!");
  }
}

void GlobalAddress::RelocInitializer::dumpType(Ostream &Stream) const {
  Stream << Ice::IceType_i32;
}

void GlobalAddress::RelocInitializer::dump(Ostream &Stream) const {
  if (Offset != 0) {
    dumpType(Stream);
    Stream << " add (";
  }
  dumpType(Stream);
  Stream << " ptrtoint (";
  if (Address.getKind() == FunctionRelocation) {
    Stream << *Address.getFunction()->getType() << " @"
           << Address.getFunction()->getName();
  } else {
    Address.getGlobalAddr()->dumpType(Stream);
    Stream << "* @" << Address.getGlobalAddr()->getName();
  }
  Stream << " to ";
  dumpType(Stream);
  Stream << ")";
  if (Offset != 0) {
    Stream << ", ";
    dumpType(Stream);
    Stream << " " << Offset << ")";
  }
}
}
