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

#ifndef sw_Shader_hpp
#define sw_Shader_hpp

#include "Common/Types.hpp"

#include <string>

namespace sw
{
	class Shader
	{
	public:
		enum ShaderType
		{
			SHADER_PIXEL = 0xFFFF,
			SHADER_VERTEX = 0xFFFE,
			SHADER_GEOMETRY = 0xFFFD
		};

		class Instruction
		{
			friend Shader;

		public:
			Instruction();
			Instruction(const unsigned long *token, int size, unsigned char majorVersion);

			virtual ~Instruction();

			struct Operation
			{
				enum Opcode
				{
					// Extracted from d3d9types.h
					OPCODE_NOP = 0,
					OPCODE_MOV,
					OPCODE_ADD,
					OPCODE_SUB,
					OPCODE_MAD,
					OPCODE_MUL,
					OPCODE_RCP,
					OPCODE_RSQ,
					OPCODE_DP3,
					OPCODE_DP4,
					OPCODE_MIN,
					OPCODE_MAX,
					OPCODE_SLT,
					OPCODE_SGE,
					OPCODE_EXP,
					OPCODE_LOG,
					OPCODE_LIT,
					OPCODE_DST,
					OPCODE_LRP,
					OPCODE_FRC,
					OPCODE_M4X4,
					OPCODE_M4X3,
					OPCODE_M3X4,
					OPCODE_M3X3,
					OPCODE_M3X2,
					OPCODE_CALL,
					OPCODE_CALLNZ,
					OPCODE_LOOP,
					OPCODE_RET,
					OPCODE_ENDLOOP,
					OPCODE_LABEL,
					OPCODE_DCL,
					OPCODE_POW,
					OPCODE_CRS,
					OPCODE_SGN,
					OPCODE_ABS,
					OPCODE_NRM,
					OPCODE_SINCOS,
					OPCODE_REP,
					OPCODE_ENDREP,
					OPCODE_IF,
					OPCODE_IFC,
					OPCODE_ELSE,
					OPCODE_ENDIF,
					OPCODE_BREAK,
					OPCODE_BREAKC,
					OPCODE_MOVA,
					OPCODE_DEFB,
					OPCODE_DEFI,

					OPCODE_TEXCOORD = 64,
					OPCODE_TEXKILL,
					OPCODE_TEX,
					OPCODE_TEXBEM,
					OPCODE_TEXBEML,
					OPCODE_TEXREG2AR,
					OPCODE_TEXREG2GB,
					OPCODE_TEXM3X2PAD,
					OPCODE_TEXM3X2TEX,
					OPCODE_TEXM3X3PAD,
					OPCODE_TEXM3X3TEX,
					OPCODE_RESERVED0,
					OPCODE_TEXM3X3SPEC,
					OPCODE_TEXM3X3VSPEC,
					OPCODE_EXPP,
					OPCODE_LOGP,
					OPCODE_CND,
					OPCODE_DEF,
					OPCODE_TEXREG2RGB,
					OPCODE_TEXDP3TEX,
					OPCODE_TEXM3X2DEPTH,
					OPCODE_TEXDP3,
					OPCODE_TEXM3X3,
					OPCODE_TEXDEPTH,
					OPCODE_CMP,
					OPCODE_BEM,
					OPCODE_DP2ADD,
					OPCODE_DSX,
					OPCODE_DSY,
					OPCODE_TEXLDD,
					OPCODE_SETP,
					OPCODE_TEXLDL,
					OPCODE_BREAKP,

					OPCODE_PHASE = 0xFFFD,
					OPCODE_COMMENT = 0xFFFE,
					OPCODE_END = 0xFFFF,

					OPCODE_PS_1_0 = 0xFFFF0100,
					OPCODE_PS_1_1 = 0xFFFF0101,
					OPCODE_PS_1_2 = 0xFFFF0102,
					OPCODE_PS_1_3 = 0xFFFF0103,
					OPCODE_PS_1_4 = 0xFFFF0104,
					OPCODE_PS_2_0 = 0xFFFF0200,
					OPCODE_PS_2_x = 0xFFFF0201,
					OPCODE_PS_3_0 = 0xFFFF0300,
					
