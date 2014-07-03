#include "osc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osc_match.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_bundle_s.h"
#include "osc_message_s.h"
#include "osc_atom_u.h"
#include "osc_parser.h"

#include "osc_expr_ast_expr.h"
#include "osc_expr_parser.h"

int main(int argc, char **argv)
{
	char *exprstrs[] = {
		"aseq(1, 10, 1)",
		"apply(aseq, 1, 10, 1)",
		"apply(/func, 1, 10, 1)",
		"apply(lambda(a){aseq(a, 10, 1)}, 1)",
		"/aseq(1, 10, 1)",
		"suckit(1, 2, 3)",
		"apply(lambda(aseq){apply(aseq, 1, 2, 3)}, aseq)",
		"apply(lambda(aseq){apply(aseq, 1, 2)}, lambda(a, b){a + b})",
	};
	for(int i = 0; i < sizeof(exprstrs) / sizeof(char*); i++){
		printf("**************************************************\n");
		char *exprstr = exprstrs[i];
		printf("EXPR: %s\n", exprstr);
		t_osc_expr_ast_expr *ast = NULL;
		osc_expr_parser_parseExpr(exprstr, &ast);
		if(!ast){
			printf("nope\n");
			return 0;
		}
		/*
		long len = OSC_HEADER_SIZE;
		char *bndl = osc_mem_alloc(len);
		memcpy(bndl, OSC_EMPTY_HEADER, len);
		*/
		char *bndlstr = "/func aseq\n/aseq `lambda(a, b, c){aseq(a, b, c)}`\n";
		t_osc_bndl_u *bndlu = NULL;
		osc_parser_parseString(strlen(bndlstr), bndlstr, &bndlu);
		long len = 0;
		char *bndl = NULL;
		osc_bundle_u_serialize(bndlu, &len, &bndl);
		t_osc_atom_ar_u *out = NULL;
		osc_expr_ast_expr_eval(ast, &len, &bndl, &out);
		printf("result: ");
		for(int i = 0; i < osc_atom_array_u_getLen(out); i++){
			t_osc_atom_u *a = osc_atom_array_u_get(out, i);
			long len = osc_atom_u_nformat(NULL, 0, a, 0);
			char buf[len + 1];
			osc_atom_u_nformat(buf, len + 1, a, 0);
			printf("%s ", buf);
		}
		printf("\n");
		osc_expr_ast_expr_free(ast);
		osc_bundle_u_free(bndlu);
		osc_mem_free(bndl);
		osc_atom_array_u_free(out);
	}
	/*
	while(1){
		sleep(1);
	}
	*/
	return 0;
}

