/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011, The Regents of
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

/** 	\file osc_expr_funcrec.r
	\author John MacCallum

*/
#ifndef __OSC_EXPR_FUNCREC_R__
#define __OSC_EXPR_FUNCREC_R__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_expr_ast_funcall.h"
#include "osc_typetag.h"

#pragma pack(push)
#pragma pack(4)

/** \struct t_osc_expr_funcrec
    A record that associates a function name (string) with a function pointer.
*/
struct _osc_expr_funcrec
{
	char *name; /**< Name of the function as a C string. */
	int input_arity;
	char **param_names;
	int variadic; /**< true/false */
	int output_arity; /**< All will be set to 1---this is here just in case we want to support this in the future. */
	char **output_names; /**< Plural for possible future support of multiple return values. */
	char *docstring;
	t_osc_expr_builtin_funcptr func;
	t_osc_expr_builtin_lvalfuncptr lval_func;
	t_osc_expr_builtin_funcptr applicator;
	void **vtab;
};
#pragma pack(pop)


#ifdef _cplusplus
}
#endif

#endif
