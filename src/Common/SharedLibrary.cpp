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

#include "SharedLibrary.hpp"

#if defined(_WIN32)
std::string getModuleDirectory()
{
	static int dummy_symbol = 0;

	HMODULE module = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&dummy_symbol, &module);

	char filename[1024];
	if(module && (GetModuleFileName(module, filename, sizeof(filename)) != 0))
	{
		std::string directory(filename);
		return directory.substr(0, directory.find_last_of("\\/") + 1).c_str();
	}
	else
	{
		return "";
	}
}
#else
std::string getModuleDirectory()
{
	static int dummy_symbol = 0;

	Dl_info dl_info;
	if(dladdr(&dummy_symbol, &dl_info) != 0)
	{
		std::string directory(dl_info.dli_fname);
		return directory.substr(0, directory.find_last_of("\\/") + 1).c_str();
	}
	else
	{
		return "";
	}
}
#endif
