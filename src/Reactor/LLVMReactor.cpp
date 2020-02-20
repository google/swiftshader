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

#include "LLVMReactor.hpp"

#include "CPUID.hpp"
#include "Debug.hpp"
#include "EmulatedReactor.hpp"
#include "LLVMReactorDebugInfo.hpp"
#include "Print.hpp"
#include "Reactor.hpp"
#include "x86.hpp"

#include "llvm/IR/Intrinsics.h"
#if LLVM_VERSION_MAJOR >= 9
#	include "llvm/IR/IntrinsicsX86.h"
#endif
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Coroutines.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"

#define ARGS(...)   \
	{               \
		__VA_ARGS__ \
	}
#define CreateCall2 CreateCall
#define CreateCall3 CreateCall

#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <unordered_map>

#if defined(__i386__) || defined(__x86_64__)
#	include <xmmintrin.h>
#endif

#include <math.h>

#if defined(__x86_64__) && defined(_WIN32)
extern "C" void X86CompilationCallback()
{
	UNIMPLEMENTED_NO_BUG("X86CompilationCallback");
}
#endif

namespace {

std::unique_ptr<rr::JITBuilder> jit;
std::mutex codegenMutex;

// Default configuration settings. Must be accessed under mutex lock.
std::mutex defaultConfigLock;
rr::Config &defaultConfig()
{
	// This uses a static in a function to avoid the cost of a global static
	// initializer. See http://neugierig.org/software/chromium/notes/2011/08/static-initializers.html
	static rr::Config config = rr::Config::Edit()
	                               .add(rr::Optimization::Pass::ScalarReplAggregates)
	                               .add(rr::Optimization::Pass::InstructionCombining)
	                               .apply({});
	return config;
}

llvm::Value *lowerPAVG(llvm::Value *x, llvm::Value *y)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());

	llvm::VectorType *extTy =
	    llvm::VectorType::getExtendedElementVectorType(ty);
	x = jit->builder->CreateZExt(x, extTy);
	y = jit->builder->CreateZExt(y, extTy);

	// (x + y + 1) >> 1
	llvm::Constant *one = llvm::ConstantInt::get(extTy, 1);
	llvm::Value *res = jit->builder->CreateAdd(x, y);
	res = jit->builder->CreateAdd(res, one);
	res = jit->builder->CreateLShr(res, one);
	return jit->builder->CreateTrunc(res, ty);
}

llvm::Value *lowerPMINMAX(llvm::Value *x, llvm::Value *y,
                          llvm::ICmpInst::Predicate pred)
{
	return jit->builder->CreateSelect(jit->builder->CreateICmp(pred, x, y), x, y);
}

llvm::Value *lowerPCMP(llvm::ICmpInst::Predicate pred, llvm::Value *x,
                       llvm::Value *y, llvm::Type *dstTy)
{
	return jit->builder->CreateSExt(jit->builder->CreateICmp(pred, x, y), dstTy, "");
}

#if defined(__i386__) || defined(__x86_64__)
llvm::Value *lowerPMOV(llvm::Value *op, llvm::Type *dstType, bool sext)
{
	llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(op->getType());
	llvm::VectorType *dstTy = llvm::cast<llvm::VectorType>(dstType);

	llvm::Value *undef = llvm::UndefValue::get(srcTy);
	llvm::SmallVector<uint32_t, 16> mask(dstTy->getNumElements());
	std::iota(mask.begin(), mask.end(), 0);
	llvm::Value *v = jit->builder->CreateShuffleVector(op, undef, mask);

	return sext ? jit->builder->CreateSExt(v, dstTy)
	            : jit->builder->CreateZExt(v, dstTy);
}

llvm::Value *lowerPABS(llvm::Value *v)
{
	llvm::Value *zero = llvm::Constant::getNullValue(v->getType());
	llvm::Value *cmp = jit->builder->CreateICmp(llvm::ICmpInst::ICMP_SGT, v, zero);
	llvm::Value *neg = jit->builder->CreateNeg(v);
	return jit->builder->CreateSelect(cmp, v, neg);
}
#endif  // defined(__i386__) || defined(__x86_64__)

#if !defined(__i386__) && !defined(__x86_64__)
llvm::Value *lowerPFMINMAX(llvm::Value *x, llvm::Value *y,
                           llvm::FCmpInst::Predicate pred)
{
	return jit->builder->CreateSelect(jit->builder->CreateFCmp(pred, x, y), x, y);
}

llvm::Value *lowerRound(llvm::Value *x)
{
	llvm::Function *nearbyint = llvm::Intrinsic::getDeclaration(
	    jit->module.get(), llvm::Intrinsic::nearbyint, { x->getType() });
	return jit->builder->CreateCall(nearbyint, ARGS(x));
}

llvm::Value *lowerRoundInt(llvm::Value *x, llvm::Type *ty)
{
	return jit->builder->CreateFPToSI(lowerRound(x), ty);
}

llvm::Value *lowerFloor(llvm::Value *x)
{
	llvm::Function *floor = llvm::Intrinsic::getDeclaration(
	    jit->module.get(), llvm::Intrinsic::floor, { x->getType() });
	return jit->builder->CreateCall(floor, ARGS(x));
}

llvm::Value *lowerTrunc(llvm::Value *x)
{
	llvm::Function *trunc = llvm::Intrinsic::getDeclaration(
	    jit->module.get(), llvm::Intrinsic::trunc, { x->getType() });
	return jit->builder->CreateCall(trunc, ARGS(x));
}

// Packed add/sub with saturation
llvm::Value *lowerPSAT(llvm::Value *x, llvm::Value *y, bool isAdd, bool isSigned)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

	unsigned numBits = ty->getScalarSizeInBits();

	llvm::Value *max, *min, *extX, *extY;
	if(isSigned)
	{
		max = llvm::ConstantInt::get(extTy, (1LL << (numBits - 1)) - 1, true);
		min = llvm::ConstantInt::get(extTy, (-1LL << (numBits - 1)), true);
		extX = jit->builder->CreateSExt(x, extTy);
		extY = jit->builder->CreateSExt(y, extTy);
	}
	else
	{
		ASSERT_MSG(numBits <= 64, "numBits: %d", int(numBits));
		uint64_t maxVal = (numBits == 64) ? ~0ULL : (1ULL << numBits) - 1;
		max = llvm::ConstantInt::get(extTy, maxVal, false);
		min = llvm::ConstantInt::get(extTy, 0, false);
		extX = jit->builder->CreateZExt(x, extTy);
		extY = jit->builder->CreateZExt(y, extTy);
	}

	llvm::Value *res = isAdd ? jit->builder->CreateAdd(extX, extY)
	                         : jit->builder->CreateSub(extX, extY);

	res = lowerPMINMAX(res, min, llvm::ICmpInst::ICMP_SGT);
	res = lowerPMINMAX(res, max, llvm::ICmpInst::ICMP_SLT);

	return jit->builder->CreateTrunc(res, ty);
}

llvm::Value *lowerSQRT(llvm::Value *x)
{
	llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(
	    jit->module.get(), llvm::Intrinsic::sqrt, { x->getType() });
	return jit->builder->CreateCall(sqrt, ARGS(x));
}

llvm::Value *lowerRCP(llvm::Value *x)
{
	llvm::Type *ty = x->getType();
	llvm::Constant *one;
	if(llvm::VectorType *vectorTy = llvm::dyn_cast<llvm::VectorType>(ty))
	{
		one = llvm::ConstantVector::getSplat(
		    vectorTy->getNumElements(),
		    llvm::ConstantFP::get(vectorTy->getElementType(), 1));
	}
	else
	{
		one = llvm::ConstantFP::get(ty, 1);
	}
	return jit->builder->CreateFDiv(one, x);
}

llvm::Value *lowerRSQRT(llvm::Value *x)
{
	return lowerRCP(lowerSQRT(x));
}

llvm::Value *lowerVectorShl(llvm::Value *x, uint64_t scalarY)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::Value *y = llvm::ConstantVector::getSplat(
	    ty->getNumElements(),
	    llvm::ConstantInt::get(ty->getElementType(), scalarY));
	return jit->builder->CreateShl(x, y);
}

llvm::Value *lowerVectorAShr(llvm::Value *x, uint64_t scalarY)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::Value *y = llvm::ConstantVector::getSplat(
	    ty->getNumElements(),
	    llvm::ConstantInt::get(ty->getElementType(), scalarY));
	return jit->builder->CreateAShr(x, y);
}

llvm::Value *lowerVectorLShr(llvm::Value *x, uint64_t scalarY)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::Value *y = llvm::ConstantVector::getSplat(
	    ty->getNumElements(),
	    llvm::ConstantInt::get(ty->getElementType(), scalarY));
	return jit->builder->CreateLShr(x, y);
}

llvm::Value *lowerMulAdd(llvm::Value *x, llvm::Value *y)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

	llvm::Value *extX = jit->builder->CreateSExt(x, extTy);
	llvm::Value *extY = jit->builder->CreateSExt(y, extTy);
	llvm::Value *mult = jit->builder->CreateMul(extX, extY);

	llvm::Value *undef = llvm::UndefValue::get(extTy);

	llvm::SmallVector<uint32_t, 16> evenIdx;
	llvm::SmallVector<uint32_t, 16> oddIdx;
	for(uint64_t i = 0, n = ty->getNumElements(); i < n; i += 2)
	{
		evenIdx.push_back(i);
		oddIdx.push_back(i + 1);
	}

	llvm::Value *lhs = jit->builder->CreateShuffleVector(mult, undef, evenIdx);
	llvm::Value *rhs = jit->builder->CreateShuffleVector(mult, undef, oddIdx);
	return jit->builder->CreateAdd(lhs, rhs);
}

llvm::Value *lowerPack(llvm::Value *x, llvm::Value *y, bool isSigned)
{
	llvm::VectorType *srcTy = llvm::cast<llvm::VectorType>(x->getType());
	llvm::VectorType *dstTy = llvm::VectorType::getTruncatedElementVectorType(srcTy);

	llvm::IntegerType *dstElemTy =
	    llvm::cast<llvm::IntegerType>(dstTy->getElementType());

	uint64_t truncNumBits = dstElemTy->getIntegerBitWidth();
	ASSERT_MSG(truncNumBits < 64, "shift 64 must be handled separately. truncNumBits: %d", int(truncNumBits));
	llvm::Constant *max, *min;
	if(isSigned)
	{
		max = llvm::ConstantInt::get(srcTy, (1LL << (truncNumBits - 1)) - 1, true);
		min = llvm::ConstantInt::get(srcTy, (-1LL << (truncNumBits - 1)), true);
	}
	else
	{
		max = llvm::ConstantInt::get(srcTy, (1ULL << truncNumBits) - 1, false);
		min = llvm::ConstantInt::get(srcTy, 0, false);
	}

	x = lowerPMINMAX(x, min, llvm::ICmpInst::ICMP_SGT);
	x = lowerPMINMAX(x, max, llvm::ICmpInst::ICMP_SLT);
	y = lowerPMINMAX(y, min, llvm::ICmpInst::ICMP_SGT);
	y = lowerPMINMAX(y, max, llvm::ICmpInst::ICMP_SLT);

	x = jit->builder->CreateTrunc(x, dstTy);
	y = jit->builder->CreateTrunc(y, dstTy);

	llvm::SmallVector<uint32_t, 16> index(srcTy->getNumElements() * 2);
	std::iota(index.begin(), index.end(), 0);

	return jit->builder->CreateShuffleVector(x, y, index);
}

llvm::Value *lowerSignMask(llvm::Value *x, llvm::Type *retTy)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::Constant *zero = llvm::ConstantInt::get(ty, 0);
	llvm::Value *cmp = jit->builder->CreateICmpSLT(x, zero);

	llvm::Value *ret = jit->builder->CreateZExt(
	    jit->builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
	for(uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
	{
		llvm::Value *elem = jit->builder->CreateZExt(
		    jit->builder->CreateExtractElement(cmp, i), retTy);
		ret = jit->builder->CreateOr(ret, jit->builder->CreateShl(elem, i));
	}
	return ret;
}

llvm::Value *lowerFPSignMask(llvm::Value *x, llvm::Type *retTy)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::Constant *zero = llvm::ConstantFP::get(ty, 0);
	llvm::Value *cmp = jit->builder->CreateFCmpULT(x, zero);

	llvm::Value *ret = jit->builder->CreateZExt(
	    jit->builder->CreateExtractElement(cmp, static_cast<uint64_t>(0)), retTy);
	for(uint64_t i = 1, n = ty->getNumElements(); i < n; ++i)
	{
		llvm::Value *elem = jit->builder->CreateZExt(
		    jit->builder->CreateExtractElement(cmp, i), retTy);
		ret = jit->builder->CreateOr(ret, jit->builder->CreateShl(elem, i));
	}
	return ret;
}
#endif  // !defined(__i386__) && !defined(__x86_64__)

#if(LLVM_VERSION_MAJOR >= 8) || (!defined(__i386__) && !defined(__x86_64__))
llvm::Value *lowerPUADDSAT(llvm::Value *x, llvm::Value *y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::uadd_sat, x, y);
#	else
	return lowerPSAT(x, y, true, false);
#	endif
}

llvm::Value *lowerPSADDSAT(llvm::Value *x, llvm::Value *y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::sadd_sat, x, y);
#	else
	return lowerPSAT(x, y, true, true);
#	endif
}

llvm::Value *lowerPUSUBSAT(llvm::Value *x, llvm::Value *y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::usub_sat, x, y);
#	else
	return lowerPSAT(x, y, false, false);
#	endif
}

