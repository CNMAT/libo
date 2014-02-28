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
#include "osc_expr_ast_aseq.h"
#include "osc_expr_ast_aseq.r"

int osc_expr_ast_aseq_evalInLexEnv(t_osc_expr_ast_expr *ast,
				   t_osc_expr_lexenv *lexenv,
				   t_osc_bndl_u *oscbndl,
				   t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_ast_aseq_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_msg_u **assign_target,
				       long *nlvals,
				       t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_aseq_format(char *buf, long n, t_osc_expr_ast_expr *ast)
{
	if(!ast){
		return 0;
	}
	long offset = 0;
	t_osc_expr_ast_aseq *a = (t_osc_expr_ast_aseq *)ast;
	//offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "[");
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_aseq_getMin(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ":");
	if(osc_expr_ast_aseq_getStepIsImplicit(a) == 0){
		t_osc_expr_ast_expr *step = osc_expr_ast_aseq_getStep(a);
		offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, step);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ":");
	}
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_aseq_getMax(a));
	//offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "]");
	return offset;
}

long osc_expr_ast_aseq_formatLisp(char *buf, long n, t_osc_expr_ast_expr *ast)
{
	if(!ast){
		return 0;
	}
	long offset = 0;
	t_osc_expr_ast_aseq *a = (t_osc_expr_ast_aseq *)ast;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(aseq ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_aseq_getMin(a));
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_aseq_getMax(a));
	if(osc_expr_ast_aseq_getStepIsImplicit(a) == 0){
		t_osc_expr_ast_expr *step = osc_expr_ast_aseq_getStep(a);
		offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
		offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, osc_expr_ast_aseq_getStep(a));
	}
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_aseq_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_aseq *a = (t_osc_expr_ast_aseq *)ast;
		t_osc_expr_ast_expr *mincopy = osc_expr_ast_expr_copy(osc_expr_ast_aseq_getMin(a));
		t_osc_expr_ast_expr *maxcopy = osc_expr_ast_expr_copy(osc_expr_ast_aseq_getMax(a));
		t_osc_expr_ast_expr *step = osc_expr_ast_aseq_getStep(a);
		t_osc_expr_ast_expr *stepcopy = NULL;
		if(step){
			stepcopy = osc_expr_ast_expr_copy(osc_expr_ast_aseq_getStep(a));
		}
		t_osc_expr_ast_aseq *copy = osc_expr_ast_aseq_alloc(mincopy, maxcopy, stepcopy);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_aseq_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_expr_ast_expr_free(osc_expr_ast_aseq_getMin((t_osc_expr_ast_aseq *)e));
		osc_expr_ast_expr_free(osc_expr_ast_aseq_getMax((t_osc_expr_ast_aseq *)e));
		osc_expr_ast_expr_free(osc_expr_ast_aseq_getStep((t_osc_expr_ast_aseq *)e));
		osc_mem_free(e);
	}
}

t_osc_bndl_u *osc_expr_ast_aseq_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_aseq *a = (t_osc_expr_ast_aseq *)e;
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_ASEQ);
	t_osc_msg_u *min = osc_message_u_allocWithBndl_u("/min", osc_expr_ast_expr_toBndl(osc_expr_ast_aseq_getMin(a)), 1);
	t_osc_msg_u *max = osc_message_u_allocWithBndl_u("/max", osc_expr_ast_expr_toBndl(osc_expr_ast_aseq_getMax(a)), 1);
	t_osc_msg_u *step = osc_message_u_allocWithBndl_u("/step", osc_expr_ast_expr_toBndl(osc_expr_ast_aseq_getStep(a)), 1);
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(b, nodetype); 
	osc_bundle_u_addMsg(b, min); 
	osc_bundle_u_addMsg(b, max); 
	osc_bundle_u_addMsg(b, step); 
	return b;
}

t_osc_expr_ast_expr *osc_expr_ast_aseq_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_aseq_getMin(t_osc_expr_ast_aseq *e)
{
	if(e){
		return e->min;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_aseq_getMax(t_osc_expr_ast_aseq *e)
{
	if(e){
		return e->max;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_aseq_getStep(t_osc_expr_ast_aseq *e)
{
	if(e){
		return e->step;
	}
	return NULL;
}

int osc_expr_ast_aseq_getStepIsImplicit(t_osc_expr_ast_aseq *e)
{
	if(e){
		return e->step_is_implicit;
	}
	return 0;
}

void osc_expr_ast_aseq_setMin(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *min)
{
	if(e){
		e->min = min;
	}
}

void osc_expr_ast_aseq_setMax(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *max)
{
	if(e){
		e->max = max;
	}
}

void osc_expr_ast_aseq_setStep(t_osc_expr_ast_aseq *e, t_osc_expr_ast_expr *step)
{
	if(e){
		e->step = step;
	}
}

void osc_expr_ast_aseq_setStepIsImplicit(t_osc_expr_ast_aseq *e, int step_is_implicit)
{
	if(e){
		e->step_is_implicit = step_is_implicit;
	}
}

t_osc_expr_ast_aseq *osc_expr_ast_aseq_alloc(t_osc_expr_ast_expr *min, t_osc_expr_ast_expr *max, t_osc_expr_ast_expr *step)
{
	if(!min || !max || !step){
		return NULL;
	}
	t_osc_expr_ast_aseq *e = osc_mem_alloc(sizeof(t_osc_expr_ast_aseq));
	if(!e){
		return NULL;
	}
	/*
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)e,
			       OSC_EXPR_AST_NODETYPE_ASEQ,
			       NULL,
			       osc_expr_ast_aseq_evalInLexEnv,
			       osc_expr_ast_aseq_format,
			       osc_expr_ast_aseq_formatLisp,
			       osc_expr_ast_aseq_free,
			       osc_expr_ast_aseq_copy,
			       osc_expr_ast_aseq_serialize,
			       osc_expr_ast_aseq_deserialize,
			       sizeof(t_osc_expr_ast_aseq));
	*/
	t_osc_expr_funcrec *funcrec = osc_expr_builtin_func_aseq;
	osc_expr_ast_funcall_init((t_osc_expr_ast_funcall *)e,
				  OSC_EXPR_AST_NODETYPE_ASEQ,
				  NULL,
				  NULL,
				  NULL,
				  osc_expr_ast_aseq_format,
				  NULL,
				  NULL,//osc_expr_ast_aseq_free,
				  osc_expr_ast_aseq_copy,
				  osc_expr_ast_aseq_toBndl,
				  osc_expr_ast_aseq_fromBndl,
				  sizeof(t_osc_expr_ast_aseq),
				  funcrec,
				  3,
				  min,
				  max,
				  step);
	osc_expr_ast_aseq_setMin(e, min);
	osc_expr_ast_aseq_setMax(e, max);
	osc_expr_ast_aseq_setStep(e, step);
	osc_expr_ast_aseq_setStepIsImplicit(e, 0);
	return e;
}
