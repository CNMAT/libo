#include "osc.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_mem.h"
#include "osc_pvec.h"
#include "osc_timetag.h"
#include "osc_byteorder.h"
#include "osc_obj.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static int osc_bndl_changeRefCount(t_osc_bndl *b, int amount);
int osc_bndl_getRefCount(t_osc_bndl *b);

#pragma pack(push)
#pragma pack(4)
struct _osc_bndl
{
	t_osc_obj obj;
	t_osc_timetag timetag;
	t_osc_pvec2 *msgs;
	int serialized_len;
	char *serialized_ptr;
	int pretty_len;
	char *pretty_ptr;
	int static_bndl;
};
#pragma pack(pop)

#define tt(b) ((b)->timetag)
#define m(b) ((b)->msgs)
#define sl(b) ((b)->serialized_len)
#define sp(b) ((b)->serialized_ptr)
#define pl(b) ((b)->pretty_len)
#define pp(b) ((b)->pretty_ptr)
#define rc(b) ((b)->obj.refcount)
#define st(b) ((b)->static_bndl)

t_osc_bndl _osc_bndl_empty = {{-1, NULL}, OSC_TIMETAG_NULL, NULL, OSC_HEADER_SIZE, OSC_EMPTY_HEADER, 2, "{}", 1};
t_osc_bndl *osc_bndl_empty = &_osc_bndl_empty;

#define OSC_BNDL_ALLOC(varname, timetag, pvec2, serialized_len, serialized_ptr, pretty_len, pretty_ptr, refcount) \
	void *OSC_UID(__osc_bndl_alloc_ptr__) = osc_mem_alloc(sizeof(t_osc_bndl));\
	t_osc_bndl OSC_UID(__osc_bndl_alloc_b__) = {{refcount, osc_bndl_free}, timetag, pvec2, serialized_len, serialized_ptr, pretty_len, pretty_ptr, 0}; \
	memcpy(OSC_UID(__osc_bndl_alloc_ptr__), &OSC_UID(__osc_bndl_alloc_b__), sizeof(t_osc_bndl));\
	t_osc_bndl *varname = (t_osc_bndl *)OSC_UID(__osc_bndl_alloc_ptr__);

t_osc_bndl *osc_bndl_allocWithPvec2(t_osc_timetag timetag, t_osc_pvec2 *pvec2)
{
	OSC_BNDL_ALLOC(ret, timetag, pvec2, 0, NULL, 0, NULL, 1);
	return ret;
}

t_osc_bndl *osc_bndl_alloc(t_osc_timetag timetag, int n, ...)
{
	t_osc_pvec2 *pvec2 = osc_pvec2_alloc(osc_msg_release);

	va_list argp;
	va_start(argp, n);
	for(int i = 0; i < n; i++){
		t_osc_msg *m = va_arg(argp, t_osc_msg *);
		pvec2 = osc_pvec2_assocN_m(pvec2, i, m);
	}
	va_end(argp);
	OSC_BNDL_ALLOC(b, timetag, pvec2, 0, NULL, 0, NULL, 1);
	return b;
}

t_osc_bndl *osc_bndl_retain(t_osc_bndl *b)
{
	if(b && st(b) == 0){
		osc_bndl_changeRefCount((t_osc_bndl *)b, 1);
	}
	return b;
}

t_osc_bndl *osc_bndl_clone(t_osc_bndl *b)
{
	if(b){
		int sl = sl(b);
		char *sp = NULL;
		if(sp(b) && sl){
			sp = osc_mem_alloc(sl);
			memcpy(sp, sp(b), sl);
		}
		int pl = pl(b);
		char *pp = NULL;
		if(pp(b) && pl){
			pp = osc_mem_alloc(pl);
			memcpy(pp, pp(b), pl);
		}
		OSC_BNDL_ALLOC(clone, tt(b), osc_pvec2_copy(m(b)), sl, sp, pl, pp, 1);
		return clone;
	}
	return NULL;
}

