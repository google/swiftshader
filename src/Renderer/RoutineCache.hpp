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

#ifndef sw_RoutineCache_hpp
#define sw_RoutineCache_hpp

#include "LRUCache.hpp"

#include "Reactor/Reactor.hpp"

namespace sw
{
	template<class State>
	class RoutineCache : public LRUCache<State, Routine>
	{
	public:
		RoutineCache(int n, const char *precache = 0);
		~RoutineCache();

	private:
		const char *precache;
		#if defined(_WIN32)
		HMODULE precacheDLL;
		#endif
	};
}

#if defined(_WIN32)
	#include "Shader/Constants.hpp"
	#include "Reactor/DLL.hpp"
#endif

namespace sw
{
	template<class State>
	RoutineCache<State>::RoutineCache(int n, const char *precache) : LRUCache<State, Routine>(n), precache(precache)
	{
		#if defined(_WIN32)
			precacheDLL = 0;

			if(precache)
			{
				char dllName[1024]; sprintf(dllName, "%s.dll", precache);
				char dirName[1024]; sprintf(dirName, "%s.dir", precache);

				precacheDLL = LoadLibrary(dllName);
				FILE *dir = fopen(dirName, "rb");
				int ordinal = 1;

				while(precacheDLL && dir)
				{
					State state;
					int offset;
					int size;

					size_t bytes = fread(&state, 1, sizeof(State), dir);
					bytes += fread(&offset, 1, sizeof(offset), dir);
					bytes += fread(&size, 1, sizeof(size), dir);

					if(bytes != sizeof(State) + sizeof(offset) + sizeof(size))
					{
						break;
					}

					void (*routine)(void) = (void(*)(void))GetProcAddress(precacheDLL, (char*)ordinal);
					ordinal++;

					if(routine)
					{
						add(state, new Routine(routine, size, offset));
					}
				}

				if(dir)
				{
					fclose(dir);
				}
			}
		#endif
	}

	template<class State>
	RoutineCache<State>::~RoutineCache()
	{
		#if defined(_WIN32)
			char dllName[1024]; sprintf(dllName, "%s.dll", precache);
			char dirName[1024]; sprintf(dirName, "%s.dir", precache);

			if(precache)
			{
				DLL dll(dllName, &constants, sizeof(Constants));
				FILE *dir = fopen(dirName, "wb");

				for(int i = 0; i < getSize(); i++)
				{
					State &state = getKey(i);
					Routine *routine = query(state);

					if(routine)
					{
						unsigned char *buffer = (unsigned char*)routine->getBuffer();
						unsigned char *entry = (unsigned char*)routine->getEntry();
						int size = routine->getBufferSize();
						int codeSize = routine->getCodeSize();

						#ifndef _M_AMD64
							for(int j = 1; j < codeSize - 4; j++)
							{
								unsigned char modRM_SIB = entry[j - 1];
								unsigned int address = *(unsigned int*)&entry[j];

								if((modRM_SIB & 0x05) == 0x05 && (address % 4) == 0)
								{
									if(address >= (unsigned int)buffer && address < (unsigned int)entry)   // Constant stored above the function entry
									{
										dll.addRelocation(buffer, &entry[j], true);

										j += 4;
									}
								}
							}
						#else
							for(int j = 1; j < codeSize - 4; j++)
							{
								unsigned char modRM_SIB = entry[j - 1];
								uint64_t address = *(uint64_t*)&entry[j];

							//	if((modRM_SIB & 0x05) == 0x05 && (address % 4) == 0)
								{
									if(address >= (uint64_t)buffer && address < (uint64_t)entry)   // Constant stored above the function entry
									{
										dll.addRelocation(buffer, &entry[j], true);

										j += 4;
									}
								}
							}
						#endif

						dll.addFunction(buffer, entry, size);
						fwrite(&state, 1, sizeof(State), dir);
						int offset = (int)(entry - buffer);
						fwrite(&offset, 1, sizeof(offset), dir);
						fwrite(&size, 1, sizeof(size), dir);
					}
				}

				FreeLibrary(precacheDLL);

				dll.emit();
				fclose(dir);
			}
			else
			{
				FreeLibrary(precacheDLL);

				remove(dllName);
				remove(dirName);
			}
		#endif
	}
}

#endif   // sw_RoutineCache_hpp
