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

#include "compiler/OutputASM.h"

#include "common/debug.h"
#include "compiler/InfoSink.h"

#include "libGLESv2/Shader.h"

#define GL_APICALL
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace sh
{
	// Integer to TString conversion
	TString str(int i)
	{
		char buffer[20];
		sprintf(buffer, "%d", i);
		return buffer;
	}

	class Temporary : public TIntermSymbol
	{
	public:
		Temporary(OutputASM *assembler) : TIntermSymbol(0, "tmp", TType(EbtFloat, EbpHigh, EvqTemporary, 4, false, false)), assembler(assembler)
		{
		}

		~Temporary()
		{
			assembler->freeTemporary(this);
		}

	private:
		OutputASM *const assembler;
	};

	class Constant : public TIntermConstantUnion
	{
	public:
		Constant(float x, float y, float z, float w) : TIntermConstantUnion(constants, TType(EbtFloat, EbpHigh, EvqConst, 4, false, false))
		{
			constants[0].setFConst(x);
			constants[1].setFConst(y);
			constants[2].setFConst(z);
			constants[3].setFConst(w);
		}

		Constant(bool b) : TIntermConstantUnion(constants, TType(EbtBool, EbpHigh, EvqConst, 1, false, false))
		{
			constants[0].setBConst(b);
		}

		Constant(int i) : TIntermConstantUnion(constants, TType(EbtInt, EbpHigh, EvqConst, 1, false, false))
		{
			constants[0].setIConst(i);
		}

		~Constant()
		{
		}

	private:
		ConstantUnion constants[4];
	};

	Uniform::Uniform(GLenum type, GLenum precision, const std::string &name, int arraySize, int registerIndex)
	{
		this->type = type;
		this->precision = precision;
		this->name = name;
		this->arraySize = arraySize;
		this->registerIndex = registerIndex;
	}

	Attribute::Attribute()
	{
		type = GL_NONE;
		arraySize = 0;
		registerIndex = 0;
	}

	Attribute::Attribute(GLenum type, const std::string &name, int arraySize, int registerIndex)
	{
		this->type = type;
		this->name = name;
		this->arraySize = arraySize;
		this->registerIndex = registerIndex;
	}

	OutputASM::OutputASM(TParseContext &context, es2::Shader *shaderObject) : TIntermTraverser(true, true, true), mContext(context), shaderObject(shaderObject)
	{
		shader = 0;
		pixelShader = 0;
		vertexShader = 0;

		if(shaderObject)
		{
			shader = shaderObject->getShader();
			pixelShader = shaderObject->getPixelShader();
			vertexShader = shaderObject->getVertexShader();
		}

		functionArray.push_back(Function(0, "main(", 0, 0));
		currentFunction = 0;
	}

	OutputASM::~OutputASM()
	{
	}

	void OutputASM::output()
	{
		if(shader)
		{
			emitShader(GLOBAL);

			if(functionArray.size() > 1)   // Only call main() when there are other functions
			{
				Instruction *callMain = emit(sw::Shader::OPCODE_CALL);
				callMain->dst.type = sw::Shader::PARAMETER_LABEL;
				callMain->dst.index = 0;   // main()

				emit(sw::Shader::OPCODE_RET);
			}

			emitShader(FUNCTION);
		}
	}

	void OutputASM::emitShader(Scope scope)
	{
		emitScope = scope;
		currentScope = GLOBAL;
		mContext.treeRoot->traverse(this);
	}

	void OutputASM::freeTemporary(Temporary *temporary)
	{
		free(temporaries, temporary);
	}

	void OutputASM::visitSymbol(TIntermSymbol *symbol)
	{
		if(symbol->getQualifier() == EvqVaryingOut || symbol->getQualifier() == EvqInvariantVaryingOut)
		{
			// Vertex varyings don't have to be actively used to successfully link
			// against pixel shaders that use them. So make sure they're declared.
			declareVarying(symbol, -1);
		}
	}

	bool OutputASM::visitBinary(Visit visit, TIntermBinary *node)
	{
		if(currentScope != emitScope)
		{
			return false;
		}

		TIntermTyped *result = node;
		TIntermTyped *left = node->getLeft();
		TIntermTyped *right = node->getRight();
		const TType &leftType = left->getType();
		const TType &rightType = right->getType();
		const TType &resultType = node->getType();
		
		switch(node->getOp())
		{
		case EOpAssign:
			if(visit == PostVisit)
			{
				assignLvalue(left, right);
				copy(result, right);
			}
			break;
		case EOpInitialize:
			if(visit == PostVisit)
			{
				copy(left, right);
			}
			break;
		case EOpMatrixTimesScalarAssign:
			if(visit == PostVisit)
			{
				for(int i = 0; i < leftType.getNominalSize(); i++)
				{
					Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, left, right);
					mul->dst.index += i;
					argument(mul->src[0], left, i);
				}

				assignLvalue(left, result);
			}
			break;
		case EOpVectorTimesMatrixAssign:
			if(visit == PostVisit)
			{
				int size = leftType.getNominalSize();

				for(int i = 0; i < size; i++)
				{
					Instruction *dot = emit(sw::Shader::OPCODE_DP(size), result, left, right);
					dot->dst.mask = 1 << i;
					argument(dot->src[1], right, i);
				}

				assignLvalue(left, result);
			}
			break;
		case EOpMatrixTimesMatrixAssign:
			if(visit == PostVisit)
			{
				int dim = leftType.getNominalSize();

				for(int i = 0; i < dim; i++)
				{
					Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, left, right);
					mul->dst.index += i;
					argument(mul->src[1], right, i);
					mul->src[1].swizzle = 0x00;

					for(int j = 1; j < dim; j++)
					{
						Instruction *mad = emit(sw::Shader::OPCODE_MAD, result, left, right, result);
						mad->dst.index += i;
						argument(mad->src[0], left, j);
						argument(mad->src[1], right, i);
						mad->src[1].swizzle = j * 0x55;
						argument(mad->src[2], result, i);
					}
				}

				assignLvalue(left, result);
			}
			break;
		case EOpIndexDirect:
			if(visit == PostVisit)
			{
				int index = right->getAsConstantUnion()->getUnionArrayPointer()->getIConst();

				if(result->isMatrix() || result->isStruct())
 				{
					ASSERT(left->isArray());
					copy(result, left, index * left->elementRegisterCount());
 				}
				else if(result->isRegister())
 				{
					Instruction *mov = emit(sw::Shader::OPCODE_MOV, result, left);

					if(left->isRegister())
					{
						mov->src[0].swizzle = index;
					}
					else if(left->isArray())
					{
						argument(mov->src[0], left, index * left->elementRegisterCount());
					}
					else if(left->isMatrix())
					{
						ASSERT(index < left->getNominalSize());   // FIXME: Report semantic error
						argument(mov->src[0], left, index);
					}
					else UNREACHABLE();
 				}
				else UNREACHABLE();
			}
			break;
		case EOpIndexIndirect:
			if(visit == PostVisit)
			{
				if(left->isArray() || left->isMatrix())
				{
					for(int index = 0; index < result->totalRegisterCount(); index++)
					{
						Instruction *mov = emit(sw::Shader::OPCODE_MOV, result, left);
						mov->dst.index += index;
						mov->dst.mask = writeMask(result, index);
						argument(mov->src[0], left, index);

						if(left->totalRegisterCount() > 1)
						{
							sw::Shader::SourceParameter relativeRegister;
							argument(relativeRegister, right);

							mov->src[0].rel.type = relativeRegister.type;
							mov->src[0].rel.index = relativeRegister.index;
							mov->src[0].rel.scale =	result->totalRegisterCount();
							mov->src[0].rel.deterministic = !(vertexShader && left->getQualifier() == EvqUniform);
						}
					}
				}
				else if(left->isRegister())
				{
					emit(sw::Shader::OPCODE_EXTRACT, result, left, right);
				}
				else UNREACHABLE();
			}
			break;
		case EOpIndexDirectStruct:
			if(visit == PostVisit)
			{
				ASSERT(leftType.isStruct());

				const TTypeList *structure = leftType.getStruct();
				const TString &fieldName = rightType.getFieldName();
				int fieldOffset = 0;

				for(size_t i = 0; i < structure->size(); i++)
				{
					const TType &fieldType = *(*structure)[i].type;

					if(fieldType.getFieldName() == fieldName)
					{
						break;
					}

					fieldOffset += fieldType.totalRegisterCount();
				}

				copy(result, left, fieldOffset);
			}
			break;
		case EOpVectorSwizzle:
			if(visit == PostVisit)
			{
				int swizzle = 0;
				TIntermAggregate *components = right->getAsAggregate();

				if(components)
				{
					TIntermSequence &sequence = components->getSequence();
					int component = 0;

					for(TIntermSequence::iterator sit = sequence.begin(); sit != sequence.end(); sit++)
					{
						TIntermConstantUnion *element = (*sit)->getAsConstantUnion();

						if(element)
						{
							int i = element->getUnionArrayPointer()[0].getIConst();
							swizzle |= i << (component * 2);
							component++;
						}
						else UNREACHABLE();
					}
				}
				else UNREACHABLE();

				Instruction *mov = emit(sw::Shader::OPCODE_MOV, result, left);
				mov->src[0].swizzle = swizzle;
			}
			break;
		case EOpAddAssign: if(visit == PostVisit) emitAssign(sw::Shader::OPCODE_ADD, result, left, left, right); break;
		case EOpAdd:       if(visit == PostVisit) emitBinary(sw::Shader::OPCODE_ADD, result, left, right);       break;
		case EOpSubAssign: if(visit == PostVisit) emitAssign(sw::Shader::OPCODE_SUB, result, left, left, right); break;
		case EOpSub:       if(visit == PostVisit) emitBinary(sw::Shader::OPCODE_SUB, result, left, right);       break;
		case EOpMulAssign: if(visit == PostVisit) emitAssign(sw::Shader::OPCODE_MUL, result, left, left, right); break;
		case EOpMul:       if(visit == PostVisit) emitBinary(sw::Shader::OPCODE_MUL, result, left, right);       break;
		case EOpDivAssign: if(visit == PostVisit) emitAssign(sw::Shader::OPCODE_DIV, result, left, left, right); break;
		case EOpDiv:       if(visit == PostVisit) emitBinary(sw::Shader::OPCODE_DIV, result, left, right);       break;
		case EOpEqual:
			if(visit == PostVisit)
			{
				emitCmp(sw::Shader::CONTROL_EQ, result, left, right);

				for(int index = 1; index < left->totalRegisterCount(); index++)
				{
					Temporary equal(this);
					emitCmp(sw::Shader::CONTROL_EQ, &equal, left, right, index);
					emit(sw::Shader::OPCODE_AND, result, result, &equal);
				}
			}
			break;
		case EOpNotEqual:
			if(visit == PostVisit)
			{
				emitCmp(sw::Shader::CONTROL_NE, result, left, right);

				for(int index = 1; index < left->totalRegisterCount(); index++)
				{
					Temporary notEqual(this);
					emitCmp(sw::Shader::CONTROL_NE, &notEqual, left, right, index);
					emit(sw::Shader::OPCODE_OR, result, result, &notEqual);
				}
			}
			break;
		case EOpLessThan:                if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_LT, result, left, right); break;
		case EOpGreaterThan:             if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_GT, result, left, right); break;
		case EOpLessThanEqual:           if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_LE, result, left, right); break;
		case EOpGreaterThanEqual:        if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_GE, result, left, right); break;
		case EOpVectorTimesScalarAssign: if(visit == PostVisit) emitAssign(sw::Shader::OPCODE_MUL, result, left, left, right); break;
		case EOpVectorTimesScalar:       if(visit == PostVisit) emit(sw::Shader::OPCODE_MUL, result, left, right); break;
		case EOpMatrixTimesScalar:
			if(visit == PostVisit)
			{
				for(int i = 0; i < leftType.getNominalSize(); i++)
				{
					Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, left, right);
					mul->dst.index += i;
					argument(mul->src[0], left, i);
				}
			}
			break;
		case EOpVectorTimesMatrix:
			if(visit == PostVisit)
			{
				int size = leftType.getNominalSize();

				for(int i = 0; i < size; i++)
				{
					Instruction *dot = emit(sw::Shader::OPCODE_DP(size), result, left, right);
					dot->dst.mask = 1 << i;
					argument(dot->src[1], right, i);
				}
			}
			break;
		case EOpMatrixTimesVector:
			if(visit == PostVisit)
			{
				Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, left, right);
				mul->src[1].swizzle = 0x00;

				for(int i = 1; i < leftType.getNominalSize(); i++)
				{
					Instruction *mad = emit(sw::Shader::OPCODE_MAD, result, left, right, result);
					argument(mad->src[0], left, i);
					mad->src[1].swizzle = i * 0x55;
				}
			}
			break;
		case EOpMatrixTimesMatrix:
			if(visit == PostVisit)
			{
				int dim = leftType.getNominalSize();

				for(int i = 0; i < dim; i++)
				{
					Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, left, right);
					mul->dst.index += i;
					argument(mul->src[1], right, i);
					mul->src[1].swizzle = 0x00;

					for(int j = 1; j < dim; j++)
					{
						Instruction *mad = emit(sw::Shader::OPCODE_MAD, result, left, right, result);
						mad->dst.index += i;
						argument(mad->src[0], left, j);
						argument(mad->src[1], right, i);
						mad->src[1].swizzle = j * 0x55;
						argument(mad->src[2], result, i);
					}
				}
			}
			break;
		case EOpLogicalOr:
			if(trivial(right, 6))
			{
				if(visit == PostVisit)
				{
					emit(sw::Shader::OPCODE_OR, result, left, right);
				}
			}
			else   // Short-circuit evaluation
			{
				if(visit == InVisit)
				{
					emit(sw::Shader::OPCODE_MOV, result, left);
					Instruction *ifnot = emit(sw::Shader::OPCODE_IF, 0, result);
					ifnot->src[0].modifier = sw::Shader::MODIFIER_NOT;
				}
				else if(visit == PostVisit)
				{
					emit(sw::Shader::OPCODE_MOV, result, right);
					emit(sw::Shader::OPCODE_ENDIF);
				}
			}
			break;
		case EOpLogicalXor:        if(visit == PostVisit) emit(sw::Shader::OPCODE_XOR, result, left, right); break;
		case EOpLogicalAnd:
			if(trivial(right, 6))
			{
				if(visit == PostVisit)
				{
					emit(sw::Shader::OPCODE_AND, result, left, right);
				}
			}
			else   // Short-circuit evaluation
			{
				if(visit == InVisit)
				{
					emit(sw::Shader::OPCODE_MOV, result, left);
					emit(sw::Shader::OPCODE_IF, 0, result);
				}
				else if(visit == PostVisit)
				{
					emit(sw::Shader::OPCODE_MOV, result, right);
					emit(sw::Shader::OPCODE_ENDIF);
				}
			}
			break;
		default: UNREACHABLE();
		}

		return true;
	}

	bool OutputASM::visitUnary(Visit visit, TIntermUnary *node)
	{
		if(currentScope != emitScope)
		{
			return false;
		}

		Constant one(1.0f, 1.0f, 1.0f, 1.0f);
		Constant rad(1.74532925e-2f, 1.74532925e-2f, 1.74532925e-2f, 1.74532925e-2f);
		Constant deg(5.72957795e+1f, 5.72957795e+1f, 5.72957795e+1f, 5.72957795e+1f);

		TIntermTyped *result = node;
		TIntermTyped *arg = node->getOperand();

		switch(node->getOp())
		{
		case EOpNegative:
			if(visit == PostVisit)
			{
				for(int index = 0; index < arg->totalRegisterCount(); index++)
				{
					Instruction *neg = emit(sw::Shader::OPCODE_MOV, result, arg);
					neg->dst.index += index;
					argument(neg->src[0], arg, index);
					neg->src[0].modifier = sw::Shader::MODIFIER_NEGATE;
				}
			}
			break;
		case EOpVectorLogicalNot: if(visit == PostVisit) emit(sw::Shader::OPCODE_NOT, result, arg); break;
		case EOpLogicalNot:       if(visit == PostVisit) emit(sw::Shader::OPCODE_NOT, result, arg); break;
		case EOpPostIncrement:
			if(visit == PostVisit)
			{
				copy(result, arg);

				for(int index = 0; index < arg->totalRegisterCount(); index++)
				{
					Instruction *add = emit(sw::Shader::OPCODE_ADD, arg, arg, &one);
					add->dst.index += index;
					argument(add->src[0], arg, index);
				}

				assignLvalue(arg, arg);
			}
			break;
		case EOpPostDecrement:
			if(visit == PostVisit)
			{
				copy(result, arg);

				for(int index = 0; index < arg->totalRegisterCount(); index++)
				{
					Instruction *sub = emit(sw::Shader::OPCODE_SUB, arg, arg, &one);
					sub->dst.index += index;
					argument(sub->src[0], arg, index);
				}

				assignLvalue(arg, arg);
			}
			break;
		case EOpPreIncrement:
			if(visit == PostVisit)
			{
				for(int index = 0; index < arg->totalRegisterCount(); index++)
				{
					Instruction *add = emit(sw::Shader::OPCODE_ADD, result, arg, &one);
					add->dst.index += index;
					argument(add->src[0], arg, index);
				}

				assignLvalue(arg, result);
			}
			break;
		case EOpPreDecrement:
			if(visit == PostVisit)
			{
				for(int index = 0; index < arg->totalRegisterCount(); index++)
				{
					Instruction *sub = emit(sw::Shader::OPCODE_SUB, result, arg, &one);
					sub->dst.index += index;
					argument(sub->src[0], arg, index);
				}

				assignLvalue(arg, result);
			}
			break;
		case EOpRadians:          if(visit == PostVisit) emit(sw::Shader::OPCODE_MUL, result, arg, &rad); break;
		case EOpDegrees:          if(visit == PostVisit) emit(sw::Shader::OPCODE_MUL, result, arg, &deg); break;
		case EOpSin:              if(visit == PostVisit) emit(sw::Shader::OPCODE_SIN, result, arg); break;
		case EOpCos:              if(visit == PostVisit) emit(sw::Shader::OPCODE_COS, result, arg); break;
		case EOpTan:              if(visit == PostVisit) emit(sw::Shader::OPCODE_TAN, result, arg); break;
		case EOpAsin:             if(visit == PostVisit) emit(sw::Shader::OPCODE_ASIN, result, arg); break;
		case EOpAcos:             if(visit == PostVisit) emit(sw::Shader::OPCODE_ACOS, result, arg); break;
		case EOpAtan:             if(visit == PostVisit) emit(sw::Shader::OPCODE_ATAN, result, arg); break;
		case EOpExp:              if(visit == PostVisit) emit(sw::Shader::OPCODE_EXP, result, arg); break;
		case EOpLog:              if(visit == PostVisit) emit(sw::Shader::OPCODE_LOG, result, arg); break;
		case EOpExp2:             if(visit == PostVisit) emit(sw::Shader::OPCODE_EXP2, result, arg); break;
		case EOpLog2:             if(visit == PostVisit) emit(sw::Shader::OPCODE_LOG2, result, arg); break;
		case EOpSqrt:             if(visit == PostVisit) emit(sw::Shader::OPCODE_SQRT, result, arg); break;
		case EOpInverseSqrt:      if(visit == PostVisit) emit(sw::Shader::OPCODE_RSQ, result, arg); break;
		case EOpAbs:              if(visit == PostVisit) emit(sw::Shader::OPCODE_ABS, result, arg); break;
		case EOpSign:             if(visit == PostVisit) emit(sw::Shader::OPCODE_SGN, result, arg); break;
		case EOpFloor:            if(visit == PostVisit) emit(sw::Shader::OPCODE_FLOOR, result, arg); break;
		case EOpCeil:             if(visit == PostVisit) emit(sw::Shader::OPCODE_CEIL, result, arg, result); break;
		case EOpFract:            if(visit == PostVisit) emit(sw::Shader::OPCODE_FRC, result, arg); break;
		case EOpLength:           if(visit == PostVisit) emit(sw::Shader::OPCODE_LEN(dim(arg)), result, arg); break;
		case EOpNormalize:        if(visit == PostVisit) emit(sw::Shader::OPCODE_NRM(dim(arg)), result, arg); break;
		case EOpDFdx:             if(visit == PostVisit) emit(sw::Shader::OPCODE_DFDX, result, arg); break;
		case EOpDFdy:             if(visit == PostVisit) emit(sw::Shader::OPCODE_DFDY, result, arg); break;
		case EOpFwidth:           if(visit == PostVisit) emit(sw::Shader::OPCODE_FWIDTH, result, arg); break;
		case EOpAny:              if(visit == PostVisit) emit(sw::Shader::OPCODE_ANY, result, arg); break;
		case EOpAll:              if(visit == PostVisit) emit(sw::Shader::OPCODE_ALL, result, arg); break;
		default: UNREACHABLE();
		}

		return true;
	}

	bool OutputASM::visitAggregate(Visit visit, TIntermAggregate *node)
	{
		if(currentScope != emitScope && node->getOp() != EOpFunction && node->getOp() != EOpSequence)
		{
			return false;
		}

		Constant zero(0.0f, 0.0f, 0.0f, 0.0f);

		TIntermTyped *result = node;
		const TType &resultType = node->getType();
		TIntermSequence &arg = node->getSequence();
		int argumentCount = arg.size();

		switch(node->getOp())
		{
		case EOpSequence:           break;
		case EOpDeclaration:        break;
		case EOpPrototype:          break;
		case EOpComma:
			if(visit == PostVisit)
			{
				copy(result, arg[1]);
			}
			break;
		case EOpFunction:
			if(visit == PreVisit)
			{
				const TString &name = node->getName();

				if(emitScope == FUNCTION)
				{
					if(functionArray.size() > 1)   // No need for a label when there's only main()
					{
						Instruction *label = emit(sw::Shader::OPCODE_LABEL);
						label->dst.type = sw::Shader::PARAMETER_LABEL;

						const Function *function = findFunction(name);
						ASSERT(function);   // Should have been added during global pass
						label->dst.index = function->label;
						currentFunction = function->label;
					}
				}
				else if(emitScope == GLOBAL)
				{
					if(name != "main(")
					{
						TIntermSequence &arguments = node->getSequence()[0]->getAsAggregate()->getSequence();
						functionArray.push_back(Function(functionArray.size(), name, &arguments, node));
					}
				}
				else UNREACHABLE();

				currentScope = FUNCTION;
			}
			else if(visit == PostVisit)
			{
				if(emitScope == FUNCTION)
				{
					if(functionArray.size() > 1)   // No need to return when there's only main()
					{
						emit(sw::Shader::OPCODE_RET);
					}
				}

				currentScope = GLOBAL;
			}
			break;
		case EOpFunctionCall:
			if(visit == PostVisit)
			{
				if(node->isUserDefined())
				{
					const TString &name = node->getName();
					const Function *function = findFunction(name);

					if(!function)
					{
						mContext.error(node->getLine(), "function definition not found", name.c_str());
						return false;
					}

					TIntermSequence &arguments = *function->arg;

					for(int i = 0; i < argumentCount; i++)
					{
						TIntermTyped *in = arguments[i]->getAsTyped();

						if(in->getQualifier() == EvqIn ||
						   in->getQualifier() == EvqInOut ||
						   in->getQualifier() == EvqConstReadOnly)
						{
							copy(in, arg[i]);
						}
					}

					Instruction *call = emit(sw::Shader::OPCODE_CALL);
					call->dst.type = sw::Shader::PARAMETER_LABEL;
					call->dst.index = function->label;

					if(function->ret && function->ret->getType().getBasicType() != EbtVoid)
					{
						copy(result, function->ret);
					}

					for(int i = 0; i < argumentCount; i++)
					{
						TIntermTyped *argument = arguments[i]->getAsTyped();
						TIntermTyped *out = arg[i]->getAsTyped();
								
						if(argument->getQualifier() == EvqOut ||
						   argument->getQualifier() == EvqInOut)
						{
							copy(out, argument);
						}
					}
				}
				else
				{
					TString name = TFunction::unmangleName(node->getName());

					if(name == "texture2D" || name == "textureCube")
					{
						if(argumentCount == 2)
						{	
							emit(sw::Shader::OPCODE_TEX, result, arg[1], arg[0]);
						}
						else if(argumentCount == 3)   // bias
						{
							Temporary uvwb(this);
							emit(sw::Shader::OPCODE_MOV, &uvwb, arg[1]);
							Instruction *bias = emit(sw::Shader::OPCODE_MOV, &uvwb, arg[2]);
							bias->dst.mask = 0x8;

							Instruction *tex = emit(sw::Shader::OPCODE_TEX, result, &uvwb, arg[0]);   // FIXME: Implement an efficient TEXLDB instruction
							tex->bias = true;
						}
						else UNREACHABLE();
					}
					else if(name == "texture2DProj")
					{
						TIntermTyped *t = arg[1]->getAsTyped();

						if(argumentCount == 2)
						{
							Instruction *tex = emit(sw::Shader::OPCODE_TEX, result, arg[1], arg[0]);
							tex->project = true;

							if(t->getNominalSize() == 3)
							{
								tex->src[0].swizzle = 0xA4;
							}
							else ASSERT(t->getNominalSize() == 4);
						}
						else if(argumentCount == 3)   // bias
						{
							Temporary proj(this);

							if(t->getNominalSize() == 3)
							{
								Instruction *div = emit(sw::Shader::OPCODE_DIV, &proj, arg[1], arg[1]);
								div->src[1].swizzle = 0xAA;
								div->dst.mask = 0x3;
							}
							else if(t->getNominalSize() == 4)
							{
								Instruction *div = emit(sw::Shader::OPCODE_DIV, &proj, arg[1], arg[1]);
								div->src[1].swizzle = 0xFF;
								div->dst.mask = 0x3;
							}
							else UNREACHABLE();

							Instruction *bias = emit(sw::Shader::OPCODE_MOV, &proj, arg[2]);
							bias->dst.mask = 0x8;

							Instruction *tex = emit(sw::Shader::OPCODE_TEX, result, &proj, arg[0]);
							tex->bias = true;
						}
						else UNREACHABLE();
					}
					else if(name == "texture2DLod" || name == "textureCubeLod")
					{
						Temporary uvwb(this);
						emit(sw::Shader::OPCODE_MOV, &uvwb, arg[1]);
						Instruction *lod = emit(sw::Shader::OPCODE_MOV, &uvwb, arg[2]);
						lod->dst.mask = 0x8;

						emit(sw::Shader::OPCODE_TEXLDL, result, &uvwb, arg[0]);
					}
					else if(name == "texture2DProjLod")
					{
						TIntermTyped *t = arg[1]->getAsTyped();
						Temporary proj(this);

						if(t->getNominalSize() == 3)
						{
							Instruction *div = emit(sw::Shader::OPCODE_DIV, &proj, arg[1], arg[1]);
							div->src[1].swizzle = 0xAA;
							div->dst.mask = 0x3;
						}
						else if(t->getNominalSize() == 4)
						{
							Instruction *div = emit(sw::Shader::OPCODE_DIV, &proj, arg[1], arg[1]);
							div->src[1].swizzle = 0xFF;
							div->dst.mask = 0x3;
						}
						else UNREACHABLE();

						Instruction *lod = emit(sw::Shader::OPCODE_MOV, &proj, arg[2]);
						lod->dst.mask = 0x8;

						emit(sw::Shader::OPCODE_TEXLDL, result, &proj, arg[0]);
					}
					else UNREACHABLE();
				}
			}
			break;
		case EOpParameters:
			break;
		case EOpConstructFloat:
		case EOpConstructVec2:
		case EOpConstructVec3:
		case EOpConstructVec4:
		case EOpConstructBool:
		case EOpConstructBVec2:
		case EOpConstructBVec3:
		case EOpConstructBVec4:
		case EOpConstructInt:
		case EOpConstructIVec2:
		case EOpConstructIVec3:
		case EOpConstructIVec4:
			if(visit == PostVisit)
			{
				int component = 0;

				for(int i = 0; i < argumentCount; i++)
				{
					TIntermTyped *argi = arg[i]->getAsTyped();
					int size = argi->getNominalSize();

					if(!argi->isMatrix())
					{
						Instruction *mov = emitCast(result, argi);
						mov->dst.mask = (0xF << component) & 0xF;
						mov->src[0].swizzle = readSwizzle(argi, size) << (component * 2);

						component += size;
					}
					else   // Matrix
					{
						int column = 0;

						while(component < resultType.getNominalSize())
						{
							Instruction *mov = emitCast(result, argi);
							mov->dst.mask = (0xF << component) & 0xF;
							mov->src[0].index += column;
							mov->src[0].swizzle = readSwizzle(argi, size) << (component * 2);

							column++;
							component += size;
						}
					}
				}
			}
			break;
		case EOpConstructMat2:
		case EOpConstructMat3:
		case EOpConstructMat4:
			if(visit == PostVisit)
			{
				TIntermTyped *arg0 = arg[0]->getAsTyped();
				const int dim = result->getNominalSize();

				if(arg0->isScalar() && arg.size() == 1)   // Construct scale matrix
				{
					for(int i = 0; i < dim; i++)
					{
						Instruction *init = emit(sw::Shader::OPCODE_MOV, result, &zero);
						init->dst.index += i;
						Instruction *mov = emitCast(result, arg0);
						mov->dst.index += i;
						mov->dst.mask = 1 << i;
						ASSERT(mov->src[0].swizzle == 0x00);
					}
				}
				else if(arg0->isMatrix())
				{
					for(int i = 0; i < dim; i++)
					{
						if(dim > dim2(arg0))
						{
							// Initialize to identity matrix
							Constant col((i == 0 ? 1.0f : 0.0f), (i == 1 ? 1.0f : 0.0f), (i == 2 ? 1.0f : 0.0f), (i == 3 ? 1.0f : 0.0f));
							Instruction *mov = emitCast(result, &col);
							mov->dst.index += i;
						}

						if(i < dim2(arg0))
						{
							Instruction *mov = emitCast(result, arg0);
							mov->dst.index += i;
							mov->dst.mask = 0xF >> (4 - dim2(arg0));
							argument(mov->src[0], arg0, i);
						}
					}
				}
				else
				{
					int column = 0;
					int row = 0;

					for(int i = 0; i < argumentCount; i++)
					{
						TIntermTyped *argi = arg[i]->getAsTyped();
						int size = argi->getNominalSize();
						int element = 0;

						while(element < size)
						{
							Instruction *mov = emitCast(result, argi);
							mov->dst.index += column;
							mov->dst.mask = (0xF << row) & 0xF;
							mov->src[0].swizzle = (readSwizzle(argi, size) << (row * 2)) + 0x55 * element;

							int end = row + size - element;
							column = end >= dim ? column + 1 : column;
							element = element + dim - row;
							row = end >= dim ? 0 : end;
						}
					}
				}
			}
			break;
		case EOpConstructStruct:
			if(visit == PostVisit)
			{
				int offset = 0;
				for(int i = 0; i < argumentCount; i++)
				{
					TIntermTyped *argi = arg[i]->getAsTyped();
					int size = argi->totalRegisterCount();

					for(int index = 0; index < size; index++)
					{
						Instruction *mov = emit(sw::Shader::OPCODE_MOV, result, argi);
						mov->dst.index += index + offset;
						mov->dst.mask = writeMask(result, offset + index);
						argument(mov->src[0], argi, index);
					}

					offset += size;
				}
			}
			break;
		case EOpLessThan:         if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_LT, result, arg[0], arg[1]); break;
		case EOpGreaterThan:      if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_GT, result, arg[0], arg[1]); break;
		case EOpLessThanEqual:    if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_LE, result, arg[0], arg[1]); break;
		case EOpGreaterThanEqual: if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_GE, result, arg[0], arg[1]); break;
		case EOpVectorEqual:      if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_EQ, result, arg[0], arg[1]); break;
		case EOpVectorNotEqual:   if(visit == PostVisit) emitCmp(sw::Shader::CONTROL_NE, result, arg[0], arg[1]); break;
		case EOpMod:              if(visit == PostVisit) emit(sw::Shader::OPCODE_MOD, result, arg[0], arg[1]); break;
		case EOpPow:              if(visit == PostVisit) emit(sw::Shader::OPCODE_POW, result, arg[0], arg[1]); break;
		case EOpAtan:             if(visit == PostVisit) emit(sw::Shader::OPCODE_ATAN2, result, arg[0], arg[1]); break;
		case EOpMin:              if(visit == PostVisit) emit(sw::Shader::OPCODE_MIN, result, arg[0], arg[1]); break;
		case EOpMax:              if(visit == PostVisit) emit(sw::Shader::OPCODE_MAX, result, arg[0], arg[1]); break;
		case EOpClamp:
			if(visit == PostVisit)
			{
				emit(sw::Shader::OPCODE_MAX, result, arg[0], arg[1]);
				emit(sw::Shader::OPCODE_MIN, result, result, arg[2]);
			}
			break;
		case EOpMix:         if(visit == PostVisit) emit(sw::Shader::OPCODE_LRP, result, arg[2], arg[1], arg[0]); break;
		case EOpStep:        if(visit == PostVisit) emit(sw::Shader::OPCODE_STEP, result, arg[0], arg[1]); break;
		case EOpSmoothStep:  if(visit == PostVisit) emit(sw::Shader::OPCODE_SMOOTH, result, arg[0], arg[1], arg[2]); break;
		case EOpDistance:    if(visit == PostVisit) emit(sw::Shader::OPCODE_DIST(dim(arg[0])), result, arg[0], arg[1]); break;
		case EOpDot:         if(visit == PostVisit) emit(sw::Shader::OPCODE_DP(dim(arg[0])), result, arg[0], arg[1]); break;
		case EOpCross:       if(visit == PostVisit) emit(sw::Shader::OPCODE_CRS, result, arg[0], arg[1]); break;
		case EOpFaceForward: if(visit == PostVisit) emit(sw::Shader::OPCODE_FORWARD(dim(arg[0])), result, arg[0], arg[1], arg[2]); break;
		case EOpReflect:     if(visit == PostVisit) emit(sw::Shader::OPCODE_REFLECT(dim(arg[0])), result, arg[0], arg[1]); break;
		case EOpRefract:     if(visit == PostVisit) emit(sw::Shader::OPCODE_REFRACT(dim(arg[0])), result, arg[0], arg[1], arg[2]); break;
		case EOpMul:
			if(visit == PostVisit)
			{
				ASSERT(dim2(arg[0]) == dim2(arg[1]));

				for(int i = 0; i < dim2(arg[0]); i++)
				{
					Instruction *mul = emit(sw::Shader::OPCODE_MUL, result, arg[0], arg[1]);
					mul->dst.index += i;
					argument(mul->src[0], arg[0], i);
					argument(mul->src[1], arg[1], i);
				}
			}
			break;
		default: UNREACHABLE();
		}

		return true;
	}

	bool OutputASM::visitSelection(Visit visit, TIntermSelection *node)
	{
		if(currentScope != emitScope)
		{
			return false;
		}

		TIntermTyped *condition = node->getCondition();
		TIntermNode *trueBlock = node->getTrueBlock();
		TIntermNode *falseBlock = node->getFalseBlock();
		TIntermConstantUnion *constantCondition = condition->getAsConstantUnion();

		condition->traverse(this);

		if(node->usesTernaryOperator())
		{
			if(constantCondition)
			{
				bool trueCondition = constantCondition->getUnionArrayPointer()->getBConst();

				if(trueCondition)
				{
					trueBlock->traverse(this);
					copy(node, trueBlock);
				}
				else
				{
					falseBlock->traverse(this);
					copy(node, falseBlock);
				}
			}
			else if(trivial(node, 6))   // Fast to compute both potential results and no side effects
			{
				trueBlock->traverse(this);
				falseBlock->traverse(this);
				emit(sw::Shader::OPCODE_SELECT, node, condition, trueBlock, falseBlock);
			}
			else
			{
				emit(sw::Shader::OPCODE_IF, 0, condition);

				if(trueBlock)
				{
					trueBlock->traverse(this);
					copy(node, trueBlock);
				}

				if(falseBlock)
				{
					emit(sw::Shader::OPCODE_ELSE);
					falseBlock->traverse(this);
					copy(node, falseBlock);
				}

				emit(sw::Shader::OPCODE_ENDIF);
			}
		}
		else  // if/else statement
		{
			if(constantCondition)
			{
				bool trueCondition = constantCondition->getUnionArrayPointer()->getBConst();

				if(trueCondition)
				{
					if(trueBlock)
					{
						trueBlock->traverse(this);
					}
				}
				else
				{
					if(falseBlock)
					{
						falseBlock->traverse(this);
					}
				}
			}
			else
			{
				emit(sw::Shader::OPCODE_IF, 0, condition);

				if(trueBlock)
				{
					trueBlock->traverse(this);
				}

				if(falseBlock)
				{
					emit(sw::Shader::OPCODE_ELSE);
					falseBlock->traverse(this);
				}

				emit(sw::Shader::OPCODE_ENDIF);
			}
		}

		return false;
	}

	bool OutputASM::visitLoop(Visit visit, TIntermLoop *node)
	{
		if(currentScope != emitScope)
		{
			return false;
		}

		unsigned int iterations = loopCount(node);

		if(iterations == 0)
		{
			return false;
		}

		bool unroll = (iterations <= 4);

		if(unroll)
		{
			DetectLoopDiscontinuity detectLoopDiscontinuity;
			unroll = !detectLoopDiscontinuity.traverse(node);
		}

		TIntermNode *init = node->getInit();
		TIntermTyped *condition = node->getCondition();
		TIntermTyped *expression = node->getExpression();
		TIntermNode *body = node->getBody();

		if(node->getType() == ELoopDoWhile)
		{
			Temporary iterate(this);
			Constant True(true);
			emit(sw::Shader::OPCODE_MOV, &iterate, &True);

			emit(sw::Shader::OPCODE_WHILE, 0, &iterate);   // FIXME: Implement real do-while

			if(body)
			{
				body->traverse(this);
			}

			emit(sw::Shader::OPCODE_TEST);

			condition->traverse(this);
			emit(sw::Shader::OPCODE_MOV, &iterate, condition);

			emit(sw::Shader::OPCODE_ENDWHILE);
		}
		else
		{
			if(init)
			{
				init->traverse(this);
			}

			if(unroll)
			{
				for(unsigned int i = 0; i < iterations; i++)
				{
				//	condition->traverse(this);   // Condition could contain statements, but not in an unrollable loop

					if(body)
					{
						body->traverse(this);
					}

					if(expression)
					{
						expression->traverse(this);
					}
				}
			}
			else
			{
				condition->traverse(this);

				emit(sw::Shader::OPCODE_WHILE, 0, condition);

				if(body)
				{
					body->traverse(this);
				}

				emit(sw::Shader::OPCODE_TEST);

				if(expression)
				{
					expression->traverse(this);
				}

				condition->traverse(this);

				emit(sw::Shader::OPCODE_ENDWHILE);
			}
		}

		return false;
	}

	bool OutputASM::visitBranch(Visit visit, TIntermBranch *node)
	{
		if(currentScope != emitScope)
		{
			return false;
		}

		switch(node->getFlowOp())
		{
		case EOpKill:      if(visit == PostVisit) emit(sw::Shader::OPCODE_DISCARD);  break;
		case EOpBreak:     if(visit == PostVisit) emit(sw::Shader::OPCODE_BREAK);    break;
		case EOpContinue:  if(visit == PostVisit) emit(sw::Shader::OPCODE_CONTINUE); break;
		case EOpReturn:
			if(visit == PostVisit)
			{
				TIntermTyped *value = node->getExpression();

				if(value)
				{
					copy(functionArray[currentFunction].ret, value);
				}

				emit(sw::Shader::OPCODE_LEAVE);
			}
			break;
		default: UNREACHABLE();
		}

		return true;
	}

	Instruction *OutputASM::emit(sw::Shader::Opcode op, TIntermTyped *dst, TIntermNode *src0, TIntermNode *src1, TIntermNode *src2, int index)
	{
		if(dst && registerType(dst) == sw::Shader::PARAMETER_SAMPLER)
		{
			op = sw::Shader::OPCODE_NULL;   // Can't assign to a sampler, but this is hit when indexing sampler arrays
		}

		Instruction *instruction = new Instruction(op);

		if(dst)
		{
			instruction->dst.type = registerType(dst);
			instruction->dst.index = registerIndex(dst) + index;
			instruction->dst.mask = writeMask(dst);
			instruction->dst.integer = (dst->getBasicType() == EbtInt);
		}

		argument(instruction->src[0], src0, index);
		argument(instruction->src[1], src1, index);
		argument(instruction->src[2], src2, index);

		shader->append(instruction);

		return instruction;
	}

	Instruction *OutputASM::emitCast(TIntermTyped *dst, TIntermTyped *src)
	{
		// Integers are implemented as float
		if((dst->getBasicType() == EbtFloat || dst->getBasicType() == EbtInt) && src->getBasicType() == EbtBool)
		{
			return emit(sw::Shader::OPCODE_B2F, dst, src);
		}
		if(dst->getBasicType() == EbtBool && (src->getBasicType() == EbtFloat || src->getBasicType() == EbtInt))
		{
			return emit(sw::Shader::OPCODE_F2B, dst, src);
		}
		if(dst->getBasicType() == EbtInt && src->getBasicType() == EbtFloat)
		{
			return emit(sw::Shader::OPCODE_TRUNC, dst, src);
		}

		return emit(sw::Shader::OPCODE_MOV, dst, src);
	}

	void OutputASM::emitBinary(sw::Shader::Opcode op, TIntermTyped *dst, TIntermNode *src0, TIntermNode *src1, TIntermNode *src2)
	{
		for(int index = 0; index < dst->elementRegisterCount(); index++)
		{
			emit(op, dst, src0, src1, src2, index);
		}
	}

	void OutputASM::emitAssign(sw::Shader::Opcode op, TIntermTyped *result, TIntermTyped *lhs, TIntermTyped *src0, TIntermTyped *src1)
	{
		emitBinary(op, result, src0, src1);
		assignLvalue(lhs, result);
	}

	void OutputASM::emitCmp(sw::Shader::Control cmpOp, TIntermTyped *dst, TIntermNode *left, TIntermNode *right, int index)
	{
		bool boolean = (left->getAsTyped()->getBasicType() == EbtBool);
		sw::Shader::Opcode opcode = boolean ? sw::Shader::OPCODE_ICMP : sw::Shader::OPCODE_CMP;

		Instruction *cmp = emit(opcode, dst, left, right);
		cmp->control = cmpOp;
		argument(cmp->src[0], left, index);
		argument(cmp->src[1], right, index);
	}

	int componentCount(const TType &type, int registers)
	{
		if(registers == 0)
		{
			return 0;
		}

		if(type.isArray() && registers >= type.elementRegisterCount())
		{
			int index = registers / type.elementRegisterCount();
			registers -= index * type.elementRegisterCount();
			return index * type.getElementSize() + componentCount(type, registers);
		}

		if(type.isStruct())
		{
			TTypeList *structure = type.getStruct();
			int elements = 0;

			for(TTypeList::const_iterator field = structure->begin(); field != structure->end(); field++)
			{
				const TType &fieldType = *field->type;

				if(fieldType.totalRegisterCount() <= registers)
				{
					registers -= fieldType.totalRegisterCount();
					elements += fieldType.getObjectSize();
				}
				else   // Register within this field
				{
					return elements + componentCount(fieldType, registers);
				}
			}
		}
		else if(type.isMatrix())
		{
			return registers * type.getNominalSize();
		}
		
		UNREACHABLE();
		return 0;
	}

	int registerSize(const TType &type, int registers)
	{
		if(registers == 0)
		{
			if(type.isStruct())
			{
				return registerSize(*type.getStruct()->begin()->type, 0);
			}

			return type.getNominalSize();
		}

		if(type.isArray() && registers >= type.elementRegisterCount())
		{
			int index = registers / type.elementRegisterCount();
			registers -= index * type.elementRegisterCount();
			return registerSize(type, registers);
		}

		if(type.isStruct())
		{
			TTypeList *structure = type.getStruct();
			int elements = 0;

			for(TTypeList::const_iterator field = structure->begin(); field != structure->end(); field++)
			{
				const TType &fieldType = *field->type;
				
				if(fieldType.totalRegisterCount() <= registers)
				{
					registers -= fieldType.totalRegisterCount();
					elements += fieldType.getObjectSize();
				}
				else   // Register within this field
				{
					return registerSize(fieldType, registers);
				}
			}
		}
		else if(type.isMatrix())
		{
			return registerSize(type, 0);
		}
		
		UNREACHABLE();
		return 0;
	}

	void OutputASM::argument(sw::Shader::SourceParameter &parameter, TIntermNode *argument, int index)
	{
		if(argument)
		{
			TIntermTyped *arg = argument->getAsTyped();
			const TType &type = arg->getType();
			const TTypeList *structure = type.getStruct();
			ASSERT(index < arg->totalRegisterCount());

			int size = registerSize(type, index);

			parameter.type = registerType(arg);

			if(arg->getQualifier() == EvqConst)
			{
				int component = componentCount(type, index);
				ConstantUnion *constants = arg->getAsConstantUnion()->getUnionArrayPointer();

				for(int i = 0; i < 4; i++)
				{
					if(size == 1)   // Replicate
					{
						parameter.value[i] = constants[component + 0].getAsFloat();
					}
					else if(i < size)
					{
						parameter.value[i] = constants[component + i].getAsFloat();
					}
					else
					{
						parameter.value[i] = 0.0f;
					}
				}
			}
			else
			{
				parameter.index = registerIndex(arg) + index;

				if(registerType(arg) == sw::Shader::PARAMETER_SAMPLER)
				{
					TIntermBinary *binary = argument->getAsBinaryNode();

					if(binary)
					{
						TIntermTyped *left = binary->getLeft();
						TIntermTyped *right = binary->getRight();

						if(binary->getOp() == EOpIndexDirect)
						{
							parameter.index += right->getAsConstantUnion()->getUnionArrayPointer()->getIConst();
						}
						else if(binary->getOp() == EOpIndexIndirect)
						{
							if(left->getArraySize() > 1)
							{
								parameter.rel.type = registerType(binary->getRight());
								parameter.rel.index = registerIndex(binary->getRight());
								parameter.rel.scale = 1;
								parameter.rel.deterministic = true;
							}
						}
						else UNREACHABLE();
					}
				}
			}

			if(!IsSampler(arg->getBasicType()))
			{
				parameter.swizzle = readSwizzle(arg, size);
			}
		}
	}

	void OutputASM::copy(TIntermTyped *dst, TIntermNode *src, int offset)
	{
		for(int index = 0; index < dst->totalRegisterCount(); index++)
		{
			Instruction *mov = emit(sw::Shader::OPCODE_MOV, dst, src);
			mov->dst.index += index;
			mov->dst.mask = writeMask(dst, index);
			argument(mov->src[0], src, offset + index);
		}
	}

	int swizzleElement(int swizzle, int index)
	{
		return (swizzle >> (index * 2)) & 0x03;
	}

	int swizzleSwizzle(int leftSwizzle, int rightSwizzle)
	{
		return (swizzleElement(leftSwizzle, swizzleElement(rightSwizzle, 0)) << 0) |
		       (swizzleElement(leftSwizzle, swizzleElement(rightSwizzle, 1)) << 2) |
		       (swizzleElement(leftSwizzle, swizzleElement(rightSwizzle, 2)) << 4) |
		       (swizzleElement(leftSwizzle, swizzleElement(rightSwizzle, 3)) << 6);
	}

	void OutputASM::assignLvalue(TIntermTyped *dst, TIntermNode *src)
	{
		TIntermBinary *binary = dst->getAsBinaryNode();

		if(binary && binary->getOp() == EOpIndexIndirect && dst->isScalar())
		{
			Instruction *insert = new Instruction(sw::Shader::OPCODE_INSERT);
			
			Temporary address(this);
			lvalue(insert->dst, address, dst);

			insert->src[0].type = insert->dst.type;
			insert->src[0].index = insert->dst.index;
			insert->src[0].rel = insert->dst.rel;
			argument(insert->src[1], src);
			argument(insert->src[2], binary->getRight());

			shader->append(insert);
		}
		else
		{
			for(int offset = 0; offset < dst->totalRegisterCount(); offset++)
			{
				Instruction *mov = new Instruction(sw::Shader::OPCODE_MOV);
			
				Temporary address(this);
				int swizzle = lvalue(mov->dst, address, dst);
				mov->dst.index += offset;

				if(offset > 0)
				{
					mov->dst.mask = writeMask(dst, offset);
				}

				argument(mov->src[0], src, offset);
				mov->src[0].swizzle = swizzleSwizzle(mov->src[0].swizzle, swizzle);

				shader->append(mov);
			}
		}
	}

	int OutputASM::lvalue(sw::Shader::DestinationParameter &dst, Temporary &address, TIntermTyped *node)
	{
		TIntermTyped *result = node;
		TIntermBinary *binary = node->getAsBinaryNode();
		TIntermSymbol *symbol = node->getAsSymbolNode();

		if(binary)
		{
			TIntermTyped *left = binary->getLeft();
			TIntermTyped *right = binary->getRight();

			int leftSwizzle = lvalue(dst, address, left);   // Resolve the l-value of the left side

			switch(binary->getOp())
			{
			case EOpIndexDirect:
				{
					int rightIndex = right->getAsConstantUnion()->getUnionArrayPointer()->getIConst();

					if(left->isRegister())
					{
						int leftMask = dst.mask;
						
						dst.mask = 1;
						while((leftMask & dst.mask) == 0)
						{
							dst.mask = dst.mask << 1;
						}

						int element = swizzleElement(leftSwizzle, rightIndex);
						dst.mask = 1 << element;
						
						return element;
					}
					else if(left->isArray() || left->isMatrix())
					{
						dst.index += rightIndex * result->totalRegisterCount();
						return 0xE4;
					}
					else UNREACHABLE();
				}
				break;
			case EOpIndexIndirect:
				{
					if(left->isRegister())
					{
						// Requires INSERT instruction (handled by calling function)
					}
					else if(left->isArray() || left->isMatrix())
					{
						int scale = result->totalRegisterCount();

						if(dst.rel.type == sw::Shader::PARAMETER_VOID)   // Use the index register as the relative address directly
						{
							if(left->totalRegisterCount() > 1)
							{
								sw::Shader::SourceParameter relativeRegister;
								argument(relativeRegister, right);

								dst.rel.index = relativeRegister.index;
								dst.rel.type = relativeRegister.type;
								dst.rel.scale = scale;
								dst.rel.deterministic = !(vertexShader && left->getQualifier() == EvqUniform);
							}
						}
						else if(dst.rel.index != registerIndex(&address))   // Move the previous index register to the address register
						{
							if(scale == 1)
							{
								Constant oldScale((int)dst.rel.scale);
								Instruction *mad = emit(sw::Shader::OPCODE_MAD, &address, &address, &oldScale, right);
								mad->src[0].index = dst.rel.index;
								mad->src[0].type = dst.rel.type;
							}
							else
							{
								Constant oldScale((int)dst.rel.scale);
								Instruction *mul = emit(sw::Shader::OPCODE_MUL, &address, &address, &oldScale);
								mul->src[0].index = dst.rel.index;
								mul->src[0].type = dst.rel.type;

								Constant newScale(scale);
								emit(sw::Shader::OPCODE_MAD, &address, right, &newScale, &address);
							}

							dst.rel.type = sw::Shader::PARAMETER_TEMP;
							dst.rel.index = registerIndex(&address);
							dst.rel.scale = 1;
						}
						else   // Just add the new index to the address register
						{
							if(scale == 1)
							{
								emit(sw::Shader::OPCODE_ADD, &address, &address, right);
							}
							else
							{
								Constant newScale(scale);
								emit(sw::Shader::OPCODE_MAD, &address, right, &newScale, &address);
							}
						}
					}
					else UNREACHABLE();
				}
				break;
			case EOpIndexDirectStruct:
				{
					const TTypeList *structure = left->getType().getStruct();
					const TString &fieldName = right->getType().getFieldName();

					int offset = 0;
					for(TTypeList::const_iterator field = structure->begin(); field != structure->end(); field++)
					{
						if(field->type->getFieldName() == fieldName)
						{
							dst.type = registerType(left);
							dst.index += offset;
							dst.mask = writeMask(right);
							
							return 0xE4;
						}

						offset += field->type->totalRegisterCount();
					}
				}
				break;
			case EOpVectorSwizzle:
				{
					ASSERT(left->isRegister());

					int leftMask = dst.mask;

					int swizzle = 0;
					int rightMask = 0;

					TIntermSequence &sequence = right->getAsAggregate()->getSequence();

					for(unsigned int i = 0; i < sequence.size(); i++)
					{
						int index = sequence[i]->getAsConstantUnion()->getUnionArrayPointer()->getIConst();

						int element = swizzleElement(leftSwizzle, index);
						rightMask = rightMask | (1 << element);
						swizzle = swizzle | swizzleElement(leftSwizzle, i) << (element * 2);
					}
					
					dst.mask = leftMask & rightMask;

					return swizzle;
				}
				break;
			default:
				UNREACHABLE();   // Not an l-value operator
				break;
			}
		}
		else if(symbol)
		{
			dst.type = registerType(symbol);
			dst.index = registerIndex(symbol);
			dst.mask = writeMask(symbol);
			return 0xE4;
		}

		return 0xE4;
	}

	sw::Shader::ParameterType OutputASM::registerType(TIntermTyped *operand)
	{
		if(IsSampler(operand->getBasicType()) && (operand->getQualifier() == EvqUniform || operand->getQualifier() == EvqTemporary))   // Function parameters are temporaries
		{
			return sw::Shader::PARAMETER_SAMPLER;
		}

		switch(operand->getQualifier())
		{
		case EvqTemporary:           return sw::Shader::PARAMETER_TEMP;
		case EvqGlobal:              return sw::Shader::PARAMETER_TEMP;
		case EvqConst:               return sw::Shader::PARAMETER_FLOAT4LITERAL;   // All converted to float
		case EvqAttribute:           return sw::Shader::PARAMETER_INPUT;
		case EvqVaryingIn:           return sw::Shader::PARAMETER_INPUT;
		case EvqVaryingOut:          return sw::Shader::PARAMETER_OUTPUT;
		case EvqInvariantVaryingIn:  return sw::Shader::PARAMETER_INPUT;    // FIXME: Guarantee invariance at the backend
		case EvqInvariantVaryingOut: return sw::Shader::PARAMETER_OUTPUT;   // FIXME: Guarantee invariance at the backend 
		case EvqUniform:             return sw::Shader::PARAMETER_CONST;
		case EvqIn:                  return sw::Shader::PARAMETER_TEMP;
		case EvqOut:                 return sw::Shader::PARAMETER_TEMP;
		case EvqInOut:               return sw::Shader::PARAMETER_TEMP;
		case EvqConstReadOnly:       return sw::Shader::PARAMETER_TEMP;
		case EvqPosition:            return sw::Shader::PARAMETER_OUTPUT;
		case EvqPointSize:           return sw::Shader::PARAMETER_OUTPUT;
		case EvqFragCoord:           return sw::Shader::PARAMETER_MISCTYPE;
		case EvqFrontFacing:         return sw::Shader::PARAMETER_MISCTYPE;
		case EvqPointCoord:          return sw::Shader::PARAMETER_INPUT;
		case EvqFragColor:           return sw::Shader::PARAMETER_COLOROUT;
		case EvqFragData:            return sw::Shader::PARAMETER_COLOROUT;
		default: UNREACHABLE();
		}

		return sw::Shader::PARAMETER_VOID;
	}

	int OutputASM::registerIndex(TIntermTyped *operand)
	{
		if(registerType(operand) == sw::Shader::PARAMETER_SAMPLER)
		{
			return samplerRegister(operand);
		}

		switch(operand->getQualifier())
		{
		case EvqTemporary:           return temporaryRegister(operand);
		case EvqGlobal:              return temporaryRegister(operand);
		case EvqConst:               UNREACHABLE();
		case EvqAttribute:           return attributeRegister(operand);
		case EvqVaryingIn:           return varyingRegister(operand);
		case EvqVaryingOut:          return varyingRegister(operand);
		case EvqInvariantVaryingIn:  return varyingRegister(operand);
		case EvqInvariantVaryingOut: return varyingRegister(operand);
		case EvqUniform:             return uniformRegister(operand);
		case EvqIn:                  return temporaryRegister(operand);
		case EvqOut:                 return temporaryRegister(operand);
		case EvqInOut:               return temporaryRegister(operand);
		case EvqConstReadOnly:       return temporaryRegister(operand);
		case EvqPosition:            return varyingRegister(operand);
		case EvqPointSize:           return varyingRegister(operand);
		case EvqFragCoord:           pixelShader->vPosDeclared = true;  return 0;
		case EvqFrontFacing:         pixelShader->vFaceDeclared = true; return 1;
		case EvqPointCoord:          return varyingRegister(operand);
		case EvqFragColor:           return 0;
		case EvqFragData:            return 0;
		default: UNREACHABLE();
		}

		return 0;
	}

	int OutputASM::writeMask(TIntermTyped *destination, int index)
	{
		if(destination->getQualifier() == EvqPointSize)
		{
			return 0x2;   // Point size stored in the y component
		}

		return 0xF >> (4 - registerSize(destination->getType(), index));
	}

	int OutputASM::readSwizzle(TIntermTyped *argument, int size)
	{
		if(argument->getQualifier() == EvqPointSize)
		{
			return 0x55;   // Point size stored in the y component
		}

		static const unsigned char swizzleSize[5] = {0x00, 0x00, 0x54, 0xA4, 0xE4};   // (void), xxxx, xyyy, xyzz, xyzw

		return swizzleSize[size];
	}

	// Conservatively checks whether an expression is fast to compute and has no side effects
	bool OutputASM::trivial(TIntermTyped *expression, int budget)
	{
		if(!expression->isRegister())
		{
			return false;
		}

		return cost(expression, budget) >= 0;
	}

	// Returns the remaining computing budget (if < 0 the expression is too expensive or has side effects)
	int OutputASM::cost(TIntermNode *expression, int budget)
	{
		if(budget < 0)
		{
			return budget;
		}

		if(expression->getAsSymbolNode())
		{
			return budget;
		}
		else if(expression->getAsConstantUnion())
		{
			return budget;
		}
		else if(expression->getAsBinaryNode())
		{
			TIntermBinary *binary = expression->getAsBinaryNode();

			switch(binary->getOp())
			{
			case EOpVectorSwizzle:
			case EOpIndexDirect:
			case EOpIndexDirectStruct:
				return cost(binary->getLeft(), budget - 0);
			case EOpAdd:
			case EOpSub:
			case EOpMul:
				return cost(binary->getLeft(), cost(binary->getRight(), budget - 1));
			default:
				return -1;
			}
		}
		else if(expression->getAsUnaryNode())
		{
			TIntermUnary *unary = expression->getAsUnaryNode();

			switch(unary->getOp())
			{
			case EOpAbs:
			case EOpNegative:
				return cost(unary->getOperand(), budget - 1);
			default:
				return -1;
			}
		}
		else if(expression->getAsSelectionNode())
		{
			TIntermSelection *selection = expression->getAsSelectionNode();

			if(selection->usesTernaryOperator())
			{
				TIntermTyped *condition = selection->getCondition();
				TIntermNode *trueBlock = selection->getTrueBlock();
				TIntermNode *falseBlock = selection->getFalseBlock();
				TIntermConstantUnion *constantCondition = condition->getAsConstantUnion();

				if(constantCondition)
				{
					bool trueCondition = constantCondition->getUnionArrayPointer()->getBConst();

					if(trueCondition)
					{
						return cost(trueBlock, budget - 0);
					}
					else
					{
						return cost(falseBlock, budget - 0);
					}
				}
				else
				{
					return cost(trueBlock, cost(falseBlock, budget - 2));
				}
			}
		}

		return -1;
	}

	const Function *OutputASM::findFunction(const TString &name)
	{
		for(unsigned int f = 0; f < functionArray.size(); f++)
		{
			if(functionArray[f].name == name)
			{
				return &functionArray[f];
			}
		}

		return 0;
	}
	
	int OutputASM::temporaryRegister(TIntermTyped *temporary)
	{
		return allocate(temporaries, temporary);
	}

	int OutputASM::varyingRegister(TIntermTyped *varying)
	{
		int var = lookup(varyings, varying);

		if(var == -1)
		{
			var = allocate(varyings, varying);
			int componentCount = varying->getNominalSize();
			int registerCount = varying->totalRegisterCount();

			if(pixelShader)
			{
				if((var + registerCount) > sw::PixelShader::MAX_INPUT_VARYINGS)
				{
					mContext.error(varying->getLine(), "Varyings packing failed: Too many varyings", "fragment shader");
					return 0;
				}

				if(varying->getQualifier() == EvqPointCoord)
				{
					ASSERT(varying->isRegister());
					if(componentCount >= 1) pixelShader->semantic[var][0] = sw::Shader::Semantic(sw::Shader::USAGE_TEXCOORD, var);
					if(componentCount >= 2) pixelShader->semantic[var][1] = sw::Shader::Semantic(sw::Shader::USAGE_TEXCOORD, var);
					if(componentCount >= 3) pixelShader->semantic[var][2] = sw::Shader::Semantic(sw::Shader::USAGE_TEXCOORD, var);
					if(componentCount >= 4) pixelShader->semantic[var][3] = sw::Shader::Semantic(sw::Shader::USAGE_TEXCOORD, var);
				}
				else
				{
					for(int i = 0; i < varying->totalRegisterCount(); i++)
					{
						if(componentCount >= 1) pixelShader->semantic[var + i][0] = sw::Shader::Semantic(sw::Shader::USAGE_COLOR, var + i);
						if(componentCount >= 2) pixelShader->semantic[var + i][1] = sw::Shader::Semantic(sw::Shader::USAGE_COLOR, var + i);
						if(componentCount >= 3) pixelShader->semantic[var + i][2] = sw::Shader::Semantic(sw::Shader::USAGE_COLOR, var + i);
						if(componentCount >= 4) pixelShader->semantic[var + i][3] = sw::Shader::Semantic(sw::Shader::USAGE_COLOR, var + i);
					}
				}
			}
			else if(vertexShader)
			{
				if((var + registerCount) > sw::VertexShader::MAX_OUTPUT_VARYINGS)
				{
					mContext.error(varying->getLine(), "Varyings packing failed: Too many varyings", "vertex shader");
					return 0;
				}

				if(varying->getQualifier() == EvqPosition)
				{
					ASSERT(varying->isRegister());
					vertexShader->output[var][0] = sw::Shader::Semantic(sw::Shader::USAGE_POSITION, 0);
					vertexShader->output[var][1] = sw::Shader::Semantic(sw::Shader::USAGE_POSITION, 0);
					vertexShader->output[var][2] = sw::Shader::Semantic(sw::Shader::USAGE_POSITION, 0);
					vertexShader->output[var][3] = sw::Shader::Semantic(sw::Shader::USAGE_POSITION, 0);
					vertexShader->positionRegister = var;
				}
				else if(varying->getQualifier() == EvqPointSize)
				{
					ASSERT(varying->isRegister());
					vertexShader->output[var][0] = sw::Shader::Semantic(sw::Shader::USAGE_PSIZE, 0);
					vertexShader->output[var][1] = sw::Shader::Semantic(sw::Shader::USAGE_PSIZE, 0);
					vertexShader->output[var][2] = sw::Shader::Semantic(sw::Shader::USAGE_PSIZE, 0);
					vertexShader->output[var][3] = sw::Shader::Semantic(sw::Shader::USAGE_PSIZE, 0);
					vertexShader->pointSizeRegister = var;
				}
				else
				{
					// Semantic indexes for user varyings will be assigned during program link to match the pixel shader
				}
			}
			else UNREACHABLE();

			declareVarying(varying, var);
		}

		return var;
	}

	void OutputASM::declareVarying(TIntermTyped *varying, int reg)
	{
		if(varying->getQualifier() != EvqPointCoord)   // gl_PointCoord does not need linking
		{
			const TType &type = varying->getType();
			const char *name = varying->getAsSymbolNode()->getSymbol().c_str();
			es2::VaryingList &activeVaryings = shaderObject->varyings;
			
			// Check if this varying has been declared before without having a register assigned
			for(es2::VaryingList::iterator v = activeVaryings.begin(); v != activeVaryings.end(); v++)
			{
				if(v->name == name)
				{
					if(reg >= 0)
					{
						ASSERT(v->reg < 0 || v->reg == reg);
						v->reg = reg;
					}

					return;
				}
			}
			
			activeVaryings.push_back(es2::Varying(glVariableType(type), name, varying->getArraySize(), reg, 0));
		}
	}

	int OutputASM::uniformRegister(TIntermTyped *uniform)
	{
		const TType &type = uniform->getType();
		ASSERT(!IsSampler(type.getBasicType()));
		TIntermSymbol *symbol = uniform->getAsSymbolNode();
		ASSERT(symbol);

		if(symbol)
		{
			int index = lookup(uniforms, uniform);

			if(index == -1)
			{
				index = allocate(uniforms, uniform);
				const TString &name = symbol->getSymbol().c_str();

				declareUniform(type, name, index);
			}

			return index;
		}

		return 0;
	}

	int OutputASM::attributeRegister(TIntermTyped *attribute)
	{
		ASSERT(!attribute->isArray());
		ASSERT(attribute->getBasicType() == EbtFloat);

		int index = lookup(attributes, attribute);

		if(index == -1)
		{
			TIntermSymbol *symbol = attribute->getAsSymbolNode();
			ASSERT(symbol);

			if(symbol)
			{
				index = allocate(attributes, attribute);
				const TType &type = attribute->getType();
				int registerCount = attribute->totalRegisterCount();

				if(vertexShader && (index + registerCount) <= sw::VertexShader::MAX_INPUT_ATTRIBUTES)
				{
					for(int i = 0; i < registerCount; i++)
					{
						vertexShader->input[index + i] = sw::Shader::Semantic(sw::Shader::USAGE_TEXCOORD, index + i);
					}
				}

				ActiveAttributes &activeAttributes = shaderObject->activeAttributes;

				const char *name = symbol->getSymbol().c_str();
				activeAttributes.push_back(Attribute(glVariableType(type), name, 0, index));
			}
		}

		return index;
	}

	int OutputASM::samplerRegister(TIntermTyped *sampler)
	{
		const TType &type = sampler->getType();
		ASSERT(IsSampler(type.getBasicType()));
		TIntermSymbol *symbol = sampler->getAsSymbolNode();
		TIntermBinary *binary = sampler->getAsBinaryNode();

		if(symbol)
		{
			int index = lookup(samplers, sampler);

			if(index == -1)
			{
				index = allocate(samplers, sampler);
				ActiveUniforms &activeUniforms = shaderObject->activeUniforms;
				const char *name = symbol->getSymbol().c_str();
				activeUniforms.push_back(Uniform(glVariableType(type), glVariablePrecision(type), name, sampler->getArraySize(), index));

				for(int i = 0; i < sampler->totalRegisterCount(); i++)
				{
					shader->declareSampler(index + i);
				}
			}

			return index;
		}
		else if(binary)
		{
			ASSERT(binary->getOp() == EOpIndexDirect || binary->getOp() == EOpIndexIndirect);

			return samplerRegister(binary->getLeft());   // Index added later
		}
		else UNREACHABLE();

		return 0;
	}

	int OutputASM::lookup(VariableArray &list, TIntermTyped *variable)
	{
		for(unsigned int i = 0; i < list.size(); i++)
		{
			if(list[i] == variable)
			{
				return i;   // Pointer match
			}
		}

		TIntermSymbol *varSymbol = variable->getAsSymbolNode();

		if(varSymbol)
		{
			for(unsigned int i = 0; i < list.size(); i++)
			{
				if(list[i])
				{
					TIntermSymbol *listSymbol = list[i]->getAsSymbolNode();

					if(listSymbol)
					{
						if(listSymbol->getId() == varSymbol->getId())
						{
							ASSERT(listSymbol->getSymbol() == varSymbol->getSymbol());
							ASSERT(listSymbol->getType() == varSymbol->getType());
							ASSERT(listSymbol->getQualifier() == varSymbol->getQualifier());

							return i;
						}
					}
				}
			}
		}

		return -1;
	}

	int OutputASM::allocate(VariableArray &list, TIntermTyped *variable)
	{
		int index = lookup(list, variable);

		if(index == -1)
		{
			unsigned int registerCount = variable->totalRegisterCount();

			for(unsigned int i = 0; i < list.size(); i++)
			{
				if(list[i] == 0)
				{
					unsigned int j = 1;
					for( ; j < registerCount && (i + j) < list.size(); j++)
					{
						if(list[i + j] != 0)
						{
							break;
						}
					}

					if(j == registerCount)   // Found free slots
					{
						for(unsigned int j = 0; j < registerCount; j++)
						{
							list[i + j] = variable;
						}

						return i;
					}
				}
			}

			index = list.size();

			for(unsigned int i = 0; i < registerCount; i++)
			{
				list.push_back(variable);
			}
		}

		return index;
	}

	void OutputASM::free(VariableArray &list, TIntermTyped *variable)
	{
		int index = lookup(list, variable);

		if(index >= 0)
		{
			list[index] = 0;
		}
	}

	void OutputASM::declareUniform(const TType &type, const TString &name, int index)
	{
		const TTypeList *structure = type.getStruct();
		ActiveUniforms &activeUniforms = shaderObject->activeUniforms;

		if(!structure)
		{
			activeUniforms.push_back(Uniform(glVariableType(type), glVariablePrecision(type), name.c_str(), type.getArraySize(), index));
		}
		else
		{
			if(type.isArray())
			{
				int elementIndex = index;

				for(int i = 0; i < type.getArraySize(); i++)
				{
					for(size_t j = 0; j < structure->size(); j++)
					{
						const TType &fieldType = *(*structure)[j].type;
						const TString &fieldName = fieldType.getFieldName();

						const TString uniformName = name + "[" + str(i) + "]." + fieldName;
						declareUniform(fieldType, uniformName, elementIndex);
						elementIndex += fieldType.totalRegisterCount();
					}
				}
			}
			else
			{
				int fieldIndex = index;

				for(size_t i = 0; i < structure->size(); i++)
				{
					const TType &fieldType = *(*structure)[i].type;
					const TString &fieldName = fieldType.getFieldName();

					const TString uniformName = name + "." + fieldName;
					declareUniform(fieldType, uniformName, fieldIndex);
					fieldIndex += fieldType.totalRegisterCount();
				}
			}
		}
	}

	GLenum OutputASM::glVariableType(const TType &type)
	{
		if(type.getBasicType() == EbtFloat)
		{
			if(type.isScalar())
			{
				return GL_FLOAT;
			}
			else if(type.isVector())
			{
				switch(type.getNominalSize())
				{
				case 2: return GL_FLOAT_VEC2;
				case 3: return GL_FLOAT_VEC3;
				case 4: return GL_FLOAT_VEC4;
				default: UNREACHABLE();
				}
			}
			else if(type.isMatrix())
			{
				switch(type.getNominalSize())
				{
				case 2: return GL_FLOAT_MAT2;
				case 3: return GL_FLOAT_MAT3;
				case 4: return GL_FLOAT_MAT4;
				default: UNREACHABLE();
				}
			}
			else UNREACHABLE();
		}
		else if(type.getBasicType() == EbtInt)
		{
			if(type.isScalar())
			{
				return GL_INT;
			}
			else if(type.isVector())
			{
				switch(type.getNominalSize())
				{
				case 2: return GL_INT_VEC2;
				case 3: return GL_INT_VEC3;
				case 4: return GL_INT_VEC4;
				default: UNREACHABLE();
				}
			}
			else UNREACHABLE();
		}
		else if(type.getBasicType() == EbtBool)
		{
			if(type.isScalar())
			{
				return GL_BOOL;
			}
			else if(type.isVector())
			{
				switch(type.getNominalSize())
				{
				case 2: return GL_BOOL_VEC2;
				case 3: return GL_BOOL_VEC3;
				case 4: return GL_BOOL_VEC4;
				default: UNREACHABLE();
				}
			}
			else UNREACHABLE();
		}
		else if(type.getBasicType() == EbtSampler2D)
		{
			return GL_SAMPLER_2D;
		}
		else if(type.getBasicType() == EbtSamplerCube)
		{
			return GL_SAMPLER_CUBE;
		}
		else if(type.getBasicType() == EbtSamplerExternalOES)
		{
			return GL_SAMPLER_EXTERNAL_OES;
		}
		else UNREACHABLE();

		return GL_NONE;
	}

	GLenum OutputASM::glVariablePrecision(const TType &type)
	{
		if(type.getBasicType() == EbtFloat)
		{
			switch(type.getPrecision())
			{
			case EbpHigh:   return GL_HIGH_FLOAT;
			case EbpMedium: return GL_MEDIUM_FLOAT;
			case EbpLow:    return GL_LOW_FLOAT;
			case EbpUndefined:
				// Should be defined as the default precision by the parser
			default: UNREACHABLE();
			}
		}
		else if (type.getBasicType() == EbtInt)
		{
			switch (type.getPrecision())
			{
			case EbpHigh:   return GL_HIGH_INT;
			case EbpMedium: return GL_MEDIUM_INT;
			case EbpLow:    return GL_LOW_INT;
			case EbpUndefined:
				// Should be defined as the default precision by the parser
			default: UNREACHABLE();
			}
		}

		// Other types (boolean, sampler) don't have a precision
		return GL_NONE;
	}

	int OutputASM::dim(TIntermNode *v)
	{
		TIntermTyped *vector = v->getAsTyped();
		ASSERT(vector && vector->isRegister());
		return vector->getNominalSize();
	}

	int OutputASM::dim2(TIntermNode *m)
	{
		TIntermTyped *matrix = m->getAsTyped();
		ASSERT(matrix && matrix->isMatrix() && !matrix->isArray());
		return matrix->getNominalSize();
	}

	// Returns ~0 if no loop count could be determined
	unsigned int OutputASM::loopCount(TIntermLoop *node)
	{
		// Parse loops of the form:
		// for(int index = initial; index [comparator] limit; index += increment)
		TIntermSymbol *index = 0;
		TOperator comparator = EOpNull;
		int initial = 0;
		int limit = 0;
		int increment = 0;

		// Parse index name and intial value
		if(node->getInit())
		{
			TIntermAggregate *init = node->getInit()->getAsAggregate();

			if(init)
			{
				TIntermSequence &sequence = init->getSequence();
				TIntermTyped *variable = sequence[0]->getAsTyped();

				if(variable && variable->getQualifier() == EvqTemporary)
				{
					TIntermBinary *assign = variable->getAsBinaryNode();

					if(assign->getOp() == EOpInitialize)
					{
						TIntermSymbol *symbol = assign->getLeft()->getAsSymbolNode();
						TIntermConstantUnion *constant = assign->getRight()->getAsConstantUnion();

						if(symbol && constant)
						{
							if(constant->getBasicType() == EbtInt && constant->getNominalSize() == 1)
							{
								index = symbol;
								initial = constant->getUnionArrayPointer()[0].getIConst();
							}
						}
					}
				}
			}
		}

		// Parse comparator and limit value
		if(index && node->getCondition())
		{
			TIntermBinary *test = node->getCondition()->getAsBinaryNode();

			if(test && test->getLeft()->getAsSymbolNode()->getId() == index->getId())
			{
				TIntermConstantUnion *constant = test->getRight()->getAsConstantUnion();

				if(constant)
				{
					if(constant->getBasicType() == EbtInt && constant->getNominalSize() == 1)
					{
						comparator = test->getOp();
						limit = constant->getUnionArrayPointer()[0].getIConst();
					}
				}
			}
		}

		// Parse increment
		if(index && comparator != EOpNull && node->getExpression())
		{
			TIntermBinary *binaryTerminal = node->getExpression()->getAsBinaryNode();
			TIntermUnary *unaryTerminal = node->getExpression()->getAsUnaryNode();

			if(binaryTerminal)
			{
				TOperator op = binaryTerminal->getOp();
				TIntermConstantUnion *constant = binaryTerminal->getRight()->getAsConstantUnion();

				if(constant)
				{
					if(constant->getBasicType() == EbtInt && constant->getNominalSize() == 1)
					{
						int value = constant->getUnionArrayPointer()[0].getIConst();

						switch(op)
						{
						case EOpAddAssign: increment = value;  break;
						case EOpSubAssign: increment = -value; break;
						default: UNIMPLEMENTED();
						}
					}
				}
			}
			else if(unaryTerminal)
			{
				TOperator op = unaryTerminal->getOp();

				switch(op)
				{
				case EOpPostIncrement: increment = 1;  break;
				case EOpPostDecrement: increment = -1; break;
				case EOpPreIncrement:  increment = 1;  break;
				case EOpPreDecrement:  increment = -1; break;
				default: UNIMPLEMENTED();
				}
			}
		}

		if(index && comparator != EOpNull && increment != 0)
		{
			if(comparator == EOpLessThanEqual)
			{
				comparator = EOpLessThan;
				limit += 1;
			}

			if(comparator == EOpLessThan)
			{
				int iterations = (limit - initial) / increment;

				if(iterations <= 0)
				{
					iterations = 0;
				}

				return iterations;
			}
			else UNIMPLEMENTED();   // Falls through
		}

		return ~0;
	}

	bool DetectLoopDiscontinuity::traverse(TIntermNode *node)
	{
		loopDepth = 0;
		loopDiscontinuity = false;
		
		node->traverse(this);
		
		return loopDiscontinuity;
	}

	bool DetectLoopDiscontinuity::visitLoop(Visit visit, TIntermLoop *loop)
	{
		if(visit == PreVisit)
		{
			loopDepth++;
		}
		else if(visit == PostVisit)
		{
			loopDepth++;
		}

		return true;
	}

	bool DetectLoopDiscontinuity::visitBranch(Visit visit, TIntermBranch *node)
	{
		if(loopDiscontinuity)
		{
			return false;
		}

		if(!loopDepth)
		{
			return true;
		}
	
		switch(node->getFlowOp())
		{
		case EOpKill:
			break;
		case EOpBreak:
		case EOpContinue:
		case EOpReturn:
			loopDiscontinuity = true;
			break;
		default: UNREACHABLE();
		}

		return !loopDiscontinuity;
	}

	bool DetectLoopDiscontinuity::visitAggregate(Visit visit, TIntermAggregate *node)
	{
		return !loopDiscontinuity;
	}
}
