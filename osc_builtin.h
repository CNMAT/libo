#ifndef __OSC_BUILTIN_H__
#define __OSC_BUILTIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle.h"
typedef t_osc_bndl *(*t_osc_builtin)(t_osc_bndl*, t_osc_bndl*);

char *osc_builtin_math;

t_osc_builtin osc_builtin_lookup(char *funcname);

#ifdef __cplusplus
}
#endif
#endif
