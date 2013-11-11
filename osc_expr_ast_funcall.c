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
//#include "osc_expr_privatedecls.h"
#include "osc_expr_rec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_funcall.r"


int osc_expr_ast_funcall_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      long *len,
				      char **oscbndl,
				      t_osc_atom_ar_u **out)
{
	return 1;
}

long osc_expr_ast_funcall_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_rec *r = osc_expr_ast_funcall_getRec((t_osc_expr_ast_funcall *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *arg = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)e);
	long offset = 0;
	if(!buf){
		offset += snprintf(NULL, 0, "%s(", osc_expr_rec_getName(r));
		while(arg){
			offset += osc_expr_ast_expr_format(NULL, 0, arg);
			arg = osc_expr_ast_expr_next(arg);
			if(arg){
				offset += snprintf(NULL, 0, ", ");
			}
		}
		offset += snprintf(NULL, 0, ")");
	}else{
		offset += snprintf(buf + offset, n - offset, "%s(", osc_expr_rec_getName(r));
		while(arg){
			offset += osc_expr_ast_expr_format(buf + offset, n - offset, arg);
			arg = osc_expr_ast_expr_next(arg);
			if(arg){
				offset += snprintf(buf + offset, n - offset, ", ");
			}
		}
		offset += snprintf(buf + offset, n - offset, ")");
	}
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_funcall_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_funcall *fc = (t_osc_expr_ast_funcall *)ast;
		t_osc_expr_rec *r = osc_expr_ast_funcall_getRecCopy(fc);
		t_osc_expr_ast_expr *args = osc_expr_ast_funcall_getArgs(fc);
		t_osc_expr_ast_funcall *copy = osc_expr_ast_funcall_alloc(r, 0);
		if(copy){
			while(args){
				t_osc_expr_ast_expr *argcopy = osc_expr_ast_expr_copy(args);
				if(argcopy){
					osc_expr_ast_funcall_appendArg(copy, argcopy);
				}else{
					break;
				}
				args = osc_expr_ast_expr_next(args);
			}
		}
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_funcall_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_mem_free(e);
	}
}

t_osc_expr_funcptr osc_expr_ast_funcall_getFunc(t_osc_expr_ast_funcall *e)
{
	if(e){
		return osc_expr_rec_getFunction(osc_expr_ast_funcall_getRec(e));
	}
	return NULL;
}

t_osc_expr_rec *osc_expr_ast_funcall_getRec(t_osc_expr_ast_funcall *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

t_osc_expr_rec *osc_expr_ast_funcall_getRecCopy(t_osc_expr_ast_funcall *e)
{
	if(e){
		t_osc_expr_rec *r = e->rec;
		t_osc_expr_rec *copy = NULL;
		osc_expr_rec_copy(&copy, r, NULL);
		return copy;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_funcall_getArgs(t_osc_expr_ast_funcall *e)
{
	if(e){
		return e->argv;
	}
	return NULL;
}

int osc_expr_ast_funcall_getNumArgs(t_osc_expr_ast_funcall *e)
{
	if(e){
		return e->argc;
	}
	return 0;
}

void osc_expr_ast_funcall_appendArg(t_osc_expr_ast_funcall *e, t_osc_expr_ast_expr *a)
{
	t_osc_expr_ast_expr *args = osc_expr_ast_funcall_getArgs(e);
	if(args){
		osc_expr_ast_expr_append(args, a);
	}else{
		e->argv = a;
	}
	e->argc++;
}

t_osc_expr_ast_funcall *osc_expr_ast_funcall_allocWithList(t_osc_expr_rec *rec, t_osc_expr_ast_expr *argv)
{
	t_osc_expr_ast_funcall *e = osc_mem_alloc(sizeof(t_osc_expr_ast_funcall));
	if(e){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e, 
				       OSC_EXPR_AST_NODETYPE_FUNCALL, 
				       NULL, 
				       osc_expr_ast_funcall_evalInLexEnv, 
				       osc_expr_ast_funcall_format, 
				       osc_expr_ast_funcall_free, 
				       osc_expr_ast_funcall_copy, 
				       sizeof(t_osc_expr_ast_funcall)); 
		e->rec = rec;
		e->argv = argv;
		t_osc_expr_ast_expr *a = argv;
		while(a){
			e->argc++;
			a = a->next;
		}
	}
	return e;
}

t_osc_expr_ast_funcall *osc_expr_ast_funcall_alloc(t_osc_expr_rec *rec, int argc, ...)
{
	if(argc > 0){
		va_list ap;
		va_start(ap, argc);
		t_osc_expr_ast_expr *argv = va_arg(ap, t_osc_expr_ast_expr*);
		for(int i = 1; i < argc; i++){
			osc_expr_ast_expr_append(argv, va_arg(ap, t_osc_expr_ast_expr*));
		}
		return osc_expr_ast_funcall_allocWithList(rec, argv);
	}else{
		return osc_expr_ast_funcall_allocWithList(rec, NULL);
	}
}
