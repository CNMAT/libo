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
#include "osc_expr_ast_function.h"

void osc_expr_ast_expr_init(t_osc_expr_ast_expr *e,
			    int nodetype,
			    t_osc_expr_ast_expr *next,
			    t_osc_expr_ast_evalfn evalfn,
			    t_osc_expr_ast_evallvalfn evallvalfn,
			    t_osc_expr_ast_formatfn formatfn,
			    t_osc_expr_ast_formatfn format_lispfn,
			    t_osc_expr_ast_freefn freefn,
			    t_osc_expr_ast_copyfn copyfn,
			    t_osc_expr_ast_tobndlfn tobndlfn,
			    t_osc_expr_ast_frombndlfn frombndlfn,
			    size_t objsize)
{
	if(e){
		e->nodetype = nodetype;
		e->next = next;
		e->eval = evalfn;
		e->evallval = evallvalfn;
		e->format = formatfn;
		e->format_lisp = format_lispfn;
		e->free = freefn;
		e->copy = copyfn;
		e->tobndl = tobndlfn;
		e->frombndl = frombndlfn;
		e->objsize = sizeof(t_osc_expr_ast_expr);
		e->leftbracket = 0;
		e->rightbracket = 0;
		e->is_static = 0;
	}
}

int osc_expr_ast_expr_evalAll(t_osc_expr_ast_expr *ast,
			      long *len,
			      char **oscbndl,
			      t_osc_atom_ar_u **out)
{
	t_osc_bndl_u *bndlu = NULL;
	osc_bundle_s_deserialize(*len, *oscbndl, &bndlu);
	int ret = 0;
	if(bndlu){
		while(ast){
			ret = osc_expr_ast_expr_evalInLexEnv(ast, NULL, bndlu, out);
			if(osc_expr_ast_expr_next(ast) != NULL){
				osc_atom_array_u_free(*out);
				*out = NULL;
			}
			if(ret){
				osc_bundle_u_free(bndlu);
				return ret;
			}
			ast = osc_expr_ast_expr_next(ast);
		}
		*len = 0;
		if(*oscbndl){
			osc_mem_free(*oscbndl);
			*oscbndl = NULL;
		}
		osc_bundle_u_serialize(bndlu, len, oscbndl);
		osc_bundle_u_free(bndlu);
		return 0;
	}
	return 1;
}

int osc_expr_ast_expr_eval(t_osc_expr_ast_expr *ast,
			   long *len,
			   char **oscbndl,
			   t_osc_atom_ar_u **out)
{
	t_osc_bndl_u *bndlu = NULL;
	osc_bundle_s_deserialize(*len, *oscbndl, &bndlu);
	int ret = 0;
	if(bndlu){
		ret = osc_expr_ast_expr_evalInLexEnv(ast, NULL, bndlu, out);
		if(ret){
			osc_bundle_u_free(bndlu);
			return ret;
		}
		osc_bundle_u_serialize(bndlu, len, oscbndl);
		osc_bundle_u_free(bndlu);
		return 0;
	}
	return 1;
}

int osc_expr_ast_expr_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *oscbndl,
				   t_osc_atom_ar_u **out)
{
	if(ast && ast->eval){
		return ast->eval(ast, lexenv, oscbndl, out);
	}else{
		return 0;
	}
}

int osc_expr_ast_expr_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_msg_u **assign_target,
				       long *nlvals,
				       t_osc_atom_u ***lvals)
{
	if(ast && ast->evallval){
		return ast->evallval(ast, lexenv, oscbndl, assign_target, nlvals, lvals);
	}else{
		return 0;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_expr_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		if(ast->copy){
			t_osc_expr_ast_expr *copy = ast->copy(ast);
			//copy->next = osc_expr_ast_expr_copy(ast->next);
			return copy;
		}else{
			t_osc_expr_ast_expr *e = osc_expr_ast_expr_alloc();
			osc_expr_ast_expr_setBrackets(e, osc_expr_ast_expr_getLeftBracket(ast), osc_expr_ast_expr_getRightBracket(ast));
			return e;
		}
	}else{
		return NULL;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_expr_copyAllLinked(t_osc_expr_ast_expr *ast)
{
	if(ast){
		if(ast->copy){
			t_osc_expr_ast_expr *copy = ast->copy(ast);
			copy->next = osc_expr_ast_expr_copyAllLinked(ast->next);
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
	if(e && !e->is_static){
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
		int i = 0;
		while(ll->next){
			ll = ll->next;
		}
		ll->next = expr_to_append;
	}
}

void osc_expr_ast_expr_setNext(t_osc_expr_ast_expr *e, t_osc_expr_ast_expr *expr_to_append)
{
	if(e){
		e->next = expr_to_append;
	}
}

long osc_expr_ast_expr_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e){
		if(e->format){
			long l = 0;
			if(osc_expr_ast_expr_getLeftBracket(e)){
				l += snprintf(buf ? buf + l : NULL, buf ? n - l : 0, "%c", osc_expr_ast_expr_getLeftBracket(e));
			}
			l += e->format(buf ? buf + l : NULL, buf ? n - l : 0, e);
			if(osc_expr_ast_expr_getRightBracket(e)){
				l += snprintf(buf ? buf + l : NULL, buf ? n - l : 0, "%c", osc_expr_ast_expr_getRightBracket(e));
			}
			return l;
		}else{
			return snprintf(buf, n, ";");
		}
	}else{
		return 0;
	}
}

long osc_expr_ast_expr_formatAllLinked(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e){
		long l = 0;
		l += osc_expr_ast_expr_format(buf ? buf + l : NULL, buf ? n - l : 0, e);
		l += snprintf(buf ? buf + l : NULL, buf ? n - l : 0, " ");
		l += osc_expr_ast_expr_formatAllLinked(buf ? buf + l : NULL, buf ? n - l : 0, osc_expr_ast_expr_next(e));
		return l;
	}else{
		return 0;
	}
}

long osc_expr_ast_expr_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e && e->format_lisp){
		long l = 0;
		l += e->format_lisp(buf ? buf + l : NULL, buf ? n - l : 0, e);
		return l;
	}else{
		return 0;
	}
}

