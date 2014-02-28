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
#include "osc_bundle_u.h"

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
		return r->vtab[(int)tt];
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

t_osc_bndl_u *osc_expr_funcrec_toBndl(t_osc_expr_funcrec *r)
{
	t_osc_msg_u *name_msg = osc_message_u_allocWithString("/name", osc_expr_funcrec_getName(r));
	int input_arity = osc_expr_funcrec_getInputArity(r);
	t_osc_msg_u *input_arity_msg = osc_message_u_allocWithInt32("/arity/input", input_arity);
	t_osc_msg_u *param_names_msg = osc_message_u_allocWithAddress("/param/names");
	char **param_names = osc_expr_funcrec_getParamNames(r);
	if(param_names){
		for(int i = 0; i < input_arity; i++){
			osc_message_u_appendString(param_names_msg, param_names[i]);
		}
	}
	t_osc_msg_u *param_types_msg = osc_message_u_allocWithAddress("/param/types");
	int *param_types = osc_expr_funcrec_getParamTypes(r);
	if(param_types){
		for(int i = 0; i < input_arity; i++){
			osc_message_u_appendInt32(param_types_msg, param_types[i]);
		}
	}
	t_osc_msg_u *variadic_msg = osc_message_u_allocWithBool("/variadic", osc_expr_funcrec_getVariadic(r));

	int output_arity = osc_expr_funcrec_getOutputArity(r);
	t_osc_msg_u *output_arity_msg = osc_message_u_allocWithInt32("/arity/output", output_arity);
	t_osc_msg_u *output_names_msg = osc_message_u_allocWithAddress("/output_names");
	char **output_names = osc_expr_funcrec_getOutputNames(r);
	if(output_names){
		for(int i = 0; i < output_arity; i++){
			osc_message_u_appendString(output_names_msg, output_names[i]);
		}
	}
	t_osc_msg_u *output_types_msg = osc_message_u_allocWithAddress("/output/types");
	int *output_types = osc_expr_funcrec_getOutputTypes(r);
	if(output_types){
		for(int i = 0; i < output_arity; i++){
			osc_message_u_appendInt32(output_types_msg, output_types[i]);
		}
	}

	t_osc_msg_u *docstring_msg = osc_message_u_allocWithString("/docstring", osc_expr_funcrec_getDocstring(r));
	// skip funcptr, lvalfuncptr and vtab

	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(b, name_msg);
	osc_bundle_u_addMsg(b, input_arity_msg);
	osc_bundle_u_addMsg(b, param_names_msg);
	osc_bundle_u_addMsg(b, param_types_msg);
	osc_bundle_u_addMsg(b, variadic_msg);
	osc_bundle_u_addMsg(b, output_arity_msg);
	osc_bundle_u_addMsg(b, output_names_msg);
	osc_bundle_u_addMsg(b, output_types_msg);
	osc_bundle_u_addMsg(b, docstring_msg);
	return b;
}

t_osc_expr_funcrec *osc_expr_funcrec_fromBndl(t_osc_bndl_u *b)
{
	return NULL;
}

t_osc_expr_ast_function *osc_expr_funcrec_allocLambda(t_osc_expr_funcrec *r)
{
	t_osc_expr_ast_value *lambdalist = NULL;
	t_osc_expr_ast_value *arglist = NULL;
	int argc = osc_expr_funcrec_getInputArity(r);
	char **param_names = osc_expr_funcrec_getParamNames(r);
	for(int i = 0; i < argc; i++){
		t_osc_atom_u *a = osc_atom_u_allocWithString(param_names[i]);
		t_osc_expr_ast_value *v = osc_expr_ast_value_allocIdentifier(a);
		if(lambdalist){
			osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, (t_osc_expr_ast_expr *)v);
			osc_expr_ast_expr_append((t_osc_expr_ast_expr *)arglist, osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)v));
		}else{
			lambdalist = v;
			arglist = osc_expr_ast_value_copy((t_osc_expr_ast_expr *)v);
		}
	}
	t_osc_expr_ast_expr *expr = osc_expr_ast_funcall_allocWithList(r, arglist);
	return osc_expr_ast_function_alloc(lambdalist, expr);
}
