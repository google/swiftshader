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

#include <vector>

namespace {

class Optimizer
{
public:
	void run(Ice::Cfg *function);

private:
	void analyzeUses(Ice::Cfg *function);

	void eliminateDeadCode();
	void propagateAlloca();
	void eliminateUnitializedLoads();
	void eliminateLoadsFollowingSingleStore();
	void optimizeStoresInSingleBasicBlock();

	void replace(Ice::Inst *instruction, Ice::Operand *newValue);
	void deleteInstruction(Ice::Inst *instruction);
	bool isDead(Ice::Inst *instruction);

	static const Ice::InstIntrinsic *asLoadSubVector(const Ice::Inst *instruction);
	static const Ice::InstIntrinsic *asStoreSubVector(const Ice::Inst *instruction);
	static bool isLoad(const Ice::Inst &instruction);
	static bool isStore(const Ice::Inst &instruction);
	static std::size_t storeSize(const Ice::Inst *instruction);
	static bool loadTypeMatchesStore(const Ice::Inst *load, const Ice::Inst *store);

	Ice::Cfg *function;
	Ice::GlobalContext *context;

	struct Uses : std::vector<Ice::Inst *>
	{
		bool areOnlyLoadStore() const;
		void insert(Ice::Operand *value, Ice::Inst *instruction);
		void erase(Ice::Inst *instruction);

		std::vector<Ice::Inst *> loads;
		std::vector<Ice::Inst *> stores;
	};

	struct LoadStoreInst
	{
		LoadStoreInst(Ice::Inst *inst, bool isStore)
		    : inst(inst)
		    , address(isStore ? inst->getStoreAddress() : inst->getLoadAddress())
		    , isStore(isStore)
		{
		}

		Ice::Inst *inst;
		Ice::Operand *address;
		bool isStore;
	};

	Optimizer::Uses *getUses(Ice::Operand *);
	void setUses(Ice::Operand *, Optimizer::Uses *);
	bool hasUses(Ice::Operand *) const;

	Ice::CfgNode *getNode(Ice::Inst *);
	void setNode(Ice::Inst *, Ice::CfgNode *);

	Ice::Inst *getDefinition(Ice::Variable *);
	void setDefinition(Ice::Variable *, Ice::Inst *);

	const std::vector<LoadStoreInst> &getLoadStoreInsts(Ice::CfgNode *);
	void setLoadStoreInsts(Ice::CfgNode *, std::vector<LoadStoreInst> *);
	bool hasLoadStoreInsts(Ice::CfgNode *node) const;

