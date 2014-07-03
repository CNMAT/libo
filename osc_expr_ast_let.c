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
#include "osc_util.h"
#include "osc_mem.h"
#include "osc_expr_builtin.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_let.h"
#include "osc_expr_ast_let.r"

int osc_expr_ast_let_evalInLexEnv(t_osc_expr_ast_expr *ast,
				  t_osc_expr_lexenv *lexenv,
				  t_osc_bndl_u *oscbndl,
				  t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_ast_let_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      t_osc_bndl_u *oscbndl,
				      t_osc_msg_u **assign_target,
				      long *nlvals,
				      t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_let_format(char *buf, long n, t_osc_expr_ast_expr *f)
{
	if(f){
		t_osc_expr_ast_let *ff = (t_osc_expr_ast_let *)f;
		char *name = "let";
		t_osc_expr_ast_expr *varlist = osc_expr_ast_let_getVarlist(ff);
		t_osc_expr_ast_expr *e = osc_expr_ast_let_getExprs(ff);
		long offset = 0;
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s(", name);
		while(varlist){
			offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, varlist);
			if(osc_expr_ast_expr_next((t_osc_expr_ast_expr *)varlist)){
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ", ");
			}else{
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "){");
			}
			varlist = osc_expr_ast_expr_next(varlist);
		}
		offset += osc_expr_ast_expr_formatAllLinked(buf ? buf + offset : NULL, buf ? n - offset : 0, e);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "}");
		return offset;
	}
	return 0;
}

long osc_expr_ast_let_formatLisp(char *buf, long n, t_osc_expr_ast_expr *f)
{
	if(f){
		t_osc_expr_ast_let *ff = (t_osc_expr_ast_let *)f;
		char *name = "let";
		t_osc_expr_ast_value *varlist = (t_osc_expr_ast_value *)osc_expr_ast_let_getVarlist(ff);
		t_osc_expr_ast_expr *e = osc_expr_ast_let_getExprs(ff);
		long offset = 0;
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s (", name);
		while(varlist){
			offset += osc_expr_ast_value_format(buf ? buf + offset : NULL, buf ? n - offset : 0, (t_osc_expr_ast_expr *)varlist);
			if(osc_expr_ast_expr_next((t_osc_expr_ast_expr *)varlist)){
				offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
			}
			varlist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)varlist);
		}
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ") ");
		offset += osc_expr_ast_expr_formatAllLinkedLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, e);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
		return offset;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_let_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_let *f = (t_osc_expr_ast_let *)ast;
		t_osc_expr_ast_value *varlist = (t_osc_expr_ast_value *)osc_expr_ast_let_getVarlist(f);
		t_osc_expr_ast_value *varlistcopy = (t_osc_expr_ast_value *)osc_expr_ast_value_copy((t_osc_expr_ast_expr *)varlist);
		varlist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)varlist);
		while(varlist){
			osc_expr_ast_expr_append((t_osc_expr_ast_expr *)varlistcopy, (t_osc_expr_ast_expr *)varlist);
			varlist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)varlist);
		}
		t_osc_expr_ast_expr *exprlist = osc_expr_ast_let_getExprs(f);
		t_osc_expr_ast_expr *exprlistcopy = osc_expr_ast_expr_copy(exprlist);
		exprlist = osc_expr_ast_expr_next(exprlist);
		while(exprlist){
			osc_expr_ast_expr_append(exprlistcopy, exprlist);
			exprlist = osc_expr_ast_expr_next(exprlist);
		}
		return (t_osc_expr_ast_expr *)osc_expr_ast_let_alloc((t_osc_expr_ast_expr *)varlistcopy, exprlistcopy);
	}else{
		return NULL;
	}
}

void osc_expr_ast_let_free(t_osc_expr_ast_expr *f)
{
	if(f){
		// free the varlist because it's a copy
	        osc_expr_ast_expr_free((t_osc_expr_ast_expr *)osc_expr_ast_let_getVarlist((t_osc_expr_ast_let *)f));
		osc_expr_ast_funcall_free(f);
	}
}

