//===- subzero/src/PNaClTranslator.cpp - ICE from bitcode -----------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the PNaCl bitcode file to Ice, to machine code
// translator.
//
//===----------------------------------------------------------------------===//

#include "PNaClTranslator.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTypeConverter.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeDecoders.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeHeader.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeParser.h"
#include "llvm/Bitcode/NaCl/NaClReaderWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ValueHandle.h"

#include <vector>
#include <cassert>

using namespace llvm;

namespace {

// TODO(kschimpf) Remove error recovery once implementation complete.
static cl::opt<bool> AllowErrorRecovery(
    "allow-pnacl-reader-error-recovery",
    cl::desc("Allow error recovery when reading PNaCl bitcode."),
    cl::init(false));

// Top-level class to read PNaCl bitcode files, and translate to ICE.
class TopLevelParser : public NaClBitcodeParser {
  TopLevelParser(const TopLevelParser &) LLVM_DELETED_FUNCTION;
  TopLevelParser &operator=(const TopLevelParser &) LLVM_DELETED_FUNCTION;

public:
  TopLevelParser(Ice::Translator &Translator, const std::string &InputName,
                 NaClBitcodeHeader &Header, NaClBitstreamCursor &Cursor,
                 bool &ErrorStatus)
      : NaClBitcodeParser(Cursor), Translator(Translator),
        Mod(new Module(InputName, getGlobalContext())), Header(Header),
        TypeConverter(getLLVMContext()), ErrorStatus(ErrorStatus), NumErrors(0),
        NumFunctionIds(0), NumFunctionBlocks(0),
        GlobalVarPlaceHolderType(convertToLLVMType(Ice::IceType_i8)) {
    Mod->setDataLayout(PNaClDataLayout);
  }

  virtual ~TopLevelParser() {}
  LLVM_OVERRIDE;

  Ice::Translator &getTranslator() { return Translator; }

  // Generates error with given Message. Always returns true.
  virtual bool Error(const std::string &Message) LLVM_OVERRIDE {
    ErrorStatus = true;
    ++NumErrors;
    NaClBitcodeParser::Error(Message);
    if (!AllowErrorRecovery)
      report_fatal_error("Unable to continue");
    return true;
  }

  /// Returns the number of errors found while parsing the bitcode
  /// file.
  unsigned getNumErrors() const { return NumErrors; }

  /// Returns the LLVM module associated with the translation.
  Module *getModule() const { return Mod.get(); }

  /// Returns the number of bytes in the bitcode header.
  size_t getHeaderSize() const { return Header.getHeaderSize(); }

  /// Returns the llvm context to use.
  LLVMContext &getLLVMContext() const { return Mod->getContext(); }

  /// Changes the size of the type list to the given size.
  void resizeTypeIDValues(unsigned NewSize) { TypeIDValues.resize(NewSize); }

  /// Returns the type associated with the given index.
  Type *getTypeByID(unsigned ID) {
    // Note: method resizeTypeIDValues expands TypeIDValues
    // to the specified size, and fills elements with NULL.
    Type *Ty = ID < TypeIDValues.size() ? TypeIDValues[ID] : NULL;
    if (Ty)
      return Ty;
    return reportTypeIDAsUndefined(ID);
  }

  /// Defines type for ID.
  void setTypeID(unsigned ID, Type *Ty) {
    if (ID < TypeIDValues.size() && TypeIDValues[ID] == NULL) {
      TypeIDValues[ID] = Ty;
      return;
    }
    reportBadSetTypeID(ID, Ty);
  }

  /// Sets the next function ID to the given LLVM function.
  void setNextFunctionID(Function *Fcn) {
    ++NumFunctionIds;
    ValueIDValues.push_back(Fcn);
  }

  /// Defines the next function ID as one that has an implementation
  /// (i.e a corresponding function block in the bitcode).
  void setNextValueIDAsImplementedFunction() {
    DefiningFunctionsList.push_back(ValueIDValues.size());
  }

  /// Returns the value id that should be associated with the the
  /// current function block. Increments internal counters during call
  /// so that it will be in correct position for next function block.
  unsigned getNextFunctionBlockValueID() {
    if (NumFunctionBlocks >= DefiningFunctionsList.size())
      report_fatal_error(
          "More function blocks than defined function addresses");
    return DefiningFunctionsList[NumFunctionBlocks++];
  }

  /// Returns the LLVM IR value associatd with the global value ID.
  Value *getGlobalValueByID(unsigned ID) const {
    if (ID >= ValueIDValues.size())
      return NULL;
    return ValueIDValues[ID];
  }

  /// Returns the number of function addresses (i.e. ID's) defined in
  /// the bitcode file.
  unsigned getNumFunctionIDs() const { return NumFunctionIds; }

  /// Returns the number of global values defined in the bitcode
  /// file.
  unsigned getNumGlobalValueIDs() const { return ValueIDValues.size(); }

  /// Resizes the list of value IDs to include Count global variable
  /// IDs.
  void resizeValueIDsForGlobalVarCount(unsigned Count) {
    ValueIDValues.resize(ValueIDValues.size() + Count);
  }

  /// Returns the global variable address associated with the given
  /// value ID. If the ID refers to a global variable address not yet
  /// defined, a placeholder is created so that we can fix it up
  /// later.
  Constant *getOrCreateGlobalVarRef(unsigned ID) {
    if (ID >= ValueIDValues.size())
      return NULL;
    if (Value *C = ValueIDValues[ID])
      return dyn_cast<Constant>(C);
    Constant *C = new GlobalVariable(*Mod, GlobalVarPlaceHolderType, false,
                                     GlobalValue::ExternalLinkage, 0);
    ValueIDValues[ID] = C;
    return C;
  }

