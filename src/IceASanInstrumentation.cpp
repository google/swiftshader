//===- subzero/src/IceASanInstrumentation.cpp - ASan ------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the AddressSanitizer instrumentation class.
///
//===----------------------------------------------------------------------===//

#include "IceASanInstrumentation.h"

#include "IceBuildDefs.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceGlobalInits.h"
#include "IceInst.h"
#include "IceTargetLowering.h"
#include "IceTypes.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Ice {

namespace {

constexpr const char *ASanPrefix = "__asan";
constexpr SizeT RzSize = 32;
constexpr const char *RzPrefix = "__$rz";
constexpr const char *RzArrayName = "__$rz_array";
constexpr const char *RzSizesName = "__$rz_sizes";
const llvm::NaClBitcodeRecord::RecordVector RzContents =
    llvm::NaClBitcodeRecord::RecordVector(RzSize, 'R');

// In order to instrument the code correctly, the .pexe must not have had its
// symbols stripped.
using string_map = std::unordered_map<std::string, std::string>;
using string_set = std::unordered_set<std::string>;
// TODO(tlively): Handle all allocation functions
const string_map FuncSubstitutions = {{"malloc", "__asan_malloc"},
                                      {"free", "__asan_free"},
                                      {"calloc", "__asan_calloc"},
                                      {"__asan_dummy_calloc", "__asan_calloc"},
                                      {"realloc", "__asan_realloc"}};
const string_set FuncBlackList = {"_Balloc"};

llvm::NaClBitcodeRecord::RecordVector sizeToByteVec(SizeT Size) {
  llvm::NaClBitcodeRecord::RecordVector SizeContents;
  for (unsigned i = 0; i < sizeof(Size); ++i) {
    SizeContents.emplace_back(Size % (1 << CHAR_BIT));
    Size >>= CHAR_BIT;
  }
  return SizeContents;
}

} // end of anonymous namespace

ICE_TLS_DEFINE_FIELD(std::vector<InstCall *> *, ASanInstrumentation,
                     LocalDtors);

bool ASanInstrumentation::isInstrumentable(Cfg *Func) {
  std::string FuncName = Func->getFunctionName().toStringOrEmpty();
  return FuncName == "" ||
         (FuncBlackList.count(FuncName) == 0 && FuncName.find(ASanPrefix) != 0);
}

// Create redzones around all global variables, ensuring that the initializer
// types of the redzones and their associated globals match so that they are
// laid out together in memory.
void ASanInstrumentation::instrumentGlobals(VariableDeclarationList &Globals) {
  std::unique_lock<std::mutex> _(GlobalsMutex);
  if (DidProcessGlobals)
    return;
  VariableDeclarationList NewGlobals;
  // Global holding pointers to all redzones
  auto *RzArray = VariableDeclaration::create(&NewGlobals);
  // Global holding sizes of all redzones
  auto *RzSizes = VariableDeclaration::create(&NewGlobals);

  RzArray->setName(Ctx, RzArrayName);
  RzSizes->setName(Ctx, RzSizesName);
  RzArray->setIsConstant(true);
  RzSizes->setIsConstant(true);
  NewGlobals.push_back(RzArray);
  NewGlobals.push_back(RzSizes);

  for (VariableDeclaration *Global : Globals) {
    assert(Global->getAlignment() <= RzSize);
    VariableDeclaration *RzLeft = VariableDeclaration::create(&NewGlobals);
    VariableDeclaration *RzRight = VariableDeclaration::create(&NewGlobals);
    RzLeft->setName(Ctx, nextRzName());
    RzRight->setName(Ctx, nextRzName());
    SizeT Alignment = std::max(RzSize, Global->getAlignment());
    SizeT RzLeftSize = Alignment;
    SizeT RzRightSize =
        RzSize + Utils::OffsetToAlignment(Global->getNumBytes(), Alignment);
    if (Global->hasNonzeroInitializer()) {
      RzLeft->addInitializer(VariableDeclaration::DataInitializer::create(
          &NewGlobals, llvm::NaClBitcodeRecord::RecordVector(RzLeftSize, 'R')));
      RzRight->addInitializer(VariableDeclaration::DataInitializer::create(
          &NewGlobals,
          llvm::NaClBitcodeRecord::RecordVector(RzRightSize, 'R')));
    } else {
      RzLeft->addInitializer(VariableDeclaration::ZeroInitializer::create(
          &NewGlobals, RzLeftSize));
      RzRight->addInitializer(VariableDeclaration::ZeroInitializer::create(
          &NewGlobals, RzRightSize));
    }
    RzLeft->setIsConstant(Global->getIsConstant());
    RzRight->setIsConstant(Global->getIsConstant());
    RzLeft->setAlignment(Alignment);
    Global->setAlignment(Alignment);
    RzRight->setAlignment(1);
    RzArray->addInitializer(VariableDeclaration::RelocInitializer::create(
        &NewGlobals, RzLeft, RelocOffsetArray(0)));
    RzArray->addInitializer(VariableDeclaration::RelocInitializer::create(
        &NewGlobals, RzRight, RelocOffsetArray(0)));
    RzSizes->addInitializer(VariableDeclaration::DataInitializer::create(
        &NewGlobals, sizeToByteVec(RzLeftSize)));
    RzSizes->addInitializer(VariableDeclaration::DataInitializer::create(
        &NewGlobals, sizeToByteVec(RzRightSize)));

    NewGlobals.push_back(RzLeft);
    NewGlobals.push_back(Global);
    NewGlobals.push_back(RzRight);
    RzGlobalsNum += 2;
  }

  // Replace old list of globals, without messing up arena allocators
  Globals.clear();
  Globals.merge(&NewGlobals);
  DidProcessGlobals = true;

  // Log the new set of globals
  if (BuildDefs::dump() && (getFlags().getVerbose() & IceV_GlobalInit)) {
    OstreamLocker _(Ctx);
    Ctx->getStrDump() << "========= Instrumented Globals =========\n";
    for (VariableDeclaration *Global : Globals) {
      Global->dump(Ctx->getStrDump());
    }
  }
}

