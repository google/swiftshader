//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ParseHelper.h"

#include <stdarg.h>
#include <stdio.h>

#include "glslang.h"
#include "preprocessor/SourceLocation.h"
#include "ValidateGlobalInitializer.h"
#include "ValidateSwitch.h"

///////////////////////////////////////////////////////////////////////
//
// Sub- vector and matrix fields
//
////////////////////////////////////////////////////////////////////////

//
// Look at a '.' field selector string and change it into offsets
// for a vector.
//
bool TParseContext::parseVectorFields(const TString& compString, int vecSize, TVectorFields& fields, const TSourceLoc &line)
{
    fields.num = (int) compString.size();
    if (fields.num > 4) {
        error(line, "illegal vector field selection", compString.c_str());
        return false;
    }

    enum {
        exyzw,
        ergba,
        estpq
    } fieldSet[4];

    for (int i = 0; i < fields.num; ++i) {
        switch (compString[i])  {
        case 'x':
            fields.offsets[i] = 0;
            fieldSet[i] = exyzw;
            break;
        case 'r':
            fields.offsets[i] = 0;
            fieldSet[i] = ergba;
            break;
        case 's':
            fields.offsets[i] = 0;
            fieldSet[i] = estpq;
            break;
        case 'y':
            fields.offsets[i] = 1;
            fieldSet[i] = exyzw;
            break;
        case 'g':
            fields.offsets[i] = 1;
            fieldSet[i] = ergba;
            break;
        case 't':
            fields.offsets[i] = 1;
            fieldSet[i] = estpq;
            break;
        case 'z':
            fields.offsets[i] = 2;
            fieldSet[i] = exyzw;
            break;
        case 'b':
            fields.offsets[i] = 2;
            fieldSet[i] = ergba;
            break;
        case 'p':
            fields.offsets[i] = 2;
            fieldSet[i] = estpq;
            break;
        case 'w':
            fields.offsets[i] = 3;
            fieldSet[i] = exyzw;
            break;
        case 'a':
            fields.offsets[i] = 3;
            fieldSet[i] = ergba;
            break;
        case 'q':
            fields.offsets[i] = 3;
            fieldSet[i] = estpq;
            break;
        default:
            error(line, "illegal vector field selection", compString.c_str());
            return false;
        }
    }

    for (int i = 0; i < fields.num; ++i) {
        if (fields.offsets[i] >= vecSize) {
            error(line, "vector field selection out of range",  compString.c_str());
            return false;
        }

        if (i > 0) {
            if (fieldSet[i] != fieldSet[i-1]) {
                error(line, "illegal - vector component fields not from the same set", compString.c_str());
                return false;
            }
        }
    }

    return true;
}


