#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "osc.h"
#include "osc_typetag.h"
#include "osc_native.h"
#include "osc_util.h"
#include "osc_mem.h"
#include "osc_byteorder.h"
#include "osc_strfmt.h"


t_osc_native osc_native_alloc(t_osc_region r, t_osc_timetag time, char *address, char typetag, int32_t num_bytes, int32_t num_padded_bytes, char *bytes)
{
	int addresslen = osc_util_getPaddedStringLen(address);
	int32_t len = OSC_HEADER_SIZE + 4 + addresslen + 4 + num_padded_bytes;
	char *buf = (char *)osc_region_getBytes(r, 4 + len);
	memset(buf, 0, len + 4);
	char *p = buf;
	*((int32_t *)p) = hton32(len);
	p += 4;
	strncpy(p, OSC_ID, OSC_ID_SIZE);
	p += OSC_ID_SIZE;
	osc_timetag_encodeForHeader(time, p);
	p += OSC_TIMETAG_SIZEOF;
	*((int32_t *)p) = hton32(len - OSC_HEADER_SIZE - 4);
	p += 4;
	strcpy(p, address);
	p += addresslen;
	*p++ = ',';
	*p++ = typetag;
	*p++ = 0;
	*p++ = 0;
	memcpy(p, bytes, num_bytes);
	return (t_osc_native)buf;
}

t_osc_native osc_native_copy(t_osc_region r, t_osc_native n)
{
	if(n){
		int32_t len = ntoh32(*((int32_t *)n));
		char *p = osc_region_getBytes(r, len + 4);
		memcpy(p, n, len + 4);
		return p;
	}
	return NULL;
}

size_t osc_native_nformat(t_osc_region r, char *s, size_t n, t_osc_native x)
{
	switch(osc_native_getTypetag(x)){
	case OSC_TT_I8:
		return osc_strfmt_int8(s, n, osc_cvalue_value(osc_native_getInt8(x)));
	case OSC_TT_U8:
		return osc_strfmt_uint8(s, n, osc_cvalue_value(osc_native_getUInt8(x)));
	case OSC_TT_I16:
		return osc_strfmt_int16(s, n, osc_cvalue_value(osc_native_getInt16(x)));
	case OSC_TT_U16:
		return osc_strfmt_uint16(s, n, osc_cvalue_value(osc_native_getUInt16(x)));
	case OSC_TT_I32:
		return osc_strfmt_int32(s, n, osc_cvalue_value(osc_native_getInt32(x)));
	case OSC_TT_U32:
		return osc_strfmt_uint32(s, n, osc_cvalue_value(osc_native_getUInt32(x)));
	case OSC_TT_I64:
		return osc_strfmt_int64(s, n, osc_cvalue_value(osc_native_getInt64(x)));
	case OSC_TT_U64:
		return osc_strfmt_uint64(s, n, osc_cvalue_value(osc_native_getUInt64(x)));
	case OSC_TT_F32:
		return osc_strfmt_float(s, n, osc_cvalue_value(osc_native_getFloat(x)));
	case OSC_TT_F64:
		return osc_strfmt_double(s, n, osc_cvalue_value(osc_native_getDouble(x)));
	case OSC_TT_STR:
		return osc_strfmt_quotedString(s, n, osc_cvalue_value(osc_native_getPtr(x)));
	case OSC_TT_SYM:
		return osc_strfmt_stringWithQuotedMeta(s, n, osc_cvalue_value(osc_native_getPtr(x)));
	case OSC_TT_BLOB:
		return osc_strfmt_blob(s, n, osc_cvalue_value(osc_native_getPtr(x)));
	case OSC_TT_TIME:
		return osc_strfmt_timetag(s, n, osc_cvalue_value(osc_native_getTime(x)));
	case OSC_TT_TRUE:
		return osc_strfmt_bool(s, n, OSC_TT_TRUE);
	case OSC_TT_FALSE:
		return osc_strfmt_bool(s, n, OSC_TT_FALSE);
	case OSC_TT_NIL:
		return osc_strfmt_null(s, n);
	case OSC_TT_FN:
		//return osc_strfmt_stringWithQuotedMeta(s, n, osc_cvalue_value(osc_native_getFnName(x)));
		return snprintf(s, n, "<native fn : %s, %p>", osc_cvalue_value(osc_native_getFnName(x)), osc_cvalue_value(osc_native_getFn(x)));
	default: return 0;
	}
}

