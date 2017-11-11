/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
/* Line 349 of yacc.c  */
#line 27 "osc_expr_parser.y"


#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
	//#ifndef WIN_VERSION
	//#include <Carbon.h>
	//#include <CoreServices.h>
	//#endif
#include "osc_expr.h"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_expr_func.h"
#include "osc_error.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_util.h"
#include "osc_atom_u.r"

	//#define OSC_EXPR_PARSER_DEBUG
#ifdef OSC_EXPR_PARSER_DEBUG
#define PP(fmt, ...)printf(fmt, ##__VA_ARGS__)
#else
#define PP(fmt, ...)
#endif

#ifdef __cplusplus
extern "C" int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started);
#else
int osc_expr_scanner_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started);
#endif



/* Line 349 of yacc.c  */
#line 104 "osc_expr_parser.c"

/* Substitute the variable and function names.  */
#define yyparse         osc_expr_parser_parse
#define yylex           osc_expr_parser_lex
#define yyerror         osc_expr_parser_error
#define yylval          osc_expr_parser_lval
#define yychar          osc_expr_parser_char
#define yydebug         osc_expr_parser_debug
#define yynerrs         osc_expr_parser_nerrs
#define yylloc          osc_expr_parser_lloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 90 "osc_expr_parser.y"


// this is a dummy so that the compiler won't complain.  we pass the hard-coded
// value of 1 to osc_expr_scanner_lex() inside of osc_expr_parser_lex() down below.
int alloc_atom = 1;


int osc_expr_parser_lex(YYSTYPE *yylval_param, YYLTYPE *llocp, yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started){
	return osc_expr_scanner_lex(yylval_param, llocp, yyscanner, 1, buflen, buf, startcond, started);
}

/*
static t_osc_atom_ar_u *osc_expr_parser_foldConstants_impl(t_osc_expr *expr, t_osc_expr_lexenv *lexenv)
{
	printf("%s: %s\n", __func__, osc_expr_rec_getName(osc_expr_getRec(expr)));
	t_osc_expr *e = expr;
	while(e){
		t_osc_expr_arg *a = osc_expr_getArgs(e);
		int eval = 1;
		while(a){
			int type = osc_expr_arg_getType(a);
			switch(type){
			case OSC_EXPR_ARG_TYPE_OSCADDRESS:
				// we can't eval this expression, but we want to continue to see if we can 
				// reduce any of the other args
				printf("address\n");
				eval = 0;
				break;
			case OSC_EXPR_ARG_TYPE_EXPR:
				printf("expr\n");
				{
					t_osc_expr *ee = osc_expr_arg_getExpr(a);
					t_osc_atom_ar_u *ar = osc_expr_parser_foldConstants_impl(ee,
												 lexenv);
					if(ar){
						osc_expr_free(ee);
						printf("reduced\n");
						if(osc_atom_array_u_getLen(ar) == 1){
							osc_expr_arg_setOSCAtom(a, osc_atom_array_u_get(ar, 0));
						}else{
							osc_expr_arg_setList(a, ar);
						}
					}
				}
			default:
				printf("default\n");
			}
			a = osc_expr_arg_next(a);
		}
		if(eval > 0){
			printf("eval = %d\n", eval);
			t_osc_atom_ar_u *res = NULL;
			int ret = osc_expr_eval(e, NULL, NULL, &res);
			if(ret){
				return NULL;
			}else{
				return res;
			}
		}else{
			return NULL;
		}
		e = osc_expr_next(e);
	}
}

static void osc_expr_parser_foldConstants(t_osc_expr *expr)
{
	osc_expr_parser_foldConstants_impl(expr, NULL);
}
*/

t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f)
{
	//printf("parsing %s\n", ptr);
	int len = strlen(ptr);
	int alloc = 0;

	// expressions really have to end with a semicolon, but it's nice to write single
	// expressions without one (or to leave it off the last one), so we add one to the
	// end of the string here just in case.
	/*
	if(ptr[len - 1] != ','){
		char *tmp = osc_mem_alloc(len + 2);
		memcpy(tmp, ptr, len);
		tmp[len] = ',';
		tmp[len + 1] = '\0';
		ptr = tmp;
		alloc = 1;
		len++;
	}
	*/
	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr *exprstack = NULL;
	t_osc_expr *tmp_exprstack = NULL;
	long buflen = 0;
	char *buf = NULL;
	int startcond = START_EXPNS;
	int started = 0;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, &tmp_exprstack, NULL, scanner, ptr, &buflen, &buf, startcond, &started);
	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	if(tmp_exprstack){
		if(exprstack){
			osc_expr_appendExpr(exprstack, tmp_exprstack);
		}else{
			exprstack = tmp_exprstack;
		}
	}
	//osc_expr_parser_foldConstants(exprstack);
	*f = exprstack;
	if(alloc){
		osc_mem_free(ptr);
	}
	return ret;
}

t_osc_err osc_expr_parser_parseFunction(char *ptr, t_osc_expr_rec **f)
{
	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr *exprstack = NULL;
	t_osc_expr *tmp_exprstack = NULL;
	long buflen = 0;
	char *buf = NULL;
	int startcond = START_FUNCTION;
	int started = 0;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, &tmp_exprstack, f, scanner, ptr, &buflen, &buf, startcond, &started);

	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	return ret;
}

/*
t_osc_err osc_expr_parser_parseString(char *ptr, t_osc_expr **f)
{
	return osc_expr_parser_parseExpr(ptr, f);
}
*/

void osc_expr_error_formatLocation(YYLTYPE *llocp, char *input_string, char **buf)
{
	int len = strlen(input_string);
	if(llocp->first_column >= len || llocp->last_column >= len){
		*buf = osc_mem_alloc(len + 1);
		strncpy(*buf, input_string, len + 1);
		return;
	}
	char s1[len * 2];
	char s2[len * 2];
	char s3[len * 2];
	memcpy(s1, input_string, llocp->first_column);
	s1[llocp->first_column] = '\0';
	memcpy(s2, input_string + llocp->first_column, llocp->last_column - llocp->first_column);
	s2[llocp->last_column - llocp->first_column] = '\0';
	memcpy(s3, input_string + llocp->last_column, len - llocp->last_column);
	s3[len - llocp->last_column] = '\0';
	*buf = osc_mem_alloc(len * 3 + 24); // way too much
	sprintf(*buf, "%s\n-->                %s\n%s\n", s1, s2, s3);
}

void osc_expr_error(YYLTYPE *llocp,
		    char *input_string,
		    t_osc_err errorcode,
		    const char * const moreinfo_fmt,
		    ...)
{
	char *loc = NULL;
	osc_expr_error_formatLocation(llocp, input_string, &loc);
	int loclen = 0;
	if(loc){
		loclen = strlen(loc);
	}
	va_list ap;
	va_start(ap, moreinfo_fmt);
	char more[256];
	memset(more, '\0', sizeof(more));
	int more_len = 0;
	if(ap){
		more_len += vsnprintf(more, 256, moreinfo_fmt, ap);
		va_end(ap);
	}
	if(loclen || more_len){
		char buf[loclen + more_len];
		char *ptr = buf;
		if(loclen){
			ptr += sprintf(ptr, "%s\n", loc);
		}
		if(more_len){
			ptr += sprintf(ptr, "%s\n", more);
		}
		osc_error_handler(__FILE__, //basename(__FILE__), // basename() seems to crash under cygwin...
				  NULL,
				  -1,
				  errorcode,
				  buf);
	}else{
	  osc_error_handler(__FILE__,//basename(__FILE__),
				  NULL,
				  -1,
				  errorcode,
				  "");
	}
	if(loc){
		osc_mem_free(loc);
	}
}