  /// Assigns the given global variable (address) to the given value
  /// ID.  Returns true if ID is a valid global variable ID. Otherwise
  /// returns false.
  bool assignGlobalVariable(GlobalVariable *GV, unsigned ID) {
    if (ID < NumFunctionIds || ID >= ValueIDValues.size())
      return false;
    WeakVH &OldV = ValueIDValues[ID];
    if (OldV == NULL) {
      ValueIDValues[ID] = GV;
      return true;
    }

    // If reached, there was a forward reference to this value. Replace it.
    Value *PrevVal = OldV;
    GlobalVariable *Placeholder = cast<GlobalVariable>(PrevVal);
    Placeholder->replaceAllUsesWith(
        ConstantExpr::getBitCast(GV, Placeholder->getType()));
    Placeholder->eraseFromParent();
    ValueIDValues[ID] = GV;
    return true;
  }

  /// Returns the corresponding ICE type for LLVMTy.
  Ice::Type convertToIceType(Type *LLVMTy) {
    Ice::Type IceTy = TypeConverter.convertToIceType(LLVMTy);
    if (IceTy >= Ice::IceType_NUM) {
      return convertToIceTypeError(LLVMTy);
    }
    return IceTy;
  }

  /// Returns the corresponding LLVM type for IceTy.
  Type *convertToLLVMType(Ice::Type IceTy) const {
    return TypeConverter.convertToLLVMType(IceTy);
  }

  /// Returns the LLVM integer type with the given number of Bits.  If
  /// Bits is not a valid PNaCl type, returns NULL.
  Type *getLLVMIntegerType(unsigned Bits) const {
    return TypeConverter.getLLVMIntegerType(Bits);
  }

  /// Returns the LLVM vector with the given Size and Ty. If not a
  /// valid PNaCl vector type, returns NULL.
  Type *getLLVMVectorType(unsigned Size, Ice::Type Ty) const {
    return TypeConverter.getLLVMVectorType(Size, Ty);
  }

  /// Returns the model for pointer types in ICE.
  Ice::Type getIcePointerType() const {
    return TypeConverter.getIcePointerType();
  }

private:
  // The translator associated with the parser.
  Ice::Translator &Translator;
  // The parsed module.
  OwningPtr<Module> Mod;
  // The bitcode header.
  NaClBitcodeHeader &Header;
  // Converter between LLVM and ICE types.
  Ice::TypeConverter TypeConverter;
  // The exit status that should be set to true if an error occurs.
  bool &ErrorStatus;
  // The number of errors reported.
  unsigned NumErrors;
  // The types associated with each type ID.
  std::vector<Type *> TypeIDValues;
  // The (global) value IDs.
  std::vector<WeakVH> ValueIDValues;
  // The number of function IDs.
  unsigned NumFunctionIds;
  // The number of function blocks (processed so far).
  unsigned NumFunctionBlocks;
  // The list of value IDs (in the order found) of defining function
  // addresses.
  std::vector<unsigned> DefiningFunctionsList;
  // Cached global variable placeholder type. Used for all forward
  // references to global variable addresses.
  Type *GlobalVarPlaceHolderType;

  virtual bool ParseBlock(unsigned BlockID) LLVM_OVERRIDE;

  /// Reports that type ID is undefined, and then returns
  /// the void type.
  Type *reportTypeIDAsUndefined(unsigned ID);

  /// Reports error about bad call to setTypeID.
  void reportBadSetTypeID(unsigned ID, Type *Ty);

  // Reports that there is no corresponding ICE type for LLVMTy, and
  // returns ICE::IceType_void.
  Ice::Type convertToIceTypeError(Type *LLVMTy);
};

Type *TopLevelParser::reportTypeIDAsUndefined(unsigned ID) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Can't find type for type id: " << ID;
  Error(StrBuf.str());
  // TODO(kschimpf) Remove error recovery once implementation complete.
  Type *Ty = TypeConverter.convertToLLVMType(Ice::IceType_void);
  // To reduce error messages, update type list if possible.
  if (ID < TypeIDValues.size())
    TypeIDValues[ID] = Ty;
  return Ty;
}

void TopLevelParser::reportBadSetTypeID(unsigned ID, Type *Ty) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  if (ID >= TypeIDValues.size()) {
    StrBuf << "Type index " << ID << " out of range: can't install.";
  } else {
    // Must be case that index already defined.
    StrBuf << "Type index " << ID << " defined as " << *TypeIDValues[ID]
           << " and " << *Ty << ".";
  }
  Error(StrBuf.str());
}

Ice::Type TopLevelParser::convertToIceTypeError(Type *LLVMTy) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Invalid LLVM type: " << *LLVMTy;
  Error(StrBuf.str());
  return Ice::IceType_void;
}

// Base class for parsing blocks within the bitcode file.  Note:
// Because this is the base class of block parsers, we generate error
// messages if ParseBlock or ParseRecord is not overridden in derived
// classes.
class BlockParserBaseClass : public NaClBitcodeParser {
public:
  // Constructor for the top-level module block parser.
  BlockParserBaseClass(unsigned BlockID, TopLevelParser *Context)
      : NaClBitcodeParser(BlockID, Context), Context(Context) {}

  virtual ~BlockParserBaseClass() LLVM_OVERRIDE {}

protected:
  // The context parser that contains the decoded state.
  TopLevelParser *Context;

