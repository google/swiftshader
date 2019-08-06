// Copyright 2019 The yarniftShader Authors. All Rights Reserved.
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

#include "Debug.hpp"

#include <cstdlib>

#include <stdarg.h>
#include <stdio.h>

namespace yarn
{

void fatal(const char* msg, ...)
{
    va_list vararg;
    va_start(vararg, msg);
    vfprintf(stderr, msg, vararg);
    va_end(vararg);
    abort();
}

void assert_has_bound_scheduler(const char* feature)
{
    // TODO
}

}  // namespace yarn
