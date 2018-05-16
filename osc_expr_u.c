/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011, The Regents of
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

/** 	\file osc_expr.c
	\author John MacCallum

*/

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <inttypes.h>

#include <string.h>
#include <math.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_bundle_s.h"
#include "osc_bundle_iterator_u.h"
#include "osc_message_s.h"
#include "osc_message_u.h"
#include "osc_message_iterator_u.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_hashtab.h"
#include "osc_util.h"
#include "osc_rset.h"
#include "osc_query.h"
#include "osc_typetag.h"
#include "osc_match.h"


#include "osc_expr_u.h"

#include "osc_expr.r"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_expr_func.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_expr_privatedecls.h"
#include "osc_expr_u_privatedecls.h"
//#define __OSC_PROFILE__
#include "osc_profile.h"

static void osc_expr_err_badInfixArg(void *context, char *func, char typetag, int argnum, t_osc_atom_u *left, t_osc_atom_u *right)
{
    long len = osc_atom_u_nformat(NULL, 0, left, 0);
    char leftstr[len + 1];
    osc_atom_u_nformat(leftstr, len + 1, left, 0);
    len = osc_atom_u_nformat(NULL, 0, right, 0);
    char rightstr[len + 1];
    osc_atom_u_nformat(rightstr, len + 1, right, 0);
    osc_error(context, OSC_ERR_EXPR_ARGCHK, "bad argument for expression %s %s %s. arg %d is a %s", leftstr, func, rightstr, argnum, osc_typetag_str(typetag));
}

static void osc_expr_err_unbound(void *context, char *address, char *func)
{
    osc_error(context, OSC_ERR_EXPR_ADDRESSUNBOUND, "%s: address %s is unbound", func, address);
}

static void osc_expr_err_argnum(void *context, unsigned int expected, unsigned int found, unsigned int optional_args_allowed, char *func)
{
    if(expected == found){
        // come on
        return;
    }
    char *errstr = NULL;
    if(optional_args_allowed){
        if(expected < found){
            errstr = "%s: expected %d arguments but found %d\n";
        }else{
            errstr = "%s: expected at least %d arguments but found only %d\n";
        }
    }else{
        errstr = "%s: expected %d arguments but found %d\n";
    }
    osc_error(context, OSC_ERR_EXPR_ARGCHK, errstr, func, expected, found);
}

t_osc_err osc_expr_u_lex(char *str, t_osc_atom_array_u **ar);

t_osc_hashtab *osc_expr_u_funcobj_ht;
void osc_expr_u_funcobj_dtor(char *key, void *val)
{
    if(key){
        osc_mem_free(key);
    }
    if(val){
        osc_expr_free((t_osc_expr *)val);
    }
}

int osc_expr_u_eval(t_osc_expr *f, t_osc_bndl_u *u_bndl, t_osc_atom_ar_u **out, void *context)
{
	return osc_expr_u_evalInLexEnv(f, NULL, u_bndl, out, context);
}

