
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
     IDENTIFIER = 313,
     TYPE_NAME = 314,
     FLOATCONSTANT = 315,
     INTCONSTANT = 316,
     UINTCONSTANT = 317,
     BOOLCONSTANT = 318,
     FIELD_SELECTION = 319,
     LEFT_OP = 320,
     RIGHT_OP = 321,
     INC_OP = 322,
     DEC_OP = 323,
     LE_OP = 324,
     GE_OP = 325,
     EQ_OP = 326,
     NE_OP = 327,
     AND_OP = 328,
     OR_OP = 329,
     XOR_OP = 330,
     MUL_ASSIGN = 331,
     DIV_ASSIGN = 332,
     ADD_ASSIGN = 333,
     MOD_ASSIGN = 334,
     LEFT_ASSIGN = 335,
     RIGHT_ASSIGN = 336,
     AND_ASSIGN = 337,
     XOR_ASSIGN = 338,
     OR_ASSIGN = 339,
     SUB_ASSIGN = 340,
     LEFT_PAREN = 341,
     RIGHT_PAREN = 342,
     LEFT_BRACKET = 343,
     RIGHT_BRACKET = 344,
     LEFT_BRACE = 345,
     RIGHT_BRACE = 346,
     DOT = 347,
     COMMA = 348,
     COLON = 349,
     EQUAL = 350,
     SEMICOLON = 351,
     BANG = 352,
     DASH = 353,
     TILDE = 354,
     PLUS = 355,
     STAR = 356,
     SLASH = 357,
     PERCENT = 358,
     LEFT_ANGLE = 359,
     RIGHT_ANGLE = 360,
     VERTICAL_BAR = 361,
     CARET = 362,
     AMPERSAND = 363,
     QUESTION = 364
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




