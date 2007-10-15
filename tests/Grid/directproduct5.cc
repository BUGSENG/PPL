/* Test Direct_Product<NNC_Polyhedron, Grid> reduction.
   Copyright (C) 2001-2007 Roberto Bagnara <bagnara@cs.unipr.it>

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

// FIXME: rename once Reduced_Product name decided

using namespace Parma_Polyhedra_Library::IO_Operators;

namespace {

typedef Open_Product<NNC_Polyhedron, Grid> Product;

// reduce()
bool
test01() {
  Variable A(0);

  Product dp(1);
  dp.add_constraint(A > 7);
  dp.add_constraint(A < 7);

  bool ok = dp.domain2().is_universe();

  dp.reduce();

  ok &= dp.domain2().is_empty();

  print_constraints(dp.domain1(), "*** dp.domain1() ***");
  print_congruences(dp.domain2(), "*** dp.domain2() ***");

  return ok;
}

#if 0
// reduce()
bool
testr0() {
  Variable A(0);

  Product dp(1);

  dp.add_congruence((A %= 0) / 2);
  dp.add_constraint(A >= 7);

  bool ok = dp.reduce();

  Product known_dp(1);
  known_dp.add_congruence((A %= 0) / 2);
  known_dp.add_constraint(A >= 8);

  ok &= (dp == known_dp);

  return ok;
}

// reduce() where there is a ph divisor > 1
bool
testr2() {
  Variable A(0);

  Product dp(1);

  dp.add_congruence((A %= 0) / 3);
  dp.add_constraint(3*A >= 2);

  Product original_dp = dp;

  bool ok = dp.reduce();

  if (dp.domain1().strictly_contains(original_dp.domain1())) {
    ok = false;
    nout << "Polyhedron was reduced." << endl;
  }
  else
    nout << "Polyhedron stayed the same." << endl;

  if (dp.domain2().strictly_contains(original_dp.domain2())) {
    ok = false;
    nout << "Grid was reduced." << endl;
  }
  else
    nout << "Grid stayed the same." << endl;

  return ok;
}

// reduce() where there is a ph divisor > 1
bool
testr1() {
  Variable A(0);

  Product dp(1);

  dp.add_congruence((A %= 0) / 2);
  dp.add_constraint(3*A >= 2);

  bool ok = dp.reduce();

  Product known_dp(1);
  known_dp.add_congruence((A %= 0) / 2);
  known_dp.add_constraint(A >= 2);

  ok &= (dp == known_dp);

  return ok;
}
#endif

// FIX tests that are specific to Open_Product

// is_bounded(), due to intersection.
bool
test02() {
  Variable A(0);
  Variable B(1);

  Product dp(2, EMPTY);
  dp.add_grid_generator(grid_point());
  dp.add_grid_generator(grid_line(A + B));
  dp.add_generator(point());
  dp.add_generator(line(A));

  bool ok = dp.is_bounded();

  print_constraints(dp.domain1(), "*** dp.domain1() ***");
  print_congruences(dp.domain2(), "*** dp.domain2() ***");

  return ok;
}

// reduce() when the added equality is implied.
bool
test03() {
  Variable A(0);

  Product dp(1);
  dp.add_constraint(A >= 7);
  dp.add_constraint(A <= 7);

  bool ok = dp.domain2().is_universe();

  dp.reduce();

  Product dp1(1);
  dp1.add_congruence((A %= 7) / 0);

  ok &= !dp.domain2().is_universe();
  ok &= (dp == dp1);

  print_constraints(dp.domain1(), "*** dp.domain1() ***");
  print_congruences(dp.domain2(), "*** dp.domain2() ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
END_MAIN
