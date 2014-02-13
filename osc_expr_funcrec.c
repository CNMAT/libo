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

#include <string.h>
#include <stdio.h>
#include "osc_expr_funcrec.r"
#include "osc_expr_funcrec.h"

char *osc_expr_funcrec_getName(t_osc_expr_funcrec *r)
{
	if(r){
		return r->name;
	}
	return NULL;
}

int osc_expr_funcrec_getInputArity(t_osc_expr_funcrec *r)
{
	if(r){
		return r->input_arity;
	}
	return -1;
}

char **osc_expr_funcrec_getParamNames(t_osc_expr_funcrec *r)
{
	if(r){
		return r->param_names;
	}
	return NULL;
}

int *osc_expr_funcrec_getParamTypes(t_osc_expr_funcrec *r)
{
	if(r){
		return r->param_types;
	}
	return NULL;
}

int osc_expr_funcrec_getVariadic(t_osc_expr_funcrec *r)
{
	if(r){
		return r->variadic;
	}
	return -1;
}

int osc_expr_funcrec_getOutputArity(t_osc_expr_funcrec *r)
{
	if(r){
		return r->output_arity;
	}
	return -1;
}

char **osc_expr_funcrec_getOutputNames(t_osc_expr_funcrec *r)
{
	if(r){
		return r->output_names;
	}
	return NULL;
}

int *osc_expr_funcrec_getOutputTypes(t_osc_expr_funcrec *r)
{
	if(r){
		return r->output_types;
	}
	return NULL;
}

char *osc_expr_funcrec_getDocstring(t_osc_expr_funcrec *r)
{
	if(r){
		return r->docstring;
	}
	return NULL;
}

t_osc_expr_builtin_funcptr osc_expr_funcrec_getFunc(t_osc_expr_funcrec *r)
{
	if(r){
		return r->func;
	}
	return NULL;
}

t_osc_expr_builtin_lvalfuncptr osc_expr_funcrec_getLvalFunc(t_osc_expr_funcrec *r)
{
	if(r){
		return r->lval_func;
	}
	return NULL;
}

int osc_expr_funcrec_hasVTab(t_osc_expr_funcrec *r)
{
	if(r){
		return r->vtab != NULL;
	}
	return 0;
}

void *osc_expr_funcrec_getFuncForTypetag(t_osc_expr_funcrec *r, char tt)
{
	if(r){
		return r->vtab[tt];
	}
	return NULL;
}

int osc_expr_funcrec_getScalarExpansionArgc(t_osc_expr_funcrec *r)
{
	if(r){
		return r->scalar_expansion_argc;
	}
	return 0;
}

unsigned int *osc_expr_funcrec_getScalarExpansionArgv(t_osc_expr_funcrec *r)
{
	if(r){
		return r->scalar_expansion_argv;
	}
	return NULL;
}

uint32_t osc_expr_funcrec_getScalarExpansionFlags(t_osc_expr_funcrec *r)
{
	if(r){
		return r->scalar_expansion_flags;
	}
	return 0;
}

int osc_expr_funcrec_getTypePromotionArgc(t_osc_expr_funcrec *r)
{
	if(r){
		return r->type_promotion_argc;
	}
	return 0;
}

unsigned int *osc_expr_funcrec_getTypePromotionArgv(t_osc_expr_funcrec *r)
{
	if(r){
		return r->type_promotion_argv;
	}
	return NULL;
}

uint32_t osc_expr_funcrec_getTypePromotionFlags(t_osc_expr_funcrec *r)
{
	if(r){
		return r->type_promotion_flags;
	}
	return 0;
}
