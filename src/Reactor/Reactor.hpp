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

#include "Nucleus.hpp"
#include "Routine.hpp"

namespace sw
{
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