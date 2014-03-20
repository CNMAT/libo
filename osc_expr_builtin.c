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

/*
---put a context flag in the expr struct that notes whether the expr is an lval or not
---pass the working bundle to all functions
---self is then just a keyword that notes whether or not to use the working bundle OR self will cause osc_expr_ast_value_evalInLexEnv to copy the pointer into an atom

---an explicit call to eval() needs to eval it's argument twice, just like lisp. So, 
---/foo./bar can't be eval(/foo, /bar), it needs to be something different like lookup().
---eval() and lookup() should take an implicit self arg
---we need lookupstr() and evalstr()
 */

#include <string.h>
#include "osc_expr_builtin.h"
#include "osc_expr_funcrec.h"
#include "osc_expr_oprec.h"
#include "osc_expr_funcrec.r"
#include "osc_expr_oprec.r"
#include "osc_expr_ast_funcall.h"
#include "osc_expr_ast_function.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_arraysubscript.h"
#include "osc_expr_ast_value.h"
#include "osc_expr_privatedecls.h" 
#include "osc_message_iterator_u.h"
#include "osc_expr_builtin_typedfuncdecls.h"

//#define __OSC_PROFILE__
#include "osc_profile.h"

int _osc_expr_sign(double f);
int osc_expr_builtin_list(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_builtin_scale(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_builtin_call_binary_func(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

#define OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, lhs, rhs, res)\
	int osc_expr_builtin_##funcname##_##typetag(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)

#ifdef OSC_EXPR_BUILTIN_BINARYOP_CHECKTYPES
#define OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs)			\
	if(osc_atom_u_getTypetag(lhs) != typetag || osc_atom_u_getTypetag(rhs) != typetag){	\
		return 1;						\
	}
#else
#define OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs)
#endif


#define OSC_EXPR_BUILTIN_BINARYOP(op, funcname, typetag, intype, outtype)	\
	OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, a, b, y){	\
		OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, a, b);\
		osc_atom_u_set##outtype (y, osc_atom_u_get##intype (a) op osc_atom_u_get##intype (b)); \
		return 0;\
	}

#define OSC_EXPR_BUILTIN_BINARY_CFUNC(funcname, cfuncname, typetag, argtype, restype, casttype) \
	OSC_EXPR_BUILTIN_BINARYOP_DECL(funcname, typetag, a, b, y){\
		OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, a, b);\
		osc_atom_u_set##restype(y, (casttype)cfuncname(osc_atom_u_get##argtype(a), osc_atom_u_get##argtype(b)));\
		return 0;\
	}

OSC_EXPR_BUILTIN_BINARYOP(+, add, c, Int8, Int8);
OSC_EXPR_BUILTIN_BINARYOP(+, add, C, UInt8, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(+, add, u, Int16, Int16);
OSC_EXPR_BUILTIN_BINARYOP(+, add, U, UInt16, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(+, add, i, Int32, Int32);
OSC_EXPR_BUILTIN_BINARYOP(+, add, I, UInt32, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(+, add, h, Int64, Int64);
OSC_EXPR_BUILTIN_BINARYOP(+, add, H, UInt64, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(+, add, f, Float, Float);
OSC_EXPR_BUILTIN_BINARYOP(+, add, d, Double, Double);

OSC_EXPR_BUILTIN_BINARYOP(-, sub, c, Int8, Int8);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, C, UInt8, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, u, Int16, Int16);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, U, UInt16, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, i, Int32, Int32);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, I, UInt32, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, h, Int64, Int64);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, H, UInt64, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, f, Float, Float);
OSC_EXPR_BUILTIN_BINARYOP(-, sub, d, Double, Double);

OSC_EXPR_BUILTIN_BINARYOP(*, mul, c, Int8, Int8);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, C, UInt8, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, u, Int16, Int16);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, U, UInt16, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, i, Int32, Int32);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, I, UInt32, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, h, Int64, Int64);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, H, UInt64, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, f, Float, Float);
OSC_EXPR_BUILTIN_BINARYOP(*, mul, d, Double, Double);

OSC_EXPR_BUILTIN_BINARYOP(/, div, c, Int8, Int8);
OSC_EXPR_BUILTIN_BINARYOP(/, div, C, UInt8, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(/, div, u, Int16, Int16);
OSC_EXPR_BUILTIN_BINARYOP(/, div, U, UInt16, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(/, div, i, Int32, Int32);
OSC_EXPR_BUILTIN_BINARYOP(/, div, I, UInt32, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(/, div, h, Int64, Int64);
OSC_EXPR_BUILTIN_BINARYOP(/, div, H, UInt64, UInt64);
OSC_EXPR_BUILTIN_BINARYOP(/, div, f, Float, Float);
OSC_EXPR_BUILTIN_BINARYOP(/, div, d, Double, Double);

OSC_EXPR_BUILTIN_BINARYOP(<, lt, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<, lt, d, Double, Bool);

OSC_EXPR_BUILTIN_BINARYOP(>, gt, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>, gt, d, Double, Bool);

OSC_EXPR_BUILTIN_BINARYOP(<=, le, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(<=, le, d, Double, Bool);

OSC_EXPR_BUILTIN_BINARYOP(>=, ge, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(>=, ge, d, Double, Bool);

OSC_EXPR_BUILTIN_BINARYOP(==, eq, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, d, Double, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, T, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(==, eq, F, Int32, Bool);

OSC_EXPR_BUILTIN_BINARYOP(!=, neq, c, Int8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, C, UInt8, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, u, Int16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, U, UInt16, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, i, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, I, UInt32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, h, Int64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, H, UInt64, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, f, Float, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, d, Double, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, T, Int32, Bool);
OSC_EXPR_BUILTIN_BINARYOP(!=, neq, F, Int32, Bool);

OSC_EXPR_BUILTIN_BINARYOP(%, mod, c, Int8, Int8);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, C, UInt8, UInt8);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, u, Int16, Int16);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, U, UInt16, UInt16);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, i, Int32, Int32);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, I, UInt32, UInt32);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, h, Int64, Int64);
OSC_EXPR_BUILTIN_BINARYOP(%, mod, H, UInt64, UInt64);
OSC_EXPR_BUILTIN_BINARY_CFUNC(mod, fmodf, f, Float, Float, float);
OSC_EXPR_BUILTIN_BINARY_CFUNC(mod, fmod, d, Double, Double, double);
/*
OSC_EXPR_BUILTIN_BINARYOP_DECL(mod, f, a, b, y)
{
	OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs);
	osc_atom_u_setFloat(y, fmodf(osc_atom_u_getFloat(a), osc_atom_u_getFloat(b)));
	return 0;
}
OSC_EXPR_BUILTIN_BINARYOP_DECL(mod, d, a, b, y)
{
	OSC_EXPR_BUILTIN_BINARYOP_TYPECHECK(typetag, lhs, rhs);
	osc_atom_u_setDouble(y, fmod(osc_atom_u_getDouble(a), osc_atom_u_getDouble(b)));
	return 0;
}
 */

OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, c, Double, Int8, int8_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, C, Double, UInt8, uint8_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, u, Double, Int16, int16_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, U, Double, UInt16, uint16_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, i, Double, Int32, int32_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, I, Double, UInt32, uint32_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, h, Double, Int64, int64_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, H, Double, UInt64, uint64_t);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, powf, f, Double, Float, float);
OSC_EXPR_BUILTIN_BINARY_CFUNC(pow, pow, d, Double, Double, double);

int osc_expr_builtin_and_T(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)
{
	osc_atom_u_setBool(res, osc_atom_u_getInt32(lhs) && osc_atom_u_getInt32(rhs));
	return 0;
}

int osc_expr_builtin_and_F(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)
{
	return osc_expr_builtin_and_T(lhs, rhs, res);
}

int osc_expr_builtin_or_T(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)
{
	osc_atom_u_setBool(res, osc_atom_u_getInt32(lhs) || osc_atom_u_getInt32(rhs));
	return 0;
}

