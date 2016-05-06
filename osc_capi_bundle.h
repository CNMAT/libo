#ifndef __OSC_CAPI_BUNDLE_H__
#define __OSC_CAPI_BUNDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_list.h"
#include "osc_timetag.h"
#include "osc_region.h"

typedef t_osc_list t_osc_bndl;
typedef t_osc_list_m t_osc_bndl_m;

#include "osc_capi_message.h"


t_osc_bndl osc_capi_bndl_alloc(t_osc_region r, t_osc_timetag time, int n, ...);
int osc_capi_bndl_getMsgCount(t_osc_region r, t_osc_bndl b);
t_osc_msg osc_capi_bndl_nth(t_osc_region r, t_osc_bndl b, int32_t n);
t_osc_bndl osc_capi_bndl_append(t_osc_region r, t_osc_bndl b, t_osc_msg m);
t_osc_bndl_m osc_capi_bndl_append_m(t_osc_region r, t_osc_bndl_m b, t_osc_msg m);

size_t osc_capi_bndl_nformat(t_osc_region r, char *s, size_t n, t_osc_bndl b, int level);
int osc_capi_bndl_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_capi_bndl_filter(t_osc_region r, t_osc_bndl b, int (*test)(t_osc_region, t_osc_msg, void*), void *context);
t_osc_bndl osc_capi_bndl_match(t_osc_region r, t_osc_bndl b, char *pattern);
t_osc_bndl osc_capi_bndl_matchFull(t_osc_region r, t_osc_bndl b, char *pattern);
t_osc_msg osc_capi_bndl_simpleLookup(t_osc_region r, t_osc_bndl b, char *pattern);
int osc_capi_bndl_exists(t_osc_region r, t_osc_bndl b, char *pattern);

void _osc_capi_bndl_print(t_osc_region r, t_osc_bndl b, char eol);
void osc_capi_bndl_print(t_osc_region r, t_osc_bndl b);
void osc_capi_bndl_println(t_osc_region r, t_osc_bndl b);

#ifdef __cplusplus
}
#endif
	
#endif
