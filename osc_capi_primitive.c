#include <string.h>
#include "osc.h"
#include "osc_capi_primitive.h"
#include "osc_capi.h"
#include "osc_typetag.h"
#include "osc_match.h"
#include "osc_byteorder.h"

t_osc_bndl osc_capi_primitive_alloc(t_osc_region r, t_osc_native value)
{
	return (t_osc_bndl)osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_NATIVE, (void *)value))));
}

int osc_capi_primitive_q(t_osc_region r, t_osc_bndl b)
{
	if(b && osc_capi_bndl_getMsgCount(r, b) > 0){
		t_osc_listitem *li = osc_list_nth((t_osc_list)b, 0);
		if(osc_list_itemGetType(li) == OSC_CAPI_TYPE_LIST &&
		   osc_list_length((t_osc_list)osc_list_itemGetPtr(li)) > 0){
			t_osc_listitem *v = osc_list_nth((t_osc_list)osc_list_itemGetPtr(li), 0);
			if(osc_list_itemGetType(v) == OSC_CAPI_TYPE_NATIVE){
				return 1;
			}
		}		
	}
	return 0;
}

size_t osc_capi_primitive_nformat(t_osc_region r, char *s, size_t n, t_osc_bndl b)
{
	if(b && osc_capi_bndl_getMsgCount(r, b) > 0){
		t_osc_listitem *li = osc_list_nth((t_osc_list)b, 0);
		if(osc_list_itemGetType(li) == OSC_CAPI_TYPE_LIST &&
		   osc_list_length((t_osc_list)osc_list_itemGetPtr(li)) > 0){
			t_osc_listitem *v = osc_list_nth((t_osc_list)osc_list_itemGetPtr(li), 0);
			if(osc_list_itemGetType(v) == OSC_CAPI_TYPE_NATIVE){
				return osc_native_nformat(r, s, n, (t_osc_native)osc_list_itemGetPtr(v));
			}
		}		
	}
	return 0;
}

t_osc_native osc_capi_primitive_getNative(t_osc_bndl b)
{
	if(b){
		t_osc_listitem *lim = osc_list_nth((t_osc_list)b, 0);
		if(lim){
			if(osc_list_itemGetType(lim) == OSC_CAPI_TYPE_LIST){
				t_osc_listitem *lin = osc_list_nth((t_osc_list)osc_list_itemGetPtr(lim), 0);
				if(osc_list_itemGetType(lin) == OSC_CAPI_TYPE_NATIVE){
					return (t_osc_native)osc_list_itemGetPtr(lin);
				}
			}
		}
	}
	return NULL;
}

char osc_capi_primitive_getType(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v)){
		t_osc_native n = osc_capi_primitive_getNative(v);
		return osc_native_getTypetag(n);
	}else{
		return OSC_TT_BNDL;
	}
}

t_osc_bndl osc_capi_primitive_int8(t_osc_region r, t_osc_timetag time, int8_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_int8(r, time, i));
}

t_osc_bndl osc_capi_primitive_uint8(t_osc_region r, t_osc_timetag time, uint8_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_uint8(r, time, i));
}

t_osc_bndl osc_capi_primitive_int16(t_osc_region r, t_osc_timetag time, int16_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_int16(r, time, i));
}

t_osc_bndl osc_capi_primitive_uint16(t_osc_region r, t_osc_timetag time, uint16_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_uint16(r, time, i));
}

t_osc_bndl osc_capi_primitive_int32(t_osc_region r, t_osc_timetag time, int32_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_int32(r, time, i));
}

t_osc_bndl osc_capi_primitive_uint32(t_osc_region r, t_osc_timetag time, uint32_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_uint32(r, time, i));
}

t_osc_bndl osc_capi_primitive_int64(t_osc_region r, t_osc_timetag time, int64_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_int64(r, time, i));
}

t_osc_bndl osc_capi_primitive_uint64(t_osc_region r, t_osc_timetag time, uint64_t i)
{
	return osc_capi_primitive_alloc(r, osc_native_uint64(r, time, i));
}

t_osc_bndl osc_capi_primitive_float(t_osc_region r, t_osc_timetag time, float f)
{
	return osc_capi_primitive_alloc(r, osc_native_float(r, time, f));
}

t_osc_bndl osc_capi_primitive_double(t_osc_region r, t_osc_timetag time, double f)
{
	return osc_capi_primitive_alloc(r, osc_native_double(r, time, f));
}

t_osc_bndl osc_capi_primitive_string(t_osc_region r, t_osc_timetag time, char *s)
{
	return osc_capi_primitive_alloc(r, osc_native_string(r, time, s));
}

t_osc_bndl osc_capi_primitive_symbol(t_osc_region r, t_osc_timetag time, char *s)
{
	return osc_capi_primitive_alloc(r, osc_native_symbol(r, time, s));
}

