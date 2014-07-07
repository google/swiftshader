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

// Top-level class to read PNaCl bitcode files, and translate to ICE.
class TopLevelParser : public NaClBitcodeParser {
  TopLevelParser(const TopLevelParser &) LLVM_DELETED_FUNCTION;
  TopLevelParser &operator=(const TopLevelParser &) LLVM_DELETED_FUNCTION;

public:
  TopLevelParser(const std::string &InputName, NaClBitcodeHeader &Header,
                 NaClBitstreamCursor &Cursor, int &ExitStatusFlag)
      : NaClBitcodeParser(Cursor),
        Mod(new Module(InputName, getGlobalContext())), Header(Header),
        ExitStatusFlag(ExitStatusFlag), NumErrors(0), NumFunctionIds(0),
        GlobalVarPlaceHolderType(Type::getInt8Ty(getLLVMContext())) {
    Mod->setDataLayout(PNaClDataLayout);
  }

  virtual ~TopLevelParser() {}
  LLVM_OVERRIDE;

  virtual bool Error(const std::string &Message) LLVM_OVERRIDE {
    ExitStatusFlag = 1;
    ++NumErrors;
    return NaClBitcodeParser::Error(Message);
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

  /// Returns the LLVM IR value associatd with the global value ID.
  Value *getGlobalValueByID(unsigned ID) const {
    if (ID >= ValueIDValues.size())
      return 0;
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
      return 0;
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
    if (OldV == 0) {
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

private:
  // The parsed module.
  OwningPtr<Module> Mod;
  // The bitcode header.
  NaClBitcodeHeader &Header;
  // The exit status flag that should be set to 1 if an error occurs.
  int &ExitStatusFlag;
  // The number of errors reported.
  unsigned NumErrors;
  // The types associated with each type ID.
  std::vector<Type *> TypeIDValues;
  // The (global) value IDs.
  std::vector<WeakVH> ValueIDValues;
  // The number of function IDs.
  unsigned NumFunctionIds;
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
};

Type *TopLevelParser::reportTypeIDAsUndefined(unsigned ID) {
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Can't find type for type id: " << ID;
  Error(StrBuf.str());
  Type *Ty = Type::getVoidTy(getLLVMContext());
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

  /// Checks if the size of the record is Size. If not, an error is
  /// produced using the given RecordName. Return true if error was
  /// reported. Otherwise false.
  bool checkRecordSize(unsigned Size, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() != Size) {
      return RecordSizeError(Size, RecordName, 0);
    }
    return false;
  }

  /// Checks if the size of the record is at least as large as the
  /// LowerLimit.
  bool checkRecordSizeAtLeast(unsigned LowerLimit, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() < LowerLimit) {
      return RecordSizeError(LowerLimit, RecordName, "at least");
    }
    return false;
  }

  /// Checks if the size of the record is no larger than the
  /// UpperLimit.
  bool checkRecordSizeNoMoreThan(unsigned UpperLimit, const char *RecordName) {
    const NaClBitcodeRecord::RecordVector &Values = Record.GetValues();
    if (Values.size() > UpperLimit) {
      return RecordSizeError(UpperLimit, RecordName, "no more than");
    }
    return false;
  }

  /// Checks if the size of the record is at least as large as the
  /// LowerLimit, and no larger than the UpperLimit.
  bool checkRecordSizeInRange(unsigned LowerLimit, unsigned UpperLimit,
                              const char *RecordName) {
    return checkRecordSizeAtLeast(LowerLimit, RecordName) ||
           checkRecordSizeNoMoreThan(UpperLimit, RecordName);
  }

private:
  /// Generates a record size error. ExpectedSize is the number
  /// of elements expected. RecordName is the name of the kind of
  /// record that has incorrect size. ContextMessage (if not 0)
  /// is appended to "record expects" to describe how ExpectedSize
  /// should be interpreted.
  bool RecordSizeError(unsigned ExpectedSize, const char *RecordName,
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
    return Error(StrBuf.str());
  }
};

bool BlockParserBaseClass::ParseBlock(unsigned BlockID) {
  // If called, derived class doesn't know how to handle block.
  // Report error and skip.
  std::string Buffer;
  raw_string_ostream StrBuf(Buffer);
  StrBuf << "Don't know how to parse block id: " << BlockID;
  Error(StrBuf.str());
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
    if (checkRecordSize(1, "Type count"))
      return;
    Context->resizeTypeIDValues(Values[0]);
    return;
  case naclbitc::TYPE_CODE_VOID:
    // VOID
    if (checkRecordSize(0, "Type void"))
      break;
    Ty = Type::getVoidTy(Context->getLLVMContext());
    break;
  case naclbitc::TYPE_CODE_FLOAT:
    // FLOAT
    if (checkRecordSize(0, "Type float"))
      break;
    Ty = Type::getFloatTy(Context->getLLVMContext());
    break;
  case naclbitc::TYPE_CODE_DOUBLE:
    // DOUBLE
    if (checkRecordSize(0, "Type double"))
      break;
    Ty = Type::getDoubleTy(Context->getLLVMContext());
    break;
  case naclbitc::TYPE_CODE_INTEGER:
    // INTEGER: [width]
    if (checkRecordSize(1, "Type integer"))
      break;
    Ty = IntegerType::get(Context->getLLVMContext(), Values[0]);
    // TODO(kschimpf) Check if size is legal.
    break;
  case naclbitc::TYPE_CODE_VECTOR:
    // VECTOR: [numelts, eltty]
    if (checkRecordSize(2, "Type vector"))
      break;
    Ty = VectorType::get(Context->getTypeByID(Values[1]), Values[0]);
    break;
  case naclbitc::TYPE_CODE_FUNCTION: {
    // FUNCTION: [vararg, retty, paramty x N]
    if (checkRecordSizeAtLeast(2, "Type signature"))
      break;
    SmallVector<Type *, 8> ArgTys;
    for (unsigned i = 2, e = Values.size(); i != e; ++i) {
      ArgTys.push_back(Context->getTypeByID(Values[i]));
    }
    Ty = FunctionType::get(Context->getTypeByID(Values[1]), ArgTys, Values[0]);
    break;
  }
  default:
    BlockParserBaseClass::ProcessRecord();
    break;
  }
  // If Ty not defined, assume error. Use void as filler.
  if (Ty == NULL)
    Ty = Type::getVoidTy(Context->getLLVMContext());
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
    if (checkRecordSize(1, "Globals count"))
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
    if (checkRecordSize(2, "Globals variable"))
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
    if (checkRecordSize(1, "globals compound"))
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
    if (checkRecordSize(1, "Globals zerofill"))
      return;
    reserveInitializer("Globals zerofill");
    Type *Ty =
        ArrayType::get(Type::getInt8Ty(Context->getLLVMContext()), Values[0]);
    Constant *Zero = ConstantAggregateZero::get(Ty);
    Initializers.push_back(Zero);
    break;
  }
  case naclbitc::GLOBALVAR_DATA: {
    // DATA: [b0, b1, ...]
    if (checkRecordSizeAtLeast(1, "Globals data"))
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
    if (checkRecordSizeInRange(1, 2, "Globals reloc"))
      return;
    Constant *BaseVal = Context->getOrCreateGlobalVarRef(Values[0]);
    if (BaseVal == 0) {
      std::string Buffer;
      raw_string_ostream StrBuf(Buffer);
      StrBuf << "Can't find global relocation value: " << Values[0];
      Error(StrBuf.str());
      return;
    }
    Type *IntPtrType = IntegerType::get(Context->getLLVMContext(), 32);
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
    if (checkRecordSizeAtLeast(2, "Valuesymtab value entry"))
      return;
    ConvertToString(ConvertedName);
    Value *V = Context->getGlobalValueByID(Values[0]);
    if (V == 0) {
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
    Error("Function block parser not yet implemented, skipping");
    SkipBlock();
    return false;
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
    if (checkRecordSize(1, "Module version"))
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
    if (checkRecordSize(4, "Function heading"))
      return;
    Type *Ty = Context->getTypeByID(Values[0]);
    FunctionType *FTy = dyn_cast<FunctionType>(Ty);
    if (FTy == 0) {
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
    bool ReturnValue = Parser.ParseThisBlock();
    // TODO(kschimpf): Remove once translating function blocks.
    errs() << "Global addresses:\n";
    for (size_t i = 0; i < ValueIDValues.size(); ++i) {
      errs() << "[" << i << "]: " << *ValueIDValues[i] << "\n";
    }
    return ReturnValue;
  }
  // Generate error message by using default block implementation.
  BlockParserBaseClass Parser(BlockID, this);
  return Parser.ParseThisBlock();
}

} // end of anonymous namespace.

