// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_VertexRoutine_hpp
#define sw_VertexRoutine_hpp

#include "Renderer/Color.hpp"
#include "Renderer/VertexProcessor.hpp"
#include "ShaderCore.hpp"
#include "VertexShader.hpp"

namespace sw
{
	class VertexRoutinePrototype : public Function<Void(Pointer<Byte>, Pointer<Byte>, Pointer<Byte>, Pointer<Byte>)>
	{
	public:
		VertexRoutinePrototype() : vertex(Arg<0>()), batch(Arg<1>()), task(Arg<2>()), data(Arg<3>()) {}
		virtual ~VertexRoutinePrototype() {};

	protected:
		const Pointer<Byte> vertex;
		const Pointer<Byte> batch;
		const Pointer<Byte> task;
		const Pointer<Byte> data;
	};

	class VertexRoutine : public VertexRoutinePrototype
	{
	public:
		VertexRoutine(const VertexProcessor::State &state, const VertexShader *shader);
		virtual ~VertexRoutine();

		void generate();

	protected:
		struct Registers
		{
			Registers(const VertexShader *shader) :
				v(shader && shader->dynamicallyIndexedInput),
				r(shader && shader->dynamicallyIndexedTemporaries),
				o(shader && shader->dynamicallyIndexedOutput)
			{
				loopDepth = -1;
				enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

				if(shader && shader->containsBreakInstruction())
				{
					enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}

				if(shader && shader->containsContinueInstruction())
				{
					enableContinue = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				}
			}

			Pointer<Byte> data;
			Pointer<Byte> constants;

			Int clipFlags;

			RegisterArray<16> v;
			RegisterArray<4096> r;
			RegisterArray<12> o;
			Vector4f a0;
			Array<Int, 4> aL;
			Vector4f p0;

			Array<Int, 4> increment;
			Array<Int, 4> iteration;

			Int loopDepth;
			Int stackIndex;   // FIXME: Inc/decrement callStack
			Array<UInt, 16> callStack;

			Int enableIndex;
			Array<Int4, 1 + 24> enableStack;
			Int4 enableBreak;
			Int4 enableContinue;
			Int4 enableLeave;

			Int instanceID;
		};

		Registers r;

		const VertexProcessor::State &state;
		const VertexShader *const shader;

	private:
		virtual void pipeline() = 0;

		typedef VertexProcessor::State::Input Stream;

		Vector4f readStream(Pointer<Byte> &buffer, UInt &stride, const Stream &stream, const UInt &index);
		void readInput(UInt &index);
		void computeClipFlags();
		void postTransform();
		void writeCache(Pointer<Byte> &cacheLine);
		void writeVertex(const Pointer<Byte> &vertex, Pointer<Byte> &cacheLine);
	};
}

#endif   // sw_VertexRoutine_hpp