int osc_expr_u_evalInLexEnv(t_osc_expr *f,
			  t_osc_expr_lexenv *lexenv,
			  t_osc_bndl_u *u_bndl,
			  t_osc_atom_ar_u **out,
				void *context)
{
	// printf("%s context %p\n",__func__, context );
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	if(f->rec->func == osc_expr_apply){
		return osc_expr_u_specFunc_apply(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_map){
		return osc_expr_u_specFunc_map(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_lreduce || f->rec->func == osc_expr_rreduce){
		return osc_expr_u_specFunc_reduce(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_assign){
		return osc_expr_u_specFunc_assign(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_assign_to_index){
		return osc_expr_u_specFunc_assigntoindex(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_if){
		return osc_expr_u_specFunc_if(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_emptybundle){
		return osc_expr_u_specFunc_emptybundle(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_bound){
		return osc_expr_u_specFunc_bound(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_exists){
		return osc_expr_u_specFunc_exists(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_getaddresses){
		return osc_expr_u_specFunc_getaddresses(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_delete){
		return osc_expr_u_specFunc_delete(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_getmsgcount){
		return osc_expr_u_specFunc_getmsgcount(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_value){
		return osc_expr_u_specFunc_value(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_quote){
		return osc_expr_u_specFunc_quote(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_eval_call){
		return osc_expr_u_specFunc_eval(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_tokenize){
		return osc_expr_u_specFunc_tokenize(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_compile){
		return osc_expr_u_specFunc_compile(f, lexenv, u_bndl, out, context);
/*  }else if(f->rec->func == osc_expr_gettimetag){
		return osc_expr_u_specFunc_gettimetag(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_settimetag){
		return osc_expr_u_specFunc_settimetag(f, lexenv, u_bndl, out, context); */
	}else if(f->rec->func == osc_expr_getbundlemember){
		return osc_expr_u_specFunc_getBundleMember(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_assigntobundlemember){
		return osc_expr_u_specFunc_assignToBundleMember(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_andalso){
		return osc_expr_u_specFunc_andalso(f, lexenv, u_bndl, out, context);
	}else if(f->rec->func == osc_expr_orelse){
		return osc_expr_u_specFunc_orelse(f, lexenv, u_bndl, out, context);
	}else{
		//////////////////////////////////////////////////
		// Call normal function
		//////////////////////////////////////////////////
        // printf("function call \n" );
		long f_argc = osc_expr_getArgCount(f);
		t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
		t_osc_atom_ar_u *argv[f_argc];
		memset(argv, '\0', sizeof(argv));
		int ret = 0;
		int i = 0;
		while(f_argv){
			int ret = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, argv + i, context);
			if(ret){
				if(ret == OSC_ERR_EXPR_ADDRESSUNBOUND){
					// if the type arg type is something else, it will be an expression which means an
					// error has already been posted
					if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
						osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), osc_expr_rec_getName(osc_expr_getRec(f)));
					}
				}
				int j;
				for(j = 0; j < i + 1; j++){
					if(argv[j]){
						osc_atom_array_u_free(argv[j]);
					}
				}
				return ret;
			}
			f_argv = f_argv->next;
			i++;
		}
        
        ret = f->rec->func(f, f_argc, argv, out, context);
        
		for(i = 0; i < f_argc; i++){
			if(argv[i]){
				osc_atom_array_u_free(argv[i]);
			}
		}
		return ret;
	}
	return 1;
}
t_osc_err osc_expr_u_evalArgInLexEnv(t_osc_expr_arg *arg,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void *context)
{
	if(!arg){
		return 1;
	}
	switch(arg->type){
	case OSC_EXPR_ARG_TYPE_ATOM:
		{
			if(lexenv && osc_atom_u_getTypetag(arg->arg.atom) == 's'){
				t_osc_atom_ar_u *tmp = NULL;
				if((tmp = osc_expr_lookupBindingInLexenv(lexenv, osc_atom_u_getStringPtr(arg->arg.atom)))){
					*out = osc_atom_array_u_copy(tmp);
					return 0;
				}
			}
			*out = osc_atom_array_u_alloc(1);

			t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
			osc_atom_u_copyInto(&a, arg->arg.atom);
		}
		return 0;
	case OSC_EXPR_ARG_TYPE_LIST:
		{
			*out = osc_atom_array_u_copy(arg->arg.list);
			return 0;
		}
	case OSC_EXPR_ARG_TYPE_EXPR:
		{
			t_osc_err e = osc_expr_u_evalInLexEnv(arg->arg.expr, lexenv, u_bndl, out, context);
			return e;
		}
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		{
			*out = NULL;
			if(!u_bndl){
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
			
            t_osc_msg_u* msg = osc_bundle_u_getFirstFullMatch(u_bndl, arg->arg.osc_address);
			if(msg){
				*out = osc_message_u_getArgArrayCopy(msg);
				return 0;
			}
			return OSC_ERR_EXPR_ADDRESSUNBOUND;
		}
    case OSC_EXPR_ARG_TYPE_FUNCTION:
		*out = osc_atom_array_u_alloc(1);
		osc_atom_u_setRec(osc_atom_array_u_get(*out, 0), (void *)osc_expr_arg_getFunction(arg));
		return 0;
}
	return OSC_ERR_INVAL; // this really shouldn't happen unless there's a bug somewhere
}

//////////////////////////////////////////////////
// Special functions
//////////////////////////////////////////////////

static int osc_expr_u_specFunc_apply(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	if((osc_expr_arg_getType(f_argv) != OSC_EXPR_ARG_TYPE_ATOM) &&
	   (osc_expr_arg_getType(f_argv) != OSC_EXPR_ARG_TYPE_FUNCTION) &&
	   (osc_expr_arg_getType(f_argv) != OSC_EXPR_ARG_TYPE_EXPR) &&
	   (osc_expr_arg_getType(f_argv) != OSC_EXPR_ARG_TYPE_OSCADDRESS)){
		osc_error_handler(context,
					basename(__FILE__),
				  __func__,
				  __LINE__,
				  OSC_ERR_EXPPARSE,
				  "the first argument to apply() should be a function, the name of a function, or an OSC address");
		return 1;
	}
	if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_FUNCTION){
		t_osc_expr_rec *r = osc_expr_arg_getFunction(f_argv);
		if(!r){
			osc_error_handler(context,
						basename(__FILE__),
					  __func__,
					  __LINE__,
					  OSC_ERR_EXPPARSE,
					  "Error parsing the first argument to apply()");
			return 1;
		}
		// arity check
		t_osc_expr_lexenv *lexenv_copy = NULL;
		if(lexenv){
			osc_expr_copyLexenv(&lexenv_copy, lexenv);
		}else{
			lexenv_copy = osc_expr_makeLexenv();
		}
		int nparams = osc_expr_rec_getNumRequiredArgs(r);
		char **params = osc_expr_rec_getRequiredArgsNames(r);
		t_osc_expr_arg *arg = f_argv->next;
		for(int i = 0; (i < nparams) && arg; i++){
			t_osc_atom_ar_u *atoms = NULL;
			t_osc_err e = osc_expr_u_evalArgInLexEnv(arg, lexenv, u_bndl, &atoms, context);
			if(e == OSC_ERR_EXPR_ADDRESSUNBOUND){
				osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(arg), "apply");
				return e;
			}
			osc_expr_bindVarInLexenv(lexenv_copy, params[i], atoms);
			arg = arg->next;
		}
		t_osc_expr *e = (t_osc_expr *)osc_expr_rec_getExtra(r);
		while(e){
			int ret =  osc_expr_u_evalInLexEnv(e, lexenv_copy, u_bndl, out, context);
			if(e->next){
				osc_atom_array_u_free(*out);
				*out = NULL;
			}
			if(ret){
				osc_expr_destroyLexenv(lexenv_copy);
				return ret;
			}
			e = osc_expr_next(e);
		}
		osc_expr_destroyLexenv(lexenv_copy);
	}else if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_ATOM ||
		 osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_EXPR ||
		 osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
		t_osc_expr_rec *r = NULL;
		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_ATOM){
			if(osc_atom_u_getTypetag(osc_expr_arg_getOSCAtom(f_argv)) != 's'){
				//error
				return 1;
			}
			/*
			t_osc_atom_ar_u *a = NULL;
			osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &a);
			if(a){
				t_osc_atom_u *aa = osc_atom_array_u_get(a, 0);
			}
			*/
			r = osc_expr_lookupFunction(osc_atom_u_getStringPtr(osc_expr_arg_getOSCAtom(f_argv)));
			if(!r){
				// maybe we're in a lambda and should look it up in the lexenv
				t_osc_atom_ar_u *xx = osc_expr_lookupBindingInLexenv(lexenv, osc_atom_u_getStringPtr(osc_expr_arg_getOSCAtom(f_argv)));
				if(osc_atom_u_getTypetag(osc_atom_array_u_get(xx, 0)) == 'r'){
					r = osc_atom_u_getRec(osc_atom_array_u_get(xx, 0));
					t_osc_expr_arg *a = osc_expr_arg_alloc();
					osc_expr_arg_setFunction(a, r);
					osc_expr_arg_setNext(a, f_argv->next);
					osc_expr_setArg(f, a);
					int ret = osc_expr_u_specFunc_apply(f, lexenv, u_bndl, out, context);
					//osc_expr_setArg(f, f_argv);
					//osc_expr_arg_setNext(a, NULL);
					//osc_expr_arg_free(a);
					return ret;
				}
				//osc_atom_array_u_free(xx);
			}
		}else{
			t_osc_atom_ar_u *ar = NULL;
			t_osc_err e = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &ar, context);
			if(e == OSC_ERR_EXPR_ADDRESSUNBOUND){
				osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "apply");
				return e;
			}
			if(!ar){
				//error
				return 1;
			}
			if(osc_atom_array_u_getLen(ar) != 1){
				//error
				return 1;
			}
			if(osc_atom_u_getTypetag(osc_atom_array_u_get(ar, 0)) != 's'){
				//error
				return 1;
			}
			char *stp = osc_atom_u_getStringPtr(osc_atom_array_u_get(ar, 0));
			r = osc_expr_lookupFunction(stp);
			if(!r){
				// lookup didn't return a valid function, so let's see
				// if we can parse this string.
				t_osc_err err = osc_expr_parser_parseFunction(stp, &r, context);
				if(!err && r){
					t_osc_expr *e = NULL;
					osc_expr_copy(&e, f);
					t_osc_expr_arg *arg1 = osc_expr_getArgs(e);
					t_osc_expr_arg *arg2 = osc_expr_arg_next(arg1);
					switch(osc_expr_arg_getType(arg1)){
					case OSC_EXPR_ARG_TYPE_ATOM:
						osc_atom_u_free(osc_expr_arg_getOSCAtom(arg1));
						break;
					case OSC_EXPR_ARG_TYPE_EXPR:
						osc_expr_free(osc_expr_arg_getExpr(arg1));
						break;
					case OSC_EXPR_ARG_TYPE_OSCADDRESS:
						osc_mem_free(osc_expr_arg_getOSCAddress(arg1));
						break;
					}
					osc_expr_arg_setFunction(arg1, r);
					osc_expr_arg_setNext(arg1, arg2);
					e->argv = arg1;
					osc_expr_arg_setFunction(osc_expr_getArgs(e), r);

					int ret = osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, out, context);
					osc_expr_free(e);
					osc_atom_array_u_free(ar);
					return ret;
				}
			}
			osc_atom_array_u_free(ar);
		}
		if(!r){
			osc_error_handler(context,
						basename(__FILE__),
					  __func__,
					  __LINE__,
					  OSC_ERR_EXPPARSE,
					  "unrecognized function %s",
					  osc_atom_u_getStringPtr(osc_expr_arg_getOSCAtom(f_argv)));
			return 1;
		}
		t_osc_expr *e = osc_expr_alloc();
		osc_expr_setRec(e, r);
		osc_expr_setArg(e, f_argv->next);
		int ret = osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, out, context);
		e->argc = 0;
		e->argv = NULL;
		osc_expr_free(e);
		return ret;
	}else{
		//error
		return 1;
	}
	return 0;
}

static int osc_expr_u_specFunc_map(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	int f_argc = osc_expr_getArgCount(f);
	if(f_argc < 2){
		// error
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_expr_rec *r = osc_expr_lookupFunction("apply");
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);

	int ac = f_argc - 1;
	t_osc_atom_ar_u *args[ac];
	memset(args, '\0', ac * sizeof(t_osc_atom_ar_u *));
	uint32_t min = ~0;
	int i = 0;
	t_osc_expr_arg *a = f_argv->next;
	while(a){
		t_osc_err err = osc_expr_u_evalArgInLexEnv(a, lexenv, u_bndl, args + i, context);
		if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
			osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(a), "map");
			// assume whoever generated this will post an error
			for(int j = 0; j < i; j++){
				if(args[j]){
					osc_atom_array_u_free(args[j]);
				}
			}
			e->argv = NULL;
			e->argc = 0;
			osc_expr_free(e);
			return err;
		}
		int count = osc_atom_array_u_getLen(args[i]);
		if(count < min){
			min = count;
		}
		i++;
		a = a->next;
	}
	// arity check needs to go here
	t_osc_expr_arg *func_args[ac + 1];
	func_args[0] = NULL;
	osc_expr_arg_copy(func_args, f_argv);
	osc_expr_appendArg(e, *func_args);
	for(i = 1; i < ac + 1; i++){
		func_args[i] = osc_expr_arg_alloc();
		osc_expr_appendArg(e, func_args[i]);
	}

	t_osc_atom_ar_u *output[min];
	memset(output, '\0', min * sizeof(t_osc_atom_ar_u *));
	int outcount = 0;
	for(i = 0; i < min; i++){
		int j;
		for(j = 0; j < ac; j++){
			osc_expr_arg_setOSCAtom(func_args[j + 1], osc_atom_array_u_get(args[j], i));
		}
		osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, output + i, context);
		outcount += osc_atom_array_u_getLen(output[i]);
	}
	*out = osc_atom_array_u_alloc(outcount);
	int pos = 0;
	for(i = 0; i < min; i++){
		osc_atom_array_u_copyInto(out, output[i], pos);
		pos += osc_atom_array_u_getLen(output[i]);
	}
	e->argv = NULL;
	e->argc = 0;
	osc_expr_free(e);
	e = NULL;
	if(func_args[0]){
		osc_expr_arg_free(func_args[0]);
	}
	for(i = 0; i < ac; i++){
		if(args[i]){
			osc_atom_array_u_free(args[i]);
		}
		if(func_args[i + 1]){
			// it looks like a bug to free a t_osc_expr_arg with osc_mem_free,
			// but we don't want to free the underlying t_osc_atom_u because
			// it will be freed when args is freed.
			osc_mem_free(func_args[i + 1]);
		}
	}
	for(i = 0; i < min; i++){
		if(output[i]){
			osc_atom_array_u_free(output[i]);
		}
	}
	return 0;
}

static int osc_expr_u_specFunc_reduce(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	int f_argc = osc_expr_getArgCount(f);
	if(f_argc < 2){
		// error
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_expr_rec *r = osc_expr_lookupFunction("apply");
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);

	int ac = 3;
	t_osc_expr_arg *func_args[ac];  // function, arg1, arg2
	func_args[0] = NULL;
	osc_expr_arg_copy(func_args, f_argv);
	osc_expr_appendArg(e, *func_args);
	for(int i = 1; i < ac; i++){
		func_args[i] = osc_expr_arg_alloc();
		osc_expr_appendArg(e, func_args[i]);
	}

	t_osc_atom_ar_u *args = NULL;
	t_osc_expr_arg *a = f_argv->next;
	int err = osc_expr_u_evalArgInLexEnv(a, lexenv, u_bndl, &args, context);
	if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(a),
				     f->rec->func == osc_expr_lreduce ? "lreduce" : "rreduce");
		if(args){
			osc_atom_array_u_free(args);
		}
		for(int i = 0; i < ac; i++){
			if(func_args[i]){
				osc_expr_arg_free(func_args[i]);
			}
		}
		e->argv = NULL;
		e->argc = 0;
		osc_expr_free(e);
		return err;
	}
	int count = osc_atom_array_u_getLen(args);

	int arg0 = 1;
	int arg1 = 2;
	int delta = 1;
	int start = 0;
	if(f->rec->func == osc_expr_rreduce){
		arg0 = 2;
		arg1 = 1;
		delta = -1;
		start = count - 1;
	}

	if(count == 0){
		osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, out, context);
		if(args){
			osc_atom_array_u_free(args);
		}
	}else if(count == 1 && f_argc == 2){
		*out = args;
	}else{
		t_osc_atom_ar_u *output = NULL;
		if(f_argc == 3){
			t_osc_atom_ar_u *sc = NULL;
			t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv->next->next, lexenv, u_bndl, &sc, context);
			if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
				osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(a),
						     f->rec->func == osc_expr_lreduce ? "lreduce" : "rreduce");
				if(args){
					osc_atom_array_u_free(args);
				}
				for(int i = 0; i < ac; i++){
					if(func_args[i]){
						osc_expr_arg_free(func_args[i]);
					}
				}
				e->argv = NULL;
				e->argc = 0;
				osc_expr_free(e);
				return err;
			}
			if(sc){
				t_osc_atom_u *acopy = NULL;
				osc_atom_u_copyInto(&acopy, osc_atom_array_u_get(sc, 0));
				osc_expr_arg_setOSCAtom(func_args[arg0], acopy);
				if(sc){
					osc_atom_array_u_free(sc);
				}
			}else{
				//error
				return 1;
			}
			osc_expr_arg_setOSCAtom(func_args[arg1], osc_atom_array_u_get(args, start));
			start += delta;
		}else{
			osc_expr_arg_setOSCAtom(func_args[arg0], osc_atom_array_u_get(args, start));
			start += delta;
			osc_expr_arg_setOSCAtom(func_args[arg1], osc_atom_array_u_get(args, start));
			start += delta;
		}
		osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, &output, context);

		for(int i = start; delta > 0 ? i < count : i >= 0; i += delta){
			t_osc_atom_u *copy1 = NULL;
			t_osc_atom_u *copy2 = NULL;
			osc_atom_u_copyInto(&copy1, osc_atom_array_u_get(output, 0));
			osc_atom_u_copyInto(&copy2, osc_atom_array_u_get(args, i));
			osc_expr_arg_setOSCAtom(func_args[arg0], copy1);
			osc_expr_arg_setOSCAtom(func_args[arg1], copy2);
			osc_atom_array_u_free(output);
			output = NULL;
			osc_expr_u_evalInLexEnv(e, lexenv, u_bndl, &output, context);
			osc_expr_arg_clear(func_args[arg0]);
			osc_expr_arg_clear(func_args[arg1]);
		}
		*out = output;
		if(args){
			osc_atom_array_u_free(args);
		}
	}

	e->argv = NULL;
	e->argc = 0;
	osc_expr_free(e);

	if(func_args[0]){
		osc_expr_arg_free(func_args[0]);
	}
	for(int i = 1; i < ac; i++){
		if(func_args[i]){
			// it looks like a bug to free a t_osc_expr_arg with osc_mem_free,
			// but we don't want to free the underlying t_osc_atom_u because
			// it will be freed when args is freed.
			osc_mem_free(func_args[i]);
		}
	}
	return 0;
}

