
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INVARIANT = 258,
     HIGH_PRECISION = 259,
     MEDIUM_PRECISION = 260,
     LOW_PRECISION = 261,
     PRECISION = 262,
     ATTRIBUTE = 263,
     CONST_QUAL = 264,
     BOOL_TYPE = 265,
     FLOAT_TYPE = 266,
     INT_TYPE = 267,
     UINT_TYPE = 268,
     BREAK = 269,
     CONTINUE = 270,
     DO = 271,
     ELSE = 272,
     FOR = 273,
     IF = 274,
     DISCARD = 275,
     RETURN = 276,
     SWITCH = 277,
     CASE = 278,
     DEFAULT = 279,
     BVEC2 = 280,
     BVEC3 = 281,
     BVEC4 = 282,
     IVEC2 = 283,
     IVEC3 = 284,
     IVEC4 = 285,
     VEC2 = 286,
     VEC3 = 287,
     VEC4 = 288,
     UVEC2 = 289,
     UVEC3 = 290,
     UVEC4 = 291,
     MATRIX2 = 292,
     MATRIX3 = 293,
     MATRIX4 = 294,
     IN_QUAL = 295,
     OUT_QUAL = 296,
     INOUT_QUAL = 297,
     UNIFORM = 298,
     VARYING = 299,
     CENTROID = 300,
     FLAT = 301,
     SMOOTH = 302,
     STRUCT = 303,
     VOID_TYPE = 304,
     WHILE = 305,
     SAMPLER2D = 306,
     SAMPLERCUBE = 307,
     SAMPLER_EXTERNAL_OES = 308,
     SAMPLER2DRECT = 309,
     SAMPLER3D = 310,
     SAMPLER3DRECT = 311,
     SAMPLER2DSHADOW = 312,
     LAYOUT = 313,
     IDENTIFIER = 314,
     TYPE_NAME = 315,
     FLOATCONSTANT = 316,
     INTCONSTANT = 317,
     UINTCONSTANT = 318,
     BOOLCONSTANT = 319,
     FIELD_SELECTION = 320,
     LEFT_OP = 321,
     RIGHT_OP = 322,
     INC_OP = 323,
     DEC_OP = 324,
     LE_OP = 325,
     GE_OP = 326,
     EQ_OP = 327,
     NE_OP = 328,
     AND_OP = 329,
     OR_OP = 330,
     XOR_OP = 331,
     MUL_ASSIGN = 332,
     DIV_ASSIGN = 333,
     ADD_ASSIGN = 334,
     MOD_ASSIGN = 335,
     LEFT_ASSIGN = 336,
     RIGHT_ASSIGN = 337,
     AND_ASSIGN = 338,
     XOR_ASSIGN = 339,
     OR_ASSIGN = 340,
     SUB_ASSIGN = 341,
     LEFT_PAREN = 342,
     RIGHT_PAREN = 343,
     LEFT_BRACKET = 344,
     RIGHT_BRACKET = 345,
     LEFT_BRACE = 346,
     RIGHT_BRACE = 347,
     DOT = 348,
     COMMA = 349,
     COLON = 350,
     EQUAL = 351,
     SEMICOLON = 352,
     BANG = 353,
     DASH = 354,
     TILDE = 355,
     PLUS = 356,
     STAR = 357,
     SLASH = 358,
     PERCENT = 359,
     LEFT_ANGLE = 360,
     RIGHT_ANGLE = 361,
     VERTICAL_BAR = 362,
     CARET = 363,
     AMPERSAND = 364,
     QUESTION = 365
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


    struct {
        TSourceLoc line;
        union {
            TString *string;
            float f;
            int i;
            unsigned int u;
            bool b;
        };
        TSymbol* symbol;
    } lex;
    struct {
        TSourceLoc line;
        TOperator op;
        union {
            TIntermNode* intermNode;
            TIntermNodePair nodePair;
            TIntermTyped* intermTypedNode;
            TIntermAggregate* intermAggregate;
        };
        union {
            TPublicType type;
            TPrecision precision;
            TLayoutQualifier layoutQualifier;
            TQualifier qualifier;
            TFunction* function;
            TParameter param;
            TTypeLine typeLine;
            TTypeList* typeList;
        };
    } interm;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