int osc_expr_parser_checkArity(YYLTYPE *llocp, char *input_string, t_osc_expr_rec *r, t_osc_expr_arg *arglist)
{
	if(!r){
		return 1;
	}
	/*
	if(r->arity < 0){
		// variable number of arguments
		return 0;
	}
	*/
	int i = 0;
	t_osc_expr_arg *a = arglist;
	while(a){
		i++;
		a = osc_expr_arg_next(a);
	}
	if(i == r->num_required_args){
		return 0;
	}
	if(i < r->num_required_args){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "expected %d %s for function %s but found %d.",
			       r->num_required_args,
			       r->num_required_args == 1 ? "argument" : "arguments",
			       r->name,
			       i);
		return 1;
	}

	// i is more than the num of required args.
 	if(r->num_optional_args < 0){
		return 0;
	}
	if(r->num_optional_args == 0 || (i - r->num_required_args) > r->num_optional_args){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "expected %d %s for function %s but found %d.",
			       r->num_required_args + r->num_optional_args,
			       (r->num_required_args + r->num_optional_args) == 1 ? "argument" : "arguments",
			       r->name,
			       i);
		return 1;
	}
	return 0;
}

 void yyerror(YYLTYPE *llocp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started, char const *e)
{
	osc_expr_error(llocp, input_string, OSC_ERR_EXPPARSE, e);
}

void osc_expr_parser_reportUnknownFunctionError(YYLTYPE *llocp,
						 char *input_string,
						 char *function_name)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "unknown function \"%s\"",
		       function_name);
}

void osc_expr_parser_reportInvalidLvalError(YYLTYPE *llocp,
					    char *input_string,
					    char *lvalue)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "\"%s\" is not a valid target for assignment (invalid lvalue)\n",
		       lvalue);
}

t_osc_expr *osc_expr_parser_reduce_PrefixFunction(YYLTYPE *llocp,
						  char *input_string,
						  char *function_name,
						  t_osc_expr_arg *arglist);

t_osc_expr *osc_expr_parser_reduce_InfixOperator(YYLTYPE *llocp,
						char *input_string,
						char *function_name,
						t_osc_expr_arg *left,
						t_osc_expr_arg *right)
{
	t_osc_expr_rec *r = osc_expr_lookupFunction(function_name);
	if(!r){
		osc_expr_parser_reportUnknownFunctionError(llocp, input_string, function_name);
		return NULL;
	}
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);
	osc_expr_prependArg(e, right);
	osc_expr_prependArg(e, left);
	return e;
}

t_osc_expr *osc_expr_parser_reduce_InfixAssignmentOperator(YYLTYPE *llocp,
							  char *input_string,
							  char *function_name,
							  t_osc_expr_arg *left,
							  t_osc_expr_arg *right)
{
	t_osc_expr *infix = osc_expr_parser_reduce_InfixOperator(llocp, input_string, function_name, left, right);
	t_osc_expr_arg *assign_target = NULL;
	osc_expr_arg_copy(&assign_target, left);
	t_osc_expr_arg *assign_arg = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(assign_arg, infix);
	osc_expr_arg_setNext(assign_target, assign_arg);
	t_osc_expr *assign = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "assign", assign_target);
	return assign;
}

t_osc_expr *osc_expr_parser_reduce_PrefixFunction(YYLTYPE *llocp,
						 char *input_string,
						 char *function_name,
						 t_osc_expr_arg *arglist)
{
	t_osc_expr_rec *r = osc_expr_lookupFunction(function_name);
	if(!r){
		osc_expr_parser_reportUnknownFunctionError(llocp, input_string, function_name);
		return NULL;
	}
	if(osc_expr_parser_checkArity(llocp, input_string, r, arglist)){
		return NULL;
	}
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);
	if(arglist){
		osc_expr_setArg(e, arglist);
	}
	return e;
}

t_osc_expr *osc_expr_parser_reduce_PrefixUnaryOperator(YYLTYPE *llocp,
						      char *input_string,
						      char *oscaddress,
						      char *op)
{
	char *ptr = oscaddress;
	if(*ptr != '/'){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "\"%s\" is not a valid target for assignment\nexpected \"%s\" in \"%s%s\" to be an OSC address\n",
			       oscaddress,
			       oscaddress,
			       op,
			       oscaddress);
		return NULL;
	}
	t_osc_expr_arg *arg = osc_expr_arg_alloc();
	osc_expr_arg_setOSCAddress(arg, ptr);
	t_osc_expr *pfu = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, op, arg);

	t_osc_expr_arg *assign_target = NULL;
	osc_expr_arg_copy(&assign_target, arg);
	t_osc_expr_arg *assign_arg = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(assign_arg, pfu);
	osc_expr_arg_setNext(assign_target, assign_arg);
	t_osc_expr *assign = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "assign", assign_target);
	return assign;
}

t_osc_expr *osc_expr_parser_reduce_PostfixUnaryOperator(YYLTYPE *llocp,
						       char *input_string,
						       char *oscaddress,
						       char *op)
{
	t_osc_expr *incdec = osc_expr_parser_reduce_PrefixUnaryOperator(llocp, input_string, oscaddress, op);
	if(!incdec){
		return NULL;
	}
	t_osc_expr_arg *arg1 = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(arg1, incdec);
	char *oscaddress_copy = NULL;
	osc_util_strdup(&oscaddress_copy, oscaddress);
	t_osc_expr_arg *arg2 = osc_expr_arg_alloc();
	osc_expr_arg_setOSCAddress(arg2, oscaddress_copy);
	osc_expr_arg_setNext(arg2, arg1);
	t_osc_expr *prog1 = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "prog1", arg2);
	return prog1;
}

t_osc_expr *osc_expr_parser_reduce_NullCoalescingOperator(YYLTYPE *llocp,
							 char *input_string,
							 t_osc_atom_u *address_to_check,
							 t_osc_expr_arg *arg_if_null)
{
	char *address = NULL;
	osc_atom_u_getString(address_to_check, 0, &address);
	if(*address != '/'){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "\"%s\" is not a valid target for assignment\nexpected \"%s\" to be an OSC address\n",
			       address,
			       address);
		osc_mem_free(address);
		return NULL;
	}
	t_osc_expr *expr_def = osc_expr_alloc();
	osc_expr_setRec(expr_def, osc_expr_lookupFunction("bound"));
	t_osc_expr_arg *def_arg = osc_expr_arg_alloc();

	osc_expr_arg_setOSCAddress(def_arg, address);
	osc_expr_setArg(expr_def, def_arg);
	t_osc_expr_arg *arg1 = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(arg1, expr_def);
	t_osc_expr_arg *arg2 = NULL;
	osc_expr_arg_copy(&arg2, def_arg);
	t_osc_expr_arg *arg3 = arg_if_null;
	osc_expr_arg_setNext(arg1, arg2);
	osc_expr_arg_setNext(arg2, arg3);
	return osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "if", arg1);
}


/* Line 371 of yacc.c  */
#line 554 "osc_expr_parser.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "osc_expr_parser.h".  */
#ifndef YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
# define YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int osc_expr_parser_debug;
#endif
/* "%code requires" blocks.  */
/* Line 387 of yacc.c  */
#line 64 "osc_expr_parser.y"

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