long osc_expr_ast_expr_formatAllLinkedLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(e && e->format_lisp){
		long l = 0;
		l += e->format_lisp(buf ? buf + l : NULL, buf ? n - l : 0, e);
		l += osc_expr_ast_expr_formatAllLinkedLisp(buf ? buf + l : NULL, buf ? n - l : 0, osc_expr_ast_expr_next(e));
		return l;
	}else{
		return 0;
	}
}

void osc_expr_ast_expr_printf(t_osc_expr_ast_expr *e)
{
	long l = osc_expr_ast_expr_format(NULL, 0, e);
	char buf[l + 1];
	osc_expr_ast_expr_format(buf, l + 1, e);
	printf("%s\n", buf);
}

t_osc_bndl_u *osc_expr_ast_expr_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_expr *ee = e;
	if(e->tobndl){
		t_osc_bndl_u *b = ee->tobndl(ee);
		if(b){
			long l = osc_expr_ast_expr_format(NULL, 0, ee);
			char buf[l + 1];
			osc_expr_ast_expr_format(buf, l + 1, ee);
			t_osc_msg_u *textmsg = osc_message_u_allocWithString("/text", buf);
			osc_bundle_u_addMsg(b, textmsg);
			//osc_message_u_appendBndl_u(exprlist, b, 1);
		}
		return b;
	}else{
		return NULL;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_expr_fromBndl(t_osc_bndl_u *bndl)
{
	long n = 0;
	t_osc_msg_u **text_msg = NULL;
	osc_bundle_u_lookupAddress(bndl, "/text", &n, &text_msg, 1);
	if(text_msg){
		t_osc_atom_u *a = NULL;
		osc_message_u_getArg(text_msg[0], 0, &a);
		t_osc_expr_ast_expr *e = NULL;
		osc_expr_parser_parseExpr(osc_atom_u_getStringPtr(a), &e);
		return e;
	}else{
		return NULL;
	}
	/*
	printf("**************************************************\n");
	long l = osc_bundle_u_nformat(NULL, 0, bndl, 0);
	char buf[l + 1];
	osc_bundle_u_nformat(buf, l + 1, bndl, 0);
	printf("%s\n", buf);
	printf("**************************************************\n");
	long n = 0;
	t_osc_msg_u **msg = NULL;
	osc_bundle_u_lookupAddress(bndl, "/nodetype", &n, &msg, 1);
	if(n == 1){
		t_osc_atom_u *a = NULL;
		osc_message_u_getArg(msg[0], 0, &a);
		int t = osc_atom_u_getInt(a);
		osc_atom_u_free(a);
		switch(t){
		case OSC_EXPR_AST_NODETYPE_EXPR:
			printf("nodetype expr\n");
			break;
		case OSC_EXPR_AST_NODETYPE_FUNCALL:
			printf("nodetype funcall\n");
			break;
		case OSC_EXPR_AST_NODETYPE_UNARYOP:
			printf("nodetype unaryop\n");
			break;
		case OSC_EXPR_AST_NODETYPE_BINARYOP:
			printf("nodetype binaryop\n");
			break;
		case OSC_EXPR_AST_NODETYPE_ARRAYSUBSCRIPT:
			printf("nodetype arraysubscript\n");
			break;
		case OSC_EXPR_AST_NODETYPE_VALUE:
			printf("nodetype value\n");
			break;
		case OSC_EXPR_AST_NODETYPE_LIST:
			printf("nodetype list\n");
			break;
		case OSC_EXPR_AST_NODETYPE_FUNCTION:
			printf("nodetype function\n");
			return osc_expr_ast_function_fromBndl(bndl);
		case OSC_EXPR_AST_NODETYPE_ASEQ:
			printf("nodetype aseq\n");
			break;
		case OSC_EXPR_AST_NODETYPE_TERNARYCOND:
			printf("nodetype ternarycond\n");
			break;
		case OSC_EXPR_AST_NODETYPE_LET:
			printf("nodetype let\n");
			break;
		}
	}
	return NULL;
	*/
}

size_t osc_expr_ast_expr_sizeof(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->objsize;
	}else{
		return 0;
	}
}

void osc_expr_ast_expr_setBrackets(t_osc_expr_ast_expr *e, char leftbracket, char rightbracket)
{
	if(e){
		e->leftbracket = leftbracket;
		e->rightbracket = rightbracket;
	}
}

char osc_expr_ast_expr_getLeftBracket(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->leftbracket;
	}
	return 0;
}

char osc_expr_ast_expr_getRightBracket(t_osc_expr_ast_expr *e)
{
	if(e){
		return e->rightbracket;
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_expr_alloc(void)
{
	t_osc_expr_ast_expr *e = (t_osc_expr_ast_expr *)osc_mem_alloc(sizeof(t_osc_expr_ast_expr));
	osc_expr_ast_expr_init(e,
			       OSC_EXPR_AST_NODETYPE_EXPR,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       NULL,
			       sizeof(t_osc_expr_ast_expr));
	return e;
}
