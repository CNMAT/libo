#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "osc.h"
#include "osc_bundle.h"
#include "osc_builtin.h"
#include "osc_atom.h"
#include "osc_parse.h"
#include "osc_typetag.h"
#include "osc_util.h"

// add initialize and finalize hooks
char *osc_builtin_math =
	"{\
		add : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/lhs, /rhs},\
			/body : { /value : __native add {/lhs, /rhs} }\
		},\
		eql : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/lhs, /rhs},\
			/body : { /value : __native eql {/lhs, /rhs} }\
		},\
		eqv : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/lhs, /rhs},\
			/body : { /value : __native eqv {/lhs, /rhs} }\
		},\
		nth : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/n, /list},\
			/body : { /value : __native nth {/n, /list} }\
		},\
		lookup : {\
			/expr/type : 'f',\
			/doc : \"some bar\",\
			/params/strict : {/lhs},\
			/params/nonstrict : {/rhs},\
			/body : { /value : __native lookup {/lhs, /rhs} }\
		},\
		address : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/string},\
			/body : { /value : __native address {/string} }\
		},\
		function : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/nonstrict : {/bndl},\
			/body : { /value : __native function {/bndl} }\
		},\
		map : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/args},\
			/params/nonstrict : {/fn},\
			/body : { /value : __native map { /fn, /args } }\
		},\
		if : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/params/strict : {/test},\
			/params/nonstrict : {/then, /else},\
			/body : { /value : __native if { /test, /then, /else } }\
		},\
		and : {\
			/expr/type : 'f',\
			/doc : \"some foo\",\
			/y : if { /test : /lhs, /then : if { /test : /rhs, /then : true, /else : false }, /else : false },\
			/return : /y\
		}\
	}";

#define OSC_BUILTIN_DEF_MATH_OP(op)\
t_osc_bndl *osc_builtin_math_##op(t_osc_bndl *b, t_osc_bndl *context)\
{\
	t_osc_atom *lhsa = osc_atom_allocSymbol("/lhs", 0);\
	t_osc_msg *lm = osc_bndl_lookup(b, lhsa, osc_atom_match);\
	if(!lm){\
		return osc_bndl_empty;\
	}\
	t_osc_atom *rhsa = osc_atom_allocSymbol("/rhs", 0);\
	t_osc_msg *rm = osc_bndl_lookup(b, rhsa, osc_atom_match);\
	if(!rm){\
		return osc_bndl_empty;\
	}\
	if(osc_atom_getTypetag(osc_msg_nth(lm, 1)) == OSC_TT_SYM || osc_atom_getTypetag(osc_msg_nth(rm, 1)) == OSC_TT_SYM){\
		return osc_bndl_empty;\
	}\
	t_osc_atom *y = osc_atom_##op(osc_msg_nth(lm, 1), osc_msg_nth(rm, 1));\
	if(y == osc_atom_undefined){\
		return osc_bndl_empty;\
	}else{\
		return osc_bndl_append(b, osc_msg_alloc(osc_atom_valueaddress, 1, y));\
	}\
}

OSC_BUILTIN_DEF_MATH_OP(add)
OSC_BUILTIN_DEF_MATH_OP(eql)
OSC_BUILTIN_DEF_MATH_OP(eqv)

t_osc_bndl *osc_builtin_math_thing(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_bndl *e = osc_bndl_evalNonstrict(b, context);
	t_osc_msg *v = osc_bndl_lookup(e, osc_atom_valueaddress, osc_atom_match);
	if(v){
		osc_bndl_release(e);
		
	}
	t_osc_bndl *t = osc_bndl_thing(e);
	osc_bndl_release(e);
	return t;
}

