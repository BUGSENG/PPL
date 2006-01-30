/* Test Grid::generalized_affine_preimage(lhs, rhs, modulus).
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

// FIX Tests 1 to 13 are equivalent to tests 1 to 13 in
// generalizedaffinepreimage1.cc.

// Expressions both constants.

void
test14() {
  nout << "test14:" << endl;

  Grid gr(2);
  gr.add_congruence(A %= 0);

  gr.generalized_affine_preimage(Linear_Expression::zero(),
				 Linear_Expression(1));

  if (find_variation(gr))
    exit(1);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_point(A));
  known_gr.add_generator(grid_line(B));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  dump_grids(gr, known_gr);

  exit(1);
}

// Left hand side constant.

void
test15() {
  nout << "test15:" << endl;

  Grid gr(2);
  gr.add_congruence(A %= 0);

  gr.generalized_affine_preimage(Linear_Expression::zero(), A - B, 5);

  if (find_variation(gr))
    exit(1);

  // FIX check
  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_point(A + B));
  known_gr.add_generator(grid_point(5*A));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  dump_grids(gr, known_gr);

  exit(1);
}

// Expressions with unique variables.

void
test16() {
  nout << "test16:" << endl;

  Grid gr(3);
  gr.add_congruence(A - B == 0);
  gr.add_congruence((C %= 0) / 3);

  gr.generalized_affine_preimage(A - B, C);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_line(A));
  known_gr.add_generator(grid_line(B));
  known_gr.add_generator(grid_point(3*C));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  dump_grids(gr, known_gr);

  exit(1);
}

// Simple expressions having common variables.

void
test17() {
  nout << "test17:" << endl;

  Grid gr(2);
  gr.add_congruence(A - B == 0);

  gr.generalized_affine_preimage(A - B, A, 0);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(2);
  known_gr.add_congruence(A == 0);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  dump_grids(gr, known_gr);

  exit(1);
}

// Expressions having common variables.

void
test18() {
  nout << "test18:" << endl;

  Grid gr(2);
  gr.add_congruence((A %= 0) / 1);
  gr.add_congruence((B %= 0) / 2);

  gr.generalized_affine_preimage(A + 2*B, A - B, 3);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_point(A));
  known_gr.add_generator(grid_line(A + B));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// FIX add an equivalent of one of the ph tests

// Expressions having common variables.

void
test20() {
  nout << "test20:" << endl;

  Grid gr(3);
  gr.add_congruence((C %= 0) / 3);
  gr.add_congruence(A - 2*B == 1);

  gr.generalized_affine_preimage(A - B + C, 2*A - B - C, 5);

  if (find_variation(gr))
    exit(1);

  // FIX check
  Grid known_gr(3);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Expressions having common variables, where
// generalized_affine_preimage must minimize the grid.

void
test21() {
  nout << "test21:" << endl;

  Grid gr(2);
  gr.add_congruence(A - B == 0);

  gr.generalized_affine_preimage(A - B, 2*A - 2*B, 5);

  Grid known_gr(2);
  known_gr.add_congruence((2*A - 2*B %= 0) / 5);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Expressions having common variables, where
// generalized_affine_preimage must minimize the grid.

void
test22() {
  nout << "test22:" << endl;

  Grid gr(2);
  gr.add_congruence(A - B == 0);

  gr.generalized_affine_preimage(2*A - 2*B, A - B, 5);

  Grid known_gr(2);
  known_gr.add_congruence((A - B %= 0) / 5);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Right hand side expression of greater space dimension than the
// grid.

void
test23() {
  nout << "test23:" << endl;

  Grid gr(3);
  gr.add_congruence(C %= -2);

  try {
    gr.generalized_affine_preimage(B + C, D + 2);
    exit(1);
  }
  catch (const std::invalid_argument& e) {}
}

// Left hand side expression of space dimension greater than the grid.

void
test24() {
  nout << "test24:" << endl;

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);

  try {
    gr.generalized_affine_preimage(A + D, A + 2);
    exit(1);
  }
  catch (const std::invalid_argument& e) {}
}

// Expressions having common variables, with a negative modulus.

void
test25() {
  nout << "test25:" << endl;

  Grid gr(3);
  gr.add_congruence((C %= 0) / 3);
  gr.add_congruence(A - B == 0);

  gr.generalized_affine_preimage(A - B, C, -5);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3);
  known_gr.add_congruence((C %= 0) / 15);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  nout << "generalizedaffinepreimage2:" << endl;

#if 0
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  test13();
#endif
  test14();
  test15();
  test16();
  test17();
  test18();
#if 0
  test19();
#endif
  test20();
  test21();
  test22();
  test23();
  test24();
  test25();

  return 0;
}
CATCH
