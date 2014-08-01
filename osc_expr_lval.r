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

/** 	\file osc_expr_lval.r
	\author John MacCallum

*/

#ifndef __OSC_EXPR_LVAL_R__
#define __OSC_EXPR_LVAL_R__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_expr_ast_expr.h"

struct _osc_expr_lval {
	t_osc_bndl_u *bndl;
	t_osc_msg_u *msg;
	t_osc_atom_ar_u *atoms;
	char *address;
	t_osc_expr_ast_expr *lval_expr;
	t_osc_atom_ar_u *indexes;
};

#ifdef __cplusplus
}
#endif

#endif