#ifndef __OSC_NATIVE_H__
#define __OSC_NATIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <inttypes.h>
#include "osc_timetag.h"
#include "osc_region.h"
#include "osc_cvalue.h"

typedef char* t_osc_native;

t_osc_native osc_native_alloc(t_osc_region r, t_osc_timetag time, char *address, char typetag, int32_t num_bytes, int32_t num_padded_bytes, char *bytes);
//void osc_native_free(t_osc_native n);
t_osc_timetag osc_native_getTimetag(t_osc_native n);
int32_t osc_native_getSize(t_osc_native n);
char osc_native_getTypetag(t_osc_native n);
char *osc_native_getData(t_osc_native n);
size_t osc_native_nformat(t_osc_region r, char *s, size_t n, t_osc_native x);

t_osc_native osc_native_int8(t_osc_region r, t_osc_timetag time, int8_t i);
t_osc_native osc_native_uint8(t_osc_region r, t_osc_timetag time, uint8_t i);
t_osc_native osc_native_int16(t_osc_region r, t_osc_timetag time, int16_t i);
t_osc_native osc_native_uint16(t_osc_region r, t_osc_timetag time, uint16_t i);
t_osc_native osc_native_int32(t_osc_region r, t_osc_timetag time, int32_t i);
t_osc_native osc_native_uint32(t_osc_region r, t_osc_timetag time, uint32_t i);
t_osc_native osc_native_int64(t_osc_region r, t_osc_timetag time, int64_t i);
t_osc_native osc_native_uint64(t_osc_region r, t_osc_timetag time, uint64_t i);
t_osc_native osc_native_float(t_osc_region r, t_osc_timetag time, float f);
t_osc_native osc_native_double(t_osc_region r, t_osc_timetag time, double f);
t_osc_native osc_native_string(t_osc_region r, t_osc_timetag time, char *s);
t_osc_native osc_native_symbol(t_osc_region r, t_osc_timetag time, char *s);
t_osc_native osc_native_time(t_osc_region r, t_osc_timetag time, t_osc_timetag t);
t_osc_native osc_native_blob(t_osc_region r, t_osc_timetag time, int32_t num_bytes, char *b);
t_osc_native osc_native_unit(t_osc_region r, t_osc_timetag time, char typetag);
t_osc_native osc_native_fn(t_osc_region r, t_osc_timetag time, t_osc_fn fn, char *name);

t_osc_cvalue_int8 osc_native_getInt8(t_osc_native v);
t_osc_cvalue_uint8 osc_native_getUInt8(t_osc_native v);
t_osc_cvalue_int16 osc_native_getInt16(t_osc_native v);
t_osc_cvalue_uint16 osc_native_getUInt16(t_osc_native v);
t_osc_cvalue_int32 osc_native_getInt32(t_osc_native v);
t_osc_cvalue_uint32 osc_native_getUInt32(t_osc_native v);
t_osc_cvalue_int64 osc_native_getInt64(t_osc_native v);
t_osc_cvalue_uint64 osc_native_getUInt64(t_osc_native v);
t_osc_cvalue_float osc_native_getFloat(t_osc_native v);
t_osc_cvalue_double osc_native_getDouble(t_osc_native v);
t_osc_cvalue_ptr osc_native_getPtr(t_osc_native v);
t_osc_cvalue_time osc_native_getTime(t_osc_native v);
t_osc_cvalue_fn osc_native_getFn(t_osc_native v);
t_osc_cvalue_ptr osc_native_getFnName(t_osc_native v);

t_osc_native osc_native_convert(t_osc_region r, t_osc_timetag time, t_osc_native n, char newtype);

int osc_native_eql(t_osc_region r, t_osc_native n1, t_osc_native n2);
int osc_native_eqv(t_osc_region r, t_osc_native n1, t_osc_native n2);
t_osc_native osc_native_add(t_osc_region r, t_osc_native lhs, t_osc_native rhs);

#ifdef __cplusplus
}
#endif

#endif
