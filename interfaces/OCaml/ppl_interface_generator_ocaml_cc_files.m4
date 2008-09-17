m4_define(`dnl', `m4_dnl')`'dnl
m4_divert(-1)dnl

dnl This m4 file generates the file ppl_ocaml.cc
dnl using the code in ppl_interface_generator_ocaml_cc_code.m4.

dnl Copyright (C) 2001-2008 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 3 of the License, or (at your
dnl option) any later version.
dnl
dnl The PPL is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
dnl
dnl For the most up-to-date information see the Parma Polyhedra Library
dnl site: http://www.cs.unipr.it/ppl/ .

dnl Include files defining macros that generate the non-fixed part.
m4_include(`ppl_interface_generator_ocaml_cc_code.m4')
m4_include(`ppl_interface_generator_ocaml_procedure_generators.m4')

dnl -----------------------------------------------------------------
dnl Macros needed for the class-dependent code.
dnl -----------------------------------------------------------------
dnl Prefix extra code for each class.
m4_define(`m4_pre_extra_class_code', `dnl
m4_define(`m4_current_interface', m4_interface_class`'$1)`'dnl
%<--%<--%<-- ppl_ocaml_`'m4_current_interface`'.cc
/* OCaml m4_current_interface interface code.
m4_include(`ppl_interface_generator_copyright')
*/

`#'include "ppl_ocaml_domains.hh"

')

dnl -----------------------------------------------------------------
dnl Generate the main class-dependent code.
dnl -----------------------------------------------------------------
m4_divert`'dnl
m4_all_code
dnl
dnl End of file generation.