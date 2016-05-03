#ifndef __OSC_CAPI_PRIMITIVE_H__
#define __OSC_CAPI_PRIMITIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle.h"
#include "osc_native.h"
#include "osc_region.h"
#include "osc_cvalue.h"

t_osc_bndl osc_capi_primitive_alloc(t_osc_region r, t_osc_native value);
int osc_capi_primitive_q(t_osc_region r, t_osc_bndl b);
t_osc_native osc_capi_primitive_getNative(t_osc_bndl b);
char osc_capi_primitive_getType(t_osc_region r, t_osc_bndl v);
size_t osc_capi_primitive_nformat(t_osc_region r, char *s, size_t n, t_osc_bndl b);
t_osc_bndl osc_capi_primitive_int8(t_osc_region r, t_osc_timetag time, int8_t i);
t_osc_bndl osc_capi_primitive_uint8(t_osc_region r, t_osc_timetag time, uint8_t i);
t_osc_bndl osc_capi_primitive_int16(t_osc_region r, t_osc_timetag time, int16_t i);
t_osc_bndl osc_capi_primitive_uint16(t_osc_region r, t_osc_timetag time, uint16_t i);
t_osc_bndl osc_capi_primitive_int32(t_osc_region r, t_osc_timetag time, int32_t i);
t_osc_bndl osc_capi_primitive_uint32(t_osc_region r, t_osc_timetag time, uint32_t i);
t_osc_bndl osc_capi_primitive_int64(t_osc_region r, t_osc_timetag time, int64_t i);
t_osc_bndl osc_capi_primitive_uint64(t_osc_region r, t_osc_timetag time, uint64_t i);
t_osc_bndl osc_capi_primitive_float(t_osc_region r, t_osc_timetag time, float f);
t_osc_bndl osc_capi_primitive_double(t_osc_region r, t_osc_timetag time, double f);
t_osc_bndl osc_capi_primitive_string(t_osc_region r, t_osc_timetag time, char *s);
t_osc_bndl osc_capi_primitive_symbol(t_osc_region r, t_osc_timetag time, char *s);
t_osc_bndl osc_capi_primitive_time(t_osc_region r, t_osc_timetag time, t_osc_timetag t);
t_osc_bndl osc_capi_primitive_blob(t_osc_region r, t_osc_timetag time, int32_t num_bytes, char *b);
t_osc_bndl osc_capi_primitive_unit(t_osc_region r, t_osc_timetag time, char typetag);
t_osc_bndl osc_capi_primitive_true(t_osc_region r, t_osc_timetag time);
t_osc_bndl osc_capi_primitive_false(t_osc_region r, t_osc_timetag time);
t_osc_bndl osc_capi_primitive_nil(t_osc_region r, t_osc_timetag time);
t_osc_bndl osc_capi_primitive_fn(t_osc_region r, t_osc_timetag time, t_osc_fn fn, char *name);

t_osc_cvalue_int8 osc_capi_primitive_getInt8(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_uint8 osc_capi_primitive_getUInt8(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_int16 osc_capi_primitive_getInt16(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_uint16 osc_capi_primitive_getUInt16(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_int32 osc_capi_primitive_getInt32(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_uint32 osc_capi_primitive_getUInt32(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_int64 osc_capi_primitive_getInt64(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_uint64 osc_capi_primitive_getUInt64(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_float osc_capi_primitive_getFloat(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_double osc_capi_primitive_getDouble(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_ptr osc_capi_primitive_getPtr(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_fn osc_capi_primitive_getFn(t_osc_region r, t_osc_bndl v);
t_osc_cvalue_ptr osc_capi_primitive_getFnName(t_osc_region r, t_osc_bndl v);

t_osc_bndl osc_capi_primitive_convert(t_osc_region r, t_osc_bndl b, char newtype);

int osc_capi_primitive_isIndexable(t_osc_region r, t_osc_bndl b);
char osc_capi_primitive_nth(t_osc_region r, t_osc_bndl b, int i);
int osc_capi_primitive_length(t_osc_region r, t_osc_bndl b);
int osc_capi_primitive_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
int osc_capi_primitive_eqv(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_cvalue_int32 osc_capi_primitive_match(t_osc_region r,
					    t_osc_bndl pattern,
					    t_osc_bndl address,
					    int *pattern_offset,
					    int *address_offset);
t_osc_cvalue_int32 osc_capi_primitive_strcmp(t_osc_region r, t_osc_bndl s1, t_osc_bndl s2);

#ifdef __cplusplus
}
#endif
	
#endif
