// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#ifndef yarn_debug_hpp
#define yarn_debug_hpp

#if !defined(YARN_DEBUG_ENABLED)
#    if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
#        define YARN_DEBUG_ENABLED 1
#    else
#        define YARN_DEBUG_ENABLED 0
#    endif
#endif

namespace yarn {

void fatal(const char* msg, ...);
void assert_has_bound_scheduler(const char* feature);

#if YARN_DEBUG_ENABLED
#define YARN_FATAL(msg, ...) yarn::fatal(msg "\n", ##__VA_ARGS__);
#define YARN_ASSERT(cond, msg, ...) do { if (!(cond)) { YARN_FATAL("ASSERT: " msg, ##__VA_ARGS__); } } while (false);
#define YARN_ASSERT_HAS_BOUND_SCHEDULER(feature) assert_has_bound_scheduler(feature);
#define YARN_UNREACHABLE() YARN_FATAL("UNREACHABLE");
#else
#define YARN_FATAL(msg, ...)
#define YARN_ASSERT(cond, msg, ...)
#define YARN_ASSERT_HAS_BOUND_SCHEDULER(feature)
#define YARN_UNREACHABLE()
#endif

} // namespace yarn

#endif  // yarn_debug_hpp
