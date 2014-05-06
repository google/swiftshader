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

#include "Shader.hpp"

#include "Math.hpp"
#include "Debug.hpp"

#include <stdarg.h>
#include <fstream>
#include <sstream>

namespace sw
{
	Shader::Instruction::Instruction()
	{
		operation.opcode = Operation::OPCODE_NOP;
		destinationParameter.type = Parameter::PARAMETER_VOID;
		sourceParameter[0].type = Parameter::PARAMETER_VOID;
		sourceParameter[1].type = Parameter::PARAMETER_VOID;
		sourceParameter[2].type = Parameter::PARAMETER_VOID;
		sourceParameter[3].type = Parameter::PARAMETER_VOID;
	}

	Shader::Instruction::Instruction(const unsigned long *token, int size, unsigned char majorVersion)
	{
		parseOperationToken(*token++, majorVersion);

		if(operation.opcode == Operation::OPCODE_IF ||
		   operation.opcode == Operation::OPCODE_IFC ||
		   operation.opcode == Operation::OPCODE_LOOP ||
		   operation.opcode == Operation::OPCODE_REP ||
		   operation.opcode == Operation::OPCODE_BREAKC ||
		   operation.opcode == Operation::OPCODE_BREAKP)   // No destination operand
		{
			if(size > 0) parseSourceToken(0, token++, majorVersion);
			if(size > 1) parseSourceToken(1, token++, majorVersion);
			if(size > 2) parseSourceToken(2, token++, majorVersion);
			if(size > 3) ASSERT(false);
		}
		else if(operation.opcode == Operation::OPCODE_DCL)
		{
			parseDeclarationToken(*token++);
			parseDestinationToken(token++, majorVersion);
		}
		else
		{
			if(size > 0)
			{
				parseDestinationToken(token, majorVersion);

				if(destinationParameter.relative && majorVersion >= 3)
				{
					token++;
					size--;
				}
				
				token++;
				size--;
			}

			if(operation.predicate)
			{
				ASSERT(size != 0);

				operation.predicateNot = (SourceParameter::Modifier)((*token & 0x0F000000) >> 24) == SourceParameter::MODIFIER_NOT;
				operation.predicateSwizzle = (unsigned char)((*token & 0x00FF0000) >> 16);
				
				token++;
				size--;
			}

			for(int i = 0; size > 0; i++)
			{
				parseSourceToken(i, token, majorVersion);

				token++;
				size--;

				if(sourceParameter[i].relative && majorVersion >= 2)
				{
					token++;
					size--;
				}
			}
		}
	}

	Shader::Instruction::~Instruction()
	{
	}

	Shader::Instruction::Operation::Opcode Shader::Instruction::getOpcode() const
	{
		return operation.opcode;
	}

	const Shader::Instruction::DestinationParameter &Shader::Instruction::getDestinationParameter() const
	{
		return destinationParameter;
	}

	const Shader::Instruction::SourceParameter &Shader::Instruction::getSourceParameter(int i) const
	{
		return sourceParameter[i];
	}

	bool Shader::Instruction::isCoissue() const
	{
		return operation.coissue;
	}

	bool Shader::Instruction::isProject() const
	{
		return operation.project;
	}

	bool Shader::Instruction::isBias() const
	{
		return operation.bias;
	}

	bool Shader::Instruction::isPredicate() const
	{
		return operation.predicate;
	}

	bool Shader::Instruction::isPredicateNot() const
	{
		return operation.predicateNot;
	}

	unsigned char Shader::Instruction::getPredicateSwizzle() const
	{
		return operation.predicateSwizzle;
	}

	Shader::Instruction::Operation::Control Shader::Instruction::getControl() const
	{
		return operation.control;
	}

	Shader::Instruction::Operation::Usage Shader::Instruction::getUsage() const
	{
		return operation.usage;
	}

	unsigned char Shader::Instruction::getUsageIndex() const
	{
		return operation.usageIndex;
	}

	Shader::Instruction::Operation::SamplerType Shader::Instruction::getSamplerType() const
	{
		return operation.samplerType;
	}

