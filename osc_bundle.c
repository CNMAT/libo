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

t_osc_bndl osc_bndl_copy(t_osc_region r, t_osc_bndl b)
{
	if(b){
		if(osc_capi_primitive_q(r, b)){
			return osc_primitive_copy(r, b);
		}else{
			//////////////////////////////////////////////////
			// handle timetag properly
			t_osc_bndl bcopy = osc_bndl_create(r, OSC_TIMETAG_NULL);
			for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
				t_osc_msg m = osc_capi_bndl_nth(r, b, i);
				if(m){
					t_osc_msg mcopy = osc_capi_msg_alloc(r, 0);
					for(int j = 0; j < osc_capi_msg_length(r, m); j++){
						mcopy = osc_capi_msg_append_m(r, (t_osc_msg_m)mcopy, osc_bndl_copy(r, osc_capi_msg_nth(r, m, j)));
					}
					bcopy = osc_capi_bndl_append_m(r, (t_osc_bndl_m)bcopy, mcopy);
				}
			}
			return bcopy;
		}
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
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
	//if(b && pattern && osc_capi_primitive_q(r, pattern) && (osc_capi_primitive_getType(r, pattern) == OSC_TT_SYM || osc_capi_primitive_getType(r, pattern) == OSC_TT_STR)){
	for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
		t_osc_msg m = osc_capi_bndl_nth(r, b, i);
		t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
		//t_osc_cvalue_int32 res = osc_capi_primitive_strcmp(r, pattern, a);
		//if(!osc_cvalue_error(res)){
		//if(osc_cvalue_value(res) == 0){
		//return m;
		//}
		//}
		if(osc_capi_primitive_q(r, a)){
			if(osc_capi_primitive_eql(r, pattern, a)){
				return m;
			}
		}
	}
	//}
	//return NULL;
	return osc_msg_create(r);
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

t_osc_bndl osc_bndl_map(t_osc_region r, t_osc_bndl fn, t_osc_bndl args, t_osc_bndl context)
{
	if(fn && args){
		t_osc_region tmp = osc_region_getTmp(r);
		unsigned int n = ~0;
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, args); i++){
			int nn = osc_capi_msg_length(r, osc_capi_bndl_nth(r, args, i));
			if(nn < n){
				n = nn;
			}
		}
		//t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(tmp, OSC_TIMETAG_NULL, "/value"));
		t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(tmp, OSC_TIMETAG_NULL, "/result"));
		for(int i = 1; i < n; i++){
			char *p = osc_region_getPtr(tmp);
			t_osc_bndl b = osc_bndl_create(tmp, OSC_TIMETAG_NULL);
			for(int j = 0; j < osc_capi_bndl_getMsgCount(r, args); j++){
				t_osc_msg m = osc_capi_bndl_nth(r, args, j);
				if(m){
					t_osc_msg mm = osc_capi_msg_alloc(tmp, 2, osc_capi_msg_nth(r, m, 0), osc_capi_msg_nth(r, m, i));
					b = osc_capi_bndl_append(tmp, b, mm);
				}
			}
			
			v = osc_capi_msg_append(r, v, osc_bndl_copy(r, osc_bndl_applyScalarScalar(tmp, fn, osc_capi_primitive_symbol(tmp, OSC_TIMETAG_NULL, "@"), b, context)));
			osc_region_unwind(tmp, p);
		}
		osc_region_releaseTmp(tmp);
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

static t_osc_bndl _osc_bndl_lreduce(t_osc_region r, t_osc_bndl fn, t_osc_bndl lhs, t_osc_bndl rhs, t_osc_bndl context)
{
	t_osc_msg ll = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/lhs"), lhs);
	t_osc_msg rr = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/rhs"), rhs);
	t_osc_bndl out = osc_bndl_applyScalarScalar(r, fn, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "@"), osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 2, ll, rr), context);
	return out;
}