//
// Look at a '.' field selector string and change it into offsets
// for a matrix.
//
bool TParseContext::parseMatrixFields(const TString& compString, int matCols, int matRows, TMatrixFields& fields, const TSourceLoc &line)
{
    fields.wholeRow = false;
    fields.wholeCol = false;
    fields.row = -1;
    fields.col = -1;

    if (compString.size() != 2) {
        error(line, "illegal length of matrix field selection", compString.c_str());
        return false;
    }

    if (compString[0] == '_') {
        if (compString[1] < '0' || compString[1] > '3') {
            error(line, "illegal matrix field selection", compString.c_str());
            return false;
        }
        fields.wholeCol = true;
        fields.col = compString[1] - '0';
    } else if (compString[1] == '_') {
        if (compString[0] < '0' || compString[0] > '3') {
            error(line, "illegal matrix field selection", compString.c_str());
            return false;
        }
        fields.wholeRow = true;
        fields.row = compString[0] - '0';
    } else {
        if (compString[0] < '0' || compString[0] > '3' ||
            compString[1] < '0' || compString[1] > '3') {
            error(line, "illegal matrix field selection", compString.c_str());
            return false;
        }
        fields.row = compString[0] - '0';
        fields.col = compString[1] - '0';
    }

    if (fields.row >= matRows || fields.col >= matCols) {
        error(line, "matrix field selection out of range", compString.c_str());
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////
//
// Errors
//
////////////////////////////////////////////////////////////////////////

//
// Track whether errors have occurred.
//
void TParseContext::recover()
{
}

//
// Used by flex/bison to output all syntax and parsing errors.
//
void TParseContext::error(const TSourceLoc& loc,
                          const char* reason, const char* token,
                          const char* extraInfo)
{
    pp::SourceLocation srcLoc(loc.first_file, loc.first_line);
    mDiagnostics.writeInfo(pp::Diagnostics::PP_ERROR,
                           srcLoc, reason, token, extraInfo);

}

void TParseContext::warning(const TSourceLoc& loc,
                            const char* reason, const char* token,
                            const char* extraInfo) {
    pp::SourceLocation srcLoc(loc.first_file, loc.first_line);
    mDiagnostics.writeInfo(pp::Diagnostics::PP_WARNING,
                           srcLoc, reason, token, extraInfo);
}

void TParseContext::trace(const char* str)
{
    mDiagnostics.writeDebug(str);
}

//
// Same error message for all places assignments don't work.
//
void TParseContext::assignError(const TSourceLoc &line, const char* op, TString left, TString right)
{
    std::stringstream extraInfoStream;
    extraInfoStream << "cannot convert from '" << right << "' to '" << left << "'";
    std::string extraInfo = extraInfoStream.str();
    error(line, "", op, extraInfo.c_str());
}

//
// Same error message for all places unary operations don't work.
//
void TParseContext::unaryOpError(const TSourceLoc &line, const char* op, TString operand)
{
    std::stringstream extraInfoStream;
    extraInfoStream << "no operation '" << op << "' exists that takes an operand of type " << operand
                    << " (or there is no acceptable conversion)";
    std::string extraInfo = extraInfoStream.str();
    error(line, " wrong operand type", op, extraInfo.c_str());
}

//
// Same error message for all binary operations don't work.
//
void TParseContext::binaryOpError(const TSourceLoc &line, const char* op, TString left, TString right)
{
    std::stringstream extraInfoStream;
    extraInfoStream << "no operation '" << op << "' exists that takes a left-hand operand of type '" << left
                    << "' and a right operand of type '" << right << "' (or there is no acceptable conversion)";
    std::string extraInfo = extraInfoStream.str();
    error(line, " wrong operand types ", op, extraInfo.c_str());
}

bool TParseContext::precisionErrorCheck(const TSourceLoc &line, TPrecision precision, TBasicType type){
    if (!mChecksPrecisionErrors)
        return false;
    switch( type ){
    case EbtFloat:
        if( precision == EbpUndefined ){
            error( line, "No precision specified for (float)", "" );
            return true;
        }
        break;
    case EbtInt:
        if( precision == EbpUndefined ){
            error( line, "No precision specified (int)", "" );
            return true;
        }
        break;
    default:
        return false;
    }
    return false;
}

//
// Both test and if necessary, spit out an error, to see if the node is really
// an l-value that can be operated on this way.
//
// Returns true if the was an error.
//
bool TParseContext::lValueErrorCheck(const TSourceLoc &line, const char* op, TIntermTyped* node)
{
    TIntermSymbol* symNode = node->getAsSymbolNode();
    TIntermBinary* binaryNode = node->getAsBinaryNode();

    if (binaryNode) {
        bool errorReturn;

        switch(binaryNode->getOp()) {
        case EOpIndexDirect:
        case EOpIndexIndirect:
        case EOpIndexDirectStruct:
            return lValueErrorCheck(line, op, binaryNode->getLeft());
        case EOpVectorSwizzle:
            errorReturn = lValueErrorCheck(line, op, binaryNode->getLeft());
            if (!errorReturn) {
                int offset[4] = {0,0,0,0};

                TIntermTyped* rightNode = binaryNode->getRight();
                TIntermAggregate *aggrNode = rightNode->getAsAggregate();

                for (TIntermSequence::iterator p = aggrNode->getSequence().begin();
                                               p != aggrNode->getSequence().end(); p++) {
                    int value = (*p)->getAsTyped()->getAsConstantUnion()->getIConst(0);
                    offset[value]++;
                    if (offset[value] > 1) {
                        error(line, " l-value of swizzle cannot have duplicate components", op);

                        return true;
                    }
                }
            }

            return errorReturn;
        default:
            break;
        }
        error(line, " l-value required", op);

        return true;
    }


    const char* symbol = 0;
    if (symNode != 0)
        symbol = symNode->getSymbol().c_str();

    const char* message = 0;
    switch (node->getQualifier()) {
    case EvqConstExpr:      message = "can't modify a const";        break;
    case EvqConstReadOnly:  message = "can't modify a const";        break;
    case EvqAttribute:      message = "can't modify an attribute";   break;
    case EvqFragmentIn:     message = "can't modify an input";       break;
    case EvqVertexIn:       message = "can't modify an input";       break;
    case EvqUniform:        message = "can't modify a uniform";      break;
    case EvqSmoothIn:
    case EvqFlatIn:
    case EvqCentroidIn:
    case EvqVaryingIn:      message = "can't modify a varying";      break;
    case EvqInput:          message = "can't modify an input";       break;
    case EvqFragCoord:      message = "can't modify gl_FragCoord";   break;
    case EvqFrontFacing:    message = "can't modify gl_FrontFacing"; break;
    case EvqPointCoord:     message = "can't modify gl_PointCoord";  break;
    case EvqInstanceID:     message = "can't modify gl_InstanceID";  break;
    default:

        //
        // Type that can't be written to?
        //
        if(IsSampler(node->getBasicType()))
        {
            message = "can't modify a sampler";
        }
        else if(node->getBasicType() == EbtVoid)
        {
            message = "can't modify void";
        }
    }

    if (message == 0 && binaryNode == 0 && symNode == 0) {
        error(line, " l-value required", op);

        return true;
    }


    //
    // Everything else is okay, no error.
    //
    if (message == 0)
        return false;

    //
    // If we get here, we have an error and a message.
    //
    if (symNode) {
        std::stringstream extraInfoStream;
        extraInfoStream << "\"" << symbol << "\" (" << message << ")";
        std::string extraInfo = extraInfoStream.str();
        error(line, " l-value required", op, extraInfo.c_str());
    }
    else {
        std::stringstream extraInfoStream;
        extraInfoStream << "(" << message << ")";
        std::string extraInfo = extraInfoStream.str();
        error(line, " l-value required", op, extraInfo.c_str());
    }

    return true;
}

//
// Both test, and if necessary spit out an error, to see if the node is really
// a constant.
//
// Returns true if the was an error.
//
bool TParseContext::constErrorCheck(TIntermTyped* node)
{
    if (node->getQualifier() == EvqConstExpr)
        return false;

    error(node->getLine(), "constant expression required", "");

    return true;
}

//
// Both test, and if necessary spit out an error, to see if the node is really
// an integer.
//
// Returns true if the was an error.
//
bool TParseContext::integerErrorCheck(TIntermTyped* node, const char* token)
{
    if (node->isScalarInt())
        return false;

    error(node->getLine(), "integer expression required", token);

    return true;
}

//
// Both test, and if necessary spit out an error, to see if we are currently
// globally scoped.
//
// Returns true if the was an error.
//
bool TParseContext::globalErrorCheck(const TSourceLoc &line, bool global, const char* token)
{
    if (global)
        return false;

    error(line, "only allowed at global scope", token);

    return true;
}

//
// For now, keep it simple:  if it starts "gl_", it's reserved, independent
// of scope.  Except, if the symbol table is at the built-in push-level,
// which is when we are parsing built-ins.
// Also checks for "webgl_" and "_webgl_" reserved identifiers if parsing a
// webgl shader.
//
// Returns true if there was an error.
//
bool TParseContext::reservedErrorCheck(const TSourceLoc &line, const TString& identifier)
{
    static const char* reservedErrMsg = "reserved built-in name";
    if (!symbolTable.atBuiltInLevel()) {
        if (identifier.compare(0, 3, "gl_") == 0) {
            error(line, reservedErrMsg, "gl_");
            return true;
        }
        if (identifier.find("__") != TString::npos) {
            error(line, "identifiers containing two consecutive underscores (__) are reserved as possible future keywords", identifier.c_str());
            return true;
        }
    }

    return false;
}

//
// Make sure there is enough data provided to the constructor to build
// something of the type of the constructor.  Also returns the type of
// the constructor.
//
// Returns true if there was an error in construction.
//
bool TParseContext::constructorErrorCheck(const TSourceLoc &line, TIntermNode* node, TFunction& function, TOperator op, TType* type)
{
    *type = function.getReturnType();

    bool constructingMatrix = false;
    switch(op) {
    case EOpConstructMat2:
    case EOpConstructMat2x3:
    case EOpConstructMat2x4:
    case EOpConstructMat3x2:
    case EOpConstructMat3:
    case EOpConstructMat3x4:
    case EOpConstructMat4x2:
    case EOpConstructMat4x3:
    case EOpConstructMat4:
        constructingMatrix = true;
        break;
    default:
        break;
    }

    //
    // Note: It's okay to have too many components available, but not okay to have unused
    // arguments.  'full' will go to true when enough args have been seen.  If we loop
    // again, there is an extra argument, so 'overfull' will become true.
    //

    int size = 0;
    bool full = false;
    bool overFull = false;
    bool matrixInMatrix = false;
    bool arrayArg = false;
    for (size_t i = 0; i < function.getParamCount(); ++i) {
        const TParameter& param = function.getParam(i);
        size += param.type->getObjectSize();

        if (constructingMatrix && param.type->isMatrix())
            matrixInMatrix = true;
        if (full)
            overFull = true;
        if (op != EOpConstructStruct && !type->isArray() && size >= type->getObjectSize())
            full = true;
        if (param.type->isArray())
            arrayArg = true;
    }

    if(type->isArray()) {
        if(type->getArraySize() == 0) {
            type->setArraySize(function.getParamCount());
        } else if(type->getArraySize() != (int)function.getParamCount()) {
            error(line, "array constructor needs one argument per array element", "constructor");
            return true;
        }
    }

    if (arrayArg && op != EOpConstructStruct) {
        error(line, "constructing from a non-dereferenced array", "constructor");
        return true;
    }

    if (matrixInMatrix && !type->isArray()) {
        if (function.getParamCount() != 1) {
          error(line, "constructing matrix from matrix can only take one argument", "constructor");
          return true;
        }
    }

    if (overFull) {
        error(line, "too many arguments", "constructor");
        return true;
    }

    if (op == EOpConstructStruct && !type->isArray() && type->getStruct()->fields().size() != function.getParamCount()) {
        error(line, "Number of constructor parameters does not match the number of structure fields", "constructor");
        return true;
    }

    if (!type->isMatrix() || !matrixInMatrix) {
        if ((op != EOpConstructStruct && size != 1 && size < type->getObjectSize()) ||
            (op == EOpConstructStruct && size < type->getObjectSize())) {
            error(line, "not enough data provided for construction", "constructor");
            return true;
        }
    }

    TIntermTyped *typed = node ? node->getAsTyped() : 0;
    if (typed == 0) {
        error(line, "constructor argument does not have a type", "constructor");
        return true;
    }
    if (op != EOpConstructStruct && IsSampler(typed->getBasicType())) {
        error(line, "cannot convert a sampler", "constructor");
        return true;
    }
    if (typed->getBasicType() == EbtVoid) {
        error(line, "cannot convert a void", "constructor");
        return true;
    }

    return false;
}

// This function checks to see if a void variable has been declared and raise an error message for such a case
//
// returns true in case of an error
//
bool TParseContext::voidErrorCheck(const TSourceLoc &line, const TString& identifier, const TBasicType& type)
{
    if(type == EbtVoid) {
        error(line, "illegal use of type 'void'", identifier.c_str());
        return true;
    }

    return false;
}

// This function checks to see if the node (for the expression) contains a scalar boolean expression or not
//
// returns true in case of an error
//
bool TParseContext::boolErrorCheck(const TSourceLoc &line, const TIntermTyped* type)
{
    if (type->getBasicType() != EbtBool || type->isArray() || type->isMatrix() || type->isVector()) {
        error(line, "boolean expression expected", "");
        return true;
    }

    return false;
}

// This function checks to see if the node (for the expression) contains a scalar boolean expression or not
//
// returns true in case of an error
//
bool TParseContext::boolErrorCheck(const TSourceLoc &line, const TPublicType& pType)
{
    if (pType.type != EbtBool || pType.array || (pType.primarySize > 1) || (pType.secondarySize > 1)) {
        error(line, "boolean expression expected", "");
        return true;
    }

    return false;
}

bool TParseContext::samplerErrorCheck(const TSourceLoc &line, const TPublicType& pType, const char* reason)
{
    if (pType.type == EbtStruct) {
        if (containsSampler(*pType.userDef)) {
            error(line, reason, getBasicString(pType.type), "(structure contains a sampler)");

            return true;
        }

        return false;
    } else if (IsSampler(pType.type)) {
        error(line, reason, getBasicString(pType.type));

        return true;
    }

    return false;
}

bool TParseContext::structQualifierErrorCheck(const TSourceLoc &line, const TPublicType& pType)
{
	switch(pType.qualifier)
	{
	case EvqVaryingOut:
	case EvqSmooth:
	case EvqFlat:
	case EvqCentroidOut:
	case EvqVaryingIn:
	case EvqSmoothIn:
	case EvqFlatIn:
	case EvqCentroidIn:
	case EvqAttribute:
	case EvqVertexIn:
	case EvqFragmentOut:
		if(pType.type == EbtStruct)
		{
			error(line, "cannot be used with a structure", getQualifierString(pType.qualifier));

			return true;
		}
		break;
	default:
		break;
	}

    if (pType.qualifier != EvqUniform && samplerErrorCheck(line, pType, "samplers must be uniform"))
        return true;

	// check for layout qualifier issues
	const TLayoutQualifier layoutQualifier = pType.layoutQualifier;

	if (pType.qualifier != EvqVertexIn && pType.qualifier != EvqFragmentOut &&
	    layoutLocationErrorCheck(line, pType.layoutQualifier))
	{
		return true;
	}

    return false;
}

// These checks are common for all declarations starting a declarator list, and declarators that follow an empty
// declaration.
//
bool TParseContext::singleDeclarationErrorCheck(const TPublicType &publicType, const TSourceLoc &identifierLocation)
{
	switch(publicType.qualifier)
	{
	case EvqVaryingIn:
	case EvqVaryingOut:
	case EvqAttribute:
	case EvqVertexIn:
	case EvqFragmentOut:
		if(publicType.type == EbtStruct)
		{
			error(identifierLocation, "cannot be used with a structure",
				getQualifierString(publicType.qualifier));
			return true;
		}

	default: break;
	}

	if(publicType.qualifier != EvqUniform && samplerErrorCheck(identifierLocation, publicType,
		"samplers must be uniform"))
	{
		return true;
	}

	// check for layout qualifier issues
	const TLayoutQualifier layoutQualifier = publicType.layoutQualifier;

	if(layoutQualifier.matrixPacking != EmpUnspecified)
	{
		error(identifierLocation, "layout qualifier", getMatrixPackingString(layoutQualifier.matrixPacking),
			"only valid for interface blocks");
		return true;
	}

	if(layoutQualifier.blockStorage != EbsUnspecified)
	{
		error(identifierLocation, "layout qualifier", getBlockStorageString(layoutQualifier.blockStorage),
			"only valid for interface blocks");
		return true;
	}

	if(publicType.qualifier != EvqVertexIn && publicType.qualifier != EvqFragmentOut &&
		layoutLocationErrorCheck(identifierLocation, publicType.layoutQualifier))
	{
		return true;
	}

	return false;
}

bool TParseContext::layoutLocationErrorCheck(const TSourceLoc &location, const TLayoutQualifier &layoutQualifier)
{
	if(layoutQualifier.location != -1)
	{
		error(location, "invalid layout qualifier:", "location", "only valid on program inputs and outputs");
		return true;
	}

	return false;
}

bool TParseContext::locationDeclaratorListCheck(const TSourceLoc& line, const TPublicType &pType)
{
	if(pType.layoutQualifier.location != -1)
	{
		error(line, "location must only be specified for a single input or output variable", "location");
		return true;
	}

	return false;
}

bool TParseContext::parameterSamplerErrorCheck(const TSourceLoc &line, TQualifier qualifier, const TType& type)
{
    if ((qualifier == EvqOut || qualifier == EvqInOut) &&
             type.getBasicType() != EbtStruct && IsSampler(type.getBasicType())) {
        error(line, "samplers cannot be output parameters", type.getBasicString());
        return true;
    }

    return false;
}

bool TParseContext::containsSampler(TType& type)
{
    if (IsSampler(type.getBasicType()))
        return true;

    if (type.getBasicType() == EbtStruct) {
        const TFieldList& fields = type.getStruct()->fields();
        for(unsigned int i = 0; i < fields.size(); ++i) {
            if (containsSampler(*fields[i]->type()))
                return true;
        }
    }

    return false;
}

//
// Do size checking for an array type's size.
//
// Returns true if there was an error.
//
bool TParseContext::arraySizeErrorCheck(const TSourceLoc &line, TIntermTyped* expr, int& size)
{
    TIntermConstantUnion* constant = expr->getAsConstantUnion();

    if (constant == 0 || !constant->isScalarInt())
    {
        error(line, "array size must be a constant integer expression", "");
        return true;
    }

    if (constant->getBasicType() == EbtUInt)
    {
        unsigned int uintSize = constant->getUConst(0);
        if (uintSize > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
            error(line, "array size too large", "");
            size = 1;
            return true;
        }

        size = static_cast<int>(uintSize);
    }
    else
    {
        size = constant->getIConst(0);

        if (size <= 0)
        {
            error(line, "array size must be a positive integer", "");
            size = 1;
            return true;
        }
    }

    return false;
}

//
// See if this qualifier can be an array.
//
// Returns true if there is an error.
//
bool TParseContext::arrayQualifierErrorCheck(const TSourceLoc &line, TPublicType type)
{
    if ((type.qualifier == EvqAttribute) || (type.qualifier == EvqVertexIn) || (type.qualifier == EvqConstExpr)) {
        error(line, "cannot declare arrays of this qualifier", TType(type).getCompleteString().c_str());
        return true;
    }

    return false;
}

//
// See if this type can be an array.
//
// Returns true if there is an error.
//
bool TParseContext::arrayTypeErrorCheck(const TSourceLoc &line, TPublicType type)
{
    //
    // Can the type be an array?
    //
    if (type.array) {
        error(line, "cannot declare arrays of arrays", TType(type).getCompleteString().c_str());
        return true;
    }

    return false;
}

bool TParseContext::arraySetMaxSize(TIntermSymbol *node, TType* type, int size, bool updateFlag, const TSourceLoc &line)
{
    bool builtIn = false;
    TSymbol* symbol = symbolTable.find(node->getSymbol(), mShaderVersion, &builtIn);
    if (symbol == 0) {
        error(line, " undeclared identifier", node->getSymbol().c_str());
        return true;
    }
    TVariable* variable = static_cast<TVariable*>(symbol);

    type->setArrayInformationType(variable->getArrayInformationType());
    variable->updateArrayInformationType(type);

    // special casing to test index value of gl_FragData. If the accessed index is >= gl_MaxDrawBuffers
    // its an error
    if (node->getSymbol() == "gl_FragData") {
        TSymbol* fragData = symbolTable.find("gl_MaxDrawBuffers", mShaderVersion, &builtIn);
        ASSERT(fragData);

        int fragDataValue = static_cast<TVariable*>(fragData)->getConstPointer()[0].getIConst();
        if (fragDataValue <= size) {
            error(line, "", "[", "gl_FragData can only have a max array size of up to gl_MaxDrawBuffers");
            return true;
        }
    }

    // we dont want to update the maxArraySize when this flag is not set, we just want to include this
    // node type in the chain of node types so that its updated when a higher maxArraySize comes in.
    if (!updateFlag)
        return false;

    size++;
    variable->getType().setMaxArraySize(size);
    type->setMaxArraySize(size);
    TType* tt = type;

    while(tt->getArrayInformationType() != 0) {
        tt = tt->getArrayInformationType();
        tt->setMaxArraySize(size);
    }

    return false;
}

//
// Enforce non-initializer type/qualifier rules.
//
// Returns true if there was an error.
//
bool TParseContext::nonInitConstErrorCheck(const TSourceLoc &line, TString& identifier, TPublicType& type, bool array)
{
    if (type.qualifier == EvqConstExpr)
    {
        // Make the qualifier make sense.
        type.qualifier = EvqTemporary;

        if (array)
        {
            error(line, "arrays may not be declared constant since they cannot be initialized", identifier.c_str());
        }
        else if (type.isStructureContainingArrays())
        {
            error(line, "structures containing arrays may not be declared constant since they cannot be initialized", identifier.c_str());
        }
        else
        {
            error(line, "variables with qualifier 'const' must be initialized", identifier.c_str());
        }

        return true;
    }

    return false;
}

//
// Do semantic checking for a variable declaration that has no initializer,
// and update the symbol table.
//
// Returns true if there was an error.
//
bool TParseContext::nonInitErrorCheck(const TSourceLoc &line, const TString& identifier, TPublicType& type)
{
	if(type.qualifier == EvqConstExpr)
	{
		// Make the qualifier make sense.
		type.qualifier = EvqTemporary;

		// Generate informative error messages for ESSL1.
		// In ESSL3 arrays and structures containing arrays can be constant.
		if(mShaderVersion < 300 && type.isStructureContainingArrays())
		{
			error(line,
				"structures containing arrays may not be declared constant since they cannot be initialized",
				identifier.c_str());
		}
		else
		{
			error(line, "variables with qualifier 'const' must be initialized", identifier.c_str());
		}

		return true;
	}
	if(type.isUnsizedArray())
	{
		error(line, "implicitly sized arrays need to be initialized", identifier.c_str());
		return true;
	}
	return false;
}

// Do some simple checks that are shared between all variable declarations,
// and update the symbol table.
//
// Returns true if declaring the variable succeeded.
//
bool TParseContext::declareVariable(const TSourceLoc &line, const TString &identifier, const TType &type,
	TVariable **variable)
{
	ASSERT((*variable) == nullptr);

	// gl_LastFragData may be redeclared with a new precision qualifier
	if(type.isArray() && identifier.compare(0, 15, "gl_LastFragData") == 0)
	{
		const TVariable *maxDrawBuffers =
			static_cast<const TVariable *>(symbolTable.findBuiltIn("gl_MaxDrawBuffers", mShaderVersion));
		if(type.getArraySize() != maxDrawBuffers->getConstPointer()->getIConst())
		{
			error(line, "redeclaration of gl_LastFragData with size != gl_MaxDrawBuffers", identifier.c_str());
			return false;
		}
	}

	if(reservedErrorCheck(line, identifier))
		return false;

	(*variable) = new TVariable(&identifier, type);
	if(!symbolTable.declare(**variable))
	{
		error(line, "redefinition", identifier.c_str());
		delete (*variable);
		(*variable) = nullptr;
		return false;
	}

	if(voidErrorCheck(line, identifier, type.getBasicType()))
		return false;

	return true;
}

bool TParseContext::paramErrorCheck(const TSourceLoc &line, TQualifier qualifier, TQualifier paramQualifier, TType* type)
{
    if (qualifier != EvqConstReadOnly && qualifier != EvqTemporary) {
        error(line, "qualifier not allowed on function parameter", getQualifierString(qualifier));
        return true;
    }
    if (qualifier == EvqConstReadOnly && paramQualifier != EvqIn) {
        error(line, "qualifier not allowed with ", getQualifierString(qualifier), getQualifierString(paramQualifier));
        return true;
    }

    if (qualifier == EvqConstReadOnly)
        type->setQualifier(EvqConstReadOnly);
    else
        type->setQualifier(paramQualifier);

    return false;
}

bool TParseContext::extensionErrorCheck(const TSourceLoc &line, const TString& extension)
{
    const TExtensionBehavior& extBehavior = extensionBehavior();
    TExtensionBehavior::const_iterator iter = extBehavior.find(extension.c_str());
    if (iter == extBehavior.end()) {
        error(line, "extension", extension.c_str(), "is not supported");
        return true;
    }
    // In GLSL ES, an extension's default behavior is "disable".
    if (iter->second == EBhDisable || iter->second == EBhUndefined) {
        error(line, "extension", extension.c_str(), "is disabled");
        return true;
    }
    if (iter->second == EBhWarn) {
        warning(line, "extension", extension.c_str(), "is being used");
        return false;
    }

    return false;
}

bool TParseContext::functionCallLValueErrorCheck(const TFunction *fnCandidate, TIntermAggregate *aggregate)
{
	for(size_t i = 0; i < fnCandidate->getParamCount(); ++i)
	{
		TQualifier qual = fnCandidate->getParam(i).type->getQualifier();
		if(qual == EvqOut || qual == EvqInOut)
		{
			TIntermTyped *node = (aggregate->getSequence())[i]->getAsTyped();
			if(lValueErrorCheck(node->getLine(), "assign", node))
			{
				error(node->getLine(),
					"Constant value cannot be passed for 'out' or 'inout' parameters.", "Error");
				recover();
				return true;
			}
		}
	}
	return false;
}

void TParseContext::es3InvariantErrorCheck(const TQualifier qualifier, const TSourceLoc &invariantLocation)
{
	switch(qualifier)
	{
	case EvqVaryingOut:
	case EvqSmoothOut:
	case EvqFlatOut:
	case EvqCentroidOut:
	case EvqVertexOut:
	case EvqFragmentOut:
		break;
	default:
		error(invariantLocation, "Only out variables can be invariant.", "invariant");
		recover();
		break;
	}
}

bool TParseContext::supportsExtension(const char* extension)
{
    const TExtensionBehavior& extbehavior = extensionBehavior();
    TExtensionBehavior::const_iterator iter = extbehavior.find(extension);
    return (iter != extbehavior.end());
}

void TParseContext::handleExtensionDirective(const TSourceLoc &line, const char* extName, const char* behavior)
{
    pp::SourceLocation loc(line.first_file, line.first_line);
    mDirectiveHandler.handleExtension(loc, extName, behavior);
}

void TParseContext::handlePragmaDirective(const TSourceLoc &line, const char* name, const char* value)
{
    pp::SourceLocation loc(line.first_file, line.first_line);
    mDirectiveHandler.handlePragma(loc, name, value);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Non-Errors.
//
/////////////////////////////////////////////////////////////////////////////////

const TVariable *TParseContext::getNamedVariable(const TSourceLoc &location,
	const TString *name,
	const TSymbol *symbol)
{
	const TVariable *variable = NULL;

	if(!symbol)
	{
		error(location, "undeclared identifier", name->c_str());
		recover();
	}
	else if(!symbol->isVariable())
	{
		error(location, "variable expected", name->c_str());
		recover();
	}
	else
	{
		variable = static_cast<const TVariable*>(symbol);

		if(symbolTable.findBuiltIn(variable->getName(), mShaderVersion))
		{
			recover();
		}

		// Reject shaders using both gl_FragData and gl_FragColor
		TQualifier qualifier = variable->getType().getQualifier();
		if(qualifier == EvqFragData)
		{
			mUsesFragData = true;
		}
		else if(qualifier == EvqFragColor)
		{
			mUsesFragColor = true;
		}

		// This validation is not quite correct - it's only an error to write to
		// both FragData and FragColor. For simplicity, and because users shouldn't
		// be rewarded for reading from undefined variables, return an error
		// if they are both referenced, rather than assigned.
		if(mUsesFragData && mUsesFragColor)
		{
			error(location, "cannot use both gl_FragData and gl_FragColor", name->c_str());
			recover();
		}
	}

	if(!variable)
	{
		TType type(EbtFloat, EbpUndefined);
		TVariable *fakeVariable = new TVariable(name, type);
		symbolTable.declare(*fakeVariable);
		variable = fakeVariable;
	}

	return variable;
}

//
// Look up a function name in the symbol table, and make sure it is a function.
//
// Return the function symbol if found, otherwise 0.
//
const TFunction* TParseContext::findFunction(const TSourceLoc &line, TFunction* call, bool *builtIn)
{
    // First find by unmangled name to check whether the function name has been
    // hidden by a variable name or struct typename.
    const TSymbol* symbol = symbolTable.find(call->getName(), mShaderVersion, builtIn);
    if (symbol == 0) {
        symbol = symbolTable.find(call->getMangledName(), mShaderVersion, builtIn);
    }

    if (symbol == 0) {
        error(line, "no matching overloaded function found", call->getName().c_str());
        return 0;
    }

    if (!symbol->isFunction()) {
        error(line, "function name expected", call->getName().c_str());
        return 0;
    }

    return static_cast<const TFunction*>(symbol);
}

//
// Initializers show up in several places in the grammar.  Have one set of
// code to handle them here.
//
bool TParseContext::executeInitializer(const TSourceLoc& line, const TString& identifier, const TPublicType& pType,
                                       TIntermTyped *initializer, TIntermNode **intermNode)
{
	ASSERT(intermNode != nullptr);
	TType type = TType(pType);

	if(type.isArray() && (type.getArraySize() == 0))
	{
		type.setArraySize(initializer->getArraySize());
	}

	TVariable *variable = nullptr;
	if(!declareVariable(line, identifier, type, &variable))
	{
		return true;
	}

	bool globalInitWarning = false;
	if(symbolTable.atGlobalLevel() && !ValidateGlobalInitializer(initializer, this, &globalInitWarning))
	{
		// Error message does not completely match behavior with ESSL 1.00, but
		// we want to steer developers towards only using constant expressions.
		error(line, "global variable initializers must be constant expressions", "=");
		return true;
	}
	if(globalInitWarning)
	{
		warning(line, "global variable initializers should be constant expressions "
			"(uniforms and globals are allowed in global initializers for legacy compatibility)", "=");
    }

    //
    // identifier must be of type constant, a global, or a temporary
    //
    TQualifier qualifier = type.getQualifier();
    if ((qualifier != EvqTemporary) && (qualifier != EvqGlobal) && (qualifier != EvqConstExpr)) {
        error(line, " cannot initialize this type of qualifier ", variable->getType().getQualifierString());
        return true;
    }
    //
    // test for and propagate constant
    //

    if (qualifier == EvqConstExpr) {
        if (qualifier != initializer->getQualifier()) {
            std::stringstream extraInfoStream;
            extraInfoStream << "'" << variable->getType().getCompleteString() << "'";
            std::string extraInfo = extraInfoStream.str();
            error(line, " assigning non-constant to", "=", extraInfo.c_str());
            variable->getType().setQualifier(EvqTemporary);
            return true;
        }

        if (type != initializer->getType()) {
            error(line, " non-matching types for const initializer ",
                variable->getType().getQualifierString());
            variable->getType().setQualifier(EvqTemporary);
            return true;
        }

        if (initializer->getAsConstantUnion()) {
            variable->shareConstPointer(initializer->getAsConstantUnion()->getUnionArrayPointer());
        } else if (initializer->getAsSymbolNode()) {
            const TSymbol* symbol = symbolTable.find(initializer->getAsSymbolNode()->getSymbol(), 0);
            const TVariable* tVar = static_cast<const TVariable*>(symbol);

            ConstantUnion* constArray = tVar->getConstPointer();
            variable->shareConstPointer(constArray);
        }
    }

    if (!variable->isConstant()) {
        TIntermSymbol* intermSymbol = intermediate.addSymbol(variable->getUniqueId(), variable->getName(), variable->getType(), line);
        *intermNode = createAssign(EOpInitialize, intermSymbol, initializer, line);
        if(*intermNode == nullptr) {
            assignError(line, "=", intermSymbol->getCompleteString(), initializer->getCompleteString());
            return true;
        }
    } else
        *intermNode = nullptr;

    return false;
}

TPublicType TParseContext::addFullySpecifiedType(TQualifier qualifier, bool invariant, TLayoutQualifier layoutQualifier, const TPublicType &typeSpecifier)
{
	TPublicType returnType = typeSpecifier;
	returnType.qualifier = qualifier;
	returnType.invariant = invariant;
	returnType.layoutQualifier = layoutQualifier;

	if(typeSpecifier.array)
	{
		error(typeSpecifier.line, "not supported", "first-class array");
		recover();
		returnType.clearArrayness();
	}

	if(mShaderVersion < 300)
	{
		if(qualifier == EvqAttribute && (typeSpecifier.type == EbtBool || typeSpecifier.type == EbtInt))
		{
			error(typeSpecifier.line, "cannot be bool or int", getQualifierString(qualifier));
			recover();
		}

		if((qualifier == EvqVaryingIn || qualifier == EvqVaryingOut) &&
			(typeSpecifier.type == EbtBool || typeSpecifier.type == EbtInt))
		{
			error(typeSpecifier.line, "cannot be bool or int", getQualifierString(qualifier));
			recover();
		}
	}
	else
	{
		switch(qualifier)
		{
		case EvqSmoothIn:
		case EvqSmoothOut:
		case EvqVertexOut:
		case EvqFragmentIn:
		case EvqCentroidOut:
		case EvqCentroidIn:
			if(typeSpecifier.type == EbtBool)
			{
				error(typeSpecifier.line, "cannot be bool", getQualifierString(qualifier));
				recover();
			}
			if(typeSpecifier.type == EbtInt || typeSpecifier.type == EbtUInt)
			{
				error(typeSpecifier.line, "must use 'flat' interpolation here", getQualifierString(qualifier));
				recover();
			}
			break;

		case EvqVertexIn:
		case EvqFragmentOut:
		case EvqFlatIn:
		case EvqFlatOut:
			if(typeSpecifier.type == EbtBool)
			{
				error(typeSpecifier.line, "cannot be bool", getQualifierString(qualifier));
				recover();
			}
			break;

		default: break;
		}
	}

	return returnType;
}

TIntermAggregate *TParseContext::parseSingleDeclaration(TPublicType &publicType,
	const TSourceLoc &identifierOrTypeLocation,
	const TString &identifier)
{
	TIntermSymbol *symbol = intermediate.addSymbol(0, identifier, TType(publicType), identifierOrTypeLocation);

	bool emptyDeclaration = (identifier == "");

	mDeferredSingleDeclarationErrorCheck = emptyDeclaration;

	if(emptyDeclaration)
	{
		if(publicType.isUnsizedArray())
		{
			// ESSL3 spec section 4.1.9: Array declaration which leaves the size unspecified is an error.
			// It is assumed that this applies to empty declarations as well.
			error(identifierOrTypeLocation, "empty array declaration needs to specify a size", identifier.c_str());
		}
	}
	else
	{
		if(singleDeclarationErrorCheck(publicType, identifierOrTypeLocation))
			recover();

		if(nonInitErrorCheck(identifierOrTypeLocation, identifier, publicType))
			recover();

		TVariable *variable = nullptr;
		if(!declareVariable(identifierOrTypeLocation, identifier, TType(publicType), &variable))
			recover();

		if(variable && symbol)
			symbol->setId(variable->getUniqueId());
	}

	return intermediate.makeAggregate(symbol, identifierOrTypeLocation);
}

TIntermAggregate *TParseContext::parseSingleArrayDeclaration(TPublicType &publicType,
	const TSourceLoc &identifierLocation,
	const TString &identifier,
	const TSourceLoc &indexLocation,
	TIntermTyped *indexExpression)
{
	mDeferredSingleDeclarationErrorCheck = false;

	if(singleDeclarationErrorCheck(publicType, identifierLocation))
		recover();

	if(nonInitErrorCheck(identifierLocation, identifier, publicType))
		recover();

	if(arrayTypeErrorCheck(indexLocation, publicType) || arrayQualifierErrorCheck(indexLocation, publicType))
	{
		recover();
	}

	TType arrayType(publicType);

	int size;
	if(arraySizeErrorCheck(identifierLocation, indexExpression, size))
	{
		recover();
	}
	// Make the type an array even if size check failed.
	// This ensures useless error messages regarding the variable's non-arrayness won't follow.
	arrayType.setArraySize(size);

	TVariable *variable = nullptr;
	if(!declareVariable(identifierLocation, identifier, arrayType, &variable))
		recover();

	TIntermSymbol *symbol = intermediate.addSymbol(0, identifier, arrayType, identifierLocation);
	if(variable && symbol)
		symbol->setId(variable->getUniqueId());

	return intermediate.makeAggregate(symbol, identifierLocation);
}

TIntermAggregate *TParseContext::parseSingleInitDeclaration(const TPublicType &publicType,
	const TSourceLoc &identifierLocation,
	const TString &identifier,
	const TSourceLoc &initLocation,
	TIntermTyped *initializer)
{
	mDeferredSingleDeclarationErrorCheck = false;

	if(singleDeclarationErrorCheck(publicType, identifierLocation))
		recover();

	TIntermNode *intermNode = nullptr;
	if(!executeInitializer(identifierLocation, identifier, publicType, initializer, &intermNode))
	{
		//
		// Build intermediate representation
		//
		return intermNode ? intermediate.makeAggregate(intermNode, initLocation) : nullptr;
	}
	else
	{
		recover();
		return nullptr;
	}
}

TIntermAggregate *TParseContext::parseSingleArrayInitDeclaration(TPublicType &publicType,
	const TSourceLoc &identifierLocation,
	const TString &identifier,
	const TSourceLoc &indexLocation,
	TIntermTyped *indexExpression,
	const TSourceLoc &initLocation,
	TIntermTyped *initializer)
{
	mDeferredSingleDeclarationErrorCheck = false;

	if(singleDeclarationErrorCheck(publicType, identifierLocation))
		recover();

	if(arrayTypeErrorCheck(indexLocation, publicType) || arrayQualifierErrorCheck(indexLocation, publicType))
	{
		recover();
	}

	TPublicType arrayType(publicType);

	int size = 0;
	// If indexExpression is nullptr, then the array will eventually get its size implicitly from the initializer.
	if(indexExpression != nullptr && arraySizeErrorCheck(identifierLocation, indexExpression, size))
	{
		recover();
	}
	// Make the type an array even if size check failed.
	// This ensures useless error messages regarding the variable's non-arrayness won't follow.
	arrayType.setArray(true, size);

	// initNode will correspond to the whole of "type b[n] = initializer".
	TIntermNode *initNode = nullptr;
	if(!executeInitializer(identifierLocation, identifier, arrayType, initializer, &initNode))
	{
		return initNode ? intermediate.makeAggregate(initNode, initLocation) : nullptr;
	}
	else
	{
		recover();
		return nullptr;
	}
}

TIntermAggregate *TParseContext::parseInvariantDeclaration(const TSourceLoc &invariantLoc,
	const TSourceLoc &identifierLoc,
	const TString *identifier,
	const TSymbol *symbol)
{
	// invariant declaration
	if(globalErrorCheck(invariantLoc, symbolTable.atGlobalLevel(), "invariant varying"))
	{
		recover();
	}

	if(!symbol)
	{
		error(identifierLoc, "undeclared identifier declared as invariant", identifier->c_str());
		recover();
		return nullptr;
	}
	else
	{
		const TString kGlFrontFacing("gl_FrontFacing");
		if(*identifier == kGlFrontFacing)
		{
			error(identifierLoc, "identifier should not be declared as invariant", identifier->c_str());
			recover();
			return nullptr;
		}
		symbolTable.addInvariantVarying(std::string(identifier->c_str()));
		const TVariable *variable = getNamedVariable(identifierLoc, identifier, symbol);
		ASSERT(variable);
		const TType &type = variable->getType();
		TIntermSymbol *intermSymbol = intermediate.addSymbol(variable->getUniqueId(),
			*identifier, type, identifierLoc);

		TIntermAggregate *aggregate = intermediate.makeAggregate(intermSymbol, identifierLoc);
		aggregate->setOp(EOpInvariantDeclaration);
		return aggregate;
	}
}

TIntermAggregate *TParseContext::parseDeclarator(TPublicType &publicType, TIntermAggregate *aggregateDeclaration,
	const TSourceLoc &identifierLocation, const TString &identifier)
{
	// If the declaration starting this declarator list was empty (example: int,), some checks were not performed.
	if(mDeferredSingleDeclarationErrorCheck)
	{
		if(singleDeclarationErrorCheck(publicType, identifierLocation))
			recover();
		mDeferredSingleDeclarationErrorCheck = false;
	}

	if(locationDeclaratorListCheck(identifierLocation, publicType))
		recover();

	if(nonInitErrorCheck(identifierLocation, identifier, publicType))
		recover();

	TVariable *variable = nullptr;
	if(!declareVariable(identifierLocation, identifier, TType(publicType), &variable))
		recover();

	TIntermSymbol *symbol = intermediate.addSymbol(0, identifier, TType(publicType), identifierLocation);
	if(variable && symbol)
		symbol->setId(variable->getUniqueId());

	return intermediate.growAggregate(aggregateDeclaration, symbol, identifierLocation);
}

TIntermAggregate *TParseContext::parseArrayDeclarator(TPublicType &publicType, TIntermAggregate *aggregateDeclaration,
	const TSourceLoc &identifierLocation, const TString &identifier,
	const TSourceLoc &arrayLocation, TIntermTyped *indexExpression)
{
	// If the declaration starting this declarator list was empty (example: int,), some checks were not performed.
	if(mDeferredSingleDeclarationErrorCheck)
	{
		if(singleDeclarationErrorCheck(publicType, identifierLocation))
			recover();
		mDeferredSingleDeclarationErrorCheck = false;
	}

	if(locationDeclaratorListCheck(identifierLocation, publicType))
		recover();

	if(nonInitErrorCheck(identifierLocation, identifier, publicType))
		recover();

	if(arrayTypeErrorCheck(arrayLocation, publicType) || arrayQualifierErrorCheck(arrayLocation, publicType))
	{
		recover();
	}
	else
	{
		TType arrayType = TType(publicType);
		int size;
		if(arraySizeErrorCheck(arrayLocation, indexExpression, size))
		{
			recover();
		}
		arrayType.setArraySize(size);

		TVariable *variable = nullptr;
		if(!declareVariable(identifierLocation, identifier, arrayType, &variable))
			recover();

		TIntermSymbol *symbol = intermediate.addSymbol(0, identifier, arrayType, identifierLocation);
		if(variable && symbol)
			symbol->setId(variable->getUniqueId());

		return intermediate.growAggregate(aggregateDeclaration, symbol, identifierLocation);
	}

	return nullptr;
}

TIntermAggregate *TParseContext::parseInitDeclarator(const TPublicType &publicType, TIntermAggregate *aggregateDeclaration,
	const TSourceLoc &identifierLocation, const TString &identifier,
	const TSourceLoc &initLocation, TIntermTyped *initializer)
{
	// If the declaration starting this declarator list was empty (example: int,), some checks were not performed.
	if(mDeferredSingleDeclarationErrorCheck)
	{
		if(singleDeclarationErrorCheck(publicType, identifierLocation))
			recover();
		mDeferredSingleDeclarationErrorCheck = false;
	}

	if(locationDeclaratorListCheck(identifierLocation, publicType))
		recover();

	TIntermNode *intermNode = nullptr;
	if(!executeInitializer(identifierLocation, identifier, publicType, initializer, &intermNode))
	{
		//
		// build the intermediate representation
		//
		if(intermNode)
		{
			return intermediate.growAggregate(aggregateDeclaration, intermNode, initLocation);
		}
		else
		{
			return aggregateDeclaration;
		}
	}
	else
	{
		recover();
		return nullptr;
	}
}

TIntermAggregate *TParseContext::parseArrayInitDeclarator(const TPublicType &publicType,
	TIntermAggregate *aggregateDeclaration,
	const TSourceLoc &identifierLocation,
	const TString &identifier,
	const TSourceLoc &indexLocation,
	TIntermTyped *indexExpression,
	const TSourceLoc &initLocation, TIntermTyped *initializer)
{
	// If the declaration starting this declarator list was empty (example: int,), some checks were not performed.
	if(mDeferredSingleDeclarationErrorCheck)
	{
		if(singleDeclarationErrorCheck(publicType, identifierLocation))
			recover();
		mDeferredSingleDeclarationErrorCheck = false;
	}

	if(locationDeclaratorListCheck(identifierLocation, publicType))
		recover();

	if(arrayTypeErrorCheck(indexLocation, publicType) || arrayQualifierErrorCheck(indexLocation, publicType))
	{
		recover();
	}

	TPublicType arrayType(publicType);

	int size = 0;
	// If indexExpression is nullptr, then the array will eventually get its size implicitly from the initializer.
	if(indexExpression != nullptr && arraySizeErrorCheck(identifierLocation, indexExpression, size))
	{
		recover();
	}
	// Make the type an array even if size check failed.
	// This ensures useless error messages regarding the variable's non-arrayness won't follow.
	arrayType.setArray(true, size);

	// initNode will correspond to the whole of "b[n] = initializer".
	TIntermNode *initNode = nullptr;
	if(!executeInitializer(identifierLocation, identifier, arrayType, initializer, &initNode))
	{
		if(initNode)
		{
			return intermediate.growAggregate(aggregateDeclaration, initNode, initLocation);
		}
		else
		{
			return aggregateDeclaration;
		}
	}
	else
	{
		recover();
		return nullptr;
	}
}

void TParseContext::parseGlobalLayoutQualifier(const TPublicType &typeQualifier)
{
	if(mShaderVersion < 300)
	{
		error(typeQualifier.line, "layout qualifiers supported in GLSL ES 3.00 only", "layout");
		recover();
		return;
	}

	if(typeQualifier.qualifier != EvqUniform)
	{
		error(typeQualifier.line, "invalid qualifier:", getQualifierString(typeQualifier.qualifier), "global layout must be uniform");
		recover();
		return;
	}

	const TLayoutQualifier layoutQualifier = typeQualifier.layoutQualifier;
	ASSERT(!layoutQualifier.isEmpty());

	if(layoutLocationErrorCheck(typeQualifier.line, typeQualifier.layoutQualifier))
	{
		recover();
		return;
	}

	if(layoutQualifier.matrixPacking != EmpUnspecified)
	{
		mDefaultMatrixPacking = layoutQualifier.matrixPacking;
	}

	if(layoutQualifier.blockStorage != EbsUnspecified)
	{
		mDefaultBlockStorage = layoutQualifier.blockStorage;
	}
}

TIntermAggregate *TParseContext::addFunctionPrototypeDeclaration(const TFunction &function, const TSourceLoc &location)
{
	// Note: symbolTableFunction could be the same as function if this is the first declaration.
	// Either way the instance in the symbol table is used to track whether the function is declared
	// multiple times.
	TFunction *symbolTableFunction =
		static_cast<TFunction *>(symbolTable.find(function.getMangledName(), getShaderVersion()));
	if(symbolTableFunction->hasPrototypeDeclaration() && mShaderVersion == 100)
	{
		// ESSL 1.00.17 section 4.2.7.
		// Doesn't apply to ESSL 3.00.4: see section 4.2.3.
		error(location, "duplicate function prototype declarations are not allowed", "function");
		recover();
	}
	symbolTableFunction->setHasPrototypeDeclaration();

	TIntermAggregate *prototype = new TIntermAggregate;
	prototype->setType(function.getReturnType());
	prototype->setName(function.getMangledName());

	for(size_t i = 0; i < function.getParamCount(); i++)
	{
		const TParameter &param = function.getParam(i);
		if(param.name != 0)
		{
			TVariable variable(param.name, *param.type);

			TIntermSymbol *paramSymbol = intermediate.addSymbol(
				variable.getUniqueId(), variable.getName(), variable.getType(), location);
			prototype = intermediate.growAggregate(prototype, paramSymbol, location);
		}
		else
		{
			TIntermSymbol *paramSymbol = intermediate.addSymbol(0, "", *param.type, location);
			prototype = intermediate.growAggregate(prototype, paramSymbol, location);
		}
	}

	prototype->setOp(EOpPrototype);

	symbolTable.pop();

	if(!symbolTable.atGlobalLevel())
	{
		// ESSL 3.00.4 section 4.2.4.
		error(location, "local function prototype declarations are not allowed", "function");
		recover();
	}

	return prototype;
}

TIntermAggregate *TParseContext::addFunctionDefinition(const TFunction &function, TIntermAggregate *functionPrototype, TIntermAggregate *functionBody, const TSourceLoc &location)
{
	//?? Check that all paths return a value if return type != void ?
	//   May be best done as post process phase on intermediate code
	if(mCurrentFunctionType->getBasicType() != EbtVoid && !mFunctionReturnsValue)
	{
		error(location, "function does not return a value:", "", function.getName().c_str());
		recover();
	}

	TIntermAggregate *aggregate = intermediate.growAggregate(functionPrototype, functionBody, location);
	intermediate.setAggregateOperator(aggregate, EOpFunction, location);
	aggregate->setName(function.getMangledName().c_str());
	aggregate->setType(function.getReturnType());

	// store the pragma information for debug and optimize and other vendor specific
	// information. This information can be queried from the parse tree
	aggregate->setOptimize(pragma().optimize);
	aggregate->setDebug(pragma().debug);

	if(functionBody && functionBody->getAsAggregate())
		aggregate->setEndLine(functionBody->getAsAggregate()->getEndLine());

	symbolTable.pop();
	return aggregate;
}

void TParseContext::parseFunctionPrototype(const TSourceLoc &location, TFunction *function, TIntermAggregate **aggregateOut)
{
	const TSymbol *builtIn = symbolTable.findBuiltIn(function->getMangledName(), getShaderVersion());

	if(builtIn)
	{
		error(location, "built-in functions cannot be redefined", function->getName().c_str());
		recover();
	}

	TFunction *prevDec = static_cast<TFunction *>(symbolTable.find(function->getMangledName(), getShaderVersion()));
	//
	// Note:  'prevDec' could be 'function' if this is the first time we've seen function
	// as it would have just been put in the symbol table.  Otherwise, we're looking up
	// an earlier occurance.
	//
	if(prevDec->isDefined())
	{
		// Then this function already has a body.
		error(location, "function already has a body", function->getName().c_str());
		recover();
	}
	prevDec->setDefined();
	//
	// Overload the unique ID of the definition to be the same unique ID as the declaration.
	// Eventually we will probably want to have only a single definition and just swap the
	// arguments to be the definition's arguments.
	//
	function->setUniqueId(prevDec->getUniqueId());

	// Raise error message if main function takes any parameters or return anything other than void
	if(function->getName() == "main")
	{
		if(function->getParamCount() > 0)
		{
			error(location, "function cannot take any parameter(s)", function->getName().c_str());
			recover();
		}
		if(function->getReturnType().getBasicType() != EbtVoid)
		{
			error(location, "", function->getReturnType().getBasicString(), "main function cannot return a value");
			recover();
		}
	}

	//
	// Remember the return type for later checking for RETURN statements.
	//
	mCurrentFunctionType = &(prevDec->getReturnType());
	mFunctionReturnsValue = false;

	//
	// Insert parameters into the symbol table.
	// If the parameter has no name, it's not an error, just don't insert it
	// (could be used for unused args).
	//
	// Also, accumulate the list of parameters into the HIL, so lower level code
	// knows where to find parameters.
	//
	TIntermAggregate *paramNodes = new TIntermAggregate;
	for(size_t i = 0; i < function->getParamCount(); i++)
	{
		const TParameter &param = function->getParam(i);
		if(param.name != 0)
		{
			TVariable *variable = new TVariable(param.name, *param.type);
			//
			// Insert the parameters with name in the symbol table.
			//
			if(!symbolTable.declare(*variable))
			{
				error(location, "redefinition", variable->getName().c_str());
				recover();
				paramNodes = intermediate.growAggregate(
					paramNodes, intermediate.addSymbol(0, "", *param.type, location), location);
				continue;
			}

			//
			// Add the parameter to the HIL
			//
			TIntermSymbol *symbol = intermediate.addSymbol(
				variable->getUniqueId(), variable->getName(), variable->getType(), location);

			paramNodes = intermediate.growAggregate(paramNodes, symbol, location);
		}
		else
		{
			paramNodes = intermediate.growAggregate(
				paramNodes, intermediate.addSymbol(0, "", *param.type, location), location);
		}
	}
	intermediate.setAggregateOperator(paramNodes, EOpParameters, location);
	*aggregateOut = paramNodes;
	setLoopNestingLevel(0);
}

TFunction *TParseContext::parseFunctionDeclarator(const TSourceLoc &location, TFunction *function)
{
	//
	// We don't know at this point whether this is a function definition or a prototype.
	// The definition production code will check for redefinitions.
	// In the case of ESSL 1.00 the prototype production code will also check for redeclarations.
	//
	// Return types and parameter qualifiers must match in all redeclarations, so those are checked
	// here.
	//
	TFunction *prevDec = static_cast<TFunction *>(symbolTable.find(function->getMangledName(), getShaderVersion()));
	if(prevDec)
	{
		if(prevDec->getReturnType() != function->getReturnType())
		{
			error(location, "overloaded functions must have the same return type",
				function->getReturnType().getBasicString());
			recover();
		}
		for(size_t i = 0; i < prevDec->getParamCount(); ++i)
		{
			if(prevDec->getParam(i).type->getQualifier() != function->getParam(i).type->getQualifier())
			{
				error(location, "overloaded functions must have the same parameter qualifiers",
					function->getParam(i).type->getQualifierString());
				recover();
			}
		}
	}

	//
	// Check for previously declared variables using the same name.
	//
	TSymbol *prevSym = symbolTable.find(function->getName(), getShaderVersion());
	if(prevSym)
	{
		if(!prevSym->isFunction())
		{
			error(location, "redefinition", function->getName().c_str(), "function");
			recover();
		}
	}

	// We're at the inner scope level of the function's arguments and body statement.
	// Add the function prototype to the surrounding scope instead.
	symbolTable.getOuterLevel()->insert(*function);

	//
	// If this is a redeclaration, it could also be a definition, in which case, we want to use the
	// variable names from this one, and not the one that's
	// being redeclared.  So, pass back up this declaration, not the one in the symbol table.
	//
	return function;
}

TFunction *TParseContext::addConstructorFunc(const TPublicType &publicTypeIn)
{
	TPublicType publicType = publicTypeIn;
	TOperator op = EOpNull;
	if(publicType.userDef)
	{
		op = EOpConstructStruct;
	}
	else
	{
		switch(publicType.type)
		{
		case EbtFloat:
			if(publicType.isMatrix())
			{
				switch(publicType.getCols())
				{
				case 2:
					switch(publicType.getRows())
					{
					case 2: op = EOpConstructMat2;  break;
					case 3: op = EOpConstructMat2x3;  break;
					case 4: op = EOpConstructMat2x4;  break;
					}
					break;
				case 3:
					switch(publicType.getRows())
					{
					case 2: op = EOpConstructMat3x2;  break;
					case 3: op = EOpConstructMat3;  break;
					case 4: op = EOpConstructMat3x4;  break;
					}
					break;
				case 4:
					switch(publicType.getRows())
					{
					case 2: op = EOpConstructMat4x2;  break;
					case 3: op = EOpConstructMat4x3;  break;
					case 4: op = EOpConstructMat4;  break;
					}
					break;
				}
			}
			else
			{
				switch(publicType.getNominalSize())
				{
				case 1: op = EOpConstructFloat; break;
				case 2: op = EOpConstructVec2;  break;
				case 3: op = EOpConstructVec3;  break;
				case 4: op = EOpConstructVec4;  break;
				}
			}
			break;

		case EbtInt:
			switch(publicType.getNominalSize())
			{
			case 1: op = EOpConstructInt;   break;
			case 2: op = EOpConstructIVec2; break;
			case 3: op = EOpConstructIVec3; break;
			case 4: op = EOpConstructIVec4; break;
			}
			break;

		case EbtUInt:
			switch(publicType.getNominalSize())
			{
			case 1: op = EOpConstructUInt;  break;
			case 2: op = EOpConstructUVec2; break;
			case 3: op = EOpConstructUVec3; break;
			case 4: op = EOpConstructUVec4; break;
			}
			break;

		case EbtBool:
			switch(publicType.getNominalSize())
			{
			case 1: op = EOpConstructBool;  break;
			case 2: op = EOpConstructBVec2; break;
			case 3: op = EOpConstructBVec3; break;
			case 4: op = EOpConstructBVec4; break;
			}
			break;

		default: break;
		}

		if(op == EOpNull)
		{
			error(publicType.line, "cannot construct this type", getBasicString(publicType.type));
			recover();
			publicType.type = EbtFloat;
			op = EOpConstructFloat;
		}
	}

	TString tempString;
	TType type(publicType);
	return new TFunction(&tempString, type, op);
}

// This function is used to test for the correctness of the parameters passed to various constructor functions
// and also convert them to the right datatype if it is allowed and required.
//
// Returns 0 for an error or the constructed node (aggregate or typed) for no error.
//
TIntermTyped* TParseContext::addConstructor(TIntermNode* arguments, const TType* type, TOperator op, TFunction* fnCall, const TSourceLoc &line)
{
    TIntermAggregate *aggregateArguments = arguments->getAsAggregate();

    if(!aggregateArguments)
    {
        aggregateArguments = new TIntermAggregate;
        aggregateArguments->getSequence().push_back(arguments);
    }

    if(op == EOpConstructStruct)
    {
        const TFieldList &fields = type->getStruct()->fields();
        TIntermSequence &args = aggregateArguments->getSequence();

        for(size_t i = 0; i < fields.size(); i++)
        {
            if(args[i]->getAsTyped()->getType() != *fields[i]->type())
            {
                error(line, "Structure constructor arguments do not match structure fields", "Error");
                recover();

                return 0;
            }
        }
    }

    // Turn the argument list itself into a constructor
    TIntermAggregate *constructor = intermediate.setAggregateOperator(aggregateArguments, op, line);
    TIntermTyped *constConstructor = foldConstConstructor(constructor, *type);
    if(constConstructor)
    {
        return constConstructor;
    }

    return constructor;
}

TIntermTyped* TParseContext::foldConstConstructor(TIntermAggregate* aggrNode, const TType& type)
{
    aggrNode->setType(type);
    if (aggrNode->isConstantFoldable()) {
        bool returnVal = false;
        ConstantUnion* unionArray = new ConstantUnion[type.getObjectSize()];
        if (aggrNode->getSequence().size() == 1)  {
            returnVal = intermediate.parseConstTree(aggrNode->getLine(), aggrNode, unionArray, aggrNode->getOp(), type, true);
        }
        else {
            returnVal = intermediate.parseConstTree(aggrNode->getLine(), aggrNode, unionArray, aggrNode->getOp(), type);
        }
        if (returnVal)
            return 0;

        return intermediate.addConstantUnion(unionArray, type, aggrNode->getLine());
    }

    return 0;
}

//
// This function returns the tree representation for the vector field(s) being accessed from contant vector.
// If only one component of vector is accessed (v.x or v[0] where v is a contant vector), then a contant node is
// returned, else an aggregate node is returned (for v.xy). The input to this function could either be the symbol
// node or it could be the intermediate tree representation of accessing fields in a constant structure or column of
// a constant matrix.
//
TIntermTyped* TParseContext::addConstVectorNode(TVectorFields& fields, TIntermTyped* node, const TSourceLoc &line)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();

    ConstantUnion *unionArray;
    if (tempConstantNode) {
        unionArray = tempConstantNode->getUnionArrayPointer();

        if (!unionArray) {
            return node;
        }
    } else { // The node has to be either a symbol node or an aggregate node or a tempConstant node, else, its an error
        error(line, "Cannot offset into the vector", "Error");
        recover();

        return 0;
    }

    ConstantUnion* constArray = new ConstantUnion[fields.num];

    for (int i = 0; i < fields.num; i++) {
        if (fields.offsets[i] >= node->getType().getObjectSize()) {
            std::stringstream extraInfoStream;
            extraInfoStream << "vector field selection out of range '" << fields.offsets[i] << "'";
            std::string extraInfo = extraInfoStream.str();
            error(line, "", "[", extraInfo.c_str());
            recover();
            fields.offsets[i] = 0;
        }

        constArray[i] = unionArray[fields.offsets[i]];

    }
    typedNode = intermediate.addConstantUnion(constArray, node->getType(), line);
    return typedNode;
}

//
// This function returns the column being accessed from a constant matrix. The values are retrieved from
// the symbol table and parse-tree is built for a vector (each column of a matrix is a vector). The input
// to the function could either be a symbol node (m[0] where m is a constant matrix)that represents a
// constant matrix or it could be the tree representation of the constant matrix (s.m1[0] where s is a constant structure)
//
TIntermTyped* TParseContext::addConstMatrixNode(int index, TIntermTyped* node, const TSourceLoc &line)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();

    if (index >= node->getType().getNominalSize()) {
        std::stringstream extraInfoStream;
        extraInfoStream << "matrix field selection out of range '" << index << "'";
        std::string extraInfo = extraInfoStream.str();
        error(line, "", "[", extraInfo.c_str());
        recover();
        index = 0;
    }

    if (tempConstantNode) {
         ConstantUnion* unionArray = tempConstantNode->getUnionArrayPointer();
         int size = tempConstantNode->getType().getNominalSize();
         typedNode = intermediate.addConstantUnion(&unionArray[size*index], tempConstantNode->getType(), line);
    } else {
        error(line, "Cannot offset into the matrix", "Error");
        recover();

        return 0;
    }

    return typedNode;
}


//
// This function returns an element of an array accessed from a constant array. The values are retrieved from
// the symbol table and parse-tree is built for the type of the element. The input
// to the function could either be a symbol node (a[0] where a is a constant array)that represents a
// constant array or it could be the tree representation of the constant array (s.a1[0] where s is a constant structure)
//
TIntermTyped* TParseContext::addConstArrayNode(int index, TIntermTyped* node, const TSourceLoc &line)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();
    TType arrayElementType = node->getType();
    arrayElementType.clearArrayness();

    if (index >= node->getType().getArraySize()) {
        std::stringstream extraInfoStream;
        extraInfoStream << "array field selection out of range '" << index << "'";
        std::string extraInfo = extraInfoStream.str();
        error(line, "", "[", extraInfo.c_str());
        recover();
        index = 0;
    }

    int arrayElementSize = arrayElementType.getObjectSize();

    if (tempConstantNode) {
         ConstantUnion* unionArray = tempConstantNode->getUnionArrayPointer();
         typedNode = intermediate.addConstantUnion(&unionArray[arrayElementSize * index], tempConstantNode->getType(), line);
    } else {
        error(line, "Cannot offset into the array", "Error");
        recover();

        return 0;
    }

    return typedNode;
}