llvm::Value *lowerPSSUBSAT(llvm::Value *x, llvm::Value *y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return jit->builder->CreateBinaryIntrinsic(llvm::Intrinsic::ssub_sat, x, y);
#	else
	return lowerPSAT(x, y, false, true);
#	endif
}
#endif  // (LLVM_VERSION_MAJOR >= 8) || (!defined(__i386__) && !defined(__x86_64__))

llvm::Value *lowerMulHigh(llvm::Value *x, llvm::Value *y, bool sext)
{
	llvm::VectorType *ty = llvm::cast<llvm::VectorType>(x->getType());
	llvm::VectorType *extTy = llvm::VectorType::getExtendedElementVectorType(ty);

	llvm::Value *extX, *extY;
	if(sext)
	{
		extX = jit->builder->CreateSExt(x, extTy);
		extY = jit->builder->CreateSExt(y, extTy);
	}
	else
	{
		extX = jit->builder->CreateZExt(x, extTy);
		extY = jit->builder->CreateZExt(y, extTy);
	}

	llvm::Value *mult = jit->builder->CreateMul(extX, extY);

	llvm::IntegerType *intTy = llvm::cast<llvm::IntegerType>(ty->getElementType());
	llvm::Value *mulh = jit->builder->CreateAShr(mult, intTy->getBitWidth());
	return jit->builder->CreateTrunc(mulh, ty);
}

llvm::Value *createGather(llvm::Value *base, llvm::Type *elTy, llvm::Value *offsets, llvm::Value *mask, unsigned int alignment, bool zeroMaskedLanes)
{
	ASSERT(base->getType()->isPointerTy());
	ASSERT(offsets->getType()->isVectorTy());
	ASSERT(mask->getType()->isVectorTy());

	auto numEls = mask->getType()->getVectorNumElements();
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
	auto i8PtrTy = i8Ty->getPointerTo();
	auto elPtrTy = elTy->getPointerTo();
	auto elVecTy = ::llvm::VectorType::get(elTy, numEls);
	auto elPtrVecTy = ::llvm::VectorType::get(elPtrTy, numEls);
	auto i8Base = jit->builder->CreatePointerCast(base, i8PtrTy);
	auto i8Ptrs = jit->builder->CreateGEP(i8Base, offsets);
	auto elPtrs = jit->builder->CreatePointerCast(i8Ptrs, elPtrVecTy);
	auto i8Mask = jit->builder->CreateIntCast(mask, ::llvm::VectorType::get(i1Ty, numEls), false);  // vec<int, int, ...> -> vec<bool, bool, ...>
	auto passthrough = zeroMaskedLanes ? ::llvm::Constant::getNullValue(elVecTy) : llvm::UndefValue::get(elVecTy);
	auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
	auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_gather, { elVecTy, elPtrVecTy });
	return jit->builder->CreateCall(func, { elPtrs, align, i8Mask, passthrough });
}

void createScatter(llvm::Value *base, llvm::Value *val, llvm::Value *offsets, llvm::Value *mask, unsigned int alignment)
{
	ASSERT(base->getType()->isPointerTy());
	ASSERT(val->getType()->isVectorTy());
	ASSERT(offsets->getType()->isVectorTy());
	ASSERT(mask->getType()->isVectorTy());

	auto numEls = mask->getType()->getVectorNumElements();
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
	auto i8PtrTy = i8Ty->getPointerTo();
	auto elVecTy = val->getType();
	auto elTy = elVecTy->getVectorElementType();
	auto elPtrTy = elTy->getPointerTo();
	auto elPtrVecTy = ::llvm::VectorType::get(elPtrTy, numEls);
	auto i8Base = jit->builder->CreatePointerCast(base, i8PtrTy);
	auto i8Ptrs = jit->builder->CreateGEP(i8Base, offsets);
	auto elPtrs = jit->builder->CreatePointerCast(i8Ptrs, elPtrVecTy);
	auto i8Mask = jit->builder->CreateIntCast(mask, ::llvm::VectorType::get(i1Ty, numEls), false);  // vec<int, int, ...> -> vec<bool, bool, ...>
	auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
	auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_scatter, { elVecTy, elPtrVecTy });
	jit->builder->CreateCall(func, { val, elPtrs, align, i8Mask });
}
}  // namespace

namespace rr {

std::string BackendName()
{
	return std::string("LLVM ") + LLVM_VERSION_STRING;
}

const Capabilities Caps = {
	true,  // CoroutinesSupported
};

// The abstract Type* types are implemented as LLVM types, except that
// 64-bit vectors are emulated using 128-bit ones to avoid use of MMX in x86
// and VFP in ARM, and eliminate the overhead of converting them to explicit
// 128-bit ones. LLVM types are pointers, so we can represent emulated types
// as abstract pointers with small enum values.
enum InternalType : uintptr_t
{
	// Emulated types:
	Type_v2i32,
	Type_v4i16,
	Type_v2i16,
	Type_v8i8,
	Type_v4i8,
	Type_v2f32,
	EmulatedTypeCount,
	// Returned by asInternalType() to indicate that the abstract Type*
	// should be interpreted as LLVM type pointer:
	Type_LLVM
};

inline InternalType asInternalType(Type *type)
{
	InternalType t = static_cast<InternalType>(reinterpret_cast<uintptr_t>(type));
	return (t < EmulatedTypeCount) ? t : Type_LLVM;
}

llvm::Type *T(Type *t)
{
	// Use 128-bit vectors to implement logically shorter ones.
	switch(asInternalType(t))
	{
		case Type_v2i32: return T(Int4::getType());
		case Type_v4i16: return T(Short8::getType());
		case Type_v2i16: return T(Short8::getType());
		case Type_v8i8: return T(Byte16::getType());
		case Type_v4i8: return T(Byte16::getType());
		case Type_v2f32: return T(Float4::getType());
		case Type_LLVM: return reinterpret_cast<llvm::Type *>(t);
		default:
			UNREACHABLE("asInternalType(t): %d", int(asInternalType(t)));
			return nullptr;
	}
}

Type *T(InternalType t)
{
	return reinterpret_cast<Type *>(t);
}

inline const std::vector<llvm::Type *> &T(const std::vector<Type *> &t)
{
	return reinterpret_cast<const std::vector<llvm::Type *> &>(t);
}

inline llvm::BasicBlock *B(BasicBlock *t)
{
	return reinterpret_cast<llvm::BasicBlock *>(t);
}

inline BasicBlock *B(llvm::BasicBlock *t)
{
	return reinterpret_cast<BasicBlock *>(t);
}

static size_t typeSize(Type *type)
{
	switch(asInternalType(type))
	{
		case Type_v2i32: return 8;
		case Type_v4i16: return 8;
		case Type_v2i16: return 4;
		case Type_v8i8: return 8;
		case Type_v4i8: return 4;
		case Type_v2f32: return 8;
		case Type_LLVM:
		{
			llvm::Type *t = T(type);

			if(t->isPointerTy())
			{
				return sizeof(void *);
			}

			// At this point we should only have LLVM 'primitive' types.
			unsigned int bits = t->getPrimitiveSizeInBits();
			ASSERT_MSG(bits != 0, "bits: %d", int(bits));

			// TODO(capn): Booleans are 1 bit integers in LLVM's SSA type system,
			// but are typically stored as one byte. The DataLayout structure should
			// be used here and many other places if this assumption fails.
			return (bits + 7) / 8;
		}
		break;
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return 0;
	}
}

static unsigned int elementCount(Type *type)
{
	switch(asInternalType(type))
	{
		case Type_v2i32: return 2;
		case Type_v4i16: return 4;
		case Type_v2i16: return 2;
		case Type_v8i8: return 8;
		case Type_v4i8: return 4;
		case Type_v2f32: return 2;
		case Type_LLVM: return llvm::cast<llvm::VectorType>(T(type))->getNumElements();
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return 0;
	}
}

static ::llvm::Function *createFunction(const char *name, ::llvm::Type *retTy, const std::vector<::llvm::Type *> &params)
{
	llvm::FunctionType *functionType = llvm::FunctionType::get(retTy, params, false);
	auto func = llvm::Function::Create(functionType, llvm::GlobalValue::InternalLinkage, name, jit->module.get());
	func->setDoesNotThrow();
	func->setCallingConv(llvm::CallingConv::C);
	return func;
}

Nucleus::Nucleus()
{
	::codegenMutex.lock();  // Reactor and LLVM are currently not thread safe

	ASSERT(jit == nullptr);
	jit.reset(new JITBuilder(Nucleus::getDefaultConfig()));
}

Nucleus::~Nucleus()
{
	jit.reset();
	::codegenMutex.unlock();
}

void Nucleus::setDefaultConfig(const Config &cfg)
{
	std::unique_lock<std::mutex> lock(::defaultConfigLock);
	::defaultConfig() = cfg;
}

void Nucleus::adjustDefaultConfig(const Config::Edit &cfgEdit)
{
	std::unique_lock<std::mutex> lock(::defaultConfigLock);
	auto &config = ::defaultConfig();
	config = cfgEdit.apply(config);
}

Config Nucleus::getDefaultConfig()
{
	std::unique_lock<std::mutex> lock(::defaultConfigLock);
	return ::defaultConfig();
}

std::shared_ptr<Routine> Nucleus::acquireRoutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
{
	std::shared_ptr<Routine> routine;

	auto acquire = [&]() {
		auto cfg = cfgEdit.apply(jit->config);

		if(jit->builder->GetInsertBlock()->empty() || !jit->builder->GetInsertBlock()->back().isTerminator())
		{
			llvm::Type *type = jit->function->getReturnType();

			if(type->isVoidTy())
			{
				createRetVoid();
			}
			else
			{
				createRet(V(llvm::UndefValue::get(type)));
			}
		}

#ifdef ENABLE_RR_DEBUG_INFO
		if(jit->debugInfo != nullptr)
		{
			jit->debugInfo->Finalize();
		}
#endif  // ENABLE_RR_DEBUG_INFO

		if(false)
		{
			std::error_code error;
			llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-unopt.txt", error);
			jit->module->print(file, 0);
		}

#if defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)
		{
			llvm::legacy::PassManager pm;
			pm.add(llvm::createVerifierPass());
			pm.run(*jit->module);
		}
#endif  // defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)

		jit->optimize(cfg);

		if(false)
		{
			std::error_code error;
			llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-opt.txt", error);
			jit->module->print(file, 0);
		}

		routine = jit->acquireRoutine(&jit->function, 1, cfg);
		jit.reset();
	};

#ifdef JIT_IN_SEPARATE_THREAD
	// Perform optimizations and codegen in a separate thread to avoid stack overflow.
	// FIXME(b/149829034): This is not a long-term solution. Reactor has no control
	// over the threading and stack sizes of its users, so this should be addressed
	// at a higher level instead.
	std::thread thread(acquire);
	thread.join();
#else
	acquire();
#endif

	return routine;
}

Value *Nucleus::allocateStackVariable(Type *type, int arraySize)
{
	// Need to allocate it in the entry block for mem2reg to work
	llvm::BasicBlock &entryBlock = jit->function->getEntryBlock();

	llvm::Instruction *declaration;

	if(arraySize)
	{
		declaration = new llvm::AllocaInst(T(type), 0, V(Nucleus::createConstantInt(arraySize)));
	}
	else
	{
		declaration = new llvm::AllocaInst(T(type), 0, (llvm::Value *)nullptr);
	}

	entryBlock.getInstList().push_front(declaration);

	return V(declaration);
}

BasicBlock *Nucleus::createBasicBlock()
{
	return B(llvm::BasicBlock::Create(jit->context, "", jit->function));
}

BasicBlock *Nucleus::getInsertBlock()
{
	return B(jit->builder->GetInsertBlock());
}

void Nucleus::setInsertBlock(BasicBlock *basicBlock)
{
	//	assert(jit->builder->GetInsertBlock()->back().isTerminator());

	Variable::materializeAll();

	jit->builder->SetInsertPoint(B(basicBlock));
}

void Nucleus::createFunction(Type *ReturnType, const std::vector<Type *> &Params)
{
	jit->function = rr::createFunction("", T(ReturnType), T(Params));

#ifdef ENABLE_RR_DEBUG_INFO
	jit->debugInfo = std::make_unique<DebugInfo>(jit->builder.get(), &jit->context, jit->module.get(), jit->function);
#endif  // ENABLE_RR_DEBUG_INFO

	jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->function));
}

Value *Nucleus::getArgument(unsigned int index)
{
	llvm::Function::arg_iterator args = jit->function->arg_begin();

	while(index)
	{
		args++;
		index--;
	}

	return V(&*args);
}

void Nucleus::createRetVoid()
{
	RR_DEBUG_INFO_UPDATE_LOC();

	ASSERT_MSG(jit->function->getReturnType() == T(Void::getType()), "Return type mismatch");

	// Code generated after this point is unreachable, so any variables
	// being read can safely return an undefined value. We have to avoid
	// materializing variables after the terminator ret instruction.
	Variable::killUnmaterialized();

	jit->builder->CreateRetVoid();
}

void Nucleus::createRet(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();

	ASSERT_MSG(jit->function->getReturnType() == V(v)->getType(), "Return type mismatch");

	// Code generated after this point is unreachable, so any variables
	// being read can safely return an undefined value. We have to avoid
	// materializing variables after the terminator ret instruction.
	Variable::killUnmaterialized();

	jit->builder->CreateRet(V(v));
}

void Nucleus::createBr(BasicBlock *dest)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	jit->builder->CreateBr(B(dest));
}

void Nucleus::createCondBr(Value *cond, BasicBlock *ifTrue, BasicBlock *ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();
	jit->builder->CreateCondBr(V(cond), B(ifTrue), B(ifFalse));
}

