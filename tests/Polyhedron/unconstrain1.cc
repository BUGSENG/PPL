/* Test Polyhedron::unconstrain().
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
  Variable B(1);

  C_Polyhedron ph(2, EMPTY);
  print_generators(ph, "*** ph ***");

  ph.unconstrain(A);

  C_Polyhedron known_result(2, EMPTY);
  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(A) ***");

  return ok;
}

bool
test02() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2, EMPTY);
  print_generators(ph, "*** ph ***");

  Variables_Set vs(A, B);
  ph.unconstrain(vs);

  C_Polyhedron known_result(2, EMPTY);
  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(vs) ***");

  return ok;
}

bool
test03() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint(A == B);
  ph.add_constraint(B >= 0);
  print_generators(ph, "*** ph ***");

  ph.unconstrain(B);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);

  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(B) ***");

  return ok;
}

bool
test04() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint(A == B);
  ph.add_constraint(B >= 0);
  print_generators(ph, "*** ph ***");

  Variables_Set vs(B);
  ph.unconstrain(vs);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);

  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(vs) ***");

  return ok;
}

bool
test05() {
  C_Polyhedron ph(0, EMPTY);
  print_generators(ph, "*** ph ***");

  Variables_Set vs;
  ph.unconstrain(vs);

  C_Polyhedron known_result(0, EMPTY);

  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(vs) ***");

  return ok;
}

bool
test06() {
  C_Polyhedron ph(0);
  print_generators(ph, "*** ph ***");

  Variables_Set vs;
  ph.unconstrain(vs);

  C_Polyhedron known_result(0);

  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(vs) ***");

  return ok;
}

bool
test07() {
  try {
    C_Polyhedron ph(1);
    // This is an invalid use of the method unconstrain(Variable):
    // it is illegal to (try to) unconstrain a space dimension
    // that is not in the polyhedron.
    ph.unconstrain(Variable(7));

    // It is an error if the exception is not thrown.
  }
  catch (std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
    return true;
  }
  catch (...) {
    // It is an error if the wrong exception is thrown.
  }
  return false;
}

bool
test08() {
  try {
    C_Polyhedron ph(1);
    // This is an invalid use of the method unconstrain(Variable):
    // it is illegal to (try to) unconstrain a space dimension
    // that is not in the polyhedron.
    Variables_Set vs(Variable(0), Variable(3));
    ph.unconstrain(vs);

    // It is an error if the exception is not thrown.
  }
  catch (std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
    return true;
  }
  catch (...) {
    // It is an error if the wrong exception is thrown.
  }
  return false;
}

bool
test09() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  C_Polyhedron ph(5);
  ph.add_constraint(A + 3*B - 7 == 9);
  ph.add_constraint(A - 3*B - D + E >= 0);
  ph.add_constraint(C <= D);
  ph.add_constraint(E <= 2*B + D);
  ph.add_constraint(E >= 0);
  print_generators(ph, "*** ph ***");

  Variables_Set vs(A, B);
  vs.insert(D);
  ph.unconstrain(vs);

  C_Polyhedron known_result(5);
  known_result.add_constraint(E >= 0);

  bool ok = (ph == known_result);

  print_generators(ph, "*** after ph.unconstrain(vs) ***");

  return ok;
}

bool
test10() {
  try {
    C_Polyhedron ph(8);
    // This is an invalid use of the method unconstrain(Variable):
    // it is illegal to (try to) unconstrain a space dimension
    // that is not in the polyhedron.
    ph.unconstrain(Variable(8));

    // It is an error if the exception is not thrown.
  }
  catch (std::invalid_argument& e) {
    nout << "invalid_argument: " << e.what() << endl << endl;
    return true;
  }
  catch (...) {
    // It is an error if the wrong exception is thrown.
  }
  return false;
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
END_MAIN
