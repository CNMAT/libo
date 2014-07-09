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
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_expr.r"
#include "osc_expr_ast_value.h"
#include "osc_expr_ast_value.r"
#include "osc_expr_lexenv.h"
//#include "osc_rset.h"
//#include "osc_query.h"
#include "osc_message_u.h"
#include "osc_message_iterator_u.h"
#include "osc_expr_builtin.h"


int osc_expr_ast_value_evalInLexEnv(t_osc_expr_ast_expr *ast,
				    t_osc_expr_lexenv *lexenv,
				    t_osc_bndl_u *oscbndl,
				    t_osc_atom_ar_u **out)
{
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)ast;
	if(v){
		switch(v->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				*out = osc_atom_array_u_alloc(1);
				t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
				osc_atom_u_copy(&a, vv);
			}
			return 0;
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				void *tmp = NULL;
				char *sp = osc_atom_u_getStringPtr(vv);
				if((tmp = osc_expr_lexenv_lookup(lexenv, sp))){
					//*out = osc_atom_array_u_copy((t_osc_atom_ar_u *)tmp);
					if(osc_expr_ast_expr_getNodetype((t_osc_expr_ast_expr *)tmp) == OSC_EXPR_AST_NODETYPE_VALUE &&
					   osc_expr_ast_value_getValueType((t_osc_expr_ast_value *)tmp) == OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER){
						t_osc_atom_u *a = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)tmp);
						if(!strcmp(osc_atom_u_getStringPtr(a), sp)){
							*out = osc_atom_array_u_alloc(1);
							osc_atom_u_setExpr(osc_atom_array_u_get(*out, 0), osc_expr_ast_expr_copy(tmp), 1);
							return 0;
						}
					}
					osc_expr_ast_expr_evalInLexEnv((t_osc_expr_ast_expr *)tmp, lexenv, oscbndl, out);
					return 0;
				}else if((tmp = osc_expr_builtin_lookupFunction(sp))){
					*out = osc_atom_array_u_alloc(1);
					t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
					osc_atom_u_setExpr(a, (t_osc_expr_ast_expr *)osc_expr_funcrec_allocLambda((t_osc_expr_funcrec *)tmp), 1);
					return 0;
				}else if((tmp = osc_expr_builtin_lookupOperator(sp))){
					// this doesn't actually work because the parser won't parse
					// an operator used as an identifier.
					*out = osc_atom_array_u_alloc(1);
					t_osc_atom_u *a = osc_atom_array_u_get(*out, 0);
					osc_atom_u_setExpr(a, (t_osc_expr_ast_expr *)osc_expr_funcrec_allocLambda(osc_expr_builtin_lookupFunctionForOperator((t_osc_expr_oprec *)tmp)), 1);
					return 0;
				}else{
					*out = osc_atom_array_u_alloc(1);
					t_osc_atom_u *copy = osc_atom_array_u_get(*out, 0);
					osc_atom_u_setExpr(copy, osc_expr_ast_value_copy((t_osc_expr_ast_expr *)v), 1);
					return OSC_ERR_EXPR_ADDRESSUNBOUND;
				}
			}
		case OSC_EXPR_AST_VALUE_TYPE_LIST:
			{
				*out = osc_atom_array_u_copy((t_osc_atom_ar_u *)osc_expr_ast_value_getList(v));
			}
			return 0;
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				if(!oscbndl){
					return OSC_ERR_EXPR_ADDRESSUNBOUND;
				}
				char *oscaddress = osc_atom_u_getStringPtr(vv);
				t_osc_msg_u **msgar = NULL;
				long nmsgs = 0;
				osc_bundle_u_lookupAddress(oscbndl, oscaddress, &nmsgs, &msgar, 1);
				if(nmsgs > 0 && msgar){
					t_osc_msg_u *m = msgar[0];
					if(m){
						long arg_count = osc_message_u_getArgCount(m);
						*out = osc_atom_array_u_alloc(arg_count);
						int i = 0;
						t_osc_msg_it_u *it = osc_msg_it_u_get(m);
						while(osc_msg_it_u_hasNext(it)){
							t_osc_atom_u *a = osc_msg_it_u_next(it);
							osc_atom_u_copyValue(osc_atom_array_u_get(*out, i), a);
							i++;
						}
						osc_msg_it_u_destroy(it);
						if(msgar){
							osc_mem_free(msgar);
						}
						return 0;
					}
				}
				if(msgar){
					osc_mem_free(msgar);
				}
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
			/*
			{
				t_osc_atom_u **lvals = NULL;
				long nlvals = 0;
				t_osc_msg_u *at = NULL;
				int ret = osc_expr_ast_value_evalLvalInLexEnv(ast, lexenv, oscbndl, &at, &nlvals, &lvals);
				if(nlvals == 0 || !lvals){
					*out = osc_atom_array_u_alloc(1);
					t_osc_atom_u *copy = osc_atom_array_u_get(*out, 0);
					osc_atom_u_setExpr(copy, osc_expr_ast_value_copy((t_osc_expr_ast_expr *)v), 1);
					return OSC_ERR_EXPR_ADDRESSUNBOUND;
				}
				if(!ret && lvals){
					*out = osc_atom_array_u_alloc(nlvals);
					for(int i = 0; i < nlvals; i++){
						t_osc_atom_u *copy = osc_atom_array_u_get(*out, i);
						osc_atom_u_copyValue(copy, lvals[i]);
					}
				}
				if(lvals){
					osc_mem_free(lvals);
				}
				return ret;
			}
			*/
		}
	}
	return 1;
}

