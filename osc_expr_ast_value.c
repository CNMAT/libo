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
//#include "osc_rset.h"
//#include "osc_query.h"
#include "osc_message_u.h"
#include "osc_message_iterator_u.h"

static void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, void *a, int valuetype);
static t_osc_expr_ast_value *osc_expr_ast_value_alloc(void *a, int type);

int osc_expr_ast_value_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    t_osc_bndl_u *oscbndl,
				    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)ast;
	if(v){
		switch(v->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				*out = osc_atom_array_u_alloc(1);
				t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
				osc_atom_u_copy(&a, vv);
			}
			return 0;
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				t_osc_atom_ar_u *tmp = NULL;
				if((tmp = osc_expr_lexenv_lookup(lexenv, osc_atom_u_getStringPtr(vv)))){
					*out = osc_atom_array_u_copy(tmp);
				}
				return 0;
			}
			return 0;
		case OSC_EXPR_AST_VALUE_TYPE_LIST:
			{
				*out = osc_atom_array_u_copy((t_osc_atom_ar_u *)osc_expr_ast_value_getList(v));
			}
			return 0;
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			{
				t_osc_atom_ar_u *lvals = NULL;
				int ret = osc_expr_ast_value_evalLvalInLexEnv(ast, lexenv, oscbndl, &lvals);
				if(!ret && lvals){
					*out = osc_atom_array_u_copy(lvals);
				}
				osc_array_free((t_osc_array *)lvals);
				return ret;
			}
		}
	}
	return 1;
}

int osc_expr_ast_value_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					t_osc_expr_lexenv *lexenv,
					t_osc_bndl_u *oscbndl,
					t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)ast;
	if(v){
		switch(v->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
		case OSC_EXPR_AST_VALUE_TYPE_LIST:
			return 1;
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				*out = NULL;
				if(!oscbndl){
					return OSC_ERR_EXPR_ADDRESSUNBOUND;
				}
				char *oscaddress = osc_atom_u_getStringPtr(vv);
				t_osc_msg_ar_u *msgar = NULL;
				osc_bundle_u_lookupAddress(oscbndl, oscaddress, &msgar, 1);
				if(msgar){
					t_osc_msg_u *m = osc_message_array_u_get(msgar, 0);
					if(m){
						long arg_count = osc_message_u_getArgCount(m);
						*out = osc_atom_array_u_alloc(arg_count);
						t_osc_atom_ar_u *atom_ar = *out;
						osc_atom_array_u_clear(atom_ar);
						int i = 0;
						t_osc_msg_it_u *it = osc_msg_it_u_get(m);
						while(osc_msg_it_u_hasNext(it)){
							t_osc_atom_u *src = osc_msg_it_u_next(it);
							t_osc_atom_u *dest = osc_atom_array_u_get(atom_ar, i);
							dest = src;
							i++;
						}
						osc_msg_it_s_destroy(it);
						osc_array_free((t_osc_array *)msgar);
						return 0;
					}
				}
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
		}
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_value_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_atom_u *atom = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)ast);
		if(atom){
			t_osc_atom_u *copy = NULL;
			osc_atom_u_copy(&copy, atom);
			return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(copy, ((t_osc_expr_ast_value *)ast)->valuetype);
		}else{
			return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(NULL, 0);
		}
	}else{
		return NULL;
	}
}

long osc_expr_ast_value_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	t_osc_expr_ast_value *vv = (t_osc_expr_ast_value *)v;
	if(vv){
		switch(vv->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
			return osc_atom_u_nformat(buf, n, osc_expr_ast_value_getValue(vv), 0);
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			return snprintf(buf, n, "%s", osc_atom_u_getStringPtr(osc_expr_ast_value_getValue(vv)));
		}
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

t_osc_err osc_expr_ast_value_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_value_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

int osc_expr_ast_value_getValueType(t_osc_expr_ast_value *v)
{
	if(v){
		return v->valuetype;
	}
	return -1;
}

void *osc_expr_ast_value_getValue(t_osc_expr_ast_value *v)
{
	if(v){
		return v->value;
	}
	return NULL;
}

t_osc_atom_u *osc_expr_ast_value_getLiteral(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_u *osc_expr_ast_value_getIdentifier(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_u *osc_expr_ast_value_getOSCAddress(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_ar_u *osc_expr_ast_value_getList(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_ar_u *)osc_expr_ast_value_getValue(v);
}

static void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, void *a, int valuetype)
{
	if(v){
		v->value = a;
		v->valuetype = valuetype;
	}
}

void osc_expr_ast_value_setLiteral(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setIdentifier(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setOSCAddress(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setList(t_osc_expr_ast_value *v, t_osc_atom_ar_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LIST);
}

static t_osc_expr_ast_value *osc_expr_ast_value_alloc(void *a, int type)
{
	t_osc_expr_ast_value *v = osc_mem_alloc(sizeof(t_osc_expr_ast_value));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_VALUE,
				       NULL,
				       osc_expr_ast_value_evalInLexEnv,
				       osc_expr_ast_value_evalLvalInLexEnv,
				       osc_expr_ast_value_format,
				       osc_expr_ast_value_format,
				       osc_expr_ast_value_free,
				       osc_expr_ast_value_copy,
				       osc_expr_ast_value_serialize,
				       osc_expr_ast_value_deserialize,
				       sizeof(t_osc_expr_ast_value));
		osc_expr_ast_value_setValue(v, a, type);
	}
	return v;
}

t_osc_expr_ast_value *osc_expr_ast_value_allocLiteral(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocIdentifier(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocOSCAddress(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocList(t_osc_atom_ar_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_LIST);
}

