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
#include "osc_bundle_iterator_s.h"
#include "osc_bundle_iterator_u.h"
#include "osc_message_s.h"
#include "osc_message_u.h"
#include "osc_message_iterator_s.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_hashtab.h"
#include "osc_util.h"
#include "osc_rset.h"
#include "osc_query.h"
#include "osc_typetag.h"
#include "osc_match.h"

#include "osc_expr.h"
#include "osc_expr.r"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_expr_func.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_expr_privatedecls.h"

//#define __OSC_PROFILE__
#include "osc_profile.h"

//i hope i don't have to add any more ;)
char *osc_expr_typestrings[] = {"", "number", "list", "", "string", "", "", "", "atom", "", "", "", "", "", "", "", "expression", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "OSC address", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "boolean", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "function", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "timetag"};

#define OSC_EXPR_MAX_TYPESTRING_LEN strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_NUMBER]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_LIST]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_STRING]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_ATOM]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_EXPR]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_OSCADDRESS]) + strlen(osc_expr_typestrings[OSC_EXPR_ARG_TYPE_BOOLEAN]) + 1 + 12 // commas, spaces, and null byte

char *osc_expr_categories[] = {"/math/operator/arithmetic", "/math/operator/relational", "/math/operator/logical", "/math/operator/assignment", "/math/arithmetic", "/math/trigonometric", "/math/power", "/math/conversion", "/math/specialfunction", "/math/constant", "/vector", "/statistics", "/string/function", "/string/operator", "/predicate", "/conditional", "/core"};

// this gets created the first time it's needed and then it's never freed
t_osc_bndl_u *osc_expr_functionBundle = NULL;
t_osc_bndl_s *osc_expr_categoryBundle = NULL;

//static double rdtsc_cps = 0;

#ifdef __WIN32
char *strtok_r(char *str, const char *delim, char **save)
{
	char *res, *last;

	if( !save )
		return strtok(str, delim);
	if( !str && !(str = *save) )
		return NULL;
	last = str + strlen(str);
	if( (*save = res = strtok(str, delim)) )
		{
			*save += strlen(res);
			if( *save < last )
				(*save)++;
			else
				*save = NULL;
		}
	return res;
}
#endif

int _osc_expr_sign(double f);

t_osc_hashtab *osc_expr_funcobj_ht;
void osc_expr_funcobj_dtor(char *key, void *val);

extern t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f, void *context);
t_osc_err osc_expr_lex(char *str, t_osc_atom_array_u **ar);

int osc_expr_eval(t_osc_expr *f, long *len, char **oscbndl, t_osc_atom_ar_u **out, void *context)
{
	return osc_expr_evalInLexEnv(f, NULL, len, oscbndl, out, context);
}

int osc_expr_evalInLexEnv(t_osc_expr *f,
			  t_osc_expr_lexenv *lexenv,
			  long *len,
			  char **oscbndl,
			  t_osc_atom_ar_u **out,
				void *context)
{
	// printf("%s context %p\n",__func__, context );
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	if(f->rec->func == osc_expr_apply){
		return osc_expr_specFunc_apply(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_map){
		return osc_expr_specFunc_map(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_lreduce || f->rec->func == osc_expr_rreduce){
		return osc_expr_specFunc_reduce(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_assign){
		return osc_expr_specFunc_assign(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_assign_to_index){
		return osc_expr_specFunc_assigntoindex(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_if){
		return osc_expr_specFunc_if(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_emptybundle){
		return osc_expr_specFunc_emptybundle(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_bound){
		return osc_expr_specFunc_bound(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_exists){
		return osc_expr_specFunc_exists(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_getaddresses){
		return osc_expr_specFunc_getaddresses(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_delete){
		return osc_expr_specFunc_delete(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_getmsgcount){
		return osc_expr_specFunc_getmsgcount(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_value){
		return osc_expr_specFunc_value(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_quote){
		return osc_expr_specFunc_quote(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_eval_call){
		return osc_expr_specFunc_eval(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_tokenize){
		return osc_expr_specFunc_tokenize(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_compile){
		return osc_expr_specFunc_compile(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_gettimetag){
		return osc_expr_specFunc_gettimetag(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_settimetag){
		return osc_expr_specFunc_settimetag(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_getbundlemember){
		return osc_expr_specFunc_getBundleMember(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_assigntobundlemember){
		return osc_expr_specFunc_assignToBundleMember(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_andalso){
		return osc_expr_specFunc_andalso(f, lexenv, len, oscbndl, out, context);
	}else if(f->rec->func == osc_expr_orelse){
		return osc_expr_specFunc_orelse(f, lexenv, len, oscbndl, out, context);
	/* }else if(f->rec->func == osc_expr_bundle){ */
	/* 	return osc_expr_specFunc_bundle(f, lexenv, len, oscbndl, out, context); */
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
			int ret = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, argv + i, context);
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

t_osc_err osc_expr_evalArgInLexEnv(t_osc_expr_arg *arg,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
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
			t_osc_err e = osc_expr_evalInLexEnv(arg->arg.expr, lexenv, len, oscbndl, out, context);
			return e;
		}
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		{
			*out = NULL;
			if(!oscbndl || !len){
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
			if(!(*oscbndl) || *len <= 16){
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
#define __FAST__
#ifdef __FAST__
			char *mbytes = osc_bundle_s_getFirstFullMatch(*len, *oscbndl, arg->arg.osc_address);
			if(mbytes){
				char mm[osc_message_s_getStructSize()];
				memset(mm, 0, osc_message_s_getStructSize());
				t_osc_msg_s *m = (t_osc_msg_s *)mm;
				osc_message_s_wrap(m, mbytes);
				long arg_count = osc_message_s_getArgCount(m);
				*out = osc_atom_array_u_alloc(arg_count);

				t_osc_atom_ar_u *atom_ar = *out;
				osc_atom_array_u_clear(atom_ar);
				int i = 0;
				t_osc_msg_it_s *it = osc_msg_it_s_get(m);
				while(osc_msg_it_s_hasNext(it)){
					t_osc_atom_s *as = osc_msg_it_s_next(it);
					t_osc_atom_u *au = osc_atom_array_u_get(atom_ar, i);
					osc_atom_s_deserializeInto(&au, as);
					i++;
				}
				osc_msg_it_s_destroy(it);
				return 0;
			}
			return OSC_ERR_EXPR_ADDRESSUNBOUND;
		}
#else
			t_osc_rset *rset = NULL;
			osc_query_select(1, &(arg->arg.osc_address), *len, *oscbndl, 0, &rset);
			t_osc_rset_result *res = osc_rset_select(rset, arg->arg.osc_address);
			if(rset){
				t_osc_bndl_s *complete_matches = osc_rset_result_getCompleteMatches(res);
				if(complete_matches){
					t_osc_msg_s *m = osc_bundle_s_getFirstMsg(complete_matches);
					if(m){
						long arg_count = osc_message_s_getArgCount(m);
						*out = osc_atom_array_u_alloc(arg_count);

						t_osc_atom_ar_u *atom_ar = *out;
						osc_atom_array_u_clear(atom_ar);
						int i = 0;
						t_osc_msg_it_s *it = osc_msg_it_s_get(m);
						while(osc_msg_it_s_hasNext(it)){
							t_osc_atom_s *as = osc_msg_it_s_next(it);
							t_osc_atom_u *au = osc_atom_array_u_get(atom_ar, i);
							osc_atom_s_deserializeInto(&au, as);
							i++;
						}
						osc_msg_it_s_destroy(it);
						osc_rset_free(rset);
						osc_message_s_free(m);
						return 0;
					}
				}
				osc_rset_free(rset);
			}
	}
		/*
		  osc_error_handler(__FILE__,
		  __func__,
		  __LINE__,
		  OSC_ERR_EXPR_ADDRESSUNBOUND,
		  "address %s is unbound\n",
		  arg->arg.osc_address);
		*/
		return OSC_ERR_EXPR_ADDRESSUNBOUND;
#endif
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

static int osc_expr_specFunc_apply(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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
			t_osc_err e = osc_expr_evalArgInLexEnv(arg, lexenv, len, oscbndl, &atoms, context);
			if(e == OSC_ERR_EXPR_ADDRESSUNBOUND){
				osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(arg), "apply");
				return e;
			}
			osc_expr_bindVarInLexenv(lexenv_copy, params[i], atoms);
			arg = arg->next;
		}
		t_osc_expr *e = (t_osc_expr *)osc_expr_rec_getExtra(r);
		while(e){
			int ret =  osc_expr_evalInLexEnv(e, lexenv_copy, len, oscbndl, out, context);
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
			osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &a);
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
					int ret = osc_expr_specFunc_apply(f, lexenv, len, oscbndl, out, context);
					//osc_expr_setArg(f, f_argv);
					//osc_expr_arg_setNext(a, NULL);
					//osc_expr_arg_free(a);
					return ret;
				}
				//osc_atom_array_u_free(xx);
			}
		}else{
			t_osc_atom_ar_u *ar = NULL;
			t_osc_err e = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &ar, context);
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

					int ret = osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, out, context);
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
		int ret = osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, out, context);
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

static int osc_expr_specFunc_map(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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
		t_osc_err err = osc_expr_evalArgInLexEnv(a, lexenv, len, oscbndl, args + i, context);
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
		osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, output + i, context);
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

static int osc_expr_specFunc_reduce(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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
	int err = osc_expr_evalArgInLexEnv(a, lexenv, len, oscbndl, &args, context);
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
		osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, out, context);
		if(args){
			osc_atom_array_u_free(args);
		}
	}else if(count == 1 && f_argc == 2){
		*out = args;
	}else{
		t_osc_atom_ar_u *output = NULL;
		if(f_argc == 3){
			t_osc_atom_ar_u *sc = NULL;
			t_osc_err err = osc_expr_evalArgInLexEnv(f_argv->next->next, lexenv, len, oscbndl, &sc, context);
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
		osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, &output, context);

		for(int i = start; delta > 0 ? i < count : i >= 0; i += delta){
			t_osc_atom_u *copy1 = NULL;
			t_osc_atom_u *copy2 = NULL;
			osc_atom_u_copyInto(&copy1, osc_atom_array_u_get(output, 0));
			osc_atom_u_copyInto(&copy2, osc_atom_array_u_get(args, i));
			osc_expr_arg_setOSCAtom(func_args[arg0], copy1);
			osc_expr_arg_setOSCAtom(func_args[arg1], copy2);
			osc_atom_array_u_free(output);
			output = NULL;
			osc_expr_evalInLexEnv(e, lexenv, len, oscbndl, &output, context);
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

static int osc_expr_specFunc_assign(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
                void *context )
{
	if(!len || !oscbndl){
		return 1;
	}
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
						err = osc_expr_evalArgInLexEnv(value_args, lexenv, len, oscbndl, &address_ar, context);
						// don't report error--we'll try again below
					}
				}
			}
		}
		if(!address_ar){
			err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &address_ar, context);
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
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "the first argument of assign() must be an OSC address.");		
		return err;
	}

	t_osc_msg_u *mm = osc_message_u_alloc();
	osc_message_u_setAddress(mm, address);

	t_osc_err ret = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, out, context);

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
    
    
    long natoms = osc_atom_array_u_getLen(*out);
	for( int i = 0; i < natoms; i++ )
    {
		t_osc_atom_u *cpy = NULL;
		osc_atom_u_copyInto(&cpy, osc_atom_array_u_get(*out, i));
		osc_message_u_appendAtom(mm, cpy);
	}

	t_osc_msg_s *osc_msg_s = osc_message_u_serialize(mm);
	
    
    char *found_ptr = osc_bundle_s_getFirstEqualAddr(*len, *oscbndl, address);
    
    if( found_ptr )
    {
        t_osc_msg_s *m = osc_message_s_alloc();
        osc_message_s_wrap(m, found_ptr);
        osc_bundle_s_replaceMessage(len, len, oscbndl, m, osc_msg_s);
        osc_message_s_free(m);
    }
    else
    {
        osc_bundle_s_appendMessage(len, oscbndl, osc_msg_s);
    }
    
    /*
    t_osc_msg_ar_s *msg_ar = osc_bundle_s_lookupAddress(*len, *oscbndl, address, 1);

    if(msg_ar){
        char * test = osc_message_s_format( osc_message_array_s_get(msg_ar, 0) );
                                           
        printf("%s %s \n", address,  test );

        osc_bundle_s_replaceMessage(len, len, oscbndl, osc_message_array_s_get(msg_ar, 0), osc_msg_s);
        osc_message_array_s_free(msg_ar);
    }else{
        osc_bundle_s_appendMessage(len, oscbndl, osc_msg_s);
    }
     */
//err = osc_bundle_s_getMsgCount(*len, *oscbndl, &mc);

    if(address){
        osc_mem_free(address);
    }

	osc_message_s_deepFree(osc_msg_s);
    osc_message_u_free(mm);
	return 0;
}

static int osc_expr_specFunc_assigntoindex(t_osc_expr *f,
                                           t_osc_expr_lexenv *lexenv,
                                           long *len,
                                           char **oscbndl,
                                           t_osc_atom_ar_u **out,
                                           void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	int f_argc = osc_expr_getArgCount(f);
	if(f_argc != 3){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_msg_ar_s *msg_ar = osc_bundle_s_lookupAddress(*len, *oscbndl, f_argv->arg.osc_address, 1);
	if(!msg_ar){
		osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "assign to index =");
		return 1;
	}

	t_osc_atom_ar_u *indexes = NULL;
	t_osc_atom_ar_u *data = NULL;
    t_osc_err err = 0;
    // above needed to allow goto, which you probably hate, sorry
	t_osc_msg_u *mm = osc_message_u_alloc();
    {
        osc_message_u_setAddress(mm, f_argv->arg.osc_address);

        // get data at target address
        err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, out, context);
        if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
            osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), "assign to index");
            goto bail;
        }
        int outlen = osc_atom_array_u_getLen(*out);

        // get index(es)
        err = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, &indexes, context);
        if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
            osc_error(context, OSC_ERR_EXPR_ADDRESSUNBOUND, "%s assign to index : address %s is unbound", osc_expr_arg_getOSCAddress(f_argv), osc_expr_arg_getOSCAddress(f_argv->next));
            // osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next), "assign to index 2");
            goto bail;
        }
        int nindexes = osc_atom_array_u_getLen(indexes);
        // get data
        err = osc_expr_evalArgInLexEnv(f_argv->next->next, lexenv, len, oscbndl, &data, context);
        if(err == OSC_ERR_EXPR_ADDRESSUNBOUND){
            //osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv->next->next), "assign to index 3");
            osc_error(context, OSC_ERR_EXPR_ADDRESSUNBOUND, "%s assign to index : address %s is unbound", osc_expr_arg_getOSCAddress(f_argv), osc_expr_arg_getOSCAddress(f_argv->next->next));

            goto bail;
        }
        int ndata = osc_atom_array_u_getLen(data);
        if(nindexes == 0 || ndata == 0){
            err = 1;
            goto bail;
        }else if(nindexes == 1 && ndata > 1){
            int idx = osc_atom_u_getInt(osc_atom_array_u_get(indexes, 0));
            if(idx >= outlen || idx < 0){
                osc_error(context, OSC_ERR_EXPR_EVAL, "%s : assign to index %d exceeds array length %d", osc_expr_arg_getOSCAddress(f_argv), idx, outlen);
                err = 1;
                goto bail;
            }
            t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
            osc_atom_u_copyInto(&dest, osc_atom_array_u_get(data, 0));
        }else if(nindexes > 1 && ndata == 1){
            int i, idx;
            t_osc_atom_u *a = osc_atom_array_u_get(data, 0);
            for(i = 0; i < nindexes; i++){
                idx = osc_atom_u_getInt(osc_atom_array_u_get(indexes, i));
                if(idx >= outlen || idx < 0){
                    osc_error(context, OSC_ERR_EXPR_EVAL, "%s : assign to index %d exceeds array length %d", osc_expr_arg_getOSCAddress(f_argv), idx, outlen);
                    continue;
                }
                t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
                osc_atom_u_copyInto(&dest, a);
            }
        }else{
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
        int i;
        for(i = 0; i < osc_atom_array_u_getLen(*out); i++){
            t_osc_atom_u *cpy = NULL;
            osc_atom_u_copyInto(&cpy, osc_atom_array_u_get(*out, i));
            osc_message_u_appendAtom(mm, cpy);
        }

        t_osc_msg_s *osc_msg_s = osc_message_u_serialize(mm);
        if(msg_ar){
            osc_bundle_s_replaceMessage(len, len, oscbndl, osc_message_array_s_get(msg_ar, 0), osc_msg_s);
            osc_message_array_s_free(msg_ar);
        }else{
            osc_bundle_s_appendMessage(len, oscbndl, osc_msg_s);
        }
	if(osc_msg_s){
		osc_message_s_deepFree(osc_msg_s);
	}
        err = 0;
    }

bail:
    if (indexes)
        osc_atom_array_u_free(indexes);
    if (data)
        osc_atom_array_u_free(data);
    if (mm)
        osc_message_u_free(mm);

    return err;
}

static int osc_expr_specFunc_if(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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
	t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &argv, context);
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
			err = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, boolvec + j, context);
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
				err = osc_expr_evalArgInLexEnv(f_argv->next->next, lexenv, len, oscbndl, boolvec + j, context);
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

static int osc_expr_specFunc_emptybundle(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	
	int f_argc = osc_expr_getArgCount(f);
	if(f_argc){
		t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
		t_osc_atom_ar_u *ar = NULL;
		t_osc_err r = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &ar, context);
		if(r == OSC_ERR_EXPR_ADDRESSUNBOUND){
			osc_expr_err_unbound(context, osc_expr_arg_getOSCAddress(f_argv), osc_expr_rec_getName(osc_expr_getRec(f)));
			return r;
		}
		if(ar && osc_atom_array_u_getLen(ar) > 0){
			t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
			if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
				if(osc_bundle_u_getMsgCount(osc_atom_u_getBndl(a))){
					osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
				}else{
					osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
				}
			}else{
				osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
			}
		}else{
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
		}
	}else{
		if(*len == OSC_HEADER_SIZE){
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
		}else{
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
		}
	}
	return 0;
}

static int osc_expr_specFunc_existsorbound(t_osc_expr *f,
					   t_osc_expr_lexenv *lexenv,
					   long *len,
					   char **oscbndl,
					   t_osc_atom_ar_u **out,
					   t_osc_err (*func)(long, char*, char*, int, int*),
	 					 void *context)
{
	if(!len || !oscbndl){
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
				osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &ar, context);
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
				int err = osc_expr_evalInLexEnv(osc_expr_arg_getExpr(f_argv), lexenv, len, oscbndl, &ar, context);
				*out = osc_atom_array_u_alloc(1);
				if(err){
					osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
				}else if(!ar){
					if(func == osc_bundle_s_addressIsBound){
						osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
					}else{
						osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
					}
				}else{
					if(osc_atom_array_u_getLen(ar) == 0 && func == osc_bundle_s_addressIsBound){
						osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
					}else{
						osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
					}
				}
				osc_atom_array_u_free(ar);
				return 0;
			}else{
				t_osc_atom_ar_u *ar = NULL;
				int err = osc_expr_evalInLexEnv(osc_expr_arg_getExpr(f_argv), lexenv, len, oscbndl, &ar, context);
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
		func(*len, *oscbndl, address, 1, &res);
		osc_atom_u_setBool(osc_atom_array_u_get(*out, 0), res);
		osc_mem_free(address);
		return 0;
	}
 err:
	osc_error(context, OSC_ERR_EXPR_ARGCHK, "arg 1 should be an OSC address");
	return 1;

}

static int osc_expr_specFunc_bound(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	return osc_expr_specFunc_existsorbound(f, lexenv, len, oscbndl, out, osc_bundle_s_addressIsBound, context);
}

static int osc_expr_specFunc_exists(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	return osc_expr_specFunc_existsorbound(f, lexenv, len, oscbndl, out, osc_bundle_s_addressExists, context);
}

static int osc_expr_specFunc_getaddresses(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	int n = 0;
	if(osc_expr_getArgCount(f)){
		t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
		t_osc_atom_ar_u *ar = NULL;
		osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &ar, context);
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
		t_osc_bndl_it_s *it = osc_bndl_it_s_get(*len, *oscbndl);
		while(osc_bndl_it_s_hasNext(it)){
			osc_bndl_it_s_next(it);
			n++;
		}
		*out = osc_atom_array_u_alloc(n);
		osc_bndl_it_s_reset(it);
		n = 0;
		while(osc_bndl_it_s_hasNext(it)){
			t_osc_msg_s *m = osc_bndl_it_s_next(it);
			osc_atom_u_setString(osc_atom_array_u_get(*out, n), osc_message_s_getAddress(m));
			n++;
		}
		osc_bndl_it_s_destroy(it);
	}
	return 0;
}

static int osc_expr_specFunc_delete(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	int argc = osc_expr_getArgCount(f);
	if(!argc){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	while(f_argv){
		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
			osc_bundle_s_removeMessage(osc_expr_arg_getOSCAddress(f_argv), len, *oscbndl, 1);
		}else if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_ATOM){
			t_osc_atom_u *a = osc_expr_arg_getOSCAtom(f_argv);
			if(osc_atom_u_getTypetag(a) == 's'){
				char *st = NULL;
				osc_atom_u_getString(a, 0, &st);
				osc_bundle_s_removeMessage(st, len, *oscbndl, 1);
			}
		}
		f_argv = f_argv->next;
	}
	return 0;
}

