// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Shell_hpp
#define sw_Shell_hpp

#include "Nucleus.hpp"

namespace sw
{
	enum TranscendentalPrecision
	{
		APPROXIMATE,
		PARTIAL,	// 2^-10
		ACCURATE,
		WHQL,		// 2^-21
		IEEE		// 2^-23
	};

	class Color4i
	{
	public:
		Color4i();
		Color4i(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha);
		Color4i(const Color4i &rhs);

		Short4 &operator[](int i);
		Color4i &operator=(const Color4i &rhs);

		Short4 r;
		Short4 g;
		Short4 b;
		Short4 a;

		Short4 &x;
		Short4 &y;
		Short4 &z;
		Short4 &w;
	};

	class Color4f
	{
	public:
		Color4f();
		Color4f(float red, float green, float blue, float alpha);
		Color4f(const Color4f &rhs);

		Float4 &operator[](int i);
		Color4f &operator=(const Color4f &rhs);

		Float4 r;
		Float4 g;
		Float4 b;
		Float4 a;

		Float4 &x;
		Float4 &y;
		Float4 &z;
		Float4 &w;

		Float4 &u;
		Float4 &v;
		Float4 &s;
		Float4 &t;
	};

	Float4 exponential(Float4 &src, bool pp = false);
	Float4 logarithm(Float4 &src, bool abs, bool pp = false);
	Float4 power(Float4 &src0, Float4 &src1, bool pp = false);
	Float4 reciprocal(Float4 &src, bool pp = false, bool finite = false);
	Float4 reciprocalSquareRoot(Float4 &src, bool abs, bool pp = false);
	Float4 sine(Float4 &src, bool pp = false);

	Float4 dot3(Color4f &src0, Color4f &src1);
	Float4 dot4(Color4f &src0, Color4f &src1);

	void transpose4x4(Short4 &row0, Short4 &row1, Short4 &row2, Short4 &row3);
	void transpose4x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x3(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x2(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4x1(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose2x4(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose2x4h(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3);
	void transpose4xN(Float4 &row0, Float4 &row1, Float4 &row2, Float4 &row3, int N);

	#define For(init, cond, inc)                     \
	init;                                            \
	for(llvm::BasicBlock *loopBB__ = beginLoop(),    \
		*bodyBB__ = Nucleus::createBasicBlock(),        \
		*endBB__ = Nucleus::createBasicBlock(),         \
		*onceBB__ = endBB__;                         \
		onceBB__ && branch(cond, bodyBB__, endBB__); \
		inc, onceBB__ = 0, Nucleus::createBr(loopBB__), Nucleus::setInsertBlock(endBB__))

	#define While(cond) For(((void*)0), cond, ((void*)0))

	#define Do \
	{ \
		llvm::BasicBlock *body = Nucleus::createBasicBlock(); \
		Nucleus::createBr(body); \
		Nucleus::setInsertBlock(body);

	#define Until(cond) \
		llvm::BasicBlock *end = Nucleus::createBasicBlock(); \
		Nucleus::createCondBr((cond).value, end, body); \
		Nucleus::setInsertBlock(end); \
	}

	#define If(cond)                                                              \
	for(llvm::BasicBlock *trueBB__ = Nucleus::createBasicBlock(), \
		*falseBB__ = Nucleus::createBasicBlock(),                 \
		*endBB__ = Nucleus::createBasicBlock(),                   \
		*onceBB__ = endBB__;                                   \
		onceBB__ && branch(cond, trueBB__, falseBB__);         \
		onceBB__ = 0, Nucleus::createBr(endBB__), Nucleus::setInsertBlock(falseBB__), Nucleus::createBr(endBB__), Nucleus::setInsertBlock(endBB__))

	#define Else                                            \
	for(llvm::BasicBlock *endBB__ = Nucleus::getInsertBlock(), \
		*falseBB__ = Nucleus::getPredecessor(endBB__),         \
		*onceBB__ = endBB__;                                \
		onceBB__ && elseBlock(falseBB__);                   \
		onceBB__ = 0, Nucleus::createBr(endBB__), Nucleus::setInsertBlock(endBB__))
}

#endif   // sw_Shell_hpp
