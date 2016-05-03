#ifndef __OSC_MESSAGE_H__
#define __OSC_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "osc_bundle.h"
#include "osc_region.h"

t_osc_bndl osc_msg_nth(t_osc_region r, t_osc_msg m, t_osc_bndl n);

#ifdef __cplusplus
}
#endif

#endif
