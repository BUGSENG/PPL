/* Test Grid::maximize(const Linear_Expression&, ...)
   and Grid::minimize(const Linear_Expression&, ...).
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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

using namespace Parma_Polyhedra_Library::IO_Operators;

namespace {

Variable A(0);
Variable B(1);
Variable C(2);
Variable D(3);
Variable E(4);
Variable F(5);

void
check_both(Grid& gr, Linear_Expression& le, string grid_name) {
  Grid_Generator ext_pnt(grid_point());
  bool dummy;
  Coefficient ext_n, ext_d;

  if (gr.minimize(le, ext_n, ext_d, dummy, ext_pnt)
      || gr.maximize(le, ext_n, ext_d, dummy, ext_pnt)) {
    nout << grid_name << " bounded expr" << endl;
    exit(1);
  }
}

void
check_minimize(Grid& gr, Linear_Expression& le,
	       Coefficient expected_n, Coefficient expected_d,
	       Grid_Generator& expected_pnt, string grid_name) {
  Grid_Generator inf_pnt(grid_point());

  bool dummy;
  Coefficient inf_n, inf_d;

  if (gr.minimize(le, inf_n, inf_d, dummy, inf_pnt))
    if (inf_n == expected_n)
      if (inf_d == expected_d)
	if (inf_pnt == expected_pnt)
	  return;
	else
	  nout << grid_name << " min point " << inf_pnt << " (expected "
	       << expected_pnt << ")" << endl;
      else
	nout << grid_name << " inf_d " << inf_d
	     << " (expected " << expected_d << ")" << endl;
    else
      nout << grid_name << " inf_n " << inf_n
	   << " (expected " << expected_n << ")" << endl;
  else
    nout << " should bound expr from below." << endl;

  exit(1);
}

void
check_maximize(Grid& gr, Linear_Expression& le,
	       Coefficient expected_n, Coefficient expected_d,
	       Grid_Generator& expected_pnt, string grid_name) {
  Grid_Generator sup_pnt(grid_point());

  bool dummy;
  Coefficient sup_n, sup_d;

  if (gr.maximize(le, sup_n, sup_d, dummy, sup_pnt))
    if (sup_n == expected_n)
      if (sup_d == expected_d)
	if (sup_pnt == expected_pnt)
	  return;
	else
	  nout << grid_name << " max point " << sup_pnt << " (expected "
	       << expected_pnt << ")" << endl;
      else
	nout << grid_name << " sup_d " << sup_d
	     << " (expected " << expected_d << ")" << endl;
    else
      nout << grid_name << " sup_n " << sup_n
	   << " (expected " << expected_n << ")" << endl;
  else
    nout << " should bound expr from above." << endl;

  exit(1);
}

// Empty.

void
test1() {
  Grid gr(7, EMPTY);

  Coefficient extr_n, extr_d;
  bool dummy;
  Grid_Generator pnt(grid_point());

  if (gr.maximize(Linear_Expression(0), extr_n, extr_d, dummy, pnt)
      || gr.minimize(Linear_Expression(0), extr_n, extr_d, dummy, pnt))
    exit(1);
}

// Zero dimension empty.

void
test2() {
  Grid gr(0, EMPTY);

  Linear_Expression le = Linear_Expression::zero();

  check_both(gr, le, "gr");
}

// Zero dimension universe.

void
test3() {
  Grid gr(0);

  Linear_Expression le = Linear_Expression::zero();

  Grid_Generator exp_pnt(grid_point());

  check_maximize(gr, le, 0, 1, exp_pnt, "gr");
  check_minimize(gr, le, 0, 1, exp_pnt, "gr");
}

// Point.

void
test4() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator_and_minimize(grid_point(3*A + 2*B, 3));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point(3*A + 2*B, 3));

  Grid gr_cgs_needs_min(2);
  gr_cgs_needs_min.add_congruence(A == 1);
  gr_cgs_needs_min.add_congruence(3*B == 2);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = A + B;

  Grid_Generator exp_pnt(grid_point(3*A + 2*B, 3));

  check_maximize(gr_gs_min, le, 5, 3, exp_pnt, "gr_gs_min");
  check_minimize(gr_gs_min, le, 5, 3, exp_pnt, "gr_gs_min");

  check_maximize(gr_gs_needs_min, le, 5, 3, exp_pnt, "gr_gs_needs_min");
  check_minimize(gr_gs_needs_min, le, 5, 3, exp_pnt, "gr_gs_needs_min");

  check_maximize(gr_cgs_needs_min, le, 5, 3, exp_pnt, "gr_cgs_needs_min");
  check_minimize(gr_cgs_needs_min, le, 5, 3, exp_pnt, "gr_cgs_needs_min");
}

// Rectilinear line.

void
test5() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator_and_minimize(grid_line(B));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_line(B));

  Grid gr_cgs_needs_min(2);
  gr_cgs_needs_min.add_congruence(A == 0);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A - B;

  check_both(gr_gs_min, le, "gr_gs_min");
  check_both(gr_gs_needs_min, le, "gr_gs_needs_min");
  check_both(gr_cgs_needs_min, le, "gr_cgs_needs_min");
}

// Line.

void
test6() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator_and_minimize(grid_line(2*A + B));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_line(2*A + B));

  Grid gr_cgs_needs_min(2);
  gr_cgs_needs_min.add_congruence(A - 2*B == 0);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A + B;

  check_both(gr_gs_min, le, "gr_gs_min");
  check_both(gr_gs_needs_min, le, "gr_gs_needs_min");
  check_both(gr_cgs_needs_min, le, "gr_cgs_needs_min");
}

// A line along the equality `expr == 0'.

void
test7() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator_and_minimize(grid_line(A + 2*B));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_line(A + 2*B));

  Grid gr_cgs_needs_min(2);
  gr_cgs_needs_min.add_congruence(2*A - B == 0);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A - B;

  Grid_Generator exp_pnt(grid_point(0*B));

  check_maximize(gr_gs_min, le, 0, 1, exp_pnt, "gr_gs_min");
  check_minimize(gr_gs_min, le, 0, 1, exp_pnt, "gr_gs_min");

  check_maximize(gr_gs_needs_min, le, 0, 1, exp_pnt, "gr_gs_needs_min");
  check_minimize(gr_gs_needs_min, le, 0, 1, exp_pnt, "gr_gs_needs_min");

  check_maximize(gr_cgs_needs_min, le, 0, 1, exp_pnt, "gr_cgs_needs_min");
  check_minimize(gr_cgs_needs_min, le, 0, 1, exp_pnt, "gr_cgs_needs_min");
}

// A parameter along the equality `expr == 0'.

void
test8() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator_and_minimize(grid_point(A + 2*B));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_point(A + 2*B));

  Grid gr_cgs_needs_min(2);
  gr_cgs_needs_min.add_congruence(2*A - B == 0);
  gr_cgs_needs_min.add_congruence((B %= 0) / 2);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A - B;

  Grid_Generator exp_pnt(grid_point(0*B));

  check_maximize(gr_gs_min, le, 0, 1, exp_pnt, "gr_gs_min");
  check_minimize(gr_gs_min, le, 0, 1, exp_pnt, "gr_gs_min");

  check_maximize(gr_gs_needs_min, le, 0, 1, exp_pnt, "gr_gs_needs_min");
  check_minimize(gr_gs_needs_min, le, 0, 1, exp_pnt, "gr_gs_needs_min");

  check_maximize(gr_cgs_needs_min, le, 0, 1, exp_pnt, "gr_cgs_needs_min");
  check_minimize(gr_cgs_needs_min, le, 0, 1, exp_pnt, "gr_cgs_needs_min");
}

// Two lines which combine to cover any (affine) line defined by expr.

void
test9() {
  Grid gr_gs_min(2, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator(grid_line(A));
  gr_gs_min.add_generator_and_minimize(grid_line(B));

  Grid gr_gs_needs_min(2, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_line(A));
  gr_gs_needs_min.add_generator(grid_line(B));

  Grid gr_cgs_needs_min(2);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = A - B;

  check_both(gr_gs_min, le, "gr_gs_min");
  check_both(gr_gs_needs_min, le, "gr_gs_needs_min");
  check_both(gr_cgs_needs_min, le, "gr_cgs_needs_min");
}

// In three dimensions, lines and parameters which combine to include
// expr.

void
test10() {
  Grid gr_gs_min(3, EMPTY);
  gr_gs_min.add_generator(grid_point());
  gr_gs_min.add_generator(grid_line(A));
  gr_gs_min.add_generator_and_minimize(grid_point(B + C));

  Grid gr_gs_needs_min(3, EMPTY);
  gr_gs_needs_min.add_generator(grid_point());
  gr_gs_needs_min.add_generator(grid_line(A));
  gr_gs_needs_min.add_generator(grid_point(B + C));

  Grid gr_cgs_needs_min(3);
  gr_cgs_needs_min.add_congruence(B - C == 0);
  gr_cgs_needs_min.add_congruence(B %= 0);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A + B - C;

  check_both(gr_gs_min, le, "gr_gs_min");
  check_both(gr_gs_needs_min, le, "gr_gs_needs_min");
  check_both(gr_cgs_needs_min, le, "gr_cgs_needs_min");
}

// Grid which bounds a 3D expr.

void
test11() {
  Grid gr_gs_min(3, EMPTY);
  gr_gs_min.add_generator(grid_point(A));
  gr_gs_min.add_generator(grid_line(3*B + C));
  gr_gs_min.add_generator_and_minimize(grid_line(A - 2*B));

  Grid gr_gs_needs_min(3, EMPTY);
  gr_gs_needs_min.add_generator(grid_point(A));
  gr_gs_needs_min.add_generator(grid_line(3*B + C));
  gr_gs_needs_min.add_generator(grid_line(A - 2*B));

  Grid gr_cgs_needs_min(3);
  gr_cgs_needs_min.add_congruence(2*A + B - 3*C - 2 == 0);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = 2*A + B - 3*C;

  Grid_Generator exp_pnt1(grid_point(2*B + 0*C));

  check_maximize(gr_gs_min, le, 2, 1, exp_pnt1, "gr_gs_min");
  check_minimize(gr_gs_min, le, 2, 1, exp_pnt1, "gr_gs_min");

  check_maximize(gr_gs_needs_min, le, 2, 1, exp_pnt1, "gr_gs_needs_min");
  check_minimize(gr_gs_needs_min, le, 2, 1, exp_pnt1, "gr_gs_needs_min");

  Grid_Generator exp_pnt2(grid_point(-2*C, 3));

  check_maximize(gr_cgs_needs_min, le, 2, 1, exp_pnt2, "gr_cgs_needs_min");
  check_minimize(gr_cgs_needs_min, le, 2, 1, exp_pnt2, "gr_cgs_needs_min");
}

// Point in 6D.

void
test12() {
  Grid gr_gs_min(6, EMPTY);
  gr_gs_min.add_generator_and_minimize(grid_point(7*A - 11*B + 19*F));

  Grid gr_gs_needs_min(6, EMPTY);
  gr_gs_needs_min.add_generator(grid_point(7*A - 11*B + 19*F));

  Grid gr_cgs_needs_min(6);
  gr_cgs_needs_min.add_congruence(A == 7);
  gr_cgs_needs_min.add_congruence(B == -11);
  gr_cgs_needs_min.add_congruence(C == 0);
  gr_cgs_needs_min.add_congruence(D == 0);
  gr_cgs_needs_min.add_congruence(E == 0);
  gr_cgs_needs_min.add_congruence(F == 19);

  assert(copy_compare(gr_gs_min, gr_gs_needs_min));
  assert(copy_compare(gr_gs_needs_min, gr_cgs_needs_min));

  Linear_Expression le = A + 2*B + 3*C + 4*D + 6*F;

  Grid_Generator exp_pnt(grid_point(7*A - 11*B + 19*F));

  check_maximize(gr_gs_min, le, 99, 1, exp_pnt, "gr_gs_min");
  check_minimize(gr_gs_min, le, 99, 1, exp_pnt, "gr_gs_min");

  check_maximize(gr_gs_needs_min, le, 99, 1, exp_pnt, "gr_gs_needs_min");
  check_minimize(gr_gs_needs_min, le, 99, 1, exp_pnt, "gr_gs_needs_min");

  check_maximize(gr_cgs_needs_min, le, 99, 1, exp_pnt, "gr_cgs_needs_min");
  check_minimize(gr_cgs_needs_min, le, 99, 1, exp_pnt, "gr_cgs_needs_min");
}

} // namespace

int
main() TRY {
  set_handlers();

  nout << "maxmin1:" << endl;

  DO_TEST(test1);
  DO_TEST(test2);
  DO_TEST(test3);
  DO_TEST(test4);
  DO_TEST(test5);
  DO_TEST(test6);
  DO_TEST(test7);
  DO_TEST(test8);
  DO_TEST(test9);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);

  return 0;
}
CATCH
