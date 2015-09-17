#include "osc.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_mem.h"
#include "osc_pvec.h"
#include "osc_util.h"
#include "osc_byteorder.h"
#include "osc_obj.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static int osc_msg_changeRefCount(t_osc_msg *m, int amount);

#pragma pack(push)
#pragma pack(4)
struct _osc_msg
{
	t_osc_obj obj;
	t_osc_pvec2 *atoms;
	int serialized_len;
	char *serialized_ptr;
	//int pretty_len;
	//char *pretty_ptr;
	int static_msg;
};
#pragma pack(pop)

#define a(m) ((m)->atoms)
#define sl(m) ((m)->serialized_len)
#define sp(m) ((m)->serialized_ptr)
//#define pl(m) ((m)->pretty_len)
//#define pp(m) ((m)->pretty_ptr)
#define rc(m) ((m)->obj.refcount)

t_osc_msg _osc_msg_empty = {{-1, NULL}, NULL, 0, NULL, 1};
t_osc_msg *osc_msg_empty = &_osc_msg_empty;

//#define OSC_MSG_ALLOC(varname, atomlist, serialized_len, serialized_ptr, pretty_len, pretty_ptr, refcount) 
#define OSC_MSG_ALLOC(varname, atomlist, serialized_len, serialized_ptr, refcount) \
	void *OSC_UID(__osc_msg_alloc_ptr__) = osc_mem_alloc(sizeof(t_osc_msg));\
	t_osc_msg OSC_UID(__osc_msg_alloc_m__) = {{refcount, osc_msg_free}, atomlist, serialized_len, serialized_ptr, 0}; \
	memcpy(OSC_UID(__osc_msg_alloc_ptr__), &OSC_UID(__osc_msg_alloc_m__), sizeof(t_osc_msg));\
	t_osc_msg *varname = (t_osc_msg *)OSC_UID(__osc_msg_alloc_ptr__);

t_osc_msg *osc_msg_allocWithPvec2(t_osc_pvec2 *pvec2)
{
	OSC_MSG_ALLOC(m, pvec2, 0, NULL, 1);
	return m;
}

t_osc_msg *osc_msg_alloc(t_osc_atom *address, int n, ...)
{
	t_osc_pvec2 *pvec2 = osc_pvec2_alloc(osc_atom_release);
	pvec2 = osc_pvec2_assocN_m(pvec2, 0, (void *)address);

	if(n == 0){
		//pvec2 = osc_pvec2_append_m(pvec2, (void *)osc_atom_undefined);
	}else{
		va_list argp;
		va_start(argp, n);
		for(int i = 0; i < n; i++){
			t_osc_atom *a = va_arg(argp, t_osc_atom *);
			pvec2 = osc_pvec2_assocN_m(pvec2, i + 1, (void *)a);
		}
		va_end(argp);
	}
	OSC_MSG_ALLOC(m, pvec2, 0, NULL, 1);
	return m;
}

t_osc_msg *osc_msg_clone(t_osc_msg *m)
{
	if(m){
		int sl = sl(m);
		char *sp = NULL;
		if(sp(m) && sl){
			sp = osc_mem_alloc(sl);
			memcpy(sp, sp(m), sl);
		}
		OSC_MSG_ALLOC(clone, osc_pvec2_copy(a(m)), sl, sp, 1);
		return clone;
	}
	return osc_msg_empty;
}

static int osc_msg_changeRefCount(t_osc_msg *m, int amount)
{
	if(m){
		return rc((t_osc_msg_m *)m) += amount;
	}
	return -1;
}

t_osc_msg *osc_msg_retain(t_osc_msg *m)
{
	if(m){
		osc_msg_changeRefCount(m, 1);
	}
	return m;
}

void osc_msg_free(void *_m)
{
	if(_m){
		t_osc_msg *m = (t_osc_msg *)_m;
		osc_pvec2_release(a(m));
		char *p = sp(m);
		if(p){
			osc_mem_free(p);
		}
		/* p = pp(m); */
		/* if(p){ */
		/* 	osc_mem_free((void *)p); */
		/* } */
		memset(m, 0, sizeof(t_osc_msg));
		osc_mem_free((void *)m);
	}
}

void osc_msg_release(t_osc_msg *m)
{
	if(m){
		int rc = osc_msg_changeRefCount((t_osc_msg *)m, -1);
		if(rc == 0){
			osc_msg_free((void *)m);
		}
	}
}

t_osc_atom *osc_msg_serializeFn(t_osc_atom *a, t_osc_bndl *context)
{
	return osc_atom_serialize(a);
}

