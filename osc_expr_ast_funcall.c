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
#include "osc_expr_privatedecls.h"
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
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	t_osc_expr_ast_funcall *f = (t_osc_expr_ast_funcall *)ast;
	t_osc_expr_funcptr ff = osc_expr_ast_funcall_getFunc(f);
	if(!ff){
		// wha?
		return 1;
	}

	if(ff == osc_expr_apply){
		return osc_expr_specFunc_apply(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_map){
		return osc_expr_specFunc_map(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_lreduce || ff == osc_expr_rreduce){
		return osc_expr_specFunc_reduce(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_assign){
		return osc_expr_specFunc_assign(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_assign_to_index){
		return osc_expr_specFunc_assigntoindex(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_if){
		return osc_expr_specFunc_if(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_emptybundle){
		return osc_expr_specFunc_emptybundle(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_bound){
		return osc_expr_specFunc_bound(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_exists){
		return osc_expr_specFunc_exists(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_getaddresses){
		return osc_expr_specFunc_getaddresses(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_delete){
		return osc_expr_specFunc_delete(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_getmsgcount){
		return osc_expr_specFunc_getmsgcount(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_value){
		return osc_expr_specFunc_value(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_quote){
		return osc_expr_specFunc_quote(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_eval_call){
		return osc_expr_specFunc_eval(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_tokenize){
		return osc_expr_specFunc_tokenize(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_compile){
		return osc_expr_specFunc_compile(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_gettimetag){
		return osc_expr_specFunc_gettimetag(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_settimetag){
		return osc_expr_specFunc_settimetag(ast, lexenv, len, oscbndl, out);
	}else if(ff == osc_expr_getbundlemember){
		return osc_expr_specFunc_getBundleMember(ast, lexenv, len, oscbndl, out);
	}else{
		//////////////////////////////////////////////////
		// Call normal function
		//////////////////////////////////////////////////
		int f_argc = osc_expr_ast_funcall_getNumArgs(f);
		t_osc_expr_ast_expr *f_argv = osc_expr_ast_funcall_getArgs(f);
		t_osc_atom_ar_u *argv[f_argc];
		memset(argv, '\0', sizeof(argv));
		int ret = 0;
		int i = 0;
		while(f_argv){
			//int ret = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, argv + i);
			int ret = osc_expr_ast_expr_evalInLexEnv(f_argv, lexenv, len, oscbndl, argv + i);
			if(ret){
				if(ret == OSC_ERR_EXPR_ADDRESSUNBOUND){
					// if the type arg type is something else, it will be an expression which means an 
					// error has already been posted
					//if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
					//osc_expr_err_unbound(osc_expr_arg_getOSCAddress(f_argv), osc_expr_rec_getName(osc_expr_getRec(f)));
					//}
				}
				int j;
				for(j = 0; j < i; j++){
					if(argv[j]){
						osc_atom_array_u_free(argv[j]);
					}
				}
				return ret;
			}
			f_argv = osc_expr_ast_expr_next(f_argv);
			i++;
		}
		// call function
	        ret = ff(f, f_argc, argv, out);
		for(i = 0; i < f_argc; i++){
			if(argv[i]){
				osc_atom_array_u_free(argv[i]);
			}
		}
		return ret;
	}
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
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s(", osc_expr_rec_getName(r));
	while(arg){
		offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, arg);
		arg = osc_expr_ast_expr_next(arg);
		if(arg){
			offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ", ");
		}
	}
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

long osc_expr_ast_funcall_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
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
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s", osc_expr_rec_getName(r));
	while(arg){
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
		offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, arg);
		arg = osc_expr_ast_expr_next(arg);
	}
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
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
		osc_expr_ast_expr_free(osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)e));
		osc_mem_free(e);
	}
}

t_osc_err osc_expr_ast_funcall_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_funcall_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
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
				       osc_expr_ast_funcall_formatLisp, 
				       osc_expr_ast_funcall_free, 
				       osc_expr_ast_funcall_copy, 
				       osc_expr_ast_funcall_serialize, 
				       osc_expr_ast_funcall_deserialize, 
				       sizeof(t_osc_expr_ast_funcall)); 
		e->rec = rec;
		e->argv = argv;
		t_osc_expr_ast_expr *a = argv;
		e->argc = 0;
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
