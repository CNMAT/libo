/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

/** 	\file osc_expr_parser.y
	\author John MacCallum

*/

%code top{

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>

#include "osc_hashtab.h"
#include "osc_parse.h"
#include "osc_lex.h"
#include "osc_util.h"


#define OSC_EXPR_PARSER_DEBUG
#ifdef OSC_EXPR_PARSER_DEBUG
#define PP(fmt, ...)printf(fmt, ##__VA_ARGS__)
#else
#define PP(fmt, ...)
#endif

#ifdef __cplusplus
	extern "C" int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r);
#else
	int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r);
#endif

}
%code requires{
#include "osc.h"
#include "osc_mem.h"
#include "osc_capi.h"
#include "osc_capi_bundle.h"
#include "osc_capi_message.h"
#include "osc_capi_primitive.h"
#include "osc_region.h"
#include "osc_list.h"
#include "osc_hashtab.h"
#include "osc_builtin.h"

#ifdef YY_DECL
#undef YY_DECL
#endif

#ifdef __cplusplus
#define YY_DECL extern "C" int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r)
#else
#define YY_DECL int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r)
#endif
#ifdef __cplusplus
extern "C"{
#endif
t_osc_bndl osc_parse(t_osc_region r, char *ptr);
t_osc_bndl osc_parse_allocApply(t_osc_region r, t_osc_bndl lhs, t_osc_bndl rhs);
t_osc_bndl osc_parse_allocApplication(t_osc_region r, t_osc_list lhs, t_osc_bndl applicator, t_osc_list rhs);
t_osc_bndl osc_parse_allocApplicationWithLists(t_osc_region r, t_osc_list lhs, t_osc_bndl applicator, t_osc_list rhs);
#ifdef __cplusplus
}
#endif
}

