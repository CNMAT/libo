#include <string.h>
#include <stdio.h>
#include "osc_region.h"
#include "osc_mem.h"

#define OSC_REGION_PRINTF_WARNING

struct _osc_region
{
	char *stackptr;
	char *stack;
	size_t stacksize;
};

t_osc_region osc_region_alloc(size_t nbytes)
{
	t_osc_region r = osc_mem_alloc(sizeof(struct _osc_region));
	if(nbytes == 0){
		r->stacksize = OSC_REGION_DEFAULT_STACK_SIZE_BYTES;
	}else{
		r->stacksize = nbytes;
	}
	r->stack = osc_mem_alloc(r->stacksize);
	r->stackptr = r->stack;
	return r;
}

t_osc_region osc_region_allocWithBytes(size_t nbytes, char *bytes)
{
	t_osc_region r = osc_mem_alloc(sizeof(struct _osc_region));
	r->stacksize = nbytes;
	r->stack = bytes;
	r->stackptr = r->stack;
	return r;
}

void osc_region_delete(t_osc_region r)
{
	if(r){
		if(r->stack){
			osc_mem_free(r->stack);
		}
		memset(r, 0, sizeof(struct _osc_region));
		osc_mem_free(r);
	}
}

void osc_region_deleteWithoutFreeingBytes(t_osc_region r)
{
	if(r){
		memset(r, 0, sizeof(struct _osc_region));
		osc_mem_free(r);
	}
}

#ifdef OSC_REGION_LOG
void *_osc_region_getBytes(t_osc_region r, size_t nbytes, const char *func)
{
	printf("%s: %ld\n", func, nbytes);
#else
void *_osc_region_getBytes(t_osc_region r, size_t nbytes)
{
#endif
	if(r && (((r->stackptr + nbytes) - r->stack) < r->stacksize)){
		char *sp = r->stackptr;
		r->stackptr += nbytes;
		return (void *)sp;
	}else{
#ifdef OSC_REGION_PRINTF_WARNING
		printf("%s: out of memory!\n", __func__);
#endif
		return NULL;
	}
}

void *osc_region_move(t_osc_region r, char *to, char *from, size_t nbytes)
{
	if(to == from){
		return to;
	}else{
		return memmove(to, from, nbytes);
	}
}

char *osc_region_getPtr(t_osc_region r)
{
	if(r){
		return r->stackptr;
	}else{
		return NULL;
	}
}

void osc_region_unwind(t_osc_region r, char *ptr)
{
	if(r && ptr){
		r->stackptr = ptr;
	}
}

size_t osc_region_bytesUsed(t_osc_region r)
{
	return r->stackptr - r->stack;
}

size_t osc_region_bytesFree(t_osc_region r)
{
	return r->stacksize - (r->stackptr - r->stack);
}
