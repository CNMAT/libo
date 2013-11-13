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
#include <string.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_message_s.h"
#include "osc_message_iterator_s.h"
#include "osc_rset.h"
#include "osc_query.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_oscaddress.h"
#include "osc_expr_ast_oscaddress.r"

int osc_expr_ast_oscaddress_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    long *len,
				    char **oscbndl,
				    t_osc_atom_ar_u **out)
{
	return 1;
}

long osc_expr_ast_oscaddress_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return osc_expr_ast_expr_format(buf, n, osc_expr_ast_oscaddress_getAddressExpr((t_osc_expr_ast_oscaddress *)v));
	}
	return 0;
}

long osc_expr_ast_oscaddress_formatLisp(char *buf, long n, t_osc_expr_ast_expr *v)
{
	if(v){
		return osc_expr_ast_expr_formatLisp(buf, n, osc_expr_ast_oscaddress_getAddressExpr((t_osc_expr_ast_oscaddress *)v));
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_oscaddress_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		return (t_osc_expr_ast_expr *)osc_expr_ast_oscaddress_alloc(osc_expr_ast_expr_copy(osc_expr_ast_oscaddress_getAddressExpr((t_osc_expr_ast_oscaddress *)ast)));
	}else{
		return NULL;
	}
}

void osc_expr_ast_oscaddress_free(t_osc_expr_ast_expr *v)
{
	if(v){
		osc_expr_ast_expr_free(osc_expr_ast_oscaddress_getAddressExpr((t_osc_expr_ast_oscaddress *)v));
		osc_mem_free(v);
	}
}

t_osc_err osc_expr_ast_oscaddress_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_oscaddress_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_expr_ast_expr *osc_expr_ast_oscaddress_getAddressExpr(t_osc_expr_ast_oscaddress *v)
{
	if(v){
		return v->oscaddress;
	}
	return NULL;
}

void osc_expr_ast_oscaddress_setAddressExpr(t_osc_expr_ast_oscaddress *v, t_osc_expr_ast_expr *oscaddress)
{
	if(v){
		v->oscaddress = oscaddress;
	}
}

t_osc_expr_ast_oscaddress *osc_expr_ast_oscaddress_alloc(t_osc_expr_ast_expr *address)
{
	t_osc_expr_ast_oscaddress *v = osc_mem_alloc(sizeof(t_osc_expr_ast_oscaddress));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_OSCADDRESS,
				       NULL,
				       osc_expr_ast_oscaddress_evalInLexEnv,
				       osc_expr_ast_oscaddress_format,
				       osc_expr_ast_oscaddress_formatLisp,
				       osc_expr_ast_oscaddress_free,
				       osc_expr_ast_oscaddress_copy,
				       osc_expr_ast_oscaddress_serialize,
				       osc_expr_ast_oscaddress_deserialize,
				       sizeof(t_osc_expr_ast_oscaddress));
		osc_expr_ast_oscaddress_setAddressExpr(v, address);
	}
	return v;
}
