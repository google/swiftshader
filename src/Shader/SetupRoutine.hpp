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

#ifndef sw_SetupRoutine_hpp
#define sw_SetupRoutine_hpp

#include "SetupProcessor.hpp"
#include "Reactor/Nucleus.hpp"

namespace sw
{
	class Context;

	class SetupRoutine
	{
	public:
		SetupRoutine(const SetupProcessor::State &state);

		virtual ~SetupRoutine();

		void generate();
		Routine *getRoutine();

	private:
		void setupGradient(Pointer<Byte> &primitive, Pointer<Byte> &triangle, Float4 &w012, Float4 (&m)[3], Pointer<Byte> &v0, Pointer<Byte> &v1, Pointer<Byte> &v2, int attribute, int planeEquation, bool flatShading, bool sprite, bool perspective, bool wrap, int component);
		void edge(Pointer<Byte> &primitive, Int &X1, Int &Y1, Int &X2, Int &Y2, Int &q);
		void conditionalRotate1(Bool condition, Pointer<Byte> &v0, Pointer<Byte> &v1, Pointer<Byte> &v2);
		void conditionalRotate2(Bool condition, Pointer<Byte> &v0, Pointer<Byte> &v1, Pointer<Byte> &v2);

		const SetupProcessor::State &state;

		Routine *routine;
	};
}

#endif   // sw_SetupRoutine_hpp