/* Line 387 of yacc.c  */
#line 613 "osc_expr_parser.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OSC_EXPR_LAMBDA = 258,
     OSC_EXPR_OSCADDRESS = 259,
     OSC_EXPR_STRING = 260,
     OSC_EXPR_NUM = 261,
     OSC_EXPR_POWEQ = 262,
     OSC_EXPR_MODEQ = 263,
     OSC_EXPR_DIVEQ = 264,
     OSC_EXPR_MULTEQ = 265,
     OSC_EXPR_MINUSEQ = 266,
     OSC_EXPR_PLUSEQ = 267,
     OSC_EXPR_TERNARY_COND = 268,
     OSC_EXPR_OROR = 269,
     OSC_EXPR_ANDAND = 270,
     OSC_EXPR_NEQ = 271,
     OSC_EXPR_EQ = 272,
     OSC_EXPR_GTE = 273,
     OSC_EXPR_LTE = 274,
     OSC_EXPR_DBLQMARKEQ = 275,
     OSC_EXPR_DBLQMARK = 276,
     OSC_EXPR_UMINUS = 277,
     OSC_EXPR_UPLUS = 278,
     OSC_EXPR_PREFIX_DEC = 279,
     OSC_EXPR_PREFIX_INC = 280,
     CLOSE_DBL_BRKTS = 281,
     OPEN_DBL_BRKTS = 282,
     OSC_EXPR_QUOTED_EXPR = 283,
     OSC_EXPR_FUNC_CALL = 284,
     OSC_EXPR_DEC = 285,
     OSC_EXPR_INC = 286,
     START_EXPNS = 287,
     START_FUNCTION = 288
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 547 "osc_expr_parser.y"

	t_osc_atom_u *atom;
	t_osc_expr *expr;
	t_osc_expr_rec *func;
	t_osc_expr_arg *arg;


/* Line 387 of yacc.c  */
#line 669 "osc_expr_parser.c"
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
int osc_expr_parser_parse (void *YYPARSE_PARAM);
#else
int osc_expr_parser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int osc_expr_parser_parse (t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started);
#else
int osc_expr_parser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 709 "osc_expr_parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   470

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  9
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNRULES -- Number of states.  */
#define YYNSTATES  165

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,     2,     2,    31,    19,     2,
      50,    53,    29,    27,    49,    28,    40,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    15,     2,
      23,     7,    24,    14,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    51,     2,    52,    32,     2,    54,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    17,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     8,     9,    10,    11,    12,    13,    16,    18,
      20,    21,    22,    25,    26,    34,    35,    36,    37,    38,
      39,    41,    42,    43,    44,    45,    46,    47,    48
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    12,    16,    18,    22,
      24,    26,    28,    30,    32,    41,    48,    56,    58,    62,
      64,    68,    73,    77,    79,    84,    88,    92,    96,   100,
     104,   108,   112,   116,   120,   124,   128,   132,   136,   140,
     144,   148,   152,   156,   160,   164,   168,   172,   176,   180,
     184,   187,   190,   193,   196,   199,   202,   206,   213,   219,
     228,   239,   245,   249,   253,   257,   260,   265,   270,   276,
     280,   284,   289,   294,   299
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    47,    57,    -1,    48,    60,    -1,    -1,
      63,    -1,    57,    49,    63,    -1,    59,    -1,    58,    49,
      59,    -1,     6,    -1,     5,    -1,     4,    -1,    63,    -1,
      60,    -1,     3,    50,    51,    61,    52,    49,    58,    53,
      -1,     3,    50,    62,    49,    58,    53,    -1,     3,    50,
      51,    52,    49,    58,    53,    -1,    62,    -1,    61,    49,
      62,    -1,     5,    -1,    50,    63,    53,    -1,     5,    50,
      58,    53,    -1,     5,    50,    53,    -1,    43,    -1,     4,
      50,    58,    53,    -1,     4,    50,    53,    -1,    59,    27,
      59,    -1,    59,    28,    59,    -1,    59,    29,    59,    -1,
      59,    30,    59,    -1,    59,    31,    59,    -1,    59,    32,
      59,    -1,    59,    22,    59,    -1,    59,    21,    59,    -1,
      59,    23,    59,    -1,    59,    26,    59,    -1,    59,    24,
      59,    -1,    59,    25,    59,    -1,    59,    19,    59,    -1,
      59,    20,    59,    -1,    59,    17,    59,    -1,    59,    18,
      59,    -1,    59,    13,    59,    -1,    59,    12,    59,    -1,
      59,    11,    59,    -1,    59,    10,    59,    -1,    59,     9,
      59,    -1,    59,     8,    59,    -1,     4,    40,     4,    -1,
       5,    40,     4,    -1,    33,    59,    -1,    28,    59,    -1,
      46,     4,    -1,    45,     4,    -1,     4,    46,    -1,     4,
      45,    -1,     4,     7,    59,    -1,     4,    42,    58,    41,
       7,    59,    -1,     4,    40,     4,     7,    59,    -1,     4,
      42,    59,    15,    59,    41,     7,    59,    -1,     4,    42,
      59,    15,    59,    15,    59,    41,     7,    59,    -1,    59,
      14,    59,    15,    59,    -1,     4,    35,    59,    -1,     4,
      34,    59,    -1,    51,    58,    52,    -1,    51,    52,    -1,
      59,    42,    58,    41,    -1,     4,    42,    58,    41,    -1,
      50,    60,    49,    58,    53,    -1,    54,     5,    54,    -1,
      54,     4,    54,    -1,     5,    46,     7,    59,    -1,     5,
      45,     7,    59,    -1,    46,     5,     7,    59,    -1,    45,
       5,     7,    59,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   599,   599,   600,   602,   614,   621,   638,   639,   644,
     648,   652,   661,   692,   700,   819,   942,  1046,  1047,  1053,
    1073,  1077,  1089,  1098,  1103,  1116,  1129,  1132,  1135,  1138,
    1141,  1144,  1147,  1150,  1153,  1156,  1159,  1162,  1165,  1168,
    1171,  1174,  1177,  1180,  1183,  1186,  1189,  1192,  1195,  1208,
    1228,  1233,  1242,  1254,  1267,  1279,  1298,  1311,  1334,  1349,
    1370,  1435,  1441,  1447,  1463,  1466,  1470,  1474,  1484,  1491,
    1496,  1502,  1508,  1514,  1520
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OSC_EXPR_LAMBDA", "OSC_EXPR_OSCADDRESS",
  "OSC_EXPR_STRING", "OSC_EXPR_NUM", "'='", "OSC_EXPR_POWEQ",
  "OSC_EXPR_MODEQ", "OSC_EXPR_DIVEQ", "OSC_EXPR_MULTEQ",
  "OSC_EXPR_MINUSEQ", "OSC_EXPR_PLUSEQ", "'?'", "':'",
  "OSC_EXPR_TERNARY_COND", "'|'", "OSC_EXPR_OROR", "'&'",
  "OSC_EXPR_ANDAND", "OSC_EXPR_NEQ", "OSC_EXPR_EQ", "'<'", "'>'",
  "OSC_EXPR_GTE", "OSC_EXPR_LTE", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'",
  "'!'", "OSC_EXPR_DBLQMARKEQ", "OSC_EXPR_DBLQMARK", "OSC_EXPR_UMINUS",
  "OSC_EXPR_UPLUS", "OSC_EXPR_PREFIX_DEC", "OSC_EXPR_PREFIX_INC", "'.'",
  "CLOSE_DBL_BRKTS", "OPEN_DBL_BRKTS", "OSC_EXPR_QUOTED_EXPR",
  "OSC_EXPR_FUNC_CALL", "OSC_EXPR_DEC", "OSC_EXPR_INC", "START_EXPNS",
  "START_FUNCTION", "','", "'('", "'['", "']'", "')'", "'`'", "$accept",
  "start", "expns", "args", "arg", "function", "parameters", "parameter",
  "expr", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,    61,   262,   263,
     264,   265,   266,   267,    63,    58,   268,   124,   269,    38,
     270,   271,   272,    60,    62,   273,   274,    43,    45,    42,
      47,    37,    94,    33,   275,   276,   277,   278,   279,   280,
      46,   281,   282,   283,   284,   285,   286,   287,   288,    44,
      40,    91,    93,    41,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    58,    58,    59,
      59,    59,    59,    59,    60,    60,    60,    61,    61,    62,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     8,     6,     7,     1,     3,     1,
       3,     4,     3,     1,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     2,     2,     3,     6,     5,     8,
      10,     5,     3,     3,     3,     2,     4,     4,     5,     3,
       3,     4,     4,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     4,     0,     0,     0,    11,    10,     9,     0,     0,
      23,     0,     0,     0,     0,     0,     2,     0,    13,    12,
       3,     1,     0,     0,     0,     0,     0,     0,    55,    54,
       0,     0,     0,     0,     0,    51,    12,    50,    53,     0,
      52,     0,    13,    12,    65,     0,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    19,     0,     0,    56,    63,    62,
      48,     0,     7,    25,     0,    49,     0,     0,    22,     0,
       0,     0,     0,    20,     0,    64,    70,    69,    12,    47,
      46,    45,    44,    43,    42,     0,    40,    41,    38,    39,
      33,    32,    34,    36,    37,    35,    26,    27,    28,    29,
      30,    31,     0,     0,     0,    17,     0,     0,    67,     0,
      24,    72,    71,    21,    74,    73,     0,     8,     0,    66,
       0,     0,     0,     0,    58,     0,     0,    68,    61,     0,
      18,     0,    15,    57,     0,     0,    16,     0,     0,     0,
      14,     0,    59,     0,    60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,    16,    45,    46,    18,   124,    76,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -70
