/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2013-14, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <string.h>
#include "osc_expr_builtins.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_oprec.h"
#include "osc_expr_funcrec.r"
#include "osc_expr_oprec.r"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_privatedecls.h"

int osc_expr_builtins_add(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

// built-in operators
static t_osc_expr_oprec osc_expr_builtins_op_add = {
	"+",
	2,
	(char *[]){"left operand", "right operand"},
	NULL,
	1,
	(char *[]){"result"},
	NULL,
	"Returns the sum of its arguments",
	-1,
	6,
	0,
	'+'
};

// built-in functions
static t_osc_expr_funcrec osc_expr_builtins_func_add = {
	"add",
	2,
	(char *[]){"a", "b"},
	NULL,
	0,
	1,
	(char *[]){"y"},
	NULL,
	"Returns the sum of its arguments",
	osc_expr_builtins_add,
	(unsigned int []){0, 1},
	0,
	NULL,
	0
};

static t_osc_expr_funcrec *osc_expr_builtins_fsymtab[] = {
	&osc_expr_builtins_func_add,
};

static t_osc_expr_oprec *osc_expr_builtins_osymtab[] = {
	&osc_expr_builtins_op_add,
};

// this array can be indexed by either the ascii char of an operator,
// or in the case of multi character operators such as <=, by the 
// token defined in osc_expr_parser.y
static t_osc_expr_oprec *osc_expr_builtins_opcodeToOpRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtins_op_eq	,
	NULL, //&osc_expr_builtins_op_neq	,
	NULL, //&osc_expr_builtins_op_le	,
	NULL, //&osc_expr_builtins_op_ge	,
	NULL, //&osc_expr_builtins_op_nullcoalesce,
	NULL, //&osc_expr_builtins_op_add1,
	NULL, //&osc_expr_builtins_op_sub1,
	NULL, //&osc_expr_builtins_op_addassign,
	NULL, //&osc_expr_builtins_op_subassign,
	NULL, //&osc_expr_builtins_op_mulassign, // 10
	NULL, //&osc_expr_builtins_op_divassign, 
	NULL, //&osc_expr_builtins_op_modassign,
	NULL, //&osc_expr_builtins_op_powassign,
	NULL, //&osc_expr_builtins_op_nullcoalesceassign,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_op_not	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_op_mod	,
	NULL, //&osc_expr_builtins_op_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	NULL, //&osc_expr_builtins_op_mul	,
	&osc_expr_builtins_op_add	,
	NULL	,
	NULL, //&osc_expr_builtins_op_sub	,
	NULL, //&osc_expr_builtins_op_lookup,
	NULL, //&osc_expr_builtins_op_div	,
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_op_lt	,// 60
	NULL, //&osc_expr_builtins_op_assign	,
	NULL, //&osc_expr_builtins_op_gt	,
	NULL,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_op_pow	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
		NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_op_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static t_osc_expr_funcrec *osc_expr_builtins_opcodeToFuncRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtins_func_eq	,
	NULL, //&osc_expr_builtins_func_neq	,
	NULL, //&osc_expr_builtins_func_le	,
	NULL, //&osc_expr_builtins_func_ge	,
	NULL    ,
	NULL    ,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 10
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_not	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_mod	,
	NULL, //&osc_expr_builtins_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	NULL, //&osc_expr_builtins_func_mul	,
	&osc_expr_builtins_func_add	,
	NULL	,
	NULL, //&osc_expr_builtins_func_sub	,
	NULL, //&osc_expr_builtins_func_lookup,
	NULL, //&osc_expr_builtins_func_div	,
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_lt	, // 60
	NULL, //&osc_expr_builtins_func_assign	,
	NULL, //&osc_expr_builtins_func_gt	,
	NULL, //&osc_expr_builtins_func_nullcoalesce,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_pow	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static char *osc_expr_builtins_opcodeToString[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtins_func_eq	,
	NULL, //&osc_expr_builtins_func_neq	,
	NULL, //&osc_expr_builtins_func_le	,
	NULL, //&osc_expr_builtins_func_ge	,
	NULL    ,
	NULL    ,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 10
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 20
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 30
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_not	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_mod	,
	NULL, //&osc_expr_builtins_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	NULL, //&osc_expr_builtins_func_mul	,
	"+"	,
	NULL	,
	NULL, //&osc_expr_builtins_func_sub	,
	NULL, //&osc_expr_builtins_func_lookup,
	NULL, //&osc_expr_builtins_func_div	,
	NULL	,
	NULL	,
	NULL	,// 50
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_lt	, // 60
	NULL, //&osc_expr_builtins_func_assign	,
	NULL, //&osc_expr_builtins_func_gt	,
	NULL, //&osc_expr_builtins_func_nullcoalesce,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 70
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 80
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 90
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_pow	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 100
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 110
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,// 120
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtins_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

t_osc_expr_funcrec *osc_expr_builtins_lookupFunction(char *name)
{
	const int n = sizeof(osc_expr_builtins_fsymtab) / sizeof(t_osc_expr_funcrec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(name, osc_expr_funcrec_getName(osc_expr_builtins_fsymtab[i]))){
			return osc_expr_builtins_fsymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtins_lookupOperator(char *op)
{
	const int n = sizeof(osc_expr_builtins_osymtab) / sizeof(t_osc_expr_oprec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(op, osc_expr_oprec_getName(osc_expr_builtins_osymtab[i]))){
			return osc_expr_builtins_osymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtins_lookupOperatorForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtins_opcodeToFuncRec) / sizeof(char *)){
		char *str = osc_expr_builtins_opcodeToString[(int)op];
		return osc_expr_builtins_lookupOperator(str);
	}
	return NULL;
}

t_osc_expr_funcrec *osc_expr_builtins_lookupFunctionForOperator(t_osc_expr_oprec *op)
{
	char bytecode = osc_expr_oprec_getBytecode(op);
	return osc_expr_builtins_lookupFunctionForOpcode(bytecode);
}

t_osc_expr_funcrec *osc_expr_builtins_lookupFunctionForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtins_opcodeToFuncRec) / sizeof(t_osc_expr_funcrec*)){
		return osc_expr_builtins_opcodeToFuncRec[(int)op];
	}
	return NULL;
}



