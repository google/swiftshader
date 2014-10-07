//===- subzero/src/IceGlobalInits.h - Global initializers -------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the representation of global addresses and
// initializers in Subzero. Global initializers are represented as a
// sequence of simple initializers.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEGLOBALINITS_H
#define SUBZERO_SRC_ICEGLOBALINITS_H

#include "IceDefs.h"

namespace llvm {
// TODO(kschimpf): Remove this dependency on LLVM IR.
class Value;
}

namespace Ice {

/// Models a global address, and its initializers.
class GlobalAddress {
  GlobalAddress(const GlobalAddress &) = delete;
  GlobalAddress &operator=(const GlobalAddress &) = delete;

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
    SizeT getNumBytes() const override { return Contents.size(); }
    void dump(Ostream &Stream) const override;
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
    SizeT getNumBytes() const override { return Size; }
    void dump(Ostream &Stream) const override;
    static bool classof(const Initializer *Z) {
      return Z->getKind() == ZeroInitializerKind;
    }

  private:
    // The number of bytes to be zero initialized.
    SizeT Size;
  };

  /// Defines the kind of relocation addresses allowed.
  enum RelocationKind { FunctionRelocation, GlobalAddressRelocation };

  /// Defines a relocation address (i.e. reference to a function
  /// or global variable address).
  class RelocationAddress {
    RelocationAddress &operator=(const RelocationAddress &) = delete;

  public:
    explicit RelocationAddress(const RelocationAddress &Addr)
        : Kind(Addr.Kind) {
      switch (Kind) {
      case FunctionRelocation:
        Address.Function = Addr.Address.Function;
        break;
      case GlobalAddressRelocation:
        Address.GlobalAddr = Addr.Address.GlobalAddr;
      }
    }
    explicit RelocationAddress(llvm::Value *Function)
        : Kind(FunctionRelocation) {
      Address.Function = Function;
    }
    explicit RelocationAddress(GlobalAddress *GlobalAddr)
        : Kind(GlobalAddressRelocation) {
      Address.GlobalAddr = GlobalAddr;
    }
    RelocationKind getKind() const { return Kind; }
    llvm::Value *getFunction() const {
      assert(Kind == FunctionRelocation);
      return Address.Function;
    }
    GlobalAddress *getGlobalAddr() const {
      assert(Kind == GlobalAddressRelocation);
      return Address.GlobalAddr;
    }
  private:
    const RelocationKind Kind;
    union {
      // TODO(kschimpf) Integrate Functions into ICE model.
      llvm::Value *Function;
      GlobalAddress *GlobalAddr;
    } Address;
  };

  // Relocation address offsets must be 32 bit values.
  typedef int32_t RelocOffsetType;
  static const SizeT RelocAddrSize = 4;

  /// Defines the relocation value of another address.
  class RelocInitializer : public Initializer {
    RelocInitializer(const RelocInitializer &) = delete;
    RelocInitializer &operator=(const RelocInitializer &) = delete;

  public:
    RelocInitializer(const RelocationAddress &Address, RelocOffsetType Offset)
        : Initializer(RelocInitializerKind), Address(Address), Offset(Offset) {}
    ~RelocInitializer() override {}
    RelocOffsetType getOffset() const { return Offset; }
    IceString getName() const;
    SizeT getNumBytes() const override { return RelocAddrSize; }
    void dump(Ostream &Stream) const override;
    virtual void dumpType(Ostream &Stream) const;
    static bool classof(const Initializer *R) {
      return R->getKind() == RelocInitializerKind;
    }

  private:
    // The global address used in the relocation.
    const RelocationAddress Address;
    // The offset to add to the relocation.
    const RelocOffsetType Offset;
  };

  /// Models the list of initializers.
  typedef std::vector<Initializer *> InitializerListType;

  GlobalAddress() : Alignment(0), IsConstant(false), IsInternal(true) {}
  ~GlobalAddress();

  const InitializerListType &getInitializers() const { return Initializers; }
  bool hasName() const { return !Name.empty(); }
  const IceString &getName() const { return Name; }
  void setName(const IceString &NewName) { Name = NewName; }
  bool getIsConstant() const { return IsConstant; }
  void setIsConstant(bool NewValue) { IsConstant = NewValue; }
  uint32_t getAlignment() const { return Alignment; }
  void setAlignment(uint32_t NewAlignment) { Alignment = NewAlignment; }
  bool getIsInternal() const { return IsInternal; }
  void setIsInternal(bool NewValue) { IsInternal = NewValue; }

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

  /// Prints out type for initializer associated with the global address
  /// to Stream.
  void dumpType(Ostream &Stream) const;

  /// Prints out the definition of the global address (including
  /// initialization).
  void dump(Ostream &Stream) const;

private:
  // list of initializers associated with the global address.
  InitializerListType Initializers;
  // The name for the global.
  IceString Name;
  // The alignment of the initializer.
  uint32_t Alignment;
  // True if a constant initializer.
  bool IsConstant;
  // True if the address is internal.
  bool IsInternal;
};

template <class StreamType>
inline StreamType &operator<<(StreamType &Stream,
                              const GlobalAddress::Initializer &Init) {
  Init.dump(Stream);
  return Stream;
}

template <class StreamType>
inline StreamType &operator<<(StreamType &Stream, const GlobalAddress &Addr) {
  Addr.dump(Stream);
  return Stream;
}

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEGLOBALINITS_H