static const yytype_int16 yypact[] =
{
      99,   198,     1,    21,   -19,    85,    -5,   -70,   198,   198,
     -70,   118,   156,   198,   105,   161,   -11,   358,   -70,    25,
     -70,   -70,    -4,   198,   198,   198,    32,   198,   -70,   -70,
     136,    40,    66,    71,   167,    46,   -70,    46,   -70,    73,
     -70,   106,   103,    62,   -70,    -6,   358,    80,   109,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   -70,    27,   119,   358,    46,    46,
     160,   -38,   280,   -70,   -44,   -70,   198,   198,   -70,    38,
     198,   198,   198,   -70,   198,   -70,   -70,   -70,    28,   358,
     358,   358,   358,   358,   358,   306,   408,   408,    75,    75,
     428,   428,   -13,   -13,   -13,   -13,    52,    52,   -22,   -22,
     -22,    46,    -7,   125,    72,   -70,   198,   198,   168,   198,
     -70,   358,   358,   -70,    46,    46,    63,   358,   198,   -70,
     198,   171,   128,    65,   358,   198,   245,   -70,   384,    83,
     -70,   198,   -70,   358,   198,   173,   -70,    96,   332,   198,
     -70,   176,   358,   198,   358
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -70,   -70,   -70,     3,    -1,     0,   -70,   -69,    26
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
      17,    74,    20,   128,     4,    94,   125,    35,    37,   130,
      72,    94,    17,    42,    67,    68,    69,    70,    71,    72,
      73,    21,    77,    78,    79,    -5,    82,    19,    -6,    73,
      81,    22,    74,    84,   139,    31,    80,    89,    49,    43,
      32,    33,    94,    94,    85,    34,    95,    75,    17,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   150,    86,    -5,    98,   122,    -6,    87,   123,
      90,    69,    70,    71,    72,   131,   132,    94,    73,   134,
     135,   133,    23,   137,    73,   136,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,     4,     5,
       6,     7,    94,    91,    94,    93,   147,    73,   152,    24,
      25,   141,    38,    39,   142,    26,   144,    27,   146,   143,
      28,    29,    94,     8,    96,    30,   156,   148,     9,     4,
       5,     6,     7,   149,   153,    94,     1,     2,    10,   160,
      11,    12,    92,   158,   157,    13,    14,    44,   162,    15,
      40,    41,   164,    97,     8,    47,    48,   127,   126,     9,
       4,     5,     6,     7,   140,   145,    74,   151,     0,    10,
     159,    11,    12,   163,     0,     0,    13,    14,     0,    83,
      15,     0,     0,     0,     0,     8,     0,     0,     0,     0,
       9,     4,     5,     6,     7,     0,     0,     0,     0,     0,
      10,     0,    11,    12,     0,     0,     0,    13,    14,     0,
      88,    15,     0,     0,     0,     0,     8,     0,     0,     0,
       0,     9,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    10,     0,    11,    12,     0,     0,     0,    13,    14,
       0,     0,    15,    50,    51,    52,    53,    54,    55,    56,
     154,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,     0,     0,
       0,     0,     0,     0,     0,     0,   155,    73,    50,    51,
      52,    53,    54,    55,    56,   129,     0,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     0,    50,    51,    52,    53,    54,    55,
      56,   138,    73,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,     0,
      50,    51,    52,    53,    54,    55,    56,     0,    73,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,     0,    50,    51,    52,    53,
      54,    55,    56,   161,    73,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,     0,     0,     0,     0,     0,     0,     0,    56,     0,
      73,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    73,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      73,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      73
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-70)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       1,     5,     2,    41,     3,    49,    75,     8,     9,    53,
      32,    49,    13,    13,    27,    28,    29,    30,    31,    32,
      42,     0,    23,    24,    25,     0,    27,     1,     0,    42,
      27,    50,     5,    30,    41,    40,     4,    34,    49,    13,
      45,    46,    49,    49,     4,    50,    52,    51,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,   141,     7,    49,    49,    73,    49,     7,    52,
       7,    29,    30,    31,    32,    86,    87,    49,    42,    90,
      91,    53,     7,    94,    42,    92,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,     3,     4,
       5,     6,    49,     7,    49,    53,    53,    42,    53,    34,
      35,    49,     4,     5,    52,    40,   127,    42,   129,   126,
      45,    46,    49,    28,    54,    50,    53,   138,    33,     3,
       4,     5,     6,   140,   145,    49,    47,    48,    43,    53,
      45,    46,    49,   154,   151,    50,    51,    52,   159,    54,
       4,     5,   163,    54,    28,     4,     5,     7,    49,    33,
       3,     4,     5,     6,    49,     7,     5,    49,    -1,    43,
       7,    45,    46,     7,    -1,    -1,    50,    51,    -1,    53,
      54,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      33,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,
      -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    45,    46,    -1,    -1,    -1,    50,    51,
      -1,    -1,    54,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,     8,     9,
      10,    11,    12,    13,    14,    15,    -1,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,     8,     9,    10,    11,    12,    13,
      14,    15,    42,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
       8,     9,    10,    11,    12,    13,    14,    -1,    42,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,     8,     9,    10,    11,
      12,    13,    14,    41,    42,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      42,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    47,    48,    56,     3,     4,     5,     6,    28,    33,
      43,    45,    46,    50,    51,    54,    57,    59,    60,    63,
      60,     0,    50,     7,    34,    35,    40,    42,    45,    46,
      50,    40,    45,    46,    50,    59,    63,    59,     4,     5,
       4,     5,    60,    63,    52,    58,    59,     4,     5,    49,
       8,     9,    10,    11,    12,    13,    14,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    42,     5,    51,    62,    59,    59,    59,
       4,    58,    59,    53,    58,     4,     7,     7,    53,    58,
       7,     7,    49,    53,    49,    52,    54,    54,    63,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    58,    52,    61,    62,    49,     7,    41,    15,
      53,    59,    59,    53,    59,    59,    58,    59,    15,    41,
      49,    49,    52,    58,    59,     7,    59,    53,    59,    58,
      62,    49,    53,    59,    15,    41,    53,    58,    59,     7,
      53,    41,    59,     7,    59
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner, alloc_atom, buflen, buf, startcond, started)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (exprstack);
  YYUSE (tmp_exprstack);
  YYUSE (rec);
  YYUSE (scanner);
  YYUSE (input_string);
  YYUSE (buflen);
  YYUSE (buf);
  YYUSE (startcond);
  YYUSE (started);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (exprstack);
  YYUSE (tmp_exprstack);
  YYUSE (rec);
  YYUSE (scanner);
  YYUSE (input_string);
  YYUSE (buflen);
  YYUSE (buf);
  YYUSE (startcond);
  YYUSE (started);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
