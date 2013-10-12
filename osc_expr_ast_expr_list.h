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

/** 	\file osc_expr_ast_expr_list.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_EXPR_LIST_H__
#define __OSC_EXPR_AST_EXPR_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_atom_u.h"

typedef struct _osc_expr_ast_expr_list t_osc_expr_ast_expr_list;

t_osc_expr_ast_expr_list *osc_expr_ast_expr_list_alloc(t_osc_atom_ar_u *a);
long osc_expr_ast_expr_list_format(char *buf, long n, t_osc_expr_ast_expr *v);
void osc_expr_ast_expr_list_free(t_osc_expr_ast_expr *v);
t_osc_atom_ar_u *osc_expr_ast_expr_list_getList(t_osc_expr_ast_expr_list *v);
void osc_expr_ast_expr_list_setList(t_osc_expr_ast_expr_list *v, t_osc_atom_ar_u *a);
int osc_expr_ast_expr_list_getLen(t_osc_expr_ast_expr_list *v);

#ifdef __cplusplus
}
#endif

#endif
