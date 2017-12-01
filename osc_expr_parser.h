/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
# define YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int osc_expr_parser_debug;
#endif
/* "%code requires" blocks.  */
#line 64 "osc_expr_parser.y" /* yacc.c:1915  */

#include "osc.h"
#include "osc_mem.h"
#include "osc_atom_u.h"
#include "osc_expr.h"

#ifdef YY_DECL
#undef YY_DECL
#endif

#ifdef __cplusplus
#define YY_DECL extern "C" int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started)
#else
#define YY_DECL int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started)
#endif
	//t_osc_err osc_expr_parser_parseString(char *ptr, t_osc_expr **f);
#ifdef __cplusplus
extern "C"{
#endif
t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f);
t_osc_err osc_expr_parser_parseFunction(char *ptr, t_osc_expr_rec **f);
#ifdef __cplusplus
}
#endif

#line 70 "osc_expr_parser.h" /* yacc.c:1915  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    OSC_EXPR_NUM = 258,
    OSC_EXPR_STRING = 259,
    OSC_EXPR_OSCADDRESS = 260,
    OSC_EXPR_LAMBDA = 261,
    OSC_EXPR_PLUSEQ = 262,
    OSC_EXPR_MINUSEQ = 263,
    OSC_EXPR_MULTEQ = 264,
    OSC_EXPR_DIVEQ = 265,
    OSC_EXPR_MODEQ = 266,
    OSC_EXPR_POWEQ = 267,
    OSC_EXPR_TERNARY_COND = 268,
    OSC_EXPR_OROR = 269,
    OSC_EXPR_ANDAND = 270,
    OSC_EXPR_EQ = 271,
    OSC_EXPR_NEQ = 272,
    OSC_EXPR_LTE = 273,
    OSC_EXPR_GTE = 274,
    OSC_EXPR_PREFIX_INC = 275,
    OSC_EXPR_PREFIX_DEC = 276,
    OSC_EXPR_UPLUS = 277,
    OSC_EXPR_UMINUS = 278,
    OSC_EXPR_DBLQMARK = 279,
    OSC_EXPR_DBLQMARKEQ = 280,
    OSC_EXPR_INC = 281,
    OSC_EXPR_DEC = 282,
    OSC_EXPR_FUNC_CALL = 283,
    OSC_EXPR_QUOTED_EXPR = 284,
    OPEN_DBL_BRKTS = 285,
    CLOSE_DBL_BRKTS = 286,
    START_EXPNS = 287,
    START_FUNCTION = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 547 "osc_expr_parser.y" /* yacc.c:1915  */

	t_osc_atom_u *atom;
	t_osc_expr *expr;
	t_osc_expr_rec *func;
	t_osc_expr_arg *arg;

#line 123 "osc_expr_parser.h" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int osc_expr_parser_parse (t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started);

#endif /* !YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED  */
