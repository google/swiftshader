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
		void eliminateDeadCode();
		void eliminateUnitializedLoads();
		void eliminateLoadsFollowingSingleStore();
		void optimizeStoresInSingleBasicBlock();

		void replace(Ice::Inst *instruction, Ice::Operand *newValue);
		void deleteInstruction(Ice::Inst *instruction);
		bool isDead(Ice::Inst *instruction);

		static bool isLoad(const Ice::Inst &instruction);
		static bool isStore(const Ice::Inst &instruction);
		static Ice::Operand *storeAddress(const Ice::Inst *instruction);
		static Ice::Operand *loadAddress(const Ice::Inst *instruction);
		static Ice::Operand *storeData(const Ice::Inst *instruction);

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
		std::map<Ice::Inst*, Ice::CfgNode*> node;
		std::map<Ice::Variable*, Ice::Inst*> definition;
	};

	void Optimizer::run(Ice::Cfg *function)
	{
		this->function = function;
		this->context = function->getContext();

		analyzeUses(function);

		eliminateDeadCode();
		eliminateUnitializedLoads();
		eliminateLoadsFollowingSingleStore();
		optimizeStoresInSingleBasicBlock();
		eliminateDeadCode();
	}

	void Optimizer::eliminateDeadCode()
	{
		bool modified;
		do
		{
			modified = false;
			for(Ice::CfgNode *basicBlock : function->getNodes())
			{
				for(Ice::Inst &inst : Ice::reverse_range(basicBlock->getInsts()))
				{
					if(inst.isDeleted())
					{
						continue;
					}

					if(isDead(&inst))
					{
						deleteInstruction(&inst);
						modified = true;
					}
				}
			}
		}
		while(modified);
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
						for(Ice::SizeT i = 0; i < use->getSrcSize(); i++)
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

	void Optimizer::eliminateLoadsFollowingSingleStore()
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
			auto &addressUses = addressEntry->second;

			if(!addressUses.areOnlyLoadStore())
			{
				continue;
			}

			if(addressUses.stores.size() == 1)
			{
				Ice::Inst *store = addressUses.stores[0];
				Ice::Operand *storeValue = storeData(store);

				for(Ice::Inst *load = &*++store->getIterator(), *next = nullptr; load != next; next = load, load = &*++store->getIterator())
				{
					if(load->isDeleted() || !isLoad(*load))
					{
						continue;
					}

					if(loadAddress(load) != address)
					{
						continue;
					}

					replace(load, storeValue);

					for(size_t i = 0; i < addressUses.loads.size(); i++)
					{
						if(addressUses.loads[i] == load)
						{
							addressUses.loads[i] = addressUses.loads.back();
							addressUses.loads.pop_back();
							break;
						}
					}

					for(size_t i = 0; i < addressUses.size(); i++)
					{
						if(addressUses[i] == load)
						{
							addressUses[i] = addressUses.back();
							addressUses.pop_back();
							break;
						}
					}

					if(addressUses.size() == 1)
					{
						assert(addressUses[0] == store);

						alloca.setDeleted();
						store->setDeleted();
						uses.erase(address);

						auto &valueUses = uses[storeValue];

						for(size_t i = 0; i < valueUses.size(); i++)
						{
							if(valueUses[i] == store)
							{
								valueUses[i] = valueUses.back();
								valueUses.pop_back();
								break;
							}
						}

						if(valueUses.empty())
						{
							uses.erase(storeValue);
						}

						break;
					}
				}
			}
		}
	}

	void Optimizer::optimizeStoresInSingleBasicBlock()
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

			Ice::CfgNode *singleBasicBlock = node[addressUses.stores[0]];

			for(size_t i = 1; i < addressUses.stores.size(); i++)
			{
				Ice::Inst *store = addressUses.stores[i];
				if(node[store] != singleBasicBlock)
				{
					singleBasicBlock = nullptr;
					break;
				}
			}

			if(singleBasicBlock)
			{
				auto &insts = singleBasicBlock->getInsts();
				Ice::Inst *store = nullptr;
				Ice::Operand *storeValue = nullptr;

				for(Ice::Inst &inst : insts)
				{
					if(inst.isDeleted())
					{
						continue;
					}

					if(isStore(inst))
					{
						if(storeAddress(&inst) != address)
						{
							continue;
						}

						// New store found. If we had a previous one, eliminate it.
						if(store)
						{
							deleteInstruction(store);
						}

						store = &inst;
						storeValue = storeData(store);
					}
					else if(isLoad(inst))
					{
						Ice::Inst *load = &inst;

						if(loadAddress(load) != address)
						{
							continue;
						}

						if(storeValue)
						{
							replace(load, storeValue);
						}
					}
				}
			}
		}
	}

	void Optimizer::analyzeUses(Ice::Cfg *function)
	{
		uses.clear();
		node.clear();
		definition.clear();

		for(Ice::CfgNode *basicBlock : function->getNodes())
		{
			for(Ice::Inst &instruction : basicBlock->getInsts())
			{
				if(instruction.isDeleted())
				{
					continue;
				}

				node[&instruction] = basicBlock;
				definition[instruction.getDest()] = &instruction;

				for(Ice::SizeT i = 0; i < instruction.getSrcSize(); i++)
				{
					Ice::SizeT unique = 0;
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

	void Optimizer::replace(Ice::Inst *instruction, Ice::Operand *newValue)
	{
		Ice::Variable *oldValue = instruction->getDest();

		if(!newValue)
		{
			newValue = context->getConstantUndef(oldValue->getType());
		}

		for(Ice::Inst *use : uses[oldValue])
		{
			assert(!use->isDeleted());   // Should have been removed from uses already

			for(Ice::SizeT i = 0; i < use->getSrcSize(); i++)
			{
				if(use->getSrc(i) == oldValue)
				{
					use->replaceSource(i, newValue);
				}
			}

			uses[newValue].insert(newValue, use);
		}

		uses.erase(oldValue);

		deleteInstruction(instruction);
	}

	void Optimizer::deleteInstruction(Ice::Inst *instruction)
	{
		if(!instruction || instruction->isDeleted())
		{
			return;
		}

		instruction->setDeleted();

		for(Ice::SizeT i = 0; i < instruction->getSrcSize(); i++)
		{
			Ice::Operand *src = instruction->getSrc(i);

			const auto &srcEntry = uses.find(src);

			if(srcEntry != uses.end())
			{
				auto &srcUses = srcEntry->second;

				srcUses.erase(instruction);

				if(srcUses.empty())
				{
					uses.erase(srcEntry);

					if(Ice::Variable *var = llvm::dyn_cast<Ice::Variable>(src))
					{
						deleteInstruction(definition[var]);
					}
				}
			}
		}
	}

	bool Optimizer::isDead(Ice::Inst *instruction)
	{
		Ice::Variable *dest = instruction->getDest();

		if(dest)
		{
			return uses[dest].empty() && !instruction->hasSideEffects();
		}
		else if(isStore(*instruction))
		{
			if(Ice::Variable *address = llvm::dyn_cast<Ice::Variable>(storeAddress(instruction)))
			{
				Ice::Inst *def = definition[address];

				if(def && llvm::isa<Ice::InstAlloca>(def))
				{
					return uses[address].size() == uses[address].stores.size();   // Dead if all uses are stores
				}
			}
		}

		return false;
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

	Ice::Operand *Optimizer::storeData(const Ice::Inst *instruction)
	{
		assert(isStore(*instruction));

		if(auto *store = llvm::dyn_cast<Ice::InstStore>(instruction))
		{
			return store->getData();
		}

		if(auto *instrinsic = llvm::dyn_cast<Ice::InstIntrinsicCall>(instruction))
		{
			if(instrinsic->getIntrinsicInfo().ID == Ice::Intrinsics::StoreSubVector)
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

		for(size_t i = 0; i < uses.size(); i++)
		{
			if(uses[i] == instruction)
			{
				uses[i] = back();
				pop_back();

				for(size_t i = 0; i < loads.size(); i++)
				{
					if(loads[i] == instruction)
					{
						loads[i] = loads.back();
						loads.pop_back();
						break;
					}
				}

				for(size_t i = 0; i < stores.size(); i++)
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