int
yyparse (exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc = yyloc_default;


    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
/* Line 1802 of yacc.c  */
#line 602 "osc_expr_parser.y"
    {
		if(*tmp_exprstack){
			if(*exprstack){
				osc_expr_appendExpr(*exprstack, *tmp_exprstack);
			}else{
				*exprstack = *tmp_exprstack;
			}
			*tmp_exprstack = NULL;
		}
 	}
    break;

  case 5:
/* Line 1802 of yacc.c  */
#line 614 "osc_expr_parser.y"
    {
		if(*tmp_exprstack){
			osc_expr_appendExpr(*tmp_exprstack, (yyvsp[(1) - (1)].expr));
		}else{
			*tmp_exprstack = (yyvsp[(1) - (1)].expr);
		}
        }
    break;

  case 6:
/* Line 1802 of yacc.c  */
#line 621 "osc_expr_parser.y"
    {
		if(*tmp_exprstack){
			osc_expr_appendExpr(*tmp_exprstack, (yyvsp[(3) - (3)].expr));
		}else{
			*tmp_exprstack = (yyvsp[(3) - (3)].expr);
		}
 	}
    break;

  case 8:
/* Line 1802 of yacc.c  */
#line 639 "osc_expr_parser.y"
    {
		osc_expr_arg_append((yyval.arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 9:
/* Line 1802 of yacc.c  */
#line 644 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom((yyval.arg), (yyvsp[(1) - (1)].atom));
 	}
    break;

  case 10:
/* Line 1802 of yacc.c  */
#line 648 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom((yyval.arg), (yyvsp[(1) - (1)].atom));
 	}
    break;

  case 11:
/* Line 1802 of yacc.c  */
#line 652 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		char *st = osc_atom_u_getStringPtr((yyvsp[(1) - (1)].atom));
		int len = strlen(st) + 1;
		char *buf = osc_mem_alloc(len);
		memcpy(buf, st, len);
		osc_expr_arg_setOSCAddress((yyval.arg), buf);
		osc_atom_u_free((yyvsp[(1) - (1)].atom));
  	}
    break;

  case 12:
/* Line 1802 of yacc.c  */
#line 661 "osc_expr_parser.y"
    {
		t_osc_expr *e = (yyvsp[(1) - (1)].expr);
		//t_osc_expr_arg *a = osc_expr_getArgs(e);
		(yyval.arg) = osc_expr_arg_alloc();
		/*
		int eval = 1;
		while(a){
			int type = osc_expr_arg_getType(a);
			if(type == OSC_EXPR_ARG_TYPE_OSCADDRESS ||
			   type == OSC_EXPR_ARG_TYPE_EXPR ||
			   type == OSC_EXPR_ARG_TYPE_FUNCTION){
				eval = 0;
				break;
			}
			a = osc_expr_arg_next(a);
		}
		if(eval){
			t_osc_atom_ar_u *res = NULL;
			int ret = osc_expr_eval(e, NULL, NULL, &res);
			if(ret){
				osc_expr_arg_setExpr($$, e);
			}else{
				// assume that this is a special function like value() or bound() that
				// needs an OSC bundle to return a value
				osc_expr_arg_setList($$, res);
			}
		}else{
		*/
			osc_expr_arg_setExpr((yyval.arg), e);
			//}
  	}
    break;

  case 13:
/* Line 1802 of yacc.c  */
#line 692 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setFunction((yyval.arg), (yyvsp[(1) - (1)].func));
	}
    break;

  case 14:
/* Line 1802 of yacc.c  */
#line 700 "osc_expr_parser.y"
    {
		int n = 0;
		t_osc_atom_u *a = (yyvsp[(4) - (8)].atom);
		while(a){
			n++;
			a = a->next;
		}
		char *params[n];
		a = (yyvsp[(4) - (8)].atom);
		for(int i = n - 1; i >= 0; i--){
			char *st = osc_atom_u_getStringPtr(a);
			int len = strlen(st) + 1;
			params[i] = (char *)osc_mem_alloc(len);
			strncpy(params[i], st, len);
			t_osc_atom_u *killme = a;
			a = a->next;
			osc_atom_u_free(killme);
		}
		t_osc_expr_rec *func = osc_expr_rec_alloc();
		osc_expr_rec_setName(func, "lambda");
		osc_expr_rec_setRequiredArgs(func, n, params, NULL);
		for(int i = 0; i < n; i++){
			if(params[i]){
				osc_mem_free(params[i]);
			}
		}
		/*
		t_osc_expr *e = *tmp_exprstack;
		while(e){
			e = osc_expr_next(e);
		}
		*/
		osc_expr_rec_setFunction(func, osc_expr_lambda);
		t_osc_expr_arg *aaa = (yyvsp[(7) - (8)].arg);
		t_osc_expr *exprlist = NULL;
		//t_osc_expr *exprlist = osc_expr_arg_getExpr(aaa);
		if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
			exprlist = osc_expr_arg_getExpr(aaa);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
		}else{
			t_osc_expr *e = osc_expr_alloc();
			osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
			t_osc_expr_arg *aaacpy = NULL;
			osc_expr_arg_copy(&aaacpy, aaa);
			osc_expr_prependArg(e, aaacpy);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			exprlist = e;
		}
		
		int i = 1;
		while(aaa){
			/*
			if(osc_expr_arg_getType(aaa) != OSC_EXPR_ARG_TYPE_EXPR){
				osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "arg %d of lambda expression is not an expression\n", i);
				return 1;
			}
			*/
			if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
				t_osc_expr *e = osc_expr_arg_getExpr(aaa);
				osc_expr_appendExpr(exprlist, e);
				osc_expr_arg_setExpr(aaa, NULL);
				t_osc_expr_arg *old = aaa;
				aaa = osc_expr_arg_next(aaa);
				osc_expr_arg_free(old);
			}else{
				t_osc_expr *e = osc_expr_alloc();
				osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
				osc_expr_prependArg(e, aaa);
				aaa = osc_expr_arg_next(aaa);
				osc_expr_appendExpr(exprlist, e);
			}
			/*
			t_osc_expr *e = osc_expr_arg_getExpr(aaa);
			osc_expr_appendExpr(exprlist, e);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			*/
			i++;
		}
		osc_expr_rec_setExtra(func, exprlist);
		//osc_expr_rec_setExtra(func, *tmp_exprstack);
		(yyval.func) = func;
		if(startcond == START_EXPNS){
			//*tmp_exprstack = NULL;
		}else if(startcond == START_FUNCTION){
			*rec = func;
		}
