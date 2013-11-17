/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-12, The Regents of
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

/** 	\file osc_expr_func.h
	\author John MacCallum

*/
#ifndef __OSC_EXPR_FUNC_H__
#define __OSC_EXPR_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <math.h>
#include "osc.h"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_expr_parser.h"


// don't fuck with these!
#define OSC_EXPR_ARG_TYPE_NUMBER 0x1
#define OSC_EXPR_ARG_TYPE_LIST 0x2
#define OSC_EXPR_ARG_TYPE_STRING 0x4
#define OSC_EXPR_ARG_TYPE_ATOM 0x8
#define OSC_EXPR_ARG_TYPE_EXPR 0x10
#define OSC_EXPR_ARG_TYPE_OSCADDRESS 0x20
#define OSC_EXPR_ARG_TYPE_BOOLEAN 0x40
#define OSC_EXPR_ARG_TYPE_FUNCTION 0x80
#define OSC_EXPR_ARG_TYPE_TIMETAG 0x100

#define OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS

#define OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR

#define OSC_EXPR_ARG_TYPE_ANYTHING OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS | OSC_EXPR_ARG_TYPE_BOOLEAN

int osc_expr_1arg_dbl(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_2arg_dbl_dbl(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_2arg(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_add(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_subtract(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_multiply(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_divide(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_lt(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_lte(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_gt(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_gte(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_eq(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_neq(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_and(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_or(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);
int osc_expr_mod(t_osc_atom_u *f1, t_osc_atom_u *f2, t_osc_atom_u **result);

int osc_expr_assign(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_add1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sub1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_nth(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_assign_to_index(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_product(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sum(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_cumsum(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_length(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_mean(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_median(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_reverse(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sort(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_list(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_nfill(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_range(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_aseq(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_interleave(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_first(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_last(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_rest(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_butlast(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_not(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_dot(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_cross(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_det(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_l2norm(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_min(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_max(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_extrema(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_clip(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_scale(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_mtof(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_ftom(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sign(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_if(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_strcmp(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_split(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_join(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_bound(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_exists(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_emptybundle(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_getaddresses(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_delete(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_getmsgcount(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_identity(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_eval_call(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_tokenize(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_compile(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_prog1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_prog2(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_progn(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_apply(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_map(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_lreduce(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_rreduce(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_quote(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_value(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_lambda(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_gettimetag(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_settimetag(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_lookup(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

// constants
int osc_expr_pi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_twopi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_halfpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_quarterpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_oneoverpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_twooverpi(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_degtorad(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_radtodeg(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_e(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_lntwo(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_lnten(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_logtwoe(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_logtene(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sqrttwo(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_sqrthalf(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_explicitCast(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_explicitCast_float32(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_float64(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_int8(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_int16(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_int32(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_int64(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_uint8(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_uint16(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_uint32(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_uint64(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_bool(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_string(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_dynamic(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_typetags(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

//////////////////////////////////////////////////
// records for simple operators
//////////////////////////////////////////////////
#define OSC_EXPR_REC_OP_ADD {"+",\
	 "/result = $1 + $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/string/operator", NULL},\
	 "Add",\
	 osc_expr_2arg,\
	 (void *)osc_expr_add,\
	 '+'}
#define OSC_EXPR_REC_OP_SUB {"-",\
	 "/result = $1 - $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/string/operator", NULL},\
	 "Subtract",\
	 osc_expr_2arg,\
	 (void *)osc_expr_subtract,\
	 '-'}
#define OSC_EXPR_REC_OP_MUL {"*",\
	 "/result = $1 * $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", NULL},\
	 "Multiply",\
	 osc_expr_2arg,\
	 (void *)osc_expr_multiply,\
	 '*'}
#define OSC_EXPR_REC_OP_DIV {"/",\
	 "/result = $1 / $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", NULL},\
	 "Divide",\
	 osc_expr_2arg,\
	 (void *)osc_expr_divide,\
	 '/'}
#define OSC_EXPR_REC_OP_LT {"<",\
	 "/result = $1 < $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Less than",\
	 osc_expr_2arg,\
	 (void *)osc_expr_lt,\
	 '<'}
#define OSC_EXPR_REC_OP_LE {"<=",\
	 "/result = $1 <= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Less than or equal to",\
	 osc_expr_2arg,\
	 (void *)osc_expr_lte,\
	 OSC_EXPR_LTE}
#define OSC_EXPR_REC_OP_GT {">",\
	 "/result = $1 > $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Greater than",\
	 osc_expr_2arg,\
	 (void *)osc_expr_gt,\
	 '>'}
#define OSC_EXPR_REC_OP_GE {">=",\
	 "/result = $1 >= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Greater than or equal to",\
	 osc_expr_2arg,\
	 (void *)osc_expr_gte,\
	 OSC_EXPR_GTE}
#define OSC_EXPR_REC_OP_EQ {"==",\
	 "/result = $1 == $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Equal",\
	 osc_expr_2arg,\
	 (void *)osc_expr_eq,\
	 OSC_EXPR_EQ}
#define OSC_EXPR_REC_OP_NEQ {"!=",\
	 "/result = $1 != $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", "/string/operator", NULL},\
	 "Not equal",\
	 osc_expr_2arg,\
	 (void *)osc_expr_neq,\
	 OSC_EXPR_NEQ}
#define OSC_EXPR_REC_OP_AND {"&&",\
	 "/result = $1 && $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/logical", NULL},\
	 "Logical and",\
	 osc_expr_2arg,\
	 (void *)osc_expr_and,\
	 OSC_EXPR_AND}
#define OSC_EXPR_REC_OP_OR {"||",\
	 "/result = $1 || $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/logical", NULL},\
	 "Logical or",\
	 osc_expr_2arg,\
	 (void *)osc_expr_or,\
	 OSC_EXPR_OR}
#define OSC_EXPR_REC_OP_MOD {"%",\
	 "/result = $1 % $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", NULL},\
	 "Modulo",\
	 osc_expr_2arg,\
	 (void *)osc_expr_mod,\
	 '%'}
#define OSC_EXPR_REC_OP_POW {"^",\
	 "/result = $1 ^ $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", NULL},\
	 "Power",\
	 osc_expr_2arg_dbl_dbl,\
	 (void *)pow,\
	 '^'}
#define OSC_EXPR_REC_OP_ASSIGN {"=",\
	 "/result = $1",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_ANYTHING},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/assignment", NULL},\
	 "Assignment",\
	 osc_expr_assign,\
	 NULL,\
	 '='}
#define OSC_EXPR_REC_OP_NOT {"!",\
	 "/result = !$1",\
	 1,\
	 0,\
	 (char *[]){"argument"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/logical", NULL},\
	 "Logical not",\
	 osc_expr_not,\
	 NULL,\
	 '!'}
#define OSC_EXPR_REC_OP_LOOKUP {".",\
	 "/result = /foo./bar",\
	 2,\
	 0,\
	 (char *[]){"address of message containing a subbundle", "address to lookup in subbundle"}, \
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_OSCADDRESS},	\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/core", NULL},\
	 "Lookup addresses in subbundles",\
	 osc_expr_lookup,\
	 NULL,\
	 '.'}
#define OSC_EXPR_REC_OP_NULLCOALESCE {"??",\
	 "/result = $1 ?? $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", NULL},\
	 "Null coalescing operator, returns the left operand if it exists, otherwise it returns the right.",\
	 NULL,\
	 NULL,\
	 0}

//////////////////////////////////////////////////
// records for functional equivalents of simple 
// unary and binary operators
//////////////////////////////////////////////////
#define OSC_EXPR_REC_ADD {"add",\
	 "/result = add($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Add",\
	 osc_expr_2arg,\
	 (void *)osc_expr_add,\
	 '+'}
#define OSC_EXPR_REC_SUB {"sub",\
	 "/result = sub($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Subtract",\
	 osc_expr_2arg,\
	 (void *)osc_expr_subtract,\
	 '-'}
#define OSC_EXPR_REC_MUL {"mul",\
	 "/result = mul($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Multiply",\
	 osc_expr_2arg,\
	 (void *)osc_expr_multiply,\
	 '*'}
#define OSC_EXPR_REC_DIV {"div",\
	 "/result = div($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Add",\
	 osc_expr_2arg,\
	 (void *)osc_expr_divide,\
	 '/'}
#define OSC_EXPR_REC_LT {"lt",\
	 "/result = lt($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Less than",\
	 osc_expr_2arg,\
	 (void *)osc_expr_lt,\
	 '<'}
#define OSC_EXPR_REC_LE {"le",\
	 "/result = le($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Less than or equal",\
	 osc_expr_2arg,\
	 (void *)osc_expr_lte,\
	 OSC_EXPR_LTE}
#define OSC_EXPR_REC_GT {"gt",\
	 "/result = gt($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Greater than",\
	 osc_expr_2arg,\
	 (void *)osc_expr_gt,\
	 '>'}
#define OSC_EXPR_REC_GE {"ge",\
	 "/result = ge($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Greater than or equal",\
	 osc_expr_2arg,\
	 (void *)osc_expr_gte,\
	 OSC_EXPR_GTE}
#define OSC_EXPR_REC_EQ {"eq",\
	 "/result = eq($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Equality",\
	 osc_expr_2arg,\
	 (void *)osc_expr_eq,\
	 OSC_EXPR_EQ}
#define OSC_EXPR_REC_NEQ {"ne",\
	 "/result = ne($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", "/string/function", NULL},\
	 "Not equal",\
	 osc_expr_2arg,\
	 (void *)osc_expr_neq,\
	 OSC_EXPR_NEQ}
#define OSC_EXPR_REC_AND {"and",\
	 "/result = and($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Logical and",\
	 osc_expr_2arg,\
	 (void *)osc_expr_and,\
	 OSC_EXPR_AND}
#define OSC_EXPR_REC_OR {"or",\
	 "/result = or($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Logical or",\
	 osc_expr_2arg,\
	 (void *)osc_expr_or,\
	 OSC_EXPR_OR}
#define OSC_EXPR_REC_MOD {"mod",\
	 "/result = mod($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Modulo",\
	 osc_expr_2arg,\
	 (void *)osc_expr_mod,\
	 '%'}
#define OSC_EXPR_REC_POW {"pow",\
	 "/result = pow($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"x", "y"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/power", NULL},\
	 "Power",\
	 osc_expr_2arg_dbl_dbl,\
	 (void *)pow,\
	 '^'}
#define OSC_EXPR_REC_ASSIGN {"assign",\
	 "assign(/result, $2)",\
	 2,\
	 0,\
	 (char *[]){"arg 1", "arg 2"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_ANYTHING},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Assignment",\
	 osc_expr_assign,\
	 NULL,\
	 '='}
#define OSC_EXPR_REC_NOT {"not",\
	 "not(/result)",\
	 1,\
	 0,\
	 (char *[]){"arg 1"},\
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/arithmetic", NULL},\
	 "Not",\
	 osc_expr_not,\
	 NULL,\
	 '!'}
#define OSC_EXPR_REC_LOOKUP {"lookup",\
	 "/result = lookup(/address1, /address2)",\
	 2,\
	 0,\
	 (char *[]){"address of message containing a subbundle", "address to lookup in subbundle"}, \
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_OSCADDRESS},	\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/core", NULL},\
	 "Lookup addresses in subbundles",\
	 osc_expr_lookup,\
	 NULL,\
	 '.'}
#define OSC_EXPR_REC_NULLCOALESCE {"nullcoalesce",\
	 "/result = nullcoalesce($1, $2)",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", NULL},\
	 "Null coalescing function, returns the first argument if it exists, otherwise it returns the second.",\
	 NULL,\
	 NULL,\
	 0}

//////////////////////////////////////////////////
// records for unary and binary compound assignment operators
//////////////////////////////////////////////////
#define OSC_EXPR_REC_OP_ADD1 {"++",\
	 "/result = $1++",\
	 1,\
	 0,\
	 (char *[]){"argument to be incremented"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Increment",\
	 osc_expr_add1,\
	 NULL,\
	 0}
#define OSC_EXPR_REC_OP_SUB1 {"--",\
	 "/result = $1--",\
	 1,\
	 0,\
	 (char *[]){"argument to be decremented"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Decrement",\
	 osc_expr_sub1,\
	 NULL,\
	 0}
#define OSC_EXPR_REC_OP_ADDASSIGN {"+=",\
	 "/result = $1 += $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Add and assign",\
	 osc_expr_2arg,\
	 (void *)osc_expr_add,\
	 0}
#define OSC_EXPR_REC_OP_SUBASSIGN {"-=",\
	 "/result = $1 -= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Subtract and assign",\
	 osc_expr_2arg,\
	 (void *)osc_expr_subtract,\
	 0}
#define OSC_EXPR_REC_OP_MULASSIGN {"*=",\
	 "/result = $1 *= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Multiply and assign",\
	 osc_expr_2arg,\
	 (void *)osc_expr_multiply,\
	 0}
#define OSC_EXPR_REC_OP_DIVASSIGN {"/=",\
	 "/result = $1 /= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Divide and assign",\
	 osc_expr_2arg,\
	 (void *)osc_expr_divide,\
	 0}
#define OSC_EXPR_REC_OP_MODASSIGN {"%=",\
	 "/result = $1 %= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Modulo and assign",\
	 osc_expr_2arg,\
	 (void *)osc_expr_mod,\
	 0}
#define OSC_EXPR_REC_OP_POWASSIGN {"^=",\
	 "/result = $1 ^= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/arithmetic", "/math/operator/assignment", NULL},\
	 "Power and assign",\
	 osc_expr_2arg,\
	 (void *)pow,\
	 0}
#define OSC_EXPR_REC_OP_NULLCOALESCEASSIGN {"??=",\
	 "/result = $1 ??= $2",\
	 2,\
	 0,\
	 (char *[]){"left operand", "right operand"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/math/operator/relational", NULL},\
	 "Null coalescing operator, returns the left operand if it exists, otherwise it returns the right.",\
	 NULL,\
	 NULL,\
	 0}

//////////////////////////////////////////////////
// other functions that have syntactic sugar and 
// need to be referenced without looking them up
//////////////////////////////////////////////////
#define OSC_EXPR_REC_NTH {"nth",\
	 "/result = nth($1, $2)",\
	 2,\
	 -1,\
	 (char *[]){"argument (list)", "index"},\
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},\
	 (char *[]){"additional indexes"},\
	 (int []){},\
	 (char *[]){"/vector", NULL},\
	 "Get an element of a list (same as [[ ]])",\
	 osc_expr_nth,\
	 NULL,\
	 0}
#define OSC_EXPR_REC_ASSIGN_TO_INDEX {"assign_to_index",\
	 "/result = assign_to_index($1, $2, $3)",\
	 3,\
	 0,\
	 (char *[]){"assignment target", "index(es)", "value(s)"},\
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},\
	 (char *[]){NULL},\
	 (int []){},\
	 (char *[]){"/vector", NULL},\
	 "Assign <arg3> to the indexes <arg2> of the address <arg1>",\
	 osc_expr_assign_to_index,\
	 NULL,\
	 0}

//////////////////////////////////////////////////
// records for simple binary and unary operators
//////////////////////////////////////////////////
static t_osc_expr_rec osc_expr_rec_op_add = OSC_EXPR_REC_OP_ADD;
static t_osc_expr_rec osc_expr_rec_op_sub = OSC_EXPR_REC_OP_SUB;
static t_osc_expr_rec osc_expr_rec_op_mul = OSC_EXPR_REC_OP_MUL;
static t_osc_expr_rec osc_expr_rec_op_div = OSC_EXPR_REC_OP_DIV;
static t_osc_expr_rec osc_expr_rec_op_lt = OSC_EXPR_REC_OP_LT;
static t_osc_expr_rec osc_expr_rec_op_le = OSC_EXPR_REC_OP_LE;
static t_osc_expr_rec osc_expr_rec_op_gt = OSC_EXPR_REC_OP_GT;
static t_osc_expr_rec osc_expr_rec_op_ge = OSC_EXPR_REC_OP_GE;
static t_osc_expr_rec osc_expr_rec_op_eq = OSC_EXPR_REC_OP_EQ;
static t_osc_expr_rec osc_expr_rec_op_neq = OSC_EXPR_REC_OP_NEQ;
static t_osc_expr_rec osc_expr_rec_op_and = OSC_EXPR_REC_OP_AND;
static t_osc_expr_rec osc_expr_rec_op_or = OSC_EXPR_REC_OP_OR;
static t_osc_expr_rec osc_expr_rec_op_mod = OSC_EXPR_REC_OP_MOD;
static t_osc_expr_rec osc_expr_rec_op_pow = OSC_EXPR_REC_OP_POW;
static t_osc_expr_rec osc_expr_rec_op_assign = OSC_EXPR_REC_OP_ASSIGN;
static t_osc_expr_rec osc_expr_rec_op_not = OSC_EXPR_REC_OP_NOT;
static t_osc_expr_rec osc_expr_rec_op_lookup = OSC_EXPR_REC_OP_LOOKUP;
static t_osc_expr_rec osc_expr_rec_op_nullcoalesce = OSC_EXPR_REC_OP_NULLCOALESCE;

//////////////////////////////////////////////////
// functional equivalents for simple operators
//////////////////////////////////////////////////
static t_osc_expr_rec osc_expr_rec_add = OSC_EXPR_REC_ADD;
static t_osc_expr_rec osc_expr_rec_sub = OSC_EXPR_REC_SUB;
static t_osc_expr_rec osc_expr_rec_mul = OSC_EXPR_REC_MUL;
static t_osc_expr_rec osc_expr_rec_div = OSC_EXPR_REC_DIV;
static t_osc_expr_rec osc_expr_rec_lt = OSC_EXPR_REC_LT;
static t_osc_expr_rec osc_expr_rec_le = OSC_EXPR_REC_LE;
static t_osc_expr_rec osc_expr_rec_gt = OSC_EXPR_REC_GT;
static t_osc_expr_rec osc_expr_rec_ge = OSC_EXPR_REC_GE;
static t_osc_expr_rec osc_expr_rec_eq = OSC_EXPR_REC_EQ;
static t_osc_expr_rec osc_expr_rec_neq = OSC_EXPR_REC_NEQ;
static t_osc_expr_rec osc_expr_rec_and = OSC_EXPR_REC_AND;
static t_osc_expr_rec osc_expr_rec_or = OSC_EXPR_REC_OR;
static t_osc_expr_rec osc_expr_rec_mod = OSC_EXPR_REC_MOD;
static t_osc_expr_rec osc_expr_rec_pow = OSC_EXPR_REC_POW;
static t_osc_expr_rec osc_expr_rec_assign = OSC_EXPR_REC_ASSIGN;
static t_osc_expr_rec osc_expr_rec_not = OSC_EXPR_REC_NOT;
static t_osc_expr_rec osc_expr_rec_lookup = OSC_EXPR_REC_LOOKUP;
static t_osc_expr_rec osc_expr_rec_nullcoalesce = OSC_EXPR_REC_NULLCOALESCE;

//////////////////////////////////////////////////
// records for unary and binary compound assignment operators
//////////////////////////////////////////////////
static t_osc_expr_rec osc_expr_rec_op_add1 = OSC_EXPR_REC_OP_ADD1;
static t_osc_expr_rec osc_expr_rec_op_sub1 = OSC_EXPR_REC_OP_SUB1;
static t_osc_expr_rec osc_expr_rec_op_addassign = OSC_EXPR_REC_OP_ADDASSIGN;
static t_osc_expr_rec osc_expr_rec_op_subassign = OSC_EXPR_REC_OP_SUBASSIGN;
static t_osc_expr_rec osc_expr_rec_op_mulassign = OSC_EXPR_REC_OP_MULASSIGN;
static t_osc_expr_rec osc_expr_rec_op_divassign = OSC_EXPR_REC_OP_DIVASSIGN;
static t_osc_expr_rec osc_expr_rec_op_modassign = OSC_EXPR_REC_OP_MODASSIGN;
static t_osc_expr_rec osc_expr_rec_op_powassign = OSC_EXPR_REC_OP_POWASSIGN;
static t_osc_expr_rec osc_expr_rec_op_nullcoalesceassign = OSC_EXPR_REC_OP_NULLCOALESCEASSIGN;

//////////////////////////////////////////////////
// other functions that have syntactic sugar and 
// need to be referred to without doing a lookup
//////////////////////////////////////////////////
static t_osc_expr_rec osc_expr_rec_nth = OSC_EXPR_REC_NTH;
static t_osc_expr_rec osc_expr_rec_assign_to_index = OSC_EXPR_REC_ASSIGN_TO_INDEX;


// this array can be indexed by either the ascii char of an operator,
// or in the case of multi character operators such as <=, by the 
// token defined in osc_expr_parser.y
static t_osc_expr_rec *osc_expr_func_opcodeToOpRec[128] = {
	NULL	,// 0
	&osc_expr_rec_op_eq	,
	&osc_expr_rec_op_neq	,
	&osc_expr_rec_op_le	,
	&osc_expr_rec_op_ge	,
	&osc_expr_rec_op_nullcoalesce,
	&osc_expr_rec_op_add1,
	&osc_expr_rec_op_sub1,
	&osc_expr_rec_op_addassign,
	&osc_expr_rec_op_subassign,
	&osc_expr_rec_op_mulassign, // 10
	&osc_expr_rec_op_divassign, 
	&osc_expr_rec_op_modassign,
	&osc_expr_rec_op_powassign,
	&osc_expr_rec_op_nullcoalesceassign,
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
	&osc_expr_rec_op_not	,
	NULL	,
	NULL	,
	NULL	,
	&osc_expr_rec_op_mod	,
	&osc_expr_rec_op_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&osc_expr_rec_op_mul	,
	&osc_expr_rec_op_add	,
	NULL	,
	&osc_expr_rec_op_sub	,
	&osc_expr_rec_op_lookup,
	&osc_expr_rec_op_div	,
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
	&osc_expr_rec_op_lt	,// 60
	&osc_expr_rec_op_assign	,
	&osc_expr_rec_op_gt	,
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
	&osc_expr_rec_op_pow	,
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
	&osc_expr_rec_op_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

static t_osc_expr_rec *osc_expr_func_opcodeToFunctionRec[128] = {
	NULL	,// 0
	&osc_expr_rec_eq	,
	&osc_expr_rec_neq	,
	&osc_expr_rec_le	,
	&osc_expr_rec_ge	,
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
	&osc_expr_rec_not	,
	NULL	,
	NULL	,
	NULL	,
	&osc_expr_rec_mod	,
	&osc_expr_rec_and	,
	NULL	,
	NULL	,// 40
	NULL	,
	&osc_expr_rec_mul	,
	&osc_expr_rec_add	,
	NULL	,
	&osc_expr_rec_sub	,
	&osc_expr_rec_lookup,
	&osc_expr_rec_div	,
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
	&osc_expr_rec_lt	, // 60
	&osc_expr_rec_assign	,
	&osc_expr_rec_gt	,
	&osc_expr_rec_nullcoalesce,
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
	&osc_expr_rec_pow	,
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
	&osc_expr_rec_or	,
	NULL	,
	NULL	,
	NULL	,
	NULL	,
};

// this is the full list of all builtin functions, in no particular order
static struct _osc_expr_rec osc_expr_funcsym[] __attribute__((unused)) = {
	//////////////////////////////////////////////////
	// simple unary and binary operators
	//////////////////////////////////////////////////
	OSC_EXPR_REC_OP_ADD,
	OSC_EXPR_REC_OP_SUB,
	OSC_EXPR_REC_OP_MUL,
	OSC_EXPR_REC_OP_DIV,
	OSC_EXPR_REC_OP_LT,
	OSC_EXPR_REC_OP_LE,
	OSC_EXPR_REC_OP_GT,
	OSC_EXPR_REC_OP_GE,
	OSC_EXPR_REC_OP_EQ,
	OSC_EXPR_REC_OP_NEQ,
	OSC_EXPR_REC_OP_AND,
	OSC_EXPR_REC_OP_OR,
	OSC_EXPR_REC_OP_MOD,
	OSC_EXPR_REC_OP_POW,
	OSC_EXPR_REC_OP_ASSIGN,
	OSC_EXPR_REC_OP_NOT,
	OSC_EXPR_REC_OP_LOOKUP,
	OSC_EXPR_REC_OP_NULLCOALESCE,
	//////////////////////////////////////////////////
	// functional equivalents of simple unary and binary operators
	//////////////////////////////////////////////////
	OSC_EXPR_REC_ADD,
	OSC_EXPR_REC_SUB,
	OSC_EXPR_REC_MUL,
	OSC_EXPR_REC_DIV,
	OSC_EXPR_REC_LT,
	OSC_EXPR_REC_LE,
	OSC_EXPR_REC_GT,
	OSC_EXPR_REC_GE,
	OSC_EXPR_REC_EQ,
	OSC_EXPR_REC_NEQ,
	OSC_EXPR_REC_AND,
	OSC_EXPR_REC_OR,
	OSC_EXPR_REC_MOD,
	OSC_EXPR_REC_POW,
	OSC_EXPR_REC_ASSIGN,
	OSC_EXPR_REC_NOT,
	OSC_EXPR_REC_LOOKUP,
	OSC_EXPR_REC_NULLCOALESCE,
	//////////////////////////////////////////////////
	// unary and binary compound assignment operators
	//////////////////////////////////////////////////
	OSC_EXPR_REC_OP_ADD1,
	OSC_EXPR_REC_OP_SUB1,
	OSC_EXPR_REC_OP_ADDASSIGN,
	OSC_EXPR_REC_OP_SUBASSIGN,
	OSC_EXPR_REC_OP_MULASSIGN,
	OSC_EXPR_REC_OP_DIVASSIGN,
	OSC_EXPR_REC_OP_MODASSIGN,
	OSC_EXPR_REC_OP_POWASSIGN,
	OSC_EXPR_REC_OP_NULLCOALESCEASSIGN,
	//////////////////////////////////////////////////
	// other functions
	//////////////////////////////////////////////////
	OSC_EXPR_REC_NTH,
	OSC_EXPR_REC_ASSIGN_TO_INDEX,

	//////////////////////////////////////////////////
	{"plus1",
	 "/result = plus1($1)",
	 1,
	 0,
	 (char *[]){"arg"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Add one and return the result without altering the argument",
	 osc_expr_add1,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"minus1",
	 "/result = minus1($1)",
	 1,
	 0,
	 (char *[]){"arg"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Subtract one and return the result without altering the argument.",
	 osc_expr_sub1,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	// most of math.h
	//////////////////////////////////////////////////
	{"abs",
	 "/result = abs($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Absolute value",
	 osc_expr_1arg_dbl,
	 (void *)fabs,
	 0},
	//////////////////////////////////////////////////
	{"acos",
	 "/result = acos($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Arc cosine",
	 osc_expr_1arg_dbl,
	 (void *)acos,
	 0},
	//////////////////////////////////////////////////
	{"asin",
	 "/result = asin($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Arc sine",
	 osc_expr_1arg_dbl,
	 (void *)asin,
	 0},
	//////////////////////////////////////////////////
	{"atan",
	 "/result = atan($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Arc tangent",
	 osc_expr_1arg_dbl,
	 (void *)atan,
	 0},
	//////////////////////////////////////////////////
	{"atan2",
	 "/result = atan2($1, $2)",
	 2,
	 0,
	 (char *[]){"y","x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Arc tangent of y/x (arg1/arg2)",
	 osc_expr_2arg_dbl_dbl,
	 (void *)atan2,
	 0},
	//////////////////////////////////////////////////
	{"ceil",
	 "/result = ceil($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Ceiling--round up to the nearest integer",
	 osc_expr_1arg_dbl,
	 (void *)ceil,
	 0},
	//////////////////////////////////////////////////
	{"cos",
	 "/result = cos($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Cosine",
	 osc_expr_1arg_dbl,
	 (void *)cos,
	 0},
	//////////////////////////////////////////////////
	{"cosh",
	 "/result = cosh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Hyperbolic cosine",
	 osc_expr_1arg_dbl,
	 (void *)cosh,
	 0},
	//////////////////////////////////////////////////
	{"exp",
	 "/result = exp($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Exponential function",
	 osc_expr_1arg_dbl,
	 (void *)exp,
	 0},
	//////////////////////////////////////////////////
	{"floor",
	 "/result = floor($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Round down to the nearest integer",
	 osc_expr_1arg_dbl,
	 (void *)floor,
	 0},
	//////////////////////////////////////////////////
	{"fmod",
	 "/result = fmod($1, $2)",
	 2,
	 0,
	 (char *[]){"x", "y"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Floating-point remainder",
	 osc_expr_2arg_dbl_dbl,
	 (void *)fmod,
	 0},
	//////////////////////////////////////////////////
	{"log",
	 "/result = log($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Natural logarithm",
	 osc_expr_1arg_dbl,
	 (void *)log,
	 0},
	//////////////////////////////////////////////////
	{"log10",
	 "/result = log10($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Base 10 logarithm",
	 osc_expr_1arg_dbl,
	 (void *)log10,
	 0},
	//////////////////////////////////////////////////
	{"sin",
	 "/result = sin($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Sine",
	 osc_expr_1arg_dbl,
	 (void *)sin,
	 0},
	//////////////////////////////////////////////////
	{"sinh",
	 "/result = sinh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Hyperbolic sine",
	 osc_expr_1arg_dbl,
	 (void *)sinh,
	 0},
	//////////////////////////////////////////////////
	{"sqrt",
	 "/result = sqrt($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Square root",
	 osc_expr_1arg_dbl,
	 (void *)sqrt,
	 0},
	//////////////////////////////////////////////////
	{"tan",
	 "/result = tan($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Tangent",
	 osc_expr_1arg_dbl,
	 (void *)tan,
	 0},
	//////////////////////////////////////////////////
	{"tanh",
	 "/result = tanh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Hyperbolic tangent",
	 osc_expr_1arg_dbl,
	 (void *)tanh,
	 0},
	//////////////////////////////////////////////////
	{"erf",
	 "/result = erf($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Error function (see http://pubs.opengroup.org/onlinepubs/007908799/xsh/erf.html)",
	 osc_expr_1arg_dbl,
	 (void *)erf,
	 0},
	//////////////////////////////////////////////////
	{"erfc",
	 "/result = erfc($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Complementary error function (see http://pubs.opengroup.org/onlinepubs/007908799/xsh/erf.html)",
	 osc_expr_1arg_dbl,
	 (void *)erfc,
	 0},
	//////////////////////////////////////////////////
	{"gamma",
	 "/result = gamma($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Log gamma function (same as \"lgamma\")", 
	 osc_expr_1arg_dbl,
	 (void *)lgamma,
	 0},
	//////////////////////////////////////////////////
	{"hypot",
	 "/result = hypot($1, $2)",
	 2,
	 0,
	 (char *[]){"x", "y"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Euclidean distance",
	 osc_expr_2arg_dbl_dbl,
	 (void *)hypot,
	 0},
	//////////////////////////////////////////////////
	{"j0",
	 "/result = j0($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "0th Bessel function of the first kind",
	 osc_expr_1arg_dbl,
	 (void *)j0,
	 0},
	//////////////////////////////////////////////////
	{"j1",
	 "/result = j1($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "1st Bessel function of the first kind",
	 osc_expr_1arg_dbl,
	 (void *)j1,
	 0},
	//////////////////////////////////////////////////
	{"jn",
	 "/result = jn($1, $2)",
	 2,
	 0,
	 (char *[]){"n", "x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "nth Bessel function of the first kind",
	 osc_expr_2arg_dbl_dbl,
	 (void *)jn,
	 0},
	//////////////////////////////////////////////////
	{"lgamma",
	 "/result = lgamma($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Log gamma function (same as \"gamma\")",
	 osc_expr_1arg_dbl,
	 (void *)lgamma,
	 0},
	//////////////////////////////////////////////////
	{"y0",
	 "/result = y0($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "0th Bessel function of the second kind",
	 osc_expr_1arg_dbl,
	 (void *)y0,
	 0},
	//////////////////////////////////////////////////
	{"y1",
	 "/result = y1($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "1st Bessel function of the second kind",
	 osc_expr_1arg_dbl,
	 (void *)y1,
	 0},
	//////////////////////////////////////////////////
	{"yn",
	 "/result = yn($1, $2)",
	 2,
	 0,
	 (char *[]){"n", "x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "nth Bessel function of the second kind",
	 osc_expr_2arg_dbl_dbl,
	 (void *)yn,
	 0},
	//////////////////////////////////////////////////
	{"acosh",
	 "/result = acosh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Inverse hyperbolic cosine",
	 osc_expr_1arg_dbl,
	 (void *)acosh,
	 0},
	//////////////////////////////////////////////////
	{"asinh",
	 "/result = asinh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Inverse hyperbolic sine",
	 osc_expr_1arg_dbl,
	 (void *)asinh,
	 0},
	//////////////////////////////////////////////////
	{"atanh",
	 "/result = atanh($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/trigonometric", NULL},
	 "Inverse hyperbolic tangent",
	 osc_expr_1arg_dbl,
	 (void *)atanh,
	 0},
	//////////////////////////////////////////////////
	{"cbrt",
	 "/result = cbrt($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Cube root",
	 osc_expr_1arg_dbl,
	 (void *)cbrt,
	 0},
	//////////////////////////////////////////////////
	{"expm1",
	 "/result = expm1($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Exponential function (e^x - 1)",
	 osc_expr_1arg_dbl,
	 (void *)expm1,
	 0},
	//////////////////////////////////////////////////
	{"ilogb",
	 "/result = ilogb($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Unbiased exponent",
	 osc_expr_1arg_dbl,
	 (void *)ilogb,
	 0},
	//////////////////////////////////////////////////
	{"logb",
	 "/result = logb($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/power", NULL},
	 "Radix-independent exponent",
	 osc_expr_1arg_dbl,
	 (void *)logb,
	 0},
	//////////////////////////////////////////////////
	{"nextafter",
	 "/result = nextafter($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "Next representable double-precision floating-point number",
	 osc_expr_1arg_dbl,
	 (void *)nextafter,
	 0},
	//////////////////////////////////////////////////
	{"remainder",
	 "/result = remainder($1, $2)",
	 2,
	 0,
	 (char *[]){"x", "y"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Remainder function (r = x - ny where y is non-zero and n is the integral value nearest x/y)",
	 osc_expr_2arg_dbl_dbl,
	 (void *)remainder,
	 0},
	//////////////////////////////////////////////////
	{"round",
	 "/result = round($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Round to nearest integral value",
	 osc_expr_1arg_dbl,
	 (void *)round,
	 0},
	//////////////////////////////////////////////////
	{"product",
	 "/result = product($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Product of all the elements of a list",
	 osc_expr_product,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"sum",
	 "/result = sum($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Sum all the elements of a list",
	 osc_expr_sum,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"cumsum",
	 "/result = cumsum($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Cumulative sum",
	 osc_expr_cumsum,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"length",
	 "/result = length($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Get the length of a list",
	 osc_expr_length,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"avg",
	 "/result = avg($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "The average of a list (same as mean)",
	 osc_expr_mean,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"mean",
	 "/result = mean($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "The average of a list (same as avg)",
	 osc_expr_mean,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"median",
	 "/result = median($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Median of a list of values",
	 osc_expr_median,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"reverse",
	 "/result = reverse($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Reverse the order of the elements of a list",
	 osc_expr_reverse,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"rev",
	 "/result = rev($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Reverse the order of the elements of a list",
	 osc_expr_reverse,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"sort",
	 "/result = sort($1)",
	 1,
	 -1,
	 (char *[]){"List to be sorted"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"Additional lists to be sorted"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/vector", NULL},
	 "Sort the elements of a list",
	 osc_expr_sort,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"list",
	 "/result = list($1, $2)",
	 0,
	 -1,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"elements to be gathered into a list"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR_STR},
	 (char *[]){"/vector", NULL},
	 "Assemble the arguments into a list.",
	 osc_expr_list,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"nfill",
	 "/result = nfill($1, $2)",
	 1,
	 1,
	 (char *[]){"number of items"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"value (default = 0)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS | OSC_EXPR_ARG_TYPE_STRING},
	 (char *[]){"/vector", NULL},
	 "Make a list of <arg1> copies of <arg2>.  <arg2> is optional and defaults to 0",
	 osc_expr_nfill,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"aseq",
	 "/result = aseq($1, $2, $3)",
	 2,
	 1,
	 (char *[]){"min", "max"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"step (default = 1)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/vector", NULL},
	 "Arithmetic progression from <arg1> to <arg2> in <arg3> steps.  <arg3> is optional and defaults to 1",
	 osc_expr_aseq,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"interleave",
	 "/result = interleave($1, $2)",
	 2,
	 -1,
	 (char *[]){"list 1", "list 2"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"additional lists (list, OSC address)"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/vector", NULL},
	 "Interleave two or more lists",
	 osc_expr_interleave,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"first",
	 "/result = first($1)",
	 1,
	 0,
	 (char *[]){"list"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Get the first element of a list.",
	 osc_expr_first,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"last",
	 "/result = last($1)",
	 1,
	 0,
	 (char *[]){"list"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Get the last element of a list.",
	 osc_expr_last,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"rest",
	 "/result = rest($1)",
	 1,
	 0,
	 (char *[]){"list"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Get everything after the first element of a list.",
	 osc_expr_rest,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"butlast",
	 "/result = butlast($1)",
	 1,
	 0,
	 (char *[]){"list"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/vector", NULL},
	 "Get everything but the last element of a list.",
	 osc_expr_butlast,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"dot",
	 "/result = dot($1, $2)",
	 2,
	 0,
	 (char *[]){"list 1", "list 2"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Dot product of arg1 and arg2",
	 osc_expr_dot,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"cross",
	 "/result = cross($1, $2)",
	 2,
	 0,
	 (char *[]){"list 1", "list 2"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Cross product of arg1 and arg2",
	 osc_expr_cross,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"det",
	 "/result = det($1, $2)",
	 2,
	 0,
	 (char *[]){"list 1", "list 2"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"additional addresses"},
	 (int []){OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/math/arithmetic", NULL},
	 "Determinant of a square matrix represented as a list of rows (det(row1, row2, ..., rowN))",
	 osc_expr_det,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"l2norm",
	 "/result = l2norm($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Norm of the argument",
	 osc_expr_l2norm,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"min",
	 "/result = min($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Minimum value of the arguments",
	 osc_expr_min,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"max",
	 "/result = max($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Maximum value of the arguments",
	 osc_expr_max,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"range",
	 "/result = range($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Range of the arguments",
	 osc_expr_extrema,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"extrema",
	 "/result = extrema($1)",
	 1,
	 0,
	 (char *[]){"argument (list)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/statistics", NULL},
	 "Min and max of the arguments",
	 osc_expr_extrema,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"clip",
	 "/result = clip($1, $2, $3)",
	 3,
	 0,
	 (char *[]){"value(s) to be clipped", "min", "max"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/conversion", NULL},
	 "Clip the data between arg2 and arg3",
	 osc_expr_clip,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"scale",
	 "/result = scale($1, $2, $3, $4, $5)",
	 5,
	 0,
	 (char *[]){"value(s) to be scaled", "input min", "input max", "output min", "output max"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/conversion", NULL},
	 "Scale arg1 from arg1:arg2 to arg3:arg4",
	 osc_expr_scale,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"mtof",
	 "/result = mtof($1)",
	 1,
	 1,
	 (char *[]){"midi note number"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){"base (default = 440)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/math/conversion", NULL},
	 "MIDI note number to frequency.  Optional arg2 sets base.",
	 osc_expr_mtof,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"ftom",
	 "/result = ftom($1)",
	 1,
	 1,
	 (char *[]){"frequency in Hz."},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){"base (default = 440)"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/math/conversion", NULL},
	 "Frequency to MIDI. Optional arg2 sets base.",
	 osc_expr_ftom,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"sign",
	 "/result = sign($1)",
	 1,
	 0,
	 (char *[]){"x"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/arithmetic", NULL},
	 "Sign function--returns -1 if <arg1> < 0, 0 if <arg1> == 0, and 1 if <arg1> > 1",
	 osc_expr_sign,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"if",
	 "/result = if($1, $2, $3)",
	 2,
	 1,
	 (char *[]){"test", "then"},
	 (int []){OSC_EXPR_ARG_TYPE_BOOLEAN | OSC_EXPR_ARG_TYPE_NUMBER, OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"else"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/conditional", NULL},
	 "Conditionally execute <arg2> or optional <arg3> based on the result of <arg1>",
	 osc_expr_if,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"strcmp",
	 "/result = strcmp($1, $2)",
	 2,
	 1,
	 (char *[]){"String 1", "String 2"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"Number of characters to compare"},
	 (int []){OSC_EXPR_ARG_TYPE_NUMBER | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/string/function", NULL},
	 "Compare two strings.",
	 osc_expr_strcmp,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"split",
	 "/result = split(\"/\", $1)",
	 2,
	 0,
	 (char *[]){"Separator", "String"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/string/function", NULL},
	 "Split a string at a separator.",
	 osc_expr_split,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"join",
	 "/result = join(\"/\", $1)",
	 2,
	 -1,
	 (char *[]){"Separator", "String"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"More strings"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){"/string/function", NULL},
	 "Join multiple strings with a separator.",
	 osc_expr_join,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"bound",
	 "/result = bound($1)",
	 1,
	 0,
	 (char *[]){"OSC address"},
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/predicate", NULL},
	 "True if the address exists and has data bound to it, false otherwise.",
	 osc_expr_bound,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"exists",
	 "/result = exists($1)",
	 1,
	 0,
	 (char *[]){"OSC address"},
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/predicate", NULL},
	 "True if the address exists (regardless of whether it has data bound to it.",
	 osc_expr_exists,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"emptybundle",
	 "/result = emptybundle()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/predicate", NULL},
	 "True if the bundle is empty, false otherwise.",
	 osc_expr_emptybundle,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"getaddresses",
	 "/result = getaddresses()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Returns a list of all addresses in the bundle.",
	 osc_expr_getaddresses,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"delete",
	 "/result = delete($1)",
	 1,
	 0,
	 (char *[]){"OSC address to be deleted"},
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Remove the message with the corresponding address from the bundle.",
	 osc_expr_delete,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"getmsgcount",
	 "/result = getmsgcount()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Returns the number of messages in the bundle.",
	 osc_expr_getmsgcount,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"identity",
	 "/result = identity($1)",
	 1,
	 0,
	 (char *[]){"argument"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/specialfunction", NULL},
	 "Returns its argument",
	 osc_expr_identity,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"eval",
	 "/result = eval($1)",
	 1,
	 0,
	 (char *[]){"OSC address or expression"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Evaluate an expression bound to an OSC address.",
	 osc_expr_eval_call,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"tokenize",
	 "/result = tokenize($1)",
	 1,
	 0,
	 (char *[]){"OSC address or expression"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Tokenize an expression",
	 osc_expr_tokenize,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"prog1",
	 "/result = prog1($1)",
	 1,
	 -1,
	 (char *[]){"expression to be evaluated"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"additional expressions"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/core", NULL},
	 "Execute a sequence of expressions and return the first one.",
	 osc_expr_prog1,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"prog2",
	 "/result = prog2($1)",
	 1,
	 -1,
	 (char *[]){"expression to be evaluated"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"additional expressions"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/core", NULL},
	 "Execute a sequence of expressions and return the second one.",
	 osc_expr_prog2,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"progn",
	 "/result = progn($1)",
	 1,
	 -1,
	 (char *[]){"expression to be evaluated"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"additional expressions"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/core", NULL},
	 "Execute a sequence of expressions and return the last one.",
	 osc_expr_progn,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"apply",
	 "/result = apply($1, $2)",
	 2,
	 -1,
	 (char *[]){"Function or function name", "Argument"},
	 (int []){OSC_EXPR_ARG_TYPE_FUNCTION | OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"Additional argument(s)"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/core", NULL},
	 "Apply a function to arguments.",
	 osc_expr_apply,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"map",
	 "/result = map($1, $2)",
	 2,
	 -1,
	 (char *[]){"Function or function name", "OSC address or list"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS | OSC_EXPR_ARG_TYPE_FUNCTION, OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"Additional lists"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/vector", NULL},
	 "Map arguments onto a function and return the result as a list.",
	 osc_expr_map,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"lreduce",
	 "/result = reduce($1, $2)",
	 2,
	 1,
	 (char *[]){"Function or function name", "OSC address or list to reduce"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS | OSC_EXPR_ARG_TYPE_FUNCTION, OSC_EXPR_ARG_TYPE_ANYTHING, OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"Start value"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/vector", NULL},
	 "Combine the elements of a list using a left-associative binary operation.",
	 osc_expr_lreduce,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"rreduce",
	 "/result = reduce($1, $2)",
	 2,
	 1,
	 (char *[]){"Function or function name", "OSC address or list to reduce"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS | OSC_EXPR_ARG_TYPE_FUNCTION, OSC_EXPR_ARG_TYPE_ANYTHING, OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"Start value"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){"/vector", NULL},
	 "Combine the elements of a list using a right-associative binary operation.",
	 osc_expr_rreduce,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"quote",
	 "/result = quote($1)",
	 1,
	 0,
	 (char *[]){"argument"},
	 (int []){OSC_EXPR_ARG_TYPE_ANYTHING},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Prevent the evaluation of <arg1>.",
	 osc_expr_quote,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"value",
	 "/result = value($1)",
	 1,
	 0,
	 (char *[]){"argument"},
	 (int []){OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Return the value associated with the argument.",
	 osc_expr_value,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"lambda",
	 "",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Anonymous function",
	 osc_expr_lambda,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"gettimetag",
	 "/timetag = gettimetag()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Get the timetag from the OSC bundle header.",
	 osc_expr_gettimetag,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"settimetag",
	 "settimetag(/time)",
	 1,
	 0,
	 (char *[]){"time to place in the header"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_OSCADDRESS},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Put a timetag in the header of the OSC bundle.",
	 osc_expr_settimetag,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"pi",
	 "/result = pi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "pi: 3.14159...",
	 osc_expr_pi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"twopi",
	 "/result = twopi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "2 * pi: 6.28318...",
	 osc_expr_twopi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"halfpi",
	 "/result = halfpi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "pi / 2: 1.57079...",
	 osc_expr_halfpi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"quarterpi",
	 "/result = quarterpi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "pi / 4: 0.78539...",
	 osc_expr_quarterpi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"oneoverpi",
	 "/result = oneoverpi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "1 / pi: 0.31830...",
	 osc_expr_oneoverpi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"twooverpi",
	 "/result = twooverpi()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "2 / pi: 0.63661...",
	 osc_expr_twooverpi,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"degtorad",
	 "/result = degtorad()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "Multiply by angle to get radians (0.017453...)",
	 osc_expr_degtorad,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"radtodeg",
	 "/result = radtodeg()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "Multiply by angle in radians to get degrees (57.29578...)",
	 osc_expr_radtodeg,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"e",
	 "/result = e()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "e: 2.718282...",
	 osc_expr_e,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"lntwo",
	 "/result = lntwo()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "ln 2: 0.69314...",
	 osc_expr_lntwo,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"lnten",
	 "/result = lnten()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "ln 10: 2.30258...",
	 osc_expr_lnten,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"logtwoe",
	 "/result = logtwoe()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "log(2 * e): 1.44269...",
	 osc_expr_logtwoe,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"logtene",
	 "/result = logtene()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "log(10 * e): 0.43420...",
	 osc_expr_logtene,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"sqrttwo",
	 "/result = sqrttwo()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "sqrt(2): 1.41421...",
	 osc_expr_sqrttwo,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"sqrthalf",
	 "/result = sqrthalf()",
	 0,
	 0,
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/math/constant", NULL},
	 "sqrt(0.5): 0.70710...",
	 osc_expr_sqrthalf,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"float32",
	 "/result = float32($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to float32",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_float32,
	 0},
	//////////////////////////////////////////////////
	{"float64",
	 "/result = float64($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to float64",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_float64,
	 0},
	//////////////////////////////////////////////////
	{"int8",
	 "/result = int8($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to int8",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_int8,
	 0},
	//////////////////////////////////////////////////
	{"char",
	 "/result = char($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to char (int8)",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_int8,
	 0},
	//////////////////////////////////////////////////
	{"int16",
	 "/result = int16($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to int16",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_int16,
	 0},
	//////////////////////////////////////////////////
	{"int32",
	 "/result = int32($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to int32",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_int32,
	 0},
	//////////////////////////////////////////////////
	{"int64",
	 "/result = int64($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to int64",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_int64,
	 0},
	//////////////////////////////////////////////////
	{"uint8",
	 "/result = uint8($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to uint8",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_uint8,
	 0},
	//////////////////////////////////////////////////
	{"uint16",
	 "/result = uint16($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to uint16",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_uint16,
	 0},
	//////////////////////////////////////////////////
	{"uint32",
	 "/result = uint32($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to uint32",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_uint32,
	 0},
	//////////////////////////////////////////////////
	{"uint64",
	 "/result = uint64($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to uint64",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_uint64,
	 0},
	//////////////////////////////////////////////////
	{"bool",
	 "/result = bool($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to bool",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_bool,
	 0},
	//////////////////////////////////////////////////
	{"string",
	 "/result = string($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast to string",
	 osc_expr_explicitCast,
	 (void *)osc_expr_explicitCast_string,
	 0},
	//////////////////////////////////////////////////
	{"cast",
	 "/result = cast($1, $2)",
	 2,
	 0,
	 (char *[]){"typetag(s)", "argument(s) to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_STRING | OSC_EXPR_ARG_TYPE_LIST | OSC_EXPR_ARG_TYPE_OSCADDRESS, OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Cast",
	 osc_expr_explicitCast_dynamic,
	 NULL,
	 0},
	//////////////////////////////////////////////////
	{"typetags",
	 "/result = typetags($1)",
	 1,
	 0,
	 (char *[]){"argument to be converted"},
	 (int []){OSC_EXPR_ARG_TYPE_NUM_LIST_ADDR},
	 (char *[]){NULL},
	 (int []){},
	 (char *[]){"/core", NULL},
	 "Get the typetags associated with <arg1> as a list of int8s",
	 osc_expr_typetags,
	 NULL,
	 0}

};

#ifdef _cplusplus
}
#endif

#endif // __OSC_EXPR_FUNC_H__
