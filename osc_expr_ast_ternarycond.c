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
#include "osc_expr_builtin.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_ternarycond.h"
#include "osc_expr_ast_ternarycond.r"

int osc_expr_ast_ternarycond_evalInLexEnv(t_osc_expr_ast_expr *ast,
					  t_osc_expr_lexenv *lexenv,
					  t_osc_bndl_u *oscbndl,
					  t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_ast_ternarycond_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					      t_osc_expr_lexenv *lexenv,
					      t_osc_bndl_u *oscbndl,
					      t_osc_msg_u **assign_target,
					      long *nlvals,
					      t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_ternarycond_format(char *buf, long n, t_osc_expr_ast_expr *ast)
{
	if(!ast){
		return 0;
	}
	long offset = 0;
	t_osc_expr_ast_ternarycond *a = (t_osc_expr_ast_ternarycond *)ast;
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getTest(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ? ");
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getLeftbranch(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " : ");
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getRightbranch(a));
	return offset;
}

long osc_expr_ast_ternarycond_formatLisp(char *buf, long n, t_osc_expr_ast_expr *ast)
{
	if(!ast){
		return 0;
	}
	long offset = 0;
	t_osc_expr_ast_ternarycond *a = (t_osc_expr_ast_ternarycond *)ast;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(if ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getTest(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getLeftbranch(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_ternarycond_getRightbranch(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_ternarycond_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_ternarycond *a = (t_osc_expr_ast_ternarycond *)ast;
		t_osc_expr_ast_expr *testcopy = osc_expr_ast_expr_copy(osc_expr_ast_ternarycond_getTest(a));
		t_osc_expr_ast_expr *leftbranchcopy = osc_expr_ast_expr_copy(osc_expr_ast_ternarycond_getLeftbranch(a));
		t_osc_expr_ast_expr *rightbranch = osc_expr_ast_ternarycond_getRightbranch(a);
		t_osc_expr_ast_expr *rightbranchcopy = osc_expr_ast_expr_copy(osc_expr_ast_ternarycond_getRightbranch(a));
		t_osc_expr_ast_ternarycond *copy = osc_expr_ast_ternarycond_alloc(testcopy, leftbranchcopy, rightbranchcopy);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_ternarycond_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_expr_ast_expr_free(osc_expr_ast_ternarycond_getTest((t_osc_expr_ast_ternarycond *)e));
		osc_expr_ast_expr_free(osc_expr_ast_ternarycond_getLeftbranch((t_osc_expr_ast_ternarycond *)e));
		osc_expr_ast_expr_free(osc_expr_ast_ternarycond_getRightbranch((t_osc_expr_ast_ternarycond *)e));
		osc_mem_free(e);
	}
}

t_osc_err osc_expr_ast_ternarycond_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	if(!e){
		return OSC_ERR_NULLPTR;
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_ternarycond_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	if(!len || !ptr){
		return OSC_ERR_NOBUNDLE;
	}
	return OSC_ERR_NONE;
}

t_osc_expr_ast_expr *osc_expr_ast_ternarycond_getTest(t_osc_expr_ast_ternarycond *e)
{
	if(e){
		return e->test;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_ternarycond_getLeftbranch(t_osc_expr_ast_ternarycond *e)
{
	if(e){
		return e->leftbranch;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_ternarycond_getRightbranch(t_osc_expr_ast_ternarycond *e)
{
	if(e){
		return e->rightbranch;
	}
	return NULL;
}

void osc_expr_ast_ternarycond_setTest(t_osc_expr_ast_ternarycond *e, t_osc_expr_ast_expr *test)
{
	if(e){
		e->test = test;
	}
}

void osc_expr_ast_ternarycond_setLeftbranch(t_osc_expr_ast_ternarycond *e, t_osc_expr_ast_expr *leftbranch)
{
	if(e){
		e->leftbranch = leftbranch;
	}
}

void osc_expr_ast_ternarycond_setRightbranch(t_osc_expr_ast_ternarycond *e, t_osc_expr_ast_expr *rightbranch)
{
	if(e){
		e->rightbranch = rightbranch;
	}
}


t_osc_expr_ast_ternarycond *osc_expr_ast_ternarycond_alloc(t_osc_expr_ast_expr *test, t_osc_expr_ast_expr *leftbranch, t_osc_expr_ast_expr *rightbranch)
{
	if(!test || !leftbranch || !rightbranch){
		return NULL;
	}
	t_osc_expr_ast_ternarycond *e = osc_mem_alloc(sizeof(t_osc_expr_ast_ternarycond));
	if(!e){
		return NULL;
	}
	/*
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e,
			       OSC_EXPR_AST_NODETYPE_TERNARYCOND,
			       NULL,
			       osc_expr_ast_ternarycond_evalInLexEnv,
			       osc_expr_ast_ternarycond_format,
			       osc_expr_ast_ternarycond_formatLisp,
			       osc_expr_ast_ternarycond_free,
			       osc_expr_ast_ternarycond_copy,
			       osc_expr_ast_ternarycond_serialize,
			       osc_expr_ast_ternarycond_deserialize,
			       sizeof(t_osc_expr_ast_ternarycond));
	*/
	t_osc_expr_funcrec *funcrec = osc_expr_builtin_func_if;
	osc_expr_ast_funcall_init((t_osc_expr_ast_funcall *)e,
				  OSC_EXPR_AST_NODETYPE_TERNARYCOND,
				  NULL,
				  NULL,
				  NULL,
				  osc_expr_ast_ternarycond_format,
				  NULL,
				  NULL,//osc_expr_ast_ternarycond_free,
				  osc_expr_ast_ternarycond_copy,
				  osc_expr_ast_ternarycond_serialize,
				  osc_expr_ast_ternarycond_deserialize,
				  sizeof(t_osc_expr_ast_ternarycond),
				  funcrec,
				  3,
				  test,
				  leftbranch,
				  rightbranch);
	osc_expr_ast_ternarycond_setTest(e, test);
	osc_expr_ast_ternarycond_setLeftbranch(e, leftbranch);
	osc_expr_ast_ternarycond_setRightbranch(e, rightbranch);
	return e;
}
