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
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_list.h"
#include "osc_expr_ast_list.r"

int osc_expr_ast_list_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out)
{
	if(!ast){
		return 1;
	}
	t_osc_expr_ast_list *l = (t_osc_expr_ast_list *)ast;
	*out = osc_expr_ast_list_getListCopy(l);
	return 0;
}

long osc_expr_ast_list_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		long count = 0;
		t_osc_atom_ar_u *ar = osc_expr_ast_list_getList((t_osc_expr_ast_list *)v);
		int arlen = osc_atom_array_u_getLen(ar);
		if(!buf){
			count += snprintf(NULL, 0, "[");
			for(int i = 0; i < arlen; i++){
				count += osc_atom_u_nformat(NULL, 0, osc_atom_array_u_get(ar, i), 0);
				if(i < arlen - 1){
					count += snprintf(NULL, 0, ", ");
				}
			}
			count += snprintf(NULL, 0, "]");
		}else{
			count += snprintf(buf + count, n - count, "[");
			for(int i = 0; i < arlen; i++){
				count += osc_atom_u_nformat(buf + count, n - count, osc_atom_array_u_get(ar, i), 0);
				if(i < arlen - 1){
					count += snprintf(buf + count, n - count, ", ");
				}
			}
			count += snprintf(buf + count, n - count, "]");
		}
		return count;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_list_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		return (t_osc_expr_ast_expr *)osc_expr_ast_list_alloc(osc_expr_ast_list_getListCopy((t_osc_expr_ast_list *)ast));
	}else{
		return NULL;
	}
}

void osc_expr_ast_list_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_atom_array_u_free(osc_expr_ast_list_getList((t_osc_expr_ast_list *)v));
		osc_mem_free(v);
	}
}

t_osc_err osc_expr_ast_list_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_list_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_atom_ar_u *osc_expr_ast_list_getList(t_osc_expr_ast_list *v)
{
	if(v){
		return v->list;
	}
	return NULL;
}

t_osc_atom_ar_u *osc_expr_ast_list_getListCopy(t_osc_expr_ast_list *v)
{
	if(v && v->list){
		return osc_atom_array_u_copy(v->list);
	}
	return NULL;
}

void osc_expr_ast_list_setList(t_osc_expr_ast_list *v, t_osc_atom_ar_u *a)
{
	if(v){
		v->list = a;
	}
}

int osc_expr_ast_list_getLen(t_osc_expr_ast_list *v)
{
	if(v){
		return osc_atom_array_u_getLen(osc_expr_ast_list_getList(v));
	}
	return 0;
}

t_osc_expr_ast_list *osc_expr_ast_list_alloc(t_osc_atom_ar_u *a)
{
	t_osc_expr_ast_list *v = osc_mem_alloc(sizeof(t_osc_expr_ast_list));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_LIST,
				       NULL,
				       osc_expr_ast_list_evalInLexEnv,
				       osc_expr_ast_list_format,
				       osc_expr_ast_list_free,
				       osc_expr_ast_list_copy,
				       osc_expr_ast_list_serialize,
				       osc_expr_ast_list_deserialize,
				       sizeof(t_osc_expr_ast_list));
		osc_expr_ast_list_setList(v, a);
	}
	return v;
}

