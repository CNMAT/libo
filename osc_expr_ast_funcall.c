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
#include "osc_expr_privatedecls.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_funcall.r"
#include "osc_expr_builtin.h"
#include "osc_typetag.h"

void osc_expr_funcall_expandScalars(int argc, t_osc_atom_ar_u **argv, int scalar_expansion_index_count, unsigned int *scalar_expansion_indexes)
{
	int sargc = scalar_expansion_index_count;
	unsigned int *sargv = scalar_expansion_indexes;
	int havescalar = 0;
	int havelist = 0;
	uint64_t length_of_shortest_list = ~0ull;
	for(int i = 0; i < sargc; i++){
		if(sargv[i] >= argc){
			printf("%s: error: index %d (%d) is greater than argc (%d)\n", __func__, i, sargv[i], argc);
			continue;
		}
		int l = osc_atom_array_u_getLen(argv[sargv[i]]);
		if(l <= 0){
			// weird
		}else if(l == 1){
			havescalar = 1;
		}else{
			havelist = 1;
			if(l < length_of_shortest_list){
				length_of_shortest_list = l;
			}
		}
	}
	if(havelist && havescalar){
		for(int i = 0; i < sargc; i++){
			int si = sargv[i];
			t_osc_atom_ar_u *ar = argv[si];
			int l = osc_atom_array_u_getLen(ar);
			if(l <= 0){
				// still weird
			}else if(l == 1){
				osc_array_resize(ar, length_of_shortest_list);
				t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
				for(int j = 1; j < length_of_shortest_list; j++){
					t_osc_atom_u *aa = osc_atom_array_u_get(ar, j);
					osc_atom_u_copy(&aa, a);
				}
			}
		}
	}
}

void osc_expr_funcall_promoteToLargestType(int argc, t_osc_atom_ar_u **argv, int arg_indexes_count, unsigned int *arg_indexes_to_promote)
{
	int aic = arg_indexes_count;
	unsigned int *aiv = arg_indexes_to_promote;
	// scalars should have been expanded already
	int n = osc_atom_array_u_getLen(*argv);
	t_osc_atom_u *av[aic];
	for(int i = 0; i < n; i++){
		for(int j = 0; j < aic; j++){
			av[j] = osc_atom_array_u_get(argv[aiv[j]], i);
		}
		char tt = osc_typetag_getLargestType(aic, av);
		if(!tt){
			// typetags couldn't be harmonized---print an error
			continue;
		}

		for(int j = 0; j < aic; j++){
			char att = osc_atom_u_getTypetag(av[j]);
			if(att != tt){
				switch(tt){
				case 'N':
					osc_atom_u_setNull(av[j]);
					break;
				case 'T':
				case 'F':
					// do nothing
					break;
				case 'c':
					osc_atom_u_setInt8(av[j], osc_atom_u_getInt8(av[j]));
					break;
				case 'C':
					osc_atom_u_setUInt8(av[j], osc_atom_u_getUInt8(av[j]));
					break;
				case 'u':
					osc_atom_u_setInt16(av[j], osc_atom_u_getInt16(av[j]));
					break;
				case 'U':
					osc_atom_u_setUInt16(av[j], osc_atom_u_getUInt16(av[j]));
					break;
				default:
				case 'i':
					osc_atom_u_setInt32(av[j], osc_atom_u_getInt32(av[j]));
					break;
				case 'I':
					osc_atom_u_setUInt32(av[j], osc_atom_u_getUInt32(av[j]));
					break;
				case 'h':
					osc_atom_u_setInt64(av[j], osc_atom_u_getInt64(av[j]));
					break;
				case 'H':
					osc_atom_u_setUInt64(av[j], osc_atom_u_getUInt64(av[j]));
					break;
				case 'f':
					osc_atom_u_setFloat(av[j], osc_atom_u_getFloat(av[j]));
					break;
				case 'd':
					osc_atom_u_setDouble(av[j], osc_atom_u_getDouble(av[j]));
					break;
				case OSC_TIMETAG_TYPETAG:
					// FIXME
					break;
				case 's':
					{
						long n = osc_atom_u_getStringLen(av[j]);
						char buf[n + 1];
						osc_atom_u_getString(av[j], n + 1, (char **)&buf);
						osc_atom_u_setString(av[j], buf);
					}
					break;
				}
			}
		}
	}
}

