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
#include "osc_expr_builtins.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_let.h"
#include "osc_expr_ast_let.r"

int osc_expr_ast_let_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       long *len,
				       char **oscbndl,
				       t_osc_atom_ar_u **out)
{
	return 1;
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
		osc_expr_ast_expr_free((t_osc_expr_ast_expr *)osc_expr_ast_let_getVarlist((t_osc_expr_ast_let *)f));
		osc_expr_ast_expr_free((t_osc_expr_ast_expr *)osc_expr_ast_let_getExprs((t_osc_expr_ast_let *)f));
		osc_mem_free(f);
	}
}

t_osc_err osc_expr_ast_let_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_let_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
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
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)f,
				       OSC_EXPR_AST_NODETYPE_LET,
				       NULL,
				       osc_expr_ast_let_evalInLexEnv,
				       osc_expr_ast_let_format,
				       osc_expr_ast_let_formatLisp,
				       osc_expr_ast_let_free,
				       osc_expr_ast_let_copy,
				       osc_expr_ast_let_serialize,
				       osc_expr_ast_let_deserialize,
				       sizeof(t_osc_expr_ast_let));
		osc_expr_ast_let_setVarlist(f, varlist);
		osc_expr_ast_let_setExprs(f, exprs);
	}
	return f;
}

