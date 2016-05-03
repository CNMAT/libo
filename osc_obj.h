#ifndef __OSC_OBJ_H__
#define __OSC_OBJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define OSC_OBJ_INVALID 666

typedef struct _osc_obj
{
	int32_t refcount;
	void (*freefn)(void *);
	int32_t type;
} t_osc_obj;

#ifdef __cplusplus
}
#endif
#endif
