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
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"


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
int osc_expr_plus1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
int osc_expr_minus1(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);
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
int osc_expr_explicitCast_string(t_osc_atom_u *dest, t_osc_atom_u *src);
int osc_expr_explicitCast_dynamic(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);

int osc_expr_typetags(t_osc_expr *f, int argc, t_osc_atom_ar_u **argv, t_osc_atom_ar_u **out);



#ifdef _cplusplus
}
#endif

#endif // __OSC_EXPR_FUNC_H__
