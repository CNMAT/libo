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
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_binaryop.r"


int osc_expr_ast_binaryop_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      long *len,
				      char **oscbndl,
				      t_osc_atom_ar_u **out)
{
	return 1;
}

long osc_expr_ast_binaryop_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_rec *r = osc_expr_ast_binaryop_getRec((t_osc_expr_ast_binaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e);
	long offset = 0;
	if(!buf){
		offset += osc_expr_ast_expr_format(NULL, n, left);
		offset += snprintf(NULL, n, "%s", osc_expr_rec_getName(r));
		offset += osc_expr_ast_expr_format(NULL, n, right);
	}else{
		offset += osc_expr_ast_expr_format(buf + offset, n, left);
		offset += snprintf(buf + offset, n, "%s", osc_expr_rec_getName(r));
		offset += osc_expr_ast_expr_format(buf + offset, n, right);
	}
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)ast;
		t_osc_expr_rec *r = osc_expr_ast_binaryop_getRecCopy(b);
		t_osc_expr_ast_expr *left = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
		t_osc_expr_ast_expr *right = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
		t_osc_expr_ast_binaryop *copy = osc_expr_ast_binaryop_alloc(r, left, right);
		osc_expr_ast_binaryop_setLeftArg(copy, left);
		osc_expr_ast_binaryop_setRightArg(copy, right);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_binaryop_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_mem_free(e);
	}
}

t_osc_expr_funcptr osc_expr_ast_binaryop_getFunc(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return osc_expr_rec_getFunction(osc_expr_ast_binaryop_getRec(e));
	}
	return NULL;
}

t_osc_expr_rec *osc_expr_ast_binaryop_getRec(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

void osc_expr_ast_binaryop_setRec(t_osc_expr_ast_binaryop *e, t_osc_expr_rec *r)
{
	if(e && r){
		e->rec = r;
	}
}

t_osc_expr_rec *osc_expr_ast_binaryop_getRecCopy(t_osc_expr_ast_binaryop *e)
{
	if(e){
		t_osc_expr_rec *r = e->rec;
		t_osc_expr_rec *copy = NULL;
		osc_expr_rec_copy(&copy, r, NULL);
		return copy;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_getLeftArg(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->left;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_getRightArg(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->right;
	}
	return NULL;
}

void osc_expr_ast_binaryop_setLeftArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *left)
{
	if(e){
		e->left = left;
	}
}

void osc_expr_ast_binaryop_setRightArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *right)
{
	if(e){
		e->right = right;
	}
}

t_osc_expr_ast_binaryop *osc_expr_ast_binaryop_alloc(t_osc_expr_rec *rec, t_osc_expr_ast_expr *left, t_osc_expr_ast_expr *right)
{
	if(!rec || !left || !right){
		return NULL;
	}
	t_osc_expr_ast_binaryop *b = osc_mem_alloc(sizeof(t_osc_expr_ast_binaryop));
	if(!b){
		return NULL;
	}
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)b,
			       OSC_EXPR_AST_NODETYPE_BINARYOP,
			       NULL,
			       osc_expr_ast_binaryop_evalInLexEnv,
			       osc_expr_ast_binaryop_format,
			       osc_expr_ast_binaryop_free,
			       osc_expr_ast_binaryop_copy,
			       sizeof(t_osc_expr_ast_binaryop));
	osc_expr_ast_binaryop_setRec(b, rec);
	osc_expr_ast_binaryop_setLeftArg(b, left);
	osc_expr_ast_binaryop_setRightArg(b, right);
	return b;
}
