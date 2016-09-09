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
	Ice::ClFlags::Flags.setOptLevel(Ice::Opt_m1);

	std::unique_ptr<Ice::Ostream> cout(new llvm::raw_os_ostream(std::cout));
	std::error_code errorCode;
	std::unique_ptr<llvm::raw_fd_ostream> out(new llvm::raw_fd_ostream("out.o", errorCode, llvm::sys::fs::F_None));
	std::unique_ptr<Ice::ELFStreamer> elf(new Ice::ELFStreamer(*out.get()));
	Ice::GlobalContext context(cout.get(), cout.get(), cout.get(), elf.get());

	// Ice::CfgLocalAllocatorScope _(nullptr);
	// context.initParserThread();
	// context.TlsInit();

	context.emitFileHeader();

	// std::unique_ptr<Ice::VariableDeclarationList> GlobalDeclarationsPool(new
	// Ice::VariableDeclarationList());
	// context.setDisposeGlobalVariablesAfterLowering(false);

	std::unique_ptr<Ice::Cfg> function(Ice::Cfg::create(&context, 0));
	{
		Ice::CfgLocalAllocatorScope _(function.get());

		function->setFunctionName(Ice::GlobalString::createWithString(&context, "Hello"));
		function->setReturnType(Ice::IceType_void);

		Ice::CfgNode *node = function->makeNode();
		function->setEntryNode(node);

		Ice::InstRet *ret = Ice::InstRet::create(function.get());
		node->appendInst(ret);

		//function->genCode();
		//context.translateFunctions();
		function->translate();

		//auto targetLowering = Ice::X8632::TargetX8632::create(function.get());
		//targetLowering->translate();

		//function->getAssembler();
		function->emitIAS();

		auto assembler = function->releaseAssembler();
		context.getObjectWriter()->writeFunctionCode(function->getFunctionName(), false, assembler.get());
		context.getObjectWriter()->writeNonUserSections();
	}

	out->close();

	return 0;
}