namespace Ice {

void PNaClTranslator::translate(const std::string &IRFilename) {
  OwningPtr<MemoryBuffer> MemBuf;
  if (error_code ec =
          MemoryBuffer::getFileOrSTDIN(IRFilename.c_str(), MemBuf)) {
    errs() << "Error reading '" << IRFilename << "': " << ec.message() << "\n";
    ExitStatus = 1;
    return;
  }

  if (MemBuf->getBufferSize() % 4 != 0) {
    errs() << IRFilename
           << ": Bitcode stream should be a multiple of 4 bytes in length.\n";
    ExitStatus = 1;
    return;
  }

  const unsigned char *BufPtr = (const unsigned char *)MemBuf->getBufferStart();
  const unsigned char *EndBufPtr = BufPtr + MemBuf->getBufferSize();

  // Read header and verify it is good.
  NaClBitcodeHeader Header;
  if (Header.Read(BufPtr, EndBufPtr) || !Header.IsSupported()) {
    errs() << "Invalid PNaCl bitcode header.\n";
    ExitStatus = 1;
    return;
  }

  // Create a bitstream reader to read the bitcode file.
  NaClBitstreamReader InputStreamFile(BufPtr, EndBufPtr);
  NaClBitstreamCursor InputStream(InputStreamFile);

  TopLevelParser Parser(MemBuf->getBufferIdentifier(), Header, InputStream,
                        ExitStatus);
  int TopLevelBlocks = 0;
  while (!InputStream.AtEndOfStream()) {
    if (Parser.Parse()) {
      ExitStatus = 1;
      return;
    }
    ++TopLevelBlocks;
  }

  if (TopLevelBlocks != 1) {
    errs() << IRFilename
           << ": Contains more than one module. Found: " << TopLevelBlocks
           << "\n";
    ExitStatus = 1;
  }
  return;
}

} // end of anonymous namespace.
