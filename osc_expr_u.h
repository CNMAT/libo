
#ifndef __OSC_EXPR_U_H__
#define __OSC_EXPR_U_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_expr.h"

int osc_expr_u_evalInLexEnv(t_osc_expr *f,
			  t_osc_expr_lexenv *lexenv,
			  t_osc_bndl_u *u_bndl,
			  t_osc_atom_ar_u **out,
				void *context);
    
int osc_expr_u_eval(t_osc_expr *function, t_osc_bndl_u *u_bndl, t_osc_atom_ar_u **out, void *context);

t_osc_err osc_expr_u_getFunctionsForCategory( char *cat, t_osc_bndl_u ** u_bndl );

t_osc_err osc_expr_u_specFunc_assignToBundleMember_proc(t_osc_expr *f,
                                                  t_osc_expr_lexenv *lexenv,
                                                  t_osc_bndl_u *u_bndl,
                                                  t_osc_atom_ar_u **out,
                                                  void *context);
    
t_osc_err osc_expr_u_specFunc_assignToBundleMember_factory(t_osc_bundle_u *bndl,
                                                     long argc,
                                                     t_osc_expr_arg *argv,
                                                     t_osc_msg_u **target_msg,
                                                     t_osc_expr_arg **value );
    


#ifdef __cplusplus
}
#endif

#endif // __OSC_EXPR_U_H__
