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

#include "osc_expr.h"
#include "osc_expr.r"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_expr_funcdefs.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_expr_privatedecls.h"

//#define __OSC_PROFILE__
#include "osc_profile.h"

//static double rdtsc_cps = 0;

int osc_expr_eval(t_osc_expr *f, long *len, char **oscbndl, t_osc_atom_ar_u **out)
{
	return osc_expr_evalInLexEnv(f, NULL, len, oscbndl, out);
}

int osc_expr_evalInLexEnv(t_osc_expr *f,
			  t_osc_expr_lexenv *lexenv,
			  long *len,
			  char **oscbndl,
			  t_osc_atom_ar_u **out)
{
	//////////////////////////////////////////////////
	// Special functions
	//////////////////////////////////////////////////
	if(f->rec->func == osc_expr_apply){
		return osc_expr_specFunc_apply(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_map){
		return osc_expr_specFunc_map(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_lreduce || f->rec->func == osc_expr_rreduce){
		return osc_expr_specFunc_reduce(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_assign){
		return osc_expr_specFunc_assign(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_assign_to_index){
		return osc_expr_specFunc_assigntoindex(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_if){
		return osc_expr_specFunc_if(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_emptybundle){
		return osc_expr_specFunc_emptybundle(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_bound){
		return osc_expr_specFunc_bound(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_exists){
		return osc_expr_specFunc_exists(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_getaddresses){
		return osc_expr_specFunc_getaddresses(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_getmsgcount){
		return osc_expr_specFunc_getmsgcount(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_value){
		return osc_expr_specFunc_value(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_quote){
		return osc_expr_specFunc_quote(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_eval_call){
		return osc_expr_specFunc_eval(f, lexenv, len, oscbndl, out);
	}else if(f->rec->func == osc_expr_tokenize){
		return osc_expr_specFunc_tokenize(f, lexenv, len, oscbndl, out);
		//}else if(f->rec->func == osc_expr_compile){
		//return osc_expr_specFunc_compile(f, lexenv, len, oscbndl, out);
	}else{
		//////////////////////////////////////////////////
		// Call normal function
		//////////////////////////////////////////////////
		int f_argc = osc_expr_getArgCount(f);
		t_osc_expr_arg *f_argv = osc_expr_getArgs(f);
		t_osc_atom_ar_u *argv[f_argc];
		memset(argv, '\0', sizeof(argv));
		int ret = 0;
		int i = 0;
		while(f_argv){
			int ret = osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, argv + i);
			if(ret){
				if(ret == OSC_ERR_EXPR_ADDRESSUNBOUND){
					// if the type arg type is something else, it will be an expression which means an 
					// error has already been posted
					if(osc_expr_arg_getType(f_argv) == OSC_EXPR_ARG_TYPE_OSCADDRESS){
						osc_expr_err_unbound(osc_expr_arg_getOSCAddress(f_argv), osc_expr_rec_getName(osc_expr_getRec(f)));
					}
				}
				int j;
				for(j = 0; j < i; j++){
					if(argv[j]){
						osc_atom_array_u_free(argv[j]);
					}
				}
				return ret;
			}
			f_argv = f_argv->next;
			i++;
		}
	        ret = f->rec->func(f, f_argc, argv, out);
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
				   t_osc_atom_ar_u **out)
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
			osc_atom_u_copy(&a, arg->arg.atom);
		}
		return 0;
	case OSC_EXPR_ARG_TYPE_LIST:
		{
			*out = osc_atom_array_u_copy(arg->arg.list);
			return 0;
		}
	case OSC_EXPR_ARG_TYPE_EXPR:
		{
			t_osc_err e = osc_expr_evalInLexEnv(arg->arg.expr, lexenv, len, oscbndl, out);
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
							osc_atom_s_deserialize(as, &au);
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
			/*
			osc_error_handler(__FILE__,
					  __func__,
					  __LINE__,
					  OSC_ERR_EXPR_ADDRESSUNBOUND,
					  "address %s is unbound\n",
					  arg->arg.osc_address);
			*/
			return OSC_ERR_EXPR_ADDRESSUNBOUND;
		}
	}
	return OSC_ERR_INVAL; // this really shouldn't happen unless there's a bug somewhere
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
		osc_atom_u_copy(&(copy->arg.atom), src->arg.atom);
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
				long buflen = 64;
				char *buf = osc_mem_alloc(buflen);
				if(!buf){
					return 0;
				}
				osc_atom_u_format(a, &buflen, &buf);
				ptr += sprintf(ptr, "%s ", buf);
				if(buf){
					osc_mem_free(buf);
				}
			}
			break;
		case OSC_EXPR_ARG_TYPE_LIST:
			{
				t_osc_atom_ar_u *ar = f_argv->arg.list;
				long buflen = 64;
				char *buf = osc_mem_alloc(buflen);
				if(!buf){
					return 0;
				}
				for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
					osc_atom_u_format(osc_atom_array_u_get(ar, i), &buflen, &buf);
					ptr += sprintf(ptr, "%s ", buf);
				}
				if(buf){
					osc_mem_free(buf);
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

