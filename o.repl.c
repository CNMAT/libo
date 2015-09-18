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

t_osc_bndl *print(t_osc_bndl *b, t_osc_bndl *context)
{
	t_osc_msg *stm = osc_bndl_lookup(b, osc_atom_allocSymbol("/__string", 0), osc_atom_match);
	if(!stm || osc_msg_length(stm) == 0){
		printf("\n");
		return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, osc_atom_emptystring));
	}
	t_osc_atom *a = osc_atom_format(osc_msg_nth(stm, 1), 0);
	printf("%s: %s\n", __func__, osc_atom_getPrettyPtr(a));
	return osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_valueaddress, 1, a));
}

char *_mylib =
	"{\
		print : {\
			/expr/type : 'f',\
			/doc : \"prints to the console\", \
			/func : _print, \
			/out : /func {/__string : /string}, \
			/return : /out\
		}\
	}";

int main(int av, char **ac)
{
	char *st = ac[1];
	t_osc_bndl *math = osc_parse(osc_builtin_math);
	//t_osc_bndl *mathe = osc_bndl_evalStrict(math, NULL);
	t_osc_bndl *b = osc_parse(st);

	printf("PARSED:\n");
	t_osc_atom *bf = osc_bndl_format(b, 0);
	printf("%s\n", osc_atom_getPrettyPtr(bf));

	printf("EVALUATING\n");
	t_osc_bndl *bb = osc_bndl_evalNonstrict(b, math);
	
	printf("EVALUATED:\n");
	t_osc_atom *bbf = osc_bndl_format(bb, 0);
	printf("%s\n", osc_atom_getPrettyPtr(bbf));

	t_osc_bndl *mylib = osc_parse(_mylib);
	t_osc_bndl *funcs = osc_bndl_alloc(OSC_TIMETAG_NULL, 1, osc_msg_alloc(osc_atom_allocSymbol("_print", 0), 1, osc_atom_allocNative(print, "_print")));
	t_osc_bndl *uu = osc_bndl_union(mylib, funcs);
	//char *_prog = "{/a : \"some foo\", /b : print { /string : /a } }";
	char *_prog =
		"{\
			/a : 10,\n \
			/e : /a,\n \
			/b : print { /string : /f },\n \
			/f : /e,\n \
			/c : print { /string : /b },\n \
			/h : \"foo\",\n \
			/i : print { /string : /h }\n \
		}";
	t_osc_bndl *prog = osc_parse(_prog);
	t_osc_atom *_p = osc_bndl_format(prog, 0);
	printf("**************************************************\n");
	printf("%s\n%s\n", _prog, osc_atom_getPrettyPtr(_p));
	t_osc_bndl *eprog = osc_bndl_evalNonstrict(prog, uu);
	t_osc_atom *_eprog = osc_bndl_format(eprog, 0);
	printf("%s\n", osc_atom_getPrettyPtr(_eprog));

	return 0;
}
