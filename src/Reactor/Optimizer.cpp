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

#include "Optimizer.hpp"

#include "src/IceCfg.h"
#include "src/IceCfgNode.h"

#include <map>
#include <vector>

namespace
{
	class Optimizer
	{
	public:
		void run(Ice::Cfg *function);

	private:
		void analyzeUses(Ice::Cfg *function);
		void eliminateUnusedAllocas();

		Ice::Cfg *function;

		std::map<Ice::Operand*, std::vector<Ice::Inst*>> uses;
	};

	void Optimizer::run(Ice::Cfg *function)
	{
		this->function = function;

		analyzeUses(function);

		eliminateUnusedAllocas();
	}

	void Optimizer::eliminateUnusedAllocas()
	{
		Ice::CfgNode *entryBlock = function->getEntryNode();

		for(Ice::Inst &alloca : entryBlock->getInsts())
		{
			if(!llvm::isa<Ice::InstAlloca>(alloca))
			{
				return;   // Allocas are all at the top
			}

			Ice::Operand *address = alloca.getDest();

			if(uses[address].empty())
			{
				alloca.setDeleted();
			}
		}
	}

	void Optimizer::analyzeUses(Ice::Cfg *function)
	{
		uses.clear();

		for(Ice::CfgNode *basicBlock : function->getNodes())
		{
			for(Ice::Inst &instruction : basicBlock->getInsts())
			{
				if(instruction.isDeleted())
				{
					continue;
				}

				for(int i = 0; i < instruction.getSrcSize(); i++)
				{
					int unique = 0;
					for(; unique < i; unique++)
					{
						if(instruction.getSrc(i) == instruction.getSrc(unique))
						{
							break;
						}
					}

					if(i == unique)
					{
						uses[instruction.getSrc(i)].push_back(&instruction);
					}
				}
			}
		}
	}
}

namespace sw
{
	void optimize(Ice::Cfg *function)
	{
		Optimizer optimizer;

		optimizer.run(function);
	}
}