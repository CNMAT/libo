#include <stdarg.h>
#include <stdio.h>
#include "osc.h"
#include "osc_capi.h"
#include "osc_capi_bundle.h"
#include "osc_capi_message.h"
#include "osc_list.h"
#include "osc_timetag.h"
#include "osc_native.h"
#include "osc_capi_primitive.h"
#include "osc_match.h"


t_osc_bndl osc_capi_bndl_alloc(t_osc_region r, t_osc_timetag time, int n, ...)
{
	t_osc_list l = osc_list_alloc(r, NULL, 0);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		t_osc_msg ll = va_arg(ap, t_osc_msg);
		osc_list_append_m(r, (t_osc_list_m)l, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)ll));
	}
	va_end(ap);
	return (t_osc_bndl)l;
}

int osc_capi_bndl_getMsgCount(t_osc_region r, t_osc_bndl b)
{
	return osc_list_length((t_osc_list)b);
}

t_osc_msg osc_capi_bndl_nth(t_osc_region r, t_osc_bndl b, int32_t n)
{
	if(b && n >= 0){
		t_osc_listitem *li = osc_list_nth((t_osc_list)b, n);
		if(li){
			if(osc_list_itemGetType(li) == OSC_CAPI_TYPE_LIST){
				return (t_osc_msg)osc_list_itemGetPtr(li);
			}
		}
	}
	return osc_capi_msg_alloc(r, 0);
}

t_osc_bndl osc_capi_bndl_append(t_osc_region r, t_osc_bndl b, t_osc_msg m)
{
	if(b){
		if(m){
			//return (t_osc_bndl)osc_list_append_m(r, (t_osc_list_m)b, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)m));
			return (t_osc_bndl)osc_list_append(r, b, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)m));
		}else{
			return b;
		}
	}else{
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, m);
	}
}

t_osc_bndl_m osc_capi_bndl_append_m(t_osc_region r, t_osc_bndl_m b, t_osc_msg m)
{
	if(b){
		if(m){
			return (t_osc_bndl_m)osc_list_append_m(r, (t_osc_list_m)b, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)m));
		}else{
			return b;
		}
	}else{
		return (t_osc_bndl_m)osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, m);
	}
}

size_t osc_capi_bndl_nformatMsgNArgs(t_osc_region r, char *s, size_t n, t_osc_msg m, int level)
{
	size_t pos = 0;
	int msglen = osc_capi_msg_length(r, m);
	if(!s || n == 0){
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, NULL, 0, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, NULL, 0, bb, level + 1);
		}
		pos += snprintf(NULL, 0, " : [ ");
		for(int j = 1; j < msglen; j++){
			t_osc_bndl bb = osc_capi_msg_nth(r, m, j);
			if(osc_capi_primitive_q(r, bb)){
				pos += osc_capi_primitive_nformat(r, NULL, 0, bb);
			}else{
				pos += osc_capi_bndl_nformat(r, NULL, 0, bb, level + 1);
			}
			if(j < msglen - 1){
				pos += snprintf(NULL, 0, ", ");
			}
		}
		pos += snprintf(NULL, 0, " ]");
	}else{
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, s + pos, n - pos, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, s + pos, n - pos, bb, level + 1);
		}
		pos += snprintf(s + pos, n - pos, " : [ ");
		for(int j = 1; j < msglen; j++){
			t_osc_bndl bb = osc_capi_msg_nth(r, m, j);
			if(osc_capi_primitive_q(r, bb)){
				pos += osc_capi_primitive_nformat(r, s + pos, n - pos, bb);
			}else{
				pos += osc_capi_bndl_nformat(r, s + pos, n - pos, bb, level + 1);
			}
			if(j < msglen - 1){
				pos += snprintf(s + pos, n - pos, ", ");
			}
		}
		pos += snprintf(s + pos, n - pos, " ]");
	}
	return pos;
}

size_t osc_capi_bndl_nformatMsg1Arg(t_osc_region r, char *s, size_t n, t_osc_msg m, int level)
{
	size_t pos = 0;
	if(!s || n == 0){
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, NULL, 0, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, NULL, 0, bb, level + 1);
		}
		pos += snprintf(NULL, 0, " : ");
		bb = osc_capi_msg_nth(r, m, 1);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, NULL, 0, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, NULL, 0, bb, level + 1);
		}
	}else{
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, s + pos, n - pos, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, s + pos, n - pos, bb, level + 1);
		}
		pos += snprintf(s + pos, n - pos, " : ");
		bb = osc_capi_msg_nth(r, m, 1);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, s + pos, n - pos, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, s + pos, n - pos, bb, level + 1);
		}
	}
	return pos;
}

