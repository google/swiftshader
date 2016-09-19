#include "src/IceCfg.h"
#include "src/IceELFStreamer.h"
#include "src/IceGlobalContext.h"
#include "src/IceCfgNode.h"
#include "src/IceELFObjectWriter.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <iostream>
#include <vector>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <cstring>

template<typename T>
struct ExecutableAllocator
{
	ExecutableAllocator() {};
	template<class U> ExecutableAllocator(const ExecutableAllocator<U> &other) {};

	using value_type = T;
	using size_type = std::size_t;

	T *allocate(size_type n)
	{
		return (T*)VirtualAlloc(NULL, sizeof(T) * n, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	}

	void deallocate(T *p, size_type n)
	{
		VirtualFree(p, 0, MEM_RELEASE);
	}
};

class ELFMemoryStreamer : public Ice::ELFStreamer
{
	ELFMemoryStreamer(const ELFMemoryStreamer &) = delete;
	ELFMemoryStreamer &operator=(const ELFMemoryStreamer &) = delete;

public:
	ELFMemoryStreamer()
	{
		position =  0;
		buffer.reserve(0x1000);
	}

	virtual ~ELFMemoryStreamer()
	{
		DWORD exeProtection;
		VirtualProtect(&buffer[0], buffer.size(), oldProtection, &exeProtection);
	}

	void write8(uint8_t Value) override
	{
		if(position == (uint64_t)buffer.size())
		{
			buffer.push_back(Value);
			position++;
		}
		else if(position < (uint64_t)buffer.size())
		{
			buffer[position] = Value;
			position++;
		}
		else assert(false);
	}

	void writeBytes(llvm::StringRef Bytes) override
	{
		std::size_t oldSize = buffer.size();
		buffer.resize(oldSize + Bytes.size());
		memcpy(&buffer[oldSize], Bytes.begin(), Bytes.size());
		position += Bytes.size();
	}

	uint64_t tell() const override { return position; }

	void seek(uint64_t Off) override { position = Off; }

	uint8_t *getBuffer()
	{
		VirtualProtect(&buffer[0], buffer.size(), PAGE_EXECUTE_READ, &oldProtection);
		position = std::numeric_limits<std::size_t>::max();  // Can't write more data after this
		return &buffer[0];
	}

private:
	std::vector<uint8_t, ExecutableAllocator<uint8_t>> buffer;
	std::size_t position;
	DWORD oldProtection;
};

void *loadImage(uint8_t *const elfImage)
{
	using ElfHeader = std::conditional<sizeof(void*) == 8, Elf64_Ehdr, Elf32_Ehdr>::type;
	ElfHeader *elfHeader = (ElfHeader*)elfImage;

	if(!elfHeader->checkMagic())
	{
		return nullptr;
	}

	using SectionHeader = std::conditional<sizeof(void*) == 8, Elf64_Shdr, Elf32_Shdr>::type;
	SectionHeader *sectionHeader = (SectionHeader*)(elfImage + elfHeader->e_shoff);
	void *entry = nullptr;

	for(int i = 0; i < elfHeader->e_shnum; i++)
	{
		if(sectionHeader[i].sh_type == SHT_PROGBITS && sectionHeader[i].sh_flags & SHF_EXECINSTR)
		{
			entry = elfImage + sectionHeader[i].sh_offset;	
		}
	}

	return entry;
}

int main()
{
	Ice::ClFlags::Flags.setTargetArch(sizeof(void*) == 8 ? Ice::Target_X8664 : Ice::Target_X8632);
	Ice::ClFlags::Flags.setOutFileType(Ice::FT_Elf);
	Ice::ClFlags::Flags.setOptLevel(Ice::Opt_2);
	Ice::ClFlags::Flags.setApplicationBinaryInterface(Ice::ABI_Platform);

	std::unique_ptr<Ice::Ostream> cout(new llvm::raw_os_ostream(std::cout));
	//std::error_code errorCode;
	//std::unique_ptr<Ice::Fdstream> out(new Ice::Fdstream("out.o", errorCode, llvm::sys::fs::F_None));
	//std::unique_ptr<Ice::ELFStreamer> elf(new Ice::ELFFileStreamer(*out.get()));
	std::unique_ptr<ELFMemoryStreamer> elf(new ELFMemoryStreamer());
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

	uint8_t *buffer = elf->getBuffer();
	int (*add)(int, int) = (int(*)(int,int))loadImage(buffer);

	int x = add(1, 2);

	//out->close();

	return 0;
}