//
// This function returns the value of a particular field inside a constant structure from the symbol table.
// If there is an embedded/nested struct, it appropriately calls addConstStructNested or addConstStructFromAggr
// function and returns the parse-tree with the values of the embedded/nested struct.
//
TIntermTyped* TParseContext::addConstStruct(const TString& identifier, TIntermTyped* node, const TSourceLoc &line)
{
    const TFieldList &fields = node->getType().getStruct()->fields();
    TIntermTyped *typedNode;
    int instanceSize = 0;
    unsigned int index = 0;
    TIntermConstantUnion *tempConstantNode = node->getAsConstantUnion();

    for ( index = 0; index < fields.size(); ++index) {
        if (fields[index]->name() == identifier) {
            break;
        } else {
            instanceSize += fields[index]->type()->getObjectSize();
        }
    }

    if (tempConstantNode) {
         ConstantUnion* constArray = tempConstantNode->getUnionArrayPointer();

         typedNode = intermediate.addConstantUnion(constArray+instanceSize, tempConstantNode->getType(), line); // type will be changed in the calling function
    } else {
        error(line, "Cannot offset into the structure", "Error");
        recover();

        return 0;
    }

    return typedNode;
}

//
// Interface/uniform blocks
//
TIntermAggregate* TParseContext::addInterfaceBlock(const TPublicType& typeQualifier, const TSourceLoc& nameLine, const TString& blockName, TFieldList* fieldList,
                                                   const TString* instanceName, const TSourceLoc& instanceLine, TIntermTyped* arrayIndex, const TSourceLoc& arrayIndexLine)
{
	if(reservedErrorCheck(nameLine, blockName))
		recover();

	if(typeQualifier.qualifier != EvqUniform)
	{
		error(typeQualifier.line, "invalid qualifier:", getQualifierString(typeQualifier.qualifier), "interface blocks must be uniform");
		recover();
	}

	TLayoutQualifier blockLayoutQualifier = typeQualifier.layoutQualifier;
	if(layoutLocationErrorCheck(typeQualifier.line, blockLayoutQualifier))
	{
		recover();
	}

	if(blockLayoutQualifier.matrixPacking == EmpUnspecified)
	{
		blockLayoutQualifier.matrixPacking = mDefaultMatrixPacking;
	}

	if(blockLayoutQualifier.blockStorage == EbsUnspecified)
	{
		blockLayoutQualifier.blockStorage = mDefaultBlockStorage;
	}

	TSymbol* blockNameSymbol = new TSymbol(&blockName);
	if(!symbolTable.declare(*blockNameSymbol)) {
		error(nameLine, "redefinition", blockName.c_str(), "interface block name");
		recover();
	}

	// check for sampler types and apply layout qualifiers
	for(size_t memberIndex = 0; memberIndex < fieldList->size(); ++memberIndex) {
		TField* field = (*fieldList)[memberIndex];
		TType* fieldType = field->type();
		if(IsSampler(fieldType->getBasicType())) {
			error(field->line(), "unsupported type", fieldType->getBasicString(), "sampler types are not allowed in interface blocks");
			recover();
		}

		const TQualifier qualifier = fieldType->getQualifier();
		switch(qualifier)
		{
		case EvqGlobal:
		case EvqUniform:
			break;
		default:
			error(field->line(), "invalid qualifier on interface block member", getQualifierString(qualifier));
			recover();
			break;
		}

		// check layout qualifiers
		TLayoutQualifier fieldLayoutQualifier = fieldType->getLayoutQualifier();
		if(layoutLocationErrorCheck(field->line(), fieldLayoutQualifier))
		{
			recover();
		}

		if(fieldLayoutQualifier.blockStorage != EbsUnspecified)
		{
			error(field->line(), "invalid layout qualifier:", getBlockStorageString(fieldLayoutQualifier.blockStorage), "cannot be used here");
			recover();
		}

		if(fieldLayoutQualifier.matrixPacking == EmpUnspecified)
		{
			fieldLayoutQualifier.matrixPacking = blockLayoutQualifier.matrixPacking;
		}
		else if(!fieldType->isMatrix())
		{
			error(field->line(), "invalid layout qualifier:", getMatrixPackingString(fieldLayoutQualifier.matrixPacking), "can only be used on matrix types");
			recover();
		}

		fieldType->setLayoutQualifier(fieldLayoutQualifier);
	}

	// add array index
	int arraySize = 0;
	if(arrayIndex != NULL)
	{
		if(arraySizeErrorCheck(arrayIndexLine, arrayIndex, arraySize))
			recover();
	}

	TInterfaceBlock* interfaceBlock = new TInterfaceBlock(&blockName, fieldList, instanceName, arraySize, blockLayoutQualifier);
	TType interfaceBlockType(interfaceBlock, typeQualifier.qualifier, blockLayoutQualifier, arraySize);

	TString symbolName = "";
	int symbolId = 0;

	if(!instanceName)
	{
		// define symbols for the members of the interface block
		for(size_t memberIndex = 0; memberIndex < fieldList->size(); ++memberIndex)
		{
			TField* field = (*fieldList)[memberIndex];
			TType* fieldType = field->type();

			// set parent pointer of the field variable
			fieldType->setInterfaceBlock(interfaceBlock);

			TVariable* fieldVariable = new TVariable(&field->name(), *fieldType);
			fieldVariable->setQualifier(typeQualifier.qualifier);

			if(!symbolTable.declare(*fieldVariable)) {
				error(field->line(), "redefinition", field->name().c_str(), "interface block member name");
				recover();
			}
		}
	}
	else
	{
		// add a symbol for this interface block
		TVariable* instanceTypeDef = new TVariable(instanceName, interfaceBlockType, false);
		instanceTypeDef->setQualifier(typeQualifier.qualifier);

		if(!symbolTable.declare(*instanceTypeDef)) {
			error(instanceLine, "redefinition", instanceName->c_str(), "interface block instance name");
			recover();
		}

		symbolId = instanceTypeDef->getUniqueId();
		symbolName = instanceTypeDef->getName();
	}

	TIntermAggregate *aggregate = intermediate.makeAggregate(intermediate.addSymbol(symbolId, symbolName, interfaceBlockType, typeQualifier.line), nameLine);
	aggregate->setOp(EOpDeclaration);

	exitStructDeclaration();
	return aggregate;
}

