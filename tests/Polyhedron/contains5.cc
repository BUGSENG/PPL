/* Test Polyhedron::contains(const Polyhedron&),
   Polyhedron::strictly_contains(const Polyhedron&),
   and operator!=(const Polyhedron&, const Polyhedron&).
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

  Constraint_System cs;
  cs.insert(A >= B);
  cs.insert(B >= 0);
  C_Polyhedron ph1(cs);
  C_Polyhedron ph2(ph1);
  ph2.add_constraint(A == B);

  bool ok = ph1.contains(ph2);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");

  if (!ok)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= B);
  cs.insert(B >= 0);
  NNC_Polyhedron ph1(cs);
  NNC_Polyhedron ph2(ph1);
  ph2.add_constraint(A > B);

  bool ok = ph1.strictly_contains(ph2);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");

  if (!ok)
    exit(1);
}

void
test3() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs1;
  cs1.insert(A >= B);
  cs1.insert(B >= 0);
  C_Polyhedron ph1(cs1);
  Constraint_System cs2;
  cs2.insert(A >= B);
  cs2.insert(A <= 0);
  C_Polyhedron ph2(cs2);

  bool ok = (ph1 != ph2);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");

  if (!ok)
    exit(1);
}

void
test4() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(B >= 0);
  NNC_Polyhedron ph1(cs);
  NNC_Polyhedron ph2(ph1);
  ph2.add_constraint(A > 0);

  bool ok = ph1.strictly_contains(ph2);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");

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

  return 0;
}
CATCH
