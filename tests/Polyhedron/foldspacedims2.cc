/* Test Polyhedron::fold_space_dimensions() for non-closed polyhedra.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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

Variable A(0);
Variable B(1);
Variable C(2);
Variable D(3);

// Test with an empty polyhedron.
void
test1() {
  NNC_Polyhedron ph1(3, EMPTY);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;
  to_fold.insert(A);

  ph1.fold_space_dimensions(to_fold, B);

  NNC_Polyhedron known_result(2, EMPTY);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "*** After folding {A} into B ***");

  if (!ok)
    exit(1);
}

// Trivial fold.
void
test2() {
  NNC_Polyhedron ph1(3);
  ph1.add_constraint(A >= 0);
  ph1.add_constraint(A + B + C < 2);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;

  ph1.fold_space_dimensions(to_fold, B);

  NNC_Polyhedron known_result(3);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A + B + C < 2);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "*** After folding {} into B ***");

  if (!ok)
    exit(1);
}

// Test as given in [GopanDMDRS04] on page 519 but with strict constraints.
void
test3() {
  NNC_Polyhedron ph1(2);
  ph1.add_constraint(A > 1);
  ph1.add_constraint(A < 3);
  ph1.add_constraint(B > 7);
  ph1.add_constraint(B < 12);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;
  to_fold.insert(A);

  ph1.fold_space_dimensions(to_fold, B);

  NNC_Polyhedron known_result(1);
  known_result.add_constraint(A > 1);
  known_result.add_constraint(A < 12);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "***  After folding {A} into B ***");

  if (!ok)
    exit(1);
}

// Test folding several dimensions into a higher dimension.
void
test4() {
  NNC_Polyhedron ph1(3);
  ph1.add_constraint(A > 1);
  ph1.add_constraint(A <= 3);
  ph1.add_constraint(B > 7);
  ph1.add_constraint(B < 12);
  ph1.add_constraint(C == 15);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;
  to_fold.insert(A);
  to_fold.insert(B);

  ph1.fold_space_dimensions(to_fold, C);

  NNC_Polyhedron known_result(1);
  known_result.add_constraint(A > 1);
  known_result.add_constraint(A <= 15);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "***  After folding {A,B} into C ***");

  if (!ok)
    exit(1);
}

// Test folding dimensions into a lower dimension.
void
test5() {
  NNC_Polyhedron ph1(4);
  ph1.add_constraint(A > 0);
  ph1.add_constraint(A + B < 2);
  ph1.add_constraint(C > 0);
  ph1.add_constraint(C + B < 2);
  ph1.add_constraint(D > 0);
  ph1.add_constraint(D + B <= 2);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;
  to_fold.insert(C);
  to_fold.insert(D);

  ph1.fold_space_dimensions(to_fold, A);

  NNC_Polyhedron known_result(2);
  known_result.add_constraint(A > 0);
  known_result.add_constraint(A + B <= 2);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "***  After folding {C,D} into A ***");

  if (!ok)
    exit(1);
}

// Test folding dimensions into an intermediate dimension.
void
test6() {
  NNC_Polyhedron ph1(4);
  ph1.add_constraint(A >= 0);
  ph1.add_constraint(B > 0);
  ph1.add_constraint(A + B <= 2);
  ph1.add_constraint(C > 0);
  ph1.add_constraint(C + B <= 2);
  ph1.add_constraint(D > 0);
  ph1.add_constraint(D + B <= 2);

  print_constraints(ph1, "*** ph1 ***");

  // This is the set of the variables that we want to fold.
  Variables_Set to_fold;
  to_fold.insert(B);
  to_fold.insert(D);

  ph1.fold_space_dimensions(to_fold, C);

  NNC_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A < 2);
  known_result.add_constraint(B > 0);
  known_result.add_constraint(B < 2);

  bool ok = (ph1 == known_result);

  print_constraints(ph1, "***  After folding {B,D} into C ***");

  if (!ok)
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
  return 0;
}
CATCH
