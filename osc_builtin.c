#include <string.h>
#include "osc_builtin.h"

char *osc_builtin_math =
	"{\
		add : {\
			/doc : \"some foo\",\
			/params : [/lhs, /rhs],\
			/lhs,\
			/rhs,\
			/expr : __native add,\
			/value\
		}\
	}";

struct _osc_bndl *osc_builtin_math_add(struct _osc_bndl *b)
{
	return b;
}

struct osc_builtin_funcrec
{
	char *name;
	//t_osc_bndl *(*func)(t_osc_bndl *);
	t_osc_builtin func;
} osc_builtin_funcrec [] = {
	{"add", osc_builtin_math_add}
};

t_osc_builtin osc_builtin_lookup(char *funcname)
{
	for(int i = 0; i < sizeof(osc_builtin_funcrec) / sizeof(struct osc_builtin_funcrec); ++i){
		if(!strcmp(funcname, osc_builtin_funcrec[i].name)){
			return osc_builtin_funcrec[i].func;
		}
	}
	return NULL;
}
