
#ifndef c_access_h
#define c_access_h

#ifdef __cplusplus
extern "C" {
    
    #include "osc_expr.h"

    void getSignatures(char **array, long *len);
    char *getSignature(int i);
    t_osc_expr_rec* getRec(int i);
    int getNumSignatures(void);
    t_osc_atom_u *generateAtom(int type);

}
#endif


#endif /* c_access_h */
