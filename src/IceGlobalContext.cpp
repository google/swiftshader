//===- subzero/src/IceGlobalContext.cpp - Global context defs -------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines aspects of the compilation that persist across
// multiple functions.
//
//===----------------------------------------------------------------------===//

#include <ctype.h> // isdigit(), isupper()
#include <locale>  // locale
#include <unordered_map>

#include "llvm/Support/Timer.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalContext.h"
#include "IceGlobalInits.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"
#include "IceTimerTree.h"
#include "IceTypes.h"

namespace std {
template <> struct hash<Ice::RelocatableTuple> {
  size_t operator()(const Ice::RelocatableTuple &Key) const {
    return hash<Ice::IceString>()(Key.Name) +
           hash<Ice::RelocOffsetT>()(Key.Offset);
  }
};
} // end of namespace std

namespace Ice {

namespace {

// Define the key comparison function for the constant pool's
// unordered_map, but only for key types of interest: integer types,
// floating point types, and the special RelocatableTuple.
template <typename KeyType, class Enable = void> struct KeyCompare {};

template <typename KeyType>
struct KeyCompare<KeyType,
                  typename std::enable_if<
                      std::is_integral<KeyType>::value ||
                      std::is_same<KeyType, RelocatableTuple>::value>::type> {
  bool operator()(const KeyType &Value1, const KeyType &Value2) const {
    return Value1 == Value2;
  }
};
template <typename KeyType>
struct KeyCompare<KeyType, typename std::enable_if<
                               std::is_floating_point<KeyType>::value>::type> {
  bool operator()(const KeyType &Value1, const KeyType &Value2) const {
    return !memcmp(&Value1, &Value2, sizeof(KeyType));
  }
};

// Define a key comparison function for sorting the constant pool's
// values after they are dumped to a vector.  This covers integer
// types, floating point types, and ConstantRelocatable values.
template <typename ValueType, class Enable = void> struct KeyCompareLess {};

template <typename ValueType>
struct KeyCompareLess<ValueType,
                      typename std::enable_if<std::is_floating_point<
                          typename ValueType::PrimType>::value>::type> {
  bool operator()(const Constant *Const1, const Constant *Const2) const {
    typedef uint64_t CompareType;
    static_assert(sizeof(typename ValueType::PrimType) <= sizeof(CompareType),
                  "Expected floating-point type of width 64-bit or less");
    typename ValueType::PrimType V1 = llvm::cast<ValueType>(Const1)->getValue();
    typename ValueType::PrimType V2 = llvm::cast<ValueType>(Const2)->getValue();
    // We avoid "V1<V2" because of NaN.
    // We avoid "memcmp(&V1,&V2,sizeof(V1))<0" which depends on the
    // endian-ness of the host system running Subzero.
    // Instead, compare the result of bit_cast to uint64_t.
    uint64_t I1 = 0, I2 = 0;
    memcpy(&I1, &V1, sizeof(V1));
    memcpy(&I2, &V2, sizeof(V2));
    return I1 < I2;
  }
};
template <typename ValueType>
struct KeyCompareLess<ValueType,
                      typename std::enable_if<std::is_integral<
                          typename ValueType::PrimType>::value>::type> {
  bool operator()(const Constant *Const1, const Constant *Const2) const {
    typename ValueType::PrimType V1 = llvm::cast<ValueType>(Const1)->getValue();
    typename ValueType::PrimType V2 = llvm::cast<ValueType>(Const2)->getValue();
    return V1 < V2;
  }
};
template <typename ValueType>
struct KeyCompareLess<
    ValueType, typename std::enable_if<
                   std::is_same<ValueType, ConstantRelocatable>::value>::type> {
  bool operator()(const Constant *Const1, const Constant *Const2) const {
    auto V1 = llvm::cast<ValueType>(Const1);
    auto V2 = llvm::cast<ValueType>(Const2);
    if (V1->getName() == V2->getName())
      return V1->getOffset() < V2->getOffset();
    return V1->getName() < V2->getName();
  }
};

// TypePool maps constants of type KeyType (e.g. float) to pointers to
// type ValueType (e.g. ConstantFloat).
template <Type Ty, typename KeyType, typename ValueType> class TypePool {
  TypePool(const TypePool &) = delete;
  TypePool &operator=(const TypePool &) = delete;

public:
  TypePool() = default;
  ValueType *getOrAdd(GlobalContext *Ctx, KeyType Key) {
    auto Iter = Pool.find(Key);
    if (Iter != Pool.end())
      return Iter->second;
    ValueType *Result = ValueType::create(Ctx, Ty, Key, NextPoolID++);
    Pool[Key] = Result;
    return Result;
  }
  ConstantList getConstantPool() const {
    ConstantList Constants;
    Constants.reserve(Pool.size());
    for (auto &I : Pool)
      Constants.push_back(I.second);
    // The sort (and its KeyCompareLess machinery) is not strictly
    // necessary, but is desirable for producing output that is
    // deterministic across unordered_map::iterator implementations.
    std::sort(Constants.begin(), Constants.end(), KeyCompareLess<ValueType>());
    return Constants;
  }

private:
  // Use the default hash function, and a custom key comparison
  // function.  The key comparison function for floating point
  // variables can't use the default == based implementation because
  // of special C++ semantics regarding +0.0, -0.0, and NaN
  // comparison.  However, it's OK to use the default hash for
  // floating point values because KeyCompare is the final source of
  // truth - in the worst case a "false" collision must be resolved.
  typedef std::unordered_map<KeyType, ValueType *, std::hash<KeyType>,
                             KeyCompare<KeyType>> ContainerType;
  ContainerType Pool;
  uint32_t NextPoolID = 0;
};

// UndefPool maps ICE types to the corresponding ConstantUndef values.
class UndefPool {
  UndefPool(const UndefPool &) = delete;
  UndefPool &operator=(const UndefPool &) = delete;

public:
  UndefPool() : Pool(IceType_NUM) {}

