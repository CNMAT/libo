%module libo

%include "stdint.i"

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
#include "osc_parser.h"
%}

%typemap(out) t_osc_bndl_s* {
#ifdef SWIGPYTHON
	$result = PyString_FromStringAndSize(osc_bundle_s_getPtr($1), osc_bundle_s_getLen($1));
#elif defined(SWIGJAVASCRIPT) || defined(SWIG_JAVASCRIPT_V8)

#endif
}

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
%include "osc_parser.h"
%inline %{
 %}
