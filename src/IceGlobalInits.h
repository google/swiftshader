//===- subzero/src/IceGlobalInits.h - Global declarations -------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the representation of function declarations, global variable
/// declarations, and the corresponding variable initializers in Subzero.
///
/// Global variable initializers are represented as a sequence of simple
/// initializers.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEGLOBALINITS_H
#define SUBZERO_SRC_ICEGLOBALINITS_H

#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceIntrinsics.h"
#include "IceOperand.h"
#include "IceTypes.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wredundant-move"
#endif // __clang__

#include "llvm/Bitcode/NaCl/NaClBitcodeParser.h" // for NaClBitcodeRecord.
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/GlobalValue.h" // for GlobalValue::LinkageTypes.

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#include <memory>
#include <utility>

// TODO(kschimpf): Remove ourselves from using LLVM representation for calling
// conventions and linkage types.

namespace Ice {

/// Base class for global variable and function declarations.
class GlobalDeclaration {
  GlobalDeclaration() = delete;
  GlobalDeclaration(const GlobalDeclaration &) = delete;
  GlobalDeclaration &operator=(const GlobalDeclaration &) = delete;

public:
  /// Discriminator for LLVM-style RTTI.
  enum GlobalDeclarationKind {
    FunctionDeclarationKind,
    VariableDeclarationKind
  };
  GlobalDeclarationKind getKind() const { return Kind; }
  const IceString &getName() const { return Name; }
  void setName(const IceString &NewName) { Name = NewName; }
  bool hasName() const { return !Name.empty(); }
  bool isInternal() const {
    return Linkage == llvm::GlobalValue::InternalLinkage;
  }
  llvm::GlobalValue::LinkageTypes getLinkage() const { return Linkage; }
  bool isExternal() const {
    return Linkage == llvm::GlobalValue::ExternalLinkage;
  }
  void setLinkage(llvm::GlobalValue::LinkageTypes NewLinkage) {
    Linkage = NewLinkage;
  }
  virtual ~GlobalDeclaration() = default;

  /// Prints out type of the global declaration.
  virtual void dumpType(Ostream &Stream) const = 0;

  /// Prints out the global declaration.
  virtual void dump(GlobalContext *Ctx, Ostream &Stream) const = 0;
  void dump(Ostream &Stream) const {
    if (!BuildDefs::dump())
      return;
    GlobalContext *const Ctx = nullptr;
    dump(Ctx, Stream);
  }

  /// Returns true if when emitting names, we should suppress mangling.
  virtual bool getSuppressMangling() const = 0;

  /// Mangles name for cross tests, unless external and not defined locally (so
  /// that relocations across pnacl-sz and pnacl-llc will work).
  virtual IceString mangleName(GlobalContext *Ctx) const {
    return getSuppressMangling() ? Name : Ctx->mangleName(Name);
  }

  /// Returns textual name of linkage.
  const char *getLinkageName() const {
    return isInternal() ? "internal" : "external";
  }

protected:
  GlobalDeclaration(GlobalDeclarationKind Kind,
                    llvm::GlobalValue::LinkageTypes Linkage)
      : Kind(Kind), Linkage(Linkage) {}

  /// Returns true if linkage is defined correctly for the global declaration,
  /// based on default rules.
  bool verifyLinkageDefault(const GlobalContext *Ctx) const {
    switch (Linkage) {
    default:
      return false;
    case llvm::GlobalValue::InternalLinkage:
      return true;
    case llvm::GlobalValue::ExternalLinkage:
      return Ctx->getFlags().getAllowExternDefinedSymbols();
    }
  }

