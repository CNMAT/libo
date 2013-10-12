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

#include <string.h>
#include <stdio.h>
#include "osc.h"
#include "osc_bundle_u.h"
#include "osc_bundle_s.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_hashtab.h"
#include "osc_expr_rec.h"
#include "osc_expr_funcdefs.h"
#include "osc_expr_funcdecls.h"

#include "osc_expr_funcdecls_records.h"

// this gets created the first time it's needed and then it's never freed
static t_osc_bndl_u *osc_expr_funcdecls_functionBundle = NULL;
static t_osc_bndl_s *osc_expr_funcdecls_categoryBundle = NULL;

//i hope i don't have to add any more ;)
static char *osc_expr_funcdecls_typestrings[] = {"", "number", "list", "", "string", "", "", "", "atom", "", "", "", "", "", "", "", "expression", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "OSC address", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "boolean", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "function", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "timetag"};

#define OSC_EXPR_FUNCDECLS_MAX_TYPESTRING_LEN strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_NUMBER]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_LIST]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_STRING]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_ATOM]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_EXPR]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_OSCADDRESS]) + strlen(osc_expr_funcdecls_typestrings[OSC_EXPR_ARG_TYPE_BOOLEAN]) + 1 + 12 // commas, spaces, and null byte

static t_osc_hashtab *osc_expr_funcdecls_categories = NULL;

static int osc_expr_funcdecls_numrecs = sizeof(osc_expr_funcdecls) / sizeof(t_osc_expr_rec);

t_osc_expr_rec *osc_expr_funcdecls_lookupFunction(char *name)
{
	t_osc_expr_rec *rec = NULL;
	int i;
	for(i = 0; i < osc_expr_funcdecls_numrecs; i++){
		if(!strcmp(name, osc_expr_funcdecls[i].name)){
			rec = osc_expr_funcdecls + i;
			break;
		}
	}
	return rec;
}

void osc_expr_funcdecls_makeFunctionBundle(void)
{
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	int i;
	char buf[128];
	for(i = 0; i < osc_expr_funcdecls_numrecs; i++){
		int j = 0;
		t_osc_expr_rec r = osc_expr_funcdecls[i];
		while(r.categories[j]){
			t_osc_msg_u *m = osc_message_u_alloc();
			sprintf(buf, "/doc/category%s/%s", r.categories[j++], r.name);
			osc_message_u_setAddress(m, buf);
			osc_message_u_appendString(m, r.name);
			osc_bundle_u_addMsg(b, m);
		}
	}
	osc_expr_funcdecls_functionBundle = b;
}

void osc_expr_funcdecls_makeCategoryBundle_cb(char *key, void *val, void *bundle)
{
	t_osc_msg_u *m = osc_message_u_alloc();
	const char * const str = "/doc/categories/list";
	int n = strlen(str) + strlen(key) + 1;
	char buf[n];
	snprintf(buf, n, "%s%s", str, key);
	osc_message_u_setAddress(m, buf);
	osc_bundle_u_addMsg(bundle, m);
}

void osc_expr_funcdecls_makeCategoryBundle(void)
{
	if(!osc_expr_funcdecls_categories){
		t_osc_hashtab *ht = osc_hashtab_new(0, NULL);
		for(int i = 0; i < osc_expr_funcdecls_numrecs; i++){
			int j = 0;
			char *cat = osc_expr_funcdecls[i].categories[j];
			while(cat){
				osc_hashtab_store(ht, strlen(cat), cat, NULL);
				cat = osc_expr_funcdecls[i].categories[++j];
			}
		}
		osc_expr_funcdecls_categories = ht;
	}
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	osc_hashtab_foreach(osc_expr_funcdecls_categories, osc_expr_funcdecls_makeCategoryBundle_cb, (void *)b);
	long len = 0;
	char *ptr = NULL;
	osc_bundle_u_serialize(b, &len, &ptr);
	osc_expr_funcdecls_categoryBundle = osc_bundle_s_alloc(len, ptr);
	osc_bundle_u_free(b);
}

t_osc_bndl_s *osc_expr_funcdecls_getCategories(void)
{
	if(!osc_expr_funcdecls_categoryBundle){
		osc_expr_funcdecls_makeCategoryBundle();
	}
	return osc_expr_funcdecls_categoryBundle;
}

t_osc_err osc_expr_funcdecls_getFunctionsForCategory(char *cat, long *len, char **ptr)
{
	if(!cat){
		return OSC_ERR_INVAL;
	}
	if(!osc_expr_funcdecls_functionBundle){
		osc_expr_funcdecls_makeFunctionBundle();
	}
	char buf[128];
	if(strncmp(cat, "/doc/category", 13)){
		sprintf(buf, "/doc/category%s", cat);
	}else{
		sprintf(buf, "%s", cat);
	}
	t_osc_msg_ar_u *ar = NULL;
	osc_bundle_u_lookupAddress(osc_expr_funcdecls_functionBundle, buf, &ar, 0);
	if(ar){
		t_osc_bndl_u *bndl = osc_bundle_u_alloc();
		osc_bundle_u_addMsgArrayCopy(bndl, ar);
		osc_message_array_u_free(ar);
		osc_bundle_u_serialize(bndl, len, ptr);
		osc_bundle_u_free(bndl);
	}
	return OSC_ERR_NONE;
}

