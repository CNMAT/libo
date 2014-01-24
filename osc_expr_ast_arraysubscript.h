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

/** 	\file osc_expr_ast_arraysubscript.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_ARRAYSUBSCRIPT_H__
#define __OSC_EXPR_AST_ARRAYSUBSCRIPT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_ast_arraysubscript t_osc_expr_ast_arraysubscript;

#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"

int osc_expr_ast_arraysubscript_evalInLexEnv(t_osc_expr_ast_expr *ast,
	t_osc_expr_lexenv *lexenv,
	long *len,
	char **oscbndl,
	t_osc_atom_ar_u **out);
long osc_expr_ast_arraysubscript_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_arraysubscript_formatLisp(char *buf, long n, t_osc_expr_ast_expr *ast);
t_osc_expr_ast_expr *osc_expr_ast_arraysubscript_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_arraysubscript_free(t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_arraysubscript_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_arraysubscript_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
t_osc_expr_ast_expr *osc_expr_ast_arraysubscript_getBase(t_osc_expr_ast_arraysubscript *e);
t_osc_expr_ast_expr *osc_expr_ast_arraysubscript_getIndexList(t_osc_expr_ast_arraysubscript *e);
void osc_expr_ast_arraysubscript_setBase(t_osc_expr_ast_arraysubscript *e, t_osc_expr_ast_expr *base);
void osc_expr_ast_arraysubscript_setIndexList(t_osc_expr_ast_arraysubscript *e, t_osc_expr_ast_expr *index_list);
t_osc_expr_ast_arraysubscript *osc_expr_ast_arraysubscript_alloc(t_osc_expr_ast_expr *base, t_osc_expr_ast_expr *index_list);

#ifdef __cplusplus
}
#endif

#endif
