#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "osc.h"
#include "osc_bundle.h"
#include "osc_mem.h"
#include "osc_capi_primitive.h"
#include "osc_primitive.h"
#include "osc_timetag.h"
#include "osc_typetag.h"
#include "osc_cvalue.h"
#include "osc_match.h"
#include "osc_parse.h"
#include "osc_message.h"

t_osc_bndl osc_bndl_create(t_osc_region r, t_osc_timetag t)
{
	return osc_capi_bndl_alloc(r, t, 0);
}
	
t_osc_bndl osc_bndl_format(t_osc_region r, t_osc_bndl b)
{
	size_t len = osc_capi_bndl_nformat(r, NULL, 0, b, 0);
	char buf[len + 1];
	osc_capi_bndl_nformat(r, buf, len + 1, b, 0);
	return osc_capi_primitive_string(r, OSC_TIMETAG_NULL, buf);
}

t_osc_bndl osc_bndl_getMsgCount(t_osc_region r, t_osc_bndl b)
{
	return osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, osc_capi_bndl_getMsgCount(r, b));
}

t_osc_msg osc_bndl_nth(t_osc_region r, t_osc_bndl b, t_osc_bndl n)
{
	t_osc_native nat = osc_capi_primitive_getNative(n);
	char tt = osc_native_getTypetag(nat);
	if(OSC_TYPETAG_ISINT(tt)){
		if(OSC_TYPETAG_ISINT32(tt)){
			return osc_capi_bndl_nth(r, b, osc_cvalue_value(osc_native_getInt32(nat)));
		}else if(OSC_TYPETAG_ISINT64(tt)){
			return osc_capi_bndl_nth(r, b, osc_cvalue_value(osc_native_getInt64(nat)));
		}
	}
	return osc_capi_msg_alloc(r, 0);
}

t_osc_bndl osc_bndl_match(t_osc_region r, t_osc_bndl b, t_osc_bndl pattern)
{
	if(b && pattern){
		char tt = osc_capi_primitive_getType(r, pattern);
		if(tt == OSC_TT_STR || tt == OSC_TT_SYM){
			t_osc_cvalue_ptr p = osc_capi_primitive_getPtr(r, pattern);
			if(!osc_cvalue_error(p)){
				return osc_capi_bndl_match(r, b, osc_cvalue_value(p));
			}
		}
	}
	return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
}

t_osc_msg osc_bndl_simpleLookup(t_osc_region r, t_osc_bndl b, t_osc_bndl pattern)
{
	if(b && pattern && osc_capi_primitive_q(r, pattern) && (osc_capi_primitive_getType(r, pattern) == OSC_TT_SYM || osc_capi_primitive_getType(r, pattern) == OSC_TT_STR)){
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
			t_osc_cvalue_int32 res = osc_capi_primitive_strcmp(r, pattern, a);
			if(!osc_cvalue_error(res)){
				if(osc_cvalue_value(res) == 0){
					return m;
				}
			}
		}
	}
	return NULL;
}

t_osc_bndl osc_bndl_union(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	t_osc_bndl b = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	int l1 = osc_capi_bndl_getMsgCount(r, b1);
	int l2 = osc_capi_bndl_getMsgCount(r, b2);
	char ii[l2];
	memset(ii, 0, l2);
	for(int i = 0; i < l1; i++){
		t_osc_msg m1 = osc_capi_bndl_nth(r, b1, i);
		b = osc_capi_bndl_append(r, b, m1);
		for(int j = 0; j < l2; j++){
			t_osc_msg m2 = osc_capi_bndl_nth(r, b2, j);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r,
									  osc_capi_msg_nth(r, m1, 0),
									  osc_capi_msg_nth(r, m2, 0),
									  &po,
									  &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE && resi & OSC_MATCH_PATTERN_COMPLETE){
					ii[j] = 1;
				}
			}
		}
	}
	for(int i = 0; i < l2; i++){
		if(!ii[i]){
			b = osc_capi_bndl_append(r, b, osc_capi_bndl_nth(r, b2, i));
		}
	}
	return b;
}

