/* Test Polyhedron::constrains().
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

#define TEST_PREDICATE_TRUE(pred)		\
  if (!pred) {					\
    nout << "!" #pred << endl;			\
    ok = false;					\
  }

#define TEST_PREDICATE_FALSE(pred)		\
  if (pred) {					\
    nout << #pred << endl;			\
    ok = false;					\
  }

namespace {

bool
test01() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint(0*A == 0);

  bool ok = true;

  TEST_PREDICATE_FALSE(ph.constrains(A));
  TEST_PREDICATE_FALSE(ph.constrains(B));

  ph.add_constraint(0*A == 1);

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  ph.add_generator(point());
  ph.add_generator(line(A+B));

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  ph.add_generator(line(A-B));

  TEST_PREDICATE_FALSE(ph.constrains(A));
  TEST_PREDICATE_FALSE(ph.constrains(B));

  ph.add_constraint(A >= 1);

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_FALSE(ph.constrains(B));

  ph.add_constraint(B >= 2);

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  ph.add_constraint(A <= B);

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  return ok;
}

bool
test02() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3, EMPTY);

  bool ok = true;

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));
  TEST_PREDICATE_TRUE(ph.constrains(C));

  return ok;
}

bool
test03() {
  Variable A(0);

  C_Polyhedron ph(0);
  ph.add_constraint(Linear_Expression::zero() == 1);

  try {
    (void) ph.constrains(A);
  }
  catch (std::invalid_argument& e) {
    return true;
  }
  catch (...) {
    return false;
  }

  return false;
}

bool
test04() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint(B >= 0);
  ph.generators();
  ph.add_constraint(B >= 7);

  bool ok = true;

  TEST_PREDICATE_FALSE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  return ok;
}

bool
test05() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2, EMPTY);
  ph.add_generator(point());
  ph.constraints();
  ph.add_generator(ray(A));
  ph.add_generator(ray(-A));

  bool ok = true;

  TEST_PREDICATE_FALSE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  return ok;
}

bool
test06() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint_and_minimize(A >= 0);

  bool ok = true;

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_FALSE(ph.constrains(B));

  ph.add_constraint(B >= A);

  TEST_PREDICATE_TRUE(ph.constrains(A));
  TEST_PREDICATE_TRUE(ph.constrains(B));

  return ok;
}

bool
test07() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(A >= B+1);
  ph.add_constraint(A <= B);

  bool ok = true;

  TEST_PREDICATE_TRUE(ph.constrains(C));
  TEST_PREDICATE_TRUE(ph.constrains(B));
  TEST_PREDICATE_TRUE(ph.constrains(A));

  return ok;
}

bool
test08() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(A >= B);
  ph.add_generator(point());
  ph.generators();
  ph.add_constraint(A <= B-1);

  bool ok = true;

  TEST_PREDICATE_TRUE(ph.constrains(C));
  TEST_PREDICATE_TRUE(ph.constrains(B));
  TEST_PREDICATE_TRUE(ph.constrains(A));

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
