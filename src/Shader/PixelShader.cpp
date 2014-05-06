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

#include "PixelShader.hpp"

#include "Debug.hpp"

namespace sw
{
	PixelShader::PixelShader(const unsigned long *token) : Shader(token)
	{
		parse(token);
	}

	PixelShader::~PixelShader()
	{
	}

	void PixelShader::parse(const unsigned long *token)
	{
		minorVersion = (unsigned char)(token[0] & 0x000000FF);
		majorVersion = (unsigned char)((token[0] & 0x0000FF00) >> 8);
		shaderType = (ShaderType)((token[0] & 0xFFFF0000) >> 16);

		length = validate(token);
		ASSERT(length != 0);

		instruction = new Shader::Instruction*[length];

		for(int i = 0; i < length; i++)
		{
			while((*token & 0x0000FFFF) == 0x0000FFFE)   // Comment token
			{
				int length = (*token & 0x7FFF0000) >> 16;

				token += length + 1;
			}

			int length = size(*token);

			instruction[i] = new Instruction(token, length, majorVersion);

			token += length + 1;
		}

		analyzeZOverride();
		analyzeTexkill();
		analyzeInterpolants();
		analyzeDirtyConstants();
		analyzeDynamicBranching();
		analyzeSamplers();
	}

	int PixelShader::validate(const unsigned long *const token)
	{
		if(!token)
		{
			return 0;
		}

		unsigned short version = (unsigned short)(token[0] & 0x0000FFFF);
		unsigned char minorVersion = (unsigned char)(token[0] & 0x000000FF);
		unsigned char majorVersion = (unsigned char)((token[0] & 0x0000FF00) >> 8);
		ShaderType shaderType = (ShaderType)((token[0] & 0xFFFF0000) >> 16);

		if(shaderType != SHADER_PIXEL || majorVersion > 3)
		{
			return 0;
		}

		int instructionCount = 1;

		for(int i = 0; token[i] != 0x0000FFFF; i++)
		{
			if((token[i] & 0x0000FFFF) == 0x0000FFFE)   // Comment token
			{
				int length = (token[i] & 0x7FFF0000) >> 16;

				i += length;
			}
			else
			{
				ShaderOpcode opcode = (ShaderOpcode)(token[i] & 0x0000FFFF);

				switch(opcode)
				{
				case ShaderOperation::OPCODE_RESERVED0:
				case ShaderOperation::OPCODE_MOVA:
					return 0;   // Unsupported operation
				default:
					instructionCount++;
					break;
				}

				i += size(token[i], version);
			}
		}

		return instructionCount;
	}

	bool PixelShader::depthOverride() const
	{
		return zOverride;
	}

	bool PixelShader::containsTexkill() const
	{
		return texkill;
	}

	bool PixelShader::containsCentroid() const
	{
		return centroid;
	}

	bool PixelShader::usesDiffuse(int component) const
	{
		return semantic[0][component].active();
	}

	bool PixelShader::usesSpecular(int component) const
	{
		return semantic[1][component].active();
	}

	bool PixelShader::usesTexture(int coordinate, int component) const
	{
		return semantic[2 + coordinate][component].active();
	}

	void PixelShader::analyzeZOverride()
	{
		zOverride = false;

		for(int i = 0; i < length; i++)
		{
			if(instruction[i]->getOpcode() == Instruction::Operation::OPCODE_TEXM3X2DEPTH ||
			   instruction[i]->getOpcode() == Instruction::Operation::OPCODE_TEXDEPTH ||
			   instruction[i]->getDestinationParameter().type == Instruction::DestinationParameter::PARAMETER_DEPTHOUT)
			{
				zOverride = true;

				break;
			}
		}
	}

	void PixelShader::analyzeTexkill()
	{
		texkill = false;

		for(int i = 0; i < length; i++)
		{
			if(instruction[i]->getOpcode() == Instruction::Operation::OPCODE_TEXKILL)
			{
				texkill = true;

				break;
			}
		}
	}