t_osc_bndl osc_bndl_intersection(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	t_osc_bndl b = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	int l1 = osc_capi_bndl_getMsgCount(r, b1);
	int l2 = osc_capi_bndl_getMsgCount(r, b2);
	for(int i = 0; i < l1; i++){
		t_osc_msg m1 = osc_capi_bndl_nth(r, b1, i);
		for(int j = 0; j < l2; j++){
			t_osc_msg m2 = osc_capi_bndl_nth(r, b2, j);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r,
									  osc_capi_msg_nth(r, m1, 0),
									  osc_capi_msg_nth(r, m2, 0),
									  &po,
									  &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE && resi & OSC_MATCH_PATTERN_COMPLETE){
					b = osc_capi_bndl_append(r, b, m1);
				}
			}
		}
	}
	return b;
}

t_osc_bndl osc_bndl_rcomplement(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	t_osc_bndl b = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	int l1 = osc_capi_bndl_getMsgCount(r, b1);
	int l2 = osc_capi_bndl_getMsgCount(r, b2);
	char ii[l1];
	memset(ii, 0, l1);
	for(int i = 0; i < l1; i++){
		t_osc_msg m1 = osc_capi_bndl_nth(r, b1, i);
		for(int j = 0; j < l2; j++){
			t_osc_msg m2 = osc_capi_bndl_nth(r, b2, j);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r,
									  osc_capi_msg_nth(r, m1, 0),
									  osc_capi_msg_nth(r, m2, 0),
									  &po,
									  &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE && resi & OSC_MATCH_PATTERN_COMPLETE){
					ii[i] = 1;
				}
			}
		}
	}
	for(int i = 0; i < l1; i++){
		if(!ii[i]){
			b = osc_capi_bndl_append(r, b, osc_capi_bndl_nth(r, b1, i));
		}
	}
	return b;
}
/*
struct elem
{
	t_osc_msg *msg;
	int ct;
};

void osc_bndl_incRefs(t_osc_bndl *b, t_osc_hashtab *refs, t_osc_hashtab *free)
{
	for(int i = 0; i < osc_bndl_length(b); i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		for(int j = 1; j < osc_msg_length(m) + 1; j++){
			t_osc_atom *a = osc_msg_nth(m, j);
			if(osc_atom_getTypetag(a) == OSC_TT_SYM){
				struct elem *e = (struct elem *)osc_hashtab_lookup(refs, osc_atom_getPrettyLen(a), osc_atom_getPrettyPtr(a));
				if(e){
					e->ct++;
				}else{
					//printf("undefined ref: %s\n", osc_atom_getPrettyPtr(a));
				}
			}else if(osc_atom_getTypetag(a) == OSC_TT_BNDL){
				//osc_bndl_incRefs(osc_atom_getBndlPtr(a), refs, free);
			}
		}
	}
}

void osc_bndl_decRefs(int *_qlen, t_osc_atom **q, int *_llen, t_osc_atom **l, t_osc_hashtab *ht)
{
	int qlen = *_qlen;
	int llen = *_llen;
	while(qlen){
		t_osc_atom *a = q[--qlen];
		l[llen++] = a;
		struct elem *e = (struct elem *)osc_hashtab_lookup(ht, osc_atom_getPrettyLen(a), osc_atom_getPrettyPtr(a));
		if(!e){
			l[llen--] = NULL;
			continue;
		}
		t_osc_msg *m = e->msg;
		for(int i = 1; i < osc_msg_length(m) + 1; i++){
			t_osc_atom *aa = osc_msg_nth(m, i);
			if(osc_atom_getTypetag(aa) == OSC_TT_SYM){
				struct elem *ee = (struct elem *)osc_hashtab_lookup(ht, osc_atom_getPrettyLen(aa), osc_atom_getPrettyPtr(aa));
				if(!ee){
					continue;
				}
				ee->ct--;
				if(ee->ct == 0){
					q[qlen++] = aa;
				}
			}else if(osc_atom_getTypetag(aa) == OSC_TT_BNDL){
				//osc_bndl_decRefs(&qlen, q, &llen, l, ht);
			}
		}
	}
	*_qlen = qlen;
	*_llen = llen;
}

t_osc_bndl *osc_bndl_toposort(t_osc_bndl *b)
{
	struct elem msgs[osc_bndl_length(b)];
	t_osc_hashtab *ht = osc_hashtab_new(-1, NULL);
	for(int i = 0; i < osc_bndl_length(b); i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		msgs[i] = (struct elem){m, 0};
		t_osc_atom *address = osc_msg_nth(m, 0);
		osc_hashtab_store(ht, osc_atom_getPrettyLen(address), osc_atom_getPrettyPtr(address), (void *)(msgs + i));
	}
	t_osc_hashtab *free = osc_hashtab_new(-1, NULL);
	osc_bndl_incRefs(b, ht, free);

	t_osc_atom *q[osc_bndl_length(b)];
	t_osc_atom *l[osc_bndl_length(b)];
	int qlen = 0, llen = 0;

	for(int i = 0; i < osc_bndl_length(b); i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		t_osc_atom *address = osc_msg_nth(m, 0);
		struct elem *e = (struct elem *)osc_hashtab_lookup(ht, osc_atom_getPrettyLen(address), osc_atom_getPrettyPtr(address));
		if(!e){
			continue;
		}
		if(e->ct == 0){
			q[qlen++] = address;
		}
	}
	osc_bndl_decRefs(&qlen, q, &llen, l, ht);
	t_osc_bndl *out = osc_bndl_alloc(osc_bndl_getTimetag(b), 0);
	t_osc_bndl *status = osc_bndl_alloc(osc_bndl_getTimetag(b), 0);
	if(llen == osc_bndl_length(b)){
		for(int i = llen - 1; i >= 0; i--){
			char *st = osc_atom_getPrettyPtr(l[i]);
			struct elem *e = (struct elem *)osc_hashtab_lookup(ht, osc_atom_getPrettyLen(l[i]), st);
			t_osc_msg *m = e->msg;
			osc_bndl_append_m((t_osc_bndl_m *)out, osc_msg_retain(m));
		}
		osc_hashtab_destroy(ht);
		return out;
	}else{
		osc_bndl_release(out);
		out = osc_bndl_appendStatus(b, osc_atom_false, osc_atom_ps_circularreference, 0);
		osc_hashtab_destroy(ht);
		return out;
	}
}
*/
/*
void osc_bndl_evalSimpleLookup(t_osc_region r, t_osc_msg sourcemsg, t_osc_msg selectormsg, t_osc_bndl context, t_osc_msg out)
{
	for(int j = 1; j < osc_capi_msg_length(selectormsg); j++){
		t_osc_bndl selector = osc_capi_msg_nth(r, selectormsg, j);
		if(!osc_capi_primitive_q(r, selector)){
			// if selector is a bundle, see if it has a /value message.
			// if not, eval it and see if one shows up.
			// otherwise, bail.
			t_osc_msg v = osc_capi_bndl_simpleLookup(r, selector, "/value");
			if(v){
				osc_bndl_evalSimpleLookup(r, sourcemsg, v, context, out);
				return;
			}else{
				t_osc_bndl b = osc_bndl_eval(r, selector, context);
				v = osc_capi_bndl_simpleLookup(r, b, "/value");
				if(v){
					osc_bndl_evalSimpleLookup(r, sourcemsg, v, context, out);
				}else{
					osc_capi_msg_append(r, out, osc_parse_allocLookup(r, sourcemsg, osc_capi_msg_alloc(r, 2, osc_capi_msg_nth(r, selectormsg, 0), b)));
				}
			}
		}else{
			// selector is a primitive
			int sourcelen = osc_capi_msg_length(sourcemsg);
			char seltype = osc_capi_primitive_getType(r, selector);
			if(seltype == OSC_TT_STR || seltype == OSC_TT_SYM){
				// selector is a string or symbol---treat them the same
				if(sourcelen > 2){
					// not sure what to do with a list as the source and a string as the selector
					printf("%s: yikes\n", __func__);
				}else{
					t_osc_bndl source;
					if(sourcelen == 1){
						// no source, so use the context
						source = context;
					}else{
						// source is the 1st element of the source message
						t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, sourcemsg, 1), context);
						t_osc_msg m = osc_capi_bndl_simpleLookup(r, e, "/value");
						if(m){
							osc_bndl_evalSimpleLookup(r, m, selectormsg, context, out);
							return;
						}else{
							source = e;
						}
					}
					if(!osc_capi_primitive_q(r, source)){
						t_osc_msg m = osc_bndl_simpleLookup(r, source, selector);
						if(m){
							for(int k = 0; k < osc_capi_msg_length(m); k++){
								osc_capi_msg_append(r, out, osc_capi_msg_nth(r, m, k));
							}
						}else{
							osc_capi_msg_append(r, out, osc_parse_allocLookupWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)source)), osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)selector))));
						}
					}else{
						osc_capi_msg_append(r, out, osc_parse_allocLookupWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)source)), osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)selector))));
					}
				}
			}else if(OSC_TYPETAG_ISINT(seltype)){
				if(sourcelen > 2){
					t_osc_cvalue_int32 ii = osc_capi_primitive_getInt32(r, selector);
					if(!osc_cvalue_error(ii)){
						t_osc_bndl b = osc_capi_msg_nth(r, sourcemsg, osc_cvalue_value(ii));
						if(b){
							osc_capi_msg_append(r, out, b);
						}else{
							osc_capi_msg_append(r, out, osc_parse_allocLookup(r, sourcemsg, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/selector"), selector)));
						}
					}else{

					}
				}else{
					t_osc_bndl source;
					if(sourcelen == 1){
						source = context;
					}else{
						t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, sourcemsg, 1), context);
						t_osc_msg m = osc_capi_bndl_simpleLookup(r, e, "/value");
						if(m){
							osc_bndl_evalSimpleLookup(r, m, selectormsg, context, out);
							return;
						}else{
							source = e;
						}
					}
					if(!osc_capi_primitive_q(r, source)){
						t_osc_msg m = osc_bndl_nth(r, source, selector);
						if(m){
							for(int k = 0; k < osc_capi_msg_length(m); k++){
								osc_capi_msg_append(r, out, osc_capi_msg_nth(r, m, k));
							}
						}else{
							osc_capi_msg_append(r, out, osc_parse_allocLookupWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)source)), osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)selector))));
						}
					}else{
						osc_capi_msg_append(r, out, osc_parse_allocLookupWithLists(r, osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)source)), osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)selector))));
					}
				}
			}else{
				// put a message in here?
				//osc_capi_msg_append(r, out, );
			}
		}
	}
}
*/
/*
t_osc_bndl osc_bndl_apply(t_osc_region r, t_osc_bndl fn, t_osc_bndl arg, t_osc_bndl context)
{
	printf("ENTER %s\n", __func__);
	printf("%s: fn:\n%s\n", __func__, osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, fn))));
	printf("%s: args:\n%s\n", __func__, osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, arg))));
	if(osc_capi_primitive_q(r, fn)){
		if(osc_capi_primitive_getType(r, fn) == OSC_TT_FN){
			t_osc_cvalue_fn cfn = osc_capi_primitive_getFn(r, fn);
			if(!osc_cvalue_error(cfn)){
				return osc_cvalue_value(cfn)(r, arg, context);
			}else{
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
			}
		}else{
			printf("shithole\n");
			return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
		}
	}else{
		// fn is a bundle
		if(osc_capi_bndl_exists(r, fn, "/lambda")){
			// fn is a lambda
			printf("%s: lambda:\n", __func__);
			printf("%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, fn))));
			t_osc_bndl exprbndl = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/expr")));
			//printf("this guy:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_bndl_eval(r, osc_bndl_intersection(r, fn, exprbndl), osc_bndl_union(r, osc_bndl_rcomplement(r, fn, exprbndl), context))))));
			t_osc_bndl strict = osc_capi_bndl_nth(r, osc_capi_bndl_simpleLookup(r, fn, "/args/strict"), 1);
			if(strict && osc_capi_bndl_getMsgCount(r, strict) > 0){
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				
				t_osc_bndl old = strict, new = osc_bndl_eval(r, osc_bndl_intersection(r, arg, strict), context);
				while(!osc_capi_bndl_eql(r, new, old)){
					printf("strict was:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, old))));
					printf("again\n");
					old = new;
					new = osc_bndl_eval(r, old, context);
					printf("strict is:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, new))));
				}
				strict = new;
				printf("strict is:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, strict))));
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				
			}
			//t_osc_bndl e = osc_bndl_eval(r, osc_bndl_intersection(r, fn, exprbndl), osc_bndl_union(r, osc_bndl_union(r, strict, arg), osc_bndl_union(r, osc_bndl_rcomplement(r, fn, exprbndl), context)));
			t_osc_bndl e = osc_bndl_eval(r, osc_bndl_intersection(r, fn, exprbndl), osc_bndl_union(r, osc_bndl_union(r, strict, arg), osc_bndl_union(r, osc_bndl_rcomplement(r, fn, exprbndl), context)));
			printf("***\n%s\n***\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, e))));
			
			if(e && !osc_capi_primitive_q(r, e)){
				t_osc_msg ee = osc_capi_bndl_simpleLookup(r, e, "/expr");
				int n = osc_capi_msg_length(ee);
				if(ee && n > 1){
					t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"));
					for(int i = 1; i < n; i++){
						osc_capi_msg_append(r, v, osc_bndl_union(r, osc_capi_msg_nth(r, ee, i), fn));
					}
					return osc_bndl_union(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v), fn);
				}else{
					return fn;
				}
			}else{
				return fn;
			}
		}else{
			t_osc_bndl e = osc_bndl_eval(r, fn, context);
		        t_osc_bndl v = osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, e, "/value"), 1);
			if(v){
				return osc_bndl_apply(r, v, arg, context);
			}else{
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
			}
		}
	}
	return NULL;
}
*/

