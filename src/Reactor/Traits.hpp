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

#ifndef rr_Traits_hpp
#define rr_Traits_hpp

#include <stdint.h>
#include <type_traits>

#ifdef Bool
#undef Bool // b/127920555
#endif

namespace rr
{
	// Forward declarations
	class Value;

	class Void;
	class Bool;
	class Byte;
	class SByte;
	class Short;
	class UShort;
	class Int;
	class UInt;
	class Long;
	class Half;
	class Float;

	template<class T> class Pointer;
	template<class T> class LValue;
	template<class T> class RValue;

	// IsDefined<T>::value is true if T is a valid type, otherwise false.
	template <typename T, typename Enable = void>
	struct IsDefined
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct IsDefined<T, typename std::enable_if<(sizeof(T)>0)>::type>
	{
		static constexpr bool value = true;
	};

	template <>
	struct IsDefined<void>
	{
		static constexpr bool value = true;
	};

	// CToReactor<T> resolves to the corresponding Reactor type for the given C
	// template type T.
	template<typename T, typename ENABLE = void> struct CToReactorT;
	template<typename T> using CToReactor = typename CToReactorT<T>::type;

	// CToReactorT specializations for POD types.
	template<> struct CToReactorT<void>    	{ using type = Void; };
	template<> struct CToReactorT<bool>    	{ using type = Bool; };
	template<> struct CToReactorT<uint8_t> 	{ using type = Byte; };
	template<> struct CToReactorT<int8_t>  	{ using type = SByte; };
	template<> struct CToReactorT<int16_t> 	{ using type = Short; };
	template<> struct CToReactorT<uint16_t>	{ using type = UShort; };
	template<> struct CToReactorT<int32_t> 	{ using type = Int; };
	template<> struct CToReactorT<uint64_t>	{ using type = Long; };
	template<> struct CToReactorT<uint32_t>	{ using type = UInt; };
	template<> struct CToReactorT<float>   	{ using type = Float; };

	// CToReactorPtrT<T>::type resolves to the corresponding Reactor Pointer<>
	// type for T*.
	// For T types that have a CToReactorT<> specialization,
	// CToReactorPtrT<T>::type resolves to Pointer< CToReactorT<T> >, otherwise
	// CToReactorPtrT<T>::type resolves to Pointer<Byte>.
	template<typename T, typename ENABLE = void> struct CToReactorPtrT { using type = Pointer<Byte>; };
	template<typename T> using CToReactorPtr = typename CToReactorPtrT<T>::type;
	template<typename T> struct CToReactorPtrT<T, typename std::enable_if< IsDefined< typename CToReactorT<T>::type >::value>::type >
	{
		using type = Pointer< typename CToReactorT<T>::type >;
	};

	// CToReactorT specialization for pointer types.
	// For T types that have a CToReactorT<> specialization,
	// CToReactorT<T*>::type resolves to Pointer< CToReactorT<T> >, otherwise
	// CToReactorT<T*>::type resolves to Pointer<Byte>.
	template<typename T>
	struct CToReactorT<T, typename std::enable_if<std::is_pointer<T>::value>::type>
	{
		using elem = typename std::remove_pointer<T>::type;
		using type = CToReactorPtr<elem>;
	};

	// CToReactorT specialization for void*.
	// Maps to Pointer<Byte> instead of Pointer<Void>.
	template<> struct CToReactorT<void*> { using type = Pointer<Byte>; };

	// CToReactorT specialization for enum types.
	template<typename T>
	struct CToReactorT<T, typename std::enable_if<std::is_enum<T>::value>::type>
	{
		using underlying = typename std::underlying_type<T>::type;
		using type = typename CToReactorT<underlying>::type;
	};

	// IsRValue::value is true if T is of type RValue<X>, where X is any type.
	template <typename T, typename Enable = void> struct IsRValue { static constexpr bool value = false; };
	template <typename T> struct IsRValue<T, typename std::enable_if<IsDefined<typename T::rvalue_underlying_type>::value>::type> { static constexpr bool value = true; };

	// IsLValue::value is true if T is of, or derives from type LValue<T>.
	template <typename T> struct IsLValue { static constexpr bool value = std::is_base_of<LValue<T>, T>::value; };

	// IsReference::value is true if T is of type Reference<X>, where X is any type.
	template <typename T, typename Enable = void> struct IsReference { static constexpr bool value = false; };
	template <typename T> struct IsReference<T, typename std::enable_if<IsDefined<typename T::reference_underlying_type>::value>::type> { static constexpr bool value = true; };

	// ReactorType<T> returns the LValue Reactor type for T.
	// T can be a C-type, RValue or LValue.
	template<typename T, typename ENABLE = void> struct ReactorTypeT;
	template<typename T> using ReactorType = typename ReactorTypeT<T>::type;
	template<typename T> struct ReactorTypeT<T, typename std::enable_if<IsDefined<CToReactor<T>>::value>::type> { using type = CToReactor<T>; };
	template<typename T> struct ReactorTypeT<T, typename std::enable_if<IsRValue<T>::value>::type> { using type = typename T::rvalue_underlying_type; };
	template<typename T> struct ReactorTypeT<T, typename std::enable_if<IsLValue<T>::value>::type> { using type = T; };
	template<typename T> struct ReactorTypeT<T, typename std::enable_if<IsReference<T>::value>::type> { using type = T; };


	// Reactor types that can be used as a return type for a function.
	template <typename T> struct CanBeUsedAsReturn { static constexpr bool value = false; };
	template <> struct CanBeUsedAsReturn<Void>     { static constexpr bool value = true; };
	template <> struct CanBeUsedAsReturn<Int>      { static constexpr bool value = true; };
	template <> struct CanBeUsedAsReturn<UInt>     { static constexpr bool value = true; };
	template <> struct CanBeUsedAsReturn<Float>    { static constexpr bool value = true; };
	template <typename T> struct CanBeUsedAsReturn<Pointer<T>> { static constexpr bool value = true; };

	// Reactor types that can be used as a parameter types for a function.
	template <typename T> struct CanBeUsedAsParameter { static constexpr bool value = false; };
	template <> struct CanBeUsedAsParameter<Int>      { static constexpr bool value = true; };
	template <> struct CanBeUsedAsParameter<UInt>     { static constexpr bool value = true; };
	template <> struct CanBeUsedAsParameter<Float>    { static constexpr bool value = true; };
	template <typename T> struct CanBeUsedAsParameter<Pointer<T>> { static constexpr bool value = true; };

	// AssertParameterTypeIsValid statically asserts that all template parameter
	// types can be used as a Reactor function parameter.
	template<typename T, typename ... other>
	struct AssertParameterTypeIsValid : AssertParameterTypeIsValid<other...>
	{
		static_assert(CanBeUsedAsParameter<T>::value, "Invalid parameter type");
	};
	template<typename T>
	struct AssertParameterTypeIsValid<T>
	{
		static_assert(CanBeUsedAsParameter<T>::value, "Invalid parameter type");
	};

	// AssertFunctionSignatureIsValid statically asserts that the Reactor
	// function signature is valid.
	template<typename Return, typename... Arguments>
	class AssertFunctionSignatureIsValid;
	template<typename Return>
	class AssertFunctionSignatureIsValid<Return(Void)> {};
	template<typename Return, typename... Arguments>
	class AssertFunctionSignatureIsValid<Return(Arguments...)>
	{
		static_assert(CanBeUsedAsReturn<Return>::value, "Invalid return type");
		static_assert(sizeof(AssertParameterTypeIsValid<Arguments...>) >= 0, "");
	};

} // namespace rr

#endif // rr_Traits_hpp
