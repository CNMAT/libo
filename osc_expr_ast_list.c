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
#include "osc_expr_funcrec.h"

int osc_expr_ast_list_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *oscbndl,
				   t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_ast_list_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_msg_u **assign_target,
				       long *nlvals,
				       t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_list_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		long offset = 0;
		t_osc_expr_ast_expr *l = osc_expr_ast_list_getList((t_osc_expr_ast_list *)v);
		//offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "[");
		while(l){
			offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, l);
			l = osc_expr_ast_expr_next(l);
			if(l){
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ", ");
			}
		}
		//offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "]");
		return offset;
	}
	return 0;
}

long osc_expr_ast_list_formatLisp(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		long offset = 0;
		t_osc_expr_ast_expr *l = osc_expr_ast_list_getList((t_osc_expr_ast_list *)v);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(list");
		while(l){
			offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
			offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, l);
			l = osc_expr_ast_expr_next(l);
		}
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
		return offset;
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
		osc_expr_ast_expr_free(osc_expr_ast_list_getList((t_osc_expr_ast_list *)v));
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

t_osc_expr_ast_expr *osc_expr_ast_list_getList(t_osc_expr_ast_list *v)
{
	if(v){
		return v->list;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_list_getListCopy(t_osc_expr_ast_list *v)
{
	if(v && v->list){
		return osc_expr_ast_expr_copyAllLinked((t_osc_expr_ast_expr *)v);
	}
	return NULL;
}

void osc_expr_ast_list_setListWithLen(t_osc_expr_ast_list *v, t_osc_expr_ast_expr *list, long len)
{
	if(v){
		v->list = list;
		v->len = len;
	}
}

void osc_expr_ast_list_setList(t_osc_expr_ast_list *v, t_osc_expr_ast_expr *list)
{
	if(v){
		long len = 0;
		t_osc_expr_ast_expr *l = list;
		while(l){
			len++;
			l = osc_expr_ast_expr_next(l);
		}
		osc_expr_ast_list_setListWithLen(v, list, len);
	}
}

long osc_expr_ast_list_getLen(t_osc_expr_ast_list *v)
{
	if(v){
		if(v->len >= 0){
			return v->len;
		}else{
			t_osc_expr_ast_expr *l = osc_expr_ast_list_getList(v);
			long len = 0;
			while(v){
				len++;
				l = osc_expr_ast_expr_next(l);
			}
			v->len = len;
			return len;
		}
	}
	return 0;
}

t_osc_expr_ast_list *osc_expr_ast_list_allocWithLen(t_osc_expr_ast_expr *list, long len)
{
	t_osc_expr_ast_list *v = osc_mem_alloc(sizeof(t_osc_expr_ast_list));
	if(v){
		/*
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_LIST,
				       NULL,
				       osc_expr_ast_list_evalInLexEnv,
				       osc_expr_ast_list_format,
				       osc_expr_ast_list_formatLisp,
				       osc_expr_ast_list_free,
				       osc_expr_ast_list_copy,
				       osc_expr_ast_list_serialize,
				       osc_expr_ast_list_deserialize,
				       sizeof(t_osc_expr_ast_list));
		osc_expr_ast_list_setListWithLen(v, list, len);
		*/
		t_osc_expr_funcrec *funcrec = osc_expr_builtin_func_list;
		osc_expr_ast_funcall_initWithList((t_osc_expr_ast_funcall *)v,
						  OSC_EXPR_AST_NODETYPE_LIST,
						  NULL,
						  NULL,
						  NULL,
						  osc_expr_ast_list_format,
						  NULL,
						  NULL,//osc_expr_ast_list_free,
						  osc_expr_ast_list_copy,
						  osc_expr_ast_list_serialize,
						  osc_expr_ast_list_deserialize,
						  sizeof(t_osc_expr_ast_list),
						  funcrec,
						  list);
	}
	return v;
}

t_osc_expr_ast_list *osc_expr_ast_list_alloc(t_osc_expr_ast_expr *list)
{
	t_osc_expr_ast_list *v = osc_mem_alloc(sizeof(t_osc_expr_ast_list));
	if(v){
		/*
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_LIST,
				       NULL,
				       osc_expr_ast_list_evalInLexEnv,
				       osc_expr_ast_list_format,
				       osc_expr_ast_list_formatLisp,
				       osc_expr_ast_list_free,
				       osc_expr_ast_list_copy,
				       osc_expr_ast_list_serialize,
				       osc_expr_ast_list_deserialize,
				       sizeof(t_osc_expr_ast_list));
		*/
		t_osc_expr_funcrec *funcrec = osc_expr_builtin_func_list;
		osc_expr_ast_funcall_initWithList((t_osc_expr_ast_funcall *)v,
						  OSC_EXPR_AST_NODETYPE_LIST,
						  NULL,
						  NULL,
						  NULL,
						  osc_expr_ast_list_format,
						  NULL,
						  NULL,//osc_expr_ast_list_free,
						  osc_expr_ast_list_copy,
						  osc_expr_ast_list_serialize,
						  osc_expr_ast_list_deserialize,
						  sizeof(t_osc_expr_ast_list),
						  funcrec,
						  list);
		osc_expr_ast_list_setList(v, list);
	}
	return v;
}

