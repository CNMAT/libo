#include <string.h>
#include "osc.h"
#include "osc_bundle.h"
#include "osc_builtin.h"
#include "osc_atom.h"

/*
add : {\
			/doc : \"some foo\",\
			/params : [\"/lhs\", \"/rhs\"],\
			/lhs,\
			/rhs,\
			/y : /func {/lhs : /lhs, /rhs : /rhs},\
			/func : __native add,\
			/value : /y\
		},\
 */
char *osc_builtin_math =
	"{\
		add : {\
			/doc : \"some foo\",\
			/params : [\"/lhs\", \"/rhs\"],\
			/lhs,\
			/rhs,\
			/expr : __native add\
		},\
		nth : {\
			/doc : \"some foo\",\
			/params : [\"/n\", \"/list\"],\
			/n,\
			/list,\
			/expr : __native nth\
		}\
	}";

t_osc_bndl *osc_builtin_math_add(t_osc_bndl *b)
{
	t_osc_msg *lhs = osc_atom_value(osc_msg_nth(osc_bndl_lookup(b, osc_atom_lhsaddress, osc_atom_match), 1));
	t_osc_msg *rhs = osc_atom_value(osc_msg_nth(osc_bndl_lookup(b, osc_atom_rhsaddress, osc_atom_match), 1));
	if(lhs && rhs){
		t_osc_atom *l = osc_msg_nth(lhs, 1);
		t_osc_atom *r = osc_msg_nth(rhs, 1);
		if(l == osc_atom_undefined || r == osc_atom_undefined){
			return osc_bndl_retain(b);
		}else{
			t_osc_atom *y = osc_atom_add(l, r);
			t_osc_bndl *out1 = osc_bndl_append(b, osc_msg_alloc(osc_atom_yaddress, 1, y));
			t_osc_bndl *out = osc_bndl_append(out1, osc_msg_alloc(osc_atom_valueaddress, 2, osc_atom_yaddress, osc_atom_retain(y)));
			osc_bndl_release(out1);
			return out;
		}
	}
	return osc_bndl_retain(b);
}

t_osc_bndl *osc_builtin_math_nth(t_osc_bndl *b)
{
	t_osc_msg *n = osc_atom_value(osc_msg_nth(osc_bndl_lookup(b, osc_atom_naddress, osc_atom_match), 1));
	t_osc_msg *list = osc_atom_value(osc_msg_nth(osc_bndl_lookup(b, osc_atom_listaddress, osc_atom_match), 1));
	if(osc_msg_length(list) == 0){
		return osc_bndl_append(b, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_undefined));
	}else if(osc_msg_length(list) == 1){
		t_osc_atom *a = osc_msg_nth(list, 1);
		if(osc_atom_getTypetag(a) == OSC_BUNDLE_TYPETAG){
			//return osc_bndl_append(b, osc_msg_prepend(osc_bndl_nth(b, n
		}
	}else{

	}
}

struct osc_builtin_funcrec
{
	char *name;
	t_osc_builtin func;
} osc_builtin_funcrec [] = {
	{"add", osc_builtin_math_add},
	{"nth", osc_builtin_math_nth}
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
