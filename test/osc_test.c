#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_bundle_u.h"
#include "osc_atom_array_u.h"
#include "osc_strfmt.h"
#include "osc_parser.h"
#define __OSC_PROFILE__
#include "osc_profile.h"

#include "osc_expr.h"
#include "osc_expr_rec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_expr_funcall.h"
#include "osc_expr_ast_expr_value.h"
#include "osc_expr_ast_expr_list.h"
#include "osc_expr_ast_expr_oscaddress.h"
#include "osc_expr_ast_expr_function.h"

int main(int len, char **argv)
{
	t_osc_atom_ar_u *ar = osc_atom_array_u_alloc(3);
	for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
		osc_atom_u_setInt32(osc_atom_array_u_get(ar, i), i);
	}
	t_osc_expr_ast_expr *fex = osc_expr_ast_expr_funcall_alloc(osc_expr_lookupFunction("add"), 2, osc_expr_ast_expr_value_alloc(osc_atom_u_allocString("a")), osc_expr_ast_expr_value_alloc(osc_atom_u_allocString("b")));
	t_osc_expr_ast_expr_function *f = osc_expr_ast_expr_function_alloc(2, (char *[]){"a", "b"}, fex);
	t_osc_expr_ast_expr_funcall *e = osc_expr_ast_expr_funcall_alloc(osc_expr_lookupFunction("apply"), 3, f, osc_expr_ast_expr_value_alloc(osc_atom_u_allocInt32(10)), osc_expr_ast_expr_value_alloc(osc_atom_u_allocInt32(20)));


	long l = osc_expr_ast_expr_format(NULL, 0, (t_osc_expr_ast_expr *)e);
	char buf[l + 1];
	osc_expr_ast_expr_format(buf, l + 1, (t_osc_expr_ast_expr *)e);
	printf("%s\n", buf);
}
