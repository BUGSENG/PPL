/* Test Grid::generalized_affine_preimage(var, ...).
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

// Simplest expression.
bool
test01() {
  Variable A(0);
  Variable B(1);

  Grid gr1(2);
  gr1.add_congruence(A %= 0);
  gr1.add_congruence(B %= 0);

  print_congruences(gr1, "*** gr1 ***");

  // Equality expression.
  gr1.generalized_affine_preimage(B, LESS_OR_EQUAL, Linear_Expression::zero());

  Grid known_gr(2, EMPTY);
  known_gr.add_grid_generator(grid_point());
  known_gr.add_grid_generator(grid_point(A));
  known_gr.add_grid_generator(grid_line(B));

  bool ok = (gr1 == known_gr);

  print_congruences(gr1, "*** gr1.generalized_affine_preimage(B, LESS_OR_EQUAL, Linear_Expression::zero() ***");

  return ok;
}

// Simple expression, with denominator.
bool
test02() {
  Variable A(0);
  Variable B(1);

  Grid gr1(2);
  gr1.add_congruence(A %= 0);
  gr1.add_congruence((A + B %= 0) / 2);

  print_congruences(gr1, "*** gr1 ***");

  // Equality expression.
  gr1.generalized_affine_preimage(B, EQUAL, A + 1, 2);

  Grid known_gr(2, EMPTY);
  known_gr.add_grid_generator(grid_point(-3*A));
  known_gr.add_grid_generator(grid_point(A));
  known_gr.add_grid_generator(grid_line(B));

  bool ok = (gr1 == known_gr);

  print_congruences(gr1, "*** gr1.generalized_affine_preimage(B, EQUAL, A + 1, 2, 0) ***");

  return ok;
}

// Empty grid.
bool
test03() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(5, EMPTY);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A, LESS_OR_EQUAL, A - 2*C + 3, 4);

  Grid known_gr(5, EMPTY);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A, LESS_OR_EQUAL, A - 2*C + 3, 4) ***");

  return ok;
}

// Empty with congruences.
bool
test04() {
  Variable A(0);

  Grid gr(1);
  gr.add_constraint(A == 0);
  gr.add_constraint(A == 3);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A, LESS_THAN, A + 2, 1);

  Grid known_gr(1, EMPTY);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A, LESS_THAN A + 2, 1) ***");

  return ok;
}

// Universe.
bool
test05() {
  Variable A(0);

  Grid gr(1);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A, LESS_THAN, A + 2, 1);

  Grid known_gr(1);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A, LESS_THAN, A + 2, 1) ***");

  return ok;
}

// Zero denominator.
bool
test06() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.generalized_affine_preimage(B, GREATER_OR_EQUAL, A + 2, 0);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Expression of a greater space dimension than the grid.
bool
test07() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.generalized_affine_preimage(B, GREATER_OR_EQUAL, D + 2, 1);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Variable of a greater space dimension than the grid.
bool
test08() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.generalized_affine_preimage(D, GREATER_OR_EQUAL, A + 2, 1);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// The expression is not EQUAL and there is a non-zero modulus.
bool
test09() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.generalized_affine_preimage(A, GREATER_OR_EQUAL, A + 2, 1, 1);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// The variable occurs in the expression.
bool
test10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(3);
  gr.add_congruence(A %= 0);
  gr.add_congruence((B %= 0) / 2);
  gr.add_congruence((C %= 0) / 3);

  print_congruences(gr, "*** gr ***");

  // Equality expression.
  gr.generalized_affine_preimage(B, GREATER_THAN, A + B);

  Grid known_gr(3, EMPTY);
  known_gr.add_grid_generator(grid_point());
  known_gr.add_grid_generator(grid_line(2*B));
  known_gr.add_grid_generator(parameter(A));
  known_gr.add_grid_generator(parameter(3*C));

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(B, GREATER_THAN, A + B) ***");

  return ok;
}

// Expressions having common variables.
bool
test11() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(3);
  gr.add_congruence((C %= 0) / 3);
  gr.add_constraint(A - 2*B == 1);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A - B + C, GREATER_THAN, 2*A - B - C);

  Grid known_gr(3);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A - B + C, GREATER_THAN, 2*A - B - C) ***");

  return ok;
}

// Expressions where one variable is unaffected.
bool
test12() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Grid gr(3);
  gr.add_constraint(A - B == 0);
  gr.add_constraint(C == 0);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A - B, GREATER_THAN, 2*A - 2*B, 0);

  Grid known_gr(3);
  known_gr.add_constraint(C == 0);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A - B, GREATER_THAN, 2*A - 2*B, 0) ***");

  return ok;
}

// Right hand side expression of greater space dimension than the
// grid.
bool
test13() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Grid gr(3);
  gr.add_congruence(C %= -2);

  try {
    gr.generalized_affine_preimage(B + C, GREATER_THAN, D + 2);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Left hand side expression of space dimension greater than the grid.
bool
test14() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);

  try {
    gr.generalized_affine_preimage(A + D, GREATER_THAN, A + 2);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Universe.
bool
test15() {
  Variable A(0);

  Grid gr(1);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A, GREATER_THAN, A + 2, 1);

  Grid known_gr(1);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A, GREATER_THAN , A + 2, 1, 1) ***");

  return ok;
}

// The relation is not EQUAL and there is a non-zero modulus.
bool
test16() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);

  try {
    gr.generalized_affine_preimage(A + D, GREATER_THAN, A + 2, 1);
  }
  catch (const std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Empty with congruences.
bool
test17() {
  Variable A(0);
  Variable B(1);

  Grid gr(2);
  gr.add_constraint(A == 0);
  gr.add_constraint(A == 3);

  print_congruences(gr, "*** gr ***");

  gr.generalized_affine_preimage(A + B, LESS_THAN, A + 2);

  Grid known_gr(2, EMPTY);

  bool ok = (gr == known_gr);

  print_congruences(gr,
		    "*** gr.generalized_affine_preimage(A, LESS_THAN, A + 2) ***");

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
END_MAIN
