/* Test Polyhedron::shrink_bounding_box().
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
#include "BBox.hh"

namespace {

// This is a non-bounded closed polyhedron consisting of the line x = y.
// The bounding box is the xy plane - the universal polyhedron.
void
test1() {
  Variable x(0);
  Variable y(1);
  C_Polyhedron ph(2);
  ph.add_constraint(x - y >= 0);

  BBox pbox(2);
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(2);
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test1 ph ***");
  nbox.print(nout, "*** test1 nbox ***");
  pbox.print(nout, "*** test1 pbox ***");

  BBox known_box(2);

  known_box.print(nout, "*** test1 known_box ***");

   if (nbox != known_box || pbox != known_box)
     exit(1);
}

// This is non-bounded closed polyhedron  in 2D consisting of a wedge bounded
// by y >= 0 and x >= y.
// The resulting bounding box depends on the complexity class.
void
test2() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.add_constraint(x >= y);
  ph.add_constraint(y >= 0);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test2 ph ***");
  nbox.print(nout, "*** test2 nbox ***");
  pbox.print(nout, "*** test2 pbox ***");

  BBox known_nbox(2);
  known_nbox.raise_lower_bound(0, true, 0, 1);
  known_nbox.raise_lower_bound(1, true, 0, 1);

  BBox known_pbox(2);
  known_pbox.raise_lower_bound(1, true, 0, 1);

  known_nbox.print(nout, "*** test2 known_nbox ***");
  known_pbox.print(nout, "*** test2 known_pbox ***");

  if (nbox != known_nbox || pbox != known_pbox || !(nbox <= pbox))
    exit(1);
}

// This is a bounded non-rectangular closed polyhedron in 2D.
void
test3() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.add_constraint(3*x + y >= 2);
  ph.add_constraint(x <= 4);
  ph.add_constraint(y <= 4);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test3 ph ***");
  nbox.print(nout, "*** test3 nbox ***");
  pbox.print(nout, "*** test3 pbox ***");

  BBox known_nbox(2);
  known_nbox.raise_lower_bound(0, true, -2, 3);
  known_nbox.lower_upper_bound(0, true, 4, 1);
  known_nbox.raise_lower_bound(1, true, -10, 1);
  known_nbox.lower_upper_bound(1, true, 12, 3);

  BBox known_pbox(2);
  known_pbox.lower_upper_bound(0, true, 4, 1);
  known_pbox.lower_upper_bound(1, true, 4, 1);

  known_nbox.print(nout, "*** test3 known_nbox ***");
  known_pbox.print(nout, "*** test3 known_pbox ***");

  if (nbox != known_nbox || pbox != known_pbox || !(nbox <= pbox))
    exit(1);
}

// This is an unbounded closed polyhedron in 4D but bounded in 2D.
void
test4() {
  Variable x(1);
  Variable y(2);
  Variable z(3);

  C_Polyhedron ph(4);
  ph.add_constraint(3*x + y >= 2);
  ph.add_constraint(x <= 4);
  ph.add_constraint(y <= 4);
  ph.add_constraint(z >= 5);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test4 ph ***");
  nbox.print(nout, "*** test4 nbox ***");
  pbox.print(nout, "*** test4 pbox ***");

  BBox known_nbox(4);
  known_nbox.raise_lower_bound(1, true, -2, 3);
  known_nbox.lower_upper_bound(1, true, 4, 1);
  known_nbox.raise_lower_bound(2, true, -10, 1);
  known_nbox.lower_upper_bound(2, true, 12, 3);
  known_nbox.raise_lower_bound(3, true, 15, 3);

  BBox known_pbox(4);
  known_pbox.lower_upper_bound(1, true, 4, 1);
  known_pbox.lower_upper_bound(2, true, 4, 1);
  known_pbox.raise_lower_bound(3, true, 5, 1);

  known_nbox.print(nout, "*** test4 known_nbox ***");
  known_pbox.print(nout, "*** test4 known_pbox ***");

  if (nbox != known_nbox || pbox != known_pbox || !(nbox <= pbox))
    exit(1);
}

// This is the universal, 2-dimensional closed polyhedron.
void
test5() {
  C_Polyhedron ph(2);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test5 ph ***");
  nbox.print(nout, "*** test5 nbox ***");
  pbox.print(nout, "*** test5 pbox ***");

  BBox known_box(2);

  known_box.print(nout, "*** test5 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// This is a zero-dimensional closed polyhedron.
void
test6() {
  C_Polyhedron ph;

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test6 ph ***");
  nbox.print(nout, "*** test6 nbox ***");
  pbox.print(nout, "*** test6 pbox ***");

  BBox known_box(0);

  known_box.print(nout, "*** test6 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// This is an empty closed polyhedron in 2D.
void
test7() {
  C_Polyhedron ph(2, EMPTY);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test7 ph ***");
  nbox.print(nout, "*** test7 nbox ***");
  pbox.print(nout, "*** test7 pbox ***");

  BBox known_box(ph.space_dimension());
  known_box.set_empty();

  known_box.print(nout, "*** test7 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// This is a bounded polyhedron that is a single point.
void
test8() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.add_constraint(x == 2);
  ph.add_constraint(y == 4);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test8 ph ***");
  nbox.print(nout, "*** test8 nbox ***");
  pbox.print(nout, "*** test8 pbox ***");

  BBox known_box(2);
  known_box.raise_lower_bound(0, true, 2, 1);
  known_box.lower_upper_bound(0, true, 2, 1);
  known_box.raise_lower_bound(1, true, 4, 1);
  known_box.lower_upper_bound(1, true, 4, 1);

  known_box.print(nout, "*** test8 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// This is a unit square closed polyhedron.
void
test9() {
  Variable x(0);
  Variable y(1);

  Constraint_System cs;
  cs.insert(x >= 0);
  cs.insert(x <= 1);
  cs.insert(y >= 0);
  cs.insert(y <= 1);

  C_Polyhedron ph(cs);

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  print_constraints(ph, "*** test9 ph ***");
  nbox.print(nout, "*** test9 nbox ***");
  pbox.print(nout, "*** test9 pbox ***");

  BBox known_box(2);
  known_box.raise_lower_bound(0, true, 0, 1);
  known_box.lower_upper_bound(0, true, 1, 1);
  known_box.raise_lower_bound(1, true, 0, 1);
  known_box.lower_upper_bound(1, true, 1, 1);

  known_box.print(nout, "*** test9 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// This is a bounded rectangular closed polyhedron;
void
test10() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.generators();
  ph.add_constraint(x >= 1);
  ph.add_constraint(x <= 3);
  ph.add_constraint(y <= 3);
  ph.add_constraint(y >= 1);

  print_constraints(ph, "*** test10 ph ***");

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  BBox nbox(ph.space_dimension());
  ph.shrink_bounding_box(nbox);

  nbox.print(nout, "*** test10 nbox ***");
  pbox.print(nout, "*** test10 pbox ***");

  BBox known_box(2);
  known_box.raise_lower_bound(0, true, 1, 1);
  known_box.lower_upper_bound(0, true, 3, 1);
  known_box.raise_lower_bound(1, true, 1, 1);
  known_box.lower_upper_bound(1, true, 3, 1);

  known_box.print(nout, "*** test10 known_box ***");

  if (nbox != known_box || pbox != known_box)
    exit(1);
}

// A bounded polyhedron having redundant constraints.
void
test11() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.add_constraint(x == 3);
  ph.add_constraint(y == 1);
  ph.add_constraint(x + y == 4);
  ph.add_constraint(x - y == 2);
  ph.add_constraint(3*x + y == 10);
  ph.add_constraint(x >= 0);
  ph.add_constraint(y <= 5);
  ph.add_constraint(x + 2*y >= 5);

  print_constraints(ph, "*** test11 ph ***");

  BBox pbox(ph.space_dimension());
  ph.shrink_bounding_box(pbox, POLYNOMIAL_COMPLEXITY);

  pbox.print(nout, "*** test11 pbox ***");

  BBox known_box(2);
  known_box.raise_lower_bound(0, true, 3, 1);
  known_box.lower_upper_bound(0, true, 3, 1);
  known_box.raise_lower_bound(1, true, 1, 1);
  known_box.lower_upper_bound(1, true, 1, 1);

  known_box.print(nout, "*** test11 known_box ***");

  if (pbox != known_box)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  test11();

  return 0;
}
CATCH
