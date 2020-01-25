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

#ifndef rr_Reactor_hpp
#define rr_Reactor_hpp

#include "Nucleus.hpp"
#include "Routine.hpp"
#include "Traits.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <tuple>
#include <unordered_set>

#undef Bool  // b/127920555

#ifdef ENABLE_RR_DEBUG_INFO
// Functions used for generating JIT debug info.
// See docs/ReactorDebugInfo.md for more information.
namespace rr {
// Update the current source location for debug.
void EmitDebugLocation();
// Bind value to its symbolic name taken from the backtrace.
void EmitDebugVariable(class Value *value);
// Flush any pending variable bindings before the line ends.
void FlushDebug();
}  // namespace rr
#	define RR_DEBUG_INFO_UPDATE_LOC() rr::EmitDebugLocation()
#	define RR_DEBUG_INFO_EMIT_VAR(value) rr::EmitDebugVariable(value)
#	define RR_DEBUG_INFO_FLUSH() rr::FlushDebug()
#else
#	define RR_DEBUG_INFO_UPDATE_LOC()
#	define RR_DEBUG_INFO_EMIT_VAR(value)
#	define RR_DEBUG_INFO_FLUSH()
#endif  // ENABLE_RR_DEBUG_INFO

namespace rr {

std::string BackendName();

struct Capabilities
{
	bool CoroutinesSupported;  // Support for rr::Coroutine<F>
};
extern const Capabilities Caps;

class Bool;
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
class Short2;
class UShort2;
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
class Half;
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

class Variable
{
	friend class Nucleus;
	friend class PrintValue;

	Variable() = delete;
	Variable &operator=(const Variable &) = delete;

public:
	void materialize() const;

	Value *loadValue() const;
	Value *storeValue(Value *value) const;

	Value *getBaseAddress() const;
	Value *getElementPointer(Value *index, bool unsignedIndex) const;

protected:
	Variable(Type *type, int arraySize);
	Variable(const Variable &) = default;

	~Variable();

	const int arraySize;

private:
	static void materializeAll();
	static void killUnmaterialized();

	static std::unordered_set<Variable *> unmaterializedVariables;

	Type *const type;
	mutable Value *rvalue = nullptr;
	mutable Value *address = nullptr;
};

template<class T>
class LValue : public Variable
{
public:
	LValue(int arraySize = 0);

	RValue<Pointer<T>> operator&();

	static bool isVoid()
	{
		return false;
	}

	// self() returns the this pointer to this LValue<T> object.
	// This function exists because operator&() is overloaded.
	inline LValue<T> *self() { return this; }
};

template<class T>
class Reference
{
public:
	using reference_underlying_type = T;

	explicit Reference(Value *pointer, int alignment = 1);

	RValue<T> operator=(RValue<T> rhs) const;
	RValue<T> operator=(const Reference<T> &ref) const;

	RValue<T> operator+=(RValue<T> rhs) const;

	RValue<Pointer<T>> operator&() const { return RValue<Pointer<T>>(address); }

	Value *loadValue() const;
	int getAlignment() const;

private:
	Value *address;

	const int alignment;
};

template<class T>
struct BoolLiteral
{
	struct type;
};

template<>
struct BoolLiteral<Bool>
{
	typedef bool type;
};

template<class T>
struct IntLiteral
{
	struct type;
};

template<>
struct IntLiteral<Int>
{
	typedef int type;
};

template<>
struct IntLiteral<UInt>
{
	typedef unsigned int type;
};

template<>
struct IntLiteral<Long>
{
	typedef int64_t type;
};

template<class T>
struct FloatLiteral
{
	struct type;
};

template<>
struct FloatLiteral<Float>
{
	typedef float type;
};

template<class T>
class RValue
{
public:
	using rvalue_underlying_type = T;

	explicit RValue(Value *rvalue);

#ifdef ENABLE_RR_DEBUG_INFO
	RValue(const RValue<T> &rvalue);
#endif  // ENABLE_RR_DEBUG_INFO

	RValue(const T &lvalue);
	RValue(typename BoolLiteral<T>::type i);
	RValue(typename IntLiteral<T>::type i);
	RValue(typename FloatLiteral<T>::type f);
	RValue(const Reference<T> &rhs);

	RValue<T> &operator=(const RValue<T> &) = delete;

	Value *value;  // FIXME: Make private
};

template<typename T>
struct Argument
{
	explicit Argument(Value *value)
	    : value(value)
	{}

	Value *value;
};

class Bool : public LValue<Bool>
{
public:
	Bool(Argument<Bool> argument);

	Bool() = default;
	Bool(bool x);
	Bool(RValue<Bool> rhs);
	Bool(const Bool &rhs);
	Bool(const Reference<Bool> &rhs);

	//	RValue<Bool> operator=(bool rhs);   // FIXME: Implement
	RValue<Bool> operator=(RValue<Bool> rhs);
	RValue<Bool> operator=(const Bool &rhs);
	RValue<Bool> operator=(const Reference<Bool> &rhs);

	static Type *getType();
};

RValue<Bool> operator!(RValue<Bool> val);
RValue<Bool> operator&&(RValue<Bool> lhs, RValue<Bool> rhs);
RValue<Bool> operator||(RValue<Bool> lhs, RValue<Bool> rhs);
RValue<Bool> operator!=(RValue<Bool> lhs, RValue<Bool> rhs);
RValue<Bool> operator==(RValue<Bool> lhs, RValue<Bool> rhs);

class Byte : public LValue<Byte>
{
public:
	Byte(Argument<Byte> argument);

	explicit Byte(RValue<Int> cast);
	explicit Byte(RValue<UInt> cast);
	explicit Byte(RValue<UShort> cast);

	Byte() = default;
	Byte(int x);
	Byte(unsigned char x);
	Byte(RValue<Byte> rhs);
	Byte(const Byte &rhs);
	Byte(const Reference<Byte> &rhs);

	//	RValue<Byte> operator=(unsigned char rhs);   // FIXME: Implement
	RValue<Byte> operator=(RValue<Byte> rhs);
	RValue<Byte> operator=(const Byte &rhs);
	RValue<Byte> operator=(const Reference<Byte> &rhs);

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
RValue<Byte> operator+=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator-=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator*=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator/=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator%=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator&=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator|=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator^=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator<<=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator>>=(Byte &lhs, RValue<Byte> rhs);
RValue<Byte> operator+(RValue<Byte> val);
RValue<Byte> operator-(RValue<Byte> val);
RValue<Byte> operator~(RValue<Byte> val);
RValue<Byte> operator++(Byte &val, int);  // Post-increment
const Byte &operator++(Byte &val);        // Pre-increment
RValue<Byte> operator--(Byte &val, int);  // Post-decrement
const Byte &operator--(Byte &val);        // Pre-decrement
RValue<Bool> operator<(RValue<Byte> lhs, RValue<Byte> rhs);
RValue<Bool> operator<=(RValue<Byte> lhs, RValue<Byte> rhs);
RValue<Bool> operator>(RValue<Byte> lhs, RValue<Byte> rhs);
RValue<Bool> operator>=(RValue<Byte> lhs, RValue<Byte> rhs);
RValue<Bool> operator!=(RValue<Byte> lhs, RValue<Byte> rhs);
RValue<Bool> operator==(RValue<Byte> lhs, RValue<Byte> rhs);

class SByte : public LValue<SByte>
{
public:
	SByte(Argument<SByte> argument);

	explicit SByte(RValue<Int> cast);
	explicit SByte(RValue<Short> cast);

	SByte() = default;
	SByte(signed char x);
	SByte(RValue<SByte> rhs);
	SByte(const SByte &rhs);
	SByte(const Reference<SByte> &rhs);

	//	RValue<SByte> operator=(signed char rhs);   // FIXME: Implement
	RValue<SByte> operator=(RValue<SByte> rhs);
	RValue<SByte> operator=(const SByte &rhs);
	RValue<SByte> operator=(const Reference<SByte> &rhs);

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
RValue<SByte> operator+=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator-=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator*=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator/=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator%=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator&=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator|=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator^=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator<<=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator>>=(SByte &lhs, RValue<SByte> rhs);
RValue<SByte> operator+(RValue<SByte> val);
RValue<SByte> operator-(RValue<SByte> val);
RValue<SByte> operator~(RValue<SByte> val);
RValue<SByte> operator++(SByte &val, int);  // Post-increment
const SByte &operator++(SByte &val);        // Pre-increment
RValue<SByte> operator--(SByte &val, int);  // Post-decrement
const SByte &operator--(SByte &val);        // Pre-decrement
RValue<Bool> operator<(RValue<SByte> lhs, RValue<SByte> rhs);
RValue<Bool> operator<=(RValue<SByte> lhs, RValue<SByte> rhs);
RValue<Bool> operator>(RValue<SByte> lhs, RValue<SByte> rhs);
RValue<Bool> operator>=(RValue<SByte> lhs, RValue<SByte> rhs);
RValue<Bool> operator!=(RValue<SByte> lhs, RValue<SByte> rhs);
RValue<Bool> operator==(RValue<SByte> lhs, RValue<SByte> rhs);

class Short : public LValue<Short>
{
public:
	Short(Argument<Short> argument);

	explicit Short(RValue<Int> cast);

	Short() = default;
	Short(short x);
	Short(RValue<Short> rhs);
	Short(const Short &rhs);
	Short(const Reference<Short> &rhs);

	//	RValue<Short> operator=(short rhs);   // FIXME: Implement
	RValue<Short> operator=(RValue<Short> rhs);
	RValue<Short> operator=(const Short &rhs);
	RValue<Short> operator=(const Reference<Short> &rhs);

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
RValue<Short> operator+=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator-=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator*=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator/=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator%=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator&=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator|=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator^=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator<<=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator>>=(Short &lhs, RValue<Short> rhs);
RValue<Short> operator+(RValue<Short> val);
RValue<Short> operator-(RValue<Short> val);
RValue<Short> operator~(RValue<Short> val);
RValue<Short> operator++(Short &val, int);  // Post-increment
const Short &operator++(Short &val);        // Pre-increment
RValue<Short> operator--(Short &val, int);  // Post-decrement
const Short &operator--(Short &val);        // Pre-decrement
RValue<Bool> operator<(RValue<Short> lhs, RValue<Short> rhs);
RValue<Bool> operator<=(RValue<Short> lhs, RValue<Short> rhs);
RValue<Bool> operator>(RValue<Short> lhs, RValue<Short> rhs);
RValue<Bool> operator>=(RValue<Short> lhs, RValue<Short> rhs);
RValue<Bool> operator!=(RValue<Short> lhs, RValue<Short> rhs);
RValue<Bool> operator==(RValue<Short> lhs, RValue<Short> rhs);

class UShort : public LValue<UShort>
{
public:
	UShort(Argument<UShort> argument);

	explicit UShort(RValue<UInt> cast);
	explicit UShort(RValue<Int> cast);

	UShort() = default;
	UShort(unsigned short x);
	UShort(RValue<UShort> rhs);
	UShort(const UShort &rhs);
	UShort(const Reference<UShort> &rhs);

	//	RValue<UShort> operator=(unsigned short rhs);   // FIXME: Implement
	RValue<UShort> operator=(RValue<UShort> rhs);
	RValue<UShort> operator=(const UShort &rhs);
	RValue<UShort> operator=(const Reference<UShort> &rhs);

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
RValue<UShort> operator+=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator-=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator*=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator/=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator%=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator&=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator|=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator^=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator<<=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator>>=(UShort &lhs, RValue<UShort> rhs);
RValue<UShort> operator+(RValue<UShort> val);
RValue<UShort> operator-(RValue<UShort> val);
RValue<UShort> operator~(RValue<UShort> val);
RValue<UShort> operator++(UShort &val, int);  // Post-increment
const UShort &operator++(UShort &val);        // Pre-increment
RValue<UShort> operator--(UShort &val, int);  // Post-decrement
const UShort &operator--(UShort &val);        // Pre-decrement
RValue<Bool> operator<(RValue<UShort> lhs, RValue<UShort> rhs);
RValue<Bool> operator<=(RValue<UShort> lhs, RValue<UShort> rhs);
RValue<Bool> operator>(RValue<UShort> lhs, RValue<UShort> rhs);
RValue<Bool> operator>=(RValue<UShort> lhs, RValue<UShort> rhs);
RValue<Bool> operator!=(RValue<UShort> lhs, RValue<UShort> rhs);
RValue<Bool> operator==(RValue<UShort> lhs, RValue<UShort> rhs);

class Byte4 : public LValue<Byte4>
{
public:
	explicit Byte4(RValue<Byte8> cast);
	explicit Byte4(RValue<UShort4> cast);
	explicit Byte4(RValue<Short4> cast);
	explicit Byte4(RValue<UInt4> cast);
	explicit Byte4(RValue<Int4> cast);

	Byte4() = default;
	//	Byte4(int x, int y, int z, int w);
	Byte4(RValue<Byte4> rhs);
	Byte4(const Byte4 &rhs);
	Byte4(const Reference<Byte4> &rhs);

	RValue<Byte4> operator=(RValue<Byte4> rhs);
	RValue<Byte4> operator=(const Byte4 &rhs);
	//	RValue<Byte4> operator=(const Reference<Byte4> &rhs);

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
//	RValue<Byte4> operator+=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator-=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator*=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator/=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator%=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator&=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator|=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator^=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator<<=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator>>=(Byte4 &lhs, RValue<Byte4> rhs);
//	RValue<Byte4> operator+(RValue<Byte4> val);
//	RValue<Byte4> operator-(RValue<Byte4> val);
//	RValue<Byte4> operator~(RValue<Byte4> val);
//	RValue<Byte4> operator++(Byte4 &val, int);   // Post-increment
//	const Byte4 &operator++(Byte4 &val);   // Pre-increment
//	RValue<Byte4> operator--(Byte4 &val, int);   // Post-decrement
//	const Byte4 &operator--(Byte4 &val);   // Pre-decrement

class SByte4 : public LValue<SByte4>
{
public:
	SByte4() = default;
	//	SByte4(int x, int y, int z, int w);
	//	SByte4(RValue<SByte4> rhs);
	//	SByte4(const SByte4 &rhs);
	//	SByte4(const Reference<SByte4> &rhs);