size_t osc_capi_bndl_nformatMsgNoArgs(t_osc_region r, char *s, size_t n, t_osc_msg m, int level)
{
	size_t pos = 0;
	if(!s || n == 0){
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, NULL, 0, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, NULL, 0, bb, level);
		}
	}else{
		// address
		t_osc_bndl bb = osc_capi_msg_nth(r, m, 0);
		if(osc_capi_primitive_q(r, bb)){
			pos += osc_capi_primitive_nformat(r, s + pos, n - pos, bb);
		}else{
			pos += osc_capi_bndl_nformat(r, s + pos, n - pos, bb, level);
		}
	}
	return pos;
}

size_t osc_capi_bndl_nformat(t_osc_region r, char *s, size_t n, t_osc_bndl b, int level)
{
	size_t pos = 0;
	char tabs[level + 2];
	for(int i = 0; i < level + 1; i++){
		tabs[i] = '\t';
	}
	tabs[level + 1] = 0;
	if(!s || n == 0){
		if(osc_capi_primitive_q(r, b)){
			return osc_capi_primitive_nformat(r, NULL, 0, b);
		}else{
			int nmsgs = osc_capi_bndl_getMsgCount(r, b);
			pos += snprintf(NULL, 0, "{\n");
			for(int i = 0; i < nmsgs; i++){
				t_osc_listitem *li = osc_list_nth((t_osc_list)b, i);
				t_osc_msg m = (t_osc_msg)osc_list_itemGetPtr(li);
				int msglen = osc_capi_msg_length(r, m);
				pos += snprintf(NULL, 0, "%s", tabs);
				if(msglen == 1){
					pos += osc_capi_bndl_nformatMsgNoArgs(r, NULL, 0, m, level);
				}else if(msglen == 2){
					pos += osc_capi_bndl_nformatMsg1Arg(r, NULL, 0, m, level);
				}else{
					pos += osc_capi_bndl_nformatMsgNArgs(r, NULL, 0, m, level);
				}
				if(i < nmsgs - 1){
					pos += snprintf(NULL, 0, ",\n");
				}
			}
			tabs[level] = '\0';
			pos += snprintf(NULL, 0, "\n%s}", tabs);
		}
	}else{
		if(osc_capi_primitive_q(r, b)){
			return osc_capi_primitive_nformat(r, s + pos, n - pos, b);
		}else{
			int nmsgs = osc_capi_bndl_getMsgCount(r, b);
			pos += snprintf(s + pos, n - pos, "{\n");
			for(int i = 0; i < nmsgs; i++){
				t_osc_listitem *li = osc_list_nth((t_osc_list)b, i);
				t_osc_msg m = (t_osc_msg)osc_list_itemGetPtr(li);
				int msglen = osc_capi_msg_length(r, m);
				pos += snprintf(s + pos, n - pos, "%s", tabs);
				if(msglen == 1){
					pos += osc_capi_bndl_nformatMsgNoArgs(r, s + pos, n - pos, m, level);
				}else if(msglen == 2){
					pos += osc_capi_bndl_nformatMsg1Arg(r, s + pos, n - pos, m, level);
				}else{
					pos += osc_capi_bndl_nformatMsgNArgs(r, s + pos, n - pos, m, level);
				}
				if(i < nmsgs - 1){
					pos += snprintf(s + pos, n - pos, ",\n");
				}
			}
			tabs[level] = '\0';
			pos += snprintf(s + pos, n - pos, "\n%s}", tabs);
		}
	}
	return pos;
}

int osc_capi_bndl_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	if(!b1 || !b2){
		return 0;
	}
	if(osc_capi_primitive_q(r, b1) && osc_capi_primitive_q(r, b2)){
		return osc_capi_primitive_eql(r, b1, b2);
	}else if(osc_capi_primitive_q(r, b1) != osc_capi_primitive_q(r, b2)){
		return 0;
	}
	if(osc_capi_bndl_getMsgCount(r, b1) != osc_capi_bndl_getMsgCount(r, b2)){
		return 0;
	}
	for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b1); i++){
		t_osc_msg m1 = osc_capi_bndl_nth(r, b1, i);
		t_osc_msg m2 = osc_capi_bndl_nth(r, b2, i);
		if(osc_capi_msg_length(r, m1) != osc_capi_msg_length(r, m2)){
			return 0;
		}
		for(int j = 0; j < osc_capi_msg_length(r, m1); j++){
			if(!osc_capi_bndl_eql(r, osc_capi_msg_nth(r, m1, j), osc_capi_msg_nth(r, m2, j))){
				return 0;
			}
		}
	}
	return 1;
}

