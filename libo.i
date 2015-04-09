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

PyObject *osc_bundle_s_swugged( t_osc_bundle_s *b ) 
{
    return PyString_FromStringAndSize( osc_bundle_s_getPtr(b), osc_bundle_s_getLen(b) );
}

// osc_bundle.c:

char *osc_bundle_s_format_p(t_osc_bundle_s *bndl)
{
	return osc_bundle_s_format(osc_bundle_s_getLen(bndl), osc_bundle_s_getPtr(bndl));
}

int osc_bundle_s_getMsgCount_p(t_osc_bundle_s* bundle)
{
	int count = 0;
	t_osc_err error;
	error = osc_bundle_s_getMsgCount(osc_bundle_s_getLen(bundle), osc_bundle_s_getPtr(bundle), &count);
	if (error == OSC_ERR_NONE) return count;
	else return -1; //// need to figure out what to do here...
}

t_osc_timetag osc_bundle_s_getTimetag_p(t_osc_bundle_s* bundle)
{
	return osc_bundle_s_getTimetag(osc_bundle_s_getLen(bundle), osc_bundle_s_getPtr(bundle));
}

t_osc_bundle_s* osc_bundle_s_setTimetag_p(t_osc_bundle_s* bundle, t_osc_timetag t)
{
	t_osc_bundle_s* copy = NULL;
	osc_bundle_s_deepCopy(&copy, bundle);
	osc_bundle_s_setTimetag(osc_bundle_s_getLen(copy), osc_bundle_s_getPtr(copy), t);
	return copy;
}

t_osc_bndl_it_s *osc_bundle_iterator_s_getIterator_p(t_osc_bundle_s* bundle)
{
	return osc_bundle_iterator_s_getIterator(osc_bundle_s_getLen(bundle), osc_bundle_s_getPtr(bundle));
}


%}