  // Constructor for nested block parsers.
  BlockParserBaseClass(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : NaClBitcodeParser(BlockID, EnclosingParser),
        Context(EnclosingParser->Context) {}

  // Gets the translator associated with the bitcode parser.
  Ice::Translator &getTranslator() { return Context->getTranslator(); }

  // Generates an error Message with the bit address prefixed to it.
  virtual bool Error(const std::string &Message) LLVM_OVERRIDE {
    uint64_t Bit = Record.GetStartBit() + Context->getHeaderSize() * 8;
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "(" << format("%" PRIu64 ":%u", (Bit / 8),
                            static_cast<unsigned>(Bit % 8)) << ") " << Message;
    return Context->Error(StrBuf.str());
  }

  // Default implementation. Reports that block is unknown and skips
  // its contents.
  virtual bool ParseBlock(unsigned BlockID) LLVM_OVERRIDE;

  // Default implementation. Reports that the record is not
  // understood.
  virtual void ProcessRecord() LLVM_OVERRIDE;

  // Checks if the size of the record is Size.  Return true if valid.
  // Otherwise generates an error and returns false.
  bool isValidRecordSize(unsigned Size, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() == Size)
      return true;
    ReportRecordSizeError(Size, RecordName, NULL);
    return false;
  }

  // Checks if the size of the record is at least as large as the
  // LowerLimit. Returns true if valid.  Otherwise generates an error
  // and returns false.
  bool isValidRecordSizeAtLeast(unsigned LowerLimit, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() >= LowerLimit)
      return true;
    ReportRecordSizeError(LowerLimit, RecordName, "at least");
    return false;
  }

  // Checks if the size of the record is no larger than the
  // UpperLimit.  Returns true if valid.  Otherwise generates an error
  // and returns false.
  bool isValidRecordSizeAtMost(unsigned UpperLimit, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() <= UpperLimit)
      return true;
    ReportRecordSizeError(UpperLimit, RecordName, "no more than");
    return false;
  }

  // Checks if the size of the record is at least as large as the
  // LowerLimit, and no larger than the UpperLimit.  Returns true if
  // valid.  Otherwise generates an error and returns false.
  bool isValidRecordSizeInRange(unsigned LowerLimit, unsigned UpperLimit,
                                const char *RecordName) {
    return isValidRecordSizeAtLeast(LowerLimit, RecordName) ||
           isValidRecordSizeAtMost(UpperLimit, RecordName);
  }

private:
  /// Generates a record size error. ExpectedSize is the number
  /// of elements expected. RecordName is the name of the kind of
  /// record that has incorrect size. ContextMessage (if not NULL)
  /// is appended to "record expects" to describe how ExpectedSize
  /// should be interpreted.
  void ReportRecordSizeError(unsigned ExpectedSize, const char *RecordName,
                             const char *ContextMessage);
};

void BlockParserBaseClass::ReportRecordSizeError(unsigned ExpectedSize,
                                                 const char *RecordName,
                                                 const char *ContextMessage) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << RecordName << " record expects";
  if (ContextMessage)
    StrBuf << " " << ContextMessage;
  StrBuf << " " << ExpectedSize << " argument";
  if (ExpectedSize > 1)
    StrBuf << "s";
  StrBuf << ". Found: " << Record.GetValues().size();
  Error(StrBuf.str());
}

bool BlockParserBaseClass::ParseBlock(unsigned BlockID) {
  // If called, derived class doesn't know how to handle block.
  // Report error and skip.
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Don't know how to parse block id: " << BlockID;
  Error(StrBuf.str());
  // TODO(kschimpf) Remove error recovery once implementation complete.
  SkipBlock();
  return false;
}

void BlockParserBaseClass::ProcessRecord() {
  // If called, derived class doesn't know how to handle.
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Don't know how to process record: " << Record;
  Error(StrBuf.str());
}

// Class to parse a types block.
class TypesParser : public BlockParserBaseClass {
public:
  TypesParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : BlockParserBaseClass(BlockID, EnclosingParser), NextTypeId(0) {}

  ~TypesParser() LLVM_OVERRIDE {}

private:
  // The type ID that will be associated with the next type defining
  // record in the types block.
  unsigned NextTypeId;

  virtual void ProcessRecord() LLVM_OVERRIDE;
};

void TypesParser::ProcessRecord() {
  Type *Ty = NULL;
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  switch (Record.GetCode()) {
  case naclbitc::TYPE_CODE_NUMENTRY:
    // NUMENTRY: [numentries]
    if (!isValidRecordSize(1, "Type count"))
      return;
    Context->resizeTypeIDValues(Values[0]);
    return;
  case naclbitc::TYPE_CODE_VOID:
    // VOID
    if (!isValidRecordSize(0, "Type void"))
      return;
    Ty = Context->convertToLLVMType(Ice::IceType_void);
    break;
  case naclbitc::TYPE_CODE_FLOAT:
    // FLOAT
    if (!isValidRecordSize(0, "Type float"))
      return;
    Ty = Context->convertToLLVMType(Ice::IceType_f32);
    break;
  case naclbitc::TYPE_CODE_DOUBLE:
    // DOUBLE
    if (!isValidRecordSize(0, "Type double"))
      return;
    Ty = Context->convertToLLVMType(Ice::IceType_f64);
    break;
  case naclbitc::TYPE_CODE_INTEGER:
    // INTEGER: [width]
    if (!isValidRecordSize(1, "Type integer"))
      return;
    Ty = Context->getLLVMIntegerType(Values[0]);
    if (Ty == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Type integer record with invalid bitsize: " << Values[0];
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      // Fix type so that we can continue.
      Ty = Context->convertToLLVMType(Ice::IceType_i32);
    }
    break;
  case naclbitc::TYPE_CODE_VECTOR: {
    // VECTOR: [numelts, eltty]
    if (!isValidRecordSize(2, "Type vector"))
      return;
    Type *BaseTy = Context->getTypeByID(Values[1]);
    Ty = Context->getLLVMVectorType(Values[0],
                                    Context->convertToIceType(BaseTy));
    if (Ty == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Invalid type vector record: <" << Values[0] << " x " << *BaseTy
             << ">";
      Error(StrBuf.str());
      Ty = Context->convertToLLVMType(Ice::IceType_void);
    }
    break;
  }
  case naclbitc::TYPE_CODE_FUNCTION: {
    // FUNCTION: [vararg, retty, paramty x N]
    if (!isValidRecordSizeAtLeast(2, "Type signature"))
      return;
    SmallVector<Type *, 8> ArgTys;
    for (unsigned i = 2, e = Values.size(); i != e; ++i) {
      ArgTys.push_back(Context->getTypeByID(Values[i]));
    }
    Ty = FunctionType::get(Context->getTypeByID(Values[1]), ArgTys, Values[0]);
    break;
  }
  default:
    BlockParserBaseClass::ProcessRecord();
    return;
  }
  // If Ty not defined, assume error. Use void as filler.
  if (Ty == NULL)
    Ty = Context->convertToLLVMType(Ice::IceType_void);
  Context->setTypeID(NextTypeId++, Ty);
}

