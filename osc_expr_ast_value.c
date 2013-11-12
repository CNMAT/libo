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

static void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, t_osc_atom_u *a, int valuetype);
static t_osc_expr_ast_value *osc_expr_ast_value_alloc(t_osc_atom_u *a, int type);

int osc_expr_ast_value_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out)
{
	return 1;
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

t_osc_atom_u *osc_expr_ast_value_getValue(t_osc_expr_ast_value *v)
{
	if(v){
		return v->value;
	}
	return NULL;
}

static void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, t_osc_atom_u *a, int valuetype)
{
	if(v){
		v->value = a;
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

static t_osc_expr_ast_value *osc_expr_ast_value_alloc(t_osc_atom_u *a, int type)
{
	t_osc_expr_ast_value *v = osc_mem_alloc(sizeof(t_osc_expr_ast_value));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_VALUE,
				       NULL,
				       osc_expr_ast_value_evalInLexEnv,
				       osc_expr_ast_value_format,
				       osc_expr_ast_value_free,
				       osc_expr_ast_value_copy,
				       osc_expr_ast_value_serialize,
				       osc_expr_ast_value_deserialize,
				       sizeof(t_osc_expr_ast_value));
		osc_expr_ast_value_setValue(v, a, type);
		v->valuetype = type;
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

