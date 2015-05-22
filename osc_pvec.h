#ifndef __OSC_PVEC_H__
#define __OSC_PVEC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _osc_pvec_node t_osc_pvec_node;


typedef struct _osc_pvec t_osc_pvec;
typedef struct _osc_pvec2 t_osc_pvec2;

extern t_osc_pvec *osc_pvec_empty;
extern t_osc_pvec2 *osc_pvec2_empty;

t_osc_pvec *osc_pvec_alloc(void (*freefn)(void *));
void osc_pvec_release(t_osc_pvec *pvec);
t_osc_pvec *osc_pvec_copy(t_osc_pvec *pvec);
t_osc_pvec *osc_pvec_clone(t_osc_pvec *pvec);
void *osc_pvec_nth(t_osc_pvec *pvec, int i);
t_osc_pvec *osc_pvec_assocN(t_osc_pvec *pvec, int i, void *val);
t_osc_pvec *osc_pvec_assocN_m(t_osc_pvec *pvec, int i, void *val);
t_osc_pvec *osc_pvec_pop(t_osc_pvec *pvec);
int osc_pvec_length(t_osc_pvec *pvec);

t_osc_pvec2 *osc_pvec2_alloc(void (*freefn)(void *));
void osc_pvec2_release(t_osc_pvec2 *pvec2);
t_osc_pvec2 *osc_pvec2_copy(t_osc_pvec2 *pvec2);
t_osc_pvec2 *osc_pvec2_clone(t_osc_pvec2 *pvec2);
void *osc_pvec2_nth(t_osc_pvec2 *pvec2, int i);
t_osc_pvec2 *osc_pvec2_assocN(t_osc_pvec2 *pvec2, int i, void *val);
t_osc_pvec2 *osc_pvec2_assocN_m(t_osc_pvec2 *pvec2, int i, void *val);
t_osc_pvec2 *osc_pvec2_append(t_osc_pvec2 *pvec2, void *val);
t_osc_pvec2 *osc_pvec2_append_m(t_osc_pvec2 *pvec2, void *val);
t_osc_pvec2 *osc_pvec2_prepend(t_osc_pvec2 *pvec2, void *val);
t_osc_pvec2 *osc_pvec2_prepend_m(t_osc_pvec2 *pvec2, void *val);
t_osc_pvec2 *osc_pvec2_pop(t_osc_pvec2 *pvec2);
int osc_pvec2_length(t_osc_pvec2 *pvec2);

void osc_pvec_print(t_osc_pvec *pvec);

#ifdef __cplusplus
}
#endif

#endif