/// Parses the globals block (i.e. global variables).
class GlobalsParser : public BlockParserBaseClass {
public:
  GlobalsParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : BlockParserBaseClass(BlockID, EnclosingParser), InitializersNeeded(0),
        Alignment(1), IsConstant(false) {
    NextGlobalID = Context->getNumFunctionIDs();
  }

  virtual ~GlobalsParser() LLVM_OVERRIDE {}

private:
  // Holds the sequence of initializers for the global.
  SmallVector<Constant *, 10> Initializers;

  // Keeps track of how many initializers are expected for
  // the global variable being built.
  unsigned InitializersNeeded;

  // The alignment assumed for the global variable being built.
  unsigned Alignment;

  // True if the global variable being built is a constant.
  bool IsConstant;

  // The index of the next global variable.
  unsigned NextGlobalID;

  virtual void ExitBlock() LLVM_OVERRIDE {
    verifyNoMissingInitializers();
    unsigned NumIDs = Context->getNumGlobalValueIDs();
    if (NextGlobalID < NumIDs) {
      unsigned NumFcnIDs = Context->getNumFunctionIDs();
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Globals block expects " << (NumIDs - NumFcnIDs)
             << " global definitions. Found: " << (NextGlobalID - NumFcnIDs);
      Error(StrBuf.str());
    }
    BlockParserBaseClass::ExitBlock();
  }

  virtual void ProcessRecord() LLVM_OVERRIDE;

  // Checks if the number of initializers needed is the same as the
  // number found in the bitcode file. If different, and error message
  // is generated, and the internal state of the parser is fixed so
  // this condition is no longer violated.
  void verifyNoMissingInitializers() {
    if (InitializersNeeded != Initializers.size()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Global variable @g"
             << (NextGlobalID - Context->getNumFunctionIDs()) << " expected "
             << InitializersNeeded << " initializer";
      if (InitializersNeeded > 1)
        StrBuf << "s";
      StrBuf << ". Found: " << Initializers.size();
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      // Fix up state so that we can continue.
      InitializersNeeded = Initializers.size();
      installGlobalVar();
    }
  }

  // Reserves a slot in the list of initializers being built. If there
  // isn't room for the slot, an error message is generated.
  void reserveInitializer(const char *RecordName) {
    if (InitializersNeeded <= Initializers.size()) {
      Error(std::string(RecordName) +
            " record: Too many initializers, ignoring.");
    }
  }

