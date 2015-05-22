#ifndef __OSC_BUILTIN_H__
#define __OSC_BUILTIN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_bndl *(*t_osc_builtin)(struct _osc_bndl*);

#include "osc_bundle.h"

char *osc_builtin_math;

t_osc_builtin osc_builtin_lookup(char *funcname);

#ifdef __cplusplus
}
#endif
#endif