t_osc_bndl *osc_builtin_math_nth(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *nthm = osc_bndl_lookup(b, osc_atom_allocSymbol("/n", 0), osc_atom_match);
	t_osc_msg *listm = osc_bndl_lookup(b, osc_atom_allocSymbol("/list", 0), osc_atom_match);
	if(!nthm || !listm || osc_msg_length(listm) == 0){
		return osc_bndl_empty;
	}
	t_osc_atom *n = osc_msg_nth(nthm, 1);
	if(OSC_TYPETAG_ISINT(osc_atom_getTypetag(n))){
		int nn = osc_atom_getInt32(n);
		if(osc_msg_length(listm) == 1){
			t_osc_atom *a = osc_msg_nth(listm, 1);
			if(osc_atom_indexable(a) == osc_atom_true){
				t_osc_atom *aa = osc_atom_nth(osc_msg_nth(listm, 1), n);
				if(aa == osc_atom_undefined){
					//osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index out of range", 0), 0);
					return osc_bndl_empty;
				}else{
					return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(aa)));
				}
			}else{
				//osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("item is not indexable", 0), 0);
				return osc_bndl_empty;
			}
		}else{
			if(nn >= 0 && nn <= osc_msg_length(listm)){
				return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_retain(osc_msg_nth(listm, nn))));
			}else{
				//osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index out of range", 0), 0);
				return osc_bndl_empty;
			}
		}
	}else{
		//osc_bndl_appendStatus_m((t_osc_bndl_m *)out, osc_atom_false, osc_atom_allocString("index must be an integer", 0), 0);
		return osc_bndl_empty;
	}
}

t_osc_bndl *osc_builtin_math_lookup(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *lhs = osc_bndl_lookup(b, osc_atom_allocSymbol("/lhs", 0), osc_atom_match);
	t_osc_msg *rhs = osc_bndl_lookup(b, osc_atom_allocSymbol("/rhs", 0), osc_atom_match);
	if(!lhs || !rhs || osc_msg_length(lhs) == 0 || osc_msg_length(rhs) == 0){
		return osc_bndl_empty;
	}
	t_osc_atom *lhsa = osc_msg_nth(lhs, 1);
	t_osc_atom *rhsa = osc_msg_nth(rhs, 1);
	if(osc_atom_getTypetag(lhsa) != OSC_TT_BNDL){
		return osc_bndl_empty;
	}
	if(osc_atom_getTypetag(rhsa) != OSC_TT_SYM){
		// should try to eval, convert string, etc...
		return osc_bndl_empty;
	}
	t_osc_atom *out = osc_atom_evalNonstrict(rhsa, osc_atom_getBndlPtr(lhsa));
	if(osc_atom_getTypetag(out) != OSC_TT_BNDL){
		return osc_bndl_empty;
	}else{
		t_osc_bndl *b = osc_bndl_retain(osc_atom_getBndlPtr(out));
		osc_atom_release(out);
		return b;
	}
}

t_osc_bndl *osc_builtin_math_address(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *m = osc_bndl_lookup(b, osc_atom_allocSymbol("/string", 0), osc_atom_match);
	if(!m || osc_msg_length(m) == 0){
		return osc_bndl_empty;
	}
	t_osc_atom *a = osc_msg_nth(m, 1);
	t_osc_atom *aa = osc_atom_convertAny(a, OSC_TT_SYM);
	return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, aa));
}

t_osc_bndl *osc_builtin_math_function(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *bndlm = osc_bndl_lookup(b, osc_atom_allocSymbol("/bndl", 0), osc_atom_match);
	if(!bndlm || osc_msg_length(bndlm) == 0){
		return osc_bndl_empty;
	}
	t_osc_atom *bndla = osc_msg_nth(bndlm, 1);
	if(osc_atom_getTypetag(bndla) != OSC_TT_BNDL){
		return osc_bndl_empty;
	}
	return osc_bndl_append(osc_atom_getBndlPtr(bndla), osc_msg_alloc(osc_atom_typeaddress, 1, osc_atom_functype));
}