  // Takes the initializers (and other parser state values) and
  // installs a global variable (with the initializers) into the list
  // of ValueIDs.
  void installGlobalVar() {
    Constant *Init = NULL;
    switch (Initializers.size()) {
    case 0:
      Error("No initializer for global variable in global vars block");
      return;
    case 1:
      Init = Initializers[0];
      break;
    default:
      Init = ConstantStruct::getAnon(Context->getLLVMContext(), Initializers,
                                     true);
      break;
    }
    GlobalVariable *GV =
        new GlobalVariable(*Context->getModule(), Init->getType(), IsConstant,
                           GlobalValue::InternalLinkage, Init, "");
    GV->setAlignment(Alignment);
    if (!Context->assignGlobalVariable(GV, NextGlobalID)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Defining global V[" << NextGlobalID
             << "] not allowed. Out of range.";
      Error(StrBuf.str());
    }
    ++NextGlobalID;
    Initializers.clear();
    InitializersNeeded = 0;
    Alignment = 1;
    IsConstant = false;
  }
};

void GlobalsParser::ProcessRecord() {
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  switch (Record.GetCode()) {
  case naclbitc::GLOBALVAR_COUNT:
    // COUNT: [n]
    if (!isValidRecordSize(1, "Globals count"))
      return;
    if (NextGlobalID != Context->getNumFunctionIDs()) {
      Error("Globals count record not first in block.");
      return;
    }
    verifyNoMissingInitializers();
    Context->resizeValueIDsForGlobalVarCount(Values[0]);
    return;
  case naclbitc::GLOBALVAR_VAR: {
    // VAR: [align, isconst]
    if (!isValidRecordSize(2, "Globals variable"))
      return;
    verifyNoMissingInitializers();
    InitializersNeeded = 1;
    Initializers.clear();
    Alignment = (1 << Values[0]) >> 1;
    IsConstant = Values[1] != 0;
    return;
  }
  case naclbitc::GLOBALVAR_COMPOUND:
    // COMPOUND: [size]
    if (!isValidRecordSize(1, "globals compound"))
      return;
    if (Initializers.size() > 0 || InitializersNeeded != 1) {
      Error("Globals compound record not first initializer");
      return;
    }
    if (Values[0] < 2) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Globals compound record size invalid. Found: " << Values[0];
      Error(StrBuf.str());
      return;
    }
    InitializersNeeded = Values[0];
    return;
  case naclbitc::GLOBALVAR_ZEROFILL: {
    // ZEROFILL: [size]
    if (!isValidRecordSize(1, "Globals zerofill"))
      return;
    reserveInitializer("Globals zerofill");
    Type *Ty =
        ArrayType::get(Context->convertToLLVMType(Ice::IceType_i8), Values[0]);
    Constant *Zero = ConstantAggregateZero::get(Ty);
    Initializers.push_back(Zero);
    break;
  }
  case naclbitc::GLOBALVAR_DATA: {
    // DATA: [b0, b1, ...]
    if (!isValidRecordSizeAtLeast(1, "Globals data"))
      return;
    reserveInitializer("Globals data");
    unsigned Size = Values.size();
    SmallVector<uint8_t, 32> Buf;
    for (unsigned i = 0; i < Size; ++i)
      Buf.push_back(static_cast<uint8_t>(Values[i]));
    Constant *Init = ConstantDataArray::get(
        Context->getLLVMContext(), ArrayRef<uint8_t>(Buf.data(), Buf.size()));
    Initializers.push_back(Init);
    break;
  }
  case naclbitc::GLOBALVAR_RELOC: {
    // RELOC: [val, [addend]]
    if (!isValidRecordSizeInRange(1, 2, "Globals reloc"))
      return;
    Constant *BaseVal = Context->getOrCreateGlobalVarRef(Values[0]);
    if (BaseVal == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Can't find global relocation value: " << Values[0];
      Error(StrBuf.str());
      return;
    }
    Type *IntPtrType = Context->convertToLLVMType(Context->getIcePointerType());
    Constant *Val = ConstantExpr::getPtrToInt(BaseVal, IntPtrType);
    if (Values.size() == 2) {
      Val = ConstantExpr::getAdd(Val, ConstantInt::get(IntPtrType, Values[1]));
    }
    Initializers.push_back(Val);
    break;
  }
  default:
    BlockParserBaseClass::ProcessRecord();
    return;
  }
  // If reached, just processed another initializer. See if time
  // to install global.
  if (InitializersNeeded == Initializers.size())
    installGlobalVar();
}

// Parses a valuesymtab block in the bitcode file.
class ValuesymtabParser : public BlockParserBaseClass {
  typedef SmallString<128> StringType;

public:
  ValuesymtabParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser,
                    bool AllowBbEntries)
      : BlockParserBaseClass(BlockID, EnclosingParser),
        AllowBbEntries(AllowBbEntries) {}

  virtual ~ValuesymtabParser() LLVM_OVERRIDE {}

private:
  // True if entries to name basic blocks allowed.
  bool AllowBbEntries;

  virtual void ProcessRecord() LLVM_OVERRIDE;

  void ConvertToString(StringType &ConvertedName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    for (size_t i = 1, e = Values.size(); i != e; ++i) {
      ConvertedName += static_cast<char>(Values[i]);
    }
  }
};

void ValuesymtabParser::ProcessRecord() {
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  StringType ConvertedName;
  switch (Record.GetCode()) {
  case naclbitc::VST_CODE_ENTRY: {
    // VST_ENTRY: [ValueId, namechar x N]
    if (!isValidRecordSizeAtLeast(2, "Valuesymtab value entry"))
      return;
    ConvertToString(ConvertedName);
    Value *V = Context->getGlobalValueByID(Values[0]);
    if (V == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Invalid global address ID in valuesymtab: " << Values[0];
      Error(StrBuf.str());
      return;
    }
    V->setName(StringRef(ConvertedName.data(), ConvertedName.size()));
    return;
  }
  case naclbitc::VST_CODE_BBENTRY: {
    // VST_BBENTRY: [BbId, namechar x N]
    // For now, since we aren't processing function blocks, don't handle.
    if (AllowBbEntries) {
      Error("Valuesymtab bb entry not implemented");
      return;
    }
    break;
  }
  default:
    break;
  }
  // If reached, don't know how to handle record.
  BlockParserBaseClass::ProcessRecord();
  return;
}

/// Parses function blocks in the bitcode file.
class FunctionParser : public BlockParserBaseClass {
  FunctionParser(const FunctionParser &) LLVM_DELETED_FUNCTION;
  FunctionParser &operator=(const FunctionParser &) LLVM_DELETED_FUNCTION;

public:
  FunctionParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : BlockParserBaseClass(BlockID, EnclosingParser),
        Func(new Ice::Cfg(getTranslator().getContext())), CurrentBbIndex(0),
        FcnId(Context->getNextFunctionBlockValueID()),
        LLVMFunc(cast<Function>(Context->getGlobalValueByID(FcnId))),
        CachedNumGlobalValueIDs(Context->getNumGlobalValueIDs()),
        InstIsTerminating(false) {
    Func->setFunctionName(LLVMFunc->getName());
    Func->setReturnType(Context->convertToIceType(LLVMFunc->getReturnType()));
    Func->setInternal(LLVMFunc->hasInternalLinkage());
    CurrentNode = InstallNextBasicBlock();
    for (Function::const_arg_iterator ArgI = LLVMFunc->arg_begin(),
                                      ArgE = LLVMFunc->arg_end();
         ArgI != ArgE; ++ArgI) {
      Func->addArg(NextInstVar(Context->convertToIceType(ArgI->getType())));
    }
  }

