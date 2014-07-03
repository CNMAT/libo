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

/** 	\file osc_expr_funcrec.h
	\author John MacCallum

	This class has no allocator, deallocator, copy, or set methods. It is meant to be used statically.
*/
#ifndef __OSC_EXPR_FUNCREC_H__
#define __OSC_EXPR_FUNCREC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_expr_funcrec t_osc_expr_funcrec;

#include "osc_expr_builtin.h"
#include "osc_expr_ast_function.h"

char *osc_expr_funcrec_getName(t_osc_expr_funcrec *r);
int osc_expr_funcrec_getInputArity(t_osc_expr_funcrec *r);
char **osc_expr_funcrec_getParamNames(t_osc_expr_funcrec *r);
int osc_expr_funcrec_getVariadic(t_osc_expr_funcrec *r);
int osc_expr_funcrec_getOutputArity(t_osc_expr_funcrec *r);
char **osc_expr_funcrec_getOutputNames(t_osc_expr_funcrec *r);
char *osc_expr_funcrec_getDocstring(t_osc_expr_funcrec *r);
int osc_expr_funcrec_shouldEvalArgs(t_osc_expr_funcrec *r);
t_osc_expr_builtin_funcptr osc_expr_funcrec_getFunc(t_osc_expr_funcrec *r);
t_osc_expr_builtin_lvalfuncptr osc_expr_funcrec_getLvalFunc(t_osc_expr_funcrec *r);
t_osc_expr_builtin_funcptr osc_expr_funcrec_getApplicator(t_osc_expr_funcrec *r);
int osc_expr_funcrec_hasVTab(t_osc_expr_funcrec *r);
void *osc_expr_funcrec_getFuncForTypetag(t_osc_expr_funcrec *r, char tt);
t_osc_bndl_u *osc_expr_funcrec_toBndl(t_osc_expr_funcrec *r);
t_osc_expr_funcrec *osc_expr_funcrec_fromBndl(t_osc_bndl_u *b);
t_osc_expr_ast_function *osc_expr_funcrec_allocLambda(t_osc_expr_funcrec *r);

#ifdef __cplusplus
}
#endif

#endif
