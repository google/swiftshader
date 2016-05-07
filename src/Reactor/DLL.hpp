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
