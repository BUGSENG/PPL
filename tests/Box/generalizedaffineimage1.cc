/* Test Box::generalized_affine_image().
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2022 BUGSENG srl (http://bugseng.com)

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
site: http://bugseng.com/products/ppl/ . */

#include "ppl_test.hh"

namespace {

bool
test01() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(B, GREATER_OR_EQUAL, A+2);

  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(A <= 4);
  cs.insert(B >= 2);

  Rational_Box known_result(cs);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image"
                    "(B, GREATER_OR_EQUAL, A+2) ***");
  return ok;
}

bool
test02() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(B >= 0);
  box.add_constraint(A >= 0);

  print_constraints(box, "*** box ***");

  Rational_Box known_result(box);

  box.generalized_affine_image(A, EQUAL, A + 2);

  known_result.affine_image(A, A + 2);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image"
                    "(A, EQUAL, A + 2) ***");

  return ok;
}

bool
test03() {
  Variable A(0);
  Variable B(1);

  TBox box(2, EMPTY);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(A, LESS_OR_EQUAL, B + 1);

  Rational_Box known_result(2, EMPTY);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image"
                    "(A, LESS_OR_EQUAL, B + 1) ***");

  return ok;
}

bool
test04() {
  Variable x(0);
  Variable y(1);

  TBox box(3);

  box.add_constraint(x >= 2);
  box.add_constraint(x <= 5);
  box.add_constraint(y <= 2);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(x, GREATER_OR_EQUAL, 2*x - 2, 2);

  Rational_Box known_result(3);
  known_result.add_constraint(x >= 1);
  known_result.add_constraint(y <= 2);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image(x, "
                    "GREATER_OR_EQUAL, 2*x - 2, 2) ***");

  return ok;
}

bool
test05() {
  Variable x(0);
  Variable y(1);

  TBox box(3);
  box.add_constraint(x >= 2);
  box.add_constraint(x <= 5);
  box.add_constraint(y >= 2);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(y, GREATER_THAN, 2*x - 2, -2);

  Rational_Box known_result(3);
  known_result.add_constraint(x >= 2);
  known_result.add_constraint(x <= 5);
  known_result.add_constraint(y > -4);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image(y, "
                    "GREATER_THAN, 2*x - 2, -2) ***");

  return ok;
}

bool
test06() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(A, LESS_THAN, Linear_Expression(1));

  Rational_Box known_result(2);
  known_result.add_constraint(A < 1);
  known_result.add_constraint(B <= 5);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image(A, "
                    "LESS_THAN, 1) ***");

  return ok;
}

bool
test07() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);
  box.add_constraint(B >= 0);

  print_constraints(box, "*** box ***");

  box.generalized_affine_image(A, GREATER_OR_EQUAL, Linear_Expression(1));

  Rational_Box known_result(2);
  known_result.add_constraint(A >= 1);
  known_result.add_constraint(B >= 0);
  known_result.add_constraint(B <= 5);

  bool ok = check_result(box, known_result);

  print_constraints(box,
                    "*** box.generalized_affine_image(A, "
                    "GREATER_OR_EQUAL, 1) ***");

  return ok;
}

bool
test08() {
  Variable A(0);
  Variable B(1);
  Linear_Expression e1(A);
  Linear_Expression e2(A);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);
  box.add_constraint(B >= 0);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(e1, GREATER_OR_EQUAL, e2);
  ph.generalized_affine_image(e1, GREATER_OR_EQUAL, e2);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(A, "
                    "GREATER_OR_EQUAL, A) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(A, "
                    "GREATER_OR_EQUAL, A) ***");

  return ok;
}

bool
test09() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);
  box.add_constraint(B >= 0);
  box.add_constraint(C >= -2);
  box.add_constraint(C <= 2);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(-B, LESS_OR_EQUAL, Linear_Expression(1));
  ph.generalized_affine_image(-B, LESS_OR_EQUAL, Linear_Expression(1));

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(-B, "
                    "LESS_OR_EQUAL, 1) ***");
  print_constraints(Rational_Box(ph),
                    "*** ph.generalized_affine_image(-B, "
                    "LESS_OR_EQUAL, 1) ***");

  return ok;
}

bool
test10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Linear_Expression e1(C);
  Linear_Expression e2(A + 1);

  TBox box(3);
  box.add_constraint(A >= 0);
  box.add_constraint(A <= 4);
  box.add_constraint(B <= 5);
  box.add_constraint(C <= 2);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(e1, LESS_OR_EQUAL, e2);
  ph.generalized_affine_image(e1, LESS_OR_EQUAL, e2);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box, "*** box.generalized_affine_image(C, "
                    "LESS_OR_EQUAL, A + 1) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(C, "
                    "LESS_OR_EQUAL, A + 1) ***");

  return ok;
}

