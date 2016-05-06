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

// allocates a block of nbytes + ntmpbytes
t_osc_region osc_region_alloc(size_t nbytes, size_t ntmpbytes)
{
	t_osc_region r = osc_mem_alloc(sizeof(struct _osc_region) * 2);
	t_osc_region tmp = r + 1;
	if(nbytes == 0){
		r->stacksize = OSC_REGION_DEFAULT_STACK_SIZE_BYTES;
	}else{
		r->stacksize = nbytes;
	}
	if(ntmpbytes == 0){
		tmp->stacksize = OSC_REGION_DEFAULT_TMP_STACK_SIZE_BYTES;
	}else{
		tmp->stacksize = ntmpbytes;
	}
	r->stack = osc_mem_alloc(r->stacksize + tmp->stacksize);
	r->stackptr = r->stack;
	tmp->stack = r->stack + r->stacksize;
	tmp->stackptr = tmp->stack;
	return r;
}

// bytes should be a pointer to a block of memory nbytes + ntmpbytes long
t_osc_region osc_region_allocWithBytes(size_t nbytes, size_t ntmpbytes, char *bytes)
{
	t_osc_region r = osc_mem_alloc(sizeof(struct _osc_region) * 2);
	t_osc_region tmp = r + 1;
	r->stacksize = nbytes;
	r->stack = bytes;
	r->stackptr = r->stack;
	tmp->stacksize = ntmpbytes;
	tmp->stack = r->stack + nbytes;
	tmp->stackptr = tmp->stack;
	return r;
}

void osc_region_delete(t_osc_region r)
{
	if(r){
		if(r->stack){
			osc_mem_free(r->stack);
		}
		memset(r, 0, sizeof(struct _osc_region) * 2);
		osc_mem_free(r);
	}
}

void osc_region_deleteWithoutFreeingBytes(t_osc_region r)
{
	if(r){
		memset(r, 0, sizeof(struct _osc_region) * 2);
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
	if(r && ((r->stackptr + nbytes) - r->stack < r->stacksize)){
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

t_osc_region osc_region_getTmp(t_osc_region r)
{
	if(r){
		return r + 1;
	}
	return NULL;
}

void osc_region_releaseTmp(t_osc_region r)
{
	if(r){
		t_osc_region tmp = r + 1;
		tmp->stackptr = tmp->stack;
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