static void _osc_msg_serialize(t_osc_msg *m, int *_len, char **_buf)
{
	if(!m){
		return;
	}
	t_osc_msg *mm = osc_msg_map(osc_msg_serializeFn, m, NULL);
	int natoms = osc_msg_length(mm);
	int n = 4; // size
	for(int i = 0; i < natoms + 1; i++){
		n += osc_atom_getSerializedLen(osc_msg_nth(mm, i));
	}
	int ttlen = osc_util_getPaddingForNBytes(natoms + 1);
	n += ttlen;
	char *buf = osc_mem_alloc(n);
	memset(buf, 0, n);
	*((int32_t *)buf) = hton32(n - 4);
	char *p = buf + 4;
	t_osc_atom *address = osc_msg_nth(mm, 0);
	int addresslen = osc_atom_getSerializedLen(address);
	memcpy(p, osc_atom_getSerializedPtr(address), addresslen);
	p += addresslen;
	char *d = p + ttlen;
	*p++ = ',';
	for(int i = 1; i < natoms + 1; i++){
		t_osc_atom *a = osc_msg_nth(mm, i);
		*p++ = osc_atom_getTypetag(a);
		int alen = osc_atom_getSerializedLen(a);
		memcpy(d, osc_atom_getSerializedPtr(a), alen);
		d += alen;
	}
	*_len = n;
	*_buf = buf;
	osc_msg_release(mm);
	//t_osc_atom *blob = osc_atom_allocWithBlob(buf, 1);
	//t_osc_msg *mmm = osc_msg_alloc(osc_atom_clone(osc_msg_nth(mm, 0)), 1, blob);
	//osc_msg_release(mm);
	//return mmm;
	return;
}

t_osc_msg_m *osc_msg_serialize_m(t_osc_msg_m *m)
{
	if(!m){
		return NULL;
	}
	if(sl(m) && sp(m)){
		return m;
	}
	int len = 0;
	char *ptr = NULL;
	_osc_msg_serialize(m, &len, &ptr);
	if(ptr){
		sl((t_osc_msg_m *)m) = len;
		sp((t_osc_msg_m *)m) = ptr;
	}
	return m;
}

t_osc_msg *osc_msg_serialize(t_osc_msg *m)
{
	if(!m){
		return NULL;
	}
	if(sl(m) && sp(m)){
		return osc_msg_retain(m);
	}
	int len = 0;
	char *ptr = NULL;
	_osc_msg_serialize(m, &len, &ptr);
	t_osc_msg *ret = osc_msg_clone(m);
	if(ptr){
		sl((t_osc_msg_m *)ret) = len;
		sp((t_osc_msg_m *)ret) = ptr;
	}
	return ret;
}

static void _osc_msg_format(t_osc_msg *m, int prefixlen, char *prefix, int postfixlen, char *postfix, int *len, char **buf, int level)
{
	if(!m){
		return;
	}
	t_osc_atom *a = NULL;
	if(osc_msg_length(m) == 0){
		a = osc_atom_formatAtomsAsMsg(a(m), prefixlen, prefix, 0, NULL, 0, NULL, 0, NULL, postfixlen, postfix, level);
	}else if(osc_msg_length(m) > 1){
		char _postfix[postfixlen + 2];
		_postfix[0] = ' ';
		_postfix[1] = ']';
		memcpy(_postfix + 2, postfix, postfixlen);
		a = osc_atom_formatAtomsAsMsg(a(m), prefixlen, prefix, 5, " : [ ", 2, ", ", 0, NULL, postfixlen + 2, _postfix, level);
	}else{
		a = osc_atom_formatAtomsAsMsg(a(m), prefixlen, prefix, 3, " : ", 2, ", ", 0, NULL, postfixlen, postfix, level);
	}
	*len = osc_atom_getPrettyLen(a);
	*buf = osc_util_strcpy(osc_atom_getPrettyPtr(a));
	osc_atom_release(a);
}

/* t_osc_msg_m *osc_msg_format_m(t_osc_msg_m *m, int prefixlen, char *prefix, int postfixlen, char *postfix, int level) */
/* { */
/* 	if(!m){ */
/* 		OSC_MSG_ALLOC(ret, osc_pvec2_alloc(osc_atom_release), 0, NULL, 0, NULL, 1); */
/* 		return (t_osc_msg_m *)ret; */
/* 	} */
/* 	if(pl(m) && pp(m)){ */
/* 		return m; */
/* 	} */
/* 	int len = 0; */
/* 	char *ptr = NULL; */
/* 	_osc_msg_format(m, prefixlen, prefix, postfixlen, postfix, &len, &ptr, level); */
/* 	if(ptr){ */
/* 		pl(m) = len; */
/* 		pp(m) = ptr; */
/* 	} */
/* 	return m; */
/* } */