Value *Nucleus::createAdd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAdd(V(lhs), V(rhs)));
}

Value *Nucleus::createSub(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSub(V(lhs), V(rhs)));
}

Value *Nucleus::createMul(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateMul(V(lhs), V(rhs)));
}

Value *Nucleus::createUDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateUDiv(V(lhs), V(rhs)));
}

Value *Nucleus::createSDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSDiv(V(lhs), V(rhs)));
}

Value *Nucleus::createFAdd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFAdd(V(lhs), V(rhs)));
}

Value *Nucleus::createFSub(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFSub(V(lhs), V(rhs)));
}

Value *Nucleus::createFMul(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFMul(V(lhs), V(rhs)));
}

Value *Nucleus::createFDiv(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFDiv(V(lhs), V(rhs)));
}

Value *Nucleus::createURem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateURem(V(lhs), V(rhs)));
}

Value *Nucleus::createSRem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSRem(V(lhs), V(rhs)));
}

Value *Nucleus::createFRem(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFRem(V(lhs), V(rhs)));
}

RValue<Float4> operator%(RValue<Float4> lhs, RValue<Float4> rhs)
{
	return RValue<Float4>(Nucleus::createFRem(lhs.value, rhs.value));
}

Value *Nucleus::createShl(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateShl(V(lhs), V(rhs)));
}

Value *Nucleus::createLShr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateLShr(V(lhs), V(rhs)));
}

Value *Nucleus::createAShr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAShr(V(lhs), V(rhs)));
}

Value *Nucleus::createAnd(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAnd(V(lhs), V(rhs)));
}

Value *Nucleus::createOr(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateOr(V(lhs), V(rhs)));
}

Value *Nucleus::createXor(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateXor(V(lhs), V(rhs)));
}

Value *Nucleus::createNeg(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateNeg(V(v)));
}

Value *Nucleus::createFNeg(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFNeg(V(v)));
}

Value *Nucleus::createNot(Value *v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateNot(V(v)));
}

Value *Nucleus::createLoad(Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	switch(asInternalType(type))
	{
		case Type_v2i32:
		case Type_v4i16:
		case Type_v8i8:
		case Type_v2f32:
			return createBitCast(
			    createInsertElement(
			        V(llvm::UndefValue::get(llvm::VectorType::get(T(Long::getType()), 2))),
			        createLoad(createBitCast(ptr, Pointer<Long>::getType()), Long::getType(), isVolatile, alignment, atomic, memoryOrder),
			        0),
			    type);
		case Type_v2i16:
		case Type_v4i8:
			if(alignment != 0)  // Not a local variable (all vectors are 128-bit).
			{
				Value *u = V(llvm::UndefValue::get(llvm::VectorType::get(T(Long::getType()), 2)));
				Value *i = createLoad(createBitCast(ptr, Pointer<Int>::getType()), Int::getType(), isVolatile, alignment, atomic, memoryOrder);
				i = createZExt(i, Long::getType());
				Value *v = createInsertElement(u, i, 0);
				return createBitCast(v, type);
			}
			// Fallthrough to non-emulated case.
		case Type_LLVM:
		{
			auto elTy = T(type);
			ASSERT(V(ptr)->getType()->getContainedType(0) == elTy);

			if(!atomic)
			{
				return V(jit->builder->CreateAlignedLoad(V(ptr), alignment, isVolatile));
			}
			else if(elTy->isIntegerTy() || elTy->isPointerTy())
			{
				// Integers and pointers can be atomically loaded by setting
				// the ordering constraint on the load instruction.
				auto load = jit->builder->CreateAlignedLoad(V(ptr), alignment, isVolatile);
				load->setAtomic(atomicOrdering(atomic, memoryOrder));
				return V(load);
			}
			else if(elTy->isFloatTy() || elTy->isDoubleTy())
			{
				// LLVM claims to support atomic loads of float types as
				// above, but certain backends cannot deal with this.
				// Load as an integer and bitcast. See b/136037244.
				auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
				auto elAsIntTy = ::llvm::IntegerType::get(jit->context, size * 8);
				auto ptrCast = jit->builder->CreatePointerCast(V(ptr), elAsIntTy->getPointerTo());
				auto load = jit->builder->CreateAlignedLoad(ptrCast, alignment, isVolatile);
				load->setAtomic(atomicOrdering(atomic, memoryOrder));
				auto loadCast = jit->builder->CreateBitCast(load, elTy);
				return V(loadCast);
			}
			else
			{
				// More exotic types require falling back to the extern:
				// void __atomic_load(size_t size, void *ptr, void *ret, int ordering)
				auto sizetTy = ::llvm::IntegerType::get(jit->context, sizeof(size_t) * 8);
				auto intTy = ::llvm::IntegerType::get(jit->context, sizeof(int) * 8);
				auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
				auto i8PtrTy = i8Ty->getPointerTo();
				auto voidTy = ::llvm::Type::getVoidTy(jit->context);
				auto funcTy = ::llvm::FunctionType::get(voidTy, { sizetTy, i8PtrTy, i8PtrTy, intTy }, false);
				auto func = jit->module->getOrInsertFunction("__atomic_load", funcTy);
				auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
				auto out = allocateStackVariable(type);
				jit->builder->CreateCall(func, {
				                                   ::llvm::ConstantInt::get(sizetTy, size),
				                                   jit->builder->CreatePointerCast(V(ptr), i8PtrTy),
				                                   jit->builder->CreatePointerCast(V(out), i8PtrTy),
				                                   ::llvm::ConstantInt::get(intTy, uint64_t(atomicOrdering(true, memoryOrder))),
				                               });
				return V(jit->builder->CreateLoad(V(out)));
			}
		}
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return nullptr;
	}
}

Value *Nucleus::createStore(Value *value, Value *ptr, Type *type, bool isVolatile, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	switch(asInternalType(type))
	{
		case Type_v2i32:
		case Type_v4i16:
		case Type_v8i8:
		case Type_v2f32:
			createStore(
			    createExtractElement(
			        createBitCast(value, T(llvm::VectorType::get(T(Long::getType()), 2))), Long::getType(), 0),
			    createBitCast(ptr, Pointer<Long>::getType()),
			    Long::getType(), isVolatile, alignment, atomic, memoryOrder);
			return value;
		case Type_v2i16:
		case Type_v4i8:
			if(alignment != 0)  // Not a local variable (all vectors are 128-bit).
			{
				createStore(
				    createExtractElement(createBitCast(value, Int4::getType()), Int::getType(), 0),
				    createBitCast(ptr, Pointer<Int>::getType()),
				    Int::getType(), isVolatile, alignment, atomic, memoryOrder);
				return value;
			}
			// Fallthrough to non-emulated case.
		case Type_LLVM:
		{
			auto elTy = T(type);
			ASSERT(V(ptr)->getType()->getContainedType(0) == elTy);

#if __has_feature(memory_sanitizer)
			// Mark all memory writes as initialized by calling __msan_unpoison
			{
				// void __msan_unpoison(const volatile void *a, size_t size)
				auto voidTy = ::llvm::Type::getVoidTy(jit->context);
				auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
				auto voidPtrTy = i8Ty->getPointerTo();
				auto sizetTy = ::llvm::IntegerType::get(jit->context, sizeof(size_t) * 8);
				auto funcTy = ::llvm::FunctionType::get(voidTy, { voidPtrTy, sizetTy }, false);
				auto func = jit->module->getOrInsertFunction("__msan_unpoison", funcTy);
				auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
				jit->builder->CreateCall(func, { jit->builder->CreatePointerCast(V(ptr), voidPtrTy),
				                                 ::llvm::ConstantInt::get(sizetTy, size) });
			}
#endif

			if(!atomic)
			{
				jit->builder->CreateAlignedStore(V(value), V(ptr), alignment, isVolatile);
			}
			else if(elTy->isIntegerTy() || elTy->isPointerTy())
			{
				// Integers and pointers can be atomically stored by setting
				// the ordering constraint on the store instruction.
				auto store = jit->builder->CreateAlignedStore(V(value), V(ptr), alignment, isVolatile);
				store->setAtomic(atomicOrdering(atomic, memoryOrder));
			}
			else if(elTy->isFloatTy() || elTy->isDoubleTy())
			{
				// LLVM claims to support atomic stores of float types as
				// above, but certain backends cannot deal with this.
				// Store as an bitcast integer. See b/136037244.
				auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
				auto elAsIntTy = ::llvm::IntegerType::get(jit->context, size * 8);
				auto valCast = jit->builder->CreateBitCast(V(value), elAsIntTy);
				auto ptrCast = jit->builder->CreatePointerCast(V(ptr), elAsIntTy->getPointerTo());
				auto store = jit->builder->CreateAlignedStore(valCast, ptrCast, alignment, isVolatile);
				store->setAtomic(atomicOrdering(atomic, memoryOrder));
			}
			else
			{
				// More exotic types require falling back to the extern:
				// void __atomic_store(size_t size, void *ptr, void *val, int ordering)
				auto sizetTy = ::llvm::IntegerType::get(jit->context, sizeof(size_t) * 8);
				auto intTy = ::llvm::IntegerType::get(jit->context, sizeof(int) * 8);
				auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
				auto i8PtrTy = i8Ty->getPointerTo();
				auto voidTy = ::llvm::Type::getVoidTy(jit->context);
				auto funcTy = ::llvm::FunctionType::get(voidTy, { sizetTy, i8PtrTy, i8PtrTy, intTy }, false);
				auto func = jit->module->getOrInsertFunction("__atomic_store", funcTy);
				auto size = jit->module->getDataLayout().getTypeStoreSize(elTy);
				auto copy = allocateStackVariable(type);
				jit->builder->CreateStore(V(value), V(copy));
				jit->builder->CreateCall(func, {
				                                   ::llvm::ConstantInt::get(sizetTy, size),
				                                   jit->builder->CreatePointerCast(V(ptr), i8PtrTy),
				                                   jit->builder->CreatePointerCast(V(copy), i8PtrTy),
				                                   ::llvm::ConstantInt::get(intTy, uint64_t(atomicOrdering(true, memoryOrder))),
				                               });
			}

			return value;
		}
		default:
			UNREACHABLE("asInternalType(type): %d", int(asInternalType(type)));
			return nullptr;
	}
}

Value *Nucleus::createMaskedLoad(Value *ptr, Type *elTy, Value *mask, unsigned int alignment, bool zeroMaskedLanes)
{
	RR_DEBUG_INFO_UPDATE_LOC();

	ASSERT(V(ptr)->getType()->isPointerTy());
	ASSERT(V(mask)->getType()->isVectorTy());

	auto numEls = V(mask)->getType()->getVectorNumElements();
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto elVecTy = ::llvm::VectorType::get(T(elTy), numEls);
	auto elVecPtrTy = elVecTy->getPointerTo();
	auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false);  // vec<int, int, ...> -> vec<bool, bool, ...>
	auto passthrough = zeroMaskedLanes ? ::llvm::Constant::getNullValue(elVecTy) : llvm::UndefValue::get(elVecTy);
	auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
	auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_load, { elVecTy, elVecPtrTy });
	return V(jit->builder->CreateCall(func, { V(ptr), align, i8Mask, passthrough }));
}

void Nucleus::createMaskedStore(Value *ptr, Value *val, Value *mask, unsigned int alignment)
{
	RR_DEBUG_INFO_UPDATE_LOC();

	ASSERT(V(ptr)->getType()->isPointerTy());
	ASSERT(V(val)->getType()->isVectorTy());
	ASSERT(V(mask)->getType()->isVectorTy());

	auto numEls = V(mask)->getType()->getVectorNumElements();
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto elVecTy = V(val)->getType();
	auto elVecPtrTy = elVecTy->getPointerTo();
	auto i8Mask = jit->builder->CreateIntCast(V(mask), ::llvm::VectorType::get(i1Ty, numEls), false);  // vec<int, int, ...> -> vec<bool, bool, ...>
	auto align = ::llvm::ConstantInt::get(i32Ty, alignment);
	auto func = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::masked_store, { elVecTy, elVecPtrTy });
	jit->builder->CreateCall(func, { V(val), V(ptr), align, i8Mask });
}

RValue<Float4> Gather(RValue<Pointer<Float>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	return As<Float4>(V(createGather(V(base.value), T(Float::getType()), V(offsets.value), V(mask.value), alignment, zeroMaskedLanes)));
}

RValue<Int4> Gather(RValue<Pointer<Int>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	return As<Int4>(V(createGather(V(base.value), T(Float::getType()), V(offsets.value), V(mask.value), alignment, zeroMaskedLanes)));
}

void Scatter(RValue<Pointer<Float>> base, RValue<Float4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	return createScatter(V(base.value), V(val.value), V(offsets.value), V(mask.value), alignment);
}

void Scatter(RValue<Pointer<Int>> base, RValue<Int4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	return createScatter(V(base.value), V(val.value), V(offsets.value), V(mask.value), alignment);
}

void Nucleus::createFence(std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	jit->builder->CreateFence(atomicOrdering(true, memoryOrder));
}