t_osc_timetag osc_native_getTimetag(t_osc_native n)
{
	if(n){
		return osc_timetag_decodeFromHeader(((char *)n) + 4 + OSC_ID_SIZE);
	}else{
		return OSC_TIMETAG_NULL;
	}
}

int32_t osc_native_getSize(t_osc_native n)
{
	if(n){
		return ntoh32(*((int32_t *)(((char *)n) + OSC_HEADER_SIZE + 4)));
	}else{
		return 0;
	}
}

char osc_native_getTypetag(t_osc_native n)
{
	if(n){
		char *p = (char *)n;
		p += 4 + OSC_HEADER_SIZE + 4;
		p += osc_util_getPaddedStringLen(p);
		p++;
		return *p;
	}else{
		return 0;
	}
}

char *osc_native_getData(t_osc_native n)
{
	if(n){
		char *p = (char *)n;
		p += 4 + OSC_HEADER_SIZE + 4; // bndl size, header, msg size
		p += osc_util_getPaddedStringLen(p); // address 
		p += 4; // typetag
		return p;
	}else{
		return 0;
	}
}

t_osc_native osc_native_int8(t_osc_region r, t_osc_timetag time, int8_t i)
{
	int32_t ii = hton32((int32_t)i);
	return osc_native_alloc(r, time, "/o", OSC_TT_I8, 4, 4, (char *)&ii);
}

t_osc_native osc_native_uint8(t_osc_region r, t_osc_timetag time, uint8_t i)
{
	int32_t ii = hton32((int32_t)i);
	return osc_native_alloc(r, time, "/o", OSC_TT_U8, 4, 4, (char *)&ii);
}

t_osc_native osc_native_int16(t_osc_region r, t_osc_timetag time, int16_t i)
{
	int32_t ii = hton32((int32_t)i);
	return osc_native_alloc(r, time, "/o", OSC_TT_I16, 4, 4, (char *)&ii);
}

t_osc_native osc_native_uint16(t_osc_region r, t_osc_timetag time, uint16_t i)
{
	int32_t ii = hton32((int32_t)i);
	return osc_native_alloc(r, time, "/o", OSC_TT_U16, 4, 4, (char *)&ii);
}

t_osc_native osc_native_int32(t_osc_region r, t_osc_timetag time, int32_t i)
{
	int32_t ii = hton32(i);
	return osc_native_alloc(r, time, "/o", OSC_TT_I32, 4, 4, (char *)&ii);
}

t_osc_native osc_native_uint32(t_osc_region r, t_osc_timetag time, uint32_t i)
{
	int32_t ii = hton32(i);
	return osc_native_alloc(r, time, "/o", OSC_TT_U32, 4, 4, (char *)&ii);
}

t_osc_native osc_native_int64(t_osc_region r, t_osc_timetag time, int64_t i)
{
	int64_t ii = hton64(i);
	t_osc_native xx = osc_native_alloc(r, time, "/o", OSC_TT_I64, 8, 8, (char *)&ii);
	return xx;
}

t_osc_native osc_native_uint64(t_osc_region r, t_osc_timetag time, uint64_t i)
{
	int64_t ii = hton64(i);
	return osc_native_alloc(r, time, "/o", OSC_TT_U64, 8, 8, (char *)&ii);
}

t_osc_native osc_native_float(t_osc_region r, t_osc_timetag time, float f)
{
	int32_t ii = hton32(*((int32_t *)&f));
	return osc_native_alloc(r, time, "/o", OSC_TT_F32, 4, 4, (char *)&ii);
}

t_osc_native osc_native_double(t_osc_region r, t_osc_timetag time, double f)
{
	int64_t ii = hton64(*((int64_t *)&f));
	return osc_native_alloc(r, time, "/o", OSC_TT_F64, 8, 8, (char *)&ii);
}