static int osc_expr_specFunc_getmsgcount(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	int n = 0;
	t_osc_bndl_it_s *it = osc_bndl_it_s_get(*len, *oscbndl);
	while(osc_bndl_it_s_hasNext(it)){
		osc_bndl_it_s_next(it);
		n++;
	}
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), n);
	osc_bndl_it_s_destroy(it);
	return 0;
}

static int osc_expr_specFunc_value(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out,
 					 void *context)
{
	if(!len || !oscbndl){
		return 1;
	}
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg, context);
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
		if(address){
			t_osc_msg_ar_s *ar = osc_bundle_s_lookupAddress(*len, *oscbndl, address, 1);
			if(ar){
				t_osc_msg_s *m = osc_message_array_s_get(ar, 0);
				int argc = osc_message_s_getArgCount(m);
				*out = osc_atom_array_u_alloc(argc);

				osc_array_clear(*out);
				t_osc_atom_s *a = osc_atom_s_alloc(0, NULL);
				int i;
				for(i = 0; i < argc; i++){
					osc_message_s_getArg(m, i, &a);
					t_osc_atom_u *au = osc_atom_array_u_get(*out, i);
					osc_atom_s_deserializeInto(&au, a);
					//double f1 = osc_atom_s_getDouble(a);
				}
				osc_mem_free(a);
				osc_message_array_s_free(ar);
			}
		}
		osc_atom_array_u_free(arg);
	}

	return 0;
}

static int osc_expr_specFunc_quote(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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

static int osc_expr_specFunc_eval(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg, context);
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
				ret = osc_expr_eval(ff, len, oscbndl, &ar, context);
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
				ret = osc_expr_eval(ff, len, oscbndl, &ar, context);
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
			t_osc_expr *e = osc_hashtab_lookup(osc_expr_funcobj_ht, strlen(a), a);
			if(e){
				t_osc_atom_ar_u *ar = NULL;
				int ret = osc_expr_eval(f, len, oscbndl, &ar, context);
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

int osc_expr_specFunc_tokenize(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	//char *ptr = "/foo += sin(2 * pi() * /bar)";
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	char *expr = NULL;
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg, context);
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

static int osc_expr_specFunc_compile(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
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
		t_osc_err err = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, &arg, context);
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
		if(!osc_expr_funcobj_ht){
			osc_expr_funcobj_ht = osc_hashtab_new(-1, osc_expr_funcobj_dtor);
		}
		osc_hashtab_store(osc_expr_funcobj_ht, keylen, key, thisf);
	}
	if(expression){
		osc_mem_free(expression);
	}
	return 0;
}

static int osc_expr_specFunc_gettimetag(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	*out = osc_atom_array_u_alloc(1);
	t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
	osc_atom_u_setTimetag(a, osc_bundle_s_getTimetag(*len, *oscbndl));
	return 0;
}

static int osc_expr_specFunc_settimetag(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out,
					void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg = NULL;
	t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg, context);
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

static int osc_expr_specFunc_getBundleMember(t_osc_expr *f,
					     t_osc_expr_lexenv *lexenv,
					     long *len,
					     char **oscbndl,
					     t_osc_atom_ar_u **out,
		 					 void *context)
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg1 = NULL;
	osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg1, context);
	if(!arg1){
		return 1;
	}
	long bndl_len_s = 0;
	char *bndl_s = NULL;
	if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == 's'){
		char *string = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg1, 0));
		t_osc_message_array_s *msgar = osc_bundle_s_lookupAddress(*len, *oscbndl, string, 1);
		if(msgar){
			t_osc_msg_s *m = osc_message_array_s_get(msgar, 0);
			t_osc_atom_s *a = NULL;
			osc_message_s_getArg(m, 0, &a);
			//char tmp[osc_bundle_s_getStructSize()];
			//t_osc_bndl_s *b = (t_osc_bndl_s *)tmp;
			t_osc_bndl_s *b = osc_atom_s_getBndlCopy(a);
			bndl_len_s = osc_bundle_s_getLen(b);
			bndl_s = osc_bundle_s_getPtr(b);
			osc_atom_s_free(a);
			osc_bundle_s_free(b);
			osc_message_array_s_free(msgar);
		}else{
			return OSC_ERR_EXPR_ADDRESSUNBOUND;
		}
	}else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == OSC_BUNDLE_TYPETAG){
		t_osc_bndl_u *b = osc_atom_u_getBndl(osc_atom_array_u_get(arg1, 0));
		bndl_len_s = osc_bundle_u_nserialize(NULL, 0, b);
		bndl_s = osc_mem_alloc(bndl_len_s);
		osc_bundle_u_nserialize(bndl_s, bndl_len_s, b);
	}//else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) ==

	int ret = 0;
	if(bndl_len_s && bndl_s){
	/* if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == OSC_BUNDLE_TYPETAG){ */
	/* 	t_osc_bndl_u *b = osc_atom_u_getBndl(osc_atom_array_u_get(arg1, 0)); */
	/* 	if(b){ */
	/* 		long l = 0; */
	/* 		char *p = NULL; */
	/* 		osc_bundle_u_serialize(b, &l, &p); */
		//if(p){

		if(osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_EXPR){
			t_osc_atom_ar_u *a = NULL;
			t_osc_expr *e = osc_expr_arg_getExpr(f_argv->next);
			if(e->rec->func == osc_expr_value){
				ret = osc_expr_evalArgInLexEnv(osc_expr_getArgs(e), lexenv, len, oscbndl, &a, context);
			}else{
				ret = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, &a, context);
			}
			if(a){
				t_osc_expr_arg *arg = osc_expr_arg_alloc();
				char *st = NULL;
				osc_atom_u_getString(osc_atom_array_u_get(a, 0), 0, &st);
				osc_expr_arg_setOSCAddress(arg, st);
				ret = osc_expr_evalArgInLexEnv(arg, lexenv, &bndl_len_s, &bndl_s, out, context);
				osc_atom_array_u_free(a);
				osc_expr_arg_free(arg);
			}else{
				return ret;
			}
		}else if(osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_STRING ||
			 osc_expr_arg_getType(f_argv->next) == OSC_EXPR_ARG_TYPE_ATOM){
			osc_expr_arg_setType(f_argv->next, OSC_EXPR_ARG_TYPE_ATOM);
			t_osc_atom_ar_u *a = NULL;
			ret = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, len, oscbndl, &a, context);
			if(a){
				t_osc_expr_arg *arg = osc_expr_arg_alloc();
				char *st = NULL;
				osc_atom_u_getString(osc_atom_array_u_get(a, 0), 0, &st);
				osc_expr_arg_setOSCAddress(arg, st);
				ret = osc_expr_evalArgInLexEnv(arg, lexenv, &bndl_len_s, &bndl_s, out, context);
				osc_atom_array_u_free(a);
				osc_expr_arg_free(arg);
			}else{
				return ret;
			}
		}else{
			ret = osc_expr_evalArgInLexEnv(f_argv->next, lexenv, &bndl_len_s, &bndl_s, out, context);
		}
		osc_mem_free(bndl_s);
		osc_atom_array_u_free(arg1);
				return ret;
	}
	return 1;
}

static int osc_expr_specFunc_assignToBundleMember(t_osc_expr *f,
						  t_osc_expr_lexenv *lexenv,
						  long *len,
						  char **oscbndl,
						  t_osc_atom_ar_u **out,
						  void *context )
{
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	t_osc_atom_ar_u *arg1 = NULL;

	t_osc_err error = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &arg1, context);
	if(!arg1){
		// if arg1 is unbound, create a bundle with message arg2 : arg3
		if( error == OSC_ERR_EXPR_ADDRESSUNBOUND && osc_expr_getArgCount(f) > 2 &&
		    f_argv->type == OSC_EXPR_ARG_TYPE_OSCADDRESS && f_argv->next->type == OSC_EXPR_ARG_TYPE_OSCADDRESS ){
			t_osc_msg_u *m = osc_message_u_allocWithAddress( f_argv->arg.osc_address );
			t_osc_bundle_u * b = osc_bundle_u_alloc();
			osc_message_u_appendBndl_u(m, b);
            
			osc_bundle_s_appendMessage(len, oscbndl, osc_message_u_serialize(m) );
            
			arg1 = osc_message_u_getArgArrayCopy(m);
            
		}else{
			return 1;
		}
	}
	int free_arg1 = 1;
	long bndl_len_s = 0;
	char *bndl_s = NULL;
	if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == 's'){
		char *string = osc_atom_u_getStringPtr(osc_atom_array_u_get(arg1, 0));
		t_osc_message_array_s *msgar = osc_bundle_s_lookupAddress(*len, *oscbndl, string, 1);
		if(msgar){
			t_osc_msg_s *m = osc_message_array_s_get(msgar, 0);
			t_osc_atom_s *a = NULL;
			osc_message_s_getArg(m, 0, &a);
			//char tmp[osc_bundle_s_getStructSize()];
			//t_osc_bndl_s *b = (t_osc_bndl_s *)tmp;
			t_osc_bndl_s *b = osc_atom_s_getBndlCopy(a);
			bndl_len_s = osc_bundle_s_getLen(b);
			bndl_s = osc_bundle_s_getPtr(b);
			osc_atom_s_free(a);
			osc_bundle_s_free(b);
			osc_message_array_s_free(msgar);
		}
	}else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == OSC_BUNDLE_TYPETAG){
		t_osc_bndl_u *b = osc_atom_u_getBndl(osc_atom_array_u_get(arg1, 0));
		bndl_len_s = osc_bundle_u_nserialize(NULL, 0, b);
		bndl_s = osc_mem_alloc(bndl_len_s);
		osc_bundle_u_nserialize(bndl_s, bndl_len_s, b);
	}

	if(bndl_len_s && bndl_s){
		t_osc_expr *assign = osc_expr_alloc();
		t_osc_expr_rec *r = osc_expr_lookupFunction("assign");
		osc_expr_setRec(assign, r);

		t_osc_atom_array_u *ar = NULL;
		osc_expr_evalArgInLexEnv(f_argv->next->next, lexenv, len, oscbndl, &ar, context);
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
		ret = osc_expr_specFunc_assign(assign, lexenv, &bndl_len_s, &bndl_s, out, context);
		osc_expr_arg_freeList(target);
		target = NULL;
		assign->argv = NULL;
		assign->argc = 0;
		if(*out){
			osc_atom_array_u_free(*out);
			*out = NULL;
		}

		if(ret){
			// cleanup
			//return ret;
			goto cleanup;
		}

		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_EXPR){
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
				osc_atom_u_setBndl_s(a, bndl_len_s, bndl_s);
				t_osc_expr_arg *arg_bndl = osc_expr_arg_alloc();
				osc_expr_arg_setOSCAtom(arg_bndl, a);
				osc_expr_arg_append(target, arg_bndl);
				osc_expr_setArg(assign, target);
				*out = NULL;
				ret = osc_expr_specFunc_assigntoindex(assign, lexenv, len, oscbndl, out, context);
			}else if(osc_atom_u_getTypetag(osc_atom_array_u_get(arg1, 0)) == 's'){
				char *target_address = NULL;
				osc_util_strdup(&target_address, osc_atom_u_getStringPtr(osc_atom_array_u_get(arg1, 0)));
				t_osc_expr_arg *target = osc_expr_arg_alloc();
				osc_expr_arg_setOSCAddress(target, target_address);

				t_osc_atom_u *value_atom = osc_atom_u_alloc();
				osc_atom_u_setBndl(value_atom, bndl_len_s, bndl_s);
				t_osc_expr_arg *value = osc_expr_arg_alloc();
				osc_expr_arg_setOSCAtom(value, value_atom);

				osc_expr_arg_append(target, value);
				osc_expr_setArg(assign, target);
				*out = NULL;
				ret = osc_expr_specFunc_assign(assign, lexenv, len, oscbndl, out, context);
			}else{
				osc_error(context, OSC_ERR_EXPR_ARGCHK, "the first argument to assigntobundlemember() is an expression that is not assignable");
				ret = 1;
			}
		}else{
			osc_atom_u_setBndl_s(osc_atom_array_u_get(arg1, 0), bndl_len_s, bndl_s);
			t_osc_expr_arg *arg_bndl = osc_expr_arg_alloc();
			osc_expr_arg_setList(arg_bndl, arg1);
			free_arg1 = 0;
			osc_expr_arg_copy(&target, f_argv);
			osc_expr_arg_append(target, arg_bndl);
			osc_expr_setArg(assign, target);
			*out = NULL;
			ret = osc_expr_specFunc_assign(assign, lexenv, len, oscbndl, out, context);
		}

	cleanup:
		if(arg1 && free_arg1){
			osc_atom_array_u_free(arg1);
		}
		if(assign){
			osc_expr_free(assign);
		}
		if(bndl_s){
			osc_mem_free(bndl_s);
		}
		return ret;
	}

	return 1;
}

int osc_expr_scanner_lex (YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started);
t_osc_err osc_expr_lex(char *str, t_osc_atom_array_u **ar){
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

void osc_expr_lexenvDtor(char *key, void *val)
{
	if(key){
		osc_mem_free(key);
	}
	if(val){
		osc_atom_array_u_free((t_osc_atom_ar_u *)val);
	}
}

t_osc_expr_lexenv *osc_expr_makeLexenv(void)
{
	return osc_hashtab_new(0, osc_expr_lexenvDtor);
}

void osc_expr_destroyLexenv(t_osc_expr_lexenv *lexenv)
{
	osc_hashtab_destroy(lexenv);
}

void osc_expr_copyLexenv_cb(char *key, void *val, void *context)
{
	t_osc_hashtab *ht = (t_osc_hashtab *)context;
	if(!ht || !val || !key){
		return;
	}
	int keylen = strlen(key) + 1;
	char *key_copy = osc_mem_alloc(keylen);
	strncpy(key_copy, key, keylen);
	t_osc_atom_ar_u *ar = osc_atom_array_u_copy((t_osc_atom_ar_u *)val);
	osc_hashtab_store(ht, keylen - 1, key_copy, (void *)ar);
}

void osc_expr_copyLexenv(t_osc_expr_lexenv **dest, t_osc_expr_lexenv *src)
{
	t_osc_hashtab *copy = osc_hashtab_new(0, osc_expr_lexenvDtor);
	osc_hashtab_foreach(src, osc_expr_copyLexenv_cb, (void *)copy);
	*dest = (t_osc_expr_lexenv *)copy;
}

void osc_expr_bindVarInLexenv(t_osc_expr_lexenv *lexenv, char *varname, t_osc_atom_ar_u *val)
{
	int len = strlen(varname) + 1;
	char *copy = osc_mem_alloc(len);
	strncpy(copy, varname, len);
	if(lexenv && varname){
		osc_hashtab_store((t_osc_hashtab *)lexenv, len - 1, copy, (void *)val);
	}
}

t_osc_atom_ar_u *osc_expr_lookupBindingInLexenv(t_osc_expr_lexenv *lexenv, char *varname)
{
	if(lexenv && varname){
		return osc_hashtab_lookup((t_osc_hashtab *)lexenv, strlen(varname), varname);
	}
	return NULL;
}

t_osc_expr_rec *osc_expr_lookupFunction(char *name)
{
	t_osc_expr_rec *rec = NULL;
	int i;
	for(i = 0; i < sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec); i++){
		if(!strcmp(name, osc_expr_funcsym[i].name)){
			rec = osc_expr_funcsym + i;
			break;
		}
	}
	return rec;
}

int osc_expr_1arg_dbl(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	long ac = osc_atom_array_u_getLen(*argv);
	if(argc == 0 || osc_atom_array_u_getLen(*argv) < 1){
        osc_expr_err_argnum( context, 1, 0, 0, f->rec->name );
		return 1;
	}
	*out = osc_atom_array_u_alloc(ac);

	t_osc_atom_ar_u *result = *out;
	double (*func)(double) = (double (*)(double))(f->rec->extra);

	int i;
	for(i = 0; i < ac; i++){
		osc_atom_u_setDouble(osc_atom_array_u_get(result, i), func(osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i))) );
	}
	return 0;
}

int osc_expr_2arg_dbl_dbl(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
    if( argc != 2 )
    {
        osc_expr_err_argnum( context, 2, argc, 0, f->rec->name );
        return 1;
    }

	uint32_t argc0 = osc_atom_array_u_getLen(argv[0]);
	uint32_t argc1 = osc_atom_array_u_getLen(argv[1]);
	if(argc0 < 1 || argc1 < 1){
		osc_expr_err_argnum(context, 2, argc0 < 1 && argc1 < 1 ? 0 : 1, 0, f->rec->name);
		return 1;
	}
	uint32_t min_argc = argc0, max_argc = argc1;
	if(argc0 > argc1){
		min_argc = argc1, max_argc = argc0;
	}
	int i;
	double (*func)(double,double) = (double (*)(double,double))(f->rec->extra);
	if(argc0 == 1){
		*out = osc_atom_array_u_alloc(max_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < max_argc; i++){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i),
					     func(osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 0)),
						  osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i))));
		}
		return 0;
	}else if(argc1 == 1){
		*out = osc_atom_array_u_alloc(max_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < max_argc; i++){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i),
					     func(osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i)),
						  osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0))));
		}
		return 0;
	}else{
		*out = osc_atom_array_u_alloc(min_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < min_argc; i++){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i),
					     func(osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i)),
						  osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i))));
		}
		return 0;
	}
}

int osc_expr_2arg(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
    
    if( argc != 2 )
    {
        osc_expr_err_argnum( context, 2, argc, 0, f->rec->name );
        return 1;
    }
    
	uint32_t argc0 = osc_atom_array_u_getLen(argv[0]);
	uint32_t argc1 = osc_atom_array_u_getLen(argv[1]);
	if(argc0 < 1 || argc1 < 1){
		osc_expr_err_argnum(context, 2, argc0 < 1 && argc1 < 1 ? 0 : 1, 0, f->rec->name);
		return 1;
	}
	uint32_t min_argc = argc0, max_argc = argc1;
	if(argc0 > argc1){
		min_argc = argc1, max_argc = argc0;
	}
	int i;
	int (*func)(t_osc_atom_u*,t_osc_atom_u*,t_osc_atom_u**,void*) = (int (*)(t_osc_atom_u*,t_osc_atom_u*,t_osc_atom_u**,void*))(f->rec->extra);
	int ret = 0;
	if(argc0 == 1){
		*out = osc_atom_array_u_alloc(max_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < max_argc; i++){
			t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
			ret = func(osc_atom_array_u_get(argv[0], 0), osc_atom_array_u_get(argv[1], i), &a, context);
			if(ret){
				return ret;
			}
		}
		return 0;
	}else if(argc1 == 1){
		*out = osc_atom_array_u_alloc(max_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < max_argc; i++){
			t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
			ret = func(osc_atom_array_u_get(argv[0], i), osc_atom_array_u_get(argv[1], 0), &a, context);
			if(ret){
				return ret;
			}
		}
		return 0;
	}else{
		*out = osc_atom_array_u_alloc(min_argc);

		osc_atom_array_u_clear(*out);
		for(i = 0; i < min_argc; i++){
			t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
			ret = func(osc_atom_array_u_get(argv[0], i), osc_atom_array_u_get(argv[1], i), &a, context);
			if(ret){
				return ret;
			}
		}
		return 0;
	}
}

