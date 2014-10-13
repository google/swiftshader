//===- subzero/src/IceGlobalInits.h - Global declarations -------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the representation of function declarations,
// global variable declarations, and the corresponding variable
// initializers in Subzero. Global variable initializers are
// represented as a sequence of simple initializers.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEGLOBALINITS_H
#define SUBZERO_SRC_ICEGLOBALINITS_H

#include "llvm/IR/CallingConv.h"
#include "llvm/IR/GlobalValue.h" // for GlobalValue::LinkageTypes

#include "IceDefs.h"
#include "IceTypes.h"

// TODO(kschimpf): Remove ourselves from using LLVM representation for calling
// conventions and linkage types.

namespace Ice {

/// Base class for global variable and function declarations.
class GlobalDeclaration {
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
  virtual ~GlobalDeclaration() {}

  /// Returns true if the declaration is external.
  virtual bool getIsExternal() const = 0;

  /// Prints out type of the global declaration.
  virtual void dumpType(Ostream &Stream) const = 0;

  /// Prints out the global declaration.
  virtual void dump(Ostream &Stream) const = 0;

  // Mangles name for cross tests, unless external and not defined locally
  // (so that relocations accross llvm2ice and pnacl-llc will work).
  virtual IceString mangleName(GlobalContext *Ctx) const = 0;

protected:
  GlobalDeclaration(GlobalDeclarationKind Kind) : Kind(Kind) {}

  const GlobalDeclarationKind Kind;
  IceString Name;
};

// Models a function declaration. This includes the type signature of
// the function, its calling conventions, and its linkage.
class FunctionDeclaration : public GlobalDeclaration {
  FunctionDeclaration(const FunctionDeclaration &) = delete;
  FunctionDeclaration &operator=(const FunctionDeclaration &) = delete;
  friend class GlobalContext;

public:
  static FunctionDeclaration *create(GlobalContext *Ctx,
                                     const FuncSigType &Signature,
                                     llvm::CallingConv::ID CallingConv,
                                     llvm::GlobalValue::LinkageTypes Linkage,
                                     bool IsProto);
  ~FunctionDeclaration() final {}
  const FuncSigType &getSignature() const { return Signature; }
  llvm::CallingConv::ID getCallingConv() const { return CallingConv; }
  llvm::GlobalValue::LinkageTypes getLinkage() const { return Linkage; }
  // isProto implies that there isn't a (local) definition for the function.
  bool isProto() const { return IsProto; }
  static bool classof(const GlobalDeclaration *Addr) {
    return Addr->getKind() == FunctionDeclarationKind;
  }
  void dumpType(Ostream &Stream) const final;
  void dump(Ostream &Stream) const final;
  bool getIsExternal() const final {
    return Linkage == llvm::GlobalValue::ExternalLinkage;
  }

  virtual IceString mangleName(GlobalContext *Ctx) const final {
    return (getIsExternal() && IsProto) ? Name : Ctx->mangleName(Name);
  }

private:
  const Ice::FuncSigType Signature;
  llvm::CallingConv::ID CallingConv;
  llvm::GlobalValue::LinkageTypes Linkage;
  bool IsProto;

  FunctionDeclaration(const FuncSigType &Signature,
                      llvm::CallingConv::ID CallingConv,
                      llvm::GlobalValue::LinkageTypes Linkage, bool IsProto)
      : GlobalDeclaration(FunctionDeclarationKind), Signature(Signature),
        CallingConv(CallingConv), Linkage(Linkage), IsProto(IsProto) {}
};

/// Models a global variable declaration, and its initializers.
class VariableDeclaration : public GlobalDeclaration {
  VariableDeclaration(const VariableDeclaration &) = delete;
  VariableDeclaration &operator=(const VariableDeclaration &) = delete;
  friend class GlobalContext;
  // TODO(kschimpf) Factor out allocation of initializers into the
  // global context, so that memory allocation/collection can be
  // optimized.
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
    virtual ~Initializer() {}
    virtual SizeT getNumBytes() const = 0;
    virtual void dump(Ostream &Stream) const = 0;
    virtual void dumpType(Ostream &Stream) const;

  protected:
    explicit Initializer(InitializerKind Kind) : Kind(Kind) {}

  private:
    const InitializerKind Kind;
  };

  // Models the data in a data initializer.
  typedef std::vector<uint8_t> DataVecType;

  /// Defines a sequence of byte values as a data initializer.
  class DataInitializer : public Initializer {
    DataInitializer(const DataInitializer &) = delete;
    DataInitializer &operator=(const DataInitializer &) = delete;