t_osc_native osc_native_string(t_osc_region r, t_osc_timetag time, char *s)
{
	return osc_native_alloc(r, time, "/o", OSC_TT_STR, strlen(s), osc_util_getPaddedStringLen(s), s);
}

t_osc_native osc_native_symbol(t_osc_region r, t_osc_timetag time, char *s)
{
	return osc_native_alloc(r, time, "/o", OSC_TT_SYM, strlen(s), osc_util_getPaddedStringLen(s), s);
}

t_osc_native osc_native_time(t_osc_region r, t_osc_timetag time, t_osc_timetag t)
{
	char buf[OSC_TIMETAG_SIZEOF];
	osc_timetag_encodeForHeader(t, buf);
	return osc_native_alloc(r, time, "/o", OSC_TT_TIME, OSC_TIMETAG_SIZEOF, OSC_TIMETAG_SIZEOF, buf);
}

t_osc_native osc_native_blob(t_osc_region r, t_osc_timetag time, int32_t num_bytes, char *b)
{
	return osc_native_alloc(r, time, "/o", OSC_TT_BLOB, num_bytes, osc_util_getPaddingForNBytes(num_bytes), b);
}

t_osc_native osc_native_unit(t_osc_region r, t_osc_timetag time, char typetag)
{
	return osc_native_alloc(r, time, "/o", typetag, 0, 0, NULL);
}

t_osc_native osc_native_fn(t_osc_region r, t_osc_timetag time, t_osc_fn fn, char *name)
{
	char *address = "/o";
	int addresslen = osc_util_getPaddedStringLen(address);
	int namelen = osc_util_getPaddedStringLen(name);
	int32_t len = OSC_HEADER_SIZE + 4 + addresslen + 4 + sizeof(intptr_t) + namelen;
	char *buf = (char *)osc_region_getBytes(r, 4 + len);
	memset(buf, 0, len + 4);
	char *p = buf;
	*((int32_t *)p) = hton32(len);
	p += 4;
	strncpy(p, OSC_ID, OSC_ID_SIZE);
	p += OSC_ID_SIZE;
	osc_timetag_encodeForHeader(time, p);
	p += OSC_TIMETAG_SIZEOF;
	*((int32_t *)p) = hton32(len - OSC_HEADER_SIZE - 4);
	p += 4;
	strcpy(p, address);
	p += addresslen;
	*p++ = ',';
	*p++ = OSC_TT_FN;
	*p++ = OSC_TT_SYM;
	*p++ = 0;
	*((intptr_t *)p) = (intptr_t)fn;
	p += sizeof(intptr_t);
	memcpy(p, name, strlen(name));
	return (t_osc_native)buf;
}

t_osc_cvalue_int8 osc_native_getInt8(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(int8, (int8_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(int8, 0, 1);
}

t_osc_cvalue_uint8 osc_native_getUInt8(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(uint8, (uint8_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(uint8, 0, 1);
}

t_osc_cvalue_int16 osc_native_getInt16(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(int16, (int16_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(int16, 0, 1);
}

t_osc_cvalue_uint16 osc_native_getUInt16(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(uint16, (uint16_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(uint16, 0, 1);
}

t_osc_cvalue_int32 osc_native_getInt32(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(int32, (int32_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(int32, 0, 1);
}

t_osc_cvalue_uint32 osc_native_getUInt32(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			return osc_cvalue_create(uint32, (uint32_t)ntoh32(*((int32_t *)data)), 0);
		}
	}
	return osc_cvalue_create(uint32, 0, 1);
}

t_osc_cvalue_int64 osc_native_getInt64(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 8){
			return osc_cvalue_create(int64, (int64_t)ntoh64(*((int64_t *)data)), 0);
		}
	}
	return osc_cvalue_create(int64, 0, 1);
}

t_osc_cvalue_uint64 osc_native_getUInt64(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 8){
			return osc_cvalue_create(uint64, (uint64_t)ntoh64(*((int64_t *)data)), 0);
		}
	}
	return osc_cvalue_create(uint64, 0, 1);
}

t_osc_cvalue_float osc_native_getFloat(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 4){
			int32_t i = ntoh32(*((int32_t *)data));
			return osc_cvalue_create(float, *((float *)&i), 0);
		}
	}
	return osc_cvalue_create(float, 0., 1);
}

t_osc_cvalue_double osc_native_getDouble(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 8){
			int64_t i = ntoh64(*((int64_t *)data));
			return osc_cvalue_create(double, *((double *)&i), 0);
		}
	}
	return osc_cvalue_create(double, 0., 1);
}