Value *Nucleus::createGEP(Value *ptr, Type *type, Value *index, bool unsignedIndex)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(V(ptr)->getType()->getContainedType(0) == T(type));
	if(sizeof(void *) == 8)
	{
		// LLVM manual: "When indexing into an array, pointer or vector,
		// integers of any width are allowed, and they are not required to
		// be constant. These integers are treated as signed values where
		// relevant."
		//
		// Thus if we want indexes to be treated as unsigned we have to
		// zero-extend them ourselves.
		//
		// Note that this is not because we want to address anywhere near
		// 4 GB of data. Instead this is important for performance because
		// x86 supports automatic zero-extending of 32-bit registers to
		// 64-bit. Thus when indexing into an array using a uint32 is
		// actually faster than an int32.
		index = unsignedIndex ? createZExt(index, Long::getType()) : createSExt(index, Long::getType());
	}

	// For non-emulated types we can rely on LLVM's GEP to calculate the
	// effective address correctly.
	if(asInternalType(type) == Type_LLVM)
	{
		return V(jit->builder->CreateGEP(V(ptr), V(index)));
	}

	// For emulated types we have to multiply the index by the intended
	// type size ourselves to obain the byte offset.
	index = (sizeof(void *) == 8) ? createMul(index, createConstantLong((int64_t)typeSize(type))) : createMul(index, createConstantInt((int)typeSize(type)));

	// Cast to a byte pointer, apply the byte offset, and cast back to the
	// original pointer type.
	return createBitCast(
	    V(jit->builder->CreateGEP(V(createBitCast(ptr, T(llvm::PointerType::get(T(Byte::getType()), 0)))), V(index))),
	    T(llvm::PointerType::get(T(type), 0)));
}

Value *Nucleus::createAtomicAdd(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicSub(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicAnd(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::And, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicOr(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Or, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicXor(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Xor, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicMin(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Min, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicMax(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Max, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicUMin(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::UMin, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicUMax(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::UMax, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicExchange(Value *ptr, Value *value, std::memory_order memoryOrder)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateAtomicRMW(llvm::AtomicRMWInst::Xchg, V(ptr), V(value), atomicOrdering(true, memoryOrder)));
}

Value *Nucleus::createAtomicCompareExchange(Value *ptr, Value *value, Value *compare, std::memory_order memoryOrderEqual, std::memory_order memoryOrderUnequal)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// Note: AtomicCmpXchgInstruction returns a 2-member struct containing {result, success-flag}, not the result directly.
	return V(jit->builder->CreateExtractValue(
	    jit->builder->CreateAtomicCmpXchg(V(ptr), V(compare), V(value), atomicOrdering(true, memoryOrderEqual), atomicOrdering(true, memoryOrderUnequal)),
	    llvm::ArrayRef<unsigned>(0u)));
}

Value *Nucleus::createTrunc(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateTrunc(V(v), T(destType)));
}

Value *Nucleus::createZExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateZExt(V(v), T(destType)));
}

Value *Nucleus::createSExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSExt(V(v), T(destType)));
}

Value *Nucleus::createFPToUI(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFPToUI(V(v), T(destType)));
}

Value *Nucleus::createFPToSI(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFPToSI(V(v), T(destType)));
}

Value *Nucleus::createSIToFP(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSIToFP(V(v), T(destType)));
}

Value *Nucleus::createFPTrunc(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFPTrunc(V(v), T(destType)));
}

Value *Nucleus::createFPExt(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFPExt(V(v), T(destType)));
}

Value *Nucleus::createBitCast(Value *v, Type *destType)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// Bitcasts must be between types of the same logical size. But with emulated narrow vectors we need
	// support for casting between scalars and wide vectors. Emulate them by writing to the stack and
	// reading back as the destination type.
	if(!V(v)->getType()->isVectorTy() && T(destType)->isVectorTy())
	{
		Value *readAddress = allocateStackVariable(destType);
		Value *writeAddress = createBitCast(readAddress, T(llvm::PointerType::get(V(v)->getType(), 0)));
		createStore(v, writeAddress, T(V(v)->getType()));
		return createLoad(readAddress, destType);
	}
	else if(V(v)->getType()->isVectorTy() && !T(destType)->isVectorTy())
	{
		Value *writeAddress = allocateStackVariable(T(V(v)->getType()));
		createStore(v, writeAddress, T(V(v)->getType()));
		Value *readAddress = createBitCast(writeAddress, T(llvm::PointerType::get(T(destType), 0)));
		return createLoad(readAddress, destType);
	}

	return V(jit->builder->CreateBitCast(V(v), T(destType)));
}

Value *Nucleus::createPtrEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpEQ(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpEQ(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpNE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpNE(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpUGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpUGT(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpUGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpUGE(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpULT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpULT(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpULE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpULE(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpSGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpSGT(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpSGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpSGE(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpSLT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpSLT(V(lhs), V(rhs)));
}

Value *Nucleus::createICmpSLE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateICmpSLE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpOEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpOEQ(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpOGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpOGT(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpOGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpOGE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpOLT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpOLT(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpOLE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpOLE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpONE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpONE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpORD(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpORD(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpUNO(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpUNO(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpUEQ(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpUEQ(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpUGT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpUGT(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpUGE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpUGE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpULT(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpULT(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpULE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpULE(V(lhs), V(rhs)));
}

Value *Nucleus::createFCmpUNE(Value *lhs, Value *rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateFCmpUNE(V(lhs), V(rhs)));
}

Value *Nucleus::createExtractElement(Value *vector, Type *type, int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(V(vector)->getType()->getContainedType(0) == T(type));
	return V(jit->builder->CreateExtractElement(V(vector), V(createConstantInt(index))));
}

Value *Nucleus::createInsertElement(Value *vector, Value *element, int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateInsertElement(V(vector), V(element), V(createConstantInt(index))));
}

Value *Nucleus::createShuffleVector(Value *v1, Value *v2, const int *select)
{
	RR_DEBUG_INFO_UPDATE_LOC();

	int size = llvm::cast<llvm::VectorType>(V(v1)->getType())->getNumElements();
	const int maxSize = 16;
	llvm::Constant *swizzle[maxSize];
	ASSERT(size <= maxSize);

	for(int i = 0; i < size; i++)
	{
		swizzle[i] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), select[i]);
	}

	llvm::Value *shuffle = llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant *>(swizzle, size));

	return V(jit->builder->CreateShuffleVector(V(v1), V(v2), shuffle));
}

Value *Nucleus::createSelect(Value *c, Value *ifTrue, Value *ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(jit->builder->CreateSelect(V(c), V(ifTrue), V(ifFalse)));
}

SwitchCases *Nucleus::createSwitch(Value *control, BasicBlock *defaultBranch, unsigned numCases)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return reinterpret_cast<SwitchCases *>(jit->builder->CreateSwitch(V(control), B(defaultBranch), numCases));
}

void Nucleus::addSwitchCase(SwitchCases *switchCases, int label, BasicBlock *branch)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	llvm::SwitchInst *sw = reinterpret_cast<llvm::SwitchInst *>(switchCases);
	sw->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), label, true), B(branch));
}

void Nucleus::createUnreachable()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	jit->builder->CreateUnreachable();
}

Type *Nucleus::getType(Value *value)
{
	return T(V(value)->getType());
}

Type *Nucleus::getContainedType(Type *vectorType)
{
	return T(T(vectorType)->getContainedType(0));
}

Type *Nucleus::getPointerType(Type *ElementType)
{
	return T(llvm::PointerType::get(T(ElementType), 0));
}

static ::llvm::Type *getNaturalIntType()
{
	return ::llvm::Type::getIntNTy(jit->context, sizeof(int) * 8);
}

Type *Nucleus::getPrintfStorageType(Type *valueType)
{
	llvm::Type *valueTy = T(valueType);
	if(valueTy->isIntegerTy())
	{
		return T(getNaturalIntType());
	}
	if(valueTy->isFloatTy())
	{
		return T(llvm::Type::getDoubleTy(jit->context));
	}

	UNIMPLEMENTED_NO_BUG("getPrintfStorageType: add more cases as needed");
	return {};
}

Value *Nucleus::createNullValue(Type *Ty)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::Constant::getNullValue(T(Ty)));
}

Value *Nucleus::createConstantLong(int64_t i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt64Ty(jit->context), i, true));
}

Value *Nucleus::createConstantInt(int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), i, true));
}

Value *Nucleus::createConstantInt(unsigned int i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt32Ty(jit->context), i, false));
}

Value *Nucleus::createConstantBool(bool b)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt1Ty(jit->context), b));
}

Value *Nucleus::createConstantByte(signed char i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(jit->context), i, true));
}

Value *Nucleus::createConstantByte(unsigned char i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt8Ty(jit->context), i, false));
}

Value *Nucleus::createConstantShort(short i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(jit->context), i, true));
}

Value *Nucleus::createConstantShort(unsigned short i)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantInt::get(llvm::Type::getInt16Ty(jit->context), i, false));
}

Value *Nucleus::createConstantFloat(float x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantFP::get(T(Float::getType()), x));
}

Value *Nucleus::createNullPointer(Type *Ty)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return V(llvm::ConstantPointerNull::get(llvm::PointerType::get(T(Ty), 0)));
}

Value *Nucleus::createConstantVector(const int64_t *constants, Type *type)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(llvm::isa<llvm::VectorType>(T(type)));
	const int numConstants = elementCount(type);                                      // Number of provided constants for the (emulated) type.
	const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();  // Number of elements of the underlying vector type.
	ASSERT(numElements <= 16 && numConstants <= numElements);
	llvm::Constant *constantVector[16];

	for(int i = 0; i < numElements; i++)
	{
		constantVector[i] = llvm::ConstantInt::get(T(type)->getContainedType(0), constants[i % numConstants]);
	}

	return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant *>(constantVector, numElements)));
}

Value *Nucleus::createConstantVector(const double *constants, Type *type)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	ASSERT(llvm::isa<llvm::VectorType>(T(type)));
	const int numConstants = elementCount(type);                                      // Number of provided constants for the (emulated) type.
	const int numElements = llvm::cast<llvm::VectorType>(T(type))->getNumElements();  // Number of elements of the underlying vector type.
	ASSERT(numElements <= 8 && numConstants <= numElements);
	llvm::Constant *constantVector[8];

	for(int i = 0; i < numElements; i++)
	{
		constantVector[i] = llvm::ConstantFP::get(T(type)->getContainedType(0), constants[i % numConstants]);
	}

	return V(llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant *>(constantVector, numElements)));
}

Value *Nucleus::createConstantString(const char *v)
{
	// NOTE: Do not call RR_DEBUG_INFO_UPDATE_LOC() here to avoid recursion when called from rr::Printv
	auto ptr = jit->builder->CreateGlobalStringPtr(v);
	return V(ptr);
}

Type *Void::getType()
{
	return T(llvm::Type::getVoidTy(jit->context));
}

Type *Bool::getType()
{
	return T(llvm::Type::getInt1Ty(jit->context));
}

Type *Byte::getType()
{
	return T(llvm::Type::getInt8Ty(jit->context));
}

Type *SByte::getType()
{
	return T(llvm::Type::getInt8Ty(jit->context));
}

Type *Short::getType()
{
	return T(llvm::Type::getInt16Ty(jit->context));
}

Type *UShort::getType()
{
	return T(llvm::Type::getInt16Ty(jit->context));
}

Type *Byte4::getType()
{
	return T(Type_v4i8);
}

Type *SByte4::getType()
{
	return T(Type_v4i8);
}

RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::paddusb(x, y);
#else
	return As<Byte8>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
}

RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psubusb(x, y);
#else
	return As<Byte8>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
}

RValue<Int> SignMask(RValue<Byte8> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmovmskb(x);
#else
	return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
}

//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y)
//	{
//#if defined(__i386__) || defined(__x86_64__)
//		return x86::pcmpgtb(x, y);   // FIXME: Signedness
//#else
//		return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
//#endif
//	}

RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pcmpeqb(x, y);
#else
	return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
}

Type *Byte8::getType()
{
	return T(Type_v8i8);
}

RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::paddsb(x, y);
#else
	return As<SByte8>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
}

RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psubsb(x, y);
#else
	return As<SByte8>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
}

RValue<Int> SignMask(RValue<SByte8> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmovmskb(As<Byte8>(x));
#else
	return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
}

RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pcmpgtb(x, y);
#else
	return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
}

RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pcmpeqb(As<Byte8>(x), As<Byte8>(y));
#else
	return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
#endif
}

Type *SByte8::getType()
{
	return T(Type_v8i8);
}

Type *Byte16::getType()
{
	return T(llvm::VectorType::get(T(Byte::getType()), 16));
}

Type *SByte16::getType()
{
	return T(llvm::VectorType::get(T(SByte::getType()), 16));
}

Type *Short2::getType()
{
	return T(Type_v2i16);
}

Type *UShort2::getType()
{
	return T(Type_v2i16);
}

Short4::Short4(RValue<Int4> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	int select[8] = { 0, 2, 4, 6, 0, 2, 4, 6 };
	Value *short8 = Nucleus::createBitCast(cast.value, Short8::getType());

	Value *packed = Nucleus::createShuffleVector(short8, short8, select);
	Value *short4 = As<Short4>(Int2(As<Int4>(packed))).value;

	storeValue(short4);
}

//	Short4::Short4(RValue<Float> cast)
//	{
//	}

Short4::Short4(RValue<Float4> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Int4 v4i32 = Int4(cast);
#if defined(__i386__) || defined(__x86_64__)
	v4i32 = As<Int4>(x86::packssdw(v4i32, v4i32));
#else
	Value *v = v4i32.loadValue();
	v4i32 = As<Int4>(V(lowerPack(V(v), V(v), true)));
#endif

	storeValue(As<Short4>(Int2(v4i32)).value);
}

RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

	return x86::psllw(lhs, rhs);
#else
	return As<Short4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psraw(lhs, rhs);
#else
	return As<Short4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
}

RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmaxsw(x, y);
#else
	return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
#endif
}

RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pminsw(x, y);
#else
	return RValue<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
#endif
}

RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::paddsw(x, y);
#else
	return As<Short4>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#endif
}

RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psubsw(x, y);
#else
	return As<Short4>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#endif
}

RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmulhw(x, y);
#else
	return As<Short4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
#endif
}

RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmaddwd(x, y);
#else
	return As<Int2>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
}

RValue<SByte8> PackSigned(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	auto result = x86::packsswb(x, y);
#else
	auto result = V(lowerPack(V(x.value), V(y.value), true));
#endif
	return As<SByte8>(Swizzle(As<Int4>(result), 0x0202));
}

RValue<Byte8> PackUnsigned(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	auto result = x86::packuswb(x, y);
#else
	auto result = V(lowerPack(V(x.value), V(y.value), false));
#endif
	return As<Byte8>(Swizzle(As<Int4>(result), 0x0202));
}

RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pcmpgtw(x, y);
#else
	return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
#endif
}

RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pcmpeqw(x, y);
#else
	return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Short4::getType()))));
#endif
}

Type *Short4::getType()
{
	return T(Type_v4i16);
}

UShort4::UShort4(RValue<Float4> cast, bool saturate)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	if(saturate)
	{
#if defined(__i386__) || defined(__x86_64__)
		if(CPUID::supportsSSE4_1())
		{
			Int4 int4(Min(cast, Float4(0xFFFF)));  // packusdw takes care of 0x0000 saturation
			*this = As<Short4>(PackUnsigned(int4, int4));
		}
		else
#endif
		{
			*this = Short4(Int4(Max(Min(cast, Float4(0xFFFF)), Float4(0x0000))));
		}
	}
	else
	{
		*this = Short4(Int4(cast));
	}
}

RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createShl(lhs.value, rhs.value));

	return As<UShort4>(x86::psllw(As<Short4>(lhs), rhs));
#else
	return As<UShort4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Short4>(Nucleus::createLShr(lhs.value, rhs.value));

	return x86::psrlw(lhs, rhs);
#else
	return As<UShort4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
}

RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UShort4>(Max(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
}

RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UShort4>(Min(As<Short4>(x) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u), As<Short4>(y) - Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u)) + Short4(0x8000u, 0x8000u, 0x8000u, 0x8000u));
}

RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::paddusw(x, y);
#else
	return As<UShort4>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#endif
}

RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psubusw(x, y);
#else
	return As<UShort4>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#endif
}

RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmulhuw(x, y);
#else
	return As<UShort4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
#endif
}

RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pavgw(x, y);
#else
	return As<UShort4>(V(lowerPAVG(V(x.value), V(y.value))));
#endif
}

Type *UShort4::getType()
{
	return T(Type_v4i16);
}

RValue<Short8> operator<<(RValue<Short8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psllw(lhs, rhs);
#else
	return As<Short8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psraw(lhs, rhs);
#else
	return As<Short8>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
}

RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmaddwd(x, y);
#else
	return As<Int4>(V(lowerMulAdd(V(x.value), V(y.value))));
#endif
}

RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmulhw(x, y);
#else
	return As<Short8>(V(lowerMulHigh(V(x.value), V(y.value), true)));
#endif
}

Type *Short8::getType()
{
	return T(llvm::VectorType::get(T(Short::getType()), 8));
}

RValue<UShort8> operator<<(RValue<UShort8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return As<UShort8>(x86::psllw(As<Short8>(lhs), rhs));
#else
	return As<UShort8>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psrlw(lhs, rhs);  // FIXME: Fallback required
#else
	return As<UShort8>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
}

RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pmulhuw(x, y);
#else
	return As<UShort8>(V(lowerMulHigh(V(x.value), V(y.value), false)));
#endif
}

Type *UShort8::getType()
{
	return T(llvm::VectorType::get(T(UShort::getType()), 8));
}

RValue<Int> operator++(Int &val, int)  // Post-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<Int> res = val;

	Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return res;
}

const Int &operator++(Int &val)  // Pre-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return val;
}

RValue<Int> operator--(Int &val, int)  // Post-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<Int> res = val;

	Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return res;
}

const Int &operator--(Int &val)  // Pre-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return val;
}

RValue<Int> RoundInt(RValue<Float> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::cvtss2si(cast);
#else
	return RValue<Int>(V(lowerRoundInt(V(cast.value), T(Int::getType()))));
#endif
}

Type *Int::getType()
{
	return T(llvm::Type::getInt32Ty(jit->context));
}

Type *Long::getType()
{
	return T(llvm::Type::getInt64Ty(jit->context));
}

UInt::UInt(RValue<Float> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *integer = Nucleus::createFPToUI(cast.value, UInt::getType());
	storeValue(integer);
}

RValue<UInt> operator++(UInt &val, int)  // Post-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<UInt> res = val;

	Value *inc = Nucleus::createAdd(res.value, Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return res;
}

const UInt &operator++(UInt &val)  // Pre-increment
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *inc = Nucleus::createAdd(val.loadValue(), Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return val;
}

RValue<UInt> operator--(UInt &val, int)  // Post-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	RValue<UInt> res = val;

	Value *inc = Nucleus::createSub(res.value, Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return res;
}

const UInt &operator--(UInt &val)  // Pre-decrement
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *inc = Nucleus::createSub(val.loadValue(), Nucleus::createConstantInt(1));
	val.storeValue(inc);

	return val;
}

//	RValue<UInt> RoundUInt(RValue<Float> cast)
//	{
//#if defined(__i386__) || defined(__x86_64__)
//		return x86::cvtss2si(val);   // FIXME: Unsigned
//#else
//		return IfThenElse(cast > 0.0f, Int(cast + 0.5f), Int(cast - 0.5f));
//#endif
//	}

Type *UInt::getType()
{
	return T(llvm::Type::getInt32Ty(jit->context));
}

//	Int2::Int2(RValue<Int> cast)
//	{
//		Value *extend = Nucleus::createZExt(cast.value, Long::getType());
//		Value *vector = Nucleus::createBitCast(extend, Int2::getType());
//
//		int shuffle[2] = {0, 0};
//		Value *replicate = Nucleus::createShuffleVector(vector, vector, shuffle);
//
//		storeValue(replicate);
//	}

RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Int2>(Nucleus::createShl(lhs.value, rhs.value));

	return x86::pslld(lhs, rhs);
#else
	return As<Int2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<Int2>(Nucleus::createAShr(lhs.value, rhs.value));

	return x86::psrad(lhs, rhs);
#else
	return As<Int2>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
}

Type *Int2::getType()
{
	return T(Type_v2i32);
}

RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<UInt2>(Nucleus::createShl(lhs.value, rhs.value));

	return As<UInt2>(x86::pslld(As<Int2>(lhs), rhs));
#else
	return As<UInt2>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	//	return RValue<UInt2>(Nucleus::createLShr(lhs.value, rhs.value));

	return x86::psrld(lhs, rhs);
#else
	return As<UInt2>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
}

Type *UInt2::getType()
{
	return T(Type_v2i32);
}

Int4::Int4(RValue<Byte4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		*this = x86::pmovzxbd(As<Byte16>(cast));
	}
	else
#endif
	{
		int swizzle[16] = { 0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23 };
		Value *a = Nucleus::createBitCast(cast.value, Byte16::getType());
		Value *b = Nucleus::createShuffleVector(a, Nucleus::createNullValue(Byte16::getType()), swizzle);

		int swizzle2[8] = { 0, 8, 1, 9, 2, 10, 3, 11 };
		Value *c = Nucleus::createBitCast(b, Short8::getType());
		Value *d = Nucleus::createShuffleVector(c, Nucleus::createNullValue(Short8::getType()), swizzle2);

		*this = As<Int4>(d);
	}
}

Int4::Int4(RValue<SByte4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		*this = x86::pmovsxbd(As<SByte16>(cast));
	}
	else
#endif
	{
		int swizzle[16] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };
		Value *a = Nucleus::createBitCast(cast.value, Byte16::getType());
		Value *b = Nucleus::createShuffleVector(a, a, swizzle);

		int swizzle2[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };
		Value *c = Nucleus::createBitCast(b, Short8::getType());
		Value *d = Nucleus::createShuffleVector(c, c, swizzle2);

		*this = As<Int4>(d) >> 24;
	}
}

Int4::Int4(RValue<Short4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		*this = x86::pmovsxwd(As<Short8>(cast));
	}
	else
#endif
	{
		int swizzle[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };
		Value *c = Nucleus::createShuffleVector(cast.value, cast.value, swizzle);
		*this = As<Int4>(c) >> 16;
	}
}

Int4::Int4(RValue<UShort4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		*this = x86::pmovzxwd(As<UShort8>(cast));
	}
	else
#endif
	{
		int swizzle[8] = { 0, 8, 1, 9, 2, 10, 3, 11 };
		Value *c = Nucleus::createShuffleVector(cast.value, Short8(0, 0, 0, 0, 0, 0, 0, 0).loadValue(), swizzle);
		*this = As<Int4>(c);
	}
}

Int4::Int4(RValue<Int> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = loadValue();
	Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

	storeValue(replicate);
}

RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::pslld(lhs, rhs);
#else
	return As<Int4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psrad(lhs, rhs);
#else
	return As<Int4>(V(lowerVectorAShr(V(lhs.value), rhs)));
#endif
}

RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLT(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSLE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createICmpSGT(x.value, y.value), Int4::getType()));
}

RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::pmaxsd(x, y);
	}
	else
#endif
	{
		RValue<Int4> greater = CmpNLE(x, y);
		return (x & greater) | (y & ~greater);
	}
}

RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::pminsd(x, y);
	}
	else
#endif
	{
		RValue<Int4> less = CmpLT(x, y);
		return (x & less) | (y & ~less);
	}
}

RValue<Int4> RoundInt(RValue<Float4> cast)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::cvtps2dq(cast);
#else
	return As<Int4>(V(lowerRoundInt(V(cast.value), T(Int4::getType()))));
#endif
}

RValue<Int4> MulHigh(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
	return As<Int4>(V(lowerMulHigh(V(x.value), V(y.value), true)));
}

RValue<UInt4> MulHigh(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// TODO: For x86, build an intrinsics version of this which uses shuffles + pmuludq.
	return As<UInt4>(V(lowerMulHigh(V(x.value), V(y.value), false)));
}

RValue<Short8> PackSigned(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::packssdw(x, y);
#else
	return As<Short8>(V(lowerPack(V(x.value), V(y.value), true)));
#endif
}

RValue<UShort8> PackUnsigned(RValue<Int4> x, RValue<Int4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::packusdw(x, y);
#else
	return As<UShort8>(V(lowerPack(V(x.value), V(y.value), false)));
#endif
}

RValue<Int> SignMask(RValue<Int4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::movmskps(As<Float4>(x));
#else
	return As<Int>(V(lowerSignMask(V(x.value), T(Int::getType()))));
#endif
}

Type *Int4::getType()
{
	return T(llvm::VectorType::get(T(Int::getType()), 4));
}

UInt4::UInt4(RValue<Float4> cast)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *xyzw = Nucleus::createFPToUI(cast.value, UInt4::getType());
	storeValue(xyzw);
}

UInt4::UInt4(RValue<UInt> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = loadValue();
	Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

	storeValue(replicate);
}

RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return As<UInt4>(x86::pslld(As<Int4>(lhs), rhs));
#else
	return As<UInt4>(V(lowerVectorShl(V(lhs.value), rhs)));
#endif
}

RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::psrld(lhs, rhs);
#else
	return As<UInt4>(V(lowerVectorLShr(V(lhs.value), rhs)));
#endif
}

RValue<UInt4> CmpEQ(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpEQ(x.value, y.value), Int4::getType()));
}

RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULT(x.value, y.value), Int4::getType()));
}

RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpULE(x.value, y.value), Int4::getType()));
}

RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpNE(x.value, y.value), Int4::getType()));
}

RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGE(x.value, y.value), Int4::getType()));
}

RValue<UInt4> CmpNLE(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<UInt4>(Nucleus::createSExt(Nucleus::createICmpUGT(x.value, y.value), Int4::getType()));
}

RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::pmaxud(x, y);
	}
	else
#endif
	{
		RValue<UInt4> greater = CmpNLE(x, y);
		return (x & greater) | (y & ~greater);
	}
}

RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::pminud(x, y);
	}
	else
#endif
	{
		RValue<UInt4> less = CmpLT(x, y);
		return (x & less) | (y & ~less);
	}
}

Type *UInt4::getType()
{
	return T(llvm::VectorType::get(T(UInt::getType()), 4));
}

Type *Half::getType()
{
	return T(llvm::Type::getInt16Ty(jit->context));
}

RValue<Float> Rcp_pp(RValue<Float> x, bool exactAtPow2)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(exactAtPow2)
	{
		// rcpss uses a piecewise-linear approximation which minimizes the relative error
		// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
		return x86::rcpss(x) * Float(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
	}
	return x86::rcpss(x);
#else
	return As<Float>(V(lowerRCP(V(x.value))));
#endif
}

RValue<Float> RcpSqrt_pp(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::rsqrtss(x);
#else
	return As<Float>(V(lowerRSQRT(V(x.value))));
#endif
}

RValue<Float> Sqrt(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::sqrtss(x);
#else
	return As<Float>(V(lowerSQRT(V(x.value))));
#endif
}

RValue<Float> Round(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::roundss(x, 0);
	}
	else
	{
		return Float4(Round(Float4(x))).x;
	}
#else
	return RValue<Float>(V(lowerRound(V(x.value))));
#endif
}

RValue<Float> Trunc(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::roundss(x, 3);
	}
	else
	{
		return Float(Int(x));  // Rounded toward zero
	}
#else
	return RValue<Float>(V(lowerTrunc(V(x.value))));
#endif
}

RValue<Float> Frac(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x - x86::floorss(x);
	}
	else
	{
		return Float4(Frac(Float4(x))).x;
	}
