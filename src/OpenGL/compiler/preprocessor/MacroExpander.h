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

#ifndef COMPILER_PREPROCESSOR_MACRO_EXPANDER_H_
#define COMPILER_PREPROCESSOR_MACRO_EXPANDER_H_

#include <cassert>
#include <memory>
#include <vector>

#include "Lexer.h"
#include "Macro.h"
#include "pp_utils.h"

namespace pp
{

class Diagnostics;

class MacroExpander : public Lexer
{
public:
	MacroExpander(Lexer* lexer, MacroSet* macroSet, Diagnostics* diagnostics, bool parseDefined);
	virtual ~MacroExpander();

	virtual void lex(Token* token);

private:
	PP_DISALLOW_COPY_AND_ASSIGN(MacroExpander);

	void getToken(Token* token);
	void ungetToken(const Token& token);
	bool isNextTokenLeftParen();

	bool pushMacro(const Macro& macro, const Token& identifier);
	void popMacro();

	bool expandMacro(const Macro& macro,
	                 const Token& identifier,
	                 std::vector<Token>* replacements);

	typedef std::vector<Token> MacroArg;
	bool collectMacroArgs(const Macro& macro,
	                      const Token& identifier,
	                      std::vector<MacroArg>* args);
	void replaceMacroParams(const Macro& macro,
	                        const std::vector<MacroArg>& args,
	                        std::vector<Token>* replacements);

	struct MacroContext
	{
		const Macro* macro;
		size_t index;
		std::vector<Token> replacements;

		MacroContext() : macro(0), index(0) { }
		bool empty() const { return index == replacements.size(); }
		const Token& get() { return replacements[index++]; }
		void unget() { assert(index > 0); --index; }
	};

	Lexer* mLexer;
	MacroSet* mMacroSet;
	Diagnostics* mDiagnostics;
	const bool mParseDefined;

	std::auto_ptr<Token> mReserveToken;
	std::vector<MacroContext*> mContextStack;
};

}  // namespace pp
#endif  // COMPILER_PREPROCESSOR_MACRO_EXPANDER_H_