void osc_bndl_free(void *_b)
{
	if(_b){
		t_osc_bndl *b = (t_osc_bndl *)_b;
		if(st(b)){
			return;
		}
		osc_pvec2_release(m(b));
		char *p = sp(b);
		if(p){
			osc_mem_free(p);
		}
		p = pp(b);
		if(p){
			osc_mem_free(p);
		}
		memset((void *)b, 0, sizeof(t_osc_bndl));
		osc_mem_free((void *)b);
	}
}

void osc_bndl_release(t_osc_bndl *b)
{
	if(b && st(b) == 0){
		int rc = osc_bndl_changeRefCount(b, -1);
		if(rc == 0){
			osc_bndl_free((void *)b);
		}
	}
}

static int osc_bndl_changeRefCount(t_osc_bndl *b, int amount)
{
	if(b){
		rc((t_osc_bndl_m *)b) += amount;
		return rc(b);
	}
	return -1;
}

t_osc_msg *osc_bndl_serializeFn(t_osc_msg *m, t_osc_bndl *context)
{
	return osc_msg_serialize(m);
}

static void _osc_bndl_serialize(t_osc_bndl *b, int *len, char **ptr)
{
	if(!b){
		return;
	}
	t_osc_bndl *bb = osc_bndl_map(osc_bndl_serializeFn, b, NULL);
	int bndllen = osc_bndl_length(bb);
	char *ptrs[bndllen];
	int lens[bndllen];
	int totallen = OSC_HEADER_SIZE;
	for(int i = 0; i < bndllen; i++){
		t_osc_msg *m = osc_bndl_nth(bb, i);
		int len = osc_msg_getSerializedLen(m);
		char *ptr = osc_msg_getSerializedPtr(m);
		ptrs[i] = ptr;
		lens[i] = len;
		totallen += len;
	}
	char *buf = osc_mem_alloc(totallen);
	char *p = buf;
	memcpy(p, OSC_EMPTY_HEADER, OSC_HEADER_SIZE);
	p += OSC_HEADER_SIZE;
	for(int i = 0; i < bndllen; i++){
		memcpy(p, ptrs[i], lens[i]);
	}
	*len = totallen;
	*ptr = buf;
	osc_bndl_release(bb);
}

t_osc_bndl_m *osc_bndl_serialize_m(t_osc_bndl_m *b)
{
	if(!b){
		OSC_BNDL_ALLOC(ret, OSC_TIMETAG_NULL, osc_pvec2_alloc(osc_msg_release), 0, NULL, 0, NULL, 1);
		return (t_osc_bndl_m *)ret;
	}
	if(sl(b) && sp(b)){
		return b;
	}
	int len = 0;
	char *ptr = NULL;
	_osc_bndl_serialize((t_osc_bndl *)b, &len, &ptr);
	if(ptr){
		sl(b) = len;
		sp(b) = ptr;
	}
	return b;
}

t_osc_bndl *osc_bndl_serialize(t_osc_bndl *b)
{
	if(!b){
		OSC_BNDL_ALLOC(ret, OSC_TIMETAG_NULL, osc_pvec2_alloc(osc_msg_release), 0, NULL, 0, NULL, 1);
		return (t_osc_bndl_m *)ret;
	}
	if(sl(b) && sp(b)){
		return osc_bndl_retain(b);
	}
	int len = 0;
	char *ptr = NULL;
	_osc_bndl_serialize(b, &len, &ptr);
	t_osc_bndl *ret = osc_bndl_clone(b);
	if(ptr){
		sl((t_osc_bndl_m *)ret) = len;
		sp((t_osc_bndl_m *)ret) = ptr;
	}
	return ret;
}

