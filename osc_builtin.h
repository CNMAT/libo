#ifndef __OSC_BUILTIN_H__
#define __OSC_BUILTIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle.h"
#include "osc_region.h"

typedef t_osc_bndl *(*t_osc_builtin_fn)(t_osc_region r, t_osc_bndl b, t_osc_bndl context);

t_osc_bndl osc_builtin_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context);
t_osc_bndl osc_builtin_std(t_osc_region r);

#ifdef __cplusplus
}
#endif
#endif
