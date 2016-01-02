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

#ifndef COMPILER_OUTPUTASM_H_
#define COMPILER_OUTPUTASM_H_

#include "intermediate.h"
#include "ParseHelper.h"
#include "Shader/PixelShader.hpp"
#include "Shader/VertexShader.hpp"

#include <list>
#include <set>
#include <map>

namespace es2
{
	class Shader;
}

typedef unsigned int GLenum;

namespace glsl
{
	struct Uniform
	{
		Uniform(GLenum type, GLenum precision, const std::string &name, int arraySize, int registerIndex, int offset, int blockId);

		GLenum type;
		GLenum precision;
		std::string name;
		int arraySize;
	
		int registerIndex;
		int offset;

		int blockId;
	};

	typedef std::vector<Uniform> ActiveUniforms;

	struct UniformBlock
	{
		UniformBlock(const std::string& name, const std::string& instanceName, unsigned int dataSize, unsigned int arraySize,
		             TLayoutBlockStorage layout, bool isRowMajorLayout, int registerIndex, int blockId);

		const std::string& getName() const { return (instanceName.length() > 0) ? instanceName : name; }

		std::string name;
		std::string instanceName;
		unsigned int dataSize;
		unsigned int arraySize;
		TLayoutBlockStorage layout;
		bool isRowMajorLayout;
		std::vector<int> fields;

		int registerIndex;

		int blockId;
	};

	typedef std::vector<UniformBlock> ActiveUniformBlocks;

	struct Attribute
	{
		Attribute();
		Attribute(GLenum type, const std::string &name, int arraySize, int location, int registerIndex);

		GLenum type;
		std::string name;
		int arraySize;
		int location;
	
		int registerIndex;
	};

	typedef std::vector<Attribute> ActiveAttributes;

	struct Varying
	{
		Varying(GLenum type, const std::string &name, int arraySize, int reg = -1, int col = -1)
			: type(type), name(name), arraySize(arraySize), reg(reg), col(col)
		{
		}

		bool isArray() const
		{
			return arraySize >= 1;
		}

		int size() const   // Unify with es2::Uniform?
		{
			return arraySize > 0 ? arraySize : 1;
		}

		GLenum type;
		std::string name;
		int arraySize;

		int reg;    // First varying register, assigned during link
		int col;    // First register element, assigned during link
	};

	typedef std::list<Varying> VaryingList;

	class Shader
	{
		friend class OutputASM;
	public:
		virtual ~Shader() {};
		virtual sw::Shader *getShader() const = 0;
		virtual sw::PixelShader *getPixelShader() const;
		virtual sw::VertexShader *getVertexShader() const;

	protected:
		VaryingList varyings;
		ActiveUniforms activeUniforms;
		ActiveAttributes activeAttributes;
		ActiveUniformBlocks activeUniformBlocks;
	};

	struct Function
	{
		Function(int label, const char *name, TIntermSequence *arg, TIntermTyped *ret) : label(label), name(name), arg(arg), ret(ret)
		{
		}

		Function(int label, const TString &name, TIntermSequence *arg, TIntermTyped *ret) : label(label), name(name), arg(arg), ret(ret)
		{
		}

		int label;
		TString name;
		TIntermSequence *arg;
		TIntermTyped *ret;
	};
	
	typedef sw::Shader::Instruction Instruction;

	class Temporary;

	class OutputASM : public TIntermTraverser
	{
	public:
		explicit OutputASM(TParseContext &context, Shader *shaderObject);
		~OutputASM();

		void output();

		void freeTemporary(Temporary *temporary);

	private:
		enum Scope
		{
			GLOBAL,
			FUNCTION
		};

		struct TextureFunction
		{
			TextureFunction(const TString& name);

			enum Method
			{
				IMPLICIT,   // Mipmap LOD determined implicitly (standard lookup)
				LOD,
				SIZE,   // textureSize()
				FETCH,
				GRAD
			};

			Method method;
			bool proj;
			bool offset;
		};

		void emitShader(Scope scope);

		// Visit AST nodes and output their code to the body stream
		virtual void visitSymbol(TIntermSymbol*);
		virtual bool visitBinary(Visit visit, TIntermBinary*);
		virtual bool visitUnary(Visit visit, TIntermUnary*);
		virtual bool visitSelection(Visit visit, TIntermSelection*);
		virtual bool visitAggregate(Visit visit, TIntermAggregate*);
		virtual bool visitLoop(Visit visit, TIntermLoop*);
		virtual bool visitBranch(Visit visit, TIntermBranch*);

