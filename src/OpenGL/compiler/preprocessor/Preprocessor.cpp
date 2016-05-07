// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Preprocessor.h"

#include <cassert>
#include <sstream>

#include "Diagnostics.h"
#include "DirectiveParser.h"
#include "Macro.h"
#include "MacroExpander.h"
#include "Token.h"
#include "Tokenizer.h"

namespace pp
{

struct PreprocessorImpl
{
	Diagnostics* diagnostics;
	MacroSet macroSet;
	Tokenizer tokenizer;
	DirectiveParser directiveParser;
	MacroExpander macroExpander;

	PreprocessorImpl(Diagnostics* diag, DirectiveHandler* directiveHandler) :
		diagnostics(diag),
		tokenizer(diag),
		directiveParser(&tokenizer, &macroSet, diag, directiveHandler),
		macroExpander(&directiveParser, &macroSet, diag, false)
	{
	}
};

Preprocessor::Preprocessor(Diagnostics* diagnostics,
                           DirectiveHandler* directiveHandler)
{
	mImpl = new PreprocessorImpl(diagnostics, directiveHandler);
}

Preprocessor::~Preprocessor()
{
	delete mImpl;
}

bool Preprocessor::init(int count,
                        const char* const string[],
                        const int length[])
{
	static const int kGLSLVersion = 100;

	// Add standard pre-defined macros.
	predefineMacro("__LINE__", 0);
	predefineMacro("__FILE__", 0);
	predefineMacro("__VERSION__", kGLSLVersion);
	predefineMacro("GL_ES", 1);

	return mImpl->tokenizer.init(count, string, length);
}

void Preprocessor::predefineMacro(const char* name, int value)
{
	std::ostringstream stream;
	stream << value;

	Token token;
	token.type = Token::CONST_INT;
	token.text = stream.str();

	Macro macro;
	macro.predefined = true;
	macro.type = Macro::kTypeObj;
	macro.name = name;
	macro.replacements.push_back(token);

	mImpl->macroSet[name] = macro;
}

void Preprocessor::lex(Token* token)
{
	bool validToken = false;
	while (!validToken)
	{
		mImpl->macroExpander.lex(token);
		switch (token->type)
		{
		// We should not be returning internal preprocessing tokens.
		// Convert preprocessing tokens to compiler tokens or report
		// diagnostics.
		case Token::PP_HASH:
			assert(false);
			break;
		case Token::CONST_INT:
		  {
			int val = 0;
			if (!token->iValue(&val))
			{
				// Do not mark the token as invalid.
				// Just emit the diagnostic and reset value to 0.
				mImpl->diagnostics->report(Diagnostics::INTEGER_OVERFLOW,
				                           token->location, token->text);
				token->text.assign("0");
			}
			validToken = true;
			break;
		  }
		case Token::CONST_FLOAT:
		  {
			float val = 0;
			if (!token->fValue(&val))
			{
				// Do not mark the token as invalid.
				// Just emit the diagnostic and reset value to 0.0.
				mImpl->diagnostics->report(Diagnostics::FLOAT_OVERFLOW,
				                           token->location, token->text);
				token->text.assign("0.0");
			}
			validToken = true;
			break;
		  }
		case Token::PP_NUMBER:
			mImpl->diagnostics->report(Diagnostics::INVALID_NUMBER,
			                           token->location, token->text);
			break;
		case Token::PP_OTHER:
			mImpl->diagnostics->report(Diagnostics::INVALID_CHARACTER,
			                           token->location, token->text);
			break;
		default:
			validToken = true;
			break;
		}
	}
}

}  // namespace pp

