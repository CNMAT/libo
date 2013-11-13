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

#include <stdio.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_ast_sugar.r"
#include "osc_expr_ast_sugar.h"

int osc_expr_ast_sugar_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out)
{
	if(ast){
		return osc_expr_ast_expr_evalInLexEnv(osc_expr_ast_sugar_getFunctionalAST((t_osc_expr_ast_sugar *)ast),
						      lexenv,
						      len,
						      oscbndl,
						      out);
	}else{
		return 0;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_sugar_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		return (t_osc_expr_ast_expr *)osc_expr_ast_sugar_alloc(
								       osc_expr_ast_expr_copy(osc_expr_ast_sugar_getParsedAST((t_osc_expr_ast_sugar *)ast)),
								       osc_expr_ast_expr_copy(osc_expr_ast_sugar_getFunctionalAST((t_osc_expr_ast_sugar *)ast)));
	}
	return NULL;
}

void osc_expr_ast_sugar_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_expr_ast_expr_free(osc_expr_ast_sugar_getParsedAST((t_osc_expr_ast_sugar *)e));
		osc_expr_ast_expr_free(osc_expr_ast_sugar_getFunctionalAST((t_osc_expr_ast_sugar *)e));
	}
}

long osc_expr_ast_sugar_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e){
		return osc_expr_ast_expr_format(buf, n, osc_expr_ast_sugar_getParsedAST((t_osc_expr_ast_sugar *)e));
	}else{
		return 0;
	}
}

long osc_expr_ast_sugar_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e){
		return osc_expr_ast_expr_formatLisp(buf, n, osc_expr_ast_sugar_getParsedAST((t_osc_expr_ast_sugar *)e));
	}else{
		return 0;
	}
}

t_osc_err osc_expr_ast_sugar_serialize(t_osc_expr_ast_expr *e, long *len, char **ptr)
{
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_ast_sugar_deserialize(long len, char *ptr, t_osc_expr_ast_expr **e)
{
	return OSC_ERR_NONE;
}

t_osc_expr_ast_expr *osc_expr_ast_sugar_getParsedAST(t_osc_expr_ast_sugar *s)
{
	if(s){
		return s->parsed_ast;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_sugar_getFunctionalAST(t_osc_expr_ast_sugar *s)
{
	if(s){
		return s->functional_ast;
	}
	return NULL;
}

t_osc_expr_ast_sugar *osc_expr_ast_sugar_alloc(t_osc_expr_ast_expr *parsed_ast, t_osc_expr_ast_expr *functional_ast)
{
	if(!parsed_ast || !functional_ast){
		return NULL;
	}
	t_osc_expr_ast_sugar *s = osc_mem_alloc(sizeof(t_osc_expr_ast_sugar));
	if(!s){
		return NULL;
	}
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)s,
			       OSC_EXPR_AST_NODETYPE_SUGAR,
			       NULL,
			       osc_expr_ast_sugar_evalInLexEnv,
			       osc_expr_ast_sugar_format,
			       osc_expr_ast_sugar_formatLisp,
			       osc_expr_ast_sugar_free,
			       osc_expr_ast_sugar_copy,
			       osc_expr_ast_sugar_serialize,
			       osc_expr_ast_sugar_deserialize,
			       sizeof(t_osc_expr_ast_sugar));
	return s;
}