  ~FunctionParser() LLVM_OVERRIDE;

private:
  // Timer for reading function bitcode and converting to ICE.
  Ice::Timer TConvert;
  // The corresponding ICE function defined by the function block.
  Ice::Cfg *Func;
  // The index to the current basic block being built.
  uint32_t CurrentBbIndex;
  // The basic block being built.
  Ice::CfgNode *CurrentNode;
  // The ID for the function.
  unsigned FcnId;
  // The corresponding LLVM function.
  Function *LLVMFunc;
  // Holds operands local to the function block, based on indices
  // defined in the bitcode file.
  std::vector<Ice::Operand *> LocalOperands;
  // Holds the dividing point between local and global absolute value indices.
  uint32_t CachedNumGlobalValueIDs;
  // True if the last processed instruction was a terminating
  // instruction.
  bool InstIsTerminating;

  virtual void ProcessRecord() LLVM_OVERRIDE;

  virtual void ExitBlock() LLVM_OVERRIDE;

  // Creates and appends a new basic block to the list of basic blocks.
  Ice::CfgNode *InstallNextBasicBlock() { return Func->makeNode(); }

  // Returns the Index-th basic block in the list of basic blocks.
  Ice::CfgNode *GetBasicBlock(uint32_t Index) {
    const Ice::NodeList &Nodes = Func->getNodes();
    if (Index >= Nodes.size()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Reference to basic block " << Index
             << " not found. Must be less than " << Nodes.size();
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Index = 0;
    }
    return Nodes[Index];
  }

  // Generates the next available local variable using the given
  // type.  Note: if Ty is void, this function returns NULL.
  Ice::Variable *NextInstVar(Ice::Type Ty) {
    if (Ty == Ice::IceType_void)
      return NULL;
    Ice::Variable *Var = Func->makeVariable(Ty, CurrentNode);
    LocalOperands.push_back(Var);
    return Var;
  }

  // Converts a relative index (to the next instruction to be read) to
  // an absolute value index.
  uint32_t convertRelativeToAbsIndex(int32_t Id) {
    int32_t AbsNextId = CachedNumGlobalValueIDs + LocalOperands.size();
    if (Id > 0 && AbsNextId < static_cast<uint32_t>(Id)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Invalid relative value id: " << Id
             << " (must be <= " << AbsNextId << ")";
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      return 0;
    }
    return AbsNextId - Id;
  }

  // Returns the value referenced by the given value Index.
  Ice::Operand *getOperand(uint32_t Index) {
    if (Index < CachedNumGlobalValueIDs) {
      // TODO(kschimpf): Define implementation.
      report_fatal_error("getOperand of global addresses not implemented");
    }
    uint32_t LocalIndex = Index - CachedNumGlobalValueIDs;
    if (LocalIndex >= LocalOperands.size()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Value index " << Index << " out of range. Must be less than "
             << (LocalOperands.size() + CachedNumGlobalValueIDs);
      Error(StrBuf.str());
      report_fatal_error("Unable to continue");
    }
    return LocalOperands[LocalIndex];
  }

  // Generates type error message for binary operator Op
  // operating on Type OpTy.
  void ReportInvalidBinaryOp(Ice::InstArithmetic::OpKind Op, Ice::Type OpTy);

  // Validates if integer logical Op, for type OpTy, is valid.
  // Returns true if valid. Otherwise generates error message and
  // returns false.
  bool isValidIntegerLogicalOp(Ice::InstArithmetic::OpKind Op, Ice::Type OpTy) {
    if (Ice::isIntegerType(OpTy))
      return true;
    ReportInvalidBinaryOp(Op, OpTy);
    return false;
  }

  // Validates if integer (or vector of integers) arithmetic Op, for type
  // OpTy, is valid.  Returns true if valid. Otherwise generates
  // error message and returns false.
  bool isValidIntegerArithOp(Ice::InstArithmetic::OpKind Op, Ice::Type OpTy) {
    if (Ice::isIntegerArithmeticType(OpTy))
      return true;
    ReportInvalidBinaryOp(Op, OpTy);
    return false;
  }

  // Checks if floating arithmetic Op, for type OpTy, is valid.
  // Returns false if valid. Otherwise generates an error message and
  // returns true.
  bool isValidFloatingArithOp(Ice::InstArithmetic::OpKind Op, Ice::Type OpTy) {
    if (Ice::isFloatingType(OpTy))
      return true;
    ReportInvalidBinaryOp(Op, OpTy);
    return false;
  }

  // Reports that the given binary Opcode, for the given type Ty,
  // is not understood.
  void ReportInvalidBinopOpcode(unsigned Opcode, Ice::Type Ty);

