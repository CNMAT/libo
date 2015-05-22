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

#include "osc_parse.h"
#include "osc_lex.h"
#include "osc_util.h"


	//#define OSC_EXPR_PARSER_DEBUG
#ifdef OSC_EXPR_PARSER_DEBUG
#define PP(fmt, ...)printf(fmt, ##__VA_ARGS__)
#else
#define PP(fmt, ...)
#endif

#ifdef __cplusplus
	extern "C" int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner);
#else
	int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner);
#endif

}
%code requires{
#include "osc.h"
#include "osc_mem.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_pvec.h"

#ifdef YY_DECL
#undef YY_DECL
#endif

#ifdef __cplusplus
	#define YY_DECL extern "C" int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner)
#else
	#define YY_DECL int osc_lex_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner)
#endif
#ifdef __cplusplus
extern "C"{
#endif
t_osc_bndl *osc_parse(char *ptr);
#ifdef __cplusplus
}
#endif
}

%{

int osc_parse_lex(YYSTYPE *yylval_param, YYLTYPE *llocp, yyscan_t yyscanner)
{
	return osc_lex_lex(yylval_param, llocp, yyscanner);
}

t_osc_bndl *osc_parse(char *ptr)
{
	yyscan_t scanner;
	osc_lex_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_lex__scan_string(ptr, scanner);
	osc_lex_set_out(NULL, scanner);
	t_osc_bndl *b = NULL;
	osc_parse_parse(scanner, &b);
	osc_lex__delete_buffer(buf_state, scanner);
	osc_lex_lex_destroy(scanner);
	return b;
}
/*
void osc_parse_error_formatLocation(YYLTYPE *llocp, char *input_string, char **buf)
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

void osc_parse_error(YYLTYPE *llocp,
		    char *input_string,
		    t_osc_err errorcode,
		    const char * const moreinfo_fmt,
		    ...)
{
}
*/
void yyerror(YYLTYPE *llocp, void *scanner, t_osc_bndl **bndl, char const *e)
{
}

t_osc_pvec2 *osc_parse_atom(t_osc_atom *a)
{
	t_osc_pvec2 *pvec2 = osc_pvec2_alloc(osc_atom_release);
	// leave room for the address
	osc_pvec2_assocN_m(pvec2, 0, (void *)osc_atom_undefined);
	osc_pvec2_assocN_m(pvec2, 1, (void *)a);
	return pvec2;
}
 
%}

%define "api.pure"
%locations
%require "2.4.2"

%parse-param{void *scanner}
%parse-param{t_osc_bndl **bndl}

%lex-param{void *scanner}

%union {
	t_osc_bndl *bndl;
	t_osc_msg *msg;
	t_osc_atom *atom;
	t_osc_pvec2 *pvec2;
}
%type <bndl> bndl binop expr funcall
%type <msg> msg
%type <atom> atom native
%type <pvec2> msgs atoms
%nonassoc <atom> OSC_PARSE_SYMBOL OSC_PARSE_NUMBER OSC_PARSE_STRING OSC_PARSE_BOOL OSC_PARSE_NIL OSC_PARSE_UNDEFINED
%token OSC_PARSE_NATIVE
/*
//%type <atom> 
%nonassoc <atom>OSC_EXPR_NUM OSC_EXPR_STRING OSC_EXPR_OSCADDRESS OSC_EXPR_LAMBDA

// low to high precedence
// adapted from http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B

// level 16
%right '=' OSC_EXPR_PLUSEQ OSC_EXPR_MINUSEQ OSC_EXPR_MULTEQ OSC_EXPR_DIVEQ OSC_EXPR_MODEQ OSC_EXPR_POWEQ

// level 15
%right OSC_EXPR_TERNARY_COND '?' ':'

// level 14
%left '|' OSC_EXPR_OROR

// level 13
%left '&' OSC_EXPR_ANDAND

// level 9
%left OSC_EXPR_EQ OSC_EXPR_NEQ

// level 8
%left '<' '>' OSC_EXPR_LTE OSC_EXPR_GTE 

// level 6
%left '+' '-'

// level 5
%left '*' '/' '%'
%left '^' 

// level 3
%right OSC_EXPR_PREFIX_INC OSC_EXPR_PREFIX_DEC OSC_EXPR_UPLUS OSC_EXPR_UMINUS '!' OSC_EXPR_DBLQMARK OSC_EXPR_DBLQMARKEQ

// level 2
%left OSC_EXPR_INC OSC_EXPR_DEC OSC_EXPR_FUNC_CALL OSC_EXPR_QUOTED_EXPR OPEN_DBL_BRKTS CLOSE_DBL_BRKTS '.'
*/
 //%token START_EXPNS START_FUNCTION
 //%start start

