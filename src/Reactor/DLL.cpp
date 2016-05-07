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

#include "Dll.hpp"

#include <time.h>

#ifndef IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE 0x0040
#endif

#ifndef IMAGE_DLLCHARACTERISTICS_NX_COMPAT
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT 0x0100
#endif

namespace sw
{
	#ifdef _M_AMD64
		const bool AMD64 = true;
	#else
		const bool AMD64 = false;
	#endif

	DLL::DLL(const char *name, const void *constants, int constSize) : constants(constants), constSize(constSize)
	{
		dllName = new char[strlen(name) + 1];
		strcpy(dllName, name);

		codeSize = 0;
	}

	DLL::~DLL()
	{
		delete[] dllName;

		for(FunctionList::iterator i = functionList.begin(); i != functionList.end(); i++)
		{
			delete i->second;
		}
	}

	void DLL::addFunction(const void *function, const void *entry, int size)
	{
		functionOrder.push_back(function);
		functionList[function] = new Function(codeSize, function, entry, size);

		codeSize += size;
	}

	void DLL::addRelocation(const void *function, const void *address, bool ripRelative)
	{
		globalRelocations[function].push_back(Relocation((unsigned int)((unsigned char*)address - (unsigned char*)function), ripRelative));
	}

	void DLL::emit()
	{
		if(codeSize == 0)
		{
			return;
		}

		for(GlobalRelocations::iterator i = globalRelocations.begin(); i != globalRelocations.end(); i++)
		{
			const unsigned char *function = (const unsigned char*)i->first;
			const std::vector<Relocation> &functionRelocations = i->second;
			unsigned int location = functionList[function]->location;

			for(unsigned int j = 0; j < functionRelocations.size(); j++)
			{
				unsigned int address = location + functionRelocations[j].offset;
				unsigned int page = address / 0x1000;
				unsigned short reloc = address - page * 0x1000;

				unsigned int relocType = AMD64 ? IMAGE_REL_BASED_DIR64 : IMAGE_REL_BASED_HIGHLOW;
				pageRelocations[page].push_back((relocType << 12) | reloc);
			}
		}

		if(pageRelocations.empty())
		{
			pageRelocations[0];   // Initialize an emtpy list
		}

		int relocSize = 0;

		for(PageRelocations::iterator i = pageRelocations.begin(); i != pageRelocations.end(); i++)
		{
			if(i->second.size() % 2)   // Pad to align to DWORD
			{
				i->second.push_back(0);
			}

			relocSize += (int)sizeof(IMAGE_BASE_RELOCATION) + (int)i->second.size() * (int)sizeof(unsigned short);
		}

		unsigned long timeDateStamp = (unsigned long)time(0);

		memset(&DOSheader, 0, sizeof(DOSheader));
		DOSheader.e_magic = IMAGE_DOS_SIGNATURE;   // "MZ"
		DOSheader.e_lfanew = sizeof(DOSheader);

		int base = 0x10000000;
		int codePage = pageAlign(sizeof(DOSheader) + (AMD64 ? sizeof(COFFheader64) : sizeof(COFFheader32)));
		int exportsPage = codePage + pageAlign(codeSize);
		int exportsSize = (int)(sizeof(IMAGE_EXPORT_DIRECTORY) + functionList.size() * sizeof(void*) + (strlen(dllName) + 1));
		int relocPage = exportsPage + pageAlign(exportsSize);
		int constPage = relocPage + pageAlign(relocSize);

		if(!AMD64)
		{
			memset(&COFFheader32, 0, sizeof(COFFheader32));
			COFFheader32.Signature = IMAGE_NT_SIGNATURE;   // "PE"
			COFFheader32.FileHeader.Machine = IMAGE_FILE_MACHINE_I386;
			COFFheader32.FileHeader.NumberOfSections = 4;
			COFFheader32.FileHeader.TimeDateStamp = timeDateStamp;
			COFFheader32.FileHeader.PointerToSymbolTable = 0;   // Deprecated COFF symbol table
			COFFheader32.FileHeader.NumberOfSymbols = 0;
			COFFheader32.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
			COFFheader32.FileHeader.Characteristics =	IMAGE_FILE_EXECUTABLE_IMAGE |
														IMAGE_FILE_32BIT_MACHINE |
														IMAGE_FILE_DLL;

			COFFheader32.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
			COFFheader32.OptionalHeader.MajorLinkerVersion = 8;
			COFFheader32.OptionalHeader.MinorLinkerVersion = 0;
			COFFheader32.OptionalHeader.SizeOfCode = fileAlign(codeSize);
			COFFheader32.OptionalHeader.SizeOfInitializedData = fileAlign(exportsSize) + fileAlign(relocSize) + fileAlign(constSize);
			COFFheader32.OptionalHeader.SizeOfUninitializedData = 0;
			COFFheader32.OptionalHeader.AddressOfEntryPoint = 0;
			COFFheader32.OptionalHeader.BaseOfCode = codePage;
			COFFheader32.OptionalHeader.BaseOfData = exportsPage;

			COFFheader32.OptionalHeader.ImageBase = base;
			COFFheader32.OptionalHeader.SectionAlignment = 0x1000;
			COFFheader32.OptionalHeader.FileAlignment = 0x200;
			COFFheader32.OptionalHeader.MajorOperatingSystemVersion = 4;
			COFFheader32.OptionalHeader.MinorOperatingSystemVersion = 0;
			COFFheader32.OptionalHeader.MajorImageVersion = 0;
			COFFheader32.OptionalHeader.MinorImageVersion = 0;
			COFFheader32.OptionalHeader.MajorSubsystemVersion = 4;
			COFFheader32.OptionalHeader.MinorSubsystemVersion = 0;
			COFFheader32.OptionalHeader.Win32VersionValue = 0;
			COFFheader32.OptionalHeader.SizeOfImage = constPage + pageAlign(constSize);
			COFFheader32.OptionalHeader.SizeOfHeaders = fileAlign(sizeof(DOSheader) + sizeof(COFFheader32));
			COFFheader32.OptionalHeader.CheckSum = 0;
			COFFheader32.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
			COFFheader32.OptionalHeader.DllCharacteristics =	IMAGE_DLLCHARACTERISTICS_NO_SEH |
															IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE |   // Base address randomization
															IMAGE_DLLCHARACTERISTICS_NX_COMPAT;       // Data Execution Prevention compatible
			COFFheader32.OptionalHeader.SizeOfStackReserve = 1024 * 1024;
			COFFheader32.OptionalHeader.SizeOfStackCommit = 4 * 1024;
			COFFheader32.OptionalHeader.SizeOfHeapReserve = 1024 * 1024;
			COFFheader32.OptionalHeader.SizeOfHeapCommit = 4 * 1024;
			COFFheader32.OptionalHeader.LoaderFlags = 0;
			COFFheader32.OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

			COFFheader32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = exportsPage;
			COFFheader32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = exportsSize;

			COFFheader32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = relocPage;
			COFFheader32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = relocSize;
		}
		else
		{
			memset(&COFFheader64, 0, sizeof(COFFheader64));
			COFFheader64.Signature = IMAGE_NT_SIGNATURE;   // "PE"
			COFFheader64.FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
			COFFheader64.FileHeader.NumberOfSections = 4;
			COFFheader64.FileHeader.TimeDateStamp = timeDateStamp;
			COFFheader64.FileHeader.PointerToSymbolTable = 0;   // Deprecated COFF symbol table
			COFFheader64.FileHeader.NumberOfSymbols = 0;
			COFFheader64.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
			COFFheader64.FileHeader.Characteristics =	IMAGE_FILE_EXECUTABLE_IMAGE |
														IMAGE_FILE_LARGE_ADDRESS_AWARE |
														IMAGE_FILE_DLL;

			COFFheader64.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
			COFFheader64.OptionalHeader.MajorLinkerVersion = 8;
			COFFheader64.OptionalHeader.MinorLinkerVersion = 0;
			COFFheader64.OptionalHeader.SizeOfCode = fileAlign(codeSize);
			COFFheader64.OptionalHeader.SizeOfInitializedData = fileAlign(exportsSize) + fileAlign(relocSize) + fileAlign(constSize);
			COFFheader64.OptionalHeader.SizeOfUninitializedData = 0;
			COFFheader64.OptionalHeader.AddressOfEntryPoint = 0;
			COFFheader64.OptionalHeader.BaseOfCode = codePage;

			COFFheader64.OptionalHeader.ImageBase = base;
			COFFheader64.OptionalHeader.SectionAlignment = 0x1000;
			COFFheader64.OptionalHeader.FileAlignment = 0x200;
			COFFheader64.OptionalHeader.MajorOperatingSystemVersion = 4;
			COFFheader64.OptionalHeader.MinorOperatingSystemVersion = 0;
			COFFheader64.OptionalHeader.MajorImageVersion = 0;
			COFFheader64.OptionalHeader.MinorImageVersion = 0;
			COFFheader64.OptionalHeader.MajorSubsystemVersion = 4;
			COFFheader64.OptionalHeader.MinorSubsystemVersion = 0;
			COFFheader64.OptionalHeader.Win32VersionValue = 0;
			COFFheader64.OptionalHeader.SizeOfImage = constPage + pageAlign(constSize);
			COFFheader64.OptionalHeader.SizeOfHeaders = fileAlign(sizeof(DOSheader) + sizeof(COFFheader64));
			COFFheader64.OptionalHeader.CheckSum = 0;
			COFFheader64.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
			COFFheader64.OptionalHeader.DllCharacteristics =	IMAGE_DLLCHARACTERISTICS_NO_SEH |
															IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE |   // Base address randomization
															IMAGE_DLLCHARACTERISTICS_NX_COMPAT;       // Data Execution Prevention compatible
			COFFheader64.OptionalHeader.SizeOfStackReserve = 1024 * 1024;
			COFFheader64.OptionalHeader.SizeOfStackCommit = 4 * 1024;
			COFFheader64.OptionalHeader.SizeOfHeapReserve = 1024 * 1024;
			COFFheader64.OptionalHeader.SizeOfHeapCommit = 4 * 1024;
			COFFheader64.OptionalHeader.LoaderFlags = 0;
			COFFheader64.OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

			COFFheader64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = exportsPage;
			COFFheader64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = exportsSize;

			COFFheader64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = relocPage;
			COFFheader64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = relocSize;
		}

		memset(&textSection, 0, sizeof(textSection));
		strcpy((char*)&textSection.Name, ".text");
		textSection.Misc.VirtualSize = pageAlign(codeSize);
		textSection.VirtualAddress = codePage;
		textSection.SizeOfRawData = fileAlign(codeSize);
		textSection.PointerToRawData = fileAlign(sizeof(DOSheader) + (AMD64 ? sizeof(COFFheader64) : sizeof(COFFheader32)));
		textSection.PointerToRelocations = 0;
		textSection.PointerToLinenumbers = 0;
		textSection.NumberOfRelocations = 0;
		textSection.NumberOfLinenumbers = 0;
		textSection.Characteristics = IMAGE_SCN_CNT_CODE |
		                              IMAGE_SCN_MEM_EXECUTE |
		                              IMAGE_SCN_MEM_READ;

		memset(&exportsSection, 0, sizeof(exportsSection));
		strcpy((char*)&exportsSection.Name, ".edata");
		exportsSection.Misc.VirtualSize = pageAlign(exportsSize);
		exportsSection.VirtualAddress = exportsPage;
		exportsSection.SizeOfRawData = fileAlign(exportsSize);
		exportsSection.PointerToRawData = textSection.PointerToRawData + fileAlign(codeSize);
		exportsSection.PointerToRelocations = 0;
		exportsSection.PointerToLinenumbers = 0;
		exportsSection.NumberOfRelocations = 0;
		exportsSection.NumberOfLinenumbers = 0;
		exportsSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA |
		                                 IMAGE_SCN_MEM_READ;

		memset(&relocSection, 0, sizeof(relocSection));
		strcpy((char*)&relocSection.Name, ".reloc");
		relocSection.Misc.VirtualSize = pageAlign(relocSize);
		relocSection.VirtualAddress = relocPage;
		relocSection.SizeOfRawData = fileAlign(relocSize);
		relocSection.PointerToRawData = exportsSection.PointerToRawData + fileAlign(exportsSize);
		relocSection.PointerToRelocations = 0;
		relocSection.PointerToLinenumbers = 0;
		relocSection.NumberOfRelocations = 0;
		relocSection.NumberOfLinenumbers = 0;
		relocSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA |
		                               IMAGE_SCN_MEM_DISCARDABLE |
		                               IMAGE_SCN_MEM_READ;

		memset(&constSection, 0, sizeof(constSection));
		strcpy((char*)&constSection.Name, ".rdata");
		constSection.Misc.VirtualSize = pageAlign(constSize);
		constSection.VirtualAddress = constPage;
		constSection.SizeOfRawData = fileAlign(constSize);
		constSection.PointerToRawData = relocSection.PointerToRawData + fileAlign(relocSize);
		constSection.PointerToRelocations = 0;
		constSection.PointerToLinenumbers = 0;
		constSection.NumberOfRelocations = 0;
		constSection.NumberOfLinenumbers = 0;
		constSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA |
		                               IMAGE_SCN_MEM_READ;

		memset(&exportDirectory, 0, sizeof(exportDirectory));
		exportDirectory.Characteristics = 0;
		exportDirectory.TimeDateStamp = timeDateStamp;
		exportDirectory.MajorVersion = 0;
		exportDirectory.MinorVersion = 0;
		exportDirectory.Name = (unsigned long)(exportsPage + sizeof(IMAGE_EXPORT_DIRECTORY) + functionList.size() * sizeof(void*));
		exportDirectory.Base = 1;
		exportDirectory.NumberOfFunctions = (unsigned long)functionList.size();
		exportDirectory.NumberOfNames = 0;
		exportDirectory.AddressOfFunctions = exportsPage + sizeof(IMAGE_EXPORT_DIRECTORY);
		exportDirectory.AddressOfNames = 0;
		exportDirectory.AddressOfNameOrdinals = 0;

		FILE *file = fopen(dllName, "wb");

		if(file)
		{
			fwrite(&DOSheader, 1, sizeof(DOSheader), file);

			if(AMD64)
			{
				fwrite(&COFFheader64, 1, sizeof(COFFheader64), file);
			}
			else
			{
				fwrite(&COFFheader32, 1, sizeof(COFFheader32), file);
			}

			fwrite(&textSection, 1, sizeof(textSection), file);
			fwrite(&exportsSection, 1, sizeof(textSection), file);
			fwrite(&relocSection, 1, sizeof(relocSection), file);
			fwrite(&constSection, 1, sizeof(constSection), file);

			for(FunctionList::iterator i = functionList.begin(); i != functionList.end(); i++)
			{
				const void *function = i->first;
				unsigned int location = i->second->location;
				const std::vector<Relocation> &functionRelocations = globalRelocations[function];

				for(unsigned int j = 0; j < functionRelocations.size(); j++)
				{
					unsigned int *address = (unsigned int*)((unsigned char*)i->second->buffer + functionRelocations[j].offset);

					if(functionRelocations[j].ripRelative)
					{
						*address = base + codePage + location + (*address - (unsigned int)(size_t)function);
					}
					else
					{
						*address = base + constPage + (*address - (unsigned int)(size_t)constants);
					}
				}

				fseek(file, textSection.PointerToRawData + location, SEEK_SET);
				fwrite(i->second->buffer, 1, i->second->size, file);
			}

			fseek(file, exportsSection.PointerToRawData, SEEK_SET);
			fwrite(&exportDirectory, 1, sizeof(exportDirectory), file);

			for(unsigned int i = 0; i < functionOrder.size(); i++)
			{
				const void *buffer = functionOrder[i];
				Function *function = functionList[buffer];

				unsigned int functionAddress = codePage + function->location;
				unsigned int functionEntry = functionAddress + (int)((size_t)function->entry - (size_t)buffer);
				fwrite(&functionEntry, 1, sizeof(functionEntry), file);
			}

			fwrite(dllName, 1, strlen(dllName) + 1, file);

			fseek(file, relocSection.PointerToRawData, SEEK_SET);

			for(PageRelocations::iterator i = pageRelocations.begin(); i != pageRelocations.end(); i++)
			{
				IMAGE_BASE_RELOCATION relocationBlock;

				relocationBlock.VirtualAddress = codePage + i->first * 0x1000;
				relocationBlock.SizeOfBlock = (unsigned long)(sizeof(IMAGE_BASE_RELOCATION) + i->second.size() * sizeof(unsigned short));

				fwrite(&relocationBlock, 1, sizeof(IMAGE_BASE_RELOCATION), file);

				if(i->second.size() > 0)
				{
					fwrite(&i->second[0], 1, i->second.size() * sizeof(unsigned short), file);
				}
			}

			fseek(file, constSection.PointerToRawData, SEEK_SET);
			fwrite(constants, 1, constSize, file);

			char *padding = new char[fileAlign(constSize) - constSize];
			fwrite(padding, 1, fileAlign(constSize) - constSize, file);
			delete[] padding;

			fclose(file);
		}
	}
}