t_osc_bndl osc_bndl_lreduce(t_osc_region r, t_osc_bndl fn, t_osc_bndl args, t_osc_bndl context)
{
	if(fn && args){
		t_osc_region tmp = osc_region_getTmp(r);
		t_osc_msg argv = osc_capi_bndl_nth(r, args, 0);
		t_osc_bndl bb = _osc_bndl_lreduce(tmp, fn, osc_capi_msg_nth(r, argv, 1), osc_capi_msg_nth(r, argv, 2), context);
		for(int i = 3; i < osc_capi_msg_length(r, argv); i++){
			if(!osc_capi_primitive_q(r, bb)){
				t_osc_msg v = osc_capi_bndl_simpleLookup(r, bb, "/value");
				if(v && osc_capi_msg_length(r, v) > 1){
					t_osc_bndl vv = osc_capi_msg_nth(r, v, 1);
					if(!osc_capi_primitive_q(r, vv)){
						if(osc_capi_bndl_exists(r, vv, "/lambda")){
							t_osc_msg retm = osc_capi_bndl_simpleLookup(r, vv, "/return");
							if(retm && osc_capi_msg_length(r, retm) > 1){
								t_osc_bndl ret = osc_capi_msg_nth(r, retm, 1);
								if(ret && osc_capi_bndl_getMsgCount(r, ret) > 0){
									t_osc_msg m = osc_capi_bndl_nth(r, ret, 0);
									if(m && osc_capi_msg_length(r, m) > 0){
										t_osc_bndl address = osc_capi_msg_nth(r, m, 0);
										if(address){
											t_osc_msg rvm = osc_bndl_simpleLookup(r, vv, address);
											if(rvm && osc_capi_msg_length(r, rvm) > 1){
												bb = osc_capi_msg_nth(r, rvm, 1);
											}
										}
									}
								}
							}
						}
					}
				}
			}
			bb = _osc_bndl_lreduce(tmp, fn, bb, osc_capi_msg_nth(r, argv, i), context);
		}
		//t_osc_msg v = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(tmp, OSC_TIMETAG_NULL, "/value"), osc_bndl_copy(r, bb));
		t_osc_msg v = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(tmp, OSC_TIMETAG_NULL, "/result"), osc_bndl_copy(r, bb));
		osc_region_releaseTmp(tmp);
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

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
					// so the result should be /expr bound to the evaluated items.
					// --if ee doesn't exist, just return lhs.
					// --if ee has a single bundle bound to expr, union it into lhs.
					// --if ee has a single primitive, or a list bound to expr, stick it/them in a
					// 	/value message and return a bundle
					t_osc_msg ee = osc_capi_bndl_simpleLookup(r, e, "/expr");
					int n = osc_capi_msg_length(r, ee);
					if(ee){
						if(n > 2 || (n == 2 && osc_capi_primitive_q(r, osc_capi_bndl_nth(r, ee, 1)))){
							t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"));
							for(int i = 1; i < n; i++){
								v = osc_capi_msg_append_m(r, (t_osc_msg_m)v, osc_bndl_union(r, osc_capi_msg_nth(r, ee, i), lhs));
							}
							return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
						}else if(n == 2){
							// ee[1] is a bundle
							return osc_bndl_union(r, osc_capi_msg_nth(r, ee, 1), lhs);
						}		      
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
		}else if(OSC_TYPETAG_ISBOOL(ltt)){
			// lhs is bool:
			// if lhs is true, return a lambda of the rhs
			// if lhs is false, return a lambda that does a test and returns the result
		}else{
			if(osc_capi_primitive_q(r, rhs)){
				if(osc_capi_primitive_isIndexable(r, lhs) && OSC_TYPETAG_ISINT(osc_capi_primitive_getType(r, rhs))){
					// lhs is indexable (string, blob, etc) and rhs is an int, do a lookup
					return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"), osc_primitive_nth(r, lhs, rhs)));
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
	if(osc_capi_primitive_q(r, lhs)){
		switch(osc_capi_primitive_getType(r, rhs)){
		default:
			{
				t_osc_msg v = osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value"));
				for(int i = 0; i < osc_capi_msg_length(r, rhs); i++){
					osc_capi_msg_append_m(r, (t_osc_msg_m)v, lhs);
				}
				return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, v);
			}
		}
	}else{

	}
	return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
}

t_osc_bndl osc_bndl_apply(t_osc_region r, t_osc_msg lhs, t_osc_bndl applicator, t_osc_msg rhs, t_osc_bndl context)
{
	int rhslen = 0;
	if(!rhs || (rhslen = osc_capi_msg_length(r, rhs)) < 2){
		return osc_bndl_create(r, OSC_TIMETAG_NULL);
	}
	int lhslen = 0;
	if(!lhs || (lhslen = osc_capi_msg_length(r, lhs)) < 2){
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
			if(!lhs || osc_capi_msg_length(r, lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, osc_capi_msg_alloc(r, 2, osc_capi_msg_nth(r, lhs, 1), context), applicator, rhs, NULL);
				return e;
			}else{
				return osc_bndl_apply(r, lhs, applicator, rhs, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), bangappsym)){
			// builtin eager lhs, lazy rhs applicator !@
			if(!lhs || osc_capi_msg_length(r, lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rhs, NULL);
				return e;
			}else{
				t_osc_msg l = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, lhs, 0));
				for(int i = 1; i < osc_capi_msg_length(r, lhs); i++){
					t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, lhs, i), context);
					t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
					if(v && osc_capi_msg_length(r, v) > 1){
						for(int j = 1; j < osc_capi_msg_length(r, v); j++){
							l = osc_capi_msg_append(r, l, osc_capi_msg_nth(r, v, j));
						}
					}else{
						l = osc_capi_msg_append(r, l, e);
					}
				}
				t_osc_bndl e = osc_bndl_apply(r, l, applicator, rhs, context);
				return e;
				//return osc_bndl_apply(r, lhs, applicator, rhs, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), appbangsym)){
			// builtin lazy lhs, eager rhs applicator @!
			t_osc_msg rr = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, rhs, 0));
			for(int i = 1; i < osc_capi_msg_length(r, rhs); i++){
				t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, rhs, i), context);
				t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
				if(v && osc_capi_msg_length(r, v) > 1){
					for(int j = 1; j < osc_capi_msg_length(r, v); j++){
						rr = osc_capi_msg_append(r, rr, osc_capi_msg_nth(r, v, j));
					}
				}else{
					rr = osc_capi_msg_append(r, rr, e);
				}
			}
			if(!lhs || osc_capi_msg_length(r, lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rr, NULL);
				return e;
			}else{
				return osc_bndl_apply(r, lhs, applicator, rr, context);
			}
		}else if(osc_capi_bndl_eql(r, osc_capi_bndl_nth(r, applicator, 1), bangappbangsym)){
			// builtin eager applicator !@!
			t_osc_msg rr = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, rhs, 0));
			for(int i = 1; i < osc_capi_msg_length(r, rhs); i++){
				t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, rhs, i), context);
				t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
				if(v && osc_capi_msg_length(r, v) > 1){
					for(int j = 1; j < osc_capi_msg_length(r, v); j++){
						rr = osc_capi_msg_append(r, rr, osc_capi_msg_nth(r, v, j));
					}
				}else{
					rr = osc_capi_msg_append(r, rr, e);
				}
			}
			if(!lhs || osc_capi_msg_length(r, lhs) < 2){
				t_osc_bndl e = osc_bndl_apply(r, context, applicator, rr, NULL);
				return e;
			}else{
				t_osc_msg l = osc_capi_msg_alloc(r, 1, osc_capi_msg_nth(r, lhs, 0));
				for(int i = 1; i < osc_capi_msg_length(r, lhs); i++){
					t_osc_bndl e = osc_bndl_eval(r, osc_capi_msg_nth(r, lhs, i), context);
					t_osc_msg v = osc_capi_bndl_simpleLookup(r, e, "/value");
					if(v && osc_capi_msg_length(r, v) > 1){
						for(int j = 1; j < osc_capi_msg_length(r, v); j++){
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
			t_osc_cvalue_ptr _applicator = osc_capi_primitive_getPtr(r, osc_capi_msg_nth(r, applicator, 1));
			if(osc_cvalue_error(_applicator)){
				return b;
			}
			int app_st_len = strlen(osc_cvalue_value(_applicator));
			char app_st_buf[app_st_len + 1];
			t_osc_bndl _lhs = lhs;
			if(osc_capi_primitive_nth(r, osc_capi_msg_nth(r, applicator, 1), 0) == '!'){
				_lhs = osc_capi_bndl_simpleLookup(r, osc_bndl_eval(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, lhs), context), "/lhs");
				memcpy(app_st_buf, osc_cvalue_value(_applicator) + 1, app_st_len--);
			}else{
				memcpy(app_st_buf, osc_cvalue_value(_applicator), app_st_len + 1);
			}
			t_osc_bndl _rhs = rhs;
			if(osc_capi_primitive_nth(r, osc_capi_msg_nth(r, applicator, 1), osc_capi_primitive_length(r, applicator) - 1) == '!'){
				_rhs = osc_capi_bndl_simpleLookup(r, osc_bndl_eval(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, rhs), context), "/rhs");
				app_st_buf[app_st_len] = 0;
			}
			t_osc_bndl ls = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/lhs");
			t_osc_bndl rs = osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/rhs");
			t_osc_bndl app_lookup = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 1, ls), appsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, app_st_buf)));
			t_osc_bndl app_second = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 2, ls, app_lookup), bangappsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, 2)));
			t_osc_bndl app_outer = osc_parse_allocApplication(r, osc_capi_msg_alloc(r, 2, ls, app_second), bangappsym, osc_capi_msg_alloc(r, 2, rs, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 2, _lhs, _rhs)));
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
				if(osc_capi_bndl_eql(r, osc_capi_msg_nth(r, m, 0), osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/h"))){
					printf("**************************************************\n");
					printf("%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_capi_msg_nth(r, m, 0)))));
					printf("**************************************************\n");
				}
				t_osc_msg newm = osc_capi_msg_alloc(r, 0);
				for(int j = 0; j < osc_capi_msg_length(r, m); j++){
					t_osc_bndl e = osc_capi_msg_nth(r, m, j);
					if(e){
						t_osc_bndl ee = osc_bndl_eval(r, e, osc_bndl_union(r, b, context));
						if(!osc_capi_primitive_q(r, ee)){
							t_osc_msg v = osc_capi_bndl_simpleLookup(r, ee, "/value");
							if(v){
								for(int i = 1; i < osc_capi_msg_length(r, v); i++){
									newm = osc_capi_msg_append(r, newm, osc_capi_msg_nth(r, v, i));
								}
							}else{
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
