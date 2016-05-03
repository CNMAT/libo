#ifndef __OSC_LIST_H__
#define __OSC_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_pvec.h"
#include "osc_region.h"

typedef const t_osc_pvec2* t_osc_list;
typedef t_osc_pvec2* t_osc_list_m;
typedef struct _osc_listitem t_osc_listitem;

t_osc_list osc_list_alloc(t_osc_region r, void (*freefn)(void *), int n, ...);
t_osc_listitem *osc_list_allocItem(t_osc_region r, int type, void *ptr);
//void osc_list_freeItem(t_osc_listitem *li);
int osc_list_itemGetType(t_osc_listitem *li);
void *osc_list_itemGetPtr(t_osc_listitem *li);
//void osc_list_release(t_osc_list list);
//t_osc_list osc_list_retain(t_osc_list list);
t_osc_listitem *osc_list_nth(t_osc_list list, int i);
t_osc_list osc_list_append(t_osc_region r, t_osc_list list, t_osc_listitem *val);
t_osc_list osc_list_append_m(t_osc_region r, t_osc_list_m list, t_osc_listitem *val);
t_osc_list osc_list_prepend(t_osc_region r, t_osc_list list, t_osc_listitem *val);
t_osc_list osc_list_prepend_m(t_osc_region r, t_osc_list_m list, t_osc_listitem *val);
t_osc_list osc_list_popFirst(t_osc_region r, t_osc_list list);
t_osc_list osc_list_popLast(t_osc_region r, t_osc_list list);
int osc_list_length(t_osc_list list);

#ifdef __cplusplus
}
#endif

#endif
