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
#include <string.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_builtin.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_function.r"
#include "osc_message_iterator_u.h"

int osc_expr_ast_function_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_atom_ar_u **out)
{
	*out = osc_atom_array_u_alloc(1);
	t_osc_expr_ast_expr *ast_copy = osc_expr_ast_expr_copy(ast);
	osc_atom_u_setExpr(osc_atom_array_u_get(*out, 0), ast_copy, 1);
	return 0;
}

int osc_expr_ast_function_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					   t_osc_expr_lexenv *lexenv,
					   t_osc_bndl_u *oscbndl,
					   t_osc_msg_u **assign_target,
					   long *nlvals,
					   t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_function_format(char *buf, long n, t_osc_expr_ast_expr *f)
{
	if(f){
		t_osc_expr_ast_function *ff = (t_osc_expr_ast_function *)f;
		char *name = "lambda";
		t_osc_expr_ast_value *lambdalist = osc_expr_ast_function_getLambdaList(ff);
		t_osc_expr_ast_expr *e = osc_expr_ast_function_getExprs(ff);
		long offset = 0;
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s(", name);
		if(lambdalist){
			while(lambdalist){
				offset += osc_expr_ast_value_format(buf ? buf + offset : NULL, buf ? n - offset : 0, (t_osc_expr_ast_expr *)lambdalist);
				if(osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist)){
					offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ", ");
				}else{
					offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "){");
				}
				lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
			}
		}else{
			offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "){");
		}
		offset += osc_expr_ast_expr_formatAllLinked(buf ? buf + offset : NULL, buf ? n - offset : 0, e);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "}");
		return offset;
	}
	return 0;
}

long osc_expr_ast_function_formatLisp(char *buf, long n, t_osc_expr_ast_expr *f)
{
	if(f){
		t_osc_expr_ast_function *ff = (t_osc_expr_ast_function *)f;
		char *name = "lambda";
		t_osc_expr_ast_value *lambdalist = osc_expr_ast_function_getLambdaList(ff);
		t_osc_expr_ast_expr *e = osc_expr_ast_function_getExprs(ff);
		long offset = 0;
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s (", name);
		while(lambdalist){
			offset += osc_expr_ast_value_format(buf ? buf + offset : NULL, buf ? n - offset : 0, (t_osc_expr_ast_expr *)lambdalist);
			if(osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist)){
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
			}
			lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
		}
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ") ");
		offset += osc_expr_ast_expr_formatAllLinkedLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, e);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
		return offset;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_function_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_function *f = (t_osc_expr_ast_function *)ast;

		t_osc_expr_ast_value *lambdalist = osc_expr_ast_function_getLambdaList(f);
		t_osc_expr_ast_value *lambdalistcopy = NULL;
		if(lambdalist){
			lambdalistcopy = (t_osc_expr_ast_value *)osc_expr_ast_expr_copyAllLinked((t_osc_expr_ast_expr *)lambdalist);
		}
		t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprs(f);
		t_osc_expr_ast_expr *exprlistcopy = osc_expr_ast_expr_copyAllLinked(exprlist);
		t_osc_expr_ast_function *copy = osc_expr_ast_function_alloc(lambdalistcopy, exprlistcopy);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_function_free(t_osc_expr_ast_expr *f)
{
	if(f){
		osc_expr_ast_expr_free((t_osc_expr_ast_expr *)osc_expr_ast_function_getLambdaList((t_osc_expr_ast_function *)f));
		osc_expr_ast_expr_free((t_osc_expr_ast_expr *)osc_expr_ast_function_getExprs((t_osc_expr_ast_function *)f));
		osc_mem_free(f);
	}
}

