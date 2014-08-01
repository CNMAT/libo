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

/** 	\file osc_expr_builtin.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_BUILTIN_H__
#define __OSC_EXPR_BUILTIN_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "osc_expr_lexenv.h"
#include "osc_atom_array_u.h"
struct _osc_expr_ast_funcall;
//typedef int (*t_osc_expr_builtin_funcptr)(struct _osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
//typedef int (*t_osc_expr_builtin_lvalfuncptr)(struct _osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_msg_u **assign_target, long *nlvals, t_osc_atom_u ***lvals);
//typedef int (*t_osc_expr_builtin_specfuncptr)(struct _osc_expr_ast_funcall *f, t_osc_expr_lexenv *lexenv, t_osc_bndl_u *oscbndl, t_osc_atom_ar_u **out);
typedef int (*t_osc_expr_builtin_funcptr)(struct _osc_expr_ast_expr *f,
					  t_osc_expr_lexenv *lexenv,
					  t_osc_bndl_u *oscbndl,
					  long argc,
					  t_osc_atom_ar_u **argv,
					  t_osc_atom_ar_u **out);
typedef t_osc_expr_builtin_funcptr t_osc_expr_builtin_lvalfuncptr;

//#define OSC_EXPR_BUILTIN_DECL(name) int osc_expr_builtin_##name (t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
//#define OSC_EXPR_BUILTIN_LVAL_DECL(name) int osc_expr_builtin_lval_##name (t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_msg_u **assign_target, long *nlvals, t_osc_atom_u ***lvals)

#include <inttypes.h>
#include <math.h>
#include "osc.h"
#include "osc_expr_oprec.h"
#include "osc_atom_u.h"
#include "osc_expr_parser.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_function.h"


// built-in operators
extern t_osc_expr_oprec *osc_expr_builtin_op_add;
extern t_osc_expr_oprec *osc_expr_builtin_op_assign;

// built-in functions
extern t_osc_expr_funcrec *osc_expr_builtin_func_add;
extern t_osc_expr_funcrec *osc_expr_builtin_func_assign;
extern t_osc_expr_funcrec *osc_expr_builtin_func_apply;
extern t_osc_expr_funcrec *osc_expr_builtin_func_nth;
extern t_osc_expr_funcrec *osc_expr_builtin_func_list;
extern t_osc_expr_funcrec *osc_expr_builtin_func_aseq;
extern t_osc_expr_funcrec *osc_expr_builtin_func_if;
extern t_osc_expr_funcrec *osc_expr_builtin_func_eval;
extern t_osc_expr_funcrec *osc_expr_builtin_func_message;
extern t_osc_expr_funcrec *osc_expr_builtin_func_bundle;

t_osc_expr_funcrec *osc_expr_builtin_lookupFunction(char *name);
t_osc_expr_oprec *osc_expr_builtin_lookupOperator(char *op);
t_osc_expr_oprec *osc_expr_builtin_lookupOperatorForOpcode(char op);
t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOperator(t_osc_expr_oprec *op);
t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOpcode(char op);

int osc_expr_builtin_apply(t_osc_expr_ast_expr *f,
			   t_osc_expr_lexenv *lexenv,
			   t_osc_bndl_u *oscbndl,
			   long argc,
			   t_osc_atom_ar_u **argv,
			   t_osc_atom_ar_u **out);
int osc_expr_builtin_applyLambda(t_osc_expr_ast_expr *ast,
			         t_osc_expr_ast_function *lambda,
				 long numargs,
				 t_osc_expr_ast_expr *args,
				 t_osc_expr_lexenv *lexenv,
				 t_osc_bndl_u *oscbndl,
				 long argc,
				 t_osc_atom_ar_u **argv,
				 t_osc_atom_ar_u **out);

#ifdef _cplusplus
}
#endif

#endif // __OSC_EXPR_BUILTIN_H__
