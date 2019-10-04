// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "marl/thread.h"

#include "marl/trace.h"

#include <cstdarg>
#include <cstdio>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <cstdlib> // mbstowcs
#elif defined(__APPLE__)
#include <mach/thread_act.h>
#include <pthread.h>
#include <unistd.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

namespace marl {

#if defined(_WIN32)

void Thread::setName(const char* fmt, ...) {
  static auto setThreadDescription =
      reinterpret_cast<HRESULT(WINAPI*)(HANDLE, PCWSTR)>(GetProcAddress(
          GetModuleHandle("kernelbase.dll"), "SetThreadDescription"));
  if (setThreadDescription == nullptr) {
    return;
  }

  char name[1024];
  va_list vararg;
  va_start(vararg, fmt);
  vsnprintf(name, sizeof(name), fmt, vararg);
  va_end(vararg);

  wchar_t wname[1024];
  mbstowcs(wname, name, 1024);
  setThreadDescription(GetCurrentThread(), wname);
  MARL_NAME_THREAD("%s", name);
}

unsigned int Thread::numLogicalCPUs() {
  DWORD_PTR processAffinityMask = 1;
  DWORD_PTR systemAffinityMask = 1;

  GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
                         &systemAffinityMask);

  auto count = 0;
  while (processAffinityMask > 0) {
    if (processAffinityMask & 1) {
      count++;
    }

    processAffinityMask >>= 1;
  }
  return count;
}

#else

void Thread::setName(const char* fmt, ...) {
  char name[1024];
  va_list vararg;
  va_start(vararg, fmt);
  vsnprintf(name, sizeof(name), fmt, vararg);
  va_end(vararg);

#if defined(__APPLE__)
  pthread_setname_np(name);
#elif !defined(__Fuchsia__)
  pthread_setname_np(pthread_self(), name);
#endif

  MARL_NAME_THREAD("%s", name);
}

unsigned int Thread::numLogicalCPUs() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

#endif

}  // namespace marl
