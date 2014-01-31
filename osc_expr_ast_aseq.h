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

/** 	\file osc_expr_ast_aseq.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_ASEQ_H__
#define __OSC_EXPR_AST_ASEQ_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_ast_aseq t_osc_expr_ast_aseq;

#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"
#include "osc_bundle_u.h"

int osc_expr_ast_aseq_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *oscbndl,
				   t_osc_atom_ar_u **out);
int osc_expr_ast_aseq_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_atom_ar_u **out);
long osc_expr_ast_aseq_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_aseq_formatLisp(char *buf, long n, t_osc_expr_ast_expr *ast);
t_osc_expr_ast_expr *osc_expr_ast_aseq_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_aseq_free(t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_aseq_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_aseq_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
t_osc_expr_ast_expr *osc_expr_ast_aseq_getMin(t_osc_expr_ast_aseq *e);
t_osc_expr_ast_expr *osc_expr_ast_aseq_getMax(t_osc_expr_ast_aseq *e);
t_osc_expr_ast_expr *osc_expr_ast_aseq_getStep(t_osc_expr_ast_aseq *e);
void osc_expr_ast_aseq_setMin(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *min);
void osc_expr_ast_aseq_setMax(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *max);
void osc_expr_ast_aseq_setStep(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *step);
t_osc_expr_ast_aseq *osc_expr_ast_aseq_alloc(t_osc_expr_ast_expr *min, t_osc_expr_ast_expr *max, t_osc_expr_ast_expr *step);

#ifdef __cplusplus
}
#endif

#endif