std::string ASanInstrumentation::nextRzName() {
  std::stringstream Name;
  Name << RzPrefix << RzNum++;
  return Name.str();
}

// Check for an alloca signaling the presence of local variables and add a
// redzone if it is found
void ASanInstrumentation::instrumentFuncStart(LoweringContext &Context) {
  if (ICE_TLS_GET_FIELD(LocalDtors) == nullptr)
    ICE_TLS_SET_FIELD(LocalDtors, new std::vector<InstCall *>());
  Cfg *Func = Context.getNode()->getCfg();
  bool HasLocals = false;
  LoweringContext C;
  C.init(Context.getNode());
  std::vector<Inst *> Initializations;
  Constant *InitFunc =
      Ctx->getConstantExternSym(Ctx->getGlobalString("__asan_poison"));
  Constant *DestroyFunc =
      Ctx->getConstantExternSym(Ctx->getGlobalString("__asan_unpoison"));

  InstAlloca *Cur;
  ConstantInteger32 *VarSizeOp;
  while (
      (Cur = llvm::dyn_cast<InstAlloca>(iteratorToInst(C.getCur()))) &&
      (VarSizeOp = llvm::dyn_cast<ConstantInteger32>(Cur->getSizeInBytes()))) {
    HasLocals = true;

    // create the new alloca that includes a redzone
    SizeT VarSize = VarSizeOp->getValue();
    Variable *Dest = Cur->getDest();
    SizeT RzPadding = RzSize + Utils::OffsetToAlignment(VarSize, RzSize);
    auto *ByteCount =
        ConstantInteger32::create(Ctx, IceType_i32, VarSize + RzPadding);
    constexpr SizeT Alignment = 8;
    auto *NewVar = InstAlloca::create(Func, Dest, ByteCount, Alignment);

    // calculate the redzone offset
    Variable *RzLocVar = Func->makeVariable(IceType_i32);
    RzLocVar->setName(Func, nextRzName());
    auto *Offset = ConstantInteger32::create(Ctx, IceType_i32, VarSize);
    auto *RzLoc = InstArithmetic::create(Func, InstArithmetic::Add, RzLocVar,
                                         Dest, Offset);

    // instructions to poison and unpoison the redzone
    constexpr SizeT NumArgs = 2;
    constexpr Variable *Void = nullptr;
    constexpr bool NoTailcall = false;
    auto *Init = InstCall::create(Func, NumArgs, Void, InitFunc, NoTailcall);
    auto *Destroy =
        InstCall::create(Func, NumArgs, Void, DestroyFunc, NoTailcall);
    Init->addArg(RzLocVar);
    Destroy->addArg(RzLocVar);
    auto *RzSizeConst = ConstantInteger32::create(Ctx, IceType_i32, RzPadding);
    Init->addArg(RzSizeConst);
    Destroy->addArg(RzSizeConst);

    Cur->setDeleted();
    C.insert(NewVar);
    ICE_TLS_GET_FIELD(LocalDtors)->emplace_back(Destroy);
    Initializations.emplace_back(RzLoc);
    Initializations.emplace_back(Init);

    C.advanceCur();
    C.advanceNext();
  }

  C.setInsertPoint(C.getCur());

  // add the leftmost redzone
  if (HasLocals) {
    Variable *LastRz = Func->makeVariable(IceType_i32);
    LastRz->setName(Func, nextRzName());
    auto *ByteCount = ConstantInteger32::create(Ctx, IceType_i32, RzSize);
    constexpr SizeT Alignment = 8;
    auto *RzAlloca = InstAlloca::create(Func, LastRz, ByteCount, Alignment);

    constexpr SizeT NumArgs = 2;
    constexpr Variable *Void = nullptr;
    constexpr bool NoTailcall = false;
    auto *Init = InstCall::create(Func, NumArgs, Void, InitFunc, NoTailcall);
    auto *Destroy =
        InstCall::create(Func, NumArgs, Void, DestroyFunc, NoTailcall);
    Init->addArg(LastRz);
    Destroy->addArg(LastRz);
    Init->addArg(RzAlloca->getSizeInBytes());
    Destroy->addArg(RzAlloca->getSizeInBytes());

    ICE_TLS_GET_FIELD(LocalDtors)->emplace_back(Destroy);
    C.insert(RzAlloca);
    C.insert(Init);
  }

  // insert initializers for the redzones
  for (Inst *Init : Initializations) {
    C.insert(Init);
  }
}

