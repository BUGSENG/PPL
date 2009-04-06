/* Test Octagonal_Shape::upper_bound_assign_if_exact().
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

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
  TOctagonal_Shape octs_empty(0, EMPTY);
  TOctagonal_Shape octs_universe(0, UNIVERSE);

  // Testing all combinations for 0-dim polyhedra.
  bool ok = true;
  TOctagonal_Shape octs;

  // empty, empty
  octs = octs_empty;
  ok &= octs.upper_bound_assign_if_exact(octs_empty);
  ok &= (octs == octs_empty);
  print_constraints(octs, "*** empty union empty ***");

  // empty, universe
  octs = octs_empty;
  ok &= octs.upper_bound_assign_if_exact(octs_universe);
  ok &= (octs == octs_universe);
  print_constraints(octs, "*** empty union universe ***");

  // universe, empty
  octs = octs_universe;
  ok &= octs.upper_bound_assign_if_exact(octs_empty);
  ok &= (octs == octs_universe);
  print_constraints(octs, "*** universe union empty ***");

  // universe, universe
  octs = octs_universe;
  ok &= octs.upper_bound_assign_if_exact(octs_universe);
  ok &= (octs == octs_universe);
  print_constraints(octs, "*** universe union universe ***");

  return ok;
}

bool
test02() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x >= -2);
  octs1.add_constraint(x <= -1);
  octs1.add_constraint(y >= 0);
  octs1.add_constraint(y <= 2);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x >= 1);
  octs2.add_constraint(x <= 2);
  octs2.add_constraint(y >= 0);
  octs2.add_constraint(y <= 2);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(octs1);

  bool ok = !octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test03() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x >= -2);
  octs1.add_constraint(x <= 0);
  octs1.add_constraint(y >= 0);
  octs1.add_constraint(y <= 2);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x >= 0);
  octs2.add_constraint(x <= 2);
  octs2.add_constraint(y >= 0);
  octs2.add_constraint(y <= 2);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(2, UNIVERSE);
  known_result.add_constraint(x >= -2);
  known_result.add_constraint(x <= 2);
  known_result.add_constraint(y >= 0);
  known_result.add_constraint(y <= 2);

  bool ok = octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test04() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x == 0);
  octs1.add_constraint(y == 0);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x >= 0);
  octs2.add_constraint(x <= 2);
  octs2.add_constraint(y >= -2);
  octs2.add_constraint(y <= 2);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(octs2);

  bool ok = octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test05() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x >= 0);
  octs1.add_constraint(y == 0);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x >= 0);
  octs2.add_constraint(y >= 2);
  octs2.add_constraint(y <= 4);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(octs1);

  bool ok = !octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test06() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x == y);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x == 0);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(octs1);

  bool ok = !octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test07() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x >= y);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x >= 0);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(octs1);

  bool ok = !octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test08() {
  Variable x(0);
  Variable y(1);

  TOctagonal_Shape octs1(2, UNIVERSE);
  octs1.add_constraint(x >= y);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(2, UNIVERSE);
  octs2.add_constraint(x <= y);

  print_constraints(octs2, "*** octs2 ***");

  TOctagonal_Shape known_result(2, UNIVERSE);

  bool ok = octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == known_result);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test09() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TOctagonal_Shape octs(3, UNIVERSE);
  octs.add_constraint(x >= 0);
  octs.add_constraint(x <= 2);
  octs.add_constraint(y >= 0);
  octs.add_constraint(y <= 4);
  octs.add_constraint(z >= 0);
  octs.add_constraint(z <= 4);
  octs.add_constraint(x - y <= 2);
  octs.add_constraint(z - y <= 2);

  TOctagonal_Shape octs1(octs);
  octs1.add_constraint(z <= 3);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(octs);
  octs2.add_constraint(x - y <= 1);

  TOctagonal_Shape known_result(octs);

  C_Polyhedron ph1(octs1);
  C_Polyhedron ph2(octs2);

  bool exact = ph1.upper_bound_assign_if_exact(ph2);
  nout << "In the c polyhedra domain, upper_bound_assign_if_exact() returns: "
       << exact << std::endl;
  print_constraints(ph1, "*** ph1.upper_bound_assign_if_exact(ph2) ***");

  bool ok = octs1.upper_bound_assign_if_exact(octs2);

  ok &= (octs1 == octs);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test10() {
  Variable x(0);
  Variable y(1);
  Variable z(2);
  Variable w(3);

  TOctagonal_Shape octs(4, UNIVERSE);
  octs.add_constraint(x - y <= 4);
  octs.add_constraint(z - w <= 4);
  octs.add_constraint(x - w <= 5);
  octs.add_constraint(z <= 0);
  octs.add_constraint(z - y <= 1);
  octs.add_constraint(y + w >= -1);

  TOctagonal_Shape octs1(octs);
  octs1.add_constraint(x - y <= 2);

  print_constraints(octs1, "*** octs1 ***");

  TOctagonal_Shape octs2(octs);
  octs2.add_constraint(z - w <= 2);

  TOctagonal_Shape known_result(octs);

  bool ok = octs1.upper_bound_assign_if_exact(octs2);
  ok &= (octs1 == octs);

  print_constraints(octs1, "*** octs1.upper_bound_assign_if_exact(octs2) ***");

  return ok;
}

bool
test11() {
  //  const dimension_type dim = 200;
  const dimension_type dim = 50;
  Constraint_System cs;
  for (dimension_type i = 1; i < dim; ++i) {
    Variable x(i);
    cs.insert(x >= 0);
    cs.insert(x <= 4);
  }

  Variable x(0);

  TOctagonal_Shape hypercube1(cs);
  hypercube1.add_constraint(x >= 0);
  hypercube1.add_constraint(x <= 4);

  TOctagonal_Shape hypercube2(cs);
  hypercube2.add_constraint(x >= 2);
  hypercube2.add_constraint(x <= 6);

  TOctagonal_Shape known_result(cs);
  known_result.add_constraint(x >= 0);
  known_result.add_constraint(x <= 6);

  bool ok = hypercube1.upper_bound_assign_if_exact(hypercube2);

  ok &= (hypercube1 == known_result);

  print_constraints(hypercube1, "*** hyp1 ***");
  print_constraints(hypercube2, "*** hyp2 ***");

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
  DO_TEST(test09);
  DO_TEST(test10);
  DO_TEST(test11);
END_MAIN