t_osc_bndl *osc_builtin_math_map(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *fnm = osc_bndl_lookup(b, osc_atom_allocSymbol("/fn", 0), osc_atom_match);
	t_osc_msg *argsm = osc_bndl_lookup(b, osc_atom_allocSymbol("/args", 0), osc_atom_match);
	if(!fnm || osc_msg_length(fnm) == 0 || !argsm || osc_msg_length(argsm) == 0){
		return osc_bndl_empty;
	}
	// need to check type of function and args
	t_osc_atom *fn = osc_msg_nth(fnm, 1);
	t_osc_bndl *argsthing = osc_bndl_thing(osc_atom_getBndlPtr(osc_msg_nth(argsm, 1)));
	t_osc_msg *v = osc_bndl_lookup(argsthing, osc_atom_valueaddress, osc_atom_match);
	t_osc_msg *out = osc_msg_alloc(osc_atom_valueaddress, 0);
	for(int i = 1; i < osc_msg_length(v) + 1; i++){
		t_osc_atom *a = osc_msg_nth(v, i);
		if(a && osc_atom_getTypetag(a) == OSC_TT_BNDL){
			t_osc_atom *expr = osc_atom_allocBndl(osc_parse_allocFuncall(fn, a), 1);
			t_osc_atom *e = osc_atom_evalNonstrict(expr, context);
			if(osc_atom_getTypetag(e) == OSC_TT_BNDL){
				t_osc_msg *vm = osc_bndl_lookup(osc_atom_getBndlPtr(e), osc_atom_valueaddress, osc_atom_match);
				if(vm){
					for(int j = 1; j < osc_msg_length(vm) + 1; j++){
						osc_msg_append_m((t_osc_msg_m *)out, osc_atom_retain(osc_msg_nth(vm, j)));
					}
					osc_atom_release(e);
				}else{
					osc_msg_append_m((t_osc_msg_m *)out, e);
				}
			}else{
				osc_msg_append_m((t_osc_msg_m *)out, e);
			}
		}
	}
	//t_osc_msg *_out = osc_msg_format(out, 0, NULL, 0, NULL, 0);
	//printf("%s\n", osc_msg_getPrettyPtr(_out));
	return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, out);
}

t_osc_bndl *osc_builtin_math_if(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *testm = osc_bndl_lookup(b, osc_atom_allocSymbol("/test", 0), osc_atom_match);
	if(!testm || osc_msg_length(testm) == 0){
		return osc_bndl_empty;
	}
	t_osc_atom *testa = osc_msg_nth(testm, 1);
	if(!(OSC_TYPETAG_ISBOOL(osc_atom_getTypetag(testa)))){
		return osc_bndl_empty;
	}
	t_osc_msg *m = NULL;
	if(osc_atom_true == testa){
		m = osc_bndl_lookup(b, osc_atom_allocSymbol("/then", 0), osc_atom_match);
	}else{
		m = osc_bndl_lookup(b, osc_atom_allocSymbol("/else", 0), osc_atom_match);
	}
	if(!m){
		return osc_bndl_empty;
	}
	t_osc_msg *v = osc_msg_alloc(osc_atom_valueaddress, 0);
	for(int i = 1; i < osc_msg_length(m) + 1; i++){
		osc_msg_append_m((t_osc_msg_m *)v, osc_atom_evalNonstrict(osc_msg_nth(m, i), context));
	}
	return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, v);
}

struct osc_builtin_funcrec
{
	char *name;
	t_osc_builtin func;
} osc_builtin_funcrec [] = {
	{"add", osc_builtin_math_add},
	{"eql", osc_builtin_math_eql},
	{"eqv", osc_builtin_math_eqv},
	{"nth", osc_builtin_math_nth},
	{"lookup", osc_builtin_math_lookup},
	{"address", osc_builtin_math_address},
	{"function", osc_builtin_math_function},
	{"map", osc_builtin_math_map},
	{"if", osc_builtin_math_if},
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