  const GlobalDeclarationKind Kind;
  IceString Name;
  llvm::GlobalValue::LinkageTypes Linkage;
};

/// Models a function declaration. This includes the type signature of the
/// function, its calling conventions, and its linkage.
class FunctionDeclaration : public GlobalDeclaration {
  FunctionDeclaration() = delete;
  FunctionDeclaration(const FunctionDeclaration &) = delete;
  FunctionDeclaration &operator=(const FunctionDeclaration &) = delete;

public:
  static FunctionDeclaration *create(GlobalContext *Context,
                                     const FuncSigType &Signature,
                                     llvm::CallingConv::ID CallingConv,
                                     llvm::GlobalValue::LinkageTypes Linkage,
                                     bool IsProto) {
    return new (Context->allocate<FunctionDeclaration>())
        FunctionDeclaration(Signature, CallingConv, Linkage, IsProto);
  }
  const FuncSigType &getSignature() const { return Signature; }
  llvm::CallingConv::ID getCallingConv() const { return CallingConv; }
  /// isProto implies that there isn't a (local) definition for the function.
  bool isProto() const { return IsProto; }
  static bool classof(const GlobalDeclaration *Addr) {
    return Addr->getKind() == FunctionDeclarationKind;
  }
  void dumpType(Ostream &Stream) const final;
  void dump(GlobalContext *Ctx, Ostream &Stream) const final;
  bool getSuppressMangling() const final { return isExternal() && IsProto; }

  /// Returns true if linkage is correct for the function declaration.
  bool verifyLinkageCorrect(const GlobalContext *Ctx) const {
    if (isPNaClABIExternalName() || isIntrinsicName(Ctx))
      return Linkage == llvm::GlobalValue::ExternalLinkage;
    return verifyLinkageDefault(Ctx);
  }

  /// Validates that the type signature of the function is correct. Returns true
  /// if valid.
  bool validateTypeSignature(const GlobalContext *Ctx) const {
    bool IsIntrinsic;
    if (const Intrinsics::FullIntrinsicInfo *Info =
            getIntrinsicInfo(Ctx, &IsIntrinsic))
      return validateIntrinsicTypeSignature(Info);
    return !IsIntrinsic && validateRegularTypeSignature();
  }

  /// Generates an error message describing why validateTypeSignature returns
  /// false.
  IceString getTypeSignatureError(const GlobalContext *Ctx);

  /// Returns corresponding PNaCl intrisic information.
  const Intrinsics::FullIntrinsicInfo *
  getIntrinsicInfo(const GlobalContext *Ctx) const {
    bool BadIntrinsic;
    return getIntrinsicInfo(Ctx, &BadIntrinsic);
  }

  /// Same as above, except IsIntrinsic is true if the function is intrinsic
  /// (even if not a PNaCl intrinsic).
  const Intrinsics::FullIntrinsicInfo *
  getIntrinsicInfo(const GlobalContext *Ctx, bool *IsIntrinsic) const;

private:
  const Ice::FuncSigType Signature;
  llvm::CallingConv::ID CallingConv;
  bool IsProto;

  FunctionDeclaration(const FuncSigType &Signature,
                      llvm::CallingConv::ID CallingConv,
                      llvm::GlobalValue::LinkageTypes Linkage, bool IsProto)
      : GlobalDeclaration(FunctionDeclarationKind, Linkage),
        Signature(Signature), CallingConv(CallingConv), IsProto(IsProto) {}

  bool isPNaClABIExternalName() const {
    const char *Name = getName().c_str();
    return strcmp(Name, "_start") == 0 || strcmp(Name, "__pnacl_pso_root") == 0;
  }

  bool isIntrinsicName(const GlobalContext *Ctx) const {
    bool IsIntrinsic;
    getIntrinsicInfo(Ctx, &IsIntrinsic);
    return IsIntrinsic;
  }

  bool validateRegularTypeSignature() const;

  bool validateIntrinsicTypeSignature(
      const Intrinsics::FullIntrinsicInfo *Info) const;
};

/// Models a global variable declaration, and its initializers.
class VariableDeclaration : public GlobalDeclaration {
  VariableDeclaration(const VariableDeclaration &) = delete;
  VariableDeclaration &operator=(const VariableDeclaration &) = delete;

public:
  /// Base class for a global variable initializer.
  class Initializer {
    Initializer(const Initializer &) = delete;
    Initializer &operator=(const Initializer &) = delete;

