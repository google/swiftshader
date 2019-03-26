// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef rr_LLVMReactor_hpp
#define rr_LLVMReactor_hpp

namespace llvm
{
	class Type;
	class Value;
}

namespace rr
{
	class Type;
	class Value;

	llvm::Type *T(Type *t);

	inline Type *T(llvm::Type *t)
	{
		return reinterpret_cast<Type*>(t);
	}

	inline llvm::Value *V(Value *t)
	{
		return reinterpret_cast<llvm::Value*>(t);
	}

	inline Value *V(llvm::Value *t)
	{
		return reinterpret_cast<Value*>(t);
	}

	// Emits a no-op instruction that will not be optimized away.
	// Useful for emitting something that can have a source location without
	// effect.
	void Nop();
}

#endif // rr_LLVMReactor_hpp
