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

#include <cassert>
#include <memory>
#include <vector>

#include "llvm/Analysis/NaCl/PNaClABIProps.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeDecoders.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeHeader.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeParser.h"
#include "llvm/Bitcode/NaCl/NaClReaderWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ValueHandle.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTypeConverter.h"
#include "PNaClTranslator.h"

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
        Mod(new Module(InputName, getGlobalContext())), DL(PNaClDataLayout),
        Header(Header), TypeConverter(getLLVMContext()),
        ErrorStatus(ErrorStatus), NumErrors(0), NumFunctionIds(0),
        NumFunctionBlocks(0),
        GlobalVarPlaceHolderType(convertToLLVMType(Ice::IceType_i8)) {
    Mod->setDataLayout(PNaClDataLayout);
    setErrStream(Translator.getContext()->getStrDump());
  }

  ~TopLevelParser() override {}

  Ice::Translator &getTranslator() { return Translator; }

  // Generates error with given Message. Always returns true.
  bool Error(const std::string &Message) override {
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

  const DataLayout &getDataLayout() const { return DL; }

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

  /// Returns the corresponding constant associated with a global value
  /// (i.e. relocatable).
  Ice::Constant *getOrCreateGlobalConstantByID(unsigned ID) {
    // TODO(kschimpf): Can this be built when creating global initializers?
    if (ID >= ValueIDConstants.size()) {
      if (ID >= ValueIDValues.size())
        return NULL;
      ValueIDConstants.resize(ValueIDValues.size());
    }
    Ice::Constant *C = ValueIDConstants[ID];
    if (C != NULL)
      return C;
    Value *V = ValueIDValues[ID];
    assert(isa<GlobalValue>(V));
    C = getTranslator().getContext()->getConstantSym(getIcePointerType(), 0,
                                                     V->getName());
    ValueIDConstants[ID] = C;
    return C;
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
  std::unique_ptr<Module> Mod;
  // The data layout to use.
  DataLayout DL;
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
  // Relocatable constants associated with ValueIDValues.
  std::vector<Ice::Constant *> ValueIDConstants;
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

  bool ParseBlock(unsigned BlockID) override;

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

  ~BlockParserBaseClass() override {}

protected:
  // The context parser that contains the decoded state.
  TopLevelParser *Context;

  // Constructor for nested block parsers.
  BlockParserBaseClass(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : NaClBitcodeParser(BlockID, EnclosingParser),
        Context(EnclosingParser->Context) {}

  // Gets the translator associated with the bitcode parser.
  Ice::Translator &getTranslator() const { return Context->getTranslator(); }

  const Ice::ClFlags &getFlags() const { return getTranslator().getFlags(); }

  // Generates an error Message with the bit address prefixed to it.
  bool Error(const std::string &Message) override {
    uint64_t Bit = Record.GetStartBit() + Context->getHeaderSize() * 8;
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "(" << format("%" PRIu64 ":%u", (Bit / 8),
                            static_cast<unsigned>(Bit % 8)) << ") " << Message;
    return Context->Error(StrBuf.str());
  }

  // Default implementation. Reports that block is unknown and skips
  // its contents.
  bool ParseBlock(unsigned BlockID) override;

  // Default implementation. Reports that the record is not
  // understood.
  void ProcessRecord() override;

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

  ~TypesParser() override {}

private:
  // The type ID that will be associated with the next type defining
  // record in the types block.
  unsigned NextTypeId;

  void ProcessRecord() override;
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

  ~GlobalsParser() override {}

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

  void ExitBlock() override {
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

  void ProcessRecord() override;

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

/// Base class for parsing a valuesymtab block in the bitcode file.
class ValuesymtabParser : public BlockParserBaseClass {
  ValuesymtabParser(const ValuesymtabParser &) LLVM_DELETED_FUNCTION;
  void operator=(const ValuesymtabParser &) LLVM_DELETED_FUNCTION;

public:
  ValuesymtabParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : BlockParserBaseClass(BlockID, EnclosingParser) {}

  ~ValuesymtabParser() override {}

protected:
  typedef SmallString<128> StringType;

  // Associates Name with the value defined by the given Index.
  virtual void setValueName(uint64_t Index, StringType &Name) = 0;

  // Associates Name with the value defined by the given Index;
  virtual void setBbName(uint64_t Index, StringType &Name) = 0;

private:

  void ProcessRecord() override;

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
    setValueName(Values[0], ConvertedName);
    return;
  }
  case naclbitc::VST_CODE_BBENTRY: {
    // VST_BBENTRY: [BbId, namechar x N]
    if (!isValidRecordSizeAtLeast(2, "Valuesymtab basic block entry"))
      return;
    ConvertToString(ConvertedName);
    setBbName(Values[0], ConvertedName);
    return;
  }
  default:
    break;
  }
  // If reached, don't know how to handle record.
  BlockParserBaseClass::ProcessRecord();
  return;
}

class FunctionValuesymtabParser;

/// Parses function blocks in the bitcode file.
class FunctionParser : public BlockParserBaseClass {
  FunctionParser(const FunctionParser &) LLVM_DELETED_FUNCTION;
  FunctionParser &operator=(const FunctionParser &) LLVM_DELETED_FUNCTION;
  friend class FunctionValuesymtabParser;

public:
  FunctionParser(unsigned BlockID, BlockParserBaseClass *EnclosingParser)
      : BlockParserBaseClass(BlockID, EnclosingParser),
        Func(new Ice::Cfg(getTranslator().getContext())), CurrentBbIndex(0),
        FcnId(Context->getNextFunctionBlockValueID()),
        LLVMFunc(cast<Function>(Context->getGlobalValueByID(FcnId))),
        CachedNumGlobalValueIDs(Context->getNumGlobalValueIDs()),
        NextLocalInstIndex(Context->getNumGlobalValueIDs()),
        InstIsTerminating(false) {
    Func->setFunctionName(LLVMFunc->getName());
    Func->setReturnType(Context->convertToIceType(LLVMFunc->getReturnType()));
    Func->setInternal(LLVMFunc->hasInternalLinkage());
    CurrentNode = InstallNextBasicBlock();
    Func->setEntryNode(CurrentNode);
    for (Function::const_arg_iterator ArgI = LLVMFunc->arg_begin(),
                                      ArgE = LLVMFunc->arg_end();
         ArgI != ArgE; ++ArgI) {
      Func->addArg(getNextInstVar(Context->convertToIceType(ArgI->getType())));
    }
  }

  ~FunctionParser() override {};

  // Set the next constant ID to the given constant C.
  void setNextConstantID(Ice::Constant *C) {
    setOperand(NextLocalInstIndex++, C);
  }

private:
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
  // Holds the dividing point between local and global absolute value indices.
  uint32_t CachedNumGlobalValueIDs;
  // Holds operands local to the function block, based on indices
  // defined in the bitcode file.
  std::vector<Ice::Operand *> LocalOperands;
  // Holds the index within LocalOperands corresponding to the next
  // instruction that generates a value.
  uint32_t NextLocalInstIndex;
  // True if the last processed instruction was a terminating
  // instruction.
  bool InstIsTerminating;
  // Upper limit of alignment power allowed by LLVM
  static const uint64_t AlignPowerLimit = 29;

  // Extracts the corresponding Alignment to use, given the AlignPower
  // (i.e. 2**AlignPower, or 0 if AlignPower == 0). InstName is the
  // name of the instruction the alignment appears in.
  void extractAlignment(const char *InstName, uint64_t AlignPower,
                        unsigned &Alignment) {
    if (AlignPower <= AlignPowerLimit) {
      Alignment = (1 << static_cast<unsigned>(AlignPower)) >> 1;
      return;
    }
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << InstName << " alignment greater than 2**" << AlignPowerLimit
           << ". Found: 2**" << AlignPower;
    Error(StrBuf.str());
    // Error recover with value that is always acceptable.
    Alignment = 1;
  }

  bool ParseBlock(unsigned BlockID) override;

  void ProcessRecord() override;

  void ExitBlock() override;

  // Creates and appends a new basic block to the list of basic blocks.
  Ice::CfgNode *InstallNextBasicBlock() { return Func->makeNode(); }

  // Returns the Index-th basic block in the list of basic blocks.
  Ice::CfgNode *getBasicBlock(uint32_t Index) {
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

  // Returns the Index-th basic block in the list of basic blocks.
  // Assumes Index corresponds to a branch instruction. Hence, if
  // the branch references the entry block, it also generates a
  // corresponding error.
  Ice::CfgNode *getBranchBasicBlock(uint32_t Index) {
    if (Index == 0) {
      Error("Branch to entry block not allowed");
      // TODO(kschimpf) Remove error recovery once implementation complete.
    }
    return getBasicBlock(Index);
  }

  // Generate an instruction variable with type Ty.
  Ice::Variable *createInstVar(Ice::Type Ty) {
    if (Ty == Ice::IceType_void) {
      Error("Can't define instruction value using type void");
      // Recover since we can't throw an exception.
      Ty = Ice::IceType_i32;
    }
    return Func->makeVariable(Ty);
  }

  // Generates the next available local variable using the given type.
  Ice::Variable *getNextInstVar(Ice::Type Ty) {
    assert(NextLocalInstIndex >= CachedNumGlobalValueIDs);
    // Before creating one, see if a forwardtyperef has already defined it.
    uint32_t LocalIndex = NextLocalInstIndex - CachedNumGlobalValueIDs;
    if (LocalIndex < LocalOperands.size()) {
      Ice::Operand *Op = LocalOperands[LocalIndex];
      if (Op != NULL) {
        if (Ice::Variable *Var = dyn_cast<Ice::Variable>(Op)) {
          if (Var->getType() == Ty) {
            ++NextLocalInstIndex;
            return Var;
          }
        }
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Illegal forward referenced instruction ("
               << NextLocalInstIndex << "): " << *Op;
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        ++NextLocalInstIndex;
        return createInstVar(Ty);
      }
    }
    Ice::Variable *Var = createInstVar(Ty);
    setOperand(NextLocalInstIndex++, Var);
    return Var;
  }

  // Converts a relative index (wrt to BaseIndex) to an absolute value
  // index.
  uint32_t convertRelativeToAbsIndex(int32_t Id, int32_t BaseIndex) {
    if (BaseIndex < Id) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Invalid relative value id: " << Id
             << " (must be <= " << BaseIndex << ")";
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      return 0;
    }
    return BaseIndex - Id;
  }

  // Returns the value referenced by the given value Index.
  Ice::Operand *getOperand(uint32_t Index) {
    if (Index < CachedNumGlobalValueIDs) {
      return Context->getOrCreateGlobalConstantByID(Index);
    }
    uint32_t LocalIndex = Index - CachedNumGlobalValueIDs;
    if (LocalIndex >= LocalOperands.size()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Value index " << Index << " not defined!";
      Error(StrBuf.str());
      report_fatal_error("Unable to continue");
    }
    Ice::Operand *Op = LocalOperands[LocalIndex];
    if (Op == NULL) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Value index " << Index << " not defined!";
      Error(StrBuf.str());
      report_fatal_error("Unable to continue");
    }
    return Op;
  }

  // Sets element Index (in the local operands list) to Op.
  void setOperand(uint32_t Index, Ice::Operand *Op) {
    assert(Op);
    // Check if simple push works.
    uint32_t LocalIndex = Index - CachedNumGlobalValueIDs;
    if (LocalIndex == LocalOperands.size()) {
      LocalOperands.push_back(Op);
      return;
    }

    // Must be forward reference, expand vector to accommodate.
    if (LocalIndex >= LocalOperands.size())
      LocalOperands.resize(LocalIndex + 1);

    // If element not defined, set it.
    Ice::Operand *OldOp = LocalOperands[LocalIndex];
    if (OldOp == NULL) {
      LocalOperands[LocalIndex] = Op;
      return;
    }

    // See if forward reference matches.
    if (OldOp == Op)
      return;

    // Error has occurred.
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "Multiple definitions for index " << Index << ": " << *Op
           << " and " << *OldOp;
    Error(StrBuf.str());
    // TODO(kschimpf) Remove error recovery once implementation complete.
    LocalOperands[LocalIndex] = Op;
  }

  // Returns the relative operand (wrt to BaseIndex) referenced by
  // the given value Index.
  Ice::Operand *getRelativeOperand(int32_t Index, int32_t BaseIndex) {
    return getOperand(convertRelativeToAbsIndex(Index, BaseIndex));
  }

  // Returns the absolute index of the next value generating instruction.
  uint32_t getNextInstIndex() const { return NextLocalInstIndex; }

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
  // Returns true if valid. Otherwise generates an error message and
  // returns false;
  bool isValidFloatingArithOp(Ice::InstArithmetic::OpKind Op, Ice::Type OpTy) {
    if (Ice::isFloatingType(OpTy))
      return true;
    ReportInvalidBinaryOp(Op, OpTy);
    return false;
  }

  // Checks if the type of operand Op is the valid pointer type, for
  // the given InstructionName. Returns true if valid. Otherwise
  // generates an error message and returns false.
  bool isValidPointerType(Ice::Operand *Op, const char *InstructionName) {
    Ice::Type PtrType = Context->getIcePointerType();
    if (Op->getType() == PtrType)
      return true;
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << InstructionName << " address not " << PtrType
           << ". Found: " << *Op;
    Error(StrBuf.str());
    return false;
  }

  // Checks if loading/storing a value of type Ty is allowed.
  // Returns true if Valid. Otherwise generates an error message and
  // returns false.
  bool isValidLoadStoreType(Ice::Type Ty, const char *InstructionName) {
    if (isLoadStoreType(Ty))
      return true;
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << InstructionName << " type not allowed: " << Ty << "*";
    Error(StrBuf.str());
    return false;
  }

  // Checks if loading/storing a value of type Ty is allowed for
  // the given Alignment. Otherwise generates an error message and
  // returns false.
  bool isValidLoadStoreAlignment(unsigned Alignment, Ice::Type Ty,
                                 const char *InstructionName) {
    if (!isValidLoadStoreType(Ty, InstructionName))
      return false;
    if (PNaClABIProps::isAllowedAlignment(&Context->getDataLayout(), Alignment,
                                          Context->convertToLLVMType(Ty)))
      return true;
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << InstructionName << " " << Ty << "*: not allowed for alignment "
           << Alignment;
    Error(StrBuf.str());
    return false;
  }

  // Reports that the given binary Opcode, for the given type Ty,
  // is not understood.
  void ReportInvalidBinopOpcode(unsigned Opcode, Ice::Type Ty);

  // Returns true if the Str begins with Prefix.
  bool isStringPrefix(Ice::IceString &Str, Ice::IceString &Prefix) {
    const size_t PrefixSize = Prefix.size();
    if (Str.size() < PrefixSize)
      return false;
    for (size_t i = 0; i < PrefixSize; ++i) {
      if (Str[i] != Prefix[i])
        return false;
    }
    return true;
  }

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

  /// Converts an LLVM cast opcode LLVMCastOp to the corresponding Ice
  /// cast opcode and assigns to CastKind. Returns true if successful,
  /// false otherwise.
  bool convertLLVMCastOpToIceOp(Instruction::CastOps LLVMCastOp,
                                Ice::InstCast::OpKind &CastKind) const {
    switch (LLVMCastOp) {
    case Instruction::ZExt:
      CastKind = Ice::InstCast::Zext;
      break;
    case Instruction::SExt:
      CastKind = Ice::InstCast::Sext;
      break;
    case Instruction::Trunc:
      CastKind = Ice::InstCast::Trunc;
      break;
    case Instruction::FPTrunc:
      CastKind = Ice::InstCast::Fptrunc;
      break;
    case Instruction::FPExt:
      CastKind = Ice::InstCast::Fpext;
      break;
    case Instruction::FPToSI:
      CastKind = Ice::InstCast::Fptosi;
      break;
    case Instruction::FPToUI:
      CastKind = Ice::InstCast::Fptoui;
      break;
    case Instruction::SIToFP:
      CastKind = Ice::InstCast::Sitofp;
      break;
    case Instruction::UIToFP:
      CastKind = Ice::InstCast::Uitofp;
      break;
    case Instruction::BitCast:
      CastKind = Ice::InstCast::Bitcast;
      break;
    default:
      return false;
    }
    return true;
  }

  // Converts PNaCl bitcode Icmp operator to corresponding ICE op.
  // Returns true if able to convert, false otherwise.
  bool convertNaClBitcICmpOpToIce(uint64_t Op,
                                  Ice::InstIcmp::ICond &Cond) const {
    switch (Op) {
    case naclbitc::ICMP_EQ:
      Cond = Ice::InstIcmp::Eq;
      return true;
    case naclbitc::ICMP_NE:
      Cond = Ice::InstIcmp::Ne;
      return true;
    case naclbitc::ICMP_UGT:
      Cond = Ice::InstIcmp::Ugt;
      return true;
    case naclbitc::ICMP_UGE:
      Cond = Ice::InstIcmp::Uge;
      return true;
    case naclbitc::ICMP_ULT:
      Cond = Ice::InstIcmp::Ult;
      return true;
    case naclbitc::ICMP_ULE:
      Cond = Ice::InstIcmp::Ule;
      return true;
    case naclbitc::ICMP_SGT:
      Cond = Ice::InstIcmp::Sgt;
      return true;
    case naclbitc::ICMP_SGE:
      Cond = Ice::InstIcmp::Sge;
      return true;
    case naclbitc::ICMP_SLT:
      Cond = Ice::InstIcmp::Slt;
      return true;
    case naclbitc::ICMP_SLE:
      Cond = Ice::InstIcmp::Sle;
      return true;
    default:
      return false;
    }
  }

  // Converts PNaCl bitcode Fcmp operator to corresponding ICE op.
  // Returns true if able to convert, false otherwise.
  bool convertNaClBitcFCompOpToIce(uint64_t Op,
                                   Ice::InstFcmp::FCond &Cond) const {
    switch (Op) {
    case naclbitc::FCMP_FALSE:
      Cond = Ice::InstFcmp::False;
      return true;
    case naclbitc::FCMP_OEQ:
      Cond = Ice::InstFcmp::Oeq;
      return true;
    case naclbitc::FCMP_OGT:
      Cond = Ice::InstFcmp::Ogt;
      return true;
    case naclbitc::FCMP_OGE:
      Cond = Ice::InstFcmp::Oge;
      return true;
    case naclbitc::FCMP_OLT:
      Cond = Ice::InstFcmp::Olt;
      return true;
    case naclbitc::FCMP_OLE:
      Cond = Ice::InstFcmp::Ole;
      return true;
    case naclbitc::FCMP_ONE:
      Cond = Ice::InstFcmp::One;
      return true;
    case naclbitc::FCMP_ORD:
      Cond = Ice::InstFcmp::Ord;
      return true;
    case naclbitc::FCMP_UNO:
      Cond = Ice::InstFcmp::Uno;
      return true;
    case naclbitc::FCMP_UEQ:
      Cond = Ice::InstFcmp::Ueq;
      return true;
    case naclbitc::FCMP_UGT:
      Cond = Ice::InstFcmp::Ugt;
      return true;
    case naclbitc::FCMP_UGE:
      Cond = Ice::InstFcmp::Uge;
      return true;
    case naclbitc::FCMP_ULT:
      Cond = Ice::InstFcmp::Ult;
      return true;
    case naclbitc::FCMP_ULE:
      Cond = Ice::InstFcmp::Ule;
      return true;
    case naclbitc::FCMP_UNE:
      Cond = Ice::InstFcmp::Une;
      return true;
    case naclbitc::FCMP_TRUE:
      Cond = Ice::InstFcmp::True;
      return true;
    default:
      return false;
    }
  }
};

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
  Func->computePredecessors();
  // Note: Once any errors have been found, we turn off all
  // translation of all remaining functions. This allows use to see
  // multiple errors, without adding extra checks to the translator
  // for such parsing errors.
  if (Context->getNumErrors() == 0)
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
    CurrentNode = getBasicBlock(++CurrentBbIndex);
  }
  // The base index for relative indexing.
  int32_t BaseIndex = getNextInstIndex();
  switch (Record.GetCode()) {
  case naclbitc::FUNC_CODE_DECLAREBLOCKS: {
    // DECLAREBLOCKS: [n]
    if (!isValidRecordSize(1, "function block count"))
      return;
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
      return;
    Ice::Operand *Op1 = getRelativeOperand(Values[0], BaseIndex);
    Ice::Operand *Op2 = getRelativeOperand(Values[1], BaseIndex);
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
      return;
    CurrentNode->appendInst(Ice::InstArithmetic::create(
        Func, Opcode, getNextInstVar(Type1), Op1, Op2));
    break;
  }
  case naclbitc::FUNC_CODE_INST_CAST: {
    // CAST: [opval, destty, castopc]
    if (!isValidRecordSize(3, "function block cast"))
      return;
    Ice::Operand *Src = getRelativeOperand(Values[0], BaseIndex);
    Type *CastType = Context->getTypeByID(Values[1]);
    Instruction::CastOps LLVMCastOp;
    Ice::InstCast::OpKind CastKind;
    if (!naclbitc::DecodeCastOpcode(Values[2], LLVMCastOp) ||
        !convertLLVMCastOpToIceOp(LLVMCastOp, CastKind)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Cast opcode not understood: " << Values[2];
      Error(StrBuf.str());
      return;
    }
    Type *SrcType = Context->convertToLLVMType(Src->getType());
    if (!CastInst::castIsValid(LLVMCastOp, SrcType, CastType)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Illegal cast: " << Instruction::getOpcodeName(LLVMCastOp)
             << " " << *SrcType << " to " << *CastType;
      Error(StrBuf.str());
      return;
    }
    CurrentNode->appendInst(Ice::InstCast::create(
        Func, CastKind, getNextInstVar(Context->convertToIceType(CastType)),
        Src));
    break;
  }
  case naclbitc::FUNC_CODE_INST_VSELECT: {
    // VSELECT: [opval, opval, pred]
    Ice::Operand *ThenVal = getRelativeOperand(Values[0], BaseIndex);
    Ice::Type ThenType = ThenVal->getType();
    Ice::Operand *ElseVal = getRelativeOperand(Values[1], BaseIndex);
    Ice::Type ElseType = ElseVal->getType();
    if (ThenType != ElseType) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Select operands not same type. Found " << ThenType << " and "
             << ElseType;
      Error(StrBuf.str());
      return;
    }
    Ice::Operand *CondVal = getRelativeOperand(Values[2], BaseIndex);
    Ice::Type CondType = CondVal->getType();
    if (isVectorType(CondType)) {
      if (!isVectorType(ThenType) ||
          typeElementType(CondType) != Ice::IceType_i1 ||
          typeNumElements(ThenType) != typeNumElements(CondType)) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Select condition type " << CondType
               << " not allowed for values of type " << ThenType;
        Error(StrBuf.str());
        return;
      }
    } else if (CondVal->getType() != Ice::IceType_i1) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Select condition " << CondVal << " not type i1. Found: "
             << CondVal->getType();
      Error(StrBuf.str());
      return;
    }
    CurrentNode->appendInst(Ice::InstSelect::create(
        Func, getNextInstVar(ThenType), CondVal, ThenVal, ElseVal));
    break;
  }
  case naclbitc::FUNC_CODE_INST_EXTRACTELT: {
    // EXTRACTELT: [opval, opval]
    if (!isValidRecordSize(2, "function block extract element"))
      return;
    Ice::Operand *Vec = getRelativeOperand(Values[0], BaseIndex);
    Ice::Type VecType = Vec->getType();
    if (!Ice::isVectorType(VecType)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Extractelement not on vector. Found: " << *Vec;
      Error(StrBuf.str());
    }
    Ice::Operand *Index = getRelativeOperand(Values[1], BaseIndex);
    if (Index->getType() != Ice::IceType_i32) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Extractelement index " << *Index << " not i32. Found: "
             << Index->getType();
      Error(StrBuf.str());
    }
    // TODO(kschimpf): Restrict index to a legal constant index (once
    // constants can be defined).
    CurrentNode->appendInst(Ice::InstExtractElement::create(
        Func, getNextInstVar(typeElementType(VecType)), Vec, Index));
    break;
  }
  case naclbitc::FUNC_CODE_INST_INSERTELT: {
    // INSERTELT: [opval, opval, opval]
    if (!isValidRecordSize(3, "function block insert element"))
      return;
    Ice::Operand *Vec = getRelativeOperand(Values[0], BaseIndex);
    Ice::Type VecType = Vec->getType();
    if (!Ice::isVectorType(VecType)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Insertelement not on vector. Found: " << *Vec;
      Error(StrBuf.str());
    }
    Ice::Operand *Elt = getRelativeOperand(Values[1], BaseIndex);
    Ice::Type EltType = Elt->getType();
    if (EltType != typeElementType(VecType)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Insertelement element " << *Elt << " not type "
             << typeElementType(VecType)
             << ". Found: " << EltType;
      Error(StrBuf.str());
    }
    Ice::Operand *Index = getRelativeOperand(Values[2], BaseIndex);
    if (Index->getType() != Ice::IceType_i32) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Insertelement index " << *Index << " not i32. Found: "
             << Index->getType();
      Error(StrBuf.str());
    }
    // TODO(kschimpf): Restrict index to a legal constant index (once
    // constants can be defined).
    CurrentNode->appendInst(Ice::InstInsertElement::create(
        Func, getNextInstVar(VecType), Vec, Elt, Index));
    break;
  }
  case naclbitc::FUNC_CODE_INST_CMP2: {
    // CMP2: [opval, opval, pred]
    if (!isValidRecordSize(3, "function block compare"))
      return;
    Ice::Operand *Op1 = getRelativeOperand(Values[0], BaseIndex);
    Ice::Operand *Op2 = getRelativeOperand(Values[1], BaseIndex);
    Ice::Type Op1Type = Op1->getType();
    Ice::Type Op2Type = Op2->getType();
    if (Op1Type != Op2Type) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Compare argument types differ: " << Op1Type
             << " and " << Op2Type;
      Error(StrBuf.str());
      // TODO(kschimpf) Remove error recovery once implementation complete.
      Op2 = Op1;
    }
    Ice::Type DestType = getCompareResultType(Op1Type);
    if (DestType == Ice::IceType_void) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Compare not defined for type " << Op1Type;
      Error(StrBuf.str());
      return;
    }
    Ice::Variable *Dest = getNextInstVar(DestType);
    if (isIntegerType(Op1Type)) {
      Ice::InstIcmp::ICond Cond;
      if (!convertNaClBitcICmpOpToIce(Values[2], Cond)) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Compare record contains unknown integer predicate index: "
               << Values[2];
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        Cond = Ice::InstIcmp::Eq;
      }
      CurrentNode->appendInst(
          Ice::InstIcmp::create(Func, Cond, Dest, Op1, Op2));
    } else if (isFloatingType(Op1Type)){
      Ice::InstFcmp::FCond Cond;
      if (!convertNaClBitcFCompOpToIce(Values[2], Cond)) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Compare record contains unknown float predicate index: "
               << Values[2];
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        Cond = Ice::InstFcmp::False;
      }
      CurrentNode->appendInst(
          Ice::InstFcmp::create(Func, Cond, Dest, Op1, Op2));
    } else {
      // Not sure this can happen, but be safe.
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Compare on type not understood: " << Op1Type;
      Error(StrBuf.str());
      return;
    }
    break;
  }
  case naclbitc::FUNC_CODE_INST_RET: {
    // RET: [opval?]
    if (!isValidRecordSizeInRange(0, 1, "function block ret"))
      return;
    if (Values.size() == 0) {
      CurrentNode->appendInst(Ice::InstRet::create(Func));
    } else {
      CurrentNode->appendInst(
          Ice::InstRet::create(Func, getRelativeOperand(Values[0], BaseIndex)));
    }
    InstIsTerminating = true;
    break;
  }
  case naclbitc::FUNC_CODE_INST_BR: {
    if (Values.size() == 1) {
      // BR: [bb#]
      Ice::CfgNode *Block = getBranchBasicBlock(Values[0]);
      if (Block == NULL)
        return;
      CurrentNode->appendInst(Ice::InstBr::create(Func, Block));
    } else {
      // BR: [bb#, bb#, opval]
      if (!isValidRecordSize(3, "function block branch"))
        return;
      Ice::Operand *Cond = getRelativeOperand(Values[2], BaseIndex);
      if (Cond->getType() != Ice::IceType_i1) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Branch condition " << *Cond << " not i1. Found: "
               << Cond->getType();
        Error(StrBuf.str());
        return;
      }
      Ice::CfgNode *ThenBlock = getBranchBasicBlock(Values[0]);
      Ice::CfgNode *ElseBlock = getBranchBasicBlock(Values[1]);
      if (ThenBlock == NULL || ElseBlock == NULL)
        return;
      CurrentNode->appendInst(
          Ice::InstBr::create(Func, Cond, ThenBlock, ElseBlock));
    }
    InstIsTerminating = true;
    break;
  }
  case naclbitc::FUNC_CODE_INST_SWITCH: {
    // SWITCH: [Condty, Cond, BbIndex, NumCases Case ...]
    // where Case = [1, 1, Value, BbIndex].
    //
    // Note: Unlike most instructions, we don't infer the type of
    // Cond, but provide it as a separate field. There are also
    // unnecesary data fields (i.e. constants 1).  These were not
    // cleaned up in PNaCl bitcode because the bitcode format was
    // already frozen when the problem was noticed.
    if (!isValidRecordSizeAtLeast(4, "function block switch"))
      return;
    Ice::Type CondTy =
        Context->convertToIceType(Context->getTypeByID(Values[0]));
    if (!Ice::isScalarIntegerType(CondTy)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Case condition must be non-wide integer. Found: " << CondTy;
      Error(StrBuf.str());
      return;
    }
    Ice::SizeT BitWidth = Ice::getScalarIntBitWidth(CondTy);
    Ice::Operand *Cond = getRelativeOperand(Values[1], BaseIndex);
    if (CondTy != Cond->getType()) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Case condition expects type " << CondTy
             << ". Found: " << Cond->getType();
      Error(StrBuf.str());
      return;
    }
    Ice::CfgNode *DefaultLabel = getBranchBasicBlock(Values[2]);
    unsigned NumCases = Values[3];

    // Now recognize each of the cases.
    if (!isValidRecordSize(4 + NumCases * 4, "Function block switch"))
      return;
    Ice::InstSwitch *Switch =
        Ice::InstSwitch::create(Func, NumCases, Cond, DefaultLabel);
    unsigned ValCaseIndex = 4;  // index to beginning of case entry.
    for (unsigned CaseIndex = 0; CaseIndex < NumCases;
         ++CaseIndex, ValCaseIndex += 4) {
      if (Values[ValCaseIndex] != 1 || Values[ValCaseIndex+1] != 1) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Sequence [1, 1, value, label] expected for case entry "
               << "in switch record. (at index" << ValCaseIndex << ")";
        Error(StrBuf.str());
        return;
      }
      APInt Value(BitWidth,
                  NaClDecodeSignRotatedValue(Values[ValCaseIndex + 2]),
                  true);
      Ice::CfgNode *Label = getBranchBasicBlock(Values[ValCaseIndex + 3]);
      Switch->addBranch(CaseIndex, Value.getSExtValue(), Label);
    }
    CurrentNode->appendInst(Switch);
    InstIsTerminating = true;
    break;
  }
  case naclbitc::FUNC_CODE_INST_UNREACHABLE: {
    // UNREACHABLE: []
    if (!isValidRecordSize(0, "function block unreachable"))
      return;
    CurrentNode->appendInst(
        Ice::InstUnreachable::create(Func));
    InstIsTerminating = true;
    break;
  }
  case naclbitc::FUNC_CODE_INST_PHI: {
    // PHI: [ty, val1, bb1, ..., valN, bbN] for n >= 2.
    if (!isValidRecordSizeAtLeast(3, "function block phi"))
      return;
    if ((Values.size() & 0x1) == 0) {
      // Not an odd number of values.
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "function block phi record size not valid: " << Values.size();
      Error(StrBuf.str());
      return;
    }
    Ice::Type Ty = Context->convertToIceType(Context->getTypeByID(Values[0]));
    if (Ty == Ice::IceType_void) {
      Error("Phi record using type void not allowed");
      return;
    }
    Ice::Variable *Dest = getNextInstVar(Ty);
    Ice::InstPhi *Phi = Ice::InstPhi::create(Func, Values.size() >> 1, Dest);
    for (unsigned i = 1; i < Values.size(); i += 2) {
      Ice::Operand *Op =
          getRelativeOperand(NaClDecodeSignRotatedValue(Values[i]), BaseIndex);
      if (Op->getType() != Ty) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Value " << *Op << " not type " << Ty
               << " in phi instruction. Found: " << Op->getType();
        Error(StrBuf.str());
        return;
      }
      Phi->addArgument(Op, getBasicBlock(Values[i + 1]));
    }
    CurrentNode->appendInst(Phi);
    break;
  }
  case naclbitc::FUNC_CODE_INST_ALLOCA: {
    // ALLOCA: [Size, align]
    if (!isValidRecordSize(2, "function block alloca"))
      return;
    Ice::Operand *ByteCount = getRelativeOperand(Values[0], BaseIndex);
    if (ByteCount->getType() != Ice::IceType_i32) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Alloca on non-i32 value. Found: " << *ByteCount;
      Error(StrBuf.str());
      return;
    }
    unsigned Alignment;
    extractAlignment("Alloca", Values[1], Alignment);
    CurrentNode->appendInst(
        Ice::InstAlloca::create(Func, ByteCount, Alignment,
                                getNextInstVar(Context->getIcePointerType())));
    break;
  }
  case naclbitc::FUNC_CODE_INST_LOAD: {
    // LOAD: [address, align, ty]
    if (!isValidRecordSize(3, "function block load"))
      return;
    Ice::Operand *Address = getRelativeOperand(Values[0], BaseIndex);
    if (!isValidPointerType(Address, "Load"))
      return;
    unsigned Alignment;
    extractAlignment("Load", Values[1], Alignment);
    Ice::Type Ty = Context->convertToIceType(Context->getTypeByID(Values[2]));
    if (!isValidLoadStoreAlignment(Alignment, Ty, "Load"))
      return;
    CurrentNode->appendInst(
        Ice::InstLoad::create(Func, getNextInstVar(Ty), Address, Alignment));
    break;
  }
  case naclbitc::FUNC_CODE_INST_STORE: {
    // STORE: [address, value, align]
    if (!isValidRecordSize(3, "function block store"))
      return;
    Ice::Operand *Address = getRelativeOperand(Values[0], BaseIndex);
    if (!isValidPointerType(Address, "Store"))
      return;
    Ice::Operand *Value = getRelativeOperand(Values[1], BaseIndex);
    unsigned Alignment;
    extractAlignment("Store", Values[2], Alignment);
    if (!isValidLoadStoreAlignment(Alignment, Value->getType(), "Store"))
      return;
    CurrentNode->appendInst(
        Ice::InstStore::create(Func, Value, Address, Alignment));
    break;
  }
  case naclbitc::FUNC_CODE_INST_CALL:
  case naclbitc::FUNC_CODE_INST_CALL_INDIRECT: {
    // CALL: [cc, fnid, arg0, arg1...]
    // CALL_INDIRECT: [cc, fn, returnty, args...]
    //
    // Note: The difference between CALL and CALL_INDIRECT is that
    // CALL has an explicit function address, while the CALL_INDIRECT
    // is just an address. For CALL, we can infer the return type by
    // looking up the type signature associated with the function
    // address. For CALL_INDIRECT we can only infer the type signature
    // via argument types, and the corresponding return type stored in
    // CALL_INDIRECT record.
    Ice::SizeT ParamsStartIndex = 2;
    if (Record.GetCode() == naclbitc::FUNC_CODE_INST_CALL) {
      if (!isValidRecordSizeAtLeast(2, "function block call"))
        return;
    } else {
      if (!isValidRecordSizeAtLeast(3, "function block call indirect"))
        return;
      ParamsStartIndex = 3;
    }

    // Extract call information.
    uint64_t CCInfo = Values[0];
    CallingConv::ID CallingConv;
    if (!naclbitc::DecodeCallingConv(CCInfo >> 1, CallingConv)) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Function call calling convention value " << (CCInfo >> 1)
             << " not understood.";
      Error(StrBuf.str());
      return;
    }
    bool IsTailCall = static_cast<bool>(CCInfo & 1);

    // Extract out the called function and its return type.
    uint32_t CalleeIndex = convertRelativeToAbsIndex(Values[1], BaseIndex);
    Ice::Operand *Callee = getOperand(CalleeIndex);
    Ice::Type ReturnType = Ice::IceType_void;
    const Ice::Intrinsics::FullIntrinsicInfo *IntrinsicInfo = NULL;
    if (Record.GetCode() == naclbitc::FUNC_CODE_INST_CALL) {
      Function *Fcn =
          dyn_cast<Function>(Context->getGlobalValueByID(CalleeIndex));
      if (Fcn == NULL) {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Function call to non-function: " << *Callee;
        Error(StrBuf.str());
        return;
      }

      FunctionType *FcnTy = Fcn->getFunctionType();
      ReturnType = Context->convertToIceType(FcnTy->getReturnType());

      // Check if this direct call is to an Intrinsic (starts with "llvm.")
      static Ice::IceString LLVMPrefix("llvm.");
      Ice::IceString Name = Fcn->getName();
      if (isStringPrefix(Name, LLVMPrefix)) {
        Ice::IceString Suffix = Name.substr(LLVMPrefix.size());
        IntrinsicInfo =
            getTranslator().getContext()->getIntrinsicsInfo().find(Suffix);
        if (!IntrinsicInfo) {
          std::string Buffer;
          raw_string_ostream StrBuf(Buffer);
          StrBuf << "Invalid PNaCl intrinsic call to " << Name;
          Error(StrBuf.str());
          return;
        }
      }
    } else {
      ReturnType = Context->convertToIceType(Context->getTypeByID(Values[2]));
    }

    // Create the call instruction.
    Ice::Variable *Dest =
        (ReturnType == Ice::IceType_void) ? NULL : getNextInstVar(ReturnType);
    Ice::SizeT NumParams = Values.size() - ParamsStartIndex;
    Ice::InstCall *Inst = NULL;
    if (IntrinsicInfo) {
      Inst =
          Ice::InstIntrinsicCall::create(Func, NumParams, Dest, Callee,
                                         IntrinsicInfo->Info);
    } else {
      Inst = Ice::InstCall::create(Func, NumParams, Dest, Callee, IsTailCall);
    }

    // Add parameters.
    for (Ice::SizeT ParamIndex = 0; ParamIndex < NumParams; ++ParamIndex) {
      Inst->addArg(
          getRelativeOperand(Values[ParamsStartIndex + ParamIndex], BaseIndex));
    }

    // If intrinsic call, validate call signature.
    if (IntrinsicInfo) {
      Ice::SizeT ArgIndex = 0;
      switch (IntrinsicInfo->validateCall(Inst, ArgIndex)) {
      default:
        Error("Unknown validation error for intrinsic call");
        // TODO(kschimpf) Remove error recovery once implementation complete.
        break;
      case Ice::Intrinsics::IsValidCall:
        break;
      case Ice::Intrinsics::BadReturnType: {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Intrinsic call expects return type "
               << IntrinsicInfo->getReturnType()
               << ". Found: " << Inst->getReturnType();
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        break;
      }
      case Ice::Intrinsics::WrongNumOfArgs: {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Intrinsic call expects " << IntrinsicInfo->getNumArgs()
               << ". Found: " << Inst->getNumArgs();
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        break;
      }
      case Ice::Intrinsics::WrongCallArgType: {
        std::string Buffer;
        raw_string_ostream StrBuf(Buffer);
        StrBuf << "Intrinsic call argument " << ArgIndex << " expects type "
               << IntrinsicInfo->getArgType(ArgIndex)
               << ". Found: " << Inst->getArg(ArgIndex)->getType();
        Error(StrBuf.str());
        // TODO(kschimpf) Remove error recovery once implementation complete.
        break;
      }
      }
    }

    CurrentNode->appendInst(Inst);
    return;
  }
  case naclbitc::FUNC_CODE_INST_FORWARDTYPEREF: {
    // FORWARDTYPEREF: [opval, ty]
    if (!isValidRecordSize(2, "function block forward type ref"))
      return;
    setOperand(Values[0], createInstVar(Context->convertToIceType(
                              Context->getTypeByID(Values[1]))));
    break;
  }
  default:
    // Generate error message!
    BlockParserBaseClass::ProcessRecord();
    break;
  }
}

