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

#ifndef sw_VertexRoutine_hpp
#define sw_VertexRoutine_hpp

#include "Renderer/Color.hpp"
#include "Renderer/VertexProcessor.hpp"
#include "Reactor/Reactor.hpp"

namespace sw
{
	class VertexRoutine
	{
	protected:
		struct Registers
		{
			Registers() : callStack(4), aL(4), increment(4), iteration(4), enableStack(1 + 24), ox(12), oy(12), oz(12), ow(12)
			{
				loopDepth = -1;
				enableStack[0] = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
			}

			Pointer<Byte> data;
			Pointer<Byte> constants;

			Array<Float4> ox;
			Array<Float4> oy;
			Array<Float4> oz;
			Array<Float4> ow;

			Int clipFlags;

			Color4f v[16];
			Color4f r[32];
			Color4f a0;
			Array<Int> aL;
			Color4f p0;

			Array<Int> increment;
			Array<Int> iteration;

			Int loopDepth;
			Int stackIndex;   // FIXME: Inc/decrement callStack
			Array<UInt> callStack;

			Int enableIndex;
			Array<Int4> enableStack;
			Int4 enableBreak;
		};

	public:
		VertexRoutine(const VertexProcessor::State &state);

		virtual ~VertexRoutine();

		void generate();
		Routine *getRoutine();

	protected:
		const VertexProcessor::State &state;

	private:		
		virtual void pipeline(Registers &r) = 0;

		typedef VertexProcessor::State::Input Stream;
		
		Color4f readStream(Registers &r, Pointer<Byte> &buffer, UInt &stride, const Stream &stream, const UInt &index);
		void readInput(Registers &r, UInt &index);
		void computeClipFlags(Registers &r);
		void postTransform(Registers &r);
		void writeCache(Pointer<Byte> &cacheLine, Registers &r);
		void writeVertex(Pointer<Byte> &vertex, Pointer<Byte> &cacheLine);

		Routine *routine;
	};
}

#endif   // sw_VertexRoutine_hpp
