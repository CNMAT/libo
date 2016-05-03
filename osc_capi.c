#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include "osc.h"
#include "osc_util.h"
#include "osc_capi.h"
#include "osc_native.h"
#include "osc_byteorder.h"
#include "osc_primitive.h"


/*
void osc_capi_freeListItem(void *p)
{
	if(p){
		t_osc_listitem *li = (t_osc_listitem *)p;
		switch(osc_list_itemGetType(li)){
		case OSC_CAPI_TYPE_NATIVE:
			osc_native_free((t_osc_native)osc_list_itemGetPtr(li));
			break;
		case OSC_CAPI_TYPE_LIST:
			osc_list_release((t_osc_list)osc_list_itemGetPtr(li));
			break;
		}
		osc_list_freeItem(li);
	}
}
*/
