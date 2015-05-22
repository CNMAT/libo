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

#define OSC_BUNDLE_TYPETAG_ID B
#define OSC_NATIVE_TYPETAG_ID n
#define OSC_EXPR_TYPETAG_ID A

#pragma pack(push)
#pragma pack(4)
struct _osc_atom
{
	t_osc_obj obj;
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
		t_osc_bndl *OSC_BUNDLE_TYPETAG_ID;
		t_osc_timetag t;
		char *b;
		//t_osc_bndl *(*OSC_NATIVE_TYPETAG_ID)(t_osc_bndl *);
		t_osc_builtin OSC_NATIVE_TYPETAG_ID;
		t_osc_bndl *OSC_EXPR_TYPETAG_ID;
	} value;
	char typetag;
	int serialized_len;
	char *serialized_ptr;
	int pretty_len;
	char *pretty_ptr;
	int should_free;
	int static_atom;
};
#pragma pack(pop)

#define v(a, f) ((a)->value.f)
#define vv(a) ((a)->value)
#define tt(a) ((a)->typetag)
#define sl(a) ((a)->serialized_len)
#define sp(a) ((a)->serialized_ptr)
#define pl(a) ((a)->pretty_len)
#define pp(a) ((a)->pretty_ptr)
#define rc(a) ((a)->obj.refcount)
#define sf(a) ((a)->should_free)
#define st(a) ((a)->static_atom)


static t_osc_atom _osc_atom_true = { {-1, NULL}, {0}, 'T', 0, NULL, 4, "true", 0, 1 };
t_osc_atom *osc_atom_true = &_osc_atom_true;
static t_osc_atom _osc_atom_false = { {-1, NULL}, {0}, 'F', 0, NULL, 5, "false", 0, 1 };
t_osc_atom *osc_atom_false = &_osc_atom_false;
static t_osc_atom _osc_atom_nil = { {-1, NULL}, {0}, 'N', 0, NULL, 3, "nil", 0, 1 };
t_osc_atom *osc_atom_nil = &_osc_atom_nil;
static t_osc_atom _osc_atom_undefined = { {-1, NULL}, {0}, OSC_UNDEFINED_TYPETAG, 0, NULL, 9, "undefined", 0, 1 };
t_osc_atom *osc_atom_undefined = &_osc_atom_undefined;

static t_osc_atom _osc_atom_emptystring = { {-1, NULL}, { .s = "\0" }, 's', 4, "\0\0\0\0", 4, "\0\0\0\0", 0, 1};
t_osc_atom *osc_atom_emptystring = &_osc_atom_emptystring;

static t_osc_atom _osc_atom_valueaddress = { {-1, NULL}, {.s = "/value" }, 's', 8, "/value\0\0", 6, "/value", 0, 1};
t_osc_atom *osc_atom_valueaddress = &_osc_atom_valueaddress;
static t_osc_atom _osc_atom_typeaddress = { {-1, NULL}, { .s = "/type" }, 's', 8, "/type\0\0\0", 5, "/type", 0, 1};
t_osc_atom *osc_atom_typeaddress = &_osc_atom_typeaddress;
static t_osc_atom _osc_atom_funcaddress = { {-1, NULL}, { .s = "/func" }, 's', 8, "/func\0\0\0", 5, "/func", 0, 1};
t_osc_atom *osc_atom_funcaddress = &_osc_atom_funcaddress;
static t_osc_atom _osc_atom_argsaddress = { {-1, NULL}, { .s = "/args" }, 's', 8, "/args\0\0\0", 5, "/args", 0, 1};
t_osc_atom *osc_atom_argsaddress = &_osc_atom_argsaddress;
static t_osc_atom _osc_atom_partialaddress = { {-1, NULL}, { .s = "/partial" }, 's', 12, "/partial\0\0\0\0", 8, "/partial", 0, 1};
t_osc_atom *osc_atom_partialaddress = &_osc_atom_partialaddress;
static t_osc_atom _osc_atom_completeaddress = { {-1, NULL}, { .s = "/complete" }, 's', 12, "/complete\0\0\0", 9, "/complete", 0, 1};
t_osc_atom *osc_atom_completeaddress = &_osc_atom_completeaddress;
static t_osc_atom _osc_atom_unmatchedaddress = { {-1, NULL}, { .s = "/unmatched" }, 's', 12, "/unmatched\0\0", 10, "/unmatched", 0, 1};
t_osc_atom *osc_atom_unmatchedaddress = &_osc_atom_unmatchedaddress;