/*
{/lambda} @ {} : function application
{/lambda} @ /sym ; lookup /sym in bndl
{/lambda} @ "string" ; lookup string in bndl
{/lambda} @ int : get nth msg out of bndl
{/lambda} @ [] : map each element of the list onto the bndl

{} @ {} : union
{} @ /sym ; lookup /sym in bndl
{} @ "string" ; lookup string in bndl
{} @ int : get nth msg out of bndl
{} @ [] : map each element of the list onto the bndl

[] @ {} : map each element of the list
[] @ /sym : map each element of the list
[] @ "string" : map each element of the list
[] @ int : get the nth element of the list
[] @ [] : element by element map

"string" @ 
 */

t_osc_bndl osc_bndl_applyScalarScalar(t_osc_region r, t_osc_bndl lhs, t_osc_bndl applicator, t_osc_bndl rhs, t_osc_bndl context)
{
	if(!osc_capi_primitive_q(r, lhs)){
		if(!osc_capi_primitive_q(r, rhs)){
			if(osc_capi_bndl_exists(r, lhs, "/lambda")){
				// lambda bndl -> apply
				// preconditions etc.
				t_osc_bndl exprbndl = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/expr")));
				// pull out /expr and evaluate it in the union of rhs, lhs, and context
				t_osc_bndl expr = osc_bndl_intersection(r, lhs, exprbndl);
				t_osc_bndl ctxt = osc_bndl_union(r, rhs, osc_bndl_union(r, osc_bndl_eval(r, osc_bndl_rcomplement(r, lhs, exprbndl), context), context));
				t_osc_bndl e = osc_bndl_eval(r, expr, ctxt);
				if(e && !osc_capi_primitive_q(r, e)){
					// the bundle we evaluated just had one message with the address /expr,
					// so the result should be /expr bound to the evaluated items
					t_osc_msg ee = osc_capi_bndl_simpleLookup(r, e, "/expr");
					int n = osc_capi_msg_length(ee);
					if(ee && n > 1){
						t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"));
						for(int i = 1; i < n; i++){
							t_osc_msg retm = osc_capi_bndl_simpleLookup(r, lhs, "/return");
							if(retm && osc_capi_msg_length(retm) > 1){

							}
							v = osc_capi_msg_append(r, v, osc_bndl_union(r, osc_capi_msg_nth(r, ee, i), lhs));
						}
						return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
					}else{
						return lhs;
					}
				}else{
					// this shouldn't really happen
					return lhs;
				}
			}else{
				// bndl bndl -> union
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), osc_bndl_union(r, lhs, rhs)));
			}
		}else{
			switch(osc_capi_primitive_getType(r, rhs)){
			case OSC_TT_SYM:
			case OSC_TT_STR:
				// bndl sym/string -> lookup
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_prepend(r, osc_bndl_simpleLookup(r, lhs, rhs), osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value")));
			case OSC_TT_I8:
			case OSC_TT_U8:
			case OSC_TT_I16:
			case OSC_TT_U16:
			case OSC_TT_I32:
			case OSC_TT_U32:
			case OSC_TT_I64:
			case OSC_TT_U64:
				// bndl int -> nth
				//return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_prepend(r, osc_bndl_nth(r, lhs, rhs), osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value")));
				{
					t_osc_bndl e = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_prepend(r, osc_bndl_nth(r, lhs, rhs), osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value")));
					return e;
				}
			case OSC_TT_FN:
				printf("cfn application\n");
				{
					t_osc_cvalue_fn cfn = osc_capi_primitive_getFn(r, rhs);
					if(!osc_cvalue_error(cfn)){
						return osc_cvalue_value(cfn)(r, rhs, context);
					}else{
						return lhs;
					}
				}
			default: return lhs;
			}
		}
	}else{
		// lhs is primitive.
		// if it's a native function, call it
		char ltt = osc_capi_primitive_getType(r, lhs);
		char rtt = osc_capi_primitive_getType(r, rhs);
		if(ltt == OSC_TT_FN){
			t_osc_cvalue_fn cv = osc_capi_primitive_getFn(r, lhs);
			if(osc_cvalue_error(cv)){
				return osc_bndl_create(r, OSC_TIMETAG_NULL);
			}else{
				t_osc_fn fn = osc_cvalue_value(cv);
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), fn(r, rhs, context)));
			}
		}else{
			if(osc_capi_primitive_q(r, rhs)){
				if(osc_capi_primitive_isIndexable(r, lhs) && OSC_TYPETAG_ISINT(osc_capi_primitive_getType(r, rhs))){
					return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), osc_primitive_nth(r, lhs, rhs)));
				}else if(ltt == OSC_TT_FN){

				}
			}else{

			}
		}
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