	std::string Shader::Instruction::string(ShaderType shaderType, unsigned short version) const
	{
		std::string instructionString;
		
		if(operation.opcode != Operation::OPCODE_DCL)
		{
			instructionString += operation.coissue ? "+ " : "";
			
			if(operation.predicate)
			{
				instructionString += operation.predicateNot ? "(!p0" : "(p0";
				instructionString += swizzleString(Parameter::PARAMETER_PREDICATE, operation.predicateSwizzle);
				instructionString += ") ";
			}

			instructionString += operation.string(version) + operation.controlString() + destinationParameter.shiftString() + destinationParameter.modifierString();

			if(destinationParameter.type != Parameter::PARAMETER_VOID)
			{
				instructionString += " " + destinationParameter.string(shaderType, version) +
				                           destinationParameter.relativeString() +
				                           destinationParameter.maskString(); 
			}

			for(int i = 0; i < 4; i++)
			{
				if(sourceParameter[i].type != Parameter::PARAMETER_VOID)
				{
					instructionString += (destinationParameter.type != Parameter::PARAMETER_VOID || i > 0) ? ", " : " ";
					instructionString += sourceParameter[i].preModifierString() +
										 sourceParameter[i].string(shaderType, version) +
										 sourceParameter[i].relativeString() + 
										 sourceParameter[i].postModifierString() + 
										 sourceParameter[i].swizzleString();
				}
			}
		}
		else   // DCL
		{
			instructionString += "dcl";

			if(destinationParameter.type == Parameter::PARAMETER_SAMPLER)
			{
				switch(operation.samplerType)
				{
				case Operation::SAMPLER_UNKNOWN:	instructionString += " ";			break;
				case Operation::SAMPLER_1D:			instructionString += "_1d ";		break;
				case Operation::SAMPLER_2D:			instructionString += "_2d ";		break;
				case Operation::SAMPLER_CUBE:		instructionString += "_cube ";		break;
				case Operation::SAMPLER_VOLUME:		instructionString += "_volume ";	break;
				default:
					ASSERT(false);
				}

				instructionString += destinationParameter.string(shaderType, version);
			}
			else if(destinationParameter.type == Parameter::PARAMETER_INPUT ||
				    destinationParameter.type == Parameter::PARAMETER_OUTPUT ||
				    destinationParameter.type == Parameter::PARAMETER_TEXTURE)
			{
				if(version >= 0x0300)
				{
					switch(operation.usage)
					{
					case Operation::USAGE_POSITION:		instructionString += "_position";		break;
					case Operation::USAGE_BLENDWEIGHT:	instructionString += "_blendweight";	break;
					case Operation::USAGE_BLENDINDICES:	instructionString += "_blendindices";	break;
					case Operation::USAGE_NORMAL:		instructionString += "_normal";			break;
					case Operation::USAGE_PSIZE:		instructionString += "_psize";			break;
					case Operation::USAGE_TEXCOORD:		instructionString += "_texcoord";		break;
					case Operation::USAGE_TANGENT:		instructionString += "_tangent";		break;
					case Operation::USAGE_BINORMAL:		instructionString += "_binormal";		break;
					case Operation::USAGE_TESSFACTOR:	instructionString += "_tessfactor";		break;
					case Operation::USAGE_POSITIONT:	instructionString += "_positiont";		break;
					case Operation::USAGE_COLOR:		instructionString += "_color";			break;
					case Operation::USAGE_FOG:			instructionString += "_fog";			break;
					case Operation::USAGE_DEPTH:		instructionString += "_depth";			break;
					case Operation::USAGE_SAMPLE:		instructionString += "_sample";			break;
					default:
						ASSERT(false);
					}

					if(operation.usageIndex > 0)
					{
						std::ostringstream buffer;

						buffer << (int)operation.usageIndex;

						instructionString += buffer.str();
					}
				}
				else ASSERT(destinationParameter.type != Parameter::PARAMETER_OUTPUT);

				instructionString += " ";

				instructionString += destinationParameter.string(shaderType, version);
				instructionString += destinationParameter.maskString();
			}
			else if(destinationParameter.type == Parameter::PARAMETER_MISCTYPE)   // vPos and vFace
			{
				instructionString += " ";

				instructionString += destinationParameter.string(shaderType, version);
			}
			else ASSERT(false);
		}

		return instructionString;
	}