//
// Parse an array index expression
//
TIntermTyped *TParseContext::addIndexExpression(TIntermTyped *baseExpression, const TSourceLoc &location, TIntermTyped *indexExpression)
{
	TIntermTyped *indexedExpression = NULL;

	if(!baseExpression->isArray() && !baseExpression->isMatrix() && !baseExpression->isVector())
	{
		if(baseExpression->getAsSymbolNode())
		{
			error(location, " left of '[' is not of type array, matrix, or vector ",
				baseExpression->getAsSymbolNode()->getSymbol().c_str());
		}
		else
		{
			error(location, " left of '[' is not of type array, matrix, or vector ", "expression");
		}
		recover();
	}

	TIntermConstantUnion *indexConstantUnion = indexExpression->getAsConstantUnion();

	if(indexExpression->getQualifier() == EvqConstExpr && indexConstantUnion)
	{
		int index = indexConstantUnion->getIConst(0);
		if(index < 0)
		{
			std::stringstream infoStream;
			infoStream << index;
			std::string info = infoStream.str();
			error(location, "negative index", info.c_str());
			recover();
			index = 0;
		}
		if(baseExpression->getType().getQualifier() == EvqConstExpr)
		{
			if(baseExpression->isArray())
			{
				// constant folding for arrays
				indexedExpression = addConstArrayNode(index, baseExpression, location);
			}
			else if(baseExpression->isVector())
			{
				// constant folding for vectors
				TVectorFields fields;
				fields.num = 1;
				fields.offsets[0] = index; // need to do it this way because v.xy sends fields integer array
				indexedExpression = addConstVectorNode(fields, baseExpression, location);
			}
			else if(baseExpression->isMatrix())
			{
				// constant folding for matrices
				indexedExpression = addConstMatrixNode(index, baseExpression, location);
			}
		}
		else
		{
			int safeIndex = -1;

			if(baseExpression->isArray())
			{
				if(index >= baseExpression->getType().getArraySize())
				{
					std::stringstream extraInfoStream;
					extraInfoStream << "array index out of range '" << index << "'";
					std::string extraInfo = extraInfoStream.str();
					error(location, "", "[", extraInfo.c_str());
					recover();
					safeIndex = baseExpression->getType().getArraySize() - 1;
				}
			}
			else if((baseExpression->isVector() || baseExpression->isMatrix()) &&
				baseExpression->getType().getNominalSize() <= index)
			{
				std::stringstream extraInfoStream;
				extraInfoStream << "field selection out of range '" << index << "'";
				std::string extraInfo = extraInfoStream.str();
				error(location, "", "[", extraInfo.c_str());
				recover();
				safeIndex = baseExpression->getType().getNominalSize() - 1;
			}

			// Don't modify the data of the previous constant union, because it can point
			// to builtins, like gl_MaxDrawBuffers. Instead use a new sanitized object.
			if(safeIndex != -1)
			{
				ConstantUnion *safeConstantUnion = new ConstantUnion();
				safeConstantUnion->setIConst(safeIndex);
				indexConstantUnion->replaceConstantUnion(safeConstantUnion);
			}

			indexedExpression = intermediate.addIndex(EOpIndexDirect, baseExpression, indexExpression, location);
		}
	}
	else
	{
		if(baseExpression->isInterfaceBlock())
		{
			error(location, "",
				"[", "array indexes for interface blocks arrays must be constant integral expressions");
			recover();
		}
		else if(baseExpression->getQualifier() == EvqFragmentOut)
		{
			error(location, "", "[", "array indexes for fragment outputs must be constant integral expressions");
			recover();
		}

		indexedExpression = intermediate.addIndex(EOpIndexIndirect, baseExpression, indexExpression, location);
	}

	if(indexedExpression == 0)
	{
		ConstantUnion *unionArray = new ConstantUnion[1];
		unionArray->setFConst(0.0f);
		indexedExpression = intermediate.addConstantUnion(unionArray, TType(EbtFloat, EbpHigh, EvqConstExpr), location);
	}
	else if(baseExpression->isArray())
	{
		const TType &baseType = baseExpression->getType();
		if(baseType.getStruct())
		{
			TType copyOfType(baseType.getStruct());
			indexedExpression->setType(copyOfType);
		}
		else if(baseType.isInterfaceBlock())
		{
			TType copyOfType(baseType.getInterfaceBlock(), EvqTemporary, baseType.getLayoutQualifier(), 0);
			indexedExpression->setType(copyOfType);
		}
		else
		{
			indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(),
				EvqTemporary, static_cast<unsigned char>(baseExpression->getNominalSize()),
				static_cast<unsigned char>(baseExpression->getSecondarySize())));
		}

		if(baseExpression->getType().getQualifier() == EvqConstExpr)
		{
			indexedExpression->getTypePointer()->setQualifier(EvqConstExpr);
		}
	}
	else if(baseExpression->isMatrix())
	{
		TQualifier qualifier = baseExpression->getType().getQualifier() == EvqConstExpr ? EvqConstExpr : EvqTemporary;
		indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(),
			qualifier, static_cast<unsigned char>(baseExpression->getSecondarySize())));
	}
	else if(baseExpression->isVector())
	{
		TQualifier qualifier = baseExpression->getType().getQualifier() == EvqConstExpr ? EvqConstExpr : EvqTemporary;
		indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(), qualifier));
	}
	else
	{
		indexedExpression->setType(baseExpression->getType());
	}

	return indexedExpression;
}