t_osc_bndl osc_bndl_applyListScalar(t_osc_region r, t_osc_msg lhs, t_osc_bndl applicator, t_osc_bndl rhs, t_osc_bndl context)
{
	if(!osc_capi_primitive_q(r, rhs)){
		// not sure what to do with a list on the left and bundle on the right...
		return osc_bndl_create(r, OSC_TIMETAG_NULL);
	}else{
		switch(osc_capi_primitive_getType(r, rhs)){
		case OSC_TT_I8:
		case OSC_TT_U8:
		case OSC_TT_I16:
		case OSC_TT_U16:
		case OSC_TT_I32:
		case OSC_TT_U32:
		case OSC_TT_I64:
		case OSC_TT_U64:
			return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), osc_msg_nth(r, lhs, rhs)));
		}
	}
}

t_osc_bndl osc_bndl_applyScalarList(t_osc_region r, t_osc_bndl lhs, t_osc_bndl applicator, t_osc_msg rhs, t_osc_bndl context)
{
	
}

t_osc_bndl osc_bndl_apply(t_osc_region r, t_osc_msg lhs, t_osc_bndl applicator, t_osc_msg rhs, t_osc_bndl context)
{
	int rhslen = 0;
	if(!rhs || (rhslen = osc_capi_msg_length(rhs)) < 2){
		return osc_bndl_create(r, OSC_TIMETAG_NULL);
	}
	int lhslen = 0;
	if(!lhs || (lhslen = osc_capi_msg_length(lhs)) < 2){
		if(rhslen > 2){
			return osc_bndl_applyScalarList(r, context, applicator, rhs, NULL);
		}else{
			return osc_bndl_applyScalarScalar(r, context, applicator, osc_capi_msg_nth(r, rhs, 1), NULL);
		}
	}else{
		if(rhslen == 2){
			if(lhslen == 2){
				return osc_bndl_applyScalarScalar(r, osc_capi_msg_nth(r, lhs, 1), applicator, osc_capi_msg_nth(r, rhs, 1), context);
			}else{
				return osc_bndl_applyListScalar(r, lhs, applicator, osc_capi_msg_nth(r, rhs, 1), context);
			}
		}else{
			if(lhslen == 2){
				return osc_bndl_applyScalarList(r, osc_capi_msg_nth(r, lhs, 1), applicator, rhs, context);
			}else{
				// both are lists
				int n = lhslen < rhslen ? rhslen : lhslen;
				t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"));
				for(int i = 1; i < n; i++){
					v = osc_capi_msg_append(r, v, osc_bndl_applyScalarScalar(r, osc_capi_msg_nth(r, lhs, i), applicator, osc_capi_msg_nth(r, rhs, i), context));
				}
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
			}
		}
	}
}