// wrappers for infix ops
int osc_expr_add(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1){
		osc_atom_u_copyInto(result, f2);
		return 0;
	}
	if(!f2){
		osc_atom_u_copyInto(result, f1);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		char *sp1 = osc_atom_u_getStringPtr(f1);
		char *sp2 = osc_atom_u_getStringPtr(f2);
		char buf[strlen(sp1) + strlen(sp2) + 1];
		sprintf(buf, "%s%s", sp1, sp2);
		osc_atom_u_setString(*result, buf);
	}else if(tt1 == 's' && OSC_TYPETAG_ISNUMERIC(tt2)){
		char *sp1 = osc_atom_u_getStringPtr(f1);
		char *sp2 = NULL;
		osc_atom_u_getString(f2, 0, &sp2);
		if(!sp2){
			return 0;
		}
		char buf[strlen(sp1) + strlen(sp2) + 1];
		sprintf(buf, "%s%s", sp1, sp2);
		osc_atom_u_setString(*result, buf);
		osc_mem_free(sp2);
	}else if(tt2 == 's' && OSC_TYPETAG_ISNUMERIC(tt1)){
		char *sp2 = osc_atom_u_getStringPtr(f2);
		char *sp1 = NULL;
		osc_atom_u_getString(f1, 0, &sp1);
		if(!sp1){
			return 0;
		}
		char buf[strlen(sp1) + strlen(sp2) + 1];
		sprintf(buf, "%s%s", sp1, sp2);
		osc_atom_u_setString(*result, buf);
		osc_mem_free(sp1);
	}else if(tt1 == OSC_TIMETAG_TYPETAG || tt2 == OSC_TIMETAG_TYPETAG){
		t_osc_timetag timetag1, timetag2;
		if(tt1 == OSC_TIMETAG_TYPETAG && tt2 == OSC_TIMETAG_TYPETAG){
			timetag1 = osc_atom_u_getTimetag(f1);
			timetag2 = osc_atom_u_getTimetag(f2);
		}else if(tt1 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt2)){
			timetag1 = osc_atom_u_getTimetag(f1);
			timetag2 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f2));
		}else if(tt2 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt1)){
			timetag1 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f1));
			timetag2 = osc_atom_u_getTimetag(f2);
		}else{
			return 1;
		}
		osc_atom_u_setTimetag(*result, osc_timetag_add(timetag1, timetag2));
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		if(OSC_TYPETAG_ISFLOAT(tt1) || OSC_TYPETAG_ISFLOAT(tt2)){
			osc_atom_u_setDouble(*result, osc_atom_u_getDouble(f1) + osc_atom_u_getDouble(f2));
		}else{
			char tt = osc_typetag_compare(tt1, tt2) > 0 ? tt1 : tt2;
			switch(tt){
			case 'i':
				osc_atom_u_setInt32(*result, osc_atom_u_getInt32(f1) + osc_atom_u_getInt32(f2));
				break;
			case 'I':
				osc_atom_u_setUInt32(*result, osc_atom_u_getUInt32(f1) + osc_atom_u_getUInt32(f2));
				break;
			case 'h':
				osc_atom_u_setInt64(*result, osc_atom_u_getInt64(f1) + osc_atom_u_getInt64(f2));
				break;
			case 'H':
				osc_atom_u_setUInt64(*result, osc_atom_u_getUInt64(f1) + osc_atom_u_getUInt64(f2));
				break;
			case 'u':
				osc_atom_u_setInt16(*result, osc_atom_u_getInt16(f1) + osc_atom_u_getInt16(f2));
				break;
			case 'U':
				osc_atom_u_setUInt16(*result, osc_atom_u_getUInt16(f1) + osc_atom_u_getUInt16(f2));
				break;
			case 'c':
				osc_atom_u_setInt8(*result, osc_atom_u_getInt8(f1) + osc_atom_u_getInt8(f2));
				break;
			case 'C':
				osc_atom_u_setUInt8(*result, osc_atom_u_getUInt8(f1) + osc_atom_u_getUInt8(f2));
				break;
			}
		}
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, "+", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, "+", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_subtract(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1){
		osc_atom_u_copyInto(result, f2);
		return 0;
	}
	if(!f2){
		osc_atom_u_copyInto(result, f1);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		if(OSC_TYPETAG_ISFLOAT(tt1) || OSC_TYPETAG_ISFLOAT(tt2)){
			osc_atom_u_setDouble(*result, osc_atom_u_getDouble(f1) - osc_atom_u_getDouble(f2));
		}else{
			osc_atom_u_setInt32(*result, osc_atom_u_getInt32(f1) - osc_atom_u_getInt32(f2));
		}
	}else if(tt1 == OSC_TIMETAG_TYPETAG || tt2 == OSC_TIMETAG_TYPETAG){
		t_osc_timetag timetag1, timetag2;
		if(tt1 == OSC_TIMETAG_TYPETAG && tt2 == OSC_TIMETAG_TYPETAG){
			timetag1 = osc_atom_u_getTimetag(f1);
			timetag2 = osc_atom_u_getTimetag(f2);
			t_osc_timetag res = osc_timetag_subtract(timetag1, timetag2);
			double f = osc_timetag_timetagToFloat(res);
			osc_atom_u_setDouble(*result, f);
			return 0;
		}else if(tt1 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt2)){
			timetag1 = osc_atom_u_getTimetag(f1);
			timetag2 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f2));
		}else if(tt2 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt1)){
			timetag1 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f1));
			timetag2 = osc_atom_u_getTimetag(f2);
		}else{
			return 1;
		}
		osc_atom_u_setTimetag(*result, osc_timetag_subtract(timetag1, timetag2));
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1)){
			osc_expr_err_badInfixArg(context, "-", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2)){
			osc_expr_err_badInfixArg(context, "-", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_multiply(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{

	if(!f1){
		osc_atom_u_copyInto(result, f2);
		return 0;
	}
	if(!f2){
		osc_atom_u_copyInto(result, f1);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		if(OSC_TYPETAG_ISFLOAT(tt1) || OSC_TYPETAG_ISFLOAT(tt2)){
			osc_atom_u_setDouble(*result, osc_atom_u_getDouble(f1) * osc_atom_u_getDouble(f2));
		}else{
			char tt = osc_typetag_compare(tt1, tt2) > 0 ? tt1 : tt2;
			switch(tt){
			case 'i':
				osc_atom_u_setInt32(*result, osc_atom_u_getInt32(f1) * osc_atom_u_getInt32(f2));
				break;
			case 'I':
				osc_atom_u_setUInt32(*result, osc_atom_u_getUInt32(f1) * osc_atom_u_getUInt32(f2));
				break;
			case 'h':
				osc_atom_u_setInt64(*result, osc_atom_u_getInt64(f1) * osc_atom_u_getInt64(f2));
				break;
			case 'H':
				osc_atom_u_setUInt64(*result, osc_atom_u_getUInt64(f1) * osc_atom_u_getUInt64(f2));
				break;
			case 'u':
				osc_atom_u_setInt16(*result, osc_atom_u_getInt16(f1) * osc_atom_u_getInt16(f2));
				break;
			case 'U':
				osc_atom_u_setUInt16(*result, osc_atom_u_getUInt16(f1) * osc_atom_u_getUInt16(f2));
				break;
			case 'c':
				osc_atom_u_setInt8(*result, osc_atom_u_getInt8(f1) * osc_atom_u_getInt8(f2));
				break;
			case 'C':
				osc_atom_u_setUInt8(*result, osc_atom_u_getUInt8(f1) * osc_atom_u_getUInt8(f2));
				break;
			}
		}
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1)){
			osc_expr_err_badInfixArg(context, "*", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2)){
			osc_expr_err_badInfixArg(context, "*", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_divide(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1){
		osc_atom_u_copyInto(result, f2);
		return 0;
	}
	if(!f2){
		osc_atom_u_copyInto(result, f1);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		if(OSC_TYPETAG_ISFLOAT(tt1) || OSC_TYPETAG_ISFLOAT(tt2)){
			double d2 = osc_atom_u_getDouble(f2);
			if(d2 == 0){
				osc_atom_u_setString(*result, "Inf");
			}else{
				osc_atom_u_setDouble(*result, osc_atom_u_getDouble(f1) / d2);
			}
		}else{
			// int32_t i2 = osc_atom_u_getInt32(f2);
			// if(i2 == 0){
			// 	osc_atom_u_setString(*result, "Inf");
			// }else{
			// 	osc_atom_u_setInt32(*result, osc_atom_u_getInt32(f1) / i2);
			// }
			char tt = osc_typetag_compare(tt1, tt2) > 0 ? tt1 : tt2;
			switch(tt){
			case 'i':
				{
					int32_t i2 = osc_atom_u_getInt32(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setInt32(*result, osc_atom_u_getInt32(f1) / i2);
					}
				}
				break;
			case 'I':
				{
					uint32_t i2 = osc_atom_u_getUInt32(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setUInt32(*result, osc_atom_u_getUInt32(f1) / i2);
					}
				}
				break;
			case 'h':
				{
					int64_t i2 = osc_atom_u_getInt64(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setInt64(*result, osc_atom_u_getInt64(f1) / i2);
					}
				}
				break;
			case 'H':
				{
					uint64_t i2 = osc_atom_u_getUInt64(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setUInt64(*result, osc_atom_u_getUInt64(f1) / i2);
					}
				}
				break;
			case 'u':
				{
					int16_t i2 = osc_atom_u_getInt16(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setInt16(*result, osc_atom_u_getInt16(f1) / i2);
					}
				}
				break;
			case 'U':
				{
					uint16_t i2 = osc_atom_u_getUInt16(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setUInt16(*result, osc_atom_u_getUInt16(f1) / i2);
					}
				}
				break;
			case 'c':
				{
					int8_t i2 = osc_atom_u_getInt8(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setInt8(*result, osc_atom_u_getInt8(f1) / i2);
					}
				}
				break;
			case 'C':
				{
					int8_t i2 = osc_atom_u_getInt8(f2);
					if(i2 == 0){
						osc_atom_u_setString(*result, "Inf");
					}else{
						osc_atom_u_setInt8(*result, osc_atom_u_getInt8(f1) / i2);
					}
				}
				break;
			}
		}
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1)){
			osc_expr_err_badInfixArg(context, "/", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2)){
			osc_expr_err_badInfixArg(context, "/", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_compareTimetag(t_osc_atom_u *f1, t_osc_atom_u *f2)
{
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	t_osc_timetag timetag1, timetag2;
	if(tt1 == OSC_TIMETAG_TYPETAG && tt2 == OSC_TIMETAG_TYPETAG){
		timetag1 = osc_atom_u_getTimetag(f1);
		timetag2 = osc_atom_u_getTimetag(f2);
	}else if(tt1 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt2)){
		timetag1 = osc_atom_u_getTimetag(f1);
		timetag2 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f2));
	}else if(tt2 == OSC_TIMETAG_TYPETAG && OSC_TYPETAG_ISNUMERIC(tt1)){
		timetag1 = osc_timetag_floatToTimetag(osc_atom_u_getDouble(f1));
		timetag2 = osc_atom_u_getTimetag(f2);
	}else{
		return 0;
	}
	return osc_timetag_compare(timetag1, timetag2);
}

int osc_expr_lt(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret < 0);
	}else if((tt1 == 's') && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setFalse(*result);
	}else if((tt2 == 's') && OSC_TYPETAG_ISNUMERIC(tt1)){
		osc_atom_u_setTrue(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) < osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) == -1);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, "<", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, "<", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_le(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret <= 0);
	}else if((tt1 == 's') && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setFalse(*result);
	}else if((tt2 == 's') && OSC_TYPETAG_ISNUMERIC(tt1)){
		osc_atom_u_setTrue(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) <= osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) != 1);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, "<=", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, "<=", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_gt(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret > 0);
	}else if((tt1 == 's') && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setTrue(*result);
	}else if((tt2 == 's') && OSC_TYPETAG_ISNUMERIC(tt1)){
		osc_atom_u_setFalse(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) > osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) == 1);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, ">", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, ">", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_gte(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret >= 0);
	}else if((tt1 == 's') && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setTrue(*result);
	}else if((tt2 == 's') && OSC_TYPETAG_ISNUMERIC(tt1)){
		osc_atom_u_setFalse(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) >= osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) != -1);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, ">=", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, ">=", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_eq(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret == 0);
		//}else if((tt1 == 's') || (tt2 == 's')){
		//osc_atom_u_setFalse(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) == osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) == 0);
	}else if(tt1 == 'N' && tt2 == 'N'){
		osc_atom_u_setBool(*result, 1);
	}else if(tt1 == 'N' || tt2 == 'N'){
		osc_atom_u_setBool(*result, 0);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, "==", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, "==", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_neq(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	if(tt1 == 's' && tt2 == 's'){
		int ret = strcmp(osc_atom_u_getStringPtr(f1), osc_atom_u_getStringPtr(f2));
		osc_atom_u_setBool(*result, ret != 0);
		//}else if((tt1 == 's') || (tt2 == 's')){
		//osc_atom_u_setTrue(*result);
	}else if(OSC_TYPETAG_ISNUMERIC(tt1) && OSC_TYPETAG_ISNUMERIC(tt2)){
		osc_atom_u_setBool(*result, osc_atom_u_getDouble(f1) != osc_atom_u_getDouble(f2));
	}else if((tt1 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt1)) &&
		 (tt2 == OSC_TIMETAG_TYPETAG || OSC_TYPETAG_ISNUMERIC(tt2))){
		osc_atom_u_setBool(*result, osc_expr_compareTimetag(f1, f2) != 0);
	}else{
		if(!OSC_TYPETAG_ISNUMERIC(tt1) && !OSC_TYPETAG_ISSTRING(tt1)){
			osc_expr_err_badInfixArg(context, "!=", tt1, 1, f1, f2);
		}else if(!OSC_TYPETAG_ISNUMERIC(tt2) && !OSC_TYPETAG_ISSTRING(tt2)){
			osc_expr_err_badInfixArg(context, "!=", tt2, 2, f1, f2);
		}
		return 1;
	}
	return 0;
}

int osc_expr_and(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1 || !f2){
		osc_atom_u_setFalse(*result);
		return 0;
	}
	osc_atom_u_setBool(*result, osc_atom_u_getBool(f1) && osc_atom_u_getBool(f2));
	return 0;
}

int osc_expr_or(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	int ff1 = 0;
	int ff2 = 0;
	if(f1){
		ff1 = osc_atom_u_getBool(f1);
	}
	if(f2){
		ff2 = osc_atom_u_getBool(f2);
	}
	osc_atom_u_setBool(*result, ff1 || ff2);
	return 0;
}

int osc_expr_andalso(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){return 0;}
int osc_expr_orelse(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){return 0;}

static int osc_expr_specFunc_andalso(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out,
				 	 void* context)
{
	long argc = osc_expr_getArgCount(f);
	if(argc != 2){
		osc_expr_err_argnum(context, 2, argc, 0, "andalso");
		return 1;
	}
	t_osc_atom_ar_u *lhs = NULL, *rhs = NULL;
	int ret = osc_expr_evalArgInLexEnv(f->argv, lexenv, len, oscbndl, &lhs, context);
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

	ret = osc_expr_evalArgInLexEnv(f->argv->next, lexenv, len, oscbndl, &rhs, context);
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

static int osc_expr_specFunc_orelse(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out,
					 void* context)
{
	long argc = osc_expr_getArgCount(f);
	if(argc != 2){
		osc_expr_err_argnum(context, 2, argc, 0, "orelse");
		return 1;
	}
	t_osc_atom_ar_u *lhs = NULL, *rhs = NULL;
	int ret = osc_expr_evalArgInLexEnv(f->argv, lexenv, len, oscbndl, &lhs, context);
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

	ret = osc_expr_evalArgInLexEnv(f->argv->next, lexenv, len, oscbndl, &rhs, context);
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

static int osc_expr_specFunc_bundle(t_osc_expr *f,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out,
				    void* context)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
	while(f_argv){
		if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
			t_osc_msg_u *m = osc_message_u_allocWithAddress(osc_expr_arg_getOSCAddress(f_argv));
			f_argv = osc_expr_arg_next(f_argv);
			if(*out){
				osc_atom_array_u_free(*out);
				*out = NULL;
			}
			t_osc_err err = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, out, context);
			if(err){
				osc_message_u_free(m);
				osc_bundle_u_free(b);
				return err;
			}
			if(*out){
				int n = osc_atom_array_u_getLen(*out);
				for(int i = 0; i < n; i++){
					osc_message_u_appendAtom(m, osc_atom_u_copy(osc_atom_array_u_get(*out, i)));
				}
			}
			osc_bundle_u_addMsg(b, m);
		}else{
			if(*out){
				osc_atom_array_u_free(*out);
				*out = NULL;
			}
			osc_bundle_u_free(b);
			return 1;
		}
		f_argv = osc_expr_arg_next(f_argv);
	}
	if(*out){
		osc_atom_array_u_free(*out);
		*out = NULL;
	}
	*out = osc_atom_array_u_alloc(1);
	t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
	osc_atom_u_setBndl_u(a, b);
	return 0;
}

double _osc_expr_fmod(double x, double m)
{
	return x - m * _osc_expr_sign(m) * floor(x / fabs(m));
}

int osc_expr_mod(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result, void* context)
{
	if(!f1){
		osc_atom_u_setInt32(*result, 0);
		return 0;
	}
	if(!f2){
		osc_atom_u_copyInto(result, f1);
		return 0;
	}
	/*
	  char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	  if(tt1 == 'f' || tt1 == 'd' || tt2 == 'f' || tt2 == 'd'){
	  double ff1 = osc_atom_u_getDouble(f1), ff2 = osc_atom_u_getDouble(f2);
	  if(ff2 == 0){
	  osc_atom_u_copyInto(result, f1);
	  return 0;
	  }
	  osc_atom_u_setDouble(*result, fmod(ff1, ff2));
	  }else{
	  int32_t ff1 = osc_atom_u_getInt32(f1), ff2 = osc_atom_u_getInt32(f2);
	  if(ff2 == 0){
	  osc_atom_u_copyInto(result, f1);
	  return 0;
	  }
	  osc_atom_u_setInt32(*result, ff1 % ff2);
	  }
	*/
	char tt1 = osc_atom_u_getTypetag(f1), tt2 = osc_atom_u_getTypetag(f2);
	double ff1 = osc_atom_u_getDouble(f1), ff2 = osc_atom_u_getDouble(f2);
	double m = _osc_expr_fmod(ff1, ff2);
	if(tt1 == 'f' || tt1 == 'd' || tt2 == 'f' || tt2 == 'd'){
		if(ff2 == 0){
			osc_atom_u_copyInto(result, f1);
			return 0;
		}
		osc_atom_u_setDouble(*result, m);
	}else{
		if(ff2 == 0){
			osc_atom_u_copyInto(result, f1);
			return 0;
		}
		osc_atom_u_setInt32(*result, (int32_t)m);
	}
	return 0;
}

int osc_expr_assign(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	/*
	int len = 0;
	int i;
	for(i = 1; i < argc; i++){
		len += osc_atom_array_u_getLen(argv[i]);
	}
	*out = osc_atom_array_u_alloc(len);

	t_osc_atom_ar_u *ar = *out;
	len = 0;
	for(i = 1; i < argc; i++){
		osc_atom_array_u_copyInto(&ar, argv[i], len);
		len += osc_atom_array_u_getLen(argv[i]);
	}
	*/
	return 0;
}

int osc_expr_plus1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	int i;
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	for(i = 0; i < len; i++){
		t_osc_atom_u *a = osc_atom_array_u_get(*argv, i);
		char tt = osc_atom_u_getTypetag(a);
		switch(tt){
		case 'i':
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), osc_atom_u_getInt32(a) + 1);
			break;
		case 'f':
			osc_atom_u_setFloat(osc_atom_array_u_get(*out, i), osc_atom_u_getFloat(a) + 1);
			break;
		case 'd':
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i), osc_atom_u_getDouble(a) + 1);
			break;
		case 'h':
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, i), osc_atom_u_getUInt32(a) + 1);
			break;
		case 'I':
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, i), osc_atom_u_getInt64(a) + 1);
			break;
		case 'H':
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, i), osc_atom_u_getUInt64(a) + 1);
			break;
		}
	}
	return 0;
}

