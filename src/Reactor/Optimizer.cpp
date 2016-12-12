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
		void eliminateUnitializedLoads();

		static bool isLoad(const Ice::Inst &instruction);
		static bool isStore(const Ice::Inst &instruction);
		static Ice::Operand *storeAddress(const Ice::Inst *instruction);
		static Ice::Operand *loadAddress(const Ice::Inst *instruction);

		Ice::Cfg *function;
		Ice::GlobalContext *context;

		struct Uses : std::vector<Ice::Inst*>
		{
			bool areOnlyLoadStore() const;
			void insert(Ice::Operand *value, Ice::Inst *instruction);
			void erase(Ice::Inst *instruction);

			std::vector<Ice::Inst*> loads;
			std::vector<Ice::Inst*> stores;
		};

		std::map<Ice::Operand*, Uses> uses;
	};

	void Optimizer::run(Ice::Cfg *function)
	{
		this->function = function;
		this->context = function->getContext();

		analyzeUses(function);

		eliminateUnusedAllocas();
		eliminateUnitializedLoads();
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

	void Optimizer::eliminateUnitializedLoads()
	{
		Ice::CfgNode *entryBlock = function->getEntryNode();

		for(Ice::Inst &alloca : entryBlock->getInsts())
		{
			if(alloca.isDeleted())
			{
				continue;
			}

			if(!llvm::isa<Ice::InstAlloca>(alloca))
			{
				return;   // Allocas are all at the top
			}

			Ice::Operand *address = alloca.getDest();
			const auto &addressEntry = uses.find(address);
			const auto &addressUses = addressEntry->second;

			if(!addressUses.areOnlyLoadStore())
			{
				continue;
			}

			if(addressUses.stores.empty())
			{
				for(Ice::Inst *load : addressUses.loads)
				{
					Ice::Variable *loadData = load->getDest();

					for(Ice::Inst *use : uses[loadData])
					{
						for(int i = 0; i < use->getSrcSize(); i++)
						{
							if(use->getSrc(i) == loadData)
							{
								auto *undef = context->getConstantUndef(loadData->getType());

								use->replaceSource(i, undef);
							}
						}
					}

					uses.erase(loadData);

					load->setDeleted();
				}

				alloca.setDeleted();
				uses.erase(addressEntry);
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
						Ice::Operand *src = instruction.getSrc(i);
						uses[src].insert(src, &instruction);
					}
				}
			}
		}
	}

	bool Optimizer::isLoad(const Ice::Inst &instruction)
	{
		if(llvm::isa<Ice::InstLoad>(&instruction))
		{
			return true;
		}

		if(auto intrinsicCall = llvm::dyn_cast<Ice::InstIntrinsicCall>(&instruction))
		{
			return intrinsicCall->getIntrinsicInfo().ID == Ice::Intrinsics::LoadSubVector;
		}

		return false;
	}

	bool Optimizer::isStore(const Ice::Inst &instruction)
	{
		if(llvm::isa<Ice::InstStore>(&instruction))
		{
			return true;
		}

		if(auto intrinsicCall = llvm::dyn_cast<Ice::InstIntrinsicCall>(&instruction))
		{
			return intrinsicCall->getIntrinsicInfo().ID == Ice::Intrinsics::StoreSubVector;
		}

		return false;
	}

	Ice::Operand *Optimizer::storeAddress(const Ice::Inst *instruction)
	{
		assert(isStore(*instruction));

		if(auto *store = llvm::dyn_cast<Ice::InstStore>(instruction))
		{
			return store->getAddr();
		}

		if(auto *instrinsic = llvm::dyn_cast<Ice::InstIntrinsicCall>(instruction))
		{
			if(instrinsic->getIntrinsicInfo().ID == Ice::Intrinsics::StoreSubVector)
			{
				return instrinsic->getSrc(2);
			}
		}

		return nullptr;
	}

	Ice::Operand *Optimizer::loadAddress(const Ice::Inst *instruction)
	{
		assert(isLoad(*instruction));

		if(auto *load = llvm::dyn_cast<Ice::InstLoad>(instruction))
		{
			return load->getSourceAddress();
		}

		if(auto *instrinsic = llvm::dyn_cast<Ice::InstIntrinsicCall>(instruction))
		{
			if(instrinsic->getIntrinsicInfo().ID == Ice::Intrinsics::LoadSubVector)
			{
				return instrinsic->getSrc(1);
			}
		}

		return nullptr;
	}

	bool Optimizer::Uses::areOnlyLoadStore() const
	{
		return size() == (loads.size() + stores.size());
	}

	void Optimizer::Uses::insert(Ice::Operand *value, Ice::Inst *instruction)
	{
		push_back(instruction);

		if(isLoad(*instruction))
		{
			if(value == loadAddress(instruction))
			{
				loads.push_back(instruction);
			}
		}
		else if(isStore(*instruction))
		{
			if(value == storeAddress(instruction))
			{
				stores.push_back(instruction);
			}
		}
	}

	void Optimizer::Uses::erase(Ice::Inst *instruction)
	{
		auto &uses = *this;

		for(int i = 0; i < uses.size(); i++)
		{
			if(uses[i] == instruction)
			{
				uses[i] = back();
				pop_back();

				for(int i = 0; i < loads.size(); i++)
				{
					if(loads[i] == instruction)
					{
						loads[i] = loads.back();
						loads.pop_back();
						break;
					}
				}

				for(int i = 0; i < stores.size(); i++)
				{
					if(stores[i] == instruction)
					{
						stores[i] = stores.back();
						stores.pop_back();
						break;
					}
				}

				break;
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