t_osc_bndl _osc_bndl_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	if(!b){
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	}
	if(osc_capi_primitive_q(r, b)){
		return b;
	}
	if(osc_capi_bndl_exists(r, b, "@")){
		t_osc_msg applicator = osc_capi_bndl_simpleLookup(r, b, "@");
		t_osc_msg lhs = osc_capi_bndl_simpleLookup(r, b, "/lhs");
		t_osc_msg rhs = osc_capi_bndl_simpleLookup(r, b, "/rhs");
		t_osc_bndl appsym = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@");
		t_osc_bndl bangappsym = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "!@");
		t_osc_bndl appbangsym = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@!");
		t_osc_bndl bangappbangsym = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "!@!");
		
		if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), appsym)){
			// builtin lazy applicator
			if(!lhs || osc_capi_msg_length(lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, osc_capi_msg_alloc(r, 2, osc_capi_msg_nth(r, lhs, 1), context), applicator, rhs, NULL);
				return e;
			}else{
				return osc_bndl_apply(r, lhs, applicator, rhs, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), bangappsym)){
			// builtin eager lhs, lazy rhs applicator !@
			if(!lhs || osc_capi_msg_length(lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rhs, NULL);
				return e;
			}else{
				t_osc_msg l = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, lhs, 0));
				for(int i = 1; i < osc_capi_msg_length(lhs); i++){
					t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, lhs, i), context);
					t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
					if(v && osc_capi_msg_length(v) > 1){
						for(int j = 1; j < osc_capi_msg_length(v); j++){
							l = osc_capi_msg_append(r, l, osc_capi_msg_nth(r, v, j));
						}
					}else{
						printf("i got no values!\n");
					}
				}
				t_osc_bndl e = osc_bndl_apply(r, l, applicator, rhs, context);
				return e;
				//return osc_bndl_apply(r, lhs, applicator, rhs, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), appbangsym)){
			// builtin lazy lhs, eager rhs applicator @!
			t_osc_msg rr = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, rhs, 0));
			for(int i = 1; i < osc_capi_msg_length(rhs); i++){
				t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, rhs, i), context);
				t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
				if(v && osc_capi_msg_length(v) > 1){
					for(int j = 1; j < osc_capi_msg_length(v); j++){
						rr = osc_capi_msg_append(r, rr, osc_capi_msg_nth(r, v, j));
					}
				}else{
					rr = osc_capi_msg_append(r, rr, e);
				}
			}
			if(!lhs || osc_capi_msg_length(lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rr, NULL);
				return e;
			}else{
				return osc_bndl_apply(r, lhs, applicator, rr, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), bangappbangsym)){
			// builtin eager applicator !@!
			t_osc_msg rr = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, rhs, 0));
			for(int i = 1; i < osc_capi_msg_length(rhs); i++){
				t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, rhs, i), context);
				t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
				if(v && osc_capi_msg_length(v) > 1){
					for(int j = 1; j < osc_capi_msg_length(v); j++){
						rr = osc_capi_msg_append(r, rr, osc_capi_msg_nth(r, v, j));
					}
				}else{
					rr = osc_capi_msg_append(r, rr, e);
				}
			}
			if(!lhs || osc_capi_msg_length(lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rr, NULL);
				return e;
			}else{
				t_osc_msg l = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, lhs, 0));
				for(int i = 1; i < osc_capi_msg_length(lhs); i++){
					t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, lhs, i), context);
					t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
					if(v && osc_capi_msg_length(v) > 1){
						for(int j = 1; j < osc_capi_msg_length(v); j++){
							l = osc_capi_msg_append(r, l, osc_capi_msg_nth(r, v, j));
						}
					}else{
						rr = osc_capi_msg_append(r, rr, e);
					}
				}
				t_osc_bndl e = osc_bndl_apply(r, l, applicator, rr, context);
				return e;
			}
		}else{
			// user applicator---make a normal application and eval
			t_osc_bndl ls = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/lhs");
			t_osc_bndl rs = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/rhs");
			t_osc_bndl app_lookup = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 1, ls), appsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_msg_nth(r, applicator, 1)));
			t_osc_bndl app_second = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 2, ls, app_lookup), bangappsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, 2)));
			t_osc_bndl app_outer = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 2, ls, app_second), bangappsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 2, lhs, rhs)));
			t_osc_bndl e = osc_bndl_eval(r, app_outer, context);
			//t_osc_msg v = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), e);
			//return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
			return e;
		}
	}else if(osc_capi_bndl_exists(r, b, "/lambda")){
		return b;
	}else{
		t_osc_bndl out = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			if(m){
				//printf("**************************************************\n");
				//printf("%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_capi_msg_nth(r, m, 0)))));
				//printf("**************************************************\n");
				t_osc_msg newm = osc_capi_msg_alloc(r, 0);
				for(int j = 0; j < osc_capi_msg_length(m); j++){
					t_osc_bndl e = osc_capi_msg_nth(r, m, j);
					if(e){
						t_osc_bndl ee = osc_bndl_eval(r, e, osc_bndl_union(r, b, context));
						if(!osc_capi_primitive_q(r, ee)){
							t_osc_msg v = osc_capi_bndl_simpleLookup(r, ee, "/value");
							if(v){
								for(int i = 1; i < osc_capi_msg_length(v); i++){
									newm = osc_capi_msg_append(r, newm, osc_capi_msg_nth(r, v, i));
								}
							}else{
								printf("no v\n");
								newm = osc_capi_msg_append(r, newm, ee);
							}
						}else{
							newm = osc_capi_msg_append(r, newm, ee);
						}
					}
				}
				out = osc_capi_bndl_append(r, out, newm);
			}
		}
		return out;
	}
}

t_osc_bndl osc_bndl_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	t_osc_bndl old = NULL, new = b;
	while(!osc_capi_bndl_eql(r, old, new)){
		old = new;
		new = _osc_bndl_eval(r, old, context);
	}
	return new;
}
