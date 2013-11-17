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

/** 	\file osc_expr_ast_unaryop.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_UNARYOP_H__
#define __OSC_EXPR_AST_UNARYOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_atom_array_u.h"

typedef struct _osc_expr_ast_unaryop t_osc_expr_ast_unaryop;

//typedef int (*t_osc_expr_funcptr)(t_osc_expr_ast_unaryop *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

#include "osc_expr.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_rec.h"

#define OSC_EXPR_AST_UNARYOP_LEFT 1
#define OSC_EXPR_AST_UNARYOP_RIGHT 2

int osc_expr_ast_unaryop_evalInLexEnv(t_osc_expr_ast_expr *ast,
	t_osc_expr_lexenv *lexenv,
	long *len,
	char **oscbndl,
	t_osc_atom_ar_u **out);
long osc_expr_ast_unaryop_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_unaryop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_unaryop_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_unaryop_free(t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_unaryop_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_unaryop_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
t_osc_expr_rec *osc_expr_ast_unaryop_getRec(t_osc_expr_ast_unaryop *e);
void osc_expr_ast_unaryop_setRec(t_osc_expr_ast_unaryop *e, t_osc_expr_rec *r);
t_osc_expr_rec *osc_expr_ast_unaryop_getRecCopy(t_osc_expr_ast_unaryop *e);
t_osc_expr_funcptr osc_expr_ast_unaryop_getFunc(t_osc_expr_ast_unaryop *e);
t_osc_expr_ast_expr *osc_expr_ast_unaryop_getArg(t_osc_expr_ast_unaryop *e);
void osc_expr_ast_unaryop_setArg(t_osc_expr_ast_unaryop *e, t_osc_expr_ast_expr *left);
int osc_expr_ast_unaryop_getSide(t_osc_expr_ast_unaryop *e);
void osc_expr_ast_unaryop_setSide(t_osc_expr_ast_unaryop *e, int side);
t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_alloc(t_osc_expr_rec *rec, t_osc_expr_ast_expr *arg, int side);
t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_allocLeft(t_osc_expr_rec *rec, t_osc_expr_ast_expr *arg);
t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_allocRight(t_osc_expr_rec *rec, t_osc_expr_ast_expr *arg);

#ifdef __cplusplus
}
#endif

#endif