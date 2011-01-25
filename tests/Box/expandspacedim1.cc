/* Test Box::expand_space_dimension().
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
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

bool
test01() {
  Variable A(0);

  TBox box(3);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 1);

  TBox known_result(4);

  bool ok = (box == known_result);

  print_constraints(box, "*** after box.expand_space_dimension(A, 1) ***");

  return ok;
}

bool
test02() {
  //  Variable A(0);
  Variable B(1);

  TBox box(3, EMPTY);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(B, 1);

  TBox known_result(4, EMPTY);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(B, 1) ***");

  return ok;
}

bool
test03() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  TBox box(2);
  box.add_constraint(A >= 0);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 0);

  TBox known_result(2);
  known_result.add_constraint(A >= 0);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(A, 0) ***");

  return ok;
}

bool
test04() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(2, EMPTY);
  box.add_constraint(B >= 0);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 1);

  TBox known_result(3, EMPTY);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(A, 1) ***");

  return ok;
}

bool
test05() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(2);
  box.add_constraint(A >= 0);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 1);

  TBox known_result(3);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(C >= 0);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(A, 1) ***");

  return ok;
}

bool
test06() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  TBox box(2);
  box.add_constraint(A >= 0);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 2);

  TBox known_result(4);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(C >= 0);
  known_result.add_constraint(D >= 0);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(A, 2) ***");

  return ok;
}

bool
test07() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  TBox box(3);
  box.add_constraint(A <= 1);
  box.add_constraint(C == 1);
  box.add_constraint(B <= 1);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(A, 1);
  box.expand_space_dimension(C, 1);

  TBox known_result(5);
  known_result.add_constraint(A <= 1);
  known_result.add_constraint(C == 1);
  known_result.add_constraint(E == 1);
  known_result.add_constraint(B <= 1);
  known_result.add_constraint(D <= 1);

  bool ok = (box == known_result);

  print_constraints(box,
                    "*** box.expand_space_dimension(A, 1);"
                    " box.expand_space_dimension(C, 1) ***");

  return ok;
}

bool
test08() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(2, EMPTY);
  box.add_constraint(A <= 2);
  box.add_constraint(B <= 1);

  print_constraints(box, "*** box ***");

  box.expand_space_dimension(B, 1);

  TBox known_result(3, EMPTY);

  bool ok = (box == known_result);

  print_constraints(box, "*** box.expand_space_dimension(A, 2) ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
  DO_TEST(test05);
  DO_TEST(test06);
  DO_TEST(test07);
  DO_TEST(test08);
END_MAIN