TIntermTyped *TParseContext::addFieldSelectionExpression(TIntermTyped *baseExpression, const TSourceLoc &dotLocation,
	const TString &fieldString, const TSourceLoc &fieldLocation)
{
	TIntermTyped *indexedExpression = NULL;

	if(baseExpression->isArray())
	{
		error(fieldLocation, "cannot apply dot operator to an array", ".");
		recover();
	}

	if(baseExpression->isVector())
	{
		TVectorFields fields;
		if(!parseVectorFields(fieldString, baseExpression->getNominalSize(), fields, fieldLocation))
		{
			fields.num = 1;
			fields.offsets[0] = 0;
			recover();
		}

		if(baseExpression->getAsConstantUnion())
		{
			// constant folding for vector fields
			indexedExpression = addConstVectorNode(fields, baseExpression, fieldLocation);
			if(indexedExpression == 0)
			{
				recover();
				indexedExpression = baseExpression;
			}
			else
			{
				indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(),
					EvqConstExpr, (unsigned char)(fieldString).size()));
			}
		}
		else
		{
			TString vectorString = fieldString;
			TIntermTyped *index = intermediate.addSwizzle(fields, fieldLocation);
			indexedExpression = intermediate.addIndex(EOpVectorSwizzle, baseExpression, index, dotLocation);
			indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(),
				baseExpression->getQualifier() == EvqConstExpr ? EvqConstExpr : EvqTemporary, (unsigned char)vectorString.size()));
		}
	}
	else if(baseExpression->isMatrix())
	{
		TMatrixFields fields;
		if(!parseMatrixFields(fieldString, baseExpression->getNominalSize(), baseExpression->getSecondarySize(), fields, fieldLocation))
		{
			fields.wholeRow = false;
			fields.wholeCol = false;
			fields.row = 0;
			fields.col = 0;
			recover();
		}

		if(fields.wholeRow || fields.wholeCol)
		{
			error(dotLocation, " non-scalar fields not implemented yet", ".");
			recover();
			ConstantUnion *unionArray = new ConstantUnion[1];
			unionArray->setIConst(0);
			TIntermTyped *index = intermediate.addConstantUnion(unionArray, TType(EbtInt, EbpUndefined, EvqConstExpr),
				fieldLocation);
			indexedExpression = intermediate.addIndex(EOpIndexDirect, baseExpression, index, dotLocation);
			indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision(),
				EvqTemporary, static_cast<unsigned char>(baseExpression->getNominalSize()),
				static_cast<unsigned char>(baseExpression->getSecondarySize())));
		}
		else
		{
			ConstantUnion *unionArray = new ConstantUnion[1];
			unionArray->setIConst(fields.col * baseExpression->getSecondarySize() + fields.row);
			TIntermTyped *index = intermediate.addConstantUnion(unionArray, TType(EbtInt, EbpUndefined, EvqConstExpr),
				fieldLocation);
			indexedExpression = intermediate.addIndex(EOpIndexDirect, baseExpression, index, dotLocation);
			indexedExpression->setType(TType(baseExpression->getBasicType(), baseExpression->getPrecision()));
		}
	}
	else if(baseExpression->getBasicType() == EbtStruct)
	{
		bool fieldFound = false;
		const TFieldList &fields = baseExpression->getType().getStruct()->fields();
		if(fields.empty())
		{
			error(dotLocation, "structure has no fields", "Internal Error");
			recover();
			indexedExpression = baseExpression;
		}
		else
		{
			unsigned int i;
			for(i = 0; i < fields.size(); ++i)
			{
				if(fields[i]->name() == fieldString)
				{
					fieldFound = true;
					break;
				}
			}
			if(fieldFound)
			{
				if(baseExpression->getType().getQualifier() == EvqConstExpr)
				{
					indexedExpression = addConstStruct(fieldString, baseExpression, dotLocation);
					if(indexedExpression == 0)
					{
						recover();
						indexedExpression = baseExpression;
					}
					else
					{
						indexedExpression->setType(*fields[i]->type());
						// change the qualifier of the return type, not of the structure field
						// as the structure definition is shared between various structures.
						indexedExpression->getTypePointer()->setQualifier(EvqConstExpr);
					}
				}
				else
				{
					ConstantUnion *unionArray = new ConstantUnion[1];
					unionArray->setIConst(i);
					TIntermTyped *index = intermediate.addConstantUnion(unionArray, *fields[i]->type(), fieldLocation);
					indexedExpression = intermediate.addIndex(EOpIndexDirectStruct, baseExpression, index, dotLocation);
					indexedExpression->setType(*fields[i]->type());
				}
			}
			else
			{
				error(dotLocation, " no such field in structure", fieldString.c_str());
				recover();
				indexedExpression = baseExpression;
			}
		}
	}
	else if(baseExpression->isInterfaceBlock())
	{
		bool fieldFound = false;
		const TFieldList &fields = baseExpression->getType().getInterfaceBlock()->fields();
		if(fields.empty())
		{
			error(dotLocation, "interface block has no fields", "Internal Error");
			recover();
			indexedExpression = baseExpression;
		}
		else
		{
			unsigned int i;
			for(i = 0; i < fields.size(); ++i)
			{
				if(fields[i]->name() == fieldString)
				{
					fieldFound = true;
					break;
				}
			}
			if(fieldFound)
			{
				ConstantUnion *unionArray = new ConstantUnion[1];
				unionArray->setIConst(i);
				TIntermTyped *index = intermediate.addConstantUnion(unionArray, *fields[i]->type(), fieldLocation);
				indexedExpression = intermediate.addIndex(EOpIndexDirectInterfaceBlock, baseExpression, index,
					dotLocation);
				indexedExpression->setType(*fields[i]->type());
			}
			else
			{
				error(dotLocation, " no such field in interface block", fieldString.c_str());
				recover();
				indexedExpression = baseExpression;
			}
		}
	}
	else
	{
		if(mShaderVersion < 300)
		{
			error(dotLocation, " field selection requires structure, vector, or matrix on left hand side",
				fieldString.c_str());
		}
		else
		{
			error(dotLocation,
				" field selection requires structure, vector, matrix, or interface block on left hand side",
				fieldString.c_str());
		}
		recover();
		indexedExpression = baseExpression;
	}

	return indexedExpression;
}