	std::string Shader::Instruction::Operation::string(unsigned short version) const
	{
		switch(opcode)
		{
		case OPCODE_NOP:			return "nop";
		case OPCODE_MOV:			return "mov";
		case OPCODE_ADD:			return "add";
		case OPCODE_SUB:			return "sub";
		case OPCODE_MAD:			return "mad";
		case OPCODE_MUL:			return "mul";
		case OPCODE_RCP:			return "rcp";
		case OPCODE_RSQ:			return "rsq";
		case OPCODE_DP3:			return "dp3";
		case OPCODE_DP4:			return "dp4";
		case OPCODE_MIN:			return "min";
		case OPCODE_MAX:			return "max";
		case OPCODE_SLT:			return "slt";
		case OPCODE_SGE:			return "sge";
		case OPCODE_EXP:			return "exp";
		case OPCODE_LOG:			return "log";
		case OPCODE_LIT:			return "lit";
		case OPCODE_DST:			return "dst";
		case OPCODE_LRP:			return "lrp";
		case OPCODE_FRC:			return "frc";
		case OPCODE_M4X4:			return "m4x4";
		case OPCODE_M4X3:			return "m4x3";
		case OPCODE_M3X4:			return "m3x4";
		case OPCODE_M3X3:			return "m3x3";
		case OPCODE_M3X2:			return "m3x2";
		case OPCODE_CALL:			return "call";
		case OPCODE_CALLNZ:			return "callnz";
		case OPCODE_LOOP:			return "loop";
		case OPCODE_RET:			return "ret";
		case OPCODE_ENDLOOP:		return "endloop";
		case OPCODE_LABEL:			return "label";
		case OPCODE_DCL:			return "dcl";
		case OPCODE_POW:			return "pow";
		case OPCODE_CRS:			return "crs";
		case OPCODE_SGN:			return "sgn";
		case OPCODE_ABS:			return "abs";
		case OPCODE_NRM:			return "nrm";
		case OPCODE_SINCOS:			return "sincos";
		case OPCODE_REP:			return "rep";
		case OPCODE_ENDREP:			return "endrep";
		case OPCODE_IF:				return "if";
		case OPCODE_IFC:			return "ifc";
		case OPCODE_ELSE:			return "else";
		case OPCODE_ENDIF:			return "endif";
		case OPCODE_BREAK:			return "break";
		case OPCODE_BREAKC:			return "breakc";
		case OPCODE_MOVA:			return "mova";
		case OPCODE_DEFB:			return "defb";
		case OPCODE_DEFI:			return "defi";
		case OPCODE_TEXCOORD:		return "texcoord";
		case OPCODE_TEXKILL:		return "texkill";
		case OPCODE_TEX:
			if(version < 0x0104)	return "tex";
			else					return "texld";
		case OPCODE_TEXBEM:			return "texbem";
		case OPCODE_TEXBEML:		return "texbeml";
		case OPCODE_TEXREG2AR:		return "texreg2ar";
		case OPCODE_TEXREG2GB:		return "texreg2gb";
		case OPCODE_TEXM3X2PAD:		return "texm3x2pad";
		case OPCODE_TEXM3X2TEX:		return "texm3x2tex";
		case OPCODE_TEXM3X3PAD:		return "texm3x3pad";
		case OPCODE_TEXM3X3TEX:		return "texm3x3tex";
		case OPCODE_RESERVED0:		return "reserved0";
		case OPCODE_TEXM3X3SPEC:	return "texm3x3spec";
		case OPCODE_TEXM3X3VSPEC:	return "texm3x3vspec";
		case OPCODE_EXPP:			return "expp";
		case OPCODE_LOGP:			return "logp";
		case OPCODE_CND:			return "cnd";
		case OPCODE_DEF:			return "def";
		case OPCODE_TEXREG2RGB:		return "texreg2rgb";
		case OPCODE_TEXDP3TEX:		return "texdp3tex";
		case OPCODE_TEXM3X2DEPTH:	return "texm3x2depth";
		case OPCODE_TEXDP3:			return "texdp3";
		case OPCODE_TEXM3X3:		return "texm3x3";
		case OPCODE_TEXDEPTH:		return "texdepth";
		case OPCODE_CMP:			return "cmp";
		case OPCODE_BEM:			return "bem";
		case OPCODE_DP2ADD:			return "dp2add";
		case OPCODE_DSX:			return "dsx";
		case OPCODE_DSY:			return "dsy";
		case OPCODE_TEXLDD:			return "texldd";
		case OPCODE_SETP:			return "setp";
		case OPCODE_TEXLDL:			return "texldl";
		case OPCODE_BREAKP:			return "breakp";
		case OPCODE_PHASE:			return "phase";
		case OPCODE_COMMENT:		return "comment";
		case OPCODE_END:			return "end";
		case OPCODE_PS_1_0:			return "ps_1_0";
		case OPCODE_PS_1_1:			return "ps_1_1";
		case OPCODE_PS_1_2:			return "ps_1_2";
		case OPCODE_PS_1_3:			return "ps_1_3";
		case OPCODE_PS_1_4:			return "ps_1_4";
		case OPCODE_PS_2_0:			return "ps_2_0";
		case OPCODE_PS_2_x:			return "ps_2_x";
		case OPCODE_PS_3_0:			return "ps_3_0";
		case OPCODE_VS_1_0:			return "vs_1_0";
		case OPCODE_VS_1_1:			return "vs_1_1";
		case OPCODE_VS_2_0:			return "vs_2_0";
		case OPCODE_VS_2_x:			return "vs_2_x";
		case OPCODE_VS_2_sw:		return "vs_2_sw";
		case OPCODE_VS_3_0:			return "vs_3_0";
		case OPCODE_VS_3_sw:		return "vs_3_sw";
		default:
			ASSERT(false);
		}

		return "<unknown>";
	}

	std::string Shader::Instruction::Operation::controlString() const
	{
		if(opcode != OPCODE_LOOP && opcode != OPCODE_BREAKC && opcode != OPCODE_IFC && opcode != OPCODE_SETP)
		{
			if(project) return "p";

			if(bias) return "b";

			// FIXME: LOD
		}

		switch(control)
		{
		case 1: return "_gt";
		case 2: return "_eq";
		case 3: return "_ge";
		case 4: return "_lt";
		case 5: return "_ne";
		case 6: return "_le";
		default:
			return "";
		//	ASSERT(false);   // FIXME
		}
	}

	std::string Shader::Instruction::DestinationParameter::modifierString() const
	{
		if(type == PARAMETER_VOID || type == PARAMETER_LABEL)
		{
			return "";
		}

		std::string modifierString;

		if(saturate)
		{
			modifierString += "_sat";
		}

		if(partialPrecision)
		{
			modifierString += "_pp";
		}

		if(centroid)
		{
			modifierString += "_centroid";
		}

		return modifierString;
	}

	std::string Shader::Instruction::DestinationParameter::shiftString() const
	{
		if(type == PARAMETER_VOID || type == PARAMETER_LABEL)
		{
			return "";
		}

		switch(shift)
		{
		case 0:		return "";
		case 1:		return "_x2";
		case 2:		return "_x4"; 
		case 3:		return "_x8";
		case -1:	return "_d2";
		case -2:	return "_d4"; 
		case -3:	return "_d8";
		default:
			return "";
		//	ASSERT(false);   // FIXME
		}
	}