bool
test11() {
  Variable x(0);
  Variable y(1);

  TBox box(2);
  box.add_constraint(x == 4);
  box.add_constraint(y <= 0);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(3*x + 2, GREATER_OR_EQUAL, 2*x - 3);
  ph.generalized_affine_image(3*x + 2, GREATER_OR_EQUAL, 2*x - 3);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(3*x + 2, "
                    "GREATER_OR_EQUAL, 2*x - 3) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(3*x + 2, "
                    "GREATER_OR_EQUAL, 2*x - 3) ***");

  return ok;
}

bool
test12() {
  Variable x(0);
  Variable y(1);

  TBox box(2);
  box.add_constraint(x <= 7);
  box.add_constraint(x >= 0);
  box.add_constraint(y >= 1);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(Linear_Expression(6), EQUAL, 3*x - 4);
  ph.generalized_affine_image(Linear_Expression(6), EQUAL, 3*x - 4);

  bool ok = check_result(box, Rational_Box(ph),
                         "2.39e-7", "1.78e-7", "1.59e-7");

  print_constraints(box,
                    "*** box.generalized_affine_image(6, EQUAL, 3*x - 4) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image("
                    "6, EQUAL, 3*x - 4) ***");

  return ok;
}

bool
test13() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(B >= 0);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(2*B + 3*A,
                              LESS_OR_EQUAL, Linear_Expression(1));
  ph.generalized_affine_image(2*B + 3*A,
                              LESS_OR_EQUAL, Linear_Expression(1));

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(3*A + 2*B, "
                    "LESS_OR_EQUAL, 1) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(3*A + 2*B, "
                    "LESS_OR_EQUAL, 1) ***");

  return ok;
}

bool
test14() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A == 0);
  box.add_constraint(B >= 1);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(-2*A + 5, EQUAL, -4*B);
  ph.generalized_affine_image(-2*A + 5, EQUAL, -4*B);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(-2*A + 5, "
                    "EQUAL, -4*B) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(-2*A + 5, "
                    "EQUAL, -4*B) ***");

  return ok;
}

bool
test15() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A <= 1);
  box.add_constraint(B <= 1);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(2*B - 5, EQUAL, 3*B);
  ph.generalized_affine_image(2*B - 5, EQUAL, 3*B);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(2*B - 5, "
                    "EQUAL, 3*B) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image("
                    "2*B - 5, EQUAL, 3*B) ***");

  return ok;
}

bool
test16() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A <= 1);
  box.add_constraint(B <= 1);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(Linear_Expression(-4),
                               LESS_OR_EQUAL, A - 3*B + 2*C);
  ph.generalized_affine_image(Linear_Expression(-4),
                               LESS_OR_EQUAL, A - 3*B + 2*C);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(-4, "
                    "LESS_OR_EQUAL, A - 3*B + 2*C) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image("
                    "-4, LESS_OR_EQUAL, A - 3*B + 2*C) ***");

  return ok;
}

bool
test17() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A <= 1);
  box.add_constraint(B <= 1);
  box.add_constraint(C <= 3);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(Linear_Expression(2),
                              GREATER_OR_EQUAL, A - 3*B + 2*C);
  ph.generalized_affine_image(Linear_Expression(2),
                              GREATER_OR_EQUAL, A - 3*B + 2*C);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(2, "
                    "GREATER_OR_EQUAL, A - 3*B + 2*C) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image("
                    "2, GREATER_OR_EQUAL, A - 3*B + 2*C) ***");

  return ok;
}

bool
test18() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A <= 0);
  box.add_constraint(B <= 1);
  box.add_constraint(B >= 2);
  box.add_constraint(C <= 3);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(-2*A - B - 1,
                              GREATER_OR_EQUAL, 3*A + B + 4*C - 2);
  ph.generalized_affine_image(-2*A - B - 1,
                              GREATER_OR_EQUAL, 3*A + B + 4*C - 2);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(-2*A - B - 1, "
                    "GREATER_OR_EQUAL, 3*A + B + 4*C - 2) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image("
                    "-2*A - B - 1, GREATER_OR_EQUAL, 3*A + B + 4*C - 2) ***");

  return ok;
}

bool
test19() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A <= 1);
  box.add_constraint(B <= 1);
  box.add_constraint(C <= 3);

  print_constraints(box, "*** box ***");

  C_Polyhedron ph(box);

  box.generalized_affine_image(-2*C + 3, GREATER_OR_EQUAL, -3*B + 4);
  ph.generalized_affine_image(-2*C + 3, GREATER_OR_EQUAL, -3*B + 4);

  bool ok = check_result(box, Rational_Box(ph));

  print_constraints(box,
                    "*** box.generalized_affine_image(-2*C + 3, "
                    "GREATER_OR_EQUAL, -3*B + 4) ***");
  print_constraints(Rational_Box(ph),
                    "*** Rational_Box(ph).generalized_affine_image(-2*C + 3, "
                    "GREATER_OR_EQUAL, -3*B + 4) ***");

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
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST(test17);
  DO_TEST(test18);
  DO_TEST(test19);
END_MAIN
