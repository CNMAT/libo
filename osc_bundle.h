#ifndef __OSC_BUNDLE_H__
#define __OSC_BUNDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_region.h"
#include "osc_list.h"
#include "osc_capi_bundle.h"
#include "osc_capi_message.h"

t_osc_bndl osc_bndl_create(t_osc_region r, t_osc_timetag t);
t_osc_bndl osc_bndl_copy(t_osc_region r, t_osc_bndl b);
t_osc_bndl osc_bndl_format(t_osc_region r, t_osc_bndl b);
t_osc_bndl osc_bndl_getMsgCount(t_osc_region r, t_osc_bndl b);
t_osc_msg osc_bndl_nth(t_osc_region r, t_osc_bndl b, t_osc_bndl n);
t_osc_bndl osc_bndl_match(t_osc_region r, t_osc_bndl b, t_osc_bndl pattern);
t_osc_msg osc_bndl_simpleLookup(t_osc_region r, t_osc_bndl b, t_osc_bndl pattern);
t_osc_bndl osc_bndl_union(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_bndl_intersection(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_bndl_rcomplement(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_bndl_map(t_osc_region r, t_osc_bndl fn, t_osc_bndl args, t_osc_bndl context);
t_osc_bndl osc_bndl_lreduce(t_osc_region r, t_osc_bndl fn, t_osc_bndl args, t_osc_bndl context);
t_osc_bndl osc_bndl_applyScalarScalar(t_osc_region r, t_osc_bndl lhs, t_osc_bndl applicator, t_osc_bndl rhs, t_osc_bndl context);
t_osc_bndl osc_bndl_apply(t_osc_region r, t_osc_msg lhs, t_osc_bndl applicator, t_osc_msg rhs, t_osc_bndl context);
t_osc_bndl osc_bndl_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context);

#ifdef __cplusplus
}
#endif
	
#endif
