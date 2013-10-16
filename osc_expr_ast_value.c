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

#include <stdarg.h>
#include <stdio.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_ast_value.r"

int osc_expr_ast_value_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out)
{
	if(!ast){
		return 1;
	}
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)ast;
	t_osc_atom_u *a = osc_expr_ast_value_getValue(v);
	if(!a){
		return 1;
	}
	if(lexenv && osc_atom_u_getTypetag(a) == 's'){
		t_osc_atom_ar_u *tmp = NULL;
		if((tmp = osc_expr_lookupBindingInLexenv(lexenv, osc_atom_u_getStringPtr(a)))){
			*out = osc_atom_array_u_copy(tmp);
			return 0;					
		}
	}
	*out = osc_atom_array_u_alloc(1);
				
	t_osc_atom_u *acopy = osc_atom_array_u_get(*out, 0);
	osc_atom_u_copy(&acopy, a);
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_value_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_atom_u *atom = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)ast);
		if(atom){
			t_osc_atom_u *copy = NULL;
			osc_atom_u_copy(&copy, atom);
			return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(copy);
		}else{
			return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(NULL);
		}
	}else{
		return NULL;
	}
}

long osc_expr_ast_value_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return osc_atom_u_nformat(buf, n, osc_expr_ast_value_getValue((t_osc_expr_ast_value *)v), 0);
	}
	return 0;
}

void osc_expr_ast_value_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_atom_u_free(osc_expr_ast_value_getValue((t_osc_expr_ast_value *)v));
		osc_mem_free(v);
	}
}

t_osc_atom_u *osc_expr_ast_value_getValue(t_osc_expr_ast_value *v)
{
	if(v){
		return v->value;
	}
	return NULL;
}

char osc_expr_ast_value_getValueType(t_osc_expr_ast_value *v)
{
	if(v && v->value){
		return osc_atom_u_getTypetag(v->value);
	}
	return '\0';
}

void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	if(v){
		v->value = a;
	}
}

t_osc_expr_ast_value *osc_expr_ast_value_alloc(t_osc_atom_u *a)
{
	t_osc_expr_ast_value *v = osc_mem_alloc(sizeof(t_osc_expr_ast_value));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v, OSC_EXPR_AST_NODETYPE_VALUE, NULL, osc_expr_ast_value_evalInLexEnv, osc_expr_ast_value_format, osc_expr_ast_value_free, osc_expr_ast_value_copy, sizeof(t_osc_expr_ast_value));
		osc_expr_ast_value_setValue(v, a);
	}
	return v;
}

