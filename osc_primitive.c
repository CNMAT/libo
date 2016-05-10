#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include "osc.h"
#include "osc_typetag.h"
#include "osc_primitive.h"
#include "osc_byteorder.h"
#include "osc_native.h"
#include "osc_capi.h"
#include "osc_capi_primitive.h"
#include "osc_match.h"

t_osc_bndl osc_primitive_copy(t_osc_region r, t_osc_bndl b)
{
	return osc_capi_primitive_alloc(r, osc_native_copy(r, osc_capi_primitive_getNative(b)));
}

t_osc_bndl osc_primitive_isIndexable(t_osc_region r, t_osc_bndl b)
{
	int ret = osc_capi_primitive_isIndexable(r, b);
	if(ret == 1){
		return osc_capi_primitive_true(r, OSC_TIMETAG_NULL);
	}else if(ret == 0){
		return osc_capi_primitive_false(r, OSC_TIMETAG_NULL);
	}else{
		return osc_capi_primitive_nil(r, OSC_TIMETAG_NULL);
	}
}

t_osc_bndl osc_primitive_nth(t_osc_region r, t_osc_bndl b, t_osc_bndl n)
{
	t_osc_cvalue_int32 i = osc_capi_primitive_getInt32(r, b);
	if(osc_cvalue_error(i)){
		return osc_capi_primitive_int8(r, OSC_TIMETAG_NULL, 0);
	}
	return osc_capi_primitive_int8(r, OSC_TIMETAG_NULL, osc_capi_primitive_nth(r, b, osc_cvalue_value(i)));
}

t_osc_bndl osc_primitive_length(t_osc_region r, t_osc_bndl b)
{
	return osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, osc_capi_primitive_length(r, b));
}

t_osc_bndl osc_primitive_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	return osc_capi_primitive_unit(r, OSC_TIMETAG_NULL, osc_capi_primitive_eql(r, b1, b2) == 0 ? OSC_TT_FALSE : OSC_TT_TRUE);
}

t_osc_bndl osc_primitive_eqv(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2)
{
	return osc_capi_primitive_unit(r, OSC_TIMETAG_NULL, osc_capi_primitive_eqv(r, b1, b2) == 0 ? OSC_TT_FALSE : OSC_TT_TRUE);
}

t_osc_bndl osc_primitive_match(t_osc_region r, t_osc_bndl pattern, t_osc_bndl address)
{
	int po = 0, ao = 0;
	t_osc_cvalue_int32 res = osc_capi_primitive_match(r, pattern, address, &po, &ao);
	t_osc_bndl pc = NULL, ac = NULL;
	if(osc_cvalue_error(res)){
		pc = osc_capi_primitive_false(r, OSC_TIMETAG_NULL);
		ac = osc_capi_primitive_true(r, OSC_TIMETAG_NULL);
	}else{
		int32_t resi = osc_cvalue_value(res);
		if(resi & OSC_MATCH_PATTERN_COMPLETE){
			pc = osc_capi_primitive_true(r, OSC_TIMETAG_NULL);
		}else{
			pc = osc_capi_primitive_false(r, OSC_TIMETAG_NULL);
		}
		if(resi & OSC_MATCH_ADDRESS_COMPLETE){
			ac = osc_capi_primitive_true(r, OSC_TIMETAG_NULL);
		}else{
			ac = osc_capi_primitive_false(r, OSC_TIMETAG_NULL);
		}
	}
	return osc_capi_bndl_alloc(r,
				   OSC_TIMETAG_NULL,
				   6,
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/pattern"), pattern),
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/address"), address),
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/complete/pattern"), pc),
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/complete/address"), ac),
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/offset/pattern"), po),
				   osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/offset/address"), ao));
}

t_osc_bndl osc_primitive_add(t_osc_region r, t_osc_bndl lhs, t_osc_bndl rhs)
{
	t_osc_native nlhs = osc_capi_primitive_getNative(lhs);
	t_osc_native nrhs = osc_capi_primitive_getNative(rhs);
	return osc_capi_primitive_alloc(r, osc_native_add(r, nlhs, nrhs));
}