	std::string Shader::Instruction::DestinationParameter::maskString() const
	{
		if(type == PARAMETER_VOID || type == PARAMETER_LABEL)
		{
			return "";
		}

		switch(mask)
		{
		case 0x0:	return "";
		case 0x1:	return ".x";
		case 0x2:	return ".y";
		case 0x3:	return ".xy";
		case 0x4:	return ".z";
		case 0x5:	return ".xz";
		case 0x6:	return ".yz";
		case 0x7:	return ".xyz";
		case 0x8:	return ".w";
		case 0x9:	return ".xw";
		case 0xA:	return ".yw";
		case 0xB:	return ".xyw";
		case 0xC:	return ".zw";
		case 0xD:	return ".xzw";
		case 0xE:	return ".yzw";
		case 0xF:	return "";
		default:
			ASSERT(false);
		}

		return "";
	}

	std::string Shader::Instruction::SourceParameter::preModifierString() const
	{
		if(type == PARAMETER_VOID)
		{
			return "";
		}

		switch(modifier)
		{
		case MODIFIER_NONE:			return "";
		case MODIFIER_NEGATE:		return "-";
		case MODIFIER_BIAS:			return "";
		case MODIFIER_BIAS_NEGATE:	return "-";
		case MODIFIER_SIGN:			return "";
		case MODIFIER_SIGN_NEGATE:	return "-";
		case MODIFIER_COMPLEMENT:	return "1-";
		case MODIFIER_X2:			return "";
		case MODIFIER_X2_NEGATE:	return "-";
		case MODIFIER_DZ:			return "";
		case MODIFIER_DW:			return "";
		case MODIFIER_ABS:			return "";
		case MODIFIER_ABS_NEGATE:	return "-";
		case MODIFIER_NOT:			return "!";
		default:
			ASSERT(false);
		}

		return "";
	}

	std::string Shader::Instruction::Parameter::relativeString() const
	{
		if(!relative) return "";

		if(relativeType == Parameter::PARAMETER_ADDR)
		{
			switch(relativeSwizzle & 0x03)
			{
			case 0: return "[a0.x]";
			case 1: return "[a0.y]";
			case 2: return "[a0.z]";
			case 3: return "[a0.w]";
			}
		}
		else if(relativeType == Parameter::PARAMETER_LOOP)
		{
			return "[aL]";
		}
		else ASSERT(false);

		return "";
	}

	std::string Shader::Instruction::SourceParameter::postModifierString() const
	{
		if(type == PARAMETER_VOID)
		{
			return "";
		}

		switch(modifier)
		{
		case MODIFIER_NONE:			return "";
		case MODIFIER_NEGATE:		return "";
		case MODIFIER_BIAS:			return "_bias";
		case MODIFIER_BIAS_NEGATE:	return "_bias";
		case MODIFIER_SIGN:			return "_bx2";
		case MODIFIER_SIGN_NEGATE:	return "_bx2";
		case MODIFIER_COMPLEMENT:	return "";
		case MODIFIER_X2:			return "_x2";
		case MODIFIER_X2_NEGATE:	return "_x2";
		case MODIFIER_DZ:			return "_dz";
		case MODIFIER_DW:			return "_dw";
		case MODIFIER_ABS:			return "_abs";
		case MODIFIER_ABS_NEGATE:	return "_abs";
		case MODIFIER_NOT:			return "";
		default:
			ASSERT(false);
		}

		return "";
	}

	std::string Shader::Instruction::SourceParameter::swizzleString() const
	{
		return Instruction::swizzleString(type, swizzle);
	}

	void Shader::Instruction::parseOperationToken(unsigned long token, unsigned char majorVersion)
	{
		if((token & 0xFFFF0000) == 0xFFFF0000 || (token & 0xFFFF0000) == 0xFFFE0000)   // Version token
		{
			operation.opcode = (Operation::Opcode)token;
			operation.predicate = false;
			operation.coissue = false;
		}
		else
		{
			operation.opcode = (Operation::Opcode)(token & 0x0000FFFF);
			operation.control = (Operation::Control)((token & 0x00FF0000) >> 16);

			int size = (token & 0x0F000000) >> 24;

			operation.predicate = (token & 0x10000000) != 0x00000000;
			operation.coissue = (token & 0x40000000) != 0x00000000;

			if(majorVersion < 2)
			{
				if(size != 0)
				{
					ASSERT(false);   // Reserved
				}
			}

			if(majorVersion < 2)
			{
				if(operation.predicate)
				{
					ASSERT(false);
				}
			}

			if((token & 0x20000000) != 0x00000000)
			{
				ASSERT(false);   // Reserved
			}

			if(majorVersion >= 2)
			{
				if(operation.coissue)
				{
					ASSERT(false);   // Reserved
				}
			}

			if((token & 0x80000000) != 0x00000000)
			{
				ASSERT(false);
			}
		}
	}

	void Shader::Instruction::parseDeclarationToken(unsigned long token)
	{
		operation.samplerType = (Operation::SamplerType)((token & 0x78000000) >> 27);
		operation.usage = (Operation::Usage)(token & 0x0000001F);
		operation.usageIndex = (unsigned char)((token & 0x000F0000) >> 16);
	}