TLayoutQualifier TParseContext::parseLayoutQualifier(const TString &qualifierType, const TSourceLoc& qualifierTypeLine)
{
    TLayoutQualifier qualifier;

    qualifier.location = -1;
	qualifier.matrixPacking = EmpUnspecified;
	qualifier.blockStorage = EbsUnspecified;

	if(qualifierType == "shared")
	{
		qualifier.blockStorage = EbsShared;
	}
	else if(qualifierType == "packed")
	{
		qualifier.blockStorage = EbsPacked;
	}
	else if(qualifierType == "std140")
	{
		qualifier.blockStorage = EbsStd140;
	}
	else if(qualifierType == "row_major")
	{
		qualifier.matrixPacking = EmpRowMajor;
	}
	else if(qualifierType == "column_major")
	{
		qualifier.matrixPacking = EmpColumnMajor;
	}
	else if(qualifierType == "location")
    {
        error(qualifierTypeLine, "invalid layout qualifier", qualifierType.c_str(), "location requires an argument");
        recover();
    }
    else
    {
        error(qualifierTypeLine, "invalid layout qualifier", qualifierType.c_str());
        recover();
    }

    return qualifier;
}

TLayoutQualifier TParseContext::parseLayoutQualifier(const TString &qualifierType, const TSourceLoc& qualifierTypeLine, const TString &intValueString, int intValue, const TSourceLoc& intValueLine)
{
    TLayoutQualifier qualifier;

    qualifier.location = -1;
    qualifier.matrixPacking = EmpUnspecified;
    qualifier.blockStorage = EbsUnspecified;

    if (qualifierType != "location")
    {
        error(qualifierTypeLine, "invalid layout qualifier", qualifierType.c_str(), "only location may have arguments");
        recover();
    }
    else
    {
        // must check that location is non-negative
        if (intValue < 0)
        {
            error(intValueLine, "out of range:", intValueString.c_str(), "location must be non-negative");
            recover();
        }
        else
        {
            qualifier.location = intValue;
        }
    }

    return qualifier;
}

