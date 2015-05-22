/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2015, The Regents of
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

/** 	\file oex_lex.l
	\author John MacCallum

*/
%{
#include "osc.h"
#include "osc_parse.h"
#include "osc_util.h"

int colnum;

#define YY_USER_INIT {yylineno = 0;colnum = 0; yylloc->first_line = 0; yylloc->first_column = 0; yylloc->last_column = 0; yylloc->last_line = 0;}
#define YY_USER_ACTION {yylloc->first_line = yylineno; yylloc->first_column = colnum; colnum=colnum+yyleng; yylloc->last_column = colnum; yylloc->last_line = yylineno;}

int osc_lex_wrap(yyscan_t scanner){
	return 1;
}

%}

dseq      ([[:digit:]]+)
dseq_opt  ([[:digit:]]*)
frac      (({dseq_opt}"."{dseq})|{dseq}".")
exp       ([eE][+-]?{dseq})
exp_opt   ({exp}?)
fsuff     [flFL]
fsuff_opt ({fsuff}?)
hpref     (0[xX])
hdseq     ([[:xdigit:]]+)
hdseq_opt ([[:xdigit:]]*)
hfrac     (({hdseq_opt}"."{hdseq})|({hdseq}"."))
bexp      ([pP][+-]?{dseq})
dfc       (({frac}{exp_opt}{fsuff_opt})|({dseq}{exp}{fsuff_opt}))
hfc       (({hpref}{hfrac}{bexp}{fsuff_opt})|({hpref}{hdseq}{bexp}{fsuff_opt}))
c99_floating_point_constant  ({dfc}|{hfc})

quoted_string [^\"\\]
oscaddress \/[^\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\)\"\'` \n\t]+
reserved_chars \+|-|\*|\/|%|^|!|=|\[|\]|:|;|\?|<|>|\&|\||\\|\{|\}|\(|\)

%option reentrant
%option bison-bridge
%option bison-locations
%option noinput
%option nounput

%x quote quote_function escape

%%

#.*

\+|-|\*|\/|%|^|!|=|\[|\]|:|\?|<|>|`|\.|&|\||,|;|\(|\)|\{|\} {
	return *yytext;
}
	/*
\<= {
	return OSC_EXPR_LTE;
}
>= {
	return OSC_EXPR_GTE;
}
!= {
	return OSC_EXPR_NEQ;
}
\&\& {
	return OSC_EXPR_ANDAND;
}
\|\| {
	return OSC_EXPR_OROR;
}
\+\+ {
	return OSC_EXPR_INC;
}
\-\- {
	return OSC_EXPR_DEC;
}
\+= {
	return OSC_EXPR_PLUSEQ;
}
\-= {
	return OSC_EXPR_MINUSEQ;
}
\*= {
	return OSC_EXPR_MULTEQ;
}
\/= {
	return OSC_EXPR_DIVEQ;
}
%= {
	return OSC_EXPR_MODEQ;
}
\^= {
	return OSC_EXPR_POWEQ;
}
\?\? {
	return OSC_EXPR_DBLQMARK;
}
\?\?= {
	return OSC_EXPR_DBLQMARKEQ;
}

quote[[:space:]]*\( {
	BEGIN(quote_function);
	}

<quote_function>.* {
	char *start = yytext;
	char *end = start;
	int np = 1;
	while(np){
		if(*end == ')'){
			np--;
		}else if(*end == '('){
			np++;
		}
		end++;
	}
	yyless(end - start);
	if(alloc_atom){
		yylval->atom = osc_atom_u_alloc();
	}
	*(end - 1) = '\0';
	osc_atom_u_setString(yylval->atom, yytext);
	*(end - 1) = ')';
	BEGIN(0);
	return OSC_EXPR_QUOTED_EXPR;
	}

\" {
	BEGIN(quote);
}
	*/
	/*
<quote>[^\"]+ { 
	if(*buf){
		*buf = osc_mem_resize(*buf, *buflen + yyleng + 1);
		*buflen += yyleng + 1;
		strcat(*buf, yytext);
	}else{
		//char *copy = osc_mem_alloc(yyleng + 1);
		//strncpy(copy, yytext, yyleng + 1);
		//t_osc_atom_u *a = osc_atom_u_alloc();
		//yylval->atom = a;
		if(alloc_atom){
			yylval->atom = osc_atom_u_alloc();
		}
		osc_atom_u_setString(yylval->atom, yytext);
		return OSC_EXPR_STRING;
	}
}

<quote>\" {
	BEGIN(0);
	if(*buf){
		t_osc_atom_u *a = osc_atom_u_alloc();
		//osc_atom_u_setStringPtr(a, *buf);
		//osc_atom_u_setShouldFreePtr(a, 1);
		yylval->atom = osc_atom_u_allocString(*buf);
		//yylval->string = *buf;
		*buf = NULL;
		*buflen = 0;
		return OSC_EXPR_STRING;
	}
}

<quote>{quoted_string}*\\. {
	if(*buf){
		*buf = osc_mem_resize(*buf, *buflen + yyleng);
		memcpy(*buf + *buflen, yytext, yyleng - 1);
		*buflen += yyleng - 1;
		(*buf)[*buflen - 1] = yytext[yyleng - 1];
		(*buf)[*buflen] = '\0';
	}else{
		*buf = osc_mem_alloc(yyleng);
		memcpy(*buf, yytext, yyleng - 1);
		*buflen = yyleng - 1;
		(*buf)[*buflen - 1] = yytext[yyleng - 1];
		(*buf)[*buflen] = '\0';
	}
}
	*/

\"(\\.|[^\\"])*\" {
			char *copy = osc_util_strcpy(yytext + 1);
			copy[yyleng - 2] = 0;
			yylval->atom = osc_atom_allocString(copy, 1);
			return OSC_PARSE_STRING;
			}

(([[:digit:]]{-}[0])[[:digit:]]*|0) 	{
						// int 
						char *endptr = NULL;
						int32_t i = strtol(yytext, &endptr, 0);
						yylval->atom = osc_atom_allocInt32(i);
						return OSC_PARSE_NUMBER;
					}
{c99_floating_point_constant}		{
						// floating point 
						char *endptr = NULL;
						double f = strtod(yytext, &endptr);
						yylval->atom = osc_atom_allocDouble(f);
						return OSC_PARSE_NUMBER;
					}
\'.\'					{
						yylval->atom = osc_atom_allocInt8(yytext[1]);
						return OSC_PARSE_NUMBER;
					}
(true)			{
						yylval->atom = osc_atom_true;
						return OSC_PARSE_BOOL;
					}
(false)			{
						yylval->atom = osc_atom_false;
						return OSC_PARSE_BOOL;
					}

(nil)				{
						yylval->atom = osc_atom_nil;
						return OSC_PARSE_NIL;
					}

(undefined)				{
						yylval->atom = osc_atom_undefined;
						return OSC_PARSE_UNDEFINED;
					}

"__native"			{
					return OSC_PARSE_NATIVE;
				}

\/?[^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`]+  {
						yylval->atom = osc_atom_allocSymbol(osc_util_strcpy(yytext), 1);
						return OSC_PARSE_SYMBOL;
				 	}

[^\'\"0-9\+\-\*\/%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`][^\'\"\+\-\*\/%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`]* {
						yylval->atom = osc_atom_allocString(osc_util_strcpy(yytext), 1);
						return OSC_PARSE_STRING;
					}

[ \t\n]+	/* eat up whitespace */



%%
