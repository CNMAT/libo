#include <stdarg.h>
#include <stdio.h>
#include "osc_capi.h"
#include "osc_capi_bundle.h"
#include "osc_capi_message.h"
#include "osc_list.h"
#include "osc_native.h"
#include "osc_primitive.h"

t_osc_msg osc_capi_msg_alloc(t_osc_region r, int n, ...)
{
	t_osc_list l = osc_list_alloc(r, NULL, 0);//osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)osc_primitive_symbol(r, OSC_TIMETAG_NULL, address)));
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		t_osc_bndl b = va_arg(ap, t_osc_bndl);
		osc_list_append_m(r, (t_osc_list_m)l, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)b));
	}
	va_end(ap);
	return (t_osc_msg)l;
}

t_osc_msg osc_capi_msg_allocWithList(t_osc_region r, t_osc_bndl address, t_osc_list l)
{
	return (t_osc_msg)osc_list_prepend_m(r, (t_osc_list_m)l, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)address));
}

int osc_capi_msg_length(t_osc_msg m)
{
	if(m){
		return osc_list_length((t_osc_list)m);
	}else{
		return 0;
	}
}

t_osc_bndl osc_capi_msg_nth(t_osc_region r, t_osc_msg m, int n)
{
	if(m && n >= 0){
		t_osc_listitem *li = (t_osc_listitem *)osc_list_nth((t_osc_list)m, n);
		if(li){
			switch(osc_list_itemGetType(li)){
			case OSC_CAPI_TYPE_LIST:
				return (t_osc_bndl)osc_list_itemGetPtr(li);
			case OSC_CAPI_TYPE_NATIVE:
				return NULL;
			}
		}
	}
	return NULL;
}

t_osc_msg osc_capi_msg_append(t_osc_region r, t_osc_msg m, t_osc_bndl e)
{
	if(e){
		if(m){
			return osc_list_append(r, m, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)e));
		}else{
			return (t_osc_msg)osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)e));
		}
	}
	return m;
}

t_osc_msg osc_capi_msg_prepend(t_osc_region r, t_osc_msg m, t_osc_bndl e)
{
	if(e){
		if(m){
			return osc_list_prepend(r, m, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)e));
		}else{
			return (t_osc_msg)osc_list_alloc(r, NULL, 1, osc_list_allocItem(r, OSC_CAPI_TYPE_LIST, (void *)e));
		}
	}
	return m;
}

void _osc_capi_msg_print(t_osc_region r, t_osc_msg m, char eol)
{
	if(m){
		osc_capi_bndl_print(r, osc_capi_msg_nth(r, m, 0));
		printf(" : ");
		for(int i = 1; i < osc_capi_msg_length(m); i++){
			osc_capi_bndl_print(r, osc_capi_msg_nth(r, m, i));
		}
		printf("%c", eol);
	}else{
		printf("message = NULL!\n");
	}
}

void osc_capi_msg_print(t_osc_region r, t_osc_msg m)
{
	_osc_capi_msg_print(r, m, ' ');
}

void osc_capi_msg_println(t_osc_region r, t_osc_msg m)
{
	_osc_capi_msg_print(r, m, '\n');
}