t_osc_cvalue_ptr osc_native_getPtr(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) > 0){
			return osc_cvalue_create(ptr, osc_native_getData(v), 0);
		}
	}
	return osc_cvalue_create(ptr, NULL, 1);
}

t_osc_cvalue_time osc_native_getTime(t_osc_native v)
{
	if(v){
		char tt = osc_native_getTypetag(v);
		char *data = osc_native_getData(v);
		if(osc_sizeof(tt, data) >= 8){
			return osc_cvalue_create(time, osc_timetag_decodeFromHeader(osc_native_getData(v)), 0);
		}
	}
	return osc_cvalue_create(time, OSC_TIMETAG_NULL, 1);
}

t_osc_cvalue_fn osc_native_getFn(t_osc_native v)
{
	if(v && osc_native_getTypetag(v) == OSC_TT_FN){
		char *data = osc_native_getData(v);
		return osc_cvalue_create(fn, (t_osc_fn)*((intptr_t *)data), 0);
	}
	return osc_cvalue_create(fn, NULL, 1);	
}

t_osc_cvalue_ptr osc_native_getFnName(t_osc_native v)
{
	if(v && osc_native_getTypetag(v) == OSC_TT_FN){
		return osc_cvalue_create(ptr, osc_native_getData(v) + sizeof(intptr_t), 0);
	}
	return osc_cvalue_create(ptr, NULL, 1);	
}