static int osc_expr_u_specFunc_assign(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!u_bndl){
		return 1;
	}
  //  printf("%s context %p\n",__func__, context );
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *address_ar = NULL;
	char *address = NULL;
	if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
		osc_util_strdup(&address, f_argv->arg.osc_address);
	}else{
		/*
		  assign() treats an address in its first argument slot differently in that it doesn't replace
		  it with its value---the address is the target of assignment.  I.e., assign(/foo, 10) assigns
		  10 to /foo, not 10 to whatever /foo is bound to which is why assign() is a special function.

		  This means that the use of the value() function in the first slot of assign() function has
		  to be treated differently as well.  Normally, value() would look up the value of the thing
		  bound to the address, i.e.,

		  /foo "/bar"
		  /bar 10

		  value(/foo) => 10

		  since /foo would be substituted for /bar before value() gets a hold of it.  Here, we would want
		  one fewer level of resolution, so assign(value(/foo), ...) should assign something to /bar.
		*/
		t_osc_err err = OSC_ERR_NONE;
		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_EXPR){
			t_osc_expr *e = osc_expr_arg_getExpr(f_argv);
			t_osc_expr_rec *r = osc_expr_getRec(e);
			if(r){
				if(!strcmp(osc_expr_rec_getName(r), "value")){
					t_osc_expr_arg *value_args = osc_expr_getArgs(e);
					if(value_args && osc_expr_arg_getType(value_args) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
						err = osc_expr_u_evalArgInLexEnv(value_args, lexenv, u_bndl, &address_ar, context);
						// don't report error--we'll try again below
					}
				}
			}
		}
		if(!address_ar){
			err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &address_ar, context);
		}
		if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
			osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "=");
			return err;
		}
		if(!address_ar){
			return 1;
		}
		t_osc_atom_u *address_atom = osc_atom_array_u_get(address_ar, 0);
		if(osc_atom_u_getTypetag(address_atom) != 's'){
			osc_atom_array_u_free(address_ar);
			osc_error(context, OSC_ERR_EXPR_ARGCHK, "the first argument to assign() should resolve to an OSC address");
			return 1;
		}
		osc_atom_u_getString(address_atom, 0, &address);
		osc_atom_array_u_free(address_ar);
	}
    
    t_osc_err err;
	if((err = osc_error_validateAddress(address))){
		return err;
	}

	t_osc_msg_u *mm = osc_message_u_alloc();
	osc_message_u_setAddress(mm, address);
	t_osc_err ret = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, out, context);

	if(ret){
        if(ret == OSC_ERR_EXPR_ADDRESSUNBOUND)
        {
            int argtype = osc_expr_arg_getType(f_argv->next);
            if( argtype == OSC_EXPR_ARG_TYPE_OSCADDRESS )
            {
                osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next), "=");
            }
            /*
             // we can assume that the inner expression printed an error
            else if ( argtype == OSC_EXPR_ARG_TYPE_EXPR )
            {
                osc_error(context, OSC_ERR_EXPR_EVAL, "%s = %s : assignment from expression result is unbound", osc_expr_arg_getOSCAddress(f_argv), osc_expr_arg_getExpr(f_argv->next)->rec->name );
            }*/
        }
        else
            osc_error(context, OSC_ERR_EXPR_EVAL, NULL);
        
		if(address){
			osc_mem_free(address);
		}
		osc_message_u_free(mm);
		return ret;
	}
	for(int i = 0; i < osc_atom_array_u_getLen(*out); i++){
		t_osc_atom_u *cpy = NULL;
		osc_atom_u_copyInto(&cpy, osc_atom_array_u_get(*out, i));
		osc_message_u_appendAtom(mm, cpy);
	}

    ret = osc_bundle_u_replaceMessage( u_bndl, mm );
    
    if(address){
        osc_mem_free(address);
    }

    // message free is handled in replace message
    // osc_message_u_free(mm);
	return ret;
}

