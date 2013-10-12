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
#include "osc_expr_rec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_expr_funcall.h"
#include "osc_expr_ast_expr_funcall.r"

t_osc_expr_ast_expr_funcall *osc_expr_ast_expr_funcall_alloc(t_osc_expr_rec *rec, int argc, ...)
{
	t_osc_expr_ast_expr_funcall *e = osc_mem_alloc(sizeof(t_osc_expr_ast_expr_funcall));
	if(e){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e, OSC_EXPR_AST_EXPR_TYPE_FUNCALL, NULL, osc_expr_ast_expr_funcall_format, osc_expr_ast_expr_funcall_free, sizeof(t_osc_expr_ast_expr_funcall));
		e->rec = rec;
		e->argv = NULL;
		va_list ap;
		va_start(ap, argc);
		for(int i = 0; i < argc; i++){
			osc_expr_ast_expr_funcall_appendArg(e, va_arg(ap, t_osc_expr_ast_expr*));
		}
		e->argc = argc;
	}
	return e;
}

long osc_expr_ast_expr_funcall_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_rec *r = osc_expr_ast_expr_funcall_getRec((t_osc_expr_ast_expr_funcall *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *arg = osc_expr_ast_expr_funcall_getArgs((t_osc_expr_ast_expr_funcall *)e);
	long offset = 0;
	if(!buf){
		offset += snprintf(NULL, 0, "%s(", osc_expr_rec_getName(r));
		while(arg){
			offset += osc_expr_ast_expr_format(NULL, n, arg);
			arg = arg->next;
			if(arg){
				offset += snprintf(NULL, 0, ", ");
			}
		}
		offset += snprintf(NULL, 0, ")");
	}else{
		offset += snprintf(buf + offset, n - offset, "%s(", osc_expr_rec_getName(r));
		while(arg){
			offset += osc_expr_ast_expr_format(buf + offset, n - offset, arg);
			arg = arg->next;
			if(arg){
				offset += snprintf(buf + offset, n - offset, ", ");
			}
		}
		offset += snprintf(buf + offset, n - offset, ")");
	}
	return offset;
}

void osc_expr_ast_expr_funcall_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_mem_free(e);
	}
}

t_osc_expr_rec *osc_expr_ast_expr_funcall_getRec(t_osc_expr_ast_expr_funcall *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_expr_funcall_getArgs(t_osc_expr_ast_expr_funcall *e)
{
	if(e){
		return e->argv;
	}
	return NULL;
}

int osc_expr_ast_expr_funcall_getNumArgs(t_osc_expr_ast_expr_funcall *e)
{
	if(e){
		return e->argc;
	}
	return 0;
}

void osc_expr_ast_expr_funcall_prependArg(t_osc_expr_ast_expr_funcall *e, t_osc_expr_ast_expr *a)
{
	if(!e || !a){
		return;
	}
	a->next = e->argv;
	e->argv = a;
	e->argc++;
}

void osc_expr_ast_expr_funcall_appendArg(t_osc_expr_ast_expr_funcall *e, t_osc_expr_ast_expr *a)
{
	if(!e || !a){
		return;
	}
	if(!(e->argv)){
		e->argv = a;
	}else{
		t_osc_expr_ast_expr *aa = e->argv;
		while(aa->next){
			aa = aa->next;
		}
		aa->next = a;
	}
	e->argc++;
}