/// Parses constants within a function block.
class ConstantsParser : public BlockParserBaseClass {
  ConstantsParser(const ConstantsParser &) LLVM_DELETED_FUNCTION;
  ConstantsParser &operator=(const ConstantsParser &) LLVM_DELETED_FUNCTION;

public:
  ConstantsParser(unsigned BlockID, FunctionParser *FuncParser)
      : BlockParserBaseClass(BlockID, FuncParser), FuncParser(FuncParser),
        NextConstantType(Ice::IceType_void) {}

  ~ConstantsParser() override {}

private:
  // The parser of the function block this constants block appears in.
  FunctionParser *FuncParser;
  // The type to use for succeeding constants.
  Ice::Type NextConstantType;

  void ProcessRecord() override;

  Ice::GlobalContext *getContext() { return getTranslator().getContext(); }

  // Returns true if the type to use for succeeding constants is defined.
  // If false, also generates an error message.
  bool isValidNextConstantType() {
    if (NextConstantType != Ice::IceType_void)
      return true;
    Error("Constant record not preceded by set type record");
    return false;
  }
};

void ConstantsParser::ProcessRecord() {
  const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
  switch (Record.GetCode()) {
  case naclbitc::CST_CODE_SETTYPE: {
    // SETTYPE: [typeid]
    if (!isValidRecordSize(1, "constants block set type"))
      return;
    NextConstantType =
        Context->convertToIceType(Context->getTypeByID(Values[0]));
    if (NextConstantType == Ice::IceType_void)
      Error("constants block set type not allowed for void type");
    return;
  }
  case naclbitc::CST_CODE_UNDEF: {
    // UNDEF
    if (!isValidRecordSize(0, "constants block undef"))
      return;
    if (!isValidNextConstantType())
      return;
    FuncParser->setNextConstantID(
        getContext()->getConstantUndef(NextConstantType));
    return;
  }
  case naclbitc::CST_CODE_INTEGER: {
    // INTEGER: [intval]
    if (!isValidRecordSize(1, "constants block integer"))
      return;
    if (!isValidNextConstantType())
      return;
    if (IntegerType *IType = dyn_cast<IntegerType>(
            Context->convertToLLVMType(NextConstantType))) {
      APInt Value(IType->getBitWidth(), NaClDecodeSignRotatedValue(Values[0]));
      Ice::Constant *C = (NextConstantType == Ice::IceType_i64)
                             ? getContext()->getConstantInt64(
                                   NextConstantType, Value.getSExtValue())
                             : getContext()->getConstantInt32(
                                   NextConstantType, Value.getSExtValue());
      FuncParser->setNextConstantID(C);
      return;
    }
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "constant block integer record for non-integer type "
           << NextConstantType;
    Error(StrBuf.str());
    return;
  }
  case naclbitc::CST_CODE_FLOAT: {
    // FLOAT: [fpval]
    if (!isValidRecordSize(1, "constants block float"))
      return;
    if (!isValidNextConstantType())
      return;
    switch (NextConstantType) {
    case Ice::IceType_f32: {
      APFloat Value(APFloat::IEEEsingle,
                    APInt(32, static_cast<uint32_t>(Values[0])));
      FuncParser->setNextConstantID(
          getContext()->getConstantFloat(Value.convertToFloat()));
      return;
    }
    case Ice::IceType_f64: {
      APFloat Value(APFloat::IEEEdouble, APInt(64, Values[0]));
      FuncParser->setNextConstantID(
          getContext()->getConstantDouble(Value.convertToDouble()));
      return;
    }
    default: {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "constant block float record for non-floating type "
             << NextConstantType;
      Error(StrBuf.str());
      return;
    }
    }
  }
  default:
    // Generate error message!
    BlockParserBaseClass::ProcessRecord();
    return;
  }
}

