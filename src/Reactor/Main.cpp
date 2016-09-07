#include "src/IceCfg.h"
#include "src/IceELFStreamer.h"
#include "src/IceGlobalContext.h"
//#include "src/IceVar

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>

int main() {
  Ice::ClFlags::Flags.setOutFileType(Ice::FT_Elf);
  Ice::ClFlags::Flags.setOptLevel(Ice::Opt_2);

  std::unique_ptr<Ice::Ostream> cout(new llvm::raw_os_ostream(std::cout));
  std::error_code errorCode;
  std::unique_ptr<llvm::raw_fd_ostream> out(
      new llvm::raw_fd_ostream("out.o", errorCode, llvm::sys::fs::F_None));
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

    function->setReturnType(Ice::IceType_i32);
  }

  out->close();

  return 0;
}
