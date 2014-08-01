/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-12, The Regents of
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

/** 	\file osc_expr_lval.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_LVAL_H__
#define __OSC_EXPR_LVAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_expr_ast_expr.h"

typedef struct _osc_expr_lval t_osc_expr_lval;

t_osc_expr_lval *osc_expr_lval_alloc(void);
void osc_expr_lval_free(t_osc_expr_lval *lval);
t_osc_bndl_u *osc_expr_lval_getBndl(t_osc_expr_lval *lval);
t_osc_msg_u *osc_expr_lval_getMsg(t_osc_expr_lval *lval);
t_osc_atom_ar_u *osc_expr_lval_getAtoms(t_osc_expr_lval *lval);
char *osc_expr_lval_getAddress(t_osc_expr_lval *lval);
t_osc_expr_ast_expr *osc_expr_lval_getLvalExpr(t_osc_expr_lval *lval);
t_osc_atom_ar_u *osc_expr_lval_getIndexes(t_osc_expr_lval *lval);

void osc_expr_lval_setBndl(t_osc_expr_lval *lval, t_osc_bndl_u *bndl);
void osc_expr_lval_setMsg(t_osc_expr_lval *lval, t_osc_msg_u *msg);
void osc_expr_lval_setAtoms(t_osc_expr_lval *lval, t_osc_atom_ar_u *ar);
void osc_expr_lval_setAddress(t_osc_expr_lval *lval, char *address);
void osc_expr_lval_setLvalExpr(t_osc_expr_lval *lval, t_osc_expr_ast_expr *lval_expr);
void osc_expr_lval_setIndexes(t_osc_expr_lval *lval, t_osc_atom_ar_u *indexes);

#ifdef __cplusplus
}
#endif

#endif
