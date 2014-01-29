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

/** 	\file osc_expr_ast_expr.h
	\author John MacCallum

*/

#ifndef __OSC_EXPR_AST_EXPR_H__
#define __OSC_EXPR_AST_EXPR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_ast_expr t_osc_expr_ast_expr;

#include "osc_expr_lexenv.h"
#include "osc_atom_array_u.h"
#include "osc_bundle_u.h"

enum{
	OSC_EXPR_AST_NODETYPE_EXPR = 1,
	OSC_EXPR_AST_NODETYPE_FUNCALL,
	OSC_EXPR_AST_NODETYPE_UNARYOP,
	OSC_EXPR_AST_NODETYPE_BINARYOP,
	OSC_EXPR_AST_NODETYPE_ARRAYSUBSCRIPT,
	OSC_EXPR_AST_NODETYPE_VALUE,
	OSC_EXPR_AST_NODETYPE_LIST,
	OSC_EXPR_AST_NODETYPE_FUNCTION,
	OSC_EXPR_AST_NODETYPE_ASEQ,
	OSC_EXPR_AST_NODETYPE_TERNARYCOND,
	OSC_EXPR_AST_NODETYPE_LET,
};


typedef long (*t_osc_expr_ast_formatfn)(char*, long, t_osc_expr_ast_expr*);
typedef void (*t_osc_expr_ast_freefn)(t_osc_expr_ast_expr*);
typedef int (*t_osc_expr_ast_evalfn)(t_osc_expr_ast_expr*,
				     t_osc_expr_lexenv*,
				     t_osc_bndl_u*,
				     t_osc_atom_ar_u**);
typedef t_osc_expr_ast_expr *(*t_osc_expr_ast_copyfn)(t_osc_expr_ast_expr*);
typedef t_osc_err (*t_osc_expr_ast_serializefn)(t_osc_expr_ast_expr*, long*, char**);
typedef t_osc_err (*t_osc_expr_ast_deserializefn)(long, char*,t_osc_expr_ast_expr**);

void osc_expr_ast_expr_init(t_osc_expr_ast_expr *e,
			    int nodetype,
			    t_osc_expr_ast_expr *next,
			    t_osc_expr_ast_evalfn evalfn,
			    t_osc_expr_ast_formatfn formatfn,
			    t_osc_expr_ast_formatfn format_lispfn,
			    t_osc_expr_ast_freefn freefn,
			    t_osc_expr_ast_copyfn copyfn,
			    t_osc_expr_ast_serializefn serializefn,
			    t_osc_expr_ast_deserializefn deserializefn,
			    size_t objsize);
int osc_expr_ast_expr_eval(t_osc_expr_ast_expr *ast,
			   long *len,
			   char **oscbndl,
			   t_osc_atom_ar_u **out);
int osc_expr_ast_expr_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *oscbndl,
				   t_osc_atom_ar_u **out);
t_osc_expr_ast_expr *osc_expr_ast_expr_copy(t_osc_expr_ast_expr *ast);
t_osc_expr_ast_expr *osc_expr_ast_expr_copyAllLinked(t_osc_expr_ast_expr *ast);
void osc_expr_ast_expr_free(t_osc_expr_ast_expr *e);
int osc_expr_ast_expr_getNodetype(t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_expr_next(t_osc_expr_ast_expr *e);
void osc_expr_ast_expr_prepend(t_osc_expr_ast_expr *e, t_osc_expr_ast_expr *expr_to_prepend);
void osc_expr_ast_expr_append(t_osc_expr_ast_expr *e, t_osc_expr_ast_expr *expr_to_append);
long osc_expr_ast_expr_format(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_expr_formatAllLinked(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_expr_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e);
long osc_expr_ast_expr_formatAllLinkedLisp(char *buf, long n, t_osc_expr_ast_expr *e);
t_osc_err osc_expr_ast_expr_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr);
t_osc_err osc_expr_ast_expr_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e);
size_t osc_expr_ast_expr_sizeof(t_osc_expr_ast_expr *e);
void osc_expr_ast_expr_setBrackets(t_osc_expr_ast_expr *e, char leftbracket, char rightbracket);
char osc_expr_ast_expr_getLeftBracket(t_osc_expr_ast_expr *e);
char osc_expr_ast_expr_getRightBracket(t_osc_expr_ast_expr *e);
t_osc_expr_ast_expr *osc_expr_ast_expr_alloc(void);

#ifdef __cplusplus
}
#endif

#endif
