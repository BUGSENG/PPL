/* Some incorrect uses of the functions of PPL.
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

using std::invalid_argument;

namespace {

void
error1() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::affine_preimage(v, expr, d): it is illegal
    // to apply to a expression with the denominator
    // equal to zero.
    Coefficient d = 0;
    bd.affine_preimage(x, x + 1, d);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error2() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::affine_preimage(v, expr, d): it is illegal
    // to apply it to an expression whose space dimension is
    // greather than the space dimension of the BDS.
    bd.affine_preimage(y, z);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error3() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::generalized_affine_image(v, r, expr, d): it is illegal
    // to use a strict relation symbol.
    bd.generalized_affine_image(x, LESS_THAN, x + 1);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error4() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::generalized_affine_image(v, r, expr, d): it is illegal
    // to use a strict relation symbol.
    bd.generalized_affine_image(x, GREATER_THAN, x + 1);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error5() {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::generalized_affine_image(v, r, expr, d): it is illegal
    // to apply to a expression with the denominator
    // equal to zero.
    Coefficient d = 0;
    bd.generalized_affine_image(x, LESS_THAN_OR_EQUAL, x + 1, d);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error6() {
  Variable x(0);
  Variable y(1);
  Variable z(2);

  TBD_Shape bd(2);
  bd.add_constraint(x >= y);

  try {
    // This is an incorrect use of the function
    // BD_Shape::generalized_affine_image(v, r, expr, d): it is illegal
    // to apply it to an expression whose space dimension is
    // greather than the BDS's space dimension.
    bd.generalized_affine_image(y, GREATER_THAN_OR_EQUAL, z);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error7() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);

  try {
    // This is an incorrect use of the function
    // BD_Shape::generalized_affine_image(lhs, r, rhs):
    // it is illegal to use a variable in the `rhs' expression that
    // does not appear in the BDS.

    bd.generalized_affine_image(A + B, GREATER_THAN_OR_EQUAL, B + C);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error8() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 1);

  try {
    // This is an incorrect use of function
    // BD_Shape::generalized_affine_image(lhs, r, rhs):
    // it is illegal to use a variable in the `lhs' expression that
    // does not appear in the BDS.
    bd.generalized_affine_image(B + C, LESS_THAN_OR_EQUAL, A + 1);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error9() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 1);

  try {
    // This is an incorrect use of function
    // BD_Shape::relation_with(c):
    // it is illegal to use a constraint that is
    // dimensional incompatible with the BDS.
    Poly_Con_Relation rel = bd.relation_with(C - B <= 2);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(3);
  bd.add_constraint(A >= 1);

  try {
    // This is an incorrect use of function
    // BD_Shape::relation_with(c):
    // it is illegal to use a constraint with three
    // dimensions.
    Poly_Con_Relation rel = bd.relation_with(A - C - B <= 2);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error11() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(3);
  bd.add_constraint(A >= 1);

  try {
    // This is an incorrect use of function
    // BD_Shape::relation_with(c):
    // it is illegal to use a constraint that is
    // not a bounded difference.
    Poly_Con_Relation rel = bd.relation_with(A - 2*B <= 2);
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

void
error12() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 1);

  try {
    // This is an incorrect use of function
    // BD_Shape::relation_with(c):
    // it is illegal to use a generator that is
    // dimensional incompatible with the BDS.
    Poly_Gen_Relation rel = bd.relation_with(ray(C));
  }
  catch (invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
  }
  catch (...) {
    exit(1);
  }
}

} // namespace

int
main() TRY {

  error1();
  error2();
  error3();
  error4();
  error5();
  error6();
  error7();
  error8();
  error9();
  error10();
  error11();
  error12();

  return 0;
}
CATCH
