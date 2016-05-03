#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "osc.h"
#include "osc_bundle.h"
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
		/expr : (@/lhs!@2 !@! @/rhs!@2)!@2,\
		/conditions/pre,\
		/conditions/post,\
		/return : {/value}\
	},\
	@+ : {\
		/lambda,\
		/doc : \"add\",\
		/args : {/lhs, /rhs},\
		/expr : (@/add!@2 !@ {@/lhs, @/rhs})!@/y!@2,\
		/conditions/pre,\
		/conditions/post\
	},\
	@map : {\
		/lambda,\
		/doc : \"map\",\
		/args : {/lhs, /rhs},\
		/expr : @/_map!@2 !@! {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	@filter : {\
		/lambda,\
		/doc : \"filter\",\
		/args : {/lhs, /rhs},\
		/expr : @/_filter!@2 !@ {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	@lreduce : {\
		/lambda,\
		/doc : \"lreduce\",\
		/args : {/lhs, /rhs},\
		/expr : @/_lreduce!@2 !@ {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/result}\
	},\
	/eval : {\
		/lambda,\
		/doc : \"doc\",\
		/args : {/expression},\
		/expr : @/_eval!@2 !@ {@/foo},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/foo}\
	},\
	/add : {\
		/lambda,\
		/doc : \"doc\",\
		/args : {/lhs, /rhs},\
		/expr : @/_add!@2 !@! {@/lhs, @/rhs},\
		/conditions/pre,\
		/conditions/post,\
		/return : {/y}\
	}\
}";

t_osc_bndl std;

t_osc_bndl osc_builtin_eval(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	//printf("**************************************************\n%s\n", __func__);
	if(b){
		//printf("%s: b:\n%s\n", __func__, osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, b))));
		//printf("%s: context:\n%s\n", __func__, osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, context))));
		//printf("%s: b:\n%s\n", __func__, osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_bndl_eval(r, b, context)))));
		//printf("%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_bndl_eval(r, b, context)))));
	}
	//printf("**************************************************\n");
	return osc_bndl_eval(r, b, context);
}

t_osc_bndl osc_builtin_add(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	if(b){
		t_osc_bndl args = osc_bndl_eval(r, b, context);
		if(args){
			t_osc_msg ll = osc_capi_bndl_simpleLookup(r, args, "/lhs");
			t_osc_msg rr = osc_capi_bndl_simpleLookup(r, args, "/rhs");
			if(ll && rr && osc_capi_msg_length(ll) > 0 && osc_capi_msg_length(rr) > 0){
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

t_osc_bndl osc_builtin_map(t_osc_region r, t_osc_bndl b, t_osc_bndl context)
{
	printf("%s\n", __func__);
	osc_capi_bndl_println(r, b);
	printf("in context, /lhs is\n");
	osc_capi_msg_println(r, osc_capi_bndl_simpleLookup(r, context, "/lhs"));
	printf("and /rhs is\n");
	osc_capi_msg_println(r, osc_capi_bndl_simpleLookup(r, context, "/rhs"));
	return osc_bndl_create(r, OSC_TIMETAG_NULL);
}

t_osc_bndl osc_builtin_std(t_osc_region r)
{
	if(std){
		return std;
	}
	t_osc_msg map = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_map"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_map, "map"));
	t_osc_msg eval = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_eval"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_eval, "eval"));
	t_osc_msg add = osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/_add"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, osc_builtin_add, "add"));
	t_osc_bndl fns = osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 3, eval, add, map);
	return std = osc_bndl_union(r, osc_parse(r, _std), fns);
}