  // Takes the PNaCl bitcode binary operator Opcode, and the opcode
  // type Ty, and sets Op to the corresponding ICE binary
  // opcode. Returns true if able to convert, false otherwise.
  bool convertBinopOpcode(unsigned Opcode, Ice::Type Ty,
                          Ice::InstArithmetic::OpKind &Op) {
    Instruction::BinaryOps LLVMOpcode;
    if (!naclbitc::DecodeBinaryOpcode(Opcode, Context->convertToLLVMType(Ty),
                                      LLVMOpcode)) {
      ReportInvalidBinopOpcode(Opcode, Ty);
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Op = Ice::InstArithmetic::Add;
      return false;
    }
    switch (LLVMOpcode) {
    default: {
      ReportInvalidBinopOpcode(Opcode, Ty);
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Op = Ice::InstArithmetic::Add;
      return false;
    }
    case Instruction::Add:
      Op = Ice::InstArithmetic::Add;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::FAdd:
      Op = Ice::InstArithmetic::Fadd;
      return isValidFloatingArithOp(Op, Ty);
    case Instruction::Sub:
      Op = Ice::InstArithmetic::Sub;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::FSub:
      Op = Ice::InstArithmetic::Fsub;
      return isValidFloatingArithOp(Op, Ty);
    case Instruction::Mul:
      Op = Ice::InstArithmetic::Mul;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::FMul:
      Op = Ice::InstArithmetic::Fmul;
      return isValidFloatingArithOp(Op, Ty);
    case Instruction::UDiv:
      Op = Ice::InstArithmetic::Udiv;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::SDiv:
      Op = Ice::InstArithmetic::Sdiv;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::FDiv:
      Op = Ice::InstArithmetic::Fdiv;
      return isValidFloatingArithOp(Op, Ty);
    case Instruction::URem:
      Op = Ice::InstArithmetic::Urem;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::SRem:
      Op = Ice::InstArithmetic::Srem;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::FRem:
      Op = Ice::InstArithmetic::Frem;
      return isValidFloatingArithOp(Op, Ty);
    case Instruction::Shl:
      Op = Ice::InstArithmetic::Shl;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::LShr:
      Op = Ice::InstArithmetic::Lshr;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::AShr:
      Op = Ice::InstArithmetic::Ashr;
      return isValidIntegerArithOp(Op, Ty);
    case Instruction::And:
      Op = Ice::InstArithmetic::And;
      return isValidIntegerLogicalOp(Op, Ty);
    case Instruction::Or:
      Op = Ice::InstArithmetic::Or;
      return isValidIntegerLogicalOp(Op, Ty);
    case Instruction::Xor:
      Op = Ice::InstArithmetic::Xor;
      return isValidIntegerLogicalOp(Op, Ty);
    }
  }
};

FunctionParser::~FunctionParser() {
  if (getTranslator().getFlags().SubzeroTimingEnabled) {
    errs() << "[Subzero timing] Convert function " << Func->getFunctionName()
           << ": " << TConvert.getElapsedSec() << " sec\n";
  }
}

void FunctionParser::ReportInvalidBinopOpcode(unsigned Opcode, Ice::Type Ty) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Binary opcode " << Opcode << "not understood for type " << Ty;
  Error(StrBuf.str());
}

void FunctionParser::ExitBlock() {
  // Before translating, check for blocks without instructions, and
  // insert unreachable. This shouldn't happen, but be safe.
  unsigned Index = 0;
  const Ice::NodeList &Nodes = Func->getNodes();
  for (std::vector<Ice::CfgNode *>::const_iterator Iter = Nodes.begin(),
                                                   IterEnd = Nodes.end();
       Iter != IterEnd; ++Iter, ++Index) {
    Ice::CfgNode *Node = *Iter;
    if (Node->getInsts().size() == 0) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Basic block " << Index << " contains no instructions";
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Node->appendInst(Ice::InstUnreachable::create(Func));
    }
  }
  getTranslator().translateFcn(Func);
}

void FunctionParser::ReportInvalidBinaryOp(Ice::InstArithmetic::OpKind Op,
                                           Ice::Type OpTy) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Invalid operator type for " << Ice::InstArithmetic::getOpName(Op)
         << ". Found " << OpTy;
  Error(StrBuf.str());
}

void FunctionParser::ProcessRecord() {
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  if (InstIsTerminating) {
    InstIsTerminating = false;
    CurrentNode = GetBasicBlock(++CurrentBbIndex);
  }
  Ice::Inst *Inst = NULL;
  switch (Record.GetCode()) {
  case naclbitc::FUNC_CODE_DECLAREBLOCKS: {
    // DECLAREBLOCKS: [n]
    if (!isValidRecordSize(1, "function block count"))
      break;
    if (Func->getNodes().size() != 1) {
      Error("Duplicate function block count record");
      return;
    }
    uint32_t NumBbs = Values[0];
    if (NumBbs == 0) {
      Error("Functions must contain at least one basic block.");
      // TODO(kschimpf) Remove error recovery once implementation complete.
      NumBbs = 1;
    }
    // Install the basic blocks, skipping bb0 which was created in the
    // constructor.
    for (size_t i = 1; i < NumBbs; ++i)
      InstallNextBasicBlock();
    break;
  }
  case naclbitc::FUNC_CODE_INST_BINOP: {
    // BINOP: [opval, opval, opcode]
    if (!isValidRecordSize(3, "function block binop"))
      break;
    Ice::Operand *Op1 = getOperand(convertRelativeToAbsIndex(Values[0]));
    Ice::Operand *Op2 = getOperand(convertRelativeToAbsIndex(Values[1]));
    Ice::Type Type1 = Op1->getType();
    Ice::Type Type2 = Op2->getType();
    if (Type1 != Type2) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Binop argument types differ: " << Type1 << " and " << Type2;
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Op2 = Op1;
    }

    Ice::InstArithmetic::OpKind Opcode;
    if (!convertBinopOpcode(Values[2], Type1, Opcode))
      break;
    Ice::Variable *Dest = NextInstVar(Type1);
    Inst = Ice::InstArithmetic::create(Func, Opcode, Dest, Op1, Op2);
    break;
  }
  case naclbitc::FUNC_CODE_INST_RET: {
    // RET: [opval?]
    InstIsTerminating = true;
    if (!isValidRecordSizeInRange(0, 1, "function block ret"))
      break;
    if (Values.size() == 0) {
      Inst = Ice::InstRet::create(Func);
    } else {
      Inst = Ice::InstRet::create(
          Func, getOperand(convertRelativeToAbsIndex(Values[0])));
    }
    break;
  }
  default:
    // Generate error message!
    BlockParserBaseClass::ProcessRecord();
    break;
  }
  if (Inst)
    CurrentNode->appendInst(Inst);
}