// go through and make sure the parameters are unique
/*
		t_osc_expr_rec *r = osc_expr_lookupFunction("lambda");
		t_osc_expr *e = osc_expr_alloc();
		osc_expr_setRec(e, r);
		t_osc_expr *expns_copy = NULL;
		t_osc_expr *ee = $<expr>6;
		while(ee){
			t_osc_expr *copy = osc_expr_copy(ee);
			if(expns_copy){
				osc_expr_appendExpr(expns_copy, copy);
			}else{
				expns_copy = copy;
			}
			ee = osc_expr_next(ee);
		}
		t_osc_expr_arg *expns = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(expns, expns_copy);
		osc_expr_arg_append($3, expns);
		osc_expr_setArg(e, $3);
*/
//osc_expr_parser_bindParameters(&yylloc, input_string, e, $3, expns_copy);
//$$ = e;
	}
    break;

  case 15:
/* Line 1802 of yacc.c  */
#line 819 "osc_expr_parser.y"
    {
		int n = 0;
		t_osc_atom_u *a = (yyvsp[(3) - (6)].atom);
		while(a){
			n++;
			a = a->next;
		}
		char *params[n];
		a = (yyvsp[(3) - (6)].atom);
		for(int i = n - 1; i >= 0; i--){
			char *st = osc_atom_u_getStringPtr(a);
			int len = strlen(st) + 1;
			params[i] = (char *)osc_mem_alloc(len);
			strncpy(params[i], st, len);
			t_osc_atom_u *killme = a;
			a = a->next;
			osc_atom_u_free(killme);
		}
		t_osc_expr_rec *func = osc_expr_rec_alloc();
		osc_expr_rec_setName(func, "lambda");
		osc_expr_rec_setRequiredArgs(func, n, params, NULL);
		for(int i = 0; i < n; i++){
			if(params[i]){
				osc_mem_free(params[i]);
			}
		}
		/*
		t_osc_expr *e = *tmp_exprstack;
		while(e){
			e = osc_expr_next(e);
		}
		*/
		osc_expr_rec_setFunction(func, osc_expr_lambda);
		t_osc_expr_arg *aaa = (yyvsp[(5) - (6)].arg);
		//t_osc_expr *exprlist = osc_expr_arg_getExpr(aaa);
		t_osc_expr *exprlist = NULL;
		if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
			exprlist = osc_expr_arg_getExpr(aaa);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
		}else{
			t_osc_expr *e = osc_expr_alloc();
			osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
			t_osc_expr_arg *aaacpy = NULL;
			osc_expr_arg_copy(&aaacpy, aaa);
			osc_expr_prependArg(e, aaacpy);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			exprlist = e;
		}
		int i = 1;
		while(aaa){
			/*
			if(osc_expr_arg_getType(aaa) != OSC_EXPR_ARG_TYPE_EXPR){
				osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "arg %d of lambda expression is not an expression\n", i);
				return 1;
			}
			*/
			if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
				t_osc_expr *e = osc_expr_arg_getExpr(aaa);
				osc_expr_appendExpr(exprlist, e);
				osc_expr_arg_setExpr(aaa, NULL);
				t_osc_expr_arg *old = aaa;
				aaa = osc_expr_arg_next(aaa);
				osc_expr_arg_free(old);
			}else{
				t_osc_expr *e = osc_expr_alloc();
				osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
				t_osc_expr_arg *aaacpy = NULL;
				osc_expr_arg_copy(&aaacpy, aaa);
				osc_expr_prependArg(e, aaacpy);
				osc_expr_arg_setExpr(aaa, NULL);
				t_osc_expr_arg *old = aaa;
				aaa = osc_expr_arg_next(aaa);
				osc_expr_arg_free(old);
				osc_expr_appendExpr(exprlist, e);
			}
			/*
			t_osc_expr *e = osc_expr_arg_getExpr(aaa);
			osc_expr_appendExpr(exprlist, e);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			*/
			i++;
		}
		osc_expr_rec_setExtra(func, exprlist);
		//osc_expr_rec_setExtra(func, *tmp_exprstack);
		(yyval.func) = func;
		if(startcond == START_EXPNS){
			//*tmp_exprstack = NULL;
		}else if(startcond == START_FUNCTION){
			*rec = func;
		}
// go through and make sure the parameters are unique
/*
		t_osc_expr_rec *r = osc_expr_lookupFunction("lambda");
		t_osc_expr *e = osc_expr_alloc();
		osc_expr_setRec(e, r);
		t_osc_expr *expns_copy = NULL;
		t_osc_expr *ee = $<expr>6;
		while(ee){
			t_osc_expr *copy = osc_expr_copy(ee);
			if(expns_copy){
				osc_expr_appendExpr(expns_copy, copy);
			}else{
				expns_copy = copy;
			}
			ee = osc_expr_next(ee);
		}
		t_osc_expr_arg *expns = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(expns, expns_copy);
		osc_expr_arg_append($3, expns);
		osc_expr_setArg(e, $3);
*/
//osc_expr_parser_bindParameters(&yylloc, input_string, e, $3, expns_copy);
//$$ = e;
	}
    break;

  case 16:
/* Line 1802 of yacc.c  */
#line 942 "osc_expr_parser.y"
    {
		int n = 0;
		t_osc_expr_rec *func = osc_expr_rec_alloc();
		osc_expr_rec_setName(func, "lambda");
		osc_expr_rec_setRequiredArgs(func, 0, NULL, NULL);
		/*
		t_osc_expr *e = *tmp_exprstack;
		while(e){
			e = osc_expr_next(e);
		}
		*/
		osc_expr_rec_setFunction(func, osc_expr_lambda);
		t_osc_expr_arg *aaa = (yyvsp[(6) - (7)].arg);
		//t_osc_expr *exprlist = osc_expr_arg_getExpr(aaa);
		t_osc_expr *exprlist = NULL;
		if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
			exprlist = osc_expr_arg_getExpr(aaa);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
		}else{
			t_osc_expr *e = osc_expr_alloc();
			osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
			t_osc_expr_arg *aaacpy = NULL;
			osc_expr_arg_copy(&aaacpy, aaa);
			osc_expr_prependArg(e, aaacpy);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			exprlist = e;
		}
		int i = 1;
		while(aaa){
			/*
			if(osc_expr_arg_getType(aaa) != OSC_EXPR_ARG_TYPE_EXPR){
				osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "arg %d of lambda expression is not an expression\n", i);
				return 1;
			}
			*/
			if(osc_expr_arg_getType(aaa) == OSC_EXPR_ARG_TYPE_EXPR){
				t_osc_expr *e = osc_expr_arg_getExpr(aaa);
				osc_expr_appendExpr(exprlist, e);
				osc_expr_arg_setExpr(aaa, NULL);
				t_osc_expr_arg *old = aaa;
				aaa = osc_expr_arg_next(aaa);
				osc_expr_arg_free(old);
			}else{
				t_osc_expr *e = osc_expr_alloc();
				osc_expr_setRec(e, osc_expr_lookupFunction("prog1"));
				t_osc_expr_arg *aaacpy = NULL;
				osc_expr_arg_copy(&aaacpy, aaa);
				osc_expr_prependArg(e, aaacpy);
				osc_expr_arg_setExpr(aaa, NULL);
				t_osc_expr_arg *old = aaa;
				aaa = osc_expr_arg_next(aaa);
				osc_expr_arg_free(old);
				osc_expr_appendExpr(exprlist, e);
			}
			/*
			t_osc_expr *e = osc_expr_arg_getExpr(aaa);
			osc_expr_appendExpr(exprlist, e);
			osc_expr_arg_setExpr(aaa, NULL);
			t_osc_expr_arg *old = aaa;
			aaa = osc_expr_arg_next(aaa);
			osc_expr_arg_free(old);
			*/
			i++;
		}
		osc_expr_rec_setExtra(func, exprlist);
		//osc_expr_rec_setExtra(func, *tmp_exprstack);
		(yyval.func) = func;
		if(startcond == START_EXPNS){
			//*tmp_exprstack = NULL;
		}else if(startcond == START_FUNCTION){
			*rec = func;
		}
