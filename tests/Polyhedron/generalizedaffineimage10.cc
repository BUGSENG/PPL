/* Test Polyhedron::generalized_affine_image() with a linear expression
   as the left hand side: when the set of variables of left hand side
   and the set of the variable of the right hand side are disjoint.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include "ppl_test.hh"

namespace {

void
test1() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(C == 0);
  ph.add_constraint(A + 3*B == 2);

  print_constraints(ph, "*** ph ***");
  print_generators(ph, "*** ph ***");

  ph.generalized_affine_image(A - C, EQUAL, B + 3);

  C_Polyhedron known_result(3);
  known_result.add_constraint(A - C == B + 3);

  bool ok = (ph == known_result);

  print_generators(ph, "*** After ph.generalized_affine_image"
		    "(A - C, EQUAL, B + 3) ***");
  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - C, EQUAL, B + 3) ***");

  if (!ok)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(C == 0);
  ph.add_constraint(A - 2*B >= 2);

  print_constraints(ph, "*** ph ***");
  print_generators(ph, "*** ph ***");

  ph.generalized_affine_image(A - C, GREATER_THAN_OR_EQUAL, B + 3);

  C_Polyhedron known_result(3);
  known_result.add_constraint(A - B - C >= 3);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - C, GREATER_THAN_OR_EQUAL, B + 3) ***");
  print_generators(ph, "*** After ph.generalized_affine_image"
		    "(A - C, GREATER_THAN_OR_EQUAL, B + 3) ***");

  if (!ok)
    exit(1);
}

void
test3() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(C == 0);
  ph.add_constraint(A >= B);

  print_constraints(ph, "*** ph ***");

  ph.generalized_affine_image(A - C, LESS_THAN_OR_EQUAL, B - 1);

  C_Polyhedron known_result(ph);
  known_result.add_constraint(A - B - C <= 1);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - C, LESS_THAN_OR_EQUAL, B - 1) ***");
  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - C, LESS_THAN_OR_EQUAL, B - 1) ***");

  if (!ok)
    exit(1);
}

void
test4() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  NNC_Polyhedron ph(3);
  ph.add_constraint(A - C == 0);
  ph.add_constraint(A >= B);

  print_constraints(ph, "*** ph ***");

  ph.generalized_affine_image(A - 2*C, LESS_THAN, B - 1);

  NNC_Polyhedron known_result(ph);
  known_result.add_constraint(A - B - 2*C < 1);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - 2*C, LESS_THAN, B - 1) ***");
  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - 2*C, LESS_THAN, B - 1) ***");

  if (!ok)
    exit(1);
}

void
test5() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  NNC_Polyhedron ph(3);
  ph.add_constraint(A - 2*C == 0);
  ph.add_constraint(A > B - 2);

  print_constraints(ph, "*** ph ***");

  ph.generalized_affine_image(A - 2*C + 3, GREATER_THAN, B - 1);

  NNC_Polyhedron known_result(ph);
  known_result.add_constraint(A - B - 2*C + 4 > 0);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - 2*C + 3, GREATER_THAN, B - 1) ***");
  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A - 2*C + 3, GREATER_THAN, B - 1) ***");

  if (!ok)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  DO_TEST(test1);
  DO_TEST(test2);
  DO_TEST(test3);
  DO_TEST(test4);
  DO_TEST(test5);

  return 0;
}
CATCH