/// Parses the module block in the bitcode file.
class ModuleParser : public BlockParserBaseClass {
public:
  ModuleParser(unsigned BlockID, TopLevelParser *Context)
      : BlockParserBaseClass(BlockID, Context) {}

  virtual ~ModuleParser() LLVM_OVERRIDE {}

protected:
  virtual bool ParseBlock(unsigned BlockID) LLVM_OVERRIDE;

  virtual void ProcessRecord() LLVM_OVERRIDE;
};

bool ModuleParser::ParseBlock(unsigned BlockID) LLVM_OVERRIDE {
  switch (BlockID) {
  case naclbitc::BLOCKINFO_BLOCK_ID:
    return NaClBitcodeParser::ParseBlock(BlockID);
  case naclbitc::TYPE_BLOCK_ID_NEW: {
    TypesParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  case naclbitc::GLOBALVAR_BLOCK_ID: {
    GlobalsParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  case naclbitc::VALUE_SYMTAB_BLOCK_ID: {
    ValuesymtabParser Parser(BlockID, this, false);
    return Parser.ParseThisBlock();
  }
  case naclbitc::FUNCTION_BLOCK_ID: {
    FunctionParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  default:
    return BlockParserBaseClass::ParseBlock(BlockID);
  }
}

void ModuleParser::ProcessRecord() {
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  switch (Record.GetCode()) {
  case naclbitc::MODULE_CODE_VERSION: {
    // VERSION: [version#]
    if (!isValidRecordSize(1, "Module version"))
      return;
    unsigned Version = Values[0];
    if (Version != 1) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Unknown bitstream version: " << Version;
      Error(StrBuf.str());
    }
    return;
  }
  case naclbitc::MODULE_CODE_FUNCTION: {
    // FUNCTION:  [type, callingconv, isproto, linkage]
    if (!isValidRecordSize(4, "Function heading"))
      return;
    Type *Ty = Context->getTypeByID(Values[0]);
    FunctionType *FTy = dyn_cast<FunctionType>(Ty);
    if (FTy == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Function heading expects function type. Found: " << Ty;
      Error(StrBuf.str());
      return;
    }
    CallingConv::ID CallingConv;
    if (!naclbitc::DecodeCallingConv(Values[1], CallingConv)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Function heading has unknown calling convention: "
             << Values[1];
      Error(StrBuf.str());
      return;
    }
    GlobalValue::LinkageTypes Linkage;
    if (!naclbitc::DecodeLinkage(Values[3], Linkage)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Function heading has unknown linkage. Found " << Values[3];
      Error(StrBuf.str());
      return;
    }
    Function *Func = Function::Create(FTy, Linkage, "", Context->getModule());
    Func->setCallingConv(CallingConv);
    if (Values[2] == 0)
      Context->setNextValueIDAsImplementedFunction();
    Context->setNextFunctionID(Func);
    // TODO(kschimpf) verify if Func matches PNaCl ABI.
    return;
  }
  default:
    BlockParserBaseClass::ProcessRecord();
    return;
  }
}

bool TopLevelParser::ParseBlock(unsigned BlockID) {
  if (BlockID == naclbitc::MODULE_BLOCK_ID) {
    ModuleParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  // Generate error message by using default block implementation.
  BlockParserBaseClass Parser(BlockID, this);
  return Parser.ParseThisBlock();
}

} // end of anonymous namespace

namespace Ice {

void PNaClTranslator::translate(const std::string &IRFilename) {
  OwningPtr<MemoryBuffer> MemBuf;
  if (error_code ec =
          MemoryBuffer::getFileOrSTDIN(IRFilename.c_str(), MemBuf)) {
    errs() << "Error reading '" << IRFilename << "': " << ec.message() << "\n";
    ErrorStatus = true;
    return;
  }

  if (MemBuf->getBufferSize() % 4 != 0) {
    errs() << IRFilename
           << ": Bitcode stream should be a multiple of 4 bytes in length.\n";
    ErrorStatus = true;
    return;
  }

  const unsigned char *BufPtr = (const unsigned char *)MemBuf->getBufferStart();
  const unsigned char *EndBufPtr = BufPtr + MemBuf->getBufferSize();

  // Read header and verify it is good.
  NaClBitcodeHeader Header;
  if (Header.Read(BufPtr, EndBufPtr) || !Header.IsSupported()) {
    errs() << "Invalid PNaCl bitcode header.\n";
    ErrorStatus = true;
    return;
  }

  // Create a bitstream reader to read the bitcode file.
  NaClBitstreamReader InputStreamFile(BufPtr, EndBufPtr);
  NaClBitstreamCursor InputStream(InputStreamFile);

  TopLevelParser Parser(*this, MemBuf->getBufferIdentifier(), Header,
                        InputStream, ErrorStatus);
  int TopLevelBlocks = 0;
  while (!InputStream.AtEndOfStream()) {
    if (Parser.Parse()) {
      ErrorStatus = true;
      return;
    }
    ++TopLevelBlocks;
  }

  if (TopLevelBlocks != 1) {
    errs() << IRFilename
           << ": Contains more than one module. Found: " << TopLevelBlocks
           << "\n";
    ErrorStatus = true;
  }
  return;
}

} // end of namespace Ice
