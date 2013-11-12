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
#include "osc_expr_ast_expr.r"
#include "osc_expr_ast_expr.h"

void osc_expr_ast_expr_init(t_osc_expr_ast_expr *e,
			    int nodetype,
			    t_osc_expr_ast_expr *next,
			    t_osc_expr_ast_evalfn evalfn,
			    t_osc_expr_ast_formatfn formatfn,
			    t_osc_expr_ast_freefn freefn,
			    t_osc_expr_ast_copyfn copyfn,
			    size_t objsize)
{
	if(e){
		e->nodetype = nodetype;
		e->next = next;
		e->eval = evalfn;
		e->format = formatfn;
		e->free = freefn;
		e->objsize = sizeof(t_osc_expr_ast_expr);
	}
}

int osc_expr_ast_expr_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   long *len,
				   char **oscbndl,
				   t_osc_atom_ar_u **out)
{
	if(ast && ast->eval){
		return ast->eval(ast, lexenv, len, oscbndl, out);
	}else{
		return 0;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_expr_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		if(ast->copy){
			t_osc_expr_ast_expr *copy = ast->copy(ast);
			copy->next = osc_expr_ast_expr_copy(ast->next);
			return copy;
		}else{
			return osc_expr_ast_expr_alloc();
		}
	}else{
		return NULL;
	}
}

void osc_expr_ast_expr_free(t_osc_expr_ast_expr *e)
{
	if(e){
		if(e->free){
			if(e->next){
				osc_expr_ast_expr_free(e->next);
			}
			e->free(e);
		}else{
			osc_mem_free(e);
		}
	}
}

int osc_expr_ast_expr_getNodetype(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->nodetype;
	}
	return -1;
}

t_osc_expr_ast_expr *osc_expr_ast_expr_next(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->next;
	}
	return NULL;
}

void osc_expr_ast_expr_prepend(t_osc_expr_ast_expr *e, t_osc_expr_ast_expr *expr_to_prepend)
{
	if(!e || !expr_to_prepend){
		return;
	}
	expr_to_prepend->next = e->next;
	e->next = expr_to_prepend;
}

void osc_expr_ast_expr_append(t_osc_expr_ast_expr *e, t_osc_expr_ast_expr *expr_to_append)
{
	if(!e || !expr_to_append){
		return;
	}
	t_osc_expr_ast_expr *ll = osc_expr_ast_expr_next(e);
	if(!ll){
		e->next = expr_to_append;
	}else{
		while(ll->next){
			ll = ll->next;
		}
		ll->next = expr_to_append;
	}
}

long osc_expr_ast_expr_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e && e->format){
		return e->format(buf, n, e);
	}else{
		return 0;
	}
}

size_t osc_expr_ast_expr_sizeof(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->objsize;
	}else{
		return 0;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_expr_alloc(void)
{
	t_osc_expr_ast_expr *e = (t_osc_expr_ast_expr *)osc_mem_alloc(sizeof(t_osc_expr_ast_expr));
	osc_expr_ast_expr_init(e, OSC_EXPR_AST_NODETYPE_EXPR, NULL, NULL, NULL, NULL, NULL, sizeof(t_osc_expr_ast_expr));
	return e;
}
