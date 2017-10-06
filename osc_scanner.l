/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-14, The Regents of
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

/** 	\file osc_scanner.l
	\author John MacCallum

*/

%{
#include "osc_parser.h"
#include "osc_mem.h"
#include "osc_timetag.h"

int osc_scanner_wrap(yyscan_t scanner){
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

string [^ \t\"\n\\\{\}\'\[\],]*
quoted_string [^\"\\]

%x quote escape

%option reentrant
%option bison-bridge
%option bison-locations
%option noinput
%option nounput

%%

\" {
	BEGIN(quote);
}

<quote>{quoted_string}+ {
	if(*buf){
		*buf = osc_mem_resize(*buf, *buflen + yyleng + 1);
		*buflen += yyleng + 1;
		strcat(*buf, yytext);
	}else{
		yylval->atom = osc_atom_u_allocWithString(yytext);
		return OSCVALUE;
	}
}

<quote>\" {
	BEGIN(0);
	if(*buf){
		yylval->atom = osc_atom_u_allocWithString(*buf);
		*buf = NULL;
		*buflen = 0;
		return OSCVALUE;
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

\"\" {
    yylval->atom = osc_atom_u_alloc();
    osc_atom_u_setString(yylval->atom, "");
    return OSCVALUE;
}

-?{c99_floating_point_constant}	{
					char *endp = NULL;
					double d = strtod(yytext, &endp); 
					yylval->atom = osc_atom_u_allocWithDouble(d);
					return OSCVALUE;
				}

true			{
						yylval->atom = osc_atom_u_alloc();
						osc_atom_u_setTrue(yylval->atom);
						return OSCVALUE;
					}
false			{
						yylval->atom = osc_atom_u_alloc();
						osc_atom_u_setFalse(yylval->atom);
						return OSCVALUE;
					}

nil					{
						yylval->atom = osc_atom_u_alloc();
						osc_atom_u_setNil(yylval->atom);
						return OSCVALUE;
					}

:|\{|\}|, {
		return *yytext;
	}

\[|\]	{
		return *yytext;
	}

\'.\' {
	yylval->atom = osc_atom_u_alloc();
	osc_atom_u_setInt8(yylval->atom, yytext[1]);
	return OSCVALUE;
}

(-?([[:digit:]]{-}[0])[[:digit:]]*|0) 	{
						char *endp = NULL;
					        long i = strtol(yytext, &endp, 0);
						yylval->atom = osc_atom_u_allocWithInt32(i);
						return OSCVALUE;
					}

\/[^ \t\n#]+	{
			if(yytext[yyleng - 2] == ','){
				yyless(yyleng - 2);
			}
			yylval->atom = osc_atom_u_allocWithString(yytext);
			return OSCADDRESS;
		}

(-?(?:[1-9][0-9]*)?[0-9]{4})-(1[0-2]|0[1-9])-(3[0-1]|0[1-9]|[1-2][0-9])?T(2[0-3]|[0-1][0-9]):([0-5][0-9]):([0-5][0-9])(\.[0-9]+)??(Z|[+-](?:2[0-3]|[0-1][0-9]):[0-5][0-9])? { /* ISO 8601 from Regular Expressions Cookbook */
	t_osc_timetag t;
	osc_timetag_fromISO8601(yytext, &t);
	yylval->atom = osc_atom_u_allocWithTimetag(t);
	return OSCVALUE;
}

[ \t\n]+

. {return *yytext;}

%%
