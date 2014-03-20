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
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_value.h"
#include "osc_atom_u.h"
#include "osc_expr_builtin.h"
#include "osc_typetag.h"

//#define __OSC_PROFILE__
#include "osc_profile.h"

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

int osc_expr_funcall_promoteToLargestType(int argc, t_osc_atom_ar_u **argv, int arg_indexes_count, unsigned int *arg_indexes_to_promote, char **type_constraints)
{
	int aic = arg_indexes_count;
	unsigned int *aiv = arg_indexes_to_promote;
	// scalars should have been expanded already
	int n = osc_atom_array_u_getLen(*argv);
	t_osc_atom_u *av[aic];
	for(int i = 0; i < n; i++){
		//char tt = osc_atom_u_getTypetag(osc_atom_array_u_get(argv[aiv[0]], 0));
		char tt = 0;
		for(int arg = 0; arg < aic; arg++){
			av[arg] = osc_atom_array_u_get(argv[aiv[arg]], i);
			tt = osc_typetag_findLUB(tt, osc_atom_u_getTypetag(av[arg]));
		}
		for(int j = 0; j < aic; j++){
			char att = osc_atom_u_getTypetag(av[j]);
			if(att != tt){
				switch(tt){
				case 'N':
					osc_atom_u_setNull(av[j]);
					break;
				case OSC_BOOL_TYPETAG:
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
				case OSC_EXPR_TYPETAG:
				case OSC_BUNDLE_TYPETAG:
					// these can't happen since nothing can be promoted to them
					break;
				case 's':
					{
						char *buf = NULL;
						osc_atom_u_getString(av[j], 0, &buf);
						osc_atom_u_setStringPtr(av[j], buf);
					}
					break;
				}
			}
		}
	}
	return 0;
}
/*
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
*/

#define OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(funcname)if(ff == osc_expr_builtin_##funcname) { \
		return osc_expr_specFunc_##funcname((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out); \
}
#define OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(funcname)		\
	if(ff == osc_expr_builtin_lval_##funcname){				\
		if(osc_expr_builtin_lval_##funcname){			\
			return osc_expr_specFunc_lval_##funcname((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, assign_target, nlvals, lvals); \
		}else{							\
			return 1;					\
		}							\
	}