static int osc_expr_u_specFunc_assigntoindex(t_osc_expr *f,
                                           t_osc_expr_lexenv *lexenv,
                                           t_osc_bndl_u *u_bndl,
                                           t_osc_atom_ar_u **out,
                                            void *context)
{
	if(!u_bndl){
		return 1;
	}
	int f_argc = osc_expr_getArgCount(f);
	if(f_argc != 3){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_msg_ar_u *msg_ar = osc_bundle_u_lookupAddress(u_bndl, f_argv->arg.osc_address, 1);
	if(!msg_ar){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "assign to index =");
		return 1;
	}

	t_osc_atom_ar_u *indexes = NULL;
	t_osc_atom_ar_u *data = NULL;
    t_osc_err err = 0;
    
    // get data at target address
    err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, out, context);
    if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
        osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "assign to index");
        goto bail;
    }
    int outlen = osc_atom_array_u_getLen(*out);

    err = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, &indexes, context);
    if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
        osc_error(context, OSC_ERR_EXPR_ADDRESSUNBOUND, "%s assign to index : address %s is unbound", osc_expr_arg_getOSCAddress(f_argv), osc_expr_arg_getOSCAddress(f_argv->next));
        // osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next), "assign to index 2");
        goto bail;
    }
    
    // get index(es)
    int nindexes = osc_atom_array_u_getLen(indexes);
    // get data
    err = osc_expr_u_evalArgInLexEnv(f_argv->next->next, lexenv, u_bndl, &data, context);
    if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
        //osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next->next), "assign to index 3");
        osc_error(context, OSC_ERR_EXPR_ADDRESSUNBOUND, "%s assign to index : address %s is unbound", osc_expr_arg_getOSCAddress(f_argv), osc_expr_arg_getOSCAddress(f_argv->next->next));

        goto bail;
    }
    
    int ndata = osc_atom_array_u_getLen(data);
    if(nindexes == 0 || ndata == 0)
    {
        err = 1;
        goto bail;
    }
    else if(nindexes == 1 && ndata > 1) // simple case, maybe try to optimize this
    {
        int idx = osc_atom_u_getInt(osc_atom_array_u_get(indexes, 0));
        if(idx >= outlen || idx < 0)
        {
            osc_error(context, OSC_ERR_EXPR_EVAL, "%s : assign to index %d exceeds array length %d", osc_expr_arg_getOSCAddress(f_argv), idx, outlen);
            err = 1;
            goto bail;
        }
        
        t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
        osc_atom_u_copyInto( &dest, osc_atom_array_u_get(data, 0) );
    }
    else if(nindexes > 1 && ndata == 1)
    {
        int i, idx;
        t_osc_atom_u *a = osc_atom_array_u_get(data, 0);
        for(i = 0; i < nindexes; i++)
        {
            idx = osc_atom_u_getInt(osc_atom_array_u_get(indexes, i));
            if(idx >= outlen || idx < 0)
            {
                osc_error(context, OSC_ERR_EXPR_EVAL, "%s : assign to index %d exceeds array length %d", osc_expr_arg_getOSCAddress(f_argv), idx, outlen);
                continue;
            }
            
            t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
            osc_atom_u_copyInto(&dest, a);
        }
    }
    else // nindexes > 1 && ndata > 1
    {
        int i, idx;
        int n = osc_atom_array_u_getLen(indexes);
        if(osc_atom_array_u_getLen(data) < n){
            n = osc_atom_array_u_getLen(data);
        }
        for(i = 0; i < n; i++){
            idx = osc_atom_u_getInt(osc_atom_array_u_get(indexes, i));
            if(idx >= outlen || idx < 0){
                osc_error(context, OSC_ERR_EXPR_EVAL, "%s : assign to index %d exceeds array length %d", osc_expr_arg_getOSCAddress(f_argv), idx, outlen);
                continue;
            }
            t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
            osc_atom_u_copyInto(&dest, osc_atom_array_u_get(data, i));
        }
    }
    
    // out now contains the atom array, but we need to copy it into the message
    t_osc_msg_u *mm = osc_message_u_allocWithAddress( f_argv->arg.osc_address );
    int i;
    for(i = 0; i < osc_atom_array_u_getLen(*out); i++){
        t_osc_atom_u *cpy = NULL;
        osc_atom_u_copyInto(&cpy, osc_atom_array_u_get(*out, i));
        osc_message_u_appendAtom(mm, cpy);
    }

    osc_bundle_u_replaceMessage(u_bndl, mm);
    
    if (mm)
        osc_message_u_free(mm);
    
    err = 0;
   
bail:
    if (indexes)
        osc_atom_array_u_free(indexes);
    if (data)
        osc_atom_array_u_free(data);


    return err;
}

static int osc_expr_u_specFunc_if(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	int f_argc = osc_expr_getArgCount(f);
    // printf("n args %d\n", f_argc);
	if(f_argc < 2 || f_argc > 3){
		osc_expr_err_argnum(context, 2, f_argc, 1, "osc_expr: if()");
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *argv = NULL;
	t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &argv, context);
	if(err){
        if(err == OSC_ERR_EXPR_ADDRESSUNBOUND)
        {
            int argtype = osc_expr_arg_getType(f_argv);
            if( argtype == OSC_EXPR_ARG_TYPE_OSCADDRESS )
            {
                osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "if(): error evaluating test argument");
            }
            else
            {
                // we can assume that the inner expression printed an error
                // printf("argytpe %i\n", argtype);
            }
        }
        else
        {
            osc_error(context, OSC_ERR_EXPR_EVAL, "osc_expr if(): error evaluating test argument" );
        }
        
        //printf("err %llu %i %i\n", err, OSC_ERR_EXPR_ADDRESSUNBOUND, OSC_ERR_NOBUNDLE );
		if(argv){
			osc_atom_array_u_free(argv);
		}
		return err;
	}

	int test_result_len = osc_atom_array_u_getLen(argv);
	t_osc_atom_ar_u *boolvec[test_result_len];
	memset(boolvec, '\0', test_result_len * sizeof(t_osc_atom_ar_u *));
	int outlen = 0;
	int j = 0;
	//for(int j = 0; j < osc_atom_array_u_getLen(argv); j++){
		if(osc_atom_u_getInt32(osc_atom_array_u_get(argv, j))){
			err = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, boolvec + j, context);
			if(err){
                if(err == OSC_ERR_EXPR_ADDRESSUNBOUND)
                {
                    int argtype = osc_expr_arg_getType(f_argv->next);
                    if( argtype == OSC_EXPR_ARG_TYPE_OSCADDRESS )
                    {
                        osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next), "if(): error evaluating \"then\" argument");
                    }
                    else
                    {
                        // we can assume that the inner expression printed an error
                     //  printf("%s argytpe %i\n", __func__, argtype);
                    }
                }
                else
                {
                    osc_error(context, OSC_ERR_EXPR_EVAL, "osc_expr if(): error evaluating \"then\" argument" );
                }
                goto out;
			}
			outlen += osc_atom_array_u_getLen(boolvec[j]);
		}else{
			if(f_argc > 2){
				err = osc_expr_u_evalArgInLexEnv(f_argv->next->next, lexenv, u_bndl, boolvec + j, context);
				if(err){
                    if(err == OSC_ERR_EXPR_ADDRESSUNBOUND)
                    {
                        int argtype = osc_expr_arg_getType(f_argv->next->next);
                        if( argtype == OSC_EXPR_ARG_TYPE_OSCADDRESS )
                        {
                            osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next->next), "if(): error evaluating \"else\" argument");
                        }
                        else
                        {
                            // we can assume that the inner expression printed an error
                            // printf("argytpe %i\n", argtype);
                        }
                    }
                    else
                    {
                        osc_error(context, OSC_ERR_EXPR_EVAL, "osc_expr if(): error evaluating \"else\" argument" );
                    }
                    goto out;
				}
				outlen += osc_atom_array_u_getLen(boolvec[j]);
			}
		}
		//}
	*out = osc_atom_array_u_alloc(outlen);
	for(int i = 0; i < test_result_len; i++){
		osc_atom_array_u_copyInto(out, boolvec[i], j);
		j += osc_atom_array_u_getLen(boolvec[i]);
	}
 out:
	for(int i = 0; i < test_result_len; i++){
		if(boolvec[i]){
			osc_atom_array_u_free(boolvec[i]);
		}
	}
	osc_atom_array_u_free(argv);
	return err;
}