TLayoutQualifier TParseContext::joinLayoutQualifiers(TLayoutQualifier leftQualifier, TLayoutQualifier rightQualifier)
{
    TLayoutQualifier joinedQualifier = leftQualifier;

    if (rightQualifier.location != -1)
    {
        joinedQualifier.location = rightQualifier.location;
    }
	if(rightQualifier.matrixPacking != EmpUnspecified)
	{
		joinedQualifier.matrixPacking = rightQualifier.matrixPacking;
	}
	if(rightQualifier.blockStorage != EbsUnspecified)
	{
		joinedQualifier.blockStorage = rightQualifier.blockStorage;
	}

    return joinedQualifier;
}


TPublicType TParseContext::joinInterpolationQualifiers(const TSourceLoc &interpolationLoc, TQualifier interpolationQualifier,
	const TSourceLoc &storageLoc, TQualifier storageQualifier)
{
	TQualifier mergedQualifier = EvqSmoothIn;

	if(storageQualifier == EvqFragmentIn) {
		if(interpolationQualifier == EvqSmooth)
			mergedQualifier = EvqSmoothIn;
		else if(interpolationQualifier == EvqFlat)
			mergedQualifier = EvqFlatIn;
		else UNREACHABLE(interpolationQualifier);
	}
	else if(storageQualifier == EvqCentroidIn) {
		if(interpolationQualifier == EvqSmooth)
			mergedQualifier = EvqCentroidIn;
		else if(interpolationQualifier == EvqFlat)
			mergedQualifier = EvqFlatIn;
		else UNREACHABLE(interpolationQualifier);
	}
	else if(storageQualifier == EvqVertexOut) {
		if(interpolationQualifier == EvqSmooth)
			mergedQualifier = EvqSmoothOut;
		else if(interpolationQualifier == EvqFlat)
			mergedQualifier = EvqFlatOut;
		else UNREACHABLE(interpolationQualifier);
	}
	else if(storageQualifier == EvqCentroidOut) {
		if(interpolationQualifier == EvqSmooth)
			mergedQualifier = EvqCentroidOut;
		else if(interpolationQualifier == EvqFlat)
			mergedQualifier = EvqFlatOut;
		else UNREACHABLE(interpolationQualifier);
	}
	else {
		error(interpolationLoc, "interpolation qualifier requires a fragment 'in' or vertex 'out' storage qualifier", getQualifierString(interpolationQualifier));
		recover();

		mergedQualifier = storageQualifier;
	}

	TPublicType type;
	type.setBasic(EbtVoid, mergedQualifier, storageLoc);
	return type;
}

TFieldList *TParseContext::addStructDeclaratorList(const TPublicType &typeSpecifier, TFieldList *fieldList)
{
	if(voidErrorCheck(typeSpecifier.line, (*fieldList)[0]->name(), typeSpecifier.type))
	{
		recover();
	}

	for(unsigned int i = 0; i < fieldList->size(); ++i)
	{
		//
		// Careful not to replace already known aspects of type, like array-ness
		//
		TType *type = (*fieldList)[i]->type();
		type->setBasicType(typeSpecifier.type);
		type->setNominalSize(typeSpecifier.primarySize);
		type->setSecondarySize(typeSpecifier.secondarySize);
		type->setPrecision(typeSpecifier.precision);
		type->setQualifier(typeSpecifier.qualifier);
		type->setLayoutQualifier(typeSpecifier.layoutQualifier);

		// don't allow arrays of arrays
		if(type->isArray())
		{
			if(arrayTypeErrorCheck(typeSpecifier.line, typeSpecifier))
				recover();
		}
		if(typeSpecifier.array)
			type->setArraySize(typeSpecifier.arraySize);
		if(typeSpecifier.userDef)
		{
			type->setStruct(typeSpecifier.userDef->getStruct());
		}

		if(structNestingErrorCheck(typeSpecifier.line, *(*fieldList)[i]))
		{
			recover();
		}
	}

	return fieldList;
}

TPublicType TParseContext::addStructure(const TSourceLoc &structLine, const TSourceLoc &nameLine,
	const TString *structName, TFieldList *fieldList)
{
	TStructure *structure = new TStructure(structName, fieldList);
	TType *structureType = new TType(structure);

	// Store a bool in the struct if we're at global scope, to allow us to
	// skip the local struct scoping workaround in HLSL.
	structure->setUniqueId(TSymbolTableLevel::nextUniqueId());
	structure->setAtGlobalScope(symbolTable.atGlobalLevel());

	if(!structName->empty())
	{
		if(reservedErrorCheck(nameLine, *structName))
		{
			recover();
		}
		TVariable *userTypeDef = new TVariable(structName, *structureType, true);
		if(!symbolTable.declare(*userTypeDef))
		{
			error(nameLine, "redefinition", structName->c_str(), "struct");
			recover();
		}
	}

	// ensure we do not specify any storage qualifiers on the struct members
	for(unsigned int typeListIndex = 0; typeListIndex < fieldList->size(); typeListIndex++)
	{
		const TField &field = *(*fieldList)[typeListIndex];
		const TQualifier qualifier = field.type()->getQualifier();
		switch(qualifier)
		{
		case EvqGlobal:
		case EvqTemporary:
			break;
		default:
			error(field.line(), "invalid qualifier on struct member", getQualifierString(qualifier));
			recover();
			break;
		}
	}

	TPublicType publicType;
	publicType.setBasic(EbtStruct, EvqTemporary, structLine);
	publicType.userDef = structureType;
	exitStructDeclaration();

	return publicType;
}

bool TParseContext::enterStructDeclaration(const TSourceLoc &line, const TString& identifier)
{
    ++mStructNestingLevel;

    // Embedded structure definitions are not supported per GLSL ES spec.
    // They aren't allowed in GLSL either, but we need to detect this here
    // so we don't rely on the GLSL compiler to catch it.
    if (mStructNestingLevel > 1) {
        error(line, "", "Embedded struct definitions are not allowed");
        return true;
    }

    return false;
}

void TParseContext::exitStructDeclaration()
{
    --mStructNestingLevel;
}

bool TParseContext::structNestingErrorCheck(const TSourceLoc &line, const TField &field)
{
	static const int kWebGLMaxStructNesting = 4;

	if(field.type()->getBasicType() != EbtStruct)
	{
		return false;
	}

	// We're already inside a structure definition at this point, so add
	// one to the field's struct nesting.
	if(1 + field.type()->getDeepestStructNesting() > kWebGLMaxStructNesting)
	{
		std::stringstream reasonStream;
		reasonStream << "Reference of struct type "
			<< field.type()->getStruct()->name().c_str()
			<< " exceeds maximum allowed nesting level of "
			<< kWebGLMaxStructNesting;
		std::string reason = reasonStream.str();
		error(line, reason.c_str(), field.name().c_str(), "");
		return true;
	}

	return false;
}

TIntermTyped *TParseContext::createUnaryMath(TOperator op, TIntermTyped *child, const TSourceLoc &loc, const TType *funcReturnType)
{
	if(child == nullptr)
	{
		return nullptr;
	}

	switch(op)
	{
	case EOpLogicalNot:
		if(child->getBasicType() != EbtBool ||
			child->isMatrix() ||
			child->isArray() ||
			child->isVector())
		{
			return nullptr;
		}
		break;
	case EOpBitwiseNot:
		if((child->getBasicType() != EbtInt && child->getBasicType() != EbtUInt) ||
			child->isMatrix() ||
			child->isArray())
		{
			return nullptr;
		}
		break;
	case EOpPostIncrement:
	case EOpPreIncrement:
	case EOpPostDecrement:
	case EOpPreDecrement:
	case EOpNegative:
		if(child->getBasicType() == EbtStruct ||
			child->getBasicType() == EbtBool ||
			child->isArray())
		{
			return nullptr;
		}
		// Operators for built-ins are already type checked against their prototype.
	default:
		break;
	}

	return intermediate.addUnaryMath(op, child, loc, funcReturnType);
}

TIntermTyped *TParseContext::addUnaryMath(TOperator op, TIntermTyped *child, const TSourceLoc &loc)
{
	TIntermTyped *node = createUnaryMath(op, child, loc, nullptr);
	if(node == nullptr)
	{
		unaryOpError(loc, getOperatorString(op), child->getCompleteString());
		recover();
		return child;
	}
	return node;
}

TIntermTyped *TParseContext::addUnaryMathLValue(TOperator op, TIntermTyped *child, const TSourceLoc &loc)
{
	if(lValueErrorCheck(loc, getOperatorString(op), child))
		recover();
	return addUnaryMath(op, child, loc);
}

bool TParseContext::binaryOpCommonCheck(TOperator op, TIntermTyped *left, TIntermTyped *right, const TSourceLoc &loc)
{
	if(left->isArray() || right->isArray())
	{
		if(mShaderVersion < 300)
		{
			error(loc, "Invalid operation for arrays", getOperatorString(op));
			return false;
		}

		if(left->isArray() != right->isArray())
		{
			error(loc, "array / non-array mismatch", getOperatorString(op));
			return false;
		}

		switch(op)
		{
		case EOpEqual:
		case EOpNotEqual:
		case EOpAssign:
		case EOpInitialize:
			break;
		default:
			error(loc, "Invalid operation for arrays", getOperatorString(op));
			return false;
		}
		// At this point, size of implicitly sized arrays should be resolved.
		if(left->getArraySize() != right->getArraySize())
		{
			error(loc, "array size mismatch", getOperatorString(op));
			return false;
		}
	}

	// Check ops which require integer / ivec parameters
	bool isBitShift = false;
	switch(op)
	{
	case EOpBitShiftLeft:
	case EOpBitShiftRight:
	case EOpBitShiftLeftAssign:
	case EOpBitShiftRightAssign:
		// Unsigned can be bit-shifted by signed and vice versa, but we need to
		// check that the basic type is an integer type.
		isBitShift = true;
		if(!IsInteger(left->getBasicType()) || !IsInteger(right->getBasicType()))
		{
			return false;
		}
		break;
	case EOpBitwiseAnd:
	case EOpBitwiseXor:
	case EOpBitwiseOr:
	case EOpBitwiseAndAssign:
	case EOpBitwiseXorAssign:
	case EOpBitwiseOrAssign:
		// It is enough to check the type of only one operand, since later it
		// is checked that the operand types match.
		if(!IsInteger(left->getBasicType()))
		{
			return false;
		}
		break;
	default:
		break;
	}

	// GLSL ES 1.00 and 3.00 do not support implicit type casting.
	// So the basic type should usually match.
	if(!isBitShift && left->getBasicType() != right->getBasicType())
	{
		return false;
	}

	// Check that type sizes match exactly on ops that require that.
	// Also check restrictions for structs that contain arrays or samplers.
	switch(op)
	{
	case EOpAssign:
	case EOpInitialize:
	case EOpEqual:
	case EOpNotEqual:
		// ESSL 1.00 sections 5.7, 5.8, 5.9
		if(mShaderVersion < 300 && left->getType().isStructureContainingArrays())
		{
			error(loc, "undefined operation for structs containing arrays", getOperatorString(op));
			return false;
		}
		// Samplers as l-values are disallowed also in ESSL 3.00, see section 4.1.7,
		// we interpret the spec so that this extends to structs containing samplers,
		// similarly to ESSL 1.00 spec.
		if((mShaderVersion < 300 || op == EOpAssign || op == EOpInitialize) &&
			left->getType().isStructureContainingSamplers())
		{
			error(loc, "undefined operation for structs containing samplers", getOperatorString(op));
			return false;
		}
	case EOpLessThan:
	case EOpGreaterThan:
	case EOpLessThanEqual:
	case EOpGreaterThanEqual:
		if((left->getNominalSize() != right->getNominalSize()) ||
			(left->getSecondarySize() != right->getSecondarySize()))
		{
			return false;
		}
	default:
		break;
	}

	return true;
}

