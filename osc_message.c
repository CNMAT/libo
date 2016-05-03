#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "osc.h"
#include "osc_message.h"
#include "osc_region.h"
#include "osc_typetag.h"
#include "osc_capi_primitive.h"

t_osc_bndl osc_msg_nth(t_osc_region r, t_osc_msg m, t_osc_bndl n)
{
	if(m && n && osc_capi_primitive_q(r, n)){
		if(OSC_TYPETAG_ISINT32(osc_capi_primitive_getType(r, n))){
			t_osc_cvalue_int32 i = osc_capi_primitive_getInt32(r, n);
			if(!osc_cvalue_error(i)){
				return osc_capi_msg_nth(r, m, osc_cvalue_value(i));
			}
		}else if(OSC_TYPETAG_ISINT64(osc_capi_primitive_getType(r, n))){
			t_osc_cvalue_int64 i = osc_capi_primitive_getInt64(r, n);
			if(!osc_cvalue_error(i)){
				return osc_capi_msg_nth(r, m, osc_cvalue_value(i));
			}
		}
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}
