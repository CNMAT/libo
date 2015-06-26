#ifndef __OSC_ATOM_H__
#define __OSC_ATOM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct _osc_atom t_osc_atom;
typedef struct _osc_atom t_osc_atom_m;

#include "osc_bundle.h"
#include "osc_builtin.h"
#include "osc_pvec.h"

#define OSC_TT_BNDL_ID B
#define OSC_TT_NATIVE_ID n
#define OSC_TT_EXPR_ID A

t_osc_atom *osc_atom_true;
t_osc_atom *osc_atom_false;
t_osc_atom *osc_atom_nil;
t_osc_atom *osc_atom_undefined;
t_osc_atom *osc_atom_emptystring;
t_osc_atom *osc_atom_valueaddress;
t_osc_atom *osc_atom_expraddress;
t_osc_atom *osc_atom_typeaddress;
t_osc_atom *osc_atom_funcaddress;
t_osc_atom *osc_atom_argsaddress;
t_osc_atom *osc_atom_partialaddress;
t_osc_atom *osc_atom_completeaddress;
t_osc_atom *osc_atom_unmatchedaddress;
t_osc_atom *osc_atom_ps_add;
t_osc_atom *osc_atom_ps_nth;
t_osc_atom *osc_atom_yaddress;
t_osc_atom *osc_atom_lhsaddress;
t_osc_atom *osc_atom_rhsaddress;
t_osc_atom *osc_atom_naddress;
t_osc_atom *osc_atom_listaddress;

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
t_osc_atom *osc_atom_allocString(char *i, int should_free_ptr);
t_osc_atom *osc_atom_allocSymbol(char *i, int should_free_ptr);
t_osc_atom *osc_atom_allocBndl(t_osc_bndl *b, int should_free_bndl);
t_osc_atom *osc_atom_allocTimetag(t_osc_timetag i);
t_osc_atom *osc_atom_allocBlob(char *b, int should_free_ptr);
t_osc_atom *osc_atom_allocTrue(void);
t_osc_atom *osc_atom_allocFalse(void);
t_osc_atom *osc_atom_allocNil(void);
t_osc_atom *osc_atom_allocNative(t_osc_builtin n, char *funcname);
t_osc_atom *osc_atom_allocExpr(t_osc_bndl *b, int should_free);

t_osc_atom *osc_atom_clone(t_osc_atom *a);
t_osc_atom *osc_atom_retain(t_osc_atom *a);
void osc_atom_free(void *_a);
void osc_atom_release(t_osc_atom *a);
char osc_atom_getTypetag(t_osc_atom *a);
int osc_atom_getSerializedLen(t_osc_atom *a);
char *osc_atom_getSerializedPtr(t_osc_atom *a);
int osc_atom_getPrettyLen(t_osc_atom *a);
char *osc_atom_getPrettyPtr(t_osc_atom *a);
t_osc_atom *osc_atom_strcat(t_osc_pvec2 *pvec2);
t_osc_atom *osc_atom_formatAtomsAsMsg(t_osc_pvec2 *pvec2,
				      int prefixlen,
				      char *prefix,
				      int firstseplen,
				      char *firstsep,
				      int restseplen,
				      char *restsep,
				      int lastseplen,
				      char *lastsep,
				      int postfixlen,
				      char *postfix,
				      int level);
t_osc_atom_m *osc_atom_serialize_m(t_osc_atom_m *a);
t_osc_atom *osc_atom_serialize(t_osc_atom *a);
t_osc_atom_m *osc_atom_format_m(t_osc_atom_m *a, int level);
t_osc_atom *osc_atom_format(t_osc_atom *a, int level);
t_osc_atom *osc_atom_promote(t_osc_atom *a, char typetag);
t_osc_atom *osc_atom_convertAny(t_osc_atom *a, char typetag);

t_osc_atom *osc_atom_apply(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_atom *a, t_osc_bndl *context);

t_osc_atom *osc_atom_match(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_nth(t_osc_atom *a, t_osc_atom *n);
t_osc_atom *osc_atom_add(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_sub(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_mul(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_div(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_eql(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_eqv(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_lt(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_le(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_gt(t_osc_atom *lhs, t_osc_atom *rhs);
t_osc_atom *osc_atom_ge(t_osc_atom *lhs, t_osc_atom *rhs);

t_osc_msg *osc_atom_value(t_osc_atom *a);
t_osc_atom *osc_atom_eagerEval(t_osc_atom *a, t_osc_bndl *context);
t_osc_atom *osc_atom_lazyEval(t_osc_atom *a, t_osc_bndl *context);
t_osc_atom *osc_atom_eval(t_osc_atom *a, t_osc_bndl *context);

#ifdef __cplusplus
}
#endif

#endif
