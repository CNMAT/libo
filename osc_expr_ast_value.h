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

/** 	\file osc_expr_ast_value.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_VALUE_H__
#define __OSC_EXPR_AST_VALUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_atom_u.h"

typedef struct _osc_expr_ast_value t_osc_expr_ast_value;

int osc_expr_ast_value_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out);
long osc_expr_ast_value_format(char *buf, long n, t_osc_expr_ast_expr *v);
t_osc_expr_ast_expr *osc_expr_ast_value_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_value_free(t_osc_expr_ast_expr *v);
t_osc_atom_u *osc_expr_ast_value_getValue(t_osc_expr_ast_value *v);
char osc_expr_ast_value_getValueType(t_osc_expr_ast_value *v);
void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, t_osc_atom_u *a);
t_osc_expr_ast_value *osc_expr_ast_value_alloc(t_osc_atom_u *a);

#ifdef __cplusplus
}
#endif

#endif
