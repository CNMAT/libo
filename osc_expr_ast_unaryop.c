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
#include "osc_expr_ast_unaryop.h"
#include "osc_expr_ast_unaryop.r"


int osc_expr_ast_unaryop_evalInLexEnv(t_osc_expr_ast_expr *ast,
				      t_osc_expr_lexenv *lexenv,
				      long *len,
				      char **oscbndl,
				      t_osc_atom_ar_u **out)
{
	return 1;
}

long osc_expr_ast_unaryop_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_oprec *r = osc_expr_ast_unaryop_getOpRec((t_osc_expr_ast_unaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *arg = osc_expr_ast_unaryop_getArg((t_osc_expr_ast_unaryop *)e);
	int side = osc_expr_ast_unaryop_getSide((t_osc_expr_ast_unaryop *)e);
	long offset = 0;
	if(side == OSC_EXPR_AST_UNARYOP_LEFT){
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s", osc_expr_oprec_getName(r));
		offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, arg);
	}else if(side == OSC_EXPR_AST_UNARYOP_RIGHT){
		offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, arg);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "%s", osc_expr_oprec_getName(r));
	}else{
		//wtf?
	}
	return offset;
}

// this shouldn't actually ever get called
long osc_expr_ast_unaryop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_unaryop_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_unaryop *u = (t_osc_expr_ast_unaryop *)ast;
		t_osc_expr_oprec *r = osc_expr_ast_unaryop_getOpRec(u);
		t_osc_expr_ast_expr *arg = osc_expr_ast_expr_copy(osc_expr_ast_unaryop_getArg(u));
		int side = osc_expr_ast_unaryop_getSide(u);
		t_osc_expr_ast_unaryop *copy = osc_expr_ast_unaryop_alloc(r, arg, side);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_unaryop_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_expr_ast_expr_free(osc_expr_ast_unaryop_getArg((t_osc_expr_ast_unaryop *)e));
		osc_mem_free(e);
	}
}

t_osc_err osc_expr_ast_unaryop_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_unaryop_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_expr_oprec *osc_expr_ast_unaryop_getOpRec(t_osc_expr_ast_unaryop *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

void osc_expr_ast_unaryop_setOpRec(t_osc_expr_ast_unaryop *e, t_osc_expr_oprec *r)
{
	if(e && r){
		e->rec = r;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_unaryop_getArg(t_osc_expr_ast_unaryop *e)
{
	if(e){
		return e->arg;
	}
	return NULL;
}

void osc_expr_ast_unaryop_setArg(t_osc_expr_ast_unaryop *e, t_osc_expr_ast_expr *arg)
{
	if(e){
		e->arg = arg;
	}
}

int osc_expr_ast_unaryop_getSide(t_osc_expr_ast_unaryop *e)
{
	if(e){
		return e->side;
	}
	return -1;
}

void osc_expr_ast_unaryop_setSide(t_osc_expr_ast_unaryop *e, int side)
{
	if(side == OSC_EXPR_AST_UNARYOP_LEFT || side == OSC_EXPR_AST_UNARYOP_RIGHT){
		if(e){
			e->side = side;
			return;
		}
	}
	printf("%s:%d: side is not left or right!\n", __func__, __LINE__);
}

t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_alloc(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *arg, int side)
{
	if(!rec || !arg || (side != OSC_EXPR_AST_UNARYOP_LEFT && side != OSC_EXPR_AST_UNARYOP_RIGHT)){
		return NULL;
	}
	t_osc_expr_ast_unaryop *b = osc_mem_alloc(sizeof(t_osc_expr_ast_unaryop));
	if(!b){
		return NULL;
	}
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)b,
			       OSC_EXPR_AST_NODETYPE_UNARYOP,
			       NULL,
			       osc_expr_ast_unaryop_evalInLexEnv,
			       osc_expr_ast_unaryop_format,
			       osc_expr_ast_unaryop_formatLisp,
			       osc_expr_ast_unaryop_free,
			       osc_expr_ast_unaryop_copy,
			       osc_expr_ast_unaryop_serialize,
			       osc_expr_ast_unaryop_deserialize,
			       sizeof(t_osc_expr_ast_unaryop));
	osc_expr_ast_unaryop_setOpRec(b, rec);
	osc_expr_ast_unaryop_setArg(b, arg);
	osc_expr_ast_unaryop_setSide(b, side);
	return b;
}

t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_allocLeft(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *arg)
{
	return osc_expr_ast_unaryop_alloc(rec, arg, OSC_EXPR_AST_UNARYOP_LEFT);
}

t_osc_expr_ast_unaryop *osc_expr_ast_unaryop_allocRight(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *arg)
{
	return osc_expr_ast_unaryop_alloc(rec, arg, OSC_EXPR_AST_UNARYOP_RIGHT);
}