int osc_expr_ast_value_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					t_osc_expr_lexenv *lexenv,
					t_osc_bndl_u *oscbndl,
					t_osc_msg_u **assign_target,
					long *nlvals,
					t_osc_atom_u ***lvals)
{
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)ast;
	if(v){
		switch(v->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
		case OSC_EXPR_AST_VALUE_TYPE_LIST:
			return 1;
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			{
				t_osc_atom_u *vv = osc_expr_ast_value_getValue(v);
				*lvals = NULL;
				if(!oscbndl){
					return OSC_ERR_EXPR_ADDRESSUNBOUND;
				}
				char *oscaddress = osc_atom_u_getStringPtr(vv);
				t_osc_msg_u **msgar = NULL;
				long nmsgs = 0;
				osc_bundle_u_lookupAddress(oscbndl, oscaddress, &nmsgs, &msgar, 1);
				if(nmsgs > 0 && msgar){
					t_osc_msg_u *m = msgar[0];
					if(m){
						long arg_count = osc_message_u_getArgCount(m);
						//*out = osc_atom_array_u_alloc(arg_count);
					        *assign_target = m;
						*lvals = (t_osc_atom_u **)osc_mem_alloc(sizeof(t_osc_atom_u*) * arg_count);
						*nlvals = arg_count;
						int i = 0;
						t_osc_msg_it_u *it = osc_msg_it_u_get(m);
						while(osc_msg_it_u_hasNext(it)){
							t_osc_atom_u *a = osc_msg_it_u_next(it);
							(*lvals)[i] = a;
							i++;
						}
						osc_msg_it_u_destroy(it);
						if(msgar){
							osc_mem_free(msgar);
						}
						return 0;
					}
				}
				if(msgar){
					osc_mem_free(msgar);
				}
				return OSC_ERR_EXPR_ADDRESSUNBOUND;
			}
		}
	}
	return 0;
}

t_osc_expr_ast_expr *osc_expr_ast_value_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		if(osc_expr_ast_value_getValueType((t_osc_expr_ast_value *)ast) == OSC_EXPR_AST_VALUE_TYPE_LIST){
			t_osc_atom_ar_u *ar = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)ast);
			if(ar){
				t_osc_atom_ar_u *copy = osc_atom_array_u_copy(ar);
				return (t_osc_expr_ast_expr *)osc_expr_ast_value_allocList(copy);
			}else{
				return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(NULL, 0);
			}
		}else{
			t_osc_atom_u *atom = osc_expr_ast_value_getValue((t_osc_expr_ast_value *)ast);
			if(atom){
				t_osc_atom_u *copy = NULL;
				osc_atom_u_copy(&copy, atom);
				return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(copy, ((t_osc_expr_ast_value *)ast)->valuetype);
			}else{
				return (t_osc_expr_ast_expr *)osc_expr_ast_value_alloc(NULL, 0);
			}
		}
	}else{
		return NULL;
	}
}

long osc_expr_ast_value_format(char *buf, long n, t_osc_expr_ast_expr *v)
{
	t_osc_expr_ast_value *vv = (t_osc_expr_ast_value *)v;
	if(vv){
		switch(vv->valuetype){
		case OSC_EXPR_AST_VALUE_TYPE_LITERAL:
			return osc_atom_u_nformat(buf, n, osc_expr_ast_value_getValue(vv), 0);
		case OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER:
		case OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS:
			return snprintf(buf, n, "%s", osc_atom_u_getStringPtr(osc_expr_ast_value_getValue(vv)));
		case OSC_EXPR_AST_VALUE_TYPE_LIST:
			{
				long count = 0;
				t_osc_atom_ar_u *ar = (t_osc_atom_ar_u *)osc_expr_ast_value_getValue(vv);
				if(buf){
					for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
						count += osc_atom_u_nformat(buf + count, n - count, osc_atom_array_u_get(ar, i), 0);
					}
				}else{
					for(int i = 0; i < osc_atom_array_u_getLen(ar); i++){
						count += osc_atom_u_nformat(NULL, 0, osc_atom_array_u_get(ar, i), 0);
					}
				}
				return count;
			}
		}
	}
	return 0;
}

