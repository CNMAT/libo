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
#include "osc_expr.h"
#include "osc_expr_func.h"
//#include "osc_expr_privatedecls.h"
#include "osc_expr_rec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_fieldop.h"
#include "osc_expr_ast_fieldop.r"


int osc_expr_ast_fieldop_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      long *len,
				      char **oscbndl,
				      t_osc_atom_ar_u **out)
{
	return 1;
}

long osc_expr_ast_fieldop_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_fieldop_getLeftArg((t_osc_expr_ast_fieldop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_fieldop_getRightArg((t_osc_expr_ast_fieldop *)e);
	long offset = 0;
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ".");
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	return offset;
}

long osc_expr_ast_fieldop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_fieldop_getLeftArg((t_osc_expr_ast_fieldop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_fieldop_getRightArg((t_osc_expr_ast_fieldop *)e);
	long offset = 0;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(. ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_fieldop_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_fieldop *b = (t_osc_expr_ast_fieldop *)ast;
		t_osc_expr_ast_expr *left = osc_expr_ast_expr_copy(osc_expr_ast_fieldop_getLeftArg(b));
		t_osc_expr_ast_expr *right = osc_expr_ast_expr_copy(osc_expr_ast_fieldop_getLeftArg(b));
		t_osc_expr_ast_fieldop *copy = osc_expr_ast_fieldop_alloc(left, right);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_fieldop_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_mem_free(e);
	}
}

t_osc_err osc_expr_ast_fieldop_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_fieldop_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_expr_ast_expr *osc_expr_ast_fieldop_getLeftArg(t_osc_expr_ast_fieldop *e)
{
	if(e){
		return e->left;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_fieldop_getRightArg(t_osc_expr_ast_fieldop *e)
{
	if(e){
		return e->right;
	}
	return NULL;
}

void osc_expr_ast_fieldop_setLeftArg(t_osc_expr_ast_fieldop *e, t_osc_expr_ast_expr *left)
{
	if(e){
		e->left = left;
	}
}

void osc_expr_ast_fieldop_setRightArg(t_osc_expr_ast_fieldop *e, t_osc_expr_ast_expr *right)
{
	if(e){
		e->right = right;
	}
}

t_osc_expr_ast_fieldop *osc_expr_ast_fieldop_alloc(t_osc_expr_ast_expr *left, t_osc_expr_ast_expr *right)
{
	if(!left || !right){
		return NULL;
	}
	t_osc_expr_ast_fieldop *b = osc_mem_alloc(sizeof(t_osc_expr_ast_fieldop));
	if(!b){
		return NULL;
	}
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)b,
			       OSC_EXPR_AST_NODETYPE_FIELDOP,
			       NULL,
			       osc_expr_ast_fieldop_evalInLexEnv,
			       osc_expr_ast_fieldop_format,
			       osc_expr_ast_fieldop_formatLisp,
			       osc_expr_ast_fieldop_free,
			       osc_expr_ast_fieldop_copy,
			       osc_expr_ast_fieldop_serialize,
			       osc_expr_ast_fieldop_deserialize,
			       sizeof(t_osc_expr_ast_fieldop));
	osc_expr_ast_fieldop_setLeftArg(b, left);
	osc_expr_ast_fieldop_setRightArg(b, right);
	return b;
}
