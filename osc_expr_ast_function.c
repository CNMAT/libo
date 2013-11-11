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
#include "osc_expr.h"
#include "osc_expr_func.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_function.r"

int osc_expr_ast_function_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       long *len,
				       char **oscbndl,
				       t_osc_atom_ar_u **out)
{
	// this shouldn't happen
	// if it does, the cute thing would be to put it in a bundle and stick it in an atom
	return 1;
}

long osc_expr_ast_function_format(char *buf, long n, t_osc_expr_ast_expr *f)
{
	if(f){
		t_osc_expr_ast_function *ff = (t_osc_expr_ast_function *)f;
		char *name = osc_expr_ast_function_getName(ff);
		int nparams = osc_expr_ast_function_getNumParams(ff);
		char **params = osc_expr_ast_function_getParams(ff);
		t_osc_expr_ast_expr *e = osc_expr_ast_function_getExprList(ff);
		long offset = 0;
		if(!buf){
			offset += snprintf(NULL, 0, "%s(", name);
			for(int i = 0; i < nparams; i++){
				if(i < nparams - 1){
					offset += snprintf(NULL, 0, "%s, ", params[i]);
				}else{
					offset += snprintf(NULL, 0, "%s){", params[i]);
				}
			}
			while(e){
				offset += osc_expr_ast_expr_format(NULL, 0, e);
				offset += snprintf(NULL, 0, ";");
				e = e->next;
			}
			offset += snprintf(NULL, 0, "}");
		}else{
			offset += snprintf(buf + offset, n - offset, "%s(", name);
			for(int i = 0; i < nparams; i++){
				if(i < nparams - 1){
					offset += snprintf(buf + offset, n - offset, "%s, ", params[i]);
				}else{
					offset += snprintf(buf + offset, n - offset, "%s){", params[i]);
				}
			}
			while(e){
				offset += osc_expr_ast_expr_format(buf + offset, n - offset, e);
				offset += snprintf(buf + offset, n - offset, ";");
				e = e->next;
			}
			offset += snprintf(buf + offset, n - offset, "}");
		}
		return offset;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_function_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_function *f = (t_osc_expr_ast_function *)ast;
		int nparams = osc_expr_ast_function_getNumParams(f);
		char **params = osc_expr_ast_function_getParams(f);
		t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprList(f);
		t_osc_expr_ast_expr *copy = osc_expr_ast_expr_copy(exprlist);
		return (t_osc_expr_ast_expr *)osc_expr_ast_function_alloc(nparams, params, copy);
	}else{
		return NULL;
	}
}

void osc_expr_ast_function_free(t_osc_expr_ast_expr *f)
{
	if(f){
		osc_mem_free(f);
	}
}

char *osc_expr_ast_function_getName(t_osc_expr_ast_function *f)
{
	if(f){
		return osc_expr_rec_getName(f->function);
	}
	return NULL;
}

int osc_expr_ast_function_getNumParams(t_osc_expr_ast_function *f)
{
	if(f){
		return osc_expr_rec_getNumRequiredArgs(f->function);
	}
	return 0;
}

char **osc_expr_ast_function_getParams(t_osc_expr_ast_function *f)
{
	if(f){
		return osc_expr_rec_getRequiredArgsNames(f->function);
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_function_getExprList(t_osc_expr_ast_function *f)
{
	if(f){
		return osc_expr_rec_getExtra(f->function);
	}
	return 0;
}

t_osc_expr_ast_function *osc_expr_ast_function_alloc(int numparams, char **params, t_osc_expr_ast_expr *exprs)
{
	t_osc_expr_ast_function *f = osc_mem_alloc(sizeof(t_osc_expr_ast_function));
	if(f){
		memset(f, '\0', sizeof(t_osc_expr_ast_function));
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)f, OSC_EXPR_AST_NODETYPE_FUNCTION, NULL, osc_expr_ast_function_evalInLexEnv, osc_expr_ast_function_format, osc_expr_ast_function_free, osc_expr_ast_function_copy, sizeof(t_osc_expr_ast_function));
		f->function = osc_expr_rec_alloc();
		osc_expr_rec_setName(f->function, "lambda");
		osc_expr_rec_setRequiredArgs(f->function, numparams, params, NULL);
		osc_expr_rec_setFunction(f->function, osc_expr_lambda);
		osc_expr_rec_setExtra(f->function, exprs);
	}
	return f;
}
