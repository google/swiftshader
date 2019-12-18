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

#include "Plane.hpp"

namespace sw {

Plane::Plane()
{
}

Plane::Plane(float p_A, float p_B, float p_C, float p_D)
{
	A = p_A;
	B = p_B;
	C = p_C;
	D = p_D;
}

Plane::Plane(const float ABCD[4])
{
	A = ABCD[0];
	B = ABCD[1];
	C = ABCD[2];
	D = ABCD[3];
}

}  // namespace sw