static void _osc_bndl_format(t_osc_bndl *b, int *_len, char **_buf, int level)
{
	if(!b){
		return;
	}
	int n = osc_bndl_length(b);
	if(n == 0){
		*_len = 3;
		*_buf = osc_mem_alloc(4);
		(*_buf)[0] = '{';
		(*_buf)[1] = ' ';
		(*_buf)[2] = '}';
		(*_buf)[3] = 0;
	}else{
		t_osc_msg *msgs[n];
		int len = 0;
		char tabs[level + 2];
		char *tabsp = tabs;
		for(int i = 0; i < level + 1; i++){
			*tabsp++ = '\t';
		}
		*tabsp = 0;
		for(int i = 0; i < n - 1; i++){
			msgs[i] = osc_msg_format(osc_bndl_nth(b, i), level + 1, tabs, 3, ", \n", level);
			len += osc_msg_getPrettyLen(msgs[i]);
		}
		msgs[n - 1] = osc_msg_format(osc_bndl_nth(b, n - 1), level + 1, tabs, 1, "\n", level);
		len += osc_msg_getPrettyLen(msgs[n - 1]);
		len += 3 + level;
		char *buf = osc_mem_alloc(len + 1);
		buf[0] = '{';
		buf[1] = '\n';
		buf[len - 1] = '}';
		buf[len] = '\0';
		char *ptr = buf + 2;
		for(int i = 0; i < n; i++){
			memcpy(ptr, osc_msg_getPrettyPtr(msgs[i]), osc_msg_getPrettyLen(msgs[i]));
			ptr += osc_msg_getPrettyLen(msgs[i]);
			osc_msg_release(msgs[i]);
		}
		tabs[level + 1] = 0;
		memcpy(ptr, tabs, level);
		*_len = len;
		*_buf = buf;
	}
}

t_osc_bndl_m *osc_bndl_format_m(t_osc_bndl_m *b, int level)
{
	if(!b){
		OSC_BNDL_ALLOC(ret, OSC_TIMETAG_NULL, osc_pvec2_alloc(osc_msg_release), 0, NULL, 0, NULL, 1);
		return (t_osc_bndl_m *)ret;
	}
	if(pl(b) && pp(b)){
		return b;
	}
	int len = 0;
	char *ptr = NULL;
	_osc_bndl_format(b, &len, &ptr, level);
	if(ptr){
		pl(b) = len;
		pp(b) = ptr;
	}
	return b;
}

t_osc_bndl *osc_bndl_format(t_osc_bndl *b, int level)
{
	if(!b){
		OSC_BNDL_ALLOC(ret, OSC_TIMETAG_NULL, osc_pvec2_alloc(osc_msg_release), 0, NULL, 0, NULL, 1);
		return ret;
	}
	if(pl(b) && pp(b)){
		return osc_bndl_retain(b);
	}
	int len = 0;
	char *ptr = NULL;
	_osc_bndl_format(b, &len, &ptr, level);
	t_osc_bndl *ret = osc_bndl_clone(b);
	if(ptr){
		pl((t_osc_bndl_m *)ret) = len;
		pp((t_osc_bndl_m *)ret) = ptr;
		return ret;
	}else{
		return osc_bndl_retain(b);
	}
}

//////////////////////////////////////////////////
// accessors
//////////////////////////////////////////////////
t_osc_timetag osc_bndl_getTimetag(t_osc_bndl *b)
{
	if(b){
		return tt(b);
	}
	return OSC_TIMETAG_NULL;
}

void osc_bndl_setSerializedLen(t_osc_bndl *b, int len)
{
	if(b){
		sl((t_osc_bndl_m *)b) = len;
	}
}

void osc_bndl_setSerializedPtr(t_osc_bndl *b, char *ptr)
{
	if(b){
		sp((t_osc_bndl_m *)b) = ptr;
	}
}

int osc_bndl_getSerializedLen(t_osc_bndl *b)
{
	if(b){
		return sl(b);
	}
	return 0;
}

char *osc_bndl_getSerializedPtr(t_osc_bndl *b)
{
	if(b){
		return sp(b);
	}
	return NULL;
}

void osc_bndl_setPrettyLen(t_osc_bndl *b, int len)
{
	if(b){
		pl((t_osc_bndl_m *)b) = len;
	}
}

void osc_bndl_setPrettyPtr(t_osc_bndl *b, char *ptr)
{
	if(b){
		pp((t_osc_bndl_m *)b) = ptr;
	}
}

int osc_bndl_getPrettyLen(t_osc_bndl *b)
{
	if(b){
		return pl(b);
	}
	return 0;
}

