
#include "c-access.h"

#include "osc_bundle_s.h"
#include "osc_bundle_iterator_s.h"
    
#include "osc_mem.h"
    
#include "osc_expr_func.h"
#include "osc_expr.h"

#include <stdio.h>

void getSignatures(char **array, long *len)
{
    long num = sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec);
    
    int i;
    for(i = 0; i < num; i++){
        t_osc_expr_rec r = osc_expr_funcsym[i];
        
        printf("%s\n", r.signature );
    }
    
}
int getNumSignatures(void)
{
    return (int)(sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec));
}

char *getSignature(int i)
{
    if( i < getNumSignatures() )
        return osc_expr_funcsym[i].signature;
    else
        return NULL;
}

t_osc_expr_rec* getRec(int i)
{
    if( i < getNumSignatures() )
        return &osc_expr_funcsym[i];
    else
        return NULL;
}

t_osc_atom_u *generateAtom(int type)
{
    t_osc_atom_u *at = osc_atom_u_alloc();
    
    switch (type){
        
            
        case OSC_EXPR_ARG_TYPE_LIST:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_LIST\n");
            break;
        case OSC_EXPR_ARG_TYPE_STRING:
            printf("\t\t\t string\n");
            osc_atom_u_setString(at, (char *)"foo");
            break;
            
        case OSC_EXPR_ARG_TYPE_ATOM:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_ATOM\n");
            break;
            
        case OSC_EXPR_ARG_TYPE_EXPR:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_EXPR\n");

            break;
        case 0x24:
            printf("\t\t\t osc addr\n");

            osc_atom_u_setString(at, (char *)"/foo");
            break;
            
        case OSC_EXPR_ARG_TYPE_BOOLEAN:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_BOOLEAN\n");

            break;
            
        case OSC_EXPR_ARG_TYPE_FUNCTION:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_FUNCTION\n");

            break;
            
        case OSC_EXPR_ARG_TYPE_NUMBER:
            printf("\t\t\t OSC_EXPR_ARG_TYPE_NUMBER\n");
            osc_atom_u_setDouble(at, 1);
            break;
        default:
            printf("\t\t\t O****O\n");
            osc_atom_u_setDouble(at, 1);
            break;
    }
    
    return at;
}