int osc_expr_minus1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int i;
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	for(i = 0; i < len; i++){
		t_osc_atom_u *a = osc_atom_array_u_get(*argv, i);
		char tt = osc_atom_u_getTypetag(a);
		switch(tt){
		case 'i':
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), osc_atom_u_getInt32(a) - 1);
			break;
		case 'f':
			osc_atom_u_setFloat(osc_atom_array_u_get(*out, i), osc_atom_u_getFloat(a) - 1);
			break;
		case 'd':
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i), osc_atom_u_getDouble(a) - 1);
			break;
		case 'h':
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, i), osc_atom_u_getUInt32(a) - 1);
			break;
		case 'I':
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, i), osc_atom_u_getInt64(a) - 1);
			break;
		case 'H':
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, i), osc_atom_u_getUInt64(a) - 1);
			break;
		}
	}
	return 0;
}

int osc_expr_ilogb(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 1){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "ilogb expects a single argument (may be a list)");
		return 1;
	}
	int n = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), ilogb(osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i))));
	}
	return 0;
}

int osc_expr_jn(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 2){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "jn expects two arguments: n (int), x (float)");
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	int n = osc_atom_u_getInt32(osc_atom_array_u_get(*argv, 0));
	double x = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), jn(n, x));
	return 0;
}

int osc_expr_yn(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 2){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "jn expects two arguments: n (int), x (float)");
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	int n = osc_atom_u_getInt32(osc_atom_array_u_get(*argv, 0));
	double x = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), yn(n, x));
	return 0;
}

int osc_expr_nth(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
        int i, j, k = 0;
        int argc_out = 0;
        for(i = 1; i < argc; i++){
                argc_out += osc_atom_array_u_getLen(argv[i]);
        }
        *out = osc_atom_array_u_alloc(argc_out);

	long argv0len = osc_atom_array_u_getLen(argv[0]);
        if(argv0len == 1 && osc_atom_u_getTypetag(osc_atom_array_u_get(argv[0], 0)) == 'b'){
                t_osc_atom_u *a = osc_atom_array_u_get(argv[0], 0);
                char *blob = osc_atom_u_getBlob(a);
                int32_t nbytes = ntoh32(*((int32_t *)blob));
		blob += 4;
                for(j = 1; j < argc; j++){
                        for(i = 0; i < osc_atom_array_u_getLen(argv[j]); i++){
                                osc_atom_u_setInt8(osc_atom_array_u_get(*out, k), 0.);
                                int32_t l = osc_atom_u_getInt32(osc_atom_array_u_get(argv[j], i));
				if(l < 0){
                                        osc_atom_array_u_free(*out);
                                        *out = NULL;
                                        osc_error(context, OSC_ERR_EXPR_EVAL, "index %d is negative", l);
                                        return 1;
				}
                                if(l > nbytes - 1){
                                        osc_atom_array_u_free(*out);
                                        *out = NULL;
                                        osc_error(context, OSC_ERR_EXPR_EVAL, "index %d exceeds array length %d", l, nbytes);
                                        return 1;
                                }
                                osc_atom_u_setInt8(osc_atom_array_u_get(*out, k), blob[l]);
                                k++;
                        }
                }
        }else{
                for(j = 1; j < argc; j++){
                        for(i = 0; i < osc_atom_array_u_getLen(argv[j]); i++){
                                osc_atom_u_setDouble(osc_atom_array_u_get(*out, k), 0.);
                                int32_t l = osc_atom_u_getInt32(osc_atom_array_u_get(argv[j], i));
				if(l < 0){
                                        osc_atom_array_u_free(*out);
                                        *out = NULL;
                                        osc_error(context, OSC_ERR_EXPR_EVAL, "index %d is negative", l);
                                        return 1;
				}
                                if(l > argv0len - 1){
                                        osc_atom_array_u_free(*out);
                                        *out = NULL;
                                        osc_error(context, OSC_ERR_EXPR_EVAL, "index %d exceeds array length %d", l, argv0len);
                                        return 1;
                                }
                                t_osc_atom_u *r = osc_atom_array_u_get(*out, k);
                                osc_atom_u_copyInto(&r, osc_atom_array_u_get(argv[0], l));
                                k++;
                        }
                }
        }
        return 0;
}

int osc_expr_assign_to_index(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 3){
		return 1;
	}
	*out = osc_atom_array_u_copy(*argv);
	int outlen = osc_atom_array_u_getLen(*out);
	int nindexes = osc_atom_array_u_getLen(argv[1]);
	int ndata = osc_atom_array_u_getLen(argv[2]);
	if(nindexes == 0 || ndata == 0){
		return 1;
	}else if(nindexes == 1 && ndata > 1){
		int idx = osc_atom_u_getInt(osc_atom_array_u_get(argv[1], 0));
		if(idx >= outlen || idx < 0){
			// error!
			return 1;
		}
		t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
		osc_atom_u_copyInto(&dest, osc_atom_array_u_get(argv[2], 0));
	}else if(nindexes > 1 && ndata == 1){
		int i, idx;
		t_osc_atom_u *a = osc_atom_array_u_get(argv[2], 0);
		for(i = 0; i < nindexes; i++){
			idx = osc_atom_u_getInt(osc_atom_array_u_get(argv[1], i));
			if(idx >= outlen || idx < 0){
				// error!
				continue;
			}
			t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
			osc_atom_u_copyInto(&dest, a);
		}
	}else{
		int i, idx;
		int n = osc_atom_array_u_getLen(argv[1]);
		if(osc_atom_array_u_getLen(argv[2]) < n){
			n = osc_atom_array_u_getLen(argv[2]);
		}
		for(i = 0; i < n; i++){
			idx = osc_atom_u_getInt(osc_atom_array_u_get(argv[1], i));
			if(idx >= outlen || idx < 0){
				// error!
				continue;
			}
			t_osc_atom_u *dest = osc_atom_array_u_get(*out, idx);
			osc_atom_u_copyInto(&dest, osc_atom_array_u_get(argv[2], i));
		}
	}
	return 0;
}

int osc_expr_product(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	double val = 1;
	int doubleup = 0;
	int j;
	for(j = 0; j < argc; j++){
		long len = osc_atom_array_u_getLen(argv[j]);
		if(len == 0){
			//osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 0.);
			//return 0;
			continue;
		}
		int i;
		for(i = 0; i < len; i++){
			val *= osc_atom_u_getDouble(osc_atom_array_u_get(argv[j], i));
		}
	}
	if(doubleup){
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), val);
	}else{
		if(val <= 0x7fffffff){
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= ((uint32_t)0xffffffff)){
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= 0x7fffffffffffffff){
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= 0xffffffffffffffff){
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 0), val);
		}else{
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), val);
		}
	}
	return 0;
}

int osc_expr_sum(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	double val = 0;
	int doubleup = 0;
	int j;
	for(j = 0; j < argc; j++){
		long len = osc_atom_array_u_getLen(argv[j]);
		if(len == 0){
			//osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 0.);
			//return 0;
			continue;
		}
		int i;
		for(i = 0; i < len; i++){
			char tt = osc_atom_u_getTypetag(osc_atom_array_u_get(argv[j], i));
			if(OSC_TYPETAG_ISFLOAT(tt)){
				doubleup = 1;
			}
			val += osc_atom_u_getDouble(osc_atom_array_u_get(argv[j], i));
		}
	}
	if(doubleup){
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), val);
	}else{
		if(val <= 0x7fffffff){
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= ((uint32_t)0xffffffff)){
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= 0x7fffffffffffffff){
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, 0), val);
		}else if(val <= 0xffffffffffffffff){
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 0), val);
		}else{
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), val);
		}
	}
	return 0;
}

int osc_expr_cumsum(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
#define _i8 0x1
#define _u8 0x2
#define _i16 0x4
#define _u16 0x8
#define _i32 0x10
#define _u32 0x20
#define _i64 0x40
#define _u64 0x80
#define _f32 0x100
#define _f64 0x200
	uint32_t largest_type = 0;

	int n = 0;
	for(int i = 0; i < argc; i++){
		n += osc_atom_array_u_getLen(argv[i]);
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			switch(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[i], j))){
			case 'c':
				if(_i8 > largest_type){
					largest_type = _i8;
				}
				break;
			case 'C':
				if(_u8 > largest_type){
					largest_type = _u8;
				}
				break;
			case 'u':
				if(_i16 > largest_type){
					largest_type = _i16;
				}
				break;
			case 'U':
				if(_u16 > largest_type){
					largest_type = _u16;
				}
				break;
			default:
			case 'i':
				if(_i32 > largest_type){
					largest_type = _i32;
				}
				break;
			case 'I':
				if(_u32 > largest_type){
					largest_type = _u32;
				}
				break;
			case 'h':
				if(_i64 > largest_type){
					largest_type = _i64;
				}
				break;
			case 'H':
				if(_u64 > largest_type){
					largest_type = _u64;
				}
				break;
			case 'f':
				if(_f32 > largest_type){
					largest_type = _f32;
				}
				break;
			case 'd':
				if(_f64 > largest_type){
					largest_type = _f64;
				}
				break;
			}
		}
	}
	*out = osc_atom_array_u_alloc(n);

	switch(largest_type){
	case _i8:
		{
			int k = 0;
			char cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getInt8(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setInt8(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _u8:
		{
			int k = 0;
			unsigned char cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getUInt8(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setUInt8(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _i16:
		{
			int k = 0;
			int16_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getInt16(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setInt16(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _u16:
		{
			int k = 0;
			uint16_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getUInt16(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setUInt16(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	default:
	case _i32:
		{
			int k = 0;
			int32_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getInt32(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setInt32(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _u32:
		{
			int k = 0;
			uint32_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getUInt32(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setUInt32(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _i64:
		{
			int k = 0;
			int64_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getInt64(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setInt64(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _u64:
		{
			int k = 0;
			uint64_t cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getUInt64(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setUInt64(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _f32:
		{
			int k = 0;
			float cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getFloat(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setFloat(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	case _f64:
		{
			int k = 0;
			double cumsum = 0;
			for(int i = 0; i < argc; i++){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
					cumsum += osc_atom_u_getDouble(osc_atom_array_u_get(argv[i], j));
					osc_atom_u_setDouble(osc_atom_array_u_get(*out, k++), cumsum);
				}
			}
		}
		break;
	}
	/*
	for(i = 0; i < argc; i++){
		int j;
		for(j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			val += osc_atom_u_getDouble(osc_atom_array_u_get(argv[i], j));
			lst[k++] = val;
			//osc_atom_u_setDouble(osc_atom_array_u_get(*out, k++), val);
		}
	}
	if(doubleup){
		for(i = 0; i < n; i++){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i), lst[i]);
		}
	}else{
		if(val <= 0x7fffffff){
			for(i = 0; i < n; i++){
				osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), lst[i]);
			}
		}else if(val <= ((uint32_t)0xffffffff)){
			for(i = 0; i < n; i++){
				osc_atom_u_setUInt32(osc_atom_array_u_get(*out, i), lst[i]);
			}
		}else if(val <= 0x7fffffffffffffff){
			for(i = 0; i < n; i++){
				osc_atom_u_setInt64(osc_atom_array_u_get(*out, i), lst[i]);
			}
		}else if(val <= 0xffffffffffffffff){
			for(i = 0; i < n; i++){
				osc_atom_u_setUInt64(osc_atom_array_u_get(*out, i), lst[i]);
			}
		}else{
			for(i = 0; i < n; i++){
				osc_atom_u_setDouble(osc_atom_array_u_get(*out, i), lst[i]);
			}
		}
	}
	*/
	return 0;
}

int osc_expr_length(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	int i;
	int count = 0;
	for(i = 0; i < argc; i++){
		count += osc_atom_array_u_getLen(argv[i]);
	}
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), count);
	return 0;
}

int osc_expr_mean(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	double sum = 0;
	int i;
	long len = osc_atom_array_u_getLen(*argv);
	for(i = 0; i < len; i++){
		sum += osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
	}
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), sum / len);
	return 0;
}

// this implementation of the median is not great--it just sorts
// the list and takes the median.  this could be optimized by
// implementing a median of medians algorithm or something
int comp(const void *val1, const void *val2){
	double v1 = *((double *)val1);
	double v2 = *((double *)val2);
	if(v1 < v2){
		return -1;
	}
	if(v1 == v2){
		return 0;
	}
	return 1;
}

int osc_expr_median(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	long len = osc_atom_array_u_getLen(*argv);
	double *tmp = NULL;
	osc_atom_array_u_getDoubleArray(*argv, &tmp);
	qsort((void *)(tmp), len, sizeof(double), comp);
	if((len % 2) == 0){
		//double left = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, (int)((len - 1) / 2.)));
		//double right = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, ((int)((len - 1) / 2.) + 1)));
		double left = tmp[(int)((len - 1) / 2.)];
		double right = tmp[(int)((len - 1) / 2.) + 1];
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), (left + right) / 2.);
	}else{
		//osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), osc_atom_u_getDouble(osc_atom_array_u_get(*argv, ((len - 1) / 2))));
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), tmp[((len - 1) / 2)]);
	}
	if(tmp){
		osc_mem_free(tmp);
	}
	return 0;
}

int osc_expr_reverse(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	int i;
	for(i = 0; i < len; i++){
		t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
		osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, len - i - 1));
	}
	return 0;
}

static int osc_expr_qsortcb(const void *a1, const void *a2)
{
	t_osc_atom_u *aa1 = (t_osc_atom_u *)a1;
	t_osc_atom_u *aa2 = (t_osc_atom_u *)a2;
	char t1 = osc_atom_u_getTypetag(aa1);
	char t2 = osc_atom_u_getTypetag(aa2);
	if(t1 == 's' && t2 == 's'){
		return strcmp(osc_atom_u_getStringPtr(aa1), osc_atom_u_getStringPtr(aa2));
	}else if(t1 == 's'){
		return 1;
	}else if(t2 == 's'){
		return -1;
	}else{
		double d1 = osc_atom_u_getDouble(aa1);
		double d2 = osc_atom_u_getDouble(aa2);
		if(d1 < d2){
			return -1;
		}else{
			return 1;
		}
	}
	return 0;
}

int osc_expr_sort(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int n = 0;
	for(int i = 0; i < argc; i++){
		n += osc_atom_array_u_getLen(argv[i]);
	}
	t_osc_atom_u *ar = (t_osc_atom_u *)osc_mem_alloc(n * osc_atom_u_getStructSize());
	n = 0;
	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			t_osc_atom_u *a = (t_osc_atom_u *)(((char *)ar) + (n++ * osc_atom_u_getStructSize()));
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(argv[i], j));
		}
	}
	qsort((void *)ar, n, osc_atom_u_getStructSize(), osc_expr_qsortcb);
	*out = osc_atom_array_u_alloc(0);
	osc_atom_array_u_set(*out, (void *)ar, n);
	return 0;
}


typedef struct _sortIndexStruct {
    t_osc_atom_u *a;
    int32_t index;
} t_sortIndexStruct;

static int osc_expr_qsortIndexcb(const void *a1, const void *a2)
{
    
    t_osc_atom_u *aa1 = ((t_sortIndexStruct *)a1)->a;
    t_osc_atom_u *aa2 = ((t_sortIndexStruct *)a2)->a;
    char t1 = osc_atom_u_getTypetag(aa1);
    char t2 = osc_atom_u_getTypetag(aa2);
    if(t1 == 's' && t2 == 's'){
        return strcmp(osc_atom_u_getStringPtr(aa1), osc_atom_u_getStringPtr(aa2));
    }else if(t1 == 's'){
        return 1;
    }else if(t2 == 's'){
        return -1;
    }else{
        double d1 = osc_atom_u_getDouble(aa1);
        double d2 = osc_atom_u_getDouble(aa2);
        return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
    }
    return 0;
}

int osc_expr_sortidx(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
/*
    if( argc > 1 )
    {
        osc_expr_err_argnum(1, argc, 1, "osc_expr: sortIndex()");
        return 1;
    }
*/
    long n = osc_atom_array_u_getLen(argv[0]);
    
    *out = osc_atom_array_u_alloc(n);
    t_sortIndexStruct sort_ar[n];
    
    for(int i = 0; i < n; i++)
    {
        sort_ar[i].index = i;
        sort_ar[i].a = osc_atom_u_alloc();
        osc_atom_u_copyInto(&sort_ar[i].a, osc_atom_array_u_get(argv[0], i));
    }

    
    qsort((void *)sort_ar, n, sizeof(t_sortIndexStruct), osc_expr_qsortIndexcb);
    
    for( int i = 0; i < n; i++ )
    {
        osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), sort_ar[i].index);
        osc_mem_free(sort_ar[i].a);
    }
    
    return 0;
}


int osc_expr_list(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int outlen = 0;
	int i;
	for(i = 0; i < argc; i++){
		outlen += osc_atom_array_u_getLen(argv[i]);
	}
	if(outlen <= 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(outlen);
	int pos = 0;
	for(i = 0; i < argc; i++){
		osc_atom_array_u_copyInto(out, argv[i], pos);
		pos += osc_atom_array_u_getLen(argv[i]);
	}
	return 0;
}

int osc_expr_nfill(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
    if(argc < 2){
        osc_expr_err_argnum(context, 2, argc, 1, "osc_expr: nfill()");
        return 1;
    }
	int n = osc_atom_u_getInt(osc_atom_array_u_get(*argv, 0));
	if(n < 0){
		n = 0;
	}
	//t_osc_atom_u *val = NULL;

	int outlen = n * osc_atom_array_u_getLen(argv[1]);
	*out = osc_atom_array_u_alloc(outlen);
	int j = 0;
	for(int i = 0; i < n; i++){
		osc_atom_array_u_copyInto(out, argv[1], j);
		j += osc_atom_array_u_getLen(argv[1]);
	}
	/*
	int alloc = 0;
	if(argc == 2){
		val = osc_atom_array_u_get(argv[1], 0);
	}
	if(!val){
		alloc = 1;
		val = osc_atom_u_alloc();
		osc_atom_u_setDouble(val, 0.);
	}
	int i;
	for(i = 0; i < n; i++){
		t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
		osc_atom_u_copyInto(&a, val);
 	}
	if(alloc){
		osc_atom_u_free(val);
	}
	*/
	return 0;
}

int osc_expr_aseq(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc < 2){
		osc_expr_err_argnum(context, 2, argc, 1, "osc_expr: aseq()");
		return 1;
	}
	int dblup = 0;
	if(osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, 0)) == 'f' ||
	   osc_atom_u_getTypetag(osc_atom_array_u_get(argv[1], 0)) == 'f' ||
	   osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, 0)) == 'd' ||
	   osc_atom_u_getTypetag(osc_atom_array_u_get(argv[1], 0)) == 'd'){
		dblup = 1;
	}
	if(dblup == 0 && argc > 2){
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[2], 0)) == 'f' ||
		   osc_atom_u_getTypetag(osc_atom_array_u_get(argv[2], 0)) == 'd'){
			dblup = 1;
		}
	}
	double start, end;
 	start = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, 0));
 	end = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
	if(start == end){
		*out = osc_atom_array_u_alloc(1);
		t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
		osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, 0));
		return 0;
	}
 	double step = _osc_expr_sign(end - start);
 	if(argc > 2){
	 	step = osc_atom_u_getDouble(osc_atom_array_u_get(argv[2], 0));
		char tt = osc_atom_u_getTypetag(osc_atom_array_u_get(argv[2], 0));
		if(tt == 'f' || tt == 'd'){
			dblup = 1;
		}
 	}
 	if(step == 0){
		osc_error(context, OSC_ERR_EXPR_ARGCHK,
			  "aseq: step must not be 0");
	 	return 1;
 	}
 	if(_osc_expr_sign(end - start) != _osc_expr_sign(step)){
		osc_error(context, OSC_ERR_EXPR_ARGCHK,
			  _osc_expr_sign(end - start) == 1 ?
			  "aseq: end > start, but step is negative!\n" :
			  "aseq: end < start, but step is positive!\n");
	 	return 1;
 	}
	double min = start, max = end, abs_step = fabs(step);
	if(min > max){
		double tmp = min;
		min = max;
		max = tmp;
	}
	int i = 0;
	int n = (int)(((max - min) / abs_step)) + 1;
	*out = osc_atom_array_u_alloc(n);

	if(dblup){
		for(i = 0; i < n; i++){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, i), start);
			start += step;
		}
	}else{
		for(i = 0; i < n; i++){
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), start);
			start += step;
		}
	}
	return 0;
}

int osc_expr_first(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		if(osc_atom_array_u_getLen(*argv)){
			*out = osc_atom_array_u_alloc(1);
			t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, 0));
		}
	}
	return 0;
}

