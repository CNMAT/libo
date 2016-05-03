#ifndef __OSC_CVALUE_H__
#define __OSC_CVALUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "osc.h"
#include "osc_timetag.h"

#pragma pack(push)
#pragma pack(4)
typedef struct _osc_cvalue_int8
{
	uint32_t error;
	int8_t val;
} t_osc_cvalue_int8;

typedef struct _osc_cvalue_uint8
{
	uint32_t error;
	uint8_t val;
} t_osc_cvalue_uint8;

typedef struct _osc_cvalue_int16
{
	uint32_t error;
	int16_t val;
} t_osc_cvalue_int16;

typedef struct _osc_cvalue_uint16
{
	uint32_t error;
	uint16_t val;
} t_osc_cvalue_uint16;

typedef struct _osc_cvalue_int32
{
	uint32_t error;
	int32_t val;
} t_osc_cvalue_int32;

typedef struct _osc_cvalue_uint32
{
	uint32_t error;
	uint32_t val;
} t_osc_cvalue_uint32;

typedef struct _osc_cvalue_int64
{
	uint32_t error;
	int64_t val;
} t_osc_cvalue_int64;

typedef struct _osc_cvalue_uint64
{
	uint32_t error;
	uint64_t val;
} t_osc_cvalue_uint64;

typedef struct _osc_cvalue_float
{
	uint32_t error;
	float val;
} t_osc_cvalue_float;

typedef struct _osc_cvalue_double
{
	uint32_t error;
	double val;
} t_osc_cvalue_double;

typedef struct _osc_cvalue_ptr
{
	uint32_t error;
	char *val;
} t_osc_cvalue_ptr;

typedef struct _osc_cvalue_time
{
	uint32_t error;
	t_osc_timetag val;
} t_osc_cvalue_time;

typedef struct _osc_cvalue_fn
{
	uint32_t error;
	t_osc_fn val;
} t_osc_cvalue_fn;

#pragma pack(pop)

#define osc_cvalue_create(type, val, error) (t_osc_cvalue_##type){error, val}
#define osc_cvalue_error(v) (v.error)
#define osc_cvalue_value(v) (v.val)



#ifdef __cplusplus
}
#endif

#endif