int osc_expr_builtin_or_F(t_osc_atom_u *lhs, t_osc_atom_u *rhs, t_osc_atom_u *res)
{
	return osc_expr_builtin_or_T(lhs, rhs, res);
}

//int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
//{

//////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////

#define OSC_EXPR_BUILTIN_DEFCONSTANT(name, val)				\
	static t_osc_expr_funcrec _osc_expr_builtin_func_##name = {	\
		#name,							\
		0,							\
		NULL,					\
		NULL,\
		0,							\
		1,							\
		(char *[]){"_y"},					\
		NULL,							\
		"returns "#val,						\
		osc_expr_builtin_##name,			\
		NULL,\
		NULL,							\
		0,							\
		NULL,				\
		0,							\
		0,							\
		NULL,				\
		0							\
	};

#define OSC_EXPR_BUILTIN_CONSTANT(name, val, type)			\
	int osc_expr_builtin_##name(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out){ \
		*out = osc_atom_array_u_alloc(1);			\
		osc_atom_u_set##type(osc_atom_array_u_get(*out, 0), val); \
		return 0;						\
	}								\
	OSC_EXPR_BUILTIN_DEFCONSTANT(name, val);

OSC_EXPR_BUILTIN_CONSTANT(pi, M_PI, Double);
OSC_EXPR_BUILTIN_CONSTANT(twopi, 2. * M_PI, Double);
OSC_EXPR_BUILTIN_CONSTANT(halfpi, M_PI_2, Double);
OSC_EXPR_BUILTIN_CONSTANT(quarterpi, M_PI_4, Double);
OSC_EXPR_BUILTIN_CONSTANT(oneoverpi, M_1_PI, Double);
OSC_EXPR_BUILTIN_CONSTANT(twooverpi, M_2_PI, Double);

OSC_EXPR_BUILTIN_CONSTANT(e, M_E, Double);

OSC_EXPR_BUILTIN_CONSTANT(lntwo, M_LN2, Double);
OSC_EXPR_BUILTIN_CONSTANT(lnten, M_LN10, Double);
OSC_EXPR_BUILTIN_CONSTANT(logtwoe, M_LOG2E, Double);
OSC_EXPR_BUILTIN_CONSTANT(logtene, M_LOG10E, Double);

OSC_EXPR_BUILTIN_CONSTANT(sqrttwo, M_SQRT2, Double);
OSC_EXPR_BUILTIN_CONSTANT(sqrthalf, M_SQRT1_2, Double);

OSC_EXPR_BUILTIN_CONSTANT(degtorad, M_PI / 180., Double);
OSC_EXPR_BUILTIN_CONSTANT(radtodeg, 180. / M_PI, Double);

//////////////////////////////////////////////////
// type casts
//////////////////////////////////////////////////
/*
#define OSC_EXPR_BUILTIN_DEFCAST(name)					\
	static t_osc_expr_funcrec _osc_expr_builtin_func_##name = {	\
		#name,							\
		1,							\
		(char *[]){"_x"},					\
		NULL,							\
		0,							\
		1,							\
		(char *[]){"_y"},					\
		NULL,							\
		"returns _x cast to "#name,				\
		osc_expr_builtin_##name,				\
		NULL,							\
		NULL,							\
		0,							\
		NULL,							\
		0,							\
		0,							\
		NULL,							\
		0							\
	};

#define OSC_EXPR_BUILTIN_CAST(name, type, typetag)			\
	int osc_expr_builtin_##name(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out){ \
		if(osc_typetag_isSubtype(osc_atom_u_getTypetag(osc_atom_array_u_get(argv[0], 0)), typetag)){ \
			*out = osc_atom_array_u_alloc(1);		\
			osc_atom_u_set##type(osc_atom_array_u_get(*out, 0), osc_atom_u_get##type(osc_atom_array_u_get(argv[0], 0))); \
		}else{							\
			return 1;					\
		}							\
		return 0;						\
	}								\
	OSC_EXPR_BUILTIN_DEFCAST(name);

// string, bundle, code
OSC_EXPR_BUILTIN_CAST(timetag, Timetag, OSC_TIMETAG_TYPETAG);
OSC_EXPR_BUILTIN_CAST(uint64, UInt64, 'H');
OSC_EXPR_BUILTIN_CAST(int64, Int64, 'h');
OSC_EXPR_BUILTIN_CAST(float64, Double, 'd');
OSC_EXPR_BUILTIN_CAST(uint32, UInt32, 'I');
OSC_EXPR_BUILTIN_CAST(float32, Float, 'f');
OSC_EXPR_BUILTIN_CAST(int32, Int32, 'i');
OSC_EXPR_BUILTIN_CAST(uint16, UInt16, 'U');
OSC_EXPR_BUILTIN_CAST(int16, Int16, 'u');
OSC_EXPR_BUILTIN_CAST(uint8, UInt8, 'C');
OSC_EXPR_BUILTIN_CAST(int8, Int8, 'c');
// can't cast to bool, true, false, or null
*/

#define OSC_EXPR_BUILTIN_ASEQ_TYPED(typetag, type, ctype)		\
	int osc_expr_builtin_aseq_##typetag(t_osc_atom_u *_min, t_osc_atom_u *_max, t_osc_atom_u *_step, int n, t_osc_atom_ar_u *out){ \
		ctype min = osc_atom_u_get##type(_min);\
		ctype max = osc_atom_u_get##type(_max);\
		ctype step = osc_atom_u_get##type(_step);\
		for(int i = 0; i < n; i++){\
			osc_atom_u_set##type(osc_atom_array_u_get(out, i), min);\
			min += step;\
		}\
		return 0;\
	}

