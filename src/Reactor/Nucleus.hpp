// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef sw_Nucleus_hpp
#define sw_Nucleus_hpp

#include <cstdarg>
#include <cstdint>
#include <vector>

namespace sw
{
	class Type;
	class Value;
	class Constant;
	class BasicBlock;
	class Routine;

	enum Optimization
	{
		Disabled             = 0,
		InstructionCombining = 1,
		CFGSimplification    = 2,
		LICM                 = 3,
		AggressiveDCE        = 4,
		GVN                  = 5,
		Reassociate          = 6,
		DeadStoreElimination = 7,
		SCCP                 = 8,
		ScalarReplAggregates = 9,

		OptimizationCount
	};

	extern Optimization optimization[10];

	class Nucleus
	{
	public:
		Nucleus();

		virtual ~Nucleus();

		Routine *acquireRoutine(const wchar_t *name, bool runOptimizations = true);

		static Value *allocateStackVariable(Type *type, int arraySize = 0);
		static BasicBlock *createBasicBlock();
		static BasicBlock *getInsertBlock();
		static void setInsertBlock(BasicBlock *basicBlock);
		static BasicBlock *getPredecessor(BasicBlock *basicBlock);

		static void createFunction(Type *ReturnType, std::vector<Type*> &Params);
		static Value *getArgument(unsigned int index);

		// Terminators
		static void createRetVoid();
		static void createRet(Value *V);
		static void createBr(BasicBlock *dest);
		static void createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse);

		// Binary operators
		static Value *createAdd(Value *lhs, Value *rhs);
		static Value *createSub(Value *lhs, Value *rhs);
		static Value *createMul(Value *lhs, Value *rhs);
		static Value *createUDiv(Value *lhs, Value *rhs);
		static Value *createSDiv(Value *lhs, Value *rhs);
		static Value *createFAdd(Value *lhs, Value *rhs);
		static Value *createFSub(Value *lhs, Value *rhs);
		static Value *createFMul(Value *lhs, Value *rhs);
		static Value *createFDiv(Value *lhs, Value *rhs);
		static Value *createURem(Value *lhs, Value *rhs);
		static Value *createSRem(Value *lhs, Value *rhs);
		static Value *createFRem(Value *lhs, Value *rhs);
		static Value *createShl(Value *lhs, Value *rhs);
		static Value *createLShr(Value *lhs, Value *rhs);
		static Value *createAShr(Value *lhs, Value *rhs);
		static Value *createAnd(Value *lhs, Value *rhs);
		static Value *createOr(Value *lhs, Value *rhs);
		static Value *createXor(Value *lhs, Value *rhs);

		// Unary operators
		static Value *createAssign(Constant *c);
		static Value *createNeg(Value *V);
		static Value *createFNeg(Value *V);
		static Value *createNot(Value *V);

		// Memory instructions
		static Value *createLoad(Value *ptr, Type *type, bool isVolatile = false, unsigned int align = 0);
		static Value *createStore(Value *value, Value *ptr, Type *type, bool isVolatile = false, unsigned int align = 0);
		static Constant *createStore(Constant *constant, Value *ptr, Type *type, bool isVolatile = false, unsigned int align = 0);
		static Value *createGEP(Value *ptr, Type *type, Value *index);

		// Atomic instructions
		static Value *createAtomicAdd(Value *ptr, Value *value);

		// Cast/Conversion Operators
		static Value *createTrunc(Value *V, Type *destType);
		static Value *createZExt(Value *V, Type *destType);
		static Value *createSExt(Value *V, Type *destType);
		static Value *createFPToSI(Value *V, Type *destType);
		static Value *createUIToFP(Value *V, Type *destType);
		static Value *createSIToFP(Value *V, Type *destType);
		static Value *createFPTrunc(Value *V, Type *destType);
		static Value *createFPExt(Value *V, Type *destType);
		static Value *createBitCast(Value *V, Type *destType);
		static Value *createIntCast(Value *V, Type *destType, bool isSigned);

		// Compare instructions
		static Value *createICmpEQ(Value *lhs, Value *rhs);
		static Value *createICmpNE(Value *lhs, Value *rhs);
		static Value *createICmpUGT(Value *lhs, Value *rhs);
		static Value *createICmpUGE(Value *lhs, Value *rhs);
		static Value *createICmpULT(Value *lhs, Value *rhs);
		static Value *createICmpULE(Value *lhs, Value *rhs);
		static Value *createICmpSGT(Value *lhs, Value *rhs);
		static Value *createICmpSGE(Value *lhs, Value *rhs);
		static Value *createICmpSLT(Value *lhs, Value *rhs);
		static Value *createICmpSLE(Value *lhs, Value *rhs);
		static Value *createFCmpOEQ(Value *lhs, Value *rhs);
		static Value *createFCmpOGT(Value *lhs, Value *rhs);
		static Value *createFCmpOGE(Value *lhs, Value *rhs);
		static Value *createFCmpOLT(Value *lhs, Value *rhs);
		static Value *createFCmpOLE(Value *lhs, Value *rhs);
		static Value *createFCmpONE(Value *lhs, Value *rhs);
		static Value *createFCmpORD(Value *lhs, Value *rhs);
		static Value *createFCmpUNO(Value *lhs, Value *rhs);
		static Value *createFCmpUEQ(Value *lhs, Value *rhs);
		static Value *createFCmpUGT(Value *lhs, Value *rhs);
		static Value *createFCmpUGE(Value *lhs, Value *rhs);
		static Value *createFCmpULT(Value *lhs, Value *rhs);
		static Value *createFCmpULE(Value *lhs, Value *rhs);
		static Value *createFCmpUNE(Value *lhs, Value *rhs);

		// Vector instructions
		static Value *createExtractElement(Value *vector, Type *type, int index);
		static Value *createInsertElement(Value *vector, Value *element, int index);
		static Value *createShuffleVector(Value *V1, Value *V2, const int *select);

		// Other instructions
		static Value *createSelect(Value *C, Value *ifTrue, Value *ifFalse);
		static Value *createSwitch(Value *V, BasicBlock *Dest, unsigned NumCases);
		static void addSwitchCase(Value *Switch, int Case, BasicBlock *Branch);
		static void createUnreachable();

		// Constant values
		static Constant *createNullValue(Type *Ty);
		static Constant *createConstantInt(int64_t i);
		static Constant *createConstantInt(int i);
		static Constant *createConstantInt(unsigned int i);
		static Constant *createConstantBool(bool b);
		static Constant *createConstantByte(signed char i);
		static Constant *createConstantByte(unsigned char i);
		static Constant *createConstantShort(short i);
		static Constant *createConstantShort(unsigned short i);
		static Constant *createConstantFloat(float x);
		static Constant *createNullPointer(Type *Ty);
		static Constant *createConstantVector(Constant *const *Vals, unsigned NumVals);
		static Constant *createConstantPointer(const void *external, Type *Ty, bool isConstant, unsigned int Align);

		static Type *getPointerType(Type *ElementType);

	private:
		void optimize();
	};
}

#endif   // sw_Nucleus_hpp
