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

/** 	\file osc_expr_privatedecls.h
	\author John MacCallum

*/

#include "osc_atom_u.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_builtins.h"

void osc_expr_err_badInfixArg(char *func, char typetag, int argnum, t_osc_atom_u *left, t_osc_atom_u *right);
void osc_expr_err_unbound(char *address, char *func); 
void osc_expr_err_argnum(unsigned int expected, unsigned int found, unsigned int optional_args_allowed, char *func);

OSC_EXPR_BUILTIN_DECL(apply);
OSC_EXPR_BUILTIN_DECL(map);
OSC_EXPR_BUILTIN_DECL(lreduce);
OSC_EXPR_BUILTIN_DECL(rreduce);
OSC_EXPR_BUILTIN_DECL(assign);
OSC_EXPR_BUILTIN_DECL(assigntoindex);
OSC_EXPR_BUILTIN_DECL(if);
OSC_EXPR_BUILTIN_DECL(emptybundle);
OSC_EXPR_BUILTIN_DECL(bound);
OSC_EXPR_BUILTIN_DECL(exists);
OSC_EXPR_BUILTIN_DECL(getaddresses);
OSC_EXPR_BUILTIN_DECL(delete);
OSC_EXPR_BUILTIN_DECL(getmsgcount);
OSC_EXPR_BUILTIN_DECL(value);
OSC_EXPR_BUILTIN_DECL(quote);
OSC_EXPR_BUILTIN_DECL(eval);
OSC_EXPR_BUILTIN_DECL(tokenize);
OSC_EXPR_BUILTIN_DECL(gettimetag);
OSC_EXPR_BUILTIN_DECL(settimetag);
OSC_EXPR_BUILTIN_DECL(lookup);

#define OSC_EXPR_SPECFUNC_DECL(name) int osc_expr_specFunc_##name(t_osc_expr_ast_expr *f, \
								  t_osc_expr_lexenv *lexenv, \
								  long *len, \
								  char **oscbndl, \
								  t_osc_atom_ar_u **out)

OSC_EXPR_SPECFUNC_DECL(apply);
OSC_EXPR_SPECFUNC_DECL(map);
OSC_EXPR_SPECFUNC_DECL(reduce);
OSC_EXPR_SPECFUNC_DECL(assign);
OSC_EXPR_SPECFUNC_DECL(assigntoindex);
OSC_EXPR_SPECFUNC_DECL(if);
OSC_EXPR_SPECFUNC_DECL(emptybundle);
OSC_EXPR_SPECFUNC_DECL(bound);
OSC_EXPR_SPECFUNC_DECL(exists);
OSC_EXPR_SPECFUNC_DECL(getaddresses);
OSC_EXPR_SPECFUNC_DECL(delete);
OSC_EXPR_SPECFUNC_DECL(getmsgcount);
OSC_EXPR_SPECFUNC_DECL(value);
OSC_EXPR_SPECFUNC_DECL(quote);
OSC_EXPR_SPECFUNC_DECL(eval);
OSC_EXPR_SPECFUNC_DECL(tokenize);
OSC_EXPR_SPECFUNC_DECL(gettimetag);
OSC_EXPR_SPECFUNC_DECL(settimetag);
OSC_EXPR_SPECFUNC_DECL(lookup);

