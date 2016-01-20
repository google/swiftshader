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
	struct BlockMemberInfo
	{
		BlockMemberInfo() : offset(-1), arrayStride(-1), matrixStride(-1), isRowMajorMatrix(false) {}

		BlockMemberInfo(int offset, int arrayStride, int matrixStride, bool isRowMajorMatrix)
			: offset(offset),
			arrayStride(arrayStride),
			matrixStride(matrixStride),
			isRowMajorMatrix(isRowMajorMatrix)
		{}

		static BlockMemberInfo getDefaultBlockInfo()
		{
			return BlockMemberInfo(-1, -1, -1, false);
		}

		int offset;
		int arrayStride;
		int matrixStride;
		bool isRowMajorMatrix;
	};

	struct Uniform
	{
		Uniform(GLenum type, GLenum precision, const std::string &name, int arraySize, int registerIndex, int blockId, const BlockMemberInfo& blockMemberInfo);

		GLenum type;
		GLenum precision;
		std::string name;
		int arraySize;
	
		int registerIndex;

		int blockId;
		BlockMemberInfo blockInfo;
	};

	typedef std::vector<Uniform> ActiveUniforms;

	struct UniformBlock
	{
		UniformBlock(const std::string& name, unsigned int dataSize, unsigned int arraySize,
		             TLayoutBlockStorage layout, bool isRowMajorLayout, int registerIndex, int blockId);

		std::string name;
		unsigned int dataSize;
		unsigned int arraySize;
		TLayoutBlockStorage layout;
		bool isRowMajorLayout;
		std::vector<int> fields;

		int registerIndex;

		int blockId;
	};

	class BlockLayoutEncoder
	{
	public:
		BlockLayoutEncoder(bool rowMajor);
		virtual ~BlockLayoutEncoder() {}

		BlockMemberInfo encodeType(const TType &type);

		size_t getBlockSize() const { return mCurrentOffset * BytesPerComponent; }

		virtual void enterAggregateType() = 0;
		virtual void exitAggregateType() = 0;

		static const size_t BytesPerComponent = 4u;
		static const unsigned int ComponentsPerRegister = 4u;

		static size_t getBlockRegister(const BlockMemberInfo &info);
		static size_t getBlockRegisterElement(const BlockMemberInfo &info);

	protected:
		size_t mCurrentOffset;
		bool isRowMajor;

		void nextRegister();

		virtual void getBlockLayoutInfo(const TType &type, unsigned int arraySize, bool isRowMajorMatrix, int *arrayStrideOut, int *matrixStrideOut) = 0;
		virtual void advanceOffset(const TType &type, unsigned int arraySize, bool isRowMajorMatrix, int arrayStride, int matrixStride) = 0;
	};

	// Block layout according to the std140 block layout
	// See "Standard Uniform Block Layout" in Section 2.11.6 of the OpenGL ES 3.0 specification
	class Std140BlockEncoder : public BlockLayoutEncoder
	{
	public:
		Std140BlockEncoder(bool rowMajor);

		void enterAggregateType() override;
		void exitAggregateType() override;

	protected:
		void getBlockLayoutInfo(const TType &type, unsigned int arraySize, bool isRowMajorMatrix, int *arrayStrideOut, int *matrixStrideOut) override;
		void advanceOffset(const TType &type, unsigned int arraySize, bool isRowMajorMatrix, int arrayStride, int matrixStride) override;
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
		Instruction *emit(sw::Shader::Opcode op, TIntermTyped *dst = 0, TIntermNode *src0 = 0, TIntermNode *src1 = 0, TIntermNode *src2 = 0, TIntermNode *src3 = 0, TIntermNode *src4 = 0);
		Instruction *emit(sw::Shader::Opcode op, TIntermTyped *dst, int dstIndex, TIntermNode *src0 = 0, int index0 = 0, TIntermNode *src1 = 0, int index1 = 0,
		                  TIntermNode *src2 = 0, int index2 = 0, TIntermNode *src3 = 0, int index3 = 0, TIntermNode *src4 = 0, int index4 = 0);
		Instruction *emitCast(TIntermTyped *dst, TIntermTyped *src);
		Instruction *emitCast(TIntermTyped *dst, int dstIndex, TIntermTyped *src, int srcIndex);
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

		void declareUniform(const TType &type, const TString &name, int registerIndex, int blockId = -1, BlockLayoutEncoder* encoder = nullptr);
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
