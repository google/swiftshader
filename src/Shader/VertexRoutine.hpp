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
		Pointer<Byte> constants;

		Int clipFlags;

		RegisterArray<16> v;   // Varying registers
		RegisterArray<12> o;   // Output registers

		const VertexProcessor::State &state;

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
