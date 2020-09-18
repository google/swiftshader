// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "Reactor.hpp"

// Implementation of intrinsic functions that purport to be as optimal as
// possible, in contrast to the rr::emulated versions, typically by
// implementing approximations of the same math functions.

namespace rr {
namespace optimal {

Float4 Sin(RValue<Float4> x);
Float4 Cos(RValue<Float4> x);
Float4 Tan(RValue<Float4> x);
Float4 Asin_4_terms(RValue<Float4> x);
Float4 Asin_8_terms(RValue<Float4> x);
Float4 Acos_4_terms(RValue<Float4> x);
Float4 Acos_8_terms(RValue<Float4> x);
Float4 Atan(RValue<Float4> x);
Float4 Atan2(RValue<Float4> y, RValue<Float4> x);
Float4 Exp2(RValue<Float4> x);
Float4 Log2(RValue<Float4> x);
Float4 Exp(RValue<Float4> x);
Float4 Log(RValue<Float4> x);
Float4 Pow(RValue<Float4> x, RValue<Float4> y);
Float4 Sinh(RValue<Float4> x);
Float4 Cosh(RValue<Float4> x);
Float4 Tanh(RValue<Float4> x);
Float4 Asinh(RValue<Float4> x);
Float4 Acosh(RValue<Float4> x);
Float4 Atanh(RValue<Float4> x);

}  // namespace optimal
}  // namespace rr