	void Shader::Instruction::parseDestinationToken(const unsigned long *token, unsigned char majorVersion)
	{
		destinationParameter.index = (unsigned short)(token[0] & 0x000007FF);
		destinationParameter.type = (Parameter::Type)(((token[0] & 0x00001800) >> 8) | ((token[0] & 0x70000000) >> 28));

		// TODO: Check type and index range

		destinationParameter.relative = (token[0] & 0x00002000) != 0x00000000;
		destinationParameter.relativeType = Parameter::PARAMETER_ADDR;
		destinationParameter.relativeSwizzle = 0x00;

		if(destinationParameter.relative && majorVersion >= 3)
		{
			destinationParameter.relativeType = (Parameter::Type)(((token[1] & 0x00001800) >> 8) | ((token[1] & 0x70000000) >> 28));
			destinationParameter.relativeSwizzle = (unsigned char)((token[1] & 0x00FF0000) >> 16);
		}
		else if(destinationParameter.relative) ASSERT(false);   // Reserved

		if((token[0] & 0x0000C000) != 0x00000000)
		{
			ASSERT(false);   // Reserved
		}

		destinationParameter.mask = (unsigned char)((token[0] & 0x000F0000) >> 16);
		destinationParameter.saturate = (token[0] & 0x00100000) != 0;
		destinationParameter.partialPrecision = (token[0] & 0x00200000) != 0;
		destinationParameter.centroid = (token[0] & 0x00400000) != 0;
		destinationParameter.shift = (signed char)((token[0] & 0x0F000000) >> 20) >> 4;

		if(majorVersion >= 2)
		{
			if(destinationParameter.shift)
			{
				ASSERT(false);   // Reserved
			}
		}

		if((token[0] & 0x80000000) != 0x80000000)
		{
			ASSERT(false);
		}
	}

	void Shader::Instruction::parseSourceToken(int i, const unsigned long *token, unsigned char majorVersion)
	{
		// Defaults
		sourceParameter[i].value = (float&)*token;
		sourceParameter[i].type = Parameter::PARAMETER_VOID;
		sourceParameter[i].modifier = SourceParameter::MODIFIER_NONE;
		sourceParameter[i].swizzle = 0xE4;
		sourceParameter[i].relative = false;
		sourceParameter[i].relativeType = Parameter::PARAMETER_ADDR;
		sourceParameter[i].relativeSwizzle = 0x00;
		
		switch(operation.opcode)
		{
		case Instruction::Operation::OPCODE_DEF:
			sourceParameter[i].type = Parameter::PARAMETER_FLOATLITERAL;
			break;
		case Instruction::Operation::OPCODE_DEFB:
			sourceParameter[i].type = Parameter::PARAMETER_BOOLLITERAL;
			break;
		case Instruction::Operation::OPCODE_DEFI:
			sourceParameter[i].type = Parameter::PARAMETER_INTLITERAL;
			break;
		default:
			sourceParameter[i].index = (unsigned short)(token[0] & 0x000007FF);
			sourceParameter[i].type = (Parameter::Type)(((token[0] & 0x00001800) >> 8) | ((token[0] & 0x70000000) >> 28));

			// FIXME: Check type and index range

			sourceParameter[i].relative = (token[0] & 0x00002000) != 0x00000000;

			if((token[0] & 0x0000C000) != 0x00000000)
			{
				if(operation.opcode != Operation::OPCODE_DEF &&
				   operation.opcode != Operation::OPCODE_DEFI &&
				   operation.opcode != Operation::OPCODE_DEFB)
				{
					ASSERT(false);
				}
			}

			sourceParameter[i].swizzle = (unsigned char)((token[0] & 0x00FF0000) >> 16);
			sourceParameter[i].modifier = (SourceParameter::Modifier)((token[0] & 0x0F000000) >> 24);

			if((token[0] & 0x80000000) != 0x80000000)
			{
				if(operation.opcode != Operation::OPCODE_DEF &&
				   operation.opcode != Operation::OPCODE_DEFI &&
				   operation.opcode != Operation::OPCODE_DEFB)
				{
					ASSERT(false);
				}
			}

			if(sourceParameter[i].relative && majorVersion >= 2)
			{
				sourceParameter[i].relativeType = (Parameter::Type)(((token[1] & 0x00001800) >> 8) | ((token[1] & 0x70000000) >> 28));
				sourceParameter[i].relativeSwizzle = (unsigned char)((token[1] & 0x00FF0000) >> 16);
			}
		}
	}

	std::string Shader::Instruction::swizzleString(Parameter::Type type, unsigned char swizzle)
	{
		if(type == Parameter::PARAMETER_VOID || type == Parameter::PARAMETER_LABEL || swizzle == 0xE4)
		{
			return "";
		}

		int x = (swizzle & 0x03) >> 0;
		int y = (swizzle & 0x0C) >> 2;
		int z = (swizzle & 0x30) >> 4;
		int w = (swizzle & 0xC0) >> 6;

		std::string swizzleString = ".";

		switch(x)
		{
		case 0: swizzleString += "x"; break;
		case 1: swizzleString += "y"; break;
		case 2: swizzleString += "z"; break;
		case 3: swizzleString += "w"; break;
		}

		if(!(x == y && y == z && z == w))
		{
			switch(y)
			{
			case 0: swizzleString += "x"; break;
			case 1: swizzleString += "y"; break;
			case 2: swizzleString += "z"; break;
			case 3: swizzleString += "w"; break;
			}

			if(!(y == z && z == w))
			{
				switch(z)
				{
				case 0: swizzleString += "x"; break;
				case 1: swizzleString += "y"; break;
				case 2: swizzleString += "z"; break;
				case 3: swizzleString += "w"; break;
				}

				if(!(z == w))
				{
					switch(w)
					{
					case 0: swizzleString += "x"; break;
					case 1: swizzleString += "y"; break;
					case 2: swizzleString += "z"; break;
					case 3: swizzleString += "w"; break;
					}
				}
			}
		}

		return swizzleString;
	}