int osc_expr_last(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		int len = osc_atom_array_u_getLen(*argv);
		if(len){
			*out = osc_atom_array_u_alloc(1);
			t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, len - 1));
		}
	}
	return 0;
}

int osc_expr_rest(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		int len = osc_atom_array_u_getLen(*argv);
		if(len){
			*out = osc_atom_array_u_alloc(len - 1);
			osc_atom_array_u_clear(*out);
			for(int i = 1; i < len; i++){
				t_osc_atom_u *a = osc_atom_array_u_get(*out, i - 1);
				osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			}
		}
	}
	return 0;
}

int osc_expr_butlast(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		int len = osc_atom_array_u_getLen(*argv);
		if(len){
			*out = osc_atom_array_u_alloc(len - 1);
			osc_atom_array_u_clear(*out);
			for(int i = 0; i < len - 1; i++){
				t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
				osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			}
		}
	}
	return 0;
}

int osc_expr_interleave(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int min = osc_atom_array_u_getLen(*argv);
	int i, j;
	for(i = 1; i < argc; i++){
		int val = osc_atom_array_u_getLen(argv[i]);
		if(val < min){
			min = val;
		}
	}
	*out = osc_atom_array_u_alloc(argc * min);

	for(i = 0; i < argc; i++){
		for(j = 0; j < min; j++){
			t_osc_atom_u *a = osc_atom_array_u_get(*out, i + (j * argc));
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(argv[i], j));
		}
	}
	return 0;
}

int osc_expr_not(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int i;
	*out = osc_atom_array_u_alloc(osc_atom_array_u_getLen(*argv));

	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		switch(osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i))){
			// lazy...
		case 'i':
		case 'I':
		case 'h':
		case 'H':
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), !(osc_atom_u_getInt32(osc_atom_array_u_get(*argv, i))));
			break;
		case 'f':
		case 'd':
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), !(osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i))));
			break;
		case 's':
		case 'b':
		case OSC_BUNDLE_TYPETAG:
			{
				t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
				osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			}
			break;
		case 'T':
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, i));
			break;
		case 'F':
		case 'N':
			osc_atom_u_setTrue(osc_atom_array_u_get(*out, i));
			break;
		default:
			osc_atom_u_setFalse(osc_atom_array_u_get(*out, i));
		}
	}
	return 0;
}

int osc_expr_dot(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 2){
		return 0;
	}
	*out = osc_atom_array_u_alloc(1);

	double s = 0;
	int i;
	long len0 = osc_atom_array_u_getLen(*argv);
	long len1 = osc_atom_array_u_getLen(argv[1]);
	// this should work on ints too
	for(i = 0; i < (len0 < len1 ? len0 : len1); i++){
		double f1 = 0, f2 = 0;
		if(i < len0){
			f1 = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		}
		if(i < len1){
			f2 = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i));
		}
		s += f1 * f2;
	}
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), s);
	return 0;
}

int osc_expr_cross_int8(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int8_t a = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 0));
	int8_t b = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 1));
	int8_t c = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 2));
	int8_t x = osc_atom_u_getInt8(osc_atom_array_u_get(argv[1], 0));
	int8_t y = osc_atom_u_getInt8(osc_atom_array_u_get(argv[1], 1));
	int8_t z = osc_atom_u_getInt8(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setInt8(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setInt8(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setInt8(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_uint8(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	uint8_t a = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[0], 0));
	uint8_t b = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[0], 1));
	uint8_t c = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[0], 2));
	uint8_t x = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[1], 0));
	uint8_t y = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[1], 1));
	uint8_t z = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setUInt8(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setUInt8(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setUInt8(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_int16(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int16_t a = osc_atom_u_getInt16(osc_atom_array_u_get(argv[0], 0));
	int16_t b = osc_atom_u_getInt16(osc_atom_array_u_get(argv[0], 1));
	int16_t c = osc_atom_u_getInt16(osc_atom_array_u_get(argv[0], 2));
	int16_t x = osc_atom_u_getInt16(osc_atom_array_u_get(argv[1], 0));
	int16_t y = osc_atom_u_getInt16(osc_atom_array_u_get(argv[1], 1));
	int16_t z = osc_atom_u_getInt16(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setInt16(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setInt16(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setInt16(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_uint16(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	uint16_t a = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[0], 0));
	uint16_t b = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[0], 1));
	uint16_t c = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[0], 2));
	uint16_t x = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[1], 0));
	uint16_t y = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[1], 1));
	uint16_t z = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setUInt16(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setUInt16(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setUInt16(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_int32(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int32_t a = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], 0));
	int32_t b = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], 1));
	int32_t c = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], 2));
	int32_t x = osc_atom_u_getInt32(osc_atom_array_u_get(argv[1], 0));
	int32_t y = osc_atom_u_getInt32(osc_atom_array_u_get(argv[1], 1));
	int32_t z = osc_atom_u_getInt32(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_uint32(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	uint32_t a = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[0], 0));
	uint32_t b = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[0], 1));
	uint32_t c = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[0], 2));
	uint32_t x = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[1], 0));
	uint32_t y = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[1], 1));
	uint32_t z = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_int64(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	int64_t a = osc_atom_u_getInt64(osc_atom_array_u_get(argv[0], 0));
	int64_t b = osc_atom_u_getInt64(osc_atom_array_u_get(argv[0], 1));
	int64_t c = osc_atom_u_getInt64(osc_atom_array_u_get(argv[0], 2));
	int64_t x = osc_atom_u_getInt64(osc_atom_array_u_get(argv[1], 0));
	int64_t y = osc_atom_u_getInt64(osc_atom_array_u_get(argv[1], 1));
	int64_t z = osc_atom_u_getInt64(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setInt64(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setInt64(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setInt64(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_uint64(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	uint64_t a = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[0], 0));
	uint64_t b = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[0], 1));
	uint64_t c = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[0], 2));
	uint64_t x = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[1], 0));
	uint64_t y = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[1], 1));
	uint64_t z = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_float32(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	float a = osc_atom_u_getFloat(osc_atom_array_u_get(argv[0], 0));
	float b = osc_atom_u_getFloat(osc_atom_array_u_get(argv[0], 1));
	float c = osc_atom_u_getFloat(osc_atom_array_u_get(argv[0], 2));
	float x = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 0));
	float y = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 1));
	float z = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross_float64(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	double a = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 0));
	double b = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 1));
	double c = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 2));
	double x = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
	double y = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 1));
	double z = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 2));
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), b * z - c * y);
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 1), c * x - a * z);
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 2), a * y - b * x);
	return 0;
}

int osc_expr_cross(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 2){
		return 1;
	}
	if(osc_atom_array_u_getLen(*argv) != 3 || osc_atom_array_u_getLen(argv[1]) != 3){
		return 1;
	}
	*out = osc_atom_array_u_alloc(3);
	// f->rec->vtab[osc_typetag_getLargestType(argc, argv)](f, argc, argv, out);
	switch(osc_typetag_getLargestType(argc, argv)){
	case 'c':
		return osc_expr_cross_int8(f, argc, argv, out, context);
	case 'C':
		return osc_expr_cross_uint8(f, argc, argv, out, context);
	case 'u':
		return osc_expr_cross_int16(f, argc, argv, out, context);
	case 'U':
		return osc_expr_cross_uint16(f, argc, argv, out, context);
	case 'i':
		return osc_expr_cross_int32(f, argc, argv, out, context);
	case 'I':
		return osc_expr_cross_uint32(f, argc, argv, out, context);
	case 'h':
		return osc_expr_cross_int64(f, argc, argv, out, context);
	case 'H':
		return osc_expr_cross_uint64(f, argc, argv, out, context);
	case 'f':
		return osc_expr_cross_float32(f, argc, argv, out, context);
	case 'd':
		return osc_expr_cross_float64(f, argc, argv, out, context);
	default:
		// type error
		return 1;
	}
}

// https://en.wikipedia.org/wiki/Determinant
int LUPDecompose(double **A, int N, double Tol, int *P) {

	int i, j, k, imax;
	double maxA, *ptr, absA;

	for (i = 0; i <= N; i++)
		P[i] = i; //Unit permutation matrix, P[N] initialized with N

	for (i = 0; i < N; i++) {
		maxA = 0.0;
		imax = i;

		for (k = i; k < N; k++)
			if ((absA = fabs(A[k][i])) > maxA) {
				maxA = absA;
				imax = k;
			}

		if (maxA < Tol) return 0; //failure, matrix is degenerate

		if (imax != i) {
			//pivoting P
			j = P[i];
			P[i] = P[imax];
			P[imax] = j;

			//pivoting rows of A
			ptr = A[i];
			A[i] = A[imax];
			A[imax] = ptr;

			//counting pivots starting from N (for determinant)
			P[N]++;
		}

		for (j = i + 1; j < N; j++) {
			A[j][i] /= A[i][i];

			for (k = i + 1; k < N; k++)
				A[j][k] -= A[j][i] * A[i][k];
		}
	}

	return 1;  //decomposition done
}

double LUPDeterminant(double **A, int *P, int N) {

	double det = A[0][0];

	for (int i = 1; i < N; i++)
		det *= A[i][i];

	return (P[N] - N) % 2 == 0 ? det : -det;
}

int osc_expr_det(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 1){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "det requires at least one argument");
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	if(argc == 1){
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), osc_atom_u_getDouble(osc_atom_array_u_get(*argv, 0)));
	}else if(argc == 2){
		if(osc_atom_array_u_getLen(argv[0]) != 2 || osc_atom_array_u_getLen(argv[1]) != 2){
			osc_error(context, OSC_ERR_EXPR_ARGCHK, "arguments to det() are the rows of a matrix; each must have the same number of elements, equal to the number of rows (arguments)");
			return 1;
		}
		double a = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 0));
		double b = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 1));
		double c = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
		double d = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 1));
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), (a * d) - (b * c));
	}else if(argc == 3){
		if(osc_atom_array_u_getLen(argv[0]) != 3
		   || osc_atom_array_u_getLen(argv[1]) != 3
		   || osc_atom_array_u_getLen(argv[2]) != 3){
			osc_error(context, OSC_ERR_EXPR_ARGCHK, "arguments to det() are the rows of a matrix; each must have the same number of elements, equal to the number of rows (arguments)");
			return 1;
		}
		double a = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 0));
		double b = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 1));
		double c = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], 2));
		double d = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
		double e = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 1));
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 2));
		double g = osc_atom_u_getDouble(osc_atom_array_u_get(argv[2], 0));
		double h = osc_atom_u_getDouble(osc_atom_array_u_get(argv[2], 1));
		double i = osc_atom_u_getDouble(osc_atom_array_u_get(argv[2], 2));
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), (a*e*i)+(b*f*g)+(c*d*h)-(c*e*g)-(b*d*i)-(a*f*h));
	}else{
		double **cells = (double **)osc_mem_alloc(argc * sizeof(double *));
		for(int i = 0; i < argc; i++){
			if(argc != osc_atom_array_u_getLen(argv[i])){
				osc_error(context, OSC_ERR_EXPR_ARGCHK, "arguments to det() are the rows of a matrix; each must have the same number of elements, equal to the number of rows (arguments)");
				for(int j = 0; j < i; j++){
					if(cells[j]){
						osc_mem_free(cells[j]);
					}
				}
				if(cells){
					osc_mem_free(cells);
				}
				return 1;
			}
			cells[i] = (double *)osc_mem_alloc(argc * sizeof(double));
			for(int j = 0; j < argc; j++){
				cells[i][j] = osc_atom_u_getDouble(osc_atom_array_u_get(argv[i], j));
			}
		}
		int P[argc + 1];
		memset(P, 0, sizeof(int) * (argc + 1));
		LUPDecompose(cells, argc, 0, P);
		double d = LUPDeterminant(cells, P, argc);
		osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), d);
		for(int j = 0; j < argc; j++){
			if(cells[j]){
				osc_mem_free(cells[j]);
			}
		}
		if(cells){
			osc_mem_free(cells);
		}
	}
	
	return 0;
}

int osc_expr_l2norm(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(1);

	double s = 0;
	int i;
	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		s += pow(osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i)), 2.);
	}
	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), sqrt(s));
	return 0;
}

double osc_expr_qhdist(double x1, double y1, double x2, double y2, double x, double y)
{
	return (y - y1) * (x2 - x1) - (y2 - y1) * (x - x1);
}

int osc_expr_qhwhichside(double d)
{
	if(d > 0){
		return 1;
	}else if(d < 0){
		return -1;
	}else{
		return 0;
	}
}

int _osc_expr_quickhull(int hulln, int *hull, int p1, int p2, int n, double *xs, double *ys, int side)
{
	int idx = -1;
	double max = 0;
	for(int i = 0; i < n; i++){
		double d = osc_expr_qhdist(xs[p1], ys[p1], xs[p2], ys[p2], xs[i], ys[i]);
		double absd = fabs(d);
		int w = osc_expr_qhwhichside(d);
		if(w == side && absd > max){
			idx = i;
			max = absd;
		}
	}
	if(idx == -1){
		if(!hull[p1]){
			hulln++;
		}
		hull[p1] = 1;
		if(!hull[p2]){
			hulln++;
		}
		hull[p2] = 1;
		return hulln;
	}

	hulln = _osc_expr_quickhull(hulln, hull, idx, p1, n, xs, ys, -osc_expr_qhwhichside(osc_expr_qhdist(xs[p1], ys[p1], xs[p2], ys[p2], xs[idx], ys[idx])));
	hulln = _osc_expr_quickhull(hulln, hull, idx, p2, n, xs, ys, -osc_expr_qhwhichside(osc_expr_qhdist(xs[p2], ys[p2], xs[p1], ys[p1], xs[idx], ys[idx])));
	return hulln;
}

int osc_expr_quickhull(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 1 && argc != 2){
		osc_expr_err_argnum(context, 1, argc, 1, "error: quickhull()");
		return 0;
	}
	int n = 0;
	if(argc == 1){
		// interleaved
		n = osc_atom_array_u_getLen(argv[0]) / 2;
	}else{
		// argv[0] == xs, argv[1] == ys
		int len = osc_atom_array_u_getLen(argv[0]);
		if(len != osc_atom_array_u_getLen(argv[1])){
			// post error
			osc_error(context, OSC_ERR_INVAL, "quickhull(): both arguments should be the same length");
			return 0;
		}
		n = len;
	}
	if(n < 3){
		// post error
		osc_error(context, OSC_ERR_INVAL, "quickhull(): requires at least three points");
		return 0;
	}
	double xs[n];
	double ys[n];
	if(argc == 1){
		for(int i = 0; i < n; i++){
			xs[i] = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i * 2));
			ys[i] = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i * 2 + 1));
		}
	}else{
		for(int i = 0; i < n; i++){
			xs[i] = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i));
			ys[i] = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i));
		}
	}
	int minx = 0, maxx = 0;
	for(int i = 1; i < n; i++){
		if(xs[i] < xs[minx]){
			minx = i;
		}else if(xs[i] > xs[maxx]){
			maxx = i;
		}
	}
	int hull[n];
	memset(hull, 0, n * sizeof(int));
	int hulln = 0;
	hulln = _osc_expr_quickhull(hulln, hull, minx, maxx, n, xs, ys, 1);
	hulln = _osc_expr_quickhull(hulln, hull, minx, maxx, n, xs, ys, -1);

	*out = osc_atom_array_u_alloc(hulln * 2);
	int j = 0;
	for(int i = 0; i < n; i++){
		if(hull[i]){
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, j * 2), xs[i]);
			osc_atom_u_setDouble(osc_atom_array_u_get(*out, j * 2 + 1), ys[i]);
			j++;
		}
	}
	return 0;
}

int osc_expr_min(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc == 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(1);

	int i;
	double min = DBL_MAX;
	t_osc_atom_u *a = NULL;
	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(f <= min){
			min = f;
			a = osc_atom_array_u_get(*argv, i);
		}
	}
	t_osc_atom_u *o = osc_atom_array_u_get(*out, 0);
	osc_atom_u_copyInto(&o, a);
	return 0;
}

int osc_expr_max(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc == 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(1);

	int i;
	double max = -DBL_MAX;
	t_osc_atom_u *a = NULL;
	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(f >= max){
			max = f;
			a = osc_atom_array_u_get(*argv, i);
		}
	}
	t_osc_atom_u *o = osc_atom_array_u_get(*out, 0);
	osc_atom_u_copyInto(&o, a);
	return 0;
}

int osc_expr_extrema(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc == 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(2);

	int i;
	double min = DBL_MAX, max = -DBL_MAX;
	t_osc_atom_u *amin = NULL, *amax = NULL;
	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(f <= min){
			min = f;
			amin = osc_atom_array_u_get(*argv, i);
		}
		if(f >= max){
			max = f;
			amax = osc_atom_array_u_get(*argv, i);
		}
	}
	t_osc_atom_u *omin = osc_atom_array_u_get(*out, 0);
	t_osc_atom_u *osc = osc_atom_array_u_get(*out, 1);
	osc_atom_u_copyInto(&omin, amin);
	osc_atom_u_copyInto(&osc, amax);
	return 0;
}

int osc_expr_range(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc == 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(1);

	int i;
	double min = DBL_MAX, max = -DBL_MAX;
	char ttmin = 'i';
	char ttmax = 'i';
	//t_osc_atom_u *amin = NULL, *amax = NULL;
	for(i = 0; i < osc_atom_array_u_getLen(*argv); i++){
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(f <= min){
			min = f;
			ttmin = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
			//amin = osc_atom_array_u_get(*argv, i);
		}
		if(f >= max){
			max = f;
			ttmax = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
			//amax = osc_atom_array_u_get(*argv, i);
		}
	}
	t_osc_atom_u *r = osc_atom_array_u_get(*out, 0);
	if(ttmin == 'f' || ttmin == 'd' || ttmax == 'f' || ttmax == 'd'){
		osc_atom_u_setDouble(r, max - min);
	}else{
		osc_atom_u_setInt32(r, max - min);
	}
	return 0;
}

int osc_expr_clip(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc != 3){
		return 1;
	}

	t_osc_atom_u *min_atom = osc_atom_array_u_get(argv[1], 0);
	t_osc_atom_u *max_atom = osc_atom_array_u_get(argv[2], 0);

	double min = osc_atom_u_getDouble(min_atom);
	double max = osc_atom_u_getDouble(max_atom);

	char min_tt = osc_atom_u_getTypetag(min_atom);
	char max_tt = osc_atom_u_getTypetag(max_atom);

	int fl = 0;
	if(min_tt == 'd' || min_tt == 'f' ||
	   max_tt == 'd' || max_tt == 'f'){
		fl = 1;
	}

	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	t_osc_atom_ar_u *result = *out;
	int i;
	for(i = 0; i < len; i++){
		char type = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
		if(type == 's' || type == 'T' || type == 'F' || type == 'N' || type == 'b' || type == OSC_BUNDLE_TYPETAG){
			t_osc_atom_u *a = osc_atom_array_u_get(result, i);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			continue;
		}
		double val = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(val < min){
			val = min;
		}
		if(val > max){
			val = max;
		}
		if(type == 'f' || type == 'd' || fl){
			osc_atom_u_setDouble(osc_atom_array_u_get(result, i), val);
		}else{
			osc_atom_u_setInt32(osc_atom_array_u_get(result, i), (int32_t)val);
		}
	}
	return 0;
}

