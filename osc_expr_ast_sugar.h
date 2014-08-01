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

/** 	\file osc_expr_ast_sugar.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_SUGAR_H__
#define __OSC_EXPR_AST_SUGAR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_ast_sugar t_osc_expr_ast_sugar;

#include "osc_expr_ast_expr.h"
#include "osc_expr.h"

int osc_expr_ast_sugar_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out);
t_osc_expr_ast_expr *osc_expr_ast_sugar_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_sugar_free(t_osc_expr_ast_expr *e);
long osc_expr_ast_sugar_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_sugar_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_sugar_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_sugar_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
t_osc_expr_ast_expr *osc_expr_ast_sugar_getParsedAST(t_osc_expr_ast_sugar *s);
t_osc_expr_ast_expr *osc_expr_ast_sugar_getFunctionalAST(t_osc_expr_ast_sugar *s);
t_osc_expr_ast_sugar *osc_expr_ast_sugar_alloc(t_osc_expr_ast_expr *parsed_ast, t_osc_expr_ast_expr *functional_ast);

#ifdef __cplusplus
}
#endif

#endif
