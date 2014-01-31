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

/** 	\file osc_expr_ast_binaryop.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_BINARYOP_H__
#define __OSC_EXPR_AST_BINARYOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_atom_array_u.h"

typedef struct _osc_expr_ast_binaryop t_osc_expr_ast_binaryop;

#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"
#include "osc_bundle_u.h"

int osc_expr_ast_binaryop_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_atom_ar_u **out);
int osc_expr_ast_binaryop_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					   t_osc_expr_lexenv *lexenv,
					   t_osc_bndl_u *oscbndl,
					   t_osc_atom_ar_u **out);
long osc_expr_ast_binaryop_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_binaryop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_binaryop_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_binaryop_free(t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_binaryop_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_binaryop_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
t_osc_expr_oprec *osc_expr_ast_binaryop_getOpRec(t_osc_expr_ast_binaryop *e);
void osc_expr_ast_binaryop_setOpRec(t_osc_expr_ast_binaryop *e, t_osc_expr_oprec *r);
t_osc_expr_ast_expr *osc_expr_ast_binaryop_getLeftArg(t_osc_expr_ast_binaryop *e);
t_osc_expr_ast_expr *osc_expr_ast_binaryop_getRightArg(t_osc_expr_ast_binaryop *e);
void osc_expr_ast_binaryop_setLeftArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *left);
void osc_expr_ast_binaryop_setRightArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *right);
t_osc_expr_ast_binaryop *osc_expr_ast_binaryop_alloc(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *left, t_osc_expr_ast_expr *right);

#ifdef __cplusplus
}
#endif

#endif