//#define OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(funcname) __OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(funcname)
//#define OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(funcname) __OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(lval_##funcname)

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
	/*
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(lreduce);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(rreduce);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(emptybundle);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(bound);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(exists);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(getaddresses);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(delete);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(getmsgcount);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(value);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(quote);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(eval);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(tokenize);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(gettimetag);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC(settimetag);
	*/
	if(ff == osc_expr_builtin_assign){
		return osc_expr_specFunc_assign((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_nth){
		return osc_expr_specFunc_nth((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_lookup){
		return osc_expr_specFunc_lookup((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_apply){
		return osc_expr_specFunc_apply((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_map){
		return osc_expr_specFunc_map((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_if){
		return osc_expr_specFunc_if((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_emptybundle){
		return osc_expr_specFunc_emptybundle((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else if(ff == osc_expr_builtin_bundle){
		return osc_expr_specFunc_bundle((t_osc_expr_ast_funcall *)ast, lexenv, oscbndl, out);
	}else{
		//////////////////////////////////////////////////
		// Call normal function
		//////////////////////////////////////////////////
		int f_argc = osc_expr_ast_funcall_getNumArgs(f);
		t_osc_expr_ast_expr *f_argv = osc_expr_ast_funcall_getArgs(f);
		t_osc_atom_ar_u *argv[f_argc];
		memset(argv, '\0', sizeof(argv));
		int ret = 0;
		int arity = osc_expr_funcrec_getInputArity(funcrec);
/*
		if(f_argc < arity){
			// partial function application: wrap expression in a lambda and return it
			int lambdalistlen = arity - f_argc;
			char *varnames[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
			t_osc_atom_u *a = osc_atom_u_alloc();
			osc_atom_u_setString(a, varnames[0]);
			t_osc_expr_ast_value *lambdalist = osc_expr_ast_value_allocIdentifier(a);
			for(int i = 1; i < lambdalistlen; i++){
				t_osc_atom_u *a = osc_atom_u_alloc();
				osc_atom_u_setString(a, varnames[i]);
				t_osc_expr_ast_value *v = osc_expr_ast_value_allocIdentifier(a);
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, (t_osc_expr_ast_expr *)v);
			}
			t_osc_expr_ast_function *function = osc_expr_ast_function_alloc(lambdalist, ast);
		}
*/
		t_osc_expr_ast_value *lambdalist = NULL;
		int make_lambda_abstraction = 0;
		int i = 0;
		while(f_argv){
			int ret = osc_expr_ast_expr_evalInLexEnv(f_argv, lexenv, oscbndl, argv + i);
			if(ret){
				if(ret == OSC_ERR_EXPR_ADDRESSUNBOUND){
					t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)f_argv;
					if(osc_expr_ast_value_getValueType(v) == OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER){
						t_osc_expr_ast_value *vcopy = osc_expr_ast_value_copy((t_osc_expr_ast_value *)v);
						if(lambdalist){
							osc_expr_ast_expr_append(lambdalist, vcopy);
						}else{
							lambdalist = vcopy;
						}
					}else if(osc_expr_ast_value_getValueType(v) == OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS){
						make_lambda_abstraction = 1;
					}
				}else{
					int j;
					for(j = 0; j < i; j++){
						if(argv[j]){
							osc_atom_array_u_free(argv[j]);
						}
					}
					return ret;
				}
			}
			if(osc_atom_array_u_getLen(argv[i]) == 1){
				if(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[i], 0)) == OSC_EXPR_TYPETAG){
					t_osc_atom_u *a = osc_atom_array_u_get(argv[i], 0);
					t_osc_expr_ast_expr *e = osc_atom_u_getExpr(a);
					switch(osc_expr_ast_expr_getNodetype(e)){
					case OSC_EXPR_AST_NODETYPE_FUNCTION:
						{
							make_lambda_abstraction = 1;
							t_osc_expr_ast_expr *body = osc_expr_ast_function_getExprs(e);
							if(lambdalist){
								osc_expr_ast_expr_append(lambdalist, osc_expr_ast_function_getLambdaList(e));
							}else{
								lambdalist = osc_expr_ast_function_getLambdaList(e);
							}
							osc_mem_free(e);
							osc_atom_u_setExpr(a, body, 1);
						}
						break;
					case OSC_EXPR_AST_NODETYPE_VALUE:
						break;
					default:
						// error
						return 1;
					}
				}
			}
			f_argv = osc_expr_ast_expr_next(f_argv);
			i++;
		}
		if(make_lambda_abstraction){
			t_osc_expr_ast_expr *e = NULL;
			for(int j = 0; j < i; j++){
				t_osc_expr_ast_expr *ee = osc_expr_ast_value_allocList(argv[j]);
				if(e){
					osc_expr_ast_expr_append(e, ee);
				}else{
					e = ee;
				}
			}
			t_osc_expr_ast_funcall *fc = osc_expr_ast_funcall_allocWithList(osc_expr_ast_funcall_getFuncRec(f), e);
			t_osc_expr_ast_function *pfunc = osc_expr_ast_function_alloc(lambdalist, (t_osc_expr_ast_expr *)fc);
			*out = osc_atom_array_u_alloc(1);
			t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
			osc_atom_u_setExpr(a, (t_osc_expr_ast_expr *)pfunc, 1);
			return 0;
		}else{
			// arity check and possible partial application

			// scalar expansion
			OSC_PROFILE_TIMER_START(funcall_expand_scalars);
			osc_expr_funcall_expandScalars(f_argc, argv, osc_expr_funcrec_getScalarExpansionArgc(funcrec), osc_expr_funcrec_getScalarExpansionArgv(funcrec));
			OSC_PROFILE_TIMER_STOP(funcall_expand_scalars);
			OSC_PROFILE_TIMER_PRINTF(funcall_expand_scalars);
			// type promotion
			OSC_PROFILE_TIMER_START(funcall_promote_types);
			ret = osc_expr_funcall_promoteToLargestType(f_argc, argv, osc_expr_funcrec_getTypePromotionArgc(funcrec), osc_expr_funcrec_getTypePromotionArgv(funcrec), osc_expr_funcrec_getParamTypeConstraints(funcrec));
			OSC_PROFILE_TIMER_STOP(funcall_promote_types);
			OSC_PROFILE_TIMER_PRINTF(funcall_promote_types);
			if(ret){
				return ret;
			}
			// call function
			OSC_PROFILE_TIMER_START(funcall_call);
			ret = ff(f, f_argc, argv, out);
			OSC_PROFILE_TIMER_STOP(funcall_call);
			OSC_PROFILE_TIMER_PRINTF(funcall_call);
			for(i = 0; i < f_argc; i++){
				if(argv[i]){
					osc_atom_array_u_free(argv[i]);
				}
			}
			return ret;
		}
	}
	return 1;
}

int osc_expr_ast_funcall_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					  t_osc_expr_lexenv *lexenv,
					  t_osc_bndl_u *oscbndl,
					  t_osc_msg_u **assign_target,
					  long *nlvals,
					  t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_funcall *f = (t_osc_expr_ast_funcall *)ast;
	t_osc_expr_builtin_lvalfuncptr ff = osc_expr_ast_funcall_getLvalFunc(f);
	t_osc_expr_funcrec *funcrec = osc_expr_ast_funcall_getFuncRec(f);
	if(!ff){
		printf("illegal function (%s) as lvalue\n", osc_expr_funcrec_getName(funcrec));
		return 1;
	}
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	/*
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(apply);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(map);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(lreduce);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(rreduce);
	*/
	//OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(assign);
	/*
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(if);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(emptybundle);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(bound);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(exists);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(getaddresses);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(delete);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(getmsgcount);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(value);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(quote);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(eval);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(tokenize);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(gettimetag);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(settimetag);
	*/
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(nth);
	OSC_EXPR_AST_FUNCALL_EVALSPECFUNC_LVAL(lookup);

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
		t_osc_expr_ast_expr *argcopy = NULL;
		int n = 0;
		while(args){
			t_osc_expr_ast_expr *ac = osc_expr_ast_expr_copy(args);
			if(argcopy){
				//osc_expr_ast_funcall_appendArg(copy, argcopy);
				osc_expr_ast_expr_append(argcopy, ac);
			}else{
				//break;
				argcopy = ac;
			}
			args = osc_expr_ast_expr_next(args);
			n++;
		}
		t_osc_expr_ast_funcall *copy = osc_expr_ast_funcall_allocWithList(r, argcopy);
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

t_osc_bndl_u *osc_expr_ast_funcall_toBndl(t_osc_expr_ast_expr *e)
{
	t_osc_expr_ast_funcall *f = (t_osc_expr_ast_funcall *)e;
	t_osc_bndl_u *bndl = osc_bundle_u_alloc();
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_FUNCALL);
	t_osc_bndl_u *recbndl = osc_expr_funcrec_toBndl(osc_expr_ast_funcall_getFuncRec(f));
	t_osc_msg_u *recmsg = osc_message_u_allocWithBndl_u("/record", recbndl, 1);
	t_osc_bndl_u *argbndl = osc_expr_ast_expr_toBndl(osc_expr_ast_funcall_getArgs(f));
	t_osc_msg_u *argmsg = osc_message_u_allocWithBndl_u("/arglist", argbndl, 1);
	osc_bundle_u_addMsg(bndl, nodetype);
	osc_bundle_u_addMsg(bndl, recmsg);
	osc_bundle_u_addMsg(bndl, argmsg);
	return bndl;
}

t_osc_expr_ast_expr *osc_expr_ast_funcall_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	return NULL;
}

t_osc_expr_builtin_funcptr osc_expr_ast_funcall_getFunc(t_osc_expr_ast_funcall *e)
{
	if(e){
		return osc_expr_funcrec_getFunc(osc_expr_ast_funcall_getFuncRec(e));
	}
	return NULL;
}

t_osc_expr_builtin_lvalfuncptr osc_expr_ast_funcall_getLvalFunc(t_osc_expr_ast_funcall *e)
{
	if(e){
		return osc_expr_funcrec_getLvalFunc(osc_expr_ast_funcall_getFuncRec(e));
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

void osc_expr_ast_funcall_setArgs(t_osc_expr_ast_funcall *e, t_osc_expr_ast_expr *args)
{
	if(e){
		e->argv = args;
		t_osc_expr_ast_expr *a = args;
		int i = 0;
		while(a){
			i++;
			a = osc_expr_ast_expr_next(a);
		}
		e->argc = i;
	}
}

int osc_expr_ast_funcall_getNumArgs(t_osc_expr_ast_funcall *e)
{
	if(e){
		return e->argc;
	}
	return 0;
}
/* something weird here...
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
*/
void osc_expr_ast_funcall_initWithList(t_osc_expr_ast_funcall *e,
				       int nodetype,
				       t_osc_expr_ast_expr *next,
				       t_osc_expr_ast_evalfn evalfn,
				       t_osc_expr_ast_evallvalfn evallvalfn,
				       t_osc_expr_ast_formatfn formatfn,
				       t_osc_expr_ast_formatfn format_lispfn,
				       t_osc_expr_ast_freefn freefn,
				       t_osc_expr_ast_copyfn copyfn,
				       t_osc_expr_ast_tobndlfn tobndlfn,
				       t_osc_expr_ast_frombndlfn frombndlfn,
				       size_t objsize,
				       t_osc_expr_funcrec *rec,
				       t_osc_expr_ast_expr *argv)
{
	if(e){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e, 
				       nodetype,
				       next,
				       evalfn ? evalfn : osc_expr_ast_funcall_evalInLexEnv,
				       evallvalfn ? evallvalfn : osc_expr_ast_funcall_evalLvalInLexEnv,
				       formatfn ? formatfn : osc_expr_ast_funcall_format,
				       format_lispfn ? format_lispfn : osc_expr_ast_funcall_formatLisp,
				       freefn ? freefn : osc_expr_ast_funcall_free,
				       copyfn ? copyfn : osc_expr_ast_funcall_copy,
				       tobndlfn ? tobndlfn : osc_expr_ast_funcall_toBndl,
				       frombndlfn ? frombndlfn : osc_expr_ast_funcall_fromBndl,
				       objsize);
		e->rec = rec;
		t_osc_expr_ast_expr *a = argv;
		int argc = 0;
		while(a){
			argc++;
			a = osc_expr_ast_expr_next(a);
		}
		e->argc = argc;
		e->argv = argv;
	}
}

void osc_expr_ast_funcall_init(t_osc_expr_ast_funcall *e,
				       int nodetype,
				       t_osc_expr_ast_expr *next,
				       t_osc_expr_ast_evalfn evalfn,
				       t_osc_expr_ast_evallvalfn evallvalfn,
				       t_osc_expr_ast_formatfn formatfn,
				       t_osc_expr_ast_formatfn format_lispfn,
				       t_osc_expr_ast_freefn freefn,
				       t_osc_expr_ast_copyfn copyfn,
				       t_osc_expr_ast_tobndlfn tobndlfn,
				       t_osc_expr_ast_frombndlfn frombndlfn,
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
					  evallvalfn,
					  formatfn,
					  format_lispfn,
					  freefn,
					  copyfn,
					  tobndlfn,
					  frombndlfn,
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
					  osc_expr_ast_funcall_evalLvalInLexEnv, 
					  osc_expr_ast_funcall_format, 
					  osc_expr_ast_funcall_formatLisp, 
					  osc_expr_ast_funcall_free, 
					  osc_expr_ast_funcall_copy, 
					  osc_expr_ast_funcall_toBndl, 
					  osc_expr_ast_funcall_fromBndl, 
					  sizeof(t_osc_expr_ast_funcall),
					  rec,
					  argv);
	}
	// if args are missing, wrap it in a lambda abstraction
	char *varnames[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"};
	int arity = osc_expr_funcrec_getInputArity(rec);
	if(e->argc < arity){
		t_osc_expr_ast_value *lambdalist = NULL;
		for(int i = 0; i < arity - e->argc; i++){
			t_osc_atom_u *a = osc_atom_u_alloc();
			osc_atom_u_setString(a, varnames[i]);
			t_osc_expr_ast_value *v = osc_expr_ast_value_allocIdentifier(a);
			osc_expr_ast_expr_append(e->argv, (t_osc_expr_ast_expr *)v);

			t_osc_atom_u *a2 = osc_atom_u_alloc();
			osc_atom_u_setString(a2, varnames[i]);
			t_osc_expr_ast_value *v2 = osc_expr_ast_value_allocIdentifier(a2);
			if(lambdalist == NULL){
				lambdalist = v2;
			}else{
				osc_expr_ast_expr_append((t_osc_expr_ast_expr *)lambdalist, (t_osc_expr_ast_expr *)v2);
			}
			e->argc++;
		}
		e = (t_osc_expr_ast_funcall *)osc_expr_ast_function_alloc(lambdalist, (t_osc_expr_ast_expr *)e);
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
			t_osc_expr_ast_expr *e = va_arg(ap, t_osc_expr_ast_expr*);
			osc_expr_ast_expr_append(argv, e);
		}
		t_osc_expr_ast_expr *ee = argv;
		while(ee){
			ee = osc_expr_ast_expr_next(ee);
		}
		return osc_expr_ast_funcall_allocWithList(rec, argv);
	}else{
		return osc_expr_ast_funcall_allocWithList(rec, NULL);
	}
}
