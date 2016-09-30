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

#ifndef sw_Reactor_hpp
#define sw_Reactor_hpp

#include "Nucleus.hpp"
#include "Routine.hpp"

#include <cstddef>
#include <cwchar>
#undef Bool

namespace sw
{
	class Byte;
	class SByte;
	class Byte4;
	class SByte4;
	class Byte8;
	class SByte8;
	class Byte16;
	class SByte16;
	class Short;
	class UShort;
	class Short4;
	class UShort4;
	class Short8;
	class UShort8;
	class Int;
	class UInt;
	class Int2;
	class UInt2;
	class Int4;
	class UInt4;
	class Long;
	class Long1;
	class Long2;
	class Float;
	class Float2;
	class Float4;

	class Void
	{
	public:
		static Type *getType();

		static bool isVoid()
		{
			return true;
		}
	};

	template<class T>
	class RValue;

	template<class T>
	class Pointer;

	template<class T>
	class LValue
	{
	public:
		LValue(int arraySize = 0);

		static bool isVoid()
		{
			return false;
		}

		Value *loadValue(unsigned int alignment = 0) const;
		Value *storeValue(Value *value, unsigned int alignment = 0) const;
		Constant *storeValue(Constant *constant, unsigned int alignment = 0) const;
		Value *getAddress(Value *index) const;

	protected:
		Value *address;
	};

	template<class T>
	class Variable : public LValue<T>
	{
	public:
		Variable(int arraySize = 0);

		RValue<Pointer<T>> operator&();
	};

	template<class T>
	class Reference
	{
	public:
		explicit Reference(Value *pointer, int alignment = 1);

		RValue<T> operator=(RValue<T> rhs) const;
		RValue<T> operator=(const Reference<T> &ref) const;

		RValue<T> operator+=(RValue<T> rhs) const;

		Value *loadValue() const;
		int getAlignment() const;

	private:
		Value *address;

		const int alignment;
	};

	template<class T>
	struct IntLiteral
	{
		struct type;
	};

	template<> struct
	IntLiteral<Int>
	{
		typedef int type;
	};

	template<> struct
	IntLiteral<UInt>
	{
		typedef unsigned int type;
	};

	template<> struct
	IntLiteral<Long>
	{
		typedef int64_t type;
	};

	template<class T>
	struct FloatLiteral
	{
		struct type;
	};

	template<> struct
	FloatLiteral<Float>
	{
		typedef float type;
	};

	template<class T>
	class RValue
	{
	public:
		explicit RValue(Value *rvalue);
		explicit RValue(Constant *constant);

		RValue(const T &lvalue);
		RValue(typename IntLiteral<T>::type i);
		RValue(typename FloatLiteral<T>::type f);
		RValue(const Reference<T> &rhs);

		RValue<T> &operator=(const RValue<T>&) = delete;

		Value *value;   // FIXME: Make private
	};

	template<typename T>
	struct Argument
	{
		explicit Argument(Value *value) : value(value) {}

		Value *value;
	};

	class Bool : public Variable<Bool>
	{
	public:
		Bool(Argument<Bool> argument);

		Bool();
		Bool(bool x);
		Bool(RValue<Bool> rhs);
		Bool(const Bool &rhs);
		Bool(const Reference<Bool> &rhs);

	//	RValue<Bool> operator=(bool rhs) const;   // FIXME: Implement
		RValue<Bool> operator=(RValue<Bool> rhs) const;
		RValue<Bool> operator=(const Bool &rhs) const;
		RValue<Bool> operator=(const Reference<Bool> &rhs) const;

		static Type *getType();
	};

	RValue<Bool> operator!(RValue<Bool> val);
	RValue<Bool> operator&&(RValue<Bool> lhs, RValue<Bool> rhs);
	RValue<Bool> operator||(RValue<Bool> lhs, RValue<Bool> rhs);

	class Byte : public Variable<Byte>
	{
	public:
		Byte(Argument<Byte> argument);

		explicit Byte(RValue<Int> cast);
		explicit Byte(RValue<UInt> cast);
		explicit Byte(RValue<UShort> cast);

		Byte();
		Byte(int x);
		Byte(unsigned char x);
		Byte(RValue<Byte> rhs);
		Byte(const Byte &rhs);
		Byte(const Reference<Byte> &rhs);

	//	RValue<Byte> operator=(unsigned char rhs) const;   // FIXME: Implement
		RValue<Byte> operator=(RValue<Byte> rhs) const;
		RValue<Byte> operator=(const Byte &rhs) const;
		RValue<Byte> operator=(const Reference<Byte> &rhs) const;

		static Type *getType();
	};