void ASanInstrumentation::instrumentCall(LoweringContext &Context,
                                         InstCall *Instr) {
  auto *CallTarget =
      llvm::dyn_cast<ConstantRelocatable>(Instr->getCallTarget());
  if (CallTarget == nullptr)
    return;

  std::string TargetName = CallTarget->getName().toStringOrEmpty();
  auto Subst = FuncSubstitutions.find(TargetName);
  if (Subst == FuncSubstitutions.end())
    return;

  std::string SubName = Subst->second;
  Constant *NewFunc = Ctx->getConstantExternSym(Ctx->getGlobalString(SubName));
  auto *NewCall =
      InstCall::create(Context.getNode()->getCfg(), Instr->getNumArgs(),
                       Instr->getDest(), NewFunc, Instr->isTailcall());
  for (SizeT I = 0, Args = Instr->getNumArgs(); I < Args; ++I)
    NewCall->addArg(Instr->getArg(I));
  Context.insert(NewCall);
  Instr->setDeleted();
}

void ASanInstrumentation::instrumentLoad(LoweringContext &Context,
                                         InstLoad *Instr) {
  Constant *Func =
      Ctx->getConstantExternSym(Ctx->getGlobalString("__asan_check_load"));
  instrumentAccess(Context, Instr->getSourceAddress(),
                   typeWidthInBytes(Instr->getDest()->getType()), Func);
}

void ASanInstrumentation::instrumentStore(LoweringContext &Context,
                                          InstStore *Instr) {
  Constant *Func =
      Ctx->getConstantExternSym(Ctx->getGlobalString("__asan_check_store"));
  instrumentAccess(Context, Instr->getAddr(),
                   typeWidthInBytes(Instr->getData()->getType()), Func);
}

// TODO(tlively): Take size of access into account as well
void ASanInstrumentation::instrumentAccess(LoweringContext &Context,
                                           Operand *Op, SizeT Size,
                                           Constant *CheckFunc) {
  constexpr SizeT NumArgs = 2;
  constexpr Variable *Void = nullptr;
  constexpr bool NoTailCall = false;
  auto *Call = InstCall::create(Context.getNode()->getCfg(), NumArgs, Void,
                                CheckFunc, NoTailCall);
  Call->addArg(Op);
  Call->addArg(ConstantInteger32::create(Ctx, IceType_i32, Size));
  // play games to insert the call before the access instruction
  InstList::iterator Next = Context.getNext();
  Context.setInsertPoint(Context.getCur());
  Context.insert(Call);
  Context.setNext(Next);
}

void ASanInstrumentation::instrumentRet(LoweringContext &Context, InstRet *) {
  Cfg *Func = Context.getNode()->getCfg();
  InstList::iterator Next = Context.getNext();
  Context.setInsertPoint(Context.getCur());
  for (InstCall *RzUnpoison : *ICE_TLS_GET_FIELD(LocalDtors)) {
    SizeT NumArgs = RzUnpoison->getNumArgs();
    Variable *Dest = RzUnpoison->getDest();
    Operand *CallTarget = RzUnpoison->getCallTarget();
    bool HasTailCall = RzUnpoison->isTailcall();
    bool IsTargetHelperCall = RzUnpoison->isTargetHelperCall();
    auto *RzUnpoisonCpy = InstCall::create(Func, NumArgs, Dest, CallTarget,
                                           HasTailCall, IsTargetHelperCall);
    for (int I = 0, Args = RzUnpoison->getNumArgs(); I < Args; ++I) {
      RzUnpoisonCpy->addArg(RzUnpoison->getArg(I));
    }
    Context.insert(RzUnpoisonCpy);
  }
  Context.setNext(Next);
}

void ASanInstrumentation::instrumentStart(Cfg *Func) {
  Constant *ShadowMemInit =
      Ctx->getConstantExternSym(Ctx->getGlobalString("__asan_init"));
  constexpr SizeT NumArgs = 3;
  constexpr Variable *Void = nullptr;
  constexpr bool NoTailCall = false;
  auto *Call = InstCall::create(Func, NumArgs, Void, ShadowMemInit, NoTailCall);
  Func->getEntryNode()->getInsts().push_front(Call);

  instrumentGlobals(*getGlobals());

  Call->addArg(ConstantInteger32::create(Ctx, IceType_i32, RzGlobalsNum));
  Call->addArg(Ctx->getConstantSym(0, Ctx->getGlobalString(RzArrayName)));
  Call->addArg(Ctx->getConstantSym(0, Ctx->getGlobalString(RzSizesName)));
}

// TODO(tlively): make this more efficient with swap idiom
void ASanInstrumentation::finishFunc(Cfg *) {
  ICE_TLS_GET_FIELD(LocalDtors)->clear();
}

} // end of namespace Ice