// Parses valuesymtab blocks appearing in a function block.
class FunctionValuesymtabParser : public ValuesymtabParser {
  FunctionValuesymtabParser(const FunctionValuesymtabParser &)
      LLVM_DELETED_FUNCTION;
  void operator=(const FunctionValuesymtabParser &) LLVM_DELETED_FUNCTION;

public:
  FunctionValuesymtabParser(unsigned BlockID, FunctionParser *EnclosingParser)
      : ValuesymtabParser(BlockID, EnclosingParser) {}

private:
  // Returns the enclosing function parser.
  FunctionParser *getFunctionParser() const {
    return reinterpret_cast<FunctionParser *>(GetEnclosingParser());
  }

  void setValueName(uint64_t Index, StringType &Name) override;
  void setBbName(uint64_t Index, StringType &Name) override;

  // Reports that the assignment of Name to the value associated with
  // index is not possible, for the given Context.
  void reportUnableToAssign(const char *Context, uint64_t Index,
                            StringType &Name) {
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "Function-local " << Context << " name '" << Name
           << "' can't be associated with index " << Index;
    Error(StrBuf.str());
  }
};

void FunctionValuesymtabParser::setValueName(uint64_t Index, StringType &Name) {
  // Note: We check when Index is too small, so that we can error recover
  // (FP->getOperand will create fatal error).
  if (Index < getFunctionParser()->CachedNumGlobalValueIDs) {
    reportUnableToAssign("instruction", Index, Name);
    // TODO(kschimpf) Remove error recovery once implementation complete.
    return;
  }
  Ice::Operand *Op = getFunctionParser()->getOperand(Index);
  if (Ice::Variable *V = dyn_cast<Ice::Variable>(Op)) {
    std::string Nm(Name.data(), Name.size());
    V->setName(Nm);
  } else {
    reportUnableToAssign("variable", Index, Name);
  }
}