%{

	int osc_parse_lex(YYSTYPE *yylval_param, YYLTYPE *llocp, yyscan_t yyscanner, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r)
{
	return osc_lex_lex(yylval_param, llocp, yyscanner, ht, strict, nonstrict, level, r);
}

t_osc_bndl osc_parse(t_osc_region r, char *ptr)
{
	yyscan_t scanner;
	osc_lex_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_lex__scan_string(ptr, scanner);
	osc_lex_set_out(NULL, scanner);
	t_osc_bndl b = NULL;
	t_osc_hashtab *ht = osc_hashtab_new(-1, NULL);
	t_osc_bndl strict = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	t_osc_bndl nonstrict = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	int level = 0;
	osc_parse_parse(scanner, &b, ptr, NULL, ht, &strict, &nonstrict, &level, r);
	osc_lex__delete_buffer(buf_state, scanner);
	osc_lex_lex_destroy(scanner);
	osc_hashtab_destroy(ht);
	return b;
}

void yyerror(YYLTYPE *llocp, void *scanner, t_osc_bndl *bndl, const char *input_string, t_osc_bndl *context, t_osc_hashtab *ht, t_osc_bndl *strict, t_osc_bndl *nonstrict, int *level, t_osc_region r, char const *e)
{
	printf("%s: error\n", __func__);
}
/*
t_osc_bndl osc_parse_allocApply(t_osc_region r, t_osc_bndl lhs, t_osc_bndl rhs)
{
	return osc_capi_bndl_alloc(r,
				   OSC_TIMETAG_NULL,
				   3,
				   osc_capi_msg_alloc(r,
						      1,
						      osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@")),
				   osc_capi_msg_alloc(r,
						      2,
						      osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@/fn"),
						      lhs),
				   osc_capi_msg_alloc(r,
						      2,
						      osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@/args"),
						      rhs));
}
*/

t_osc_bndl osc_parse_allocApplication(t_osc_region r, t_osc_msg lhs, t_osc_bndl applicator, t_osc_msg rhs)
{
	return osc_capi_bndl_alloc(r,
				   OSC_TIMETAG_NULL,
				   3,
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@"), applicator),
				   lhs,
				   rhs);
}

t_osc_bndl osc_parse_allocApplicationWithLists(t_osc_region r, t_osc_list lhs, t_osc_bndl applicator, t_osc_list rhs)
{
	t_osc_msg lhsmsg;
	if(lhs){
		lhsmsg = osc_capi_msg_allocWithList(r, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/lhs"), lhs);
	}else{
		lhsmsg = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/lhs"));
	}
	t_osc_msg rhsmsg = osc_capi_msg_allocWithList(r, (void *)osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/rhs"), rhs);
	return osc_parse_allocApplication(r, lhsmsg, applicator, rhsmsg);
}

%}

%define "api.pure"
%locations
%require "2.4.2"

%parse-param{void *scanner}
%parse-param{t_osc_bndl *bndl}
%parse-param{const char *input_string}
%parse-param{t_osc_bndl *context}
%parse-param{t_osc_hashtab *ht}
%parse-param{t_osc_bndl *strict}
%parse-param{t_osc_bndl *nonstrict}
%parse-param{int *level}
%parse-param{t_osc_region r}

%lex-param{void *scanner}
%lex-param{t_osc_hashtab *ht}
%lex-param{t_osc_bndl *strict}
%lex-param{t_osc_bndl *nonstrict}
%lex-param{int *level}
%lex-param{t_osc_region r}

%union {
	t_osc_bndl bndl;
	t_osc_msg msg;
	t_osc_list list;
}
%type <bndl> bndl expr number msgaddress //applicator //funcall nth binop unaryop ternaryop 
%type <msg> msg
 //%type <atom> native expr 
%type <list> msgs expns list
%nonassoc <bndl> OSC_PARSE_SYMBOL OSC_PARSE_INT OSC_PARSE_FLOAT OSC_PARSE_STRING OSC_PARSE_BOOL OSC_PARSE_NIL OSC_PARSE_PATTERN
%left <bndl> OSC_PARSE_APPLICATOR
%token OSC_PARSE_NATIVE

//%type <atom> 
 //%nonassoc <atom>OSC_EXPR_NUM OSC_EXPR_STRING OSC_EXPR_OSCADDRESS OSC_EXPR_LAMBDA

// low to high precedence
// adapted from http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
 /*
// level 16
%right OSC_EXPR_PLUSEQ OSC_EXPR_MINUSEQ OSC_EXPR_MULTEQ OSC_EXPR_DIVEQ OSC_EXPR_MODEQ OSC_EXPR_POWEQ

// level 15
%right OSC_PARSE_TERNARYCOND ':' '?'

// level 14
%left '|' OSC_EXPR_OROR

// level 13
%left '&' OSC_EXPR_ANDAND

// level 9
%left '=' '~' OSC_EXPR_NEQ

// level 8
%left '<' '>' OSC_EXPR_LTE OSC_EXPR_GTE 

// level 6
%left '+' '-'

// level 5
%left '*' '/' '%'
%left '^' 

// level 3
 //%right OSC_EXPR_PREFIX_INC OSC_EXPR_PREFIX_DEC OSC_EXPR_UPLUS OSC_EXPR_UMINUS '!' OSC_EXPR_DBLQMARK OSC_EXPR_DBLQMARKEQ
 */

// level 2
 //%left OSC_EXPR_INC OSC_EXPR_DEC OSC_EXPR_FUNC_CALL OSC_EXPR_QUOTED_EXPR OPEN_DBL_BRKTS CLOSE_DBL_BRKTS '.'
%left '[' ']' '.' '\'' 
%left OSC_PARSE_FUNCALL
%left OSC_PARSE_LEFT_ASSOC
%left OSC_PARSE_RIGHT_ASSOC


%%

bndl: '{' '}' {
		$$ = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
		*bndl = $$;
 	}
	| '{' msgs '}' {
		$$ = (t_osc_bndl)$2;
		*bndl = $$;
  	}
	| '{' list '}' {
		$$ = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, (t_osc_msg)$2);
	}
;
/*
applicator:
	'@' OSC_PARSE_APPLICATOR
	| '!' '@' OSC_PARSE_APPLICATOR {
		$$ = 2;
	}
	| OSC_PARSE_APPLICATOR '!' {
		$$ = 1;
	}
	| '!' OSC_PARSE_APPLICATOR '!' {
		$$ = 2;
	}
;
*/
//msgaddress: OSC_PARSE_SYMBOL | OSC_PARSE_PATTERN | OSC_PARSE_APPLICATOR;
msgaddress: expr;
msg:
//msgaddress {
//$$ = osc_capi_msg_alloc(r, 1, $1);
//}
	msgaddress ':' expr {
		$$ = osc_capi_msg_alloc(r, 2, $1, $3);
	}
	| msgaddress ':' list {
		$$ = (t_osc_msg)osc_list_prepend_m(r, (t_osc_list_m)$3, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$1));
	}
	| expr {
		$$ = osc_capi_msg_alloc(r, 1, $1);
	}
;
msgs: msg {
		$$ = osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$1));
 	}
	| msgs ',' msg {
		$$ = osc_list_append_m(r, (t_osc_list_m)$1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$3));
 	}
;

list:
	'[' expns ']' {
		$$ = $2;
	}
;

number: OSC_PARSE_INT | OSC_PARSE_FLOAT;
expr:
	number | OSC_PARSE_SYMBOL | OSC_PARSE_STRING | OSC_PARSE_BOOL | OSC_PARSE_NIL | OSC_PARSE_PATTERN | bndl | OSC_PARSE_APPLICATOR 
	| '(' expr ')' {
		$$ = $2;
	}
	| OSC_PARSE_APPLICATOR expr {
		$$ = osc_parse_allocApplicationWithLists(r, NULL, $1, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$2)));
	}
	| expr OSC_PARSE_APPLICATOR expr {
		$$ = osc_parse_allocApplicationWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$1)), $2, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$3)));
	}
	| OSC_PARSE_APPLICATOR list {
		$$ = osc_parse_allocApplicationWithLists(r, NULL, $1, $2);
	}
	| expr OSC_PARSE_APPLICATOR list {
		$$ = osc_parse_allocApplicationWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$1)), $2, $3);
	}
	| list OSC_PARSE_APPLICATOR expr {
		$$ = osc_parse_allocApplicationWithLists(r, $1, $2, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$3)));
	}
	| list OSC_PARSE_APPLICATOR list {
		$$ = osc_parse_allocApplicationWithLists(r, $1, $2, $3);
	}
;
expns:
	expr {
		$$ = osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$1));
	}
	| expns ',' expr {
		$$ = osc_list_append_m(r, (t_osc_list_m)$1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)$3));
	}
;

