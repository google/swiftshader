// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_DLL_hpp
#define sw_DLL_hpp

#include <windows.h>
#include <vector>
#include <map>

namespace sw
{
	class DLL
	{
	public:
		DLL(const char *name, const void *constants = 0, int constSize = 0);

		~DLL();

		void addFunction(const void *function, const void *entry, int size);
		void addRelocation(const void *function, const void *address, bool ripRelative);
		void emit();

	private:
		int pageAlign(int address)   // Align to 4 kB virtual page size
		{
			return (address + 0xFFF) & -0x1000;
		}

		int fileAlign(int address)   // Align to 512 byte file sections
		{
			return (address + 0x1FF) & -0x200;
		}

		char *dllName;

		IMAGE_DOS_HEADER DOSheader;
		IMAGE_NT_HEADERS32 COFFheader32;
		IMAGE_NT_HEADERS64 COFFheader64;
		IMAGE_SECTION_HEADER textSection;
		IMAGE_SECTION_HEADER exportsSection;
		IMAGE_SECTION_HEADER relocSection;
		IMAGE_SECTION_HEADER constSection;

		IMAGE_EXPORT_DIRECTORY exportDirectory;

		struct Function
		{
			Function() {};
			Function(unsigned int location, const void *function, const void *entry, int size) : location(location), entry(entry), size(size)
			{
				buffer = new unsigned char[size];
				
				memcpy(buffer, function, size);
			}

			~Function()
			{
				delete[] buffer;
			}

			void *buffer;

			unsigned int location;
			const void *entry;
			int size;
		};

		std::vector<const void*> functionOrder;
		typedef std::map<const void*, Function*> FunctionList;
		FunctionList functionList;
		int codeSize;

		const void *constants;
		int constSize;

		struct Relocation
		{
			Relocation(unsigned int offset, bool ripRelative) : offset(offset), ripRelative(ripRelative)
			{
			}

			unsigned int offset;
			bool ripRelative;
		};

		typedef std::map<const void*, std::vector<Relocation> > GlobalRelocations;
		GlobalRelocations globalRelocations;
		typedef std::map<unsigned int, std::vector<unsigned short> > PageRelocations;
		PageRelocations pageRelocations;
	};
}

#endif   // sw_DLL_hpp
