%module odot

%{
#include "osc_match.h"
#include "osc_bundle_s.h"
#include "osc_bundle_u.h"
#include "osc_bundle_iterator_s.h"
#include "osc_bundle_iterator_u.h"
#include "osc_error.h"
#include "osc_mem.h"
#include "osc_message_s.h"
#include "osc_message_u.h"
#include "osc_message_iterator_s.h"
#include "osc_message_iterator_u.h"
#include "osc_atom_s.h"
#include "osc_atom_u.h"
#include "osc_array.h"
#include "osc_atom_array_s.h"
#include "osc_atom_array_u.h"
#include "osc_expr.h"
#include "osc_vtable.h"
#include "osc_dispatch.h"
#include "osc_hashtab.h"
#include "osc_linkedlist.h"
#include "osc_util.h"
#include "osc_rset.h"
#include "osc_query.h"
#include "osc_strfmt.h"
#include "osc_expr_rec.h"
#include "osc_typetag.h"
#include "contrib/strptime.h"
#include "osc_timetag.h"
#include "osc_serial.h"
#include "osc.h"
%}

%include "osc_match.h"
%include "osc_bundle_s.h"
%include "osc_bundle_u.h"
%include "osc_bundle_iterator_s.h"
%include "osc_bundle_iterator_u.h"
%include "osc_error.h"
%include "osc_mem.h"
%include "osc_message_s.h"
%include "osc_message_u.h"
%include "osc_message_iterator_s.h"
%include "osc_message_iterator_u.h"
%include "osc_atom_s.h"
%include "osc_atom_u.h"
%include "osc_array.h"
%include "osc_atom_array_s.h"
%include "osc_atom_array_u.h"
%include "osc_expr.h"
%include "osc_vtable.h"
%include "osc_dispatch.h"
%include "osc_hashtab.h"
%include "osc_linkedlist.h"
%include "osc_util.h"
%include "osc_rset.h"
%include "osc_query.h"
%include "osc_strfmt.h"
%include "osc_expr_rec.h"
%include "osc_typetag.h"
%include "contrib/strptime.h"
%include "osc_timetag.h"
%include "osc_serial.h"
%include "osc.h"
%inline %{
t_osc_ar *osc_bundle_s_lookupAddress_r(int len, char *buf, const char *address, int fullmatch)
{
	t_osc_ar *ar = NULL;
	osc_bundle_s_lookupAddress(len, buf, address, &ar, fullmatch);
	return ar;
}

t_osc_ar *osc_bundle_u_lookupAddress_r(t_osc_bndl_u *bndl, const char *address, int fullmatch)
{
	t_osc_ar *ar = NULL;
	osc_bundle_u_lookupAddress(bndl, address, &ar, fullmatch);
	return ar;
}

t_osc_bndl_u *osc_bundle_s_deserialize_r(long len, char *ptr)
{
	t_osc_bndl_u *bndl = NULL;
	osc_bundle_s_deserialize(len, ptr, &bndl);
	return bndl;
}

t_osc_msg_s *void_to_osc_message_s(void *p)
{
	return (t_osc_msg_s *)p;
}

t_osc_msg_u *void_to_osc_message_u(void *p)
{
	return (t_osc_msg_u *)p;
}

t_osc_atom_u *osc_message_u_getArg_r(t_osc_msg_u *m, int n)
{
	t_osc_atom_u *a = NULL;
	osc_message_u_getArg(m, n, &a);
	return a;
}

 %}
	 //extern t_osc_msg_s *void_to_osc_message_s(void *p);
