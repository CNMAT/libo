#!/usr/bin/env python

from distutils.core import setup, Extension

libo_module = Extension('_libo',
                        sources=['libo_wrap.c', 'osc_match.c', 'osc_bundle_s.c', 'osc_bundle_u.c', 'osc_bundle_iterator_s.c', 'osc_bundle_iterator_u.c', 'osc_error.c', 'osc_mem.c', 'osc_message_s.c', 'osc_message_u.c', 'osc_message_iterator_s.c', 'osc_message_iterator_u.c', 'osc_atom_s.c', 'osc_atom_u.c', 'osc_array.c', 'osc_atom_array_s.c', 'osc_atom_array_u.c', 'osc_expr.c', 'osc_vtable.c', 'osc_dispatch.c', 'osc_hashtab.c', 'osc_linkedlist.c', 'osc_util.c', 'osc_rset.c', 'osc_query.c', 'osc_strfmt.c', 'osc_expr_rec.c', 'osc_typetag.c', 'contrib/strptime.c', 'osc_timetag.c', 'osc_serial.c', 'osc_scanner.c', 'osc_expr_scanner.c', 'osc_parser.c', 'osc_expr_parser.c'],
                        extra_compile_args=["-std=c99", "-DLINUX_VERSION", "-D_XOPEN_SOURCE", "-O3"])

setup(name = 'libo',
      version = '1.0',
      author = "lots",
      description = """libo""",
      ext_modules = [libo_module],
      py_modules = ["libo"])
