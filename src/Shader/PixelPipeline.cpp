// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "PixelPipeline.hpp"
#include "Renderer.hpp"
#include "SamplerCore.hpp"

namespace sw
{
	extern bool postBlendSRGB;

	void PixelPipeline::setBuiltins(PixelRoutine::Registers &rBase, Int &x, Int &y, Float4(&z)[4], Float4 &w)
	{
		Registers& r = *static_cast<Registers*>(&rBase);

		if(state.color[0].component & 0x1) r.diffuse.x = convertFixed12(r.v[0].x); else r.diffuse.x = Short4(0x1000);
		if(state.color[0].component & 0x2) r.diffuse.y = convertFixed12(r.v[0].y); else r.diffuse.y = Short4(0x1000);
		if(state.color[0].component & 0x4) r.diffuse.z = convertFixed12(r.v[0].z); else r.diffuse.z = Short4(0x1000);
		if(state.color[0].component & 0x8) r.diffuse.w = convertFixed12(r.v[0].w); else r.diffuse.w = Short4(0x1000);

		if(state.color[1].component & 0x1) r.specular.x = convertFixed12(r.v[1].x); else r.specular.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		if(state.color[1].component & 0x2) r.specular.y = convertFixed12(r.v[1].y); else r.specular.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		if(state.color[1].component & 0x4) r.specular.z = convertFixed12(r.v[1].z); else r.specular.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		if(state.color[1].component & 0x8) r.specular.w = convertFixed12(r.v[1].w); else r.specular.w = Short4(0x0000, 0x0000, 0x0000, 0x0000);
	}

	void PixelPipeline::fixedFunction(Registers& r)
	{
		r.current = r.diffuse;
		Vector4s temp(0x0000, 0x0000, 0x0000, 0x0000);

		for(int stage = 0; stage < 8; stage++)
		{
			if(state.textureStage[stage].stageOperation == TextureStage::STAGE_DISABLE)
			{
				break;
			}

			Vector4s texture;

			if(state.textureStage[stage].usesTexture)
			{
				sampleTexture(r, texture, stage, stage);
			}

			blendTexture(r, temp, texture, stage);
		}

		specularPixel(r.current, r.specular);
	}