// go through and make sure the parameters are unique
/*
		t_osc_expr_rec *r = osc_expr_lookupFunction("lambda");
		t_osc_expr *e = osc_expr_alloc();
		osc_expr_setRec(e, r);
		t_osc_expr *expns_copy = NULL;
		t_osc_expr *ee = $<expr>6;
		while(ee){
			t_osc_expr *copy = osc_expr_copy(ee);
			if(expns_copy){
				osc_expr_appendExpr(expns_copy, copy);
			}else{
				expns_copy = copy;
			}
			ee = osc_expr_next(ee);
		}
		t_osc_expr_arg *expns = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(expns, expns_copy);
		osc_expr_arg_append($3, expns);
		osc_expr_setArg(e, $3);
*/
//osc_expr_parser_bindParameters(&yylloc, input_string, e, $3, expns_copy);
//$$ = e;
	}
    break;

  case 18:
/* Line 1802 of yacc.c  */
#line 1047 "osc_expr_parser.y"
    {
		(yyvsp[(3) - (3)].atom)->next = (yyvsp[(1) - (3)].atom);
		(yyval.atom) = (yyvsp[(3) - (3)].atom);
 	}
    break;

  case 19:
/* Line 1802 of yacc.c  */
#line 1053 "osc_expr_parser.y"
    {
		if(osc_atom_u_getTypetag((yyvsp[(1) - (1)].atom)) == 's'){
			char *st = osc_atom_u_getStringPtr((yyvsp[(1) - (1)].atom));
			if(st){
				if(*st == '/' && st[1] != '\0'){
					// this is an OSC address
					//error
				}else{
					(yyval.atom) = (yyvsp[(1) - (1)].atom);
				}
			}else{
				//error
			}
		}else{
			//error
		}
	}
    break;

  case 20:
/* Line 1802 of yacc.c  */
#line 1073 "osc_expr_parser.y"
    {
		(yyval.expr) = (yyvsp[(2) - (3)].expr);
  	}
    break;

  case 21:
/* Line 1802 of yacc.c  */
#line 1077 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc,
								     input_string,
								     osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)),
								     (yyvsp[(3) - (4)].arg));
		if(!e){
			osc_atom_u_free((yyvsp[(1) - (4)].atom));
			return 1;
		}
		(yyval.expr) = e;
		osc_atom_u_free((yyvsp[(1) - (4)].atom));
  	}
    break;

  case 22:
/* Line 1802 of yacc.c  */
#line 1089 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, osc_atom_u_getStringPtr((yyvsp[(1) - (3)].atom)), NULL);
		if(!e){
			osc_atom_u_free((yyvsp[(1) - (3)].atom));
			return 1;
		}
		(yyval.expr) = e;
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
  	}
    break;

  case 23:
/* Line 1802 of yacc.c  */
#line 1098 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom(arg, (yyvsp[(1) - (1)].atom));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "quote", arg);
	}
    break;

  case 24:
/* Line 1802 of yacc.c  */
#line 1103 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_alloc();
		t_osc_expr_rec *r = osc_expr_lookupFunction("apply");
		osc_expr_setRec(e, r);
		t_osc_expr_arg *a = osc_expr_arg_alloc();
		char *address = NULL;
		osc_atom_u_getString((yyvsp[(1) - (4)].atom), 0, &address);
		osc_expr_arg_setOSCAddress(a, address);
		osc_expr_arg_append(a, (yyvsp[(3) - (4)].arg));
		osc_expr_setArg(e, a);
		osc_atom_u_free((yyvsp[(1) - (4)].atom));
		(yyval.expr) = e;
	}
    break;

  case 25:
/* Line 1802 of yacc.c  */
#line 1116 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_alloc();
		t_osc_expr_rec *r = osc_expr_lookupFunction("apply");
		osc_expr_setRec(e, r);
		t_osc_expr_arg *a = osc_expr_arg_alloc();
		char *address = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &address);
		osc_expr_arg_setOSCAddress(a, address);
		osc_expr_setArg(e, a);
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
		(yyval.expr) = e;
	}
    break;

  case 26:
/* Line 1802 of yacc.c  */
#line 1129 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "+", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 27:
/* Line 1802 of yacc.c  */
#line 1132 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "-", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 28:
/* Line 1802 of yacc.c  */
#line 1135 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "*", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 29:
/* Line 1802 of yacc.c  */
#line 1138 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "/", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 30:
/* Line 1802 of yacc.c  */
#line 1141 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "%", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 31:
/* Line 1802 of yacc.c  */
#line 1144 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "^", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 32:
/* Line 1802 of yacc.c  */
#line 1147 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "==", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 33:
/* Line 1802 of yacc.c  */
#line 1150 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "!=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 34:
/* Line 1802 of yacc.c  */
#line 1153 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "<", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 35:
/* Line 1802 of yacc.c  */
#line 1156 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "<=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 36:
/* Line 1802 of yacc.c  */
#line 1159 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ">", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 37:
/* Line 1802 of yacc.c  */
#line 1162 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ">=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 38:
/* Line 1802 of yacc.c  */
#line 1165 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "&", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 39:
/* Line 1802 of yacc.c  */
#line 1168 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "&&", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 40:
/* Line 1802 of yacc.c  */
#line 1171 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "|", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 41:
/* Line 1802 of yacc.c  */
#line 1174 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "||", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 42:
/* Line 1802 of yacc.c  */
#line 1177 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "+", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 43:
/* Line 1802 of yacc.c  */
#line 1180 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "-", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 44:
/* Line 1802 of yacc.c  */
#line 1183 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "*", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 45:
/* Line 1802 of yacc.c  */
#line 1186 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "/", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 46:
/* Line 1802 of yacc.c  */
#line 1189 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "%", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 47:
/* Line 1802 of yacc.c  */
#line 1192 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "^", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 48:
/* Line 1802 of yacc.c  */
#line 1195 "osc_expr_parser.y"
    {
		t_osc_expr_arg *a1 = osc_expr_arg_alloc();
		t_osc_expr_arg *a2 = osc_expr_arg_alloc();
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &ptr);
		osc_expr_arg_setOSCAddress(a1, ptr);
		ptr = NULL;
		osc_atom_u_getString((yyvsp[(3) - (3)].atom), 0, &ptr);
		osc_expr_arg_setOSCAddress(a2, ptr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ".", a1, a2);
		osc_mem_free((yyvsp[(1) - (3)].atom));
		osc_mem_free((yyvsp[(3) - (3)].atom));
	}
    break;

  case 49:
/* Line 1802 of yacc.c  */
#line 1208 "osc_expr_parser.y"
    {
		printf("hi\n");
		t_osc_expr_arg *a1 = osc_expr_arg_alloc();
		t_osc_expr_arg *a2 = osc_expr_arg_alloc();
		char *ptr = NULL;
		//osc_atom_u_getString($1, 0, &ptr);
		//osc_expr_arg_setOSCAddress(a1, ptr);
		osc_expr_arg_setOSCAtom(a1, (yyvsp[(1) - (3)].atom));
		//ptr = NULL;
		osc_atom_u_getString((yyvsp[(3) - (3)].atom), 0, &ptr);
		osc_expr_arg_setOSCAddress(a2, ptr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ".", a1, a2);
		//osc_mem_free($1);
		osc_mem_free((yyvsp[(3) - (3)].atom));
	}
    break;

  case 50:
