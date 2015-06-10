//TODO: copyrights

#include "program.h"
#include "device.h"
#include "kernel.h"
#include "builtins.h"

#include "program.h"

//#include <llvm/PassManager.h>
//#include <llvm/Analysis/Passes.h>
//#include <llvm/Analysis/Verifier.h>
//#include <llvm/Transforms/Scalar.h>
//#include <llvm/Transforms/IPO.h>
//#include <llvm/ExecutionEngine/ExecutionEngine.h>
//#include <llvm/ExecutionEngine/JIT.h>
//#include <llvm/ExecutionEngine/Interpreter.h>

#include <string>
#include <iostream>

using namespace Devices;

CPUProgram::CPUProgram(CPUDevice *device, Program *program)
	: DeviceProgram(), p_device(device), p_program(program), p_jit(0)
{

}

CPUProgram::~CPUProgram()
{
	if(p_jit)
	{
		// Dont delete the module
		//p_jit->removeModule(p_module);

		//delete p_jit;
	}
}

bool CPUProgram::linkStdLib() const
{
	return true;
}

void CPUProgram::createOptimizationPasses(llvm::PassManager *manager, bool optimize)
{
	if(optimize)
	{
		/*
		* Inspired by code from "The LLVM Compiler Infrastructure"
		*/
		//manager->add(llvm::createDeadArgEliminationPass());
		//manager->add(llvm::createInstructionCombiningPass());
		//manager->add(llvm::createFunctionInliningPass());
		//manager->add(llvm::createPruneEHPass());   // Remove dead EH info.
		//manager->add(llvm::createGlobalOptimizerPass());
		//manager->add(llvm::createGlobalDCEPass()); // Remove dead functions.
		//manager->add(llvm::createArgumentPromotionPass());
		//manager->add(llvm::createInstructionCombiningPass());
		//manager->add(llvm::createJumpThreadingPass());
		//manager->add(llvm::createScalarReplAggregatesPass());
		//manager->add(llvm::createFunctionAttrsPass()); // Add nocapture.
		//manager->add(llvm::createGlobalsModRefPass()); // IP alias analysis.
		//manager->add(llvm::createLICMPass());      // Hoist loop invariants.
		//manager->add(llvm::createGVNPass());       // Remove redundancies.
		//manager->add(llvm::createMemCpyOptPass()); // Remove dead memcpys.
		//manager->add(llvm::createDeadStoreEliminationPass());
		//manager->add(llvm::createInstructionCombiningPass());
		//manager->add(llvm::createJumpThreadingPass());
		//manager->add(llvm::createCFGSimplificationPass());
	}
}

bool CPUProgram::build(llvm::Module *module)
{
	// Nothing to build
	p_module = module;

	return true;
}

bool CPUProgram::initJIT()
{
	if(p_jit)
		return true;

	if(!p_module)
		return false;

	// Create the JIT
	std::string err;
	//llvm::EngineBuilder builder(p_module);

	//builder.setErrorStr(&err);
	//builder.setAllocateGVsWithCode(false);

	//p_jit = builder.create();

	//if(!p_jit)
	//{
	//	std::cout << "Unable to create a JIT: " << err << std::endl;
	//	return false;
	//}

	//p_jit->DisableSymbolSearching(true);    // Avoid an enormous security hole (a kernel calling system())
	//p_jit->InstallLazyFunctionCreator(&getBuiltin);

	return true;
}

llvm::ExecutionEngine *CPUProgram::jit() const
{
	return p_jit;
}
