#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "osc.h"
#include "osc_message.h"
#include "osc_region.h"
#include "osc_typetag.h"
#include "osc_capi_primitive.h"

t_osc_msg osc_msg_create(t_osc_region r)
{
	return osc_capi_msg_alloc(r, 0);
}

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

t_osc_msg osc_msg_rest(t_osc_region r, t_osc_msg m)
{
	if(m){
		//return (t_osc_msg)osc_list_popFirst(r, (t_osc_list)m);
		int n = osc_capi_msg_length(r, m);
		t_osc_msg mm = osc_msg_create(r);
		for(int i = 1; i < n; i++){
			mm = (t_osc_msg)osc_capi_msg_append_m(r, (t_osc_msg_m)mm, osc_capi_msg_nth(r, m, i));
		}
		return mm;
	}
	return osc_msg_create(r);
}