t_osc_atom *osc_msg_format(t_osc_msg *m, int prefixlen, char *prefix, int postfixlen, char *postfix, int level)
{
	if(!m){
		//OSC_MSG_ALLOC(ret, osc_pvec2_alloc(osc_atom_release), 0, NULL, 0, NULL, 1);
		return osc_atom_emptystring;
	}
	/* if(pl(m) && pp(m)){ */
	/* 	return osc_msg_retain(m); */
	/* } */
	int len = 0;
	char *ptr = NULL;
	_osc_msg_format(m, prefixlen, prefix, postfixlen, postfix, &len, &ptr, level);
	if(ptr){
		return osc_atom_allocString(ptr, 1);
	}else{
		return osc_atom_emptystring;
	}
	/* if(ptr){ */
	/* 	t_osc_msg *ret = osc_msg_clone(m); */
	/* 	pl((t_osc_msg_m *)ret) = len; */
	/* 	pp((t_osc_msg_m *)ret) = ptr; */
	/* 	return ret; */
	/* }else{ */
	/* 	return osc_msg_retain(m); */
	/* } */
}

//////////////////////////////////////////////////
// accessors
//////////////////////////////////////////////////
int osc_msg_getSerializedLen(t_osc_msg *m)
{
	if(m){
		return m->serialized_len;
	}
	return 0;
}

char *osc_msg_getSerializedPtr(t_osc_msg *m)
{
	if(m){
		return m->serialized_ptr;
	}
	return NULL;
}

/* int osc_msg_getPrettyLen(t_osc_msg *m) */
/* { */
/* 	if(m){ */
/* 		return m->pretty_len; */
/* 	} */
/* 	return 0; */
/* } */

/* char *osc_msg_getPrettyPtr(t_osc_msg *m) */
/* { */
/* 	if(m){ */
/* 		return m->pretty_ptr; */
/* 	} */
/* 	return NULL; */
/* } */

int osc_msg_length(t_osc_msg *m)
{
	if(m){
		return osc_pvec2_length(a(m)) - 1;
	}
	return 0;
}

t_osc_atom *osc_msg_nth(t_osc_msg *m, int idx)
{
	if(m){
		return (t_osc_atom *)osc_pvec2_nth(a(m), idx);
	}
	return NULL;
}

t_osc_msg *osc_msg_assocn(t_osc_msg *m, t_osc_atom *a, int idx)
{
	if(m){
		t_osc_pvec2 *new = osc_pvec2_assocN(a(m), idx, (void *)a);
		OSC_MSG_ALLOC(mm, new, 0, NULL, 1);
		return mm;
	}
	return osc_msg_empty;
}

t_osc_msg_m *osc_msg_assocn_m(t_osc_msg_m *m, t_osc_atom *a, int idx)
{
	if(m){
		osc_pvec2_assocN_m(a(m), idx, (void *)a);
		return m;
	}
	return (t_osc_msg_m *)osc_msg_empty;
}

t_osc_msg *osc_msg_append(t_osc_msg *m, t_osc_atom *a)
{
	return osc_msg_assocn(m, a, osc_msg_length(m) + 1);
}

t_osc_msg_m *osc_msg_append_m(t_osc_msg_m *m, t_osc_atom *a)
{
	return osc_msg_assocn_m(m, a, osc_msg_length(m) + 1);
}

t_osc_msg *osc_msg_prepend(t_osc_msg *m, t_osc_atom *a)
{
	if(m){
		return osc_msg_allocWithPvec2(osc_pvec2_prepend(a(m), (void *)a));
	}else{
		return osc_msg_alloc(a, 0);
	}
}

t_osc_msg_m *osc_msg_prepend_m(t_osc_msg_m *m, t_osc_atom *a)
{
	if(m){
		osc_pvec2_prepend_m(a(m), (void *)a);
		return m;
	}else{
		return (t_osc_msg_m *)osc_msg_alloc(a, 0);
	}
}

t_osc_msg *osc_msg_popFirst(t_osc_msg *m)
{
	if(m){
		t_osc_msg *mm = NULL;
		if(osc_msg_length(m) < 2){
			return osc_msg_empty;
		}else{
			mm = osc_msg_allocWithPvec2(osc_pvec2_popFirst(a(m)));
		}
		return mm;
	}else{
		return osc_msg_empty;
	}
}

t_osc_msg *osc_msg_popLast(t_osc_msg *m)
{
	if(m){
		t_osc_msg *mm = NULL;
		if(osc_msg_length(m) < 2){
			return osc_msg_empty;
		}else{
			mm = osc_msg_allocWithPvec2(osc_pvec2_popLast(a(m)));
		}
		return mm;
	}else{
		return osc_msg_empty;
	}
}

