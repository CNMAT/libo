#ifndef __OSC_BUNDLE_H__
#define __OSC_BUNDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct _osc_bndl t_osc_bndl;
typedef struct _osc_bndl t_osc_bndl_m;

#include "osc_timetag.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_pvec.h"

t_osc_bndl *osc_bndl_allocWithPvec2(t_osc_timetag timetag, t_osc_pvec2 *pvec2);
t_osc_bndl *osc_bndl_alloc(t_osc_timetag timetag, int n, ...);

//t_osc_bndl *osc_bndl_empty;
#define osc_bndl_empty osc_bndl_alloc(OSC_TIMETAG_NULL, 0);

t_osc_bndl *osc_bndl_retain(t_osc_bndl *b);
void osc_bndl_release(t_osc_bndl *b);
void osc_bndl_free(void *b);
t_osc_bndl *osc_bndl_clone(t_osc_bndl *);
t_osc_bndl_m *osc_bndl_serialize_m(t_osc_bndl_m *b);
t_osc_bndl *osc_bndl_serialize(t_osc_bndl *b);
t_osc_bndl_m *osc_bndl_format_m(t_osc_bndl_m *b, int level);
t_osc_bndl *osc_bndl_format(t_osc_bndl *b, int level);
t_osc_timetag osc_bndl_getTimetag(t_osc_bndl *b);
void osc_bndl_setSerializedLen(t_osc_bndl *b, int len);
void osc_bndl_setSerializedPtr(t_osc_bndl *b, char *ptr);
void osc_bndl_setPrettyLen(t_osc_bndl *b, int len);
void osc_bndl_setPrettyPtr(t_osc_bndl *b, char *ptr);
int osc_bndl_getSerializedLen(t_osc_bndl *b);
char *osc_bndl_getSerializedPtr(t_osc_bndl *b);
int osc_bndl_getPrettyLen(t_osc_bndl *b);
char *osc_bndl_getPrettyPtr(t_osc_bndl *b);
int osc_bndl_length(t_osc_bndl *b);
t_osc_msg *osc_bndl_nth(t_osc_bndl *b, int idx);
t_osc_bndl *osc_bndl_assocn(t_osc_bndl *b, t_osc_msg *m, int idx);
t_osc_bndl_m *osc_bndl_assocn_m(t_osc_bndl_m *b, t_osc_msg *m, int idx);
t_osc_bndl *osc_bndl_append(t_osc_bndl *b, t_osc_msg *m);
t_osc_bndl_m *osc_bndl_append_m(t_osc_bndl_m *b, t_osc_msg *m);
t_osc_bndl *osc_bndl_prepend(t_osc_bndl *b, t_osc_msg *m);
t_osc_bndl_m *osc_bndl_prepend_m(t_osc_bndl_m *b, t_osc_msg *m);
t_osc_bndl *osc_bndl_apply(t_osc_bndl *(*fn)(t_osc_bndl *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context);
t_osc_bndl *osc_bndl_map(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context);
t_osc_bndl *osc_bndl_filter(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_bndl *b, t_osc_bndl *context);
t_osc_msg *osc_bndl_strcmpAddress(t_osc_msg *m, t_osc_bndl *context);

t_osc_bndl *osc_bndl_union(t_osc_bndl *lhs, t_osc_bndl *rhs);
t_osc_bndl *osc_bndl_intersect(t_osc_bndl *lhs, t_osc_bndl *rhs);
t_osc_bndl *osc_bndl_rcompliment(t_osc_bndl *lhs, t_osc_bndl *rhs);

t_osc_atom *osc_bndl_isFixpoint(t_osc_bndl *b);
t_osc_bndl *osc_bndl_eval(t_osc_bndl *b, t_osc_bndl *context);
t_osc_bndl *osc_bndl_reduce(t_osc_bndl *b);

t_osc_msg *osc_bndl_lookup(t_osc_bndl *b, t_osc_atom *key, t_osc_atom *(*f)(t_osc_atom *, t_osc_atom *));

t_osc_bndl *osc_bndl_appendStatus(t_osc_bndl *b, t_osc_atom *ok, t_osc_atom *msg, int nmsgs, ...);
t_osc_bndl_m *osc_bndl_appendStatus_m(t_osc_bndl_m *b, t_osc_atom *ok, t_osc_atom *msg, int nmsgs, ...);
t_osc_atom *osc_bndl_statusOK(t_osc_bndl *b);
t_osc_msg *osc_bndl_getStatusMsg(t_osc_bndl *b);
t_osc_bndl *osc_bndl_clearStatus(t_osc_bndl *b);
t_osc_bndl *osc_bndl_clearAllStatus(t_osc_bndl *b);

#ifdef __cplusplus
}
#endif
	
#endif