OSC_EXPR_BUILTIN_ASEQ_TYPED(c, Int8, int8_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(C, UInt8, uint8_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(u, Int16, int16_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(U, UInt16, uint16_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(i, Int32, int32_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(I, UInt32, uint32_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(h, Int64, int64_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(H, UInt64, uint64_t)
OSC_EXPR_BUILTIN_ASEQ_TYPED(f, Float, float)
OSC_EXPR_BUILTIN_ASEQ_TYPED(d, Double, double)

int osc_expr_builtin_aseq(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	t_osc_atom_u *start_a = osc_atom_array_u_get(argv[0], 0);
	t_osc_atom_u *end_a = osc_atom_array_u_get(argv[1], 0);
	t_osc_atom_u *step_a = osc_atom_array_u_get(argv[2], 0);

	double start = osc_atom_u_getDouble(start_a);
	double end = osc_atom_u_getDouble(end_a);
	double step = osc_atom_u_getDouble(step_a);

	if(step == 0){
		// error
		return 1;
	}
	if(_osc_expr_sign(end - start) != _osc_expr_sign(step)){
		// error
		return 1;
	}
	if(start > end){
		start_a = end_a;
		end_a = osc_atom_array_u_get(argv[0], 0);
		double tmp = start;
		start = end;
		end = start;
	}
	int n = (int)((end - start) / fabs(step)) + 1;
	t_osc_expr_funcrec *rec = osc_expr_ast_funcall_getFuncRec(ast);
	int (*f)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, int, t_osc_atom_ar_u*) = (int (*)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, int, t_osc_atom_ar_u*))osc_expr_funcrec_getFuncForTypetag(rec, osc_atom_u_getTypetag(start_a));
	if(f){
		*out = osc_atom_array_u_alloc(n);
		return f(start_a, end_a, step_a, n, *out);
	}else{
		return 1;
	}
}

#define OSC_EXPR_BUILTIN_SCALE_TYPED(typetag, type, ctype)		\
	int osc_expr_builtin_scale_##typetag(t_osc_atom_u *_x, t_osc_atom_u *_x1, t_osc_atom_u *_x2, t_osc_atom_u *_y1, t_osc_atom_u *_y2, t_osc_atom_u *out){ \
		ctype x = osc_atom_u_get##type(_x);\
		ctype x1 = osc_atom_u_get##type(_x1);\
		ctype x2 = osc_atom_u_get##type(_x2);\
		ctype y1 = osc_atom_u_get##type(_y1);\
		ctype y2 = osc_atom_u_get##type(_y2);\
		ctype m = (y2 - y1) / (x2 - x1);\
		ctype b = (y1 - (m * x1));\
		osc_atom_u_set##type(out, m * x + b);\
		return 0;\
	}

OSC_EXPR_BUILTIN_SCALE_TYPED(c, Int8, int8_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(C, UInt8, uint8_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(u, Int16, int16_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(U, UInt16, uint16_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(i, Int32, int32_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(I, UInt32, uint32_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(h, Int64, int64_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(H, UInt64, uint64_t)
OSC_EXPR_BUILTIN_SCALE_TYPED(f, Float, float)
OSC_EXPR_BUILTIN_SCALE_TYPED(d, Double, double)

int osc_expr_builtin_scale(t_osc_expr_ast_funcall *ast, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	// shouldn't have to check argc against arity---that should already be done.
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	t_osc_atom_u *x1 = osc_atom_array_u_get(argv[1], 0);
	t_osc_atom_u *x2 = osc_atom_array_u_get(argv[2], 0);
	t_osc_atom_u *y1 = osc_atom_array_u_get(argv[3], 0);
	t_osc_atom_u *y2 = osc_atom_array_u_get(argv[4], 0);
	for(int i = 0; i < n; i++){
		t_osc_atom_u *x = osc_atom_array_u_get(argv[0], i);
		t_osc_expr_funcrec *rec = osc_expr_ast_funcall_getFuncRec(ast);
		int (*f)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*) =
		(int (*)(t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*, t_osc_atom_u*))osc_expr_funcrec_getFuncForTypetag(rec, osc_atom_u_getTypetag(x));
		if(f){
			int ret = f(x, x1, x2, y1, y2, osc_atom_array_u_get(*out, i));
			if(ret){
				return ret;
			}
		}else{
			return 1;
		}
	}
	return 0;
}

#define osc_expr_builtin_lval_NULL NULL
#define OSC_EXPR_BUILTIN_DEFOP(op, opcode, func, lvalfunc, vtab, lhs, rhs, return, assoc, prec, docstring) \
t_osc_expr_oprec _osc_expr_builtin_op_##func = {\
	#op,\
	2,\
	(char *[]){#lhs, #rhs},			\
	(char *[]){(char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}}, \
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
	(char *[]){(char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}}, \
	0,\
	1,\
	(char *[]){#return},\
	NULL,\
	docstring,\
	osc_expr_builtin_call_binary_func,\
	osc_expr_builtin_lval_##lvalfunc,	\
	vtab,\
	2,\
	(unsigned int []){0, 1},\
	0,\
	2,\
	(unsigned int []){0, 1},\
	0\
};

//OSC_EXPR_BUILTIN_DEFOP(=, '=', assign, NULL, NULL, _lval, _rval, _rval, OSC_EXPR_PARSER_ASSOC_RIGHT, 16, "Assigns _rval to _lval and returns _rval");
OSC_EXPR_BUILTIN_DEFOP(<, '<', lt, NULL, OSC_EXPR_BUILTIN_LT_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(>, '>', gt, NULL, OSC_EXPR_BUILTIN_GT_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(<=, OSC_EXPR_LTE, le, NULL, OSC_EXPR_BUILTIN_LE_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is less than or equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(>=, OSC_EXPR_GTE, ge, NULL, OSC_EXPR_BUILTIN_GE_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is greater than or equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(==, OSC_EXPR_EQ, eq, NULL, OSC_EXPR_BUILTIN_EQ_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(!=, OSC_EXPR_NEQ, neq, NULL, OSC_EXPR_BUILTIN_NEQ_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 8, "Returns true (bool) if _a is equal to _b or false otherwise");
OSC_EXPR_BUILTIN_DEFOP(+, '+', add, NULL, OSC_EXPR_BUILTIN_ADD_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the sum of its arguments");
OSC_EXPR_BUILTIN_DEFOP(-, '-', sub, NULL, OSC_EXPR_BUILTIN_SUB_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 6, "Returns the difference of its arguments");
OSC_EXPR_BUILTIN_DEFOP(*, '*', mul, NULL, OSC_EXPR_BUILTIN_MUL_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the product of its arguments");
OSC_EXPR_BUILTIN_DEFOP(/, '/', div, NULL, OSC_EXPR_BUILTIN_DIV_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the quotient of its arguments");
OSC_EXPR_BUILTIN_DEFOP(%, '%', mod, NULL, OSC_EXPR_BUILTIN_MOD_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 5, "Returns the remainder of _a divided by _b");
OSC_EXPR_BUILTIN_DEFOP(^, '^', pow, NULL, OSC_EXPR_BUILTIN_POW_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 4, "Returns _a raised to the power of _b");
OSC_EXPR_BUILTIN_DEFOP(&, OSC_EXPR_AND, and, NULL, OSC_EXPR_BUILTIN_AND_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 13, "Returns true if _a and _b are true, false otherwise");
OSC_EXPR_BUILTIN_DEFOP(|, OSC_EXPR_OR, or, NULL, OSC_EXPR_BUILTIN_OR_VTAB, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 14, "Returns true if either _a or _b are true, false otherwise");
//OSC_EXPR_BUILTIN_DEFOP(., '.', lookup, lookup, NULL, _a, _b, _y, OSC_EXPR_PARSER_ASSOC_LEFT, 2, "Returns the value bound to _b in a subbundle associated with _a");

static t_osc_expr_oprec _osc_expr_builtin_op_assign = {
	"=",
	2,
	(char *[]){"_lval", "_rval"},
	NULL,
	1,
	(char *[]){"_rval"},
	NULL,
	"Assigns _rval to _lval and returns _rval",
	OSC_EXPR_PARSER_ASSOC_RIGHT,
	16,
	0,
	'='
};

static t_osc_expr_funcrec _osc_expr_builtin_func_assign = {
	"assign",
	2,
	(char *[]){"_lval", "_rval"},
	NULL,
	0,
	1,
	(char *[]){"_rval"},
	NULL,
	"Assigns _rval to _lval and returns _rval",
	osc_expr_builtin_assign,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_apply = {
	"apply",
	1,
	(char *[]){"_function"},
	NULL,
	1,
	1,
	(char *[]){"_result"},
	NULL,
	"Applies _function to _args and returns the result",
	osc_expr_builtin_apply,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_map = {
	"map",
	1,
	(char *[]){"_function"},
	NULL,
	1,
	1,
	(char *[]){"_result"},
	NULL,
	"Maps _function onto _args and returns the result",
	osc_expr_builtin_map,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_lreduce = {
	"lreduce",
	2,
	(char *[]){"_function", "_list"},
	NULL,
	0,
	1,
	(char *[]){"_result"},
	NULL,
	"Combines all the elements in a list using a left-associative binary operation.",
	osc_expr_builtin_lreduce,
	NULL,
	NULL,
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
	osc_expr_builtin_lval_nth,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_oprec _osc_expr_builtin_op_lookup = {
	".",
	2,
	(char *[]){"_a", "_b"},
	NULL,
	1,
	(char *[]){"_y"},
	NULL,
	"Returns the value bound to _b in a subbundle associated with _a",
	OSC_EXPR_PARSER_ASSOC_LEFT,
	2,
	0,
	'.'
};

static t_osc_expr_funcrec _osc_expr_builtin_func_lookup = {
	"lookup",
	2,
	(char *[]){"_a", "_b"},
	NULL,
	0,
	1,
	(char *[]){"_y"},
	NULL,
	"Returns the value bound to _b in a subbundle associated with _a",
	osc_expr_builtin_lookup,
	osc_expr_builtin_lval_lookup,
	NULL,
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
	NULL,
	NULL,
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
	(char *[]){(char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}},
	0,
	1,
	(char *[]){"_aseq"},
	NULL,
	"Returns an arithmetic sequence counting from min to max.",
	osc_expr_builtin_aseq,
	NULL,
	OSC_EXPR_BUILTIN_ASEQ_VTAB,
	0,
	NULL,
	0,
	3,
	(unsigned int []){0, 1, 2},
	0,
};

static t_osc_expr_funcrec _osc_expr_builtin_func_scale = {
	"scale",
	5,
	(char *[]){"_x", "_x1", "_x2", "_y1", "_y2"},
	(char *[]){(char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}, (char []){OSC_TYPETAG_CLASS_NUMERIC, 0}},
	0,
	1,
	(char *[]){"_y"},
	NULL,
	"Scales _x from [_x1,_x2] to [_y1,_y2].",
	osc_expr_builtin_scale,
	NULL,
	OSC_EXPR_BUILTIN_SCALE_VTAB,
	0,
	NULL,
	0,
	5,
	(unsigned int []){0, 1, 2, 3, 4},
	0,
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
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_bundle = {
	"bundle",
	1,
	(char *[]){"_messages"},
	NULL,
	1,
	1,
	(char *[]){"_bundle"},
	NULL,
	"Creates a bundle containing the messages passed to it",
	osc_expr_builtin_bundle,
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	NULL,
	0
};

static t_osc_expr_funcrec _osc_expr_builtin_func_message = {
	"message",
	1,
	(char *[]){"_address", "_data"},
	NULL,
	1,
	1,
	(char *[]){"_message"},
	NULL,
	"Creates a message with the address and data passed to it",
	osc_expr_builtin_message,
	NULL,
	NULL,
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
t_osc_expr_funcrec *osc_expr_builtin_func_map = &_osc_expr_builtin_func_map;
t_osc_expr_funcrec *osc_expr_builtin_func_lreduce = &_osc_expr_builtin_func_lreduce;
t_osc_expr_funcrec *osc_expr_builtin_func_nth = &_osc_expr_builtin_func_nth;
t_osc_expr_funcrec *osc_expr_builtin_func_list = &_osc_expr_builtin_func_list;
t_osc_expr_funcrec *osc_expr_builtin_func_aseq = &_osc_expr_builtin_func_aseq;
t_osc_expr_funcrec *osc_expr_builtin_func_scale = &_osc_expr_builtin_func_scale;
t_osc_expr_funcrec *osc_expr_builtin_func_if = &_osc_expr_builtin_func_if;
t_osc_expr_funcrec *osc_expr_builtin_func_lookup = &_osc_expr_builtin_func_lookup;
t_osc_expr_funcrec *osc_expr_builtin_func_message = &_osc_expr_builtin_func_message;
t_osc_expr_funcrec *osc_expr_builtin_func_bundle = &_osc_expr_builtin_func_bundle;

static t_osc_expr_funcrec *osc_expr_builtin_fsymtab[] = {
	&_osc_expr_builtin_func_assign,
	&_osc_expr_builtin_func_lt,
	&_osc_expr_builtin_func_gt,
	&_osc_expr_builtin_func_le,
	&_osc_expr_builtin_func_ge,
	&_osc_expr_builtin_func_eq,
	&_osc_expr_builtin_func_neq,
	&_osc_expr_builtin_func_add,
	&_osc_expr_builtin_func_sub,
	&_osc_expr_builtin_func_mul,
	&_osc_expr_builtin_func_div,
	&_osc_expr_builtin_func_mod,
	&_osc_expr_builtin_func_pow,
	&_osc_expr_builtin_func_and,
	&_osc_expr_builtin_func_or,
	&_osc_expr_builtin_func_lookup,
 	&_osc_expr_builtin_func_apply,
	&_osc_expr_builtin_func_map,
 	&_osc_expr_builtin_func_lreduce,
	&_osc_expr_builtin_func_nth,
	&_osc_expr_builtin_func_list,
	&_osc_expr_builtin_func_aseq,
	&_osc_expr_builtin_func_scale,
	&_osc_expr_builtin_func_if,
	&_osc_expr_builtin_func_message,
	&_osc_expr_builtin_func_bundle,
	// constants
	&_osc_expr_builtin_func_pi,
	&_osc_expr_builtin_func_twopi,
	&_osc_expr_builtin_func_halfpi,
	&_osc_expr_builtin_func_quarterpi,
	&_osc_expr_builtin_func_oneoverpi,
	&_osc_expr_builtin_func_twooverpi,
	&_osc_expr_builtin_func_e,
	&_osc_expr_builtin_func_lntwo,
	&_osc_expr_builtin_func_lnten,
	&_osc_expr_builtin_func_logtwoe,
	&_osc_expr_builtin_func_logtene,
	&_osc_expr_builtin_func_sqrttwo,
	&_osc_expr_builtin_func_sqrthalf,
	&_osc_expr_builtin_func_degtorad,
	&_osc_expr_builtin_func_radtodeg,
	// casts
	/*
	&_osc_expr_builtin_func_timetag,
	&_osc_expr_builtin_func_uint64,
	&_osc_expr_builtin_func_int64,
	&_osc_expr_builtin_func_float64,
	&_osc_expr_builtin_func_uint32,
	&_osc_expr_builtin_func_float32,
	&_osc_expr_builtin_func_int32,
	&_osc_expr_builtin_func_uint16,
	&_osc_expr_builtin_func_int16,
	&_osc_expr_builtin_func_uint8,
	&_osc_expr_builtin_func_int8
	*/
};

static t_osc_expr_oprec *osc_expr_builtin_osymtab[] = {
	&_osc_expr_builtin_op_assign,
	&_osc_expr_builtin_op_lt,
	&_osc_expr_builtin_op_gt,
	&_osc_expr_builtin_op_le,
	&_osc_expr_builtin_op_ge,
	&_osc_expr_builtin_op_eq,
	&_osc_expr_builtin_op_neq,
	&_osc_expr_builtin_op_add,
	&_osc_expr_builtin_op_sub,
	&_osc_expr_builtin_op_mul,
	&_osc_expr_builtin_op_div,
	&_osc_expr_builtin_op_mod,
	&_osc_expr_builtin_op_pow,
	&_osc_expr_builtin_op_and,
	&_osc_expr_builtin_op_or,
	&_osc_expr_builtin_op_lookup,
};

// this array can be indexed by either the ascii char of an operator,
// or in the case of multi character operators such as <=, by the 
// token defined in osc_expr_parser.y
static t_osc_expr_oprec *osc_expr_builtin_opcodeToOpRec[128] = {
	NULL	,// 0
	&_osc_expr_builtin_op_eq	,
	&_osc_expr_builtin_op_neq	,
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
	&_osc_expr_builtin_op_and	,
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
	NULL	,
	&_osc_expr_builtin_op_or	,
	NULL	,
	NULL	,
	NULL	,
};

static t_osc_expr_funcrec *osc_expr_builtin_opcodeToFuncRec[128] = {
	NULL	,// 0
	&_osc_expr_builtin_func_eq	,
	&_osc_expr_builtin_func_neq	,
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
	&_osc_expr_builtin_func_and	,
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
	NULL	,
	&_osc_expr_builtin_func_or	,
	NULL	,
	NULL	,
	NULL	,
};

static char *osc_expr_builtin_opcodeToString[128] = {
	NULL	,// 0
	"==",
	"!=",
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
	"&"     ,
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
	NULL	,
	"|"     ,
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

typedef int (*t_osc_expr_builtin_binary_func)(t_osc_atom_u *, t_osc_atom_u *, t_osc_atom_u *);
int osc_expr_builtin_call_binary_func(t_osc_expr_ast_funcall *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out)
{
	// scalars should have been expanded, so all arg lists should be the same length
	int n = osc_atom_array_u_getLen(argv[0]);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		t_osc_expr_funcrec *rec = osc_expr_ast_funcall_getFuncRec(f);
		t_osc_atom_u *a = osc_atom_array_u_get(argv[0], i);
		t_osc_atom_u *b = osc_atom_array_u_get(argv[1], i);
		t_osc_atom_u *y = osc_atom_array_u_get(*out, i);
		t_osc_expr_builtin_binary_func f = (t_osc_expr_builtin_binary_func)osc_expr_funcrec_getFuncForTypetag(rec, osc_atom_u_getTypetag(a));
		if(f){
			int ret = f(a, b, y);
			if(ret){
				return ret;
			}
		}else{
			printf("type error\n");
			return 1;
		}
	}
	return 0;
}

//////////////////////////////////////////////////
// built in c functions
//////////////////////////////////////////////////
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

int _osc_expr_sign(double f)
{
	if(f == 0){
		return 0;
	}else if(f < 0){
		return -1;
	}else{
		return 1;
	}
}

//////////////////////////////////////////////////
// special functions
//////////////////////////////////////////////////
void printbndl(t_osc_bndl_u *bndl)
{
	long len = osc_bundle_u_nformat(NULL, 0, bndl, 0);
	char buf[len + 1];
	osc_bundle_u_nformat(buf, len + 1, bndl, 0);
	printf("%s\n", buf);
}

int _osc_expr_apply(t_osc_expr_ast_function *f, t_osc_expr_lexenv *lexenv, t_osc_expr_ast_expr *exprlist, t_osc_bndl_u *oscbndl, t_osc_atom_ar_u **out)
{
	while(exprlist){
		int ret = osc_expr_ast_expr_evalInLexEnv(exprlist, lexenv, oscbndl, out);
		exprlist = osc_expr_ast_expr_next(exprlist);
		if(exprlist){
			osc_atom_array_u_free(*out);
			*out = NULL;
		}
		if(ret){
			return ret;
		}
	}
	return 0;
}

// might want to copy this block into apply and map so that we don't have to copy the function unnecessarily
t_osc_expr_ast_function *_osc_expr_getFuncArg(t_osc_expr_ast_expr *e, t_osc_expr_lexenv *lexenv, t_osc_bndl_u *oscbndl)
{
	t_osc_atom_ar_u *ar = NULL;
	t_osc_expr_ast_function *f = NULL;
	int ret = osc_expr_ast_expr_evalInLexEnv(e, lexenv, oscbndl, &ar);
	if(ret || !ar){
		if(ar){
			osc_atom_array_u_free(ar);
		}
		return NULL;
	}
	t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
	if(!a){
		if(ar){
			osc_atom_array_u_free(ar);
		}
		return NULL;
	}
	switch(osc_atom_u_getTypetag(a)){
	case OSC_EXPR_TYPETAG:
		{
			t_osc_expr_ast_expr *ee = osc_atom_u_getExpr(a);
			if(osc_expr_ast_expr_getNodetype(ee) == OSC_EXPR_AST_NODETYPE_FUNCTION){
				f = (t_osc_expr_ast_function *)ee;
			}
		}
		break;
	default:
		printf("found a '%c' (%d) as the first arg to apply()\n", osc_atom_u_getTypetag(a), osc_atom_u_getTypetag(a));
		return NULL;
	}
	t_osc_expr_ast_function *copy = (t_osc_expr_ast_function *)osc_expr_ast_expr_copy((t_osc_expr_ast_expr *)f);
	if(ar){
		osc_atom_array_u_free(ar);
	}
	return copy;
}

void _osc_expr_makeLexEnvForFunc(t_osc_expr_ast_function *f, t_osc_expr_ast_expr *exprs_to_bind, t_osc_expr_lexenv *lexenv)
{
	t_osc_expr_ast_value *lambdalist = osc_expr_ast_function_getLambdaList(f);
	//osc_expr_lexenv_copy(&new_lexenv, current_lexenv);
	int i = 0;
	while(lambdalist && exprs_to_bind){
		t_osc_atom_u *param = osc_expr_ast_value_getIdentifier(lambdalist);
		osc_expr_lexenv_bind(lexenv, osc_atom_u_getStringPtr(param), exprs_to_bind);
		lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
		exprs_to_bind = osc_expr_ast_expr_next(exprs_to_bind);
	}
	// bind any remaining parameters to NULL
	while(lambdalist){
		t_osc_atom_u *param = osc_expr_ast_value_getIdentifier(lambdalist);
		osc_expr_lexenv_bind(lexenv, osc_atom_u_getStringPtr(param), NULL);
		lambdalist = (t_osc_expr_ast_value *)osc_expr_ast_expr_next((t_osc_expr_ast_expr *)lambdalist);
	}
}

int osc_expr_specFunc_apply(t_osc_expr_ast_funcall *f, 
			    t_osc_expr_lexenv *lexenv, 
			    t_osc_bndl_u *oscbndl,
			    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_expr *first_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *rest_expr = osc_expr_ast_expr_next(first_expr);
	t_osc_expr_ast_function *func = _osc_expr_getFuncArg(first_expr, lexenv, oscbndl);
	if(!func){
		// error
		return 1;
	}

	t_osc_expr_lexenv *my_lexenv = osc_expr_lexenv_alloc();
	osc_expr_lexenv_copy(&my_lexenv, lexenv);
	_osc_expr_makeLexEnvForFunc(func, rest_expr, my_lexenv);

	t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprs(func);
	int ret =  _osc_expr_apply(func, my_lexenv, exprlist, oscbndl, out);
	osc_expr_lexenv_free(my_lexenv);
	osc_expr_ast_function_free((t_osc_expr_ast_expr *)func);
	return ret;
}

int osc_expr_specFunc_map(t_osc_expr_ast_funcall *f, 
			    t_osc_expr_lexenv *lexenv, 
			    t_osc_bndl_u *oscbndl,
			    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_expr *first_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *rest_expr = osc_expr_ast_expr_next(first_expr);
	t_osc_expr_ast_function *func = _osc_expr_getFuncArg(first_expr, lexenv, oscbndl);
	if(!func){
		// error
		return 1;
	}
	int n = osc_expr_ast_funcall_getNumArgs(f) - 1;
	t_osc_atom_ar_u *ar[n];
	memset(ar, '\0', n * sizeof(t_osc_atom_ar_u *));
	t_osc_expr_ast_expr *argv[n];
	int i = 0;
	unsigned int min = ~0;
	while(rest_expr){
		int ret = osc_expr_ast_expr_evalInLexEnv(rest_expr, lexenv, oscbndl, ar + i);
		if(ret){
			// CLEANUP
			return 1;
		}
		argv[i] = (t_osc_expr_ast_expr *)osc_expr_ast_value_allocLiteral(NULL);
		if(i > 0){
			osc_expr_ast_expr_append(argv[i - 1], argv[i]);
		}
		int nn = osc_atom_array_u_getLen(ar[i]);
		if(nn < min){
			min = nn;
		}
		rest_expr = osc_expr_ast_expr_next(rest_expr);
		//i++;
	}
	t_osc_atom_ar_u *out_tmp[min];
	memset(out_tmp, '\0', min * sizeof(t_osc_atom_ar_u*));
	t_osc_expr_lexenv *my_lexenv = osc_expr_lexenv_alloc();	
	int outlen = 0;
	for(int i = 0; i < min; i++){
		for(int j = 0; j < n; j++){
			osc_expr_ast_value_setLiteral((t_osc_expr_ast_value *)argv[j], osc_atom_array_u_get(ar[j], i));
		}
		_osc_expr_makeLexEnvForFunc(func, argv[0], my_lexenv);
		t_osc_expr_ast_expr *exprlist = osc_expr_ast_function_getExprs(func);
		int ret =  _osc_expr_apply(func, my_lexenv, exprlist, oscbndl, out_tmp + i);
		if(ret){
			// CLEANUP
			return 1;
		}
		outlen += osc_atom_array_u_getLen(out_tmp[i]);
	}
	*out = osc_atom_array_u_alloc(outlen);
	int pos = 0;
	for(int i = 0; i < min; i++){
		osc_atom_array_u_copyInto(out, out_tmp[i], pos);
		pos += osc_atom_array_u_getLen(out_tmp[i]);
		osc_atom_array_u_free(out_tmp[i]);
	}
	osc_expr_lexenv_free(my_lexenv);
	osc_expr_ast_function_free((t_osc_expr_ast_expr *)func);
	for(int i = 0; i < n; i++){
		if(ar[i]){
			osc_atom_array_u_free(ar[i]);
		}
		if(argv[i]){
			osc_expr_ast_value_setLiteral((t_osc_expr_ast_value *)argv[i], NULL);
			osc_expr_ast_expr_free(argv[i]);
		}
	}
	return 0;
}

int osc_expr_specFunc_lreduce(t_osc_expr_ast_funcall *f, 
			      t_osc_expr_lexenv *lexenv, 
			      t_osc_bndl_u *oscbndl,
			      t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_specFunc_assign(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	if(!oscbndl){
		return 1;
	}
	t_osc_msg_u *at = NULL;
	t_osc_atom_u **ar = NULL;
	long len = 0;
	t_osc_expr_ast_expr *lval = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)f);
	t_osc_expr_ast_expr *rval = osc_expr_ast_expr_next(lval);
	t_osc_err ret = 0;
	switch(osc_expr_ast_expr_getNodetype(lval)){
	case OSC_EXPR_AST_NODETYPE_VALUE:
		{
			osc_expr_ast_expr_evalLvalInLexEnv(lval, lexenv, oscbndl, &at, &len, &ar);
			osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
			if(at){
				osc_message_u_clearArgs(at);
				osc_message_u_setArgArrayCopy(at, *out);
			}else{
				t_osc_msg_u *m = osc_message_u_allocWithArray(osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress((t_osc_expr_ast_value *)lval)), *out);
				osc_bundle_u_addMsg(oscbndl, m);
			}
		}
		break;
	case OSC_EXPR_AST_NODETYPE_ARRAYSUBSCRIPT:
		{
			int ret = osc_expr_ast_expr_evalLvalInLexEnv(lval, lexenv, oscbndl, &at, &len, &ar);
			if(at){
				int ret = osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
				for(int i = 0; i < len; i++){
					osc_atom_u_copyValue(ar[i], osc_atom_array_u_get(*out, i));
				}
			}else{
				// error
				ret = 1;
				goto cleanup;
			}
		}
		break;
	case OSC_EXPR_AST_NODETYPE_FUNCALL:
	case OSC_EXPR_AST_NODETYPE_BINARYOP:
		{
			t_osc_expr_ast_expr *llval = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)lval);
			t_osc_expr_ast_expr *rlval = osc_expr_ast_expr_next(llval);
			osc_expr_ast_expr_evalLvalInLexEnv(llval, lexenv, oscbndl, &at, &len, &ar);
			char *address = osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress((t_osc_expr_ast_value *)rlval));
			if(at){
				osc_expr_ast_expr_evalInLexEnv(rval, lexenv, oscbndl, out);
				t_osc_bndl_u *b = NULL;
				for(int i = 0; i < len; i++){
					if(osc_atom_u_getTypetag(ar[i]) == OSC_BUNDLE_TYPETAG){
						b = osc_atom_u_getBndl(ar[i]);
						break;
					}
				}
				if(!b){
					b = osc_bundle_u_alloc();
					osc_message_u_appendBndl_u(at, b, 1);
				}
				t_osc_msg_u *m = osc_message_u_allocWithAddress(address);
				for(int i = 0; i < osc_atom_array_u_getLen(*out); i++){
					t_osc_atom_u *copy = NULL;
					osc_atom_u_copy(&copy, osc_atom_array_u_get(*out, i));
					osc_message_u_appendAtom(m, copy);
				}
				osc_bundle_u_addMsg(b, m);
			}else{
				t_osc_bndl_u *b = osc_bundle_u_alloc();
				t_osc_atom_u *a = osc_atom_u_alloc();
				osc_atom_u_setBndl_u(a, b, 1);
				t_osc_expr_ast_value *v = osc_expr_ast_value_allocLiteral(a);
				t_osc_expr_ast_funcall *assign = osc_expr_ast_funcall_alloc(osc_expr_builtin_func_assign, 2, osc_expr_ast_expr_copy(llval), v);
				t_osc_atom_array_u *tmp = NULL;
				ret = osc_expr_specFunc_assign(assign, lexenv, oscbndl, &tmp);
				if(tmp){
					osc_atom_array_u_free(tmp);
				}
				osc_expr_ast_funcall_free((t_osc_expr_ast_expr *)assign);
				if(ret){
					goto cleanup;
				}
				ret = osc_expr_specFunc_assign(f, lexenv, oscbndl, out);
			}
		}
		break;
	}
 cleanup:
	if(ar){
		osc_mem_free(ar);
	}
	return ret;
}

/*
Thread 0 Crashed:: Dispatch queue: com.apple.main-thread
0   libsystem_kernel.dylib        	0x00007fff8d8aa212 __pthread_kill + 10
1   libsystem_c.dylib             	0x00007fff972cab24 pthread_kill + 90
2   libsystem_c.dylib             	0x00007fff9730ef61 abort + 143
3   libsystem_c.dylib             	0x00007fff972e2989 free + 392
4   edu.cnmat.berkeley.o.expr     	0x0000000110325e6d osc_array_free + 29
5   edu.cnmat.berkeley.o.expr     	0x00000001103332a0 osc_expr_specFunc_if + 432
6   edu.cnmat.berkeley.o.expr     	0x0000000110329ad4 osc_expr_ast_funcall_evalInLexEnv + 292
7   edu.cnmat.berkeley.o.expr     	0x0000000110332e0c osc_expr_specFunc_assign + 172
8   edu.cnmat.berkeley.o.expr     	0x0000000110329a20 osc_expr_ast_funcall_evalInLexEnv + 112
9   edu.cnmat.berkeley.o.expr     	0x0000000110328cab osc_expr_ast_expr_eval + 91
10  edu.cnmat.berkeley.o.expr     	0x00000001103184a9 oexpr_fullPacket + 313
 */

int osc_expr_specFunc_if(t_osc_expr_ast_funcall *f, 
			    t_osc_expr_lexenv *lexenv, 
			    t_osc_bndl_u *oscbndl,
			    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_expr *test_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *then_expr = osc_expr_ast_expr_next(test_expr);
	t_osc_expr_ast_expr *else_expr = osc_expr_ast_expr_next(then_expr);
	t_osc_atom_ar_u *test_ar = NULL;
	int ret = osc_expr_ast_expr_evalInLexEnv(test_expr, lexenv, oscbndl, &test_ar);
	if(ret){
		// report error
		if(test_ar){
			osc_atom_array_u_free(test_ar);
		}
	}
	int test_result_len = osc_atom_array_u_getLen(test_ar);
	t_osc_atom_ar_u *branch_vals[test_result_len];
	memset(branch_vals, '\0', test_result_len * sizeof(t_osc_atom_ar_u *));
	int outlen = 0;
	for(int i = 0; i < osc_atom_array_u_getLen(test_ar); i++){
		if(osc_atom_u_getInt32(osc_atom_array_u_get(test_ar, i))){
			ret = osc_expr_ast_expr_evalInLexEnv(then_expr, lexenv, oscbndl, branch_vals + i);
			if(ret){
				// report error
				goto out;
			}
			outlen += osc_atom_array_u_getLen(branch_vals[i]);
		}else{
			ret = osc_expr_ast_expr_evalInLexEnv(else_expr, lexenv, oscbndl, branch_vals + i);
			if(ret){
				// report error
				goto out;
			}
			outlen += osc_atom_array_u_getLen(branch_vals[i]);
		}
	}
	*out = osc_atom_array_u_alloc(outlen);
	int j = 0;
	for(int i = 0; i < test_result_len; i++){
		osc_atom_array_u_copyInto(out, branch_vals[i], j);
		j += osc_atom_array_u_getLen(branch_vals[i]);
	}
 out:
	for(int i = 0; i < test_result_len; i++){
		if(branch_vals[i]){
			osc_atom_array_u_free(branch_vals[i]);
		}
	}
	osc_atom_array_u_free(test_ar);
	return 0;
}

int osc_expr_specFunc_lval_lookup(t_osc_expr_ast_funcall *f, 
				  t_osc_expr_lexenv *lexenv, 
				  t_osc_bndl_u *oscbndl,
				  t_osc_msg_u **assign_target,
				  long *nlvals,
				  t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_expr *lhs = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)f);
	t_osc_expr_ast_expr *rhs = osc_expr_ast_expr_next(lhs);
	long l = osc_expr_ast_expr_format(NULL, 0, rhs);
	char buf[l + 1];
	osc_expr_ast_expr_format(buf, l + 1, rhs);
	t_osc_bndl_u *b = NULL;
	//if(lhs == (t_osc_expr_ast_expr *)osc_expr_keyword_this){
	if((osc_expr_ast_expr_getNodetype(lhs) == OSC_EXPR_AST_NODETYPE_VALUE) &&
	   osc_expr_ast_value_getValueType((t_osc_expr_ast_value *)lhs) == OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER){
		t_osc_atom_u *a = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)lhs);
		if((osc_atom_u_getTypetag(a) == 's') && !strncmp(osc_atom_u_getStringPtr(a), "this", 4)){
			b = oscbndl;
		}
	}else{
		t_osc_msg_u *at = NULL;
		t_osc_atom_u **ar = NULL;
		long len = 0;
		osc_expr_ast_expr_evalLvalInLexEnv(lhs, lexenv, oscbndl, &at, &len, &ar);
		if(ar){
			for(int i = 0; i < len; i++){
				t_osc_atom_u *a = ar[i];
				if(osc_atom_u_getTypetag(a) == OSC_BUNDLE_TYPETAG){
					b = osc_atom_u_getBndl(a);
					break;
				}
			}
		}else{
			return 1;
		}
		osc_mem_free(ar);
	}
	if(b){
		int i = osc_expr_ast_expr_evalLvalInLexEnv(rhs, lexenv, b, assign_target, nlvals, lvals);
		t_osc_atom_u *a = lvals[0][0];
		long l = osc_atom_u_nformat(NULL, 0, a, 1);
		char buf[l + 1];
		osc_atom_u_nformat(buf, l + 1, a, 1);
		return i;
	}else{
		return 1;
	}
	return 0;
}

int osc_expr_specFunc_lookup(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	t_osc_msg_u *assign_target = NULL;
	long nlvals = 0;
	t_osc_atom_u **lvals = NULL;
	long l = osc_expr_ast_expr_format(NULL, 0, f);
	char buf[l + 1];
	osc_expr_ast_expr_format(buf, l + 1, f);
	osc_expr_specFunc_lval_lookup(f, lexenv, oscbndl, &assign_target, &nlvals, &lvals);
	if(lvals){
		*out = osc_atom_array_u_alloc(nlvals);
		for(int i = 0; i < nlvals; i++){
			t_osc_atom_u *dest = osc_atom_array_u_get(*out, i);
			osc_atom_u_copy(&dest, lvals[i]);
		}
		osc_mem_free(lvals);
	}else{
		return 1;
	}
	return 0;
}

// nth doesn't seem like it needs to be a special function, but it does
// because one of its arguments needs to be evaluated in an lval context
// and the other must not be. 
int osc_expr_specFunc_lval_nth(t_osc_expr_ast_funcall *f, 
			       t_osc_expr_lexenv *lexenv, 
			       t_osc_bndl_u *oscbndl,
			       t_osc_msg_u **assign_target,
			       long *nlvals,
			       t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_expr *vals_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *idxs_expr = osc_expr_ast_expr_next(vals_expr);
	long nlvals_tmp = 0;
	int ret = osc_expr_ast_expr_evalLvalInLexEnv(vals_expr, lexenv, oscbndl, assign_target, &nlvals_tmp, lvals);
	if(ret){
		return ret;
	}
	if(!(*lvals)){
		// error
		return 1;
	}
	t_osc_atom_ar_u *idxs = NULL;
	osc_expr_ast_expr_evalInLexEnv(idxs_expr, lexenv, oscbndl, &idxs);
	if(!idxs){
		// error
		return 1;
	}
	long nidxs = osc_atom_array_u_getLen(idxs);
	*nlvals = nidxs;
	t_osc_atom_u *lvals_tmp[nlvals_tmp];
	memcpy(lvals_tmp, *lvals, nlvals_tmp * sizeof(t_osc_atom_u *));
	memset(*lvals, '\0', nlvals_tmp * sizeof(t_osc_atom_u*));
	for(int i = 0; i < nidxs; i++){
		int idx = osc_atom_u_getInt(osc_atom_array_u_get(idxs, i));
		if(idx >= nlvals_tmp){
			// index out of bounds error
			return 1;
		}
		(*lvals)[i] = lvals_tmp[idx];
	}
	osc_atom_array_u_free(idxs);
	return 0;
}

int osc_expr_specFunc_nth(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	// can't just call osc_expr_specFunc_lval_nth() because this may be a list literal or something
	// that's not part of the bundle
	t_osc_expr_ast_expr *vals_expr = osc_expr_ast_funcall_getArgs(f);
	t_osc_expr_ast_expr *idxs_expr = osc_expr_ast_expr_next(vals_expr);
	t_osc_atom_ar_u *vals = NULL;
	osc_expr_ast_expr_evalInLexEnv(vals_expr, lexenv, oscbndl, &vals);
	if(!vals){
		// error 
		return 1;
	}
	t_osc_atom_ar_u *idxs = NULL;
	osc_expr_ast_expr_evalInLexEnv(idxs_expr, lexenv, oscbndl, &idxs);
	if(!idxs){
		// error 
		return 1;
	}
	int nvals = osc_atom_array_u_getLen(vals);
	int n = osc_atom_array_u_getLen(idxs);
	*out = osc_atom_array_u_alloc(n);
	for(int i = 0; i < n; i++){
		int idx = osc_atom_u_getInt(osc_atom_array_u_get(idxs, i));
		if(idx >= nvals){
			// index out of bounds error
			return 1;
		}
		t_osc_atom_u *a = osc_atom_array_u_get(*out, i);
		osc_atom_u_copyValue(a, osc_atom_array_u_get(vals, idx));
	}
	osc_atom_array_u_free(vals);
	osc_atom_array_u_free(idxs);
	return 0;
}

int osc_expr_specFunc_emptybundle(t_osc_expr_ast_funcall *f, 
				  t_osc_expr_lexenv *lexenv, 
				  t_osc_bndl_u *oscbndl,
				  t_osc_atom_ar_u **out)
{
	*out = osc_atom_array_u_alloc(1);
			
	if(osc_bundle_u_getMsgCount(oscbndl)){
		osc_atom_u_setTrue(osc_atom_array_u_get(*out, 0));
	}else{
		osc_atom_u_setFalse(osc_atom_array_u_get(*out, 0));
	}
	return 0;
}

int osc_expr_specFunc_bundle(t_osc_expr_ast_funcall *f, 
			     t_osc_expr_lexenv *lexenv, 
			     t_osc_bndl_u *oscbndl,
			     t_osc_atom_ar_u **out)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_expr_ast_funcall *msg = (t_osc_expr_ast_funcall *)osc_expr_ast_funcall_getArgs(f);
	while(msg){
		t_osc_msg_u *m = osc_message_u_alloc();
		t_osc_expr_ast_expr *args = osc_expr_ast_funcall_getArgs(msg);
		if(osc_expr_ast_expr_getNodetype(args) != OSC_EXPR_AST_NODETYPE_VALUE){
			return 1;
		}
		t_osc_expr_ast_value *address = (t_osc_expr_ast_value *)args;
		osc_message_u_setAddress(m, osc_atom_u_getStringPtr(osc_expr_ast_value_getOSCAddress(address)));
		args = osc_expr_ast_expr_next(args);
		while(args){
			t_osc_atom_ar_u *ar = NULL;
			osc_expr_ast_expr_evalInLexEnv(args, lexenv, oscbndl, &ar);
			if(ar){
				osc_message_u_setArgArrayCopy(m, ar);
				osc_atom_array_u_free(ar);
			}
			args = osc_expr_ast_expr_next(args);
		}
		osc_bundle_u_addMsg(b, m);
		msg = osc_expr_ast_expr_next((t_osc_expr_ast_expr *)msg);
	}
	*out = osc_atom_array_u_alloc(1);
	osc_atom_u_setBndl_u(osc_atom_array_u_get(*out, 0), b, 1);
	return 0;
}
/*
int osc_expr_specFunc_existsorbound(t_osc_expr_ast_funcall *f,
				    t_osc_expr_lexenv *lexenv,
				    t_osc_bndl_u *oscbndl,
				    t_osc_atom_ar_u **out,
				    t_osc_err (*func)(long, char*, char*, int, int*))
{
	t_osc_expr_ast_expr *arg = osc_expr_ast_funcall_getArgs(f);
	int type = osc_expr_ast_expr_getNodetype(arg);
	char *address = NULL;

	// if arg is an OSC address, get the address as a string, otherwise eval the arg
	// and see if it returns a string
	switch(type){
	case OSC_EXPR_ARG_TYPE_ATOM:
		{
			t_osc_atom_u *a = osc_expr_arg_getOSCAtom(f_argv);
			if(osc_atom_u_getTypetag(a) == 's'){
				t_osc_atom_ar_u *ar = NULL;
				osc_expr_evalArgInLexEnv(f_argv, lexenv, len, oscbndl, &ar);
				if(ar){
					t_osc_atom_u *aa = osc_atom_array_u_get(ar, 0);
					if(osc_atom_u_getTypetag(aa) == 's'){
						osc_atom_u_getString(aa, 0, &address);
					}
					osc_atom_array_u_free(ar);
				}
			}
		}
		break;
	case OSC_EXPR_ARG_TYPE_OSCADDRESS:
		{
			char *tmp = osc_expr_arg_getOSCAddress(f_argv);
			osc_util_strdup(&address, tmp);
		}
		break;
	case OSC_EXPR_ARG_TYPE_EXPR:
		{
			// this could be value(/foo)
			t_osc_atom_ar_u *ar = NULL;
			int err = osc_expr_evalInLexEnv(osc_expr_arg_getExpr(f_argv), lexenv, len, oscbndl, &ar);
			if(!err && ar){
				if(osc_atom_array_u_getLen(ar) != 1){
					goto err;
				}
				t_osc_atom_u *a = osc_atom_array_u_get(ar, 0);
				if(!a){
					goto err;
				}
				if(osc_atom_u_getTypetag(a) != 's'){
					goto err;
				}
				osc_atom_u_getString(a, 0, &address);
				if(!address){
					goto err;
				}
				if(address[0] != '/'){
					goto err;
				}
			}
		}
		break;
	}
	if(address){
		*out = osc_atom_array_u_alloc(1);
		int res = 0;
		func(*len, *oscbndl, address, 1, &res);
		osc_atom_u_setBool(osc_atom_array_u_get(*out, 0), res);
		osc_mem_free(address);
		return 0;
	}
 err:
	osc_error(OSC_ERR_EXPR_ARGCHK, "arg 1 should be an OSC address");
	return 1;

}

int osc_expr_specFunc_bound(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out)
{
	return osc_expr_specFunc_existsorbound(f, lexenv, len, oscbndl, out, osc_bundle_s_addressIsBound);
}

int osc_expr_specFunc_exists(t_osc_expr *f,
			    t_osc_expr_lexenv *lexenv,
			    long *len,
			    char **oscbndl,
			    t_osc_atom_ar_u **out)
{
	return osc_expr_specFunc_existsorbound(f, lexenv, len, oscbndl, out, osc_bundle_s_addressExists);
}
*/
// these are dummy functions---they're used to do a pointer comparison in osc_expr_ast_funcall_evalInLexEnv
OSC_EXPR_BUILTIN_DECL(apply){return 0;}
OSC_EXPR_BUILTIN_DECL(map){return 0;}
OSC_EXPR_BUILTIN_DECL(lreduce){return 0;}
OSC_EXPR_BUILTIN_DECL(rreduce){return 0;}
OSC_EXPR_BUILTIN_DECL(assign){return 0;}
OSC_EXPR_BUILTIN_DECL(if){return 0;}
OSC_EXPR_BUILTIN_DECL(emptybundle){return 0;}
OSC_EXPR_BUILTIN_DECL(bound){return 0;}
OSC_EXPR_BUILTIN_DECL(exists){return 0;}
OSC_EXPR_BUILTIN_DECL(getaddresses){return 0;}
OSC_EXPR_BUILTIN_DECL(delete){return 0;}
OSC_EXPR_BUILTIN_DECL(getmsgcount){return 0;}
OSC_EXPR_BUILTIN_DECL(value){return 0;}
OSC_EXPR_BUILTIN_DECL(quote){return 0;}
OSC_EXPR_BUILTIN_DECL(tokenize){return 0;}
OSC_EXPR_BUILTIN_DECL(gettimetag){return 0;}
OSC_EXPR_BUILTIN_DECL(settimetag){return 0;}
OSC_EXPR_BUILTIN_DECL(lookup){return 0;}
OSC_EXPR_BUILTIN_LVAL_DECL(lookup){return 0;}
OSC_EXPR_BUILTIN_DECL(nth){return 0;}
OSC_EXPR_BUILTIN_LVAL_DECL(nth){return 0;}
OSC_EXPR_BUILTIN_DECL(message){return 0;}
OSC_EXPR_BUILTIN_DECL(bundle){return 0;}

/*
// special functions: bypass scalar expansion and type promotion and have access to the bundle
assign
nth
bound
exists
emptybundle
getaddresses
delete
getmsgcount
eval
apply
map
lreduce
rreduce
quote
lambda
gettimetag
settimetag
lookup
if

// typed builtins: builtins subject to scalar expansion and type promotion

// Num a => a -> a
math.h...

// Num a => (a, a) -> a
add
sub
mul
div
lt
le
gt
ge
eq
neq
and
or
mod
pow
min
max

//// (a, a, a) -> [a]
aseq

//// bool -> bool
not

//// [] -> int
length

//// [] -> []
reverse
sort
list

//// a -> [char]
typetags

//// [a] -> [a] -> a
dot

//// [a, a, a] -> [a, a, a] -> [a, a, a]
cross

//// [a] -> a
l2norm

//// string -> int
strcmp

//// string -> [string]
split
tokenize

//// a -> b
cast
float32
float64
int8
int16
int32
int64
uint8
uint16
uint32
uint64
string

// user defined: functions implemented in the expression language
add1: add(1)
sub1: sub(1)
product: lreduce(mul)
sum: lreduce(add)
cumsum: 
mean: lambda(l){sum(l) / length(l);}
median: 
nfill: lambda(n, v){map(lambda(x){v}, [0:n-1])}
range: lambda(l){max(l) - min(l)}
interleave: map(list)
first: nth(, 0)
last: lambda(l){nth(l, length(l) - 1)}
rest: lambda(l){nth(l, [1:length(l) - 1])}
butlast: lambda(l){nth(l, [0:length(l) - 2])}
extrema: 
clip:
scale:
mtof:
ftom:
sign:
identity: lambda(x){x}

// constants
pi
twopi
halfpi
quarterpi
oneoverpi
twooverpi
degtorad
radtodeg
e
lntwo
lnten
logtwoe
logtene
sqrttwo
sqrthalf

// deprecated/removed
assign_to_index
join
prog1
prog2
progn
value
 */
