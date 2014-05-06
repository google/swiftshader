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

#include "Nucleus.hpp"

namespace sw
{
	namespace x86
	{
		RValue<Int> cvtss2si(const RValue<Float> &val);
		RValue<Int2> cvtps2pi(const RValue<Float4> &val);
		RValue<Int2> cvttps2pi(const RValue<Float4> &val);
		RValue<Int4> cvtps2dq(const RValue<Float4> &val);
		RValue<Int4> cvttps2dq(const RValue<Float4> &val);
		
		RValue<Float4> cvtpi2ps(const RValue<Float4> &x, const RValue<Int2> &y);
		RValue<Float4> cvtdq2ps(const RValue<Int4> &val);

		RValue<Float> rcpss(const RValue<Float> &val);
		RValue<Float> sqrtss(const RValue<Float> &val);
		RValue<Float> rsqrtss(const RValue<Float> &val);

		RValue<Float4> rcpps(const RValue<Float4> &val);
		RValue<Float4> sqrtps(const RValue<Float4> &val);
		RValue<Float4> rsqrtps(const RValue<Float4> &val);
		RValue<Float4> maxps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> minps(const RValue<Float4> &x, const RValue<Float4> &y);

		RValue<Float> roundss(const RValue<Float> &val, unsigned char imm);
		RValue<Float> floorss(const RValue<Float> &val);
		RValue<Float> ceilss(const RValue<Float> &val);

		RValue<Float4> roundps(const RValue<Float4> &val, unsigned char imm);
		RValue<Float4> floorps(const RValue<Float4> &val);
		RValue<Float4> ceilps(const RValue<Float4> &val);

		RValue<Float4> cmpps(const RValue<Float4> &x, const RValue<Float4> &y, unsigned char imm);
		RValue<Float4> cmpeqps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpltps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpleps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpunordps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpneqps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpnltps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpnleps(const RValue<Float4> &x, const RValue<Float4> &y);
		RValue<Float4> cmpordps(const RValue<Float4> &x, const RValue<Float4> &y);

		RValue<Float> cmpss(const RValue<Float> &x, const RValue<Float> &y, unsigned char imm);
		RValue<Float> cmpeqss(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpltss(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpless(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpunordss(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpneqss(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpnltss(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpnless(const RValue<Float> &x, const RValue<Float> &y);
		RValue<Float> cmpordss(const RValue<Float> &x, const RValue<Float> &y);

		RValue<Int4> pabsd(const RValue<Int4> &x, const RValue<Int4> &y);

		RValue<Short4> paddsw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<Short4> psubsw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<UShort4> paddusw(const RValue<UShort4> &x, const RValue<UShort4> &y);
		RValue<UShort4> psubusw(const RValue<UShort4> &x, const RValue<UShort4> &y);
		RValue<SByte8> paddsb(const RValue<SByte8> &x, const RValue<SByte8> &y);
		RValue<SByte8> psubsb(const RValue<SByte8> &x, const RValue<SByte8> &y);
		RValue<Byte8> paddusb(const RValue<Byte8> &x, const RValue<Byte8> &y);
		RValue<Byte8> psubusb(const RValue<Byte8> &x, const RValue<Byte8> &y);

		RValue<UShort4> pavgw(const RValue<UShort4> &x, const RValue<UShort4> &y);

		RValue<Short4> pmaxsw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<Short4> pminsw(const RValue<Short4> &x, const RValue<Short4> &y);

		RValue<Short4> pcmpgtw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<Short4> pcmpeqw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<Byte8> pcmpgtb(const RValue<SByte8> &x, const RValue<SByte8> &y);
		RValue<Byte8> pcmpeqb(const RValue<Byte8> &x, const RValue<Byte8> &y);

		RValue<Short4> packssdw(const RValue<Int2> &x, const RValue<Int2> &y);
		RValue<Short8> packssdw(const RValue<Int4> &x, const RValue<Int4> &y);
		RValue<SByte8> packsswb(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<Byte8> packuswb(const RValue<UShort4> &x, const RValue<UShort4> &y);

		RValue<UShort8> packusdw(const RValue<UInt4> &x, const RValue<UInt4> &y);

		RValue<UShort4> psrlw(const RValue<UShort4> &x, unsigned char y);
		RValue<UShort8> psrlw(const RValue<UShort8> &x, unsigned char y);
		RValue<Short4> psraw(const RValue<Short4> &x, unsigned char y);
		RValue<Short8> psraw(const RValue<Short8> &x, unsigned char y);
		RValue<Short4> psllw(const RValue<Short4> &x, unsigned char y);
		RValue<Short8> psllw(const RValue<Short8> &x, unsigned char y);
		RValue<Int2> pslld(const RValue<Int2> &x, unsigned char y);
		RValue<Int4> pslld(const RValue<Int4> &x, unsigned char y);
		RValue<Int2> psrad(const RValue<Int2> &x, unsigned char y);
		RValue<Int4> psrad(const RValue<Int4> &x, unsigned char y);
		RValue<UInt2> psrld(const RValue<UInt2> &x, unsigned char y);
		RValue<UInt4> psrld(const RValue<UInt4> &x, unsigned char y);

		RValue<UShort4> psrlw(const RValue<UShort4> &x, const RValue<Long1> &y);
		RValue<Short4> psraw(const RValue<Short4> &x, const RValue<Long1> &y);
		RValue<Short4> psllw(const RValue<Short4> &x, const RValue<Long1> &y);
		RValue<Int2> pslld(const RValue<Int2> &x, const RValue<Long1> &y);
		RValue<UInt2> psrld(const RValue<UInt2> &x, const RValue<Long1> &y);
		RValue<Int2> psrad(const RValue<Int2> &x, const RValue<Long1> &y);

		RValue<Short4> pmulhw(const RValue<Short4> &x, const RValue<Short4> &y);
		RValue<UShort4> pmulhuw(const RValue<UShort4> &x, const RValue<UShort4> &y);
		RValue<Int2> pmaddwd(const RValue<Short4> &x, const RValue<Short4> &y);

		RValue<Short8> pmulhw(const RValue<Short8> &x, const RValue<Short8> &y);
		RValue<UShort8> pmulhuw(const RValue<UShort8> &x, const RValue<UShort8> &y);
		RValue<Int4> pmaddwd(const RValue<Short8> &x, const RValue<Short8> &y);

		RValue<Int> movmskps(const RValue<Float4> &x);
		RValue<Int> pmovmskb(const RValue<Byte8> &x);

		RValue<Int4> pmovzxbd(const RValue<Int4> &x);
		RValue<Int4> pmovsxbd(const RValue<Int4> &x);
		RValue<Int4> pmovzxwd(const RValue<Int4> &x);
		RValue<Int4> pmovsxwd(const RValue<Int4> &x);

		void emms();
	}
}
