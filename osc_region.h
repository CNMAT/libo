#ifndef __OSC_REGION_H__
#define __OSC_REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define OSC_REGION_DEFAULT_STACK_SIZE_BYTES 1000000
#define OSC_REGION_DEFAULT_TMP_STACK_SIZE_BYTES 256000

typedef struct _osc_region* t_osc_region;

// allocates a block of nbytes + ntmpbytes
t_osc_region osc_region_alloc(size_t nbytes, size_t ntmpbytes);
// bytes should be a pointer to a block of memory nbytes + ntmpbytes long
t_osc_region osc_region_allocWithBytes(size_t nbytes, size_t ntmpbytes, char *bytes);
void osc_region_delete(t_osc_region r);
void osc_region_deleteWithoutFreeingBytes(t_osc_region r);
//#define OSC_REGION_LOG 1
#ifdef OSC_REGION_LOG
void *_osc_region_getBytes(t_osc_region r, size_t nbytes, const char *func);
#define osc_region_getBytes(r, nbytes) _osc_region_getBytes(r, nbytes, __func__);
#else
void *_osc_region_getBytes(t_osc_region r, size_t nbytes);
#define osc_region_getBytes(r, nbytes) _osc_region_getBytes(r, nbytes)
#endif
t_osc_region osc_region_getTmp(t_osc_region r);
void osc_region_releaseTmp(t_osc_region r);
char *osc_region_getPtr(t_osc_region r);
void osc_region_unwind(t_osc_region r, char *ptr);
size_t osc_region_bytesUsed(t_osc_region r);
size_t osc_region_bytesFree(t_osc_region r);

#ifdef __cplusplus
}
#endif

#endif
