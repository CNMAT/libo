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
#include "osc_expr_builtins.h"
#include "osc_expr_oprec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_binaryop.r"


long osc_expr_ast_binaryop_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec((t_osc_expr_ast_binaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e);
	long offset = 0;
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " %s ", osc_expr_oprec_getName(r));
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	return offset;
}

long osc_expr_ast_binaryop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec((t_osc_expr_ast_binaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e);
	long offset = 0;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s ", osc_expr_oprec_getName(r));
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)ast;
		t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec(b);
		t_osc_expr_ast_expr *left = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
		t_osc_expr_ast_expr *right = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
		t_osc_expr_ast_binaryop *copy = osc_expr_ast_binaryop_alloc(r, left, right);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_binaryop_free(t_osc_expr_ast_expr *e)
{
	if(e){
		printf("%s:%d\n", __func__, __LINE__);
		osc_expr_ast_expr_free(osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e));
		printf("%s:%d\n", __func__, __LINE__);
		osc_expr_ast_expr_free(osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e));
		printf("%s:%d\n", __func__, __LINE__);
		osc_mem_free(e);
		printf("%s:%d\n", __func__, __LINE__);
	}
}

t_osc_err osc_expr_ast_binaryop_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_binaryop_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_expr_oprec *osc_expr_ast_binaryop_getOpRec(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

void osc_expr_ast_binaryop_setOpRec(t_osc_expr_ast_binaryop *e, t_osc_expr_oprec *r)
{
	if(e && r){
		e->rec = r;
	}
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

t_osc_expr_ast_binaryop *osc_expr_ast_binaryop_alloc(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *left, t_osc_expr_ast_expr *right)
{
	if(!rec || !left || !right){
		return NULL;
	}
	t_osc_expr_ast_binaryop *b = osc_mem_alloc(sizeof(t_osc_expr_ast_binaryop));
	if(!b){
		return NULL;
	}
	/*
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)b,
			       OSC_EXPR_AST_NODETYPE_BINARYOP,
			       NULL,
			       osc_expr_ast_binaryop_evalInLexEnv,
			       osc_expr_ast_binaryop_format,
			       osc_expr_ast_binaryop_formatLisp,
			       osc_expr_ast_binaryop_free,
			       osc_expr_ast_binaryop_copy,
			       osc_expr_ast_binaryop_serialize,
			       osc_expr_ast_binaryop_deserialize,
			       sizeof(t_osc_expr_ast_binaryop));
	*/
	t_osc_expr_funcrec *funcrec = osc_expr_builtins_lookupFunctionForOperator(rec);
	if(!funcrec){
		return NULL;
	}
	osc_expr_ast_funcall_init((t_osc_expr_ast_funcall *)b,
					  OSC_EXPR_AST_NODETYPE_BINARYOP,
					  NULL,
					  NULL,
					  osc_expr_ast_binaryop_format,
					  NULL,
				  NULL,//osc_expr_ast_binaryop_free,
					  osc_expr_ast_binaryop_copy,
					  osc_expr_ast_binaryop_serialize,
					  osc_expr_ast_binaryop_deserialize,
					  sizeof(t_osc_expr_ast_binaryop),
					  funcrec,
					  2,
					  left,
					  right);
	osc_expr_ast_binaryop_setOpRec(b, rec);
	osc_expr_ast_binaryop_setLeftArg(b, left);
	osc_expr_ast_binaryop_setRightArg(b, right);
	return b;
}