static int osc_atom_changeRefCount(t_osc_atom *a, int amount);

#define OSC_ATOM_ALLOC(varname, value, unionfield, typetag, serialized_len, serialized_ptr, pretty_len, pretty_ptr, refcount, should_free) \
	void *OSC_UID(__osc_atom_alloc_ptr__) = osc_mem_alloc(sizeof(t_osc_atom));	\
	t_osc_atom OSC_UID(__osc_atom_alloc_a__) = {{refcount, osc_atom_free}, {.unionfield = value}, typetag, serialized_len, serialized_ptr, pretty_len, pretty_ptr, should_free, 0}; \
	memcpy(OSC_UID(__osc_atom_alloc_ptr__), &OSC_UID(__osc_atom_alloc_a__), sizeof(t_osc_atom)); \
	t_osc_atom *varname = (t_osc_atom *)OSC_UID(__osc_atom_alloc_ptr__);

// serialize and pretty print on init?
t_osc_atom *osc_atom_allocInt8(int8_t i)
{
	OSC_ATOM_ALLOC(a, i, c, 'c', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt8(uint8_t i)
{
	OSC_ATOM_ALLOC(a, i, C, 'C', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt16(int16_t i)
{
	OSC_ATOM_ALLOC(a, i, u, 'u', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt16(uint16_t i)
{
	OSC_ATOM_ALLOC(a, i, U, 'U', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt32(int32_t i)
{
	OSC_ATOM_ALLOC(a, i, i, 'i', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt32(uint32_t i)
{
	OSC_ATOM_ALLOC(a, i, I, 'I', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocInt64(int64_t i)
{
	OSC_ATOM_ALLOC(a, i, h, 'h', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocUInt64(uint64_t i)
{
	OSC_ATOM_ALLOC(a, i, H, 'H', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocFloat(float i)
{
	OSC_ATOM_ALLOC(a, i, f, 'f', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocDouble(double i)
{
	OSC_ATOM_ALLOC(a, i, d, 'd', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocString(char *i, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, i, s, 's', 0, NULL, 0, NULL, 1, should_free_ptr);
	return a;
}

t_osc_atom *osc_atom_allocSymbol(char *i, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, i, S, 'S', 0, NULL, 0, NULL, 1, should_free_ptr);
	return a;
}

t_osc_atom *osc_atom_allocBndl(t_osc_bndl *b, int should_free_bndl)
{
	OSC_ATOM_ALLOC(a, b, OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, should_free_bndl);
	return a;
}

t_osc_atom *osc_atom_allocTimetag(t_osc_timetag i)
{
	OSC_ATOM_ALLOC(a, i, t, 't', 0, NULL, 0, NULL, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocBlob(char *b, int should_free_ptr)
{
	OSC_ATOM_ALLOC(a, b, b, 'b', 0, NULL, 0, NULL, 1, should_free_ptr);
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
	OSC_ATOM_ALLOC(a, f, OSC_NATIVE_TYPETAG_ID, OSC_NATIVE_TYPETAG, 0, NULL, n, buf, 1, 0);
	return a;
}

t_osc_atom *osc_atom_allocExpr(t_osc_bndl *b, int should_free)
{
	OSC_ATOM_ALLOC(a, b, OSC_EXPR_TYPETAG_ID, OSC_EXPR_TYPETAG, 0, NULL, 0, NULL, 1, should_free);
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
	case 's':
		{
			OSC_ATOM_ALLOC(ret, osc_util_strcpy(v(a, s)), s, 's', sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case 'S':
		{
			OSC_ATOM_ALLOC(ret, osc_util_strcpy(v(a, S)), S, 'S', sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case OSC_BUNDLE_TYPETAG:
		{
			OSC_ATOM_ALLOC(ret, osc_bndl_retain(v(a, OSC_BUNDLE_TYPETAG_ID)), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, sl, sp, pl, pp, 1, 1);
			return ret;
		}
	case 'b':
		{
			OSC_ATOM_ALLOC(ret, osc_util_blobcpy(v(a, b)), b, 'b', sl, sp, pl, pp, 1, 1);
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
		case 's':
		case 'S':
		case 'b':
			if(sf(a)){
				p = v(a, s);
			}
			break;
		case OSC_BUNDLE_TYPETAG:
			if(sf(a)){
				osc_bndl_release(v(a, OSC_BUNDLE_TYPETAG_ID));
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
		atoms[i] = osc_atom_convertAny((t_osc_atom *)osc_pvec2_nth(pvec2, i), 's');
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
	memcpy(ptr, prefix, prefixlen);
	ptr += prefixlen;
	memcpy(ptr, pp(atoms[0]), pl(atoms[0]));
	ptr += pl(atoms[0]);
	osc_atom_release(atoms[0]);
	memcpy(ptr, firstsep, firstseplen);
	ptr += firstseplen;
	for(int i = 1; i < n - 1; i++){
		memcpy(ptr, pp(atoms[i]), pl(atoms[i]));
		ptr += pl(atoms[i]);
	        memcpy(ptr, restsep, restseplen);
		ptr += restseplen;
		osc_atom_release(atoms[i]);
	}
	memcpy(ptr, pp(atoms[n - 1]), pl(atoms[n - 1]));
	ptr += pl(atoms[n - 1]);
	osc_atom_release(atoms[n - 1]);
	memcpy(ptr, lastsep, lastseplen);
	ptr += lastseplen;
	memcpy(ptr, postfix, postfixlen);
	ptr += postfixlen;
	*ptr = '\0';
	OSC_ATOM_ALLOC(ret, buf, s, 's', 0, NULL, (ptr - buf), osc_util_strcpy(buf), 1, 1);
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
	case 'c':
	case 'C':
	case 'u':
	case 'U':
	case 'i':
	case 'I':
		{
			len = 4;
			buf = osc_mem_alloc(len);
			int32_t i = v(a, i);
			*((int32_t *)buf) = hton32(i);
		}
		break;
	case 'f':
		{
			len = 4;
			buf = osc_mem_alloc(len);
			float f = v(a, f);
			*((int32_t *)buf) = hton32(*((int32_t *)(&f)));
		}
		break;
	case 'd':
		{
			len = 8;
			buf = osc_mem_alloc(len);
			double d = v(a, d);
			*((int64_t *)buf) = hton64(*((int64_t *)(&d)));
		}
		break;
	case 's':
	case 'S':
		{
			int l = strlen(v(a, s));
			len = osc_util_getPaddingForNBytes(l);
			buf = osc_mem_alloc(len);
			memset(buf, '\0', len);
			memcpy(buf, v(a, s), l);
		}
		break;
	case OSC_BUNDLE_TYPETAG:
		{
			t_osc_bndl *b = osc_bndl_serialize(v(a, OSC_BUNDLE_TYPETAG_ID));
			len = osc_bndl_getSerializedLen(b);
			buf = osc_bndl_getSerializedPtr(b);
			osc_bndl_setSerializedLen(b, 0);
			osc_bndl_setSerializedPtr(b, NULL);
			osc_bndl_release(b);
		}
		break;
	case OSC_TIMETAG_TYPETAG:
		len = OSC_TIMETAG_SIZEOF;
		buf = osc_mem_alloc(len);
		osc_timetag_encodeForHeader(v(a, t), buf);
		break;
	case 'h':
	case 'H':
		len = 8;
		buf = osc_mem_alloc(len);
		*((int64_t *)buf) = hton64(v(a, h));
		break;
// nothing to do for T, F, or N
	case 'b':
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
	case 'c':
		{
			int8_t i = v(a, c);
			len = osc_strfmt_int8(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int8(buf, len + 1, i);
		}
		break;
	case 'C':
		{
			uint8_t i = v(a, C);
			len = osc_strfmt_uint8(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint8(buf, len + 1, i);
		}
		break;
        case 'u':
		{
			int16_t i = v(a, u);
			len = osc_strfmt_int16(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int16(buf, len + 1, i);
		}
		break;
	case 'U':
		{
			uint16_t i = v(a, U);
			len = osc_strfmt_uint16(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint16(buf, len + 1, i);
		}
		break;
	case 'i':
		{
			int32_t i = v(a, i);
			len = osc_strfmt_int32(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int32(buf, len + 1, i);
		}
		break;
	case 'I':
		{
			uint32_t i = v(a, I);
			len = osc_strfmt_uint32(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint32(buf, len + 1, i);
		}
		break;
	case 'h':
		{
			int64_t i = v(a, h);
			len = osc_strfmt_int64(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_int64(buf, len + 1, i);
		}
		break;
	case 'H':
		{
			uint64_t i = v(a, H);
			len = osc_strfmt_uint64(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_uint64(buf, len + 1, i);
		}
		break;
	case 'f':
		{
			float i = v(a, f);
			len = osc_strfmt_float(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_float(buf, len + 1, i);
		}
		break;
	case 'd':
		{
			double i = v(a, d);
			len = osc_strfmt_double(NULL, 0, i);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_double(buf, len + 1, i);
		}
		break;
	case 's':
		{
			char *str = v(a, s);
			len = osc_strfmt_quotedStringWithQuotedMeta(NULL, 0, str);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_quotedStringWithQuotedMeta(buf, len + 1, str);
		}
		break;
	case 'S':
		{
			char *str = v(a, S);
			len = strlen(str);
			buf = osc_mem_alloc(len + 1);
			strncpy(buf, str, len + 1);
		}
		break;
	case OSC_BUNDLE_TYPETAG:
		{
			t_osc_bndl *b = osc_bndl_format(v(a, OSC_BUNDLE_TYPETAG_ID), level + 1);
			len = osc_bndl_getPrettyLen(b);
			buf = osc_bndl_getPrettyPtr(b);
			osc_bndl_setPrettyLen(b, 0);
			osc_bndl_setPrettyPtr(b, NULL);
			osc_bndl_release(b);
		}
		break;
	case 't':
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
	case 'b':
		{
			char *blob = v(a, b);
			len = osc_strfmt_blob(NULL, 0, blob);
			buf = osc_mem_alloc(len + 1);
			osc_strfmt_blob(buf, len + 1, blob);
		}
		break;
	case OSC_NATIVE_TYPETAG:
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
	case 'c':
		switch(typetag){
		case 'c':
			return osc_atom_retain(a);
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, c), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, c), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, c), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, c), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, c), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, c), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, c), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, c), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, c), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				t_osc_msg *m = osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a));
				t_osc_bndl *b = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, m);
				OSC_ATOM_ALLOC(aa, b, OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, c)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'C':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, C), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			return osc_atom_retain(a);
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, C), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, C), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, C), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, C), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, C), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, C), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, C), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, C), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, C)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'u':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, u), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, u), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			return osc_atom_retain(a);
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, u), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, u), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, u), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, u), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, u), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, u), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, u), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, u)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'U':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, U), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, U), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, U), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			return osc_atom_retain(a);
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, U), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, U), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, U), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, U), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, U), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, U), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, U)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'i':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, i), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, i), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, i), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, i), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			return osc_atom_retain(a);
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, i), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, i), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, i), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, i), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, i), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, i)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'I':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, I), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, I), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, I), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, I), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, I), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			return osc_atom_retain(a);
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, I), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, I), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, I), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, I), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, I)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'h':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, h), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, h), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, h), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, h), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, h), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, h), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			return osc_atom_retain(a);
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, h), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, h), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, h), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, h)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'H':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, H), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, H), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, H), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, H), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, H), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, H), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, H), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			return osc_atom_retain(a);
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, H), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, H), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, H)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'f':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, f), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, f), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, f), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, f), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, f), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, f), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, f), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, f), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			return osc_atom_retain(a);
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, f), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, f)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'd':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (double)v(a, d), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)v(a, d), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)v(a, d), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)v(a, d), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)v(a, d), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)v(a, d), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)v(a, d), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)v(a, d), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)v(a, d), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			return osc_atom_retain(a);
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_floatToTimetag((double)v(a, d)), t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 's':
	case 'S':
		switch(typetag){
		case 'c':
			{
				char *ptr = v(a, s);
				if(ptr){
					OSC_ATOM_ALLOC(aa, *ptr, c, 'c', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'C':
			{
				char *ptr = v(a, s);
				if(ptr){
					OSC_ATOM_ALLOC(aa, *ptr, C, 'C', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'u':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int16_t)strtol(ptr, &endptr, 0), u, 'u', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'U':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int16_t)strtol(ptr, &endptr, 0), U, 'U', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'i':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int32_t)strtol(ptr, &endptr, 0), i, 'i', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'I':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (uint32_t)strtoul(ptr, &endptr, 0), I, 'I', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'h':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (int64_t)strtoll(ptr, &endptr, 0), h, 'h', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'H':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, (uint64_t)strtoull(ptr, &endptr, 0), H, 'H', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'f':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, strtof(ptr, &endptr), f, 'f', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, f, 'f', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'd':
			{
				char *ptr = v(a, s);
				if(ptr){
					char *endptr = NULL;
					OSC_ATOM_ALLOC(aa, strtod(ptr, &endptr), d, 'd', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 's':
		case 'S':
			return osc_atom_retain(a);
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				char *ptr = v(a, s);
				if(ptr){
					t_osc_timetag t;
					osc_timetag_fromISO8601(ptr, &t);
					OSC_ATOM_ALLOC(aa, t, t, 't', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case OSC_BUNDLE_TYPETAG:
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, 0, f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, 0, d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 't':
		switch(typetag){
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, (int8_t)osc_timetag_timetagToFloat(v(a, t)), c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, (uint8_t)osc_timetag_timetagToFloat(v(a, t)), C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, (int16_t)osc_timetag_timetagToFloat(v(a, t)), u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, (uint16_t)osc_timetag_timetagToFloat(v(a, t)), U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, (int32_t)osc_timetag_timetagToFloat(v(a, t)), i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, (uint32_t)osc_timetag_timetagToFloat(v(a, t)), I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, (int64_t)osc_timetag_timetagToFloat(v(a, t)), h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, (uint64_t)osc_timetag_timetagToFloat(v(a, t)), H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, (float)osc_timetag_timetagToFloat(v(a, t)), f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, osc_timetag_timetagToFloat(v(a, t)), d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			return osc_atom_retain(a);
		case 'b':
			{
				t_osc_atom *aa = osc_atom_serialize(a);
				char *blob = osc_mem_alloc(sl(aa) + 4);
				*((int32_t *)blob) = hton32(sl(aa));
				memcpy(blob + 4, sp(aa), sl(aa));
				OSC_ATOM_ALLOC(out, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	case 'b':
		switch(typetag){
		case 'c':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int8_t)ntoh32(*((int32_t *)(ptr + 4))), c, 'c', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'C':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint8_t)ntoh32(*((int32_t *)(ptr + 4))), C, 'C', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'u':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int16_t)ntoh32(*((int32_t *)(ptr + 4))), u, 'u', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'U':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint16_t)ntoh32(*((int32_t *)(ptr + 4))), U, 'U', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'i':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int32_t)ntoh32(*((int32_t *)(ptr + 4))), i, 'i', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'I':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint32_t)ntoh32(*((int32_t *)(ptr + 4))), I, 'I', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'h':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (int64_t)ntoh32(*((int32_t *)(ptr + 4))), h, 'h', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'H':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, (uint64_t)ntoh32(*((int32_t *)(ptr + 4))), H, 'H', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'f':
			{
				char *ptr = v(a, b);
				int32_t i = ntoh32(*((int32_t *)(ptr + 4)));
				if(ptr){
					OSC_ATOM_ALLOC(aa, *((float *)&i), f, 'f', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, f, 'f', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'd':
			{
				char *ptr = v(a, b);
				int64_t i = ntoh64(*((int64_t *)(ptr + 4)));
				if(ptr){
					OSC_ATOM_ALLOC(aa, *((double *)&i), d, 'd', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, 0, d, 'd', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				char *ptr = v(a, b);
				if(ptr){
					OSC_ATOM_ALLOC(aa, osc_timetag_decodeFromHeader(ptr + 4), t, 't', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}else{
					OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
					ret = aa;
				}
				break;
			}
		case 'b':
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
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, 0., f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, 0., d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, 0., f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, 0., d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
		case 'c':
			{
				OSC_ATOM_ALLOC(aa, 0, c, 'c', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'C':
			{
				OSC_ATOM_ALLOC(aa, 0, C, 'C', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'u':
			{
				OSC_ATOM_ALLOC(aa, 0, u, 'u', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'U':
			{
				OSC_ATOM_ALLOC(aa, 0, U, 'U', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'i':
			{
				OSC_ATOM_ALLOC(aa, 0, i, 'i', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'I':
			{
				OSC_ATOM_ALLOC(aa, 0, I, 'I', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'h':
			{
				OSC_ATOM_ALLOC(aa, 0, h, 'h', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'H':
			{
				OSC_ATOM_ALLOC(aa, 0, H, 'H', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'f':
			{
				OSC_ATOM_ALLOC(aa, 0., f, 'f', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'd':
			{
				OSC_ATOM_ALLOC(aa, 0., d, 'd', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 's':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), s, 's', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case 'S':
			{
				t_osc_atom *fa = osc_atom_format(a, 0);
				OSC_ATOM_ALLOC(aa, osc_util_strcpy(pp(a)), S, 'S', 0, NULL, pl(fa), osc_util_strcpy(pp(fa)), 1, 1);
				osc_atom_release(fa);
				ret = aa;
				break;
			}
		case OSC_BUNDLE_TYPETAG:
			{
				OSC_ATOM_ALLOC(aa, osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(a))), OSC_BUNDLE_TYPETAG_ID, OSC_BUNDLE_TYPETAG, 0, NULL, 0, NULL, 1, 1);
				ret = aa;
				break;
			}
		case 't':
			{
				OSC_ATOM_ALLOC(aa, OSC_TIMETAG_NULL, t, 't', 0, NULL, 0, NULL, 1, 0);
				ret = aa;
				break;
			}
		case 'b':
			{
				char *blob = osc_mem_alloc(4);
				memset(blob, 0, 4);
				OSC_ATOM_ALLOC(aa, blob, b, 'b', 0, NULL, 0, NULL, 1, 1);
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
	if(tt(lhs) != tt(rhs)){
		return osc_atom_false;
	}
	if(tt(lhs) != 's' && tt(lhs) != 'S'){
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
		OSC_ATOM_ALLOC(ret, buf, s, 's', 0, NULL, 0, NULL, 1, 1);
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
	case 's':
	case 'S':
		{
			char *ptr = v(a, s);
			int len = strlen(ptr);
			if(n < len){
				OSC_ATOM_ALLOC(ret, ptr[n], c, 'c', 0, NULL, 0, NULL, 1, 0);
				return ret;
			}else{
				return osc_atom_undefined;
			}
		}
	case 'b':
		{
			char *ptr = v(a, b);
			int len = ntoh32(*((int32_t *)ptr));
			ptr += 4;
			if(n < len){
				OSC_ATOM_ALLOC(ret, ptr[n], c, 'c', 0, NULL, 0, NULL, 1, 0);
				return ret;
			}else{
				return osc_atom_undefined;
			}
		}
	case OSC_BUNDLE_TYPETAG:
		return osc_msg_nth(osc_bndl_nth(v(a, OSC_BUNDLE_TYPETAG_ID), n), 0);
	default:
		return osc_atom_undefined;
	}
}

t_osc_atom *osc_atom_nth(t_osc_atom *a, t_osc_atom *n)
{
	if(!a || !n){
		return osc_atom_undefined;
	}
	if(tt(n) == 'i'){
		return _osc_atom_nth(a, v(n, i));
	}else if(OSC_TYPETAG_ISINT(tt(n))){
		return _osc_atom_nth(a, (int32_t)v(n, i));
	}else{
		char tt = osc_typetag_findLUB(tt(n), 'i');
		t_osc_atom *_nn = osc_atom_convertAny(n, tt);
		t_osc_atom nn = *_nn;
		osc_atom_release(_nn);
		return osc_atom_nth(a, &nn);
	}
}

#define OSC_ATOM_IBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'i');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'i');			\
	OSC_ATOM_ALLOC(varname, v(l, i) op v(r, i), i, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'I');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'I');			\
	OSC_ATOM_ALLOC(varname, v(l, I) op v(r, I), I, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_IBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'h');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'h');			\
	OSC_ATOM_ALLOC(varname, v(l, h) op v(r, h), h, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'H');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'H');			\
	OSC_ATOM_ALLOC(varname, v(l, H) op v(r, H), H, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBINOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'f');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'f');			\
	OSC_ATOM_ALLOC(varname, v(l, f) op v(r, f), f, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBINOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'd');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'd');			\
	OSC_ATOM_ALLOC(varname, v(l, d) op v(r, d), d, tt, 0, NULL, 0, NULL, 1, 0);\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_BINOP(varname, op, lhs, rhs)				\
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
		if(tt == 'f'){\
			OSC_ATOM_FBINOP32(a, tt, op, lhs, rhs);\
			varname = a;\
		}else{\
			OSC_ATOM_FBINOP64(a, tt, op, lhs, rhs);\
			varname = a;\
		}\
	}

#define OSC_ATOM_IBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'i');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'i');			\
	t_osc_atom *varname = v(l, i) op v(r, i) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'I');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'I');			\
	t_osc_atom *varname = v(l, I) op v(r, I) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_IBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'h');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'h');			\
	t_osc_atom *varname = v(l, h) op v(r, h) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_UIBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'H');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'H');			\
	t_osc_atom *varname = v(l, H) op v(r, H) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBOOLOP32(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'f');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'f');			\
	t_osc_atom *varname = v(l, f) op v(r, f) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_FBOOLOP64(varname, tt, op, lhs, rhs)			\
	t_osc_atom *l = osc_atom_convertAny(lhs, 'd');			\
	t_osc_atom *r = osc_atom_convertAny(rhs, 'd');			\
	t_osc_atom *varname = v(l, d) op v(r, d) ? osc_atom_true : osc_atom_false;\
	osc_atom_release(l);\
	osc_atom_release(r);

#define OSC_ATOM_BOOLOP(varname, op, lhs, rhs)				\
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
		if(tt == 'f'){\
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
		case 'c':
			return v(lhs, c) == v(rhs, c) ? osc_atom_true : osc_atom_false;
		case 'C':
			return v(lhs, C) == v(rhs, C) ? osc_atom_true : osc_atom_false;
		case 'u':
			return v(lhs, u) == v(rhs, u) ? osc_atom_true : osc_atom_false;
		case 'U':
			return v(lhs, U) == v(rhs, U) ? osc_atom_true : osc_atom_false;
		case 'i':
			return v(lhs, i) == v(rhs, i) ? osc_atom_true : osc_atom_false;
		case 'I':
			return v(lhs, I) == v(rhs, I) ? osc_atom_true : osc_atom_false;
		case 'h':
			return v(lhs, h) == v(rhs, h) ? osc_atom_true : osc_atom_false;
		case 'H':
			return v(lhs, H) == v(rhs, H) ? osc_atom_true : osc_atom_false;
		case 'f':
			return v(lhs, f) == v(rhs, f) ? osc_atom_true : osc_atom_false;
		case 'd':
			return v(lhs, d) == v(rhs, d) ? osc_atom_true : osc_atom_false;
		case 's':
		case 'S':
			return v(lhs, s) == v(rhs, s) ? osc_atom_true : strcmp(v(lhs, s), v(rhs, s)) == 0 ? osc_atom_true : osc_atom_false;
		case OSC_BUNDLE_TYPETAG:
			// need to call osc_bundle_eql which doesn't exist yet
			return v(lhs, OSC_BUNDLE_TYPETAG_ID) == v(rhs, OSC_BUNDLE_TYPETAG_ID) ? osc_atom_true : osc_atom_false;
		case 't':
			return osc_timetag_compare(v(lhs, t), v(rhs, t)) == 0 ? osc_atom_true : osc_atom_false;
		case 'b':
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

// this function is probably in the wrong place
t_osc_bndl *osc_atom_evalExpr(t_osc_bndl *expr, t_osc_bndl *context)
{
	if(!expr){
		return osc_bndl_empty;
	}
	t_osc_msg *typem = NULL;
	for(int i = 0; i < osc_bndl_length(expr); i++){
		t_osc_msg *m = osc_bndl_nth(expr, i);
		if(osc_atom_match(osc_atom_typeaddress, osc_msg_nth(m, 0)) == osc_atom_true){
			typem = m;
			break;
		}
	}
	if(!typem){
		// return bundle with debugging info
		return osc_bndl_empty;
	}
	t_osc_atom *type = osc_msg_nth(typem, 1);
	if(osc_atom_getTypetag(type) != 'c'){
		// return bundle with debugging info
		return osc_bndl_empty;
	}
	switch(v(type, c)){
	case 'f': // function call
		{
			t_osc_msg *funcm = NULL;
			t_osc_msg *argsm = NULL;
			for(int i = 0; i < osc_bndl_length(context); i++){
				t_osc_msg *m = osc_bndl_nth(context, i);
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
				return osc_bndl_empty;
			}
			// argsm contains an atom with a bundle in it that needs to have every element eval'd
			// in the current context
		}
	default:
		return osc_bndl_empty;
	}
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
			t_osc_bndl *killme = completeb;
			completeb = osc_bndl_append(completeb, osc_msg_retain(m));
			osc_bndl_release(killme);
		}else if(res == osc_atom_false){
			// no match
			t_osc_bndl *killme = unmatchedb;
			unmatchedb = osc_bndl_append(unmatchedb, osc_msg_retain(m));
			osc_bndl_release(killme);
		}else{
			// partial match
			t_osc_bndl *killme = partialb;
			partialb = osc_bndl_append(partialb, osc_msg_assocn(m, res, 0));
			osc_bndl_release(killme);
		}
	}
	t_osc_msg *partialm = osc_msg_alloc(osc_atom_partialaddress, 1, osc_atom_allocBndl(partialb, 1));
	t_osc_msg *completem = osc_msg_alloc(osc_atom_completeaddress, 1, osc_atom_allocBndl(completeb, 1));
	t_osc_msg *unmatchedm = osc_msg_alloc(osc_atom_unmatchedaddress, 1, osc_atom_allocBndl(unmatchedb, 1));
	t_osc_msg *valuem = NULL;
	if(osc_bndl_length(completeb) == 0){
		valuem = osc_msg_alloc(osc_atom_valueaddress, 0);
	}else{
		valuem = osc_msg_prepend(osc_bndl_nth(completeb, 0), osc_atom_valueaddress);
	}
	t_osc_bndl *out = osc_bndl_alloc(OSC_TIMETAG_NULL, 4, partialm, completem, unmatchedm, valuem);
	return osc_atom_allocBndl(out, 1);
}

t_osc_atom *osc_atom_eval(t_osc_atom *a, t_osc_bndl *context)
{
	if(!a){
		return osc_atom_undefined;
	}
	switch(tt(a)){
	case OSC_BUNDLE_TYPETAG:
		return osc_atom_allocBndl(osc_bndl_eval(v(a, OSC_BUNDLE_TYPETAG_ID), NULL), 1);
	case OSC_EXPR_TYPETAG:
		//return osc_atom_allocBndl(osc_expr_eval(context, v(a, OSC_EXPR_TYPETAG_ID)));
		return osc_atom_undefined;
	case 'S':
		return osc_atom_evalSymbol(a, context);
	default:
		return osc_atom_retain(a);
	}
	return NULL;
}