int osc_capi_bndl_eqv(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	if(!b1 || !b2){
		return 0;
	}
	if(osc_capi_primitive_q(r, b1) && osc_capi_primitive_q(r, b2)){
		return osc_capi_primitive_eqv(r, b1, b2);
	}else if(osc_capi_primitive_q(r, b1) != osc_capi_primitive_q(r, b2)){
		return 0;
	}
	if(osc_capi_bndl_getMsgCount(r, b1) != osc_capi_bndl_getMsgCount(r, b2)){
		return 0;
	}
	char *mark = osc_region_getPtr(r);
	if(osc_capi_bndl_getMsgCount(r, osc_bndl_rcomplement(r, b1, b2)) != 0){
		osc_region_unwind(r, mark);
		return 0;
	}
	for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b1); i++){
		t_osc_msg m1 = osc_capi_bndl_nth(r, b1, i);
		t_osc_bndl address = osc_capi_msg_nth(r, m1, 0);
		
	}
	return 0;
}

t_osc_bndl osc_capi_bndl_filter(t_osc_region r, t_osc_bndl b, int (*test)(t_osc_region, t_osc_msg, void*), void *context)
{
	if(!b || !test){
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	}
	t_osc_bndl out = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
		t_osc_msg m = osc_capi_bndl_nth(r, b, i);
		if(test(r, m, context)){
			out = osc_capi_bndl_append(r, out, m);
		}
	}
	return out;
}

t_osc_bndl osc_capi_bndl_match(t_osc_region r, t_osc_bndl b, char *pattern)
{
	t_osc_bndl out = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	if(b && pattern){
		t_osc_bndl p = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, pattern);
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r, p, a, &po, &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE){
					out = osc_capi_bndl_append(r, out, m);
				}
			}
		}
	}
	return out;
}

t_osc_bndl osc_capi_bndl_matchFull(t_osc_region r, t_osc_bndl b, char *pattern)
{
	t_osc_bndl out = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
	if(b && pattern){
		t_osc_bndl p = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, pattern);
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r, p, a, &po, &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE && resi & OSC_MATCH_PATTERN_COMPLETE){
					out = osc_capi_bndl_append(r, out, m);
				}
			}
		}
	}
	return out;
}

t_osc_msg osc_capi_bndl_simpleLookup(t_osc_region r, t_osc_bndl b, char *pattern)
{
	if(b && pattern){
		t_osc_bndl p = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, pattern);
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
			t_osc_cvalue_int32 res = osc_capi_primitive_strcmp(r, p, a);
			if(!osc_cvalue_error(res)){
				if(osc_cvalue_value(res) == 0){
					return m;
				}
			}
		}
	}
	return NULL;
}

int osc_capi_bndl_exists(t_osc_region r, t_osc_bndl b, char *pattern)
{
	if(b && pattern){
		t_osc_bndl p = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, pattern);
		for(int i = 0; i < osc_capi_bndl_getMsgCount(r, b); i++){
			t_osc_msg m = osc_capi_bndl_nth(r, b, i);
			t_osc_bndl a = osc_capi_msg_nth(r, m, 0);
			int po = 0, ao = 0;
			t_osc_cvalue_int32 res = osc_capi_primitive_match(r, p, a, &po, &ao);
			if(!osc_cvalue_error(res)){
				int32_t resi = osc_cvalue_value(res);
				if(resi & OSC_MATCH_ADDRESS_COMPLETE){
					return 1;
				}
			}
		}
	}
	return 0;
}

t_osc_bndl osc_capi_bndl_apply(t_osc_region r, t_osc_fn fn, t_osc_bndl args, t_osc_bndl context)
{
	if(fn){
		return fn(r, args, context);
	}
	return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
}

void _osc_capi_bndl_print(t_osc_region r, t_osc_bndl b, char eol)
{
	if(b){
		t_osc_cvalue_ptr p = osc_capi_primitive_getPtr(r, osc_bndl_format(r, b));
		if(!osc_cvalue_error(p)){
			printf("%s%c", osc_cvalue_value(p), eol);
		}else{
			printf("error: %d\n", osc_cvalue_error(p));
		}
	}else{
		printf("bundle = NULL!\n");
	}
}

void osc_capi_bndl_print(t_osc_region r, t_osc_bndl b)
{
	_osc_capi_bndl_print(r, b, ' ');
}

void osc_capi_bndl_println(t_osc_region r, t_osc_bndl b)
{
	_osc_capi_bndl_print(r, b, '\n');
}
