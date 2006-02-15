/* Test BD_Shape::generalized_affine_image().
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
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  bd.add_constraint(A <= B);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(B, GREATER_THAN_OR_EQUAL, A+2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A <= 4);
  known_result.add_constraint(B - A >= 2);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image"
		        "(B, GREATER_THAN_OR_EQUAL, A+2) ***");

  return ok;
}

bool
test2() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(B >= 0);
  bd.add_constraint(A - B >= 0);

  print_constraints(bd, "*** bd ***");

  BD_Shape<mpq_class> known_result(bd);

  bd.generalized_affine_image(A, EQUAL, A + 2);

  known_result.affine_image(A, A + 2);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image"
		    "(A, EQUAL, A + 2) ***");

  return ok;
}

bool
test3() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2, EMPTY);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(A, LESS_THAN_OR_EQUAL, B + 1);

  BD_Shape<mpq_class> known_result(2, EMPTY);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image"
		    "(A, LESS_THAN_OR_EQUAL, B + 1) ***");

  return ok;
}

bool
test4() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(3);

  bd.add_constraint(x >= 2);
  bd.add_constraint(x - y <= 3);
  bd.add_constraint(y <= 2);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(x,GREATER_THAN_OR_EQUAL, 2*x - 2, 2);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(x >= 1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(- y <= 1);
  known_result.add_constraint(x - y >= -1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(x, "
                        "GREATER_THAN_OR_EQUAL, 2*x - 2, 2) ***");

  return ok;
}

bool
test5() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(3);
  bd.add_constraint(x >= 2);
  bd.add_constraint(x - y <= 3);
  bd.add_constraint(y <= 2);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(y,GREATER_THAN_OR_EQUAL, 2*x - 2, 2);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(x >= 2);
  known_result.add_constraint(x <= 5);
  known_result.add_constraint(y >= x - 1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(y, "
                        "GREATER_THAN_OR_EQUAL, 2*x - 2, 2) ***");

  return ok;
}

bool
test6() {
  Variable A(0);
  Variable B(1);
  Linear_Expression e1(A);
  Linear_Expression e2(A);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  bd.add_constraint(A <= B);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(e1, GREATER_THAN_OR_EQUAL, e2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B >= 0);
  known_result.add_constraint(B <= 5);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(A, "
                        "GREATER_THAN_OR_EQUAL, A) ***");

  return ok;
}

bool
test7() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  bd.add_constraint(A - B <= 0);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(A, GREATER_THAN_OR_EQUAL, Linear_Expression(1));

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A >= 1);
  known_result.add_constraint(B >= 0);
  known_result.add_constraint(B <= 5);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(A, "
                        "GREATER_THAN_OR_EQUAL, 1) ***");

  return ok;
}

bool
test8() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(3);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  bd.add_constraint(A - B <= 0);
  bd.add_constraint(B - C == 2);
  bd.add_constraint(C - A <= -2);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(-B, LESS_THAN_OR_EQUAL, Linear_Expression(1));

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A <= 4);
  known_result.add_constraint(A - C == 2);
  known_result.add_constraint(C >= -2);
  known_result.add_constraint(C <= 2);
  known_result.add_constraint(B >= -1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(-B, "
                        "LESS_THAN_OR_EQUAL, 1) ***");

  return ok;
}

bool
test9() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Linear_Expression e1(C);
  Linear_Expression e2(A + 1);

  TBD_Shape bd(3);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  bd.add_constraint(C <= 2);
  bd.add_constraint(C - A <= 2);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(e1, LESS_THAN_OR_EQUAL, e2);

  BD_Shape<mpq_class> known_result(3);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A <= 4);
  known_result.add_constraint(B <= 5);
  known_result.add_constraint(C - A <= 1);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(C, "
                        "LESS_THAN_OR_EQUAL, A + 1) ***");

  return ok;
}

bool
test10() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(A, LESS_THAN_OR_EQUAL, Linear_Expression(1));

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A <= 1);
  known_result.add_constraint(B <= 5);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(A, "
                        "LESS_THAN_OR_EQUAL, 1) ***");

  return ok;
}

bool
test11() {
  Variable A(0);
  Variable B(1);
  Linear_Expression e1(A);
  Linear_Expression e2(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(e1, EQUAL, e2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A == 1);
  known_result.add_constraint(B <= 5);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(A, EQUAL, 1) ***");

  return ok;
}

bool
test12() {
  Variable A(0);
  Variable B(1);
  Linear_Expression e1(B - 3);
  Linear_Expression e2(B + 1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);

  print_constraints(bd, "*** bd ***");

  bd.generalized_affine_image(e1, EQUAL, e2);

  BD_Shape<mpq_class> known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A <= 4);
  known_result.add_constraint(B <= 9);

  bool ok = (BD_Shape<mpq_class>(bd) == known_result) ;

  print_constraints(bd, "*** bd.generalized_affine_image(B-3,EQUAL, B+1) ***");

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
  NEW_TEST(test12);
END_MAIN
