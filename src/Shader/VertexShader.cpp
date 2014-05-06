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

#include "VertexShader.hpp"

#include "Vertex.hpp"
#include "Debug.hpp"

namespace sw
{
	VertexShader::VertexShader(const unsigned long *token) : Shader(token)
	{
		parse(token);
	}

	VertexShader::~VertexShader()
	{
	}

	void VertexShader::parse(const unsigned long *token)
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

			int tokenCount = size(*token);

			instruction[i] = new Instruction(token, tokenCount, majorVersion);

			token += 1 + tokenCount;
		}

		analyzeInput();
		analyzeOutput();
		analyzeDirtyConstants();
		analyzeTexldl();
		analyzeDynamicBranching();
		analyzeSamplers();
	}

	int VertexShader::validate(const unsigned long *const token)
	{
		if(!token)
		{
			return 0;
		}

		unsigned short version = (unsigned short)(token[0] & 0x0000FFFF);
		unsigned char minorVersion = (unsigned char)(token[0] & 0x000000FF);
		unsigned char majorVersion = (unsigned char)((token[0] & 0x0000FF00) >> 8);
		ShaderType shaderType = (ShaderType)((token[0] & 0xFFFF0000) >> 16);

		if(shaderType != SHADER_VERTEX || majorVersion > 3)
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
				case ShaderOperation::OPCODE_TEXCOORD:
				case ShaderOperation::OPCODE_TEXKILL:
				case ShaderOperation::OPCODE_TEX:
				case ShaderOperation::OPCODE_TEXBEM:
				case ShaderOperation::OPCODE_TEXBEML:
				case ShaderOperation::OPCODE_TEXREG2AR:
				case ShaderOperation::OPCODE_TEXREG2GB:
				case ShaderOperation::OPCODE_TEXM3X2PAD:
				case ShaderOperation::OPCODE_TEXM3X2TEX:
				case ShaderOperation::OPCODE_TEXM3X3PAD:
				case ShaderOperation::OPCODE_TEXM3X3TEX:
				case ShaderOperation::OPCODE_RESERVED0:
				case ShaderOperation::OPCODE_TEXM3X3SPEC:
				case ShaderOperation::OPCODE_TEXM3X3VSPEC:
				case ShaderOperation::OPCODE_TEXREG2RGB:
				case ShaderOperation::OPCODE_TEXDP3TEX:
				case ShaderOperation::OPCODE_TEXM3X2DEPTH:
				case ShaderOperation::OPCODE_TEXDP3:
				case ShaderOperation::OPCODE_TEXM3X3:
				case ShaderOperation::OPCODE_TEXDEPTH:
				case ShaderOperation::OPCODE_CMP:
				case ShaderOperation::OPCODE_BEM:
				case ShaderOperation::OPCODE_DP2ADD:
				case ShaderOperation::OPCODE_DSX:
				case ShaderOperation::OPCODE_DSY:
				case ShaderOperation::OPCODE_TEXLDD:
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

	bool VertexShader::containsTexldl() const
	{
		return texldl;
	}

	void VertexShader::analyzeInput()
	{
		for(int i = 0; i < 16; i++)
		{
			input[i] = Semantic(-1, -1);
		}

		for(int i = 0; i < length; i++)
		{
			if(instruction[i]->getOpcode() == ShaderOperation::OPCODE_DCL &&
			   instruction[i]->getDestinationParameter().type == ShaderParameter::PARAMETER_INPUT)
			{
				int index = instruction[i]->getDestinationParameter().index;

				input[index] = Semantic(instruction[i]->getUsage(), instruction[i]->getUsageIndex());
			}
		}
	}

	void VertexShader::analyzeOutput()
	{
		positionRegister = Pos;
		pointSizeRegister = -1;   // No vertex point size

		if(version < 0x0300)
		{
			output[Pos][0] = Semantic(ShaderOperation::USAGE_POSITION, 0);
			output[Pos][1] = Semantic(ShaderOperation::USAGE_POSITION, 0);
			output[Pos][2] = Semantic(ShaderOperation::USAGE_POSITION, 0);
			output[Pos][3] = Semantic(ShaderOperation::USAGE_POSITION, 0);

			for(int i = 0; i < length; i++)
			{
				const Instruction::DestinationParameter &dst = instruction[i]->getDestinationParameter();

				switch(dst.type)
				{
				case ShaderParameter::PARAMETER_RASTOUT:
					switch(dst.index)
					{
					case 0:
						// Position already assumed written
						break;
					case 1:
						output[Fog][0] = Semantic(ShaderOperation::USAGE_FOG, 0);
						break;
					case 2:
						output[Pts][1] = Semantic(ShaderOperation::USAGE_PSIZE, 0);
						pointSizeRegister = Pts;
						break;
					default: ASSERT(false);
					}
					break;
				case ShaderParameter::PARAMETER_ATTROUT:
					if(dst.index == 0)
					{
						if(dst.x) output[D0][0] = Semantic(ShaderOperation::USAGE_COLOR, 0);
						if(dst.y) output[D0][1] = Semantic(ShaderOperation::USAGE_COLOR, 0);
						if(dst.z) output[D0][2] = Semantic(ShaderOperation::USAGE_COLOR, 0);
						if(dst.w) output[D0][3] = Semantic(ShaderOperation::USAGE_COLOR, 0);
					}
					else if(dst.index == 1)
					{
						if(dst.x) output[D1][0] = Semantic(ShaderOperation::USAGE_COLOR, 1);
						if(dst.y) output[D1][1] = Semantic(ShaderOperation::USAGE_COLOR, 1);
						if(dst.z) output[D1][2] = Semantic(ShaderOperation::USAGE_COLOR, 1);
						if(dst.w) output[D1][3] = Semantic(ShaderOperation::USAGE_COLOR, 1);
					}
					else ASSERT(false);
					break;
				case ShaderParameter::PARAMETER_TEXCRDOUT:
					if(dst.x) output[T0 + dst.index][0] = Semantic(ShaderOperation::USAGE_TEXCOORD, dst.index);
					if(dst.y) output[T0 + dst.index][1] = Semantic(ShaderOperation::USAGE_TEXCOORD, dst.index);
					if(dst.z) output[T0 + dst.index][2] = Semantic(ShaderOperation::USAGE_TEXCOORD, dst.index);
					if(dst.w) output[T0 + dst.index][3] = Semantic(ShaderOperation::USAGE_TEXCOORD, dst.index);	
					break;
				default:
					break;
				}
			}
		}
		else   // Shader Model 3.0 input declaration
		{
			for(int i = 0; i < length; i++)
			{
				if(instruction[i]->getOpcode() == ShaderOperation::OPCODE_DCL &&
				   instruction[i]->getDestinationParameter().type == ShaderParameter::PARAMETER_OUTPUT)
				{
					unsigned char usage = instruction[i]->getUsage();
					unsigned char usageIndex = instruction[i]->getUsageIndex();

					const Instruction::DestinationParameter &dst = instruction[i]->getDestinationParameter();

					if(dst.x) output[dst.index][0] = Semantic(usage, usageIndex);
					if(dst.y) output[dst.index][1] = Semantic(usage, usageIndex);
					if(dst.z) output[dst.index][2] = Semantic(usage, usageIndex);
					if(dst.w) output[dst.index][3] = Semantic(usage, usageIndex);

					if(usage == ShaderOperation::USAGE_POSITION && usageIndex == 0)
					{
						positionRegister = dst.index;
					}

					if(usage == ShaderOperation::USAGE_PSIZE && usageIndex == 0)
					{
						pointSizeRegister = dst.index;
					}
				}
			}
		}
	}

	void VertexShader::analyzeTexldl()
	{
		texldl = false;

		for(int i = 0; i < length; i++)
		{
			if(instruction[i]->getOpcode() == Instruction::Operation::OPCODE_TEXLDL)
			{
				texldl = true;

				break;
			}
		}
	}
}
