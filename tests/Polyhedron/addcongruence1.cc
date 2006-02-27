/* Test Polyhedron::add_congruence().
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

bool
test01() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_congruence(A %= 0);
  ph.add_congruence((B == 5) / 0);

  C_Polyhedron known_result(2);
  known_result.add_constraint(B == 5);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** ph ***");

  return ok;
}

bool
test02() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);

  ph.add_congruence((A + B %= 3) / 4);
  ph.add_congruence((A == -1) / 0);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A == -1);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** ph ***");

  return ok;
}

bool
test03() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2, EMPTY);

  print_constraints(ph, "*** ph ***");

  C_Polyhedron known_result(ph);

  ph.add_congruence((A - B == 0) / 0);
  ph.add_congruence((A + B %= 1) / 2);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** ph after adding congruences ***");

  return ok;
}

bool
test04() {
  C_Polyhedron ph(0);

  print_constraints(ph, "*** ph ***");

  C_Polyhedron known_result(0);

  ph.add_congruence((Linear_Expression::zero() %= 0) / 2);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** ph after adding congruence ***");

  return ok;
}

bool
test05() {
  C_Polyhedron ph(0);

  print_constraints(ph, "*** ph ***");

  ph.add_congruence((Linear_Expression::zero() %= 1) / 0);

  C_Polyhedron known_result(0, EMPTY);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** ph after adding congruence ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  NEW_TEST(test01);
  NEW_TEST(test02);
  NEW_TEST(test03);
  NEW_TEST(test04);
  NEW_TEST(test05);
END_MAIN
