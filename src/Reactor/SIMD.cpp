// Copyright 2022 The SwiftShader Authors. All Rights Reserved.
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

#include "SIMD.hpp"

namespace rr::SIMD {

SIMD::Int::Int(RValue<SIMD::Int> rhs)
{
	store(rhs);
}

SIMD::Int::Int(const Reference<SIMD::Int> &rhs)
{
	storeValue(rhs.loadValue());
}

RValue<SIMD::Int> operator+(RValue<SIMD::Int> lhs, RValue<SIMD::Int> rhs)
{
	return RValue<SIMD::Int>(Nucleus::createAdd(lhs.value(), rhs.value()));
}

}  // namespace rr::SIMD