t_osc_bndl_u *osc_expr_ast_function_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_function *f = (t_osc_expr_ast_function *)e;
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_FUNCTION);
	t_osc_msg_u *lambdalist = NULL;
	t_osc_expr_ast_value *ll = osc_expr_ast_function_getLambdaList(f);
	if(ll){
		lambdalist = osc_message_u_allocWithBndl_u("/lambdalist", osc_expr_ast_expr_toBndl((t_osc_expr_ast_expr *)ll), 1);
	}
	t_osc_msg_u *exprs = osc_message_u_allocWithBndl_u("/exprlist", osc_expr_ast_expr_toBndl((t_osc_expr_ast_expr *)osc_expr_ast_function_getExprs(f)), 1);
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(b, nodetype);
	if(ll){
		osc_bundle_u_addMsg(b, lambdalist);
	}else{
		osc_bundle_u_addMsg(b, osc_message_u_allocWithAddress("/lambdalist"));
	}
	osc_bundle_u_addMsg(b, exprs);
	return b;
}

t_osc_expr_ast_expr *osc_expr_ast_function_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	long n = 0;
	t_osc_msg_u **lambdalist_msg = NULL;
	t_osc_msg_u **exprlist_msg = NULL;
	t_osc_msg_u **text_msg = NULL;
	osc_bundle_u_lookupAddress(b, "/lambdalist", &n, &lambdalist_msg, 1);
	osc_bundle_u_lookupAddress(b, "/exprlist", &n, &exprlist_msg, 1);
	if(!lambdalist_msg || !exprlist_msg){
		return NULL;
	}
	t_osc_expr_ast_expr *lambdalist = NULL;
	t_osc_expr_ast_expr *exprlist = NULL;
	t_osc_msg_it_u *it = osc_msg_it_u_get(lambdalist_msg[0]);
	while(osc_msg_it_u_hasNext(it)){
		t_osc_atom_u *a = osc_msg_it_u_next(it);
		char tt = osc_atom_u_getTypetag(a);
		if(tt != OSC_EXPR_TYPETAG || tt != OSC_BUNDLE_TYPETAG){
			goto out;
		}
		t_osc_expr_ast_expr *e = osc_atom_u_getExpr(a);
	}
 out:
	if(lambdalist_msg){
		osc_mem_free(lambdalist_msg);
	}
	if(exprlist_msg){
		osc_mem_free(exprlist_msg);
	}
	return NULL;
}

void osc_expr_ast_function_setLambdaList(t_osc_expr_ast_function *f, t_osc_expr_ast_value *lambdalist)
{
	if(f){
		f->lambdalist = lambdalist;
	}
}

void osc_expr_ast_function_setExprs(t_osc_expr_ast_function *f, t_osc_expr_ast_expr *exprs)
{
	if(f){
		f->exprs = exprs;
	}
}

t_osc_expr_ast_value *osc_expr_ast_function_getLambdaList(t_osc_expr_ast_function *f)
{
	if(f){
		return f->lambdalist;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_function_getExprs(t_osc_expr_ast_function *f)
{
	if(f){
		return f->exprs;
	}
	return 0;
}

t_osc_expr_ast_function *osc_expr_ast_function_alloc(t_osc_expr_ast_value *lambdalist, t_osc_expr_ast_expr *exprs)
{
	t_osc_expr_ast_function *f = osc_mem_alloc(sizeof(t_osc_expr_ast_function));
	if(f){
		memset(f, '\0', sizeof(t_osc_expr_ast_function));
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)f,
				       OSC_EXPR_AST_NODETYPE_FUNCTION,
				       NULL,
				       osc_expr_ast_function_evalInLexEnv,
				       osc_expr_ast_function_evalLvalInLexEnv,
				       osc_expr_ast_function_format,
				       osc_expr_ast_function_formatLisp,
				       osc_expr_ast_function_free,
				       osc_expr_ast_function_copy,
				       osc_expr_ast_function_toBndl,
				       osc_expr_ast_function_fromBndl,
				       sizeof(t_osc_expr_ast_function));
		osc_expr_ast_function_setLambdaList(f, lambdalist);
		osc_expr_ast_function_setExprs(f, exprs);
	}
	return f;
}

