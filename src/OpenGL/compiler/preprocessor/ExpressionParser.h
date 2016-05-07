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

#ifndef COMPILER_PREPROCESSOR_EXPRESSION_PARSER_H_
#define COMPILER_PREPROCESSOR_EXPRESSION_PARSER_H_

#include "pp_utils.h"

namespace pp
{

class Diagnostics;
class Lexer;
struct Token;

class ExpressionParser
{
public:
	ExpressionParser(Lexer* lexer, Diagnostics* diagnostics);

	bool parse(Token* token, int* result);

private:
	PP_DISALLOW_COPY_AND_ASSIGN(ExpressionParser);

	Lexer* mLexer;
	Diagnostics* mDiagnostics;
};

}  // namespace pp
#endif  // COMPILER_PREPROCESSOR_EXPRESSION_PARSER_H_
