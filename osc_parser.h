/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
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

#ifndef YY_OSC_PARSER_OSC_PARSER_H_INCLUDED
# define YY_OSC_PARSER_OSC_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int osc_parser_debug;
#endif
/* "%code requires" blocks.  */
/* Line 2060 of yacc.c  */
#line 65 "osc_parser.y"

#include "osc.h"
#include "osc_error.h"
#include "osc_message_u.h"

#ifdef YY_DECL
#undef YY_DECL
#endif

#ifdef __cplusplus
#define YY_DECL extern "C" int osc_scanner_lex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner, long *buflen, char **buf)
#else
#define YY_DECL int osc_scanner_lex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner, long *buflen, char **buf)
#endif

#ifndef __HAVE_OSC_BNDL_LIST__
#define __HAVE_OSC_BNDL_LIST__
typedef struct _osc_parser_bndl_list{
	struct _osc_bundle_u *bndl;
	struct _osc_parser_bndl_list *next;
} t_osc_parser_bndl_list;
#endif

#ifndef __HAVE_OSC_PARSER_PARSESTRING__
#define __HAVE_OSC_PARSER_PARSESTRING__
#ifdef __cplusplus
extern "C"{
#endif
t_osc_err osc_parser_parseString(long len, char *ptr, struct _osc_bundle_u **bndl);
struct _osc_bundle_u *osc_parser_parseString_r(long len, char *ptr);
#ifdef __cplusplus
}
#endif
#endif


/* Line 2060 of yacc.c  */
#line 83 "osc_parser.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OSCADDRESS = 258,
     OSCVALUE = 259
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 166 "osc_parser.y"

	t_osc_bndl_u *bundle;
	t_osc_msg_u *message;
	t_osc_atom_u *atom;


/* Line 2060 of yacc.c  */
#line 109 "osc_parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int osc_parser_parse (void *YYPARSE_PARAM);
#else
int osc_parser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int osc_parser_parse (t_osc_bndl_u **bndl, void *scanner, long *buflen, char **buf);
#else
int osc_parser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_OSC_PARSER_OSC_PARSER_H_INCLUDED  */