	void PixelShader::analyzeInterpolants()
	{
		vPosDeclared = false;
		vFaceDeclared = false;
		centroid = false;

		if(version < 0x0300)
		{
			// Set default mapping; disable unused interpolants below
			semantic[0][0] = Semantic(ShaderOperation::USAGE_COLOR, 0);
			semantic[0][1] = Semantic(ShaderOperation::USAGE_COLOR, 0);
			semantic[0][2] = Semantic(ShaderOperation::USAGE_COLOR, 0);
			semantic[0][3] = Semantic(ShaderOperation::USAGE_COLOR, 0);

			semantic[1][0] = Semantic(ShaderOperation::USAGE_COLOR, 1);
			semantic[1][1] = Semantic(ShaderOperation::USAGE_COLOR, 1);
			semantic[1][2] = Semantic(ShaderOperation::USAGE_COLOR, 1);
			semantic[1][3] = Semantic(ShaderOperation::USAGE_COLOR, 1);

			for(int i = 0; i < 8; i++)
			{
				semantic[2 + i][0] = Semantic(ShaderOperation::USAGE_TEXCOORD, i);
				semantic[2 + i][1] = Semantic(ShaderOperation::USAGE_TEXCOORD, i);
				semantic[2 + i][2] = Semantic(ShaderOperation::USAGE_TEXCOORD, i);
				semantic[2 + i][3] = Semantic(ShaderOperation::USAGE_TEXCOORD, i);
			}

			Instruction::Operation::SamplerType samplerType[16];

			for(int i = 0; i < 16; i++)
			{
				samplerType[i] = Instruction::Operation::SAMPLER_UNKNOWN;
			}

			for(int i = 0; i < length; i++)
			{
				if(instruction[i]->getDestinationParameter().type == Instruction::SourceParameter::PARAMETER_SAMPLER)
				{
					int sampler = instruction[i]->getDestinationParameter().index;

					samplerType[sampler] = instruction[i]->getSamplerType();
				}
			}

			bool interpolant[10][4] = {false};   // Interpolants in use

			for(int i = 0; i < length; i++)
			{
				if(instruction[i]->getDestinationParameter().type == Instruction::SourceParameter::PARAMETER_TEXTURE)
				{	
					int index = instruction[i]->getDestinationParameter().index + 2;
					int mask = instruction[i]->getDestinationParameter().mask;

					switch(instruction[i]->getOpcode())
					{
					case Instruction::Operation::OPCODE_TEX:
					case Instruction::Operation::OPCODE_TEXBEM:
					case Instruction::Operation::OPCODE_TEXBEML:
					case Instruction::Operation::OPCODE_TEXCOORD:
					case Instruction::Operation::OPCODE_TEXDP3:
					case Instruction::Operation::OPCODE_TEXDP3TEX:
					case Instruction::Operation::OPCODE_TEXM3X2DEPTH:
					case Instruction::Operation::OPCODE_TEXM3X2PAD:
					case Instruction::Operation::OPCODE_TEXM3X2TEX:
					case Instruction::Operation::OPCODE_TEXM3X3:
					case Instruction::Operation::OPCODE_TEXM3X3PAD:
					case Instruction::Operation::OPCODE_TEXM3X3TEX:
						interpolant[index][0] = true;
						interpolant[index][1] = true;
						interpolant[index][2] = true;
						break;
					case Instruction::Operation::OPCODE_TEXKILL:
						if(majorVersion < 2)
						{
							interpolant[index][0] = true;
							interpolant[index][1] = true;
							interpolant[index][2] = true;
						}
						else
						{
							interpolant[index][0] = true;
							interpolant[index][1] = true;
							interpolant[index][2] = true;
							interpolant[index][3] = true;
						}
						break;
					case Instruction::Operation::OPCODE_TEXM3X3VSPEC:
						interpolant[index][0] = true;
						interpolant[index][1] = true;
						interpolant[index][2] = true;
						interpolant[index - 2][3] = true;
						interpolant[index - 1][3] = true;
						interpolant[index - 0][3] = true;
						break;
					case Instruction::Operation::OPCODE_DCL:
						break;   // Ignore
					default:   // Arithmetic instruction
						if(version >= 0x0104)
						{
							ASSERT(false);
						}
					}
				}

				for(int argument = 0; argument < 4; argument++)
				{
					if(instruction[i]->getSourceParameter(argument).type == Instruction::SourceParameter::PARAMETER_INPUT ||
					   instruction[i]->getSourceParameter(argument).type == Instruction::SourceParameter::PARAMETER_TEXTURE)
					{
						int index = instruction[i]->getSourceParameter(argument).index;
						int swizzle = instruction[i]->getSourceParameter(argument).swizzle;
						int mask = instruction[i]->getDestinationParameter().mask;
						
						if(instruction[i]->getSourceParameter(argument).type == Instruction::SourceParameter::PARAMETER_TEXTURE)
						{
							index += 2;
						}

						switch(instruction[i]->getOpcode())
						{
						case Instruction::Operation::OPCODE_TEX:
						case Instruction::Operation::OPCODE_TEXLDD:
						case Instruction::Operation::OPCODE_TEXLDL:
							{
								int sampler = instruction[i]->getSourceParameter(1).index;

								switch(samplerType[sampler])
								{
								case Instruction::Operation::SAMPLER_UNKNOWN:
									if(version == 0x0104)
									{
										if((instruction[i]->getSourceParameter(0).swizzle & 0x30) == 0x20)   // .xyz
										{
											interpolant[index][0] = true;
											interpolant[index][1] = true;
											interpolant[index][2] = true;
										}
										else   // .xyw
										{
											interpolant[index][0] = true;
											interpolant[index][1] = true;
											interpolant[index][3] = true;
										}
									}
									else
									{
										ASSERT(false);
									}
									break;
								case Instruction::Operation::SAMPLER_1D:
									interpolant[index][0] = true;
									break;
								case Instruction::Operation::SAMPLER_2D:
									interpolant[index][0] = true;
									interpolant[index][1] = true;
									break;
								case Instruction::Operation::SAMPLER_CUBE:
									interpolant[index][0] = true;
									interpolant[index][1] = true;
									interpolant[index][2] = true;
									break;
								case Instruction::Operation::SAMPLER_VOLUME:
									interpolant[index][0] = true;
									interpolant[index][1] = true;
									interpolant[index][2] = true;
									break;
								default:
									ASSERT(false);
								}

								if(instruction[i]->isBias())
								{
									interpolant[index][3] = true;
								}

								if(instruction[i]->isProject())
								{
									interpolant[index][3] = true;
								}

								if(version == 0x0104 && instruction[i]->getOpcode() == Instruction::Operation::OPCODE_TEX)
								{
									if(instruction[i]->getSourceParameter(0).modifier == Instruction::SourceParameter::MODIFIER_DZ)
									{
										interpolant[index][2] = true;
									}

									if(instruction[i]->getSourceParameter(0).modifier == Instruction::SourceParameter::MODIFIER_DW)
									{
										interpolant[index][3] = true;
									}
								}
							}
							break;
						case Instruction::Operation::OPCODE_M3X2:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
							}

							if(argument == 1)
							{
								if(mask & 0x2)
								{
									interpolant[index + 1][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 1][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 1][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 1][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}
							}
							break;
						case Instruction::Operation::OPCODE_M3X3:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
							}

							if(argument == 1)
							{
								if(mask & 0x2)
								{
									interpolant[index + 1][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 1][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 1][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 1][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}

								if(mask & 0x4)
								{
									interpolant[index + 2][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 2][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 2][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 2][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}
							}
							break;
						case Instruction::Operation::OPCODE_M3X4:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
							}

							if(argument == 1)
							{
								if(mask & 0x2)
								{
									interpolant[index + 1][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 1][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 1][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 1][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}

								if(mask & 0x4)
								{
									interpolant[index + 2][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 2][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 2][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 2][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}

								if(mask & 0x8)
								{
									interpolant[index + 3][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
									interpolant[index + 3][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
									interpolant[index + 3][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
									interpolant[index + 3][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
								}
							}
							break;
						case Instruction::Operation::OPCODE_M4X3:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponent(swizzle, 0);
								interpolant[index][1] |= swizzleContainsComponent(swizzle, 1);
								interpolant[index][2] |= swizzleContainsComponent(swizzle, 2);
								interpolant[index][3] |= swizzleContainsComponent(swizzle, 3);
							}

							if(argument == 1)
							{
								if(mask & 0x2)
								{
									interpolant[index + 1][0] |= swizzleContainsComponent(swizzle, 0);
									interpolant[index + 1][1] |= swizzleContainsComponent(swizzle, 1);
									interpolant[index + 1][2] |= swizzleContainsComponent(swizzle, 2);
									interpolant[index + 1][3] |= swizzleContainsComponent(swizzle, 3);
								}

								if(mask & 0x4)
								{
									interpolant[index + 2][0] |= swizzleContainsComponent(swizzle, 0);
									interpolant[index + 2][1] |= swizzleContainsComponent(swizzle, 1);
									interpolant[index + 2][2] |= swizzleContainsComponent(swizzle, 2);
									interpolant[index + 2][3] |= swizzleContainsComponent(swizzle, 3);
								}
							}
							break;
						case Instruction::Operation::OPCODE_M4X4:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponent(swizzle, 0);
								interpolant[index][1] |= swizzleContainsComponent(swizzle, 1);
								interpolant[index][2] |= swizzleContainsComponent(swizzle, 2);
								interpolant[index][3] |= swizzleContainsComponent(swizzle, 3);
							}

							if(argument == 1)
							{
								if(mask & 0x2)
								{
									interpolant[index + 1][0] |= swizzleContainsComponent(swizzle, 0);
									interpolant[index + 1][1] |= swizzleContainsComponent(swizzle, 1);
									interpolant[index + 1][2] |= swizzleContainsComponent(swizzle, 2);
									interpolant[index + 1][3] |= swizzleContainsComponent(swizzle, 3);
								}

								if(mask & 0x4)
								{
									interpolant[index + 2][0] |= swizzleContainsComponent(swizzle, 0);
									interpolant[index + 2][1] |= swizzleContainsComponent(swizzle, 1);
									interpolant[index + 2][2] |= swizzleContainsComponent(swizzle, 2);
									interpolant[index + 2][3] |= swizzleContainsComponent(swizzle, 3);
								}

								if(mask & 0x8)
								{
									interpolant[index + 3][0] |= swizzleContainsComponent(swizzle, 0);
									interpolant[index + 3][1] |= swizzleContainsComponent(swizzle, 1);
									interpolant[index + 3][2] |= swizzleContainsComponent(swizzle, 2);
									interpolant[index + 3][3] |= swizzleContainsComponent(swizzle, 3);
								}
							}
							break;
						case Instruction::Operation::OPCODE_CRS:
							if(mask & 0x1)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x6);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x6);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x6);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x6);
							}

							if(mask & 0x2)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x5);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x5);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x5);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x5);
							}

