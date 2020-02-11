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

// Stubs SAL annotation macros for platforms that do not support them.
// See
// https://docs.microsoft.com/en-us/visualstudio/code-quality/annotating-locking-behavior?view=vs-2019

#ifndef marl_sal_h
#define marl_sal_h

#ifndef _Acquires_lock_
#define _Acquires_lock_(...)
#endif

#ifndef _Guarded_by_
#define _Guarded_by_(...)
#endif

#ifndef _Releases_lock_
#define _Releases_lock_(...)
#endif

#ifndef _Requires_lock_held_
#define _Requires_lock_held_(...)
#endif

#ifndef _When_
#define _When_(...)
#endif

#endif  // marl_sal_h