static int osc_expr_u_specFunc_emptybundle(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!u_bndl){
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);

    if( osc_bundle_u_getMsgCount(u_bndl) == 0 ) {
		osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
	} else {
		osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
	}
	return 0;
}

static int osc_expr_u_specFunc_existsorbound(t_osc_expr *f,
					   t_osc_expr_lexenv *lexenv,
                       t_osc_bndl_u *u_bndl,
					   t_osc_atom_ar_u **out,
					   t_osc_err (*func)(t_osc_bndl_u *, char*, int, int*),
	 					 void *context)
{
	if( !u_bndl ){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	int type = osc_expr_arg_getType(f_argv);
	char *address = NULL;
	switch(type){
	case OSC_EXPR_ARG_TYPE_ATOM:
		{
			t_osc_atom_u *a = osc_expr_arg_getOSCAtom(f_argv);
			if(osc_atom_u_getTypetag(a) == 's'){
				t_osc_atom_ar_u *ar = NULL;
				osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &ar, context);
				if(ar){
					t_osc_atom_u *aa = osc_atom_array_u_get(ar, 0);
					if(osc_atom_u_getTypetag(aa) == 's'){
						osc_atom_u_getString(aa, 0, &address);
					}
					osc_atom_array_u_free(ar);
				}
			}
		}
		break;
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		{
			char *tmp = osc_expr_arg_getOSCAddress(f_argv);
			osc_util_strdup(&address, tmp);
		}
		break;
	case OSC_EXPR_ARG_TYPE_EXPR:
		{
			if(osc_expr_arg_getExpr(f_argv)->rec->func == osc_expr_getbundlemember){
				t_osc_atom_ar_u *ar = NULL;
				int err = osc_expr_u_evalInLexEnv(osc_expr_arg_getExpr(f_argv), lexenv, u_bndl, &ar, context);
				*out = osc_atom_array_u_alloc(1);
				if(err){
					osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
				}else if(!ar){
					if(func == osc_bundle_u_addressIsBound){
						osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
					}else{
						osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
					}
				}else{
					if(osc_atom_array_u_getLen(ar) == 0 && func == osc_bundle_u_addressIsBound){
						osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
					}else{
						osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
					}
				}
				osc_atom_array_u_free(ar);
				return 0;
			} else {
				t_osc_atom_ar_u *ar = NULL;
				int err = osc_expr_u_evalInLexEnv(osc_expr_arg_getExpr(f_argv), lexenv, u_bndl, &ar, context);
				if(!err && ar){
					if(osc_atom_array_u_getLen(ar) != 1){
						osc_atom_array_u_free(ar);
						goto err;
					}
					t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
					if(!a){
						osc_atom_array_u_free(ar);
						goto err;
					}
					if(osc_atom_u_getTypetag(a) != 's'){
						osc_atom_array_u_free(ar);
						goto err;
					}
					osc_atom_u_getString(a, 0, &address);
					if(!address){
						osc_atom_array_u_free(ar);
						goto err;
					}
					if(address[0] != '/'){
						osc_atom_array_u_free(ar);
						goto err;
					}
				}
			}
		}
		break;
	}
	if(address){
		*out = osc_atom_array_u_alloc(1);
		int res = 0;
		func(u_bndl, address, 1, &res);
		osc_atom_u_setBool(osc_atom_array_u_get(*out, 0), res);
		osc_mem_free(address);
		return 0;
	}
 err:
	osc_error(context, OSC_ERR_EXPR_ARGCHK, "arg 1 should be an OSC address");
	return 1;

}

static int osc_expr_u_specFunc_bound(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	return osc_expr_u_specFunc_existsorbound(f, lexenv, u_bndl, out, osc_bundle_u_addressIsBound, context);
}

static int osc_expr_u_specFunc_exists(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	return osc_expr_u_specFunc_existsorbound(f, lexenv, u_bndl, out, osc_bundle_u_addressExists, context);
}

static int osc_expr_u_specFunc_getaddresses(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!u_bndl){
		return 1;
	}
	int n = 0;
	if(osc_expr_getArgCount(f)){
		t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
		t_osc_atom_ar_u *ar = NULL;
		osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &ar, context);
		if(ar){
			t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
			if(osc_atom_u_getTypetag(a) != OSC_BUNDLE_TYPETAG){
				osc_atom_array_u_free(ar);
				osc_error(context, OSC_ERR_EXPR_ARGCHK, "argumnt to getaddresses() should be a bundle.");
				return 1;
			}
			t_osc_bndl_u *b = osc_atom_u_getBndl(a);
			t_osc_bndl_it_u *it = osc_bndl_it_u_get(b);
			while(osc_bndl_it_u_hasNext(it)){
				osc_bndl_it_u_next(it);
				n++;
			}
			*out = osc_atom_array_u_alloc(n);
			osc_bndl_it_u_reset(it);
			n = 0;
			while(osc_bndl_it_u_hasNext(it)){
				t_osc_msg_u *m = osc_bndl_it_u_next(it);
				osc_atom_u_setString(osc_atom_array_u_get(*out, n), osc_message_u_getAddress(m));
				n++;
			}
			osc_bndl_it_u_destroy(it);
			osc_atom_array_u_free(ar);
		}
	}else{
		t_osc_bndl_it_u *it = osc_bndl_it_u_get(u_bndl);
		while(osc_bndl_it_u_hasNext(it)){
			osc_bndl_it_u_next(it);
			n++;
		}
		*out = osc_atom_array_u_alloc(n);
		osc_bndl_it_u_reset(it);
		n = 0;
		while(osc_bndl_it_u_hasNext(it)){
			t_osc_msg_u *m = osc_bndl_it_u_next(it);
			osc_atom_u_setString(osc_atom_array_u_get(*out, n), osc_message_u_getAddress(m));
			n++;
		}
		osc_bndl_it_u_destroy(it);
	}
	return 0;
}

static int osc_expr_u_specFunc_delete(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!u_bndl){
		return 1;
	}
	int argc = osc_expr_getArgCount(f);
	if(!argc){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	while(f_argv){
		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
			osc_bundle_u_removeMessage(u_bndl, osc_expr_arg_getOSCAddress(f_argv) );
		}else if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_ATOM){
			t_osc_atom_u *a = osc_expr_arg_getOSCAtom(f_argv);
			if(osc_atom_u_getTypetag(a) == 's'){
				char *st = NULL;
				osc_atom_u_getString(a, 0, &st);
                osc_bundle_u_removeMessage(u_bndl, st );
			}
		}
		f_argv = f_argv->next;
	}
	return 0;
}

static int osc_expr_u_specFunc_getmsgcount(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!u_bndl){
		return 1;
	}
	int n = 0;
	t_osc_bndl_it_u *it = osc_bndl_it_u_get(u_bndl);
	while(osc_bndl_it_u_hasNext(it)){
		osc_bndl_it_u_next(it);
		n++;
	}
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), n);
	osc_bndl_it_u_destroy(it);
	return 0;
}

static int osc_expr_u_specFunc_value(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
 					 void *context)
{
	if(!u_bndl){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg, context);
	if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "value");
		return err;
	}
	if(arg){
		char *address = NULL;
		if(f_argv->type == OSC_EXPR_ARG_TYPE_OSCADDRESS){
			//*out = arg;
			address = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg, 0));
		}else{
			if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg, 0)) == 's'){
				address = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg, 0));
				/*
			}else{
				*out = arg;
				return 0;
				*/
			}
		}
		if(address)
        {
            t_osc_msg_u *m = osc_bundle_u_getFirstFullMatch(u_bndl, address);
			if(m)
            {
				int argc = osc_message_u_getArgCount(m);
				*out = osc_atom_array_u_alloc(argc);
                osc_array_clear(*out);

                int count = 0;
                t_osc_msg_it_u *it = osc_msg_it_u_get(m);
                while(osc_msg_it_u_hasNext(it)){
                    t_osc_atom_u *a = osc_atom_u_copy( osc_msg_it_u_next(it) );
                    osc_atom_array_u_set(*out, (void*)a, count++);
                }
                osc_msg_it_u_destroy(it);
			}
		}
		osc_atom_array_u_free(arg);
	}

	return 0;
}

