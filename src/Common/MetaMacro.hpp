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

#ifndef sw_MetaMacro_hpp
#define sw_MetaMacro_hpp

//  Disables the "identifier was truncated to '255' characters in the browser information" warning
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

namespace Meta
{
#define META_ASSERT(condition) typedef int COMPILE_TIME_ASSERT_##__LINE__[static_cast<bool>(condition) ? 1 : -1]

	template<class T>
	struct IsVoid
	{
		enum {res = false};
	};

	template<>
	struct IsVoid<void>
	{
		enum {res = true};
	};

#define META_IS_VOID(T) Meta::IsVoid<T>::res

	template<bool>
	struct Select
	{
		template<class T0, class T1>
		struct Type
		{
			typedef T1 Res;
		};
	};

	template<>
	struct Select<true>
	{
		template<class T0, class T1>
		struct Type
		{
			typedef T0 Res;
		};
	};

#define META_SELECT(i, T0, T1) Meta::Select<i>::template Type<T0, T1>::Res

	template<class B0, class B1>
	struct Inherit : B0, B1
	{
	};

#define META_INHERIT(B0, B1) Meta::Inherit<B0, B1>

	template<class B0, class B1>
	class Catenate
	{
		typedef typename META_SELECT(META_IS_VOID(B0), B1, B0) T0;
		typedef typename META_SELECT(META_IS_VOID(B0), void, B1) T1;

	public:
		typedef typename META_INHERIT(T0, T1) T01;
		typedef typename META_SELECT(META_IS_VOID(T1), T0, T01) Res;

	private:
		typedef typename META_SELECT(META_IS_VOID(T1), int, T1) CheckedT1;

		META_ASSERT(META_IS_VOID(T1) || sizeof(Res) == sizeof(T0) + sizeof(CheckedT1));
	};

#define META_CATENATE(B0, B1) Meta::Catenate<B0, B1>::Res

	template<bool condition, class B0, class B1>
	class ConditionalInherit
	{
		typedef typename META_CATENATE(B0, B1) MetaInherit;

	public:
		typedef typename META_SELECT(condition, MetaInherit, B0) Res;
	};

#define META_CONDITIONAL_INHERIT(condition, B0, B1) Meta::ConditionalInherit<condition, B0, B1>::Res
}

#endif   // sw_MetaMacro_hpp
