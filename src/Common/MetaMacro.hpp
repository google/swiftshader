// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

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