int osc_expr_scale(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc != 5){
		return 1;
	}
	t_osc_atom_u *min_in_atom = osc_atom_array_u_get(argv[1], 0);
	t_osc_atom_u *max_in_atom = osc_atom_array_u_get(argv[2], 0);
	t_osc_atom_u *min_out_atom = osc_atom_array_u_get(argv[3], 0);
	t_osc_atom_u *max_out_atom = osc_atom_array_u_get(argv[4], 0);

	char min_in_tt = osc_atom_u_getTypetag(min_in_atom);
	char max_in_tt = osc_atom_u_getTypetag(max_in_atom);
	char min_out_tt = osc_atom_u_getTypetag(min_out_atom);
	char max_out_tt = osc_atom_u_getTypetag(max_out_atom);

	double min_in = osc_atom_u_getDouble(min_in_atom);
	double max_in = osc_atom_u_getDouble(max_in_atom);
	double min_out = osc_atom_u_getDouble(min_out_atom);
	double max_out = osc_atom_u_getDouble(max_out_atom);

	int fl = 0;
	if(min_in_tt == 'd' || min_in_tt == 'f' ||
	   max_in_tt == 'd' || max_in_tt == 'f' ||
	   min_out_tt == 'd' || min_out_tt == 'f' ||
	   max_out_tt == 'd' || max_out_tt == 'f'){
		fl = 1;
	}

	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);
	t_osc_atom_ar_u *result = *out;
	if(max_out == min_out){
		int i;
		for(i = 0; i < len; i++){
			char type = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
			if(type == 's' || type == 'T' || type == 'F' || type == 'N' || type == 'b' || type == OSC_BUNDLE_TYPETAG){
				t_osc_atom_u *a = osc_atom_array_u_get(result, i);
				osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
				continue;
			}
			if(fl || type == 'd' || type == 'f'){
				osc_atom_u_setDouble(osc_atom_array_u_get(result, i), min_out);
			}else{
				osc_atom_u_setInt32(osc_atom_array_u_get(result, i), min_out);
			}
		}
		return 0;
	}
	double m = (max_out - min_out) / (max_in - min_in);
	double b = (min_out - (m * min_in));
		
	int i;
	for(i = 0; i < len; i++){
		char type = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
		if(type == 's' || type == 'T' || type == 'F' || type == 'N' || type == 'b' || type == OSC_BUNDLE_TYPETAG){
			t_osc_atom_u *a = osc_atom_array_u_get(result, i);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			continue;
		}
		double val = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		val = m * val + b;
		if(fl || type == 'd' || type == 'f'){
			osc_atom_u_setDouble(osc_atom_array_u_get(result, i), val);
		}else{
			osc_atom_u_setInt32(osc_atom_array_u_get(result, i), (int32_t)val);
		}
	}
	return 0;
}

int osc_expr_mtof(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	if(argc == 0){
		return 1;
	}
	double base = 440;
	if(argc > 1){
		if(osc_atom_array_u_getLen(argv[1]) > 0){
			base = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
		}
	}
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	t_osc_atom_ar_u *result = *out;
	int i;
	for(i = 0; i < len; i++){
		char type = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
		if(type == 's' || type == 'T' || type == 'F' || type == 'N' || type == 'b' || type == OSC_BUNDLE_TYPETAG){
			t_osc_atom_u *a = osc_atom_array_u_get(result, i);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			continue;
		}
		double val = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		// l(2) * (1. / 12.)
		val = base * exp(.05776226504666210911810267678817 * (val - 69));
		osc_atom_u_setDouble(osc_atom_array_u_get(result, i), val);
	}
	return 0;
}

int osc_expr_ftom(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc == 0){
		return 1;
	}
	double base = 440;
	if(argc > 1){
		if(osc_atom_array_u_getLen(argv[1]) > 0){
			base = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], 0));
		}
	}
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	t_osc_atom_ar_u *result = *out;
	int i;
	for(i = 0; i < len; i++){
		char type = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
		if(type == 's' || type == 'T' || type == 'F' || type == 'N' || type == 'b' || type == OSC_BUNDLE_TYPETAG){
			t_osc_atom_u *a = osc_atom_array_u_get(result, i);
			osc_atom_u_copyInto(&a, osc_atom_array_u_get(*argv, i));
			continue;
		}
		double val = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		// 1. / (l(2) * (1. / 12))
		val = (69 + (17.31234049066756088831909617202611 * log(val / base)));
		osc_atom_u_setDouble(osc_atom_array_u_get(result, i), val);
	}
	return 0;
}

int _osc_expr_sign(double f)
{
	if(f == 0){
		return 0;
	}else if(f < 0){
		return -1;
	}else{
		return 1;
	}
}

int osc_expr_sign(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	long len = osc_atom_array_u_getLen(*argv);
	*out = osc_atom_array_u_alloc(len);

	int i;
	for(i = 0; i < len; i++){
		double f = osc_atom_u_getDouble(osc_atom_array_u_get(*argv, i));
		if(f == 0){
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), 0);
		}else if(f < 0){
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), -1);
		}else{
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, i), 1);
		}
	}
	return 0;
}

int osc_expr_if(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_bound(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_exists(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_emptybundle(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_getaddresses(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_delete(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_getmsgcount(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_identity(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	// pass through
	*out = osc_atom_array_u_copy(*argv);
	return 0;
}

int osc_expr_eval_call(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context){
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_tokenize(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_compile(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_prog1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		*out = osc_atom_array_u_copy(argv[0]);
		return 0;
	}else{
		return 1;
	}
}

int osc_expr_prog2(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc > 1){
		*out = osc_atom_array_u_copy(argv[1]);
		return 0;
	}else{
		return 1;
	}
}

int osc_expr_progn(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		*out = osc_atom_array_u_copy(argv[argc - 1]);
		return 0;
	}else{
		return 1;
	}
}

int osc_expr_apply(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	return 0;
}

int osc_expr_map(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	return 0;
}

int osc_expr_lreduce(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	return 0;
}

int osc_expr_rreduce(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	return 0;
}

int osc_expr_quote(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_value(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// this is a dummy function.  we'll use this to do a pointer comparison.
	return 0;
}

int osc_expr_typetags(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		int len = osc_atom_array_u_getLen(*argv);
		*out = osc_atom_array_u_alloc(len);

		int i;
		for(i = 0; i < len; i++){
			char tt = osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, i));
			osc_atom_u_setInt8(osc_atom_array_u_get(*out, i), tt);
		}

	}
	return 0;
}

int osc_expr_lambda(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// dummy
	return 0;
}

int osc_expr_gettimetag(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// dummy
	return 0;
}

int osc_expr_settimetag(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// dummy
	return 0;
}

int osc_expr_getbundlemember(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// dummy
	return 0;
}

int osc_expr_assigntobundlemember(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	// dummy
	return 0;
}

int osc_expr_strcmp(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 2){
		return 1;
	}
	char *st1 = NULL, *st2 = NULL;
	osc_atom_u_getString(osc_atom_array_u_get(*argv, 0), 0, &st1);
	osc_atom_u_getString(osc_atom_array_u_get(argv[1], 0), 0, &st2);
	int ret = 0;
	if(argc == 3){
		int n = osc_atom_u_getInt(osc_atom_array_u_get(argv[2], 0));
		ret = strncmp(st1, st2, n);
	}else{
		ret = strcmp(st1, st2);
	}
    osc_mem_free(st1);
    osc_mem_free(st2);
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), ret);
	return 0;
}

int osc_expr_strlen(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
    if( argc == 0 )
        return 1;
    
    long len = osc_atom_array_u_getLen(*argv);
    *out = osc_atom_array_u_alloc(len);
    
    int i;
    for(i = 0; i < len; i++)
    {
        char *str = NULL;
        osc_atom_u_getString(osc_atom_array_u_get(*argv, i), 0, &str);
        long len = strlen(str);
        osc_atom_u_setInt64(osc_atom_array_u_get(*out, i), len);
        osc_mem_free(str);
    }
    return 0;
}


int osc_expr_strchar(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
    if( argc != 2 )
        return 1;
    
    // strchar( [1,2,3], /str )
    long numchars = osc_atom_array_u_getLen(argv[0]);
    long len = osc_atom_array_u_getLen(argv[1]);
    *out = osc_atom_array_u_alloc(len);
    
    int i, j;
    for(i = 0; i < len; i++)
    {
        char *str = NULL;
        osc_atom_u_getString(osc_atom_array_u_get(argv[1], i), 0, &str);
        long len = strlen(str);
        
        char buf[numchars+1];
        for( j = 0; j < numchars; j++ )
        {
            int pos = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], j));
            if( pos >= 0 && pos < len )
            {
                buf[j] = str[pos];
            }
            else
            {
                osc_error(context, OSC_ERR_EXPR_ARGCHK, "char index out of range.");
                osc_mem_free(str);
                return 1;
            }
        }
        buf[numchars] = '\0';
        osc_atom_u_setString(osc_atom_array_u_get(*out, i), buf );
        osc_mem_free(str);
    }
    return 0;
}

int osc_expr_strfind(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
    if(argc != 2){
        return 1;
    }
    char *str = NULL;
    char *substr = NULL;
    osc_atom_u_getString(osc_atom_array_u_get(argv[1], 0), 0, &str);
    osc_atom_u_getString(osc_atom_array_u_get(argv[0], 0), 0, &substr);
    if(!substr || !str){
        return 1;
    }
    // strfind( "/foo", "/foo/bar" )

    // return first character position of found substring
    long substrlen = strlen(substr);
    if(substrlen == 0)
    {
        return 0;
    }
    
    long n = strlen(str);
    t_osc_atom_ar_u *ar = osc_atom_array_u_alloc(n);
    int i = 0;
    char *p = str;
    while( (p = strstr(p, substr)) != NULL )
    {
        long pos = p - str;
        p += substrlen;
        osc_atom_u_setInt64(osc_atom_array_u_get(ar, i++), pos);
    }
    osc_atom_array_u_resize(ar, i);
    
    *out = ar;
    
    if(substr){
        osc_mem_free(substr);
    }
    if(str){
        osc_mem_free(str);
    }
    return 0;
}


int osc_expr_split(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 2){
		return 1;
	}
	char *str = NULL;
	char *sep = NULL;
	osc_atom_u_getString(osc_atom_array_u_get(argv[1], 0), 0, &str);
	osc_atom_u_getString(osc_atom_array_u_get(argv[0], 0), 0, &sep);
	if(!sep || !str){
		return 1;
	}
	if(strlen(sep) == 0){
		int len = strlen(str);
		*out = osc_atom_array_u_alloc(len);
		for(int i = 0; i < len; i++){
			t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
			osc_atom_u_setInt8(a, str[i]);
		}
		return 0;
	}
	int n = 16;
	t_osc_atom_ar_u *ar = osc_atom_array_u_alloc(n);
	int i = 0;
	char *c = NULL;
	char *tok = NULL;
	tok = strtok_r(str, sep, &c);
	while(tok){
		if(i >= n){
			n += 16;
			osc_atom_array_u_resize(ar, n);
		}
		if(tok){
			osc_atom_u_setString(osc_atom_array_u_get(ar, i++), tok);
		}
		tok = strtok_r(NULL, sep, &c);
	}
	osc_atom_array_u_resize(ar, i);
	*out = ar;
	if(sep){
		osc_mem_free(sep);
	}
	if(str){
		osc_mem_free(str);
	}
	return 0;
}

int osc_expr_join(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 2){
		return 1;
	}
	char *sep = NULL;
	osc_atom_u_getString(osc_atom_array_u_get(argv[0], 0), 0, &sep);
	int seplen = 0;
	if(sep){
		seplen = strlen(sep);
	}else{
		sep = "";
	}
	int n = 0;
	for(int i = 1; i < argc; i++){
		n += osc_atom_array_u_getLen(argv[i]);
	}
	char *ptrs[n];
	int len = 0;
	n = 0;
	for(int i = 1; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			char *ptr = NULL;
			t_osc_atom_u *a = osc_atom_array_u_get(argv[i], j);
			osc_atom_u_getString(a, 0, &ptr);
			char tt = osc_atom_u_getTypetag(a);
			if(tt == 'c' || tt == 'C'){
				ptr[0] = ptr[1];
				ptr[1] = 0;
			}
			ptrs[n++] = ptr;
			if(ptr){
				len += strlen(ptr) + seplen;
			}
		}
	}
	len++;
	char *buf = (char *)osc_mem_alloc(len);
	char *bufptr = buf;
	for(int i = 0; i < n; i++){
		if(ptrs[i]){
			if(i == n - 1){
				bufptr += sprintf(bufptr, "%s", ptrs[i]);
			}else{
				bufptr += sprintf(bufptr, "%s%s", ptrs[i], sep);
			}
			osc_mem_free(ptrs[i]);
		}
	}
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setStringPtr(osc_atom_array_u_get(*out, 0), buf);
	osc_atom_u_setShouldFreePtr(osc_atom_array_u_get(*out, 0), 1);
	if(sep){
		osc_mem_free(sep);
	}
	return 0;
}

int osc_expr_bitand(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 2){
		return 1;
	}
	if(!OSC_TYPETAG_ISINT(osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, 0))) || !OSC_TYPETAG_ISINT(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[1], 0)))){
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	char largest_type = osc_typetag_getLargestType(argc, argv);
	switch(largest_type){
	case 'i':
		{
			int32_t i1 = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], 0));
			int32_t i2 = osc_atom_u_getInt32(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'I':
		{
			uint32_t i1 = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[0], 0));
			uint32_t i2 = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'h':
		{
			int64_t i1 = osc_atom_u_getInt64(osc_atom_array_u_get(argv[0], 0));
			int64_t i2 = osc_atom_u_getInt64(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'H':
		{
			uint64_t i1 = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[0], 0));
			uint64_t i2 = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'u':
		{
			int16_t i1 = osc_atom_u_getInt16(osc_atom_array_u_get(argv[0], 0));
			int16_t i2 = osc_atom_u_getInt16(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt16(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'U':
		{
			uint16_t i1 = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[0], 0));
			uint16_t i2 = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt16(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'c':
		{
			int8_t i1 = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 0));
			int8_t i2 = osc_atom_u_getInt8(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt8(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	case 'C':
		{
			uint8_t i1 = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[0], 0));
			uint8_t i2 = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt8(osc_atom_array_u_get(*out, 0), i1 & i2);
		}
		break;
	}
	return 0;
}

int osc_expr_bitor(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc != 2){
		return 1;
	}
	if(!OSC_TYPETAG_ISINT(osc_atom_u_getTypetag(osc_atom_array_u_get(*argv, 0))) || !OSC_TYPETAG_ISINT(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[1], 0)))){
		return 1;
	}
	*out = osc_atom_array_u_alloc(1);
	char largest_type = osc_typetag_getLargestType(argc, argv);
	switch(largest_type){
	case 'i':
		{
			int32_t i1 = osc_atom_u_getInt32(osc_atom_array_u_get(argv[0], 0));
			int32_t i2 = osc_atom_u_getInt32(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt32(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'I':
		{
			uint32_t i1 = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[0], 0));
			uint32_t i2 = osc_atom_u_getUInt32(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt32(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'h':
		{
			int64_t i1 = osc_atom_u_getInt64(osc_atom_array_u_get(argv[0], 0));
			int64_t i2 = osc_atom_u_getInt64(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt64(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'H':
		{
			uint64_t i1 = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[0], 0));
			uint64_t i2 = osc_atom_u_getUInt64(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt64(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'u':
		{
			int16_t i1 = osc_atom_u_getInt16(osc_atom_array_u_get(argv[0], 0));
			int16_t i2 = osc_atom_u_getInt16(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt16(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'U':
		{
			uint16_t i1 = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[0], 0));
			uint16_t i2 = osc_atom_u_getUInt16(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setUInt16(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'c':
		{
			int8_t i1 = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 0));
			int8_t i2 = osc_atom_u_getInt8(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt8(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	case 'C':
		{
			uint8_t i1 = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[0], 0));
			uint8_t i2 = osc_atom_u_getUInt8(osc_atom_array_u_get(argv[1], 0));
			osc_atom_u_setInt8(osc_atom_array_u_get(*out, 0), i1 | i2);
		}
		break;
	}
	return 0;
}

//=====================================================================================================
// MadgwickAHRS.c
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Definitions

#define sampleFreq	512.0f		// sample frequency in Hz
#define beta		0.1f		// 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions

//volatile float beta = betaDef;								// 2 * proportional gain (Kp)
//volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

float invSqrt(float x);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float *_q0, float *_q1, float *_q2, float *_q3);

//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float *_q0, float *_q1, float *_q2, float *_q3) {
	float q0 = *_q0;
	float q1 = *_q1;
	float q2 = *_q2;
	float q3 = *_q3;
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az, &q0, &q1, &q2, &q3);
		return;
	}

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Normalise magnetometer measurement
		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0f * q0 * mx;
		_2q0my = 2.0f * q0 * my;
		_2q0mz = 2.0f * q0 * mz;
		_2q1mx = 2.0f * q1 * mx;
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_2q0q2 = 2.0f * q0 * q2;
		_2q2q3 = 2.0f * q2 * q3;
		q0q0 = q0 * q0;
		q0q1 = q0 * q1;
		q0q2 = q0 * q2;
		q0q3 = q0 * q3;
		q1q1 = q1 * q1;
		q1q2 = q1 * q2;
		q1q3 = q1 * q3;
		q2q2 = q2 * q2;
		q2q3 = q2 * q3;
		q3q3 = q3 * q3;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		// Gradient decent algorithm corrective step
		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;

	*_q0 = q0;
	*_q1 = q1;
	*_q2 = q2;
	*_q3 = q3;
}

//---------------------------------------------------------------------------------------------------
// IMU algorithm update

void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float *_q0, float *_q1, float *_q2, float *_q3) {
	float q0 = *_q0;
	float q1 = *_q1;
	float q2 = *_q2;
	float q3 = *_q3;

	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;

	*_q0 = q0;
	*_q1 = q1;
	*_q2 = q2;
	*_q3 = q3;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

//====================================================================================================
// END OF IMU CODE
//====================================================================================================

int osc_expr_imu(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc < 2){
		return 1;
	}
	if(osc_atom_array_u_getLen(*argv) < 9){
		return 1;
	}
	if(osc_atom_array_u_getLen(argv[1]) < 4){
		return 1;
	}
	// imu(/imu, /quaternion)
	float gx = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 0));
	float gy = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 1));
	float gz = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 2));
	float ax = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 3));
	float ay = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 4));
	float az = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 5));
	float mx = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 6));
	float my = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 7));
	float mz = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 8));
	float temp = 0;
	int havetemp = 0;
	if(osc_atom_array_u_getLen(*argv) == 10){
		temp = osc_atom_u_getFloat(osc_atom_array_u_get(*argv, 9));
		havetemp++;
	}
	float q0 = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 0));
	float q1 = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 1));
	float q2 = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 2));
	float q3 = osc_atom_u_getFloat(osc_atom_array_u_get(argv[1], 3));
	*out = osc_atom_array_u_alloc(4);

	MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz, &q0, &q1, &q2, &q3);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 0), q0);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 1), q1);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 2), q2);
	osc_atom_u_setFloat(osc_atom_array_u_get(*out, 3), q3);
	return 0;
}

// constants
int osc_expr_pi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_PI);
	return 0;
}

int osc_expr_twopi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 2 * M_PI);
	return 0;
}

int osc_expr_halfpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_PI_2);
	return 0;
}

int osc_expr_quarterpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_PI_4);
	return 0;
}

int osc_expr_oneoverpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_1_PI);
	return 0;
}

int osc_expr_twooverpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_2_PI);
	return 0;
}

int osc_expr_degtorad(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 0.017453);
	return 0;
}

int osc_expr_radtodeg(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 57.29578);
	return 0;
}

int osc_expr_e(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_E);
	return 0;
}

int osc_expr_lntwo(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_LN2);
	return 0;
}

int osc_expr_lnten(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_LN10);
	return 0;
}

int osc_expr_logtwoe(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_LOG2E);
	return 0;
}

int osc_expr_logtene(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_LOG10E);
	return 0;
}

int osc_expr_sqrttwo(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_SQRT2);
	return 0;
}

int osc_expr_sqrthalf(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), M_SQRT1_2);
	return 0;
}

int osc_expr_phi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 1.618033988749895);
	return 0;
}

int osc_expr_twelfth_root_of_2(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 1.059463094359295);
	return 0;
}

int osc_expr_thirteenth_root_of_3(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	*out = osc_atom_array_u_alloc(1);

	osc_atom_u_setDouble(osc_atom_array_u_get(*out, 0), 1.088182243463317);
	return 0;
}

int osc_expr_explicitCast(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void* context)
{
	if(argc){
		if(f->rec->extra == osc_expr_explicitCast_blob){
			long n = osc_atom_array_u_getLen(*argv);
			char *blob = osc_mem_alloc(n + 4);
			for(int i = 0; i < n; i++){
				blob[i + 4] = osc_atom_u_getInt8(osc_atom_array_u_get(*argv, i));
			}
			*((int32_t *)blob) = hton32(n);
			*out = osc_atom_array_u_alloc(1);
			osc_atom_u_setBlobPtr(osc_atom_array_u_get(*out, 0), blob);
		}else{
			int n = osc_atom_array_u_getLen(*argv);
			*out = osc_atom_array_u_alloc(n);
			osc_atom_array_u_clear(*out);

			int i;
			for(i = 0; i < n; i++){
				int ret;
				int (*func)(t_osc_atom_u *dest, t_osc_atom_u *src, void* context) =
					(int (*)(t_osc_atom_u *, t_osc_atom_u *, void*))f->rec->extra;
				ret = func(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(*argv, i), context );
				if(ret){
					return ret;
				}
			}
		}
	}
	return 0;
}