	std::string Shader::Instruction::Parameter::string(ShaderType shaderType, unsigned short version) const
	{
		std::ostringstream buffer;

		if(type == PARAMETER_FLOATLITERAL)
		{
			buffer << value;

			return buffer.str();
		}
		else
		{
			if(type != PARAMETER_RASTOUT && !(type == PARAMETER_ADDR && shaderType == SHADER_VERTEX) && type != PARAMETER_LOOP && type != PARAMETER_PREDICATE && type != PARAMETER_MISCTYPE)
			{
				buffer << index;

				return typeString(shaderType, version) + buffer.str();
			}
			else
			{
				return typeString(shaderType, version);
			}
		}
	}

	std::string Shader::Instruction::Parameter::typeString(ShaderType shaderType, unsigned short version) const
	{
		switch(type)
		{
		case PARAMETER_TEMP:			return "r";
		case PARAMETER_INPUT:			return "v";
		case PARAMETER_CONST:			return "c";
		case PARAMETER_TEXTURE:
	//	case PARAMETER_ADDR:
			if(shaderType == SHADER_PIXEL)	return "t";
			else							return "a0";
		case PARAMETER_RASTOUT:
			if(index == 0)              return "oPos";
			else if(index == 1)         return "oFog";
			else if(index == 2)         return "oPts";
			else                        ASSERT(false);
		case PARAMETER_ATTROUT:			return "oD";
		case PARAMETER_TEXCRDOUT:
	//	case PARAMETER_OUTPUT:			return "";
			if(version < 0x0300)		return "oT";
			else						return "o";
		case PARAMETER_CONSTINT:		return "i";
		case PARAMETER_COLOROUT:		return "oC";
		case PARAMETER_DEPTHOUT:		return "oDepth";
		case PARAMETER_SAMPLER:			return "s";
	//	case PARAMETER_CONST2:			return "";
	//	case PARAMETER_CONST3:			return "";
	//	case PARAMETER_CONST4:			return "";
		case PARAMETER_CONSTBOOL:		return "b";
		case PARAMETER_LOOP:			return "aL";
	//	case PARAMETER_TEMPFLOAT16:		return "";
		case PARAMETER_MISCTYPE:
			if(index == 0)				return "vPos";
			else if(index == 1)			return "vFace";
			else						ASSERT(false);
		case PARAMETER_LABEL:			return "l";
		case PARAMETER_PREDICATE:		return "p0";
		case PARAMETER_FLOATLITERAL:	return "";
		case PARAMETER_BOOLLITERAL:		return "";
		case PARAMETER_INTLITERAL:		return "";
	//	case PARAMETER_VOID:			return "";
		default:
			ASSERT(false);
		}

		return "";
	}

	Shader::Shader(const unsigned long *shaderToken)
	{
		instruction = 0;
		length = 0;

		tokenCount = 0;

		while(shaderToken[tokenCount] != 0x0000FFFF)
		{
			tokenCount += sw::Shader::size(shaderToken[tokenCount], (unsigned short)(shaderToken[0] & 0xFFFF)) + 1;
		}

		tokenCount += 1;

		this->shaderToken = new unsigned long[tokenCount];
		memcpy(this->shaderToken, shaderToken, tokenCount * sizeof(unsigned long));

		unsigned long *hashTokens = new unsigned long[tokenCount];
		memcpy(hashTokens, shaderToken, tokenCount * sizeof(unsigned long));
		removeComments(hashTokens, tokenCount);
		hash = FNV_1((unsigned char*)hashTokens, tokenCount * sizeof(unsigned long));
		delete[] hashTokens;
	}

	Shader::~Shader()
	{
		delete[] shaderToken;
		shaderToken = 0;

		for(int i = 0; i < length; i++)
		{
			delete instruction[i];
			instruction[i] = 0;
		}

		delete[] instruction;
		instruction = 0;
	}

	void Shader::getFunction(void *data, unsigned int *size)
	{
		if(data)
		{
			memcpy(data, shaderToken, tokenCount * 4);
		}

		*size = tokenCount * 4;
	}

	int Shader::size(unsigned long opcode) const
	{
		return size(opcode, version);
	}

