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

/** 	\file osc_expr_ast_expr_funcall.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_EXPR_FUNCALL_H__
#define __OSC_EXPR_AST_EXPR_FUNCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_ast_expr_funcall t_osc_expr_ast_expr_funcall;

t_osc_expr_ast_expr_funcall *osc_expr_ast_expr_funcall_alloc(t_osc_expr_rec *rec, int argc, ...);
long osc_expr_ast_expr_funcall_format(char *buf, long n, t_osc_expr_ast_expr *e);
void osc_expr_ast_expr_funcall_free(t_osc_expr_ast_expr *e);
t_osc_expr_rec *osc_expr_ast_expr_funcall_getRec(t_osc_expr_ast_expr_funcall *e);
t_osc_expr_ast_expr *osc_expr_ast_expr_funcall_getArgs(t_osc_expr_ast_expr_funcall *e);
int osc_expr_ast_expr_funcall_getNumArgs(t_osc_expr_ast_expr_funcall *e);
void osc_expr_ast_expr_funcall_prependArg(t_osc_expr_ast_expr_funcall *e, t_osc_expr_ast_expr *a);
void osc_expr_ast_expr_funcall_appendArg(t_osc_expr_ast_expr_funcall *e, t_osc_expr_ast_expr *a);

#ifdef __cplusplus
}
#endif

#endif