t_osc_native osc_native_convert(t_osc_region r, t_osc_timetag time, t_osc_native n, char newtype)
{
	char tt = osc_native_getTypetag(n);
	switch(tt){
	case OSC_TT_I8:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getInt8(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_int8(NULL, 0, osc_cvalue_value(osc_native_getInt8(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int8(s, len + 1, osc_cvalue_value(osc_native_getInt8(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_int8(NULL, 0, osc_cvalue_value(osc_native_getInt8(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int8(s, len + 1, osc_cvalue_value(osc_native_getInt8(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getInt8(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_U8:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getUInt8(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_uint8(NULL, 0, osc_cvalue_value(osc_native_getUInt8(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint8(s, len + 1, osc_cvalue_value(osc_native_getUInt8(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_uint8(NULL, 0, osc_cvalue_value(osc_native_getUInt8(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint8(s, len + 1, osc_cvalue_value(osc_native_getUInt8(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getUInt8(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_I16:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getInt16(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_int16(NULL, 0, osc_cvalue_value(osc_native_getInt16(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int16(s, len + 1, osc_cvalue_value(osc_native_getInt16(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_int16(NULL, 0, osc_cvalue_value(osc_native_getInt16(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int16(s, len + 1, osc_cvalue_value(osc_native_getInt16(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getInt16(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_U16:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getUInt16(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_uint16(NULL, 0, osc_cvalue_value(osc_native_getUInt16(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint16(s, len + 1, osc_cvalue_value(osc_native_getUInt16(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_uint16(NULL, 0, osc_cvalue_value(osc_native_getUInt16(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint16(s, len + 1, osc_cvalue_value(osc_native_getUInt16(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getUInt16(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_I32:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getInt32(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_int32(NULL, 0, osc_cvalue_value(osc_native_getInt32(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int32(s, len + 1, osc_cvalue_value(osc_native_getInt32(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_int32(NULL, 0, osc_cvalue_value(osc_native_getInt32(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int32(s, len + 1, osc_cvalue_value(osc_native_getInt32(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getInt32(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_U32:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getUInt32(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_uint32(NULL, 0, osc_cvalue_value(osc_native_getUInt32(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint32(s, len + 1, osc_cvalue_value(osc_native_getUInt32(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_uint32(NULL, 0, osc_cvalue_value(osc_native_getUInt32(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint32(s, len + 1, osc_cvalue_value(osc_native_getUInt32(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getUInt32(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_I64:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getInt64(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_int64(NULL, 0, osc_cvalue_value(osc_native_getInt64(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int64(s, len + 1, osc_cvalue_value(osc_native_getInt64(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_int64(NULL, 0, osc_cvalue_value(osc_native_getInt64(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_int64(s, len + 1, osc_cvalue_value(osc_native_getInt64(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getInt64(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_U64:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_cvalue_value(osc_native_getUInt64(n)));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_uint64(NULL, 0, osc_cvalue_value(osc_native_getUInt64(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint64(s, len + 1, osc_cvalue_value(osc_native_getUInt64(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_uint64(NULL, 0, osc_cvalue_value(osc_native_getUInt64(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_uint64(s, len + 1, osc_cvalue_value(osc_native_getUInt64(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_cvalue_value(osc_native_getUInt64(n))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_STR:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_STR:
			return n;
		case OSC_TT_SYM:
			return osc_native_symbol(r, OSC_TIMETAG_NULL, osc_cvalue_value(osc_native_getPtr(n)));
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, OSC_TIMETAG_NULL);
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_SYM:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_STR:
			return osc_native_string(r, OSC_TIMETAG_NULL, osc_cvalue_value(osc_native_getPtr(n)));
		case OSC_TT_SYM:
			return n;
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, OSC_TIMETAG_NULL);
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_TIME:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, (int8_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, (uint8_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, (int16_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, (uint16_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, (int32_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, (uint32_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, (int64_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, (uint64_t)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, (float)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, (double)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n))));
		case OSC_TT_STR:
			{
				size_t len = osc_strfmt_timetag(NULL, 0, osc_cvalue_value(osc_native_getTime(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_timetag(s, len + 1, osc_cvalue_value(osc_native_getTime(n)));
				return osc_native_string(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_SYM:
			{
				size_t len = osc_strfmt_timetag(NULL, 0, osc_cvalue_value(osc_native_getTime(n)));
				char *s = osc_region_getBytes(r, len + 1);
				osc_strfmt_timetag(s, len + 1, osc_cvalue_value(osc_native_getTime(n)));
				return osc_native_symbol(r, OSC_TIMETAG_NULL, s);
			}
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, osc_timetag_floatToTimetag((double)osc_timetag_timetagToFloat(osc_cvalue_value(osc_native_getTime(n)))));
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_BLOB:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_STR:
			return osc_native_string(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_SYM:
			return osc_native_symbol(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, OSC_TIMETAG_NULL);				
		case OSC_TT_BLOB:
			return n;
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_TRUE:
	case OSC_TT_FALSE:
	case OSC_TT_NIL:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_STR:
			return osc_native_string(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_SYM:
			return osc_native_symbol(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, OSC_TIMETAG_NULL);
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	case OSC_TT_BNDL:
		switch(newtype){
		case OSC_TT_I8:
			return osc_native_int8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U8:
			return osc_native_uint8(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I16:
			return osc_native_int16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U16:
			return osc_native_uint16(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I32:
			return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U32:
			return osc_native_uint32(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_I64:
			return osc_native_int64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_U64:
			return osc_native_uint64(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F32:
			return osc_native_float(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_F64:
			return osc_native_double(r, OSC_TIMETAG_NULL, 0);
		case OSC_TT_STR:
			return osc_native_string(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_SYM:
			return osc_native_symbol(r, OSC_TIMETAG_NULL, "");
		case OSC_TT_TIME:
			return osc_native_time(r, OSC_TIMETAG_NULL, OSC_TIMETAG_NULL);
		case OSC_TT_BLOB:
			return osc_native_blob(r, OSC_TIMETAG_NULL, osc_native_getSize(n), (char *)(n + 4));
		case OSC_TT_TRUE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_TRUE);
		case OSC_TT_FALSE:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_FALSE);
		case OSC_TT_NIL:
			return osc_native_unit(r, OSC_TIMETAG_NULL, OSC_TT_NIL);
		case OSC_TT_BNDL:
			return n;
		}
	}
	return n;
}

int osc_native_eql(t_osc_region r, t_osc_native n1, t_osc_native n2)
{
	if(n1 && n2){
		char *_n1 = (char *)n1;
		char *_n2 = (char *)n2;
		int32_t l1 = ntoh32(*((int32_t *)_n1));
		int32_t l2 = ntoh32(*((int32_t *)_n2));
		if(l1 == l2){
			return memcmp(_n1 + 20, _n2 + 20, l1 - 16) == 0;
		}
	}
	return 0;
}

int osc_native_eqv(t_osc_region r, t_osc_native n1, t_osc_native n2)
{
	char *sp = osc_region_getPtr(r);
	char tt1 = osc_native_getTypetag(n1);
	char tt2 = osc_native_getTypetag(n2);
	char lub = osc_typetag_findLUB(tt1, tt2);
	t_osc_native _n1, _n2;
	if(tt1 == tt2){
		_n1 = n1;
		_n2 = n2;
	}else if(tt1 != lub && tt2 == lub){
		_n1 = osc_native_convert(r, OSC_TIMETAG_NULL, n1, lub);
		_n2 = n2;
	}else if(tt1 == lub && tt2 != lub){
		_n1 = n1;
		_n2 = osc_native_convert(r, OSC_TIMETAG_NULL, n2, lub);
	}else{
		_n1 = osc_native_convert(r, OSC_TIMETAG_NULL, n1, lub);
		_n2 = osc_native_convert(r, OSC_TIMETAG_NULL, n2, lub);
	}
	int res = osc_native_eql(r, _n1, _n2);
	osc_region_unwind(r, sp);
	return res;
}

t_osc_native osc_native_add(t_osc_region r, t_osc_native lhs, t_osc_native rhs)
{
	char tt = osc_native_getTypetag(lhs);
	if(OSC_TYPETAG_ISNUMERIC(tt)){
		if(OSC_TYPETAG_ISINT32(tt)){
			t_osc_cvalue_int32 ll = osc_native_getInt32(lhs);
			t_osc_cvalue_int32 rr = osc_native_getInt32(rhs);
			if(!osc_cvalue_error(ll) && !osc_cvalue_error(rr)){
				return osc_native_int32(r, OSC_TIMETAG_NULL, osc_cvalue_value(ll) + osc_cvalue_value(rr));
			}
		}else if(OSC_TYPETAG_ISINT64(tt)){
			t_osc_cvalue_int64 ll = osc_native_getInt64(lhs);
			t_osc_cvalue_int64 rr = osc_native_getInt64(rhs);
			if(!osc_cvalue_error(ll) && !osc_cvalue_error(rr)){
				return osc_native_int64(r, OSC_TIMETAG_NULL, osc_cvalue_value(ll) + osc_cvalue_value(rr));
			}
		}else if(tt == OSC_TT_F32){
			t_osc_cvalue_float ll = osc_native_getFloat(lhs);
			t_osc_cvalue_float rr = osc_native_getFloat(rhs);
			if(!osc_cvalue_error(ll) && !osc_cvalue_error(rr)){
				return osc_native_float(r, OSC_TIMETAG_NULL, osc_cvalue_value(ll) + osc_cvalue_value(rr));
			}
		}else if(tt == OSC_TT_F64){
			t_osc_cvalue_double ll = osc_native_getDouble(lhs);
			t_osc_cvalue_double rr = osc_native_getDouble(rhs);
			if(!osc_cvalue_error(ll) && !osc_cvalue_error(rr)){
				return osc_native_double(r, OSC_TIMETAG_NULL, osc_cvalue_value(ll) + osc_cvalue_value(rr));
			}
		}
	}
	return osc_native_int32(r, OSC_TIMETAG_NULL, 0);
}