	int Shader::size(unsigned long opcode, unsigned short version)
	{
		if(version > 0x0300)
		{
			ASSERT(false);
		}

		static const char size[] =
		{
			0,   // NOP = 0
			2,   // MOV
			3,   // ADD
			3,   // SUB
			4,   // MAD
			3,   // MUL
			2,   // RCP
			2,   // RSQ
			3,   // DP3
			3,   // DP4
			3,   // MIN
			3,   // MAX
			3,   // SLT
			3,   // SGE
			2,   // EXP
			2,   // LOG
			2,   // LIT
			3,   // DST
			4,   // LRP
			2,   // FRC
			3,   // M4x4
			3,   // M4x3
			3,   // M3x4
			3,   // M3x3
			3,   // M3x2
			1,   // CALL
			2,   // CALLNZ
			2,   // LOOP
			0,   // RET
			0,   // ENDLOOP
			1,   // LABEL
			2,   // DCL
			3,   // POW
			3,   // CRS
			4,   // SGN
			2,   // ABS
			2,   // NRM
			4,   // SINCOS
			1,   // REP
			0,   // ENDREP
			1,   // IF
			2,   // IFC
			0,   // ELSE
			0,   // ENDIF
			0,   // BREAK
			2,   // BREAKC
			2,   // MOVA
			2,   // DEFB
			5,   // DEFI
			-1,  // 49
			-1,  // 50
			-1,  // 51
			-1,  // 52
			-1,  // 53
			-1,  // 54
			-1,  // 55
			-1,  // 56
			-1,  // 57
			-1,  // 58
			-1,  // 59
			-1,  // 60
			-1,  // 61
			-1,  // 62
			-1,  // 63
			1,   // TEXCOORD = 64
			1,   // TEXKILL
			1,   // TEX
			2,   // TEXBEM
			2,   // TEXBEML
			2,   // TEXREG2AR
			2,   // TEXREG2GB
			2,   // TEXM3x2PAD
			2,   // TEXM3x2TEX
			2,   // TEXM3x3PAD
			2,   // TEXM3x3TEX
			-1,  // RESERVED0
			3,   // TEXM3x3SPEC
			2,   // TEXM3x3VSPEC
			2,   // EXPP
			2,   // LOGP
			4,   // CND
			5,   // DEF
			2,   // TEXREG2RGB
			2,   // TEXDP3TEX
			2,   // TEXM3x2DEPTH
			2,   // TEXDP3
			2,   // TEXM3x3
			1,   // TEXDEPTH
			4,   // CMP
			3,   // BEM
			4,   // DP2ADD
			2,   // DSX
			2,   // DSY
			5,   // TEXLDD
			3,   // SETP
			3,   // TEXLDL
			2,   // BREAKP
			-1,  // 97
			-1,  // 98
			-1,  // 99
			-1,  // 100
			-1,  // 101
			-1,  // 102
			-1,  // 103
			-1,  // 104
			-1,  // 105
			-1,  // 106
			-1,  // 107
			-1,  // 108
			-1,  // 109
			-1,  // 110
			-1,  // 111
			-1,  // 112
		};

		int length = 0;

		if((opcode & 0x0000FFFF) == ShaderOperation::OPCODE_COMMENT)
		{
			return (opcode & 0x7FFF0000) >> 16;
		}

		if(opcode != ShaderOperation::OPCODE_PS_1_0 &&
		   opcode != ShaderOperation::OPCODE_PS_1_1 &&
		   opcode != ShaderOperation::OPCODE_PS_1_2 &&
		   opcode != ShaderOperation::OPCODE_PS_1_3 &&
		   opcode != ShaderOperation::OPCODE_PS_1_4 &&
		   opcode != ShaderOperation::OPCODE_PS_2_0 &&
		   opcode != ShaderOperation::OPCODE_PS_2_x &&
		   opcode != ShaderOperation::OPCODE_PS_3_0 &&
		   opcode != ShaderOperation::OPCODE_VS_1_0 &&
		   opcode != ShaderOperation::OPCODE_VS_1_1 &&
		   opcode != ShaderOperation::OPCODE_VS_2_0 &&
		   opcode != ShaderOperation::OPCODE_VS_2_x &&
		   opcode != ShaderOperation::OPCODE_VS_2_sw &&
		   opcode != ShaderOperation::OPCODE_VS_3_0 &&
		   opcode != ShaderOperation::OPCODE_VS_3_sw &&
		   opcode != ShaderOperation::OPCODE_PHASE &&
		   opcode != ShaderOperation::OPCODE_END)
		{
			if(version >= 0x0200)
			{
				length = (opcode & 0x0F000000) >> 24;
			}
			else
			{
				length = size[opcode & 0x0000FFFF];
			}
		}

		if(length < 0)
		{
			ASSERT(false);
		}

		if(version == 0x0104)
		{
			switch(opcode & 0x0000FFFF)
			{
			case ShaderOperation::OPCODE_TEX:
				length += 1;
				break;
			case ShaderOperation::OPCODE_TEXCOORD:
				length += 1;
				break;
			default:
				break;
			}
		}

		return length;
	}

	bool Shader::maskContainsComponent(int mask, int component)
	{
		return (mask & (1 << component)) != 0;
	}

	bool Shader::swizzleContainsComponent(int swizzle, int component)
	{
		if((swizzle & 0x03) >> 0 == component) return true;
		if((swizzle & 0x0C) >> 2 == component) return true;
		if((swizzle & 0x30) >> 4 == component) return true;
		if((swizzle & 0xC0) >> 6 == component) return true;

		return false;
	}

	bool Shader::swizzleContainsComponentMasked(int swizzle, int component, int mask)
	{
		if(mask & 0x1) if((swizzle & 0x03) >> 0 == component) return true;
		if(mask & 0x2) if((swizzle & 0x0C) >> 2 == component) return true;
		if(mask & 0x4) if((swizzle & 0x30) >> 4 == component) return true;
		if(mask & 0x8) if((swizzle & 0xC0) >> 6 == component) return true;

		return false;
	}

