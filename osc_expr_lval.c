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
#include "osc_expr_lval.r"
#include "osc_expr_lval.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_atom_array_u.h"
#include "osc_expr_ast_expr.h"
#include "osc_mem.h"

t_osc_expr_lval *osc_expr_lval_alloc(void)
{
	t_osc_expr_lval *lval = (t_osc_expr_lval *)osc_mem_alloc(sizeof(t_osc_expr_lval));
	if(lval){
		memset(lval, '\0', sizeof(t_osc_expr_lval));
	}
	return lval;
}

void osc_expr_lval_free(t_osc_expr_lval *lval)
{
	if(lval){
		osc_mem_free(lval);
	}
}

t_osc_bndl_u *osc_expr_lval_getBndl(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->bndl;
	}
	return NULL;
}

t_osc_msg_u *osc_expr_lval_getMsg(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->msg;
	}
	return NULL;
}

t_osc_atom_ar_u *osc_expr_lval_getAtoms(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->atoms;
	}
	return NULL;
}

char *osc_expr_lval_getAddress(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->address;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_lval_getLvalExpr(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->lval_expr;
	}
	return NULL;
}

t_osc_atom_ar_u *osc_expr_lval_getIndexes(t_osc_expr_lval *lval)
{
	if(lval){
		return lval->indexes;
	}
	return NULL;
}

void osc_expr_lval_setBndl(t_osc_expr_lval *lval, t_osc_bndl_u *bndl)
{
	if(lval){
		lval->bndl = bndl;
	}
}

void osc_expr_lval_setMsg(t_osc_expr_lval *lval, t_osc_msg_u *msg)
{
	if(lval){
		lval->msg = msg;
	}
}

void osc_expr_lval_setAtoms(t_osc_expr_lval *lval, t_osc_atom_ar_u *ar)
{
	if(lval){
		lval->atoms = ar;
	}
}

void osc_expr_lval_setAddress(t_osc_expr_lval *lval, char *address)
{
	if(lval){
		lval->address = address;
	}
}

void osc_expr_lval_setLvalExpr(t_osc_expr_lval *lval, t_osc_expr_ast_expr *lval_expr)
{
	if(lval){
		lval->lval_expr = lval_expr;
	}
}

void osc_expr_lval_setIndexes(t_osc_expr_lval *lval, t_osc_atom_ar_u *indexes)
{
	if(lval){
		lval->indexes = indexes;
	}
}
