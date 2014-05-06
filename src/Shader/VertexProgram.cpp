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

#include "VertexProgram.hpp"

#include "Renderer.hpp"
#include "VertexShader.hpp"
#include "Vertex.hpp"
#include "Half.hpp"
#include "SamplerCore.hpp"
#include "Debug.hpp"

extern bool localShaderConstants;

namespace sw
{
	VertexProgram::VertexProgram(const VertexProcessor::State &state, const VertexShader *vertexShader) : VertexRoutine(state), vertexShader(vertexShader)
	{
		returns = false;
		ifDepth = 0;
		loopRepDepth = 0;
		breakDepth = 0;

		for(int i = 0; i < 2048; i++)
		{
			labelBlock[i] = 0;
		}
	}

	VertexProgram::~VertexProgram()
	{
		for(int i = 0; i < 4; i++)
		{
			delete sampler[i];
		}
	}

	void VertexProgram::pipeline(Registers &r)
	{
		for(int i = 0; i < 4; i++)
		{
			sampler[i] = new SamplerCore(r.constants, state.samplerState[i]);
		}

		if(!state.preTransformed)
		{
			shader(r);
		}
		else
		{
			passThrough(r);
		}
	}

	Color4f VertexProgram::readConstant(Registers &r, const Src &src, int offset)
	{
		Color4f c;

		int i = src.index + offset;
		bool relative = src.relative;

		if(!relative)
		{
			c.r = c.g = c.b = c.a = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c[i]));

			c.r = c.r.xxxx;
			c.g = c.g.yyyy;
			c.b = c.b.zzzz;
			c.a = c.a.wwww;

