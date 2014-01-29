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

int osc_expr_ast_function_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_atom_ar_u **out)
{
	return 1;
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
		while(lambdalist){
			offset += osc_expr_ast_value_format(buf ? buf + offset : NULL, buf ? n - offset : 0, (t_osc_expr_ast_expr *)lambdalist);
			if(osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist)){
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ", ");
			}else{
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "){");
			}
			lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
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
		t_osc_expr_ast_value *lambdalistcopy = (t_osc_expr_ast_value *)osc_expr_ast_value_copy((t_osc_expr_ast_expr *)lambdalist);
		lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
		while(lambdalist){
			osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalistcopy, (t_osc_expr_ast_expr *)lambdalist);
			lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
		}
		t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprs(f);
		t_osc_expr_ast_expr *exprlistcopy = osc_expr_ast_expr_copy(exprlist);
		exprlist = osc_expr_ast_expr_next(exprlist);
		while(exprlist){
			osc_expr_ast_expr_append(exprlistcopy, exprlist);
			exprlist = osc_expr_ast_expr_next(exprlist);
		}
		return (t_osc_expr_ast_expr *)osc_expr_ast_function_alloc(lambdalistcopy, exprlistcopy);
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

t_osc_err osc_expr_ast_function_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_function_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
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
				       osc_expr_ast_function_format,
				       osc_expr_ast_function_formatLisp,
				       osc_expr_ast_function_free,
				       osc_expr_ast_function_copy,
				       osc_expr_ast_function_serialize,
				       osc_expr_ast_function_deserialize,
				       sizeof(t_osc_expr_ast_function));
		osc_expr_ast_function_setLambdaList(f, lambdalist);
		osc_expr_ast_function_setExprs(f, exprs);
	}
	return f;
}