	//	RValue<SByte4> operator=(RValue<SByte4> rhs);
	//	RValue<SByte4> operator=(const SByte4 &rhs);
	//	RValue<SByte4> operator=(const Reference<SByte4> &rhs);

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
//	RValue<SByte4> operator+=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator-=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator*=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator/=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator%=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator&=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator|=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator^=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator<<=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator>>=(SByte4 &lhs, RValue<SByte4> rhs);
//	RValue<SByte4> operator+(RValue<SByte4> val);
//	RValue<SByte4> operator-(RValue<SByte4> val);
//	RValue<SByte4> operator~(RValue<SByte4> val);
//	RValue<SByte4> operator++(SByte4 &val, int);   // Post-increment
//	const SByte4 &operator++(SByte4 &val);   // Pre-increment
//	RValue<SByte4> operator--(SByte4 &val, int);   // Post-decrement
//	const SByte4 &operator--(SByte4 &val);   // Pre-decrement

class Byte8 : public LValue<Byte8>
{
public:
	Byte8() = default;
	Byte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7);
	Byte8(RValue<Byte8> rhs);
	Byte8(const Byte8 &rhs);
	Byte8(const Reference<Byte8> &rhs);

	RValue<Byte8> operator=(RValue<Byte8> rhs);
	RValue<Byte8> operator=(const Byte8 &rhs);
	RValue<Byte8> operator=(const Reference<Byte8> &rhs);

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
RValue<Byte8> operator+=(Byte8 &lhs, RValue<Byte8> rhs);
RValue<Byte8> operator-=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator*=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator/=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator%=(Byte8 &lhs, RValue<Byte8> rhs);
RValue<Byte8> operator&=(Byte8 &lhs, RValue<Byte8> rhs);
RValue<Byte8> operator|=(Byte8 &lhs, RValue<Byte8> rhs);
RValue<Byte8> operator^=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator<<=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator>>=(Byte8 &lhs, RValue<Byte8> rhs);
//	RValue<Byte8> operator+(RValue<Byte8> val);
//	RValue<Byte8> operator-(RValue<Byte8> val);
RValue<Byte8> operator~(RValue<Byte8> val);
//	RValue<Byte8> operator++(Byte8 &val, int);   // Post-increment
//	const Byte8 &operator++(Byte8 &val);   // Pre-increment
//	RValue<Byte8> operator--(Byte8 &val, int);   // Post-decrement
//	const Byte8 &operator--(Byte8 &val);   // Pre-decrement

RValue<Byte8> AddSat(RValue<Byte8> x, RValue<Byte8> y);
RValue<Byte8> SubSat(RValue<Byte8> x, RValue<Byte8> y);
RValue<Short4> Unpack(RValue<Byte4> x);
RValue<Short4> Unpack(RValue<Byte4> x, RValue<Byte4> y);
RValue<Short4> UnpackLow(RValue<Byte8> x, RValue<Byte8> y);
RValue<Short4> UnpackHigh(RValue<Byte8> x, RValue<Byte8> y);
RValue<Int> SignMask(RValue<Byte8> x);
//	RValue<Byte8> CmpGT(RValue<Byte8> x, RValue<Byte8> y);
RValue<Byte8> CmpEQ(RValue<Byte8> x, RValue<Byte8> y);
RValue<Byte8> Swizzle(RValue<Byte8> x, uint32_t select);

class SByte8 : public LValue<SByte8>
{
public:
	SByte8() = default;
	SByte8(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7);
	SByte8(RValue<SByte8> rhs);
	SByte8(const SByte8 &rhs);
	SByte8(const Reference<SByte8> &rhs);

	RValue<SByte8> operator=(RValue<SByte8> rhs);
	RValue<SByte8> operator=(const SByte8 &rhs);
	RValue<SByte8> operator=(const Reference<SByte8> &rhs);

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
RValue<SByte8> operator+=(SByte8 &lhs, RValue<SByte8> rhs);
RValue<SByte8> operator-=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator*=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator/=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator%=(SByte8 &lhs, RValue<SByte8> rhs);
RValue<SByte8> operator&=(SByte8 &lhs, RValue<SByte8> rhs);
RValue<SByte8> operator|=(SByte8 &lhs, RValue<SByte8> rhs);
RValue<SByte8> operator^=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator<<=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator>>=(SByte8 &lhs, RValue<SByte8> rhs);
//	RValue<SByte8> operator+(RValue<SByte8> val);
//	RValue<SByte8> operator-(RValue<SByte8> val);
RValue<SByte8> operator~(RValue<SByte8> val);
//	RValue<SByte8> operator++(SByte8 &val, int);   // Post-increment
//	const SByte8 &operator++(SByte8 &val);   // Pre-increment
//	RValue<SByte8> operator--(SByte8 &val, int);   // Post-decrement
//	const SByte8 &operator--(SByte8 &val);   // Pre-decrement

RValue<SByte8> AddSat(RValue<SByte8> x, RValue<SByte8> y);
RValue<SByte8> SubSat(RValue<SByte8> x, RValue<SByte8> y);
RValue<Short4> UnpackLow(RValue<SByte8> x, RValue<SByte8> y);
RValue<Short4> UnpackHigh(RValue<SByte8> x, RValue<SByte8> y);
RValue<Int> SignMask(RValue<SByte8> x);
RValue<Byte8> CmpGT(RValue<SByte8> x, RValue<SByte8> y);
RValue<Byte8> CmpEQ(RValue<SByte8> x, RValue<SByte8> y);

class Byte16 : public LValue<Byte16>
{
public:
	Byte16() = default;
	Byte16(RValue<Byte16> rhs);
	Byte16(const Byte16 &rhs);
	Byte16(const Reference<Byte16> &rhs);

	RValue<Byte16> operator=(RValue<Byte16> rhs);
	RValue<Byte16> operator=(const Byte16 &rhs);
	RValue<Byte16> operator=(const Reference<Byte16> &rhs);

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
//	RValue<Byte16> operator+=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator-=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator*=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator/=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator%=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator&=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator|=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator^=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator<<=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator>>=(Byte16 &lhs, RValue<Byte16> rhs);
//	RValue<Byte16> operator+(RValue<Byte16> val);
//	RValue<Byte16> operator-(RValue<Byte16> val);
//	RValue<Byte16> operator~(RValue<Byte16> val);
//	RValue<Byte16> operator++(Byte16 &val, int);   // Post-increment
//	const Byte16 &operator++(Byte16 &val);   // Pre-increment
//	RValue<Byte16> operator--(Byte16 &val, int);   // Post-decrement
//	const Byte16 &operator--(Byte16 &val);   // Pre-decrement
RValue<Byte16> Swizzle(RValue<Byte16> x, uint64_t select);

class SByte16 : public LValue<SByte16>
{
public:
	SByte16() = default;
	//	SByte16(int x, int y, int z, int w);
	//	SByte16(RValue<SByte16> rhs);
	//	SByte16(const SByte16 &rhs);
	//	SByte16(const Reference<SByte16> &rhs);

	//	RValue<SByte16> operator=(RValue<SByte16> rhs);
	//	RValue<SByte16> operator=(const SByte16 &rhs);
	//	RValue<SByte16> operator=(const Reference<SByte16> &rhs);

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
//	RValue<SByte16> operator+=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator-=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator*=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator/=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator%=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator&=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator|=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator^=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator<<=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator>>=(SByte16 &lhs, RValue<SByte16> rhs);
//	RValue<SByte16> operator+(RValue<SByte16> val);
//	RValue<SByte16> operator-(RValue<SByte16> val);
//	RValue<SByte16> operator~(RValue<SByte16> val);
//	RValue<SByte16> operator++(SByte16 &val, int);   // Post-increment
//	const SByte16 &operator++(SByte16 &val);   // Pre-increment
//	RValue<SByte16> operator--(SByte16 &val, int);   // Post-decrement
//	const SByte16 &operator--(SByte16 &val);   // Pre-decrement

class Short2 : public LValue<Short2>
{
public:
	explicit Short2(RValue<Short4> cast);

	static Type *getType();
};

class UShort2 : public LValue<UShort2>
{
public:
	explicit UShort2(RValue<UShort4> cast);

	static Type *getType();
};

class Short4 : public LValue<Short4>
{
public:
	explicit Short4(RValue<Int> cast);
	explicit Short4(RValue<Int4> cast);
	//	explicit Short4(RValue<Float> cast);
	explicit Short4(RValue<Float4> cast);

	Short4() = default;
	Short4(short xyzw);
	Short4(short x, short y, short z, short w);
	Short4(RValue<Short4> rhs);
	Short4(const Short4 &rhs);
	Short4(const Reference<Short4> &rhs);
	Short4(RValue<UShort4> rhs);
	Short4(const UShort4 &rhs);
	Short4(const Reference<UShort4> &rhs);

	RValue<Short4> operator=(RValue<Short4> rhs);
	RValue<Short4> operator=(const Short4 &rhs);
	RValue<Short4> operator=(const Reference<Short4> &rhs);
	RValue<Short4> operator=(RValue<UShort4> rhs);
	RValue<Short4> operator=(const UShort4 &rhs);
	RValue<Short4> operator=(const Reference<UShort4> &rhs);

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
RValue<Short4> operator+=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator-=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator*=(Short4 &lhs, RValue<Short4> rhs);
//	RValue<Short4> operator/=(Short4 &lhs, RValue<Short4> rhs);
//	RValue<Short4> operator%=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator&=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator|=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator^=(Short4 &lhs, RValue<Short4> rhs);
RValue<Short4> operator<<=(Short4 &lhs, unsigned char rhs);
RValue<Short4> operator>>=(Short4 &lhs, unsigned char rhs);
//	RValue<Short4> operator+(RValue<Short4> val);
RValue<Short4> operator-(RValue<Short4> val);
RValue<Short4> operator~(RValue<Short4> val);
//	RValue<Short4> operator++(Short4 &val, int);   // Post-increment
//	const Short4 &operator++(Short4 &val);   // Pre-increment
//	RValue<Short4> operator--(Short4 &val, int);   // Post-decrement
//	const Short4 &operator--(Short4 &val);   // Pre-decrement
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
RValue<SByte8> PackSigned(RValue<Short4> x, RValue<Short4> y);
RValue<Byte8> PackUnsigned(RValue<Short4> x, RValue<Short4> y);
RValue<Int2> UnpackLow(RValue<Short4> x, RValue<Short4> y);
RValue<Int2> UnpackHigh(RValue<Short4> x, RValue<Short4> y);
RValue<Short4> Swizzle(RValue<Short4> x, uint16_t select);
RValue<Short4> Insert(RValue<Short4> val, RValue<Short> element, int i);
RValue<Short> Extract(RValue<Short4> val, int i);
RValue<Short4> CmpGT(RValue<Short4> x, RValue<Short4> y);
RValue<Short4> CmpEQ(RValue<Short4> x, RValue<Short4> y);

class UShort4 : public LValue<UShort4>
{
public:
	explicit UShort4(RValue<Int4> cast);
	explicit UShort4(RValue<Float4> cast, bool saturate = false);

	UShort4() = default;
	UShort4(unsigned short xyzw);
	UShort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w);
	UShort4(RValue<UShort4> rhs);
	UShort4(const UShort4 &rhs);
	UShort4(const Reference<UShort4> &rhs);
	UShort4(RValue<Short4> rhs);
	UShort4(const Short4 &rhs);
	UShort4(const Reference<Short4> &rhs);

	RValue<UShort4> operator=(RValue<UShort4> rhs);
	RValue<UShort4> operator=(const UShort4 &rhs);
	RValue<UShort4> operator=(const Reference<UShort4> &rhs);
	RValue<UShort4> operator=(RValue<Short4> rhs);
	RValue<UShort4> operator=(const Short4 &rhs);
	RValue<UShort4> operator=(const Reference<Short4> &rhs);