void osc_expr_ast_value_free(t_osc_expr_ast_expr *v)
{
	if(v){
		t_osc_expr_ast_value *vv = (t_osc_expr_ast_value *)v;
		if(osc_expr_ast_value_getValueType(vv) == OSC_EXPR_AST_VALUE_TYPE_LIST){
			osc_atom_array_u_free(osc_expr_ast_value_getList(vv));
		}else{
			osc_atom_u_free(osc_expr_ast_value_getValue(vv));
		}
		osc_mem_free(v);
	}
}

t_osc_bndl_u *osc_expr_ast_value_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_value *v = (t_osc_expr_ast_value *)e;
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_VALUE);
	int valuetype = osc_expr_ast_value_getValueType(v);
	t_osc_msg_u *value = NULL;
	if(valuetype == OSC_EXPR_AST_VALUE_TYPE_LIST){
		t_osc_atom_ar_u *ar = (t_osc_atom_ar_u *)osc_expr_ast_value_getValue(v);
		value = osc_message_u_allocWithArray("/value", ar);
	}else{
		t_osc_atom_u *a = (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
		t_osc_atom_u *copy = NULL;
		osc_atom_u_copy(&copy, a);
		value = osc_message_u_allocWithAtom("/value", copy);
	}
	t_osc_msg_u *type = osc_message_u_allocWithInt32("/type", valuetype);
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(b, nodetype);
	osc_bundle_u_addMsg(b, value);
	osc_bundle_u_addMsg(b, type);
	return b;
}

t_osc_expr_ast_expr *osc_expr_ast_value_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	return NULL;
}

int osc_expr_ast_value_getValueType(t_osc_expr_ast_value *v)
{
	if(v){
		return v->valuetype;
	}
	return -1;
}

void *osc_expr_ast_value_getValue(t_osc_expr_ast_value *v)
{
	if(v){
		return v->value;
	}
	return NULL;
}

t_osc_atom_u *osc_expr_ast_value_getLiteral(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_u *osc_expr_ast_value_getIdentifier(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_u *osc_expr_ast_value_getOSCAddress(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_u *)osc_expr_ast_value_getValue(v);
}

t_osc_atom_ar_u *osc_expr_ast_value_getList(t_osc_expr_ast_value *v)
{
	return (t_osc_atom_ar_u *)osc_expr_ast_value_getValue(v);
}

void osc_expr_ast_value_setValue(t_osc_expr_ast_value *v, void *a, int valuetype)
{
	if(v){
		v->value = a;
		v->valuetype = valuetype;
	}
}

void osc_expr_ast_value_setLiteral(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setIdentifier(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setOSCAddress(t_osc_expr_ast_value *v, t_osc_atom_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

void osc_expr_ast_value_setList(t_osc_expr_ast_value *v, t_osc_atom_ar_u *a)
{
	osc_expr_ast_value_setValue(v, a, OSC_EXPR_AST_VALUE_TYPE_LIST);
}

t_osc_expr_ast_value *osc_expr_ast_value_alloc(void *a, int type)
{
	t_osc_expr_ast_value *v = osc_mem_alloc(sizeof(t_osc_expr_ast_value));
	if(v){
		osc_expr_ast_expr_init((t_osc_expr_ast_expr *)v,
				       OSC_EXPR_AST_NODETYPE_VALUE,
				       NULL,
				       osc_expr_ast_value_evalInLexEnv,
				       osc_expr_ast_value_evalLvalInLexEnv,
				       osc_expr_ast_value_format,
				       osc_expr_ast_value_format,
				       osc_expr_ast_value_free,
				       osc_expr_ast_value_copy,
				       osc_expr_ast_value_toBndl,
				       osc_expr_ast_value_fromBndl,
				       sizeof(t_osc_expr_ast_value));
		osc_expr_ast_value_setValue(v, a, type);
	}
	return v;
}

t_osc_expr_ast_value *osc_expr_ast_value_allocLiteral(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_LITERAL);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocIdentifier(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_IDENTIFIER);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocOSCAddress(t_osc_atom_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_OSCADDRESS);
}

t_osc_expr_ast_value *osc_expr_ast_value_allocList(t_osc_atom_ar_u *a)
{
	return osc_expr_ast_value_alloc(a, OSC_EXPR_AST_VALUE_TYPE_LIST);
}

