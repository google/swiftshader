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

#ifndef rr_Traits_inl
#define rr_Traits_inl

namespace rr
{
	// Non-specialized implementation of CToReactorPtrT::cast() defaults to
	// returning a ConstantPointer for v.
	template<typename T, typename ENABLE>
	Pointer<Byte> CToReactorPtrT<T, ENABLE>::cast(const T* v)
	{
		return ConstantPointer(v);
	}

	// CToReactorPtrT specialization for T types that have a CToReactorT<>
	// specialization.
	template<typename T>
	Pointer<CToReactorT<T>>
	CToReactorPtrT<T, typename std::enable_if< IsDefined< CToReactorT<T> >::value>::type >::cast(const T* v)
	{
		return type(v);
	}

	// CToReactorPtrT specialization for void*.
	Pointer<Byte> CToReactorPtrT<void, void>::cast(const void* v)
	{
		return ConstantPointer(v);
	}

	// CToReactorPtrT specialization for function pointer types.
	template<typename T>
	Pointer<Byte>
	CToReactorPtrT<T, typename std::enable_if< std::is_function<T>::value >::type>::cast(T* v)
	{
		return ConstantPointer(v);
	}

	// CToReactor specialization for pointer types.
	template<typename T>
	CToReactorPtr<typename std::remove_pointer<T>::type>
	CToReactor<T, typename std::enable_if<std::is_pointer<T>::value>::type>::cast(T v)
	{
		return CToReactorPtrT<elem>::cast(v);
	}

	// CToReactor specialization for enum types.
	template<typename T>
	CToReactorT<typename std::underlying_type<T>::type>
	CToReactor<T, typename std::enable_if<std::is_enum<T>::value>::type>::cast(T v)
	{
		return CToReactor<underlying>::cast(v);
	}

} // namespace rr

#endif // rr_Traits_inl