#else
	// x - floor(x) can be 1.0 for very small negative x.
	// Clamp against the value just below 1.0.
	return Min(x - Floor(x), As<Float>(Int(0x3F7FFFFF)));
#endif
}

RValue<Float> Floor(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::floorss(x);
	}
	else
	{
		return Float4(Floor(Float4(x))).x;
	}
#else
	return RValue<Float>(V(lowerFloor(V(x.value))));
#endif
}

RValue<Float> Ceil(RValue<Float> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::ceilss(x);
	}
	else
#endif
	{
		return Float4(Ceil(Float4(x))).x;
	}
}

Type *Float::getType()
{
	return T(llvm::Type::getFloatTy(jit->context));
}

Type *Float2::getType()
{
	return T(Type_v2f32);
}

RValue<Float> Exp2(RValue<Float> v)
{
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp2, { T(Float::getType()) });
	return RValue<Float>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float> Log2(RValue<Float> v)
{
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log2, { T(Float::getType()) });
	return RValue<Float>(V(jit->builder->CreateCall(func, V(v.value))));
}

Float4::Float4(RValue<Float> rhs)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = loadValue();
	Value *insert = Nucleus::createInsertElement(vector, rhs.value, 0);

	int swizzle[4] = { 0, 0, 0, 0 };
	Value *replicate = Nucleus::createShuffleVector(insert, insert, swizzle);

	storeValue(replicate);
}

RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::maxps(x, y);
#else
	return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OGT)));
#endif
}

RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::minps(x, y);
#else
	return As<Float4>(V(lowerPFMINMAX(V(x.value), V(y.value), llvm::FCmpInst::FCMP_OLT)));
#endif
}

RValue<Float4> Rcp_pp(RValue<Float4> x, bool exactAtPow2)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(exactAtPow2)
	{
		// rcpps uses a piecewise-linear approximation which minimizes the relative error
		// but is not exact at power-of-two values. Rectify by multiplying by the inverse.
		return x86::rcpps(x) * Float4(1.0f / _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ps1(1.0f))));
	}
	return x86::rcpps(x);
#else
	return As<Float4>(V(lowerRCP(V(x.value))));
#endif
}

RValue<Float4> RcpSqrt_pp(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::rsqrtps(x);
#else
	return As<Float4>(V(lowerRSQRT(V(x.value))));
#endif
}

RValue<Float4> Sqrt(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::sqrtps(x);
#else
	return As<Float4>(V(lowerSQRT(V(x.value))));
#endif
}

RValue<Int> SignMask(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	return x86::movmskps(x);
#else
	return As<Int>(V(lowerFPSignMask(V(x.value), T(Int::getType()))));
#endif
}

RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpeqps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOEQ(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpltps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLT(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpleps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOLE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpneqps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpONE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpnltps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	//	return As<Int4>(x86::cmpnleps(x, y));
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpOGT(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpUEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUEQ(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpULT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULT(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpULE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpULE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpUNEQ(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUNE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpUNLT(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGE(x.value, y.value), Int4::getType()));
}

RValue<Int4> CmpUNLE(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<Int4>(Nucleus::createSExt(Nucleus::createFCmpUGT(x.value, y.value), Int4::getType()));
}

RValue<Float4> Round(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::roundps(x, 0);
	}
	else
	{
		return Float4(RoundInt(x));
	}
#else
	return RValue<Float4>(V(lowerRound(V(x.value))));
#endif
}

RValue<Float4> Trunc(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::roundps(x, 3);
	}
	else
	{
		return Float4(Int4(x));
	}
#else
	return RValue<Float4>(V(lowerTrunc(V(x.value))));
#endif
}

RValue<Float4> Frac(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Float4 frc;

#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		frc = x - Floor(x);
	}
	else
	{
		frc = x - Float4(Int4(x));  // Signed fractional part.

		frc += As<Float4>(As<Int4>(CmpNLE(Float4(0.0f), frc)) & As<Int4>(Float4(1.0f)));  // Add 1.0 if negative.
	}
#else
	frc = x - Floor(x);
#endif

	// x - floor(x) can be 1.0 for very small negative x.
	// Clamp against the value just below 1.0.
	return Min(frc, As<Float4>(Int4(0x3F7FFFFF)));
}

RValue<Float4> Floor(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::floorps(x);
	}
	else
	{
		return x - Frac(x);
	}
#else
	return RValue<Float4>(V(lowerFloor(V(x.value))));
#endif
}

RValue<Float4> Ceil(RValue<Float4> x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
#if defined(__i386__) || defined(__x86_64__)
	if(CPUID::supportsSSE4_1())
	{
		return x86::ceilps(x);
	}
	else
#endif
	{
		return -Floor(-x);
	}
}

RValue<Float4> Sin(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sin, { V(v.value)->getType() });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float4> Cos(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cos, { V(v.value)->getType() });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float4> Tan(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Sin(v) / Cos(v);
}

static RValue<Float4> TransformFloat4PerElement(RValue<Float4> v, const char *name)
{
	auto funcTy = ::llvm::FunctionType::get(T(Float::getType()), ::llvm::ArrayRef<llvm::Type *>(T(Float::getType())), false);
	auto func = jit->module->getOrInsertFunction(name, funcTy);
	llvm::Value *out = ::llvm::UndefValue::get(T(Float4::getType()));
	for(uint64_t i = 0; i < 4; i++)
	{
		auto el = jit->builder->CreateCall(func, V(Nucleus::createExtractElement(v.value, Float::getType(), i)));
		out = V(Nucleus::createInsertElement(V(out), V(el), i));
	}
	return RValue<Float4>(V(out));
}

RValue<Float4> Asin(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "asinf");
}

RValue<Float4> Acos(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "acosf");
}

RValue<Float4> Atan(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "atanf");
}

RValue<Float4> Sinh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Sinh(v);
}

RValue<Float4> Cosh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return emulated::Cosh(v);
}

RValue<Float4> Tanh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "tanhf");
}

RValue<Float4> Asinh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "asinhf");
}

RValue<Float4> Acosh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "acoshf");
}

RValue<Float4> Atanh(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return TransformFloat4PerElement(v, "atanhf");
}

RValue<Float4> Atan2(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	::llvm::SmallVector<::llvm::Type *, 2> paramTys;
	paramTys.push_back(T(Float::getType()));
	paramTys.push_back(T(Float::getType()));
	auto funcTy = ::llvm::FunctionType::get(T(Float::getType()), paramTys, false);
	auto func = jit->module->getOrInsertFunction("atan2f", funcTy);
	llvm::Value *out = ::llvm::UndefValue::get(T(Float4::getType()));
	for(uint64_t i = 0; i < 4; i++)
	{
		auto el = jit->builder->CreateCall2(func, ARGS(
		                                              V(Nucleus::createExtractElement(x.value, Float::getType(), i)),
		                                              V(Nucleus::createExtractElement(y.value, Float::getType(), i))));
		out = V(Nucleus::createInsertElement(V(out), V(el), i));
	}
	return RValue<Float4>(V(out));
}

RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::pow, { T(Float4::getType()) });
	return RValue<Float4>(V(jit->builder->CreateCall2(func, ARGS(V(x.value), V(y.value)))));
}

RValue<Float4> Exp(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp, { T(Float4::getType()) });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float4> Log(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log, { T(Float4::getType()) });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float4> Exp2(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::exp2, { T(Float4::getType()) });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<Float4> Log2(RValue<Float4> v)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::log2, { T(Float4::getType()) });
	return RValue<Float4>(V(jit->builder->CreateCall(func, V(v.value))));
}

RValue<UInt> Ctlz(RValue<UInt> v, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::ctlz, { T(UInt::getType()) });
	return RValue<UInt>(V(jit->builder->CreateCall2(func, ARGS(
	                                                          V(v.value),
	                                                          isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)))));
}

RValue<UInt4> Ctlz(RValue<UInt4> v, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::ctlz, { T(UInt4::getType()) });
	return RValue<UInt4>(V(jit->builder->CreateCall2(func, ARGS(
	                                                           V(v.value),
	                                                           isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)))));
}

RValue<UInt> Cttz(RValue<UInt> v, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cttz, { T(UInt::getType()) });
	return RValue<UInt>(V(jit->builder->CreateCall2(func, ARGS(
	                                                          V(v.value),
	                                                          isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)))));
}

RValue<UInt4> Cttz(RValue<UInt4> v, bool isZeroUndef)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto func = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::cttz, { T(UInt4::getType()) });
	return RValue<UInt4>(V(jit->builder->CreateCall2(func, ARGS(
	                                                           V(v.value),
	                                                           isZeroUndef ? ::llvm::ConstantInt::getTrue(jit->context) : ::llvm::ConstantInt::getFalse(jit->context)))));
}

RValue<Int> MinAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder)
{
	return RValue<Int>(Nucleus::createAtomicMin(x.value, y.value, memoryOrder));
}

RValue<UInt> MinAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder)
{
	return RValue<UInt>(Nucleus::createAtomicUMin(x.value, y.value, memoryOrder));
}

RValue<Int> MaxAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder)
{
	return RValue<Int>(Nucleus::createAtomicMax(x.value, y.value, memoryOrder));
}

RValue<UInt> MaxAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder)
{
	return RValue<UInt>(Nucleus::createAtomicUMax(x.value, y.value, memoryOrder));
}

Type *Float4::getType()
{
	return T(llvm::VectorType::get(T(Float::getType()), 4));
}

RValue<Long> Ticks()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	llvm::Function *rdtsc = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::readcyclecounter);

	return RValue<Long>(V(jit->builder->CreateCall(rdtsc)));
}

RValue<Pointer<Byte>> ConstantPointer(void const *ptr)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	// Note: this should work for 32-bit pointers as well because 'inttoptr'
	// is defined to truncate (and zero extend) if necessary.
	auto ptrAsInt = ::llvm::ConstantInt::get(::llvm::Type::getInt64Ty(jit->context), reinterpret_cast<uintptr_t>(ptr));
	return RValue<Pointer<Byte>>(V(jit->builder->CreateIntToPtr(ptrAsInt, T(Pointer<Byte>::getType()))));
}

RValue<Pointer<Byte>> ConstantData(void const *data, size_t size)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	auto str = ::std::string(reinterpret_cast<const char *>(data), size);
	auto ptr = jit->builder->CreateGlobalStringPtr(str);
	return RValue<Pointer<Byte>>(V(ptr));
}

Value *Call(RValue<Pointer<Byte>> fptr, Type *retTy, std::initializer_list<Value *> args, std::initializer_list<Type *> argTys)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	::llvm::SmallVector<::llvm::Type *, 8> paramTys;
	for(auto ty : argTys) { paramTys.push_back(T(ty)); }
	auto funcTy = ::llvm::FunctionType::get(T(retTy), paramTys, false);

	auto funcPtrTy = funcTy->getPointerTo();
	auto funcPtr = jit->builder->CreatePointerCast(V(fptr.value), funcPtrTy);

	::llvm::SmallVector<::llvm::Value *, 8> arguments;
	for(auto arg : args) { arguments.push_back(V(arg)); }
	return V(jit->builder->CreateCall(funcPtr, arguments));
}

void Breakpoint()
{
	RR_DEBUG_INFO_UPDATE_LOC();
	llvm::Function *debugtrap = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::debugtrap);

	jit->builder->CreateCall(debugtrap);
}

}  // namespace rr

namespace rr {

#if defined(__i386__) || defined(__x86_64__)
namespace x86 {

RValue<Int> cvtss2si(RValue<Float> val)
{
	llvm::Function *cvtss2si = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_cvtss2si);

	Float4 vector;
	vector.x = val;

	return RValue<Int>(V(jit->builder->CreateCall(cvtss2si, ARGS(V(RValue<Float4>(vector).value)))));
}

RValue<Int4> cvtps2dq(RValue<Float4> val)
{
	llvm::Function *cvtps2dq = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_cvtps2dq);

	return RValue<Int4>(V(jit->builder->CreateCall(cvtps2dq, ARGS(V(val.value)))));
}

RValue<Float> rcpss(RValue<Float> val)
{
	llvm::Function *rcpss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rcp_ss);

	Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

	return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall(rcpss, ARGS(V(vector)))), Float::getType(), 0));
}

RValue<Float> sqrtss(RValue<Float> val)
{
	llvm::Function *sqrt = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sqrt, { V(val.value)->getType() });
	return RValue<Float>(V(jit->builder->CreateCall(sqrt, ARGS(V(val.value)))));
}

RValue<Float> rsqrtss(RValue<Float> val)
{
	llvm::Function *rsqrtss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rsqrt_ss);

	Value *vector = Nucleus::createInsertElement(V(llvm::UndefValue::get(T(Float4::getType()))), val.value, 0);

	return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall(rsqrtss, ARGS(V(vector)))), Float::getType(), 0));
}

RValue<Float4> rcpps(RValue<Float4> val)
{
	llvm::Function *rcpps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rcp_ps);

	return RValue<Float4>(V(jit->builder->CreateCall(rcpps, ARGS(V(val.value)))));
}

RValue<Float4> sqrtps(RValue<Float4> val)
{
	llvm::Function *sqrtps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::sqrt, { V(val.value)->getType() });

	return RValue<Float4>(V(jit->builder->CreateCall(sqrtps, ARGS(V(val.value)))));
}

RValue<Float4> rsqrtps(RValue<Float4> val)
{
	llvm::Function *rsqrtps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_rsqrt_ps);

	return RValue<Float4>(V(jit->builder->CreateCall(rsqrtps, ARGS(V(val.value)))));
}