int osc_expr_explicitCast_float32(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setFloat(dest, osc_atom_u_getFloat(src));
	return 0;
}

int osc_expr_explicitCast_float64(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setDouble(dest, osc_atom_u_getDouble(src));
	return 0;
}

int osc_expr_explicitCast_int8(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setInt8(dest, osc_atom_u_getInt8(src));
	return 0;
}

int osc_expr_explicitCast_int16(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setInt16(dest, osc_atom_u_getInt16(src));
	return 0;
}

int osc_expr_explicitCast_int32(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setInt32(dest, osc_atom_u_getInt32(src));
	return 0;
}

int osc_expr_explicitCast_int64(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setInt64(dest, osc_atom_u_getInt64(src));
	return 0;
}

int osc_expr_explicitCast_uint8(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setUInt8(dest, osc_atom_u_getUInt8(src));
	return 0;
}

int osc_expr_explicitCast_uint16(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setUInt16(dest, osc_atom_u_getUInt16(src));
	return 0;
}

int osc_expr_explicitCast_uint32(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setUInt32(dest, osc_atom_u_getUInt32(src));
	return 0;
}

int osc_expr_explicitCast_uint64(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setUInt64(dest, osc_atom_u_getUInt64(src));
	return 0;
}

int osc_expr_explicitCast_bool(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	osc_atom_u_setBool(dest, osc_atom_u_getBool(src));
	return 0;
}

int osc_expr_explicitCast_string(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	char *s = NULL;
	if(osc_atom_u_getTypetag(src) == 'b'){
		char *b = osc_atom_u_getBlob(src);
		osc_atom_u_setString(dest, b + 4);
		return 0;
	}else{
		if(osc_atom_u_getTypetag(src) == 'c'){
			char c = osc_atom_u_getInt8(src);
			char buf[6];
			if(c >= 32 && c < 127){
				buf[0] = c;
				buf[1] = 0;
			}else{
				snprintf(buf, 4, "%d", c);
			}
			osc_atom_u_setString(dest, buf);
			return 0;
		}else if(osc_atom_u_getTypetag(src) == 'C'){
			unsigned char c = osc_atom_u_getInt8(src);
			char buf[4];
			if(c >= 32 && c < 127){
				buf[0] = c;
				buf[1] = 0;
			}else{
				snprintf(buf, 4, "%u", c);
			}
			osc_atom_u_setString(dest, buf);
			return 0;
		}else{
			osc_atom_u_getString(src, 0, &s);
			if(s){
				osc_atom_u_setString(dest, s);
				osc_mem_free(s);
				return 0;
			}else{
				return 1;
			}
		}
	}
}

int osc_expr_explicitCast_blob(t_osc_atom_u *dest, t_osc_atom_u *src)
{
	int32_t l = 0;
	char *blob = NULL;
	osc_atom_u_getBlobCopy(src, &l, &blob);
	osc_atom_u_setBlobPtr(dest, blob);
	return 0;
}

int osc_expr_explicitCast_dynamic(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc > 1){
		t_osc_atom_ar_u *ttarray = NULL;
		int ttlen = osc_atom_array_u_getLen(argv[0]);
		int arglen = osc_atom_array_u_getLen(argv[1]);
		int alloc = 0;
		if(arglen > 1 && ttlen == 1){
			alloc = 1;
			ttarray = osc_atom_array_u_alloc(arglen);
			char tt = osc_atom_u_getInt8(osc_atom_array_u_get(argv[0], 0));
			for(int i = 0; i < arglen; i++){
				osc_atom_u_setInt8(osc_atom_array_u_get(ttarray, i), tt);
			}
		}else{
			ttarray = *argv;
		}
		int n = osc_atom_array_u_getLen(argv[1]);
		*out = osc_atom_array_u_alloc(n);
		osc_atom_array_u_clear(*out);

		int i;
		for(i = 0; i < n; i++){
			char tt = osc_atom_u_getInt8(osc_atom_array_u_get(ttarray, i));
			switch(tt){
			case 'c':
				osc_expr_explicitCast_int8(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'C':
				osc_expr_explicitCast_uint8(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'u':
				osc_expr_explicitCast_int16(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'U':
				osc_expr_explicitCast_uint16(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'i':
				osc_expr_explicitCast_int32(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'I':
				osc_expr_explicitCast_uint32(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'h':
				osc_expr_explicitCast_int64(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'H':
				osc_expr_explicitCast_uint64(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'f':
				osc_expr_explicitCast_float32(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'd':
				osc_expr_explicitCast_float64(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 's':
				osc_expr_explicitCast_string(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			case 'b':
				osc_expr_explicitCast_blob(osc_atom_array_u_get(*out, i), osc_atom_array_u_get(argv[1], i));
				break;
			}
		}
		if(alloc){
			osc_atom_array_u_free(ttarray);
		}
		return 0;
	}
	return 1;
}

int osc_expr_hton32(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	int count = 0;
	for(int i = 0; i < argc; i++){
		count += osc_atom_array_u_getLen(argv[i]);
	}
	*out = osc_atom_array_u_alloc(count);
	int k = 0;
	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			t_osc_atom_u *a = osc_atom_array_u_get(argv[i], j);
			switch(osc_atom_u_getTypetag(a)){
			case 'c':
			case 'u':
			case 'i':
				osc_atom_u_setInt32(osc_atom_array_u_get(*out, k), hton32(osc_atom_u_getInt32(a)));
				break;
			case 'C':
			case 'U':
			case 'I':
				osc_atom_u_setUInt32(osc_atom_array_u_get(*out, k), hton32(osc_atom_u_getUInt32(a)));
				break;
			}
			k++;
		}
	}
	return 0;
}

int osc_expr_ntoh32(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	int count = 0;
	for(int i = 0; i < argc; i++){
		count += osc_atom_array_u_getLen(argv[i]);
	}
	*out = osc_atom_array_u_alloc(count);
	int k = 0;
	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			t_osc_atom_u *a = osc_atom_array_u_get(argv[i], j);
			switch(osc_atom_u_getTypetag(a)){
			case 'c':
			case 'u':
			case 'i':
				osc_atom_u_setInt32(osc_atom_array_u_get(*out, k), ntoh32(osc_atom_u_getInt32(a)));
				break;
			case 'C':
			case 'U':
			case 'I':
				osc_atom_u_setUInt32(osc_atom_array_u_get(*out, k), ntoh32(osc_atom_u_getUInt32(a)));
				break;
			}
			k++;
		}
	}
	return 0;
}

int osc_expr_hton64(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	int count = 0;
	for(int i = 0; i < argc; i++){
		count += osc_atom_array_u_getLen(argv[i]);
	}
	*out = osc_atom_array_u_alloc(count);
	int k = 0;
	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			t_osc_atom_u *a = osc_atom_array_u_get(argv[i], j);
			switch(osc_atom_u_getTypetag(a)){
			case 'c':
			case 'u':
			case 'i':
			case 'h':
				osc_atom_u_setUInt64(osc_atom_array_u_get(*out, k), hton64(osc_atom_u_getUInt64(a)));
				break;
			case 'C':
			case 'U':
			case 'I':
			case 'H':
				osc_atom_u_setUInt64(osc_atom_array_u_get(*out, k), hton64(osc_atom_u_getUInt64(a)));
				break;
			}
			k++;
		}
	}
	return 0;
}

int osc_expr_ntoh64(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	int count = 0;
	for(int i = 0; i < argc; i++){
		count += osc_atom_array_u_getLen(argv[i]);
	}
	*out = osc_atom_array_u_alloc(count);
	int k = 0;
	for(int i = 0; i < argc; i++){
		for(int j = 0; j < osc_atom_array_u_getLen(argv[i]); j++){
			t_osc_atom_u *a = osc_atom_array_u_get(argv[i], j);
			switch(osc_atom_u_getTypetag(a)){
			case 'c':
			case 'u':
			case 'i':
			case 'h':
				osc_atom_u_setInt64(osc_atom_array_u_get(*out, k), ntoh64(osc_atom_u_getInt64(a)));
				break;
			case 'C':
			case 'U':
			case 'I':
			case 'H':
				osc_atom_u_setUInt64(osc_atom_array_u_get(*out, k), ntoh64(osc_atom_u_getUInt64(a)));
				break;
			}
			k++;
		}
	}
	return 0;
}

int osc_expr_readstring(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc > 1){
		osc_expr_err_argnum(context, 1, argc, 0, "readstring");
		return 1;
	}
	if(osc_atom_array_u_getLen(argv[0]) > 1){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "readstring takes a single string or address as an argument. found a list\n");
		return 1;
	}
	t_osc_atom_u *a = osc_atom_array_u_get(argv[0], 0);
	if(osc_atom_u_getTypetag(a) != 's'){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "argument to readstring must be a string\n");
		return 1;
	}
	char *st = osc_atom_u_getStringPtr(a);
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setString(osc_atom_array_u_get(*out, 0), st);
	return 0;
}

int osc_expr_strtotime(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 1){
		osc_expr_err_argnum(context, 1, argc, 0, "strtotime");
	}
	if(osc_atom_array_u_getLen(argv[0]) > 1){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "strtotime takes a single string as an argument. found a list\n");
		return 1;
	}
	t_osc_atom_u *a = osc_atom_array_u_get(argv[0], 0);
	if(osc_atom_u_getTypetag(a) != 's'){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "argument to strtotime must be a string\n");
		return 1;
	}
	char *st = osc_atom_u_getStringPtr(a);
	*out = osc_atom_array_u_alloc(1);
	t_osc_timetag tt;
	osc_timetag_fromISO8601(st, &tt);
	osc_atom_u_setTimetag(osc_atom_array_u_get(*out, 0), tt);
	return 0;
}

int osc_expr_floattotime(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 1){
		osc_expr_err_argnum(context, 1, argc, 0, "floattotime");
	}
	if(osc_atom_array_u_getLen(argv[0]) > 1){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "floattotime takes a single float as an argument. found a list\n");
		return 1;
	}
	t_osc_atom_u *a = osc_atom_array_u_get(argv[0], 0);
	if(osc_atom_u_getTypetag(a) != 'f' && osc_atom_u_getTypetag(a) != 'd'){
		osc_error(context, OSC_ERR_EXPR_ARGCHK, "argument to strtotime must be a float or a double\n");
		return 1;
	}
	double d = osc_atom_u_getDouble(a);
	*out = osc_atom_array_u_alloc(1);
	t_osc_timetag tt = osc_timetag_floatToTimetag(d);
	osc_atom_u_setTimetag(osc_atom_array_u_get(*out, 0), tt);
	return 0;
}

int osc_expr_match(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	if(argc != 2){
		osc_expr_err_argnum(context, 2, argc, 0, "match");
	}
	int npatterns = osc_atom_array_u_getLen(argv[0]);
	int naddresses = osc_atom_array_u_getLen(argv[1]);
	*out = osc_atom_array_u_alloc(npatterns);
	for(int i = 0; i < npatterns; i++){
		t_osc_atom_u *pattern = osc_atom_array_u_get(argv[0], i);
		if(osc_atom_u_getTypetag(pattern) != 's'){
			continue;
		}
		char *pattern_string = osc_atom_u_getStringPtr(pattern);
		t_osc_bndl_u *b = osc_bundle_u_alloc();
		t_osc_msg_u *pattern_msg = osc_message_u_allocWithString("/pattern", pattern_string);
		t_osc_msg_u *full = osc_message_u_allocWithAddress("/full");
		t_osc_msg_u *partial = osc_message_u_allocWithAddress("/partial");
		t_osc_msg_u *unmatched = osc_message_u_allocWithAddress("/unmatched");
		int matches[naddresses];
		memset(matches, 0, sizeof(int) * naddresses);
		for(int j = 0; j < naddresses; j++){
			t_osc_atom_u *address = osc_atom_array_u_get(argv[1], j);
			if(osc_atom_u_getTypetag(address) != 's'){
				continue;
			}
			char *address_string = osc_atom_u_getStringPtr(address);
			int po = 0, ao = 0;
			int ret = osc_match(pattern_string, address_string, &po, &ao);
			if((ret & OSC_MATCH_ADDRESS_COMPLETE) && ((ret & OSC_MATCH_PATTERN_COMPLETE))){
				// complete match
				osc_message_u_appendString(full, address_string);
				matches[j]++;
			}else if(po > 0 && ((pattern_string[po] == '/'))){
				// partial match
				osc_message_u_appendString(partial, address_string);
				matches[j]++;
			}
		}
		for(int j = 0; j < naddresses; j++){
			if(matches[j] == 0){
				t_osc_atom_u *address = osc_atom_array_u_get(argv[1], j);
				if(osc_atom_u_getTypetag(address) != 's'){
					continue;
				}
				char *address_string = osc_atom_u_getStringPtr(address);
				osc_message_u_appendString(unmatched, address_string);
			}
		}
		osc_bundle_u_addMsg(b, pattern_msg);
		osc_bundle_u_addMsg(b, full);
		osc_bundle_u_addMsg(b, partial);
		osc_bundle_u_addMsg(b, unmatched);
		osc_atom_u_setBndl_u(osc_atom_array_u_get(*out, i), b);
	}
	return 0;
	/*
	int nselectors = osc_atom_array_u_getLen(argv[1]);
	char *selectors[nselectors];
	for(int i = 0; i < nselectors; i++){
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[1], i)) != 's'){
			selectors[i] = NULL;
		}else{
			selectors[i] = osc_atom_u_getStringPtr(osc_atom_array_u_get(argv[1], i));
		}
	}
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	for(int i = 0; i < osc_atom_array_u_getLen(argv[0]); i++){
		if(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[0], i)) != 's'){
			continue;
		}else{
			t_osc_msg_u *m = osc_message_u_allocWithAddress(osc_atom_u_getStringPtr(osc_atom_array_u_get(argv[0], i)));
			osc_bundle_u_addMsg(b, m);
		}
	}
	t_osc_bndl_s *bs = osc_bundle_u_serialize(b);
	osc_bundle_u_free(b);
	t_osc_rset *rset = osc_rset_alloc();
	t_osc_err e = osc_query_select(nselectors, selectors, osc_bundle_s_getLen(bs), osc_bundle_s_getPtr(bs), 1, &rset);
	osc_rset_initIterator(rset);
	*out = osc_atom_array_u_alloc(nselectors + 1);
	int i = 0;
	while(osc_rset_hasNext(rset)){
		t_osc_rset_result *rr = osc_rset_next(rset);
		t_osc_bndl_u *b = osc_bundle_u_alloc();
		t_osc_msg_u *full = osc_message_u_allocWithAddress("/full");
		t_osc_bndl_s *fullb = osc_rset_result_getCompleteMatches(rr);
		t_osc_msg_u *partial = osc_message_u_allocWithAddress("/partial");
		t_osc_bndl_s *partialb = osc_rset_result_getPartialMatches(rr);
		osc_message_u_appendBndl(full, osc_bundle_s_getLen(fullb), osc_bundle_s_getPtr(fullb));
		osc_message_u_appendBndl(partial, osc_bundle_s_getLen(partialb), osc_bundle_s_getPtr(partialb));
		osc_bundle_u_addMsg(b, full);
		osc_bundle_u_addMsg(b, partial);
		osc_atom_u_setBndl_u(osc_atom_array_u_get(*out, i), b);
		i++;
	}
	t_osc_bndl_s *unmatched = osc_rset_getUnmatched(rset);
	osc_atom_u_setBndl(osc_atom_array_u_get(*out, i), osc_bundle_s_getLen(unmatched), osc_bundle_s_getPtr(unmatched));
	return 0;
	*/
}

int osc_expr_bundle(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out, void *context)
{
	*out = osc_atom_array_u_alloc(1);
	t_osc_atom_u *aout = osc_atom_array_u_get(*out, 0);
	if(argc == 1){
		// if there's only one argument, it's either:
		// -- a blob: convert it to a subbundle
		// -- a list: treat it as a list of bytes and convert it to a subbundle
		// -- a string: create a subbundle and make that string the address of the only message
		int32_t n = osc_atom_array_u_getLen(argv[0]);
		if(n == 1){
			t_osc_atom_u *a = osc_atom_array_u_get(*argv, 0);
			if(osc_atom_u_getTypetag(a) == 'b'){
				int32_t len = 0;
				char *ptr = NULL;
				osc_atom_u_getBlobCopy(a, &len, &ptr);
				len -= 4;
				osc_atom_u_setBndl(aout, len, ptr);
				return 0;
			}else if(osc_atom_u_getTypetag(a) == 's'){
				t_osc_bndl_u *b = osc_bundle_u_alloc();
				t_osc_msg_u *m = osc_message_u_allocWithAddress(osc_atom_u_getStringPtr(a));
				osc_bundle_u_addMsg(b, m);
				osc_atom_u_setBndl_u(aout, b);
				return 0;
			}
		}else{
			int np = n;
			while(np % 4){
				np++;
			}
			char buf[np];
			memset(buf, 0, np);
			for(int i = 0; i < n; i++){
				buf[i] = osc_atom_u_getInt32(osc_atom_array_u_get(*argv, i));
			}
			osc_atom_u_setBndl(aout, np, buf);
			return 0;
		}
	}else{
		// multiple arguments means at least one message
		t_osc_bndl_u *b = osc_bundle_u_alloc();
		for(int i = 0; i < argc; i++){
			t_osc_atom_u *aa = osc_atom_array_u_get(argv[i], 0);
			if(osc_atom_u_getTypetag(aa) != 's'){
				osc_bundle_u_free(b);
				osc_atom_array_u_free(*out);
				*out = NULL;
				return 1;
			}
			t_osc_msg_u *m = osc_message_u_allocWithAddress(osc_atom_u_getStringPtr(aa));
			if(i + 1 < argc){
				for(int j = 0; j < osc_atom_array_u_getLen(argv[i + 1]); j++){
					osc_message_u_appendAtom(m, osc_atom_u_copy(osc_atom_array_u_get(argv[i + 1], j)));
				}
				i++;
			}
			osc_bundle_u_addMsg(b, m);
		}
		/* *out = osc_atom_array_u_alloc(1); */
		/* t_osc_atom_u *a = osc_atom_array_u_get(*out, 0); */
		osc_atom_u_setBndl_u(aout, b);
		return 0;
	}
	return 1;
}

t_osc_expr *osc_expr_alloc(void)
{
	t_osc_expr *expr = (t_osc_expr *)osc_mem_alloc(sizeof(t_osc_expr));
	memset(expr, '\0', sizeof(t_osc_expr));
	return expr;
}

t_osc_expr_arg *osc_expr_arg_alloc(void)
{
	t_osc_expr_arg *arg = (t_osc_expr_arg *)osc_mem_alloc(sizeof(t_osc_expr_arg));
	memset(arg, '\0', sizeof(t_osc_expr_arg));
	return arg;
}

void osc_expr_free(t_osc_expr *e)
{
	t_osc_expr *ee = e;
	while(ee){
		t_osc_expr *next = ee->next;
		osc_expr_arg_freeList(ee->argv);
		osc_mem_free(ee);
		ee = next;
	}
}

void osc_expr_copy(t_osc_expr **dest, t_osc_expr *src)
{
	if(!src){
		return;
	}
	if(!(*dest)){
		*dest = osc_expr_alloc();
	}
	t_osc_expr *copy = *dest;
	copy->rec = src->rec;
	copy->argc = src->argc;
	t_osc_expr_arg *arg = src->argv;
	t_osc_expr_arg *first_arg_copy = NULL;
	t_osc_expr_arg *last_arg_copy = NULL;
	while(arg){
		t_osc_expr_arg *arg_copy = NULL;
		osc_expr_arg_copy(&arg_copy, arg);
		if(last_arg_copy){
			last_arg_copy->next = arg_copy;
		}
		last_arg_copy = arg_copy;
		if(!first_arg_copy){
			first_arg_copy = arg_copy;
		}
		arg = arg->next;
	}
	copy->argv = first_arg_copy;
}