t_osc_bndl osc_capi_primitive_time(t_osc_region r, t_osc_timetag time, t_osc_timetag t)
{
	return osc_capi_primitive_alloc(r, osc_native_time(r, time, t));
}

t_osc_bndl osc_capi_primitive_blob(t_osc_region r, t_osc_timetag time, int32_t num_bytes, char *b)
{
	return osc_capi_primitive_alloc(r, osc_native_blob(r, time, num_bytes, b));
}

t_osc_bndl osc_capi_primitive_unit(t_osc_region r, t_osc_timetag time, char typetag)
{
	return osc_capi_primitive_alloc(r, osc_native_unit(r, time, typetag));
}

t_osc_bndl osc_capi_primitive_true(t_osc_region r, t_osc_timetag time)
{
	return osc_capi_primitive_unit(r, time, OSC_TT_TRUE);
}

t_osc_bndl osc_capi_primitive_false(t_osc_region r, t_osc_timetag time)
{
	return osc_capi_primitive_unit(r, time, OSC_TT_FALSE);
}

t_osc_bndl osc_capi_primitive_nil(t_osc_region r, t_osc_timetag time)
{
	return osc_capi_primitive_unit(r, time, OSC_TT_NIL);
}

t_osc_bndl osc_capi_primitive_fn(t_osc_region r, t_osc_timetag time, t_osc_fn fn, char *name)
{
	return osc_capi_primitive_alloc(r, osc_native_fn(r, time, fn, name));
}

