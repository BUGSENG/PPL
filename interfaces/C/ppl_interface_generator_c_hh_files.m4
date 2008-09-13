m4_define(`dnl', `m4_dnl')`'dnl
m4_divert(-1)

dnl This m4 file generates the file ppl_c_h
dnl using the code in ppl_interface_generator_c_h_code.m4.

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
dnl m4_include(`ppl_interface_generator_c_h_code.m4')
m4_include(`ppl_interface_generator_c_procedure_generators.m4')

m4_divert`'dnl
%<--%<--%<-- ppl_c_domains.hh
/* C interface code: inclusion of all domain declarations.
m4_include(`ppl_interface_generator_copyright')dnl
*/
m4_divert(-1)

dnl m4_pre_extra_class_code(Class, CPP_Class, Class_Kind)
dnl Prefix extra code for each class.
m4_define(`m4_declaration_code', `dnl
m4_ifelse(m4_class_group$1, product,
            `typedef @CPP_CLASS@ @CPPDEF_CLASS@;')

DECLARE_CONVERSIONS(m4_interface_class`'$1, @CPPDEF_CLASS@)
')

m4_define(`m4_pre_extra_class_code', `dnl
m4_define(`m4_current_interface', m4_interface_class`'$1)`'dnl
%<--%<--%<-- ppl_c_domains.hh
`#'include "ppl_c_`'m4_current_interface.hh"
%<--%<--%<-- ppl_c_`'m4_current_interface.hh
/* C m4_current_interface interface code: declarations.
m4_include(`ppl_interface_generator_copyright')`'dnl
*/

dnl PPL_TYPE_DECLARATION(m4_interface_class$1);
dnl
dnl /*! \name Functions Related to m4_interface_class$1 */
dnl /*@{*/

m4_replace_all_patterns_in_string($1,
                                  `m4_declaration_code($1)',
                                  m4_pattern_list)

m4_undefine(`m4_current_interface')`'dnl

')

dnl m4_post_extra_class_code(Class, CPP_Class, Class_Kind)
dnl Postfix extra code for each class.
m4_define(`m4_post_extra_class_code', `dnl
dnl PPL_DECLARE_OUTPUT_FUNCTIONS(m4_interface_class$1)
')

m4_divert`'dnl
dnl
dnl Output the fixed preamble.
dnl As the preamble has quotes, first change the quote characters.
m4_changequote(`@<<@',`@>>@')@<<@@>>@dnl
dnl m4_include(@<<@ppl_interface_generator_c_h_preamble@>>@)@<<@@>>@dnl
dnl Change the quote characters back to the standard.
m4_changequote`'dnl
dnl
dnl Generate the non-fixed part of the file.
m4_all_code`'dnl
dnl
dnl End of file generation.
