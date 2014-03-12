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
#include "osc_expr_oprec.r"
#include "osc_expr_oprec.h"
#include "osc_message_u.h"

char *osc_expr_oprec_getName(t_osc_expr_oprec *r)
{
	if(r){
		return r->name;
	}
	return NULL;
}

int osc_expr_oprec_getInputArity(t_osc_expr_oprec *r)
{
	if(r){
		return r->input_arity;
	}
	return -1;
}

char **osc_expr_oprec_getParamNames(t_osc_expr_oprec *r)
{
	if(r){
		return r->param_names;
	}
	return NULL;
}

char *osc_expr_oprec_getTypeConstraintsForParam(t_osc_expr_oprec *r, int param)
{
	if(r){
		if(param >= osc_expr_oprec_getInputArity(r)){
			return NULL;
		}
		if(r->param_type_constraints){
			return r->param_type_constraints[param];
		}else{
			return NULL;
		}
	}
	return NULL;
}

int osc_expr_oprec_getOutputArity(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_arity;
	}
	return -1;
}

char **osc_expr_oprec_getOutputNames(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_names;
	}
	return NULL;
}

int *osc_expr_oprec_getOutputTypes(t_osc_expr_oprec *r)
{
	if(r){
		return r->output_types;
	}
	return NULL;
}

char *osc_expr_oprec_getDocstring(t_osc_expr_oprec *r)
{
	if(r){
		return r->docstring;
	}
	return NULL;
}

int osc_expr_oprec_getAssociativity(t_osc_expr_oprec *r)
{
	if(r){
		return r->associativity;
	}
	return -1;
}

int osc_expr_oprec_getPrecedence(t_osc_expr_oprec *r)
{
	if(r){
		return r->precedence;
	}
	return -1;
}

int osc_expr_oprec_getFixity(t_osc_expr_oprec *r)
{
	if(r){
		return r->fixity;
	}
	return -1;
}

char osc_expr_oprec_getBytecode(t_osc_expr_oprec *r)
{
	if(r){
		return r->bytecode;
	}
	return -1;
}

t_osc_bndl_u *osc_expr_oprec_toBndl(t_osc_expr_oprec *r)
{
	if(!r){
		return NULL;
	}
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_msg_u *name_msg = osc_message_u_allocWithString("/name", osc_expr_oprec_getName(r));

	int input_arity = osc_expr_oprec_getInputArity(r);
	t_osc_msg_u *input_arity_msg = osc_message_u_allocWithInt32("/arity/input", input_arity);
	t_osc_msg_u *param_names_msg = osc_message_u_allocWithAddress("/param/names");
	char **param_names = osc_expr_oprec_getParamNames(r);
	if(param_names){
		for(int i = 0; i < input_arity; i++){
			osc_message_u_appendString(param_names_msg, param_names[i]);
		}
	}

	for(int i = 0; i < input_arity; i++){
		char *tc_list = osc_expr_oprec_getTypeConstraintsForParam(r, i);
		if(tc_list){
			char buf[10];
			char tc = tc_list[0];
			int j = 0;
			while(tc){
				buf[j] = tc;
				j++;
				tc = tc_list[j];
			}
			buf[j] = tc;
			char address[32];
			snprintf(address, sizeof(address), "/param/%d/typeconstraints", i);
			t_osc_msg_u *msg = osc_message_u_allocWithString(address, buf);
			osc_bundle_u_addMsg(b, msg);
		}
	}

	int output_arity = osc_expr_oprec_getOutputArity(r);
	t_osc_msg_u *output_arity_msg = osc_message_u_allocWithInt32("/arity/output", output_arity);
	t_osc_msg_u *output_names_msg = osc_message_u_allocWithAddress("/output_names");
	char **output_names = osc_expr_oprec_getOutputNames(r);
	if(output_names){
		for(int i = 0; i < output_arity; i++){
			osc_message_u_appendString(output_names_msg, output_names[i]);
		}
	}
	t_osc_msg_u *output_types_msg = osc_message_u_allocWithAddress("/output/types");
	int *output_types = osc_expr_oprec_getOutputTypes(r);
	if(output_types){
		for(int i = 0; i < output_arity; i++){
			osc_message_u_appendInt32(output_types_msg, output_types[i]);
		}
	}
	t_osc_msg_u *docstring_msg = osc_message_u_allocWithString("/docstring", osc_expr_oprec_getDocstring(r));

	t_osc_msg_u *assoc_msg = osc_message_u_allocWithInt32("/associativity", osc_expr_oprec_getAssociativity(r));
	t_osc_msg_u *precedence_msg = osc_message_u_allocWithInt32("/precedence", osc_expr_oprec_getPrecedence(r));
	t_osc_msg_u *fixity_msg = osc_message_u_allocWithInt32("/fixity", osc_expr_oprec_getFixity(r));
	t_osc_msg_u *bytecode_msg = osc_message_u_allocWithUInt8("/bytecode", osc_expr_oprec_getBytecode(r));

	osc_bundle_u_addMsg(b, name_msg);
	osc_bundle_u_addMsg(b, input_arity_msg);
	osc_bundle_u_addMsg(b, param_names_msg);
	osc_bundle_u_addMsg(b, output_arity_msg);
	osc_bundle_u_addMsg(b, output_names_msg);
	osc_bundle_u_addMsg(b, output_types_msg);
	osc_bundle_u_addMsg(b, docstring_msg);
	osc_bundle_u_addMsg(b, assoc_msg);
	osc_bundle_u_addMsg(b, precedence_msg);
	osc_bundle_u_addMsg(b, fixity_msg);
	osc_bundle_u_addMsg(b, bytecode_msg);
	return b;
}

t_osc_expr_oprec *osc_expr_oprec_fromBndl(t_osc_bndl_u *b)
{
	return NULL;
}