	void PixelPipeline::applyShader(PixelRoutine::Registers &rBase, Int cMask[4])
	{
		Registers& r = *static_cast<Registers*>(&rBase);

		if(!shader)
		{
			fixedFunction(r);
			return;
		}

		int pad = 0;        // Count number of texm3x3pad instructions
		Vector4s dPairing;   // Destination for first pairing instruction

		for(size_t i = 0; i < shader->getLength(); i++)
		{
			const Shader::Instruction *instruction = shader->getInstruction(i);
			Shader::Opcode opcode = instruction->opcode;

			//	#ifndef NDEBUG   // FIXME: Centralize debug output control
			//		shader->printInstruction(i, "debug.txt");
			//	#endif

			if(opcode == Shader::OPCODE_DCL || opcode == Shader::OPCODE_DEF || opcode == Shader::OPCODE_DEFI || opcode == Shader::OPCODE_DEFB)
			{
				continue;
			}

			const Dst &dst = instruction->dst;
			const Src &src0 = instruction->src[0];
			const Src &src1 = instruction->src[1];
			const Src &src2 = instruction->src[2];

			unsigned short version = shader->getVersion();
			bool pairing = i + 1 < shader->getLength() && shader->getInstruction(i + 1)->coissue;   // First instruction of pair
			bool coissue = instruction->coissue;                                                              // Second instruction of pair

			Vector4s d;
			Vector4s s0;
			Vector4s s1;
			Vector4s s2;

			if(src0.type != Shader::PARAMETER_VOID) s0 = fetchRegisterS(r, src0);
			if(src1.type != Shader::PARAMETER_VOID) s1 = fetchRegisterS(r, src1);
			if(src2.type != Shader::PARAMETER_VOID) s2 = fetchRegisterS(r, src2);

			Float4 u = version < 0x0104 ? r.v[2 + dst.index].x : r.v[2 + src0.index].x;
			Float4 v = version < 0x0104 ? r.v[2 + dst.index].y : r.v[2 + src0.index].y;
			Float4 s = version < 0x0104 ? r.v[2 + dst.index].z : r.v[2 + src0.index].z;
			Float4 t = version < 0x0104 ? r.v[2 + dst.index].w : r.v[2 + src0.index].w;

			switch(opcode)
			{
			case Shader::OPCODE_PS_1_0: break;
			case Shader::OPCODE_PS_1_1: break;
			case Shader::OPCODE_PS_1_2: break;
			case Shader::OPCODE_PS_1_3: break;
			case Shader::OPCODE_PS_1_4: break;

			case Shader::OPCODE_DEF:    break;

			case Shader::OPCODE_NOP:    break;
			case Shader::OPCODE_MOV: MOV(d, s0);         break;
			case Shader::OPCODE_ADD: ADD(d, s0, s1);     break;
			case Shader::OPCODE_SUB: SUB(d, s0, s1);     break;
			case Shader::OPCODE_MAD: MAD(d, s0, s1, s2); break;
			case Shader::OPCODE_MUL: MUL(d, s0, s1);     break;
			case Shader::OPCODE_DP3: DP3(d, s0, s1);     break;
			case Shader::OPCODE_DP4: DP4(d, s0, s1);     break;
			case Shader::OPCODE_LRP: LRP(d, s0, s1, s2); break;
			case Shader::OPCODE_TEXCOORD:
				if(version < 0x0104)
				{
					TEXCOORD(d, u, v, s, dst.index);
				}
				else
				{
					if((src0.swizzle & 0x30) == 0x20)   // .xyz
					{
						TEXCRD(d, u, v, s, src0.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
					else   // .xyw
					{
						TEXCRD(d, u, v, t, src0.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
				}
				break;
			case Shader::OPCODE_TEXKILL:
				if(version < 0x0104)
				{
					TEXKILL(cMask, u, v, s);
				}
				else if(version == 0x0104)
				{
					if(dst.type == Shader::PARAMETER_TEXTURE)
					{
						TEXKILL(cMask, u, v, s);
					}
					else
					{
						TEXKILL(cMask, r.rs[dst.index]);
					}
				}
				else ASSERT(false);
				break;
			case Shader::OPCODE_TEX:
				if(version < 0x0104)
				{
					TEX(r, d, u, v, s, dst.index, false);
				}
				else if(version == 0x0104)
				{
					if(src0.type == Shader::PARAMETER_TEXTURE)
					{
						if((src0.swizzle & 0x30) == 0x20)   // .xyz
						{
							TEX(r, d, u, v, s, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
						}
						else   // .xyw
						{
							TEX(r, d, u, v, t, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
						}
					}
					else
					{
						TEXLD(r, d, s0, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
				}
				else ASSERT(false);
				break;
			case Shader::OPCODE_TEXBEM:       TEXBEM(r, d, s0, u, v, s, dst.index);                                             break;
			case Shader::OPCODE_TEXBEML:      TEXBEML(r, d, s0, u, v, s, dst.index);                                            break;
			case Shader::OPCODE_TEXREG2AR:    TEXREG2AR(r, d, s0, dst.index);                                                   break;
			case Shader::OPCODE_TEXREG2GB:    TEXREG2GB(r, d, s0, dst.index);                                                   break;
			case Shader::OPCODE_TEXM3X2PAD:   TEXM3X2PAD(r, u, v, s, s0, 0, src0.modifier == Shader::MODIFIER_SIGN);            break;
			case Shader::OPCODE_TEXM3X2TEX:   TEXM3X2TEX(r, d, u, v, s, dst.index, s0, src0.modifier == Shader::MODIFIER_SIGN); break;
			case Shader::OPCODE_TEXM3X3PAD:   TEXM3X3PAD(r, u, v, s, s0, pad++ % 2, src0.modifier == Shader::MODIFIER_SIGN);    break;
			case Shader::OPCODE_TEXM3X3TEX:   TEXM3X3TEX(r, d, u, v, s, dst.index, s0, src0.modifier == Shader::MODIFIER_SIGN); break;
			case Shader::OPCODE_TEXM3X3SPEC:  TEXM3X3SPEC(r, d, u, v, s, dst.index, s0, s1);                                    break;
			case Shader::OPCODE_TEXM3X3VSPEC: TEXM3X3VSPEC(r, d, u, v, s, dst.index, s0);                                       break;
			case Shader::OPCODE_CND:          CND(d, s0, s1, s2);                                                               break;
			case Shader::OPCODE_TEXREG2RGB:   TEXREG2RGB(r, d, s0, dst.index);                                                  break;
			case Shader::OPCODE_TEXDP3TEX:    TEXDP3TEX(r, d, u, v, s, dst.index, s0);                                          break;
			case Shader::OPCODE_TEXM3X2DEPTH: TEXM3X2DEPTH(r, d, u, v, s, s0, src0.modifier == Shader::MODIFIER_SIGN);          break;
			case Shader::OPCODE_TEXDP3:       TEXDP3(r, d, u, v, s, s0);                                                        break;
			case Shader::OPCODE_TEXM3X3:      TEXM3X3(r, d, u, v, s, s0, src0.modifier == Shader::MODIFIER_SIGN);               break;
			case Shader::OPCODE_TEXDEPTH:     TEXDEPTH(r);                                                                      break;
			case Shader::OPCODE_CMP0:         CMP(d, s0, s1, s2);                                                               break;
			case Shader::OPCODE_BEM:          BEM(r, d, s0, s1, dst.index);                                                     break;
			case Shader::OPCODE_PHASE:                                                                                          break;
			case Shader::OPCODE_END:                                                                                            break;
			default:
				ASSERT(false);
			}

			if(dst.type != Shader::PARAMETER_VOID && opcode != Shader::OPCODE_TEXKILL)
			{
				if(dst.shift > 0)
				{
					if(dst.mask & 0x1) { d.x = AddSat(d.x, d.x); if(dst.shift > 1) d.x = AddSat(d.x, d.x); if(dst.shift > 2) d.x = AddSat(d.x, d.x); }
					if(dst.mask & 0x2) { d.y = AddSat(d.y, d.y); if(dst.shift > 1) d.y = AddSat(d.y, d.y); if(dst.shift > 2) d.y = AddSat(d.y, d.y); }
					if(dst.mask & 0x4) { d.z = AddSat(d.z, d.z); if(dst.shift > 1) d.z = AddSat(d.z, d.z); if(dst.shift > 2) d.z = AddSat(d.z, d.z); }
					if(dst.mask & 0x8) { d.w = AddSat(d.w, d.w); if(dst.shift > 1) d.w = AddSat(d.w, d.w); if(dst.shift > 2) d.w = AddSat(d.w, d.w); }
				}
				else if(dst.shift < 0)
				{
					if(dst.mask & 0x1) d.x = d.x >> -dst.shift;
					if(dst.mask & 0x2) d.y = d.y >> -dst.shift;
					if(dst.mask & 0x4) d.z = d.z >> -dst.shift;
					if(dst.mask & 0x8) d.w = d.w >> -dst.shift;
				}

				if(dst.saturate)
				{
					if(dst.mask & 0x1) { d.x = Min(d.x, Short4(0x1000)); d.x = Max(d.x, Short4(0x0000, 0x0000, 0x0000, 0x0000)); }
					if(dst.mask & 0x2) { d.y = Min(d.y, Short4(0x1000)); d.y = Max(d.y, Short4(0x0000, 0x0000, 0x0000, 0x0000)); }
					if(dst.mask & 0x4) { d.z = Min(d.z, Short4(0x1000)); d.z = Max(d.z, Short4(0x0000, 0x0000, 0x0000, 0x0000)); }
					if(dst.mask & 0x8) { d.w = Min(d.w, Short4(0x1000)); d.w = Max(d.w, Short4(0x0000, 0x0000, 0x0000, 0x0000)); }
				}

				if(pairing)
				{
					if(dst.mask & 0x1) dPairing.x = d.x;
					if(dst.mask & 0x2) dPairing.y = d.y;
					if(dst.mask & 0x4) dPairing.z = d.z;
					if(dst.mask & 0x8) dPairing.w = d.w;
				}

				if(coissue)
				{
					const Dst &dst = shader->getInstruction(i - 1)->dst;

					writeDestination(r, dPairing, dst);
				}

				if(!pairing)
				{
					writeDestination(r, d, dst);
				}
			}
		}
	}

	Bool PixelPipeline::alphaTest(PixelRoutine::Registers &rBase, Int cMask[4])
	{
		Registers& r = *static_cast<Registers*>(&rBase);

		r.current.x = Min(r.current.x, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.x = Max(r.current.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		r.current.y = Min(r.current.y, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.y = Max(r.current.y, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		r.current.z = Min(r.current.z, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.z = Max(r.current.z, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		r.current.w = Min(r.current.w, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.w = Max(r.current.w, Short4(0x0000, 0x0000, 0x0000, 0x0000));

		if(!state.alphaTestActive())
		{
			return true;
		}

		Int aMask;

		if(state.transparencyAntialiasing == TRANSPARENCY_NONE)
		{
			PixelRoutine::alphaTest(r, aMask, r.current.w);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				cMask[q] &= aMask;
			}
		}
		else if(state.transparencyAntialiasing == TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			Float4 alpha = Float4(r.current.w) * Float4(1.0f / 0x1000);

			alphaToCoverage(r, cMask, alpha);
		}
		else ASSERT(false);

		Int pass = cMask[0];

		for(unsigned int q = 1; q < state.multiSample; q++)
		{
			pass = pass | cMask[q];
		}

		return pass != 0x0;
	}

	void PixelPipeline::rasterOperation(PixelRoutine::Registers &rBase, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		Registers& r = *static_cast<Registers*>(&rBase);

		if(!state.colorWriteActive(0))
		{
			return;
		}

		Vector4f oC;

		switch(state.targetFormat[0])
		{
		case FORMAT_R5G6B5:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
		case FORMAT_A8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
			if(!postBlendSRGB && state.writeSRGB)
			{
				linearToSRGB12_16(r, r.current);
			}
			else
			{
				r.current.x <<= 4;
				r.current.y <<= 4;
				r.current.z <<= 4;
				r.current.w <<= 4;
			}

			if(state.targetFormat[0] == FORMAT_R5G6B5)
			{
				r.current.x &= Short4(0xF800u);
				r.current.y &= Short4(0xFC00u);
				r.current.z &= Short4(0xF800u);
			}

			fogBlend(r, r.current, fog);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Pointer<Byte> buffer = cBuffer[0] + q * *Pointer<Int>(r.data + OFFSET(DrawData, colorSliceB[0]));
				Vector4s color = r.current;

				if(state.multiSampleMask & (1 << q))
				{
					alphaBlend(r, 0, buffer, color, x);
					logicOperation(r, 0, buffer, color, x);
					writeColor(r, 0, buffer, x, color, sMask[q], zMask[q], cMask[q]);
				}
			}
			break;
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
			convertSigned12(oC, r.current);
			PixelRoutine::fogBlend(r, oC, fog);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Pointer<Byte> buffer = cBuffer[0] + q * *Pointer<Int>(r.data + OFFSET(DrawData, colorSliceB[0]));
				Vector4f color = oC;

				if(state.multiSampleMask & (1 << q))
				{
					alphaBlend(r, 0, buffer, color, x);
					writeColor(r, 0, buffer, x, color, sMask[q], zMask[q], cMask[q]);
				}
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelPipeline::blendTexture(Registers &r, Vector4s &temp, Vector4s &texture, int stage)
	{
		Vector4s *arg1;
		Vector4s *arg2;
		Vector4s *arg3;
		Vector4s res;

		Vector4s constant;
		Vector4s tfactor;

		const TextureStage::State &textureStage = state.textureStage[stage];

		if(textureStage.firstArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_CONSTANT)
		{
			constant.x = *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].constantColor4[0]));
			constant.y = *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].constantColor4[1]));
			constant.z = *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].constantColor4[2]));
			constant.w = *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].constantColor4[3]));
		}

		if(textureStage.firstArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_TFACTOR)
		{
			tfactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[0]));
			tfactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[1]));
			tfactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[2]));
			tfactor.w = *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[3]));
		}

		// Premodulate
		if(stage > 0 && textureStage.usesTexture)
		{
			if(state.textureStage[stage - 1].stageOperation == TextureStage::STAGE_PREMODULATE)
			{
				r.current.x = MulHigh(r.current.x, texture.x) << 4;
				r.current.y = MulHigh(r.current.y, texture.y) << 4;
				r.current.z = MulHigh(r.current.z, texture.z) << 4;
			}

			if(state.textureStage[stage - 1].stageOperationAlpha == TextureStage::STAGE_PREMODULATE)
			{
				r.current.w = MulHigh(r.current.w, texture.w) << 4;
			}
		}

		if(luminance)
		{
			texture.x = MulHigh(texture.x, r.L) << 4;
			texture.y = MulHigh(texture.y, r.L) << 4;
			texture.z = MulHigh(texture.z, r.L) << 4;

			luminance = false;
		}

		switch(textureStage.firstArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg1 = &texture;    break;
		case TextureStage::SOURCE_CONSTANT:	arg1 = &constant;   break;
		case TextureStage::SOURCE_CURRENT:	arg1 = &r.current;  break;
		case TextureStage::SOURCE_DIFFUSE:	arg1 = &r.diffuse;  break;
		case TextureStage::SOURCE_SPECULAR:	arg1 = &r.specular; break;
		case TextureStage::SOURCE_TEMP:		arg1 = &temp;       break;
		case TextureStage::SOURCE_TFACTOR:	arg1 = &tfactor;    break;
		default:
			ASSERT(false);
		}

		switch(textureStage.secondArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg2 = &texture;    break;
		case TextureStage::SOURCE_CONSTANT:	arg2 = &constant;   break;
		case TextureStage::SOURCE_CURRENT:	arg2 = &r.current;  break;
		case TextureStage::SOURCE_DIFFUSE:	arg2 = &r.diffuse;  break;
		case TextureStage::SOURCE_SPECULAR:	arg2 = &r.specular; break;
		case TextureStage::SOURCE_TEMP:		arg2 = &temp;       break;
		case TextureStage::SOURCE_TFACTOR:	arg2 = &tfactor;    break;
		default:
			ASSERT(false);
		}

		switch(textureStage.thirdArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg3 = &texture;    break;
		case TextureStage::SOURCE_CONSTANT:	arg3 = &constant;   break;
		case TextureStage::SOURCE_CURRENT:	arg3 = &r.current;  break;
		case TextureStage::SOURCE_DIFFUSE:	arg3 = &r.diffuse;  break;
		case TextureStage::SOURCE_SPECULAR:	arg3 = &r.specular; break;
		case TextureStage::SOURCE_TEMP:		arg3 = &temp;       break;
		case TextureStage::SOURCE_TFACTOR:	arg3 = &tfactor;    break;
		default:
			ASSERT(false);
		}

		Vector4s mod1;
		Vector4s mod2;
		Vector4s mod3;

		switch(textureStage.firstModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			mod1.x = SubSat(Short4(0x1000), arg1->x);
			mod1.y = SubSat(Short4(0x1000), arg1->y);
			mod1.z = SubSat(Short4(0x1000), arg1->z);
			mod1.w = SubSat(Short4(0x1000), arg1->w);

			arg1 = &mod1;
			break;
		case TextureStage::MODIFIER_ALPHA:
			mod1.x = arg1->w;
			mod1.y = arg1->w;
			mod1.z = arg1->w;
			mod1.w = arg1->w;

			arg1 = &mod1;
			break;
		case TextureStage::MODIFIER_INVALPHA:
			mod1.x = SubSat(Short4(0x1000), arg1->w);
			mod1.y = SubSat(Short4(0x1000), arg1->w);
			mod1.z = SubSat(Short4(0x1000), arg1->w);
			mod1.w = SubSat(Short4(0x1000), arg1->w);

			arg1 = &mod1;
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.secondModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			mod2.x = SubSat(Short4(0x1000), arg2->x);
			mod2.y = SubSat(Short4(0x1000), arg2->y);
			mod2.z = SubSat(Short4(0x1000), arg2->z);
			mod2.w = SubSat(Short4(0x1000), arg2->w);

			arg2 = &mod2;
			break;
		case TextureStage::MODIFIER_ALPHA:
			mod2.x = arg2->w;
			mod2.y = arg2->w;
			mod2.z = arg2->w;
			mod2.w = arg2->w;

			arg2 = &mod2;
			break;
		case TextureStage::MODIFIER_INVALPHA:
			mod2.x = SubSat(Short4(0x1000), arg2->w);
			mod2.y = SubSat(Short4(0x1000), arg2->w);
			mod2.z = SubSat(Short4(0x1000), arg2->w);
			mod2.w = SubSat(Short4(0x1000), arg2->w);

			arg2 = &mod2;
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.thirdModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			mod3.x = SubSat(Short4(0x1000), arg3->x);
			mod3.y = SubSat(Short4(0x1000), arg3->y);
			mod3.z = SubSat(Short4(0x1000), arg3->z);
			mod3.w = SubSat(Short4(0x1000), arg3->w);

			arg3 = &mod3;
			break;
		case TextureStage::MODIFIER_ALPHA:
			mod3.x = arg3->w;
			mod3.y = arg3->w;
			mod3.z = arg3->w;
			mod3.w = arg3->w;

			arg3 = &mod3;
			break;
		case TextureStage::MODIFIER_INVALPHA:
			mod3.x = SubSat(Short4(0x1000), arg3->w);
			mod3.y = SubSat(Short4(0x1000), arg3->w);
			mod3.z = SubSat(Short4(0x1000), arg3->w);
			mod3.w = SubSat(Short4(0x1000), arg3->w);

			arg3 = &mod3;
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
			break;
		case TextureStage::STAGE_SELECTARG1: // Arg1
			res.x = arg1->x;
			res.y = arg1->y;
			res.z = arg1->z;
			break;
		case TextureStage::STAGE_SELECTARG2: // Arg2
			res.x = arg2->x;
			res.y = arg2->y;
			res.z = arg2->z;
			break;
		case TextureStage::STAGE_SELECTARG3: // Arg3
			res.x = arg3->x;
			res.y = arg3->y;
			res.z = arg3->z;
			break;
		case TextureStage::STAGE_MODULATE: // Arg1 * Arg2
			res.x = MulHigh(arg1->x, arg2->x) << 4;
			res.y = MulHigh(arg1->y, arg2->y) << 4;
			res.z = MulHigh(arg1->z, arg2->z) << 4;
			break;
		case TextureStage::STAGE_MODULATE2X: // Arg1 * Arg2 * 2
			res.x = MulHigh(arg1->x, arg2->x) << 5;
			res.y = MulHigh(arg1->y, arg2->y) << 5;
			res.z = MulHigh(arg1->z, arg2->z) << 5;
			break;
		case TextureStage::STAGE_MODULATE4X: // Arg1 * Arg2 * 4
			res.x = MulHigh(arg1->x, arg2->x) << 6;
			res.y = MulHigh(arg1->y, arg2->y) << 6;
			res.z = MulHigh(arg1->z, arg2->z) << 6;
			break;
		case TextureStage::STAGE_ADD: // Arg1 + Arg2
			res.x = AddSat(arg1->x, arg2->x);
			res.y = AddSat(arg1->y, arg2->y);
			res.z = AddSat(arg1->z, arg2->z);
			break;
		case TextureStage::STAGE_ADDSIGNED: // Arg1 + Arg2 - 0.5
			res.x = AddSat(arg1->x, arg2->x);
			res.y = AddSat(arg1->y, arg2->y);
			res.z = AddSat(arg1->z, arg2->z);

			res.x = SubSat(res.x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			res.y = SubSat(res.y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			res.z = SubSat(res.z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			break;
		case TextureStage::STAGE_ADDSIGNED2X: // (Arg1 + Arg2 - 0.5) << 1
			res.x = AddSat(arg1->x, arg2->x);
			res.y = AddSat(arg1->y, arg2->y);
			res.z = AddSat(arg1->z, arg2->z);

			res.x = SubSat(res.x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			res.y = SubSat(res.y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			res.z = SubSat(res.z, Short4(0x0800, 0x0800, 0x0800, 0x0800));

			res.x = AddSat(res.x, res.x);
			res.y = AddSat(res.y, res.y);
			res.z = AddSat(res.z, res.z);
			break;
		case TextureStage::STAGE_SUBTRACT: // Arg1 - Arg2
			res.x = SubSat(arg1->x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z);
			break;
		case TextureStage::STAGE_ADDSMOOTH: // Arg1 + Arg2 - Arg1 * Arg2
			{
				Short4 tmp;

				tmp = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(arg1->x, arg2->x); res.x = SubSat(res.x, tmp);
				tmp = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(arg1->y, arg2->y); res.y = SubSat(res.y, tmp);
				tmp = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(arg1->z, arg2->z); res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_MULTIPLYADD: // Arg3 + Arg1 * Arg2
			res.x = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(res.x, arg3->x);
			res.y = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(res.y, arg3->y);
			res.z = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(res.z, arg3->z);
			break;
		case TextureStage::STAGE_LERP: // Arg3 * (Arg1 - Arg2) + Arg2
			res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, arg3->x) << 4; res.x = AddSat(res.x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, arg3->y) << 4; res.y = AddSat(res.y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, arg3->z) << 4; res.z = AddSat(res.z, arg2->z);
			break;
		case TextureStage::STAGE_DOT3: // 2 * (Arg1.x - 0.5) * 2 * (Arg2.x - 0.5) + 2 * (Arg1.y - 0.5) * 2 * (Arg2.y - 0.5) + 2 * (Arg1.z - 0.5) * 2 * (Arg2.z - 0.5)
			{
				Short4 tmp;

				res.x = SubSat(arg1->x, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->x, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.x = MulHigh(res.x, tmp);
				res.y = SubSat(arg1->y, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->y, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.y = MulHigh(res.y, tmp);
				res.z = SubSat(arg1->z, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->z, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.z = MulHigh(res.z, tmp);

				res.x = res.x << 6;
				res.y = res.y << 6;
				res.z = res.z << 6;

				res.x = AddSat(res.x, res.y);
				res.x = AddSat(res.x, res.z);

				// Clamp to [0, 1]
				res.x = Max(res.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				res.x = Min(res.x, Short4(0x1000));

				res.y = res.x;
				res.z = res.x;
				res.w = res.x;
			}
			break;
		case TextureStage::STAGE_BLENDCURRENTALPHA: // Alpha * (Arg1 - Arg2) + Arg2
			res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, r.current.w) << 4; res.x = AddSat(res.x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, r.current.w) << 4; res.y = AddSat(res.y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, r.current.w) << 4; res.z = AddSat(res.z, arg2->z);
			break;
		case TextureStage::STAGE_BLENDDIFFUSEALPHA: // Alpha * (Arg1 - Arg2) + Arg2
			res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, r.diffuse.w) << 4; res.x = AddSat(res.x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, r.diffuse.w) << 4; res.y = AddSat(res.y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, r.diffuse.w) << 4; res.z = AddSat(res.z, arg2->z);
			break;
		case TextureStage::STAGE_BLENDFACTORALPHA: // Alpha * (Arg1 - Arg2) + Arg2
			res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[3]))) << 4; res.x = AddSat(res.x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[3]))) << 4; res.y = AddSat(res.y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[3]))) << 4; res.z = AddSat(res.z, arg2->z);
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHA: // Alpha * (Arg1 - Arg2) + Arg2
			res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, texture.w) << 4; res.x = AddSat(res.x, arg2->x);
			res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, texture.w) << 4; res.y = AddSat(res.y, arg2->y);
			res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, texture.w) << 4; res.z = AddSat(res.z, arg2->z);
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM: // Arg1 + Arg2 * (1 - Alpha)
			res.x = SubSat(Short4(0x1000), texture.w); res.x = MulHigh(res.x, arg2->x) << 4; res.x = AddSat(res.x, arg1->x);
			res.y = SubSat(Short4(0x1000), texture.w); res.y = MulHigh(res.y, arg2->y) << 4; res.y = AddSat(res.y, arg1->y);
			res.z = SubSat(Short4(0x1000), texture.w); res.z = MulHigh(res.z, arg2->z) << 4; res.z = AddSat(res.z, arg1->z);
			break;
		case TextureStage::STAGE_PREMODULATE:
			res.x = arg1->x;
			res.y = arg1->y;
			res.z = arg1->z;
			break;
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR: // Arg1 + Arg1.w * Arg2
			res.x = MulHigh(arg1->w, arg2->x) << 4; res.x = AddSat(res.x, arg1->x);
			res.y = MulHigh(arg1->w, arg2->y) << 4; res.y = AddSat(res.y, arg1->y);
			res.z = MulHigh(arg1->w, arg2->z) << 4; res.z = AddSat(res.z, arg1->z);
			break;
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA: // Arg1 * Arg2 + Arg1.w
			res.x = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(res.x, arg1->w);
			res.y = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(res.y, arg1->w);
			res.z = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(res.z, arg1->w);
			break;
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR: // (1 - Arg1.w) * Arg2 + Arg1
			{
				Short4 tmp;

				res.x = AddSat(arg1->x, arg2->x); tmp = MulHigh(arg1->w, arg2->x) << 4; res.x = SubSat(res.x, tmp);
				res.y = AddSat(arg1->y, arg2->y); tmp = MulHigh(arg1->w, arg2->y) << 4; res.y = SubSat(res.y, tmp);
				res.z = AddSat(arg1->z, arg2->z); tmp = MulHigh(arg1->w, arg2->z) << 4; res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA: // (1 - Arg1) * Arg2 + Arg1.w
			{
				Short4 tmp;

				res.x = AddSat(arg1->w, arg2->x); tmp = MulHigh(arg1->x, arg2->x) << 4; res.x = SubSat(res.x, tmp);
				res.y = AddSat(arg1->w, arg2->y); tmp = MulHigh(arg1->y, arg2->y) << 4; res.y = SubSat(res.y, tmp);
				res.z = AddSat(arg1->w, arg2->z); tmp = MulHigh(arg1->z, arg2->z) << 4; res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_BUMPENVMAP:
			{
				r.du = Float4(texture.x) * Float4(1.0f / 0x0FE0);
				r.dv = Float4(texture.y) * Float4(1.0f / 0x0FE0);

				Float4 du2;
				Float4 dv2;

				du2 = r.du;
				dv2 = r.dv;
				r.du *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][0]));
				dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][0]));
				r.du += dv2;
				r.dv *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][1]));
				du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][1]));
				r.dv += du2;

				perturbate = true;

				res.x = r.current.x;
				res.y = r.current.y;
				res.z = r.current.z;
				res.w = r.current.w;
			}
			break;
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			{
				r.du = Float4(texture.x) * Float4(1.0f / 0x0FE0);
				r.dv = Float4(texture.y) * Float4(1.0f / 0x0FE0);

				Float4 du2;
				Float4 dv2;

				du2 = r.du;
				dv2 = r.dv;

				r.du *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][0]));
				dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][0]));
				r.du += dv2;
				r.dv *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][1]));
				du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][1]));
				r.dv += du2;

				perturbate = true;

				r.L = texture.z;
				r.L = MulHigh(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].luminanceScale4)));
				r.L = r.L << 4;
				r.L = AddSat(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].luminanceOffset4)));
				r.L = Max(r.L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				r.L = Min(r.L, Short4(0x1000));

				luminance = true;

				res.x = r.current.x;
				res.y = r.current.y;
				res.z = r.current.z;
				res.w = r.current.w;
			}
			break;
		default:
			ASSERT(false);
		}

		if(textureStage.stageOperation != TextureStage::STAGE_DOT3)
		{
			switch(textureStage.firstArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg1 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg1 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg1 = &r.current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg1 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg1 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg1 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg1 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.secondArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg2 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg2 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg2 = &r.current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg2 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg2 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg2 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg2 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.thirdArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg3 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg3 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg3 = &r.current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg3 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg3 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg3 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg3 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.firstModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				mod1.w = SubSat(Short4(0x1000), arg1->w);

				arg1 = &mod1;
				break;
			case TextureStage::MODIFIER_ALPHA:
				// Redudant
				break;
			case TextureStage::MODIFIER_INVALPHA:
				mod1.w = SubSat(Short4(0x1000), arg1->w);

				arg1 = &mod1;
				break;
			default:
				ASSERT(false);
			}

			switch(textureStage.secondModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				mod2.w = SubSat(Short4(0x1000), arg2->w);

				arg2 = &mod2;
				break;
			case TextureStage::MODIFIER_ALPHA:
				// Redudant
				break;
			case TextureStage::MODIFIER_INVALPHA:
				mod2.w = SubSat(Short4(0x1000), arg2->w);

				arg2 = &mod2;
				break;
			default:
				ASSERT(false);
			}

			switch(textureStage.thirdModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				mod3.w = SubSat(Short4(0x1000), arg3->w);

				arg3 = &mod3;
				break;
			case TextureStage::MODIFIER_ALPHA:
				// Redudant
				break;
			case TextureStage::MODIFIER_INVALPHA:
				mod3.w = SubSat(Short4(0x1000), arg3->w);

				arg3 = &mod3;
				break;
			default:
				ASSERT(false);
			}

			switch(textureStage.stageOperationAlpha)
			{
			case TextureStage::STAGE_DISABLE:
				break;
			case TextureStage::STAGE_SELECTARG1: // Arg1
				res.w = arg1->w;
				break;
			case TextureStage::STAGE_SELECTARG2: // Arg2
				res.w = arg2->w;
				break;
			case TextureStage::STAGE_SELECTARG3: // Arg3
				res.w = arg3->w;
				break;
			case TextureStage::STAGE_MODULATE: // Arg1 * Arg2
				res.w = MulHigh(arg1->w, arg2->w) << 4;
				break;
			case TextureStage::STAGE_MODULATE2X: // Arg1 * Arg2 * 2
				res.w = MulHigh(arg1->w, arg2->w) << 5;
				break;
			case TextureStage::STAGE_MODULATE4X: // Arg1 * Arg2 * 4
				res.w = MulHigh(arg1->w, arg2->w) << 6;
				break;
			case TextureStage::STAGE_ADD: // Arg1 + Arg2
				res.w = AddSat(arg1->w, arg2->w);
				break;
			case TextureStage::STAGE_ADDSIGNED: // Arg1 + Arg2 - 0.5
				res.w = AddSat(arg1->w, arg2->w);
				res.w = SubSat(res.w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				break;
			case TextureStage::STAGE_ADDSIGNED2X: // (Arg1 + Arg2 - 0.5) << 1
				res.w = AddSat(arg1->w, arg2->w);
				res.w = SubSat(res.w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.w = AddSat(res.w, res.w);
				break;
			case TextureStage::STAGE_SUBTRACT: // Arg1 - Arg2
				res.w = SubSat(arg1->w, arg2->w);
				break;
			case TextureStage::STAGE_ADDSMOOTH: // Arg1 + Arg2 - Arg1 * Arg2
				{
					Short4 tmp;

					tmp = MulHigh(arg1->w, arg2->w) << 4; res.w = AddSat(arg1->w, arg2->w); res.w = SubSat(res.w, tmp);
				}
				break;
			case TextureStage::STAGE_MULTIPLYADD: // Arg3 + Arg1 * Arg2
				res.w = MulHigh(arg1->w, arg2->w) << 4; res.w = AddSat(res.w, arg3->w);
				break;
			case TextureStage::STAGE_LERP: // Arg3 * (Arg1 - Arg2) + Arg2
				res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, arg3->w) << 4; res.w = AddSat(res.w, arg2->w);
				break;
			case TextureStage::STAGE_DOT3:
				break;   // Already computed in color channel
			case TextureStage::STAGE_BLENDCURRENTALPHA: // Alpha * (Arg1 - Arg2) + Arg2
				res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, r.current.w) << 4; res.w = AddSat(res.w, arg2->w);
				break;
			case TextureStage::STAGE_BLENDDIFFUSEALPHA: // Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, r.diffuse.w) << 4; res.w = AddSat(res.w, arg2->w);
				break;
			case TextureStage::STAGE_BLENDFACTORALPHA:
				res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, *Pointer<Short4>(r.data + OFFSET(DrawData, factor.textureFactor4[3]))) << 4; res.w = AddSat(res.w, arg2->w);
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHA: // Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, texture.w) << 4; res.w = AddSat(res.w, arg2->w);
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHAPM: // Arg1 + Arg2 * (1 - Alpha)
				res.w = SubSat(Short4(0x1000), texture.w); res.w = MulHigh(res.w, arg2->w) << 4; res.w = AddSat(res.w, arg1->w);
				break;
			case TextureStage::STAGE_PREMODULATE:
				res.w = arg1->w;
				break;
			case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
			case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
			case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
			case TextureStage::STAGE_BUMPENVMAP:
			case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
				break;   // Invalid alpha operations
			default:
				ASSERT(false);
			}
		}

		// Clamp result to [0, 1]

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			if(state.textureStage[stage].cantUnderflow)
			{
				break;   // Can't go below zero
			}
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
			res.x = Max(res.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.y = Max(res.y, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.z = Max(res.z, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperationAlpha)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			if(state.textureStage[stage].cantUnderflow)
			{
				break;   // Can't go below zero
			}
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
			res.w = Max(res.w, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			break;   // Can't go above one
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			res.x = Min(res.x, Short4(0x1000));
			res.y = Min(res.y, Short4(0x1000));
			res.z = Min(res.z, Short4(0x1000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperationAlpha)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			break;   // Can't go above one
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			res.w = Min(res.w, Short4(0x1000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.destinationArgument)
		{
		case TextureStage::DESTINATION_CURRENT:
			r.current.x = res.x;
			r.current.y = res.y;
			r.current.z = res.z;
			r.current.w = res.w;
			break;
		case TextureStage::DESTINATION_TEMP:
			temp.x = res.x;
			temp.y = res.y;
			temp.z = res.z;
			temp.w = res.w;
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelPipeline::fogBlend(Registers &r, Vector4s &current, Float4 &f)
	{
		if(!state.fogActive)
		{
			return;
		}

		if(state.pixelFogMode != FOG_NONE)
		{
			pixelFog(r, f);
		}

		UShort4 fog = convertFixed16(f, true);

		current.x = As<Short4>(MulHigh(As<UShort4>(current.x), fog));
		current.y = As<Short4>(MulHigh(As<UShort4>(current.y), fog));
		current.z = As<Short4>(MulHigh(As<UShort4>(current.z), fog));

		UShort4 invFog = UShort4(0xFFFFu) - fog;

		current.x += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData, fog.color4[0]))));
		current.y += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData, fog.color4[1]))));
		current.z += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData, fog.color4[2]))));
	}

	void PixelPipeline::specularPixel(Vector4s &current, Vector4s &specular)
	{
		if(!state.specularAdd)
		{
			return;
		}

		current.x = AddSat(current.x, specular.x);
		current.y = AddSat(current.y, specular.y);
		current.z = AddSat(current.z, specular.z);
	}

	void PixelPipeline::sampleTexture(Registers &r, Vector4s &c, int coordinates, int stage, bool project)
	{
		Float4 u = r.v[2 + coordinates].x;
		Float4 v = r.v[2 + coordinates].y;
		Float4 w = r.v[2 + coordinates].z;
		Float4 q = r.v[2 + coordinates].w;

		if(perturbate)
		{
			u += r.du;
			v += r.dv;

			perturbate = false;
		}

		sampleTexture(r, c, stage, u, v, w, q, project);
	}

	void PixelPipeline::sampleTexture(Registers &r, Vector4s &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project, bool bias)
	{
		Vector4f dsx;
		Vector4f dsy;

		sampleTexture(r, c, stage, u, v, w, q, dsx, dsy, project, bias, false);
	}

	void PixelPipeline::sampleTexture(Registers &r, Vector4s &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project, bool bias, bool gradients, bool lodProvided)
	{
#if PERF_PROFILE
		Long texTime = Ticks();
#endif

		Pointer<Byte> texture = r.data + OFFSET(DrawData, mipmap) + stage * sizeof(Texture);

		if(!project)
		{
			sampler[stage]->sampleTexture(texture, c, u, v, w, q, dsx, dsy, bias, gradients, lodProvided);
		}
		else
		{
			Float4 rq = reciprocal(q);

			Float4 u_q = u * rq;
			Float4 v_q = v * rq;
			Float4 w_q = w * rq;

			sampler[stage]->sampleTexture(texture, c, u_q, v_q, w_q, q, dsx, dsy, bias, gradients, lodProvided);
		}

#if PERF_PROFILE
		r.cycles[PERF_TEX] += Ticks() - texTime;
#endif
	}

	Short4 PixelPipeline::convertFixed12(RValue<Float4> cf)
	{
		return RoundShort4(cf * Float4(0x1000));
	}

	void PixelPipeline::convertFixed12(Vector4s &cs, Vector4f &cf)
	{
		cs.x = convertFixed12(cf.x);
		cs.y = convertFixed12(cf.y);
		cs.z = convertFixed12(cf.z);
		cs.w = convertFixed12(cf.w);
	}

	Float4 PixelPipeline::convertSigned12(Short4 &cs)
	{
		return Float4(cs) * Float4(1.0f / 0x0FFE);
	}

	void PixelPipeline::convertSigned12(Vector4f &cf, Vector4s &cs)
	{
		cf.x = convertSigned12(cs.x);
		cf.y = convertSigned12(cs.y);
		cf.z = convertSigned12(cs.z);
		cf.w = convertSigned12(cs.w);
	}

	void PixelPipeline::writeDestination(Registers &r, Vector4s &d, const Dst &dst)
	{
		switch(dst.type)
		{
		case Shader::PARAMETER_TEMP:
			if(dst.mask & 0x1) r.rs[dst.index].x = d.x;
			if(dst.mask & 0x2) r.rs[dst.index].y = d.y;
			if(dst.mask & 0x4) r.rs[dst.index].z = d.z;
			if(dst.mask & 0x8) r.rs[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_INPUT:
			if(dst.mask & 0x1) r.vs[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vs[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vs[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vs[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_CONST: ASSERT(false); break;
		case Shader::PARAMETER_TEXTURE:
			if(dst.mask & 0x1) r.ts[dst.index].x = d.x;
			if(dst.mask & 0x2) r.ts[dst.index].y = d.y;
			if(dst.mask & 0x4) r.ts[dst.index].z = d.z;
			if(dst.mask & 0x8) r.ts[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_COLOROUT:
			if(dst.mask & 0x1) r.vs[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vs[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vs[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vs[dst.index].w = d.w;
			break;
		default:
			ASSERT(false);
		}
	}

	Vector4s PixelPipeline::fetchRegisterS(Registers &r, const Src &src)
	{
		Vector4s *reg;
		int i = src.index;

		Vector4s c;

		if(src.type == Shader::PARAMETER_CONST)
		{
			c.x = *Pointer<Short4>(r.data + OFFSET(DrawData, ps.cW[i][0]));
			c.y = *Pointer<Short4>(r.data + OFFSET(DrawData, ps.cW[i][1]));
			c.z = *Pointer<Short4>(r.data + OFFSET(DrawData, ps.cW[i][2]));
			c.w = *Pointer<Short4>(r.data + OFFSET(DrawData, ps.cW[i][3]));
		}

		switch(src.type)
		{
		case Shader::PARAMETER_TEMP:          reg = &r.rs[i]; break;
		case Shader::PARAMETER_INPUT:         reg = &r.vs[i]; break;
		case Shader::PARAMETER_CONST:         reg = &c;       break;
		case Shader::PARAMETER_TEXTURE:       reg = &r.ts[i]; break;
		case Shader::PARAMETER_VOID:          return r.rs[0]; // Dummy
		case Shader::PARAMETER_FLOAT4LITERAL: return r.rs[0]; // Dummy
		default: ASSERT(false); return r.rs[0];
		}

		const Short4 &x = (*reg)[(src.swizzle >> 0) & 0x3];
		const Short4 &y = (*reg)[(src.swizzle >> 2) & 0x3];
		const Short4 &z = (*reg)[(src.swizzle >> 4) & 0x3];
		const Short4 &w = (*reg)[(src.swizzle >> 6) & 0x3];

		Vector4s mod;

		switch(src.modifier)
		{
		case Shader::MODIFIER_NONE:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			break;
		case Shader::MODIFIER_BIAS:
			mod.x = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.y = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.z = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.w = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			break;
		case Shader::MODIFIER_BIAS_NEGATE:
			mod.x = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.y = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.z = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.w = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			break;
		case Shader::MODIFIER_COMPLEMENT:
			mod.x = SubSat(Short4(0x1000), x);
			mod.y = SubSat(Short4(0x1000), y);
			mod.z = SubSat(Short4(0x1000), z);
			mod.w = SubSat(Short4(0x1000), w);
			break;
		case Shader::MODIFIER_NEGATE:
			mod.x = -x;
			mod.y = -y;
			mod.z = -z;
			mod.w = -w;
			break;
		case Shader::MODIFIER_X2:
			mod.x = AddSat(x, x);
			mod.y = AddSat(y, y);
			mod.z = AddSat(z, z);
			mod.w = AddSat(w, w);
			break;
		case Shader::MODIFIER_X2_NEGATE:
			mod.x = -AddSat(x, x);
			mod.y = -AddSat(y, y);
			mod.z = -AddSat(z, z);
			mod.w = -AddSat(w, w);
			break;
		case Shader::MODIFIER_SIGN:
			mod.x = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.y = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.z = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.w = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.x = AddSat(mod.x, mod.x);
			mod.y = AddSat(mod.y, mod.y);
			mod.z = AddSat(mod.z, mod.z);
			mod.w = AddSat(mod.w, mod.w);
			break;
		case Shader::MODIFIER_SIGN_NEGATE:
			mod.x = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.y = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.z = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.w = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			mod.x = AddSat(mod.x, mod.x);
			mod.y = AddSat(mod.y, mod.y);
			mod.z = AddSat(mod.z, mod.z);
			mod.w = AddSat(mod.w, mod.w);
			break;
		case Shader::MODIFIER_DZ:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			// Projection performed by texture sampler
			break;
		case Shader::MODIFIER_DW:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			// Projection performed by texture sampler
			break;
		default:
			ASSERT(false);
		}

		if(src.type == Shader::PARAMETER_CONST && (src.modifier == Shader::MODIFIER_X2 || src.modifier == Shader::MODIFIER_X2_NEGATE))
		{
			mod.x = Min(mod.x, Short4(0x1000)); mod.x = Max(mod.x, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.y = Min(mod.y, Short4(0x1000)); mod.y = Max(mod.y, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.z = Min(mod.z, Short4(0x1000)); mod.z = Max(mod.z, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.w = Min(mod.w, Short4(0x1000)); mod.w = Max(mod.w, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
		}

		return mod;
	}

	void PixelPipeline::MOV(Vector4s &dst, Vector4s &src0)
	{
		dst.x = src0.x;
		dst.y = src0.y;
		dst.z = src0.z;
		dst.w = src0.w;
	}

	void PixelPipeline::ADD(Vector4s &dst, Vector4s &src0, Vector4s &src1)
	{
		dst.x = AddSat(src0.x, src1.x);
		dst.y = AddSat(src0.y, src1.y);
		dst.z = AddSat(src0.z, src1.z);
		dst.w = AddSat(src0.w, src1.w);
	}

	void PixelPipeline::SUB(Vector4s &dst, Vector4s &src0, Vector4s &src1)
	{
		dst.x = SubSat(src0.x, src1.x);
		dst.y = SubSat(src0.y, src1.y);
		dst.z = SubSat(src0.z, src1.z);
		dst.w = SubSat(src0.w, src1.w);
	}

	void PixelPipeline::MAD(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{ dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x); }
		{
		dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);
	}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z); }
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w); }
	}

	void PixelPipeline::MUL(Vector4s &dst, Vector4s &src0, Vector4s &src1)
	{
		// FIXME: Long fixed-point multiply fixup
		{ dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); }
		{
		dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y);
	}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); }
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); }
	}

	void PixelPipeline::DP3(Vector4s &dst, Vector4s &src0, Vector4s &src1)
	{
		Short4 t0;
		Short4 t1;

		// FIXME: Long fixed-point multiply fixup
		t0 = MulHigh(src0.x, src1.x); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0);
		t1 = MulHigh(src0.y, src1.y); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1);
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.z, src1.z); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1);
		t0 = AddSat(t0, t1);

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelPipeline::DP4(Vector4s &dst, Vector4s &src0, Vector4s &src1)
	{
		Short4 t0;
		Short4 t1;

		// FIXME: Long fixed-point multiply fixup
		t0 = MulHigh(src0.x, src1.x); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0);
		t1 = MulHigh(src0.y, src1.y); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1);
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.z, src1.z); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1);
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.w, src1.w); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1);
		t0 = AddSat(t0, t1);

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelPipeline::LRP(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{ dst.x = SubSat(src1.x, src2.x); dst.x = MulHigh(dst.x, src0.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x); }
		{
		dst.y = SubSat(src1.y, src2.y); dst.y = MulHigh(dst.y, src0.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);
	}
		{dst.z = SubSat(src1.z, src2.z); dst.z = MulHigh(dst.z, src0.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z); }
		{dst.w = SubSat(src1.w, src2.w); dst.w = MulHigh(dst.w, src0.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w); }
	}

	void PixelPipeline::TEXCOORD(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate)
	{
		Float4 uw;
		Float4 vw;
		Float4 sw;

		if(state.interpolant[2 + coordinate].component & 0x01)
		{
			uw = Max(u, Float4(0.0f));
			uw = Min(uw, Float4(1.0f));
			dst.x = convertFixed12(uw);
		}
		else
		{
			dst.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw = Max(v, Float4(0.0f));
			vw = Min(vw, Float4(1.0f));
			dst.y = convertFixed12(vw);
		}
		else
		{
			dst.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw = Max(s, Float4(0.0f));
			sw = Min(sw, Float4(1.0f));
			dst.z = convertFixed12(sw);
		}
		else
		{
			dst.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		dst.w = Short4(0x1000);
	}

	void PixelPipeline::TEXCRD(Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project)
	{
		Float4 uw = u;
		Float4 vw = v;
		Float4 sw = s;

		if(project)
		{
			uw *= Rcp_pp(s);
			vw *= Rcp_pp(s);
		}

		if(state.interpolant[2 + coordinate].component & 0x01)
		{
			uw *= Float4(0x1000);
			uw = Max(uw, Float4(-0x8000));
			uw = Min(uw, Float4(0x7FFF));
			dst.x = RoundShort4(uw);
		}
		else
		{
			dst.x = Short4(0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw *= Float4(0x1000);
			vw = Max(vw, Float4(-0x8000));
			vw = Min(vw, Float4(0x7FFF));
			dst.y = RoundShort4(vw);
		}
		else
		{
			dst.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw *= Float4(0x1000);
			sw = Max(sw, Float4(-0x8000));
			sw = Min(sw, Float4(0x7FFF));
			dst.z = RoundShort4(sw);
		}
		else
		{
			dst.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}
	}

	void PixelPipeline::TEXDP3(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src)
	{
		TEXM3X3PAD(r, u, v, s, src, 0, false);

		Short4 t0 = RoundShort4(r.u_ * Float4(0x1000));

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelPipeline::TEXDP3TEX(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 0, false);

		r.v_ = Float4(0.0f);
		r.w_ = Float4(0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelPipeline::TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s)
	{
		Int kill = SignMask(CmpNLT(u, Float4(0.0f))) &
			SignMask(CmpNLT(v, Float4(0.0f))) &
			SignMask(CmpNLT(s, Float4(0.0f)));

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelPipeline::TEXKILL(Int cMask[4], Vector4s &src)
	{
		Short4 test = src.x | src.y | src.z;
		Int kill = SignMask(Pack(test, test)) ^ 0x0000000F;

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelPipeline::TEX(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int sampler, bool project)
	{
		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelPipeline::TEXLD(Registers &r, Vector4s &dst, Vector4s &src, int sampler, bool project)
	{
		Float4 u = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src.y) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelPipeline::TEXBEM(Registers &r, Vector4s &dst, Vector4s &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 dv = Float4(src.y) * Float4(1.0f / 0x0FFE);

		Float4 du2 = du;
		Float4 dv2 = dv;

		du *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][0]));
		dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][0]));
		du += dv2;
		dv *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][1]));
		du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][1]));
		dv += du2;

		Float4 u_ = u + du;
		Float4 v_ = v + dv;

		sampleTexture(r, dst, stage, u_, v_, s, s);
	}

	void PixelPipeline::TEXBEML(Registers &r, Vector4s &dst, Vector4s &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 dv = Float4(src.y) * Float4(1.0f / 0x0FFE);

		Float4 du2 = du;
		Float4 dv2 = dv;

		du *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][0]));
		dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][0]));
		du += dv2;
		dv *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[1][1]));
		du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4F[0][1]));
		dv += du2;

		Float4 u_ = u + du;
		Float4 v_ = v + dv;

		sampleTexture(r, dst, stage, u_, v_, s, s);

		Short4 L;

		L = src.z;
		L = MulHigh(L, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].luminanceScale4)));
		L = L << 4;
		L = AddSat(L, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].luminanceOffset4)));
		L = Max(L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		L = Min(L, Short4(0x1000));

		dst.x = MulHigh(dst.x, L); dst.x = dst.x << 4;
		dst.y = MulHigh(dst.y, L); dst.y = dst.y << 4;
		dst.z = MulHigh(dst.z, L); dst.z = dst.z << 4;
	}

	void PixelPipeline::TEXREG2AR(Registers &r, Vector4s &dst, Vector4s &src0, int stage)
	{
		Float4 u = Float4(src0.w) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.x) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src0.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelPipeline::TEXREG2GB(Registers &r, Vector4s &dst, Vector4s &src0, int stage)
	{
		Float4 u = Float4(src0.y) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.z) * Float4(1.0f / 0x0FFE);
		Float4 s = v;

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelPipeline::TEXREG2RGB(Registers &r, Vector4s &dst, Vector4s &src0, int stage)
	{
		Float4 u = Float4(src0.x) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.y) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src0.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelPipeline::TEXM3X2DEPTH(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src, 1, signedScaling);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelPipeline::TEXM3X2PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, int component, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, component, signedScaling);
	}

	void PixelPipeline::TEXM3X2TEX(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src0, 1, signedScaling);

		r.w_ = Float4(0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelPipeline::TEXM3X3(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		dst.x = RoundShort4(r.u_ * Float4(0x1000));
		dst.y = RoundShort4(r.v_ * Float4(0x1000));
		dst.z = RoundShort4(r.w_ * Float4(0x1000));
		dst.w = Short4(0x1000);
	}

	void PixelPipeline::TEXM3X3PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4s &src0, int component, bool signedScaling)
	{
		if(component == 0 || previousScaling != signedScaling)   // FIXME: Other source modifiers?
		{
			r.U = Float4(src0.x);
			r.V = Float4(src0.y);
			r.W = Float4(src0.z);

			previousScaling = signedScaling;
		}

		Float4 x = r.U * u + r.V * v + r.W * s;

		x *= Float4(1.0f / 0x1000);

		switch(component)
		{
		case 0:	r.u_ = x; break;
		case 1:	r.v_ = x; break;
		case 2: r.w_ = x; break;
		default: ASSERT(false);
		}
	}

	void PixelPipeline::TEXM3X3SPEC(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, Vector4s &src1)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = Float4(src1.x) * Float4(1.0f / 0x0FFE);
		E[1] = Float4(src1.y) * Float4(1.0f / 0x0FFE);
		E[2] = Float4(src1.z) * Float4(1.0f / 0x0FFE);

		// Reflection
		Float4 u__;
		Float4 v__;
		Float4 w__;

		// (u'', v'', w'') = 2 * (N . E) * N - E * (N . N)
		u__ = r.u_ * E[0];
		v__ = r.v_ * E[1];
		w__ = r.w_ * E[2];
		u__ += v__ + w__;
		u__ += u__;
		v__ = u__;
		w__ = u__;
		u__ *= r.u_;
		v__ *= r.v_;
		w__ *= r.w_;
		r.u_ *= r.u_;
		r.v_ *= r.v_;
		r.w_ *= r.w_;
		r.u_ += r.v_ + r.w_;
		u__ -= E[0] * r.u_;
		v__ -= E[1] * r.u_;
		w__ -= E[2] * r.u_;

		sampleTexture(r, dst, stage, u__, v__, w__, w__);
	}

	void PixelPipeline::TEXM3X3TEX(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelPipeline::TEXM3X3VSPEC(Registers &r, Vector4s &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4s &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = r.v[2 + stage - 2].w;
		E[1] = r.v[2 + stage - 1].w;
		E[2] = r.v[2 + stage - 0].w;

		// Reflection
		Float4 u__;
		Float4 v__;
		Float4 w__;

		// (u'', v'', w'') = 2 * (N . E) * N - E * (N . N)
		u__ = r.u_ * E[0];
		v__ = r.v_ * E[1];
		w__ = r.w_ * E[2];
		u__ += v__ + w__;
		u__ += u__;
		v__ = u__;
		w__ = u__;
		u__ *= r.u_;
		v__ *= r.v_;
		w__ *= r.w_;
		r.u_ *= r.u_;
		r.v_ *= r.v_;
		r.w_ *= r.w_;
		r.u_ += r.v_ + r.w_;
		u__ -= E[0] * r.u_;
		v__ -= E[1] * r.u_;
		w__ -= E[2] * r.u_;

		sampleTexture(r, dst, stage, u__, v__, w__, w__);
	}

	void PixelPipeline::TEXDEPTH(Registers &r)
	{
		r.u_ = Float4(r.rs[5].x);
		r.v_ = Float4(r.rs[5].y);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelPipeline::CND(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2)
	{
		{ Short4 t0; t0 = src0.x; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.x; t1 = t1 & t0; t0 = ~t0 & src2.x; t0 = t0 | t1; dst.x = t0; };
		{Short4 t0; t0 = src0.y; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.y; t1 = t1 & t0; t0 = ~t0 & src2.y; t0 = t0 | t1; dst.y = t0; };
		{Short4 t0; t0 = src0.z; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.z; t1 = t1 & t0; t0 = ~t0 & src2.z; t0 = t0 | t1; dst.z = t0; };
		{Short4 t0; t0 = src0.w; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.w; t1 = t1 & t0; t0 = ~t0 & src2.w; t0 = t0 | t1; dst.w = t0; };
	}

	void PixelPipeline::CMP(Vector4s &dst, Vector4s &src0, Vector4s &src1, Vector4s &src2)
	{
		{ Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.x); Short4 t1; t1 = src2.x; t1 &= t0; t0 = ~t0 & src1.x; t0 |= t1; dst.x = t0; };
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.y); Short4 t1; t1 = src2.y; t1 &= t0; t0 = ~t0 & src1.y; t0 |= t1; dst.y = t0; };
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.z); Short4 t1; t1 = src2.z; t1 &= t0; t0 = ~t0 & src1.z; t0 |= t1; dst.z = t0; };
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.w); Short4 t1; t1 = src2.w; t1 &= t0; t0 = ~t0 & src1.w; t0 |= t1; dst.w = t0; };
	}

	void PixelPipeline::BEM(Registers &r, Vector4s &dst, Vector4s &src0, Vector4s &src1, int stage)
	{
		Short4 t0;
		Short4 t1;

		// dst.x = src0.x + BUMPENVMAT00(stage) * src1.x + BUMPENVMAT10(stage) * src1.y
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4W[0][0]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4W[1][0]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.x);
		dst.x = t0;

		// dst.y = src0.y + BUMPENVMAT01(stage) * src1.x + BUMPENVMAT11(stage) * src1.y
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4W[0][1]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData, textureStage[stage].bumpmapMatrix4W[1][1]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.y);
		dst.y = t0;
	}
}

