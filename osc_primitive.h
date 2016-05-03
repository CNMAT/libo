#ifndef __OSC_PRIMITIVE_H__
#define __OSC_PRIMITIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle.h"
#include "osc_native.h"
#include "osc_region.h"

t_osc_bndl osc_primitive_isIndexable(t_osc_region r, t_osc_bndl b);
t_osc_bndl osc_primitive_nth(t_osc_region r, t_osc_bndl b, t_osc_bndl n);
t_osc_bndl osc_primitive_length(t_osc_region r, t_osc_bndl b);

t_osc_bndl osc_primitive_eql(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_primitive_eqv(t_osc_region r, t_osc_bndl b1, t_osc_bndl b2);
t_osc_bndl osc_primitive_match(t_osc_region r, t_osc_bndl pattern, t_osc_bndl address);
t_osc_bndl osc_primitive_add(t_osc_region r, t_osc_bndl lhs, t_osc_bndl rhs);

#ifdef __cplusplus
}
#endif
	
#endif