/* Line 1802 of yacc.c  */
#line 1228 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_alloc();
		osc_expr_setRec((yyval.expr), osc_expr_lookupFunction("!"));
		osc_expr_setArg((yyval.expr), (yyvsp[(2) - (2)].arg));
	}
    break;

  case 51:
/* Line 1802 of yacc.c  */
#line 1233 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_alloc();
		osc_expr_setRec((yyval.expr), osc_expr_lookupFunction("-"));
		t_osc_expr_arg *zero = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom(zero, osc_atom_u_allocWithInt32(0));
		osc_expr_arg_setNext(zero, (yyvsp[(2) - (2)].arg));
		osc_expr_setArg((yyval.expr), zero);
	}
    break;

  case 52:
/* Line 1802 of yacc.c  */
#line 1242 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(2) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PrefixUnaryOperator(&yylloc, input_string, copy, "plus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(2) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(2) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 53:
/* Line 1802 of yacc.c  */
#line 1254 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(2) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PrefixUnaryOperator(&yylloc, input_string, copy, "minus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(2) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(2) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 54:
/* Line 1802 of yacc.c  */
#line 1267 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(1) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PostfixUnaryOperator(&yylloc, input_string, copy, "plus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(1) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(1) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 55:
/* Line 1802 of yacc.c  */
#line 1279 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(1) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PostfixUnaryOperator(&yylloc, input_string, copy, "minus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(1) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(1) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 56:
/* Line 1802 of yacc.c  */
#line 1298 "osc_expr_parser.y"
    {
		// basic assignment 
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "assign", arg, (yyvsp[(3) - (3)].arg));
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
 	}
    break;

  case 57:
/* Line 1802 of yacc.c  */
#line 1311 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (6)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		t_osc_expr_arg *indexes = (yyvsp[(3) - (6)].arg);
		if(osc_expr_arg_next((yyvsp[(3) - (6)].arg))){
			// /foo[[1, 2, 3]] = ...
			t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "list", (yyvsp[(3) - (6)].arg));
			indexes = NULL;
			indexes = osc_expr_arg_alloc();
			osc_expr_arg_setExpr(indexes, e);
		}
		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(6) - (6)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (6)].atom));
	}
    break;

  case 58:
/* Line 1802 of yacc.c  */
#line 1334 "osc_expr_parser.y"
    {
		t_osc_expr_arg *a1 = osc_expr_arg_alloc();
		t_osc_expr_arg *a2 = osc_expr_arg_alloc();
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (5)].atom), 0, &ptr);
		osc_expr_arg_setOSCAddress(a1, ptr);
		ptr = NULL;
		osc_atom_u_getString((yyvsp[(3) - (5)].atom), 0, &ptr);
		osc_expr_arg_setOSCAddress(a2, ptr);
		osc_expr_arg_append(a1, a2);
		osc_expr_arg_append(a1, (yyvsp[(5) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assigntobundlemember", a1);
		osc_mem_free((yyvsp[(1) - (5)].atom));
		osc_mem_free((yyvsp[(3) - (5)].atom));
	}
    break;

  case 59:
/* Line 1802 of yacc.c  */
#line 1349 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (8)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();

		osc_expr_arg_append((yyvsp[(3) - (8)].arg), (yyvsp[(5) - (8)].arg));
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(3) - (8)].arg));
		t_osc_expr_arg *indexes = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(indexes, e);

		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(8) - (8)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (8)].atom));
	}
    break;

  case 60:
/* Line 1802 of yacc.c  */
#line 1370 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (10)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();

		osc_expr_arg_append((yyvsp[(3) - (10)].arg), (yyvsp[(7) - (10)].arg));
		osc_expr_arg_append((yyvsp[(3) - (10)].arg), (yyvsp[(5) - (10)].arg));
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(3) - (10)].arg));
		t_osc_expr_arg *indexes = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(indexes, e);

		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(10) - (10)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (10)].atom));
	}
    break;

  case 61:
/* Line 1802 of yacc.c  */
#line 1435 "osc_expr_parser.y"
    {
		// ternary conditional
		osc_expr_arg_append((yyvsp[(1) - (5)].arg), (yyvsp[(3) - (5)].arg));
		osc_expr_arg_append((yyvsp[(1) - (5)].arg), (yyvsp[(5) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "if", (yyvsp[(1) - (5)].arg));
  	}
    break;

  case 62:
/* Line 1802 of yacc.c  */
#line 1441 "osc_expr_parser.y"
    {
		// null coalescing operator from C#.  
		// /foo ?? 10 means /foo if /foo is in the bundle, otherwise 10
		(yyval.expr) = osc_expr_parser_reduce_NullCoalescingOperator(&yylloc, input_string, (yyvsp[(1) - (3)].atom), (yyvsp[(3) - (3)].arg));
		osc_atom_u_free((yyvsp[(1) - (3)].atom)); // the above function will copy that
	}
    break;

  case 63:
/* Line 1802 of yacc.c  */
#line 1447 "osc_expr_parser.y"
    {
		// null coalescing operator from C#.  
		// /foo ?? 10 means /foo if /foo is in the bundle, otherwise 10
		t_osc_expr *if_expr = osc_expr_parser_reduce_NullCoalescingOperator(&yylloc, input_string, (yyvsp[(1) - (3)].atom), (yyvsp[(3) - (3)].arg));
		if(!if_expr){
			return 1;
		}
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &ptr);
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		t_osc_expr_arg *arg2 = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(arg2, if_expr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "=", arg, arg2);
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
	}
    break;

  case 64:
/* Line 1802 of yacc.c  */
#line 1463 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "list", (yyvsp[(2) - (3)].arg));
	}
    break;

  case 65:
/* Line 1802 of yacc.c  */
#line 1466 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "list", NULL);
	}
    break;

  case 66:
/* Line 1802 of yacc.c  */
#line 1470 "osc_expr_parser.y"
    {
		osc_expr_arg_setNext((yyvsp[(1) - (4)].arg), (yyvsp[(3) - (4)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "nth", (yyvsp[(1) - (4)].arg));
	}
    break;

  case 67:
/* Line 1802 of yacc.c  */
#line 1474 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (4)].atom), 0, &ptr);
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_setNext(arg, (yyvsp[(3) - (4)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "nth", arg);
		osc_atom_u_free((yyvsp[(1) - (4)].atom));
	}
    break;

  case 68:
/* Line 1802 of yacc.c  */
#line 1484 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setFunction(arg, (yyvsp[(2) - (5)].func));
		osc_expr_arg_append(arg, (yyvsp[(4) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "apply", arg);
	}
    break;

  case 69:
/* Line 1802 of yacc.c  */
#line 1491 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom(arg, (yyvsp[(2) - (3)].atom));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "value", arg);
	}
    break;

  case 70:
/* Line 1802 of yacc.c  */
#line 1496 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, osc_atom_u_getStringPtr((yyvsp[(2) - (3)].atom)));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "value", arg);
	}
    break;

  case 71:
/* Line 1802 of yacc.c  */
#line 1502 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom))) + 3];
		sprintf(buf, "%s++", osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 72:
/* Line 1802 of yacc.c  */
#line 1508 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom))) + 3];
		sprintf(buf, "%s--", osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 73:
/* Line 1802 of yacc.c  */
#line 1514 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom))) + 3];
		sprintf(buf, "++%s", osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 74:
/* Line 1802 of yacc.c  */
#line 1520 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom))) + 3];
		sprintf(buf, "--%s", osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;


/* Line 1802 of yacc.c  */
#line 3468 "osc_expr_parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