int osc_expr_ast_funcall_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      t_osc_bndl_u *oscbndl,
				      t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_funcall *f = (t_osc_expr_ast_funcall *)ast;
	t_osc_expr_builtin_funcptr ff = osc_expr_ast_funcall_getFunc(f);
	t_osc_expr_funcrec *funcrec = osc_expr_ast_funcall_getFuncRec(f);
	if(!ff){
		// wha?
		return 1;
	}
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	if(ff == osc_expr_builtin_apply){
		return osc_expr_specFunc_apply(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_map){
		return osc_expr_specFunc_map(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_lreduce || ff == osc_expr_builtin_rreduce){
		return osc_expr_specFunc_reduce(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_assign){
		return osc_expr_specFunc_assign(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_assigntoindex){
		return osc_expr_specFunc_assigntoindex(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_if){
		return osc_expr_specFunc_if(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_emptybundle){
		return osc_expr_specFunc_emptybundle(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_bound){
		return osc_expr_specFunc_bound(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_exists){
		return osc_expr_specFunc_exists(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_getaddresses){
		return osc_expr_specFunc_getaddresses(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_delete){
		return osc_expr_specFunc_delete(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_getmsgcount){
		return osc_expr_specFunc_getmsgcount(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_value){
		return osc_expr_specFunc_value(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_quote){
		return osc_expr_specFunc_quote(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_eval){
		return osc_expr_specFunc_eval(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_tokenize){
		return osc_expr_specFunc_tokenize(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_gettimetag){
		return osc_expr_specFunc_gettimetag(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_settimetag){
		return osc_expr_specFunc_settimetag(ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_lookup){
		return osc_expr_specFunc_lookup(ast, lexenv, oscbndl, out);
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
			int ret = osc_expr_ast_expr_evalInLexEnv(f_argv, lexenv, oscbndl, argv + i);
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
		// arity check and possible partial application
		// scalar expansion
		osc_expr_funcall_expandScalars(f_argc, argv, osc_expr_funcrec_getScalarExpansionArgc(funcrec), osc_expr_funcrec_getScalarExpansionArgv(funcrec));
		// type promotion
		osc_expr_funcall_promoteToLargestType(f_argc, argv, osc_expr_funcrec_getTypePromotionArgc(funcrec), osc_expr_funcrec_getTypePromotionArgv(funcrec));
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
	t_osc_expr_funcrec *r = osc_expr_ast_funcall_getFuncRec((t_osc_expr_ast_funcall *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *arg = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)e);
	long offset = 0;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s(", osc_expr_funcrec_getName(r));
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
	t_osc_expr_funcrec *r = osc_expr_ast_funcall_getFuncRec((t_osc_expr_ast_funcall *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *arg = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)e);
	long offset = 0;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s", osc_expr_funcrec_getName(r));
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
		t_osc_expr_funcrec *r = osc_expr_ast_funcall_getFuncRec(fc);
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

t_osc_expr_builtin_funcptr osc_expr_ast_funcall_getFunc(t_osc_expr_ast_funcall *e)
{
	if(e){
		return osc_expr_funcrec_getFunc(osc_expr_ast_funcall_getFuncRec(e));
	}
	return NULL;
}

t_osc_expr_funcrec *osc_expr_ast_funcall_getFuncRec(t_osc_expr_ast_funcall *e)
{
	if(e){
		return e->rec;
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

void osc_expr_ast_funcall_initWithList(t_osc_expr_ast_funcall *e,
				       int nodetype,
				       t_osc_expr_ast_expr *next,
				       t_osc_expr_ast_evalfn evalfn,
				       t_osc_expr_ast_formatfn formatfn,
				       t_osc_expr_ast_formatfn format_lispfn,
				       t_osc_expr_ast_freefn freefn,
				       t_osc_expr_ast_copyfn copyfn,
				       t_osc_expr_ast_serializefn serializefn,
				       t_osc_expr_ast_deserializefn deserializefn,
				       size_t objsize,
				       t_osc_expr_funcrec *rec,
				       t_osc_expr_ast_expr *argv)
{
	if(e){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e, 
				       nodetype,
				       next,
				       evalfn ? evalfn : osc_expr_ast_funcall_evalInLexEnv,
				       formatfn ? formatfn : osc_expr_ast_funcall_format,
				       format_lispfn ? format_lispfn : osc_expr_ast_funcall_formatLisp,
				       freefn ? freefn : osc_expr_ast_funcall_free,
				       copyfn ? copyfn : osc_expr_ast_funcall_copy,
				       serializefn ? serializefn : osc_expr_ast_funcall_serialize,
				       deserializefn ? deserializefn : osc_expr_ast_funcall_deserialize,
				       objsize);
		e->rec = rec;
		e->argv = argv;
		t_osc_expr_ast_expr *a = argv;
		e->argc = 0;
		while(a){
			e->argc++;
			a = osc_expr_ast_expr_next(a);
		}
	}
}

void osc_expr_ast_funcall_init(t_osc_expr_ast_funcall *e,
				       int nodetype,
				       t_osc_expr_ast_expr *next,
				       t_osc_expr_ast_evalfn evalfn,
				       t_osc_expr_ast_formatfn formatfn,
				       t_osc_expr_ast_formatfn format_lispfn,
				       t_osc_expr_ast_freefn freefn,
				       t_osc_expr_ast_copyfn copyfn,
				       t_osc_expr_ast_serializefn serializefn,
				       t_osc_expr_ast_deserializefn deserializefn,
				       size_t objsize,
				       t_osc_expr_funcrec *rec,
				       int argc,
				       ...)
{
	t_osc_expr_ast_expr *argv = NULL;
	if(argc > 0){
		va_list ap;
		va_start(ap, argc);
		argv = va_arg(ap, t_osc_expr_ast_expr*);
		for(int i = 1; i < argc; i++){
			t_osc_expr_ast_expr *ee = va_arg(ap, t_osc_expr_ast_expr*);
			osc_expr_ast_expr_append(argv, ee);
		}
	}
	osc_expr_ast_funcall_initWithList(e,
					  nodetype,
					  next,
					  evalfn,
					  formatfn,
					  format_lispfn,
					  freefn,
					  copyfn,
					  serializefn,
					  deserializefn,
					  objsize,
					  rec,
					  argv);
}

t_osc_expr_ast_funcall *osc_expr_ast_funcall_allocWithList(t_osc_expr_funcrec *rec, t_osc_expr_ast_expr *argv)
{
	t_osc_expr_ast_funcall *e = osc_mem_alloc(sizeof(t_osc_expr_ast_funcall));
	if(e){
		osc_expr_ast_funcall_initWithList(e, 
					  OSC_EXPR_AST_NODETYPE_FUNCALL, 
					  NULL, 
					  osc_expr_ast_funcall_evalInLexEnv, 
					  osc_expr_ast_funcall_format, 
					  osc_expr_ast_funcall_formatLisp, 
					  osc_expr_ast_funcall_free, 
					  osc_expr_ast_funcall_copy, 
					  osc_expr_ast_funcall_serialize, 
					  osc_expr_ast_funcall_deserialize, 
					  sizeof(t_osc_expr_ast_funcall),
					  rec,
					  argv);
	}
	return e;
}

t_osc_expr_ast_funcall *osc_expr_ast_funcall_alloc(t_osc_expr_funcrec *rec, int argc, ...)
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