t_osc_cvalue_int8 osc_capi_primitive_getInt8(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_I8){
		return osc_native_getInt8(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(int8, 0, 1);
}

t_osc_cvalue_uint8 osc_capi_primitive_getUInt8(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_U8){
		return osc_native_getUInt8(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(uint8, 0, 1);
}

t_osc_cvalue_int16 osc_capi_primitive_getInt16(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_I16){
		return osc_native_getInt16(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(int16, 0, 1);
}

t_osc_cvalue_uint16 osc_capi_primitive_getUInt16(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_U16){
		return osc_native_getUInt16(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(uint16, 0, 1);
}

t_osc_cvalue_int32 osc_capi_primitive_getInt32(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_I32){
		return osc_native_getInt32(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(int32, 0, 1);
}

t_osc_cvalue_uint32 osc_capi_primitive_getUInt32(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_U32){
		return osc_native_getUInt32(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(uint32, 0, 1);
}

t_osc_cvalue_int64 osc_capi_primitive_getInt64(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_I64){
		return osc_native_getInt64(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(int64, 0, 1);
}

t_osc_cvalue_uint64 osc_capi_primitive_getUInt64(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_U64){
		return osc_native_getUInt64(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(uint64, 0, 1);
}

t_osc_cvalue_float osc_capi_primitive_getFloat(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_F32){
		return osc_native_getFloat(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(float, 0, 1);
}

t_osc_cvalue_double osc_capi_primitive_getDouble(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_F64){
		return osc_native_getDouble(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(double, 0, 1);
}

t_osc_cvalue_ptr osc_capi_primitive_getPtr(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v)){
		if(osc_capi_primitive_getType(r, v) == OSC_TT_STR ||
	    	   osc_capi_primitive_getType(r, v) == OSC_TT_SYM ||
		   osc_capi_primitive_getType(r, v) == OSC_TT_BLOB){
			return osc_native_getPtr(osc_capi_primitive_getNative(v));
		}
	}
	return osc_cvalue_create(ptr, 0, 1);
}

t_osc_cvalue_fn osc_capi_primitive_getFn(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_FN){
		return osc_native_getFn(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(fn, 0, 1);
}

t_osc_cvalue_ptr osc_capi_primitive_getFnName(t_osc_region r, t_osc_bndl v)
{
	if(osc_capi_primitive_q(r, v) && osc_capi_primitive_getType(r, v) == OSC_TT_FN){
		return osc_native_getFnName(osc_capi_primitive_getNative(v));
	}
	return osc_cvalue_create(ptr, 0, 1);
}

t_osc_bndl osc_capi_primitive_convert(t_osc_region r, t_osc_bndl b, char newtype)
{
	if(b && osc_capi_primitive_q(r, b)){
		return osc_capi_primitive_alloc(r, osc_native_convert(r, OSC_TIMETAG_NULL, osc_capi_primitive_getNative(b), newtype));
	}
	return b;
}

int osc_capi_primitive_isIndexable(t_osc_region r, t_osc_bndl b)
{
	if(osc_capi_primitive_q(r, b)){
		return -1;
	}
	switch(osc_capi_primitive_getType(r, b)){
	case OSC_TT_SYM:
	case OSC_TT_STR:
	case OSC_TT_BLOB:
		return 1;
	default: return 0;
	}
}

char osc_capi_primitive_nth(t_osc_region r, t_osc_bndl b, int i)
{
	if(osc_capi_primitive_isIndexable(r, b)){
		switch(osc_capi_primitive_getType(r, b)){
		case OSC_TT_SYM:
		case OSC_TT_STR:
			{
				t_osc_cvalue_ptr cv = osc_capi_primitive_getPtr(r, b);
				if(!osc_cvalue_error(cv)){
					char *ptr = osc_cvalue_value(cv);
					if(i < strlen(ptr)){
						return ptr[i];
					}
				}
				return 0;
			}
		case OSC_TT_BLOB:
			{
				t_osc_cvalue_ptr cv = osc_capi_primitive_getPtr(r, b);
				if(!osc_cvalue_error(cv)){
					char *ptr = osc_cvalue_value(cv);
					int32_t len = ntoh32(*((int32_t *)ptr));
					if(i < len){
						return ptr[i + 4];
					}
				}
				return 0;
			}
		}
	}
	return 0;
}

int osc_capi_primitive_length(t_osc_region r, t_osc_bndl b)
{
	if(!osc_capi_primitive_q(r, b)){
		return 0;
	}
	switch(osc_capi_primitive_getType(r, b)){
	case OSC_TT_STR:
	case OSC_TT_SYM:
		{
			t_osc_cvalue_ptr cv = osc_capi_primitive_getPtr(r, b);
			if(!osc_cvalue_error(cv)){
				return strlen(osc_cvalue_value(cv));
			}
			return 0;
		}
	case OSC_TT_BLOB:
		{
			t_osc_cvalue_ptr cv = osc_capi_primitive_getPtr(r, b);
			if(!osc_cvalue_error(cv)){
				char *ptr = osc_cvalue_value(cv);
				int32_t len = ntoh32(*((int32_t *)ptr));
				return len;
			}
			return 0;
		}
	}
	return 0;
}

int osc_capi_primitive_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	return osc_native_eql(r, osc_capi_primitive_getNative(b1), osc_capi_primitive_getNative(b2));
}

int osc_capi_primitive_eqv(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	return osc_native_eqv(r, osc_capi_primitive_getNative(b1), osc_capi_primitive_getNative(b2));
}

t_osc_cvalue_int32 osc_capi_primitive_match(t_osc_region r,
					    t_osc_bndl pattern,
					    t_osc_bndl address,
					    int *pattern_offset,
					    int *address_offset)
{
	if(pattern && address && osc_capi_primitive_q(r, pattern) && osc_capi_primitive_q(r, address)){
		char pt = osc_capi_primitive_getType(r, pattern);
		char at = osc_capi_primitive_getType(r, address);
		if((pt == OSC_TT_STR || pt == OSC_TT_SYM) &&
		   (at == OSC_TT_STR || at == OSC_TT_SYM)){
			return osc_cvalue_create(int32,
						 osc_match(osc_cvalue_value(osc_capi_primitive_getPtr(r, pattern)),
							   osc_cvalue_value(osc_capi_primitive_getPtr(r, address)),
							   pattern_offset,
							   address_offset),
						 0);
		}
	}
	*pattern_offset = 0;
	*address_offset = 0;
	return osc_cvalue_create(int32, 0, 1);
}

t_osc_cvalue_int32 osc_capi_primitive_strcmp(t_osc_region r, t_osc_bndl s1, t_osc_bndl s2)
{
	if(!s1 && !s2){
		return osc_cvalue_create(int32, 0, 1);
	}
	if(s1 && !s2){
		return osc_cvalue_create(int32, 1, 1);
	}
	if(!s1 && s2){
		return osc_cvalue_create(int32, -1, 1);
	}
	char tt1 = osc_capi_primitive_getType(r, s1);
	char tt2 = osc_capi_primitive_getType(r, s2);
	if(!(tt1 == OSC_TT_STR || tt1 == OSC_TT_SYM) && !(tt2 == OSC_TT_STR || tt2 == OSC_TT_SYM)){
		return osc_cvalue_create(int32, 0, 1);
	}
	if((tt1 == OSC_TT_STR || tt1 == OSC_TT_SYM) && !(tt2 == OSC_TT_STR || tt2 == OSC_TT_SYM)){
		return osc_cvalue_create(int32, 1, 1);
	}
	if(!(tt1 == OSC_TT_STR || tt1 == OSC_TT_SYM) && (tt2 == OSC_TT_STR || tt2 == OSC_TT_SYM)){
		return osc_cvalue_create(int32, -1, 1);
	}
	return osc_cvalue_create(int32, strcmp(osc_cvalue_value(osc_capi_primitive_getPtr(r, s1)), osc_cvalue_value(osc_capi_primitive_getPtr(r, s2))), 0);
}