%%

bndl: '{' '}' {
		$$ = osc_bndl_empty;
		*bndl = $$;
 	}
	| '{' msgs '}' {
		$$ = osc_bndl_allocWithPvec2(OSC_TIMETAG_NULL, $2);
		*bndl = $$;
  	}
;

msg: OSC_PARSE_SYMBOL {
		$$ = osc_msg_alloc($1, 0);
 	}
	| OSC_PARSE_SYMBOL ':' atoms {
		osc_pvec2_assocN_m($3, 0, (void *)$1);
		$$ = osc_msg_allocWithPvec2($3);
	}
;

msgs: msg {
		t_osc_pvec2 *pvec2 = osc_pvec2_alloc(osc_msg_release);
		osc_pvec2_assocN_m(pvec2, 0, (void *)$1);
		$$ = pvec2;
 	}
	| msgs ',' msg {
		osc_pvec2_assocN_m($1, osc_pvec2_length($1), (void *)$3);
		$$ = $1;
 	}
;

atom:
	OSC_PARSE_NUMBER | OSC_PARSE_STRING | OSC_PARSE_BOOL | OSC_PARSE_NIL | OSC_PARSE_UNDEFINED | OSC_PARSE_SYMBOL | native
	| bndl {
		$$ = osc_atom_allocBndl($1, 1);
 	}
	| expr {
		$$ = osc_atom_allocExpr($1, 1);
	}
;

atoms:
	atom {
		$$ = osc_parse_atom($1);
	}
	| '[' atom ']' {
		$$ = osc_parse_atom($2);
 	}
	| '[' atoms ',' atom ']' {
		osc_pvec2_assocN_m($2, osc_pvec2_length($2), (void *)$4);
		$$ = $2;
	}
;

binop:
	OSC_PARSE_NUMBER '+' OSC_PARSE_NUMBER {
		$$ = NULL;
	}
	| bndl '\\' bndl {
		$$ = NULL;
	}
;

funcall:
	bndl bndl {
		$$ = NULL;
	}
	| OSC_PARSE_SYMBOL bndl {
		$$ = osc_bndl_alloc(OSC_TIMETAG_NULL, 3,
				    osc_msg_alloc(osc_atom_allocSymbol("/type", 0), 1, osc_atom_allocInt8('f')),
				    osc_msg_alloc(osc_atom_allocSymbol("/func", 0), 1, $1),
				    osc_msg_alloc(osc_atom_allocSymbol("/args", 0), 1, osc_atom_allocBndl($2, 1)));
	}
	| OSC_PARSE_SYMBOL OSC_PARSE_SYMBOL {
		$$ = NULL;
	}
;

expr:
	binop | funcall;

native:
	OSC_PARSE_NATIVE OSC_PARSE_SYMBOL {
		osc_atom_format_m($2, 0);
		t_osc_builtin f = osc_builtin_lookup(osc_atom_getPrettyPtr($2));
		if(f){
			$$ = osc_atom_allocNative(f, osc_atom_getPrettyPtr($2));
		}else{
			$$ = NULL;
		}
	}
;

