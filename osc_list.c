#include <stdarg.h>
#include <stdio.h>
#include "osc_list.h"
#include "osc_pvec.h"
#include "osc_mem.h"

struct _osc_listitem
{
	int type;
	void *ptr;
};

t_osc_list osc_list_alloc(t_osc_region r, void (*freefn)(void *), int n, ...)
{
	t_osc_pvec2 *l = osc_pvec2_alloc(r, freefn);
	va_list ap;
	va_start(ap, n);
	for(int i = 0; i < n; i++){
		t_osc_listitem *li = va_arg(ap, t_osc_listitem*);
		osc_pvec2_append_m(r, l, (void *)li);
	}
	va_end(ap);
	return (t_osc_list)l;
}

t_osc_listitem *osc_list_allocItem(t_osc_region r, int type, void *ptr)
{
	t_osc_listitem *li = (t_osc_listitem *)osc_region_getBytes(r, sizeof(t_osc_listitem));
	li->type = type;
	li->ptr = ptr;
	return li;
}

/*
void osc_list_freeItem(t_osc_listitem *li)
{
	if(li){
		osc_mem_free(li);
	}
}
*/

int osc_list_itemGetType(t_osc_listitem *li)
{
	if(li){
		return li->type;
	}else{
		return -666;
	}
}

void *osc_list_itemGetPtr(t_osc_listitem *li)
{
	if(li){
		return li->ptr;
	}else{
		return NULL;
	}
}

/*
void osc_list_release(t_osc_list list)
{
	osc_pvec2_release((t_osc_pvec2 *)list);
}

t_osc_list osc_list_retain(t_osc_list list)
{
	return (t_osc_list)osc_pvec2_retain((t_osc_pvec2 *)list);
}
*/

t_osc_listitem *osc_list_nth(t_osc_list list, int i)
{
	return (t_osc_listitem *)osc_pvec2_nth((t_osc_pvec2 *)list, i);
}

t_osc_list osc_list_append(t_osc_region r, t_osc_list list, t_osc_listitem *val)
{
	return (t_osc_list)osc_pvec2_append(r, (t_osc_pvec2 *)list, (void *)val);
}

t_osc_list osc_list_append_m(t_osc_region r, t_osc_list_m list, t_osc_listitem *val)
{
	return (t_osc_list)osc_pvec2_append_m(r, (t_osc_pvec2 *)list, (void *)val);
}

t_osc_list osc_list_prepend(t_osc_region r, t_osc_list list, t_osc_listitem *val)
{
	return (t_osc_list)osc_pvec2_prepend(r, (t_osc_pvec2 *)list, (void *)val);
}

t_osc_list osc_list_prepend_m(t_osc_region r, t_osc_list_m list, t_osc_listitem *val)
{
	return (t_osc_list)osc_pvec2_prepend_m(r, (t_osc_pvec2 *)list, (void *)val);
}

t_osc_list osc_list_popFirst(t_osc_region r, t_osc_list list)
{
	return (t_osc_list)osc_pvec2_popFirst(r, (t_osc_pvec2 *)list);
}

t_osc_list osc_list_popLast(t_osc_region r, t_osc_list list)
{
	return (t_osc_list)osc_pvec2_popFirst(r, (t_osc_pvec2 *)list);
}

int osc_list_length(t_osc_list list)
{
	return osc_pvec2_length((t_osc_pvec2 *)list);
}