t_osc_msg *osc_msg_apply(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context)
{
	if(!fn){
		return osc_msg_clone(m);
	}
	return fn(m, context);
}

t_osc_msg *osc_msg_map(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context)
{
	if(!m){
		OSC_MSG_ALLOC(ret, NULL, 0, NULL, 1);
		return ret;
	}
	if(!fn){
		return osc_msg_clone(m);
	}
	int n = osc_msg_length(m);
	t_osc_msg *ret = osc_msg_alloc(osc_atom_clone(osc_msg_nth(m, 0)), 0);
	for(int i = 0; i < n + 1; i++){
		t_osc_atom *a = osc_msg_nth(m, i);
		t_osc_atom *aa = fn(a, context);
		ret = osc_msg_assocn(ret, aa, i);
	}
	return ret;
}

t_osc_msg *osc_msg_filter(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context)
{
	if(!m){
		OSC_MSG_ALLOC(ret, NULL, 0, NULL, 1);
		return ret;
	}
	if(!fn){
		return osc_msg_clone(m);
	}
	int n = osc_msg_length(m);
	t_osc_msg *ret = osc_msg_alloc(osc_msg_nth(m, 0), 0);
	for(int i = 0; i < n + 1; i++){
		t_osc_atom *a = osc_msg_nth(m, i);
		t_osc_atom *aa = fn(a, context);
		if(aa){
			ret = osc_msg_assocn(ret, aa, i);
		}
	}
	return ret;
}

t_osc_msg *osc_msg_lreduce(t_osc_atom *(*fn)(t_osc_atom *, t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context)
{
	if(!m){
		OSC_MSG_ALLOC(ret, NULL, 0, NULL, 1);
		return ret;
	}
	if(!fn){
		return osc_msg_clone(m);
	}
	int n = osc_msg_length(m);
	t_osc_atom *lhs = osc_msg_nth(m, 0);
	for(int i = 1; i < n + 1; i++){
		t_osc_atom *rhs = osc_msg_nth(m, i);
		lhs = fn(lhs, rhs, context);
	}
	t_osc_msg *ret = osc_msg_alloc(osc_msg_nth(m, 0), 0);
	ret = osc_msg_assocn(ret, lhs, 1);
	return ret;
}

t_osc_msg *osc_msg_rreduce(t_osc_atom *(*fn)(t_osc_atom *, t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context)
{
	if(!m){
		OSC_MSG_ALLOC(ret, NULL, 0, NULL, 1);
		return ret;
	}
	if(!fn){
		return osc_msg_clone(m);
	}
	int n = osc_msg_length(m);
	t_osc_atom *rhs = osc_msg_nth(m, 0);
	for(int i = 1; i < n + 1; i++){
		t_osc_atom *lhs = osc_msg_nth(m, i);
		lhs = fn(lhs, rhs, context);
	}
	t_osc_msg *ret = osc_msg_alloc(osc_msg_nth(m, 0), 0);
	ret = osc_msg_assocn(ret, rhs, 1);
	return ret;
}

t_osc_msg *osc_msg_evalStrict(t_osc_msg *m, t_osc_bndl *context)
{
	t_osc_msg *nm = osc_msg_alloc(osc_atom_retain(osc_msg_nth(m, 0)), 0);
	for(int j = 1; j < osc_msg_length(m) + 1; j++){
		t_osc_atom *a = osc_msg_nth(m, j);
		t_osc_atom *e = osc_atom_evalStrict(a, context);
		if(osc_atom_getTypetag(e) == OSC_TT_BNDL || osc_atom_getTypetag(e) == OSC_TT_EXPR){
			t_osc_bndl *eb = osc_atom_getBndlPtr(e);
			if(osc_bndl_statusOK(eb) == osc_atom_true){
				t_osc_msg *vm = osc_bndl_lookup(eb, osc_atom_valueaddress, osc_atom_match);
				if(vm){
					for(int k = 1; k < osc_msg_length(vm) + 1; k++){
						t_osc_atom *aa = osc_msg_nth(vm, k);
						osc_msg_append_m((t_osc_msg_m *)nm, osc_atom_retain(aa));
					}
				}else{
					osc_msg_append_m((t_osc_msg_m *)nm, osc_atom_retain(e));
				}
			}else{
				osc_msg_append_m((t_osc_msg_m *)nm, osc_atom_retain(e));
			}
		}else{
			osc_msg_append_m((t_osc_msg_m *)nm, osc_atom_retain(e));
		}
		osc_atom_release(e);
		//osc_msg_append_m((t_osc_msg_m *)nm, e);
	}
	return nm;
}