	RValue<Byte> operator+(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator-(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator*(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator/(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator%(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator&(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator|(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator^(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator<<(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator>>(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Byte> operator+=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator-=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator*=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator/=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator%=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator&=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator|=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator^=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator<<=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator>>=(const Byte &lhs, RValue<Byte> rhs);
	RValue<Byte> operator+(RValue<Byte> val);
	RValue<Byte> operator-(RValue<Byte> val);
	RValue<Byte> operator~(RValue<Byte> val);
	RValue<Byte> operator++(const Byte &val, int);   // Post-increment
	const Byte &operator++(const Byte &val);   // Pre-increment
	RValue<Byte> operator--(const Byte &val, int);   // Post-decrement
	const Byte &operator--(const Byte &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Bool> operator<=(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Bool> operator>(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Bool> operator>=(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Bool> operator!=(RValue<Byte> lhs, RValue<Byte> rhs);
	RValue<Bool> operator==(RValue<Byte> lhs, RValue<Byte> rhs);

	class SByte : public Variable<SByte>
	{
	public:
		SByte(Argument<SByte> argument);

		explicit SByte(RValue<Int> cast);
		explicit SByte(RValue<Short> cast);

		SByte();
		SByte(signed char x);
		SByte(RValue<SByte> rhs);
		SByte(const SByte &rhs);
		SByte(const Reference<SByte> &rhs);

	//	RValue<SByte> operator=(signed char rhs) const;   // FIXME: Implement
		RValue<SByte> operator=(RValue<SByte> rhs) const;
		RValue<SByte> operator=(const SByte &rhs) const;
		RValue<SByte> operator=(const Reference<SByte> &rhs) const;

		static Type *getType();
	};

	RValue<SByte> operator+(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator-(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator*(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator/(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator%(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator&(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator|(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator^(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator<<(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator>>(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<SByte> operator+=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator-=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator*=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator/=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator%=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator&=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator|=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator^=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator<<=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator>>=(const SByte &lhs, RValue<SByte> rhs);
	RValue<SByte> operator+(RValue<SByte> val);
	RValue<SByte> operator-(RValue<SByte> val);
	RValue<SByte> operator~(RValue<SByte> val);
	RValue<SByte> operator++(const SByte &val, int);   // Post-increment
	const SByte &operator++(const SByte &val);   // Pre-increment
	RValue<SByte> operator--(const SByte &val, int);   // Post-decrement
	const SByte &operator--(const SByte &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<Bool> operator<=(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<Bool> operator>(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<Bool> operator>=(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<Bool> operator!=(RValue<SByte> lhs, RValue<SByte> rhs);
	RValue<Bool> operator==(RValue<SByte> lhs, RValue<SByte> rhs);

	class Short : public Variable<Short>
	{
	public:
		Short(Argument<Short> argument);

		explicit Short(RValue<Int> cast);

		Short();
		Short(short x);
		Short(RValue<Short> rhs);
		Short(const Short &rhs);
		Short(const Reference<Short> &rhs);

	//	RValue<Short> operator=(short rhs) const;   // FIXME: Implement
		RValue<Short> operator=(RValue<Short> rhs) const;
		RValue<Short> operator=(const Short &rhs) const;
		RValue<Short> operator=(const Reference<Short> &rhs) const;

		static Type *getType();
	};

	RValue<Short> operator+(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator-(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator*(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator/(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator%(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator&(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator|(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator^(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator<<(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator>>(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Short> operator+=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator-=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator*=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator/=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator%=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator&=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator|=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator^=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator<<=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator>>=(const Short &lhs, RValue<Short> rhs);
	RValue<Short> operator+(RValue<Short> val);
	RValue<Short> operator-(RValue<Short> val);
	RValue<Short> operator~(RValue<Short> val);
	RValue<Short> operator++(const Short &val, int);   // Post-increment
	const Short &operator++(const Short &val);   // Pre-increment
	RValue<Short> operator--(const Short &val, int);   // Post-decrement
	const Short &operator--(const Short &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Bool> operator<=(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Bool> operator>(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Bool> operator>=(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Bool> operator!=(RValue<Short> lhs, RValue<Short> rhs);
	RValue<Bool> operator==(RValue<Short> lhs, RValue<Short> rhs);

	class UShort : public Variable<UShort>
	{
	public:
		UShort(Argument<UShort> argument);

		explicit UShort(RValue<UInt> cast);
		explicit UShort(RValue<Int> cast);

		UShort();
		UShort(unsigned short x);
		UShort(RValue<UShort> rhs);
		UShort(const UShort &rhs);
		UShort(const Reference<UShort> &rhs);

	//	RValue<UShort> operator=(unsigned short rhs) const;   // FIXME: Implement
		RValue<UShort> operator=(RValue<UShort> rhs) const;
		RValue<UShort> operator=(const UShort &rhs) const;
		RValue<UShort> operator=(const Reference<UShort> &rhs) const;

		static Type *getType();
	};

	RValue<UShort> operator+(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator-(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator*(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator/(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator%(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator&(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator|(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator^(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator<<(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator>>(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<UShort> operator+=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator-=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator*=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator/=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator%=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator&=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator|=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator^=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator<<=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator>>=(const UShort &lhs, RValue<UShort> rhs);
	RValue<UShort> operator+(RValue<UShort> val);
	RValue<UShort> operator-(RValue<UShort> val);
	RValue<UShort> operator~(RValue<UShort> val);
	RValue<UShort> operator++(const UShort &val, int);   // Post-increment
	const UShort &operator++(const UShort &val);   // Pre-increment
	RValue<UShort> operator--(const UShort &val, int);   // Post-decrement
	const UShort &operator--(const UShort &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<Bool> operator<=(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<Bool> operator>(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<Bool> operator>=(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<Bool> operator!=(RValue<UShort> lhs, RValue<UShort> rhs);
	RValue<Bool> operator==(RValue<UShort> lhs, RValue<UShort> rhs);

	class Byte4 : public Variable<Byte4>
	{
	public:
	//	Byte4();
	//	Byte4(int x, int y, int z, int w);
	//	Byte4(RValue<Byte4> rhs);
	//	Byte4(const Byte4 &rhs);
	//	Byte4(const Reference<Byte4> &rhs);

	//	RValue<Byte4> operator=(RValue<Byte4> rhs) const;
	//	RValue<Byte4> operator=(const Byte4 &rhs) const;
	//	RValue<Byte4> operator=(const Reference<Byte4> &rhs) const;

		static Type *getType();
	};

//	RValue<Byte4> operator+(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator-(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator*(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator/(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator%(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator&(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator|(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator^(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator<<(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator>>(RValue<Byte4> lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator+=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator-=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator*=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator/=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator%=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator&=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator|=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator^=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator<<=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator>>=(const Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator+(RValue<Byte4> val);
//	RValue<Byte4> operator-(RValue<Byte4> val);
//	RValue<Byte4> operator~(RValue<Byte4> val);
//	RValue<Byte4> operator++(const Byte4 &val, int);   // Post-increment
//	const Byte4 &operator++(const Byte4 &val);   // Pre-increment
//	RValue<Byte4> operator--(const Byte4 &val, int);   // Post-decrement
//	const Byte4 &operator--(const Byte4 &val);   // Pre-decrement

	class SByte4 : public Variable<SByte4>
	{
	public:
	//	SByte4();
	//	SByte4(int x, int y, int z, int w);
	//	SByte4(RValue<SByte4> rhs);
	//	SByte4(const SByte4 &rhs);
	//	SByte4(const Reference<SByte4> &rhs);

	//	RValue<SByte4> operator=(RValue<SByte4> rhs) const;
	//	RValue<SByte4> operator=(const SByte4 &rhs) const;
	//	RValue<SByte4> operator=(const Reference<SByte4> &rhs) const;

		static Type *getType();
	};

//	RValue<SByte4> operator+(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator-(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator*(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator/(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator%(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator&(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator|(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator^(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator<<(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator>>(RValue<SByte4> lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator+=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator-=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator*=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator/=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator%=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator&=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator|=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator^=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator<<=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator>>=(const SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator+(RValue<SByte4> val);
//	RValue<SByte4> operator-(RValue<SByte4> val);
//	RValue<SByte4> operator~(RValue<SByte4> val);
//	RValue<SByte4> operator++(const SByte4 &val, int);   // Post-increment
//	const SByte4 &operator++(const SByte4 &val);   // Pre-increment
//	RValue<SByte4> operator--(const SByte4 &val, int);   // Post-decrement
//	const SByte4 &operator--(const SByte4 &val);   // Pre-decrement

	class Byte8 : public Variable<Byte8>
	{
	public:
		Byte8();
		Byte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7);
		Byte8(int64_t x);
		Byte8(RValue<Byte8> rhs);
		Byte8(const Byte8 &rhs);
		Byte8(const Reference<Byte8> &rhs);

		RValue<Byte8> operator=(RValue<Byte8> rhs) const;
		RValue<Byte8> operator=(const Byte8 &rhs) const;
		RValue<Byte8> operator=(const Reference<Byte8> &rhs) const;

		static Type *getType();
	};

	RValue<Byte8> operator+(RValue<Byte8> lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator-(RValue<Byte8> lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator*(RValue<Byte8> lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator/(RValue<Byte8> lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator%(RValue<Byte8> lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator&(RValue<Byte8> lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator|(RValue<Byte8> lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator^(RValue<Byte8> lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator<<(RValue<Byte8> lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator>>(RValue<Byte8> lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator+=(const Byte8 &lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator-=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator*=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator/=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator%=(const Byte8 &lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator&=(const Byte8 &lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator|=(const Byte8 &lhs, RValue<Byte8> rhs);
	RValue<Byte8> operator^=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator<<=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator>>=(const Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator+(RValue<Byte8> val);
//	RValue<Byte8> operator-(RValue<Byte8> val);
	RValue<Byte8> operator~(RValue<Byte8> val);
//	RValue<Byte8> operator++(const Byte8 &val, int);   // Post-increment
//	const Byte8 &operator++(const Byte8 &val);   // Pre-increment
//	RValue<Byte8> operator--(const Byte8 &val, int);   // Post-decrement
//	const Byte8 &operator--(const Byte8 &val);   // Pre-decrement

	RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y);
	RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y);
	RValue<Short4> Unpack(RValue<Byte4> x);
	RValue<Short4> UnpackLow(RValue<Byte8> x, RValue<Byte8> y);
	RValue<Short4> UnpackHigh(RValue<Byte8> x, RValue<Byte8> y);
	RValue<Int> SignMask(RValue<Byte8> x);
//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y);
	RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y);

	class SByte8 : public Variable<SByte8>
	{
	public:
		SByte8();
		SByte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7);
		SByte8(int64_t x);
		SByte8(RValue<SByte8> rhs);
		SByte8(const SByte8 &rhs);
		SByte8(const Reference<SByte8> &rhs);

		RValue<SByte8> operator=(RValue<SByte8> rhs) const;
		RValue<SByte8> operator=(const SByte8 &rhs) const;
		RValue<SByte8> operator=(const Reference<SByte8> &rhs) const;

		static Type *getType();
	};

	RValue<SByte8> operator+(RValue<SByte8> lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator-(RValue<SByte8> lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator*(RValue<SByte8> lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator/(RValue<SByte8> lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator%(RValue<SByte8> lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator&(RValue<SByte8> lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator|(RValue<SByte8> lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator^(RValue<SByte8> lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator<<(RValue<SByte8> lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator>>(RValue<SByte8> lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator+=(const SByte8 &lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator-=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator*=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator/=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator%=(const SByte8 &lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator&=(const SByte8 &lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator|=(const SByte8 &lhs, RValue<SByte8> rhs);
	RValue<SByte8> operator^=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator<<=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator>>=(const SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator+(RValue<SByte8> val);
//	RValue<SByte8> operator-(RValue<SByte8> val);
	RValue<SByte8> operator~(RValue<SByte8> val);
//	RValue<SByte8> operator++(const SByte8 &val, int);   // Post-increment
//	const SByte8 &operator++(const SByte8 &val);   // Pre-increment
//	RValue<SByte8> operator--(const SByte8 &val, int);   // Post-decrement
//	const SByte8 &operator--(const SByte8 &val);   // Pre-decrement

	RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y);
	RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y);
	RValue<Short4> UnpackLow(RValue<SByte8> x, RValue<SByte8> y);
	RValue<Short4> UnpackHigh(RValue<SByte8> x, RValue<SByte8> y);
	RValue<Int> SignMask(RValue<SByte8> x);
	RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y);
	RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y);

	class Byte16 : public Variable<Byte16>
	{
	public:
	//	Byte16();
	//	Byte16(int x, int y, int z, int w);
		Byte16(RValue<Byte16> rhs);
		Byte16(const Byte16 &rhs);
		Byte16(const Reference<Byte16> &rhs);

		RValue<Byte16> operator=(RValue<Byte16> rhs) const;
		RValue<Byte16> operator=(const Byte16 &rhs) const;
		RValue<Byte16> operator=(const Reference<Byte16> &rhs) const;

		static Type *getType();
	};

//	RValue<Byte16> operator+(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator-(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator*(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator/(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator%(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator&(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator|(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator^(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator<<(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator>>(RValue<Byte16> lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator+=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator-=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator*=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator/=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator%=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator&=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator|=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator^=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator<<=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator>>=(const Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator+(RValue<Byte16> val);
//	RValue<Byte16> operator-(RValue<Byte16> val);
//	RValue<Byte16> operator~(RValue<Byte16> val);
//	RValue<Byte16> operator++(const Byte16 &val, int);   // Post-increment
//	const Byte16 &operator++(const Byte16 &val);   // Pre-increment
//	RValue<Byte16> operator--(const Byte16 &val, int);   // Post-decrement
//	const Byte16 &operator--(const Byte16 &val);   // Pre-decrement

	class SByte16 : public Variable<SByte16>
	{
	public:
	//	SByte16();
	//	SByte16(int x, int y, int z, int w);
	//	SByte16(RValue<SByte16> rhs);
	//	SByte16(const SByte16 &rhs);
	//	SByte16(const Reference<SByte16> &rhs);

	//	RValue<SByte16> operator=(RValue<SByte16> rhs) const;
	//	RValue<SByte16> operator=(const SByte16 &rhs) const;
	//	RValue<SByte16> operator=(const Reference<SByte16> &rhs) const;

		static Type *getType();
	};

//	RValue<SByte16> operator+(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator-(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator*(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator/(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator%(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator&(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator|(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator^(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator<<(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator>>(RValue<SByte16> lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator+=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator-=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator*=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator/=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator%=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator&=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator|=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator^=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator<<=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator>>=(const SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator+(RValue<SByte16> val);
//	RValue<SByte16> operator-(RValue<SByte16> val);
//	RValue<SByte16> operator~(RValue<SByte16> val);
//	RValue<SByte16> operator++(const SByte16 &val, int);   // Post-increment
//	const SByte16 &operator++(const SByte16 &val);   // Pre-increment
//	RValue<SByte16> operator--(const SByte16 &val, int);   // Post-decrement
//	const SByte16 &operator--(const SByte16 &val);   // Pre-decrement

	class Short4 : public Variable<Short4>
	{
	public:
		explicit Short4(RValue<Int> cast);
		explicit Short4(RValue<Int4> cast);
	//	explicit Short4(RValue<Float> cast);
		explicit Short4(RValue<Float4> cast);

		Short4();
		Short4(short xyzw);
		Short4(short x, short y, short z, short w);
		Short4(RValue<Short4> rhs);
		Short4(const Short4 &rhs);
		Short4(const Reference<Short4> &rhs);
		Short4(RValue<UShort4> rhs);
		Short4(const UShort4 &rhs);
		Short4(const Reference<UShort4> &rhs);

		RValue<Short4> operator=(RValue<Short4> rhs) const;
		RValue<Short4> operator=(const Short4 &rhs) const;
		RValue<Short4> operator=(const Reference<Short4> &rhs) const;
		RValue<Short4> operator=(RValue<UShort4> rhs) const;
		RValue<Short4> operator=(const UShort4 &rhs) const;
		RValue<Short4> operator=(const Reference<UShort4> &rhs) const;

		static Type *getType();
	};

	RValue<Short4> operator+(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator-(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator*(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Short4> operator/(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Short4> operator%(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator&(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator|(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator^(RValue<Short4> lhs, RValue<Short4> rhs);
	RValue<Short4> operator<<(RValue<Short4> lhs, unsigned char rhs);
	RValue<Short4> operator>>(RValue<Short4> lhs, unsigned char rhs);
	RValue<Short4> operator<<(RValue<Short4> lhs, RValue<Long1> rhs);
	RValue<Short4> operator>>(RValue<Short4> lhs, RValue<Long1> rhs);
	RValue<Short4> operator+=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator-=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator*=(const Short4 &lhs, RValue<Short4> rhs);
//	RValue<Short4> operator/=(const Short4 &lhs, RValue<Short4> rhs);
//	RValue<Short4> operator%=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator&=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator|=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator^=(const Short4 &lhs, RValue<Short4> rhs);
	RValue<Short4> operator<<=(const Short4 &lhs, unsigned char rhs);
	RValue<Short4> operator>>=(const Short4 &lhs, unsigned char rhs);
	RValue<Short4> operator<<=(const Short4 &lhs, RValue<Long1> rhs);
	RValue<Short4> operator>>=(const Short4 &lhs, RValue<Long1> rhs);
//	RValue<Short4> operator+(RValue<Short4> val);
	RValue<Short4> operator-(RValue<Short4> val);
	RValue<Short4> operator~(RValue<Short4> val);
//	RValue<Short4> operator++(const Short4 &val, int);   // Post-increment
//	const Short4 &operator++(const Short4 &val);   // Pre-increment
//	RValue<Short4> operator--(const Short4 &val, int);   // Post-decrement
//	const Short4 &operator--(const Short4 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Bool> operator<=(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Bool> operator>(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Bool> operator>=(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Bool> operator!=(RValue<Short4> lhs, RValue<Short4> rhs);
//	RValue<Bool> operator==(RValue<Short4> lhs, RValue<Short4> rhs);

	RValue<Short4> RoundShort4(RValue<Float4> cast);
	RValue<Short4> Max(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> Min(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> AddSat(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> SubSat(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> MulHigh(RValue<Short4> x, RValue<Short4> y);
	RValue<Int2> MulAdd(RValue<Short4> x, RValue<Short4> y);
	RValue<SByte8> Pack(RValue<Short4> x, RValue<Short4> y);
	RValue<Int2> UnpackLow(RValue<Short4> x, RValue<Short4> y);
	RValue<Int2> UnpackHigh(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> Swizzle(RValue<Short4> x, unsigned char select);
	RValue<Short4> Insert(RValue<Short4> val, RValue<Short> element, int i);
	RValue<Short> Extract(RValue<Short4> val, int i);
	RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y);
	RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y);

	class UShort4 : public Variable<UShort4>
	{
	public:
		explicit UShort4(RValue<Int4> cast);
		explicit UShort4(RValue<Float4> cast, bool saturate = false);

		UShort4();
		UShort4(unsigned short xyzw);
		UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w);
		UShort4(RValue<UShort4> rhs);
		UShort4(const UShort4 &rhs);
		UShort4(const Reference<UShort4> &rhs);
		UShort4(RValue<Short4> rhs);
		UShort4(const Short4 &rhs);
		UShort4(const Reference<Short4> &rhs);

		RValue<UShort4> operator=(RValue<UShort4> rhs) const;
		RValue<UShort4> operator=(const UShort4 &rhs) const;
		RValue<UShort4> operator=(const Reference<UShort4> &rhs) const;
		RValue<UShort4> operator=(RValue<Short4> rhs) const;
		RValue<UShort4> operator=(const Short4 &rhs) const;
		RValue<UShort4> operator=(const Reference<Short4> &rhs) const;

		static Type *getType();
	};

	RValue<UShort4> operator+(RValue<UShort4> lhs, RValue<UShort4> rhs);
	RValue<UShort4> operator-(RValue<UShort4> lhs, RValue<UShort4> rhs);
	RValue<UShort4> operator*(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator/(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator%(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator&(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator|(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator^(RValue<UShort4> lhs, RValue<UShort4> rhs);
	RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs);
	RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs);
	RValue<UShort4> operator<<(RValue<UShort4> lhs, RValue<Long1> rhs);
	RValue<UShort4> operator>>(RValue<UShort4> lhs, RValue<Long1> rhs);
//	RValue<UShort4> operator+=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator-=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator*=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator/=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator%=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator&=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator|=(const UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator^=(const UShort4 &lhs, RValue<UShort4> rhs);
	RValue<UShort4> operator<<=(const UShort4 &lhs, unsigned char rhs);
	RValue<UShort4> operator>>=(const UShort4 &lhs, unsigned char rhs);
	RValue<UShort4> operator<<=(const UShort4 &lhs, RValue<Long1> rhs);
	RValue<UShort4> operator>>=(const UShort4 &lhs, RValue<Long1> rhs);
//	RValue<UShort4> operator+(RValue<UShort4> val);
//	RValue<UShort4> operator-(RValue<UShort4> val);
	RValue<UShort4> operator~(RValue<UShort4> val);
//	RValue<UShort4> operator++(const UShort4 &val, int);   // Post-increment
//	const UShort4 &operator++(const UShort4 &val);   // Pre-increment
//	RValue<UShort4> operator--(const UShort4 &val, int);   // Post-decrement
//	const UShort4 &operator--(const UShort4 &val);   // Pre-decrement

	RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y);
	RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y);
	RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y);
	RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y);
	RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y);
	RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y);
	RValue<Byte8> Pack(RValue<UShort4> x, RValue<UShort4> y);

	class Short8 : public Variable<Short8>
	{
	public:
	//	Short8();
		Short8(short c0, short c1, short c2, short c3, short c4, short c5, short c6, short c7);
		Short8(RValue<Short8> rhs);
	//	Short8(const Short8 &rhs);
		Short8(const Reference<Short8> &rhs);
		Short8(RValue<Short4> lo, RValue<Short4> hi);

	//	RValue<Short8> operator=(RValue<Short8> rhs) const;
	//	RValue<Short8> operator=(const Short8 &rhs) const;
	//	RValue<Short8> operator=(const Reference<Short8> &rhs) const;

		static Type *getType();
	};

	RValue<Short8> operator+(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator-(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator*(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator/(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator%(RValue<Short8> lhs, RValue<Short8> rhs);
	RValue<Short8> operator&(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator|(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator^(RValue<Short8> lhs, RValue<Short8> rhs);
	RValue<Short8> operator<<(RValue<Short8> lhs, unsigned char rhs);
	RValue<Short8> operator>>(RValue<Short8> lhs, unsigned char rhs);
//	RValue<Short8> operator<<(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator>>(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Short8> operator+=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator-=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator*=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator/=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator%=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator&=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator|=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator^=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator<<=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator>>=(const Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator+(RValue<Short8> val);
//	RValue<Short8> operator-(RValue<Short8> val);
//	RValue<Short8> operator~(RValue<Short8> val);
//	RValue<Short8> operator++(const Short8 &val, int);   // Post-increment
//	const Short8 &operator++(const Short8 &val);   // Pre-increment
//	RValue<Short8> operator--(const Short8 &val, int);   // Post-decrement
//	const Short8 &operator--(const Short8 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator<=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator>(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator>=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator!=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator==(RValue<Short8> lhs, RValue<Short8> rhs);

	RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y);
	RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y);
	RValue<Int4> Abs(RValue<Int4> x);

	class UShort8 : public Variable<UShort8>
	{
	public:
	//	UShort8();
		UShort8(unsigned short c0, unsigned short c1, unsigned short c2, unsigned short c3, unsigned short c4, unsigned short c5, unsigned short c6, unsigned short c7);
		UShort8(RValue<UShort8> rhs);
	//	UShort8(const UShort8 &rhs);
		UShort8(const Reference<UShort8> &rhs);
		UShort8(RValue<UShort4> lo, RValue<UShort4> hi);

		RValue<UShort8> operator=(RValue<UShort8> rhs) const;
		RValue<UShort8> operator=(const UShort8 &rhs) const;
		RValue<UShort8> operator=(const Reference<UShort8> &rhs) const;

		static Type *getType();
	};

	RValue<UShort8> operator+(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator-(RValue<UShort8> lhs, RValue<UShort8> rhs);
	RValue<UShort8> operator*(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator/(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator%(RValue<UShort8> lhs, RValue<UShort8> rhs);
	RValue<UShort8> operator&(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator|(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator^(RValue<UShort8> lhs, RValue<UShort8> rhs);
	RValue<UShort8> operator<<(RValue<UShort8> lhs, unsigned char rhs);
	RValue<UShort8> operator>>(RValue<UShort8> lhs, unsigned char rhs);
//	RValue<UShort8> operator<<(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator>>(RValue<UShort8> lhs, RValue<UShort8> rhs);
	RValue<UShort8> operator+=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator-=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator*=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator/=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator%=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator&=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator|=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator^=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator<<=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator>>=(const UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator+(RValue<UShort8> val);
//	RValue<UShort8> operator-(RValue<UShort8> val);
	RValue<UShort8> operator~(RValue<UShort8> val);
//	RValue<UShort8> operator++(const UShort8 &val, int);   // Post-increment
//	const UShort8 &operator++(const UShort8 &val);   // Pre-increment
//	RValue<UShort8> operator--(const UShort8 &val, int);   // Post-decrement
//	const UShort8 &operator--(const UShort8 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator<=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator>(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator>=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator!=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator==(RValue<UShort8> lhs, RValue<UShort8> rhs);

	RValue<UShort8> Swizzle(RValue<UShort8> x, char select0, char select1, char select2, char select3, char select4, char select5, char select6, char select7);
	RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y);

	class Int : public Variable<Int>
	{
	public:
		Int(Argument<Int> argument);

		explicit Int(RValue<Byte> cast);
		explicit Int(RValue<SByte> cast);
		explicit Int(RValue<Short> cast);
		explicit Int(RValue<UShort> cast);
		explicit Int(RValue<Int2> cast);
		explicit Int(RValue<Long> cast);
		explicit Int(RValue<Float> cast);

		Int();
		Int(int x);
		Int(RValue<Int> rhs);
		Int(RValue<UInt> rhs);
		Int(const Int &rhs);
		Int(const UInt &rhs);
		Int(const Reference<Int> &rhs);
		Int(const Reference<UInt> &rhs);

		RValue<Int> operator=(int rhs) const;
		RValue<Int> operator=(RValue<Int> rhs) const;
		RValue<Int> operator=(RValue<UInt> rhs) const;
		RValue<Int> operator=(const Int &rhs) const;
		RValue<Int> operator=(const UInt &rhs) const;
		RValue<Int> operator=(const Reference<Int> &rhs) const;
		RValue<Int> operator=(const Reference<UInt> &rhs) const;

		static Type *getType();
	};

	RValue<Int> operator+(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator-(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator*(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator/(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator%(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator&(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator|(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator^(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator<<(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator>>(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Int> operator+=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator-=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator*=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator/=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator%=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator&=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator|=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator^=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator<<=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator>>=(const Int &lhs, RValue<Int> rhs);
	RValue<Int> operator+(RValue<Int> val);
	RValue<Int> operator-(RValue<Int> val);
	RValue<Int> operator~(RValue<Int> val);
	RValue<Int> operator++(const Int &val, int);   // Post-increment
	const Int &operator++(const Int &val);   // Pre-increment
	RValue<Int> operator--(const Int &val, int);   // Post-decrement
	const Int &operator--(const Int &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Bool> operator<=(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Bool> operator>(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Bool> operator>=(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Bool> operator!=(RValue<Int> lhs, RValue<Int> rhs);
	RValue<Bool> operator==(RValue<Int> lhs, RValue<Int> rhs);

	RValue<Int> Max(RValue<Int> x, RValue<Int> y);
	RValue<Int> Min(RValue<Int> x, RValue<Int> y);
	RValue<Int> Clamp(RValue<Int> x, RValue<Int> min, RValue<Int> max);
	RValue<Int> RoundInt(RValue<Float> cast);

	class Long : public Variable<Long>
	{
	public:
	//	Long(Argument<Long> argument);

	//	explicit Long(RValue<Short> cast);
	//	explicit Long(RValue<UShort> cast);
		explicit Long(RValue<Int> cast);
		explicit Long(RValue<UInt> cast);
	//	explicit Long(RValue<Float> cast);

		Long();
	//	Long(qword x);
		Long(RValue<Long> rhs);
	//	Long(RValue<ULong> rhs);
	//	Long(const Long &rhs);
	//	Long(const Reference<Long> &rhs);
	//	Long(const ULong &rhs);
	//	Long(const Reference<ULong> &rhs);

		RValue<Long> operator=(int64_t rhs) const;
		RValue<Long> operator=(RValue<Long> rhs) const;
	//	RValue<Long> operator=(RValue<ULong> rhs) const;
		RValue<Long> operator=(const Long &rhs) const;
		RValue<Long> operator=(const Reference<Long> &rhs) const;
	//	RValue<Long> operator=(const ULong &rhs) const;
	//	RValue<Long> operator=(const Reference<ULong> &rhs) const;

		static Type *getType();
	};

	RValue<Long> operator+(RValue<Long> lhs, RValue<Long> rhs);
	RValue<Long> operator-(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator*(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator/(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator%(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator&(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator|(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator^(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator<<(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator>>(RValue<Long> lhs, RValue<Long> rhs);
	RValue<Long> operator+=(const Long &lhs, RValue<Long> rhs);
	RValue<Long> operator-=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator*=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator/=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator%=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator&=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator|=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator^=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator<<=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator>>=(const Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator+(RValue<Long> val);
//	RValue<Long> operator-(RValue<Long> val);
//	RValue<Long> operator~(RValue<Long> val);
//	RValue<Long> operator++(const Long &val, int);   // Post-increment
//	const Long &operator++(const Long &val);   // Pre-increment
//	RValue<Long> operator--(const Long &val, int);   // Post-decrement
//	const Long &operator--(const Long &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator<=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator>(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator>=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator!=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator==(RValue<Long> lhs, RValue<Long> rhs);

//	RValue<Long> RoundLong(RValue<Float> cast);
	RValue<Long> AddAtomic( RValue<Pointer<Long>> x, RValue<Long> y);

	class Long1 : public Variable<Long1>
	{
	public:
	//	Long1(Argument<Long1> argument);

	//	explicit Long1(RValue<Short> cast);
	//	explicit Long1(RValue<UShort> cast);
	//	explicit Long1(RValue<Int> cast);
		explicit Long1(RValue<UInt> cast);
	//	explicit Long1(RValue<Float> cast);

	//	Long1();
	//	Long1(qword x);
		Long1(RValue<Long1> rhs);
	//	Long1(RValue<ULong1> rhs);
	//	Long1(const Long1 &rhs);
	//	Long1(const Reference<Long1> &rhs);
	//	Long1(const ULong1 &rhs);
	//	Long1(const Reference<ULong1> &rhs);

	//	RValue<Long1> operator=(qword rhs) const;
	//	RValue<Long1> operator=(RValue<Long1> rhs) const;
	//	RValue<Long1> operator=(RValue<ULong1> rhs) const;
	//	RValue<Long1> operator=(const Long1 &rhs) const;
	//	RValue<Long1> operator=(const Reference<Long1> &rhs) const;
	//	RValue<Long1> operator=(const ULong1 &rhs) const;
	//	RValue<Long1> operator=(const Reference<ULong1> &rhs) const;

		static Type *getType();
	};

//	RValue<Long1> operator+(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator-(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator*(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator/(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator%(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator&(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator|(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator^(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator<<(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator>>(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Long1> operator+=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator-=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator*=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator/=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator%=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator&=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator|=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator^=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator<<=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator>>=(const Long1 &lhs, RValue<Long1> rhs);
//	RValue<Long1> operator+(RValue<Long1> val);
//	RValue<Long1> operator-(RValue<Long1> val);
//	RValue<Long1> operator~(RValue<Long1> val);
//	RValue<Long1> operator++(const Long1 &val, int);   // Post-increment
//	const Long1 &operator++(const Long1 &val);   // Pre-increment
//	RValue<Long1> operator--(const Long1 &val, int);   // Post-decrement
//	const Long1 &operator--(const Long1 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Bool> operator<=(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Bool> operator>(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Bool> operator>=(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Bool> operator!=(RValue<Long1> lhs, RValue<Long1> rhs);
//	RValue<Bool> operator==(RValue<Long1> lhs, RValue<Long1> rhs);

//	RValue<Long1> RoundLong1(RValue<Float> cast);

	class Long2 : public Variable<Long2>
	{
	public:
	//	explicit Long2(RValue<Long> cast);
	//	explicit Long2(RValue<Long1> cast);

	//	Long2();
	//	Long2(int x, int y);
	//	Long2(RValue<Long2> rhs);
	//	Long2(const Long2 &rhs);
	//	Long2(const Reference<Long2> &rhs);

	//	RValue<Long2> operator=(RValue<Long2> rhs) const;
	//	RValue<Long2> operator=(const Long2 &rhs) const;
	//	RValue<Long2> operator=(const Reference<Long2 &rhs) const;

		static Type *getType();
	};

//	RValue<Long2> operator+(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator-(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator*(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator/(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator%(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator&(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator|(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator^(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Long2> operator<<(RValue<Long2> lhs, unsigned char rhs);
//	RValue<Long2> operator>>(RValue<Long2> lhs, unsigned char rhs);
//	RValue<Long2> operator<<(RValue<Long2> lhs, RValue<Long1> rhs);
//	RValue<Long2> operator>>(RValue<Long2> lhs, RValue<Long1> rhs);
//	RValue<Long2> operator+=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator-=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator*=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator/=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator%=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator&=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator|=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator^=(const Long2 &lhs, RValue<Long2> rhs);
//	RValue<Long2> operator<<=(const Long2 &lhs, unsigned char rhs);
//	RValue<Long2> operator>>=(const Long2 &lhs, unsigned char rhs);
//	RValue<Long2> operator<<=(const Long2 &lhs, RValue<Long1> rhs);
//	RValue<Long2> operator>>=(const Long2 &lhs, RValue<Long1> rhs);
//	RValue<Long2> operator+(RValue<Long2> val);
//	RValue<Long2> operator-(RValue<Long2> val);
//	RValue<Long2> operator~(RValue<Long2> val);
//	RValue<Long2> operator++(const Long2 &val, int);   // Post-increment
//	const Long2 &operator++(const Long2 &val);   // Pre-increment
//	RValue<Long2> operator--(const Long2 &val, int);   // Post-decrement
//	const Long2 &operator--(const Long2 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Bool> operator<=(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Bool> operator>(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Bool> operator>=(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Bool> operator!=(RValue<Long2> lhs, RValue<Long2> rhs);
//	RValue<Bool> operator==(RValue<Long2> lhs, RValue<Long2> rhs);

//	RValue<Long2> RoundInt(RValue<Float4> cast);
//	RValue<Long2> UnpackLow(RValue<Long2> x, RValue<Long2> y);
	RValue<Long2> UnpackHigh(RValue<Long2> x, RValue<Long2> y);
//	RValue<Int> Extract(RValue<Long2> val, int i);
//	RValue<Long2> Insert(RValue<Long2> val, RValue<Int> element, int i);

	class UInt : public Variable<UInt>
	{
	public:
		UInt(Argument<UInt> argument);

		explicit UInt(RValue<UShort> cast);
		explicit UInt(RValue<Long> cast);
		explicit UInt(RValue<Float> cast);

		UInt();
		UInt(int x);
		UInt(unsigned int x);
		UInt(RValue<UInt> rhs);
		UInt(RValue<Int> rhs);
		UInt(const UInt &rhs);
		UInt(const Int &rhs);
		UInt(const Reference<UInt> &rhs);
		UInt(const Reference<Int> &rhs);

		RValue<UInt> operator=(unsigned int rhs) const;
		RValue<UInt> operator=(RValue<UInt> rhs) const;
		RValue<UInt> operator=(RValue<Int> rhs) const;
		RValue<UInt> operator=(const UInt &rhs) const;
		RValue<UInt> operator=(const Int &rhs) const;
		RValue<UInt> operator=(const Reference<UInt> &rhs) const;
		RValue<UInt> operator=(const Reference<Int> &rhs) const;

		static Type *getType();
	};

	RValue<UInt> operator+(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator-(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator*(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator/(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator%(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator&(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator|(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator^(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator<<(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator>>(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<UInt> operator+=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator-=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator*=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator/=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator%=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator&=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator|=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator^=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator<<=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator>>=(const UInt &lhs, RValue<UInt> rhs);
	RValue<UInt> operator+(RValue<UInt> val);
	RValue<UInt> operator-(RValue<UInt> val);
	RValue<UInt> operator~(RValue<UInt> val);
	RValue<UInt> operator++(const UInt &val, int);   // Post-increment
	const UInt &operator++(const UInt &val);   // Pre-increment
	RValue<UInt> operator--(const UInt &val, int);   // Post-decrement
	const UInt &operator--(const UInt &val);   // Pre-decrement
	RValue<Bool> operator<(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<Bool> operator<=(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<Bool> operator>(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<Bool> operator>=(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<Bool> operator!=(RValue<UInt> lhs, RValue<UInt> rhs);
	RValue<Bool> operator==(RValue<UInt> lhs, RValue<UInt> rhs);

	RValue<UInt> Max(RValue<UInt> x, RValue<UInt> y);
	RValue<UInt> Min(RValue<UInt> x, RValue<UInt> y);
	RValue<UInt> Clamp(RValue<UInt> x, RValue<UInt> min, RValue<UInt> max);
//	RValue<UInt> RoundUInt(RValue<Float> cast);

	class Int2 : public Variable<Int2>
	{
	public:
	//	explicit Int2(RValue<Int> cast);
		explicit Int2(RValue<Int4> cast);

		Int2();
		Int2(int x, int y);
		Int2(RValue<Int2> rhs);
		Int2(const Int2 &rhs);
		Int2(const Reference<Int2> &rhs);
		Int2(RValue<Int> lo, RValue<Int> hi);

		RValue<Int2> operator=(RValue<Int2> rhs) const;
		RValue<Int2> operator=(const Int2 &rhs) const;
		RValue<Int2> operator=(const Reference<Int2> &rhs) const;

		static Type *getType();
	};

	RValue<Int2> operator+(RValue<Int2> lhs, RValue<Int2> rhs);
	RValue<Int2> operator-(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Int2> operator*(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Int2> operator/(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Int2> operator%(RValue<Int2> lhs, RValue<Int2> rhs);
	RValue<Int2> operator&(RValue<Int2> lhs, RValue<Int2> rhs);
	RValue<Int2> operator|(RValue<Int2> lhs, RValue<Int2> rhs);
	RValue<Int2> operator^(RValue<Int2> lhs, RValue<Int2> rhs);
	RValue<Int2> operator<<(RValue<Int2> lhs, unsigned char rhs);
	RValue<Int2> operator>>(RValue<Int2> lhs, unsigned char rhs);
	RValue<Int2> operator<<(RValue<Int2> lhs, RValue<Long1> rhs);
	RValue<Int2> operator>>(RValue<Int2> lhs, RValue<Long1> rhs);
	RValue<Int2> operator+=(const Int2 &lhs, RValue<Int2> rhs);
	RValue<Int2> operator-=(const Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator*=(const Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator/=(const Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator%=(const Int2 &lhs, RValue<Int2> rhs);
	RValue<Int2> operator&=(const Int2 &lhs, RValue<Int2> rhs);
	RValue<Int2> operator|=(const Int2 &lhs, RValue<Int2> rhs);
	RValue<Int2> operator^=(const Int2 &lhs, RValue<Int2> rhs);
	RValue<Int2> operator<<=(const Int2 &lhs, unsigned char rhs);
	RValue<Int2> operator>>=(const Int2 &lhs, unsigned char rhs);
	RValue<Int2> operator<<=(const Int2 &lhs, RValue<Long1> rhs);
	RValue<Int2> operator>>=(const Int2 &lhs, RValue<Long1> rhs);
//	RValue<Int2> operator+(RValue<Int2> val);
//	RValue<Int2> operator-(RValue<Int2> val);
	RValue<Int2> operator~(RValue<Int2> val);
//	RValue<Int2> operator++(const Int2 &val, int);   // Post-increment
//	const Int2 &operator++(const Int2 &val);   // Pre-increment
//	RValue<Int2> operator--(const Int2 &val, int);   // Post-decrement
//	const Int2 &operator--(const Int2 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator<=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator>(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator>=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator!=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator==(RValue<Int2> lhs, RValue<Int2> rhs);

//	RValue<Int2> RoundInt(RValue<Float4> cast);
	RValue<Long1> UnpackLow(RValue<Int2> x, RValue<Int2> y);
	RValue<Long1> UnpackHigh(RValue<Int2> x, RValue<Int2> y);
	RValue<Int> Extract(RValue<Int2> val, int i);
	RValue<Int2> Insert(RValue<Int2> val, RValue<Int> element, int i);

	class UInt2 : public Variable<UInt2>
	{
	public:
		UInt2();
		UInt2(unsigned int x, unsigned int y);
		UInt2(RValue<UInt2> rhs);
		UInt2(const UInt2 &rhs);
		UInt2(const Reference<UInt2> &rhs);

		RValue<UInt2> operator=(RValue<UInt2> rhs) const;
		RValue<UInt2> operator=(const UInt2 &rhs) const;
		RValue<UInt2> operator=(const Reference<UInt2> &rhs) const;

		static Type *getType();
	};

	RValue<UInt2> operator+(RValue<UInt2> lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator-(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator*(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator/(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator%(RValue<UInt2> lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator&(RValue<UInt2> lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator|(RValue<UInt2> lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator^(RValue<UInt2> lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator<<(RValue<UInt2> lhs, unsigned char rhs);
	RValue<UInt2> operator>>(RValue<UInt2> lhs, unsigned char rhs);
	RValue<UInt2> operator<<(RValue<UInt2> lhs, RValue<Long1> rhs);
	RValue<UInt2> operator>>(RValue<UInt2> lhs, RValue<Long1> rhs);
	RValue<UInt2> operator+=(const UInt2 &lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator-=(const UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator*=(const UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator/=(const UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator%=(const UInt2 &lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator&=(const UInt2 &lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator|=(const UInt2 &lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator^=(const UInt2 &lhs, RValue<UInt2> rhs);
	RValue<UInt2> operator<<=(const UInt2 &lhs, unsigned char rhs);
	RValue<UInt2> operator>>=(const UInt2 &lhs, unsigned char rhs);
	RValue<UInt2> operator<<=(const UInt2 &lhs, RValue<Long1> rhs);
	RValue<UInt2> operator>>=(const UInt2 &lhs, RValue<Long1> rhs);
//	RValue<UInt2> operator+(RValue<UInt2> val);
//	RValue<UInt2> operator-(RValue<UInt2> val);
	RValue<UInt2> operator~(RValue<UInt2> val);
//	RValue<UInt2> operator++(const UInt2 &val, int);   // Post-increment
//	const UInt2 &operator++(const UInt2 &val);   // Pre-increment
//	RValue<UInt2> operator--(const UInt2 &val, int);   // Post-decrement
//	const UInt2 &operator--(const UInt2 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator<=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator>(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator>=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator!=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator==(RValue<UInt2> lhs, RValue<UInt2> rhs);

//	RValue<UInt2> RoundInt(RValue<Float4> cast);

	class Int4 : public Variable<Int4>
	{
	public:
		explicit Int4(RValue<Byte4> cast);
		explicit Int4(RValue<SByte4> cast);
		explicit Int4(RValue<Float4> cast);
		explicit Int4(RValue<Short4> cast);
		explicit Int4(RValue<UShort4> cast);

		Int4();
		Int4(int xyzw);
		Int4(int x, int yzw);
		Int4(int x, int y, int zw);
		Int4(int x, int y, int z, int w);
		Int4(RValue<Int4> rhs);
		Int4(const Int4 &rhs);
		Int4(const Reference<Int4> &rhs);
		Int4(RValue<UInt4> rhs);
		Int4(const UInt4 &rhs);
		Int4(const Reference<UInt4> &rhs);
		Int4(RValue<Int2> lo, RValue<Int2> hi);
		Int4(RValue<Int> rhs);
		Int4(const Int &rhs);
		Int4(const Reference<Int> &rhs);

		RValue<Int4> operator=(RValue<Int4> rhs) const;
		RValue<Int4> operator=(const Int4 &rhs) const;
		RValue<Int4> operator=(const Reference<Int4> &rhs) const;

		static Type *getType();

	private:
		void constant(int x, int y, int z, int w);
	};

	RValue<Int4> operator+(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator-(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator*(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator/(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator%(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator&(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator|(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator^(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator<<(RValue<Int4> lhs, unsigned char rhs);
	RValue<Int4> operator>>(RValue<Int4> lhs, unsigned char rhs);
	RValue<Int4> operator<<(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator>>(RValue<Int4> lhs, RValue<Int4> rhs);
	RValue<Int4> operator+=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator-=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator*=(const Int4 &lhs, RValue<Int4> rhs);
//	RValue<Int4> operator/=(const Int4 &lhs, RValue<Int4> rhs);
//	RValue<Int4> operator%=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator&=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator|=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator^=(const Int4 &lhs, RValue<Int4> rhs);
	RValue<Int4> operator<<=(const Int4 &lhs, unsigned char rhs);
	RValue<Int4> operator>>=(const Int4 &lhs, unsigned char rhs);
	RValue<Int4> operator+(RValue<Int4> val);
	RValue<Int4> operator-(RValue<Int4> val);
	RValue<Int4> operator~(RValue<Int4> val);
//	RValue<Int4> operator++(const Int4 &val, int);   // Post-increment
//	const Int4 &operator++(const Int4 &val);   // Pre-increment
//	RValue<Int4> operator--(const Int4 &val, int);   // Post-decrement
//	const Int4 &operator--(const Int4 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator<=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator>(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator>=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator!=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator==(RValue<Int4> lhs, RValue<Int4> rhs);

	RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y);
	RValue<Int4> RoundInt(RValue<Float4> cast);
	RValue<Short8> Pack(RValue<Int4> x, RValue<Int4> y);
	RValue<Int> Extract(RValue<Int4> x, int i);
	RValue<Int4> Insert(RValue<Int4> val, RValue<Int> element, int i);
	RValue<Int> SignMask(RValue<Int4> x);
	RValue<Int4> Swizzle(RValue<Int4> x, unsigned char select);

	class UInt4 : public Variable<UInt4>
	{
	public:
		explicit UInt4(RValue<Float4> cast);

		UInt4();
		UInt4(int xyzw);
		UInt4(int x, int yzw);
		UInt4(int x, int y, int zw);
		UInt4(int x, int y, int z, int w);
		UInt4(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
		UInt4(RValue<UInt4> rhs);
		UInt4(const UInt4 &rhs);
		UInt4(const Reference<UInt4> &rhs);
		UInt4(RValue<Int4> rhs);
		UInt4(const Int4 &rhs);
		UInt4(const Reference<Int4> &rhs);
		UInt4(RValue<UInt2> lo, RValue<UInt2> hi);

		RValue<UInt4> operator=(RValue<UInt4> rhs) const;
		RValue<UInt4> operator=(const UInt4 &rhs) const;
		RValue<UInt4> operator=(const Reference<UInt4> &rhs) const;

		static Type *getType();

	private:
		void constant(int x, int y, int z, int w);
	};

	RValue<UInt4> operator+(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator-(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator*(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator/(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator%(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator&(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator|(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator^(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator<<(RValue<UInt4> lhs, unsigned char rhs);
	RValue<UInt4> operator>>(RValue<UInt4> lhs, unsigned char rhs);
	RValue<UInt4> operator<<(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator>>(RValue<UInt4> lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator+=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator-=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator*=(const UInt4 &lhs, RValue<UInt4> rhs);
//	RValue<UInt4> operator/=(const UInt4 &lhs, RValue<UInt4> rhs);
//	RValue<UInt4> operator%=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator&=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator|=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator^=(const UInt4 &lhs, RValue<UInt4> rhs);
	RValue<UInt4> operator<<=(const UInt4 &lhs, unsigned char rhs);
	RValue<UInt4> operator>>=(const UInt4 &lhs, unsigned char rhs);
	RValue<UInt4> operator+(RValue<UInt4> val);
	RValue<UInt4> operator-(RValue<UInt4> val);
	RValue<UInt4> operator~(RValue<UInt4> val);
//	RValue<UInt4> operator++(const UInt4 &val, int);   // Post-increment
//	const UInt4 &operator++(const UInt4 &val);   // Pre-increment
//	RValue<UInt4> operator--(const UInt4 &val, int);   // Post-decrement
//	const UInt4 &operator--(const UInt4 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<UInt4> lhs, RValue<UInt4> rhs);
//	RValue<Bool> operator<=(RValue<UInt4> lhs, RValue<UInt4> rhs);
//	RValue<Bool> operator>(RValue<UInt4> lhs, RValue<UInt4> rhs);
//	RValue<Bool> operator>=(RValue<UInt4> lhs, RValue<UInt4> rhs);
//	RValue<Bool> operator!=(RValue<UInt4> lhs, RValue<UInt4> rhs);
//	RValue<Bool> operator==(RValue<UInt4> lhs, RValue<UInt4> rhs);

	RValue<UInt4> CmpEQ(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> CmpLT(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> CmpLE(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> CmpNEQ(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> CmpNLT(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> CmpNLE(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y);
	RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y);
//	RValue<UInt4> RoundInt(RValue<Float4> cast);
	RValue<UShort8> Pack(RValue<UInt4> x, RValue<UInt4> y);

	template<int T>
	class Swizzle2Float4
	{
		friend class Float4;

	public:
		operator RValue<Float4>() const;

	private:
		Float4 *parent;
	};

	template<int T>
	class SwizzleFloat4
	{
	public:
		operator RValue<Float4>() const;

	private:
		Float4 *parent;
	};

	template<int T>
	class SwizzleMaskFloat4
	{
		friend class Float4;

	public:
		operator RValue<Float4>() const;

		RValue<Float4> operator=(RValue<Float4> rhs) const;
		RValue<Float4> operator=(RValue<Float> rhs) const;

	private:
		Float4 *parent;
	};

	template<int T>
	class SwizzleMask1Float4
	{
	public:
		operator RValue<Float>() const;
		operator RValue<Float4>() const;

		RValue<Float4> operator=(float x) const;
		RValue<Float4> operator=(RValue<Float4> rhs) const;
		RValue<Float4> operator=(RValue<Float> rhs) const;

	private:
		Float4 *parent;
	};

	template<int T>
	class SwizzleMask2Float4
	{
		friend class Float4;

	public:
		operator RValue<Float4>() const;

		RValue<Float4> operator=(RValue<Float4> rhs) const;

	private:
		Float4 *parent;
	};

	class Float : public Variable<Float>
	{
	public:
		explicit Float(RValue<Int> cast);

		Float();
		Float(float x);
		Float(RValue<Float> rhs);
		Float(const Float &rhs);
		Float(const Reference<Float> &rhs);

		template<int T>
		Float(const SwizzleMask1Float4<T> &rhs);

	//	RValue<Float> operator=(float rhs) const;   // FIXME: Implement
		RValue<Float> operator=(RValue<Float> rhs) const;
		RValue<Float> operator=(const Float &rhs) const;
		RValue<Float> operator=(const Reference<Float> &rhs) const;

		template<int T>
		RValue<Float> operator=(const SwizzleMask1Float4<T> &rhs) const;

		static Type *getType();
	};

	RValue<Float> operator+(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Float> operator-(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Float> operator*(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Float> operator/(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Float> operator+=(const Float &lhs, RValue<Float> rhs);
	RValue<Float> operator-=(const Float &lhs, RValue<Float> rhs);
	RValue<Float> operator*=(const Float &lhs, RValue<Float> rhs);
	RValue<Float> operator/=(const Float &lhs, RValue<Float> rhs);
	RValue<Float> operator+(RValue<Float> val);
	RValue<Float> operator-(RValue<Float> val);
	RValue<Bool> operator<(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Bool> operator<=(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Bool> operator>(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Bool> operator>=(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Bool> operator!=(RValue<Float> lhs, RValue<Float> rhs);
	RValue<Bool> operator==(RValue<Float> lhs, RValue<Float> rhs);

	RValue<Float> Abs(RValue<Float> x);
	RValue<Float> Max(RValue<Float> x, RValue<Float> y);
	RValue<Float> Min(RValue<Float> x, RValue<Float> y);
	RValue<Float> Rcp_pp(RValue<Float> val, bool exactAtPow2 = false);
	RValue<Float> RcpSqrt_pp(RValue<Float> val);
	RValue<Float> Sqrt(RValue<Float> x);
	RValue<Float> Round(RValue<Float> val);
	RValue<Float> Trunc(RValue<Float> val);
	RValue<Float> Frac(RValue<Float> val);
	RValue<Float> Floor(RValue<Float> val);
	RValue<Float> Ceil(RValue<Float> val);

	class Float2 : public Variable<Float2>
	{
	public:
	//	explicit Float2(RValue<Byte2> cast);
	//	explicit Float2(RValue<Short2> cast);
	//	explicit Float2(RValue<UShort2> cast);
	//	explicit Float2(RValue<Int2> cast);
	//	explicit Float2(RValue<UInt2> cast);
		explicit Float2(RValue<Float4> cast);

	//	Float2();
	//	Float2(float x, float y);
	//	Float2(RValue<Float2> rhs);
	//	Float2(const Float2 &rhs);
	//	Float2(const Reference<Float2> &rhs);
	//	Float2(RValue<Float> rhs);
	//	Float2(const Float &rhs);
	//	Float2(const Reference<Float> &rhs);

	//	template<int T>
	//	Float2(const SwizzleMask1Float4<T> &rhs);

	//	RValue<Float2> operator=(float replicate) const;
	//	RValue<Float2> operator=(RValue<Float2> rhs) const;
	//	RValue<Float2> operator=(const Float2 &rhs) const;
	//	RValue<Float2> operator=(const Reference<Float2> &rhs) const;
	//	RValue<Float2> operator=(RValue<Float> rhs) const;
	//	RValue<Float2> operator=(const Float &rhs) const;
	//	RValue<Float2> operator=(const Reference<Float> &rhs) const;

	//	template<int T>
	//	RValue<Float2> operator=(const SwizzleMask1Float4<T> &rhs);

		static Type *getType();
	};

//	RValue<Float2> operator+(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator-(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator*(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator/(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator%(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator+=(const Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator-=(const Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator*=(const Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator/=(const Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator%=(const Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator+(RValue<Float2> val);
//	RValue<Float2> operator-(RValue<Float2> val);

//	RValue<Float2> Abs(RValue<Float2> x);
//	RValue<Float2> Max(RValue<Float2> x, RValue<Float2> y);
//	RValue<Float2> Min(RValue<Float2> x, RValue<Float2> y);
//	RValue<Float2> Swizzle(RValue<Float2> x, unsigned char select);
//	RValue<Float2> Mask(Float2 &lhs, RValue<Float2> rhs, unsigned char select);

	class Float4 : public Variable<Float4>
	{
	public:
		explicit Float4(RValue<Byte4> cast);
		explicit Float4(RValue<SByte4> cast);
		explicit Float4(RValue<Short4> cast);
		explicit Float4(RValue<UShort4> cast);
		explicit Float4(RValue<Int4> cast);
		explicit Float4(RValue<UInt4> cast);

		Float4();
		Float4(float xyzw);
		Float4(float x, float yzw);
		Float4(float x, float y, float zw);
		Float4(float x, float y, float z, float w);
		Float4(RValue<Float4> rhs);
		Float4(const Float4 &rhs);
		Float4(const Reference<Float4> &rhs);
		Float4(RValue<Float> rhs);
		Float4(const Float &rhs);
		Float4(const Reference<Float> &rhs);

		template<int T>
		Float4(const SwizzleMask1Float4<T> &rhs);
		template<int T>
		Float4(const SwizzleFloat4<T> &rhs);
		template<int X, int Y>
		Float4(const Swizzle2Float4<X> &x, const Swizzle2Float4<Y> &y);
		template<int X, int Y>
		Float4(const SwizzleMask2Float4<X> &x, const Swizzle2Float4<Y> &y);
		template<int X, int Y>
		Float4(const Swizzle2Float4<X> &x, const SwizzleMask2Float4<Y> &y);
		template<int X, int Y>
		Float4(const SwizzleMask2Float4<X> &x, const SwizzleMask2Float4<Y> &y);

		RValue<Float4> operator=(float replicate) const;
		RValue<Float4> operator=(RValue<Float4> rhs) const;
		RValue<Float4> operator=(const Float4 &rhs) const;
		RValue<Float4> operator=(const Reference<Float4> &rhs) const;
		RValue<Float4> operator=(RValue<Float> rhs) const;
		RValue<Float4> operator=(const Float &rhs) const;
		RValue<Float4> operator=(const Reference<Float> &rhs) const;

		template<int T>
		RValue<Float4> operator=(const SwizzleMask1Float4<T> &rhs);
		template<int T>
		RValue<Float4> operator=(const SwizzleFloat4<T> &rhs);

		static Type *getType();

		union
		{
			SwizzleMask1Float4<0x00> x;
			SwizzleMask1Float4<0x55> y;
			SwizzleMask1Float4<0xAA> z;
			SwizzleMask1Float4<0xFF> w;
			Swizzle2Float4<0x00>     xx;
			Swizzle2Float4<0x01>     yx;
			Swizzle2Float4<0x02>     zx;
			Swizzle2Float4<0x03>     wx;
			SwizzleMask2Float4<0x54> xy;
			Swizzle2Float4<0x55>     yy;
			Swizzle2Float4<0x56>     zy;
			Swizzle2Float4<0x57>     wy;
			SwizzleMask2Float4<0xA8> xz;
			SwizzleMask2Float4<0xA9> yz;
			Swizzle2Float4<0xAA>     zz;
			Swizzle2Float4<0xAB>     wz;
			SwizzleMask2Float4<0xFC> xw;
			SwizzleMask2Float4<0xFD> yw;
			SwizzleMask2Float4<0xFE> zw;
			Swizzle2Float4<0xFF>     ww;
			SwizzleFloat4<0x00>      xxx;
			SwizzleFloat4<0x01>      yxx;
			SwizzleFloat4<0x02>      zxx;
			SwizzleFloat4<0x03>      wxx;
			SwizzleFloat4<0x04>      xyx;
			SwizzleFloat4<0x05>      yyx;
			SwizzleFloat4<0x06>      zyx;
			SwizzleFloat4<0x07>      wyx;
			SwizzleFloat4<0x08>      xzx;
			SwizzleFloat4<0x09>      yzx;
			SwizzleFloat4<0x0A>      zzx;
			SwizzleFloat4<0x0B>      wzx;
			SwizzleFloat4<0x0C>      xwx;
			SwizzleFloat4<0x0D>      ywx;
			SwizzleFloat4<0x0E>      zwx;
			SwizzleFloat4<0x0F>      wwx;
			SwizzleFloat4<0x50>      xxy;
			SwizzleFloat4<0x51>      yxy;
			SwizzleFloat4<0x52>      zxy;
			SwizzleFloat4<0x53>      wxy;
			SwizzleFloat4<0x54>      xyy;
			SwizzleFloat4<0x55>      yyy;
			SwizzleFloat4<0x56>      zyy;
			SwizzleFloat4<0x57>      wyy;
			SwizzleFloat4<0x58>      xzy;
			SwizzleFloat4<0x59>      yzy;
			SwizzleFloat4<0x5A>      zzy;
			SwizzleFloat4<0x5B>      wzy;
			SwizzleFloat4<0x5C>      xwy;
			SwizzleFloat4<0x5D>      ywy;
			SwizzleFloat4<0x5E>      zwy;
			SwizzleFloat4<0x5F>      wwy;
			SwizzleFloat4<0xA0>      xxz;
			SwizzleFloat4<0xA1>      yxz;
			SwizzleFloat4<0xA2>      zxz;
			SwizzleFloat4<0xA3>      wxz;
			SwizzleMaskFloat4<0xA4>  xyz;
			SwizzleFloat4<0xA5>      yyz;
			SwizzleFloat4<0xA6>      zyz;
			SwizzleFloat4<0xA7>      wyz;
			SwizzleFloat4<0xA8>      xzz;
			SwizzleFloat4<0xA9>      yzz;
			SwizzleFloat4<0xAA>      zzz;
			SwizzleFloat4<0xAB>      wzz;
			SwizzleFloat4<0xAC>      xwz;
			SwizzleFloat4<0xAD>      ywz;
			SwizzleFloat4<0xAE>      zwz;
			SwizzleFloat4<0xAF>      wwz;
			SwizzleFloat4<0xF0>      xxw;
			SwizzleFloat4<0xF1>      yxw;
			SwizzleFloat4<0xF2>      zxw;
			SwizzleFloat4<0xF3>      wxw;
			SwizzleMaskFloat4<0xF4>  xyw;
			SwizzleFloat4<0xF5>      yyw;
			SwizzleFloat4<0xF6>      zyw;
			SwizzleFloat4<0xF7>      wyw;
			SwizzleMaskFloat4<0xF8>  xzw;
			SwizzleMaskFloat4<0xF9>  yzw;
			SwizzleFloat4<0xFA>      zzw;
			SwizzleFloat4<0xFB>      wzw;
			SwizzleFloat4<0xFC>      xww;
			SwizzleFloat4<0xFD>      yww;
			SwizzleFloat4<0xFE>      zww;
			SwizzleFloat4<0xFF>      www;
			SwizzleFloat4<0x00>      xxxx;
			SwizzleFloat4<0x01>      yxxx;
			SwizzleFloat4<0x02>      zxxx;
			SwizzleFloat4<0x03>      wxxx;
			SwizzleFloat4<0x04>      xyxx;
			SwizzleFloat4<0x05>      yyxx;
			SwizzleFloat4<0x06>      zyxx;
			SwizzleFloat4<0x07>      wyxx;
			SwizzleFloat4<0x08>      xzxx;
			SwizzleFloat4<0x09>      yzxx;
			SwizzleFloat4<0x0A>      zzxx;
			SwizzleFloat4<0x0B>      wzxx;
			SwizzleFloat4<0x0C>      xwxx;
			SwizzleFloat4<0x0D>      ywxx;
			SwizzleFloat4<0x0E>      zwxx;
			SwizzleFloat4<0x0F>      wwxx;
			SwizzleFloat4<0x10>      xxyx;
			SwizzleFloat4<0x11>      yxyx;
			SwizzleFloat4<0x12>      zxyx;
			SwizzleFloat4<0x13>      wxyx;
			SwizzleFloat4<0x14>      xyyx;
			SwizzleFloat4<0x15>      yyyx;
			SwizzleFloat4<0x16>      zyyx;
			SwizzleFloat4<0x17>      wyyx;
			SwizzleFloat4<0x18>      xzyx;
			SwizzleFloat4<0x19>      yzyx;
			SwizzleFloat4<0x1A>      zzyx;
			SwizzleFloat4<0x1B>      wzyx;
			SwizzleFloat4<0x1C>      xwyx;
			SwizzleFloat4<0x1D>      ywyx;
			SwizzleFloat4<0x1E>      zwyx;
			SwizzleFloat4<0x1F>      wwyx;
			SwizzleFloat4<0x20>      xxzx;
			SwizzleFloat4<0x21>      yxzx;
			SwizzleFloat4<0x22>      zxzx;
			SwizzleFloat4<0x23>      wxzx;
			SwizzleFloat4<0x24>      xyzx;
			SwizzleFloat4<0x25>      yyzx;
			SwizzleFloat4<0x26>      zyzx;
			SwizzleFloat4<0x27>      wyzx;
			SwizzleFloat4<0x28>      xzzx;
			SwizzleFloat4<0x29>      yzzx;
			SwizzleFloat4<0x2A>      zzzx;
			SwizzleFloat4<0x2B>      wzzx;
			SwizzleFloat4<0x2C>      xwzx;
			SwizzleFloat4<0x2D>      ywzx;
			SwizzleFloat4<0x2E>      zwzx;
			SwizzleFloat4<0x2F>      wwzx;
			SwizzleFloat4<0x30>      xxwx;
			SwizzleFloat4<0x31>      yxwx;
			SwizzleFloat4<0x32>      zxwx;
			SwizzleFloat4<0x33>      wxwx;
			SwizzleFloat4<0x34>      xywx;
			SwizzleFloat4<0x35>      yywx;
			SwizzleFloat4<0x36>      zywx;
			SwizzleFloat4<0x37>      wywx;
			SwizzleFloat4<0x38>      xzwx;
			SwizzleFloat4<0x39>      yzwx;
			SwizzleFloat4<0x3A>      zzwx;
			SwizzleFloat4<0x3B>      wzwx;
			SwizzleFloat4<0x3C>      xwwx;
			SwizzleFloat4<0x3D>      ywwx;
			SwizzleFloat4<0x3E>      zwwx;
			SwizzleFloat4<0x3F>      wwwx;
			SwizzleFloat4<0x40>      xxxy;
			SwizzleFloat4<0x41>      yxxy;
			SwizzleFloat4<0x42>      zxxy;
			SwizzleFloat4<0x43>      wxxy;
			SwizzleFloat4<0x44>      xyxy;
			SwizzleFloat4<0x45>      yyxy;
			SwizzleFloat4<0x46>      zyxy;
			SwizzleFloat4<0x47>      wyxy;
			SwizzleFloat4<0x48>      xzxy;
			SwizzleFloat4<0x49>      yzxy;
			SwizzleFloat4<0x4A>      zzxy;
			SwizzleFloat4<0x4B>      wzxy;
			SwizzleFloat4<0x4C>      xwxy;
			SwizzleFloat4<0x4D>      ywxy;
			SwizzleFloat4<0x4E>      zwxy;
			SwizzleFloat4<0x4F>      wwxy;
			SwizzleFloat4<0x50>      xxyy;
			SwizzleFloat4<0x51>      yxyy;
			SwizzleFloat4<0x52>      zxyy;
			SwizzleFloat4<0x53>      wxyy;
			SwizzleFloat4<0x54>      xyyy;
			SwizzleFloat4<0x55>      yyyy;
			SwizzleFloat4<0x56>      zyyy;
			SwizzleFloat4<0x57>      wyyy;
			SwizzleFloat4<0x58>      xzyy;
			SwizzleFloat4<0x59>      yzyy;
			SwizzleFloat4<0x5A>      zzyy;
			SwizzleFloat4<0x5B>      wzyy;
			SwizzleFloat4<0x5C>      xwyy;
			SwizzleFloat4<0x5D>      ywyy;
			SwizzleFloat4<0x5E>      zwyy;
			SwizzleFloat4<0x5F>      wwyy;
			SwizzleFloat4<0x60>      xxzy;
			SwizzleFloat4<0x61>      yxzy;
			SwizzleFloat4<0x62>      zxzy;
			SwizzleFloat4<0x63>      wxzy;
			SwizzleFloat4<0x64>      xyzy;
			SwizzleFloat4<0x65>      yyzy;
			SwizzleFloat4<0x66>      zyzy;
			SwizzleFloat4<0x67>      wyzy;
			SwizzleFloat4<0x68>      xzzy;
			SwizzleFloat4<0x69>      yzzy;
			SwizzleFloat4<0x6A>      zzzy;
			SwizzleFloat4<0x6B>      wzzy;
			SwizzleFloat4<0x6C>      xwzy;
			SwizzleFloat4<0x6D>      ywzy;
			SwizzleFloat4<0x6E>      zwzy;
			SwizzleFloat4<0x6F>      wwzy;
			SwizzleFloat4<0x70>      xxwy;
			SwizzleFloat4<0x71>      yxwy;
			SwizzleFloat4<0x72>      zxwy;
			SwizzleFloat4<0x73>      wxwy;
			SwizzleFloat4<0x74>      xywy;
			SwizzleFloat4<0x75>      yywy;
			SwizzleFloat4<0x76>      zywy;
			SwizzleFloat4<0x77>      wywy;
			SwizzleFloat4<0x78>      xzwy;
			SwizzleFloat4<0x79>      yzwy;
			SwizzleFloat4<0x7A>      zzwy;
			SwizzleFloat4<0x7B>      wzwy;
			SwizzleFloat4<0x7C>      xwwy;
			SwizzleFloat4<0x7D>      ywwy;
			SwizzleFloat4<0x7E>      zwwy;
			SwizzleFloat4<0x7F>      wwwy;
			SwizzleFloat4<0x80>      xxxz;
			SwizzleFloat4<0x81>      yxxz;
			SwizzleFloat4<0x82>      zxxz;
			SwizzleFloat4<0x83>      wxxz;
			SwizzleFloat4<0x84>      xyxz;
			SwizzleFloat4<0x85>      yyxz;
			SwizzleFloat4<0x86>      zyxz;
			SwizzleFloat4<0x87>      wyxz;
			SwizzleFloat4<0x88>      xzxz;
			SwizzleFloat4<0x89>      yzxz;
			SwizzleFloat4<0x8A>      zzxz;
			SwizzleFloat4<0x8B>      wzxz;
			SwizzleFloat4<0x8C>      xwxz;
			SwizzleFloat4<0x8D>      ywxz;
			SwizzleFloat4<0x8E>      zwxz;
			SwizzleFloat4<0x8F>      wwxz;
			SwizzleFloat4<0x90>      xxyz;
			SwizzleFloat4<0x91>      yxyz;
			SwizzleFloat4<0x92>      zxyz;
			SwizzleFloat4<0x93>      wxyz;
			SwizzleFloat4<0x94>      xyyz;
			SwizzleFloat4<0x95>      yyyz;
			SwizzleFloat4<0x96>      zyyz;
			SwizzleFloat4<0x97>      wyyz;
			SwizzleFloat4<0x98>      xzyz;
			SwizzleFloat4<0x99>      yzyz;
			SwizzleFloat4<0x9A>      zzyz;
			SwizzleFloat4<0x9B>      wzyz;
			SwizzleFloat4<0x9C>      xwyz;
			SwizzleFloat4<0x9D>      ywyz;
			SwizzleFloat4<0x9E>      zwyz;
			SwizzleFloat4<0x9F>      wwyz;
			SwizzleFloat4<0xA0>      xxzz;
			SwizzleFloat4<0xA1>      yxzz;
			SwizzleFloat4<0xA2>      zxzz;
			SwizzleFloat4<0xA3>      wxzz;
			SwizzleFloat4<0xA4>      xyzz;
			SwizzleFloat4<0xA5>      yyzz;
			SwizzleFloat4<0xA6>      zyzz;
			SwizzleFloat4<0xA7>      wyzz;
			SwizzleFloat4<0xA8>      xzzz;
			SwizzleFloat4<0xA9>      yzzz;
			SwizzleFloat4<0xAA>      zzzz;
			SwizzleFloat4<0xAB>      wzzz;
			SwizzleFloat4<0xAC>      xwzz;
			SwizzleFloat4<0xAD>      ywzz;
			SwizzleFloat4<0xAE>      zwzz;
			SwizzleFloat4<0xAF>      wwzz;
			SwizzleFloat4<0xB0>      xxwz;
			SwizzleFloat4<0xB1>      yxwz;
			SwizzleFloat4<0xB2>      zxwz;
			SwizzleFloat4<0xB3>      wxwz;
			SwizzleFloat4<0xB4>      xywz;
			SwizzleFloat4<0xB5>      yywz;
			SwizzleFloat4<0xB6>      zywz;
			SwizzleFloat4<0xB7>      wywz;
			SwizzleFloat4<0xB8>      xzwz;
			SwizzleFloat4<0xB9>      yzwz;
			SwizzleFloat4<0xBA>      zzwz;
			SwizzleFloat4<0xBB>      wzwz;
			SwizzleFloat4<0xBC>      xwwz;
			SwizzleFloat4<0xBD>      ywwz;
			SwizzleFloat4<0xBE>      zwwz;
			SwizzleFloat4<0xBF>      wwwz;
			SwizzleFloat4<0xC0>      xxxw;
			SwizzleFloat4<0xC1>      yxxw;
			SwizzleFloat4<0xC2>      zxxw;
			SwizzleFloat4<0xC3>      wxxw;
			SwizzleFloat4<0xC4>      xyxw;
			SwizzleFloat4<0xC5>      yyxw;
			SwizzleFloat4<0xC6>      zyxw;
			SwizzleFloat4<0xC7>      wyxw;
			SwizzleFloat4<0xC8>      xzxw;
			SwizzleFloat4<0xC9>      yzxw;
			SwizzleFloat4<0xCA>      zzxw;
			SwizzleFloat4<0xCB>      wzxw;
			SwizzleFloat4<0xCC>      xwxw;
			SwizzleFloat4<0xCD>      ywxw;
			SwizzleFloat4<0xCE>      zwxw;
			SwizzleFloat4<0xCF>      wwxw;
			SwizzleFloat4<0xD0>      xxyw;
			SwizzleFloat4<0xD1>      yxyw;
			SwizzleFloat4<0xD2>      zxyw;
			SwizzleFloat4<0xD3>      wxyw;
			SwizzleFloat4<0xD4>      xyyw;
			SwizzleFloat4<0xD5>      yyyw;
			SwizzleFloat4<0xD6>      zyyw;
			SwizzleFloat4<0xD7>      wyyw;
			SwizzleFloat4<0xD8>      xzyw;
			SwizzleFloat4<0xD9>      yzyw;
			SwizzleFloat4<0xDA>      zzyw;
			SwizzleFloat4<0xDB>      wzyw;
			SwizzleFloat4<0xDC>      xwyw;
			SwizzleFloat4<0xDD>      ywyw;
			SwizzleFloat4<0xDE>      zwyw;
			SwizzleFloat4<0xDF>      wwyw;
			SwizzleFloat4<0xE0>      xxzw;
			SwizzleFloat4<0xE1>      yxzw;
			SwizzleFloat4<0xE2>      zxzw;
			SwizzleFloat4<0xE3>      wxzw;
			SwizzleMaskFloat4<0xE4>  xyzw;
			SwizzleFloat4<0xE5>      yyzw;
			SwizzleFloat4<0xE6>      zyzw;
			SwizzleFloat4<0xE7>      wyzw;
			SwizzleFloat4<0xE8>      xzzw;
			SwizzleFloat4<0xE9>      yzzw;
			SwizzleFloat4<0xEA>      zzzw;
			SwizzleFloat4<0xEB>      wzzw;
			SwizzleFloat4<0xEC>      xwzw;
			SwizzleFloat4<0xED>      ywzw;
			SwizzleFloat4<0xEE>      zwzw;
			SwizzleFloat4<0xEF>      wwzw;
			SwizzleFloat4<0xF0>      xxww;
			SwizzleFloat4<0xF1>      yxww;
			SwizzleFloat4<0xF2>      zxww;
			SwizzleFloat4<0xF3>      wxww;
			SwizzleFloat4<0xF4>      xyww;
			SwizzleFloat4<0xF5>      yyww;
			SwizzleFloat4<0xF6>      zyww;
			SwizzleFloat4<0xF7>      wyww;
			SwizzleFloat4<0xF8>      xzww;
			SwizzleFloat4<0xF9>      yzww;
			SwizzleFloat4<0xFA>      zzww;
			SwizzleFloat4<0xFB>      wzww;
			SwizzleFloat4<0xFC>      xwww;
			SwizzleFloat4<0xFD>      ywww;
			SwizzleFloat4<0xFE>      zwww;
			SwizzleFloat4<0xFF>      wwww;
		};

	private:
		void constant(float x, float y, float z, float w);
	};

	RValue<Float4> operator+(RValue<Float4> lhs, RValue<Float4> rhs);
	RValue<Float4> operator-(RValue<Float4> lhs, RValue<Float4> rhs);
	RValue<Float4> operator*(RValue<Float4> lhs, RValue<Float4> rhs);
	RValue<Float4> operator/(RValue<Float4> lhs, RValue<Float4> rhs);
	RValue<Float4> operator%(RValue<Float4> lhs, RValue<Float4> rhs);
	RValue<Float4> operator+=(const Float4 &lhs, RValue<Float4> rhs);
	RValue<Float4> operator-=(const Float4 &lhs, RValue<Float4> rhs);
	RValue<Float4> operator*=(const Float4 &lhs, RValue<Float4> rhs);
	RValue<Float4> operator/=(const Float4 &lhs, RValue<Float4> rhs);
	RValue<Float4> operator%=(const Float4 &lhs, RValue<Float4> rhs);
	RValue<Float4> operator+(RValue<Float4> val);
	RValue<Float4> operator-(RValue<Float4> val);

	RValue<Float4> Abs(RValue<Float4> x);
	RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y);
	RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y);
	RValue<Float4> Rcp_pp(RValue<Float4> val, bool exactAtPow2 = false);
	RValue<Float4> RcpSqrt_pp(RValue<Float4> val);
	RValue<Float4> Sqrt(RValue<Float4> x);
	RValue<Float4> Insert(const Float4 &val, RValue<Float> element, int i);
	RValue<Float> Extract(RValue<Float4> x, int i);
	RValue<Float4> Swizzle(RValue<Float4> x, unsigned char select);
	RValue<Float4> ShuffleLowHigh(RValue<Float4> x, RValue<Float4> y, unsigned char imm);
	RValue<Float4> UnpackLow(RValue<Float4> x, RValue<Float4> y);
	RValue<Float4> UnpackHigh(RValue<Float4> x, RValue<Float4> y);
	RValue<Float4> Mask(Float4 &lhs, RValue<Float4> rhs, unsigned char select);
	RValue<Int> SignMask(RValue<Float4> x);
	RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y);
	RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y);
	RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y);
	RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y);
	RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y);
	RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y);
	RValue<Float4> Round(RValue<Float4> x);
	RValue<Float4> Trunc(RValue<Float4> x);
	RValue<Float4> Frac(RValue<Float4> x);
	RValue<Float4> Floor(RValue<Float4> x);
	RValue<Float4> Ceil(RValue<Float4> x);

	template<class T>
	class Pointer : public Variable<Pointer<T>>
	{
	public:
		template<class S>
		Pointer(RValue<Pointer<S>> pointerS, int alignment = 1) : alignment(alignment)
		{
			Value *pointerT = Nucleus::createBitCast(pointerS.value, Nucleus::getPointerType(T::getType()));
			LValue<Pointer<T>>::storeValue(pointerT);
		}

		template<class S>
		Pointer(const Pointer<S> &pointer, int alignment = 1) : alignment(alignment)
		{
			Value *pointerS = pointer.loadValue(alignment);
			Value *pointerT = Nucleus::createBitCast(pointerS, Nucleus::getPointerType(T::getType()));
			LValue<Pointer<T>>::storeValue(pointerT);
		}

		Pointer(Argument<Pointer<T>> argument);
		explicit Pointer(const void *external);

		Pointer();
		Pointer(RValue<Pointer<T>> rhs);
		Pointer(const Pointer<T> &rhs);
		Pointer(const Reference<Pointer<T>> &rhs);

		RValue<Pointer<T>> operator=(RValue<Pointer<T>> rhs) const;
		RValue<Pointer<T>> operator=(const Pointer<T> &rhs) const;
		RValue<Pointer<T>> operator=(const Reference<Pointer<T>> &rhs) const;

		Reference<T> operator*();
		Reference<T> operator[](int index);
		Reference<T> operator[](RValue<Int> index);

		static Type *getType();

	private:
		const int alignment;
	};

	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, int offset);
	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<Int> offset);
	RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<UInt> offset);
	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, int offset);
	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, RValue<Int> offset);
	RValue<Pointer<Byte>> operator+=(const Pointer<Byte> &lhs, RValue<UInt> offset);

	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, int offset);
	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<Int> offset);
	RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<UInt> offset);
	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, int offset);
	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, RValue<Int> offset);
	RValue<Pointer<Byte>> operator-=(const Pointer<Byte> &lhs, RValue<UInt> offset);

	template<class T, int S = 1>
	class Array : public Variable<T>
	{
	public:
		Array(int size = S);

		Reference<T> operator[](int index);
		Reference<T> operator[](RValue<Int> index);
	};

//	RValue<Array<T>> operator++(const Array<T> &val, int);   // Post-increment
//	const Array<T> &operator++(const Array<T> &val);   // Pre-increment
//	RValue<Array<T>> operator--(const Array<T> &val, int);   // Post-decrement
//	const Array<T> &operator--(const Array<T> &val);   // Pre-decrement

	BasicBlock *beginLoop();
	bool branch(RValue<Bool> cmp, BasicBlock *bodyBB, BasicBlock *endBB);
	bool elseBlock(BasicBlock *falseBB);

	void Return();
	void Return(bool ret);
	void Return(const Int &ret);

	template<class T>
	void Return(const Pointer<T> &ret);

	template<class T>
	void Return(RValue<Pointer<T>> ret);

	template<unsigned int index, typename... Arguments>
	struct ArgI;

	template<typename Arg0, typename... Arguments>
	struct ArgI<0, Arg0, Arguments...>
	{
		typedef Arg0 Type;
	};

	template<unsigned int index, typename Arg0, typename... Arguments>
	struct ArgI<index, Arg0, Arguments...>
	{
		typedef typename ArgI<index - 1, Arguments...>::Type Type;
	};

	// Generic template, leave undefined!
	template<typename FunctionType>
	class Function;

	// Specialized for function types
	template<typename Return, typename... Arguments>
	class Function<Return(Arguments...)>
	{
	public:
		Function();

		virtual ~Function();

		template<int index>
		Argument<typename ArgI<index, Arguments...>::Type> Arg() const
		{
			Value *arg = Nucleus::getArgument(index);
			return Argument<typename ArgI<index, Arguments...>::Type>(arg);
		}

		Routine *operator()(const wchar_t *name, ...);

	protected:
		Nucleus *core;
		std::vector<Type*> arguments;
	};

	template<typename Return>
	class Function<Return()> : public Function<Return(Void)>
	{
	};

	template<int index, typename Return, typename... Arguments>
	Argument<typename ArgI<index, Arguments...>::Type> Arg(Function<Return(Arguments...)> &function)
	{
		return Argument<typename ArgI<index, Arguments...>::Type>(function.arg(index));
	}

	RValue<Long> Ticks();
}

namespace sw
{
	template<class T>
	LValue<T>::LValue(int arraySize)
	{
		address = Nucleus::allocateStackVariable(T::getType(), arraySize);
	}

	template<class T>
	Value *LValue<T>::loadValue(unsigned int alignment) const
	{
		return Nucleus::createLoad(address, T::getType(), false, alignment);
	}

	template<class T>
	Value *LValue<T>::storeValue(Value *value, unsigned int alignment) const
	{
		return Nucleus::createStore(value, address, T::getType(), false, alignment);
	}

	template<class T>
	Constant *LValue<T>::storeValue(Constant *constant, unsigned int alignment) const
	{
		return Nucleus::createStore(constant, address, T::getType(), false, alignment);
	}

	template<class T>
	Value *LValue<T>::getAddress(Value *index) const
	{
		return Nucleus::createGEP(address, T::getType(), index);
	}

	template<class T>
	Variable<T>::Variable(int arraySize) : LValue<T>(arraySize)
	{
	}

	template<class T>
	RValue<Pointer<T>> Variable<T>::operator&()
	{
		return RValue<Pointer<T>>(LValue<T>::address);
	}

	template<class T>
	Reference<T>::Reference(Value *pointer, int alignment) : alignment(alignment)
	{
		address = pointer;
	}

	template<class T>
	RValue<T> Reference<T>::operator=(RValue<T> rhs) const
	{
		Nucleus::createStore(rhs.value, address, T::getType(), false, alignment);

		return rhs;
	}

	template<class T>
	RValue<T> Reference<T>::operator=(const Reference<T> &ref) const
	{
		Value *tmp = Nucleus::createLoad(ref.address, T::getType(), false, ref.alignment);
		Nucleus::createStore(tmp, address, T::getType(), false, alignment);

		return RValue<T>(tmp);
	}

	template<class T>
	RValue<T> Reference<T>::operator+=(RValue<T> rhs) const
	{
		return *this = *this + rhs;
	}

	template<class T>
	Value *Reference<T>::loadValue() const
	{
		return Nucleus::createLoad(address, T::getType(), false, alignment);
	}

	template<class T>
	int Reference<T>::getAlignment() const
	{
		return alignment;
	}

	template<class T>
	RValue<T>::RValue(Value *rvalue)
	{
		value = rvalue;
	}

	template<class T>
	RValue<T>::RValue(Constant *constant)
	{
		value = Nucleus::createAssign(constant);
	}

	template<class T>
	RValue<T>::RValue(const T &lvalue)
	{
		value = lvalue.loadValue();
	}

	template<class T>
	RValue<T>::RValue(typename IntLiteral<T>::type i)
	{
		value = (Value*)Nucleus::createConstantInt(i);
	}

	template<class T>
	RValue<T>::RValue(typename FloatLiteral<T>::type f)
	{
		value = (Value*)Nucleus::createConstantFloat(f);
	}

	template<class T>
	RValue<T>::RValue(const Reference<T> &ref)
	{
		value = ref.loadValue();
	}

	template<int T>
	Swizzle2Float4<T>::operator RValue<Float4>() const
	{
		Value *vector = parent->loadValue();

		return Swizzle(RValue<Float4>(vector), T);
	}

	template<int T>
	SwizzleFloat4<T>::operator RValue<Float4>() const
	{
		Value *vector = parent->loadValue();

		return Swizzle(RValue<Float4>(vector), T);
	}

	template<int T>
	SwizzleMaskFloat4<T>::operator RValue<Float4>() const
	{
		Value *vector = parent->loadValue();

		return Swizzle(RValue<Float4>(vector), T);
	}

	template<int T>
	RValue<Float4> SwizzleMaskFloat4<T>::operator=(RValue<Float4> rhs) const
	{
		return Mask(*parent, rhs, T);
	}

	template<int T>
	RValue<Float4> SwizzleMaskFloat4<T>::operator=(RValue<Float> rhs) const
	{
		return Mask(*parent, Float4(rhs), T);
	}

	template<int T>
	SwizzleMask1Float4<T>::operator RValue<Float>() const   // FIXME: Call a non-template function
	{
		return Extract(*parent, T & 0x3);
	}

	template<int T>
	SwizzleMask1Float4<T>::operator RValue<Float4>() const
	{
		Value *vector = parent->loadValue();

		return Swizzle(RValue<Float4>(vector), T);
	}

	template<int T>
	RValue<Float4> SwizzleMask1Float4<T>::operator=(float x) const
	{
		return Insert(*parent, Float(x), T & 0x3);
	}

	template<int T>
	RValue<Float4> SwizzleMask1Float4<T>::operator=(RValue<Float4> rhs) const
	{
		return Mask(*parent, Float4(rhs), T);
	}

	template<int T>
	RValue<Float4> SwizzleMask1Float4<T>::operator=(RValue<Float> rhs) const   // FIXME: Call a non-template function
	{
		return Insert(*parent, rhs, T & 0x3);
	}

	template<int T>
	SwizzleMask2Float4<T>::operator RValue<Float4>() const
	{
		Value *vector = parent->loadValue();

		return Swizzle(RValue<Float4>(vector), T);
	}

	template<int T>
	RValue<Float4> SwizzleMask2Float4<T>::operator=(RValue<Float4> rhs) const
	{
		return Mask(*parent, Float4(rhs), T);
	}

	template<int T>
	Float::Float(const SwizzleMask1Float4<T> &rhs)
	{
		*this = rhs.operator RValue<Float>();
	}

	template<int T>
	RValue<Float> Float::operator=(const SwizzleMask1Float4<T> &rhs) const
	{
		return *this = rhs.operator RValue<Float>();
	}

	template<int T>
	Float4::Float4(const SwizzleMask1Float4<T> &rhs)
	{
		xyzw.parent = this;

		*this = rhs.operator RValue<Float4>();
	}

	template<int T>
	Float4::Float4(const SwizzleFloat4<T> &rhs)
	{
		xyzw.parent = this;

		*this = rhs.operator RValue<Float4>();
	}

	template<int X, int Y>
	Float4::Float4(const Swizzle2Float4<X> &x, const Swizzle2Float4<Y> &y)
	{
		xyzw.parent = this;

		*this = ShuffleLowHigh(*x.parent, *y.parent, (X & 0xF) | (Y & 0xF) << 4);
	}

	template<int X, int Y>
	Float4::Float4(const SwizzleMask2Float4<X> &x, const Swizzle2Float4<Y> &y)
	{
		xyzw.parent = this;

		*this = ShuffleLowHigh(*x.parent, *y.parent, (X & 0xF) | (Y & 0xF) << 4);
	}

	template<int X, int Y>
	Float4::Float4(const Swizzle2Float4<X> &x, const SwizzleMask2Float4<Y> &y)
	{
		xyzw.parent = this;

		*this = ShuffleLowHigh(*x.parent, *y.parent, (X & 0xF) | (Y & 0xF) << 4);
	}

	template<int X, int Y>
	Float4::Float4(const SwizzleMask2Float4<X> &x, const SwizzleMask2Float4<Y> &y)
	{
		xyzw.parent = this;

		*this = ShuffleLowHigh(*x.parent, *y.parent, (X & 0xF) | (Y & 0xF) << 4);
	}

	template<int T>
	RValue<Float4> Float4::operator=(const SwizzleMask1Float4<T> &rhs)
	{
		return *this = rhs.operator RValue<Float4>();
	}

	template<int T>
	RValue<Float4> Float4::operator=(const SwizzleFloat4<T> &rhs)
	{
		return *this = rhs.operator RValue<Float4>();
	}

	template<class T>
	Pointer<T>::Pointer(Argument<Pointer<T>> argument) : alignment(1)
	{
		LValue<Pointer<T>>::storeValue(argument.value);
	}

	template<class T>
	Pointer<T>::Pointer(const void *external) : alignment((intptr_t)external & 0x0000000F ? 1 : 16)
	{
		Constant *globalPointer = Nucleus::createConstantPointer(external, T::getType(), false, alignment);

		LValue<Pointer<T>>::storeValue(globalPointer);
	}

	template<class T>
	Pointer<T>::Pointer() : alignment(1)
	{
		LValue<Pointer<T>>::storeValue(Nucleus::createNullPointer(T::getType()));
	}

	template<class T>
	Pointer<T>::Pointer(RValue<Pointer<T>> rhs) : alignment(1)
	{
		LValue<Pointer<T>>::storeValue(rhs.value);
	}

	template<class T>
	Pointer<T>::Pointer(const Pointer<T> &rhs) : alignment(rhs.alignment)
	{
		Value *value = rhs.loadValue();
		LValue<Pointer<T>>::storeValue(value);
	}

	template<class T>
	Pointer<T>::Pointer(const Reference<Pointer<T>> &rhs) : alignment(rhs.getAlignment())
	{
		Value *value = rhs.loadValue();
		LValue<Pointer<T>>::storeValue(value);
	}

	template<class T>
	RValue<Pointer<T>> Pointer<T>::operator=(RValue<Pointer<T>> rhs) const
	{
		LValue<Pointer<T>>::storeValue(rhs.value);

		return rhs;
	}

	template<class T>
	RValue<Pointer<T>> Pointer<T>::operator=(const Pointer<T> &rhs) const
	{
		Value *value = rhs.loadValue();
		LValue<Pointer<T>>::storeValue(value);

		return RValue<Pointer<T>>(value);
	}

	template<class T>
	RValue<Pointer<T>> Pointer<T>::operator=(const Reference<Pointer<T>> &rhs) const
	{
		Value *value = rhs.loadValue();
		LValue<Pointer<T>>::storeValue(value);

		return RValue<Pointer<T>>(value);
	}

	template<class T>
	Reference<T> Pointer<T>::operator*()
	{
		return Reference<T>(LValue<Pointer<T>>::loadValue(), alignment);
	}

	template<class T>
	Reference<T> Pointer<T>::operator[](int index)
	{
		Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), (Value*)Nucleus::createConstantInt(index));

		return Reference<T>(element, alignment);
	}

	template<class T>
	Reference<T> Pointer<T>::operator[](RValue<Int> index)
	{
		Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), index.value);

		return Reference<T>(element, alignment);
	}

	template<class T>
	Type *Pointer<T>::getType()
	{
		return Nucleus::getPointerType(T::getType());
	}

	template<class T, int S>
	Array<T, S>::Array(int size) : Variable<T>(size)
	{
	}

	template<class T, int S>
	Reference<T> Array<T, S>::operator[](int index)
	{
		Value *element = LValue<T>::getAddress((Value*)Nucleus::createConstantInt(index));

		return Reference<T>(element);
	}

	template<class T, int S>
	Reference<T> Array<T, S>::operator[](RValue<Int> index)
	{
		Value *element = LValue<T>::getAddress(index.value);

		return Reference<T>(element);
	}

//	template<class T>
//	RValue<Array<T>> operator++(const Array<T> &val, int)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	const Array<T> &operator++(const Array<T> &val)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	RValue<Array<T>> operator--(const Array<T> &val, int)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	const Array<T> &operator--(const Array<T> &val)
//	{
//		// FIXME: Requires storing the address of the array
//	}

	template<class T>
	RValue<T> IfThenElse(RValue<Bool> condition, RValue<T> ifTrue, RValue<T> ifFalse)
	{
		return RValue<T>(Nucleus::createSelect(condition.value, ifTrue.value, ifFalse.value));
	}

	template<class T>
	RValue<T> IfThenElse(RValue<Bool> condition, const T &ifTrue, RValue<T> ifFalse)
	{
		Value *trueValue = ifTrue.loadValue();

		return RValue<T>(Nucleus::createSelect(condition.value, trueValue, ifFalse.value));
	}

	template<class T>
	RValue<T> IfThenElse(RValue<Bool> condition, RValue<T> ifTrue, const T &ifFalse)
	{
		Value *falseValue = ifFalse.loadValue();

		return RValue<T>(Nucleus::createSelect(condition.value, ifTrue.value, falseValue));
	}

	template<class T>
	RValue<T> IfThenElse(RValue<Bool> condition, const T &ifTrue, const T &ifFalse)
	{
		Value *trueValue = ifTrue.loadValue();
		Value *falseValue = ifFalse.loadValue();

		return RValue<T>(Nucleus::createSelect(condition.value, trueValue, falseValue));
	}

	template<class T>
	void Return(const Pointer<T> &ret)
	{
		Nucleus::createRet(Nucleus::createLoad(ret.address, Pointer<T>::getType()));
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
	}

	template<class T>
	void Return(RValue<Pointer<T>> ret)
	{
		Nucleus::createRet(ret.value);
		Nucleus::setInsertBlock(Nucleus::createBasicBlock());
	}

	template<typename Return, typename... Arguments>
	Function<Return(Arguments...)>::Function()
	{
		core = new Nucleus();

		Type *types[] = {Arguments::getType()...};
		for(Type *type : types)
		{
			if(type != Void::getType())
			{
				arguments.push_back(type);
			}
		}

		Nucleus::createFunction(Return::getType(), arguments);
	}

	template<typename Return, typename... Arguments>
	Function<Return(Arguments...)>::~Function()
	{
		delete core;
	}

	template<typename Return, typename... Arguments>
	Routine *Function<Return(Arguments...)>::operator()(const wchar_t *name, ...)
	{
		wchar_t fullName[1024 + 1];

		va_list vararg;
		va_start(vararg, name);
		vswprintf(fullName, 1024, name, vararg);
		va_end(vararg);

		return core->acquireRoutine(fullName, true);
	}

	template<class T, class S>
	RValue<T> ReinterpretCast(RValue<S> val)
	{
		return RValue<T>(Nucleus::createBitCast(val.value, T::getType()));
	}

	template<class T, class S>
	RValue<T> ReinterpretCast(const LValue<S> &var)
	{
		Value *val = var.loadValue();

		return RValue<T>(Nucleus::createBitCast(val, T::getType()));
	}

	template<class T, class S>
	RValue<T> ReinterpretCast(const Reference<S> &var)
	{
		return ReinterpretCast<T>(RValue<S>(var));
	}

	template<class T, class S>
	RValue<T> As(RValue<S> val)
	{
		return ReinterpretCast<T>(val);
	}

	template<class T, class S>
	RValue<T> As(const LValue<S> &var)
	{
		return ReinterpretCast<T>(var);
	}

	template<class T, class S>
	RValue<T> As(const Reference<S> &val)
	{
		return ReinterpretCast<T>(val);
	}

	#define For(init, cond, inc)                     \
	init;                                            \
	for(BasicBlock *loopBB__ = beginLoop(),          \
		*bodyBB__ = Nucleus::createBasicBlock(),     \
		*endBB__ = Nucleus::createBasicBlock(),      \
		*onceBB__ = endBB__;                         \
		onceBB__ && branch(cond, bodyBB__, endBB__); \
		inc, onceBB__ = 0, Nucleus::createBr(loopBB__), Nucleus::setInsertBlock(endBB__))

	#define While(cond) For(((void*)0), cond, ((void*)0))

	#define Do                                          \
	{                                                   \
		BasicBlock *body = Nucleus::createBasicBlock(); \
		Nucleus::createBr(body);                        \
		Nucleus::setInsertBlock(body);

	#define Until(cond)                                 \
		BasicBlock *end = Nucleus::createBasicBlock();  \
		Nucleus::createCondBr((cond).value, end, body); \
		Nucleus::setInsertBlock(end);                   \
	}

	#define If(cond)                                        \
	for(BasicBlock *trueBB__ = Nucleus::createBasicBlock(), \
		*falseBB__ = Nucleus::createBasicBlock(),           \
		*endBB__ = Nucleus::createBasicBlock(),             \
		*onceBB__ = endBB__;                                \
		onceBB__ && branch(cond, trueBB__, falseBB__);      \
		onceBB__ = 0, Nucleus::createBr(endBB__), Nucleus::setInsertBlock(falseBB__), Nucleus::createBr(endBB__), Nucleus::setInsertBlock(endBB__))

	#define Else                                         \
	for(BasicBlock *endBB__ = Nucleus::getInsertBlock(), \
		*falseBB__ = Nucleus::getPredecessor(endBB__),   \
		*onceBB__ = endBB__;                             \
		onceBB__ && elseBlock(falseBB__);                \
		onceBB__ = 0, Nucleus::createBr(endBB__), Nucleus::setInsertBlock(endBB__))
}

#endif   // sw_Reactor_hpp