RValue<Float4> maxps(RValue<Float4> x, RValue<Float4> y)
{
	llvm::Function *maxps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_max_ps);

	return RValue<Float4>(V(jit->builder->CreateCall2(maxps, ARGS(V(x.value), V(y.value)))));
}

RValue<Float4> minps(RValue<Float4> x, RValue<Float4> y)
{
	llvm::Function *minps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_min_ps);

	return RValue<Float4>(V(jit->builder->CreateCall2(minps, ARGS(V(x.value), V(y.value)))));
}

RValue<Float> roundss(RValue<Float> val, unsigned char imm)
{
	llvm::Function *roundss = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_round_ss);

	Value *undef = V(llvm::UndefValue::get(T(Float4::getType())));
	Value *vector = Nucleus::createInsertElement(undef, val.value, 0);

	return RValue<Float>(Nucleus::createExtractElement(V(jit->builder->CreateCall3(roundss, ARGS(V(undef), V(vector), V(Nucleus::createConstantInt(imm))))), Float::getType(), 0));
}

RValue<Float> floorss(RValue<Float> val)
{
	return roundss(val, 1);
}

RValue<Float> ceilss(RValue<Float> val)
{
	return roundss(val, 2);
}

RValue<Float4> roundps(RValue<Float4> val, unsigned char imm)
{
	llvm::Function *roundps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_round_ps);

	return RValue<Float4>(V(jit->builder->CreateCall2(roundps, ARGS(V(val.value), V(Nucleus::createConstantInt(imm))))));
}

RValue<Float4> floorps(RValue<Float4> val)
{
	return roundps(val, 1);
}

RValue<Float4> ceilps(RValue<Float4> val)
{
	return roundps(val, 2);
}

RValue<Int4> pabsd(RValue<Int4> x)
{
	return RValue<Int4>(V(lowerPABS(V(x.value))));
}

