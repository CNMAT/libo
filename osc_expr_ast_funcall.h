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

/** 	\file osc_expr_ast_funcall.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_FUNCALL_H__
#define __OSC_EXPR_AST_FUNCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_atom_array_u.h"

typedef struct _osc_expr_ast_funcall t_osc_expr_ast_funcall;

#include "osc_expr_funcrec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_builtin.h"
#include "osc_bundle_u.h"

int osc_expr_ast_funcall_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      t_osc_bndl_u *oscbndl,
				      t_osc_atom_ar_u **out);
int osc_expr_ast_funcall_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					  t_osc_expr_lexenv *lexenv,
					  t_osc_bndl_u *oscbndl,
					  t_osc_msg_u **assign_target,
					  long *nlvals,
					  t_osc_atom_u ***lvals);
long osc_expr_ast_funcall_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_funcall_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_funcall_copy(t_osc_expr_ast_expr *ast);
void osc_expr_ast_funcall_free(t_osc_expr_ast_expr *e);
t_osc_bndl_u *osc_expr_ast_funcall_toBndl(t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_funcall_fromBndl(t_osc_bndl_u *);
t_osc_expr_funcrec *osc_expr_ast_funcall_getFuncRec(t_osc_expr_ast_funcall *e);
t_osc_expr_ast_value *osc_expr_ast_funcall_getFunctionToken(t_osc_expr_ast_funcall *e);
t_osc_expr_builtin_funcptr osc_expr_ast_funcall_getFunc(t_osc_expr_ast_funcall *e);
t_osc_expr_builtin_lvalfuncptr osc_expr_ast_funcall_getLvalFunc(t_osc_expr_ast_funcall *e);
t_osc_expr_ast_expr *osc_expr_ast_funcall_getArgs(t_osc_expr_ast_funcall *e);
void osc_expr_ast_funcall_setArgs(t_osc_expr_ast_funcall *e, t_osc_expr_ast_expr *args);
int osc_expr_ast_funcall_getNumArgs(t_osc_expr_ast_funcall *e);
void osc_expr_ast_funcall_prependArg(t_osc_expr_ast_funcall *e, t_osc_expr_ast_expr *a);
void osc_expr_ast_funcall_appendArg(t_osc_expr_ast_funcall *e, t_osc_expr_ast_expr *a);
void osc_expr_ast_funcall_initWithList(t_osc_expr_ast_funcall *e,
				       int nodetype,
				       t_osc_expr_ast_expr *next,
				       t_osc_expr_ast_evalfn evalfn,
				       t_osc_expr_ast_evallvalfn evallvalfn,
				       t_osc_expr_ast_formatfn formatfn,
				       t_osc_expr_ast_formatfn format_lispfn,
				       t_osc_expr_ast_freefn freefn,
				       t_osc_expr_ast_copyfn copyfn,
				       t_osc_expr_ast_tobndlfn tobndlfn,
				       t_osc_expr_ast_frombndlfn frombndlfn,
				       size_t objsize,
				       t_osc_expr_funcrec *rec,
				       t_osc_expr_ast_value *function_token,
				       t_osc_expr_ast_expr *argv);
void osc_expr_ast_funcall_init(t_osc_expr_ast_funcall *e,
			       int nodetype,
			       t_osc_expr_ast_expr *next,
			       t_osc_expr_ast_evalfn evalfn,
			       t_osc_expr_ast_evallvalfn evallvalfn,
			       t_osc_expr_ast_formatfn formatfn,
			       t_osc_expr_ast_formatfn format_lispfn,
			       t_osc_expr_ast_freefn freefn,
			       t_osc_expr_ast_copyfn copyfn,
			       t_osc_expr_ast_tobndlfn tobndlfn,
			       t_osc_expr_ast_frombndlfn frombndlfn,
			       size_t objsize,
			       t_osc_expr_funcrec *rec,
			       t_osc_expr_ast_value *function_token,
			       int argc,
			       ...);
t_osc_expr_ast_funcall *osc_expr_ast_funcall_allocWithList(t_osc_expr_funcrec *rec, t_osc_expr_ast_value *function_token, t_osc_expr_ast_expr *argv);
t_osc_expr_ast_funcall *osc_expr_ast_funcall_alloc(t_osc_expr_funcrec *rec, t_osc_expr_ast_value *function_token, int argc, ...);

#ifdef __cplusplus
}
#endif

#endif