static int osc_expr_u_specFunc_quote(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	switch(f_argv->type){
	case OSC_EXPR_ARG_TYPE_ATOM:
		{
			*out = osc_atom_array_u_alloc(1);

			t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
			osc_atom_u_copyInto(&a, f_argv->arg.atom);
		}
		return 0;
		// the parser always puts the arguments to a quote form in an OSC_EXPR_ARG_TYPE_ATOM
		// so the next two cases will never happen.
	case OSC_EXPR_ARG_TYPE_EXPR:{}
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:{}
	}
	return 0;
}

static int osc_expr_u_specFunc_eval(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg, context);
	if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "eval");
		return err;
	}
	if(arg){
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg, 0)) == 's' && osc_atom_array_u_getLen(arg) == 1){
			char *expr = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg, 0));
			t_osc_expr *f = NULL;
			OSC_PROFILE_TIMER_START(foo);
			osc_expr_parser_parseExpr(expr, &f, context);
			OSC_PROFILE_TIMER_STOP(foo);
			OSC_PROFILE_TIMER_PRINTF(foo);
			OSC_PROFILE_TIMER_SNPRINTF(foo, buff);
#ifdef __OSC_PROFILE__
			printf("%s\n", buff);
#endif
			int ret = 0;
			*out = osc_atom_array_u_alloc(1);

			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), 0);
			t_osc_expr *ff = f;
			while(ff){
				t_osc_atom_ar_u *ar = NULL;
				ret = osc_expr_u_eval(ff, u_bndl, &ar, context);
				if(ar){
					osc_atom_array_u_free(ar);
				}
				if(ret){
					osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), ret);
					break;
				}
				ff = osc_expr_next(ff);
			}
			osc_expr_free(f);
		}else{
			long buflen = 0;
			char *buf = NULL;
			osc_atom_array_u_getStringArray(arg, &buflen, &buf, " ");
			t_osc_expr *f = NULL;
			osc_expr_parser_parseExpr(buf, &f, context);
			int ret = 0;
			*out = osc_atom_array_u_alloc(1);

			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), 0);
			t_osc_expr *ff = f;
			while(ff){
				t_osc_atom_ar_u *ar = NULL;
				ret = osc_expr_u_eval(ff, u_bndl, &ar, context);
				if(ar){
					osc_atom_array_u_free(ar);
				}
				if(ret){
					osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), ret);
					break;
				}
				ff = osc_expr_next(ff);
			}
			osc_expr_free(f);
			if(buf){
				osc_mem_free(buf);
			}
		}
		osc_atom_array_u_free(arg);
	}else{
		char *a = osc_expr_arg_getOSCAddress(f_argv);
		if(a){
			t_osc_expr *e = osc_hashtab_lookup(osc_expr_u_funcobj_ht, strlen(a), a);
			if(e){
				t_osc_atom_ar_u *ar = NULL;
				int ret = osc_expr_u_eval(f, u_bndl, &ar, context);
				*out = osc_atom_array_u_alloc(1);

				osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), ret);
				if(ar){
					osc_atom_array_u_free(ar);
				}
			}
		}
	}
	return 0;
}


int osc_expr_u_specFunc_tokenize(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	//char *ptr = "/foo += sin(2 * pi() * /bar)";
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	char *expr = NULL;
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg, context);
	if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "tokenize");
		return err;
	}
	if(arg){
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg, 0)) == 's' && osc_atom_array_u_getLen(arg) == 1){
			expr = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg, 0));
		}else{
			long buflen = 0;
			osc_atom_array_u_getStringArray(arg, &buflen, &expr, " ");
		}
		if(expr){
			t_osc_err e = osc_expr_lex(expr, out);
			if(e){
				return e;
			}
		}
	}
	return 0;
}

static int osc_expr_u_specFunc_compile(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	if(osc_expr_arg_getType(f_argv) != OSC_EXPR_ARG_TYPE_OSCADDRESS){
		return 1;
	}
	char *key_orig = osc_expr_arg_getOSCAddress(f_argv);
	int keylen = strlen(key_orig);
	char *key = osc_mem_alloc(keylen + 1);
	strncpy(key, key_orig, keylen + 1);
	char *expression = NULL;
	t_osc_expr *thisf = NULL;
	int arg2type = osc_expr_arg_getType(f_argv->next);
	if(arg2type == OSC_EXPR_ARG_TYPE_OSCADDRESS){
		// 2nd arg is an OSC address--look it up in the bundle
		t_osc_atom_ar_u *arg = NULL;
		t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, &arg, context);
		if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
			osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "compile");
			osc_mem_free(key);
			return err;
		}
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg, 0)) == 's' &&
		   osc_atom_array_u_getLen(arg) == 1){
			osc_atom_u_getString(osc_atom_array_u_get(arg, 0), 0, &expression);
			osc_expr_parser_parseExpr(expression, &thisf, context);
		}else{
			long buflen = 0;
			osc_atom_array_u_getStringArray(arg, &buflen, &expression, " ");
			osc_expr_parser_parseExpr(expression, &thisf, context);
		}
		osc_atom_array_u_free(arg);
	}else if(arg2type == OSC_EXPR_ARG_TYPE_EXPR){
		// 2nd arg is an expr
		osc_expr_copy(&f, osc_expr_arg_getExpr(f_argv->next));
	}else{
		// free mem here
		return 1;
	}
	if(thisf){
		if(!osc_expr_u_funcobj_ht){
			osc_expr_u_funcobj_ht = osc_hashtab_new(-1, osc_expr_u_funcobj_dtor);
		}
		osc_hashtab_store(osc_expr_u_funcobj_ht, keylen, key, thisf);
	}
	if(expression){
		osc_mem_free(expression);
	}
	return 0;
}

/*
static int osc_expr_u_specFunc_gettimetag(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	*out = osc_atom_array_u_alloc(1);
	t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
    // pretty sure this isn't how to do this
    // do u_bndls have timetags in the header?
    t_osc_bndl_s *sbndl = osc_bundle_u_serialize(u_bndl);
	osc_atom_u_setTimetag(a, osc_bundle_s_getTimetag(osc_bundle_s_getLen(sbndl), osc_bundle_s_getPtr(sbndl)));
    
	return 0;
}

static int osc_expr_u_specFunc_settimetag(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    t_osc_bndl_u *u_bndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg, context);
	if(err){
		return 1;
	}
	if(arg){
		if(osc_atom_array_u_getLen(arg) > 0){
			t_osc_atom_u *a = osc_atom_array_u_get(arg, 0);
			if(a){
				t_osc_timetag t = osc_atom_u_getTimetag(a);
				osc_bundle_s_setTimetag(*len, *oscbndl, t);
				return 0;
			}
		}
	}
	return 1;
}
*/

static int osc_expr_u_specFunc_getBundleMember(t_osc_expr *f,
					     t_osc_expr_lexenv *lexenv,
					     t_osc_bndl_u *u_bndl,
					     t_osc_atom_ar_u **out,
		 					 void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg1 = NULL;
	osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg1, context);
	if(!arg1){
		return 1;
	}
    t_osc_bndl_u *b = NULL;
    int ret = 0;
    
    if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == 's'){
		char *string = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg1, 0));
        t_osc_msg_u *m = osc_bundle_u_getFirstFullMatch(u_bndl, string);
		if ( m ) {
            b = osc_atom_u_getBndl(osc_message_u_getArg(m, 0));
		} else {
			return OSC_ERR_EXPR_ADDRESSUNBOUND;
		}
	}
    else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == OSC_BUNDLE_TYPETAG)
    {
		b = osc_atom_u_getBndl(osc_atom_array_u_get(arg1, 0));
	}
    
	if(b)
    {
		if(osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_EXPR){
			t_osc_atom_ar_u *a = NULL;
			t_osc_expr *e = osc_expr_arg_getExpr(f_argv->next);
			if(e->rec->func == osc_expr_value){
				ret = osc_expr_u_evalArgInLexEnv(osc_expr_getArgs(e), lexenv, u_bndl, &a, context);
			}else{
				ret = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, &a, context);
			}
			if(a){
				t_osc_expr_arg *arg = osc_expr_arg_alloc();
				char *st = NULL;
				osc_atom_u_getString(osc_atom_array_u_get(a, 0), 0, &st);
				osc_expr_arg_setOSCAddress(arg, st);
				ret = osc_expr_u_evalArgInLexEnv(arg, lexenv, b, out, context);
				osc_atom_array_u_free(a);
				osc_expr_arg_free(arg);
			}else{
				return ret;
			}
		}else if(osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_STRING ||
			 osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_ATOM){
			osc_expr_arg_setType(f_argv->next, OSC_EXPR_ARG_TYPE_ATOM);
			t_osc_atom_ar_u *a = NULL;
			ret = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, u_bndl, &a, context);
			if(a){
				t_osc_expr_arg *arg = osc_expr_arg_alloc();
				char *st = NULL;
				osc_atom_u_getString(osc_atom_array_u_get(a, 0), 0, &st);
				osc_expr_arg_setOSCAddress(arg, st);
				ret = osc_expr_u_evalArgInLexEnv(arg, lexenv, b, out, context);
				osc_atom_array_u_free(a);
				osc_expr_arg_free(arg);
			}else{
				return ret;
			}
		}else{
			ret = osc_expr_u_evalArgInLexEnv(f_argv->next, lexenv, b, out, context);
		}
		osc_atom_array_u_free(arg1);
        return ret;
	}
	return 1;
}