t_osc_err osc_expr_funcdecls_getDocForFunction(char *function_name, t_osc_bndl_u **bndl)
{
	t_osc_expr_rec *rec = osc_expr_funcdecls_lookupFunction(function_name);
	if(rec){
		return osc_expr_funcdecls_formatFunctionDoc(rec, bndl);
	}else{
		return OSC_ERR_EXPR_FUNCTIONNOTFOUND;
	}
}

t_osc_err osc_expr_funcdecls_formatFunctionDoc(t_osc_expr_rec *rec, t_osc_bndl_u **bndl)
{
	if(!rec){
		return OSC_ERR_EXPR_FUNCTIONNOTFOUND;
	}
	t_osc_bndl_u *b = osc_bundle_u_alloc();
	t_osc_message_u *name = osc_message_u_alloc();
	osc_message_u_setAddress(name, "/doc/function/name");
	osc_message_u_appendString(name, rec->name);
	osc_bundle_u_addMsg(b, name);

	t_osc_message_u *sig = osc_message_u_alloc();
	osc_message_u_setAddress(sig, "/doc/function/signature");
	osc_message_u_appendString(sig, rec->signature);
	osc_bundle_u_addMsg(b, sig);

	t_osc_message_u *docstring = osc_message_u_alloc();
	osc_message_u_setAddress(docstring, "/doc/function/docstring");
	osc_message_u_appendString(docstring, rec->docstring);
	osc_bundle_u_addMsg(b, docstring);

	t_osc_message_u *num_required_args = osc_message_u_alloc();
	osc_message_u_setAddress(num_required_args, "/doc/function/num_required_args");
	osc_message_u_appendInt32(num_required_args, rec->num_required_args);
	osc_bundle_u_addMsg(b, num_required_args);

	t_osc_message_u *num_optional_args = osc_message_u_alloc();
	osc_message_u_setAddress(num_optional_args, "/doc/function/num_optional_args");
	osc_message_u_appendInt32(num_optional_args, rec->num_optional_args);
	osc_bundle_u_addMsg(b, num_optional_args);

	int i;
	for(i = 0; i < rec->num_required_args; i++){
		if(rec->required_args_names){
			if(rec->required_args_names[i]){
				char a[128];
				sprintf(a, "/doc/function/arg/required/name/%d", i + 1);
				t_osc_msg_u *m = osc_message_u_alloc();
				osc_message_u_setAddress(m, a);
				osc_message_u_appendString(m, rec->required_args_names[i]);
				osc_bundle_u_addMsg(b, m);

				sprintf(a, "/doc/function/arg/required/types/%d", i + 1);
				t_osc_msg_u *mt = osc_message_u_alloc();
				osc_message_u_setAddress(mt, a);
				char ts[OSC_EXPR_FUNCDECLS_MAX_TYPESTRING_LEN];
				osc_expr_funcdecls_formatTypes(rec->required_args_types[i], ts);
				osc_message_u_appendString(mt, ts);
				osc_bundle_u_addMsg(b, mt);
			}
		}
 	}
	for(i = 0; i < abs(rec->num_optional_args); i++){
		if(rec->optional_args_names){
			if(rec->optional_args_names[i]){
				char a[128];
				sprintf(a, "/doc/function/arg/optional/name/%d", i + 1);
				t_osc_msg_u *m = osc_message_u_alloc();
				osc_message_u_setAddress(m, a);
				osc_message_u_appendString(m, rec->optional_args_names[i]);
				osc_bundle_u_addMsg(b, m);

				sprintf(a, "/doc/function/arg/optional/types/%d", i + 1);
				t_osc_msg_u *mt = osc_message_u_alloc();
				osc_message_u_setAddress(mt, a);
				char ts[OSC_EXPR_FUNCDECLS_MAX_TYPESTRING_LEN];
				osc_expr_funcdecls_formatTypes(rec->optional_args_types[i], ts);
				osc_message_u_appendString(mt, ts);
				osc_bundle_u_addMsg(b, mt);
			}
		}
	}
	t_osc_msg_u *categories = osc_message_u_alloc();
	osc_message_u_setAddress(categories, "/doc/function/categories");
	i = 0;
	while(rec->categories[i]){
		osc_message_u_appendString(categories, rec->categories[i++]);
	}
	osc_bundle_u_addMsg(b, categories);

	*bndl = b;
	return OSC_ERR_NONE;
}

void osc_expr_funcdecls_formatTypes(int types, char *str)
{
	if(!str){
		return;
	}
	char *ptr = str;
	char *fmt = "%s";
	if(types & OSC_EXPR_ARG_TYPE_NUMBER){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_NUMBER]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_LIST){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_LIST]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_STRING){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_STRING]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_ATOM){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_ATOM]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_EXPR){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_EXPR]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_OSCADDRESS){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_OSCADDRESS]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_BOOLEAN){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_BOOLEAN]);
		fmt = ", %s";
	}
	if(types & OSC_EXPR_ARG_TYPE_FUNCTION){
		ptr += sprintf(ptr, fmt, osc_expr_funcdecls_typestrings[types & OSC_EXPR_ARG_TYPE_FUNCTION]);
		fmt = ", %s";
	}
}
