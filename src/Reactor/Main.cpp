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

#include "Reactor.hpp"

#include <cassert>

using namespace sw;

int reference(int *p, int y)
{
	int x = *p;
	int z = 4;

	for(int i = 0; i < 10; i++)
	{
		z += (2 << i) - (i / 3);
	}

	int sum = x + y + z;
   
	return sum;
}

int main()
{
	Routine *routine = nullptr;

	{
		Function<Int(Pointer<Int>, Int)> function;
		{
			Pointer<Int> p = function.Arg<0>();
			Int x = *p;
			Int y = function.Arg<1>();
			Int z = 4;

			For(Int i = 0, i < 10, i++)
			{
				z += (2 << i) - (i / 3);
			}

			Int sum = x + y + z;
   
			Return(sum);
		}

		routine = function(L"one");

		if(routine)
		{
			int (*callable)(int*, int) = (int(*)(int*,int))routine->getEntry();
			int one = 1;
			int result = callable(&one, 2);
			assert(result == reference(&one, 2));
		}
	}

	delete routine;

	return 0;
}