void osc_expr_copyAllLinked(t_osc_expr **dest, t_osc_expr *src)
{
	if(!src){
		return;
	}
	t_osc_expr *e = src;
	while(e){
		t_osc_expr *copy = NULL;
		osc_expr_copy(&copy, e);
		if(*dest){
			osc_expr_appendExpr(*dest, copy);
		}else{
			*dest = copy;
		}
		e = osc_expr_next(e);
	}
}

void osc_expr_appendExpr(t_osc_expr *e, t_osc_expr *expr_to_append)
{
	if(!e){
		return;
	}
	t_osc_expr *ee = e;
	t_osc_expr *last = ee;

	while(ee){
		last = ee;
		ee = ee->next;
	}
	last->next = expr_to_append;
	expr_to_append->next = NULL;
}

void osc_expr_arg_clear(t_osc_expr_arg *a)
{
	if(!a){
		return;
	}
	switch(a->type){
	case OSC_EXPR_ARG_TYPE_EXPR:
		osc_expr_free(a->arg.expr);
		break;
	case OSC_EXPR_ARG_TYPE_ATOM:
		osc_atom_u_free(a->arg.atom);
		break;
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		if(a->arg.atom){
			osc_mem_free(a->arg.osc_address);
		}
		break;
	case OSC_EXPR_ARG_TYPE_FUNCTION:
		{
			t_osc_expr *e = NULL;
			osc_expr_rec_free(a->arg.func, (void **)&e);
			if(e){
				osc_expr_free(e);
			}
		}
		break;
	case OSC_EXPR_ARG_TYPE_LIST:
		{
			if(a->arg.list){
				osc_atom_array_u_free(a->arg.list);
			}
		}
		break;
	}
}

void osc_expr_arg_free(t_osc_expr_arg *a)
{
	osc_expr_arg_clear(a);
	osc_mem_free(a);
}

void osc_expr_arg_freeList(t_osc_expr_arg *a)
{
	while(a){
		t_osc_expr_arg *next = osc_expr_arg_next(a);
		osc_expr_arg_free(a);
		a = next;
	}
}

void osc_expr_setRec(t_osc_expr *e, t_osc_expr_rec *rec)
{
	if(e){
		e->rec = rec;
	}
}

t_osc_expr_rec *osc_expr_getRec(t_osc_expr *e)
{
	if(e){
		return e->rec;
	}else{
		return NULL;
	}
}

void osc_expr_setArg(t_osc_expr *e, t_osc_expr_arg *a)
{
	if(e){
		if(a){
			t_osc_expr_arg *aa = a;
			int count = 0;
			while(aa){
				count++;
				aa = aa->next;
			}
			e->argv = a;
			e->argc = count;
		}
	}
}

void osc_expr_prependArg(t_osc_expr *e, t_osc_expr_arg *a)
{
	if(e){
		if(!(e->argv)){
			e->argv = a;
			e->argc = 1;
		}else{
			t_osc_expr_arg *aa = a;
			t_osc_expr_arg *last = aa;
			int count = 0;
			while(aa){
				last = aa;
				aa = aa->next;
				count++;
			}
			if(last){
				last->next = e->argv;
				e->argv = a;
				e->argc += count;
			}
		}
	}
}

void osc_expr_appendArg(t_osc_expr *e, t_osc_expr_arg *a)
{
	if(e){
		if(!(e->argv)){
			e->argv = a;
			e->argc = 1;
		}else{
			t_osc_expr_arg *aa = e->argv;
			t_osc_expr_arg *last = aa;
			while(aa){
				last = aa;
				aa = aa->next;
			}
			if(last){
				last->next = a;
				aa = a;
				int count = 0;
				while(aa){
					count++;
					aa = aa->next;
				}
				e->argc += count;
			}
		}
	}
}

inline t_osc_expr_arg *osc_expr_getArgs(t_osc_expr *e)
{
	if(!e){
		return NULL;
	}
	return e->argv;
}

inline long osc_expr_getArgCount(t_osc_expr *e)
{
	if(!e){
		return 0;
	}
	return e->argc;
}

inline void osc_expr_setNext(t_osc_expr *e, t_osc_expr *next)
{
	if(!e){
		return;
	}
	e->next = next;
}

inline t_osc_expr *osc_expr_next(t_osc_expr *e)
{
	if(!e){
		return NULL;
	}
	return e->next;
}

inline void osc_expr_arg_setOSCAtom(t_osc_expr_arg *a, t_osc_atom_u *atom)
{
	if(!a){
		return;
	}
	a->arg.atom = atom;
	a->type = OSC_EXPR_ARG_TYPE_ATOM;
}

inline void osc_expr_arg_setFunction(t_osc_expr_arg *a, t_osc_expr_rec *rec)
{
	if(!a){
		return;
	}
	a->arg.func = rec;
	a->type = OSC_EXPR_ARG_TYPE_FUNCTION;
}

inline void osc_expr_arg_setExpr(t_osc_expr_arg *a, t_osc_expr *e)
{
	if(!a){
		return;
	}
	a->arg.expr = e;
	a->type = OSC_EXPR_ARG_TYPE_EXPR;
}

inline void osc_expr_arg_setOSCAddress(t_osc_expr_arg *a, char *osc_address)
{
	if(!a){
		return;
	}
	a->arg.osc_address = osc_address;
	a->type = OSC_EXPR_ARG_TYPE_OSCADDRESS;
}

inline void osc_expr_arg_setList(t_osc_expr_arg *a, t_osc_atom_ar_u *ar)
{
	if(!a){
		return;
	}
	a->arg.list = ar;
	a->type = OSC_EXPR_ARG_TYPE_LIST;
}

inline void osc_expr_arg_setType(t_osc_expr_arg *a, long type)
{
	if(a){
		a->type = type;
	}
}

inline int osc_expr_arg_getType(t_osc_expr_arg *a)
{
	if(a){
		return a->type;
	}
	return -1;
}

inline t_osc_atom_u *osc_expr_arg_getOSCAtom(t_osc_expr_arg *a)
{
	if(a){
		return a->arg.atom;
	}
	return NULL;
}

inline t_osc_expr *osc_expr_arg_getExpr(t_osc_expr_arg *a)
{
	if(a){
		return a->arg.expr;
	}
	return NULL;
}

inline char *osc_expr_arg_getOSCAddress(t_osc_expr_arg *a)
{
	if(a){
		return a->arg.osc_address;
	}
	return NULL;
}

inline t_osc_expr_rec *osc_expr_arg_getFunction(t_osc_expr_arg *a)
{
	if(a){
		return a->arg.func;
	}
	return NULL;
}

t_osc_err osc_expr_arg_copy(t_osc_expr_arg **dest, t_osc_expr_arg *src)
{
	if(!src){
		return OSC_ERR_NONE;
	}
	if(!(*dest)){
		*dest = osc_expr_arg_alloc();
	}
	t_osc_expr_arg *copy = *dest;
	memset(copy, '\0', sizeof(t_osc_expr_arg));
	copy->type = src->type;
	switch(src->type){
	case OSC_EXPR_ARG_TYPE_ATOM:
		osc_atom_u_copyInto(&(copy->arg.atom), src->arg.atom);
		break;
	case OSC_EXPR_ARG_TYPE_EXPR:
		osc_expr_copy(&(copy->arg.expr), src->arg.expr);
		break;
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		{
			int len = strlen(src->arg.osc_address) + 1;
			copy->arg.osc_address = osc_mem_alloc(len);
			memcpy(copy->arg.osc_address, src->arg.osc_address, len);
		}
		break;
	case OSC_EXPR_ARG_TYPE_FUNCTION:
		{
			t_osc_expr_rec *copy = NULL;
			osc_expr_rec_copy(&copy, src->arg.func, (void (*)(void **, void*))osc_expr_copyAllLinked);
			(*dest)->arg.func = copy;
		}
		break;
	}
	return OSC_ERR_NONE;
}

void osc_expr_arg_setNext(t_osc_expr_arg *a, t_osc_expr_arg *next)
{
	a->next = next;
}

int osc_expr_arg_append(t_osc_expr_arg *a, t_osc_expr_arg *arg_to_append)
{
	t_osc_expr_arg *aa = a;
	t_osc_expr_arg *next = aa;
	int count = 0;
	while(aa){
		next = aa;
		aa = aa->next;
		count++;
	}
	next->next = arg_to_append;
	return count;
}

t_osc_expr_arg *osc_expr_arg_next(t_osc_expr_arg *a)
{
	return a->next;
}

void osc_expr_funcobj_dtor(char *key, void *val)
{
	if(key){
		osc_mem_free(key);
	}
	if(val){
		osc_expr_free((t_osc_expr *)val);
	}
}

int osc_expr_format_r(t_osc_expr *fg, char *buf)
{
	if(!fg){
		return 0;
	}
	char *ptr = buf;
	ptr += sprintf(ptr, "(%s ", fg->rec->name);
	t_osc_expr_arg *f_argv = fg->argv;
	while(f_argv){
		switch(f_argv->type){
		case OSC_EXPR_ARG_TYPE_ATOM:
			{
				t_osc_atom_u *a = f_argv->arg.atom;
				long buflen = osc_atom_u_nformat(NULL, 0, a, 0);
				char buf[buflen + 1];
				osc_atom_u_nformat(buf, buflen + 1, a, 0);
				ptr += sprintf(ptr, "%s ", buf);
			}
			break;
		case OSC_EXPR_ARG_TYPE_LIST:
			{
				t_osc_atom_ar_u *ar = f_argv->arg.list;
				for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
					t_osc_atom_u *a = osc_atom_array_u_get(ar, i);
					long buflen = osc_atom_u_nformat(NULL, 0, a, 0);
					char buf[buflen + 1];
					osc_atom_u_nformat(buf, buflen + 1, a, 0);
					ptr += sprintf(ptr, "%s ", buf);
				}
			}
			break;
		case OSC_EXPR_ARG_TYPE_OSCADDRESS:
			ptr += sprintf(ptr, "%s ", f_argv->arg.osc_address);
			break;
		case OSC_EXPR_ARG_TYPE_EXPR:
			ptr += osc_expr_format_r(f_argv->arg.expr, ptr);
			break;
 		case OSC_EXPR_ARG_TYPE_FUNCTION:
			{
				t_osc_expr_rec *r = f_argv->arg.func;
				ptr += sprintf(ptr, "(lambda (");
				int numparams = osc_expr_rec_getNumRequiredArgs(r);
				char **paramnames = osc_expr_rec_getRequiredArgsNames(r);
				for(int i = 0; i < numparams; i++){
					if(i < numparams - 1){
						ptr += sprintf(ptr, "%s, ", paramnames[i]);
					}else{
						ptr += sprintf(ptr, "%s) ", paramnames[i]);
					}
				}
				t_osc_expr *e = (t_osc_expr *)osc_expr_rec_getExtra(r);
				while(e){
					ptr += osc_expr_format_r(e, ptr);
					e = osc_expr_next(e);
				}
				ptr += sprintf(ptr, ") ");
			}
			break;
		}
		f_argv = f_argv->next;
	}
	ptr--;
	ptr += sprintf(ptr, ") ");
	return ptr - buf;
}

void osc_expr_format(t_osc_expr *fg, long *buflen, char **fmt)
{
	if(!(*fmt)){
		*fmt = osc_mem_alloc(512);
	}
	long bufsize = 512, bufpos = 0;
	t_osc_expr *f = fg;
	while(f){
		if(bufpos < 256){
			*fmt = osc_mem_resize(*fmt, bufsize + 256);
		}
		if(!(*fmt)){
			return;
		}
		bufsize += 256;
		bufpos += osc_expr_format_r(f, (*fmt) + bufpos);
		bufpos += sprintf((*fmt) + bufpos, "\n");
		f = f->next;
	}
	*buflen = bufpos;
}

void osc_expr_makeFunctionBundle(void)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	int i;
	char buf[128];
	for(i = 0; i < sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec); i++){
		int j = 0;
		t_osc_expr_rec r = osc_expr_funcsym[i];
		while(r.categories[j]){
			t_osc_msg_u *m = osc_message_u_alloc();
			char *name = r.name;
			if(!strcmp(r.name, "+")){
				name = "add";
			}else if(!strcmp(r.name, "-")){
				name = "sub";
			}else if(!strcmp(r.name, "*")){
				name = "mul";
			}else if(!strcmp(r.name, "/")){
				name = "div";
			}else if(!strcmp(r.name, "%")){
				name = "mod";
			}else if(!strcmp(r.name, "++")){
				name = "inc";
			}else if(!strcmp(r.name, "--")){
				name = "dec";
			}else if(!strcmp(r.name, "+=")){
				name = "addassign";
			}else if(!strcmp(r.name, "-=")){
				name = "subassign";
			}else if(!strcmp(r.name, "*=")){
				name = "mulassign";
			}else if(!strcmp(r.name, "/=")){
				name = "divassign";
			}else if(!strcmp(r.name, "%=")){
				name = "modassign";
			}
			sprintf(buf, "/doc/category%s/%s", r.categories[j++], name);
			osc_message_u_setAddress(m, buf);
			osc_message_u_appendString(m, r.name);
			osc_bundle_u_addMsg(b, m);
		}
	}
	osc_expr_functionBundle = b;
}

void osc_expr_makeCategoryBundle(void)
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
	long len = osc_bundle_u_nserialize(NULL, 0, b);
	char *ptr = osc_mem_alloc(len);
	osc_bundle_u_nserialize(ptr, len, b);
	osc_expr_categoryBundle = osc_bundle_s_alloc(len, ptr);
	osc_bundle_u_free(b);
}

t_osc_bndl_s *osc_expr_getCategories(void)
{
	if(!osc_expr_categoryBundle){
		osc_expr_makeCategoryBundle();
	}
	return osc_expr_categoryBundle;
}

t_osc_err osc_expr_getFunctionsForCategory(char *cat, long *len, char **ptr)
{
	if(!cat){
		return OSC_ERR_INVAL;
	}
	if(!osc_expr_functionBundle){
		osc_expr_makeFunctionBundle();
	}
	char buf[128];
	if(strncmp(cat, "/doc/category", 13)){
		sprintf(buf, "/doc/category%s", cat);
	}else{
		sprintf(buf, "%s", cat);
	}
	t_osc_msg_ar_u *ar = osc_bundle_u_lookupAddress(osc_expr_functionBundle, buf, 0);
	if(ar){
		t_osc_bndl_u *bndl = osc_bundle_u_alloc();
		osc_bundle_u_addMsgArrayCopy(bndl, ar);
		osc_message_array_u_free(ar);
		*len = osc_bundle_u_nserialize(NULL, 0, bndl);
		*ptr = osc_mem_alloc(*len);
		osc_bundle_u_nserialize(*ptr, *len, bndl);
		osc_bundle_u_free(bndl);
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_getDocForFunction(char *function_name, t_osc_bndl_u **bndl)
{
	t_osc_expr_rec *rec = osc_expr_lookupFunction(function_name);
	if(rec){
		return osc_expr_formatFunctionDoc(rec, bndl);
	}else{
		return OSC_ERR_EXPR_FUNCTIONNOTFOUND;
	}
}

t_osc_err osc_expr_formatFunctionDoc(t_osc_expr_rec *rec, t_osc_bndl_u **bndl)
{
	if(!rec){
		return OSC_ERR_EXPR_FUNCTIONNOTFOUND;
	}
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_message_u *name = osc_message_u_alloc();
	osc_message_u_setAddress(name, "/doc/function/name");
	osc_message_u_appendString(name, rec->name);
	osc_bundle_u_addMsg(b, name);

	t_osc_message_u *sig = osc_message_u_alloc();
	osc_message_u_setAddress(sig, "/doc/function/signature");
	osc_message_u_appendString(sig, rec->signature);
	osc_bundle_u_addMsg(b, sig);

	t_osc_message_u *docstring = osc_message_u_alloc();
	osc_message_u_setAddress(docstring, "/doc/function/docstring");
	osc_message_u_appendString(docstring, rec->docstring);
	osc_bundle_u_addMsg(b, docstring);

	t_osc_message_u *num_required_args = osc_message_u_alloc();
	osc_message_u_setAddress(num_required_args, "/doc/function/num_required_args");
	osc_message_u_appendInt32(num_required_args, rec->num_required_args);
	osc_bundle_u_addMsg(b, num_required_args);

	t_osc_message_u *num_optional_args = osc_message_u_alloc();
	osc_message_u_setAddress(num_optional_args, "/doc/function/num_optional_args");
	osc_message_u_appendInt32(num_optional_args, rec->num_optional_args);
	osc_bundle_u_addMsg(b, num_optional_args);

	int i;
	for(i = 0; i < rec->num_required_args; i++){
		if(rec->required_args_names){
			if(rec->required_args_names[i]){
				char a[128];
				sprintf(a, "/doc/function/arg/required/name/%d", i + 1);
				t_osc_msg_u *m = osc_message_u_alloc();
				osc_message_u_setAddress(m, a);
				osc_message_u_appendString(m, rec->required_args_names[i]);
				osc_bundle_u_addMsg(b, m);

				sprintf(a, "/doc/function/arg/required/types/%d", i + 1);
				t_osc_msg_u *mt = osc_message_u_alloc();
				osc_message_u_setAddress(mt, a);
				char ts[OSC_EXPR_MAX_TYPESTRING_LEN];
				osc_expr_formatTypes(rec->required_args_types[i], ts);
				osc_message_u_appendString(mt, ts);
				osc_bundle_u_addMsg(b, mt);
			}
		}
 	}
	for(i = 0; i < abs(rec->num_optional_args); i++){
		if(rec->optional_args_names){
			if(rec->optional_args_names[i]){
				char a[128];
				sprintf(a, "/doc/function/arg/optional/name/%d", i + 1);
				t_osc_msg_u *m = osc_message_u_alloc();
				osc_message_u_setAddress(m, a);
				osc_message_u_appendString(m, rec->optional_args_names[i]);
				osc_bundle_u_addMsg(b, m);

				sprintf(a, "/doc/function/arg/optional/types/%d", i + 1);
				t_osc_msg_u *mt = osc_message_u_alloc();
				osc_message_u_setAddress(mt, a);
				char ts[OSC_EXPR_MAX_TYPESTRING_LEN];
				osc_expr_formatTypes(rec->optional_args_types[i], ts);
				osc_message_u_appendString(mt, ts);
				osc_bundle_u_addMsg(b, mt);
			}
		}
	}
	t_osc_msg_u *categories = osc_message_u_alloc();
	osc_message_u_setAddress(categories, "/doc/function/categories");
	i = 0;
	while(rec->categories[i]){
		osc_message_u_appendString(categories, rec->categories[i++]);
	}
	osc_bundle_u_addMsg(b, categories);

	*bndl = b;
	return OSC_ERR_NONE;
}

void osc_expr_formatTypes(int types, char *str)
{
	if(!str){
		return;
	}
	char *ptr = str;
	char *fmt = "%s";
	if(types & OSC_EXPR_ARG_TYPE_NUMBER){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_NUMBER]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_LIST){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_LIST]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_STRING){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_STRING]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_ATOM){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_ATOM]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_EXPR){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_EXPR]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_OSCADDRESS){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_OSCADDRESS]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_BOOLEAN){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_BOOLEAN]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_FUNCTION){
		ptr += sprintf(ptr, fmt, osc_expr_typestrings[types & OSC_EXPR_ARG_TYPE_FUNCTION]);
		fmt = ", %s";
	}
}

// these are for the global hashtab of compiled expressions.  may be removed or changed
void osc_expr_doFormatFunctionTable(char *key, void *val, void *context)
{
	struct context{
		long *buflen;
		long *bufpos;
		char **buf;
	} *ctxt = ((struct context *)context);
	if(*(ctxt->buflen) - *(ctxt->bufpos) < 64){
		*(ctxt->buf) = osc_mem_resize(*(ctxt->buf), *(ctxt->buflen) + 256);
		(*(ctxt->buflen)) += 256;
	}
	*(ctxt->bufpos) += sprintf(*(ctxt->buf) + *(ctxt->bufpos), "address %s is bound to compiled function object %p\n", key, val);
}

void osc_expr_formatFunctionTable(long *buflen, char **buf)
{
	*buflen = 256;
	long bufpos = 0;
	(*buf) = osc_mem_alloc(*buflen);
	struct context{
		long *buflen;
		long *bufpos;
		char **buf;
	} context = {buflen, &bufpos, buf};
	osc_hashtab_foreach(osc_expr_funcobj_ht, osc_expr_doFormatFunctionTable, &context);
	*buflen = bufpos;
}

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