	static Type *getType();
};

RValue<UShort4> operator+(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator-(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator*(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator/(RValue<UShort4> lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator%(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator&(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator|(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator^(RValue<UShort4> lhs, RValue<UShort4> rhs);
RValue<UShort4> operator<<(RValue<UShort4> lhs, unsigned char rhs);
RValue<UShort4> operator>>(RValue<UShort4> lhs, unsigned char rhs);
//	RValue<UShort4> operator+=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator-=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator*=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator/=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator%=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator&=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator|=(UShort4 &lhs, RValue<UShort4> rhs);
//	RValue<UShort4> operator^=(UShort4 &lhs, RValue<UShort4> rhs);
RValue<UShort4> operator<<=(UShort4 &lhs, unsigned char rhs);
RValue<UShort4> operator>>=(UShort4 &lhs, unsigned char rhs);
//	RValue<UShort4> operator+(RValue<UShort4> val);
//	RValue<UShort4> operator-(RValue<UShort4> val);
RValue<UShort4> operator~(RValue<UShort4> val);
//	RValue<UShort4> operator++(UShort4 &val, int);   // Post-increment
//	const UShort4 &operator++(UShort4 &val);   // Pre-increment
//	RValue<UShort4> operator--(UShort4 &val, int);   // Post-decrement
//	const UShort4 &operator--(UShort4 &val);   // Pre-decrement

RValue<UShort4> Max(RValue<UShort4> x, RValue<UShort4> y);
RValue<UShort4> Min(RValue<UShort4> x, RValue<UShort4> y);
RValue<UShort4> AddSat(RValue<UShort4> x, RValue<UShort4> y);
RValue<UShort4> SubSat(RValue<UShort4> x, RValue<UShort4> y);
RValue<UShort4> MulHigh(RValue<UShort4> x, RValue<UShort4> y);
RValue<UShort4> Average(RValue<UShort4> x, RValue<UShort4> y);

class Short8 : public LValue<Short8>
{
public:
	Short8() = default;
	Short8(short c);
	Short8(short c0, short c1, short c2, short c3, short c4, short c5, short c6, short c7);
	Short8(RValue<Short8> rhs);
	//	Short8(const Short8 &rhs);
	Short8(const Reference<Short8> &rhs);
	Short8(RValue<Short4> lo, RValue<Short4> hi);

	RValue<Short8> operator=(RValue<Short8> rhs);
	RValue<Short8> operator=(const Short8 &rhs);
	RValue<Short8> operator=(const Reference<Short8> &rhs);

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
//	RValue<Short8> operator+=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator-=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator*=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator/=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator%=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator&=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator|=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator^=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator<<=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator>>=(Short8 &lhs, RValue<Short8> rhs);
//	RValue<Short8> operator+(RValue<Short8> val);
//	RValue<Short8> operator-(RValue<Short8> val);
//	RValue<Short8> operator~(RValue<Short8> val);
//	RValue<Short8> operator++(Short8 &val, int);   // Post-increment
//	const Short8 &operator++(Short8 &val);   // Pre-increment
//	RValue<Short8> operator--(Short8 &val, int);   // Post-decrement
//	const Short8 &operator--(Short8 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator<=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator>(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator>=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator!=(RValue<Short8> lhs, RValue<Short8> rhs);
//	RValue<Bool> operator==(RValue<Short8> lhs, RValue<Short8> rhs);

RValue<Short8> MulHigh(RValue<Short8> x, RValue<Short8> y);
RValue<Int4> MulAdd(RValue<Short8> x, RValue<Short8> y);
RValue<Int4> Abs(RValue<Int4> x);

class UShort8 : public LValue<UShort8>
{
public:
	UShort8() = default;
	UShort8(unsigned short c);
	UShort8(unsigned short c0, unsigned short c1, unsigned short c2, unsigned short c3, unsigned short c4, unsigned short c5, unsigned short c6, unsigned short c7);
	UShort8(RValue<UShort8> rhs);
	//	UShort8(const UShort8 &rhs);
	UShort8(const Reference<UShort8> &rhs);
	UShort8(RValue<UShort4> lo, RValue<UShort4> hi);

	RValue<UShort8> operator=(RValue<UShort8> rhs);
	RValue<UShort8> operator=(const UShort8 &rhs);
	RValue<UShort8> operator=(const Reference<UShort8> &rhs);

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
RValue<UShort8> operator+=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator-=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator*=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator/=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator%=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator&=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator|=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator^=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator<<=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator>>=(UShort8 &lhs, RValue<UShort8> rhs);
//	RValue<UShort8> operator+(RValue<UShort8> val);
//	RValue<UShort8> operator-(RValue<UShort8> val);
RValue<UShort8> operator~(RValue<UShort8> val);
//	RValue<UShort8> operator++(UShort8 &val, int);   // Post-increment
//	const UShort8 &operator++(UShort8 &val);   // Pre-increment
//	RValue<UShort8> operator--(UShort8 &val, int);   // Post-decrement
//	const UShort8 &operator--(UShort8 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator<=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator>(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator>=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator!=(RValue<UShort8> lhs, RValue<UShort8> rhs);
//	RValue<Bool> operator==(RValue<UShort8> lhs, RValue<UShort8> rhs);

RValue<UShort8> Swizzle(RValue<UShort8> x, uint32_t select);
RValue<UShort8> MulHigh(RValue<UShort8> x, RValue<UShort8> y);

class Int : public LValue<Int>
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

	Int() = default;
	Int(int x);
	Int(RValue<Int> rhs);
	Int(RValue<UInt> rhs);
	Int(const Int &rhs);
	Int(const UInt &rhs);
	Int(const Reference<Int> &rhs);
	Int(const Reference<UInt> &rhs);

	RValue<Int> operator=(int rhs);
	RValue<Int> operator=(RValue<Int> rhs);
	RValue<Int> operator=(RValue<UInt> rhs);
	RValue<Int> operator=(const Int &rhs);
	RValue<Int> operator=(const UInt &rhs);
	RValue<Int> operator=(const Reference<Int> &rhs);
	RValue<Int> operator=(const Reference<UInt> &rhs);

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
RValue<Int> operator+=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator-=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator*=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator/=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator%=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator&=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator|=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator^=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator<<=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator>>=(Int &lhs, RValue<Int> rhs);
RValue<Int> operator+(RValue<Int> val);
RValue<Int> operator-(RValue<Int> val);
RValue<Int> operator~(RValue<Int> val);
RValue<Int> operator++(Int &val, int);  // Post-increment
const Int &operator++(Int &val);        // Pre-increment
RValue<Int> operator--(Int &val, int);  // Post-decrement
const Int &operator--(Int &val);        // Pre-decrement
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

class Long : public LValue<Long>
{
public:
	//	Long(Argument<Long> argument);

	//	explicit Long(RValue<Short> cast);
	//	explicit Long(RValue<UShort> cast);
	explicit Long(RValue<Int> cast);
	explicit Long(RValue<UInt> cast);
	//	explicit Long(RValue<Float> cast);

	Long() = default;
	//	Long(qword x);
	Long(RValue<Long> rhs);
	//	Long(RValue<ULong> rhs);
	//	Long(const Long &rhs);
	//	Long(const Reference<Long> &rhs);
	//	Long(const ULong &rhs);
	//	Long(const Reference<ULong> &rhs);

	RValue<Long> operator=(int64_t rhs);
	RValue<Long> operator=(RValue<Long> rhs);
	//	RValue<Long> operator=(RValue<ULong> rhs);
	RValue<Long> operator=(const Long &rhs);
	RValue<Long> operator=(const Reference<Long> &rhs);
	//	RValue<Long> operator=(const ULong &rhs);
	//	RValue<Long> operator=(const Reference<ULong> &rhs);

	static Type *getType();
};

RValue<Long> operator+(RValue<Long> lhs, RValue<Long> rhs);
RValue<Long> operator-(RValue<Long> lhs, RValue<Long> rhs);
RValue<Long> operator*(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator/(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator%(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator&(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator|(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator^(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Long> operator<<(RValue<Long> lhs, RValue<Long> rhs);
RValue<Long> operator>>(RValue<Long> lhs, RValue<Long> rhs);
RValue<Long> operator+=(Long &lhs, RValue<Long> rhs);
RValue<Long> operator-=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator*=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator/=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator%=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator&=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator|=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator^=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator<<=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator>>=(Long &lhs, RValue<Long> rhs);
//	RValue<Long> operator+(RValue<Long> val);
//	RValue<Long> operator-(RValue<Long> val);
//	RValue<Long> operator~(RValue<Long> val);
//	RValue<Long> operator++(Long &val, int);   // Post-increment
//	const Long &operator++(Long &val);   // Pre-increment
//	RValue<Long> operator--(Long &val, int);   // Post-decrement
//	const Long &operator--(Long &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator<=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator>(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator>=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator!=(RValue<Long> lhs, RValue<Long> rhs);
//	RValue<Bool> operator==(RValue<Long> lhs, RValue<Long> rhs);

//	RValue<Long> RoundLong(RValue<Float> cast);
RValue<Long> AddAtomic(RValue<Pointer<Long>> x, RValue<Long> y);

class UInt : public LValue<UInt>
{
public:
	UInt(Argument<UInt> argument);

	explicit UInt(RValue<UShort> cast);
	explicit UInt(RValue<Long> cast);
	explicit UInt(RValue<Float> cast);

	UInt() = default;
	UInt(int x);
	UInt(unsigned int x);
	UInt(RValue<UInt> rhs);
	UInt(RValue<Int> rhs);
	UInt(const UInt &rhs);
	UInt(const Int &rhs);
	UInt(const Reference<UInt> &rhs);
	UInt(const Reference<Int> &rhs);

	RValue<UInt> operator=(unsigned int rhs);
	RValue<UInt> operator=(RValue<UInt> rhs);
	RValue<UInt> operator=(RValue<Int> rhs);
	RValue<UInt> operator=(const UInt &rhs);
	RValue<UInt> operator=(const Int &rhs);
	RValue<UInt> operator=(const Reference<UInt> &rhs);
	RValue<UInt> operator=(const Reference<Int> &rhs);

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
RValue<UInt> operator+=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator-=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator*=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator/=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator%=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator&=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator|=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator^=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator<<=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator>>=(UInt &lhs, RValue<UInt> rhs);
RValue<UInt> operator+(RValue<UInt> val);
RValue<UInt> operator-(RValue<UInt> val);
RValue<UInt> operator~(RValue<UInt> val);
RValue<UInt> operator++(UInt &val, int);  // Post-increment
const UInt &operator++(UInt &val);        // Pre-increment
RValue<UInt> operator--(UInt &val, int);  // Post-decrement
const UInt &operator--(UInt &val);        // Pre-decrement
RValue<Bool> operator<(RValue<UInt> lhs, RValue<UInt> rhs);
RValue<Bool> operator<=(RValue<UInt> lhs, RValue<UInt> rhs);
RValue<Bool> operator>(RValue<UInt> lhs, RValue<UInt> rhs);
RValue<Bool> operator>=(RValue<UInt> lhs, RValue<UInt> rhs);
RValue<Bool> operator!=(RValue<UInt> lhs, RValue<UInt> rhs);
RValue<Bool> operator==(RValue<UInt> lhs, RValue<UInt> rhs);

RValue<UInt> Max(RValue<UInt> x, RValue<UInt> y);
RValue<UInt> Min(RValue<UInt> x, RValue<UInt> y);
RValue<UInt> Clamp(RValue<UInt> x, RValue<UInt> min, RValue<UInt> max);

RValue<UInt> AddAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> SubAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> AndAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> OrAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> XorAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<Int> MinAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder);
RValue<Int> MaxAtomic(RValue<Pointer<Int>> x, RValue<Int> y, std::memory_order memoryOrder);
RValue<UInt> MinAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> MaxAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> ExchangeAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, std::memory_order memoryOrder);
RValue<UInt> CompareExchangeAtomic(RValue<Pointer<UInt>> x, RValue<UInt> y, RValue<UInt> compare, std::memory_order memoryOrderEqual, std::memory_order memoryOrderUnequal);

//	RValue<UInt> RoundUInt(RValue<Float> cast);

class Int2 : public LValue<Int2>
{
public:
	//	explicit Int2(RValue<Int> cast);
	explicit Int2(RValue<Int4> cast);

	Int2() = default;
	Int2(int x, int y);
	Int2(RValue<Int2> rhs);
	Int2(const Int2 &rhs);
	Int2(const Reference<Int2> &rhs);
	Int2(RValue<Int> lo, RValue<Int> hi);

	RValue<Int2> operator=(RValue<Int2> rhs);
	RValue<Int2> operator=(const Int2 &rhs);
	RValue<Int2> operator=(const Reference<Int2> &rhs);

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
RValue<Int2> operator+=(Int2 &lhs, RValue<Int2> rhs);
RValue<Int2> operator-=(Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator*=(Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator/=(Int2 &lhs, RValue<Int2> rhs);
//	RValue<Int2> operator%=(Int2 &lhs, RValue<Int2> rhs);
RValue<Int2> operator&=(Int2 &lhs, RValue<Int2> rhs);
RValue<Int2> operator|=(Int2 &lhs, RValue<Int2> rhs);
RValue<Int2> operator^=(Int2 &lhs, RValue<Int2> rhs);
RValue<Int2> operator<<=(Int2 &lhs, unsigned char rhs);
RValue<Int2> operator>>=(Int2 &lhs, unsigned char rhs);
//	RValue<Int2> operator+(RValue<Int2> val);
//	RValue<Int2> operator-(RValue<Int2> val);
RValue<Int2> operator~(RValue<Int2> val);
//	RValue<Int2> operator++(Int2 &val, int);   // Post-increment
//	const Int2 &operator++(Int2 &val);   // Pre-increment
//	RValue<Int2> operator--(Int2 &val, int);   // Post-decrement
//	const Int2 &operator--(Int2 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator<=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator>(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator>=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator!=(RValue<Int2> lhs, RValue<Int2> rhs);
//	RValue<Bool> operator==(RValue<Int2> lhs, RValue<Int2> rhs);

//	RValue<Int2> RoundInt(RValue<Float4> cast);
RValue<Short4> UnpackLow(RValue<Int2> x, RValue<Int2> y);
RValue<Short4> UnpackHigh(RValue<Int2> x, RValue<Int2> y);
RValue<Int> Extract(RValue<Int2> val, int i);
RValue<Int2> Insert(RValue<Int2> val, RValue<Int> element, int i);

class UInt2 : public LValue<UInt2>
{
public:
	UInt2() = default;
	UInt2(unsigned int x, unsigned int y);
	UInt2(RValue<UInt2> rhs);
	UInt2(const UInt2 &rhs);
	UInt2(const Reference<UInt2> &rhs);

	RValue<UInt2> operator=(RValue<UInt2> rhs);
	RValue<UInt2> operator=(const UInt2 &rhs);
	RValue<UInt2> operator=(const Reference<UInt2> &rhs);

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
RValue<UInt2> operator+=(UInt2 &lhs, RValue<UInt2> rhs);
RValue<UInt2> operator-=(UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator*=(UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator/=(UInt2 &lhs, RValue<UInt2> rhs);
//	RValue<UInt2> operator%=(UInt2 &lhs, RValue<UInt2> rhs);
RValue<UInt2> operator&=(UInt2 &lhs, RValue<UInt2> rhs);
RValue<UInt2> operator|=(UInt2 &lhs, RValue<UInt2> rhs);
RValue<UInt2> operator^=(UInt2 &lhs, RValue<UInt2> rhs);
RValue<UInt2> operator<<=(UInt2 &lhs, unsigned char rhs);
RValue<UInt2> operator>>=(UInt2 &lhs, unsigned char rhs);
//	RValue<UInt2> operator+(RValue<UInt2> val);
//	RValue<UInt2> operator-(RValue<UInt2> val);
RValue<UInt2> operator~(RValue<UInt2> val);
//	RValue<UInt2> operator++(UInt2 &val, int);   // Post-increment
//	const UInt2 &operator++(UInt2 &val);   // Pre-increment
//	RValue<UInt2> operator--(UInt2 &val, int);   // Post-decrement
//	const UInt2 &operator--(UInt2 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator<=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator>(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator>=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator!=(RValue<UInt2> lhs, RValue<UInt2> rhs);
//	RValue<Bool> operator==(RValue<UInt2> lhs, RValue<UInt2> rhs);

//	RValue<UInt2> RoundInt(RValue<Float4> cast);
RValue<UInt> Extract(RValue<UInt2> val, int i);
RValue<UInt2> Insert(RValue<UInt2> val, RValue<UInt> element, int i);

template<class T>
struct Scalar;

template<class Vector4>
struct XYZW;

template<class Vector4, int T>
class Swizzle2
{
	friend Vector4;

public:
	operator RValue<Vector4>() const;

private:
	Vector4 *parent;
};

template<class Vector4, int T>
class Swizzle4
{
public:
	operator RValue<Vector4>() const;

private:
	Vector4 *parent;
};

template<class Vector4, int T>
class SwizzleMask4
{
	friend XYZW<Vector4>;

public:
	operator RValue<Vector4>() const;

	RValue<Vector4> operator=(RValue<Vector4> rhs);
	RValue<Vector4> operator=(RValue<typename Scalar<Vector4>::Type> rhs);

private:
	Vector4 *parent;
};

template<>
struct Scalar<Float4>
{
	using Type = Float;
};

template<>
struct Scalar<Int4>
{
	using Type = Int;
};

template<>
struct Scalar<UInt4>
{
	using Type = UInt;
};

template<class Vector4, int T>
class SwizzleMask1
{
public:
	operator RValue<typename Scalar<Vector4>::Type>() const;
	operator RValue<Vector4>() const;

	RValue<Vector4> operator=(float x);
	RValue<Vector4> operator=(RValue<Vector4> rhs);
	RValue<Vector4> operator=(RValue<typename Scalar<Vector4>::Type> rhs);

private:
	Vector4 *parent;
};

template<class Vector4, int T>
class SwizzleMask2
{
	friend class Float4;

public:
	operator RValue<Vector4>() const;

	RValue<Vector4> operator=(RValue<Vector4> rhs);

private:
	Float4 *parent;
};

template<class Vector4>
struct XYZW
{
	friend Vector4;

private:
	XYZW(Vector4 *parent)
	{
		xyzw.parent = parent;
	}

public:
	union
	{
		SwizzleMask1<Vector4, 0x0000> x;
		SwizzleMask1<Vector4, 0x1111> y;
		SwizzleMask1<Vector4, 0x2222> z;
		SwizzleMask1<Vector4, 0x3333> w;
		Swizzle2<Vector4, 0x0000> xx;
		Swizzle2<Vector4, 0x1000> yx;
		Swizzle2<Vector4, 0x2000> zx;
		Swizzle2<Vector4, 0x3000> wx;
		SwizzleMask2<Vector4, 0x0111> xy;
		Swizzle2<Vector4, 0x1111> yy;
		Swizzle2<Vector4, 0x2111> zy;
		Swizzle2<Vector4, 0x3111> wy;
		SwizzleMask2<Vector4, 0x0222> xz;
		SwizzleMask2<Vector4, 0x1222> yz;
		Swizzle2<Vector4, 0x2222> zz;
		Swizzle2<Vector4, 0x3222> wz;
		SwizzleMask2<Vector4, 0x0333> xw;
		SwizzleMask2<Vector4, 0x1333> yw;
		SwizzleMask2<Vector4, 0x2333> zw;
		Swizzle2<Vector4, 0x3333> ww;
		Swizzle4<Vector4, 0x0000> xxx;
		Swizzle4<Vector4, 0x1000> yxx;
		Swizzle4<Vector4, 0x2000> zxx;
		Swizzle4<Vector4, 0x3000> wxx;
		Swizzle4<Vector4, 0x0100> xyx;
		Swizzle4<Vector4, 0x1100> yyx;
		Swizzle4<Vector4, 0x2100> zyx;
		Swizzle4<Vector4, 0x3100> wyx;
		Swizzle4<Vector4, 0x0200> xzx;
		Swizzle4<Vector4, 0x1200> yzx;
		Swizzle4<Vector4, 0x2200> zzx;
		Swizzle4<Vector4, 0x3200> wzx;
		Swizzle4<Vector4, 0x0300> xwx;
		Swizzle4<Vector4, 0x1300> ywx;
		Swizzle4<Vector4, 0x2300> zwx;
		Swizzle4<Vector4, 0x3300> wwx;
		Swizzle4<Vector4, 0x0011> xxy;
		Swizzle4<Vector4, 0x1011> yxy;
		Swizzle4<Vector4, 0x2011> zxy;
		Swizzle4<Vector4, 0x3011> wxy;
		Swizzle4<Vector4, 0x0111> xyy;
		Swizzle4<Vector4, 0x1111> yyy;
		Swizzle4<Vector4, 0x2111> zyy;
		Swizzle4<Vector4, 0x3111> wyy;
		Swizzle4<Vector4, 0x0211> xzy;
		Swizzle4<Vector4, 0x1211> yzy;
		Swizzle4<Vector4, 0x2211> zzy;
		Swizzle4<Vector4, 0x3211> wzy;
		Swizzle4<Vector4, 0x0311> xwy;
		Swizzle4<Vector4, 0x1311> ywy;
		Swizzle4<Vector4, 0x2311> zwy;
		Swizzle4<Vector4, 0x3311> wwy;
		Swizzle4<Vector4, 0x0022> xxz;
		Swizzle4<Vector4, 0x1022> yxz;
		Swizzle4<Vector4, 0x2022> zxz;
		Swizzle4<Vector4, 0x3022> wxz;
		SwizzleMask4<Vector4, 0x0122> xyz;
		Swizzle4<Vector4, 0x1122> yyz;
		Swizzle4<Vector4, 0x2122> zyz;
		Swizzle4<Vector4, 0x3122> wyz;
		Swizzle4<Vector4, 0x0222> xzz;
		Swizzle4<Vector4, 0x1222> yzz;
		Swizzle4<Vector4, 0x2222> zzz;
		Swizzle4<Vector4, 0x3222> wzz;
		Swizzle4<Vector4, 0x0322> xwz;
		Swizzle4<Vector4, 0x1322> ywz;
		Swizzle4<Vector4, 0x2322> zwz;
		Swizzle4<Vector4, 0x3322> wwz;
		Swizzle4<Vector4, 0x0033> xxw;
		Swizzle4<Vector4, 0x1033> yxw;
		Swizzle4<Vector4, 0x2033> zxw;
		Swizzle4<Vector4, 0x3033> wxw;
		SwizzleMask4<Vector4, 0x0133> xyw;
		Swizzle4<Vector4, 0x1133> yyw;
		Swizzle4<Vector4, 0x2133> zyw;
		Swizzle4<Vector4, 0x3133> wyw;
		SwizzleMask4<Vector4, 0x0233> xzw;
		SwizzleMask4<Vector4, 0x1233> yzw;
		Swizzle4<Vector4, 0x2233> zzw;
		Swizzle4<Vector4, 0x3233> wzw;
		Swizzle4<Vector4, 0x0333> xww;
		Swizzle4<Vector4, 0x1333> yww;
		Swizzle4<Vector4, 0x2333> zww;
		Swizzle4<Vector4, 0x3333> www;
		Swizzle4<Vector4, 0x0000> xxxx;
		Swizzle4<Vector4, 0x1000> yxxx;
		Swizzle4<Vector4, 0x2000> zxxx;
		Swizzle4<Vector4, 0x3000> wxxx;
		Swizzle4<Vector4, 0x0100> xyxx;
		Swizzle4<Vector4, 0x1100> yyxx;
		Swizzle4<Vector4, 0x2100> zyxx;
		Swizzle4<Vector4, 0x3100> wyxx;
		Swizzle4<Vector4, 0x0200> xzxx;
		Swizzle4<Vector4, 0x1200> yzxx;
		Swizzle4<Vector4, 0x2200> zzxx;
		Swizzle4<Vector4, 0x3200> wzxx;
		Swizzle4<Vector4, 0x0300> xwxx;
		Swizzle4<Vector4, 0x1300> ywxx;
		Swizzle4<Vector4, 0x2300> zwxx;
		Swizzle4<Vector4, 0x3300> wwxx;
		Swizzle4<Vector4, 0x0010> xxyx;
		Swizzle4<Vector4, 0x1010> yxyx;
		Swizzle4<Vector4, 0x2010> zxyx;
		Swizzle4<Vector4, 0x3010> wxyx;
		Swizzle4<Vector4, 0x0110> xyyx;
		Swizzle4<Vector4, 0x1110> yyyx;
		Swizzle4<Vector4, 0x2110> zyyx;
		Swizzle4<Vector4, 0x3110> wyyx;
		Swizzle4<Vector4, 0x0210> xzyx;
		Swizzle4<Vector4, 0x1210> yzyx;
		Swizzle4<Vector4, 0x2210> zzyx;
		Swizzle4<Vector4, 0x3210> wzyx;
		Swizzle4<Vector4, 0x0310> xwyx;
		Swizzle4<Vector4, 0x1310> ywyx;
		Swizzle4<Vector4, 0x2310> zwyx;
		Swizzle4<Vector4, 0x3310> wwyx;
		Swizzle4<Vector4, 0x0020> xxzx;
		Swizzle4<Vector4, 0x1020> yxzx;
		Swizzle4<Vector4, 0x2020> zxzx;
		Swizzle4<Vector4, 0x3020> wxzx;
		Swizzle4<Vector4, 0x0120> xyzx;
		Swizzle4<Vector4, 0x1120> yyzx;
		Swizzle4<Vector4, 0x2120> zyzx;
		Swizzle4<Vector4, 0x3120> wyzx;
		Swizzle4<Vector4, 0x0220> xzzx;
		Swizzle4<Vector4, 0x1220> yzzx;
		Swizzle4<Vector4, 0x2220> zzzx;
		Swizzle4<Vector4, 0x3220> wzzx;
		Swizzle4<Vector4, 0x0320> xwzx;
		Swizzle4<Vector4, 0x1320> ywzx;
		Swizzle4<Vector4, 0x2320> zwzx;
		Swizzle4<Vector4, 0x3320> wwzx;
		Swizzle4<Vector4, 0x0030> xxwx;
		Swizzle4<Vector4, 0x1030> yxwx;
		Swizzle4<Vector4, 0x2030> zxwx;
		Swizzle4<Vector4, 0x3030> wxwx;
		Swizzle4<Vector4, 0x0130> xywx;
		Swizzle4<Vector4, 0x1130> yywx;
		Swizzle4<Vector4, 0x2130> zywx;
		Swizzle4<Vector4, 0x3130> wywx;
		Swizzle4<Vector4, 0x0230> xzwx;
		Swizzle4<Vector4, 0x1230> yzwx;
		Swizzle4<Vector4, 0x2230> zzwx;
		Swizzle4<Vector4, 0x3230> wzwx;
		Swizzle4<Vector4, 0x0330> xwwx;
		Swizzle4<Vector4, 0x1330> ywwx;
		Swizzle4<Vector4, 0x2330> zwwx;
		Swizzle4<Vector4, 0x3330> wwwx;
		Swizzle4<Vector4, 0x0001> xxxy;
		Swizzle4<Vector4, 0x1001> yxxy;
		Swizzle4<Vector4, 0x2001> zxxy;
		Swizzle4<Vector4, 0x3001> wxxy;
		Swizzle4<Vector4, 0x0101> xyxy;
		Swizzle4<Vector4, 0x1101> yyxy;
		Swizzle4<Vector4, 0x2101> zyxy;
		Swizzle4<Vector4, 0x3101> wyxy;
		Swizzle4<Vector4, 0x0201> xzxy;
		Swizzle4<Vector4, 0x1201> yzxy;
		Swizzle4<Vector4, 0x2201> zzxy;
		Swizzle4<Vector4, 0x3201> wzxy;
		Swizzle4<Vector4, 0x0301> xwxy;
		Swizzle4<Vector4, 0x1301> ywxy;
		Swizzle4<Vector4, 0x2301> zwxy;
		Swizzle4<Vector4, 0x3301> wwxy;
		Swizzle4<Vector4, 0x0011> xxyy;
		Swizzle4<Vector4, 0x1011> yxyy;
		Swizzle4<Vector4, 0x2011> zxyy;
		Swizzle4<Vector4, 0x3011> wxyy;
		Swizzle4<Vector4, 0x0111> xyyy;
		Swizzle4<Vector4, 0x1111> yyyy;
		Swizzle4<Vector4, 0x2111> zyyy;
		Swizzle4<Vector4, 0x3111> wyyy;
		Swizzle4<Vector4, 0x0211> xzyy;
		Swizzle4<Vector4, 0x1211> yzyy;
		Swizzle4<Vector4, 0x2211> zzyy;
		Swizzle4<Vector4, 0x3211> wzyy;
		Swizzle4<Vector4, 0x0311> xwyy;
		Swizzle4<Vector4, 0x1311> ywyy;
		Swizzle4<Vector4, 0x2311> zwyy;
		Swizzle4<Vector4, 0x3311> wwyy;
		Swizzle4<Vector4, 0x0021> xxzy;
		Swizzle4<Vector4, 0x1021> yxzy;
		Swizzle4<Vector4, 0x2021> zxzy;
		Swizzle4<Vector4, 0x3021> wxzy;
		Swizzle4<Vector4, 0x0121> xyzy;
		Swizzle4<Vector4, 0x1121> yyzy;
		Swizzle4<Vector4, 0x2121> zyzy;
		Swizzle4<Vector4, 0x3121> wyzy;
		Swizzle4<Vector4, 0x0221> xzzy;
		Swizzle4<Vector4, 0x1221> yzzy;
		Swizzle4<Vector4, 0x2221> zzzy;
		Swizzle4<Vector4, 0x3221> wzzy;
		Swizzle4<Vector4, 0x0321> xwzy;
		Swizzle4<Vector4, 0x1321> ywzy;
		Swizzle4<Vector4, 0x2321> zwzy;
		Swizzle4<Vector4, 0x3321> wwzy;
		Swizzle4<Vector4, 0x0031> xxwy;
		Swizzle4<Vector4, 0x1031> yxwy;
		Swizzle4<Vector4, 0x2031> zxwy;
		Swizzle4<Vector4, 0x3031> wxwy;
		Swizzle4<Vector4, 0x0131> xywy;
		Swizzle4<Vector4, 0x1131> yywy;
		Swizzle4<Vector4, 0x2131> zywy;
		Swizzle4<Vector4, 0x3131> wywy;
		Swizzle4<Vector4, 0x0231> xzwy;
		Swizzle4<Vector4, 0x1231> yzwy;
		Swizzle4<Vector4, 0x2231> zzwy;
		Swizzle4<Vector4, 0x3231> wzwy;
		Swizzle4<Vector4, 0x0331> xwwy;
		Swizzle4<Vector4, 0x1331> ywwy;
		Swizzle4<Vector4, 0x2331> zwwy;
		Swizzle4<Vector4, 0x3331> wwwy;
		Swizzle4<Vector4, 0x0002> xxxz;
		Swizzle4<Vector4, 0x1002> yxxz;
		Swizzle4<Vector4, 0x2002> zxxz;
		Swizzle4<Vector4, 0x3002> wxxz;
		Swizzle4<Vector4, 0x0102> xyxz;
		Swizzle4<Vector4, 0x1102> yyxz;
		Swizzle4<Vector4, 0x2102> zyxz;
		Swizzle4<Vector4, 0x3102> wyxz;
		Swizzle4<Vector4, 0x0202> xzxz;
		Swizzle4<Vector4, 0x1202> yzxz;
		Swizzle4<Vector4, 0x2202> zzxz;
		Swizzle4<Vector4, 0x3202> wzxz;
		Swizzle4<Vector4, 0x0302> xwxz;
		Swizzle4<Vector4, 0x1302> ywxz;
		Swizzle4<Vector4, 0x2302> zwxz;
		Swizzle4<Vector4, 0x3302> wwxz;
		Swizzle4<Vector4, 0x0012> xxyz;
		Swizzle4<Vector4, 0x1012> yxyz;
		Swizzle4<Vector4, 0x2012> zxyz;
		Swizzle4<Vector4, 0x3012> wxyz;
		Swizzle4<Vector4, 0x0112> xyyz;
		Swizzle4<Vector4, 0x1112> yyyz;
		Swizzle4<Vector4, 0x2112> zyyz;
		Swizzle4<Vector4, 0x3112> wyyz;
		Swizzle4<Vector4, 0x0212> xzyz;
		Swizzle4<Vector4, 0x1212> yzyz;
		Swizzle4<Vector4, 0x2212> zzyz;
		Swizzle4<Vector4, 0x3212> wzyz;
		Swizzle4<Vector4, 0x0312> xwyz;
		Swizzle4<Vector4, 0x1312> ywyz;
		Swizzle4<Vector4, 0x2312> zwyz;
		Swizzle4<Vector4, 0x3312> wwyz;
		Swizzle4<Vector4, 0x0022> xxzz;
		Swizzle4<Vector4, 0x1022> yxzz;
		Swizzle4<Vector4, 0x2022> zxzz;
		Swizzle4<Vector4, 0x3022> wxzz;
		Swizzle4<Vector4, 0x0122> xyzz;
		Swizzle4<Vector4, 0x1122> yyzz;
		Swizzle4<Vector4, 0x2122> zyzz;
		Swizzle4<Vector4, 0x3122> wyzz;
		Swizzle4<Vector4, 0x0222> xzzz;
		Swizzle4<Vector4, 0x1222> yzzz;
		Swizzle4<Vector4, 0x2222> zzzz;
		Swizzle4<Vector4, 0x3222> wzzz;
		Swizzle4<Vector4, 0x0322> xwzz;
		Swizzle4<Vector4, 0x1322> ywzz;
		Swizzle4<Vector4, 0x2322> zwzz;
		Swizzle4<Vector4, 0x3322> wwzz;
		Swizzle4<Vector4, 0x0032> xxwz;
		Swizzle4<Vector4, 0x1032> yxwz;
		Swizzle4<Vector4, 0x2032> zxwz;
		Swizzle4<Vector4, 0x3032> wxwz;
		Swizzle4<Vector4, 0x0132> xywz;
		Swizzle4<Vector4, 0x1132> yywz;
		Swizzle4<Vector4, 0x2132> zywz;
		Swizzle4<Vector4, 0x3132> wywz;
		Swizzle4<Vector4, 0x0232> xzwz;
		Swizzle4<Vector4, 0x1232> yzwz;
		Swizzle4<Vector4, 0x2232> zzwz;
		Swizzle4<Vector4, 0x3232> wzwz;
		Swizzle4<Vector4, 0x0332> xwwz;
		Swizzle4<Vector4, 0x1332> ywwz;
		Swizzle4<Vector4, 0x2332> zwwz;
		Swizzle4<Vector4, 0x3332> wwwz;
		Swizzle4<Vector4, 0x0003> xxxw;
		Swizzle4<Vector4, 0x1003> yxxw;
		Swizzle4<Vector4, 0x2003> zxxw;
		Swizzle4<Vector4, 0x3003> wxxw;
		Swizzle4<Vector4, 0x0103> xyxw;
		Swizzle4<Vector4, 0x1103> yyxw;
		Swizzle4<Vector4, 0x2103> zyxw;
		Swizzle4<Vector4, 0x3103> wyxw;
		Swizzle4<Vector4, 0x0203> xzxw;
		Swizzle4<Vector4, 0x1203> yzxw;
		Swizzle4<Vector4, 0x2203> zzxw;
		Swizzle4<Vector4, 0x3203> wzxw;
		Swizzle4<Vector4, 0x0303> xwxw;
		Swizzle4<Vector4, 0x1303> ywxw;
		Swizzle4<Vector4, 0x2303> zwxw;
		Swizzle4<Vector4, 0x3303> wwxw;
		Swizzle4<Vector4, 0x0013> xxyw;
		Swizzle4<Vector4, 0x1013> yxyw;
		Swizzle4<Vector4, 0x2013> zxyw;
		Swizzle4<Vector4, 0x3013> wxyw;
		Swizzle4<Vector4, 0x0113> xyyw;
		Swizzle4<Vector4, 0x1113> yyyw;
		Swizzle4<Vector4, 0x2113> zyyw;
		Swizzle4<Vector4, 0x3113> wyyw;
		Swizzle4<Vector4, 0x0213> xzyw;
		Swizzle4<Vector4, 0x1213> yzyw;
		Swizzle4<Vector4, 0x2213> zzyw;
		Swizzle4<Vector4, 0x3213> wzyw;
		Swizzle4<Vector4, 0x0313> xwyw;
		Swizzle4<Vector4, 0x1313> ywyw;
		Swizzle4<Vector4, 0x2313> zwyw;
		Swizzle4<Vector4, 0x3313> wwyw;
		Swizzle4<Vector4, 0x0023> xxzw;
		Swizzle4<Vector4, 0x1023> yxzw;
		Swizzle4<Vector4, 0x2023> zxzw;
		Swizzle4<Vector4, 0x3023> wxzw;
		SwizzleMask4<Vector4, 0x0123> xyzw;
		Swizzle4<Vector4, 0x1123> yyzw;
		Swizzle4<Vector4, 0x2123> zyzw;
		Swizzle4<Vector4, 0x3123> wyzw;
		Swizzle4<Vector4, 0x0223> xzzw;
		Swizzle4<Vector4, 0x1223> yzzw;
		Swizzle4<Vector4, 0x2223> zzzw;
		Swizzle4<Vector4, 0x3223> wzzw;
		Swizzle4<Vector4, 0x0323> xwzw;
		Swizzle4<Vector4, 0x1323> ywzw;
		Swizzle4<Vector4, 0x2323> zwzw;
		Swizzle4<Vector4, 0x3323> wwzw;
		Swizzle4<Vector4, 0x0033> xxww;
		Swizzle4<Vector4, 0x1033> yxww;
		Swizzle4<Vector4, 0x2033> zxww;
		Swizzle4<Vector4, 0x3033> wxww;
		Swizzle4<Vector4, 0x0133> xyww;
		Swizzle4<Vector4, 0x1133> yyww;
		Swizzle4<Vector4, 0x2133> zyww;
		Swizzle4<Vector4, 0x3133> wyww;
		Swizzle4<Vector4, 0x0233> xzww;
		Swizzle4<Vector4, 0x1233> yzww;
		Swizzle4<Vector4, 0x2233> zzww;
		Swizzle4<Vector4, 0x3233> wzww;
		Swizzle4<Vector4, 0x0333> xwww;
		Swizzle4<Vector4, 0x1333> ywww;
		Swizzle4<Vector4, 0x2333> zwww;
		Swizzle4<Vector4, 0x3333> wwww;
	};
};

class Int4 : public LValue<Int4>, public XYZW<Int4>
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

	RValue<Int4> operator=(RValue<Int4> rhs);
	RValue<Int4> operator=(const Int4 &rhs);
	RValue<Int4> operator=(const Reference<Int4> &rhs);

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
RValue<Int4> operator+=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator-=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator*=(Int4 &lhs, RValue<Int4> rhs);
//	RValue<Int4> operator/=(Int4 &lhs, RValue<Int4> rhs);
//	RValue<Int4> operator%=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator&=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator|=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator^=(Int4 &lhs, RValue<Int4> rhs);
RValue<Int4> operator<<=(Int4 &lhs, unsigned char rhs);
RValue<Int4> operator>>=(Int4 &lhs, unsigned char rhs);
RValue<Int4> operator+(RValue<Int4> val);
RValue<Int4> operator-(RValue<Int4> val);
RValue<Int4> operator~(RValue<Int4> val);
//	RValue<Int4> operator++(Int4 &val, int);   // Post-increment
//	const Int4 &operator++(Int4 &val);   // Pre-increment
//	RValue<Int4> operator--(Int4 &val, int);   // Post-decrement
//	const Int4 &operator--(Int4 &val);   // Pre-decrement
//	RValue<Bool> operator<(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator<=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator>(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator>=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator!=(RValue<Int4> lhs, RValue<Int4> rhs);
//	RValue<Bool> operator==(RValue<Int4> lhs, RValue<Int4> rhs);

inline RValue<Int4> operator+(RValue<Int> lhs, RValue<Int4> rhs)
{
	return Int4(lhs) + rhs;
}

inline RValue<Int4> operator+(RValue<Int4> lhs, RValue<Int> rhs)
{
	return lhs + Int4(rhs);
}

RValue<Int4> CmpEQ(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> CmpLT(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> CmpLE(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> CmpNEQ(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> CmpNLT(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> CmpNLE(RValue<Int4> x, RValue<Int4> y);
inline RValue<Int4> CmpGT(RValue<Int4> x, RValue<Int4> y)
{
	return CmpNLE(x, y);
}
inline RValue<Int4> CmpGE(RValue<Int4> x, RValue<Int4> y)
{
	return CmpNLT(x, y);
}
RValue<Int4> Max(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> Min(RValue<Int4> x, RValue<Int4> y);
RValue<Int4> RoundInt(RValue<Float4> cast);
RValue<Short8> PackSigned(RValue<Int4> x, RValue<Int4> y);
RValue<UShort8> PackUnsigned(RValue<Int4> x, RValue<Int4> y);
RValue<Int> Extract(RValue<Int4> val, int i);
RValue<Int4> Insert(RValue<Int4> val, RValue<Int> element, int i);
RValue<Int> SignMask(RValue<Int4> x);
RValue<Int4> Swizzle(RValue<Int4> x, uint16_t select);
RValue<Int4> Shuffle(RValue<Int4> x, RValue<Int4> y, uint16_t select);
RValue<Int4> MulHigh(RValue<Int4> x, RValue<Int4> y);

class UInt4 : public LValue<UInt4>, public XYZW<UInt4>
{
public:
	explicit UInt4(RValue<Float4> cast);

	UInt4();
	UInt4(int xyzw);
	UInt4(int x, int yzw);
	UInt4(int x, int y, int zw);
	UInt4(int x, int y, int z, int w);
	UInt4(RValue<UInt4> rhs);
	UInt4(const UInt4 &rhs);
	UInt4(const Reference<UInt4> &rhs);
	UInt4(RValue<Int4> rhs);
	UInt4(const Int4 &rhs);
	UInt4(const Reference<Int4> &rhs);
	UInt4(RValue<UInt2> lo, RValue<UInt2> hi);
	UInt4(RValue<UInt> rhs);
	UInt4(const UInt &rhs);
	UInt4(const Reference<UInt> &rhs);

	RValue<UInt4> operator=(RValue<UInt4> rhs);
	RValue<UInt4> operator=(const UInt4 &rhs);
	RValue<UInt4> operator=(const Reference<UInt4> &rhs);

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
RValue<UInt4> operator+=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator-=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator*=(UInt4 &lhs, RValue<UInt4> rhs);
//	RValue<UInt4> operator/=(UInt4 &lhs, RValue<UInt4> rhs);
//	RValue<UInt4> operator%=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator&=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator|=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator^=(UInt4 &lhs, RValue<UInt4> rhs);
RValue<UInt4> operator<<=(UInt4 &lhs, unsigned char rhs);
RValue<UInt4> operator>>=(UInt4 &lhs, unsigned char rhs);
RValue<UInt4> operator+(RValue<UInt4> val);
RValue<UInt4> operator-(RValue<UInt4> val);
RValue<UInt4> operator~(RValue<UInt4> val);
//	RValue<UInt4> operator++(UInt4 &val, int);   // Post-increment
//	const UInt4 &operator++(UInt4 &val);   // Pre-increment
//	RValue<UInt4> operator--(UInt4 &val, int);   // Post-decrement
//	const UInt4 &operator--(UInt4 &val);   // Pre-decrement
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
inline RValue<UInt4> CmpGT(RValue<UInt4> x, RValue<UInt4> y)
{
	return CmpNLE(x, y);
}
inline RValue<UInt4> CmpGE(RValue<UInt4> x, RValue<UInt4> y)
{
	return CmpNLT(x, y);
}
RValue<UInt4> Max(RValue<UInt4> x, RValue<UInt4> y);
RValue<UInt4> Min(RValue<UInt4> x, RValue<UInt4> y);
RValue<UInt4> MulHigh(RValue<UInt4> x, RValue<UInt4> y);
RValue<UInt> Extract(RValue<UInt4> val, int i);
RValue<UInt4> Insert(RValue<UInt4> val, RValue<UInt> element, int i);
//	RValue<UInt4> RoundInt(RValue<Float4> cast);
RValue<UInt4> Swizzle(RValue<UInt4> x, uint16_t select);
RValue<UInt4> Shuffle(RValue<UInt4> x, RValue<UInt4> y, uint16_t select);

class Half : public LValue<Half>
{
public:
	explicit Half(RValue<Float> cast);

	static Type *getType();
};

class Float : public LValue<Float>
{
public:
	explicit Float(RValue<Int> cast);
	explicit Float(RValue<UInt> cast);
	explicit Float(RValue<Half> cast);

	Float() = default;
	Float(float x);
	Float(RValue<Float> rhs);
	Float(const Float &rhs);
	Float(const Reference<Float> &rhs);
	Float(Argument<Float> argument);

	template<int T>
	Float(const SwizzleMask1<Float4, T> &rhs);

	//	RValue<Float> operator=(float rhs);   // FIXME: Implement
	RValue<Float> operator=(RValue<Float> rhs);
	RValue<Float> operator=(const Float &rhs);
	RValue<Float> operator=(const Reference<Float> &rhs);

	template<int T>
	RValue<Float> operator=(const SwizzleMask1<Float4, T> &rhs);

	static Float infinity();

	static Type *getType();
};

RValue<Float> operator+(RValue<Float> lhs, RValue<Float> rhs);
RValue<Float> operator-(RValue<Float> lhs, RValue<Float> rhs);
RValue<Float> operator*(RValue<Float> lhs, RValue<Float> rhs);
RValue<Float> operator/(RValue<Float> lhs, RValue<Float> rhs);
RValue<Float> operator+=(Float &lhs, RValue<Float> rhs);
RValue<Float> operator-=(Float &lhs, RValue<Float> rhs);
RValue<Float> operator*=(Float &lhs, RValue<Float> rhs);
RValue<Float> operator/=(Float &lhs, RValue<Float> rhs);
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

//	RValue<Int4> IsInf(RValue<Float> x);
//	RValue<Int4> IsNan(RValue<Float> x);
RValue<Float> Round(RValue<Float> x);
RValue<Float> Trunc(RValue<Float> x);
RValue<Float> Frac(RValue<Float> x);
RValue<Float> Floor(RValue<Float> x);
RValue<Float> Ceil(RValue<Float> x);

// Trigonometric functions
// TODO: Currently unimplemented for Subzero.
//	RValue<Float> Sin(RValue<Float> x);
//	RValue<Float> Cos(RValue<Float> x);
//	RValue<Float> Tan(RValue<Float> x);
//	RValue<Float> Asin(RValue<Float> x);
//	RValue<Float> Acos(RValue<Float> x);
//	RValue<Float> Atan(RValue<Float> x);
//	RValue<Float> Sinh(RValue<Float> x);
//	RValue<Float> Cosh(RValue<Float> x);
//	RValue<Float> Tanh(RValue<Float> x);
//	RValue<Float> Asinh(RValue<Float> x);
//	RValue<Float> Acosh(RValue<Float> x);
//	RValue<Float> Atanh(RValue<Float> x);
//	RValue<Float> Atan2(RValue<Float> x, RValue<Float> y);

// Exponential functions
// TODO: Currently unimplemented for Subzero.
//	RValue<Float> Pow(RValue<Float> x, RValue<Float> y);
//	RValue<Float> Exp(RValue<Float> x);
//	RValue<Float> Log(RValue<Float> x);
RValue<Float> Exp2(RValue<Float> x);
RValue<Float> Log2(RValue<Float> x);

class Float2 : public LValue<Float2>
{
public:
	//	explicit Float2(RValue<Byte2> cast);
	//	explicit Float2(RValue<Short2> cast);
	//	explicit Float2(RValue<UShort2> cast);
	//	explicit Float2(RValue<Int2> cast);
	//	explicit Float2(RValue<UInt2> cast);
	explicit Float2(RValue<Float4> cast);

	Float2() = default;
	//	Float2(float x, float y);
	//	Float2(RValue<Float2> rhs);
	//	Float2(const Float2 &rhs);
	//	Float2(const Reference<Float2> &rhs);
	//	Float2(RValue<Float> rhs);
	//	Float2(const Float &rhs);
	//	Float2(const Reference<Float> &rhs);

	//	template<int T>
	//	Float2(const SwizzleMask1<T> &rhs);

	//	RValue<Float2> operator=(float replicate);
	//	RValue<Float2> operator=(RValue<Float2> rhs);
	//	RValue<Float2> operator=(const Float2 &rhs);
	//	RValue<Float2> operator=(const Reference<Float2> &rhs);
	//	RValue<Float2> operator=(RValue<Float> rhs);
	//	RValue<Float2> operator=(const Float &rhs);
	//	RValue<Float2> operator=(const Reference<Float> &rhs);

	//	template<int T>
	//	RValue<Float2> operator=(const SwizzleMask1<T> &rhs);

	static Type *getType();
};

//	RValue<Float2> operator+(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator-(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator*(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator/(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator%(RValue<Float2> lhs, RValue<Float2> rhs);
//	RValue<Float2> operator+=(Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator-=(Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator*=(Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator/=(Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator%=(Float2 &lhs, RValue<Float2> rhs);
//	RValue<Float2> operator+(RValue<Float2> val);
//	RValue<Float2> operator-(RValue<Float2> val);

//	RValue<Float2> Abs(RValue<Float2> x);
//	RValue<Float2> Max(RValue<Float2> x, RValue<Float2> y);
//	RValue<Float2> Min(RValue<Float2> x, RValue<Float2> y);
//	RValue<Float2> Swizzle(RValue<Float2> x, uint16_t select);
//	RValue<Float2> Mask(Float2 &lhs, RValue<Float2> rhs, uint16_t select);

class Float4 : public LValue<Float4>, public XYZW<Float4>
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
	Float4(const SwizzleMask1<Float4, T> &rhs);
	template<int T>
	Float4(const Swizzle4<Float4, T> &rhs);
	template<int X, int Y>
	Float4(const Swizzle2<Float4, X> &x, const Swizzle2<Float4, Y> &y);
	template<int X, int Y>
	Float4(const SwizzleMask2<Float4, X> &x, const Swizzle2<Float4, Y> &y);
	template<int X, int Y>
	Float4(const Swizzle2<Float4, X> &x, const SwizzleMask2<Float4, Y> &y);
	template<int X, int Y>
	Float4(const SwizzleMask2<Float4, X> &x, const SwizzleMask2<Float4, Y> &y);

	RValue<Float4> operator=(float replicate);
	RValue<Float4> operator=(RValue<Float4> rhs);
	RValue<Float4> operator=(const Float4 &rhs);
	RValue<Float4> operator=(const Reference<Float4> &rhs);
	RValue<Float4> operator=(RValue<Float> rhs);
	RValue<Float4> operator=(const Float &rhs);
	RValue<Float4> operator=(const Reference<Float> &rhs);

	template<int T>
	RValue<Float4> operator=(const SwizzleMask1<Float4, T> &rhs);
	template<int T>
	RValue<Float4> operator=(const Swizzle4<Float4, T> &rhs);

	static Float4 infinity();

	static Type *getType();

private:
	void constant(float x, float y, float z, float w);
};

RValue<Float4> operator+(RValue<Float4> lhs, RValue<Float4> rhs);
RValue<Float4> operator-(RValue<Float4> lhs, RValue<Float4> rhs);
RValue<Float4> operator*(RValue<Float4> lhs, RValue<Float4> rhs);
RValue<Float4> operator/(RValue<Float4> lhs, RValue<Float4> rhs);
RValue<Float4> operator%(RValue<Float4> lhs, RValue<Float4> rhs);
RValue<Float4> operator+=(Float4 &lhs, RValue<Float4> rhs);
RValue<Float4> operator-=(Float4 &lhs, RValue<Float4> rhs);
RValue<Float4> operator*=(Float4 &lhs, RValue<Float4> rhs);
RValue<Float4> operator/=(Float4 &lhs, RValue<Float4> rhs);
RValue<Float4> operator%=(Float4 &lhs, RValue<Float4> rhs);
RValue<Float4> operator+(RValue<Float4> val);
RValue<Float4> operator-(RValue<Float4> val);

RValue<Float4> Abs(RValue<Float4> x);
RValue<Float4> Max(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Min(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Rcp_pp(RValue<Float4> val, bool exactAtPow2 = false);
RValue<Float4> RcpSqrt_pp(RValue<Float4> val);
RValue<Float4> Sqrt(RValue<Float4> x);
RValue<Float4> Insert(RValue<Float4> val, RValue<Float> element, int i);
RValue<Float> Extract(RValue<Float4> x, int i);
RValue<Float4> Swizzle(RValue<Float4> x, uint16_t select);
RValue<Float4> Shuffle(RValue<Float4> x, RValue<Float4> y, uint16_t select);
RValue<Float4> ShuffleLowHigh(RValue<Float4> x, RValue<Float4> y, uint16_t imm);
RValue<Float4> UnpackLow(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> UnpackHigh(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Mask(Float4 &lhs, RValue<Float4> rhs, uint16_t select);
RValue<Int> SignMask(RValue<Float4> x);

// Ordered comparison functions
RValue<Int4> CmpEQ(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpLT(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpLE(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpNEQ(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpNLT(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpNLE(RValue<Float4> x, RValue<Float4> y);
inline RValue<Int4> CmpGT(RValue<Float4> x, RValue<Float4> y)
{
	return CmpNLE(x, y);
}
inline RValue<Int4> CmpGE(RValue<Float4> x, RValue<Float4> y)
{
	return CmpNLT(x, y);
}

// Unordered comparison functions
RValue<Int4> CmpUEQ(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpULT(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpULE(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpUNEQ(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpUNLT(RValue<Float4> x, RValue<Float4> y);
RValue<Int4> CmpUNLE(RValue<Float4> x, RValue<Float4> y);
inline RValue<Int4> CmpUGT(RValue<Float4> x, RValue<Float4> y)
{
	return CmpUNLE(x, y);
}
inline RValue<Int4> CmpUGE(RValue<Float4> x, RValue<Float4> y)
{
	return CmpUNLT(x, y);
}

RValue<Int4> IsInf(RValue<Float4> x);
RValue<Int4> IsNan(RValue<Float4> x);
RValue<Float4> Round(RValue<Float4> x);
RValue<Float4> Trunc(RValue<Float4> x);
RValue<Float4> Frac(RValue<Float4> x);
RValue<Float4> Floor(RValue<Float4> x);
RValue<Float4> Ceil(RValue<Float4> x);

// Trigonometric functions
// TODO: Currently unimplemented for Subzero.
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

// Exponential functions
// TODO: Currently unimplemented for Subzero.
RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y);
RValue<Float4> Exp(RValue<Float4> x);
RValue<Float4> Log(RValue<Float4> x);
RValue<Float4> Exp2(RValue<Float4> x);
RValue<Float4> Log2(RValue<Float4> x);

// Bit Manipulation functions.
// TODO: Currently unimplemented for Subzero.

// Count leading zeros.
// Returns 32 when: !isZeroUndef && x == 0.
// Returns an undefined value when: isZeroUndef && x == 0.
RValue<UInt> Ctlz(RValue<UInt> x, bool isZeroUndef);
RValue<UInt4> Ctlz(RValue<UInt4> x, bool isZeroUndef);

// Count trailing zeros.
// Returns 32 when: !isZeroUndef && x == 0.
// Returns an undefined value when: isZeroUndef && x == 0.
RValue<UInt> Cttz(RValue<UInt> x, bool isZeroUndef);
RValue<UInt4> Cttz(RValue<UInt4> x, bool isZeroUndef);

template<class T>
class Pointer : public LValue<Pointer<T>>
{
public:
	template<class S>
	Pointer(RValue<Pointer<S>> pointerS, int alignment = 1)
	    : alignment(alignment)
	{
		Value *pointerT = Nucleus::createBitCast(pointerS.value, Nucleus::getPointerType(T::getType()));
		LValue<Pointer<T>>::storeValue(pointerT);
	}

	template<class S>
	Pointer(const Pointer<S> &pointer, int alignment = 1)
	    : alignment(alignment)
	{
		Value *pointerS = pointer.loadValue();
		Value *pointerT = Nucleus::createBitCast(pointerS, Nucleus::getPointerType(T::getType()));
		LValue<Pointer<T>>::storeValue(pointerT);
	}

	Pointer(Argument<Pointer<T>> argument);

	Pointer();
	Pointer(RValue<Pointer<T>> rhs);
	Pointer(const Pointer<T> &rhs);
	Pointer(const Reference<Pointer<T>> &rhs);
	Pointer(std::nullptr_t);

	RValue<Pointer<T>> operator=(RValue<Pointer<T>> rhs);
	RValue<Pointer<T>> operator=(const Pointer<T> &rhs);
	RValue<Pointer<T>> operator=(const Reference<Pointer<T>> &rhs);
	RValue<Pointer<T>> operator=(std::nullptr_t);

	Reference<T> operator*();
	Reference<T> operator[](int index);
	Reference<T> operator[](unsigned int index);
	Reference<T> operator[](RValue<Int> index);
	Reference<T> operator[](RValue<UInt> index);

	static Type *getType();

private:
	const int alignment;
};

RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, int offset);
RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<Int> offset);
RValue<Pointer<Byte>> operator+(RValue<Pointer<Byte>> lhs, RValue<UInt> offset);
RValue<Pointer<Byte>> operator+=(Pointer<Byte> &lhs, int offset);
RValue<Pointer<Byte>> operator+=(Pointer<Byte> &lhs, RValue<Int> offset);
RValue<Pointer<Byte>> operator+=(Pointer<Byte> &lhs, RValue<UInt> offset);

RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, int offset);
RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<Int> offset);
RValue<Pointer<Byte>> operator-(RValue<Pointer<Byte>> lhs, RValue<UInt> offset);
RValue<Pointer<Byte>> operator-=(Pointer<Byte> &lhs, int offset);
RValue<Pointer<Byte>> operator-=(Pointer<Byte> &lhs, RValue<Int> offset);
RValue<Pointer<Byte>> operator-=(Pointer<Byte> &lhs, RValue<UInt> offset);

template<typename T>
RValue<Bool> operator==(const Pointer<T> &lhs, const Pointer<T> &rhs)
{
	return RValue<Bool>(Nucleus::createPtrEQ(lhs.loadValue(), rhs.loadValue()));
}

template<typename T>
RValue<T> Load(RValue<Pointer<T>> pointer, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	return RValue<T>(Nucleus::createLoad(pointer.value, T::getType(), false, alignment, atomic, memoryOrder));
}

template<typename T>
RValue<T> Load(Pointer<T> pointer, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	return Load(RValue<Pointer<T>>(pointer), alignment, atomic, memoryOrder);
}

// TODO: Use SIMD to template these.
RValue<Float4> MaskedLoad(RValue<Pointer<Float4>> base, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
RValue<Int4> MaskedLoad(RValue<Pointer<Int4>> base, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
void MaskedStore(RValue<Pointer<Float4>> base, RValue<Float4> val, RValue<Int4> mask, unsigned int alignment);
void MaskedStore(RValue<Pointer<Int4>> base, RValue<Int4> val, RValue<Int4> mask, unsigned int alignment);

RValue<Float4> Gather(RValue<Pointer<Float>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
RValue<Int4> Gather(RValue<Pointer<Int>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes = false);
void Scatter(RValue<Pointer<Float>> base, RValue<Float4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment);
void Scatter(RValue<Pointer<Int>> base, RValue<Int4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment);

template<typename T>
void Store(RValue<T> value, RValue<Pointer<T>> pointer, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	Nucleus::createStore(value.value, pointer.value, T::getType(), false, alignment, atomic, memoryOrder);
}

template<typename T>
void Store(RValue<T> value, Pointer<T> pointer, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	Store(value, RValue<Pointer<T>>(pointer), alignment, atomic, memoryOrder);
}

template<typename T>
void Store(T value, Pointer<T> pointer, unsigned int alignment, bool atomic, std::memory_order memoryOrder)
{
	Store(RValue<T>(value), RValue<Pointer<T>>(pointer), alignment, atomic, memoryOrder);
}

// Fence adds a memory barrier that enforces ordering constraints on memory
// operations. memoryOrder can only be one of:
// std::memory_order_acquire, std::memory_order_release,
// std::memory_order_acq_rel, or std::memory_order_seq_cst.
void Fence(std::memory_order memoryOrder);

template<class T, int S = 1>
class Array : public LValue<T>
{
public:
	Array(int size = S);

	Reference<T> operator[](int index);
	Reference<T> operator[](unsigned int index);
	Reference<T> operator[](RValue<Int> index);
	Reference<T> operator[](RValue<UInt> index);

	// self() returns the this pointer to this Array object.
	// This function exists because operator&() is overloaded by LValue<T>.
	inline Array *self() { return this; }
};

//	RValue<Array<T>> operator++(Array<T> &val, int);   // Post-increment
//	const Array<T> &operator++(Array<T> &val);   // Pre-increment
//	RValue<Array<T>> operator--(Array<T> &val, int);   // Post-decrement
//	const Array<T> &operator--(Array<T> &val);   // Pre-decrement

void branch(RValue<Bool> cmp, BasicBlock *bodyBB, BasicBlock *endBB);

// ValueOf returns a rr::Value* for the given C-type, RValue<T>, LValue<T>
// or Reference<T>.
template<typename T>
inline Value *ValueOf(const T &v)
{
	return ReactorType<T>::cast(v).loadValue();
}

void Return();

template<class T>
void Return(const T &ret)
{
	static_assert(CanBeUsedAsReturn<ReactorTypeT<T>>::value, "Unsupported type for Return()");
	Nucleus::createRet(ValueOf<T>(ret));
	// Place any unreachable instructions in an unreferenced block.
	Nucleus::setInsertBlock(Nucleus::createBasicBlock());
}

// Generic template, leave undefined!
template<typename FunctionType>
class Function;

// Specialized for function types
template<typename Return, typename... Arguments>
class Function<Return(Arguments...)>
{
	// Static assert that the function signature is valid.
	static_assert(sizeof(AssertFunctionSignatureIsValid<Return(Arguments...)>) >= 0, "Invalid function signature");

public:
	Function();

	virtual ~Function();

	template<int index>
	Argument<typename std::tuple_element<index, std::tuple<Arguments...>>::type> Arg() const
	{
		Value *arg = Nucleus::getArgument(index);
		return Argument<typename std::tuple_element<index, std::tuple<Arguments...>>::type>(arg);
	}

	std::shared_ptr<Routine> operator()(const char *name, ...);
	std::shared_ptr<Routine> operator()(const Config::Edit &cfg, const char *name, ...);

protected:
	Nucleus *core;
	std::vector<Type *> arguments;
};

template<typename Return>
class Function<Return()> : public Function<Return(Void)>
{
};

// FunctionT accepts a C-style function type template argument, allowing it to return a type-safe RoutineT wrapper
template<typename FunctionType>
class FunctionT;

template<typename Return, typename... Arguments>
class FunctionT<Return(Arguments...)> : public Function<CToReactorT<Return>(CToReactorT<Arguments>...)>
{
public:
	// Type of base class
	using BaseType = Function<CToReactorT<Return>(CToReactorT<Arguments>...)>;

	// Function type, e.g. void(int,float)
	using CFunctionType = Return(Arguments...);

	// Reactor function type, e.g. Void(Int, Float)
	using ReactorFunctionType = CToReactorT<Return>(CToReactorT<Arguments>...);

	// Returned RoutineT type
	using RoutineType = RoutineT<CFunctionType>;

	// Hide base implementations of operator()

	RoutineType operator()(const char *name, ...)
	{
		return RoutineType(BaseType::operator()(name));
	}

	RoutineType operator()(const Config::Edit &cfg, const char *name, ...)
	{
		return RoutineType(BaseType::operator()(cfg, name));
	}
};

RValue<Long> Ticks();

}  // namespace rr

/* Inline implementations */

namespace rr {

template<class T>
LValue<T>::LValue(int arraySize)
    : Variable(T::getType(), arraySize)
{
#ifdef ENABLE_RR_DEBUG_INFO
	materialize();
#endif  // ENABLE_RR_DEBUG_INFO
}

inline void Variable::materialize() const
{
	if(!address)
	{
		address = Nucleus::allocateStackVariable(type, arraySize);
		RR_DEBUG_INFO_EMIT_VAR(address);

		if(rvalue)
		{
			storeValue(rvalue);
			rvalue = nullptr;
		}
	}
}

inline Value *Variable::loadValue() const
{
	if(rvalue)
	{
		return rvalue;
	}

	if(!address)
	{
		// TODO: Return undef instead.
		materialize();
	}

	return Nucleus::createLoad(address, type, false, 0);
}

inline Value *Variable::storeValue(Value *value) const
{
	if(address)
	{
		return Nucleus::createStore(value, address, type, false, 0);
	}

	rvalue = value;

	return value;
}

inline Value *Variable::getBaseAddress() const
{
	materialize();

	return address;
}

inline Value *Variable::getElementPointer(Value *index, bool unsignedIndex) const
{
	return Nucleus::createGEP(getBaseAddress(), type, index, unsignedIndex);
}

template<class T>
RValue<Pointer<T>> LValue<T>::operator&()
{
	return RValue<Pointer<T>>(getBaseAddress());
}

template<class T>
Reference<T>::Reference(Value *pointer, int alignment)
    : alignment(alignment)
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

#ifdef ENABLE_RR_DEBUG_INFO
template<class T>
RValue<T>::RValue(const RValue<T> &rvalue)
    : value(rvalue.value)
{
	RR_DEBUG_INFO_EMIT_VAR(value);
}
#endif  // ENABLE_RR_DEBUG_INFO

template<class T>
RValue<T>::RValue(Value *rvalue)
{
	assert(Nucleus::createBitCast(rvalue, T::getType()) == rvalue);  // Run-time type should match T, so bitcast is no-op.

	value = rvalue;
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class T>
RValue<T>::RValue(const T &lvalue)
{
	value = lvalue.loadValue();
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class T>
RValue<T>::RValue(typename BoolLiteral<T>::type i)
{
	value = Nucleus::createConstantBool(i);
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class T>
RValue<T>::RValue(typename IntLiteral<T>::type i)
{
	value = Nucleus::createConstantInt(i);
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class T>
RValue<T>::RValue(typename FloatLiteral<T>::type f)
{
	value = Nucleus::createConstantFloat(f);
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class T>
RValue<T>::RValue(const Reference<T> &ref)
{
	value = ref.loadValue();
	RR_DEBUG_INFO_EMIT_VAR(value);
}

template<class Vector4, int T>
Swizzle2<Vector4, T>::operator RValue<Vector4>() const
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = parent->loadValue();

	return Swizzle(RValue<Vector4>(vector), T);
}

template<class Vector4, int T>
Swizzle4<Vector4, T>::operator RValue<Vector4>() const
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = parent->loadValue();

	return Swizzle(RValue<Vector4>(vector), T);
}

template<class Vector4, int T>
SwizzleMask4<Vector4, T>::operator RValue<Vector4>() const
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = parent->loadValue();

	return Swizzle(RValue<Vector4>(vector), T);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask4<Vector4, T>::operator=(RValue<Vector4> rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Mask(*parent, rhs, T);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask4<Vector4, T>::operator=(RValue<typename Scalar<Vector4>::Type> rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Mask(*parent, Vector4(rhs), T);
}

template<class Vector4, int T>
SwizzleMask1<Vector4, T>::operator RValue<typename Scalar<Vector4>::Type>() const  // FIXME: Call a non-template function
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Extract(*parent, T & 0x3);
}

template<class Vector4, int T>
SwizzleMask1<Vector4, T>::operator RValue<Vector4>() const
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = parent->loadValue();

	return Swizzle(RValue<Vector4>(vector), T);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask1<Vector4, T>::operator=(float x)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return *parent = Insert(*parent, Float(x), T & 0x3);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask1<Vector4, T>::operator=(RValue<Vector4> rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Mask(*parent, Float4(rhs), T);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask1<Vector4, T>::operator=(RValue<typename Scalar<Vector4>::Type> rhs)  // FIXME: Call a non-template function
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return *parent = Insert(*parent, rhs, T & 0x3);
}

template<class Vector4, int T>
SwizzleMask2<Vector4, T>::operator RValue<Vector4>() const
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *vector = parent->loadValue();

	return Swizzle(RValue<Float4>(vector), T);
}

template<class Vector4, int T>
RValue<Vector4> SwizzleMask2<Vector4, T>::operator=(RValue<Vector4> rhs)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return Mask(*parent, Float4(rhs), T);
}

template<int T>
Float::Float(const SwizzleMask1<Float4, T> &rhs)
{
	*this = rhs.operator RValue<Float>();
}

template<int T>
RValue<Float> Float::operator=(const SwizzleMask1<Float4, T> &rhs)
{
	return *this = rhs.operator RValue<Float>();
}

template<int T>
Float4::Float4(const SwizzleMask1<Float4, T> &rhs)
    : XYZW(this)
{
	*this = rhs.operator RValue<Float4>();
}

template<int T>
Float4::Float4(const Swizzle4<Float4, T> &rhs)
    : XYZW(this)
{
	*this = rhs.operator RValue<Float4>();
}

template<int X, int Y>
Float4::Float4(const Swizzle2<Float4, X> &x, const Swizzle2<Float4, Y> &y)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	*this = ShuffleLowHigh(*x.parent, *y.parent, (uint16_t(X) & 0xFF00u) | (uint16_t(Y >> 8) & 0x00FFu));
}

template<int X, int Y>
Float4::Float4(const SwizzleMask2<Float4, X> &x, const Swizzle2<Float4, Y> &y)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	*this = ShuffleLowHigh(*x.parent, *y.parent, (uint16_t(X) & 0xFF00u) | (uint16_t(Y >> 8) & 0x00FFu));
}

template<int X, int Y>
Float4::Float4(const Swizzle2<Float4, X> &x, const SwizzleMask2<Float4, Y> &y)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	*this = ShuffleLowHigh(*x.parent, *y.parent, (uint16_t(X) & 0xFF00u) | (uint16_t(Y >> 8) & 0x00FFu));
}

template<int X, int Y>
Float4::Float4(const SwizzleMask2<Float4, X> &x, const SwizzleMask2<Float4, Y> &y)
    : XYZW(this)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	*this = ShuffleLowHigh(*x.parent, *y.parent, (uint16_t(X) & 0xFF00u) | (uint16_t(Y >> 8) & 0x00FFu));
}

template<int T>
RValue<Float4> Float4::operator=(const SwizzleMask1<Float4, T> &rhs)
{
	return *this = rhs.operator RValue<Float4>();
}

template<int T>
RValue<Float4> Float4::operator=(const Swizzle4<Float4, T> &rhs)
{
	return *this = rhs.operator RValue<Float4>();
}

// Returns a reactor pointer to the fixed-address ptr.
RValue<Pointer<Byte>> ConstantPointer(void const *ptr);

// Returns a reactor pointer to an immutable copy of the data of size bytes.
RValue<Pointer<Byte>> ConstantData(void const *data, size_t size);

template<class T>
Pointer<T>::Pointer(Argument<Pointer<T>> argument)
    : alignment(1)
{
	LValue<Pointer<T>>::storeValue(argument.value);
}

template<class T>
Pointer<T>::Pointer()
    : alignment(1)
{}

template<class T>
Pointer<T>::Pointer(RValue<Pointer<T>> rhs)
    : alignment(1)
{
	LValue<Pointer<T>>::storeValue(rhs.value);
}

template<class T>
Pointer<T>::Pointer(const Pointer<T> &rhs)
    : alignment(rhs.alignment)
{
	Value *value = rhs.loadValue();
	LValue<Pointer<T>>::storeValue(value);
}

template<class T>
Pointer<T>::Pointer(const Reference<Pointer<T>> &rhs)
    : alignment(rhs.getAlignment())
{
	Value *value = rhs.loadValue();
	LValue<Pointer<T>>::storeValue(value);
}

template<class T>
Pointer<T>::Pointer(std::nullptr_t)
    : alignment(1)
{
	Value *value = Nucleus::createNullPointer(T::getType());
	LValue<Pointer<T>>::storeValue(value);
}

template<class T>
RValue<Pointer<T>> Pointer<T>::operator=(RValue<Pointer<T>> rhs)
{
	LValue<Pointer<T>>::storeValue(rhs.value);

	return rhs;
}

template<class T>
RValue<Pointer<T>> Pointer<T>::operator=(const Pointer<T> &rhs)
{
	Value *value = rhs.loadValue();
	LValue<Pointer<T>>::storeValue(value);

	return RValue<Pointer<T>>(value);
}

template<class T>
RValue<Pointer<T>> Pointer<T>::operator=(const Reference<Pointer<T>> &rhs)
{
	Value *value = rhs.loadValue();
	LValue<Pointer<T>>::storeValue(value);

	return RValue<Pointer<T>>(value);
}

template<class T>
RValue<Pointer<T>> Pointer<T>::operator=(std::nullptr_t)
{
	Value *value = Nucleus::createNullPointer(T::getType());
	LValue<Pointer<T>>::storeValue(value);

	return RValue<Pointer<T>>(this);
}

template<class T>
Reference<T> Pointer<T>::operator*()
{
	return Reference<T>(LValue<Pointer<T>>::loadValue(), alignment);
}

template<class T>
Reference<T> Pointer<T>::operator[](int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), Nucleus::createConstantInt(index), false);

	return Reference<T>(element, alignment);
}

template<class T>
Reference<T> Pointer<T>::operator[](unsigned int index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), Nucleus::createConstantInt(index), true);

	return Reference<T>(element, alignment);
}

template<class T>
Reference<T> Pointer<T>::operator[](RValue<Int> index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), index.value, false);

	return Reference<T>(element, alignment);
}

template<class T>
Reference<T> Pointer<T>::operator[](RValue<UInt> index)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *element = Nucleus::createGEP(LValue<Pointer<T>>::loadValue(), T::getType(), index.value, true);

	return Reference<T>(element, alignment);
}

template<class T>
Type *Pointer<T>::getType()
{
	return Nucleus::getPointerType(T::getType());
}

template<class T, int S>
Array<T, S>::Array(int size)
    : LValue<T>(size)
{
}

template<class T, int S>
Reference<T> Array<T, S>::operator[](int index)
{
	assert(index < this->arraySize);
	Value *element = LValue<T>::getElementPointer(Nucleus::createConstantInt(index), false);

	return Reference<T>(element);
}

template<class T, int S>
Reference<T> Array<T, S>::operator[](unsigned int index)
{
	assert(index < static_cast<unsigned int>(this->arraySize));
	Value *element = LValue<T>::getElementPointer(Nucleus::createConstantInt(index), true);

	return Reference<T>(element);
}

template<class T, int S>
Reference<T> Array<T, S>::operator[](RValue<Int> index)
{
	Value *element = LValue<T>::getElementPointer(index.value, false);

	return Reference<T>(element);
}

template<class T, int S>
Reference<T> Array<T, S>::operator[](RValue<UInt> index)
{
	Value *element = LValue<T>::getElementPointer(index.value, true);

	return Reference<T>(element);
}

//	template<class T>
//	RValue<Array<T>> operator++(Array<T> &val, int)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	const Array<T> &operator++(Array<T> &val)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	RValue<Array<T>> operator--(Array<T> &val, int)
//	{
//		// FIXME: Requires storing the address of the array
//	}

//	template<class T>
//	const Array<T> &operator--(Array<T> &val)
//	{
//		// FIXME: Requires storing the address of the array
//	}

template<class T>
RValue<T> IfThenElse(RValue<Bool> condition, RValue<T> ifTrue, RValue<T> ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<T>(Nucleus::createSelect(condition.value, ifTrue.value, ifFalse.value));
}

template<class T>
RValue<T> IfThenElse(RValue<Bool> condition, const T &ifTrue, RValue<T> ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *trueValue = ifTrue.loadValue();

	return RValue<T>(Nucleus::createSelect(condition.value, trueValue, ifFalse.value));
}

template<class T>
RValue<T> IfThenElse(RValue<Bool> condition, RValue<T> ifTrue, const T &ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *falseValue = ifFalse.loadValue();

	return RValue<T>(Nucleus::createSelect(condition.value, ifTrue.value, falseValue));
}

template<class T>
RValue<T> IfThenElse(RValue<Bool> condition, const T &ifTrue, const T &ifFalse)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *trueValue = ifTrue.loadValue();
	Value *falseValue = ifFalse.loadValue();

	return RValue<T>(Nucleus::createSelect(condition.value, trueValue, falseValue));
}

template<typename Return, typename... Arguments>
Function<Return(Arguments...)>::Function()
{
	core = new Nucleus();

	Type *types[] = { Arguments::getType()... };
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
std::shared_ptr<Routine> Function<Return(Arguments...)>::operator()(const char *name, ...)
{
	char fullName[1024 + 1];

	va_list vararg;
	va_start(vararg, name);
	vsnprintf(fullName, 1024, name, vararg);
	va_end(vararg);

	return core->acquireRoutine(fullName, Config::Edit::None);
}

template<typename Return, typename... Arguments>
std::shared_ptr<Routine> Function<Return(Arguments...)>::operator()(const Config::Edit &cfg, const char *name, ...)
{
	char fullName[1024 + 1];

	va_list vararg;
	va_start(vararg, name);
	vsnprintf(fullName, 1024, name, vararg);
	va_end(vararg);

	return core->acquireRoutine(fullName, cfg);
}

template<class T, class S>
RValue<T> ReinterpretCast(RValue<S> val)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<T>(Nucleus::createBitCast(val.value, T::getType()));
}

template<class T, class S>
RValue<T> ReinterpretCast(const LValue<S> &var)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	Value *val = var.loadValue();

	return RValue<T>(Nucleus::createBitCast(val, T::getType()));
}

template<class T, class S>
RValue<T> ReinterpretCast(const Reference<S> &var)
{
	return ReinterpretCast<T>(RValue<S>(var));
}

template<class T>
RValue<T> As(Value *val)
{
	RR_DEBUG_INFO_UPDATE_LOC();
	return RValue<T>(Nucleus::createBitCast(val, T::getType()));
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

// Calls the function pointer fptr with the given arguments, return type
// and parameter types. Returns the call's return value if the function has
// a non-void return type.
Value *Call(RValue<Pointer<Byte>> fptr, Type *retTy, std::initializer_list<Value *> args, std::initializer_list<Type *> paramTys);

template<typename F>
class CallHelper
{};

template<typename Return, typename... Arguments>
class CallHelper<Return(Arguments...)>
{
public:
	using RReturn = CToReactorT<Return>;

	static inline RReturn Call(Return(fptr)(Arguments...), CToReactorT<Arguments>... args)
	{
		return RValue<RReturn>(rr::Call(
		    ConstantPointer(reinterpret_cast<void *>(fptr)),
		    RReturn::getType(),
		    { ValueOf(args)... },
		    { CToReactorT<Arguments>::getType()... }));
	}

	static inline RReturn Call(Pointer<Byte> fptr, CToReactorT<Arguments>... args)
	{
		return RValue<RReturn>(rr::Call(
		    fptr,
		    RReturn::getType(),
		    { ValueOf(args)... },
		    { CToReactorT<Arguments>::getType()... }));
	}
};

template<typename... Arguments>
class CallHelper<void(Arguments...)>
{
public:
	static inline void Call(void(fptr)(Arguments...), CToReactorT<Arguments>... args)
	{
		rr::Call(ConstantPointer(reinterpret_cast<void *>(fptr)),
		         Void::getType(),
		         { ValueOf(args)... },
		         { CToReactorT<Arguments>::getType()... });
	}

	static inline void Call(Pointer<Byte> fptr, CToReactorT<Arguments>... args)
	{
		rr::Call(fptr,
		         Void::getType(),
		         { ValueOf(args)... },
		         { CToReactorT<Arguments>::getType()... });
	}
};

template<typename T>
inline ReactorTypeT<T> CastToReactor(const T &v)
{
	return ReactorType<T>::cast(v);
}

// Calls the static function pointer fptr with the given arguments args.
template<typename Return, typename... CArgs, typename... RArgs>
inline CToReactorT<Return> Call(Return(fptr)(CArgs...), RArgs &&... args)
{
	return CallHelper<Return(CArgs...)>::Call(fptr, CastToReactor(std::forward<RArgs>(args))...);
}

// Calls the static function pointer fptr with the given arguments args.
// Overload for calling functions with void return type.
template<typename... CArgs, typename... RArgs>
inline void Call(void(fptr)(CArgs...), RArgs &&... args)
{
	CallHelper<void(CArgs...)>::Call(fptr, CastToReactor(std::forward<RArgs>(args))...);
}

// Calls the member function pointer fptr with the given arguments args.
// object can be a Class*, or a Pointer<Byte>.
template<typename Return, typename Class, typename C, typename... CArgs, typename... RArgs>
inline CToReactorT<Return> Call(Return (Class::*fptr)(CArgs...), C &&object, RArgs &&... args)
{
	using Helper = CallHelper<Return(Class *, void *, CArgs...)>;
	using fptrTy = decltype(fptr);

	struct Static
	{
		static inline Return Call(Class *object, void *fptrptr, CArgs... args)
		{
			auto fptr = *reinterpret_cast<fptrTy *>(fptrptr);
			return (object->*fptr)(std::forward<CArgs>(args)...);
		}
	};

	return Helper::Call(&Static::Call,
	                    CastToReactor(object),
	                    ConstantData(&fptr, sizeof(fptr)),
	                    CastToReactor(std::forward<RArgs>(args))...);
}

// Calls the member function pointer fptr with the given arguments args.
// Overload for calling functions with void return type.
// object can be a Class*, or a Pointer<Byte>.
template<typename Class, typename C, typename... CArgs, typename... RArgs>
inline void Call(void (Class::*fptr)(CArgs...), C &&object, RArgs &&... args)
{
	using Helper = CallHelper<void(Class *, void *, CArgs...)>;
	using fptrTy = decltype(fptr);

	struct Static
	{
		static inline void Call(Class *object, void *fptrptr, CArgs... args)
		{
			auto fptr = *reinterpret_cast<fptrTy *>(fptrptr);
			(object->*fptr)(std::forward<CArgs>(args)...);
		}
	};

	Helper::Call(&Static::Call,
	             CastToReactor(object),
	             ConstantData(&fptr, sizeof(fptr)),
	             CastToReactor(std::forward<RArgs>(args))...);
}

// Calls the Reactor function pointer fptr with the signature
// FUNCTION_SIGNATURE and arguments.
template<typename FUNCTION_SIGNATURE, typename... RArgs>
inline void Call(Pointer<Byte> fptr, RArgs &&... args)
{
	CallHelper<FUNCTION_SIGNATURE>::Call(fptr, CastToReactor(std::forward<RArgs>(args))...);
}

// Breakpoint emits an instruction that will cause the application to trap.
// This can be used to stop an attached debugger at the given call.
void Breakpoint();

class ForData
{
public:
	ForData(bool init)
	    : loopOnce(init)
	{
	}

	operator bool()
	{
		return loopOnce;
	}

	bool operator=(bool value)
	{
		return loopOnce = value;
	}

	bool setup()
	{
		RR_DEBUG_INFO_FLUSH();
		if(Nucleus::getInsertBlock() != endBB)
		{
			testBB = Nucleus::createBasicBlock();

			Nucleus::createBr(testBB);
			Nucleus::setInsertBlock(testBB);

			return true;
		}

		return false;
	}

	bool test(RValue<Bool> cmp)
	{
		BasicBlock *bodyBB = Nucleus::createBasicBlock();
		endBB = Nucleus::createBasicBlock();

		Nucleus::createCondBr(cmp.value, bodyBB, endBB);
		Nucleus::setInsertBlock(bodyBB);

		return true;
	}

	void end()
	{
		Nucleus::createBr(testBB);
		Nucleus::setInsertBlock(endBB);
	}

private:
	BasicBlock *testBB = nullptr;
	BasicBlock *endBB = nullptr;
	bool loopOnce = true;
};

class IfElseData
{
public:
	IfElseData(RValue<Bool> cmp)
	    : iteration(0)
	{
		condition = cmp.value;

		beginBB = Nucleus::getInsertBlock();
		trueBB = Nucleus::createBasicBlock();
		falseBB = nullptr;
		endBB = Nucleus::createBasicBlock();

		Nucleus::setInsertBlock(trueBB);
	}

	~IfElseData()
	{
		Nucleus::createBr(endBB);

		Nucleus::setInsertBlock(beginBB);
		Nucleus::createCondBr(condition, trueBB, falseBB ? falseBB : endBB);

		Nucleus::setInsertBlock(endBB);
	}

	operator int()
	{
		return iteration;
	}

	IfElseData &operator++()
	{
		++iteration;

		return *this;
	}

	void elseClause()
	{
		Nucleus::createBr(endBB);

		falseBB = Nucleus::createBasicBlock();
		Nucleus::setInsertBlock(falseBB);
	}

private:
	Value *condition;
	BasicBlock *beginBB;
	BasicBlock *trueBB;
	BasicBlock *falseBB;
	BasicBlock *endBB;
	int iteration;
};

#define For(init, cond, inc)                        \
	for(ForData for__ = true; for__; for__ = false) \
		for(init; for__.setup() && for__.test(cond); inc, for__.end())

#define While(cond) For((void)0, cond, (void)0)

#define Do                                                \
	{                                                     \
		BasicBlock *body__ = Nucleus::createBasicBlock(); \
		Nucleus::createBr(body__);                        \
		Nucleus::setInsertBlock(body__);

#define Until(cond)                                     \
	BasicBlock *end__ = Nucleus::createBasicBlock();    \
	Nucleus::createCondBr((cond).value, end__, body__); \
	Nucleus::setInsertBlock(end__);                     \
	}                                                   \
	do                                                  \
	{                                                   \
	} while(false)  // Require a semi-colon at the end of the Until()

enum
{
	IF_BLOCK__,
	ELSE_CLAUSE__,
	ELSE_BLOCK__,
	IFELSE_NUM__
};

#define If(cond)                                                        \
	for(IfElseData ifElse__(cond); ifElse__ < IFELSE_NUM__; ++ifElse__) \
		if(ifElse__ == IF_BLOCK__)

#define Else                           \
	else if(ifElse__ == ELSE_CLAUSE__) \
	{                                  \
		ifElse__.elseClause();         \
	}                                  \
	else  // ELSE_BLOCK__

}  // namespace rr

#include "Traits.inl"

#endif  // rr_Reactor_hpp