t_osc_bndl_u *osc_expr_ast_let_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_let *l = (t_osc_expr_ast_let *)e;
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_LET);
	t_osc_msg_u *varlist = osc_message_u_allocWithBndl_u("/varlist", osc_expr_ast_expr_toBndl(osc_expr_ast_let_getVarlist(l)), 1);
	t_osc_msg_u *exprs = osc_message_u_allocWithBndl_u("/exprlist", osc_expr_ast_expr_toBndl(osc_expr_ast_let_getExprs(l)), 1);
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(b, nodetype);
	osc_bundle_u_addMsg(b, varlist);
	osc_bundle_u_addMsg(b, exprs);
	return b;
}

t_osc_expr_ast_expr *osc_expr_ast_let_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	return NULL;
}

void osc_expr_ast_let_setVarlist(t_osc_expr_ast_let *f, t_osc_expr_ast_expr *varlist)
{
	if(f){
		f->varlist = (t_osc_expr_ast_value *)varlist;
	}
}

void osc_expr_ast_let_setExprs(t_osc_expr_ast_let *f, t_osc_expr_ast_expr *exprs)
{
	if(f){
		f->exprs = exprs;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_let_getVarlist(t_osc_expr_ast_let *f)
{
	if(f){
		return (t_osc_expr_ast_expr *)f->varlist;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_let_getExprs(t_osc_expr_ast_let *f)
{
	if(f){
		return f->exprs;
	}
	return 0;
}

t_osc_expr_ast_let *osc_expr_ast_let_alloc(t_osc_expr_ast_expr *varlist, t_osc_expr_ast_expr *exprs)
{
	t_osc_expr_ast_let *f = osc_mem_alloc(sizeof(t_osc_expr_ast_let));
	if(f){
		memset(f, '\0', sizeof(t_osc_expr_ast_let));

		// convert let(a = 10, b = /b){a + b;} to apply(lambda(a, b){a + b;}, 10, /b)
		t_osc_expr_ast_expr *v = varlist;
		t_osc_expr_ast_value *lambdalist = NULL;
		t_osc_expr_ast_expr *rval = NULL;
		while(v){
			t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)v;
			if(!lambdalist){
				lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
				//lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_binaryop_getLeftArg(b);
			}else{
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b)));
				//osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, osc_expr_ast_binaryop_getLeftArg(b));
			}
			if(!rval){
				rval = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getRightArg(b));
				//rval = osc_expr_ast_binaryop_getRightArg(b);
			}else{
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)rval, osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getRightArg(b)));
				//osc_expr_ast_expr_append((t_osc_expr_ast_expr *)rval, osc_expr_ast_binaryop_getRightArg(b));
			}
			v = osc_expr_ast_expr_next(v);
		}
		t_osc_expr_ast_function *lambda = osc_expr_ast_function_alloc(lambdalist, exprs);
		osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambda, rval);

		t_osc_expr_funcrec *funcrec = osc_expr_builtin_func_apply;
		osc_expr_ast_funcall_initWithList((t_osc_expr_ast_funcall *)f,
						  OSC_EXPR_AST_NODETYPE_LET,
						  NULL,
						  NULL,
						  NULL,
						  osc_expr_ast_let_format,
						  NULL,
						  osc_expr_ast_let_free,
						  osc_expr_ast_let_copy,
						  osc_expr_ast_let_toBndl,
						  osc_expr_ast_let_fromBndl,
						  sizeof(t_osc_expr_ast_let),
						  funcrec,
				  		  osc_expr_ast_value_allocIdentifier(osc_atom_u_allocWithString(osc_expr_funcrec_getName(funcrec))),
						  (t_osc_expr_ast_expr *)lambda);

		osc_expr_ast_let_setVarlist(f, varlist);
		osc_expr_ast_let_setExprs(f, exprs);
	}
	return f;
}

