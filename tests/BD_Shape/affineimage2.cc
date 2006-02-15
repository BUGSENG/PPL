/* Test BD_Shape::affine_image().
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
test1() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TBD_Shape bd(3);
  bd.add_constraint(x <= 1);
  bd.add_constraint(y <= 2);
  bd.add_constraint(z >= 3);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -x);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(x >= -1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(z >= 3);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result);

  print_constraints(bd, "*** bd.affine_image(x, -x) ***");

  return ok;
}

bool
test2() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TBD_Shape bd(3);
  bd.add_constraint(x <= 1);
  bd.add_constraint(y <= 2);
  bd.add_constraint(z >= 3);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -z);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(x <= -3);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(z >= 3);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result);

  print_constraints(bd, "*** bd.affine_image(x, -z) ***");

  return ok;
}

bool
test3() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= 1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -y + 1);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(x <= 0);
  known_result.add_constraint(x >= -1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= 1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result);

  print_constraints(bd, "*** bd.affine_image(x, -y + 1) ***");

  return ok;
}

bool
test4() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= 1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -2*y + 1, -2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(2*x - 2*y == -1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= 1);

  bool ok = check_result(bd, known_result);

  print_constraints(bd, "*** bd.affine_image(x, -2*y + 1, -2) ***");

  return ok;
}

bool
test5() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= 1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -2*y + 1, 2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(2*x <= -1);
  known_result.add_constraint(2*x >= -3);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= 1);

  bool ok = check_result(bd, known_result);

  print_constraints(bd, "*** bd.affine_image(x, -2*y + 1, 2) ***");

  return ok;
}

bool
test6() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(x >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= -1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, 2*x + y + 1);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(x <= 5);
  known_result.add_constraint(x >= 0);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= -1);
  known_result.add_constraint(x - y <= 3);
  known_result.add_constraint(x - y >= 1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result);

  print_constraints(bd, "*** bd.affine_image(x, 2*x + y + 1) ***");

  return ok;
}

bool
test7() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(x >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= -1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -2*x + y + 1);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(x <= 3);
  known_result.add_constraint(x >= -2);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= -1);
  known_result.add_constraint(x - y <= 1);
  known_result.add_constraint(x - y >= -1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result);

  print_constraints(bd, "*** bd.affine_image(x, -2*x + y + 1) ***");

  return ok;
}

bool
test8() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(x >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= -1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, 2*x - 3*y + 1, 5);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(5*x <= 6);
  known_result.add_constraint(x >= -1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= -1);

  bool ok = check_result(bd, known_result, "9.54e-8", "6.75e-8", "4.77e-8");

  print_constraints(bd, "*** bd.affine_image(x, 2*x - 3*y + 1, 5) ***");

  return ok;
}

bool
test9() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(x >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= -1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, -2*x - 3*y + 1, 5);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(5*x <= 4);
  known_result.add_constraint(5*x >= -7);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= -1);

  bool ok = check_result(bd, known_result, "2.15e-7", "1.36e-7", "9.54e-8");

  print_constraints(bd, "*** bd.affine_image(x, -2*x - 3*y + 1, 5) ***");

  return ok;
}

bool
test10() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x <= 1);
  bd.add_constraint(x >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(y >= -1);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, 2*x - 3*y + 1, -5);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(x <= 1);
  known_result.add_constraint(5*x >= -6);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(y >= -1);
  known_result.add_constraint(5*x - 5*y <= 1);
  known_result.add_constraint(5*x - 5*y >= -7);

  bool ok = check_result(bd, known_result, "1.91e-7", "1.17e-7", "9.54e-8");

  print_constraints(bd, "*** bd.affine_image(x, 2*x - 3*y + 1, -5) ***");

  return ok;
}

bool
test11() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TBD_Shape bd(3);
  bd.add_constraint(y >= 0);
  bd.add_constraint(y <= 2);
  bd.add_constraint(z <= 3);

  print_constraints(bd, "*** bd ***");

  bd.affine_image(x, y + 5*z, 3);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(3*x <= 17);
  known_result.add_constraint(y >= 0);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(z <= 3);
  known_result.add_constraint(x - y <= 5);
  known_result.add_constraint(3*x - 3*z <= 8);

  bool ok = check_result(bd, known_result, "1.12e-6", "6.56e-7", "4.77e-7");

  print_constraints(bd, "*** bd.affine_image(x, y + 5*z, 3) ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  NEW_TEST(test1);
  NEW_TEST(test2);
  NEW_TEST(test3);
  NEW_TEST(test4);
  NEW_TEST(test5);
  NEW_TEST(test6);
  NEW_TEST(test7);
  NEW_TEST(test8);
  NEW_TEST(test9);
  NEW_TEST(test10);
  NEW_TEST(test11);
END_MAIN
