#include "osc.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_mem.h"
#include "osc_pvec.h"
#include "osc_byteorder.h"
#include "osc_util.h"
#include "osc_strfmt.h"
#include "osc_timetag.h"
#include "osc_typetag.h"
#include "osc_match.h"
#include "osc_obj.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void osc_atom_print(t_osc_atom *a);

#pragma pack(push)
#pragma pack(4)
typedef struct _osc_value{
	union _value {
		int8_t c;
		uint8_t C;
		int16_t u;
		uint16_t U;
		int32_t i;
		uint32_t I;
		int64_t h;
		uint64_t H;
		float f;
		double d;
		//float q[4];
		//double Q[4];
		char *s;
		char *S;
		t_osc_bndl *OSC_TT_BNDL_ID;
		t_osc_timetag t;
		char *b;
		//t_osc_bndl *(*OSC_TT_NATIVE_ID)(t_osc_bndl *);
		t_osc_builtin OSC_TT_NATIVE_ID;
		t_osc_bndl *OSC_TT_EXPR_ID;
	} value;
	char typetag;
} t_osc_value;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(4)
struct _osc_atom
{
	t_osc_obj obj;
	t_osc_value value;
	int serialized_len;
	char *serialized_ptr;
	int pretty_len;
	char *pretty_ptr;
	int should_free;
	int static_atom;
};
#pragma pack(pop)

#define v(a, f) ((a)->value.value.f)
#define vv(a) ((a)->value)
#define tt(a) ((a)->value.typetag)
#define sl(a) ((a)->serialized_len)
#define sp(a) ((a)->serialized_ptr)
#define pl(a) ((a)->pretty_len)
#define pp(a) ((a)->pretty_ptr)
#define rc(a) ((a)->obj.refcount)
#define sf(a) ((a)->should_free)
#define st(a) ((a)->static_atom)


static t_osc_atom _osc_atom_true = { {-1, NULL}, {{0}, 'T'}, 0, NULL, 4, "true", 0, 1 };
t_osc_atom *osc_atom_true = &_osc_atom_true;
static t_osc_atom _osc_atom_false = { {-1, NULL}, {{0}, 'F'}, 0, NULL, 5, "false", 0, 1 };
t_osc_atom *osc_atom_false = &_osc_atom_false;
static t_osc_atom _osc_atom_nil = { {-1, NULL}, {{0}, 'N'}, 0, NULL, 3, "nil", 0, 1 };
t_osc_atom *osc_atom_nil = &_osc_atom_nil;
static t_osc_atom _osc_atom_undefined = { {-1, NULL}, {{0}, OSC_TT_UNDEF}, 0, NULL, 9, "undefined", 0, 1 };
t_osc_atom *osc_atom_undefined = &_osc_atom_undefined;

static t_osc_atom _osc_atom_emptystring = { {-1, NULL}, {{ .S = "\0" }, OSC_TT_SYM}, 4, "\0\0\0\0", 4, "\0\0\0\0", 0, 1};
t_osc_atom *osc_atom_emptystring = &_osc_atom_emptystring;

static t_osc_atom _osc_atom_valueaddress = { {-1, NULL}, {{.S = "/value" }, OSC_TT_SYM}, 8, "/value\0\0", 6, "/value", 0, 1};
t_osc_atom *osc_atom_valueaddress = &_osc_atom_valueaddress;
static t_osc_atom _osc_atom_expraddress = { {-1, NULL}, {{.S = "/expr" }, OSC_TT_SYM}, 8, "/expr\0\0\0", 5, "/expr", 0, 1};
t_osc_atom *osc_atom_expraddress = &_osc_atom_expraddress;
static t_osc_atom _osc_atom_typeaddress = { {-1, NULL}, {{.S = "/type" }, OSC_TT_SYM}, 8, "/type\0\0\0", 5, "/type", 0, 1};
t_osc_atom *osc_atom_typeaddress = &_osc_atom_typeaddress;
static t_osc_atom _osc_atom_funcaddress = { {-1, NULL}, {{.S = "/func" }, OSC_TT_SYM}, 8, "/func\0\0\0", 5, "/func", 0, 1};
t_osc_atom *osc_atom_funcaddress = &_osc_atom_funcaddress;
static t_osc_atom _osc_atom_argsaddress = { {-1, NULL}, {{.S = "/args" }, OSC_TT_SYM}, 8, "/args\0\0\0", 5, "/args", 0, 1};
t_osc_atom *osc_atom_argsaddress = &_osc_atom_argsaddress;
static t_osc_atom _osc_atom_partialaddress = { {-1, NULL}, {{.S = "/partial" }, OSC_TT_SYM}, 12, "/partial\0\0\0\0", 8, "/partial", 0, 1};
t_osc_atom *osc_atom_partialaddress = &_osc_atom_partialaddress;
static t_osc_atom _osc_atom_completeaddress = { {-1, NULL}, {{.S = "/complete" }, OSC_TT_SYM}, 12, "/complete\0\0\0", 9, "/complete", 0, 1};
t_osc_atom *osc_atom_completeaddress = &_osc_atom_completeaddress;
static t_osc_atom _osc_atom_unmatchedaddress = { {-1, NULL}, {{.S = "/unmatched" }, OSC_TT_SYM}, 12, "/unmatched\0\0", 10, "/unmatched", 0, 1};
t_osc_atom *osc_atom_unmatchedaddress = &_osc_atom_unmatchedaddress;

static t_osc_atom _osc_atom_ps_add = { {-1, NULL}, {{.S = "add" }, OSC_TT_SYM}, 4, "add\0", 3, "add", 0, 1};
t_osc_atom *osc_atom_ps_add = &_osc_atom_ps_add;
static t_osc_atom _osc_atom_ps_nth = { {-1, NULL}, {{.S = "nth" }, OSC_TT_SYM}, 4, "nth\0", 3, "nth", 0, 1};
t_osc_atom *osc_atom_ps_nth = &_osc_atom_ps_nth;

static t_osc_atom _osc_atom_yaddress = { {-1, NULL}, {{.S = "/y" }, OSC_TT_SYM}, 4, "/y\0\0", 2, "/y", 0, 1};
t_osc_atom *osc_atom_yaddress = &_osc_atom_yaddress;
static t_osc_atom _osc_atom_lhsaddress = { {-1, NULL}, {{.S = "/lhs" }, OSC_TT_SYM}, 8, "/lhs\0\0\0\0", 4, "/lhs", 0, 1};
t_osc_atom *osc_atom_lhsaddress = &_osc_atom_lhsaddress;
static t_osc_atom _osc_atom_rhsaddress = { {-1, NULL}, {{.S = "/rhs" }, OSC_TT_SYM}, 8, "/rhs\0\0\0\0", 4, "/rhs", 0, 1};
t_osc_atom *osc_atom_rhsaddress = &_osc_atom_rhsaddress;
static t_osc_atom _osc_atom_naddress = { {-1, NULL}, {{.S = "/n" }, OSC_TT_SYM}, 4, "/n\0\0", 2, "/n", 0, 1};
t_osc_atom *osc_atom_naddress = &_osc_atom_naddress;
static t_osc_atom _osc_atom_listaddress = { {-1, NULL}, {{.S = "/list" }, OSC_TT_SYM}, 8, "/list\0\0\0", 5, "/list", 0, 1};
t_osc_atom *osc_atom_listaddress = &_osc_atom_listaddress;