  public:
    template <class IntContainer>
    DataInitializer(const IntContainer &Values)
        : Initializer(DataInitializerKind), Contents(Values.size()) {
      size_t i = 0;
      for (auto &V : Values) {
        Contents[i] = static_cast<uint8_t>(V);
        ++i;
      }
    }
    DataInitializer(const char *Str, size_t StrLen)
        : Initializer(DataInitializerKind), Contents(StrLen) {
      for (size_t i = 0; i < StrLen; ++i)
        Contents[i] = static_cast<uint8_t>(Str[i]);
    }
    ~DataInitializer() override {}
    const DataVecType &getContents() const { return Contents; }
    SizeT getNumBytes() const final { return Contents.size(); }
    void dump(Ostream &Stream) const final;
    static bool classof(const Initializer *D) {
      return D->getKind() == DataInitializerKind;
    }

  private:
    // The byte contents of the data initializer.
    DataVecType Contents;
  };

  /// Defines a sequence of bytes initialized to zero.
  class ZeroInitializer : public Initializer {
    ZeroInitializer(const ZeroInitializer &) = delete;
    ZeroInitializer &operator=(const ZeroInitializer &) = delete;

  public:
    explicit ZeroInitializer(SizeT Size)
        : Initializer(ZeroInitializerKind), Size(Size) {}
    ~ZeroInitializer() override {}
    SizeT getNumBytes() const final { return Size; }
    void dump(Ostream &Stream) const final;
    static bool classof(const Initializer *Z) {
      return Z->getKind() == ZeroInitializerKind;
    }

  private:
    // The number of bytes to be zero initialized.
    SizeT Size;
  };

  // Relocation address offsets must be 32 bit values.
  typedef int32_t RelocOffsetType;
  static const SizeT RelocAddrSize = 4;

  /// Defines the relocation value of another global declaration.
  class RelocInitializer : public Initializer {
    RelocInitializer(const RelocInitializer &) = delete;
    RelocInitializer &operator=(const RelocInitializer &) = delete;

  public:
    RelocInitializer(const GlobalDeclaration *Declaration,
                     RelocOffsetType Offset)
        : Initializer(RelocInitializerKind), Declaration(Declaration),
          Offset(Offset) {}
    ~RelocInitializer() override {}
    RelocOffsetType getOffset() const { return Offset; }
    const GlobalDeclaration *getDeclaration() const { return Declaration; }
    SizeT getNumBytes() const final { return RelocAddrSize; }
    void dump(Ostream &Stream) const final;
    void dumpType(Ostream &Stream) const final;
    static bool classof(const Initializer *R) {
      return R->getKind() == RelocInitializerKind;
    }

  private:
    // The global declaration used in the relocation.
    const GlobalDeclaration *Declaration;
    // The offset to add to the relocation.
    const RelocOffsetType Offset;
  };

  /// Models the list of initializers.
  typedef std::vector<Initializer *> InitializerListType;

  static VariableDeclaration *create(GlobalContext *Ctx);
  ~VariableDeclaration() final;

  const InitializerListType &getInitializers() const { return Initializers; }
  bool getIsConstant() const { return IsConstant; }
  void setIsConstant(bool NewValue) { IsConstant = NewValue; }
  uint32_t getAlignment() const { return Alignment; }
  void setAlignment(uint32_t NewAlignment) { Alignment = NewAlignment; }
  bool getIsInternal() const { return IsInternal; }
  void setIsInternal(bool NewValue) { IsInternal = NewValue; }
  bool getIsExternal() const final { return !getIsInternal(); }
  bool hasInitializer() const {
    return !(Initializers.size() == 1 &&
             llvm::isa<ZeroInitializer>(Initializers[0]));
  }

  /// Returns the number of bytes for the initializer of the global
  /// address.
  SizeT getNumBytes() const {
    SizeT Count = 0;
    for (Initializer *Init : Initializers) {
      Count += Init->getNumBytes();
    }
    return Count;
  }

  /// Adds Initializer to the list of initializers. Takes ownership of
  /// the initializer.
  void addInitializer(Initializer *Initializer) {
    Initializers.push_back(Initializer);
  }

  /// Prints out type for initializer associated with the declaration
  /// to Stream.
  void dumpType(Ostream &Stream) const final;

  /// Prints out the definition of the global variable declaration
  /// (including initialization).
  void dump(Ostream &Stream) const final;

  static bool classof(const GlobalDeclaration *Addr) {
    return Addr->getKind() == VariableDeclarationKind;
  }

  IceString mangleName(GlobalContext *Ctx) const final {
    return (getIsExternal() && !hasInitializer())
        ? Name : Ctx->mangleName(Name);
  }


private:
  // list of initializers for the declared variable.
  InitializerListType Initializers;
  // The alignment of the declared variable.
  uint32_t Alignment;
  // True if a declared (global) constant.
  bool IsConstant;
  // True if the declaration is internal.
  bool IsInternal;

  VariableDeclaration()
      : GlobalDeclaration(VariableDeclarationKind), Alignment(0),
        IsConstant(false), IsInternal(true) {}
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