					OPCODE_VS_1_0 = 0xFFFE0100,
					OPCODE_VS_1_1 = 0xFFFE0101,
					OPCODE_VS_2_0 = 0xFFFE0200,
					OPCODE_VS_2_x = 0xFFFE0201,
					OPCODE_VS_2_sw = 0xFFFE02FF,
					OPCODE_VS_3_0 = 0xFFFE0300,
					OPCODE_VS_3_sw = 0xFFFE03FF,
				};

				enum Control
				{
					CONTROL_RESERVED0,
					CONTROL_GT,
					CONTROL_EQ,
					CONTROL_GE,
					CONTROL_LT,
					CONTROL_NE,
					CONTROL_LE,
					CONTROL_RESERVED1
				};

				enum SamplerType
				{
					SAMPLER_UNKNOWN,
					SAMPLER_1D,
					SAMPLER_2D,
					SAMPLER_CUBE,
					SAMPLER_VOLUME
				};

				enum Usage   // For vertex input/output declarations
				{
					USAGE_POSITION = 0,
					USAGE_BLENDWEIGHT = 1,
					USAGE_BLENDINDICES = 2,
					USAGE_NORMAL = 3,
					USAGE_PSIZE = 4,
					USAGE_TEXCOORD = 5,
					USAGE_TANGENT = 6,
					USAGE_BINORMAL = 7,
					USAGE_TESSFACTOR = 8,
					USAGE_POSITIONT = 9,
					USAGE_COLOR = 10,
					USAGE_FOG = 11,
					USAGE_DEPTH = 12,
					USAGE_SAMPLE = 13
				};

				Operation() : opcode(OPCODE_NOP), control(CONTROL_RESERVED0), predicate(false), predicateNot(false), predicateSwizzle(0xE4), coissue(false), samplerType(SAMPLER_UNKNOWN), usage(USAGE_POSITION), usageIndex(0)
				{
				}

				std::string string(unsigned short version) const;
				std::string controlString() const;

				Opcode opcode;
				
				union
				{
					Control control;
					
					struct
					{
						unsigned char project : 1;
						unsigned char bias : 1;
					};
				};

				bool predicate;
				bool predicateNot;   // Negative predicate
				unsigned char predicateSwizzle;

				bool coissue;
				SamplerType samplerType;
				Usage usage;
				unsigned char usageIndex;
			};

			struct Parameter
			{
				enum Type
				{
					PARAMETER_TEMP = 0,
					PARAMETER_INPUT = 1,
					PARAMETER_CONST = 2,
					PARAMETER_TEXTURE = 3,
					PARAMETER_ADDR = 3,
					PARAMETER_RASTOUT = 4,
					PARAMETER_ATTROUT = 5,
					PARAMETER_TEXCRDOUT = 6,
					PARAMETER_OUTPUT = 6,
					PARAMETER_CONSTINT = 7,
					PARAMETER_COLOROUT = 8,
					PARAMETER_DEPTHOUT = 9,
					PARAMETER_SAMPLER = 10,
					PARAMETER_CONST2 = 11,
					PARAMETER_CONST3 = 12,
					PARAMETER_CONST4 = 13,
					PARAMETER_CONSTBOOL = 14,
					PARAMETER_LOOP = 15,
					PARAMETER_TEMPFLOAT16 = 16,
					PARAMETER_MISCTYPE = 17,
					PARAMETER_LABEL = 18,
					PARAMETER_PREDICATE = 19,

					// Internally used
					PARAMETER_FLOATLITERAL = 20,
					PARAMETER_BOOLLITERAL = 21,
					PARAMETER_INTLITERAL = 22,

					PARAMETER_VOID
				};

				union
				{
					unsigned int index;   // For registers
					float value;          // For float constants
					int integer;          // For integer constants
					bool boolean;         // For boolean constants
				};

				Parameter() : type(PARAMETER_VOID), index(0), relative(false), relativeType(PARAMETER_VOID), relativeSwizzle(0xE4)
				{
				}

				std::string string(ShaderType shaderType, unsigned short version) const;
				std::string typeString(ShaderType shaderType, unsigned short version) const;
				std::string relativeString() const;

				Type type;
				bool relative;
				Type relativeType;
				unsigned char relativeSwizzle;
			};

			struct DestinationParameter : Parameter
			{
				union
				{
					unsigned char mask;

					struct
					{
						bool x : 1;
						bool y : 1;
						bool z : 1;
						bool w : 1;
					};
				};

				DestinationParameter() : mask(0xF), saturate(false), partialPrecision(false), centroid(false), shift(0)
				{
				}

