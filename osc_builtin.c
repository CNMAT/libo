#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "osc.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_builtin.h"
#include "osc_parse.h"
#include "osc_typetag.h"
#include "osc_util.h"
#include "osc_region.h"
#include "osc_cvalue.h"
#include "osc_primitive.h"


/*
context should have a bundle bound to /parent
need osc_bndl_evalFully and osc_bndl_eval
region should have a temp region that works from the end of the block
implement copy for bundles
eval should take a bundle with /expr in it and <---this isn't quite clear...
osc->odot conversion function
timetag in bundles as 0th element
 */
char *_std = "{\
	@` : {\
		/lambda,\
		/doc : \"doc\",\
		/args : {/lhs, /rhs},\
		/expr : @(@/lhs!@2),\
		/conditions/pre,\
		/conditions/post,\
		/return : {/value}\
	},\
	@+ : {\
		/lambda,\
		/name : @+,\
		/doc : \"add\",\
		/args : {/lhs, /rhs},\
		/expr : @/rest!@2 !@ {/list : ((@/add!@2 !@! {@/lhs, @/rhs})!@/y)},\
		/conditions/pre,\
		/conditions/post\
	},\
	@= : {\
		/lambda,\
		/name : @+,\
		/doc : \"test for equality\",\
		/args : {/lhs, /rhs},\
		/expr : {/value : @/rest!@2 !@ {/list : ((@/eql!@2 !@! {@/lhs, @/rhs})!@/y)}},\
		/conditions/pre,\
		/conditions/post\
	},\
	/map : {\
		/lambda,\
		/name : /map,\
		/doc : \"map\",\
		/args : {/fn, /args},\
		/expr : @/_map!@2 !@! {@/fn, @/args},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	/lreduce : {\
		/lambda,\
		/name : /lreduce,\
		/doc : \"lreduce\",\
		/args : {/fn, /args},\
		/expr : @/_lreduce!@2 !@! {@/fn, @/args},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	/eval : {\
		/lambda,\
		/name : /eval,\
		/doc : \"doc\",\
		/args : {/expression},\
		/expr : @/_eval!@2 !@! {@/expression},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	/rest : {\
		/lambda,\
		/name : /rest,\
		/doc : \"doc\",\
		/args : {/list},\
		/expr : @/_rest!@2 !@! {@/list},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/value}\
	},\
	/add : {\
		/lambda,\
		/name : /add,\
		/doc : \"doc\",\
		/args : {/lhs, /rhs},\
		/expr : @/_add!@2 !@! {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/y}\
	},\
	/eql : {\
		/lambda,\
		/name : /eql,\
		/doc : \"doc\",\
		/args : {/lhs, /rhs},\
		/expr : @/_eql!@2 !@! {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/y}\
	},\
	/add_ : {\
		/lambda,\
		/name : /add,\
		/doc : \"doc\",\
		/args : {/lhs, /rhs},\
		/expr : {/value : @/rest!@2 !@! {/list : (@/_add!@2 !@! {@/lhs, @/rhs})!@/y}},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/y}\
	},\
	/length : {\
		/lambda,\
		/name : /length,\
		/doc : \"doc\",\
		/args : {/list},\
		/expr : @/lreduce!@2 !@! {/fn : {/lambda, /expr : {/value : @/rest!@2 !@! {/list : (@/add!@2 !@! {@/lhs, @/rhs})!@/y}}}, /args : (1 @! @/list)},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/y}\
	},\
	/repeat : {\
		/lambda,\
		/name : /repeat,\
		/doc : \"doc\",\
		/args : {/val},\
		/expr : {/val : [@/rest!@2 !@! {/list : @/val}, @/rest!@2 !@! {/list : (@/repeat!@2 !@! {@/val})!@/val}]},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/xxx}\
	}\
}";
///expr : (@/lreduce!@2 !@ {/fn : {/lambda, /expr : @/rest!@2 !@ {/list : @/add!@2}}, /args : {/list : @/map!@2 !@ {/fn : @/1!@2, /args : {/list : @/a}}}})!@/y!@2,
t_osc_bndl std;

