#ifndef __OSC_ATOM_H__
#define __OSC_ATOM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle.h"

typedef const t_osc_bndl t_osc_atom;
typedef t_osc_bndl_m t_osc_atom_m;

#define OSC_TT_BNDL_ID B
#define OSC_TT_NATIVE_ID n
#define OSC_TT_EXPR_ID A

t_osc_atom *osc_atom_allocInt8(int8_t i);
t_osc_atom *osc_atom_allocUInt8(uint8_t i);
t_osc_atom *osc_atom_allocInt16(int16_t i);
t_osc_atom *osc_atom_allocUInt16(uint16_t i);
t_osc_atom *osc_atom_allocInt32(int32_t i);
t_osc_atom *osc_atom_allocUInt32(uint32_t i);
t_osc_atom *osc_atom_allocInt64(int64_t i);
t_osc_atom *osc_atom_allocUInt64(uint64_t i);
t_osc_atom *osc_atom_allocFloat(float i);
t_osc_atom *osc_atom_allocDouble(double i);
t_osc_atom *osc_atom_allocString(char *i);
t_osc_atom *osc_atom_allocSymbol(char *i);
t_osc_atom *osc_atom_allocBndl(t_osc_bndl *b);
t_osc_atom *osc_atom_allocTimetag(t_osc_timetag i);
t_osc_atom *osc_atom_allocBlob(char *b);
t_osc_atom *osc_atom_allocTrue(void);
t_osc_atom *osc_atom_allocFalse(void);
t_osc_atom *osc_atom_allocNil(void);
t_osc_atom *osc_atom_allocNative(t_osc_builtin n, char *funcname);
t_osc_atom *osc_atom_allocExpr(t_osc_bndl *b);

int8_t osc_atom_getInt8(t_osc_atom *a);
int8_t osc_atom_getUInt8(t_osc_atom *a);
int16_t osc_atom_getInt16(t_osc_atom *a);
int16_t osc_atom_getUInt16(t_osc_atom *a);
int32_t osc_atom_getInt32(t_osc_atom *a);
int32_t osc_atom_getUInt32(t_osc_atom *a);
int64_t osc_atom_getInt64(t_osc_atom *a);
int64_t osc_atom_getUInt64(t_osc_atom *a);
float osc_atom_getFloat(t_osc_atom *a);
double osc_atom_getDouble(t_osc_atom *a);
char *osc_atom_getStringPtr(t_osc_atom *a);
char *osc_atom_getSymbolPtr(t_osc_atom *a);
t_osc_bndl *osc_atom_getBndlPtr(t_osc_atom *a);
t_osc_timetag osc_atom_getTimetag(t_osc_atom *a);
char *osc_atom_getBlobPtr(t_osc_atom *a);
t_osc_builtin osc_atom_getNativePtr(t_osc_atom *a);
t_osc_bndl *osc_atom_getExprPtr(t_osc_atom *a);

t_osc_atom *osc_atom_retain(t_osc_atom *a);
void osc_atom_release(t_osc_atom *a);
char osc_atom_getTypetag(t_osc_atom *a);

#ifdef __cplusplus
}
#endif

#endif