	std::vector<Ice::Operand *> operandsWithUses;
};

void Optimizer::run(Ice::Cfg *function)
{
	this->function = function;
	this->context = function->getContext();

	analyzeUses(function);

	// Start by eliminating any dead code, to avoid redundant work for the
	// subsequent optimization passes.
	eliminateDeadCode();

	// Eliminate allocas which store the address of other allocas.
	propagateAlloca();

	eliminateUnitializedLoads();
	eliminateLoadsFollowingSingleStore();
	optimizeStoresInSingleBasicBlock();
	eliminateDeadCode();

	for(auto operand : operandsWithUses)
	{
		// Deletes the Uses instance on the operand
		setUses(operand, nullptr);
	}
	operandsWithUses.clear();
}

// Eliminates allocas which store the address of other allocas.
void Optimizer::propagateAlloca()
{
	Ice::CfgNode *entryBlock = function->getEntryNode();
	Ice::InstList &instList = entryBlock->getInsts();

	for(Ice::Inst &inst : instList)
	{
		if(inst.isDeleted())
		{
			continue;
		}

		auto *alloca = llvm::dyn_cast<Ice::InstAlloca>(&inst);

		if(!alloca)
		{
			break;  // Allocas are all at the top
		}

		// Look for stores of this alloca's address.
		Ice::Operand *address = alloca->getDest();
		Uses uses = *getUses(address);  // Hard copy

		for(auto *store : uses)
		{
			if(isStore(*store) && store->getData() == address)
			{
				Ice::Operand *dest = store->getStoreAddress();
				Ice::Variable *destVar = llvm::dyn_cast<Ice::Variable>(dest);
				Ice::Inst *def = destVar ? getDefinition(destVar) : nullptr;

				// If the address is stored into another stack variable, eliminate the latter.
				if(def && def->getKind() == Ice::Inst::Alloca)
				{
					Uses destUses = *getUses(dest);  // Hard copy

					// Make sure the only store into the stack variable is this address, and that all of its other uses are loads.
					// This prevents dynamic array loads/stores to be replaced by a scalar.
					if((destUses.stores.size() == 1) && (destUses.loads.size() == destUses.size() - 1))
					{
						for(auto *load : destUses.loads)
						{
							replace(load, address);
						}

						// The address is now only stored, never loaded, so the store can be eliminated, together with its alloca.
						assert(getUses(dest)->size() == 1);
						deleteInstruction(store);
						assert(def->isDeleted());
					}
				}
			}
		}
	}
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
	} while(modified);
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
			break;  // Allocas are all at the top
		}

		Ice::Operand *address = alloca.getDest();

		if(!hasUses(address))
		{
			continue;
		}

		const auto &addressUses = *getUses(address);

		if(!addressUses.areOnlyLoadStore())
		{
			continue;
		}

		if(addressUses.stores.empty())
		{
			for(Ice::Inst *load : addressUses.loads)
			{
				Ice::Variable *loadData = load->getDest();

				if(hasUses(loadData))
				{
					for(Ice::Inst *use : *getUses(loadData))
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

					setUses(loadData, nullptr);
				}

				load->setDeleted();
			}

			alloca.setDeleted();
			setUses(address, nullptr);
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
			break;  // Allocas are all at the top
		}

		Ice::Operand *address = alloca.getDest();

		if(!hasUses(address))
		{
			continue;
		}

		auto &addressUses = *getUses(address);

		if(!addressUses.areOnlyLoadStore())
		{
			continue;
		}

		if(addressUses.stores.size() == 1)
		{
			Ice::Inst *store = addressUses.stores[0];
			Ice::Operand *storeValue = store->getData();

			auto instIterator = store->getIterator();
			auto basicBlockEnd = getNode(store)->getInsts().end();

			while(++instIterator != basicBlockEnd)
			{
				Ice::Inst *load = &*instIterator;

				if(load->isDeleted() || !isLoad(*load))
				{
					continue;
				}

				if(load->getLoadAddress() != address)
				{
					continue;
				}

				if(!loadTypeMatchesStore(load, store))
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
					setUses(address, nullptr);

					if(hasUses(storeValue))
					{
						auto &valueUses = *getUses(storeValue);

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
							setUses(storeValue, nullptr);
						}
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

	std::vector<std::vector<LoadStoreInst> *> allocatedVectors;

	for(Ice::Inst &alloca : entryBlock->getInsts())
	{
		if(alloca.isDeleted())
		{
			continue;
		}

		if(!llvm::isa<Ice::InstAlloca>(alloca))
		{
			break;  // Allocas are all at the top
		}

		Ice::Operand *address = alloca.getDest();

		if(!hasUses(address))
		{
			continue;
		}

		const auto &addressUses = *getUses(address);

		if(!addressUses.areOnlyLoadStore())
		{
			continue;
		}

		Ice::CfgNode *singleBasicBlock = getNode(addressUses.stores[0]);

		for(size_t i = 1; i < addressUses.stores.size(); i++)
		{
			Ice::Inst *store = addressUses.stores[i];
			if(getNode(store) != singleBasicBlock)
			{
				singleBasicBlock = nullptr;
				break;
			}
		}

		if(singleBasicBlock)
		{
			if(!hasLoadStoreInsts(singleBasicBlock))
			{
				std::vector<LoadStoreInst> *loadStoreInstVector = new std::vector<LoadStoreInst>();
				setLoadStoreInsts(singleBasicBlock, loadStoreInstVector);
				allocatedVectors.push_back(loadStoreInstVector);
				for(Ice::Inst &inst : singleBasicBlock->getInsts())
				{
					if(inst.isDeleted())
					{
						continue;
					}

					bool isStoreInst = isStore(inst);
					bool isLoadInst = isLoad(inst);

					if(isStoreInst || isLoadInst)
					{
						loadStoreInstVector->push_back(LoadStoreInst(&inst, isStoreInst));
					}
				}
			}

			Ice::Inst *store = nullptr;
			Ice::Operand *storeValue = nullptr;
			bool unmatchedLoads = false;

			for(auto &loadStoreInst : getLoadStoreInsts(singleBasicBlock))
			{
				Ice::Inst *inst = loadStoreInst.inst;

				if((loadStoreInst.address != address) || inst->isDeleted())
				{
					continue;
				}

				if(loadStoreInst.isStore)
				{
					// New store found. If we had a previous one, try to eliminate it.
					if(store && !unmatchedLoads)
					{
						// If the previous store is wider than the new one, we can't eliminate it
						// because there could be a wide load reading its non-overwritten data.
						if(storeSize(inst) >= storeSize(store))
						{
							deleteInstruction(store);
						}
					}

					store = inst;
					storeValue = store->getData();
					unmatchedLoads = false;
				}
				else
				{
					if(!loadTypeMatchesStore(inst, store))
					{
						unmatchedLoads = true;
						continue;
					}

					replace(inst, storeValue);
				}
			}
		}
	}

	for(auto loadStoreInstVector : allocatedVectors)
	{
		delete loadStoreInstVector;
	}
}

void Optimizer::analyzeUses(Ice::Cfg *function)
{
	for(Ice::CfgNode *basicBlock : function->getNodes())
	{
		for(Ice::Inst &instruction : basicBlock->getInsts())
		{
			if(instruction.isDeleted())
			{
				continue;
			}

			setNode(&instruction, basicBlock);
			if(instruction.getDest())
			{
				setDefinition(instruction.getDest(), &instruction);
			}

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
					getUses(src)->insert(src, &instruction);
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

	if(hasUses(oldValue))
	{
		for(Ice::Inst *use : *getUses(oldValue))
		{
			assert(!use->isDeleted());  // Should have been removed from uses already

			for(Ice::SizeT i = 0; i < use->getSrcSize(); i++)
			{
				if(use->getSrc(i) == oldValue)
				{
					use->replaceSource(i, newValue);
				}
			}

			getUses(newValue)->insert(newValue, use);
		}

		setUses(oldValue, nullptr);
	}

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

		if(hasUses(src))
		{
			auto &srcUses = *getUses(src);

			srcUses.erase(instruction);

			if(srcUses.empty())
			{
				setUses(src, nullptr);

				if(Ice::Variable *var = llvm::dyn_cast<Ice::Variable>(src))
				{
					deleteInstruction(getDefinition(var));
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
		return (!hasUses(dest) || getUses(dest)->empty()) && !instruction->hasSideEffects();
	}
	else if(isStore(*instruction))
	{
		if(Ice::Variable *address = llvm::dyn_cast<Ice::Variable>(instruction->getStoreAddress()))
		{
			Ice::Inst *def = getDefinition(address);

			if(def && llvm::isa<Ice::InstAlloca>(def))
			{
				if(hasUses(address))
				{
					Optimizer::Uses *uses = getUses(address);
					return uses->size() == uses->stores.size();  // Dead if all uses are stores
				}
				else
				{
					return true;  // No uses
				}
			}
		}
	}

	return false;
}

const Ice::InstIntrinsic *Optimizer::asLoadSubVector(const Ice::Inst *instruction)
{
	if(auto *instrinsic = llvm::dyn_cast<Ice::InstIntrinsic>(instruction))
	{
		if(instrinsic->getIntrinsicID() == Ice::Intrinsics::LoadSubVector)
		{
			return instrinsic;
		}
	}

	return nullptr;
}

const Ice::InstIntrinsic *Optimizer::asStoreSubVector(const Ice::Inst *instruction)
{
	if(auto *instrinsic = llvm::dyn_cast<Ice::InstIntrinsic>(instruction))
	{
		if(instrinsic->getIntrinsicID() == Ice::Intrinsics::StoreSubVector)
		{
			return instrinsic;
		}
	}

	return nullptr;
}

bool Optimizer::isLoad(const Ice::Inst &instruction)
{
	if(llvm::isa<Ice::InstLoad>(&instruction))
	{
		return true;
	}

	return asLoadSubVector(&instruction) != nullptr;
}

bool Optimizer::isStore(const Ice::Inst &instruction)
{
	if(llvm::isa<Ice::InstStore>(&instruction))
	{
		return true;
	}

	return asStoreSubVector(&instruction) != nullptr;
}

std::size_t Optimizer::storeSize(const Ice::Inst *store)
{
	assert(isStore(*store));

	if(auto *instStore = llvm::dyn_cast<Ice::InstStore>(store))
	{
		return Ice::typeWidthInBytes(instStore->getData()->getType());
	}

	if(auto *storeSubVector = asStoreSubVector(store))
	{
		return llvm::cast<Ice::ConstantInteger32>(storeSubVector->getSrc(3))->getValue();
	}

	return 0;
}

bool Optimizer::loadTypeMatchesStore(const Ice::Inst *load, const Ice::Inst *store)
{
	if(!load || !store)
	{
		return false;
	}

	assert(isLoad(*load) && isStore(*store));
	assert(load->getLoadAddress() == store->getStoreAddress());

	if(store->getData()->getType() != load->getDest()->getType())
	{
		return false;
	}

	if(auto *storeSubVector = asStoreSubVector(store))
	{
		if(auto *loadSubVector = asLoadSubVector(load))
		{
			// Check for matching sub-vector width.
			return llvm::cast<Ice::ConstantInteger32>(storeSubVector->getSrc(2))->getValue() ==
			       llvm::cast<Ice::ConstantInteger32>(loadSubVector->getSrc(1))->getValue();
		}
	}

	return true;
}

Optimizer::Uses *Optimizer::getUses(Ice::Operand *operand)
{
	Optimizer::Uses *uses = (Optimizer::Uses *)operand->Ice::Operand::getExternalData();
	if(!uses)
	{
		uses = new Optimizer::Uses;
		setUses(operand, uses);
		operandsWithUses.push_back(operand);
	}
	return uses;
}

void Optimizer::setUses(Ice::Operand *operand, Optimizer::Uses *uses)
{
	if(auto *oldUses = reinterpret_cast<Optimizer::Uses *>(operand->Ice::Operand::getExternalData()))
	{
		delete oldUses;
	}

	operand->Ice::Operand::setExternalData(uses);
}

bool Optimizer::hasUses(Ice::Operand *operand) const
{
	return operand->Ice::Operand::getExternalData() != nullptr;
}

Ice::CfgNode *Optimizer::getNode(Ice::Inst *inst)
{
	return (Ice::CfgNode *)inst->Ice::Inst::getExternalData();
}

void Optimizer::setNode(Ice::Inst *inst, Ice::CfgNode *node)
{
	inst->Ice::Inst::setExternalData(node);
}

Ice::Inst *Optimizer::getDefinition(Ice::Variable *var)
{
	return (Ice::Inst *)var->Ice::Variable::getExternalData();
}

void Optimizer::setDefinition(Ice::Variable *var, Ice::Inst *inst)
{
	var->Ice::Variable::setExternalData(inst);
}

const std::vector<Optimizer::LoadStoreInst> &Optimizer::getLoadStoreInsts(Ice::CfgNode *node)
{
	return *((const std::vector<LoadStoreInst> *)node->Ice::CfgNode::getExternalData());
}

void Optimizer::setLoadStoreInsts(Ice::CfgNode *node, std::vector<LoadStoreInst> *insts)
{
	node->Ice::CfgNode::setExternalData(insts);
}

bool Optimizer::hasLoadStoreInsts(Ice::CfgNode *node) const
{
	return node->Ice::CfgNode::getExternalData() != nullptr;
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
		if(value == instruction->getLoadAddress())
		{
			loads.push_back(instruction);
		}
	}
	else if(isStore(*instruction))
	{
		if(value == instruction->getStoreAddress())
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

}  // anonymous namespace

namespace rr {

void optimize(Ice::Cfg *function)
{
	Optimizer optimizer;

	optimizer.run(function);
}

}  // namespace rr