		sw::Shader::Opcode getOpcode(sw::Shader::Opcode op, TIntermTyped *in) const;
		Instruction *emit(sw::Shader::Opcode op, TIntermTyped *dst = 0, TIntermNode *src0 = 0, TIntermNode *src1 = 0, TIntermNode *src2 = 0, TIntermNode *src3 = 0, TIntermNode *src4 = 0, int index = 0);
		Instruction *emitCast(TIntermTyped *dst, TIntermTyped *src);
		void emitBinary(sw::Shader::Opcode op, TIntermTyped *dst = 0, TIntermNode *src0 = 0, TIntermNode *src1 = 0, TIntermNode *src2 = 0);
		void emitAssign(sw::Shader::Opcode op, TIntermTyped *result, TIntermTyped *lhs, TIntermTyped *src0, TIntermTyped *src1 = 0);
		void emitCmp(sw::Shader::Control cmpOp, TIntermTyped *dst, TIntermNode *left, TIntermNode *right, int index = 0);
		void emitDeterminant(TIntermTyped *result, TIntermTyped *arg, int size, int col = -1, int row = -1, int outCol = 0, int outRow = 0);
		void argument(sw::Shader::SourceParameter &parameter, TIntermNode *argument, int index = 0);
		void copy(TIntermTyped *dst, TIntermNode *src, int offset = 0);
		void assignLvalue(TIntermTyped *dst, TIntermTyped *src);
		int lvalue(sw::Shader::DestinationParameter &dst, Temporary &address, TIntermTyped *node);
		sw::Shader::ParameterType registerType(TIntermTyped *operand);
		unsigned int registerIndex(TIntermTyped *operand);
		int writeMask(TIntermTyped *destination, int index = 0);
		int readSwizzle(TIntermTyped *argument, int size);
		bool trivial(TIntermTyped *expression, int budget);   // Fast to compute and no side effects
		int cost(TIntermNode *expression, int budget);
		const Function *findFunction(const TString &name);

		int temporaryRegister(TIntermTyped *temporary);
		int varyingRegister(TIntermTyped *varying);
		void declareVarying(TIntermTyped *varying, int reg);
		int uniformRegister(TIntermTyped *uniform);
		int attributeRegister(TIntermTyped *attribute);
		int fragmentOutputRegister(TIntermTyped *fragmentOutput);
		int samplerRegister(TIntermTyped *sampler);
		int samplerRegister(TIntermSymbol *sampler);

		typedef std::vector<TIntermTyped*> VariableArray;

		int lookup(VariableArray &list, TIntermTyped *variable);
		int allocate(VariableArray &list, TIntermTyped *variable);
		void free(VariableArray &list, TIntermTyped *variable);

		void declareUniform(const TType &type, const TString &name, int registerIndex, int offset = 0, int blockId = -1);
		GLenum glVariableType(const TType &type);
		GLenum glVariablePrecision(const TType &type);

		static int dim(TIntermNode *v);
		static int dim2(TIntermNode *m);
		static unsigned int loopCount(TIntermLoop *node);
		static bool isSamplerRegister(TIntermTyped *operand);
		static bool isSamplerRegister(const TType &type);

		Shader *const shaderObject;
		sw::Shader *shader;
		sw::PixelShader *pixelShader;
		sw::VertexShader *vertexShader;

		VariableArray temporaries;
		VariableArray uniforms;
		VariableArray varyings;
		VariableArray attributes;
		VariableArray samplers;
		VariableArray fragmentOutputs;

		Scope emitScope;
		Scope currentScope;

		int currentFunction;
		std::vector<Function> functionArray;

		TQualifier outputQualifier;

		TParseContext &mContext;
	};

	// Checks whether a loop can run for a variable number of iterations
	class DetectLoopDiscontinuity : public TIntermTraverser
	{
	public:
		bool traverse(TIntermNode *node);

	private:
		bool visitBranch(Visit visit, TIntermBranch *node);
		bool visitLoop(Visit visit, TIntermLoop *loop);
		bool visitAggregate(Visit visit, TIntermAggregate *node);

		int loopDepth;
		bool loopDiscontinuity;
	};
}

#endif   // COMPILER_OUTPUTASM_H_
