#ifndef __OSC_CAPI_MESSAGE_H__
#define __OSC_CAPI_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_list.h"
#include "osc_capi_bundle.h"
#include "osc_region.h"

typedef t_osc_list t_osc_msg;
typedef t_osc_list_m t_osc_msg_m;

t_osc_msg osc_capi_msg_alloc(t_osc_region r, int n, ...);
t_osc_msg osc_capi_msg_allocWithList(t_osc_region r, t_osc_bndl address, t_osc_list l);
int osc_capi_msg_length(t_osc_region r, t_osc_msg m);
t_osc_bndl osc_capi_msg_nth(t_osc_region r, t_osc_msg m, int n);
t_osc_msg osc_capi_msg_append(t_osc_region r, t_osc_msg m, t_osc_bndl e);
t_osc_msg osc_capi_msg_prepend(t_osc_region r, t_osc_msg m, t_osc_bndl e);
t_osc_msg_m osc_capi_msg_append_m(t_osc_region r, t_osc_msg_m m, t_osc_bndl e);
t_osc_msg_m osc_capi_msg_prepend_m(t_osc_region r, t_osc_msg_m m, t_osc_bndl e);

void _osc_capi_msg_print(t_osc_region r, t_osc_msg m, char eol);
void osc_capi_msg_print(t_osc_region r, t_osc_msg m);
void osc_capi_msg_println(t_osc_region r, t_osc_msg m);

#ifdef __cplusplus
}
#endif
	
#endif