t_osc_bndl osc_builtin_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	if(b){
		t_osc_bndl e = osc_bndl_eval(r, b, context);
		return e;
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

t_osc_bndl osc_builtin_add(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	if(b){
		t_osc_bndl args = osc_bndl_eval(r, b, context);
		if(args){
			t_osc_msg ll = osc_capi_bndl_simpleLookup(r, args, "/lhs");
			t_osc_msg rr = osc_capi_bndl_simpleLookup(r, args, "/rhs");
			if(ll && rr && osc_capi_msg_length(r, ll) > 0 && osc_capi_msg_length(r, rr) > 0){
				t_osc_bndl lll = osc_capi_msg_nth(r, ll, 1);
				t_osc_bndl rrr = osc_capi_msg_nth(r, rr, 1);
				if(osc_capi_primitive_q(r, lll) && osc_capi_primitive_q(r, rrr)){
					char lt = osc_capi_primitive_getType(r, lll);
					char rt = osc_capi_primitive_getType(r, rrr);
					if(OSC_TYPETAG_ISNUMERIC(lt) && OSC_TYPETAG_ISNUMERIC(rt)){
						if(lt == rt){
							return osc_bndl_union(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/y"), osc_primitive_add(r, lll, rrr))), args);
						}else if(osc_typetag_isSubtype(lt, rt)){
							return osc_bndl_union(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/y"), osc_primitive_add(r, osc_capi_primitive_convert(r, lll, rt), rrr))), args);
						}else if(osc_typetag_isSubtype(rt, lt)){
							return osc_bndl_union(r, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/y"), osc_primitive_add(r, lll, osc_capi_primitive_convert(r, rrr, lt)))), args);
						}else{
						}
					}else{
					}
				}
			}
		}
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

t_osc_bndl osc_builtin_eql(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	if(b){
		t_osc_bndl args = osc_bndl_eval(r, b, context);
		if(args){
			t_osc_msg ll = osc_capi_bndl_simpleLookup(r, args, "/lhs");
			t_osc_msg rr = osc_capi_bndl_simpleLookup(r, args, "/rhs");
			if(ll && rr && osc_capi_msg_length(r, ll) > 0 && osc_capi_msg_length(r, rr) > 0){
				t_osc_bndl lll = osc_capi_msg_nth(r, ll, 1);
				t_osc_bndl rrr = osc_capi_msg_nth(r, rr, 1);
				t_osc_bndl out = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/y"), osc_bndl_eql(r, lll, rrr)));
				return out;
			}
		}
	}
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

t_osc_bndl osc_builtin_rest(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	t_osc_msg list = osc_capi_bndl_simpleLookup(r, b, "/list");
	if(list && osc_capi_msg_length(r, list) > 0){
		return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_prepend(r, osc_msg_rest(r, osc_msg_rest(r, list)), osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/value")));
	}
	return osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0);
}

t_osc_bndl osc_builtin_map(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	t_osc_bndl out = osc_bndl_map(r, osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, b, "/fn"), 1), osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, b, "/args"), 1), context);
	return out;
}

t_osc_bndl osc_builtin_lreduce(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	//t_osc_bndl out = osc_bndl_lreduce(r, osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, b, "/fn"), 1), osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, b, "/args"), 1), context);
	t_osc_bndl out = osc_bndl_lreduce(r, osc_capi_msg_nth(r, osc_capi_bndl_simpleLookup(r, b, "/fn"), 1), osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_bndl_simpleLookup(r, b, "/args")), context);
	return out;
}

t_osc_bndl osc_builtin_println(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	osc_capi_bndl_println(r, b);
	return b;
}

t_osc_bndl osc_builtin_std(t_osc_region r)
{
	if(std){
		return std;
	}
	t_osc_msg map = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_map"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_map, "map"));
	t_osc_msg lreduce = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_lreduce"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_lreduce, "lreduce"));
	t_osc_msg eval = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_eval"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_eval, "eval"));
	t_osc_msg rest = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_rest"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_rest, "rest"));
	t_osc_msg add = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_add"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_add, "add"));
	t_osc_msg eql = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_eql"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_eql, "eql"));
	t_osc_msg println = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_println"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_println, "println"));
	t_osc_bndl fns = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 7, rest, eval, add, map, lreduce, eql, println);
	return std = osc_bndl_union(r, osc_parse(r, _std), fns);
}
