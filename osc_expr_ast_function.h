/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-13, The Regents of
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

/** 	\file osc_expr_ast_function.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_FUNCTION_H__
#define __OSC_EXPR_AST_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_expr_rec.h"

typedef struct _osc_expr_ast_function t_osc_expr_ast_function;

int osc_expr_ast_function_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       long *len,
				       char **oscbndl,
				       t_osc_atom_ar_u **out);
long osc_expr_ast_function_format(char *buf, long n, t_osc_expr_ast_expr *f);
void osc_expr_ast_function_free(t_osc_expr_ast_expr *f);
t_osc_expr_rec *osc_expr_ast_function_getFunction(t_osc_expr_ast_function *f);
char *osc_expr_ast_function_getName(t_osc_expr_ast_function *f);
int osc_expr_ast_function_getNumParams(t_osc_expr_ast_function *f);
char **osc_expr_ast_function_getParams(t_osc_expr_ast_function *f);
t_osc_expr_ast_expr *osc_expr_ast_function_getExprList(t_osc_expr_ast_function *f);
t_osc_expr_ast_function *osc_expr_ast_function_alloc(int numparams, char **params, t_osc_expr_ast_expr *exprs);

#ifdef __cplusplus
}
#endif

#endif