							if(mask & 0x4)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x3);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x3);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x3);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x3);
							}
							break;
						case Instruction::Operation::OPCODE_DP2ADD:
							if(argument == 0 || argument == 1)
							{
								interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x3);
								interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x3);
								interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x3);
								interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x3);
							}
							else   // argument == 2
							{
								interpolant[index][0] |= swizzleContainsComponent(swizzle, 0);
								interpolant[index][1] |= swizzleContainsComponent(swizzle, 1);
								interpolant[index][2] |= swizzleContainsComponent(swizzle, 2);
								interpolant[index][3] |= swizzleContainsComponent(swizzle, 3);
							}
							break;
						case Instruction::Operation::OPCODE_DP3:
							interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7);
							interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7);
							interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7);
							interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7);
							break;
						case Instruction::Operation::OPCODE_DP4:
							interpolant[index][0] |= swizzleContainsComponent(swizzle, 0);
							interpolant[index][1] |= swizzleContainsComponent(swizzle, 1);
							interpolant[index][2] |= swizzleContainsComponent(swizzle, 2);
							interpolant[index][3] |= swizzleContainsComponent(swizzle, 3);
							break;
						case Instruction::Operation::OPCODE_SINCOS:
						case Instruction::Operation::OPCODE_EXP:
						case Instruction::Operation::OPCODE_LOG:
						case Instruction::Operation::OPCODE_POW:
						case Instruction::Operation::OPCODE_RCP:
						case Instruction::Operation::OPCODE_RSQ:
							interpolant[index][0] |= swizzleContainsComponent(swizzle, 0);
							interpolant[index][1] |= swizzleContainsComponent(swizzle, 1);
							interpolant[index][2] |= swizzleContainsComponent(swizzle, 2);
							interpolant[index][3] |= swizzleContainsComponent(swizzle, 3);
							break;
						case Instruction::Operation::OPCODE_NRM:
							interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, 0x7 | mask);
							interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, 0x7 | mask);
							interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, 0x7 | mask);
							interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, 0x7 | mask);
							break;
						case Instruction::Operation::OPCODE_MOV:
						case Instruction::Operation::OPCODE_ADD:
						case Instruction::Operation::OPCODE_SUB:
						case Instruction::Operation::OPCODE_MUL:
						case Instruction::Operation::OPCODE_MAD:
						case Instruction::Operation::OPCODE_ABS:
						case Instruction::Operation::OPCODE_CMP:
						case Instruction::Operation::OPCODE_CND:
						case Instruction::Operation::OPCODE_FRC:
						case Instruction::Operation::OPCODE_LRP:
						case Instruction::Operation::OPCODE_MAX:
						case Instruction::Operation::OPCODE_MIN:
						case Instruction::Operation::OPCODE_SETP:
						case Instruction::Operation::OPCODE_BREAKC:
						case Instruction::Operation::OPCODE_DSX:
						case Instruction::Operation::OPCODE_DSY:
							interpolant[index][0] |= swizzleContainsComponentMasked(swizzle, 0, mask);
							interpolant[index][1] |= swizzleContainsComponentMasked(swizzle, 1, mask);
							interpolant[index][2] |= swizzleContainsComponentMasked(swizzle, 2, mask);
							interpolant[index][3] |= swizzleContainsComponentMasked(swizzle, 3, mask);
							break;
						case Instruction::Operation::OPCODE_TEXCOORD:
							interpolant[index][0] = true;
							interpolant[index][1] = true;
							interpolant[index][2] = true;
							interpolant[index][3] = true;
							break;
						case Instruction::Operation::OPCODE_TEXDP3:
						case Instruction::Operation::OPCODE_TEXDP3TEX:
						case Instruction::Operation::OPCODE_TEXM3X2PAD:
						case Instruction::Operation::OPCODE_TEXM3X3PAD:
						case Instruction::Operation::OPCODE_TEXM3X2TEX:
						case Instruction::Operation::OPCODE_TEXM3X3SPEC:
						case Instruction::Operation::OPCODE_TEXM3X3VSPEC:
						case Instruction::Operation::OPCODE_TEXBEM:
						case Instruction::Operation::OPCODE_TEXBEML:
						case Instruction::Operation::OPCODE_TEXM3X2DEPTH:
						case Instruction::Operation::OPCODE_TEXM3X3:
						case Instruction::Operation::OPCODE_TEXM3X3TEX:
							interpolant[index][0] = true;
							interpolant[index][1] = true;
							interpolant[index][2] = true;
							break;
						case Instruction::Operation::OPCODE_TEXREG2AR:
						case Instruction::Operation::OPCODE_TEXREG2GB:
						case Instruction::Operation::OPCODE_TEXREG2RGB:
							break;
						default:
						//	ASSERT(false);   // Refine component usage
							interpolant[index][0] = true;
							interpolant[index][1] = true;
							interpolant[index][2] = true;
							interpolant[index][3] = true;
						}
					}
				}
			}

			for(int index = 0; index < 10; index++)
			{
				for(int component = 0; component < 4; component++)
				{
					if(!interpolant[index][component])
					{
						semantic[index][component] = Semantic();
					}
				}
			}
		}
		else   // Shader Model 3.0 input declaration; v# indexable
		{
			for(int i = 0; i < length; i++)
			{
				if(instruction[i]->getOpcode() == ShaderOperation::OPCODE_DCL)
				{
					if(instruction[i]->getDestinationParameter().type == ShaderParameter::PARAMETER_INPUT)
					{
						unsigned char usage = instruction[i]->getUsage();
						unsigned char index = instruction[i]->getUsageIndex();
						unsigned char mask = instruction[i]->getDestinationParameter().mask;
						unsigned char reg = instruction[i]->getDestinationParameter().index;

						if(mask & 0x01)
						{
							semantic[reg][0] = Semantic(usage, index);
						}

						if(mask & 0x02)
						{
							semantic[reg][1] = Semantic(usage, index);
						}

						if(mask & 0x04)
						{
							semantic[reg][2] = Semantic(usage, index);
						}

						if(mask & 0x08)
						{
							semantic[reg][3] = Semantic(usage, index);
						}
					}
					else if(instruction[i]->getDestinationParameter().type == ShaderParameter::PARAMETER_MISCTYPE)
					{
						unsigned char index = instruction[i]->getDestinationParameter().index;

						if(index == 0)
						{
							vPosDeclared = true;
						}
						else if(index == 1)
						{
							vFaceDeclared = true;
						}
						else ASSERT(false);
					}
				}
			}
		}

		if(version >= 0x0200)
		{
			for(int i = 0; i < length; i++)
			{
				if(instruction[i]->getOpcode() == ShaderOperation::OPCODE_DCL)
				{
					bool centroid = instruction[i]->getDestinationParameter().centroid;
					unsigned char reg = instruction[i]->getDestinationParameter().index;

					switch(instruction[i]->getDestinationParameter().type)
					{
					case ShaderParameter::PARAMETER_INPUT:
						semantic[reg][0].centroid = centroid;
						break;
					case ShaderParameter::PARAMETER_TEXTURE:
						semantic[2 + reg][0].centroid = centroid;
						break;
					}

					this->centroid = this->centroid || centroid;
				}
			}
		}
	}
}