	bool Shader::containsDynamicBranching() const
	{
		return dynamicBranching;
	}

	bool Shader::usesSampler(int index) const
	{
		return (sampler & (1 << index)) != 0;
	}

	int64_t Shader::getHash() const
	{
		return hash;
	}

	int Shader::getLength() const
	{
		return length;
	}

	Shader::ShaderType Shader::getShaderType() const
	{
		return shaderType;
	}

	unsigned short Shader::getVersion() const
	{
		return version;
	}

	void Shader::print(const char *fileName, ...) const
	{
		char fullName[1024 + 1];

		va_list vararg;
		va_start(vararg, fileName);
		vsnprintf(fullName, 1024, fileName, vararg);
		va_end(vararg);

		std::ofstream file(fullName, std::ofstream::out | std::ofstream::app);

		for(int i = 0; i < length; i++)
		{
			file << instruction[i]->string(shaderType, version) << std::endl;
		}
	}

	void Shader::printInstruction(int index, const char *fileName) const
	{
		std::ofstream file(fileName, std::ofstream::out | std::ofstream::app);

		file << instruction[index]->string(shaderType, version) << std::endl;
	}

	const ShaderInstruction *Shader::getInstruction(int i) const
	{
		if(i < 0 || i >= length)
		{
			ASSERT(false);
		}

		return instruction[i];
	}

	void Shader::analyzeDirtyConstants()
	{
		dirtyConstantsF = 0;
		dirtyConstantsI = 0;
		dirtyConstantsB = 0;

		for(int i = 0; i < length; i++)
		{
			switch(instruction[i]->operation.opcode)
			{
			case ShaderOperation::OPCODE_DEF:
				if(instruction[i]->destinationParameter.index + 1 > dirtyConstantsF)
				{
					dirtyConstantsF = instruction[i]->destinationParameter.index + 1;
				}
				break;
			case ShaderOperation::OPCODE_DEFI:
				if(instruction[i]->destinationParameter.index + 1 > dirtyConstantsI)
				{
					dirtyConstantsI = instruction[i]->destinationParameter.index + 1;
				}
				break;
			case ShaderOperation::OPCODE_DEFB:
				if(instruction[i]->destinationParameter.index + 1 > dirtyConstantsB)
				{
					dirtyConstantsB = instruction[i]->destinationParameter.index + 1;
				}
				break;
			}
		}
	}

	void Shader::analyzeDynamicBranching()
	{
		dynamicBranching = false;

		for(int i = 0; i < length; i++)
		{
			switch(instruction[i]->getOpcode())
			{
			case ShaderOperation::OPCODE_CALLNZ:
			case ShaderOperation::OPCODE_IF:
			case ShaderOperation::OPCODE_IFC:
			case ShaderOperation::OPCODE_BREAK:
			case ShaderOperation::OPCODE_BREAKC:
			case ShaderOperation::OPCODE_SETP:
			case ShaderOperation::OPCODE_BREAKP:
				if(instruction[i]->sourceParameter[0].type != ShaderParameter::PARAMETER_CONSTBOOL)
				{
					dynamicBranching = true;
					break;
				}
			}
		}
	}

	void Shader::analyzeSamplers()
	{
		sampler = 0;

		for(int i = 0; i < length; i++)
		{
			switch(instruction[i]->getOpcode())
			{
			case ShaderOperation::OPCODE_TEX:
			case ShaderOperation::OPCODE_TEXBEM:
			case ShaderOperation::OPCODE_TEXBEML:
			case ShaderOperation::OPCODE_TEXREG2AR:
			case ShaderOperation::OPCODE_TEXREG2GB:
			case ShaderOperation::OPCODE_TEXM3X2TEX:
			case ShaderOperation::OPCODE_TEXM3X3TEX:
			case ShaderOperation::OPCODE_TEXM3X3SPEC:
			case ShaderOperation::OPCODE_TEXM3X3VSPEC:
			case ShaderOperation::OPCODE_TEXREG2RGB:
			case ShaderOperation::OPCODE_TEXDP3TEX:
			case ShaderOperation::OPCODE_TEXM3X2DEPTH:
			case ShaderOperation::OPCODE_TEXLDD:
			case ShaderOperation::OPCODE_TEXLDL:
				{
					ShaderParameter &dst = instruction[i]->destinationParameter;
					ShaderParameter &src1 = instruction[i]->sourceParameter[1];

					if(majorVersion >= 2)
					{
						ASSERT(src1.type == ShaderParameter::PARAMETER_SAMPLER);
						sampler |= 1 << src1.index;
					}
					else
					{
						sampler |= 1 << dst.index;
					}
				}
				break;
			}
		}
	}

	void Shader::removeComments(unsigned long *shaderToken, int tokenCount)
	{
		for(int i = 0; i < tokenCount; )
		{
			int instructionSize = sw::Shader::size(shaderToken[i], (unsigned short)(shaderToken[0] & 0xFFFF)) + 1;

			if((shaderToken[i] & 0x0000FFFF) == ShaderOperation::OPCODE_COMMENT)
			{
				for(int j = 0; j < instructionSize; j++)
				{
					shaderToken[i + j] = ShaderOperation::OPCODE_NOP;
				}
			}

			i += instructionSize;
		}
	}
}
