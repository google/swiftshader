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

#include "Reactor.hpp"

// Implementation of Reactor functions that are "emulated" - that is,
// implemented either in terms of Reactor code, or make use of
// rr::Call to C functions. These are typically slower than implementing
// in terms of direct calls to the JIT backend; however, provide a good
// starting point for implementing a new backend, or for when adding
// functionality to an existing backend is non-trivial.

namespace rr {
namespace emulated {

RValue<Float4> Gather(RValue<Pointer<Float>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
RValue<Int4> Gather(RValue<Pointer<Int>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
void Scatter(RValue<Pointer<Float>> base, RValue<Float4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment);
void Scatter(RValue<Pointer<Int>> base, RValue<Int4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment);
RValue<Float> Exp2(RValue<Float> x);
RValue<Float> Log2(RValue<Float> x);
RValue<Float4> Sin(RValue<Float4> x);
RValue<Float4> Cos(RValue<Float4> x);
RValue<Float4> Tan(RValue<Float4> x);
RValue<Float4> Asin(RValue<Float4> x);
RValue<Float4> Acos(RValue<Float4> x);
RValue<Float4> Atan(RValue<Float4> x);
RValue<Float4> Sinh(RValue<Float4> x);
RValue<Float4> Cosh(RValue<Float4> x);
RValue<Float4> Tanh(RValue<Float4> x);
RValue<Float4> Asinh(RValue<Float4> x);
RValue<Float4> Acosh(RValue<Float4> x);
RValue<Float4> Atanh(RValue<Float4> x);
RValue<Float4> Atan2(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Exp(RValue<Float4> x);
RValue<Float4> Log(RValue<Float4> x);
RValue<Float4> Exp2(RValue<Float4> x);
RValue<Float4> Log2(RValue<Float4> x);
RValue<Int> MinAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder);
RValue<UInt> MinAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<Int> MaxAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder);
RValue<UInt> MaxAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<Float4> FRem(RValue<Float4> lhs, RValue<Float4> rhs);

}  // namespace emulated
}  // namespace rr
