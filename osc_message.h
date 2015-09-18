#ifndef __OSC_MESSAGE_H__
#define __OSC_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct _osc_msg t_osc_msg;
typedef struct _osc_msg t_osc_msg_m;

#include "osc_bundle.h"
#include "osc_atom.h"

t_osc_msg *osc_msg_alloc(t_osc_atom *address, int n, ...);
t_osc_msg *osc_msg_allocWithPvec2(t_osc_pvec2 *pvec2);
t_osc_msg *osc_msg_clone(t_osc_msg *m);
t_osc_msg *osc_msg_retain(t_osc_msg *m);
void osc_msg_free(void *m);
void osc_msg_release(t_osc_msg *m);
int osc_msg_getSerializedLen(t_osc_msg *m);
char *osc_msg_getSerializedPtr(t_osc_msg *m);
t_osc_msg *osc_msg_serialize(t_osc_msg *m);
t_osc_atom *osc_msg_format(t_osc_msg *m, int prefixlen, char *prefix, int postfixlen, char *postfix, int level);

int osc_msg_length(t_osc_msg *m);
t_osc_atom *osc_msg_nth(t_osc_msg *m, int idx);
t_osc_msg *osc_msg_assocn(t_osc_msg *m, t_osc_atom *a, int idx);
t_osc_msg_m *osc_msg_assocn_m(t_osc_msg_m *m, t_osc_atom *a, int idx);
t_osc_msg *osc_msg_append(t_osc_msg *m, t_osc_atom *a);
t_osc_msg_m *osc_msg_append_m(t_osc_msg_m *m, t_osc_atom *a);
t_osc_msg *osc_msg_prepend(t_osc_msg *m, t_osc_atom *a);
t_osc_msg_m *osc_msg_prepend_m(t_osc_msg_m *m, t_osc_atom *a);
t_osc_msg *osc_msg_popFirst(t_osc_msg *m);
t_osc_msg *osc_msg_popLast(t_osc_msg *m);
t_osc_msg *osc_msg_apply(t_osc_msg *(*fn)(t_osc_msg *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context);
t_osc_msg *osc_msg_map(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context);
t_osc_msg *osc_msg_filter(t_osc_atom *(*fn)(t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context);
t_osc_msg *osc_msg_lreduce(t_osc_atom *(*fn)(t_osc_atom *, t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context);
t_osc_msg *osc_msg_rreduce(t_osc_atom *(*fn)(t_osc_atom *, t_osc_atom *, t_osc_bndl *), t_osc_msg *m, t_osc_bndl *context);

t_osc_msg *osc_msg_evalStrict(t_osc_msg *m, t_osc_bndl *context);
t_osc_atom *osc_msg_eql(t_osc_msg *m1, t_osc_msg *m2);

#ifdef __cplusplus
}
#endif

#endif
