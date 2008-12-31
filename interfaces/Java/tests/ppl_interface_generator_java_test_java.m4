m4_define(`dnl', `m4_dnl')`'dnl
m4_divert(-1)

dnl This m4 file generates the file ppl_java_classes_test.java
dnl using the code in ppl_interface_generator_java_test_code.m4.
dnl
dnl Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>
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

dnl ==================================================================
dnl Common files are included here
dnl ==================================================================

m4_include(`ppl_interface_generator_common.m4')
m4_include(`ppl_interface_generator_common_dat.m4')
m4_include(`ppl_interface_generator_java_procedure_generators.m4')
m4_include(`ppl_interface_generator_java_test_java_code.m4')

dnl ==================================================================
dnl Copyright and Domain independent code is added first
dnl ==================================================================
m4_divert
/* Java code for checking all classes.  -*- C++ -*-
m4_include(`ppl_interface_generator_copyright') */

m4_include(`ppl_java_tests_common')

    public static void main(String[] args) {
	ppl_java_generated_tests test1 = new ppl_java_generated_tests();
        // Initialize output variables.
        PPL_Test.initialize();
	test1.initialize();

dnl ==================================================================
dnl Add test statements (one for each domain instantiation.
dnl ==================================================================
	// Here generated tests are called.
m4_divert(1)
    }

dnl ==================================================================
dnl Add the code for domain dependent tests.
dnl ==================================================================
    // Here generated tests are defined.
m4_divert(2)`'dnl
}
m4_divert(-1)

dnl ==================================================================
dnl Define a test statement for each domain, for m4_divert(1)
dnl ==================================================================
m4_pushdef(`m4_one_class_code', `
    test1.run_`'m4_interface_class$1`'_test();
')

m4_divert`'dnl
m4_all_code`'dnl
m4_undivert(1)`'dnl
m4_divert(-1)
m4_popdef(`m4_one_class_code')

dnl ==================================================================
dnl Define code for all tests to check all methods, for m4_divert(2)
dnl ==================================================================
dnl Prefix extra code for each domain.
m4_pushdef(`m4_pre_extra_class_code', `
    public boolean run_`'m4_interface_class$1`'_test() {
    try {

')
dnl Postfix extra code for each domain.
m4_pushdef(`m4_post_extra_class_code', `dnl
}
catch (parma_polyhedra_library.Overflow_Error_Exception e) {
System.out.println("*Overflow detected*::exception caught");
}
System.gc();
return true;

    }

')

m4_divert`'dnl
m4_all_code`'dnl
m4_undivert(2)`'dnl