void FunctionValuesymtabParser::setBbName(uint64_t Index, StringType &Name) {
  if (Index >= getFunctionParser()->Func->getNumNodes()) {
    reportUnableToAssign("block", Index, Name);
    return;
  }
  std::string Nm(Name.data(), Name.size());
  getFunctionParser()->Func->getNodes()[Index]->setName(Nm);
}

bool FunctionParser::ParseBlock(unsigned BlockID) {
  switch (BlockID) {
  case naclbitc::CONSTANTS_BLOCK_ID: {
    ConstantsParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  case naclbitc::VALUE_SYMTAB_BLOCK_ID: {
    if (PNaClAllowLocalSymbolTables) {
      FunctionValuesymtabParser Parser(BlockID, this);
      return Parser.ParseThisBlock();
    }
    break;
  }
  default:
    break;
  }
  return BlockParserBaseClass::ParseBlock(BlockID);
}

/// Parses the module block in the bitcode file.
class ModuleParser : public BlockParserBaseClass {
public:
  ModuleParser(unsigned BlockID, TopLevelParser *Context)
      : BlockParserBaseClass(BlockID, Context),
        GlobalAddressNamesAndInitializersInstalled(false) {}

  ~ModuleParser() override {}

private:
  // True if we have already instaledl names for unnamed global addresses,
  // and generated global constant initializers.
  bool GlobalAddressNamesAndInitializersInstalled;

  // Temporary hack to generate names for unnamed global addresses,
  // and generate global constant initializers. May be called multiple
  // times. Only the first call will do the installation.
  // NOTE: Doesn't handle relocations for global constant initializers.
  void InstallGlobalAddressNamesAndInitializers() {
    if (!GlobalAddressNamesAndInitializersInstalled) {
      getTranslator().nameUnnamedGlobalAddresses(Context->getModule());
      if (!getFlags().DisableGlobals)
        getTranslator().convertGlobals(Context->getModule());
      GlobalAddressNamesAndInitializersInstalled = true;
    }
  }

  bool ParseBlock(unsigned BlockID) override;

  void ExitBlock() override {
    InstallGlobalAddressNamesAndInitializers();
    getTranslator().emitConstants();
  }

  void ProcessRecord() override;
};

class ModuleValuesymtabParser : public ValuesymtabParser {
  ModuleValuesymtabParser(const ModuleValuesymtabParser &)
      LLVM_DELETED_FUNCTION;
  void operator=(const ModuleValuesymtabParser &) LLVM_DELETED_FUNCTION;

public:
  ModuleValuesymtabParser(unsigned BlockID, ModuleParser *MP)
      : ValuesymtabParser(BlockID, MP) {}

  ~ModuleValuesymtabParser() override {}

private:
  void setValueName(uint64_t Index, StringType &Name) override;
  void setBbName(uint64_t Index, StringType &Name) override;
};

void ModuleValuesymtabParser::setValueName(uint64_t Index, StringType &Name) {
  Value *V = Context->getGlobalValueByID(Index);
  if (V == NULL) {
    std::string Buffer;
    raw_string_ostream StrBuf(Buffer);
    StrBuf << "Invalid global address ID in valuesymtab: " << Index;
    Error(StrBuf.str());
    return;
  }
  V->setName(StringRef(Name.data(), Name.size()));
}

void ModuleValuesymtabParser::setBbName(uint64_t Index, StringType &Name) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Can't define basic block name at global level: '" << Name
         << "' -> " << Index;
  Error(StrBuf.str());
}

bool ModuleParser::ParseBlock(unsigned BlockID) {
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
    ModuleValuesymtabParser Parser(BlockID, this);
    return Parser.ParseThisBlock();
  }
  case naclbitc::FUNCTION_BLOCK_ID: {
    InstallGlobalAddressNamesAndInitializers();
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
}

} // end of namespace Ice
