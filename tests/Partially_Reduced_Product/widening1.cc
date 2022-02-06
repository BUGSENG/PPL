/* Test Product<NNC_Polyhedron, Grid>::widening().
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
#include "partially_reduced_product_test.hh"

typedef NNC_Polyhedron DOMAIN1;
typedef Grid DOMAIN2;
typedef Domain_Product<DOMAIN1x, DOMAIN2x>::Constraints_Product Product;

namespace {

// widening_assign
bool
test01() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product prp_prev(3);
  prp_prev.refine_with_constraint(C == 0);
  prp_prev.refine_with_constraint(A - B >= 1);
  prp_prev.refine_with_constraint(A <= 2);
  prp_prev.refine_with_constraint(B >= 0);
  prp_prev.refine_with_congruence((B %= 0) / 2);
  prp_prev.refine_with_congruence(3*A %= 0);

  print_congruences(prp_prev, "*** prp_prev congruences ***");
  print_constraints(prp_prev, "*** prp_prev constraints ***");

  Product prp(3);
  prp.refine_with_constraint(C == 0);
  prp.refine_with_constraint(A <= 2);
  prp.refine_with_constraint(B >= 0);
  prp.refine_with_constraint(2*A - B >= 2);
  prp.refine_with_constraint(B >= 0);
  prp.refine_with_congruence(6*A %= 0);
  prp.refine_with_congruence((B %= 0) / 2);

  prp.upper_bound_assign(prp_prev);

  print_congruences(prp, "*** prp congruences ***");
  print_constraints(prp, "*** prp constraints ***");

  prp.widening_assign(prp_prev);

  Product known_prp(3);
  known_prp.refine_with_constraint(C == 0);
  known_prp.refine_with_constraint(A <= 2);
  known_prp.refine_with_constraint(B >= 0);
  known_prp.refine_with_congruence((B %= 0) / 2);

  bool ok = (prp == known_prp);

  print_congruences(prp,
                    "*** prp.widening_assign(prp_prev) congruences ***");
  print_constraints(prp,
                    "*** prp.widening_assign(prp_prev) constraints ***");

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
#if C_Poly_Class
  DO_TEST_F8A(test07);
#else
  DO_TEST(test07);
#endif
  DO_TEST(test08);
  DO_TEST(test09);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST_F8(test17);
  DO_TEST(test18);
  DO_TEST(test19);
END_MAIN