static int osc_atom_changeRefCount(t_osc_atom *a, int amount);
t_osc_atom *osc_atom_evalSymbol(t_osc_atom *a, t_osc_bndl *context);

#define OSC_ATOM_ALLOC(varname, value, unionfield, typetag, serialized_len, serialized_ptr, pretty_len, pretty_ptr, refcount, should_free) \
	void *OSC_UID(__osc_atom_alloc_ptr__) = osc_mem_alloc(sizeof(t_osc_atom));	\
	t_osc_atom OSC_UID(__osc_atom_alloc_a__) = {{refcount, osc_atom_free}, {{.unionfield = value}, typetag}, serialized_len, serialized_ptr, pretty_len, pretty_ptr, should_free, 0}; \
	memcpy(OSC_UID(__osc_atom_alloc_ptr__), &OSC_UID(__osc_atom_alloc_a__), sizeof(t_osc_atom)); \
	t_osc_atom *varname = (t_osc_atom *)OSC_UID(__osc_atom_alloc_ptr__);

// serialize and pretty print on init?
t_osc_atom *osc_atom_allocInt8(int8_t i)
{
	OSC_ATOM_ALLOC(a, i, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt8(uint8_t i)
{
	OSC_ATOM_ALLOC(a, i, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt16(int16_t i)
{
	OSC_ATOM_ALLOC(a, i, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt16(uint16_t i)
{
	OSC_ATOM_ALLOC(a, i, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt32(int32_t i)
{
	OSC_ATOM_ALLOC(a, i, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt32(uint32_t i)
{
	OSC_ATOM_ALLOC(a, i, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt64(int64_t i)
{
	OSC_ATOM_ALLOC(a, i, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt64(uint64_t i)
{
	OSC_ATOM_ALLOC(a, i, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocFloat(float i)
{
	OSC_ATOM_ALLOC(a, i, f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocDouble(double i)
{
	OSC_ATOM_ALLOC(a, i, d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocString(char *i, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, i, s, OSC_TT_STR, 0, NULL, 0, NULL, 1, should_free_ptr);
	return a;
}

t_osc_atom *osc_atom_allocSymbol(char *i, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, i, S, OSC_TT_SYM, 0, NULL, 0, NULL, 1, should_free_ptr);
	return a;
}

t_osc_atom *osc_atom_allocBndl(t_osc_bndl *b, int should_free_bndl)
{
	OSC_ATOM_ALLOC(a, b, OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, should_free_bndl);
	return a;
}

t_osc_atom *osc_atom_allocTimetag(t_osc_timetag i)
{
	OSC_ATOM_ALLOC(a, i, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocBlob(char *b, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, b, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, should_free_ptr);
	return a;
}

t_osc_atom *osc_atom_allocTrue(void)
{
	return osc_atom_true;
}

t_osc_atom *osc_atom_allocFalse(void)
{
	return osc_atom_false;
}

t_osc_atom *osc_atom_allocNil(void)
{
	return osc_atom_nil;
}

t_osc_atom *osc_atom_allocNative(t_osc_builtin f, char *funcname)
{
	char *buf = osc_mem_alloc(128);
	int n = snprintf(buf, 128, "__native %s", funcname);
	OSC_ATOM_ALLOC(a, f, OSC_TT_NATIVE_ID, OSC_TT_NATIVE, 0, NULL, n, buf, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocExpr(t_osc_bndl *b, int should_free)
{
	OSC_ATOM_ALLOC(a, b, OSC_TT_EXPR_ID, OSC_TT_EXPR, 0, NULL, 0, NULL, 1, should_free);
	return a;
}

t_osc_atom *osc_atom_clone(t_osc_atom *a)
{
	if(!a){
		return NULL;
	}
	if(st(a)){
		return a;
	}
	int sl = 0;
	char *sp = NULL;
	if(sp(a)){
		sl = sl(a);
		sp = osc_mem_alloc(sl);
		memcpy(sp, sp(a), sl);
	}
	int pl = 0;
	char *pp = NULL;
	if(pp(a)){
		pl = pl(a);
		pp = osc_mem_alloc(pl);
		memcpy(pp, pp(a), pl);
	}
	switch(tt(a)){
	case OSC_TT_STR:
		{
			OSC_ATOM_ALLOC(ret, osc_util_strcpy(v(a, s)), s, OSC_TT_STR, sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case OSC_TT_SYM:
		{
			OSC_ATOM_ALLOC(ret, osc_util_strcpy(v(a, S)), S, OSC_TT_SYM, sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case OSC_TT_BNDL:
		{
			OSC_ATOM_ALLOC(ret, osc_bndl_retain(v(a, OSC_TT_BNDL_ID)), OSC_TT_BNDL_ID, OSC_TT_BNDL, sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case OSC_TT_BLOB:
		{
			OSC_ATOM_ALLOC(ret, osc_util_blobcpy(v(a, b)), b, OSC_TT_BLOB, sl, sp, pl, pp, 1, 1);
			return ret;
		}
	default:
		{
			OSC_ATOM_ALLOC(ret, v(a, s), s, tt(a), sl, sp, pl, pp, 1, sf(a));
			return ret;
		}
	}
}

static int osc_atom_changeRefCount(t_osc_atom *a, int amount)
{
	if(a && st(a) == 0){
		return rc((t_osc_atom_m *)a) += amount;
	}
	return -1;
}

t_osc_atom *osc_atom_retain(t_osc_atom *a)
{
	if(a && st(a) == 0){
		osc_atom_changeRefCount(a, 1);
	}
	return a;
}

void osc_atom_free(void *_a)
{
	if(_a){
		t_osc_atom *a = (t_osc_atom *)_a;
		if(st(a)){
			return;
		}
		char *p = NULL;
		switch(tt(a)){
		case OSC_TT_STR:
		case OSC_TT_SYM:
		case OSC_TT_BLOB:
			if(sf(a)){
				p = v(a, s);
			}
			break;
		case OSC_TT_BNDL:
			if(sf(a)){
				osc_bndl_release(v(a, OSC_TT_BNDL_ID));
			}
			break;
		}
		if(p){
			osc_mem_free(p);
		}
		p = sp(a);
		if(p){
			osc_mem_free(p);
		}
		p = pp(a);
		if(p){
			osc_mem_free(p);
		}
		memset((void *)a, 0, sizeof(t_osc_atom));
		osc_mem_free((void *)a);
	}
}

void osc_atom_release(t_osc_atom *a)
{
	if(a && st(a) == 0){
		int rc = osc_atom_changeRefCount(a, -1);
		if(rc == 0){
			osc_atom_free((void *)a);
		}
	}
}

//////////////////////////////////////////////////
// accessors
//////////////////////////////////////////////////
char osc_atom_getTypetag(t_osc_atom *a)
{
	if(a){
		return tt(a);
	}
	return 0;
}

int osc_atom_getSerializedLen(t_osc_atom *a)
{
	if(a){
		return sl(a);
	}
	return 0;
}

char *osc_atom_getSerializedPtr(t_osc_atom *a)
{
	if(a){
		return sp(a);
	}
	return NULL;
}

int osc_atom_getPrettyLen(t_osc_atom *a)
{
	if(a){
		return pl(a);
	}
	return 0;
}

char *osc_atom_getPrettyPtr(t_osc_atom *a)
{
	if(a){
		return pp(a);
	}
	return NULL;
}

t_osc_atom *osc_atom_strcat(t_osc_pvec2 *pvec2)
{
	int n = osc_pvec2_length(pvec2);
	t_osc_atom *atoms[n];
	int lengths[n];
	int len = 0;
	for(int i = 0; i < n; i++){
		atoms[i] = osc_atom_convertAny((t_osc_atom *)osc_pvec2_nth(pvec2, i), OSC_TT_STR);
		lengths[i] = strlen(v(atoms[i], s));
		len += lengths[i];
	}
	char *buf = osc_mem_alloc(len + 1);
	buf[len - 1] = 0;
	char *ptr = buf;
	for(int i = 0; i < n; i++){
		memcpy(ptr, v(atoms[i], s), lengths[i]);
		ptr += lengths[i];
	}
	return NULL;
}

t_osc_atom *osc_atom_formatAtomsAsMsg(t_osc_pvec2 *pvec2,
				      int prefixlen,
				      char *prefix,
				      int firstseplen,
				      char *firstsep,
				      int restseplen,
				      char *restsep,
				      int lastseplen,
				      char *lastsep,
				      int postfixlen,
				      char *postfix,
				      int level)
{
	int n = osc_pvec2_length(pvec2);
	if(n == 0){
		return osc_atom_emptystring;
	}
	t_osc_atom *atoms[n];
	int len = 1;
	for(int i = 0; i < n; i++){
		atoms[i] = osc_atom_format((t_osc_atom *)osc_pvec2_nth(pvec2, i), level);
		len += pl(atoms[i]);
	}
	len += prefixlen;
	len += firstseplen;
	len += (restseplen * (n - 2));
	len += lastseplen;
	len += postfixlen;

	char *buf = osc_mem_alloc(len);
	char *ptr = buf;
	if(prefix){
		memcpy(ptr, prefix, prefixlen);
	}
	ptr += prefixlen;
	memcpy(ptr, pp(atoms[0]), pl(atoms[0]));
	ptr += pl(atoms[0]);
	osc_atom_release(atoms[0]);
	if(firstsep){
		memcpy(ptr, firstsep, firstseplen);
	}
	ptr += firstseplen;
	for(int i = 1; i < n - 1; i++){
		memcpy(ptr, pp(atoms[i]), pl(atoms[i]));
		ptr += pl(atoms[i]);
		if(restsep){
	        	memcpy(ptr, restsep, restseplen);
		}
		ptr += restseplen;
		osc_atom_release(atoms[i]);
	}
	if(n > 1){
		memcpy(ptr, pp(atoms[n - 1]), pl(atoms[n - 1]));
		ptr += pl(atoms[n - 1]);
		osc_atom_release(atoms[n - 1]);
	}
	if(lastsep){
		memcpy(ptr, lastsep, lastseplen);
	}
	ptr += lastseplen;
	if(postfix){
		memcpy(ptr, postfix, postfixlen);
	}
	ptr += postfixlen;
	*ptr = '\0';
	OSC_ATOM_ALLOC(ret, buf, s, OSC_TT_STR, 0, NULL, (ptr - buf), osc_util_strcpy(buf), 1, 1);
	return ret;
}

static void _osc_atom_serialize(t_osc_atom *a, int *_len, char **_buf)
{
	if(!a){
		return;
	}
	int len = 0;
	char *buf = NULL;
	switch(tt(a)){
	case OSC_TT_I8:
	case OSC_TT_U8:
	case OSC_TT_I16:
	case OSC_TT_U16:
	case OSC_TT_I32:
	case OSC_TT_U32:
		{
			len = 4;
			buf = osc_mem_alloc(len);
			int32_t i = v(a, i);
			*((int32_t *)buf) = hton32(i);
		}
		break;
	case OSC_TT_F32:
		{
			len = 4;
			buf = osc_mem_alloc(len);
			float f = v(a, f);
			*((int32_t *)buf) = hton32(*((int32_t *)(&f)));
		}
		break;
	case OSC_TT_F64:
		{
			len = 8;
			buf = osc_mem_alloc(len);
			double d = v(a, d);
			*((int64_t *)buf) = hton64(*((int64_t *)(&d)));
		}
		break;
	case OSC_TT_STR:
	case OSC_TT_SYM:
		{
			int l = strlen(v(a, s));
			len = osc_util_getPaddingForNBytes(l);
			buf = osc_mem_alloc(len);
			memset(buf, '\0', len);
			memcpy(buf, v(a, s), l);
		}
		break;
	case OSC_TT_BNDL:
		{
			t_osc_bndl *b = osc_bndl_serialize(v(a, OSC_TT_BNDL_ID));
			len = osc_bndl_getSerializedLen(b);
			buf = osc_bndl_getSerializedPtr(b);
			osc_bndl_setSerializedLen(b, 0);
			osc_bndl_setSerializedPtr(b, NULL);
			osc_bndl_release(b);
		}
		break;
	case OSC_TT_TIME:
		len = OSC_TIMETAG_SIZEOF;
		buf = osc_mem_alloc(len);
		osc_timetag_encodeForHeader(v(a, t), buf);
		break;
	case OSC_TT_I64:
	case OSC_TT_U64:
		len = 8;
		buf = osc_mem_alloc(len);
		*((int64_t *)buf) = hton64(v(a, h));
		break;
// nothing to do for T, F, or N
	case OSC_TT_BLOB:
		{
			len = osc_util_getBlobLength(ntoh32(*((int32_t *)v(a, b)))); // includes size
			buf = osc_mem_alloc(len);
			memset(buf, '\0', len);
			memcpy(buf, v(a, b), len);
		}
		break;
	default: ;
	}
	*_len = len;
	*_buf = buf;
}

t_osc_atom_m *osc_atom_serialize_m(t_osc_atom_m *a)
{
	if(!a){
		return (t_osc_atom_m *)osc_atom_undefined;
	}
	if(sl(a) && sp(a)){
		return a;
	}
	int len = 0;
	char *ptr = NULL;
	_osc_atom_serialize(a, &len, &ptr);
	if(ptr){
		sl(a) = len;
		sp(a) = ptr;
	}
	return a;
}

t_osc_atom *osc_atom_serialize(t_osc_atom *a)
{
	if(!a){
		return osc_atom_undefined;
	}
	if(sl(a) && sp(a)){
		return osc_atom_retain(a);
	}
	int len = 0;
	char *ptr = NULL;
	_osc_atom_serialize(a, &len, &ptr);
	t_osc_atom *ret = osc_atom_clone(a);
	if(ptr){
		sl((t_osc_atom_m *)ret) = len;
		sp((t_osc_atom_m *)ret) = ptr;
	}
	return ret;
}

void _osc_atom_format(t_osc_atom *a, int *_len, char **_buf, int level)
{
	if(!a){
		return;
	}
	int len = 0;
	char *buf = NULL;
	switch(tt(a)){
	case OSC_TT_I8:
		{
			int8_t i = v(a, c);
			len = osc_strfmt_int8(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int8(buf, len + 1, i);
		}
		break;
	case OSC_TT_U8:
		{
			uint8_t i = v(a, C);
			len = osc_strfmt_uint8(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint8(buf, len + 1, i);
		}
		break;
        case OSC_TT_I16:
		{
			int16_t i = v(a, u);
			len = osc_strfmt_int16(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int16(buf, len + 1, i);
		}
		break;
	case OSC_TT_U16:
		{
			uint16_t i = v(a, U);
			len = osc_strfmt_uint16(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint16(buf, len + 1, i);
		}
		break;
	case OSC_TT_I32:
		{
			int32_t i = v(a, i);
			len = osc_strfmt_int32(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int32(buf, len + 1, i);
		}
		break;
	case OSC_TT_U32:
		{
			uint32_t i = v(a, I);
			len = osc_strfmt_uint32(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint32(buf, len + 1, i);
		}
		break;
	case OSC_TT_I64:
		{
			int64_t i = v(a, h);
			len = osc_strfmt_int64(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int64(buf, len + 1, i);
		}
		break;
	case OSC_TT_U64:
		{
			uint64_t i = v(a, H);
			len = osc_strfmt_uint64(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint64(buf, len + 1, i);
		}
		break;
	case OSC_TT_F32:
		{
			float i = v(a, f);
			len = osc_strfmt_float(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_float(buf, len + 1, i);
		}
		break;
	case OSC_TT_F64:
		{
			double i = v(a, d);
			len = osc_strfmt_double(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_double(buf, len + 1, i);
		}
		break;
	case OSC_TT_STR:
		{
			char *str = v(a, s);
			len = osc_strfmt_quotedStringWithQuotedMeta(NULL, 0, str);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_quotedStringWithQuotedMeta(buf, len + 1, str);
		}
		break;
	case OSC_TT_SYM:
		{
			char *str = v(a, S);
			len = strlen(str);
			buf = osc_mem_alloc(len + 1);
			strncpy(buf, str, len + 1);
		}
		break;
	case OSC_TT_EXPR:
	case OSC_TT_BNDL:
		{
			t_osc_bndl *b = osc_bndl_format(v(a, OSC_TT_BNDL_ID), level + 1);
			len = osc_bndl_getPrettyLen(b);
			buf = osc_bndl_getPrettyPtr(b);
			osc_bndl_setPrettyLen(b, 0);
			osc_bndl_setPrettyPtr(b, NULL);
			osc_bndl_release(b);
		}
		break;
	case OSC_TT_TIME:
		{
			t_osc_timetag tt = v(a, t);
			len = osc_strfmt_timetag(NULL, 0, tt);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_timetag(buf, len + 1, v(a, t));
		}
		break;
	case 'T':
		len = osc_strfmt_bool(NULL, 0, 'T');
		buf = osc_mem_alloc(len + 1);
		osc_strfmt_bool(buf, len + 1, 'T');
		break;
	case 'F':
		len = osc_strfmt_bool(NULL, 0, 'F');
		buf = osc_mem_alloc(len + 1);
		osc_strfmt_bool(buf, len + 1, 'F');
		break;
	case 'N':
		len = osc_strfmt_null(NULL, 0);
		buf = osc_mem_alloc(len + 1);
		osc_strfmt_null(buf, len + 1);
		break;
	case OSC_TT_BLOB:
		{
			char *blob = v(a, b);
			len = osc_strfmt_blob(NULL, 0, blob);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_blob(buf, len + 1, blob);
		}
		break;
	case OSC_TT_NATIVE:
		len = pl(a);
		buf = pp(a);
		break;
	default: ;

	}
	*_len = len;
	*_buf = buf;
}

t_osc_atom_m *osc_atom_format_m(t_osc_atom_m *a, int level)
{
	if(!a){
		return (t_osc_atom_m *)osc_atom_undefined;
	}
	if(pl(a) && pp(a)){
		return a;
	}
	int len = 0;
	char *ptr = NULL;
	_osc_atom_format((t_osc_atom *)a, &len, &ptr, level);
	if(ptr){
		pl(a) = len;
		pp(a) = ptr;
	}
	return a;
}

t_osc_atom *osc_atom_format(t_osc_atom *a, int level)
{
	if(!a){
		return osc_atom_undefined;
	}
	if(pl(a) && pp(a)){
		return osc_atom_retain(a);
	}
	int len = 0;
	char *ptr = NULL;
	_osc_atom_format(a, &len, &ptr, level);
	t_osc_atom *ret = osc_atom_clone(a);
	if(ptr){
		pl((t_osc_atom_m *)ret) = len;
		pp((t_osc_atom_m *)ret) = ptr;
	}
	return ret;
}

t_osc_atom *osc_atom_promote(t_osc_atom *a, char typetag)
{
	if(!a){
		return osc_atom_undefined;
	}
	char tt = tt(a);
	if(tt == typetag){
		return osc_atom_retain(a);
	}
	char newtt = osc_typetag_isSubtype(tt, typetag);
	if(newtt != typetag){
		return osc_atom_undefined;
	}
	return osc_atom_convertAny(a, typetag);
}

t_osc_atom *osc_atom_convertAny(t_osc_atom *a, char typetag)
{
	if(!a){
		return osc_atom_undefined;
	}
	char tt = tt(a);
	if(tt == typetag){
		return osc_atom_retain(a);
	}
	t_osc_atom *ret = NULL;
	switch(tt){
	case OSC_TT_I8:
		switch(typetag){
		case OSC_TT_I8:
			return osc_atom_retain(a);
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, c), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, c), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, c), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, c), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, c), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, c), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, c), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, c), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, c), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				t_osc_msg *m = osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a));
				t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, m);
				OSC_ATOM_ALLOC(aa, b, OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, c)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_U8:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, C), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			return osc_atom_retain(a);
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, C), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, C), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, C), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, C), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, C), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, C), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, C), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, C), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, C)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;

		default: break;
		}
		break;
	case OSC_TT_I16:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, u), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, u), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			return osc_atom_retain(a);
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, u), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, u), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, u), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, u), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, u), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, u), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, u), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, u)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_U16:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, U), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, U), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, U), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			return osc_atom_retain(a);
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, U), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, U), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, U), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, U), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, U), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, U), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, U)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_I32:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, i), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, i), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, i), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, i), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			return osc_atom_retain(a);
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, i), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, i), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, i), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, i), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, i), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, i)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_U32:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, I), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, I), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, I), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, I), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, I), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			return osc_atom_retain(a);
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, I), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, I), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, I), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, I), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, I)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_I64:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, h), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, h), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, h), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, h), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, h), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, h), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			return osc_atom_retain(a);
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, h), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, h), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, h), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, h)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_U64:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, H), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, H), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, H), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, H), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, H), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, H), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, H), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			return osc_atom_retain(a);
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, H), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, H), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, H)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_F32:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, f), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, f), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, f), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, f), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, f), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, f), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, f), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, f), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			return osc_atom_retain(a);
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, f), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, f)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_F64:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, d), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, d), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, d), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, d), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, d), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, d), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, d), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, d), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, d), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			return osc_atom_retain(a);
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, d)), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_STR:
	case OSC_TT_SYM:
		switch(typetag){
		case OSC_TT_I8:
			{
				char *ptr = v(a, s);
				if(ptr){
					OSC_ATOM_ALLOC(aa, *ptr, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U8:
			{
				char *ptr = v(a, s);
				if(ptr){
					OSC_ATOM_ALLOC(aa, *ptr, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I16:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int16_t)strtol(ptr, &endptr, 0), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U16:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int16_t)strtol(ptr, &endptr, 0), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I32:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int32_t)strtol(ptr, &endptr, 0), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U32:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (uint32_t)strtoul(ptr, &endptr, 0), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I64:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int64_t)strtoll(ptr, &endptr, 0), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U64:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (uint64_t)strtoull(ptr, &endptr, 0), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_F32:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, strtof(ptr, &endptr), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_F64:
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, strtod(ptr, &endptr), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_STR:
		case OSC_TT_SYM:
			return osc_atom_retain(a);
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				char *ptr = v(a, s);
				if(ptr){
					t_osc_timetag t;
					osc_timetag_fromISO8601(ptr, &t);
					OSC_ATOM_ALLOC(aa, t, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_BNDL:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, 0, f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, 0, d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_TIME:
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, (int8_t)osc_timetag_timetagToFloat(v(a, t)), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)osc_timetag_timetagToFloat(v(a, t)), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, (int16_t)osc_timetag_timetagToFloat(v(a, t)), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)osc_timetag_timetagToFloat(v(a, t)), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, (int32_t)osc_timetag_timetagToFloat(v(a, t)), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)osc_timetag_timetagToFloat(v(a, t)), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, (int64_t)osc_timetag_timetagToFloat(v(a, t)), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)osc_timetag_timetagToFloat(v(a, t)), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, (float)osc_timetag_timetagToFloat(v(a, t)), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_timetagToFloat(v(a, t)), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			return osc_atom_retain(a);
		case OSC_TT_BLOB:
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = out;
				osc_atom_release(aa);
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case OSC_TT_BLOB:
		switch(typetag){
		case OSC_TT_I8:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int8_t)ntoh32(*((int32_t *)(ptr + 4))), c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U8:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint8_t)ntoh32(*((int32_t *)(ptr + 4))), C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I16:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int16_t)ntoh32(*((int32_t *)(ptr + 4))), u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U16:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint16_t)ntoh32(*((int32_t *)(ptr + 4))), U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I32:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int32_t)ntoh32(*((int32_t *)(ptr + 4))), i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U32:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint32_t)ntoh32(*((int32_t *)(ptr + 4))), I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_I64:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int64_t)ntoh32(*((int32_t *)(ptr + 4))), h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_U64:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint64_t)ntoh32(*((int32_t *)(ptr + 4))), H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_F32:
			{
				char *ptr = v(a, b);
				int32_t i = ntoh32(*((int32_t *)(ptr + 4)));
				if(ptr){
					OSC_ATOM_ALLOC(aa, *((float *)&i), f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_F64:
			{
				char *ptr = v(a, b);
				int64_t i = ntoh64(*((int64_t *)(ptr + 4)));
				if(ptr){
					OSC_ATOM_ALLOC(aa, *((double *)&i), d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, osc_timetag_decodeFromHeader(ptr + 4), t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case OSC_TT_BLOB:
			return osc_atom_retain(a);
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case 'T':
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, 0., f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, 0., d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 'T':
			return osc_atom_retain(a);
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case 'F':
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, 0., f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, 0., d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			return osc_atom_retain(a);
		case 'N':
			ret = osc_atom_nil;
			break;
		default: break;
		}
		break;
	case 'N':
		switch(typetag){
		case OSC_TT_I8:
			{
				OSC_ATOM_ALLOC(aa, 0, c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U8:
			{
				OSC_ATOM_ALLOC(aa, 0, C, OSC_TT_U8, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I16:
			{
				OSC_ATOM_ALLOC(aa, 0, u, OSC_TT_I16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U16:
			{
				OSC_ATOM_ALLOC(aa, 0, U, OSC_TT_U16, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I32:
			{
				OSC_ATOM_ALLOC(aa, 0, i, OSC_TT_I32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U32:
			{
				OSC_ATOM_ALLOC(aa, 0, I, OSC_TT_U32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_I64:
			{
				OSC_ATOM_ALLOC(aa, 0, h, OSC_TT_I64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_U64:
			{
				OSC_ATOM_ALLOC(aa, 0, H, OSC_TT_U64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F32:
			{
				OSC_ATOM_ALLOC(aa, 0., f, OSC_TT_F32, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_F64:
			{
				OSC_ATOM_ALLOC(aa, 0., d, OSC_TT_F64, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_STR:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, OSC_TT_STR, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_SYM:
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, OSC_TT_SYM, 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_TT_BNDL:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_TT_BNDL_ID, OSC_TT_BNDL, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case OSC_TT_TIME:
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, OSC_TT_TIME, 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case OSC_TT_BLOB:
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, OSC_TT_BLOB, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 'T':
			ret = osc_atom_true;
			break;
		case 'F':
			ret = osc_atom_false;
			break;
		case 'N':
			return osc_atom_retain(a);

		default: break;
		}
		break;
	default: return osc_atom_undefined;
	}
	if(ret){
		return ret;
	}else{
		return osc_atom_undefined;
	}
}

t_osc_atom *osc_atom_apply(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_atom *a, t_osc_bndl *context)
{
	if(!fn){
		return NULL;
	}
	return fn(a, context);
}

t_osc_atom *osc_atom_match(t_osc_atom *lhs, t_osc_atom *rhs)
{
	if(!lhs || !rhs){
		return osc_atom_false;
	}
	if(tt(lhs) != tt(rhs)){
		return osc_atom_false;
	}
	if(tt(lhs) != OSC_TT_STR && tt(lhs) != OSC_TT_SYM){
		return osc_atom_false;
	}
	char *s1 = v(lhs, s); // "pattern"
	char *s2 = v(rhs, s); // "address"
	int po = 0, ao = 0;
	int ret = osc_match(s1, s2, &po, &ao);
	if((ret & OSC_MATCH_ADDRESS_COMPLETE) && (ret & OSC_MATCH_PATTERN_COMPLETE)){
		return osc_atom_true;
	}else if(po > 0 && ((s1[po] == '/') || ((s1[po] == '*') && (s1[po + 1] == '\0')))){
		int len = strlen(s1);
		char *buf = osc_mem_alloc(len + 1);
		memcpy(buf, s1 + po, len - po);
		buf[len - po] = '\0';
		OSC_ATOM_ALLOC(ret, buf, s, OSC_TT_STR, 0, NULL, 0, NULL, 1, 1);
		return ret;
	}else{
		return osc_atom_false;
	}
}

t_osc_atom *_osc_atom_nth(t_osc_atom *a, int n)
{
	if(!a || n < 0){
		return osc_atom_undefined;
	}
	switch(tt(a)){
	case OSC_TT_STR:
	case OSC_TT_SYM:
		{
			char *ptr = v(a, s);
			int len = strlen(ptr);
			if(n < len){
				OSC_ATOM_ALLOC(ret, ptr[n], c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				return ret;
			}else{
				return osc_atom_undefined;
			}
		}
	case OSC_TT_BLOB:
		{
			char *ptr = v(a, b);
			int len = ntoh32(*((int32_t *)ptr));
			ptr += 4;
			if(n < len){
				OSC_ATOM_ALLOC(ret, ptr[n], c, OSC_TT_I8, 0, NULL, 0, NULL, 1, 0);
				return ret;
			}else{
				return osc_atom_undefined;
			}
		}
	case OSC_TT_BNDL:
		return osc_msg_nth(osc_bndl_nth(v(a, OSC_TT_BNDL_ID), n), 0);
	default:
		return osc_atom_undefined;
	}
}

t_osc_atom *osc_atom_nth(t_osc_atom *a, t_osc_atom *n)
{
	if(!a || !n){
		return osc_atom_undefined;
	}
	if(tt(n) == OSC_TT_I32){
		return _osc_atom_nth(a, v(n, i));
	}else if(OSC_TYPETAG_ISINT(tt(n))){
		return _osc_atom_nth(a, (int32_t)v(n, i));
	}else{
		char tt = osc_typetag_findLUB(tt(n), OSC_TT_I32);
		t_osc_atom *_nn = osc_atom_convertAny(n, tt);
		t_osc_atom nn = *_nn;
		osc_atom_release(_nn);
		return osc_atom_nth(a, &nn);
	}
}

#define OSC_ATOM_IBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_I32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_I32);			\
	OSC_ATOM_ALLOC(varname, v(l, i) op v(r, i), i, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_U32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_U32);			\
	OSC_ATOM_ALLOC(varname, v(l, I) op v(r, I), I, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_IBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_I64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_I64);			\
	OSC_ATOM_ALLOC(varname, v(l, h) op v(r, h), h, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_U64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_U64);			\
	OSC_ATOM_ALLOC(varname, v(l, H) op v(r, H), H, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_F32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_F32);			\
	OSC_ATOM_ALLOC(varname, v(l, f) op v(r, f), f, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_F64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_F64);			\
	OSC_ATOM_ALLOC(varname, v(l, d) op v(r, d), d, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_BINOP(varname, op, lhs, rhs)				\
	if(!lhs || !rhs){\
		return osc_atom_undefined;\
	}\
	if(!OSC_TYPETAG_ISNUMERIC(tt(lhs)) || !OSC_TYPETAG_ISNUMERIC(tt(rhs))){ \
		return osc_atom_undefined;				\
	}								\
	char tt = osc_typetag_findLUB(tt(lhs), tt(rhs));		\
	if(!OSC_TYPETAG_ISNUMERIC(tt)){					\
		return osc_atom_undefined;				\
	}\
	t_osc_atom *varname = NULL;\
	if(OSC_TYPETAG_ISINT(tt)){\
		if(OSC_TYPETAG_ISINT32(tt)){\
			if(OSC_TYPETAG_ISSIGNED(tt)){\
				OSC_ATOM_IBINOP32(a, tt, op, lhs, rhs);\
				varname = a;\
			}else{\
				OSC_ATOM_UIBINOP32(a, tt, op, lhs, rhs);\
				varname = a;\
			}\
		}else{\
			if(OSC_TYPETAG_ISSIGNED(tt)){			\
				OSC_ATOM_IBINOP64(a, tt, op, lhs, rhs);	\
				varname = a;				\
			}else{						\
				OSC_ATOM_UIBINOP64(a, tt, op, lhs, rhs); \
				varname = a;				\
			}						\
		}\
	}else if(OSC_TYPETAG_ISFLOAT(tt)){\
		if(tt == OSC_TT_F32){\
			OSC_ATOM_FBINOP32(a, tt, op, lhs, rhs);\
			varname = a;\
		}else{\
			OSC_ATOM_FBINOP64(a, tt, op, lhs, rhs);\
			varname = a;\
		}\
	}

#define OSC_ATOM_IBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_I32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_I32);			\
	t_osc_atom *varname = v(l, i) op v(r, i) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_U32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_U32);			\
	t_osc_atom *varname = v(l, I) op v(r, I) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_IBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_I64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_I64);			\
	t_osc_atom *varname = v(l, h) op v(r, h) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_U64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_U64);			\
	t_osc_atom *varname = v(l, H) op v(r, H) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_F32);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_F32);			\
	t_osc_atom *varname = v(l, f) op v(r, f) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, OSC_TT_F64);			\
	t_osc_atom *r = osc_atom_convertAny(rhs, OSC_TT_F64);			\
	t_osc_atom *varname = v(l, d) op v(r, d) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_BOOLOP(varname, op, lhs, rhs)				\
	if(!lhs || !rhs){						\
		return osc_atom_undefined;				\
	}								\
	if(!OSC_TYPETAG_ISNUMERIC(tt(lhs)) || !OSC_TYPETAG_ISNUMERIC(tt(rhs))){ \
		return osc_atom_undefined;				\
	}								\
	char tt = osc_typetag_findLUB(tt(lhs), tt(rhs));		\
	if(!OSC_TYPETAG_ISNUMERIC(tt)){					\
		return osc_atom_undefined;				\
	}\
	t_osc_atom *varname = NULL;\
	if(OSC_TYPETAG_ISINT(tt)){\
		if(OSC_TYPETAG_ISINT32(tt)){\
			if(OSC_TYPETAG_ISSIGNED(tt)){\
				OSC_ATOM_IBOOLOP32(a, tt, op, lhs, rhs);\
				varname = a;\
			}else{\
				OSC_ATOM_UIBOOLOP32(a, tt, op, lhs, rhs);\
				varname = a;\
			}\
		}else{\
			if(OSC_TYPETAG_ISSIGNED(tt)){			\
				OSC_ATOM_IBOOLOP64(a, tt, op, lhs, rhs);	\
				varname = a;				\
			}else{						\
				OSC_ATOM_UIBOOLOP64(a, tt, op, lhs, rhs); \
				varname = a;				\
			}						\
		}\
	}else if(OSC_TYPETAG_ISFLOAT(tt)){\
		if(tt == OSC_TT_F32){\
			OSC_ATOM_FBOOLOP32(a, tt, op, lhs, rhs);\
			varname = a;\
		}else{\
			OSC_ATOM_FBOOLOP64(a, tt, op, lhs, rhs);\
			varname = a;\
		}\
	}

t_osc_atom *osc_atom_add(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BINOP(ret, +, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_sub(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BINOP(ret, -, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_mul(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BINOP(ret, *, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_div(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BINOP(ret, /, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_eql(t_osc_atom *lhs, t_osc_atom *rhs)
{
	if(!lhs || !rhs){
		return osc_atom_false;
	}
	if(lhs == rhs){
		return osc_atom_true;
	}
	if(tt(lhs) == tt(rhs)){
		switch(tt(lhs)){
		case OSC_TT_I8:
			return v(lhs, c) == v(rhs, c) ? osc_atom_true : osc_atom_false;
		case OSC_TT_U8:
			return v(lhs, C) == v(rhs, C) ? osc_atom_true : osc_atom_false;
		case OSC_TT_I16:
			return v(lhs, u) == v(rhs, u) ? osc_atom_true : osc_atom_false;
		case OSC_TT_U16:
			return v(lhs, U) == v(rhs, U) ? osc_atom_true : osc_atom_false;
		case OSC_TT_I32:
			return v(lhs, i) == v(rhs, i) ? osc_atom_true : osc_atom_false;
		case OSC_TT_U32:
			return v(lhs, I) == v(rhs, I) ? osc_atom_true : osc_atom_false;
		case OSC_TT_I64:
			return v(lhs, h) == v(rhs, h) ? osc_atom_true : osc_atom_false;
		case OSC_TT_U64:
			return v(lhs, H) == v(rhs, H) ? osc_atom_true : osc_atom_false;
		case OSC_TT_F32:
			return v(lhs, f) == v(rhs, f) ? osc_atom_true : osc_atom_false;
		case OSC_TT_F64:
			return v(lhs, d) == v(rhs, d) ? osc_atom_true : osc_atom_false;
		case OSC_TT_STR:
		case OSC_TT_SYM:
			return v(lhs, s) == v(rhs, s) ? osc_atom_true : strcmp(v(lhs, s), v(rhs, s)) == 0 ? osc_atom_true : osc_atom_false;
		case OSC_TT_BNDL:
			// need to call osc_bundle_eql which doesn't exist yet
			return v(lhs, OSC_TT_BNDL_ID) == v(rhs, OSC_TT_BNDL_ID) ? osc_atom_true : osc_atom_false;
		case OSC_TT_TIME:
			return osc_timetag_compare(v(lhs, t), v(rhs, t)) == 0 ? osc_atom_true : osc_atom_false;
		case OSC_TT_BLOB:
			if(v(lhs, b) == v(rhs, b)){
				return osc_atom_true;
			}else{
				int32_t s1 = ntoh32(*((int32_t *)v(lhs, b)));
				int32_t s2 = ntoh32(*((int32_t *)v(rhs, b)));
				if(s1 == s2){
					return memcmp(v(lhs, b) + 4, v(rhs, b) + 4, s1) == 0 ? osc_atom_true : osc_atom_false;
				}else{
					return osc_atom_false;
				}
			}
		case 'T':
		case 'F':
		case 'N':
		default:
			return osc_atom_true;
		}
	}else{
		return osc_atom_false;
	}
}

t_osc_atom *osc_atom_eqv(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BOOLOP(ret, ==, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_lt(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BOOLOP(ret, <, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_le(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BOOLOP(ret, <=, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_gt(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BOOLOP(ret, >, lhs, rhs);
	return ret;
}

t_osc_atom *osc_atom_ge(t_osc_atom *lhs, t_osc_atom *rhs)
{
	OSC_ATOM_BOOLOP(ret, >=, lhs, rhs);
	return ret;
}

t_osc_msg *osc_atom_value(t_osc_atom *a)
{
	if(!a){
		return NULL;
	}
	if(tt(a) == OSC_TT_BNDL){
		t_osc_bndl *b = v(a, OSC_TT_BNDL_ID);
		if(!b){
			return NULL;
		}
		return osc_bndl_value(b);
	}
	return osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a));
}

t_osc_atom *osc_atom_evalExpr(t_osc_atom *expr, t_osc_bndl *context)
{
	if(!expr){
		return osc_atom_undefined;
	}
	t_osc_bndl *b = v(expr, OSC_TT_BNDL_ID);
	t_osc_msg *typem = NULL;
	for(int i = 0; i < osc_bndl_length(b); i++){
		t_osc_msg *m = osc_bndl_nth(b, i);
		if(osc_atom_match(osc_atom_typeaddress, osc_msg_nth(m, 0)) == osc_atom_true){
			typem = m;
			break;
		}
	}
	if(!typem){
		// return bundle with debugging info
		return osc_atom_allocBndl(osc_bndl_empty, 0);
	}
	t_osc_atom *type = osc_msg_nth(typem, 1);
	if(osc_atom_getTypetag(type) != OSC_TT_I8){
		// return bundle with debugging info
		return osc_atom_allocBndl(osc_bndl_empty, 0);
	}
	switch(v(type, c)){
	case OSC_TT_F32: // function call
		{
			t_osc_msg *funcm = NULL;
			t_osc_msg *argsm = NULL;
			for(int i = 0; i < osc_bndl_length(b); i++){
				t_osc_msg *m = osc_bndl_nth(b, i);
				if(osc_atom_match(osc_atom_funcaddress, osc_msg_nth(m, 0)) == osc_atom_true){
					funcm = m;
				}else if(osc_atom_match(osc_atom_argsaddress, osc_msg_nth(m, 0)) == osc_atom_true){
					argsm = m;
				}
				if(funcm && argsm){
					break;
				}
			}
			if(!funcm || !argsm){
				return osc_atom_undefined;
			}
			
			t_osc_atom *args = osc_atom_eval(osc_msg_nth(argsm, 1), context);
			t_osc_atom *func = NULL;
			t_osc_atom *first = osc_msg_nth(funcm, 1);
			switch(tt(first)){
			case OSC_TT_SYM:
				{
					t_osc_msg *m = osc_bndl_lookup(context, first, osc_atom_match);
					if(m){
						func = osc_atom_retain(osc_msg_nth(m, 1));
					}else{
						osc_atom_retain(expr);
					}
				}
				break;
			case OSC_TT_BNDL:
				func = osc_atom_retain(first);
				break;
			default:
				func = osc_atom_eval(first, context);
				break;
			}
			t_osc_msg *argsv = osc_atom_value(args);
			t_osc_atom *argsb = NULL;
			if(argsv){
				argsb = osc_atom_retain(osc_msg_nth(argsv, 2));
				osc_msg_release(argsv);
			}else{
				argsb = osc_atom_retain(args);
			}
 
			t_osc_msg *funcv = osc_atom_value(func);
			t_osc_atom *funcb = NULL;
			if(funcv){
				funcb = osc_atom_retain(osc_msg_nth(funcv, 1));
				osc_msg_release(funcv);
			}else{
				funcb = osc_atom_retain(func);
			}
			t_osc_bndl *bb = osc_bndl_union(v(argsb, OSC_TT_BNDL_ID), v(funcb, OSC_TT_BNDL_ID));
			
			t_osc_msg *expr = osc_bndl_lookup(bb, osc_atom_expraddress, osc_atom_match);
			t_osc_atom *r = NULL;
			if(expr){
				r = osc_atom_eval(osc_msg_nth(expr, 1), bb);
			}else{
				r = osc_atom_allocBndl(osc_bndl_retain(bb), 1);
			}
			osc_atom_release(args);
			osc_atom_release(func);
			osc_msg_release(argsv);
			osc_atom_release(argsb);
			osc_msg_release(funcv);
			osc_atom_release(funcb);
			osc_bndl_release(bb);
			return r;
		}
		break;
	default:
		return osc_atom_undefined;
	}
	return osc_atom_undefined;
}

t_osc_atom *osc_atom_evalSymbol(t_osc_atom *a, t_osc_bndl *context)
{
	t_osc_bndl *partialb = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	t_osc_bndl *completeb = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	t_osc_bndl *unmatchedb = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	for(int i = 0; i < osc_bndl_length(context); i++){
		t_osc_msg *m = osc_bndl_nth(context, i);
		t_osc_atom *address = osc_msg_nth(m, 0);
		t_osc_atom *res = osc_atom_match(address, a);
		if(res == osc_atom_true){
			// complete match
			t_osc_msg *em = osc_msg_alloc(osc_atom_retain(osc_msg_nth(m, 0)), 0);
			for(int i = 0; i < osc_msg_length(m); i++){
				t_osc_atom *ea = NULL;
				if(tt(a) == OSC_TT_BNDL){
					ea = osc_atom_eval(osc_msg_nth(m, i + 1), NULL);
				}else{
					ea = osc_atom_eval(osc_msg_nth(m, i + 1), context);
				}
				osc_msg_append_m((t_osc_msg_m *)em, ea);
			}
			osc_bndl_append_m((t_osc_bndl_m *)completeb, em);
		}else if(res == osc_atom_false){
			// no match
			osc_bndl_append_m((t_osc_bndl_m *)unmatchedb, osc_msg_retain(m));
		}else{
			// partial match
			t_osc_msg *em = osc_msg_alloc(osc_atom_retain(res), 0);
			for(int i = 0; i < osc_msg_length(m); i++){
				t_osc_atom *ea = NULL;
				if(tt(a) == OSC_TT_BNDL){
					ea = osc_atom_eval(osc_msg_nth(m, i + 1), NULL);
				}else{
					ea = osc_atom_eval(osc_msg_nth(m, i + 1), context);
				}
				osc_msg_append_m((t_osc_msg_m *)em, ea);
			}
			osc_bndl_append_m((t_osc_bndl_m *)partialb, em);
		}
	}
	/*
	if(osc_bndl_length(partialb) == 0 && osc_bndl_length(completeb) == 0){
		osc_bndl_release(partialb);
		osc_bndl_release(completeb);
		osc_bndl_release(unmatchedb);
		return osc_atom_retain(a);
	}
	*/
	t_osc_msg *partialm = osc_msg_alloc(osc_atom_partialaddress, 1, osc_atom_allocBndl(partialb, 1));
	t_osc_msg *completem = osc_msg_alloc(osc_atom_completeaddress, 1, osc_atom_allocBndl(completeb, 1));
	t_osc_msg *unmatchedm = osc_msg_alloc(osc_atom_unmatchedaddress, 1, osc_atom_allocBndl(unmatchedb, 1));
	t_osc_bndl *out = NULL;
	if(osc_bndl_length(completeb) == 0){
		t_osc_msg *valuem = osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_undefined);
		out = osc_bndl_alloc(OSC_TIMETAG_NULL, 4, partialm, completem, unmatchedm, valuem);
	}else{
		//t_osc_msg *valuem = osc_msg_assocn(osc_bndl_nth(completeb, 0), osc_atom_valueaddress, 0);
		t_osc_msg *valuem = osc_msg_prepend(osc_bndl_nth(completeb, 0), osc_atom_valueaddress);
		out = osc_bndl_alloc(OSC_TIMETAG_NULL, 4, partialm, completem, unmatchedm, valuem);
	}
	return osc_atom_allocBndl(out, 1);
}

t_osc_atom *osc_atom_eval(t_osc_atom *a, t_osc_bndl *context)
{
	if(!a){
		return osc_atom_undefined;
	}
	t_osc_atom *r = NULL;
	switch(tt(a)){
	case OSC_TT_BNDL:
		r = osc_atom_allocBndl(osc_bndl_eval(v(a, OSC_TT_BNDL_ID), context), 1);
		break;
	case OSC_TT_EXPR:
		r = osc_atom_evalExpr(a, context);
		break;
	case OSC_TT_SYM:
		r = osc_atom_evalSymbol(a, context);
		break;
	case OSC_TT_NATIVE:
		r = osc_atom_allocBndl(v(a, OSC_TT_NATIVE_ID)(context), 1);
		break;
	default:
		r = osc_atom_retain(a);
		break;
	}
	return r;
}

void osc_atom_print(t_osc_atom *a)
{
	if(a){
		t_osc_atom *aa = osc_atom_format(a, 0);
		printf("%s\n", osc_atom_getPrettyPtr(aa));
		osc_atom_release(aa);
	}
}
