#include <string.h>
#include "osc.h"
#include "osc_bundle.h"
#include "osc_builtin.h"
#include "osc_atom.h"
#include "osc_parse.h"
#include "osc_typetag.h"

// add initialize and finalize hooks
char *osc_builtin_math =
	"{\
		add : {\
			/doc : \"some foo\",\
			/params : [\"/lhs\", \"/rhs\"],\
			/lhs,\
			/rhs,\
			/y : /func {/__lhs : /lhs, /__rhs : /rhs},\
			/func : __native add,\
			/value : /y\
		},\
		nth : {\
			/doc : \"some foo\",\
			/params : [\"/n\", \"/list\"],\
			/n,\
			/list,\
			/func : __native nth,\
			/nth : /func {/__n : /n, /__list : /list},\
			/value : /nth\
		},\
		lookup : {\
			/doc : \"some foo\",\
			/params : [\"/lhs\", \"/rhs\"],\
			/lhs,\
			/rhs,\
			/func : __native lookup,\
			/y : /func {/__lhs : /lhs, /__rhs : /rhs},\
			/value : /y\
		},\
		quote : {\
			/doc : \"some foo\",\
			/params : [\"/arg\"],\
			/arg,\
			/func : __native quote,\
			/y : /func {/__arg : /arg},\
			/value : /y\
		}\
	}";

t_osc_bndl *osc_builtin_math_add(t_osc_bndl *b)
{
	t_osc_atom *lhsa = osc_atom_allocSymbol("/__lhs", 0);
	t_osc_atom *rhsa = osc_atom_allocSymbol("/__rhs", 0);
	t_osc_atom *lhs = osc_msg_nth(osc_bndl_lookup(b, lhsa, osc_atom_match), 1);
	t_osc_atom *rhs = osc_msg_nth(osc_bndl_lookup(b, rhsa, osc_atom_match), 1);
	osc_atom_release(lhsa);
	osc_atom_release(rhsa);
	t_osc_bndl *out = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	if(lhs && rhs){
		t_osc_atom *l = lhs;
		t_osc_atom *r = rhs;
		if(l == osc_atom_undefined || r == osc_atom_undefined){
			osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("value undefined", 0), 0);
			return out;
		}else{
			t_osc_atom *y = osc_atom_add(l, r);
			if(osc_atom_undefined == y){
				osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("output undefined", 0), 0);
				return out;
			}else{
				return osc_bndl_append(b, osc_msg_alloc(osc_atom_valueaddress, 1, y));
			}
		}
	}
	osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_true, osc_atom_allocString("partial evaluation", 0), 0);
	return out;
}

t_osc_bndl *osc_builtin_math_nth(t_osc_bndl *b)
{
	t_osc_atom *na = osc_atom_allocSymbol("/__n", 0);
	t_osc_atom *lista = osc_atom_allocSymbol("/__list", 0);
	t_osc_msg *nm = osc_bndl_lookup(b, na, osc_atom_match);
	t_osc_msg *listm = osc_bndl_lookup(b, lista, osc_atom_match);
	osc_atom_release(na);
	osc_atom_release(lista);
	t_osc_bndl *out = osc_bndl_alloc(OSC_TIMETAG_NULL, 0);
	if(nm && listm){
		t_osc_msg *v = osc_msg_alloc(osc_atom_valueaddress, 0);
		for(int i = 1; i < osc_msg_length(nm) + 1; i++){
			t_osc_atom *n = osc_msg_nth(nm, i);
			if(OSC_TYPETAG_ISINT(osc_atom_getTypetag(n))){
				int nn = osc_atom_getInt32(n);
				if(osc_msg_length(listm) == 1){
					t_osc_atom *a = osc_msg_nth(listm, 1);
					if(osc_atom_indexable(a) == osc_atom_true){
						t_osc_atom *aa = osc_atom_nth(osc_msg_nth(listm, 1), n);
						if(aa == osc_atom_undefined){
							osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index out of range", 0), 0);
							break;
						}else{
							osc_msg_append_m((t_osc_msg_m *)v, aa);
						}
					}else{
						osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("item is not indexable", 0), 0);
						break;
					}
				}else{
					if(nn >= 0 && nn <= osc_msg_length(listm)){
						osc_msg_append_m((t_osc_msg_m *)v, osc_atom_retain(osc_msg_nth(listm, nn)));
					}else{
						osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index out of range", 0), 0);
						break;
					}
				}
			}else{
				osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index must be an integer", 0), 0);
				break;
			}
		}
		return osc_bndl_append_m((t_osc_bndl_m *)out, v);
	}
	return out;
}

t_osc_bndl *osc_builtin_math_lookup(t_osc_bndl *b)
{
	t_osc_atom *lhsa = osc_atom_allocSymbol("/__lhs", 0);
	t_osc_atom *rhsa = osc_atom_allocSymbol("/__rhs", 0);
	t_osc_atom *lhs = osc_msg_nth(osc_bndl_lookup(b, lhsa, osc_atom_match), 1);
	t_osc_atom *rhs = osc_msg_nth(osc_bndl_lookup(b, rhsa, osc_atom_match), 1);
	osc_atom_release(lhsa);
	osc_atom_release(rhsa);
	if(lhs && rhs){
		t_osc_atom *l = lhs;
		t_osc_atom *r = rhs;
		if(l == osc_atom_undefined || r == osc_atom_undefined){
			return osc_bndl_empty;
		}else{
			if(osc_atom_getTypetag(l) == OSC_TT_BNDL){
				t_osc_bndl *bb = osc_atom_getBndlPtr(l);
				t_osc_msg *m = osc_bndl_lookup(bb, rhs, osc_atom_match);
				if(m){
					return osc_bndl_append(b, osc_msg_assocn(m, osc_atom_valueaddress, 0));
				}else{
					return osc_bndl_empty;
				}
			}else{
				return osc_bndl_empty;
			}
		}
	}
	return osc_bndl_empty;
}

struct osc_builtin_funcrec
{
	char *name;
	t_osc_builtin func;
} osc_builtin_funcrec [] = {
	{"add", osc_builtin_math_add},
	{"nth", osc_builtin_math_nth},
	{"lookup", osc_builtin_math_lookup},
	//{"quote", osc_builtin_math_quote}
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

