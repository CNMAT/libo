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
#include "osc_expr_builtin.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_oprec.h"
#include "osc_expr_funcrec.r"
#include "osc_expr_oprec.r"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_arraysubscript.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_privatedecls.h" 
#include "osc_message_iterator_u.h"

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

int osc_expr_builtin_add(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_sub(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_mul(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_div(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_mod(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_pow(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_lt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_gt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_le(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_ge(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_nth(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_list(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

#define OSC_EXPR_BUILTIN_DEFOP(op, opcode, func, lhs, rhs, return, assoc, prec, docstring) \
t_osc_expr_oprec _osc_expr_builtin_op_##func = {\
	#op,\
	2,\
	(char *[]){#lhs, #rhs},			\
	NULL,\
	1,\
	(char *[]){#return},\
	NULL,\
	docstring,\
	assoc,\
	prec,\
	0,\
	opcode\
};\
static t_osc_expr_funcrec _osc_expr_builtin_func_##func = {\
	#func,\
	2,\
	(char *[]){#lhs, #rhs},			\
	NULL,\
	0,\
	1,\
	(char *[]){#return},\
	NULL,\
	docstring,\
	osc_expr_builtin_##func,\
	2,\
	(unsigned int []){0, 1},\
	0,\
	2,\
	(unsigned int []){0, 1},\
	0\
};

OSC_EXPR_BUILTIN_DEFOP(=, '=', assign, _lval, _rval, _rval, OSC_EXPR_PARSER_ASSOC_RIGHT, 16, "Assigns _rval to _lval and returns _rval");
OSC_EXPR_BUILTIN_DEFOP(<, '<', lt, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(>, '>', gt, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(<=, OSC_EXPR_LTE, le, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than or equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(>=, OSC_EXPR_GTE, ge, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than or equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(+, '+', add, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the sum of its arguments");
OSC_EXPR_BUILTIN_DEFOP(-, '-', sub, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the difference of its arguments");
OSC_EXPR_BUILTIN_DEFOP(*, '*', mul, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the product of its arguments");
OSC_EXPR_BUILTIN_DEFOP(/, '/', div, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the quotient of its arguments");
OSC_EXPR_BUILTIN_DEFOP(%, '%', mod, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the remainder of _a divided by _b");
OSC_EXPR_BUILTIN_DEFOP(^, '^', pow, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 4, "Returns _a raised to the power of _b");
OSC_EXPR_BUILTIN_DEFOP(., '.', lookup, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 2, "Returns the value bound to _b in a subbundle associated with _a");

static t_osc_expr_funcrec _osc_expr_builtin_func_apply = {
	"apply",
	2,
	(char *[]){"_function", "_args"},
	NULL,
	1,
	1,
	(char *[]){"_result"},
	NULL,
	"Applies _function to _args and returns the result",
	osc_expr_builtin_apply,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_nth = {
	"nth",
	2,
	(char *[]){"_list", "_index"},
	NULL,
	0,
	1,
	(char *[]){"_item"},
	NULL,
	"Returns the nth item in a list (counting from 0)",
	osc_expr_builtin_nth,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_list = {
	"list",
	1,
	(char *[]){"_listelems"},
	NULL,
	1,
	1,
	(char *[]){"_list"},
	NULL,
	"Returns a list containing all elements passed as arguments",
	osc_expr_builtin_list,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_aseq = {
	"aseq",
	3,
	(char *[]){"_min", "_max", "_step"},
	NULL,
	0,
	1,
	(char *[]){"_aseq"},
	NULL,
	"Returns an arithmetic sequence counting from min to max.",
	osc_expr_builtin_aseq,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_if = {
	"if",
	3,
	(char *[]){"_test", "_then", "_else"},
	NULL,
	0,
	1,
	(char *[]){"_result"},
	NULL,
	"Evaluates _then if _test is true, and _else if it is false. Returns the result of the evaluated expression(s).",
	osc_expr_builtin_if,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

t_osc_expr_oprec *osc_expr_builtin_op_add = &_osc_expr_builtin_op_add;
t_osc_expr_oprec *osc_expr_builtin_op_assign = &_osc_expr_builtin_op_assign;

t_osc_expr_funcrec *osc_expr_builtin_func_add = &_osc_expr_builtin_func_add;
t_osc_expr_funcrec *osc_expr_builtin_func_assign = &_osc_expr_builtin_func_assign;
t_osc_expr_funcrec *osc_expr_builtin_func_apply = &_osc_expr_builtin_func_apply;
t_osc_expr_funcrec *osc_expr_builtin_func_nth = &_osc_expr_builtin_func_nth;
t_osc_expr_funcrec *osc_expr_builtin_func_list = &_osc_expr_builtin_func_list;
t_osc_expr_funcrec *osc_expr_builtin_func_aseq = &_osc_expr_builtin_func_aseq;
t_osc_expr_funcrec *osc_expr_builtin_func_if = &_osc_expr_builtin_func_if;

static t_osc_expr_funcrec *osc_expr_builtin_fsymtab[] = {
	&_osc_expr_builtin_func_assign,
	&_osc_expr_builtin_func_lt,
	&_osc_expr_builtin_func_gt,
	&_osc_expr_builtin_func_le,
	&_osc_expr_builtin_func_ge,
	&_osc_expr_builtin_func_add,
	&_osc_expr_builtin_func_sub,
	&_osc_expr_builtin_func_mul,
	&_osc_expr_builtin_func_div,
	&_osc_expr_builtin_func_mod,
	&_osc_expr_builtin_func_pow,
	&_osc_expr_builtin_func_lookup,
	&_osc_expr_builtin_func_apply,
	&_osc_expr_builtin_func_nth,
	&_osc_expr_builtin_func_list,
	&_osc_expr_builtin_func_aseq,
	&_osc_expr_builtin_func_if,
};

static t_osc_expr_oprec *osc_expr_builtin_osymtab[] = {
	&_osc_expr_builtin_op_assign,
	&_osc_expr_builtin_op_lt,
	&_osc_expr_builtin_op_gt,
	&_osc_expr_builtin_op_le,
	&_osc_expr_builtin_op_ge,
	&_osc_expr_builtin_op_add,
	&_osc_expr_builtin_op_sub,
	&_osc_expr_builtin_op_mul,
	&_osc_expr_builtin_op_div,
	&_osc_expr_builtin_op_mod,
	&_osc_expr_builtin_op_pow,
	&_osc_expr_builtin_op_lookup,
};

// this array can be indexed by either the ascii char of an operator,
// or in the case of multi character operators such as <=, by the 
// token defined in osc_expr_parser.y
static t_osc_expr_oprec *osc_expr_builtin_opcodeToOpRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_op_eq	,
	NULL, //&osc_expr_builtin_op_neq	,
	&_osc_expr_builtin_op_le	,
	&_osc_expr_builtin_op_ge	,
	NULL, //&_osc_expr_builtin_op_nullcoalesce,
	NULL, //&osc_expr_builtin_op_add1,
	NULL, //&osc_expr_builtin_op_sub1,
	NULL, //&osc_expr_builtin_op_addassign,
	NULL, //&osc_expr_builtin_op_subassign,
	NULL, //&osc_expr_builtin_op_mulassign, // 10
	NULL, //&osc_expr_builtin_op_divassign, 
	NULL, //&osc_expr_builtin_op_modassign,
	NULL, //&osc_expr_builtin_op_powassign,
	NULL, //&osc_expr_builtin_op_nullcoalesceassign,
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
	NULL, //&osc_expr_builtin_op_not	,
	NULL	,
	NULL	,
	NULL	,
	NULL, //&osc_expr_builtin_op_mod	,
	NULL, //&osc_expr_builtin_op_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&_osc_expr_builtin_op_mul	,
	&_osc_expr_builtin_op_add	,
	NULL	,
	&_osc_expr_builtin_op_sub	,
	&_osc_expr_builtin_op_lookup,
	&_osc_expr_builtin_op_div	,
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
	&_osc_expr_builtin_op_lt	,// 60
	&_osc_expr_builtin_op_assign	,
	&_osc_expr_builtin_op_gt	,
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
	&_osc_expr_builtin_op_pow	,
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
	NULL, //&osc_expr_builtin_op_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static t_osc_expr_funcrec *osc_expr_builtin_opcodeToFuncRec[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_func_eq	,
	NULL, //&osc_expr_builtin_func_neq	,
	&_osc_expr_builtin_func_le	,
	&_osc_expr_builtin_func_ge	,
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
	NULL, //&osc_expr_builtin_func_not	,
	NULL	,
	NULL	,
	NULL	,
	&_osc_expr_builtin_func_mod	,
	NULL, //&osc_expr_builtin_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&_osc_expr_builtin_func_mul	,
	&_osc_expr_builtin_func_add	,
	NULL	,
	&_osc_expr_builtin_func_sub	,
	&_osc_expr_builtin_func_lookup,
	&_osc_expr_builtin_func_div	,
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
	&_osc_expr_builtin_func_lt	, // 60
	&_osc_expr_builtin_func_assign	,
	&_osc_expr_builtin_func_gt	,
	NULL, //&osc_expr_builtin_func_nullcoalesce,
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
	&_osc_expr_builtin_func_pow	,
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
	NULL, //&osc_expr_builtin_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static char *osc_expr_builtin_opcodeToString[128] = {
	NULL	,// 0
	NULL, //&osc_expr_builtin_func_eq	,
	NULL, //&osc_expr_builtin_func_neq	,
	"<="	,
	">="	,
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
	NULL, //&osc_expr_builtin_func_not	,
	NULL	,
	NULL	,
	NULL	,
	"%"	,
	NULL, //&osc_expr_builtin_func_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	"*"	,
	"+"	,
	NULL	,
	"-",
	".",
	"/",
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
	"<",
        "="     ,
	">",
	NULL, //&osc_expr_builtin_func_nullcoalesce,
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
	"^",
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
	NULL, //&osc_expr_builtin_func_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

t_osc_expr_funcrec *osc_expr_builtin_lookupFunction(char *name)
{
	const int n = sizeof(osc_expr_builtin_fsymtab) / sizeof(t_osc_expr_funcrec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(name, osc_expr_funcrec_getName(osc_expr_builtin_fsymtab[i]))){
			return osc_expr_builtin_fsymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtin_lookupOperator(char *op)
{
	const int n = sizeof(osc_expr_builtin_osymtab) / sizeof(t_osc_expr_oprec*);
	for(int i = 0; i < n; i++){
		if(!strcmp(op, osc_expr_oprec_getName(osc_expr_builtin_osymtab[i]))){
			return osc_expr_builtin_osymtab[i];
		}
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_builtin_lookupOperatorForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtin_opcodeToFuncRec) / sizeof(char *)){
		char *str = osc_expr_builtin_opcodeToString[(int)op];
		t_osc_expr_oprec *or = osc_expr_builtin_lookupOperator(str);
		return or;
	}
	return NULL;
}

t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOperator(t_osc_expr_oprec *op)
{
	char bytecode = osc_expr_oprec_getBytecode(op);
	return osc_expr_builtin_lookupFunctionForOpcode(bytecode);
}

t_osc_expr_funcrec *osc_expr_builtin_lookupFunctionForOpcode(char op)
{
	if(op > 0 && op < sizeof(osc_expr_builtin_opcodeToFuncRec) / sizeof(t_osc_expr_funcrec*)){
		return osc_expr_builtin_opcodeToFuncRec[(int)op];
	}
	return NULL;
}



//////////////////////////////////////////////////
// built in c functions
//////////////////////////////////////////////////
// binary op that produces a numeric value
#define OSC_EXPR_BUILTIN_BINARYOP_CASE(tt, type, op) case tt: osc_atom_u_set##type (y, osc_atom_u_get##type (l) op osc_atom_u_get##type (r)); break;

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_INT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('i', Int32, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('c', Int8, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('C', UInt8, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('u', Int16, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('U', UInt16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('I', UInt32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('h', Int64, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('H', UInt64, op);

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_FLOAT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('f', Float, op);		\
	OSC_EXPR_BUILTIN_BINARYOP_CASE('d', Double, op);

#define OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(op)	\
	OSC_EXPR_BUILTIN_BINARYOP_SWITCH_INT(op);	\
	OSC_EXPR_BUILTIN_BINARYOP_SWITCH_FLOAT(op);

// binary op that has to be called as a function
#define OSC_EXPR_BUILTIN_BINARYFUNC_CASE(tt, type, func) case tt: osc_atom_u_set##type (y, func (osc_atom_u_get##type (l), osc_atom_u_get##type (r))); break;

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_INT(func)		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('i', Int32, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('c', Int8, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('C', UInt8, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('u', Int16, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('U', UInt16, func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('I', UInt32, func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('h', Int64, func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('H', UInt64, func);

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_FLOAT(float_func, double_func)		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('f', Float, float_func);		\
	OSC_EXPR_BUILTIN_BINARYFUNC_CASE('d', Double, double_func);

#define OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_NUMERIC(int_func, float_func, double_func) \
	OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_INT(int_func);	\
	OSC_EXPR_BUILTIN_BINARYFUNC_SWITCH_FLOAT(float_func, double_func);

// binary op that produces a bool value
#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE(tt, type, op) case tt: osc_atom_u_setBool (y, osc_atom_u_get##type (l) op osc_atom_u_get##type (r)); break;

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_INT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('i', Int32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('c', Int8, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('C', UInt8, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('u', Int16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('U', UInt16, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('I', UInt32, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('h', Int64, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('H', UInt64, op);

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_FLOAT(op)		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('f', Float, op);	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_CASE('d', Double, op);

#define OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(op)	\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_INT(op);		\
	OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_FLOAT(op);

int osc_expr_builtin_add(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	// all argument vectors have been expanded to be the same length
	// all types have been harmonized
	// arity has been checked
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(+);
			// string
			// bundle
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_sub(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(-);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_mul(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(*);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_div(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_SWITCH_NUMERIC(/);
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_mod(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_CASE('c', Int8, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('C', UInt8, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('u', Int16, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('U', UInt16, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('i', Int32, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('I', UInt32, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('h', Int64, %);
			OSC_EXPR_BUILTIN_BINARYOP_CASE('H', UInt64, %);
			OSC_EXPR_BUILTIN_BINARYFUNC_CASE('f', Float, fmodf);
			OSC_EXPR_BUILTIN_BINARYFUNC_CASE('d', Double, fmod);
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_pow(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		double l = osc_atom_u_getDouble(osc_atom_array_u_get(argv[0], i));
		double r = osc_atom_u_getDouble(osc_atom_array_u_get(argv[1], i));
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[0], i))){			
		case 'c':
			osc_atom_u_setInt8(y, (int8_t)pow(l, r));
			break;
		case 'C':
			osc_atom_u_setUInt8(y, (uint8_t)pow(l, r));
			break;
		case 'u':
			osc_atom_u_setInt16(y, (int16_t)pow(l, r));
			break;
		case 'U':
			osc_atom_u_setUInt16(y, (uint16_t)pow(l, r));
			break;
		case 'i':
			osc_atom_u_setInt32(y, (int32_t)pow(l, r));
			break;
		case 'I':
			osc_atom_u_setUInt32(y, (uint32_t)pow(l, r));
			break;
		case 'h':
			osc_atom_u_setInt64(y, (int64_t)pow(l, r));
			break;
		case 'H':
			osc_atom_u_setUInt64(y, (uint64_t)pow(l, r));
			break;
		case 'f':
			osc_atom_u_setFloat(y, powf(l, r));
			break;
		case 'd':
			osc_atom_u_setDouble(y, pow(l, r));
			break;
			// timetag
		default:
			;
		}
	}

	return 0;
}

int osc_expr_builtin_lt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(<);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_gt(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(>);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_le(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(<=);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_ge(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *l = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *r = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		switch(osc_atom_u_getTypetag(l)){				
			OSC_EXPR_BUILTIN_BINARYOP_BOOL_SWITCH_NUMERIC(>=);
			// timetag
		default:
			;
		}
	}
	return 0;
}

int osc_expr_builtin_nth(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	printf("%s would be evaluated if it were implemented...\n", __func__);
	return 0;
}

int osc_expr_builtin_list(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	int outlen = 0;
	int i;
	for(i = 0; i < argc; i++){
		outlen += osc_atom_array_u_getLen(argv[i]);
	}
	if(outlen <= 0){
		return 0;
	}
	*out = osc_atom_array_u_alloc(outlen);
	int pos = 0;
	for(i = 0; i < argc; i++){
		osc_atom_array_u_copyInto(out, argv[i], pos);
		pos += osc_atom_array_u_getLen(argv[i]);
	}
	return 0;
}

int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	printf("%s would be evaluated if it were implemented...\n", __func__);
	return 0;
}

//////////////////////////////////////////////////
// special functions
//////////////////////////////////////////////////
int osc_expr_specFunc_assign_impl(t_osc_expr_ast_expr *ast,
				  t_osc_expr_lexenv *lexenv,
				  t_osc_bndl_u *oscbndl,
				  t_osc_atom_ar_u *indicies,
				  t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_expr *lval_expr = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)ast);
	t_osc_expr_ast_expr *rval_expr = osc_expr_ast_expr_next(lval_expr);
	if(!lval_expr || !rval_expr){
		return 1;
	}
	switch(osc_expr_ast_expr_getNodetype(lval_expr)){
	case OSC_EXPR_AST_NODETYPE_VALUE:
		printf("value\n");
		{
			osc_expr_ast_expr_evalInLexEnv(rval_expr, lexenv, oscbndl, out);
			if(!*out){
				printf("bail %d\n", __LINE__);
				return 1;
			}
			t_osc_atom_ar_u *rval = *out;
			t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)lval_expr;
			switch(osc_expr_ast_value_getValueType(v)){
			case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
				// this is illegal
				printf("bail %d\n", __LINE__);
				return 1;
			case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
				// could be a string
				printf("bail %d\n", __LINE__);
				break;
			case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
				printf("%s:%d\n", __func__, __LINE__);
				{
					char *address = osc_atom_u_getStringPtr(osc_expr_ast_value_getValue(v));
					t_osc_msg_u *m = NULL;
					if(indicies){
						t_osc_msg_ar_u *msgar = NULL;
						osc_bundle_u_lookupAddress(oscbndl, address, &msgar, 1);
						if(!msgar){
							printf("bail %d\n", __LINE__);
							return 1;
						}
						t_osc_msg_u *m = osc_message_array_u_get(msgar, 0);
						for(int i = 0; i < osc_atom_array_u_getLen(indicies); i++){
							int idx = osc_atom_u_getInt(osc_atom_array_u_get(indicies, i));
							int n = osc_atom_array_u_getLen(rval);
							if(idx < n){
								t_osc_atom_u *dest = NULL;
								osc_message_u_getArg(m, idx, &dest);
								t_osc_atom_u *src = osc_atom_array_u_get(rval, i);
								osc_atom_u_copy(&dest, src);
							}else{
								printf("bail %d\n", __LINE__);
								return 1;
							}
						}
					}else{
						m = osc_message_u_allocWithArray(address, rval);
						osc_bundle_u_addMsgWithoutDups(oscbndl, m);
					}
				}
				printf("%s:%d\n", __func__, __LINE__);
				return 0;
			}
		}
		printf("value done\n");
		break;
	case OSC_EXPR_AST_NODETYPE_ARRAYSUBSCRIPT:
		{
			printf("array subscript\n");
			t_osc_expr_ast_arraysubscript *as = (t_osc_expr_ast_arraysubscript *)lval_expr;
			t_osc_expr_ast_expr *base = osc_expr_ast_arraysubscript_getBase(as);
			t_osc_expr_ast_expr *indexList = osc_expr_ast_arraysubscript_getIndexList(as);
			t_osc_atom_ar_u *idxs = NULL;
			osc_expr_ast_expr_evalInLexEnv(indexList, lexenv, oscbndl, &idxs);
			if(!idxs){
				printf("%s: %d no indexes in an array subscript!\n", __func__, __LINE__);
				return 1;
			}
			t_osc_expr_ast_funcall *f = osc_expr_ast_funcall_alloc(osc_expr_builtin_func_assign, 2, osc_expr_ast_expr_copy(base), osc_expr_ast_expr_copy(rval_expr));
			osc_expr_specFunc_assign_impl((t_osc_expr_ast_expr *)f, lexenv, oscbndl, idxs, out);
			// free vars we just allocated
		}
		printf("array subscript done\n");
		break;
	case OSC_EXPR_AST_NODETYPE_BINARYOP:
		{
			printf("binary op\n");
		}
		break;

	}
	return 0;
}

int osc_expr_specFunc_assign(t_osc_expr_ast_expr *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	if(!oscbndl){
		return 1;
	}
	return osc_expr_specFunc_assign_impl(f, lexenv, oscbndl, NULL, out);
}

int osc_expr_specFunc_lookup(t_osc_expr_ast_expr *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	printf("%s\n", __func__);
	t_osc_expr_ast_expr *lhs = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)f);
	t_osc_expr_ast_expr *rhs = osc_expr_ast_expr_next(lhs);
	t_osc_atom_ar_u *ar = NULL;
	osc_expr_ast_expr_evalInLexEnv(lhs, lexenv, oscbndl, &ar);
	if(!ar){
		printf("%s bail at %d\n", __func__, __LINE__);
		return 1;
	}
	t_osc_bndl_u *b = NULL;
	for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
		t_osc_atom_u *a = osc_atom_array_u_get(ar, i);
		if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
			b = osc_atom_u_getBndl(a);
			break;
		}
	}
	if(!b){
		printf("%s bail at %d\n", __func__, __LINE__);
		return 1;
	}
	char *address = NULL;
	switch(osc_expr_ast_expr_getNodetype(rhs)){
	case OSC_EXPR_AST_NODETYPE_VALUE:
		if(osc_expr_ast_value_getValueType((t_osc_expr_ast_value *)rhs) == OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS){
			address = osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress((t_osc_expr_ast_value *)rhs));
			printf("%s:%d: %s\n", __func__, __LINE__, address);
		}else{
			printf("%s bail at %d\n", __func__, __LINE__);
			goto out;
		}
		break;
	default:
		{
			t_osc_atom_ar_u *ar = NULL;
			osc_expr_ast_expr_evalInLexEnv(rhs, lexenv, oscbndl, &ar);
			if(!ar){
				printf("%s bail at %d\n", __func__, __LINE__);
				goto out;
			}
			// first val has to be a string...
			address = osc_atom_u_getStringPtr(osc_atom_array_u_get(ar, 0));
			osc_atom_array_u_free(ar);
		}
	}
	if(!address){
		printf("%s bail at %d\n", __func__, __LINE__);
		goto out;
	}
	t_osc_msg_ar_u *msgar = NULL;
	printf("address: %s\n", address);
	osc_bundle_u_lookupAddress(b, address, &msgar, 1);
	t_osc_msg_u *m = osc_message_array_u_get(msgar, 0);
	printf("%s: message address: %s\n", __func__, osc_message_u_getAddress(m));
	/*
	t_osc_msg_it_u *it = osc_msg_it_u_get(m);
	while(osc_msg_it_u_hasNext(it)){
		t_osc_atom_u *a = osc_msg_it_u_next(it);
		if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
			t_osc_atom_ar_u *ar = osc_atom_array_u_alloc(1);
			t_osc_atom_u *dest = osc_atom_array_u_get(ar, 0);
			osc_atom_u_copy(&dest, a);
			*out = ar;
			break;
		}else{
			printf("haha!\n");
		}
	}
	osc_msg_it_u_destroy(it);
	*/
	*out = osc_message_u_getArgArrayCopy(m);
	osc_message_array_u_free(msgar);
 out:
	// cleanup
	return 0;
}

OSC_EXPR_SPECFUNC_DECL(apply)
{printf("%s bitches\n", __func__);return 0;}
OSC_EXPR_SPECFUNC_DECL(map)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(reduce)
{return 0;}
//OSC_EXPR_SPECFUNC_DECL(assign)
//{printf("%s bitches\n", __func__);return 0;}
OSC_EXPR_SPECFUNC_DECL(assigntoindex)
{return 0;}
OSC_EXPR_SPECFUNC_DECL(if)
{printf("%s bitches\n", __func__);return 0;}
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
//OSC_EXPR_SPECFUNC_DECL(lookup)
//{return 0;}
