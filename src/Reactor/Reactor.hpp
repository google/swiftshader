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