  public:
    /// Discriminator for LLVM-style RTTI.
    enum InitializerKind {
      DataInitializerKind,
      ZeroInitializerKind,
      RelocInitializerKind
    };
    InitializerKind getKind() const { return Kind; }
    virtual ~Initializer() = default;
    virtual SizeT getNumBytes() const = 0;
    virtual void dump(GlobalContext *Ctx, Ostream &Stream) const = 0;
    void dump(Ostream &Stream) const {
      if (BuildDefs::dump())
        dump(nullptr, Stream);
    }
    virtual void dumpType(Ostream &Stream) const;

  protected:
    explicit Initializer(InitializerKind Kind) : Kind(Kind) {}

  private:
    const InitializerKind Kind;
  };

  /// Models the data in a data initializer.
  using DataVecType = std::vector<char>;

  /// Defines a sequence of byte values as a data initializer.
  class DataInitializer : public Initializer {
    DataInitializer(const DataInitializer &) = delete;
    DataInitializer &operator=(const DataInitializer &) = delete;

  public:
    template <class... Args>
    static std::unique_ptr<DataInitializer> create(Args &&... TheArgs) {
      return makeUnique<DataInitializer>(std::forward<Args>(TheArgs)...);
    }

    const DataVecType &getContents() const { return Contents; }
    SizeT getNumBytes() const final { return Contents.size(); }
    void dump(GlobalContext *Ctx, Ostream &Stream) const final;
    static bool classof(const Initializer *D) {
      return D->getKind() == DataInitializerKind;
    }

  private:
    ENABLE_MAKE_UNIQUE;

    DataInitializer(const llvm::NaClBitcodeRecord::RecordVector &Values)
        : Initializer(DataInitializerKind), Contents(Values.size()) {
      for (SizeT I = 0; I < Values.size(); ++I)
        Contents[I] = static_cast<int8_t>(Values[I]);
    }

    DataInitializer(const char *Str, size_t StrLen)
        : Initializer(DataInitializerKind), Contents(StrLen) {
      for (size_t i = 0; i < StrLen; ++i)
        Contents[i] = Str[i];
    }

    /// The byte contents of the data initializer.
    DataVecType Contents;
  };

  /// Defines a sequence of bytes initialized to zero.
  class ZeroInitializer : public Initializer {
    ZeroInitializer(const ZeroInitializer &) = delete;
    ZeroInitializer &operator=(const ZeroInitializer &) = delete;

  public:
    static std::unique_ptr<ZeroInitializer> create(SizeT Size) {
      return makeUnique<ZeroInitializer>(Size);
    }
    SizeT getNumBytes() const final { return Size; }
    void dump(GlobalContext *Ctx, Ostream &Stream) const final;
    static bool classof(const Initializer *Z) {
      return Z->getKind() == ZeroInitializerKind;
    }

  private:
    ENABLE_MAKE_UNIQUE;

    explicit ZeroInitializer(SizeT Size)
        : Initializer(ZeroInitializerKind), Size(Size) {}

    /// The number of bytes to be zero initialized.
    SizeT Size;
  };

  /// Defines the relocation value of another global declaration.
  class RelocInitializer : public Initializer {
    RelocInitializer(const RelocInitializer &) = delete;
    RelocInitializer &operator=(const RelocInitializer &) = delete;

  public:
    static std::unique_ptr<RelocInitializer>
    create(const GlobalDeclaration *Declaration,
           const RelocOffsetArray &OffsetExpr) {
      return makeUnique<RelocInitializer>(Declaration, OffsetExpr);
    }

    RelocOffsetT getOffset() const {
      RelocOffsetT Offset = 0;
      for (const auto *RelocOffset : OffsetExpr) {
        Offset += RelocOffset->getOffset();
      }
      return Offset;
    }