RValue<Short4> paddsw(RValue<Short4> x, RValue<Short4> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<Short4>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *paddsw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_padds_w);

	return As<Short4>(V(jit->builder->CreateCall2(paddsw, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<Short4> psubsw(RValue<Short4> x, RValue<Short4> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<Short4>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *psubsw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubs_w);

	return As<Short4>(V(jit->builder->CreateCall2(psubsw, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<UShort4> paddusw(RValue<UShort4> x, RValue<UShort4> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<UShort4>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *paddusw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_paddus_w);

	return As<UShort4>(V(jit->builder->CreateCall2(paddusw, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<UShort4> psubusw(RValue<UShort4> x, RValue<UShort4> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<UShort4>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *psubusw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubus_w);

	return As<UShort4>(V(jit->builder->CreateCall2(psubusw, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<SByte8> paddsb(RValue<SByte8> x, RValue<SByte8> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<SByte8>(V(lowerPSADDSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *paddsb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_padds_b);

	return As<SByte8>(V(jit->builder->CreateCall2(paddsb, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<SByte8> psubsb(RValue<SByte8> x, RValue<SByte8> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<SByte8>(V(lowerPSSUBSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *psubsb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubs_b);

	return As<SByte8>(V(jit->builder->CreateCall2(psubsb, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<Byte8> paddusb(RValue<Byte8> x, RValue<Byte8> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<Byte8>(V(lowerPUADDSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *paddusb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_paddus_b);

	return As<Byte8>(V(jit->builder->CreateCall2(paddusb, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<Byte8> psubusb(RValue<Byte8> x, RValue<Byte8> y)
{
#	if LLVM_VERSION_MAJOR >= 8
	return As<Byte8>(V(lowerPUSUBSAT(V(x.value), V(y.value))));
#	else
	llvm::Function *psubusb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psubus_b);

	return As<Byte8>(V(jit->builder->CreateCall2(psubusb, ARGS(V(x.value), V(y.value)))));
#	endif
}

RValue<UShort4> pavgw(RValue<UShort4> x, RValue<UShort4> y)
{
	return As<UShort4>(V(lowerPAVG(V(x.value), V(y.value))));
}

RValue<Short4> pmaxsw(RValue<Short4> x, RValue<Short4> y)
{
	return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
}

RValue<Short4> pminsw(RValue<Short4> x, RValue<Short4> y)
{
	return As<Short4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
}

RValue<Short4> pcmpgtw(RValue<Short4> x, RValue<Short4> y)
{
	return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Short4::getType()))));
}

RValue<Short4> pcmpeqw(RValue<Short4> x, RValue<Short4> y)
{
	return As<Short4>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Short4::getType()))));
}

RValue<Byte8> pcmpgtb(RValue<SByte8> x, RValue<SByte8> y)
{
	return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_SGT, V(x.value), V(y.value), T(Byte8::getType()))));
}

RValue<Byte8> pcmpeqb(RValue<Byte8> x, RValue<Byte8> y)
{
	return As<Byte8>(V(lowerPCMP(llvm::ICmpInst::ICMP_EQ, V(x.value), V(y.value), T(Byte8::getType()))));
}

RValue<Short4> packssdw(RValue<Int2> x, RValue<Int2> y)
{
	llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packssdw_128);

	return As<Short4>(V(jit->builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
}

RValue<Short8> packssdw(RValue<Int4> x, RValue<Int4> y)
{
	llvm::Function *packssdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packssdw_128);

	return RValue<Short8>(V(jit->builder->CreateCall2(packssdw, ARGS(V(x.value), V(y.value)))));
}

RValue<SByte8> packsswb(RValue<Short4> x, RValue<Short4> y)
{
	llvm::Function *packsswb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packsswb_128);

	return As<SByte8>(V(jit->builder->CreateCall2(packsswb, ARGS(V(x.value), V(y.value)))));
}

RValue<Byte8> packuswb(RValue<Short4> x, RValue<Short4> y)
{
	llvm::Function *packuswb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_packuswb_128);

	return As<Byte8>(V(jit->builder->CreateCall2(packuswb, ARGS(V(x.value), V(y.value)))));
}

RValue<UShort8> packusdw(RValue<Int4> x, RValue<Int4> y)
{
	if(CPUID::supportsSSE4_1())
	{
		llvm::Function *packusdw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse41_packusdw);

		return RValue<UShort8>(V(jit->builder->CreateCall2(packusdw, ARGS(V(x.value), V(y.value)))));
	}
	else
	{
		RValue<Int4> bx = (x & ~(x >> 31)) - Int4(0x8000);
		RValue<Int4> by = (y & ~(y >> 31)) - Int4(0x8000);

		return As<UShort8>(packssdw(bx, by) + Short8(0x8000u));
	}
}

RValue<UShort4> psrlw(RValue<UShort4> x, unsigned char y)
{
	llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_w);

	return As<UShort4>(V(jit->builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<UShort8> psrlw(RValue<UShort8> x, unsigned char y)
{
	llvm::Function *psrlw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_w);

	return RValue<UShort8>(V(jit->builder->CreateCall2(psrlw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Short4> psraw(RValue<Short4> x, unsigned char y)
{
	llvm::Function *psraw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_w);

	return As<Short4>(V(jit->builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Short8> psraw(RValue<Short8> x, unsigned char y)
{
	llvm::Function *psraw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_w);

	return RValue<Short8>(V(jit->builder->CreateCall2(psraw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Short4> psllw(RValue<Short4> x, unsigned char y)
{
	llvm::Function *psllw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_w);

	return As<Short4>(V(jit->builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Short8> psllw(RValue<Short8> x, unsigned char y)
{
	llvm::Function *psllw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_w);

	return RValue<Short8>(V(jit->builder->CreateCall2(psllw, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Int2> pslld(RValue<Int2> x, unsigned char y)
{
	llvm::Function *pslld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_d);

	return As<Int2>(V(jit->builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Int4> pslld(RValue<Int4> x, unsigned char y)
{
	llvm::Function *pslld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pslli_d);

	return RValue<Int4>(V(jit->builder->CreateCall2(pslld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Int2> psrad(RValue<Int2> x, unsigned char y)
{
	llvm::Function *psrad = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_d);

	return As<Int2>(V(jit->builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Int4> psrad(RValue<Int4> x, unsigned char y)
{
	llvm::Function *psrad = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrai_d);

	return RValue<Int4>(V(jit->builder->CreateCall2(psrad, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<UInt2> psrld(RValue<UInt2> x, unsigned char y)
{
	llvm::Function *psrld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_d);

	return As<UInt2>(V(jit->builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<UInt4> psrld(RValue<UInt4> x, unsigned char y)
{
	llvm::Function *psrld = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_psrli_d);

	return RValue<UInt4>(V(jit->builder->CreateCall2(psrld, ARGS(V(x.value), V(Nucleus::createConstantInt(y))))));
}

RValue<Int4> pmaxsd(RValue<Int4> x, RValue<Int4> y)
{
	return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SGT)));
}

RValue<Int4> pminsd(RValue<Int4> x, RValue<Int4> y)
{
	return RValue<Int4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_SLT)));
}

RValue<UInt4> pmaxud(RValue<UInt4> x, RValue<UInt4> y)
{
	return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_UGT)));
}

RValue<UInt4> pminud(RValue<UInt4> x, RValue<UInt4> y)
{
	return RValue<UInt4>(V(lowerPMINMAX(V(x.value), V(y.value), llvm::ICmpInst::ICMP_ULT)));
}

RValue<Short4> pmulhw(RValue<Short4> x, RValue<Short4> y)
{
	llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulh_w);

	return As<Short4>(V(jit->builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
}

RValue<UShort4> pmulhuw(RValue<UShort4> x, RValue<UShort4> y)
{
	llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulhu_w);

	return As<UShort4>(V(jit->builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
}

RValue<Int2> pmaddwd(RValue<Short4> x, RValue<Short4> y)
{
	llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmadd_wd);

	return As<Int2>(V(jit->builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
}

RValue<Short8> pmulhw(RValue<Short8> x, RValue<Short8> y)
{
	llvm::Function *pmulhw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulh_w);

	return RValue<Short8>(V(jit->builder->CreateCall2(pmulhw, ARGS(V(x.value), V(y.value)))));
}

RValue<UShort8> pmulhuw(RValue<UShort8> x, RValue<UShort8> y)
{
	llvm::Function *pmulhuw = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmulhu_w);

	return RValue<UShort8>(V(jit->builder->CreateCall2(pmulhuw, ARGS(V(x.value), V(y.value)))));
}

RValue<Int4> pmaddwd(RValue<Short8> x, RValue<Short8> y)
{
	llvm::Function *pmaddwd = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmadd_wd);

	return RValue<Int4>(V(jit->builder->CreateCall2(pmaddwd, ARGS(V(x.value), V(y.value)))));
}

RValue<Int> movmskps(RValue<Float4> x)
{
	llvm::Function *movmskps = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse_movmsk_ps);

	return RValue<Int>(V(jit->builder->CreateCall(movmskps, ARGS(V(x.value)))));
}

RValue<Int> pmovmskb(RValue<Byte8> x)
{
	llvm::Function *pmovmskb = llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::x86_sse2_pmovmskb_128);

	return RValue<Int>(V(jit->builder->CreateCall(pmovmskb, ARGS(V(x.value))))) & 0xFF;
}

RValue<Int4> pmovzxbd(RValue<Byte16> x)
{
	return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
}

RValue<Int4> pmovsxbd(RValue<SByte16> x)
{
	return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
}

RValue<Int4> pmovzxwd(RValue<UShort8> x)
{
	return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), false)));
}

RValue<Int4> pmovsxwd(RValue<Short8> x)
{
	return RValue<Int4>(V(lowerPMOV(V(x.value), T(Int4::getType()), true)));
}

}  // namespace x86
#endif  // defined(__i386__) || defined(__x86_64__)

#ifdef ENABLE_RR_PRINT
void VPrintf(const std::vector<Value *> &vals)
{
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
	auto funcTy = ::llvm::FunctionType::get(i32Ty, { i8PtrTy }, true);
	auto func = jit->module->getOrInsertFunction("printf", funcTy);
	jit->builder->CreateCall(func, V(vals));
}
#endif  // ENABLE_RR_PRINT

void Nop()
{
	auto voidTy = ::llvm::Type::getVoidTy(jit->context);
	auto funcTy = ::llvm::FunctionType::get(voidTy, {}, false);
	auto func = jit->module->getOrInsertFunction("nop", funcTy);
	jit->builder->CreateCall(func);
}

void EmitDebugLocation()
{
#ifdef ENABLE_RR_DEBUG_INFO
	if(jit->debugInfo != nullptr)
	{
		jit->debugInfo->EmitLocation();
	}
#endif  // ENABLE_RR_DEBUG_INFO
}

void EmitDebugVariable(Value *value)
{
#ifdef ENABLE_RR_DEBUG_INFO
	if(jit->debugInfo != nullptr)
	{
		jit->debugInfo->EmitVariable(value);
	}
#endif  // ENABLE_RR_DEBUG_INFO
}

void FlushDebug()
{
#ifdef ENABLE_RR_DEBUG_INFO
	if(jit->debugInfo != nullptr)
	{
		jit->debugInfo->Flush();
	}
#endif  // ENABLE_RR_DEBUG_INFO
}

}  // namespace rr

// ------------------------------  Coroutines ------------------------------

namespace {

// Magic values retuned by llvm.coro.suspend.
// See: https://llvm.org/docs/Coroutines.html#llvm-coro-suspend-intrinsic
enum SuspendAction
{
	SuspendActionSuspend = -1,
	SuspendActionResume = 0,
	SuspendActionDestroy = 1
};

void promoteFunctionToCoroutine()
{
	ASSERT(jit->coroutine.id == nullptr);

	// Types
	auto voidTy = ::llvm::Type::getVoidTy(jit->context);
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);
	auto i32Ty = ::llvm::Type::getInt32Ty(jit->context);
	auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
	auto promiseTy = jit->coroutine.yieldType;
	auto promisePtrTy = promiseTy->getPointerTo();

	// LLVM intrinsics
	auto coro_id = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_id);
	auto coro_size = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_size, { i32Ty });
	auto coro_begin = ::llvm::Intrinsic::getDeclaration(jit->module.get(), llvm::Intrinsic::coro_begin);
	auto coro_resume = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_resume);
	auto coro_end = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_end);
	auto coro_free = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_free);
	auto coro_destroy = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_destroy);
	auto coro_promise = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_promise);
	auto coro_done = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_done);
	auto coro_suspend = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_suspend);

	auto allocFrameTy = ::llvm::FunctionType::get(i8PtrTy, { i32Ty }, false);
	auto allocFrame = jit->module->getOrInsertFunction("coroutine_alloc_frame", allocFrameTy);
	auto freeFrameTy = ::llvm::FunctionType::get(voidTy, { i8PtrTy }, false);
	auto freeFrame = jit->module->getOrInsertFunction("coroutine_free_frame", freeFrameTy);

	auto oldInsertionPoint = jit->builder->saveIP();

	// Build the coroutine_await() function:
	//
	//    bool coroutine_await(CoroutineHandle* handle, YieldType* out)
	//    {
	//        if(llvm.coro.done(handle))
	//        {
	//            return false;
	//        }
	//        else
	//        {
	//            *value = (T*)llvm.coro.promise(handle);
	//            llvm.coro.resume(handle);
	//            return true;
	//        }
	//    }
	//
	{
		auto args = jit->coroutine.await->arg_begin();
		auto handle = args++;
		auto outPtr = args++;
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "co_await", jit->coroutine.await));
		auto doneBlock = llvm::BasicBlock::Create(jit->context, "done", jit->coroutine.await);
		auto resumeBlock = llvm::BasicBlock::Create(jit->context, "resume", jit->coroutine.await);

		auto done = jit->builder->CreateCall(coro_done, { handle }, "done");
		jit->builder->CreateCondBr(done, doneBlock, resumeBlock);

		jit->builder->SetInsertPoint(doneBlock);
		jit->builder->CreateRet(::llvm::ConstantInt::getFalse(i1Ty));

		jit->builder->SetInsertPoint(resumeBlock);
		auto promiseAlignment = ::llvm::ConstantInt::get(i32Ty, 4);  // TODO: Get correct alignment.
		auto promisePtr = jit->builder->CreateCall(coro_promise, { handle, promiseAlignment, ::llvm::ConstantInt::get(i1Ty, 0) });
		auto promise = jit->builder->CreateLoad(jit->builder->CreatePointerCast(promisePtr, promisePtrTy));
		jit->builder->CreateStore(promise, outPtr);
		jit->builder->CreateCall(coro_resume, { handle });
		jit->builder->CreateRet(::llvm::ConstantInt::getTrue(i1Ty));
	}

	// Build the coroutine_destroy() function:
	//
	//    void coroutine_destroy(CoroutineHandle* handle)
	//    {
	//        llvm.coro.destroy(handle);
	//    }
	//
	{
		auto handle = jit->coroutine.destroy->arg_begin();
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.destroy));
		jit->builder->CreateCall(coro_destroy, { handle });
		jit->builder->CreateRetVoid();
	}

	// Begin building the main coroutine_begin() function.
	//
	//    CoroutineHandle* coroutine_begin(<Arguments>)
	//    {
	//        YieldType promise;
	//        auto id = llvm.coro.id(0, &promise, nullptr, nullptr);
	//        void* frame = coroutine_alloc_frame(llvm.coro.size.i32());
	//        CoroutineHandle *handle = llvm.coro.begin(id, frame);
	//
	//        ... <REACTOR CODE> ...
	//
	//    end:
	//        SuspendAction action = llvm.coro.suspend(none, true /* final */);  // <-- RESUME POINT
	//        switch(action)
	//        {
	//        case SuspendActionResume:
	//            UNREACHABLE(); // Illegal to resume after final suspend.
	//        case SuspendActionDestroy:
	//            goto destroy;
	//        default: // (SuspendActionSuspend)
	//            goto suspend;
	//        }
	//
	//    destroy:
	//        coroutine_free_frame(llvm.coro.free(id, handle));
	//        goto suspend;
	//
	//    suspend:
	//        llvm.coro.end(handle, false);
	//        return handle;
	//    }
	//

#ifdef ENABLE_RR_DEBUG_INFO
	jit->debugInfo = std::make_unique<rr::DebugInfo>(jit->builder.get(), &jit->context, jit->module.get(), jit->function);
#endif  // ENABLE_RR_DEBUG_INFO

	jit->coroutine.suspendBlock = llvm::BasicBlock::Create(jit->context, "suspend", jit->function);
	jit->coroutine.endBlock = llvm::BasicBlock::Create(jit->context, "end", jit->function);
	jit->coroutine.destroyBlock = llvm::BasicBlock::Create(jit->context, "destroy", jit->function);

	jit->builder->SetInsertPoint(jit->coroutine.entryBlock, jit->coroutine.entryBlock->begin());
	jit->coroutine.promise = jit->builder->CreateAlloca(promiseTy, nullptr, "promise");
	jit->coroutine.id = jit->builder->CreateCall(coro_id, {
	                                                          ::llvm::ConstantInt::get(i32Ty, 0),
	                                                          jit->builder->CreatePointerCast(jit->coroutine.promise, i8PtrTy),
	                                                          ::llvm::ConstantPointerNull::get(i8PtrTy),
	                                                          ::llvm::ConstantPointerNull::get(i8PtrTy),
	                                                      });
	auto size = jit->builder->CreateCall(coro_size, {});
	auto frame = jit->builder->CreateCall(allocFrame, { size });
	jit->coroutine.handle = jit->builder->CreateCall(coro_begin, { jit->coroutine.id, frame });

	// Build the suspend block
	jit->builder->SetInsertPoint(jit->coroutine.suspendBlock);
	jit->builder->CreateCall(coro_end, { jit->coroutine.handle, ::llvm::ConstantInt::get(i1Ty, 0) });
	jit->builder->CreateRet(jit->coroutine.handle);

	// Build the end block
	jit->builder->SetInsertPoint(jit->coroutine.endBlock);
	auto action = jit->builder->CreateCall(coro_suspend, {
	                                                         ::llvm::ConstantTokenNone::get(jit->context),
	                                                         ::llvm::ConstantInt::get(i1Ty, 1),  // final: true
	                                                     });
	auto switch_ = jit->builder->CreateSwitch(action, jit->coroutine.suspendBlock, 3);
	// switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionResume), trapBlock); // TODO: Trap attempting to resume after final suspend
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionDestroy), jit->coroutine.destroyBlock);

	// Build the destroy block
	jit->builder->SetInsertPoint(jit->coroutine.destroyBlock);
	auto memory = jit->builder->CreateCall(coro_free, { jit->coroutine.id, jit->coroutine.handle });
	jit->builder->CreateCall(freeFrame, { memory });
	jit->builder->CreateBr(jit->coroutine.suspendBlock);

	// Switch back to original insert point to continue building the coroutine.
	jit->builder->restoreIP(oldInsertionPoint);
}

}  // anonymous namespace

namespace rr {

void Nucleus::createCoroutine(Type *YieldType, const std::vector<Type *> &Params)
{
	// Coroutines are initially created as a regular function.
	// Upon the first call to Yield(), the function is promoted to a true
	// coroutine.
	auto voidTy = ::llvm::Type::getVoidTy(jit->context);
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8PtrTy = ::llvm::Type::getInt8PtrTy(jit->context);
	auto handleTy = i8PtrTy;
	auto boolTy = i1Ty;
	auto promiseTy = T(YieldType);
	auto promisePtrTy = promiseTy->getPointerTo();

	jit->function = rr::createFunction("coroutine_begin", handleTy, T(Params));
	jit->coroutine.await = rr::createFunction("coroutine_await", boolTy, { handleTy, promisePtrTy });
	jit->coroutine.destroy = rr::createFunction("coroutine_destroy", voidTy, { handleTy });
	jit->coroutine.yieldType = promiseTy;
	jit->coroutine.entryBlock = llvm::BasicBlock::Create(jit->context, "function", jit->function);

	jit->builder->SetInsertPoint(jit->coroutine.entryBlock);
}

void Nucleus::yield(Value *val)
{
	if(jit->coroutine.id == nullptr)
	{
		// First call to yield().
		// Promote the function to a full coroutine.
		promoteFunctionToCoroutine();
		ASSERT(jit->coroutine.id != nullptr);
	}

	//      promise = val;
	//
	//      auto action = llvm.coro.suspend(none, false /* final */); // <-- RESUME POINT
	//      switch(action)
	//      {
	//      case SuspendActionResume:
	//          goto resume;
	//      case SuspendActionDestroy:
	//          goto destroy;
	//      default: // (SuspendActionSuspend)
	//          goto suspend;
	//      }
	//  resume:
	//

	RR_DEBUG_INFO_UPDATE_LOC();
	Variable::materializeAll();

	// Types
	auto i1Ty = ::llvm::Type::getInt1Ty(jit->context);
	auto i8Ty = ::llvm::Type::getInt8Ty(jit->context);

	// Intrinsics
	auto coro_suspend = ::llvm::Intrinsic::getDeclaration(jit->module.get(), ::llvm::Intrinsic::coro_suspend);

	// Create a block to resume execution.
	auto resumeBlock = llvm::BasicBlock::Create(jit->context, "resume", jit->function);

	// Store the promise (yield value)
	jit->builder->CreateStore(V(val), jit->coroutine.promise);
	auto action = jit->builder->CreateCall(coro_suspend, {
	                                                         ::llvm::ConstantTokenNone::get(jit->context),
	                                                         ::llvm::ConstantInt::get(i1Ty, 0),  // final: true
	                                                     });
	auto switch_ = jit->builder->CreateSwitch(action, jit->coroutine.suspendBlock, 3);
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionResume), resumeBlock);
	switch_->addCase(::llvm::ConstantInt::get(i8Ty, SuspendActionDestroy), jit->coroutine.destroyBlock);

	// Continue building in the resume block.
	jit->builder->SetInsertPoint(resumeBlock);
}

std::shared_ptr<Routine> Nucleus::acquireCoroutine(const char *name, const Config::Edit &cfgEdit /* = Config::Edit::None */)
{
	bool isCoroutine = jit->coroutine.id != nullptr;
	if(isCoroutine)
	{
		jit->builder->CreateBr(jit->coroutine.endBlock);
	}
	else
	{
		// Coroutine without a Yield acts as a regular function.
		// The 'coroutine_begin' function returns a nullptr for the coroutine
		// handle.
		jit->builder->CreateRet(llvm::Constant::getNullValue(jit->function->getReturnType()));
		// The 'coroutine_await' function always returns false (coroutine done).
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.await));
		jit->builder->CreateRet(llvm::Constant::getNullValue(jit->coroutine.await->getReturnType()));
		// The 'coroutine_destroy' does nothing, returns void.
		jit->builder->SetInsertPoint(llvm::BasicBlock::Create(jit->context, "", jit->coroutine.destroy));
		jit->builder->CreateRetVoid();
	}

#ifdef ENABLE_RR_DEBUG_INFO
	if(jit->debugInfo != nullptr)
	{
		jit->debugInfo->Finalize();
	}
#endif  // ENABLE_RR_DEBUG_INFO

	if(false)
	{
		std::error_code error;
		llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-unopt.txt", error);
		jit->module->print(file, 0);
	}

	if(isCoroutine)
	{
		// Run manadory coroutine transforms.
		llvm::legacy::PassManager pm;

#if LLVM_VERSION_MAJOR >= 9
		pm.add(llvm::createCoroEarlyLegacyPass());
		pm.add(llvm::createCoroSplitLegacyPass());
		pm.add(llvm::createCoroElideLegacyPass());
		pm.add(llvm::createBarrierNoopPass());
		pm.add(llvm::createCoroCleanupLegacyPass());
#else
		pm.add(llvm::createCoroEarlyPass());
		pm.add(llvm::createCoroSplitPass());
		pm.add(llvm::createCoroElidePass());
		pm.add(llvm::createBarrierNoopPass());
		pm.add(llvm::createCoroCleanupPass());
#endif

		pm.run(*jit->module);
	}

#if defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)
	{
		llvm::legacy::PassManager pm;
		pm.add(llvm::createVerifierPass());
		pm.run(*jit->module);
	}
#endif  // defined(ENABLE_RR_LLVM_IR_VERIFICATION) || !defined(NDEBUG)

	auto cfg = cfgEdit.apply(jit->config);
	jit->optimize(cfg);

	if(false)
	{
		std::error_code error;
		llvm::raw_fd_ostream file(std::string(name) + "-llvm-dump-opt.txt", error);
		jit->module->print(file, 0);
	}

	llvm::Function *funcs[Nucleus::CoroutineEntryCount];
	funcs[Nucleus::CoroutineEntryBegin] = jit->function;
	funcs[Nucleus::CoroutineEntryAwait] = jit->coroutine.await;
	funcs[Nucleus::CoroutineEntryDestroy] = jit->coroutine.destroy;
	auto routine = jit->acquireRoutine(funcs, Nucleus::CoroutineEntryCount, cfg);
	jit.reset();

	return routine;
}

Nucleus::CoroutineHandle Nucleus::invokeCoroutineBegin(Routine &routine, std::function<Nucleus::CoroutineHandle()> func)
{
	return func();
}

}  // namespace rr
