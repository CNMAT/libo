
#include <stdio.h>

#include "osc_bundle_s.h"
#include "osc_bundle_iterator_s.h"

#include "osc_expr.h"
#include "osc_mem.h"

#include "osc_expr_func.h"
#include "osc_expr.h"

void getFunctionArray(struct _osc_expr_rec* symList, size_t* num )
{
    symList = osc_expr_funcsym;
    *num = sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec);
    
    int i;
    for(i = 0; i < *num; i++){
        t_osc_expr_rec r = symList[i];
        printf("%s\n", r.signature );
    }
    
}

int main(int argc, const char * argv[]) {
    struct _osc_expr_rec* symList = NULL;
    size_t num = 0;
    getFunctionArray(symList, &num);
    return 0;
}