char *osc_bndl_getPrettyPtr(t_osc_bndl *b)
{
	if(b){
		return pp(b);
	}
	return NULL;
}

//////////////////////////////////////////////////
// low-level basic functions
//////////////////////////////////////////////////

int osc_bndl_length(t_osc_bndl *b)
{
	if(b){
		return osc_pvec2_length(m(b));
	}
	return 0;
}

t_osc_msg *osc_bndl_nth(t_osc_bndl *b, int idx)
{
	if(b){
		return (t_osc_msg *)osc_pvec2_nth(m(b), idx);
	}else{
		return NULL;
	}
}

t_osc_bndl *osc_bndl_assocn(t_osc_bndl *b, t_osc_msg *m, int idx)
{
	if(b){
		t_osc_pvec2 *new = osc_pvec2_assocN(m(b), idx, (void *)m);
		OSC_BNDL_ALLOC(bb, tt(b), new, 0, NULL, 0, NULL, 1);
		return bb;
	}
	OSC_BNDL_ALLOC(bb, OSC_TIMETAG_NULL, osc_pvec2_alloc(osc_msg_release), 0, NULL, 0, NULL, 1);
	return bb;
}

t_osc_bndl_m *osc_bndl_assocn_m(t_osc_bndl_m *b, t_osc_msg *m, int idx)
{
	if(b){
		osc_pvec2_assocN_m(m(b), idx, (void *)m);
		return b;
	}
	return (t_osc_bndl_m *)osc_bndl_empty;
}

t_osc_bndl *osc_bndl_append(t_osc_bndl *b, t_osc_msg *m)
{
	return osc_bndl_assocn(b, m, osc_bndl_length(b));
}

t_osc_bndl_m *osc_bndl_append_m(t_osc_bndl_m *b, t_osc_msg *m)
{
	return osc_bndl_assocn_m(b, m, osc_bndl_length(b));
}

t_osc_bndl *osc_bndl_prepend(t_osc_bndl *b, t_osc_msg *m)
{
	return osc_bndl_allocWithPvec2(OSC_TIMETAG_NULL, osc_pvec2_prepend(m(b), (void *)m));
}

t_osc_bndl_m *osc_bndl_prepend_m(t_osc_bndl_m *b, t_osc_msg *m)
{
	osc_pvec2_prepend_m(m(b), (void *)m);
	return b;
}

