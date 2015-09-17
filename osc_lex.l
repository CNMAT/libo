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
#include "osc_hashtab.h"

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
pattern \/?({range}|[a-zA-Z0-9_\?])+

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

\+|-|\*|\/|%|^|!|=|\[|\]|:|\?|<|>|`|\.|&|\||,|;|\(|\)|\{|\}|@|~ {
	return *yytext;
}

\"(\\.|[^\\"])*\" {
			char *copy = osc_util_strcpy(yytext + 1);
			copy[yyleng - 2] = 0;
			yylval->atom = osc_atom_allocString(copy, 1);
			osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
			return OSC_PARSE_STRING;
			}

(([[:digit:]]{-}[0])[[:digit:]]*|0) 	{
						// int
						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							char *endptr = NULL;
							int32_t i = strtol(yytext, &endptr, 0);
							yylval->atom = osc_atom_allocInt32(i);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, (int)yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
						}
						return OSC_PARSE_NUMBER;
					}
{c99_floating_point_constant}		{
						// floating point
						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							char *endptr = NULL;
							double f = strtod(yytext, &endptr);
							yylval->atom = osc_atom_allocDouble(f);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
						}
						return OSC_PARSE_NUMBER;
					}
\'.\'					{
						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							yylval->atom = osc_atom_allocInt8(yytext[1]);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
						}
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

	/* \/?[^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`]+  { */
[^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`@][^.\+\-\*%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`]+  {
						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							yylval->atom = osc_atom_allocSymbol(osc_util_strcpy(yytext), 1);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
						}
						return OSC_PARSE_SYMBOL;
				 	}

{pattern}				{
						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							// fixme---this should identify only correctly formatted patterns
							printf("pattern: %s\n", yytext);
							yylval->atom = osc_atom_allocSymbol(osc_util_strcpy(yytext), 1);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
						}
						return OSC_PARSE_PATTERN;
				 	}

[^\'\"0-9\+\-\*\/%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`][^\'\"\+\-\*\/%\^!=\[\]:;,\?<>\&\|\\\{\}\(\) \n\t`]* {

						t_osc_atom *a = osc_hashtab_lookup(ht, (int)yyleng, yytext);
						if(a){
							yylval->atom = osc_atom_retain(a);
						}else{
							yylval->atom = osc_atom_allocString(osc_util_strcpy(yytext), 1);
							osc_atom_setPrettyPtr((t_osc_atom_m *)yylval->atom, yyleng, osc_util_strcpy(yytext));
							osc_hashtab_store(ht, (int)yyleng, osc_util_strcpy(yytext), (void *)yylval->atom);
}
						return OSC_PARSE_STRING;
					}

[ \t\n]+	/* eat up whitespace */



%%