    const GlobalDeclaration *getDeclaration() const { return Declaration; }
    SizeT getNumBytes() const final { return RelocAddrSize; }
    void dump(GlobalContext *Ctx, Ostream &Stream) const final;
    void dumpType(Ostream &Stream) const final;
    static bool classof(const Initializer *R) {
      return R->getKind() == RelocInitializerKind;
    }

  private:
    ENABLE_MAKE_UNIQUE;

    RelocInitializer(const GlobalDeclaration *Declaration,
                     const RelocOffsetArray &OffsetExpr)
        : Initializer(RelocInitializerKind),
          Declaration(Declaration), // The global declaration used in the reloc.
          OffsetExpr(OffsetExpr) {}

    const GlobalDeclaration *Declaration;
    /// The offset to add to the relocation.
    const RelocOffsetArray OffsetExpr;
  };

  /// Models the list of initializers.
  using InitializerListType = std::vector<std::unique_ptr<Initializer>>;

  static VariableDeclaration *create(GlobalContext *Context) {
    return new (Context->allocate<VariableDeclaration>()) VariableDeclaration();
  }

  const InitializerListType &getInitializers() const { return *Initializers; }
  bool getIsConstant() const { return IsConstant; }
  void setIsConstant(bool NewValue) { IsConstant = NewValue; }
  uint32_t getAlignment() const { return Alignment; }
  void setAlignment(uint32_t NewAlignment) { Alignment = NewAlignment; }
  bool hasInitializer() const { return HasInitializer; }
  bool hasNonzeroInitializer() const {
    return !(Initializers->size() == 1 &&
             llvm::isa<ZeroInitializer>((*Initializers)[0].get()));
  }

  /// Returns the number of bytes for the initializer of the global address.
  SizeT getNumBytes() const {
    SizeT Count = 0;
    for (const std::unique_ptr<Initializer> &Init : *Initializers) {
      Count += Init->getNumBytes();
    }
    return Count;
  }

  /// Adds Initializer to the list of initializers. Takes ownership of the
  /// initializer.
  void addInitializer(std::unique_ptr<Initializer> Initializer) {
    Initializers->emplace_back(std::move(Initializer));
    HasInitializer = true;
  }

  /// Prints out type for initializer associated with the declaration to Stream.
  void dumpType(Ostream &Stream) const final;

  /// Prints out the definition of the global variable declaration (including
  /// initialization).
  void dump(GlobalContext *Ctx, Ostream &Stream) const final;

  /// Returns true if linkage is correct for the variable declaration.
  bool verifyLinkageCorrect(const GlobalContext *Ctx) const {
    return verifyLinkageDefault(Ctx);
  }

  static bool classof(const GlobalDeclaration *Addr) {
    return Addr->getKind() == VariableDeclarationKind;
  }

  bool getSuppressMangling() const final {
    if (ForceSuppressMangling)
      return true;
    return isExternal() && !hasInitializer();
  }

  void setSuppressMangling() { ForceSuppressMangling = true; }

  void discardInitializers() { Initializers = nullptr; }

private:
  /// List of initializers for the declared variable.
  std::unique_ptr<InitializerListType> Initializers;
  bool HasInitializer;
  /// The alignment of the declared variable.
  uint32_t Alignment;
  /// True if a declared (global) constant.
  bool IsConstant;
  /// If set to true, force getSuppressMangling() to return true.
  bool ForceSuppressMangling;

  VariableDeclaration()
      : GlobalDeclaration(VariableDeclarationKind,
                          llvm::GlobalValue::InternalLinkage),
        Initializers(new InitializerListType), HasInitializer(false),
        Alignment(0), IsConstant(false), ForceSuppressMangling(false) {}
};

template <class StreamType>
inline StreamType &operator<<(StreamType &Stream,
                              const VariableDeclaration::Initializer &Init) {
  Init.dump(Stream);
  return Stream;
}

template <class StreamType>
inline StreamType &operator<<(StreamType &Stream,
                              const GlobalDeclaration &Addr) {
  Addr.dump(Stream);
  return Stream;
}

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEGLOBALINITS_H
