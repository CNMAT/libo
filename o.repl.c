#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osc.h"
#include "osc_mem.h"
#include "osc_parse.h"
#include "osc_bundle.h"
#include "osc_message.h"
#include "osc_atom.h"
#include "osc_timetag.h"
#include "osc_builtin.h"

t_osc_bndl *foo(t_osc_bndl *b)
{
	return osc_bndl_append(b, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_allocString("hello world", 0)));
}

t_osc_bndl *bar(t_osc_bndl *b)
{
	t_osc_atom *lhsa = osc_atom_allocSymbol("/_l", 0);
	t_osc_atom *rhsa = osc_atom_allocSymbol("/_r", 0);
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
	osc_bndl_append_m((t_osc_bndl_m *)out, osc_msg_alloc(osc_atom_valueaddress, 0));
	return out;
}

char *_mylib = "{foo : {/doc : \"prints hello world\", /args, /func : natfoo, /out : /func {}, /value : /out}, bar : {/doc : \"adds numbers\", /args : [\"/l\", \"/r\"], /func : natbar, /y : /func {/_l : /l, /_r : /r}, /value : /y}}";

int main(int av, char **ac)
{

	char *st = ac[1];
	t_osc_bndl *math = osc_parse(osc_builtin_math);
	t_osc_bndl *b = osc_parse(st);

	printf("PARSED:\n");
	t_osc_bndl *bf = osc_bndl_format(b, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bf));

	printf("EVALUATING\n");
	t_osc_bndl *bb = osc_bndl_eval(b, math);
	
	printf("EVAL'D:\n");
	t_osc_bndl *bbf = osc_bndl_format(bb, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bbf));

	bb = osc_bndl_clearAllStatus(bb);
	t_osc_bndl *i = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_allocSymbol("/i", 0), 1, osc_atom_allocInt32(33)));
	t_osc_bndl *u = osc_bndl_union(i, bb);
	t_osc_bndl *bbb = osc_bndl_eval(u, math);

	printf("again:\n");
	t_osc_bndl *bbbf = osc_bndl_format(bbb, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(bbbf));


	t_osc_bndl *mylib = osc_parse(_mylib);
	t_osc_bndl *funcs = osc_bndl_alloc(OSC_TIMETAG_NULL, 2, osc_msg_alloc(osc_atom_allocSymbol("natfoo", 0), 1, osc_atom_allocNative(foo, "natfoo")), osc_msg_alloc(osc_atom_allocSymbol("natbar", 0), 1, osc_atom_allocNative(bar, "natbar")));
	t_osc_bndl *uu = osc_bndl_union(mylib, funcs);
	char *_prog = "{/b : bar {/l : 10, /r : 20}}";
	t_osc_bndl *prog = osc_parse(_prog);
	t_osc_bndl *eprog = osc_bndl_eval(prog, uu);
	t_osc_bndl *_eprog = osc_bndl_format(eprog, 0);
	printf("%s\n", osc_bndl_getPrettyPtr(_eprog));
	
	return 0;
}

int _main(int av, char **ac)
{
	int buflen = 65536;
	char *buf = osc_mem_alloc(buflen);
	int n = 0;
	t_osc_bndl *g = NULL;
	t_osc_bndl *math = osc_parse(osc_builtin_math);
	while(1){
		printf("> ");
		while((buf[n++] = getchar()) != '\n'){
			if(n == buflen){
				buf = osc_mem_resize(buf, buflen += 65536);
			}
		}
		if(n == 2){
			switch(buf[0]){
			case OSC_TT_I8:
				osc_bndl_release(g);
				g = NULL;
				break;
			default:
				printf("I don't understand %c\n", buf[0]);
				break;
			}
			n = 0;
			continue;
		}
		t_osc_bndl *b = osc_parse(buf);
		if(!b){
			n = 0;
			continue;
		}
		osc_bndl_format_m((t_osc_bndl_m *)b, 0);
		printf("parsed bndl:\n%s\n", osc_bndl_getPrettyPtr(b));
		t_osc_bndl *w = NULL;
		if(g == NULL){
			w = osc_bndl_retain(b);
			//w = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_expraddress, 1, osc_atom_allocExpr(b, 1)));
		}else{
			w = osc_bndl_union(b, g);
		}
		//t_osc_bndl *ww = w;
		//w = osc_bndl_union(w, math);
		//osc_bndl_release(ww);
		osc_bndl_format_m((t_osc_bndl_m *)w, 0);
		printf("working bndl:\n%s\n", osc_bndl_getPrettyPtr(w));
		osc_bndl_release(g);
		t_osc_bndl *r = osc_bndl_eval(w, math);
		osc_bndl_format_m((t_osc_bndl_m *)r, 0);
		printf("eval'd bndl:\n%s\n", osc_bndl_getPrettyPtr(r));
		//t_osc_bndl *rr = osc_bndl_reduce(r, math);
		//osc_bndl_format_m((t_osc_bndl_m *)rr, 0);
		//printf("reduced bndl:\n%s\n", osc_bndl_getPrettyPtr(rr));
		g = r;
		osc_bndl_release(w);
		n = 0;
	}
	return 0;
}