			if(localShaderConstants)   // Constant may be known at compile time
			{
				for(int j = 0; j < vertexShader->getLength(); j++)
				{
					const ShaderInstruction &instruction = *vertexShader->getInstruction(j);

					if(instruction.getOpcode() == ShaderOperation::OPCODE_DEF)
					{
						if(instruction.getDestinationParameter().index == i)
						{
							c.r = Float4(instruction.getSourceParameter(0).value);
							c.g = Float4(instruction.getSourceParameter(1).value);
							c.b = Float4(instruction.getSourceParameter(2).value);
							c.a = Float4(instruction.getSourceParameter(3).value);

							break;
						}
					}
				}
			}
		}
		else if(src.relativeType == Src::PARAMETER_LOOP)
		{
			Int loopCounter = r.aL[r.loopDepth];

			c.r = c.g = c.b = c.a = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c[i]) + loopCounter * 16);

			c.r = c.r.xxxx;
			c.g = c.g.yyyy;
			c.b = c.b.zzzz;
			c.a = c.a.wwww;
		}
		else
		{
			Int index0;
			Int index1;
			Int index2;
			Int index3;

			Float4 a0_;

			switch(src.relativeSwizzle & 0x03)
			{
			case 0: a0_ = r.a0.x; break;
			case 1: a0_ = r.a0.y; break;
			case 2: a0_ = r.a0.z; break;
			case 3: a0_ = r.a0.w; break;
			}

			index0 = i + RoundInt(Float(a0_.x));
			index1 = i + RoundInt(Float(a0_.y));
			index2 = i + RoundInt(Float(a0_.z));
			index3 = i + RoundInt(Float(a0_.w));

			// Clamp to constant register range, c[256] = {0, 0, 0, 0}
			index0 = IfThenElse(UInt(index0) > UInt(256), Int(256), index0);
			index1 = IfThenElse(UInt(index1) > UInt(256), Int(256), index1);
			index2 = IfThenElse(UInt(index2) > UInt(256), Int(256), index2);
			index3 = IfThenElse(UInt(index3) > UInt(256), Int(256), index3);

			c.x = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c) + index0 * 16, 16);
			c.y = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c) + index1 * 16, 16);
			c.z = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c) + index2 * 16, 16);
			c.w = *Pointer<Float4>(r.data + OFFSET(DrawData,vs.c) + index3 * 16, 16);

			transpose4x4(c.x, c.y, c.z, c.w);
		}

		return c;
	}

	void VertexProgram::shader(Registers &r)
	{
	//	vertexShader->print("VertexShader-%0.16llX.txt", state.shaderHash);

		unsigned short version = vertexShader->getVersion();

		r.enableIndex = 0;
		r.stackIndex = 0;
	
		for(int i = 0; i < vertexShader->getLength(); i++)
		{
			const ShaderInstruction *instruction = vertexShader->getInstruction(i);
			Op::Opcode opcode = instruction->getOpcode();

		//	#ifndef NDEBUG   // FIXME: Centralize debug output control
		//		vertexShader->printInstruction(i, "debug.txt");
		//	#endif

			if(opcode == Op::OPCODE_DCL || opcode == Op::OPCODE_DEF || opcode == Op::OPCODE_DEFI || opcode == Op::OPCODE_DEFB)
			{
				continue;
			}

			Dst dest = instruction->getDestinationParameter();
			Src src0 = instruction->getSourceParameter(0);
			Src src1 = instruction->getSourceParameter(1);
			Src src2 = instruction->getSourceParameter(2);
			Src src3 = instruction->getSourceParameter(3);

			bool predicate = instruction->isPredicate();
			int size = vertexShader->size(opcode);
			Usage usage = instruction->getUsage();
			unsigned char usageIndex = instruction->getUsageIndex();
			Control control = instruction->getControl();
			bool integer = dest.type == Dst::PARAMETER_ADDR;
			bool pp = dest.partialPrecision;

			Color4f d;
			Color4f s0;
			Color4f s1;
			Color4f s2;
			Color4f s3;

			if(src0.type != Src::PARAMETER_VOID) s0 = reg(r, src0);
			if(src1.type != Src::PARAMETER_VOID) s1 = reg(r, src1);
			if(src2.type != Src::PARAMETER_VOID) s2 = reg(r, src2);
			if(src3.type != Src::PARAMETER_VOID) s3 = reg(r, src3);

			switch(opcode)
			{
			case Op::OPCODE_VS_1_0:										break;
			case Op::OPCODE_VS_1_1:										break;
			case Op::OPCODE_VS_2_0:										break;
			case Op::OPCODE_VS_2_x:										break;
			case Op::OPCODE_VS_2_sw:									break;
			case Op::OPCODE_VS_3_0:										break;
			case Op::OPCODE_VS_3_sw:									break;
			case Op::OPCODE_DCL:										break;
			case Op::OPCODE_DEF:										break;
			case Op::OPCODE_DEFI:										break;
			case Op::OPCODE_DEFB:										break;
			case Op::OPCODE_NOP:										break;
			case Op::OPCODE_ABS:		abs(d, s0);						break;
			case Op::OPCODE_ADD:		add(d, s0, s1);					break;
			case Op::OPCODE_CRS:		crs(d, s0, s1);					break;
			case Op::OPCODE_DP3:		dp3(d, s0, s1);					break;
			case Op::OPCODE_DP4:		dp4(d, s0, s1);					break;
			case Op::OPCODE_DST:		dst(d, s0, s1);					break;
			case Op::OPCODE_EXP:		exp(d, s0, pp);					break;
			case Op::OPCODE_EXPP:		expp(d, s0, version);			break;
			case Op::OPCODE_FRC:		frc(d, s0);						break;
			case Op::OPCODE_LIT:		lit(d, s0);						break;
			case Op::OPCODE_LOG:		log(d, s0, pp);					break;
			case Op::OPCODE_LOGP:		logp(d, s0, version);			break;
			case Op::OPCODE_LRP:		lrp(d, s0, s1, s2);				break;
			case Op::OPCODE_M3X2:		M3X2(r, d, s0, src1);			break;
			case Op::OPCODE_M3X3:		M3X3(r, d, s0, src1);			break;
			case Op::OPCODE_M3X4:		M3X4(r, d, s0, src1);			break;
			case Op::OPCODE_M4X3:		M4X3(r, d, s0, src1);			break;
			case Op::OPCODE_M4X4:		M4X4(r, d, s0, src1);			break;
			case Op::OPCODE_MAD:		mad(d, s0, s1, s2);				break;
			case Op::OPCODE_MAX:		max(d, s0, s1);					break;
			case Op::OPCODE_MIN:		min(d, s0, s1);					break;
			case Op::OPCODE_MOV:		mov(d, s0, integer);			break;
			case Op::OPCODE_MOVA:		mov(d, s0);						break;
			case Op::OPCODE_MUL:		mul(d, s0, s1);					break;
			case Op::OPCODE_NRM:		nrm(d, s0, pp);					break;
			case Op::OPCODE_POW:		pow(d, s0, s1, pp);				break;
			case Op::OPCODE_RCP:		rcp(d, s0, pp);					break;
			case Op::OPCODE_RSQ:		rsq(d, s0, pp);					break;
			case Op::OPCODE_SGE:		sge(d, s0, s1);					break;
			case Op::OPCODE_SGN:		sgn(d, s0);						break;
			case Op::OPCODE_SINCOS:		sincos(d, s0, pp);				break;
			case Op::OPCODE_SLT:		slt(d, s0, s1);					break;
			case Op::OPCODE_SUB:		sub(d, s0, s1);					break;
			case Op::OPCODE_BREAK:		BREAK(r);						break;
			case Op::OPCODE_BREAKC:		BREAKC(r, s0, s1, control);		break;
			case Op::OPCODE_BREAKP:		BREAKP(r, src0);				break;
			case Op::OPCODE_CALL:		CALL(r, dest.index);			break;
			case Op::OPCODE_CALLNZ:		CALLNZ(r, dest.index, src0);	break;
			case Op::OPCODE_ELSE:		ELSE(r);						break;
			case Op::OPCODE_ENDIF:		ENDIF(r);						break;
			case Op::OPCODE_ENDLOOP:	ENDLOOP(r);						break;
			case Op::OPCODE_ENDREP:		ENDREP(r);						break;
			case Op::OPCODE_IF:			IF(r, src0);					break;
			case Op::OPCODE_IFC:		IFC(r, s0, s1, control);		break;
			case Op::OPCODE_LABEL:		LABEL(dest.index);				break;
			case Op::OPCODE_LOOP:		LOOP(r, src1);					break;
			case Op::OPCODE_REP:		REP(r, src0);					break;
			case Op::OPCODE_RET:		RET(r);							break;
			case Op::OPCODE_SETP:		setp(d, s0, s1, control);		break;
			case Op::OPCODE_TEXLDL:		TEXLDL(r, d, s0, src1);			break;
			case Op::OPCODE_END:										break;
			default:
				ASSERT(false);
			}

			if(dest.type != Dst::PARAMETER_VOID && dest.type != Dst::PARAMETER_LABEL)
			{
				if(dest.saturate)
				{
					if(dest.x) d.r = Max(d.r, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dest.y) d.g = Max(d.g, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dest.z) d.b = Max(d.b, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dest.w) d.a = Max(d.a, Float4(0.0f, 0.0f, 0.0f, 0.0f));

					if(dest.x) d.r = Min(d.r, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dest.y) d.g = Min(d.g, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dest.z) d.b = Min(d.b, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dest.w) d.a = Min(d.a, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				}

				if(vertexShader->containsDynamicBranching())
				{
					Color4f pDst;   // FIXME: Rename

					switch(dest.type)
					{
					case Dst::PARAMETER_VOID:																		break;
					case Dst::PARAMETER_TEMP:		pDst = r.r[dest.index];											break;
					case Dst::PARAMETER_ADDR:		pDst = r.a0;													break;
					case Dst::PARAMETER_RASTOUT:
						switch(dest.index)
						{
						case 0:
							if(dest.x) pDst.x = r.ox[Pos];
							if(dest.y) pDst.y = r.oy[Pos];
							if(dest.z) pDst.z = r.oz[Pos];
							if(dest.w) pDst.w = r.ow[Pos];
							break;
						case 1:
							pDst.x = r.ox[Fog];
							break;
						case 2:
							pDst.x = r.oy[Pts];
							break;
						default:
							ASSERT(false);
						}
						break;
					case Dst::PARAMETER_ATTROUT:
						if(dest.x) pDst.x = r.ox[D0 + dest.index];
						if(dest.y) pDst.y = r.oy[D0 + dest.index];
						if(dest.z) pDst.z = r.oz[D0 + dest.index];
						if(dest.w) pDst.w = r.ow[D0 + dest.index];
						break;
					case Dst::PARAMETER_TEXCRDOUT:
				//	case Dst::PARAMETER_OUTPUT:
						if(version < 0x0300)
						{
							if(dest.x) pDst.x = r.ox[T0 + dest.index];
							if(dest.y) pDst.y = r.oy[T0 + dest.index];
							if(dest.z) pDst.z = r.oz[T0 + dest.index];
							if(dest.w) pDst.w = r.ow[T0 + dest.index];
						}
						else
						{
							if(!dest.relative)
							{
								if(dest.x) pDst.x = r.ox[dest.index];
								if(dest.y) pDst.y = r.oy[dest.index];
								if(dest.z) pDst.z = r.oz[dest.index];
								if(dest.w) pDst.w = r.ow[dest.index];
							}
							else
							{
								Int aL = r.aL[r.loopDepth];

								if(dest.x) pDst.x = r.ox[dest.index + aL];
								if(dest.y) pDst.y = r.oy[dest.index + aL];
								if(dest.z) pDst.z = r.oz[dest.index + aL];
								if(dest.w) pDst.w = r.ow[dest.index + aL];
							}
						}
						break;
					case Dst::PARAMETER_LABEL:																		break;
					case Dst::PARAMETER_PREDICATE:	pDst = r.p0;													break;
					case Dst::PARAMETER_INPUT:																		break;
					default:
						ASSERT(false);
					}

					Int4 enable = r.enableStack[r.enableIndex] & r.enableBreak;

					Int4 xEnable = enable;
					Int4 yEnable = enable;
					Int4 zEnable = enable;
					Int4 wEnable = enable;

					if(predicate)
					{
						unsigned char pSwizzle = instruction->getPredicateSwizzle();

						Float4 xPredicate = r.p0[(pSwizzle >> 0) & 0x03];
						Float4 yPredicate = r.p0[(pSwizzle >> 2) & 0x03];
						Float4 zPredicate = r.p0[(pSwizzle >> 4) & 0x03];
						Float4 wPredicate = r.p0[(pSwizzle >> 6) & 0x03];

						if(!instruction->isPredicateNot())
						{
							if(dest.x) xEnable = xEnable & As<Int4>(xPredicate);
							if(dest.y) yEnable = yEnable & As<Int4>(yPredicate);
							if(dest.z) zEnable = zEnable & As<Int4>(zPredicate);
							if(dest.w) wEnable = wEnable & As<Int4>(wPredicate);
						}
						else
						{
							if(dest.x) xEnable = xEnable & ~As<Int4>(xPredicate);
							if(dest.y) yEnable = yEnable & ~As<Int4>(yPredicate);
							if(dest.z) zEnable = zEnable & ~As<Int4>(zPredicate);
							if(dest.w) wEnable = wEnable & ~As<Int4>(wPredicate);
						}
					}

					if(dest.x) d.x = As<Float4>(As<Int4>(d.x) & xEnable);
					if(dest.y) d.y = As<Float4>(As<Int4>(d.y) & yEnable);
					if(dest.z) d.z = As<Float4>(As<Int4>(d.z) & zEnable);
					if(dest.w) d.w = As<Float4>(As<Int4>(d.w) & wEnable);

					if(dest.x) d.x = As<Float4>(As<Int4>(d.x) | (As<Int4>(pDst.x) & ~xEnable));
					if(dest.y) d.y = As<Float4>(As<Int4>(d.y) | (As<Int4>(pDst.y) & ~yEnable));
					if(dest.z) d.z = As<Float4>(As<Int4>(d.z) | (As<Int4>(pDst.z) & ~zEnable));
					if(dest.w) d.w = As<Float4>(As<Int4>(d.w) | (As<Int4>(pDst.w) & ~wEnable));
				}

				switch(dest.type)
				{
				case Dst::PARAMETER_VOID:
					break;
				case Dst::PARAMETER_TEMP:
					if(dest.x) r.r[dest.index].x = d.x;
					if(dest.y) r.r[dest.index].y = d.y;
					if(dest.z) r.r[dest.index].z = d.z;
					if(dest.w) r.r[dest.index].w = d.w;
					break;
				case Dst::PARAMETER_ADDR:
					if(dest.x) r.a0.x = d.x;
					if(dest.y) r.a0.y = d.y;
					if(dest.z) r.a0.z = d.z;
					if(dest.w) r.a0.w = d.w;
					break;
				case Dst::PARAMETER_RASTOUT:
					switch(dest.index)
					{
					case 0:
						if(dest.x) r.ox[Pos] = d.x;
						if(dest.y) r.oy[Pos] = d.y;
						if(dest.z) r.oz[Pos] = d.z;
						if(dest.w) r.ow[Pos] = d.w;
						break;
					case 1:
						r.ox[Fog] = d.x;
						break;
					case 2:		
						r.oy[Pts] = d.x;
						break;
					default:	ASSERT(false);
					}
					break;
				case Dst::PARAMETER_ATTROUT:	
					if(dest.x) r.ox[D0 + dest.index] = d.x;
					if(dest.y) r.oy[D0 + dest.index] = d.y;
					if(dest.z) r.oz[D0 + dest.index] = d.z;
					if(dest.w) r.ow[D0 + dest.index] = d.w;
					break;
				case Dst::PARAMETER_TEXCRDOUT:
			//	case Dst::PARAMETER_OUTPUT:
					if(version < 0x0300)
					{
						if(dest.x) r.ox[T0 + dest.index] = d.x;
						if(dest.y) r.oy[T0 + dest.index] = d.y;
						if(dest.z) r.oz[T0 + dest.index] = d.z;
						if(dest.w) r.ow[T0 + dest.index] = d.w;
					}
					else
					{
						if(!dest.relative)
						{
							if(dest.x) r.ox[dest.index] = d.x;
							if(dest.y) r.oy[dest.index] = d.y;
							if(dest.z) r.oz[dest.index] = d.z;
							if(dest.w) r.ow[dest.index] = d.w;
						}
						else
						{
							Int aL = r.aL[r.loopDepth];

							if(dest.x) r.ox[dest.index + aL] = d.x;
							if(dest.y) r.oy[dest.index + aL] = d.y;
							if(dest.z) r.oz[dest.index + aL] = d.z;
							if(dest.w) r.ow[dest.index + aL] = d.w;
						}
					}
					break;
				case Dst::PARAMETER_LABEL:																		break;
				case Dst::PARAMETER_PREDICATE:	r.p0 = d;														break;
				case Dst::PARAMETER_INPUT:																		break;
				default:
					ASSERT(false);
				}
			}
		}

		if(returns)
		{
			Nucleus::setInsertBlock(returnBlock);
		}
	}

	void VertexProgram::passThrough(Registers &r)
	{
		if(vertexShader)
		{
			for(int i = 0; i < 12; i++)
			{
				unsigned char usage = vertexShader->output[i][0].usage;
				unsigned char index = vertexShader->output[i][0].index;

				switch(usage)
				{
				case 0xFF:
					continue;
				case ShaderOperation::USAGE_PSIZE:
					r.oy[i] = r.v[i].x;
					break;
				case ShaderOperation::USAGE_TEXCOORD:
					r.ox[i] = r.v[i].x;
					r.oy[i] = r.v[i].y;
					r.oz[i] = r.v[i].z;
					r.ow[i] = r.v[i].w;
					break;
				case ShaderOperation::USAGE_POSITION:
					r.ox[i] = r.v[i].x;
					r.oy[i] = r.v[i].y;
					r.oz[i] = r.v[i].z;
					r.ow[i] = r.v[i].w;
					break;
				case ShaderOperation::USAGE_COLOR:
					r.ox[i] = r.v[i].x;
					r.oy[i] = r.v[i].y;
					r.oz[i] = r.v[i].z;
					r.ow[i] = r.v[i].w;
					break;
				case ShaderOperation::USAGE_FOG:
					r.ox[i] = r.v[i].x;
					break;
				default:
					ASSERT(false);
				}
			}
		}
		else
		{
			r.ox[Pos] = r.v[PositionT].x;
			r.oy[Pos] = r.v[PositionT].y;
			r.oz[Pos] = r.v[PositionT].z;
			r.ow[Pos] = r.v[PositionT].w;

			for(int i = 0; i < 2; i++)
			{
				r.ox[D0 + i] = r.v[Color0 + i].x;
				r.oy[D0 + i] = r.v[Color0 + i].y;
				r.oz[D0 + i] = r.v[Color0 + i].z;
				r.ow[D0 + i] = r.v[Color0 + i].w;
			}

			for(int i = 0; i < 8; i++)
			{
				r.ox[T0 + i] = r.v[TexCoord0 + i].x;
				r.oy[T0 + i] = r.v[TexCoord0 + i].y;
				r.oz[T0 + i] = r.v[TexCoord0 + i].z;
				r.ow[T0 + i] = r.v[TexCoord0 + i].w;
			}

			r.oy[Pts] = r.v[PSize].x;
		}
	}

	Color4f VertexProgram::reg(Registers &r, const Src &src, int offset)
	{
		int i = src.index + offset;

		Color4f reg;

		if(src.type == Src::PARAMETER_CONST)
		{
			reg = readConstant(r, src, offset);
		}
		
		switch(src.type)
		{
		case Src::PARAMETER_TEMP:			reg = r.r[i];	break;
		case Src::PARAMETER_CONST:							break;
		case Src::PARAMETER_INPUT:			reg = r.v[i];	break;
		case Src::PARAMETER_VOID:			return r.r[0];   // Dummy
		case Src::PARAMETER_FLOATLITERAL:	return r.r[0];   // Dummy
		case Src::PARAMETER_ADDR:			reg = r.a0;		break;
		case Src::PARAMETER_CONSTBOOL:		return r.r[0];   // Dummy
		case Src::PARAMETER_CONSTINT:		return r.r[0];   // Dummy
		case Src::PARAMETER_LOOP:			return r.r[0];   // Dummy
		case Src::PARAMETER_PREDICATE:		return r.r[0];   // Dummy
		case Src::PARAMETER_SAMPLER:		return r.r[0];   // Dummy
		default:
			ASSERT(false);
		}

		Color4f mod;

		mod.x = reg[(src.swizzle >> 0) & 0x03];
		mod.y = reg[(src.swizzle >> 2) & 0x03];
		mod.z = reg[(src.swizzle >> 4) & 0x03];
		mod.w = reg[(src.swizzle >> 6) & 0x03];

		switch(src.modifier)
		{
		case Src::MODIFIER_NONE:
			break;
		case Src::MODIFIER_NEGATE:
			mod.x = -mod.x;
			mod.y = -mod.y;
			mod.z = -mod.z;
			mod.w = -mod.w;
			break;
		case Src::MODIFIER_BIAS:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_BIAS_NEGATE:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_SIGN:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_SIGN_NEGATE:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_COMPLEMENT:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_X2:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_X2_NEGATE:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_DZ:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_DW:
			ASSERT(false);   // NOTE: Unimplemented
			break;
		case Src::MODIFIER_ABS:
			mod.x = Abs(mod.x);
			mod.y = Abs(mod.y);
			mod.z = Abs(mod.z);
			mod.w = Abs(mod.w);
			break;
		case Src::MODIFIER_ABS_NEGATE:
			mod.x = -Abs(mod.x);
			mod.y = -Abs(mod.y);
			mod.z = -Abs(mod.z);
			mod.w = -Abs(mod.w);
			break;
		case Src::MODIFIER_NOT:
			UNIMPLEMENTED();
			break;
		default:
			ASSERT(false);
		}

		return mod;
	}

	void VertexProgram::M3X2(Registers &r, Color4f &dst, Color4f &src0, Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
	}

	void VertexProgram::M3X3(Registers &r, Color4f &dst, Color4f &src0, Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
	}

	void VertexProgram::M3X4(Registers &r, Color4f &dst, Color4f &src0, Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);
		Color4f row3 = reg(r, src1, 3);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
		dst.w = dot3(src0, row3);
	}

	void VertexProgram::M4X3(Registers &r, Color4f &dst, Color4f &src0, Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
	}

	void VertexProgram::M4X4(Registers &r, Color4f &dst, Color4f &src0, Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);
		Color4f row3 = reg(r, src1, 3);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
		dst.w = dot4(src0, row3);
	}

	void VertexProgram::BREAK(Registers &r)
	{
		llvm::BasicBlock *deadBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		if(breakDepth == 0)
		{
			Nucleus::createBr(endBlock);
		}
		else
		{
			r.enableBreak = r.enableBreak & ~r.enableStack[r.enableIndex];
			Bool allBreak = SignMask(r.enableBreak) == 0x0;

			branch(allBreak, endBlock, deadBlock);
		}

		Nucleus::setInsertBlock(deadBlock);
	}

	void VertexProgram::BREAKC(Registers &r, Color4f &src0, Color4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Op::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Op::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Op::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Op::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Op::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Op::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		condition &= r.enableStack[r.enableIndex];

		llvm::BasicBlock *continueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		r.enableBreak = r.enableBreak & ~condition;
		Bool allBreak = SignMask(r.enableBreak) == 0x0;

		branch(allBreak, endBlock, continueBlock);
		Nucleus::setInsertBlock(continueBlock);
	}

	void VertexProgram::BREAKP(Registers &r, const Src &predicateRegister)   // FIXME: Factor out parts common with BREAKC
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		llvm::BasicBlock *continueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		r.enableBreak = r.enableBreak & ~condition;
		Bool allBreak = SignMask(r.enableBreak) == 0x0;

		branch(allBreak, endBlock, continueBlock);
		Nucleus::setInsertBlock(continueBlock);
	}

	void VertexProgram::CALL(Registers &r, int labelIndex)
	{
		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((unsigned int)callRetBlock.size() - 1);   // FIXME

		Nucleus::createBr(labelBlock[labelIndex]);
		Nucleus::setInsertBlock(retBlock);
	}

	void VertexProgram::CALLNZ(Registers &r, int labelIndex, const Src &src)
	{
		if(src.type == Src::PARAMETER_CONSTBOOL)
		{
			CALLNZb(r, labelIndex, src);
		}
		else if(src.type == Src::PARAMETER_PREDICATE)
		{
			CALLNZp(r, labelIndex, src);
		}
		else ASSERT(false);
	}

	void VertexProgram::CALLNZb(Registers &r, int labelIndex, const Src &boolRegister)
	{
		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,vs.b[boolRegister.index])) != Byte(0));   // FIXME
		
		if(boolRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = !condition;	
		}

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((int)callRetBlock.size() - 1);   // FIXME

		branch(condition, labelBlock[labelIndex], retBlock);
		Nucleus::setInsertBlock(retBlock);
	}

	void VertexProgram::CALLNZp(Registers &r, int labelIndex, const Src &predicateRegister)
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((int)callRetBlock.size() - 1);   // FIXME

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, labelBlock[labelIndex], retBlock);
		Nucleus::setInsertBlock(retBlock);

		r.enableIndex--;
	}

	void VertexProgram::ELSE(Registers &r)
	{
		ifDepth--;

		llvm::BasicBlock *falseBlock = ifFalseBlock[ifDepth];
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		if(isConditionalIf[ifDepth])
		{
			Int4 condition = ~r.enableStack[r.enableIndex] & r.enableStack[r.enableIndex - 1];
			Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

			branch(notAllFalse, falseBlock, endBlock);

			r.enableStack[r.enableIndex] = ~r.enableStack[r.enableIndex] & r.enableStack[r.enableIndex - 1];
		}
		else
		{
			Nucleus::createBr(endBlock);
			Nucleus::setInsertBlock(falseBlock);
		}

		ifFalseBlock[ifDepth] = endBlock;

		ifDepth++;
	}

	void VertexProgram::ENDIF(Registers &r)
	{
		ifDepth--;

		llvm::BasicBlock *endBlock = ifFalseBlock[ifDepth];

		Nucleus::createBr(endBlock);
		Nucleus::setInsertBlock(endBlock);

		if(isConditionalIf[ifDepth])
		{
			breakDepth--;
			r.enableIndex--;
		}
	}

	void VertexProgram::ENDREP(Registers &r)
	{
		loopRepDepth--;

		llvm::BasicBlock *testBlock = loopRepTestBlock[loopRepDepth];
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth];

		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(endBlock);

		r.loopDepth--;
		r.enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	}

	void VertexProgram::ENDLOOP(Registers &r)
	{
		loopRepDepth--;

		r.aL[r.loopDepth] = r.aL[r.loopDepth] + r.increment[r.loopDepth];   // FIXME: +=

		llvm::BasicBlock *testBlock = loopRepTestBlock[loopRepDepth];
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth];

		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(endBlock);

		r.loopDepth--;
		r.enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	}

	void VertexProgram::IF(Registers &r, const Src &src)
	{
		if(src.type == Src::PARAMETER_CONSTBOOL)
		{
			IFb(r, src);
		}
		else if(src.type == Src::PARAMETER_PREDICATE)
		{
			IFp(r, src);
		}
		else ASSERT(false);
	}

	void VertexProgram::IFb(Registers &r, const Src &boolRegister)
	{
		ASSERT(ifDepth < 24 + 4);

		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,vs.b[boolRegister.index])) != Byte(0));   // FIXME

		if(boolRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = !condition;	
		}

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		branch(condition, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = false;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
	}

	void VertexProgram::IFp(Registers &r, const Src &predicateRegister)   // FIXME: Factor out parts common with IFC
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = true;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
		breakDepth++;
	}

	void VertexProgram::IFC(Registers &r, Color4f &src0, Color4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Op::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Op::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Op::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Op::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Op::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Op::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		condition &= r.enableStack[r.enableIndex];

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = true;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
		breakDepth++;
	}

	void VertexProgram::LABEL(int labelIndex)
	{
		Nucleus::setInsertBlock(labelBlock[labelIndex]);
	}

	void VertexProgram::LOOP(Registers &r, const Src &integerRegister)
	{
		r.loopDepth++;

		r.iteration[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,vs.i[integerRegister.index][0]));
		r.aL[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,vs.i[integerRegister.index][1]));
		r.increment[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,vs.i[integerRegister.index][2]));

		// FIXME: Compiles to two instructions?
		If(r.increment[r.loopDepth] == 0)
		{
			r.increment[r.loopDepth] = 1;
		}

		llvm::BasicBlock *loopBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *testBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		loopRepTestBlock[loopRepDepth] = testBlock;
		loopRepEndBlock[loopRepDepth] = endBlock;

		// FIXME: jump(testBlock)
		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(testBlock);

		branch(r.iteration[r.loopDepth] > 0, loopBlock, endBlock);
		Nucleus::setInsertBlock(loopBlock);

		r.iteration[r.loopDepth] = r.iteration[r.loopDepth] - 1;   // FIXME: --
		
		loopRepDepth++;
		breakDepth = 0;
	}

	void VertexProgram::REP(Registers &r, const Src &integerRegister)
	{
		r.loopDepth++;

		r.iteration[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,vs.i[integerRegister.index][0]));
		r.aL[r.loopDepth] = r.aL[r.loopDepth - 1];

		llvm::BasicBlock *loopBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *testBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		loopRepTestBlock[loopRepDepth] = testBlock;
		loopRepEndBlock[loopRepDepth] = endBlock;

		// FIXME: jump(testBlock)
		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(testBlock);

		branch(r.iteration[r.loopDepth] > 0, loopBlock, endBlock);
		Nucleus::setInsertBlock(loopBlock);

		r.iteration[r.loopDepth] = r.iteration[r.loopDepth] - 1;   // FIXME: --

		loopRepDepth++;
		breakDepth = 0;
	}

	void VertexProgram::RET(Registers &r)
	{
		if(!returns)
		{
			returnBlock = Nucleus::createBasicBlock();
			Nucleus::createBr(returnBlock);

			returns = true;
		}
		else
		{
			// FIXME: Encapsulate
			UInt index = r.callStack[--r.stackIndex];
 
			llvm::BasicBlock *unreachableBlock = Nucleus::createBasicBlock();
			llvm::Value *value = Nucleus::createLoad(index.address);
			llvm::Value *switchInst = Nucleus::createSwitch(value, unreachableBlock, (int)callRetBlock.size());

			for(unsigned int i = 0; i < callRetBlock.size(); i++)
			{
				Nucleus::addSwitchCase(switchInst, i, callRetBlock[i]);
			}

			Nucleus::setInsertBlock(unreachableBlock);
			Nucleus::createUnreachable();
		}
	}

	void VertexProgram::TEXLDL(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Pointer<Byte> texture = r.data + OFFSET(DrawData,mipmap[16]) + src1.index * sizeof(Texture);

		Color4f tmp;

		sampler[src1.index]->sampleTexture(texture, tmp, src0.x, src0.y, src0.z, src0.w, src0, src0, false, false, true);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}
}