//////////////////////////////////////////////////
// built in c functions
//////////////////////////////////////////////////

int osc_expr_builtins_add(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	printf("hells yep\n");
	return 0;
}

OSC_EXPR_BUILTIN_DECL(apply)
{return 0;}
OSC_EXPR_BUILTIN_DECL(map)
{return 0;}
OSC_EXPR_BUILTIN_DECL(lreduce)
{return 0;}
OSC_EXPR_BUILTIN_DECL(rreduce)
{return 0;}
OSC_EXPR_BUILTIN_DECL(assign)
{return 0;}
OSC_EXPR_BUILTIN_DECL(assigntoindex)
{return 0;}
OSC_EXPR_BUILTIN_DECL(if)
{return 0;}
OSC_EXPR_BUILTIN_DECL(emptybundle)
{return 0;}
OSC_EXPR_BUILTIN_DECL(bound)
{return 0;}
OSC_EXPR_BUILTIN_DECL(exists)
{return 0;}
OSC_EXPR_BUILTIN_DECL(getaddresses)
{return 0;}
OSC_EXPR_BUILTIN_DECL(delete)
{return 0;}
OSC_EXPR_BUILTIN_DECL(getmsgcount)
{return 0;}
OSC_EXPR_BUILTIN_DECL(value)
{return 0;}
OSC_EXPR_BUILTIN_DECL(quote)
{return 0;}
OSC_EXPR_BUILTIN_DECL(eval)
{return 0;}
OSC_EXPR_BUILTIN_DECL(tokenize)
{return 0;}
OSC_EXPR_BUILTIN_DECL(gettimetag)
{return 0;}
OSC_EXPR_BUILTIN_DECL(settimetag)
{return 0;}
OSC_EXPR_BUILTIN_DECL(lookup)
{return 0;}

OSC_EXPR_SPECFUNC_DECL(apply)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(map)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(reduce)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(assign)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(assigntoindex)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(if)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(emptybundle)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(bound)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(exists)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(getaddresses)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(delete)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(getmsgcount)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(value)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(quote)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(eval)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(tokenize)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(gettimetag)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(settimetag)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(lookup)
{return 0;}