static int osc_expr_u_specFunc_assignToBundleMember(t_osc_expr *f,
					     t_osc_expr_lexenv *lexenv,
					     t_osc_bndl_u *u_bndl,
					     t_osc_atom_ar_u **out, void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg1 = NULL;
	t_osc_err error = osc_expr_u_evalArgInLexEnv(f_argv, lexenv, u_bndl, &arg1, context);
    int free_arg1 = 1;
    t_osc_bndl_u *b = NULL;
	if(!arg1)
    {
        if( error == OSC_ERR_EXPR_ADDRESSUNBOUND && osc_expr_getArgCount(f) > 2 &&
           f_argv->type == OSC_EXPR_ARG_TYPE_OSCADDRESS && f_argv->next->type == OSC_EXPR_ARG_TYPE_OSCADDRESS )
        {
            t_osc_msg_u *m = osc_message_u_allocWithAddress( f_argv->arg.osc_address );
            b = osc_bundle_u_alloc();
            osc_message_u_appendBndl_u(m, b);
            osc_bundle_u_addMsg(u_bndl, m);
            arg1 = osc_message_u_getArgArrayCopy(m);
        }
        else
        {
            return 1;
        }
	}
	else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == 's')
    {
		char *string = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg1, 0));
        t_osc_msg_u *m = osc_bundle_u_getFirstFullMatch(u_bndl, string);
        if ( m ) {
            b = osc_atom_u_getBndl(osc_message_u_getArg(m, 0));
        } else {
            return OSC_ERR_EXPR_ADDRESSUNBOUND;
        }
    }
    else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == OSC_BUNDLE_TYPETAG)
    {
        b = osc_atom_u_getBndl(osc_atom_array_u_get(arg1, 0));
    }
    
    if(b)
    {
		t_osc_expr *assign = osc_expr_alloc();
		t_osc_expr_rec *r = osc_expr_lookupFunction("assign");
		osc_expr_setRec(assign, r);

		t_osc_atom_array_u *ar = NULL;
		osc_expr_u_evalArgInLexEnv(f_argv->next->next, lexenv, u_bndl, &ar, context);
		int ret = 0;
		if(!ar){
			goto cleanup;
		}
		t_osc_expr_arg *target = NULL;
		osc_expr_arg_copy(&target, f_argv->next);
		t_osc_expr_arg *val = osc_expr_arg_alloc();
		if(osc_atom_array_u_getLen(ar) == 1){
			t_osc_atom_u *copy = osc_atom_u_alloc();
			osc_atom_u_copyInto(&copy, osc_atom_array_u_get(ar, 0));
			osc_expr_arg_setOSCAtom(val, copy);
			osc_atom_array_u_free(ar);
		}else{
			t_osc_atom_ar_u *copy = osc_atom_array_u_copy(ar);
			osc_expr_arg_setList(val, copy);
			osc_atom_array_u_free(ar);
		}
		osc_expr_arg_append(target, val);
		osc_expr_setArg(assign, target);
		ret = osc_expr_u_specFunc_assign(assign, lexenv, b, out, context);
		osc_expr_arg_freeList(target);
		target = NULL;
		assign->argv = NULL;
		assign->argc = 0;
		if(*out){
			osc_atom_array_u_free(*out);
			*out = NULL;
		}

		if(ret){
			// cleanup?
			return ret;
		}

		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_EXPR) {
			t_osc_expr *e = osc_expr_arg_getExpr(f_argv);
			t_osc_expr_rec *r = osc_expr_getRec(e);
			if(osc_expr_rec_getFunction(r) == osc_expr_nth){
				osc_expr_setRec(assign, osc_expr_lookupFunction("assign_to_index"));
				t_osc_expr_arg *nth_args = osc_expr_getArgs(e);
				osc_expr_arg_copy(&target, nth_args);
				t_osc_expr_arg *nth_arg2 = NULL;
				osc_expr_arg_copy(&nth_arg2, osc_expr_arg_next(nth_args));
				osc_expr_arg_append(target, nth_arg2);
				t_osc_atom_u *a = osc_atom_u_alloc();
                
                t_osc_bndl_u *bcpy = osc_bundle_u_alloc();
                osc_bundle_u_copy(&bcpy, b);
				osc_atom_u_setBndl_u(a, bcpy);
				
                t_osc_expr_arg *arg_bndl = osc_expr_arg_alloc();
				osc_expr_arg_setOSCAtom(arg_bndl, a);
				osc_expr_arg_append(target, arg_bndl);
				osc_expr_setArg(assign, target);
				*out = NULL;
				ret = osc_expr_u_specFunc_assigntoindex(assign, lexenv, u_bndl, out, context);
			} else {
				return 1;
			}
		} else {
            
            t_osc_bndl_u *bcpy = osc_bundle_u_alloc();
            osc_bundle_u_copy(&bcpy, b);
            osc_atom_u_setBndl_u(osc_atom_array_u_get(arg1, 0), bcpy);

			t_osc_expr_arg *arg_bndl = osc_expr_arg_alloc();
			osc_expr_arg_setList(arg_bndl, arg1);
			free_arg1 = 0;
			osc_expr_arg_copy(&target, f_argv);
			osc_expr_arg_append(target, arg_bndl);
			osc_expr_setArg(assign, target);
			*out = NULL;
            
            // internally assign uses the replace message function, which frees the input message
            // so if there is no error, from the assign(), then I think we can skip freeing the assign
			ret = osc_expr_u_specFunc_assign(assign, lexenv, u_bndl, out, context);

		}

    cleanup:
		if(arg1 && free_arg1){
			osc_atom_array_u_free(arg1);
		}
		if(assign){
			osc_expr_free(assign);
		}
		return ret;
	}

	return 1;
}

static int osc_expr_u_specFunc_andalso(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
				 	 void* context)
{
	long argc = osc_expr_getArgCount(f);
	if(argc != 2){
		osc_expr_err_argnum(context, 2, argc, 0, "andalso");
		return 1;
	}
	t_osc_atom_ar_u *lhs = NULL, *rhs = NULL;
	int ret = osc_expr_u_evalArgInLexEnv(f->argv, lexenv, u_bndl, &lhs, context);
	if(!lhs || ret){
		return ret;
	}
	char lhstt = osc_atom_u_getTypetag(osc_atom_array_u_get(lhs, 0));
	if(lhstt == 'T' || lhstt == 'F'){
		if(lhstt == 'F'){
			*out = osc_atom_array_u_alloc(1);
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
			goto out;
		}
	}else{
		// post an error about type mismatch
		ret = 1;
		goto out;
	}

	ret = osc_expr_u_evalArgInLexEnv(f->argv->next, lexenv, u_bndl, &rhs, context);
	if(!rhs || ret){
		goto out;
	}
	char rhstt = osc_atom_u_getTypetag(osc_atom_array_u_get(rhs, 0));
	if(rhstt == 'T' || rhstt == 'F'){
		*out = osc_atom_array_u_alloc(1);
		if(rhstt == 'F'){
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
		}else{
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
		}
		goto out;
	}else{
		// post an error about type mismatch
		ret = 1;
		goto out;
	}
 out:
	if(lhs){
		osc_atom_array_u_free(lhs);
	}
	if(rhs){
		osc_atom_array_u_free(rhs);
	}
	return ret;
}