				std::string modifierString() const;
				std::string shiftString() const;
				std::string maskString() const;

				bool saturate;
				bool partialPrecision;
				bool centroid;
				signed char shift;
			};

			struct SourceParameter : Parameter
			{
				enum Modifier
				{
					MODIFIER_NONE,
					MODIFIER_NEGATE,
					MODIFIER_BIAS,
					MODIFIER_BIAS_NEGATE,
					MODIFIER_SIGN,
					MODIFIER_SIGN_NEGATE,
					MODIFIER_COMPLEMENT,
					MODIFIER_X2,
					MODIFIER_X2_NEGATE,
					MODIFIER_DZ,
					MODIFIER_DW,
					MODIFIER_ABS,
					MODIFIER_ABS_NEGATE,
					MODIFIER_NOT
				};

				SourceParameter() : swizzle(0xE4), modifier(MODIFIER_NONE)
				{
				}

				std::string swizzleString() const;
				std::string preModifierString() const;
				std::string postModifierString() const;

				unsigned char swizzle;
				Modifier modifier;
			};

			void parseOperationToken(unsigned long token, unsigned char majorVersion);
			void parseDeclarationToken(unsigned long token);
			void parseDestinationToken(const unsigned long *token, unsigned char majorVersion);
			void parseSourceToken(int i, const unsigned long *token, unsigned char majorVersion);

			Operation::Opcode getOpcode() const;
			const DestinationParameter &getDestinationParameter() const;
			const SourceParameter &getSourceParameter(int i) const;

			bool isCoissue() const;
			bool isProject() const;
			bool isBias() const;
			bool isPredicate() const;
			bool isPredicateNot() const;
			unsigned char getPredicateSwizzle() const;
			Operation::Control getControl() const;
			Operation::Usage getUsage() const;
			unsigned char getUsageIndex() const;
			Operation::SamplerType getSamplerType() const;

			std::string string(ShaderType shaderType, unsigned short version) const;

		protected:
			Operation operation;
			DestinationParameter destinationParameter;
			SourceParameter sourceParameter[4];

		private:
			static std::string swizzleString(Parameter::Type type, unsigned char swizzle);
		};

		Shader(const unsigned long *shaderToken);

		~Shader();

		void getFunction(void *data, unsigned int *size);

		int64_t getHash() const;
		int getLength() const;
		ShaderType getShaderType() const;
		unsigned short getVersion() const;

		const Instruction *getInstruction(int i) const;
		int size(unsigned long opcode) const;
		static int size(unsigned long opcode, unsigned short version);

		void print(const char *fileName, ...) const;
		void printInstruction(int index, const char *fileName) const;

		static bool maskContainsComponent(int mask, int component);
		static bool swizzleContainsComponent(int swizzle, int component);
		static bool swizzleContainsComponentMasked(int swizzle, int component, int mask);

		bool containsDynamicBranching() const;
		bool usesSampler(int i) const;

		struct Semantic
		{
			Semantic(unsigned char usage = 0xFF, unsigned char index = 0xFF) : usage(usage), index(index), centroid(false)
			{
			}

			bool operator==(const Semantic &semantic) const
			{
				return usage == semantic.usage && index == semantic.index;
			}

			bool active() const
			{
				return usage != 0xFF;
			}

			unsigned char usage;
			unsigned char index;
			bool centroid;
		};

		unsigned int dirtyConstantsF;   // FIXME: Private
		unsigned int dirtyConstantsI;   // FIXME: Private
		unsigned int dirtyConstantsB;   // FIXME: Private

	protected:
		void analyzeDirtyConstants();
		void analyzeDynamicBranching();
		void analyzeSamplers();

		ShaderType shaderType;

		union
		{
			unsigned short version;

			struct
			{
				unsigned char minorVersion;
				unsigned char majorVersion;
			};
		};

		int length;
		Instruction **instruction;

	private:
		static void removeComments(unsigned long *shaderToken, int tokenCount);

		int64_t hash;

		bool dynamicBranching;
		unsigned short sampler;

		unsigned long *shaderToken;
		int tokenCount;
	};

	typedef Shader::Instruction::Operation ShaderOperation;
	typedef Shader::Instruction ShaderInstruction;
	typedef Shader::Instruction::Parameter ShaderParameter;
	typedef Shader::Instruction::Operation::Opcode ShaderOpcode;
}

#endif   // sw_Shader_hpp