TIntermSwitch *TParseContext::addSwitch(TIntermTyped *init, TIntermAggregate *statementList, const TSourceLoc &loc)
{
	TBasicType switchType = init->getBasicType();
	if((switchType != EbtInt && switchType != EbtUInt) ||
	   init->isMatrix() ||
	   init->isArray() ||
	   init->isVector())
	{
		error(init->getLine(), "init-expression in a switch statement must be a scalar integer", "switch");
		recover();
		return nullptr;
	}

	if(statementList)
	{
		if(!ValidateSwitch::validate(switchType, this, statementList, loc))
		{
			recover();
			return nullptr;
		}
	}

	TIntermSwitch *node = intermediate.addSwitch(init, statementList, loc);
	if(node == nullptr)
	{
		error(loc, "erroneous switch statement", "switch");
		recover();
		return nullptr;
	}
	return node;
}

TIntermCase *TParseContext::addCase(TIntermTyped *condition, const TSourceLoc &loc)
{
	if(mSwitchNestingLevel == 0)
	{
		error(loc, "case labels need to be inside switch statements", "case");
		recover();
		return nullptr;
	}
	if(condition == nullptr)
	{
		error(loc, "case label must have a condition", "case");
		recover();
		return nullptr;
	}
	if((condition->getBasicType() != EbtInt && condition->getBasicType() != EbtUInt) ||
	   condition->isMatrix() ||
	   condition->isArray() ||
	   condition->isVector())
	{
		error(condition->getLine(), "case label must be a scalar integer", "case");
		recover();
	}
	TIntermConstantUnion *conditionConst = condition->getAsConstantUnion();
	if(conditionConst == nullptr)
	{
		error(condition->getLine(), "case label must be constant", "case");
		recover();
	}
	TIntermCase *node = intermediate.addCase(condition, loc);
	if(node == nullptr)
	{
		error(loc, "erroneous case statement", "case");
		recover();
		return nullptr;
	}
	return node;
}

TIntermCase *TParseContext::addDefault(const TSourceLoc &loc)
{
	if(mSwitchNestingLevel == 0)
	{
		error(loc, "default labels need to be inside switch statements", "default");
		recover();
		return nullptr;
	}
	TIntermCase *node = intermediate.addCase(nullptr, loc);
	if(node == nullptr)
	{
		error(loc, "erroneous default statement", "default");
		recover();
		return nullptr;
	}
	return node;
}
TIntermTyped *TParseContext::createAssign(TOperator op, TIntermTyped *left, TIntermTyped *right, const TSourceLoc &loc)
{
	if(binaryOpCommonCheck(op, left, right, loc))
	{
		return intermediate.addAssign(op, left, right, loc);
	}
	return nullptr;
}

TIntermTyped *TParseContext::addAssign(TOperator op, TIntermTyped *left, TIntermTyped *right, const TSourceLoc &loc)
{
	TIntermTyped *node = createAssign(op, left, right, loc);
	if(node == nullptr)
	{
		assignError(loc, "assign", left->getCompleteString(), right->getCompleteString());
		recover();
		return left;
	}
	return node;
}

TIntermTyped *TParseContext::addBinaryMathInternal(TOperator op, TIntermTyped *left, TIntermTyped *right,
	const TSourceLoc &loc)
{
	if(!binaryOpCommonCheck(op, left, right, loc))
		return nullptr;

	switch(op)
	{
	case EOpEqual:
	case EOpNotEqual:
		break;
	case EOpLessThan:
	case EOpGreaterThan:
	case EOpLessThanEqual:
	case EOpGreaterThanEqual:
		ASSERT(!left->isArray() && !right->isArray());
		if(left->isMatrix() || left->isVector() ||
			left->getBasicType() == EbtStruct)
		{
			return nullptr;
		}
		break;
	case EOpLogicalOr:
	case EOpLogicalXor:
	case EOpLogicalAnd:
		ASSERT(!left->isArray() && !right->isArray());
		if(left->getBasicType() != EbtBool ||
			left->isMatrix() || left->isVector())
		{
			return nullptr;
		}
		break;
	case EOpAdd:
	case EOpSub:
	case EOpDiv:
	case EOpMul:
		ASSERT(!left->isArray() && !right->isArray());
		if(left->getBasicType() == EbtStruct || left->getBasicType() == EbtBool)
		{
			return nullptr;
		}
		break;
	case EOpIMod:
		ASSERT(!left->isArray() && !right->isArray());
		// Note that this is only for the % operator, not for mod()
		if(left->getBasicType() == EbtStruct || left->getBasicType() == EbtBool || left->getBasicType() == EbtFloat)
		{
			return nullptr;
		}
		break;
		// Note that for bitwise ops, type checking is done in promote() to
		// share code between ops and compound assignment
	default:
		break;
	}

	return intermediate.addBinaryMath(op, left, right, loc);
}

TIntermTyped *TParseContext::addBinaryMath(TOperator op, TIntermTyped *left, TIntermTyped *right, const TSourceLoc &loc)
{
	TIntermTyped *node = addBinaryMathInternal(op, left, right, loc);
	if(node == 0)
	{
		binaryOpError(loc, getOperatorString(op), left->getCompleteString(), right->getCompleteString());
		recover();
		return left;
	}
	return node;
}

TIntermTyped *TParseContext::addBinaryMathBooleanResult(TOperator op, TIntermTyped *left, TIntermTyped *right, const TSourceLoc &loc)
{
	TIntermTyped *node = addBinaryMathInternal(op, left, right, loc);
	if(node == 0)
	{
		binaryOpError(loc, getOperatorString(op), left->getCompleteString(), right->getCompleteString());
		recover();
		ConstantUnion *unionArray = new ConstantUnion[1];
		unionArray->setBConst(false);
		return intermediate.addConstantUnion(unionArray, TType(EbtBool, EbpUndefined, EvqConstExpr), loc);
	}
	return node;
}

TIntermBranch *TParseContext::addBranch(TOperator op, const TSourceLoc &loc)
{
	switch(op)
	{
	case EOpContinue:
		if(mLoopNestingLevel <= 0)
		{
			error(loc, "continue statement only allowed in loops", "");
			recover();
		}
		break;
	case EOpBreak:
		if(mLoopNestingLevel <= 0 && mSwitchNestingLevel <= 0)
		{
			error(loc, "break statement only allowed in loops and switch statements", "");
			recover();
		}
		break;
	case EOpReturn:
		if(mCurrentFunctionType->getBasicType() != EbtVoid)
		{
			error(loc, "non-void function must return a value", "return");
			recover();
		}
		break;
	default:
		// No checks for discard
		break;
	}
	return intermediate.addBranch(op, loc);
}

TIntermBranch *TParseContext::addBranch(TOperator op, TIntermTyped *returnValue, const TSourceLoc &loc)
{
	ASSERT(op == EOpReturn);
	mFunctionReturnsValue = true;
	if(mCurrentFunctionType->getBasicType() == EbtVoid)
	{
		error(loc, "void function cannot return a value", "return");
		recover();
	}
	else if(*mCurrentFunctionType != returnValue->getType())
	{
		error(loc, "function return is not matching type:", "return");
		recover();
	}
	return intermediate.addBranch(op, returnValue, loc);
}

TIntermTyped *TParseContext::addFunctionCallOrMethod(TFunction *fnCall, TIntermNode *paramNode, TIntermNode *thisNode, const TSourceLoc &loc, bool *fatalError)
{
	*fatalError = false;
	TOperator op = fnCall->getBuiltInOp();
	TIntermTyped *callNode = nullptr;

	if(thisNode != nullptr)
	{
		ConstantUnion *unionArray = new ConstantUnion[1];
		int arraySize = 0;
		TIntermTyped *typedThis = thisNode->getAsTyped();
		if(fnCall->getName() != "length")
		{
			error(loc, "invalid method", fnCall->getName().c_str());
			recover();
		}
		else if(paramNode != nullptr)
		{
			error(loc, "method takes no parameters", "length");
			recover();
		}
		else if(typedThis == nullptr || !typedThis->isArray())
		{
			error(loc, "length can only be called on arrays", "length");
			recover();
		}
		else
		{
			arraySize = typedThis->getArraySize();
			if(typedThis->getAsSymbolNode() == nullptr)
			{
				// This code path can be hit with expressions like these:
				// (a = b).length()
				// (func()).length()
				// (int[3](0, 1, 2)).length()
				// ESSL 3.00 section 5.9 defines expressions so that this is not actually a valid expression.
				// It allows "An array name with the length method applied" in contrast to GLSL 4.4 spec section 5.9
				// which allows "An array, vector or matrix expression with the length method applied".
				error(loc, "length can only be called on array names, not on array expressions", "length");
				recover();
			}
		}
		unionArray->setIConst(arraySize);
		callNode = intermediate.addConstantUnion(unionArray, TType(EbtInt, EbpUndefined, EvqConstExpr), loc);
	}
	else if(op != EOpNull)
	{
		//
		// Then this should be a constructor.
		// Don't go through the symbol table for constructors.
		// Their parameters will be verified algorithmically.
		//
		TType type(EbtVoid, EbpUndefined);  // use this to get the type back
		if(!constructorErrorCheck(loc, paramNode, *fnCall, op, &type))
		{
			//
			// It's a constructor, of type 'type'.
			//
			callNode = addConstructor(paramNode, &type, op, fnCall, loc);
		}

		if(callNode == nullptr)
		{
			recover();
			callNode = intermediate.setAggregateOperator(nullptr, op, loc);
		}
	}
	else
	{
		//
		// Not a constructor.  Find it in the symbol table.
		//
		const TFunction *fnCandidate;
		bool builtIn;
		fnCandidate = findFunction(loc, fnCall, &builtIn);
		if(fnCandidate)
		{
			//
			// A declared function.
			//
			if(builtIn && !fnCandidate->getExtension().empty() &&
				extensionErrorCheck(loc, fnCandidate->getExtension()))
			{
				recover();
			}
			op = fnCandidate->getBuiltInOp();
			if(builtIn && op != EOpNull)
			{
				//
				// A function call mapped to a built-in operation.
				//
				if(fnCandidate->getParamCount() == 1)
				{
					//
					// Treat it like a built-in unary operator.
					//
					callNode = createUnaryMath(op, paramNode->getAsTyped(), loc, &fnCandidate->getReturnType());
					if(callNode == nullptr)
					{
						std::stringstream extraInfoStream;
						extraInfoStream << "built in unary operator function.  Type: "
							<< static_cast<TIntermTyped*>(paramNode)->getCompleteString();
						std::string extraInfo = extraInfoStream.str();
						error(paramNode->getLine(), " wrong operand type", "Internal Error", extraInfo.c_str());
						*fatalError = true;
						return nullptr;
					}
				}
				else
				{
					TIntermAggregate *aggregate = intermediate.setAggregateOperator(paramNode, op, loc);
					aggregate->setType(fnCandidate->getReturnType());

					// Some built-in functions have out parameters too.
					functionCallLValueErrorCheck(fnCandidate, aggregate);

					callNode = aggregate;

					if(fnCandidate->getParamCount() == 2)
					{
						TIntermSequence &parameters = paramNode->getAsAggregate()->getSequence();
						TIntermTyped *left = parameters[0]->getAsTyped();
						TIntermTyped *right = parameters[1]->getAsTyped();

						TIntermConstantUnion *leftTempConstant = left->getAsConstantUnion();
						TIntermConstantUnion *rightTempConstant = right->getAsConstantUnion();
						if (leftTempConstant && rightTempConstant)
						{
							TIntermTyped *typedReturnNode = leftTempConstant->fold(op, rightTempConstant, infoSink());

							if(typedReturnNode)
							{
								callNode = typedReturnNode;
							}
						}
					}
				}
			}
			else
			{
				// This is a real function call

				TIntermAggregate *aggregate = intermediate.setAggregateOperator(paramNode, EOpFunctionCall, loc);
				aggregate->setType(fnCandidate->getReturnType());

				// this is how we know whether the given function is a builtIn function or a user defined function
				// if builtIn == false, it's a userDefined -> could be an overloaded builtIn function also
				// if builtIn == true, it's definitely a builtIn function with EOpNull
				if(!builtIn)
					aggregate->setUserDefined();
				aggregate->setName(fnCandidate->getMangledName());

				callNode = aggregate;

				functionCallLValueErrorCheck(fnCandidate, aggregate);
			}
		}
		else
		{
			// error message was put out by findFunction()
			// Put on a dummy node for error recovery
			ConstantUnion *unionArray = new ConstantUnion[1];
			unionArray->setFConst(0.0f);
			callNode = intermediate.addConstantUnion(unionArray, TType(EbtFloat, EbpUndefined, EvqConstExpr), loc);
			recover();
		}
	}
	delete fnCall;
	return callNode;
}

TIntermTyped *TParseContext::addTernarySelection(TIntermTyped *cond, TIntermTyped *trueBlock, TIntermTyped *falseBlock, const TSourceLoc &loc)
{
	if(boolErrorCheck(loc, cond))
		recover();

	if(trueBlock->getType() != falseBlock->getType())
	{
		binaryOpError(loc, ":", trueBlock->getCompleteString(), falseBlock->getCompleteString());
		recover();
		return falseBlock;
	}
	// ESSL1 sections 5.2 and 5.7:
	// ESSL3 section 5.7:
	// Ternary operator is not among the operators allowed for structures/arrays.
	if(trueBlock->isArray() || trueBlock->getBasicType() == EbtStruct)
	{
		error(loc, "ternary operator is not allowed for structures or arrays", ":");
		recover();
		return falseBlock;
	}
	return intermediate.addSelection(cond, trueBlock, falseBlock, loc);
}

//
// Parse an array of strings using yyparse.
//
// Returns 0 for success.
//
int PaParseStrings(int count, const char* const string[], const int length[],
                   TParseContext* context) {
    if ((count == 0) || (string == NULL))
        return 1;

    if (glslang_initialize(context))
        return 1;

    int error = glslang_scan(count, string, length, context);
    if (!error)
        error = glslang_parse(context);

    glslang_finalize(context);

    return (error == 0) && (context->numErrors() == 0) ? 0 : 1;
}



