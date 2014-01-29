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

/** 	\file osc_expr_ast_let.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_LET_H__
#define __OSC_EXPR_AST_LET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_expr_ast_value.h"
#include "osc_bundle_u.h"

typedef struct _osc_expr_ast_let t_osc_expr_ast_let;

int osc_expr_ast_let_evalInLexEnv(t_osc_expr_ast_expr *ast,
				  t_osc_expr_lexenv *lexenv,
				  t_osc_bndl_u *oscbndl,
				  t_osc_atom_ar_u **out);
long osc_expr_ast_let_format(char *buf, long n, t_osc_expr_ast_expr *f);
long osc_expr_ast_let_formatLisp(char *buf, long n, t_osc_expr_ast_expr *f);
void osc_expr_ast_let_free(t_osc_expr_ast_expr *f);
t_osc_err osc_expr_ast_let_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_let_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
void osc_expr_ast_let_setVarlist(t_osc_expr_ast_let *f, t_osc_expr_ast_expr *varlist);
void osc_expr_ast_let_setExprs(t_osc_expr_ast_let *f, t_osc_expr_ast_expr *exprs);
t_osc_expr_ast_expr *osc_expr_ast_let_getVarlist(t_osc_expr_ast_let *f);
t_osc_expr_ast_expr *osc_expr_ast_let_getExprs(t_osc_expr_ast_let *f);
t_osc_expr_ast_let *osc_expr_ast_let_alloc(t_osc_expr_ast_expr *varlist, t_osc_expr_ast_expr *exprs);

#ifdef __cplusplus
}
#endif

#endif
