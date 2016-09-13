#include "src/IceCfg.h"
#include "src/IceELFStreamer.h"
#include "src/IceGlobalContext.h"
#include "src/IceCfgNode.h"
#include "src/IceTargetLoweringX8632.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>

int main()
{
	Ice::ClFlags::Flags.setOutFileType(Ice::FT_Elf);
	Ice::ClFlags::Flags.setOptLevel(Ice::Opt_2);

	std::unique_ptr<Ice::Ostream> cout(new llvm::raw_os_ostream(std::cout));
	std::error_code errorCode;
	std::unique_ptr<Ice::Fdstream> out(new Ice::Fdstream("out.o", errorCode, llvm::sys::fs::F_None));
	std::unique_ptr<Ice::ELFStreamer> elf(new Ice::ELFStreamer(*out.get()));
	Ice::GlobalContext context(cout.get(), cout.get(), cout.get(), elf.get());

	std::unique_ptr<Ice::Cfg> function(Ice::Cfg::create(&context, 0));
	{
		Ice::CfgLocalAllocatorScope _(function.get());

		function->setFunctionName(Ice::GlobalString::createWithString(&context, "HelloWorld"));
		Ice::Variable *arg1 = function->makeVariable(Ice::IceType_i32);
		Ice::Variable *arg2 = function->makeVariable(Ice::IceType_i32);
		function->addArg(arg1);
		function->addArg(arg2);
		function->setReturnType(Ice::IceType_i32);

		Ice::CfgNode *node = function->makeNode();
		function->setEntryNode(node);

		Ice::Variable *sum = function->makeVariable(Ice::IceType_i32);
		Ice::InstArithmetic *add = Ice::InstArithmetic::create(function.get(), Ice::InstArithmetic::Add, sum, arg1, arg2);
		node->appendInst(add);

		Ice::InstRet *ret = Ice::InstRet::create(function.get(), sum);
		node->appendInst(ret);

		function->translate();

		context.emitFileHeader();
		function->emitIAS();
		auto assembler = function->releaseAssembler();
		context.getObjectWriter()->writeFunctionCode(function->getFunctionName(), false, assembler.get());
		context.getObjectWriter()->writeNonUserSections();
	}

	out->close();

	return 0;
}
