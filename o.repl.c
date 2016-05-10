#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "osc.h"
#include "osc_typetag.h"
#define __OSC_PROFILE__
#include "osc_parse.h"
#include "osc_profile.h"
#include "osc_mem.h"
#include "osc_timetag.h"
#include "osc_capi.h"
#include "osc_capi_bundle.h"
#include "osc_capi_message.h"
#include "osc_byteorder.h"
#include "osc_list.h"
#include "osc_native.h"
#include "osc_capi_primitive.h"
#include "osc_primitive.h"
#include "osc_region.h"
#include "osc_cvalue.h"
#include "osc_bundle.h"


///f : $($/c$2)$2
int main(int av, char **ac)
{
	t_osc_region r = osc_region_alloc(100000000);
	/*		char *_prog = "{				\
			/sum : {/lambda, /expr : {/value : @/lreduce!@2 !@ {/fn : {/lambda, /expr : {/value : @/rest!@2 !@ {/list : ((@/add!@2 !@! {@/lhs, @/rhs})!@/y)}}}, /args : {@/list}}}},\
			/n : 3,\
			/a : [10, 1, 13],\
			/aa : [100, 10, 130],\
			/aaa : @/rest!@2 !@! {/list : @/aa},\
			/a1 : @/a@2,\
			/a2 : @/a!@2,\
			/a3 : @/a!@! (@/n!@2),\
			/b : (@/add!@2 !@ {/lhs : 3, /rhs : @/a3!@2}),\
			/c : @/map!@2 !@ {/fn : {/lambda, /expr : {/value : @/rest!@2 !@ {/list : ((@/add!@2 !@! {@/lhs, @/rhs})!@/y)}}}, /args : {/lhs : @/rest!@2 !@ {/list : @/a}, /rhs : @/rest!@2 !@ {/list : @/aa}}},\
			/ccc : @/lreduce!@2 !@ {/fn : {/lambda, /expr : {/value : @/rest!@2 !@ {/list : ((@/add!@2 !@! {@/lhs, @/rhs})!@/y)}}}, /args : {/list : [1, 2, 3]}},\
			/cc : @/lreduce!@2 !@ {/fn : {/lambda, /expr : @/add!@2!@/y!@2}, /args : {/list : @/map!@2 !@ {/fn : @/1!@2, /args : {/list : @/a}}}},\
			/cccc : @/sum!@2 !@ {/list : [1, 2, 3]},\
			/d : 3,\
			/e : 3 @+ 4,\
			/f : @/a,\
			/g : {/a : 1} @ {/b : @/a, /a : 4},\
			/h : @/length!@2 !@ {/list : [1, 2, 3]},\
			/i : @/length!@2 !@ {/list : @/f},\
			/j : 1 @ [1, 2, 3],\
			/k : @/length!@2 !@ (1 @ [1, 2, 3]),\
			/l : @/add!@2 !@ {/lhs : 3, /rhs : 5},\
			/m : @/add_!@2 !@! {/lhs : 3, /rhs : 5},\
			/p : @!(@/o!@2)\
}";
	*/
	/*
			char *_prog = "{				\
			/t : true @ {/then : 10} !@ {/else : 20},\
			/f : @/sub!@2 !@ {/lhs : 3, /rhs : 4},\
			/g : @/sub,\
			/r : @/lreduce!@2 !@! {/fn : {/lambda, /expr : {/value : (@/add!@2 !@! {/lhs : @/lhs!@2, /rhs : @/rhs!@2})}}, /args : {/list : 1 @ [1, 2, 3]}}\
}";
	*/
	/*
	char *_prog = "{				\
			/r : @/rest!@2 !@! {/list : @/lreduce!@2 !@! {/fn : {/lambda, /expr : {/value : (@/add!@2 !@! {@/lhs, @/rhs})!@/y!@2}}, /args : {/list : 1 @ [1, 2, 3]}}!@/result}\
}";
	*/
	/*
	char *_prog = "{\
			/a : (@/lreduce!@2 !@! {/fn : {/lambda, /expr : {/value : @/rest!@2 !@! {/list : (@/add!@2 !@! {@/lhs, @/rhs})!@/y}}}, /args : [1, 2, 3]})!@/result!@2,\
			/b : @/mul!@2 !@! {/lhs : (@/add!@2 !@! {/lhs : 3, /rhs : 5})!@/y!@2, /rhs : 7},\
			/c : (@/length!@2 !@ {/list : @/rest!@2 !@! {/list : @/lst}})!@/result!@2,\
			/lst : [1, 2, 3],\
			/t : @/rest!@2 !@! {/list : ((@/eql!@2 !@! {/lhs : @/rest!@2 !@! {/list : @/c}, /rhs : 5})!@/y!@2 !@ {/then : @/rest!@2 !@! {/list : @/lst}} !@ {/else : [4, 5, 6]})!@/result},\
			/add_ : {/lambda, /expr : {/value : @/rest!@2 !@! {/list : (@/add!@2 !@! {@/lhs, @/rhs})!@/y}}},\
			@` : {/lambda, /expr : {/value : @/rest!@2 !@! {/list : @/rest!@2 !@! {/list : (@/lhs !@! @/rhs)}}}},\
			/x : {/rest : [1, 2, 3]} @`/rest\
}";
	*/
	/*
/r : {/lambda, /lst, /expr : @/eql!@2 !@! {/lhs : 6, /rhs : ((@/length!@2 !@! {/list : @/rest!@2 !@! {/list : @/lst}})!@/result!@2)} !@ {/then : @/rest!@2 !@! {/list : @/lst}} !@ {/else : @/r!@2 !@! {/lst : [{/value : @/rest!@2 !@! {/list : @/lst}}, 10]}}},\
	*/
	char *_prog = "{\
			/r : {/lambda, /lst, /expr : {/value : (((@/eql!@2 !@! {/lhs : 10, /rhs : ((@/length!@2 !@! {/list : @/rest!@2 !@! {/list : @/lst}})!@/result!@2)})!@/y!@2) !@ {/then : @/rest!@2 !@! {/list : @/lst}} !@ {/else : @/rest!@2 !@! {/list : (@/r!@2 !@! {[@/lst, 10]})!@/result}})}},\
			/s : @/rest!@2 !@! {/list : (@/r!@2 !@ {/lst : [1, 2, 3]})!@/result}\
}";/*
			/t : true !@ {/then : 10} !@ {/else : 20}\
			}";*/

	//t_osc_region tmp = osc_region_getTmp(r);
	t_osc_bndl prog = osc_parse(r, _prog);
	//osc_capi_bndl_println(tmp, prog);
	//t_osc_bndl p2 = osc_bndl_copy(r, prog);
	//osc_capi_bndl_println(r, p2);

	//return 0;
	
	printf("parsed:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, prog))));
	//printf("std:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, osc_builtin_std(r)))));
	t_osc_bndl std = osc_builtin_std(r);
	t_osc_bndl eval = osc_bndl_eval(r, prog, std);
	printf("eval'd:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, eval))));
	//eval = osc_bndl_eval(r, eval, std);
	//printf("eval'd:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, osc_bndl_format(r, eval))));
	printf("%ld bytes used, %ld bytes free\n", osc_region_bytesUsed(r), osc_region_bytesFree(r));
	osc_region_delete(r);
	return 0;
}
/*
{
	t_osc_region r = osc_region_alloc(0);
	t_osc_bndl b = osc_capi_bndl_alloc(r,
					   osc_timetag_now(),
					   6,
					   osc_capi_msg_alloc(r,
							      5,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/foo"),
							      osc_capi_primitive_int32(r, osc_timetag_now(), 666),
							      osc_capi_primitive_float(r, osc_timetag_now(), 33.2),
							      osc_capi_primitive_string(r, osc_timetag_now(), "bar"),
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/jean")),
					   osc_capi_msg_alloc(r,
							      2,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/foo/bar"),
							      osc_capi_primitive_unit(r, osc_timetag_now(), OSC_TT_TRUE)),
					   osc_capi_msg_alloc(r,
							      2,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/bndl"),
							      osc_capi_bndl_alloc(r,
										  osc_timetag_now(),
										  1,
										  osc_capi_msg_alloc(r,
												     2,
												     osc_capi_primitive_symbol(r, osc_timetag_now(), "/flarp"),
												     osc_capi_primitive_symbol(r, osc_timetag_now(), "/x")))),
					   osc_capi_msg_alloc(r,
							      4,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/bndllist"),
							      osc_capi_bndl_alloc(r,
										  osc_timetag_now(),
										  1,
										  osc_capi_msg_alloc(r,
												     2,
												     osc_capi_primitive_symbol(r, osc_timetag_now(), "/x"),
												     osc_capi_primitive_int32(r, osc_timetag_now(), 500))),
							      osc_capi_bndl_alloc(r,
										  osc_timetag_now(),
										  1,
										  osc_capi_msg_alloc(r,
												     2,
												     osc_capi_primitive_symbol(r, osc_timetag_now(), "/y"),
												     osc_capi_primitive_int32(r, osc_timetag_now(), 600))),
							      osc_capi_bndl_alloc(r,
										  osc_timetag_now(),
										  1,
										  osc_capi_msg_alloc(r,
												     2,
												     osc_capi_primitive_symbol(r, osc_timetag_now(), "/z"),
												     osc_capi_primitive_int32(r, osc_timetag_now(), 700)))),
					   osc_capi_msg_alloc(r,
							      3,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/jean"),
							      osc_capi_primitive_int32(r, osc_timetag_now(), 666),
							      osc_capi_primitive_float(r, osc_timetag_now(), 33.2)),
					   osc_capi_msg_alloc(r,
							      2,
							      osc_capi_primitive_symbol(r, osc_timetag_now(), "/test"),
							      osc_primitive_eql(r,
										osc_capi_primitive_float(r, osc_timetag_now(), 66.6),
										osc_capi_primitive_float(r, osc_timetag_now(), 66.4))));
	osc_capi_bndl_append(r, b, osc_capi_msg_alloc(r, 1, osc_capi_primitive_symbol(r, osc_timetag_now(), "/suck")));
	int len = osc_capi_bndl_nformat(r, NULL, 0, b, 0);
	char buf[len + 1];
	osc_capi_bndl_nformat(r, buf, len + 1, b, 0);
	printf("%s\n", buf);
	t_osc_bndl nth = osc_bndl_nth(r, b, osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, 3));
	t_osc_bndl nthf = osc_bndl_format(r, nth);
	printf("%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, nthf)));

	t_osc_bndl match = osc_bndl_match(r, b, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "/foo/*"));
	t_osc_bndl matchf = osc_bndl_format(r, match);
	printf("matched:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, matchf)));

	t_osc_bndl bbb = osc_parse(r,
				   "{\
					/foo : 10, \
					/x : {/a : .1, /b : .2, /c : 0.2}, \
					/bar : /foo, \
					/barf : \"xxx\", \
					/expr : add @ {/lhs : 10} @ { /rhs : 20}\
				    }");
	printf("bbb = %d\n", osc_capi_msg_length(r, osc_capi_bndl_nth(r, bbb, 0)));
	len = osc_capi_bndl_nformat(r, NULL, 0, bbb, 0);
	char buf1[len + 1];
	osc_capi_bndl_nformat(r, buf1, len + 1, bbb, 0);
	printf("%s\n", buf1);

	t_osc_bndl p1 = osc_capi_primitive_int32(r, OSC_TIMETAG_NULL, 33);
	t_osc_bndl p2 = osc_capi_primitive_string(r, OSC_TIMETAG_NULL, "33");
	len = osc_capi_bndl_nformat(r, NULL, 0, p1, 0);
	char s1[len + 1];
	osc_capi_bndl_nformat(r, s1, len + 1, p1, 0);
	len = osc_capi_bndl_nformat(r, NULL, 0, p2, 0);
	char s2[len + 1];
	osc_capi_bndl_nformat(r, s2, len + 1, p2, 0);
	printf("%s %s %s\n", s1, osc_capi_primitive_eql(r, p1, p2) == 1 ? "=" : "!=", s2);
	printf("%s %s %s\n", s1, osc_capi_primitive_eqv(r, p1, p2) == 1 ? "~" : "!~", s2);

	t_osc_bndl u = osc_bndl_union(r, b, bbb);
	t_osc_bndl uf = osc_bndl_format(r, u);
	
	t_osc_bndl isect = osc_bndl_intersection(r, b, bbb);
	t_osc_bndl isectf = osc_bndl_format(r, isect);
	
	t_osc_bndl rc = osc_bndl_rcomplement(r, b, bbb);
	t_osc_bndl rcf = osc_bndl_format(r, rc);

	printf("union:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, uf)));
	printf("intersection:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, isectf)));
	printf("relative complement:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, rcf)));
	printf("\n\n**************************************************\n\n");

	printf("myfn %p\n", myfn);
	t_osc_bndl fn = osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, myfn, "myfn");
	t_osc_bndl fnf = osc_bndl_format(r, fn);
	printf("fn:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, fnf)));
	osc_bndl_apply(r, fn, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0), osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 0));

	t_osc_bndl ev = osc_bndl_eval(r, bbb, osc_capi_bndl_alloc(r, OSC_TIMETAG_NULL, 1, osc_capi_msg_alloc(r, 2, osc_capi_primitive_symbol(r, OSC_TIMETAG_NULL, "add"), osc_capi_primitive_fn(r, OSC_TIMETAG_NULL, myfn, "add"))));
	t_osc_bndl evf = osc_bndl_format(r, ev);
	printf("ev:\n%s\n", osc_cvalue_value(osc_capi_primitive_getPtr(r, evf)));
	printf("%ld bytes used, %ld bytes free\n", osc_region_bytesUsed(r), osc_region_bytesFree(r));
	osc_region_delete(r);
	while(1){
		sleep(1);
	}

	return 0;
}
*/
