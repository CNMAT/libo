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
#include "osc_typetag.h"
#include "osc_parse.h"
#include "osc_util.h"
#include "osc_hashtab.h"
#include "osc_capi_bundle.h"
#include "osc_capi_primitive.h"

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

range \[!?[a-zA-Z0-9]+-[a-zA-Z0-9]+\]
pattern \/?({range}|[a-zA-Z0-9_\?\*])+


illegal_symbols #
wildcards \*|\?
brackets \[|\]|\{|\}|\(|\)
semantic_symbols {wildcards}|{brackets}|\/|\"|\'|,|\.|@|!|:
alphanum [0-9a-zA-Z]
non_semantic_symbols \$|%|&|\+|\-|<|=|>|\?|\^|_|\||~|`

%option reentrant
%option bison-bridge
%option bison-locations
%option noinput
%option nounput

%x quote quote_function escape

%%

#.*

\{ {
	*level = *level + 1;
	return *yytext;
}

\} {
	*level = *level - 1;
	return *yytext;
}
	/*
\+|-|\*|\/|%|^|!|=|\?|<|>|`|&|\||@|~|\$ {
	//return *yytext;
	char buf[2] = {*yytext, 0};
	yylval->bndl = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, buf);
	printf("sym %s\n", buf);
	return OSC_PARSE_SYMBOL;
}
	*/

\"(\\.|[^\\"])*\" {
	// string
	char copy[yyleng];
	memcpy(copy, yytext + 1, yyleng - 1);
	copy[yyleng - 2] = 0;
	yylval->bndl = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, copy);
	return OSC_PARSE_STRING;
}

(([[:digit:]]{-}[0])[[:digit:]]*|0) 	{
	// int
	char *endp = NULL;
	int32_t i = strtol(yytext, &endp, 0);
	yylval->bndl = osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, i);
	return OSC_PARSE_INT;
}

{c99_floating_point_constant}		{
	// float
	char *endp = NULL;
	double f = strtod(yytext, &endp);
	yylval->bndl = osc_capi_primitive_double(r, OSC_TIMETAG_NULL, f);
	return OSC_PARSE_FLOAT;
}

\'.\'					{
	// int8 (char)
	yylval->bndl = osc_capi_primitive_int8(r, OSC_TIMETAG_NULL, yytext[1]);
	return OSC_PARSE_INT;
}

(true)			{
	yylval->bndl = osc_capi_primitive_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
	return OSC_PARSE_BOOL;
}

(false)			{
	yylval->bndl = osc_capi_primitive_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
	return OSC_PARSE_BOOL;
}

(nil)			{
	yylval->bndl = osc_capi_primitive_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
	return OSC_PARSE_NIL;
}
	/*
	(undefined)				{
						yylval->atom = osc_atom_undefined;
						return OSC_PARSE_UNDEFINED;
	}
	*/

	/*
	"__native"			{
					return OSC_PARSE_NATIVE;
	}
	*/

	/*
	[^\$.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`@][^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`@$]+  {
	[^\[\]:,\{\}\(\) \n\t`\'][^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`@$]+  {
	[^.\[\]:,\{\}\(\) \n\t`\'][^.\*\[\]:,\?<>\|\\\{\}\(\) \n\t]*  {
	*/

!?@!?({non_semantic_symbols}|[a-zA-Z_])* {
	yylval->bndl = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, yytext);
	return OSC_PARSE_APPLICATOR;
}

(\/?({non_semantic_symbols}|{alphanum})+)+ {
	yylval->bndl = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, yytext);
	return OSC_PARSE_SYMBOL;
}

{pattern}		{
	yylval->bndl = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, yytext);
	return OSC_PARSE_PATTERN;
}

{semantic_symbols} {
	return *yytext;
}



[ \t\n]+	/* eat up whitespace */



%%