static int osc_expr_u_specFunc_orelse(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *u_bndl,
				   t_osc_atom_ar_u **out,
					 void* context)
{
	long argc = osc_expr_getArgCount(f);
	if(argc != 2){
		osc_expr_err_argnum(context, 2, argc, 0, "orelse");
		return 1;
	}
	t_osc_atom_ar_u *lhs = NULL, *rhs = NULL;
	int ret = osc_expr_u_evalArgInLexEnv(f->argv, lexenv, u_bndl, &lhs, context);
	if(!lhs || ret){
		return ret;
	}
	char lhstt = osc_atom_u_getTypetag(osc_atom_array_u_get(lhs, 0));
	if(lhstt == 'T' || lhstt == 'F'){
		if(lhstt == 'T'){
			*out = osc_atom_array_u_alloc(1);
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
			goto out;
		}
	}else{
		// post an error about type mismatch
		ret = 1;
		goto out;
	}

	ret = osc_expr_u_evalArgInLexEnv(f->argv->next, lexenv, u_bndl, &rhs, context);
	if(!rhs || ret){
		goto out;
	}
	char rhstt = osc_atom_u_getTypetag(osc_atom_array_u_get(rhs, 0));
	if(rhstt == 'T' || rhstt == 'F'){
		*out = osc_atom_array_u_alloc(1);
		if(rhstt == 'F'){
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
		}else{
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
		}
		goto out;
	}else{
		// post an error about type mismatch
		ret = 1;
		goto out;
	}
 out:
	if(lhs){
		osc_atom_array_u_free(lhs);
	}
	if(rhs){
		osc_atom_array_u_free(rhs);
	}
	return ret;
}

int osc_expr_scanner_lex (YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started);
t_osc_err osc_expr_u_lex(char *str, t_osc_atom_array_u **ar)
{
    //char *ptr = "/foo = sin(2 * pi() * /bar";
    char *ptr = str;
    YYSTYPE yylval_param;
    YYLTYPE llocp;
    yyscan_t scanner;
    osc_expr_scanner_lex_init(&scanner);
    YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
    osc_expr_scanner_set_out(NULL, scanner);
    int n = 64;
    //size_t atomsize = osc_atom_u_getStructSize();
    //t_osc_atom_u *out = osc_mem_alloc(n * atomsize);
    //memset(out, '\0', n * atomsize);
    *ar = osc_atom_array_u_alloc(n);
    int i = 0;
    //yylval_param.atom = out;
    yylval_param.atom = osc_atom_array_u_get(*ar, 0);
    long buflen = 0;
    char *buf = NULL;
    int startcond = START_EXPNS, started = 0;
    int ret = osc_expr_scanner_lex(&yylval_param, &llocp, scanner, 0, &buflen, &buf, startcond, &started);
    while(ret){
        if(i == n){
            n += 64;
            osc_array_resize(*ar, n);
        }
        char *st = NULL;
        switch(ret){
            case START_EXPNS:
                goto cont;
            case OSC_EXPR_STRING:
            case OSC_EXPR_NUM:
            case OSC_EXPR_OSCADDRESS:
            case OSC_EXPR_QUOTED_EXPR:
                break;
            case OSC_EXPR_LAMBDA:
                st = "lambda";
                break;
            case OSC_EXPR_POWEQ:
                st = "^=";
                break;
            case OSC_EXPR_MODEQ:
                st = "%=";
                break;
            case OSC_EXPR_DIVEQ:
                st = "/=";
                break;
            case OSC_EXPR_MULTEQ:
                st = "*=";
                break;
            case OSC_EXPR_MINUSEQ:
                st = "-=";
                break;
            case OSC_EXPR_PLUSEQ:
                st = "+=";
                break;
            case OSC_EXPR_DBLQMARKEQ:
                st = "??=";
                break;
            case OSC_EXPR_DBLQMARK:
                st = "??";
                break;
            case OSC_EXPR_OROR:
                st = "||";
                break;
            case OSC_EXPR_ANDAND:
                st = "&&";
                break;
            case OSC_EXPR_NEQ:
                st = "!=";
                break;
            case OSC_EXPR_EQ:
                st = "=";
                break;
            case OSC_EXPR_GTE:
                st = ">=";
                break;
            case OSC_EXPR_LTE:
                st = "<=";
                break;
            case CLOSE_DBL_BRKTS:
                st = "]]";
                break;
            case OPEN_DBL_BRKTS:
                st = "[[";
                break;
            case OSC_EXPR_DEC:
                st = "--";
                break;
            case OSC_EXPR_INC:
                st = "++";
                break;
            default:
            {
                char buf[2];
                sprintf(buf, "%c", ret);
                //osc_atom_u_setString((t_osc_atom_u *)(((long)out) + (i * atomsize)), buf);
                osc_atom_u_setString(osc_atom_array_u_get(*ar, i), buf);
                //osc_atom_u_setInt8(osc_atom_array_u_get(*ar, i), ret);
            }
        }
        if(st){
            //osc_atom_u_setString((t_osc_atom_u *)(((long)out) + (i * atomsize)), st);
            osc_atom_u_setString(osc_atom_array_u_get(*ar, i), st);
        }
        long len = osc_atom_u_nformat(NULL, 0, osc_atom_array_u_get(*ar, i), 0);
        char *fmt = osc_mem_alloc(len + 1);
        osc_atom_u_nformat(fmt, len + 1, osc_atom_array_u_get(*ar, i), 0);
        //osc_atom_u_format(osc_atom_array_u_get(*ar, i), &len, &fmt);
        i++;
        //yylval_param.atom = (t_osc_atom_u *)(((long)out) + (i * atomsize));
    cont:
        yylval_param.atom = osc_atom_array_u_get(*ar, i);
        ret = osc_expr_scanner_lex(&yylval_param, &llocp, scanner, 0, &buflen, &buf, startcond, &started);
    }
    //*ar = osc_atom_array_u_alloc(0);
    //osc_atom_array_u_set(*ar, out, i);
    osc_array_resize(*ar, i);
    osc_expr_scanner__delete_buffer(buf_state, scanner);
    osc_expr_scanner_lex_destroy(scanner);
    return 0;
}


/*
 *  documentation
 *
 */

static char *osc_expr_categories[] = {"/math/operator/arithmetic", "/math/operator/relational", "/math/operator/logical", "/math/operator/assignment", "/math/arithmetic", "/math/trigonometric", "/math/power", "/math/conversion", "/math/specialfunction", "/math/constant", "/vector", "/statistics", "/string/function", "/string/operator", "/predicate", "/conditional", "/core"};

// this gets created the first time it's needed and then it's never freed
t_osc_bndl_u *osc_expr_u_functionBundle = NULL;
t_osc_bndl_u *osc_expr_u_categoryBundle = NULL;


void osc_expr_u_makeFunctionBundle(void)
{
    t_osc_bndl_u *b = osc_bundle_u_alloc();
    int i;
    char buf[128];
    for(i = 0; i < sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec); i++){
        int j = 0;
        t_osc_expr_rec r = osc_expr_funcsym[i];
        while(r.categories[j]){
            t_osc_msg_u *m = osc_message_u_alloc();
            sprintf(buf, "/doc/category%s/%s", r.categories[j++], r.name);
            osc_message_u_setAddress(m, buf);
            osc_message_u_appendString(m, r.name);
            osc_bundle_u_addMsg(b, m);
        }
    }
    osc_expr_u_functionBundle = b;
}

void osc_expr_u_makeCategoryBundle(void)
{
    t_osc_bndl_u *b = osc_bundle_u_alloc();
    int i;
    char buf[128];
    for(i = 0; i < sizeof(osc_expr_categories) / sizeof(char *); i++){
        t_osc_msg_u *m = osc_message_u_alloc();
        sprintf(buf, "/doc/category/list%s", osc_expr_categories[i]);
        osc_message_u_setAddress(m, buf);
        osc_bundle_u_addMsg(b, m);
    }
}

t_osc_bndl_u *osc_expr_u_getCategories(void)
{
    if(!osc_expr_u_categoryBundle){
        osc_expr_u_makeCategoryBundle();
    }
    return osc_expr_u_categoryBundle;
}


t_osc_err osc_expr_u_getFunctionsForCategory(char *cat, t_osc_bndl_u ** u_bndl )
{
    if(!cat){
        return OSC_ERR_INVAL;
    }
    if(!osc_expr_u_functionBundle){
        osc_expr_u_makeFunctionBundle();
    }
    char buf[128];
    if(strncmp(cat, "/doc/category", 13)){
        sprintf(buf, "/doc/category%s", cat);
    }else{
        sprintf(buf, "%s", cat);
    }
    t_osc_msg_ar_u *ar = osc_bundle_u_lookupAddress(osc_expr_u_functionBundle, buf, 0);
    if(ar){
        if( !u_bndl )
            *u_bndl = osc_bundle_u_alloc();
        
        osc_bundle_u_addMsgArrayCopy(*u_bndl, ar);
        osc_message_array_u_free(ar);
    }
    return OSC_ERR_NONE;
}