  ConstantUndef *getOrAdd(GlobalContext *Ctx, Type Ty) {
    if (Pool[Ty] == nullptr)
      Pool[Ty] = ConstantUndef::create(Ctx, Ty, NextPoolID++);
    return Pool[Ty];
  }

private:
  uint32_t NextPoolID = 0;
  std::vector<ConstantUndef *> Pool;
};

} // end of anonymous namespace

// The global constant pool bundles individual pools of each type of
// interest.
class ConstantPool {
  ConstantPool(const ConstantPool &) = delete;
  ConstantPool &operator=(const ConstantPool &) = delete;

public:
  ConstantPool() = default;
  TypePool<IceType_f32, float, ConstantFloat> Floats;
  TypePool<IceType_f64, double, ConstantDouble> Doubles;
  TypePool<IceType_i1, int8_t, ConstantInteger32> Integers1;
  TypePool<IceType_i8, int8_t, ConstantInteger32> Integers8;
  TypePool<IceType_i16, int16_t, ConstantInteger32> Integers16;
  TypePool<IceType_i32, int32_t, ConstantInteger32> Integers32;
  TypePool<IceType_i64, int64_t, ConstantInteger64> Integers64;
  TypePool<IceType_i32, RelocatableTuple, ConstantRelocatable> Relocatables;
  TypePool<IceType_i32, RelocatableTuple, ConstantRelocatable>
      ExternRelocatables;
  UndefPool Undefs;
};

void GlobalContext::CodeStats::dump(const IceString &Name, Ostream &Str) {
  if (!BuildDefs::dump())
    return;
#define X(str, tag)                                                            \
  Str << "|" << Name << "|" str "|" << Stats[CS_##tag] << "\n";
  CODESTATS_TABLE
#undef X
  Str << "|" << Name << "|Spills+Fills|"
      << Stats[CS_NumSpills] + Stats[CS_NumFills] << "\n";
  Str << "|" << Name << "|Memory Usage|";
  if (ssize_t MemUsed = llvm::TimeRecord::getCurrentTime(false).getMemUsed())
    Str << MemUsed;
  else
    Str << "(requires '-track-memory')";
  Str << "\n";
}

GlobalContext::GlobalContext(Ostream *OsDump, Ostream *OsEmit, Ostream *OsError,
                             ELFStreamer *ELFStr, const ClFlags &Flags)
    : ConstPool(new ConstantPool()), ErrorStatus(), StrDump(OsDump),
      StrEmit(OsEmit), StrError(OsError), Flags(Flags),
      RNG(Flags.getRandomSeed()), ObjectWriter(),
      OptQ(/*Sequential=*/Flags.isSequential(),
           /*MaxSize=*/Flags.getNumTranslationThreads()),
      // EmitQ is allowed unlimited size.
      EmitQ(/*Sequential=*/Flags.isSequential()),
      DataLowering(TargetDataLowering::createLowering(this)) {
  assert(OsDump && "OsDump is not defined for GlobalContext");
  assert(OsEmit && "OsEmit is not defined for GlobalContext");
  assert(OsError && "OsError is not defined for GlobalContext");
  // Make sure thread_local fields are properly initialized before any
  // accesses are made.  Do this here instead of at the start of
  // main() so that all clients (e.g. unit tests) can benefit for
  // free.
  GlobalContext::TlsInit();
  Cfg::TlsInit();
  // Create a new ThreadContext for the current thread.  No need to
  // lock AllThreadContexts at this point since no other threads have
  // access yet to this GlobalContext object.
  ThreadContext *MyTLS = new ThreadContext();
  AllThreadContexts.push_back(MyTLS);
  ICE_TLS_SET_FIELD(TLS, MyTLS);
  // Pre-register built-in stack names.
  if (BuildDefs::dump()) {
    // TODO(stichnot): There needs to be a strong relationship between
    // the newTimerStackID() return values and TSK_Default/TSK_Funcs.
    newTimerStackID("Total across all functions");
    newTimerStackID("Per-function summary");
  }
  Timers.initInto(MyTLS->Timers);
  switch (Flags.getOutFileType()) {
  case FT_Elf:
    ObjectWriter.reset(new ELFObjectWriter(*this, *ELFStr));
    break;
  case FT_Asm:
  case FT_Iasm:
    break;
  }

  // ProfileBlockInfoVarDecl is initialized here because it takes this as a
  // parameter -- we want to
  // ensure that at least this' member variables are initialized.
  ProfileBlockInfoVarDecl = VariableDeclaration::create(this);
  ProfileBlockInfoVarDecl->setAlignment(typeWidthInBytes(IceType_i64));
  ProfileBlockInfoVarDecl->setIsConstant(true);

  // Note: if you change this symbol, make sure to update
  // runtime/szrt_profiler.c as well.
  ProfileBlockInfoVarDecl->setName("__Sz_block_profile_info");
  ProfileBlockInfoVarDecl->setSuppressMangling();
  ProfileBlockInfoVarDecl->setLinkage(llvm::GlobalValue::ExternalLinkage);

  // Initialize the randomization cookie for constant blinding only if constant
  // blinding or pooling is turned on.
  // TODO(stichnot): Using RNG for constant blinding will affect the random
  // number to be used in nop-insertion and randomize-regalloc.
  if (Flags.getRandomizeAndPoolImmediatesOption() != RPI_None)
    RandomizationCookie =
        (uint32_t)RNG.next((uint64_t)std::numeric_limits<uint32_t>::max + 1);
}

void GlobalContext::translateFunctions() {
  while (std::unique_ptr<Cfg> Func = optQueueBlockingPop()) {
    // Install Func in TLS for Cfg-specific container allocators.
    Cfg::setCurrentCfg(Func.get());
    // Reset per-function stats being accumulated in TLS.
    resetStats();
    // Set verbose level to none if the current function does NOT
    // match the -verbose-focus command-line option.
    if (!matchSymbolName(Func->getFunctionName(),
                         getFlags().getVerboseFocusOn()))
      Func->setVerbose(IceV_None);
    // Disable translation if -notranslate is specified, or if the
    // current function matches the -translate-only option.  If
    // translation is disabled, just dump the high-level IR and
    // continue.
    if (getFlags().getDisableTranslation() ||
        !matchSymbolName(Func->getFunctionName(),
                         getFlags().getTranslateOnly())) {
      Func->dump();
      Cfg::setCurrentCfg(nullptr);
      continue; // Func goes out of scope and gets deleted
    }

    Func->translate();
    EmitterWorkItem *Item = nullptr;
    if (Func->hasError()) {
      getErrorStatus()->assign(EC_Translation);
      OstreamLocker L(this);
      getStrError() << "ICE translation error: " << Func->getFunctionName()
                    << ": " << Func->getError() << "\n";
      Item = new EmitterWorkItem(Func->getSequenceNumber());
    } else {
      Func->getAssembler<>()->setInternal(Func->getInternal());
      switch (getFlags().getOutFileType()) {
      case FT_Elf:
      case FT_Iasm: {
        Func->emitIAS();
        // The Cfg has already emitted into the assembly buffer, so
        // stats have been fully collected into this thread's TLS.
        // Dump them before TLS is reset for the next Cfg.
        dumpStats(Func->getFunctionName());
        Assembler *Asm = Func->releaseAssembler();
        // Copy relevant fields into Asm before Func is deleted.
        Asm->setFunctionName(Func->getFunctionName());
        Item = new EmitterWorkItem(Func->getSequenceNumber(), Asm);
        Item->setGlobalInits(Func->getGlobalInits());
      } break;
      case FT_Asm:
        // The Cfg has not been emitted yet, so stats are not ready
        // to be dumped.
        std::unique_ptr<VariableDeclarationList> GlobalInits =
            Func->getGlobalInits();
        Item = new EmitterWorkItem(Func->getSequenceNumber(), Func.release());
        Item->setGlobalInits(std::move(GlobalInits));
        break;
      }
    }
    Cfg::setCurrentCfg(nullptr);
    assert(Item);
    emitQueueBlockingPush(Item);
    // The Cfg now gets deleted as Func goes out of scope.
  }
}

namespace {

void addBlockInfoPtrs(const VariableDeclarationList &Globals,
                      VariableDeclaration *ProfileBlockInfo) {
  for (const VariableDeclaration *Global : Globals) {
    if (Cfg::isProfileGlobal(*Global)) {
      constexpr RelocOffsetT BlockExecutionCounterOffset = 0;
      ProfileBlockInfo->addInitializer(
          VariableDeclaration::RelocInitializer::create(
              Global, BlockExecutionCounterOffset));
    }
  }
}

// Ensure Pending is large enough that Pending[Index] is valid.
void resizePending(std::vector<EmitterWorkItem *> &Pending, uint32_t Index) {
  if (Index >= Pending.size())
    Pending.resize(Index + 1);
}

} // end of anonymous namespace

void GlobalContext::emitFileHeader() {
  TimerMarker T1(Ice::TimerStack::TT_emit, this);
  if (getFlags().getOutFileType() == FT_Elf) {
    getObjectWriter()->writeInitialELFHeader();
  } else {
    if (!BuildDefs::dump()) {
      getStrError() << "emitFileHeader for non-ELF";
      getErrorStatus()->assign(EC_Translation);
    }
    TargetHeaderLowering::createLowering(this)->lower();
  }
}

void GlobalContext::lowerConstants() { DataLowering->lowerConstants(); }

void GlobalContext::lowerGlobals(const IceString &SectionSuffix) {
  TimerMarker T(TimerStack::TT_emitGlobalInitializers, this);
  const bool DumpGlobalVariables = BuildDefs::dump() && Flags.getVerbose() &&
                                   Flags.getVerboseFocusOn().empty();
  if (DumpGlobalVariables) {
    OstreamLocker L(this);
    Ostream &Stream = getStrDump();
    for (const Ice::VariableDeclaration *Global : Globals) {
      Global->dump(this, Stream);
    }
  }
  if (Flags.getDisableTranslation())
    return;

  addBlockInfoPtrs(Globals, ProfileBlockInfoVarDecl);
  DataLowering->lowerGlobals(Globals, SectionSuffix);
  for (VariableDeclaration *Var : Globals) {
    Var->discardInitializers();
  }
  Globals.clear();
}

void GlobalContext::lowerProfileData() {
  // ProfileBlockInfoVarDecl is initialized in the constructor, and will only
  // ever be nullptr after this method completes. This assertion is a convoluted
  // way of ensuring lowerProfileData is invoked a single time.
  assert(ProfileBlockInfoVarDecl != nullptr);
  // This adds a 64-bit sentinel entry to the end of our array. For 32-bit
  // architectures this will waste 4 bytes.
  const SizeT Sizeof64BitNullPtr = typeWidthInBytes(IceType_i64);
  ProfileBlockInfoVarDecl->addInitializer(
      VariableDeclaration::ZeroInitializer::create(Sizeof64BitNullPtr));
  Globals.push_back(ProfileBlockInfoVarDecl);
  constexpr char ProfileDataSection[] = "$sz_profiler$";
  lowerGlobals(ProfileDataSection);
  ProfileBlockInfoVarDecl = nullptr;
}

void GlobalContext::emitItems() {
  const bool Threaded = !getFlags().isSequential();
  // Pending is a vector containing the reassembled, ordered list of
  // work items.  When we're ready for the next item, we first check
  // whether it's in the Pending list.  If not, we take an item from
  // the work queue, and if it's not the item we're waiting for, we
  // insert it into Pending and repeat.  The work item is deleted
  // after it is processed.
  std::vector<EmitterWorkItem *> Pending;
  uint32_t DesiredSequenceNumber = getFirstSequenceNumber();
  while (true) {
    resizePending(Pending, DesiredSequenceNumber);
    // See if Pending contains DesiredSequenceNumber.
    EmitterWorkItem *RawItem = Pending[DesiredSequenceNumber];
    if (RawItem == nullptr)
      RawItem = emitQueueBlockingPop();
    if (RawItem == nullptr)
      break;
    uint32_t ItemSeq = RawItem->getSequenceNumber();
    if (Threaded && ItemSeq != DesiredSequenceNumber) {
      resizePending(Pending, ItemSeq);
      Pending[ItemSeq] = RawItem;
      continue;
    }

    std::unique_ptr<EmitterWorkItem> Item(RawItem);
    ++DesiredSequenceNumber;
    switch (Item->getKind()) {
    case EmitterWorkItem::WI_Nop:
      break;
    case EmitterWorkItem::WI_GlobalInits: {
      accumulateGlobals(Item->getGlobalInits());
    } break;
    case EmitterWorkItem::WI_Asm: {
      lowerGlobalsIfNoCodeHasBeenSeen();
      accumulateGlobals(Item->getGlobalInits());

      std::unique_ptr<Assembler> Asm = Item->getAsm();
      Asm->alignFunction();
      IceString MangledName = mangleName(Asm->getFunctionName());
      switch (getFlags().getOutFileType()) {
      case FT_Elf:
        getObjectWriter()->writeFunctionCode(MangledName, Asm->getInternal(),
                                             Asm.get());
        break;
      case FT_Iasm: {
        OstreamLocker L(this);
        Cfg::emitTextHeader(MangledName, this, Asm.get());
        Asm->emitIASBytes(this);
      } break;
      case FT_Asm:
        llvm::report_fatal_error("Unexpected FT_Asm");
        break;
      }
    } break;
    case EmitterWorkItem::WI_Cfg: {
      if (!BuildDefs::dump())
        llvm::report_fatal_error("WI_Cfg work item created inappropriately");
      lowerGlobalsIfNoCodeHasBeenSeen();
      accumulateGlobals(Item->getGlobalInits());

      assert(getFlags().getOutFileType() == FT_Asm);
      std::unique_ptr<Cfg> Func = Item->getCfg();
      // Unfortunately, we have to temporarily install the Cfg in TLS
      // because Variable::asType() uses the allocator to create the
      // differently-typed copy.
      Cfg::setCurrentCfg(Func.get());
      Func->emit();
      Cfg::setCurrentCfg(nullptr);
      dumpStats(Func->getFunctionName());
    } break;
    }
  }

  // In case there are no code to be generated, we invoke the conditional
  // lowerGlobals again -- this is a no-op if code has been emitted.
  lowerGlobalsIfNoCodeHasBeenSeen();
}

// Scan a string for S[0-9A-Z]*_ patterns and replace them with
// S<num>_ where <num> is the next base-36 value.  If a type name
// legitimately contains that pattern, then the substitution will be
// made in error and most likely the link will fail.  In this case,
// the test classes can be rewritten not to use that pattern, which is
// much simpler and more reliable than implementing a full demangling
// parser.  Another substitution-in-error may occur if a type
// identifier ends with the pattern S[0-9A-Z]*, because an immediately
// following substitution string like "S1_" or "PS1_" may be combined
// with the previous type.
void GlobalContext::incrementSubstitutions(ManglerVector &OldName) const {
  const std::locale CLocale("C");
  // Provide extra space in case the length of <num> increases.
  ManglerVector NewName(OldName.size() * 2);
  size_t OldPos = 0;
  size_t NewPos = 0;
  size_t OldLen = OldName.size();
  for (; OldPos < OldLen; ++OldPos, ++NewPos) {
    if (OldName[OldPos] == '\0')
      break;
    if (OldName[OldPos] == 'S') {
      // Search forward until we find _ or invalid character (including \0).
      bool AllZs = true;
      bool Found = false;
      size_t Last;
      for (Last = OldPos + 1; Last < OldLen; ++Last) {
        char Ch = OldName[Last];
        if (Ch == '_') {
          Found = true;
          break;
        } else if (std::isdigit(Ch) || std::isupper(Ch, CLocale)) {
          if (Ch != 'Z')
            AllZs = false;
        } else {
          // Invalid character, stop searching.
          break;
        }
      }
      if (Found) {
        NewName[NewPos++] = OldName[OldPos++]; // 'S'
        size_t Length = Last - OldPos;
        // NewPos and OldPos point just past the 'S'.
        assert(NewName[NewPos - 1] == 'S');
        assert(OldName[OldPos - 1] == 'S');
        assert(OldName[OldPos + Length] == '_');
        if (AllZs) {
          // Replace N 'Z' characters with a '0' (if N=0) or '1' (if
          // N>0) followed by N '0' characters.
          NewName[NewPos++] = (Length ? '1' : '0');
          for (size_t i = 0; i < Length; ++i) {
            NewName[NewPos++] = '0';
          }
        } else {
          // Iterate right-to-left and increment the base-36 number.
          bool Carry = true;
          for (size_t i = 0; i < Length; ++i) {
            size_t Offset = Length - 1 - i;
            char Ch = OldName[OldPos + Offset];
            if (Carry) {
              Carry = false;
              switch (Ch) {
              case '9':
                Ch = 'A';
                break;
              case 'Z':
                Ch = '0';
                Carry = true;
                break;
              default:
                ++Ch;
                break;
              }
            }
            NewName[NewPos + Offset] = Ch;
          }
          NewPos += Length;
        }
        OldPos = Last;
        // Fall through and let the '_' be copied across.
      }
    }
    NewName[NewPos] = OldName[OldPos];
  }
  assert(NewName[NewPos] == '\0');
  OldName = NewName;
}

// In this context, name mangling means to rewrite a symbol using a
// given prefix.  For a C++ symbol, nest the original symbol inside
// the "prefix" namespace.  For other symbols, just prepend the
// prefix.
IceString GlobalContext::mangleName(const IceString &Name) const {
  // An already-nested name like foo::bar() gets pushed down one
  // level, making it equivalent to Prefix::foo::bar().
  //   _ZN3foo3barExyz ==> _ZN6Prefix3foo3barExyz
  // A non-nested but mangled name like bar() gets nested, making it
  // equivalent to Prefix::bar().
  //   _Z3barxyz ==> ZN6Prefix3barExyz
  // An unmangled, extern "C" style name, gets a simple prefix:
  //   bar ==> Prefixbar
  if (!BuildDefs::dump() || getFlags().getTestPrefix().empty())
    return Name;

  const IceString &TestPrefix = getFlags().getTestPrefix();
  unsigned PrefixLength = TestPrefix.length();
  ManglerVector NameBase(1 + Name.length());
  const size_t BufLen = 30 + Name.length() + PrefixLength;
  ManglerVector NewName(BufLen);
  uint32_t BaseLength = 0; // using uint32_t due to sscanf format string

  int ItemsParsed = sscanf(Name.c_str(), "_ZN%s", NameBase.data());
  if (ItemsParsed == 1) {
    // Transform _ZN3foo3barExyz ==> _ZN6Prefix3foo3barExyz
    //   (splice in "6Prefix")          ^^^^^^^
    snprintf(NewName.data(), BufLen, "_ZN%u%s%s", PrefixLength,
             TestPrefix.c_str(), NameBase.data());
    // We ignore the snprintf return value (here and below).  If we
    // somehow miscalculated the output buffer length, the output will
    // be truncated, but it will be truncated consistently for all
    // mangleName() calls on the same input string.
    incrementSubstitutions(NewName);
    return NewName.data();
  }

  // Artificially limit BaseLength to 9 digits (less than 1 billion)
  // because sscanf behavior is undefined on integer overflow.  If
  // there are more than 9 digits (which we test by looking at the
  // beginning of NameBase), then we consider this a failure to parse
  // a namespace mangling, and fall back to the simple prefixing.
  ItemsParsed = sscanf(Name.c_str(), "_Z%9u%s", &BaseLength, NameBase.data());
  if (ItemsParsed == 2 && BaseLength <= strlen(NameBase.data()) &&
      !isdigit(NameBase[0])) {
    // Transform _Z3barxyz ==> _ZN6Prefix3barExyz
    //                           ^^^^^^^^    ^
    // (splice in "N6Prefix", and insert "E" after "3bar")
    // But an "I" after the identifier indicates a template argument
    // list terminated with "E"; insert the new "E" before/after the
    // old "E".  E.g.:
    // Transform _Z3barIabcExyz ==> _ZN6Prefix3barIabcEExyz
    //                                ^^^^^^^^         ^
    // (splice in "N6Prefix", and insert "E" after "3barIabcE")
    ManglerVector OrigName(Name.length());
    ManglerVector OrigSuffix(Name.length());
    uint32_t ActualBaseLength = BaseLength;
    if (NameBase[ActualBaseLength] == 'I') {
      ++ActualBaseLength;
      while (NameBase[ActualBaseLength] != 'E' &&
             NameBase[ActualBaseLength] != '\0')
        ++ActualBaseLength;
    }
    strncpy(OrigName.data(), NameBase.data(), ActualBaseLength);
    OrigName[ActualBaseLength] = '\0';
    strcpy(OrigSuffix.data(), NameBase.data() + ActualBaseLength);
    snprintf(NewName.data(), BufLen, "_ZN%u%s%u%sE%s", PrefixLength,
             TestPrefix.c_str(), BaseLength, OrigName.data(),
             OrigSuffix.data());
    incrementSubstitutions(NewName);
    return NewName.data();
  }

  // Transform bar ==> Prefixbar
  //                   ^^^^^^
  return TestPrefix + Name;
}

GlobalContext::~GlobalContext() {
  llvm::DeleteContainerPointers(AllThreadContexts);
  LockedPtr<DestructorArray> Dtors = getDestructors();
  // Destructors are invoked in the opposite object construction order.
  for (auto DtorIter = Dtors->crbegin(); DtorIter != Dtors->crend();
       ++DtorIter) {
    (*DtorIter)();
  }
}

// TODO(stichnot): Consider adding thread-local caches of constant
// pool entries to reduce contention.

// All locking is done by the getConstantInt[0-9]+() target function.
Constant *GlobalContext::getConstantInt(Type Ty, int64_t Value) {
  switch (Ty) {
  case IceType_i1:
    return getConstantInt1(Value);
  case IceType_i8:
    return getConstantInt8(Value);
  case IceType_i16:
    return getConstantInt16(Value);
  case IceType_i32:
    return getConstantInt32(Value);
  case IceType_i64:
    return getConstantInt64(Value);
  default:
    llvm_unreachable("Bad integer type for getConstant");
  }
  return nullptr;
}

Constant *GlobalContext::getConstantInt1(int8_t ConstantInt1) {
  ConstantInt1 &= INT8_C(1);
  return getConstPool()->Integers1.getOrAdd(this, ConstantInt1);
}

Constant *GlobalContext::getConstantInt8(int8_t ConstantInt8) {
  return getConstPool()->Integers8.getOrAdd(this, ConstantInt8);
}

Constant *GlobalContext::getConstantInt16(int16_t ConstantInt16) {
  return getConstPool()->Integers16.getOrAdd(this, ConstantInt16);
}

Constant *GlobalContext::getConstantInt32(int32_t ConstantInt32) {
  return getConstPool()->Integers32.getOrAdd(this, ConstantInt32);
}

Constant *GlobalContext::getConstantInt64(int64_t ConstantInt64) {
  return getConstPool()->Integers64.getOrAdd(this, ConstantInt64);
}

Constant *GlobalContext::getConstantFloat(float ConstantFloat) {
  return getConstPool()->Floats.getOrAdd(this, ConstantFloat);
}

Constant *GlobalContext::getConstantDouble(double ConstantDouble) {
  return getConstPool()->Doubles.getOrAdd(this, ConstantDouble);
}

Constant *GlobalContext::getConstantSym(RelocOffsetT Offset,
                                        const IceString &Name,
                                        bool SuppressMangling) {
  return getConstPool()->Relocatables.getOrAdd(
      this, RelocatableTuple(Offset, Name, SuppressMangling));
}

Constant *GlobalContext::getConstantExternSym(const IceString &Name) {
  const RelocOffsetT Offset = 0;
  const bool SuppressMangling = true;
  return getConstPool()->ExternRelocatables.getOrAdd(
      this, RelocatableTuple(Offset, Name, SuppressMangling));
}

Constant *GlobalContext::getConstantUndef(Type Ty) {
  return getConstPool()->Undefs.getOrAdd(this, Ty);
}

// All locking is done by the getConstant*() target function.
Constant *GlobalContext::getConstantZero(Type Ty) {
  switch (Ty) {
  case IceType_i1:
    return getConstantInt1(0);
  case IceType_i8:
    return getConstantInt8(0);
  case IceType_i16:
    return getConstantInt16(0);
  case IceType_i32:
    return getConstantInt32(0);
  case IceType_i64:
    return getConstantInt64(0);
  case IceType_f32:
    return getConstantFloat(0);
  case IceType_f64:
    return getConstantDouble(0);
  case IceType_v4i1:
  case IceType_v8i1:
  case IceType_v16i1:
  case IceType_v16i8:
  case IceType_v8i16:
  case IceType_v4i32:
  case IceType_v4f32: {
    IceString Str;
    llvm::raw_string_ostream BaseOS(Str);
    BaseOS << "Unsupported constant type: " << Ty;
    llvm_unreachable(BaseOS.str().c_str());
  } break;
  case IceType_void:
  case IceType_NUM:
    break;
  }
  llvm_unreachable("Unknown type");
}

ConstantList GlobalContext::getConstantPool(Type Ty) {
  switch (Ty) {
  case IceType_i1:
  case IceType_i8:
    return getConstPool()->Integers8.getConstantPool();
  case IceType_i16:
    return getConstPool()->Integers16.getConstantPool();
  case IceType_i32:
    return getConstPool()->Integers32.getConstantPool();
  case IceType_i64:
    return getConstPool()->Integers64.getConstantPool();
  case IceType_f32:
    return getConstPool()->Floats.getConstantPool();
  case IceType_f64:
    return getConstPool()->Doubles.getConstantPool();
  case IceType_v4i1:
  case IceType_v8i1:
  case IceType_v16i1:
  case IceType_v16i8:
  case IceType_v8i16:
  case IceType_v4i32:
  case IceType_v4f32: {
    IceString Str;
    llvm::raw_string_ostream BaseOS(Str);
    BaseOS << "Unsupported constant type: " << Ty;
    llvm_unreachable(BaseOS.str().c_str());
  } break;
  case IceType_void:
  case IceType_NUM:
    break;
  }
  llvm_unreachable("Unknown type");
}

ConstantList GlobalContext::getConstantExternSyms() {
  return getConstPool()->ExternRelocatables.getConstantPool();
}

TimerStackIdT GlobalContext::newTimerStackID(const IceString &Name) {
  if (!BuildDefs::dump())
    return 0;
  auto Timers = getTimers();
  TimerStackIdT NewID = Timers->size();
  Timers->push_back(TimerStack(Name));
  return NewID;
}

TimerIdT GlobalContext::getTimerID(TimerStackIdT StackID,
                                   const IceString &Name) {
  auto Timers = &ICE_TLS_GET_FIELD(TLS)->Timers;
  assert(StackID < Timers->size());
  return Timers->at(StackID).getTimerID(Name);
}

void GlobalContext::pushTimer(TimerIdT ID, TimerStackIdT StackID) {
  auto Timers = &ICE_TLS_GET_FIELD(TLS)->Timers;
  assert(StackID < Timers->size());
  Timers->at(StackID).push(ID);
}

void GlobalContext::popTimer(TimerIdT ID, TimerStackIdT StackID) {
  auto Timers = &ICE_TLS_GET_FIELD(TLS)->Timers;
  assert(StackID < Timers->size());
  Timers->at(StackID).pop(ID);
}

void GlobalContext::resetTimer(TimerStackIdT StackID) {
  auto Timers = &ICE_TLS_GET_FIELD(TLS)->Timers;
  assert(StackID < Timers->size());
  Timers->at(StackID).reset();
}

void GlobalContext::setTimerName(TimerStackIdT StackID,
                                 const IceString &NewName) {
  auto Timers = &ICE_TLS_GET_FIELD(TLS)->Timers;
  assert(StackID < Timers->size());
  Timers->at(StackID).setName(NewName);
}

// Note: optQueueBlockingPush and optQueueBlockingPop use unique_ptr
// at the interface to take and transfer ownership, but they
// internally store the raw Cfg pointer in the work queue.  This
// allows e.g. future queue optimizations such as the use of atomics
// to modify queue elements.
void GlobalContext::optQueueBlockingPush(std::unique_ptr<Cfg> Func) {
  assert(Func);
  OptQ.blockingPush(Func.release());
  if (getFlags().isSequential())
    translateFunctions();
}

std::unique_ptr<Cfg> GlobalContext::optQueueBlockingPop() {
  return std::unique_ptr<Cfg>(OptQ.blockingPop());
}

void GlobalContext::emitQueueBlockingPush(EmitterWorkItem *Item) {
  assert(Item);
  EmitQ.blockingPush(Item);
  if (getFlags().isSequential())
    emitItems();
}

EmitterWorkItem *GlobalContext::emitQueueBlockingPop() {
  return EmitQ.blockingPop();
}

void GlobalContext::dumpStats(const IceString &Name, bool Final) {
  if (!getFlags().getDumpStats())
    return;
  OstreamLocker OL(this);
  if (Final) {
    getStatsCumulative()->dump(Name, getStrDump());
  } else {
    ICE_TLS_GET_FIELD(TLS)->StatsFunction.dump(Name, getStrDump());
  }
}

void GlobalContext::dumpTimers(TimerStackIdT StackID, bool DumpCumulative) {
  if (!BuildDefs::dump())
    return;
  auto Timers = getTimers();
  assert(Timers->size() > StackID);
  OstreamLocker L(this);
  Timers->at(StackID).dump(getStrDump(), DumpCumulative);
}

void TimerMarker::push() {
  switch (StackID) {
  case GlobalContext::TSK_Default:
    Active = Ctx->getFlags().getSubzeroTimingEnabled();
    break;
  case GlobalContext::TSK_Funcs:
    Active = Ctx->getFlags().getTimeEachFunction();
    break;
  default:
    break;
  }
  if (Active)
    Ctx->pushTimer(ID, StackID);
}

void TimerMarker::pushCfg(const Cfg *Func) {
  Ctx = Func->getContext();
  Active =
      Func->getFocusedTiming() || Ctx->getFlags().getSubzeroTimingEnabled();
  if (Active)
    Ctx->pushTimer(ID, StackID);
}

ICE_TLS_DEFINE_FIELD(GlobalContext::ThreadContext *, GlobalContext, TLS);

} // end of namespace Ice
