/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-13, The Regents of
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

#include <stdarg.h>
#include <stdio.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_expr_value.h"
#include "osc_expr_ast_expr_value.r"

t_osc_expr_ast_expr_value *osc_expr_ast_expr_value_alloc(t_osc_atom_u *a)
{
	t_osc_expr_ast_expr_value *v = osc_mem_alloc(sizeof(t_osc_expr_ast_expr_value));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v, OSC_EXPR_AST_EXPR_TYPE_VALUE, NULL, osc_expr_ast_expr_value_format, osc_expr_ast_expr_value_free, sizeof(t_osc_expr_ast_expr_value));
		osc_expr_ast_expr_value_setValue(v, a);
	}
	return v;
}

long osc_expr_ast_expr_value_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return osc_atom_u_nformat(buf, n, osc_expr_ast_expr_value_getValue((t_osc_expr_ast_expr_value *)v), 0);
	}
	return 0;
}

void osc_expr_ast_expr_value_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_atom_u_free(osc_expr_ast_expr_value_getValue((t_osc_expr_ast_expr_value *)v));
		osc_mem_free(v);
	}
}

t_osc_atom_u *osc_expr_ast_expr_value_getValue(t_osc_expr_ast_expr_value *v)
{
	if(v){
		return v->value;
	}
	return NULL;
}

void osc_expr_ast_expr_value_setValue(t_osc_expr_ast_expr_value *v, t_osc_atom_u *a)
{
	if(v){
		v->value = a;
	}
}