//////////////////////////////////////////////////
// higher order functions
//////////////////////////////////////////////////
t_osc_bndl *osc_bndl_apply(t_osc_bndl *(*fn)(t_osc_bndl *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context)
{
	if(!fn){
		return osc_bndl_retain(b);
	}
	return fn(b, context);
}

t_osc_bndl *osc_bndl_map(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context)
{
	if(!b){
		return osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	}
	if(!fn){
		return osc_bndl_retain(b);
	}
	int n = osc_bndl_length(b);
	t_osc_bndl *ret = osc_bndl_alloc(osc_bndl_getTimetag(b), 0);
	for(int i = 0; i < n; i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		t_osc_msg *mm = fn(m, context);
		ret = osc_bndl_assocn(ret, mm, i);
	}
	return ret;
}

t_osc_bndl *osc_bndl_filter(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context)
{
	if(!b){
		return osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	}
	if(!fn){
		return osc_bndl_retain(b);
	}
	int n = osc_bndl_length(b);
	t_osc_bndl *ret = osc_bndl_alloc(osc_bndl_getTimetag(b), 0);
	int j = 0;
	for(int i = 0; i < n; i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		t_osc_msg *mm = fn(m, context);
		if(mm){
			ret = osc_bndl_assocn(ret, mm, j++);
		}
	}
	return ret;
}

//////////////////////////////////////////////////
// set operations
//////////////////////////////////////////////////

t_osc_bndl *osc_bndl_union(t_osc_bndl *lhs, t_osc_bndl *rhs)
{
	if((!lhs && !rhs) || osc_bndl_length(lhs) == 0 || osc_bndl_length(rhs) == 0){
		return osc_bndl_empty;
	}
	t_osc_bndl *out = osc_bndl_retain(lhs);
	for(int i = 0; i < osc_bndl_length(rhs); i++){
		t_osc_msg *rm = osc_bndl_nth(rhs, i);
		t_osc_atom *pattern = osc_msg_nth(rm, 0);
		int match = 0;
		for(int j = 0; j < osc_bndl_length(lhs); j++){
			t_osc_msg *lm = osc_bndl_nth(lhs, j);
			t_osc_atom *address = osc_msg_nth(lm, 0);
			t_osc_atom *a = osc_atom_match(pattern, address);
			if(a == osc_atom_true){
				match = 1;
				osc_atom_release(a);
				break;
			}
			osc_atom_release(a);
		}
		if(!match){
			out = osc_bndl_assocn(out, rm, osc_bndl_length(out));
		}
	}
	return out;
}

t_osc_bndl *osc_bndl_intersect(t_osc_bndl *lhs, t_osc_bndl *rhs)
{
	if((!lhs && !rhs) || osc_bndl_length(lhs) == 0 || osc_bndl_length(rhs) == 0){
		return osc_bndl_empty;
	}
	t_osc_bndl *out = osc_bndl_alloc(tt(lhs), 0);
	for(int i = 0; i < osc_bndl_length(rhs); i++){
		t_osc_msg *rm = osc_bndl_nth(rhs, i);
		t_osc_atom *pattern = osc_msg_nth(rm, 0);
		for(int j = 0; j < osc_bndl_length(lhs); j++){
			t_osc_msg *lm = osc_bndl_nth(lhs, j);
			t_osc_atom *address = osc_msg_nth(lm, 0);
			t_osc_atom *a = osc_atom_match(pattern, address);
			if(a == osc_atom_true){
				out = osc_bndl_assocn(out, rm, osc_bndl_length(out));
				osc_atom_release(a);
				break;
			}
			osc_atom_release(a);
		}
	}
	return out;
}

// return a bundle containing the messages in lhs that are not also in rhs
t_osc_bndl *osc_bndl_rcompliment(t_osc_bndl *lhs, t_osc_bndl *rhs)
{
	if((!lhs && !rhs) || osc_bndl_length(lhs) == 0 || osc_bndl_length(rhs) == 0){
		return osc_bndl_empty;
	}
	t_osc_bndl *out = osc_bndl_alloc(tt(lhs), 0);
	for(int i = 0; i < osc_bndl_length(lhs); i++){
		t_osc_msg *lm = osc_bndl_nth(lhs, i);
		t_osc_atom *address = osc_msg_nth(lm, 0);
		int match = 0;
		for(int j = 0; j < osc_bndl_length(rhs); j++){
			t_osc_msg *rm = osc_bndl_nth(rhs, j);
			t_osc_atom *pattern = osc_msg_nth(rm, 0);
			t_osc_atom *a = osc_atom_match(pattern, address);
			if(a == osc_atom_true){
				match = 1;
				osc_atom_release(a);
				break;
			}
			osc_atom_release(a);
		}
		if(!match){
			out = osc_bndl_assocn(out, lm, osc_bndl_length(out));
		}
	}
	return out;
}

t_osc_bndl *osc_bndl_eval(t_osc_bndl *b, t_osc_bndl *context)
{
	if(!b){
		return osc_bndl_empty;
	}
	if(context == NULL){
		context = b;
	}
	t_osc_bndl *newb = osc_bndl_alloc(osc_bndl_getTimetag(b), 0);
	for(int i = 0; i < osc_bndl_length(b); i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		if(!m){
			continue;
		}
		t_osc_atom *address = osc_msg_nth(m, 0);
		t_osc_msg *newm = osc_msg_alloc(osc_atom_retain(address), 0);
		for(int j = 1; j < osc_msg_length(m) + 1; j++){
			t_osc_atom *a = osc_msg_nth(m, j);
			if(!a){
				continue;
			}
			newm = osc_msg_append((t_osc_msg_m *)newm, osc_atom_eval(a, context));
		}
		newb = osc_bndl_append_m((t_osc_bndl_m *)newb, newm);
	}
	return newb;
}
